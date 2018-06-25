#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include "qemu/osdep.h"
#include "target/s390x/cpu.h"

#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include "hw/boards.h"
#include "hw/s390x/s390_flic.h"
#include "hw/s390x/s390-pci-inst.h"
#include "hw/s390x/s390-virtio-hcall.h"
#include "hw/s390x/s390-virtio-ccw.h"
#include "hw/s390x/sclp.h"
#include "hw/s390x/storage-keys.h"
#include "libtcg.h"
#include "qapi/error.h"
#include "qemu/cutils.h"
#include "runtime-sclp.h"
#include "sysemu/sysemu.h"
#include "target/s390x/internal.h"
#include "tcg/tcg.h"
#include "trace/control.h"
#include "trace/control-internal.h"
#include "trace-root.h"

extern struct S390CPU cpu;
extern struct S390CPUModel cpu_model;
extern struct S390CPUDef cpu_def;
extern char memory[];

__attribute__((constructor))
static void init_cpu()
{
    cpu.model = &cpu_model;
    cpu.model->def = &cpu_def;
}

TraceEvent _TRACE_GUEST_MEM_BEFORE_EXEC_EVENT;
char _TRACE_QEMU_S390_AIRQ_SUPPRESSED_DSTATE;
char _TRACE_QEMU_S390_SUPPRESS_AIRQ_DSTATE;

bool address_space_access_valid(AddressSpace *as, hwaddr addr, int len,
                                bool is_write, MemTxAttrs attrs)
{
    abort();
}

AddressSpace address_space_memory;

int clp_service_call(S390CPU *cpu, uint8_t r2, uintptr_t ra)
{
    abort();
}

int css_do_sic(CPUS390XState *env, uint8_t isc, uint16_t mode)
{
    abort();
}

void cpu_abort(CPUState *cpu, const char *fmt, ...)
{
    abort();
}

GuestPanicInformation *cpu_get_crash_info(CPUState *cpu)
{
    abort();
}

static void handle_interrupt(CPUState *cpu, int mask)
{
    cpu->interrupt_request |= mask;
}

CPUInterruptHandler cpu_interrupt_handler = handle_interrupt;

void cpu_loop_exit(CPUState *cpu)
{
    siglongjmp(cpu->jmp_env, 1);
}

void cpu_loop_exit_atomic(CPUState *cpu, uintptr_t pc)
{
    abort();
}

void cpu_loop_exit_noexc(CPUState *cpu)
{
    abort();
}

void cpu_loop_exit_restore(CPUState *cpu, uintptr_t pc)
{
    cpu_loop_exit(cpu);
}

void *cpu_physical_memory_map(hwaddr addr,
                              hwaddr *plen,
                              int is_write)
{
    return &memory[addr];
}

void cpu_physical_memory_rw(hwaddr addr, uint8_t *buf,
                            int len, int is_write)
{
    abort();
}

void cpu_physical_memory_unmap(void *buffer, hwaddr len,
                               int is_write, hwaddr access_len)
{
}

int cpu_watchpoint_insert(CPUState *cpu, vaddr addr, vaddr len,
                          int flags, CPUWatchpoint **watchpoint)
{
    abort();
}

void cpu_watchpoint_remove_all(CPUState *cpu, int mask)
{
}

struct CPUTailQ cpus = QTAILQ_HEAD_INITIALIZER(cpus);

__attribute__((constructor))
void cpus_init()
{
    QTAILQ_INSERT_TAIL(&cpus, &cpu.parent_obj, node);
}

bool css_migration_enabled(void)
{
    abort();
}

void do_stop_interrupt(CPUS390XState *env)
{
    abort();
}

void error_setg_internal(Error **errp,
                         const char *src, int line, const char *func,
                         const char *fmt, ...)
{
    abort();
}

int handle_diag_288(CPUS390XState *env, uint64_t r1, uint64_t r3)
{
    abort();
}

void handle_diag_308(CPUS390XState *env, uint64_t r1, uint64_t r3,
                     uintptr_t ra)
{
    abort();
}

int handle_sigp(CPUS390XState *env, uint8_t order, uint64_t r1, uint64_t r3)
{
    abort();
}

