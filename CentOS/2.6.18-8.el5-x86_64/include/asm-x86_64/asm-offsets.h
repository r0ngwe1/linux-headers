#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__
/*
 * DO NOT MODIFY.
 *
 * This file was generated by /builddir/build/BUILD/kernel-2.6.18/linux-2.6.18.x86_64/Kbuild
 *
 */

#define tsk_state 0 /* offsetof(struct task_struct, state)	# */
#define tsk_flags 24 /* offsetof(struct task_struct, flags)	# */
#define tsk_thread 736 /* offsetof(struct task_struct, thread)	# */
#define tsk_pid 292 /* offsetof(struct task_struct, pid)	# */

#define threadinfo_flags 16 /* offsetof(struct thread_info, flags)	# */
#define threadinfo_addr_limit 32 /* offsetof(struct thread_info, addr_limit)	# */
#define threadinfo_preempt_count 28 /* offsetof(struct thread_info, preempt_count)	# */
#define threadinfo_status 20 /* offsetof(struct thread_info, status)	# */

#define pda_kernelstack 16 /* offsetof(struct x8664_pda, kernelstack)	# */
#define pda_oldrsp 24 /* offsetof(struct x8664_pda, oldrsp)	# */
#define pda_pcurrent 0 /* offsetof(struct x8664_pda, pcurrent)	# */
#define pda_irqcount 40 /* offsetof(struct x8664_pda, irqcount)	# */
#define pda_cpunumber 44 /* offsetof(struct x8664_pda, cpunumber)	# */
#define pda_irqstackptr 48 /* offsetof(struct x8664_pda, irqstackptr)	# */
#define pda_data_offset 8 /* offsetof(struct x8664_pda, data_offset)	# */

#define IA32_SIGCONTEXT_eax 44 /* offsetof(struct sigcontext_ia32, eax)	# */
#define IA32_SIGCONTEXT_ebx 32 /* offsetof(struct sigcontext_ia32, ebx)	# */
#define IA32_SIGCONTEXT_ecx 40 /* offsetof(struct sigcontext_ia32, ecx)	# */
#define IA32_SIGCONTEXT_edx 36 /* offsetof(struct sigcontext_ia32, edx)	# */
#define IA32_SIGCONTEXT_esi 20 /* offsetof(struct sigcontext_ia32, esi)	# */
#define IA32_SIGCONTEXT_edi 16 /* offsetof(struct sigcontext_ia32, edi)	# */
#define IA32_SIGCONTEXT_ebp 24 /* offsetof(struct sigcontext_ia32, ebp)	# */
#define IA32_SIGCONTEXT_esp 28 /* offsetof(struct sigcontext_ia32, esp)	# */
#define IA32_SIGCONTEXT_eip 56 /* offsetof(struct sigcontext_ia32, eip)	# */

#define IA32_RT_SIGFRAME_sigcontext 164 /* offsetof (struct rt_sigframe32, uc.uc_mcontext)	# */

#define pbe_address 0 /* offsetof(struct pbe, address)	# */
#define pbe_orig_address 8 /* offsetof(struct pbe, orig_address)	# */
#define pbe_next 16 /* offsetof(struct pbe, next)	# */

#define TSS_ist 36 /* offsetof(struct tss_struct, ist)	# */

#define crypto_tfm_ctx_offset 80 /* offsetof(struct crypto_tfm, __crt_ctx)	# */

#endif
