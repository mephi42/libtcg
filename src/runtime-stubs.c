#include <stdlib.h>

#include "qemu/osdep.h"
#include "target/s390x/cpu.h"

#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include "hw/s390x/s390_flic.h"
#include "hw/s390x/s390-pci-inst.h"
#include "hw/s390x/s390-virtio-hcall.h"
#include "hw/s390x/sclp.h"
#include "hw/s390x/storage-keys.h"
#include "libtcg.h"
#include "sysemu/sysemu.h"
#include "target/s390x/internal.h"
#include "tcg/tcg.h"
#include "trace/control.h"
#include "trace/control-internal.h"
#include "trace-root.h"

TraceEvent _TRACE_GUEST_MEM_BEFORE_EXEC_EVENT;

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

void cpu_inject_clock_comparator(S390CPU *cpu)
{
    abort();
}

void cpu_inject_cpu_timer(S390CPU *cpu)
{
    abort();
}

void cpu_loop_exit(CPUState *cpu)
{
    abort();
}

void cpu_loop_exit_atomic(CPUState *cpu, uintptr_t pc)
{
    abort();
}

void cpu_loop_exit_restore(CPUState *cpu, uintptr_t pc)
{
    abort();
}

void *cpu_physical_memory_map(hwaddr addr,
                              hwaddr *plen,
                              int is_write)
{
    abort();
}

void cpu_physical_memory_rw(hwaddr addr, uint8_t *buf,
                            int len, int is_write)
{
    abort();
}

void cpu_physical_memory_unmap(void *buffer, hwaddr len,
                               int is_write, hwaddr access_len)
{
    abort();
}

int cpu_watchpoint_insert(CPUState *cpu, vaddr addr, vaddr len,
                          int flags, CPUWatchpoint **watchpoint)
{
    abort();
}

void cpu_watchpoint_remove_all(CPUState *cpu, int mask)
{
    abort();
}

void g_assertion_message_expr(const char *domain,
                              const char *file,
                              int line,
                              const char *func,
                              const char *expr)
{
    abort();
}

void g_free(gpointer mem)
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

uint32_t helper_be_ldl_cmmu(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

uint64_t helper_be_ldq_mmu(CPUArchState *env, target_ulong addr,
                           TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

tcg_target_ulong helper_be_ldul_mmu(CPUArchState *env, target_ulong addr,
                                    TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

tcg_target_ulong helper_be_lduw_mmu(CPUArchState *env, target_ulong addr,
                                    TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

uint16_t helper_be_ldw_cmmu(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

void helper_be_stl_mmu(CPUArchState *env, target_ulong addr, uint32_t val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

void helper_be_stq_mmu(CPUArchState *env, target_ulong addr, uint64_t val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

void helper_be_stw_mmu(CPUArchState *env, target_ulong addr, uint16_t val,
                       TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

uint8_t helper_ret_ldb_cmmu(CPUArchState *env, target_ulong addr,
                            TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

tcg_target_ulong helper_ret_ldub_mmu(CPUArchState *env, target_ulong addr,
                                     TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
}

void helper_ret_stb_mmu(CPUArchState *env, target_ulong addr, uint8_t val,
                        TCGMemOpIdx oi, uintptr_t retaddr)
{
    abort();
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

int mmu_translate(CPUS390XState *env, target_ulong vaddr, int rw, uint64_t asc,
                  target_ulong *raddr, int *flags, bool exc)
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
    abort();
}

Object *object_dynamic_cast_assert(Object *obj, const char *typename,
                                   const char *file, int line, const char *func)
{
    abort();
}

ObjectClass *object_get_class(Object *obj)
{
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

Object *qdev_get_machine(void)
{
    abort();
}

int64_t qemu_clock_get_ns(QEMUClockType type)
{
    abort();
}

int qemu_log(const char *fmt, ...)
{
    abort();
}

int qemu_loglevel;

void qemu_mutex_lock_iothread(void)
{
    abort();
}

void qemu_mutex_unlock_iothread(void)
{
    abort();
}

const char *qemu_name = "qemu";

QEMUS390FlicIO *qemu_s390_flic_dequeue_io(QEMUS390FLICState *flic,
                                          uint64_t cr6)
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

int rpcit_service_call(S390CPU *cpu, uint8_t r1, uint8_t r2, uintptr_t ra)
{
    abort();
}

int s390_cpu_handle_mmu_fault(CPUState *cpu, vaddr address, int size, int rw,
                              int mmu_idx)
{
    abort();
}

unsigned int s390_cpu_halt(S390CPU *cpu)
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
    abort();
}

void s390_get_feat_block(S390FeatType type, uint8_t *data)
{
    abort();
}

S390FLICState *s390_get_flic(void)
{
    abort();
}

QEMUS390FLICState *s390_get_qemu_flic(S390FLICState *fs)
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

void s390_program_interrupt(CPUS390XState *env, uint32_t code, int ilen,
                            uintptr_t ra)
{
    abort();
}

void s390_io_interrupt(uint16_t subchannel_id, uint16_t subchannel_nr,
                       uint32_t io_int_parm, uint32_t io_int_word)
{
    abort();
}

int s390_virtio_hypercall(CPUS390XState *env)
{
    abort();
}

int sclp_service_call(CPUS390XState *env, uint64_t sccb, uint32_t code)
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
    abort();
}

void tlb_flush_all_cpus_synced(CPUState *src_cpu)
{
    abort();
}

void tlb_flush_page(CPUState *cpu, target_ulong addr)
{
    abort();
}

void tlb_flush_page_all_cpus_synced(CPUState *src, target_ulong addr)
{
    abort();
}

int trace_events_enabled_count;
