/******************************************************************************
 * hypercall.h
 * 
 * Linux-specific hypervisor handling.
 * 
 * Copyright (c) 2002-2004, K A Fraser
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation; or, when distributed
 * separately from the Linux kernel or incorporated into other
 * software packages, subject to the following license:
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this source file (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __HYPERCALL_H__
#define __HYPERCALL_H__

#include <linux/string.h> /* memcpy() */

#ifndef __HYPERVISOR_H__
# error "please don't include this file directly"
#endif

#define __STR(x) #x
#define STR(x) __STR(x)

#ifdef CONFIG_XEN
#define HYPERCALL_STR(name)					\
	"call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"
#else
#define HYPERCALL_STR(name)					\
	"mov hypercall_stubs,%%eax; "				\
	"add $("STR(__HYPERVISOR_##name)" * 32),%%eax; "	\
	"call *%%eax"
#endif

#define _hypercall0(type, name)			\
({						\
	long __res;				\
	asm volatile (				\
		HYPERCALL_STR(name)		\
		: "=a" (__res)			\
		:				\
		: "memory" );			\
	(type)__res;				\
})

#define _hypercall1(type, name, a1)				\
({								\
	long __res, __ign1;					\
	asm volatile (						\
		HYPERCALL_STR(name)				\
		: "=a" (__res), "=b" (__ign1)			\
		: "1" ((long)(a1))				\
		: "memory" );					\
	(type)__res;						\
})

#define _hypercall2(type, name, a1, a2)				\
({								\
	long __res, __ign1, __ign2;				\
	asm volatile (						\
		HYPERCALL_STR(name)				\
		: "=a" (__res), "=b" (__ign1), "=c" (__ign2)	\
		: "1" ((long)(a1)), "2" ((long)(a2))		\
		: "memory" );					\
	(type)__res;						\
})

#define _hypercall3(type, name, a1, a2, a3)			\
({								\
	long __res, __ign1, __ign2, __ign3;			\
	asm volatile (						\
		HYPERCALL_STR(name)				\
		: "=a" (__res), "=b" (__ign1), "=c" (__ign2), 	\
		"=d" (__ign3)					\
		: "1" ((long)(a1)), "2" ((long)(a2)),		\
		"3" ((long)(a3))				\
		: "memory" );					\
	(type)__res;						\
})

#define _hypercall4(type, name, a1, a2, a3, a4)			\
({								\
	long __res, __ign1, __ign2, __ign3, __ign4;		\
	asm volatile (						\
		HYPERCALL_STR(name)				\
		: "=a" (__res), "=b" (__ign1), "=c" (__ign2),	\
		"=d" (__ign3), "=S" (__ign4)			\
		: "1" ((long)(a1)), "2" ((long)(a2)),		\
		"3" ((long)(a3)), "4" ((long)(a4))		\
		: "memory" );					\
	(type)__res;						\
})

#define _hypercall5(type, name, a1, a2, a3, a4, a5)		\
({								\
	long __res, __ign1, __ign2, __ign3, __ign4, __ign5;	\
	asm volatile (						\
		HYPERCALL_STR(name)				\
		: "=a" (__res), "=b" (__ign1), "=c" (__ign2),	\
		"=d" (__ign3), "=S" (__ign4), "=D" (__ign5)	\
		: "1" ((long)(a1)), "2" ((long)(a2)),		\
		"3" ((long)(a3)), "4" ((long)(a4)),		\
		"5" ((long)(a5))				\
		: "memory" );					\
	(type)__res;						\
})

static inline int
HYPERVISOR_set_trap_table(
	trap_info_t *table)
{
	return _hypercall1(int, set_trap_table, table);
}

static inline int
HYPERVISOR_mmu_update(
	mmu_update_t *req, int count, int *success_count, domid_t domid)
{
	return _hypercall4(int, mmu_update, req, count, success_count, domid);
}

static inline int
HYPERVISOR_mmuext_op(
	struct mmuext_op *op, int count, int *success_count, domid_t domid)
{
	return _hypercall4(int, mmuext_op, op, count, success_count, domid);
}

static inline int
HYPERVISOR_set_gdt(
	unsigned long *frame_list, int entries)
{
	return _hypercall2(int, set_gdt, frame_list, entries);
}

static inline int
HYPERVISOR_stack_switch(
	unsigned long ss, unsigned long esp)
{
	return _hypercall2(int, stack_switch, ss, esp);
}

static inline int
HYPERVISOR_set_callbacks(
	unsigned long event_selector, unsigned long event_address,
	unsigned long failsafe_selector, unsigned long failsafe_address)
{
	return _hypercall4(int, set_callbacks,
			   event_selector, event_address,
			   failsafe_selector, failsafe_address);
}

static inline int
HYPERVISOR_fpu_taskswitch(
	int set)
{
	return _hypercall1(int, fpu_taskswitch, set);
}

static inline int
HYPERVISOR_sched_op_compat(
	int cmd, unsigned long arg)
{
	return _hypercall2(int, sched_op_compat, cmd, arg);
}

static inline int
HYPERVISOR_sched_op(
	int cmd, void *arg)
{
	return _hypercall2(int, sched_op, cmd, arg);
}

static inline long
HYPERVISOR_set_timer_op(
	u64 timeout)
{
	unsigned long timeout_hi = (unsigned long)(timeout>>32);
	unsigned long timeout_lo = (unsigned long)timeout;
	return _hypercall2(long, set_timer_op, timeout_lo, timeout_hi);
}

static inline int
HYPERVISOR_dom0_op(
	dom0_op_t *dom0_op)
{
	dom0_op->interface_version = DOM0_INTERFACE_VERSION;
	return _hypercall1(int, dom0_op, dom0_op);
}

static inline int
HYPERVISOR_set_debugreg(
	int reg, unsigned long value)
{
	return _hypercall2(int, set_debugreg, reg, value);
}

static inline unsigned long
HYPERVISOR_get_debugreg(
	int reg)
{
	return _hypercall1(unsigned long, get_debugreg, reg);
}

static inline int
HYPERVISOR_update_descriptor(
	u64 ma, u64 desc)
{
	return _hypercall4(int, update_descriptor, ma, ma>>32, desc, desc>>32);
}

static inline int
HYPERVISOR_memory_op(
	unsigned int cmd, void *arg)
{
	return _hypercall2(int, memory_op, cmd, arg);
}

static inline int
HYPERVISOR_multicall(
	void *call_list, int nr_calls)
{
	return _hypercall2(int, multicall, call_list, nr_calls);
}

static inline int
HYPERVISOR_update_va_mapping(
	unsigned long va, pte_t new_val, unsigned long flags)
{
	unsigned long pte_hi = 0;
#ifdef CONFIG_X86_PAE
	pte_hi = new_val.pte_high;
#endif
	return _hypercall4(int, update_va_mapping, va,
			   new_val.pte_low, pte_hi, flags);
}

static inline int
HYPERVISOR_event_channel_op(
	int cmd, void *arg)
{
	int rc = _hypercall2(int, event_channel_op, cmd, arg);
	if (unlikely(rc == -ENOSYS)) {
		struct evtchn_op op;
		op.cmd = cmd;
		memcpy(&op.u, arg, sizeof(op.u));
		rc = _hypercall1(int, event_channel_op_compat, &op);
		memcpy(arg, &op.u, sizeof(op.u));
	}
	return rc;
}

static inline int
HYPERVISOR_acm_op(
	int cmd, void *arg)
{
	return _hypercall2(int, acm_op, cmd, arg);
}

static inline int
HYPERVISOR_xen_version(
	int cmd, void *arg)
{
	return _hypercall2(int, xen_version, cmd, arg);
}

static inline int
HYPERVISOR_console_io(
	int cmd, int count, char *str)
{
	return _hypercall3(int, console_io, cmd, count, str);
}

static inline int
HYPERVISOR_physdev_op(
	int cmd, void *arg)
{
	int rc = _hypercall2(int, physdev_op, cmd, arg);
	if (unlikely(rc == -ENOSYS)) {
		struct physdev_op op;
		op.cmd = cmd;
		memcpy(&op.u, arg, sizeof(op.u));
		rc = _hypercall1(int, physdev_op_compat, &op);
		memcpy(arg, &op.u, sizeof(op.u));
	}
	return rc;
}

static inline int
HYPERVISOR_grant_table_op(
	unsigned int cmd, void *uop, unsigned int count)
{
	return _hypercall3(int, grant_table_op, cmd, uop, count);
}

static inline int
HYPERVISOR_update_va_mapping_otherdomain(
	unsigned long va, pte_t new_val, unsigned long flags, domid_t domid)
{
	unsigned long pte_hi = 0;
#ifdef CONFIG_X86_PAE
	pte_hi = new_val.pte_high;
#endif
	return _hypercall5(int, update_va_mapping_otherdomain, va,
			   new_val.pte_low, pte_hi, flags, domid);
}

static inline int
HYPERVISOR_vm_assist(
	unsigned int cmd, unsigned int type)
{
	return _hypercall2(int, vm_assist, cmd, type);
}

static inline int
HYPERVISOR_vcpu_op(
	int cmd, int vcpuid, void *extra_args)
{
	return _hypercall3(int, vcpu_op, cmd, vcpuid, extra_args);
}

static inline int
HYPERVISOR_suspend(
	unsigned long srec)
{
	struct sched_shutdown sched_shutdown = {
		.reason = SHUTDOWN_suspend
	};

	int rc = _hypercall3(int, sched_op, SCHEDOP_shutdown,
			     &sched_shutdown, srec);

	if (rc == -ENOSYS)
		rc = _hypercall3(int, sched_op_compat, SCHEDOP_shutdown,
				 SHUTDOWN_suspend, srec);

	return rc;
}

static inline int
HYPERVISOR_nmi_op(
	unsigned long op, void *arg)
{
	return _hypercall2(int, nmi_op, op, arg);
}

static inline unsigned long
HYPERVISOR_hvm_op(
    int op, void *arg)
{
    return _hypercall2(unsigned long, hvm_op, op, arg);
}

static inline int
HYPERVISOR_callback_op(
	int cmd, void *arg)
{
	return _hypercall2(int, callback_op, cmd, arg);
}

static inline int
HYPERVISOR_xenoprof_op(
	int op, void *arg)
{
	return _hypercall2(int, xenoprof_op, op, arg);
}


#endif /* __HYPERCALL_H__ */
