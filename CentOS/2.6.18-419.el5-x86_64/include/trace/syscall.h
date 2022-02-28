#ifndef _TRACE_SYSCALL_H
#define _TRACE_SYSCALL_H

#include <linux/tracepoint.h>
#include <asm/ptrace.h>

#ifdef CONFIG_HAVE_SYSCALL_TRACEPOINTS

extern void syscall_regfunc(void);
extern void syscall_unregfunc(void);

#endif

DEFINE_TRACE_WITH_CALLBACK(syscall_enter,
	TPPROTO(struct pt_regs *regs, long id),
	TPARGS(regs, id),
	syscall_regfunc,
	syscall_unregfunc
);

DEFINE_TRACE_WITH_CALLBACK(syscall_exit,
	TPPROTO(struct pt_regs *regs, long ret),
	TPARGS(regs, ret),
	syscall_regfunc,
	syscall_unregfunc
);

#endif
