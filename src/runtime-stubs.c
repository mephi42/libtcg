#include <stdlib.h>

#include "qemu/osdep.h"
#include "target/s390x/cpu.h"

#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include "hw/s390x/storage-keys.h"
#include "libtcg.h"
#include "sysemu/sysemu.h"
#include "target/s390x/internal.h"
#include "tcg/tcg.h"
#include "trace/control.h"
#include "trace/control-internal.h"
#include "trace-root.h"

TraceEvent _TRACE_GUEST_MEM_BEFORE_EXEC_EVENT;

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

void HELPER(exception)(CPUS390XState *env, uint32_t excp)
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

int mmu_translate(CPUS390XState *env, target_ulong vaddr, int rw, uint64_t asc,
                  target_ulong *raddr, int *flags, bool exc)
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

unsigned int s390_cpu_halt(S390CPU *cpu)
{
    abort();
}

bool tcg_allowed = true;

ObjectClass *object_get_class(Object *obj)
{
    abort();
}

void probe_write(CPUArchState *env, target_ulong addr, int size, int mmu_idx,
                 uintptr_t retaddr)
{
    abort();
}

int qemu_log(const char *fmt, ...)
{
    abort();
}

int qemu_loglevel;

void qemu_system_guest_panicked(GuestPanicInformation *info)
{
    abort();
}

void qemu_system_shutdown_request(ShutdownCause reason)
{
    abort();
}

ram_addr_t ram_size = RAM_SIZE;

int s390_cpu_handle_mmu_fault(CPUState *cpu, vaddr address, int size, int rw,
                              int mmu_idx)
{
    abort();
}

void s390_cpu_recompute_watchpoints(CPUState *cs)
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