uint32_t helper_atomic_cmpxchgl_be_mmu(CPUArchState *env, target_ulong addr,
                                       uint32_t cmpv, uint32_t newv,
                                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

Int128 helper_atomic_cmpxchgo_be_mmu(CPUArchState *env, target_ulong addr,
                                     Int128 cmpv, Int128 newv,
                                     TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

uint64_t helper_atomic_cmpxchgq_be_mmu(CPUArchState *env, target_ulong addr,
                                       uint64_t cmpv, uint64_t newv,
                                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

Int128 helper_atomic_ldo_be_mmu(CPUArchState *env, target_ulong addr,
                                TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

void helper_atomic_sto_be_mmu(CPUArchState *env, target_ulong addr, Int128 val,
                              TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

// softmmu_template.h
uint32_t helper_be_ldl_cmmu(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    return be32_to_cpu(*(uint32_t *)&memory[addr]);
}

uint64_t helper_be_ldq_mmu(CPUArchState *env, target_ulong addr,
                           TCGMemOpIdx oi, uintptr_t retaddr)
{
    return be64_to_cpu(*(uint64_t *)&memory[addr]);
}

tcg_target_ulong helper_be_ldul_mmu(CPUArchState *env, target_ulong addr,
                                    TCGMemOpIdx oi, uintptr_t retaddr)
{
    return be32_to_cpu(*(uint32_t *)&memory[addr]);
}

tcg_target_ulong helper_be_lduw_mmu(CPUArchState *env, target_ulong addr,
                                    TCGMemOpIdx oi, uintptr_t retaddr)
{
    return be16_to_cpu(*(uint16_t *)&memory[addr]);
}

uint16_t helper_be_ldw_cmmu(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    return be16_to_cpu(*(uint16_t *)&memory[addr]);
}

void helper_be_stl_mmu(CPUArchState *env, target_ulong addr, uint32_t val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    *(uint32_t *)&memory[addr] = cpu_to_be32(val);
}

void helper_be_stq_mmu(CPUArchState *env, target_ulong addr, uint64_t val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    *(uint64_t *)&memory[addr] = cpu_to_be64(val);
}

void helper_be_stw_mmu(CPUArchState *env, target_ulong addr, uint16_t val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    *(uint16_t *)&memory[addr] = cpu_to_be16(val);
}

uint8_t helper_ret_ldb_cmmu(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    return (uint8_t)memory[addr];
}

tcg_target_ulong helper_ret_ldub_mmu(CPUArchState *env, target_ulong addr,
                                     TCGMemOpIdx oi, uintptr_t retaddr)
{
    if (addr + sizeof(uint8_t) <= ram_size)
        return (uint8_t)memory[addr];
    else {
        s390_program_interrupt(env, PGM_ADDRESSING, ILEN_AUTO, retaddr);
        abort();
    }
}

void helper_ret_stb_mmu(CPUArchState *env, target_ulong addr, uint8_t val,
                        TCGMemOpIdx oi, uintptr_t retaddr)
{
    // cpu_ldst.h:406
    int index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    CPUTLBEntry *tlbentry = &env->tlb_table[get_mmuidx(oi)][index];

    tlbentry->addr_read = tlbentry->addr_write = tlbentry->addr_code = addr;
    tlbentry->addend = (uintptr_t)&memory;
    memory[addr] = (char)val;
}

void ioinst_handle_chsc(S390CPU *cpu, uint32_t ipb, uintptr_t ra)
{
    abort();
}

void ioinst_handle_csch(S390CPU *cpu, uint64_t reg1, uintptr_t ra)
{
    abort();
}

void ioinst_handle_hsch(S390CPU *cpu, uint64_t reg1, uintptr_t ra)
{
    abort();
}

void ioinst_handle_msch(S390CPU *cpu, uint64_t reg1, uint32_t ipb,
                        uintptr_t ra)
{
    abort();
}

void ioinst_handle_rchp(S390CPU *cpu, uint64_t reg1, uintptr_t ra)
{
    abort();
}

void ioinst_handle_rsch(S390CPU *cpu, uint64_t reg1, uintptr_t ra)
{
    abort();
}

void ioinst_handle_sal(S390CPU *cpu, uint64_t reg1, uintptr_t ra)
{
    abort();
}

void ioinst_handle_schm(S390CPU *cpu, uint64_t reg1, uint64_t reg2,
                        uint32_t ipb, uintptr_t ra)
{
    abort();
}

void ioinst_handle_ssch(S390CPU *cpu, uint64_t reg1, uint32_t ipb,
                        uintptr_t ra)
{
    abort();
}

void ioinst_handle_stcrw(S390CPU *cpu, uint32_t ipb, uintptr_t ra)
{
    abort();
}

void ioinst_handle_stsch(S390CPU *cpu, uint64_t reg1, uint32_t ipb,
                         uintptr_t ra)
{
    abort();
}

int ioinst_handle_tsch(S390CPU *cpu, uint64_t reg1, uint32_t ipb, uintptr_t ra)
{
    abort();
}

void ioinst_handle_xsch(S390CPU *cpu, uint64_t reg1, uintptr_t ra)
{
    abort();
}

bool is_daemonized()
{
    return false;
}

unsigned int max_cpus = 1;

int mmu_translate(CPUS390XState *env, target_ulong vaddr, int rw, uint64_t asc,
                  target_ulong *raddr, int *flags, bool exc)
{
    abort();
}

int mmu_translate_real(CPUS390XState *env, target_ulong raddr, int rw,
                       target_ulong *addr, int *flags)
{
    abort();
}

int mpcifc_service_call(S390CPU *cpu, uint8_t r1, uint64_t fiba, uint8_t ar,
                        uintptr_t ra)
{
    abort();
}

ObjectClass *object_class_dynamic_cast_assert(ObjectClass *klass,
                                              const char *typename,
                                              const char *file, int line,
                                              const char *func)
{
    return klass;
}

Object *object_dynamic_cast_assert(Object *obj, const char *typename,
                                   const char *file, int line, const char *func)
{
    return obj;
}

ObjectClass *object_get_class(Object *obj)
{
    return obj->class;
}

void object_property_add_child(Object *obj, const char *name,
                               Object *child, Error **errp)
{
    abort();
}

static S390FLICStateClass flic_class;
static QEMUS390FLICState flic;

Object *object_resolve_path_type(const char *path, const char *typename,
                                 bool *ambiguous)
{
    if (strcmp(path, "") == 0 && strcmp(typename, TYPE_S390_FLIC_COMMON) == 0) {
        if (ambiguous)
            *ambiguous = false;
        return &flic.parent_obj.parent_obj.parent_obj.parent_obj;
    }
    abort();
}

int pcilg_service_call(S390CPU *cpu, uint8_t r1, uint8_t r2, uintptr_t ra)
{
    abort();
}

int pcistb_service_call(S390CPU *cpu, uint8_t r1, uint8_t r3, uint64_t gaddr,
                        uint8_t ar, uintptr_t ra)
{
    abort();
}

int pcistg_service_call(S390CPU *cpu, uint8_t r1, uint8_t r2, uintptr_t ra)
{
    abort();
}

void probe_write(CPUArchState *env, target_ulong addr, int size, int mmu_idx,
                 uintptr_t retaddr)
{
    abort();
}

DeviceState *qdev_create(BusState *bus, const char *name)
{
    abort();
}

static char possible_cpus[sizeof(CPUArchIdList) + sizeof(CPUArchId)];
static struct MachineState ms;

__attribute__((constructor))
static void init_ms()
{
    // s390-virtio-ccw.c:461
    ms.possible_cpus = (CPUArchIdList *)&possible_cpus;
    ms.possible_cpus->len = 1;
    ms.possible_cpus->cpus[0].arch_id = 0;
    ms.possible_cpus->cpus[0].vcpus_count = 1;
    ms.possible_cpus->cpus[0].props.has_core_id = true;
    ms.possible_cpus->cpus[0].props.core_id = 0;
    ms.possible_cpus->cpus[0].type = machine_type;
    ms.possible_cpus->cpus[0].cpu = &cpu.parent_obj.parent_obj.parent_obj;
}

Object *qdev_get_machine(void)
{
    return &ms.parent_obj;
}

void qdev_init_nofail(DeviceState *dev)
{
    abort();
}

const PropertyInfo qdev_prop_uint32;

int64_t qemu_clock_get_ns(QEMUClockType type)
{
    abort();
}

int qemu_loglevel;

// We are single-threaded, so locks are not needed.
static __thread bool iothread_locked = true;

bool qemu_mutex_iothread_locked(void)
{
    return iothread_locked;
}

void qemu_mutex_lock_iothread(void)
{
    iothread_locked = true;
}

void qemu_mutex_unlock_iothread(void)
{
    iothread_locked = false;
}

const char *qemu_name = "qemu";

int qemu_strtou64(const char *nptr, const char **endptr, int base,
                  uint64_t *result)
{
    abort();
}

void qemu_system_guest_panicked(GuestPanicInformation *info)
{
    abort();
}

void qemu_system_shutdown_request(ShutdownCause reason)
{
    abort();
}

QemuUUID qemu_uuid;

ram_addr_t ram_size = RAM_SIZE;

void register_module_init(void (*fn)(void), module_init_type type)
{
    fn();
}

int rpcit_service_call(S390CPU *cpu, uint8_t r1, uint8_t r2, uintptr_t ra)
{
    abort();
}

unsigned int s390_cpu_halt(S390CPU *cpu)
{
    uint64_t r2 = cpu->env.regs[2];
    bool has_service;

    qemu_mutex_lock_iothread();
    has_service = qemu_s390_flic_has_service(&flic);
    if ((cpu->env.psw.mask & PSW_MASK_EXT) && has_service) {
        cpu->parent_obj.exception_index = EXCP_EXT;
        cpu_loop_exit(&cpu->parent_obj);
    }
    qemu_mutex_unlock_iothread();

    if (r2) {
        fprintf(stderr, "R2=0x%"PRIx64"\n", r2);
        exit(EXIT_FAILURE);
    } else
        exit(EXIT_SUCCESS);
}

void s390_cpu_unhalt(S390CPU *cpu)
{
    abort();
}

void s390_cpu_virt_mem_handle_exc(S390CPU *cpu, uintptr_t ra)
{
    abort();
}

int s390_cpu_virt_mem_rw(S390CPU *cpu, vaddr laddr, uint8_t ar, void *hostbuf,
                         int len, bool is_write)
{
    if (is_write)
        memcpy(&memory[laddr], hostbuf, len);
    else
        memcpy(hostbuf, &memory[laddr], len);
    return 0;
}

void s390_get_feat_block(S390FeatType type, uint8_t *data)
{
    abort();
}

S390SKeysState *s390_get_skeys_device(void)
{
    abort();
}

bool s390_has_feat(S390Feat feat)
{
    abort();
}

int s390_virtio_hypercall(CPUS390XState *env)
{
    abort();
}

struct sclp_handler sclp_handlers[MAX_SCLP_HANDLERS];
size_t n_sclp_handlers;

int sclp_service_call(CPUS390XState *env, uint64_t sccb_addr, uint32_t code)
{
    struct SCCB *sccb = (struct SCCB *)&memory[sccb_addr];
    size_t i;

    for (i = 0; i < n_sclp_handlers; ++i)
        if ((code & sclp_handlers[i].mask) == sclp_handlers[i].value) {
            int result = sclp_handlers[i].f(env, sccb, code);

            // sclp.c:237
            s390_sclp_extint((uint32_t)sccb & ~3);
            return result;
        }

    abort();
}

int slow_bitmap_empty(const unsigned long *bitmap, long bits)
{
    abort();
}

int stpcifc_service_call(S390CPU *cpu, uint8_t r1, uint64_t fiba, uint8_t ar,
                         uintptr_t ra)
{
    abort();
}

bool tcg_allowed = true;

void timer_mod(QEMUTimer *ts, int64_t expire_timer)
{
    abort();
}

void tlb_flush(CPUState *cpu)
{
}

void tlb_flush_all_cpus_synced(CPUState *src_cpu)
{
}

void tlb_flush_page(CPUState *cpu, target_ulong addr)
{
}

void tlb_flush_page_all_cpus_synced(CPUState *src, target_ulong addr)
{
}

void tlb_set_page(CPUState *cpu, target_ulong vaddr,
                  hwaddr paddr, int prot,
                  int mmu_idx, target_ulong size)
{
    abort();
}

void trace_enable_events(const char *line_buf)
{
    abort();
}

int trace_events_enabled_count;

Type type_register_static(const TypeInfo *info)
{
    if (info->name && strcmp(info->name, TYPE_QEMU_S390_FLIC) == 0) {
        if (info->class_init)
            info->class_init(&flic_class.parent_class.parent_class, NULL);
        if (info->instance_init)
            info->instance_init(&flic.parent_obj.parent_obj.parent_obj.parent_obj);
        flic.parent_obj.parent_obj.parent_obj.parent_obj.class =
                &flic_class.parent_class.parent_class;
    }
    return NULL;
}

const VMStateInfo vmstate_info_uint32;

const VMStateInfo vmstate_info_uint64;

const VMStateInfo vmstate_info_uint8;
