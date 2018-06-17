#include <string.h>

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "tcg/tcg.h"
#include "sysemu/sysemu.h"
#include "qemu-common.h"
#include "migration/misc.h"
#include "sysemu/accel.h"
#include "hw/boards.h"
#include "hw/s390x/s390-virtio-ccw.h"
#include "qapi/error.h"
#include "qemu/config-file.h"
#include "qemu/option.h"
#include "internal.h"
#include "hw/loader.h"

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

int main(int argc, char ** argv) {
    struct S390CPU *cpu;
    struct TranslationBlock *tb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

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
    ram_size = 1 * 1024 * 1024 * 1024;
    // vl.c:3995
    current_machine = MACHINE(object_new("s390-ccw-virtio-3.0-machine"));
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
    bios_name = "build/pc-bios/s390-ccw.img";
    // vl.c:4302
    current_machine->kernel_filename = argv[1];
    current_machine->kernel_cmdline = "";
    // vl.c:4461
    current_machine->ram_size = ram_size;
    // vl.c:4467
    current_machine->cpu_type = S390_CPU_TYPE_NAME("qemu");
    // vl.c:4477
    machine_run_board_init(current_machine);
    // vl.c:4561
    if (rom_check_and_register_reset() != 0) {
        fprintf(stderr, "%s: rom_check_and_register_reset() failed\n", argv[0]);
        return EXIT_FAILURE;
    }
    // vl.c:4572
    qemu_system_reset(SHUTDOWN_CAUSE_NONE);

    // set pc
    cpu = S390_CPU(first_cpu);
    do_restart_interrupt(&cpu->env);

    // translate-all.c:1250
    tb = tcg_tb_alloc(tcg_ctx);
    if (!tb) {
        fprintf(stderr, "%s: tcg_tb_alloc() failed\n", argv[0]);
        return EXIT_FAILURE;
    }
    // translate-all.c:1272
    cpu_get_tb_cpu_state(&cpu->env, &tb->pc, &tb->cs_base, &tb->flags);
    // translate-all.c:1285
    tcg_func_start(tcg_ctx);
    // translate-all.c:1288
    gen_intermediate_code(&cpu->parent_obj, tb);

    // print results
    tcg_dump_ops(tcg_ctx);
}
