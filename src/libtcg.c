#include "libtcg.h"

#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "sysemu/sysemu.h"
#include "migration/misc.h"
#include "sysemu/accel.h"
#include "hw/boards.h"
#include "qapi/error.h"
#include "qemu/config-file.h"
#include "qemu/option.h"
#include "internal.h"
#include "hw/loader.h"
#include "tcg/tcg.h"
#include "disas/disas.h"

// vl.c:274
static QemuOptsList qemu_machine_opts = {
    .name = "machine",
    .implied_opt_name = "type",
    .merge_lists = true,
    .head = QTAILQ_HEAD_INITIALIZER(qemu_machine_opts.head),
    .desc = { { } },
};

// vl.c:309
static QemuOptsList qemu_boot_opts = {
    .name = "boot-opts",
    .implied_opt_name = "order",
    .merge_lists = true,
    .head = QTAILQ_HEAD_INITIALIZER(qemu_boot_opts.head),
    .desc = { { } },
};

struct CPUState *libtcg_init(char *path)
{
    struct S390CPU *cpu;

    // vl.c:2936
    qemu_init_cpu_list();
    // vl.c:2937
    qemu_init_cpu_loop();
    // vl.c:2939
    qemu_mutex_lock_iothread();
    // vl.c:2945
    module_call_init(MODULE_INIT_QOM);
    // vl.c:2962
    qemu_add_opts(&qemu_machine_opts);
    // vl.c:2966
    qemu_add_opts(&qemu_boot_opts);
    // vl.c:3956
    ram_size = RAM_SIZE;
    // vl.c:3995
    current_machine = MACHINE(object_new(machine_type));
    // vl.c:4000
    object_property_add_child(object_get_root(), "machine", OBJECT(current_machine), &error_abort);
    // vl.c:4075
    smp_cpus = max_cpus = 1;
    // vl.c:4012
    cpu_exec_init_all();
    // vl.c:4043
    qemu_set_log(0);
    // vl.c:4257
    configure_accelerator(current_machine);
    // vl.c:4269
    migration_object_init();
    // vl.c:4279
    bios_name = "qemu/pc-bios/s390-ccw.img";
    // vl.c:4302
    current_machine->kernel_filename = path;
    current_machine->kernel_cmdline = "";
    // vl.c:4461
    current_machine->ram_size = ram_size;
    current_machine->maxram_size = ram_size;
    // vl.c:4467
    current_machine->cpu_type = S390_CPU_TYPE_NAME("qemu");
    // vl.c:4477
    machine_run_board_init(current_machine);
    // vl.c:4561
    if (rom_check_and_register_reset() != 0)
        return NULL;
    // vl.c:4572
    qemu_system_reset(SHUTDOWN_CAUSE_NONE);

    // make sure saved state has 0 icount_decr
    cpu = S390_CPU(first_cpu);
    cpu->parent_obj.icount_decr.u32 = 0;

    return &cpu->parent_obj;
}

static __thread struct TranslationBlock *tb;

struct TCGContext *libtcg_gen(struct CPUState *cpu, char *disasm, size_t disasm_len)
{
    FILE *fdisasm;

    // translate-all.c:1250
    if (!tb) {
        tb = tcg_tb_alloc(tcg_ctx);
        if (!tb)
            return NULL;
        // generate 1 insn per tb
        tb->cflags = (tb->cflags & ~CF_COUNT_MASK) | 1;
    }
    // translate-all.c:1272
    cpu_get_tb_cpu_state(&S390_CPU(cpu)->env, &tb->pc, &tb->cs_base, &tb->flags);
    // assume PER is irrelevant
    tb->flags &= ~FLAG_MASK_PER;
    // translate-all.c:1285
    tcg_func_start(tcg_ctx);
    // translate-all.c:1288
    gen_intermediate_code(cpu, tb);
    if (tb->size == 0)
        return NULL;
    fdisasm = fmemopen(disasm, disasm_len, "w");
    target_disas(fdisasm, cpu, tb->pc, tb->size);
    disasm[disasm_len - 1] = 0;
    fclose(fdisasm);
    return tcg_ctx;
}
