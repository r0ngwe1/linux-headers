#ifndef _ASM_X86_UACCESS_64_H
#define _ASM_X86_UACCESS_64_H

/*
 * User space memory access functions
 */
#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/lockdep.h>
#include <asm/alternative.h>
#include <asm/cpufeatures.h>
#include <asm/page.h>

/*
 * Copy To/From Userspace
 */

/* Handles exceptions in both to and from, but doesn't do access_ok */
__must_check unsigned long
copy_user_enhanced_fast_string(void *to, const void *from, unsigned len);
__must_check unsigned long
copy_user_generic_string(void *to, const void *from, unsigned len);
__must_check unsigned long
copy_user_generic_unrolled(void *to, const void *from, unsigned len);

static __always_inline __must_check unsigned long
copy_user_generic(void *to, const void *from, unsigned len)
{
	unsigned ret;

	/*
	 * If CPU has ERMS feature, use copy_user_enhanced_fast_string.
	 * Otherwise, if CPU has rep_good feature, use copy_user_generic_string.
	 * Otherwise, use copy_user_generic_unrolled.
	 */
	alternative_call_2(copy_user_generic_unrolled,
			 copy_user_generic_string,
			 X86_FEATURE_REP_GOOD,
			 copy_user_enhanced_fast_string,
			 X86_FEATURE_ERMS,
			 ASM_OUTPUT2("=a" (ret), "=D" (to), "=S" (from),
				     "=d" (len)),
			 "1" (to), "2" (from), "3" (len)
			 : "memory", "rcx", "r8", "r9", "r10", "r11");
	return ret;
}

static __always_inline __must_check unsigned long
copy_to_user_mcsafe(void *to, const void *from, unsigned len)
{
	unsigned long ret;

	__uaccess_begin();
	/*
	 * Note, __memcpy_mcsafe() is explicitly used since it can
	 * handle exceptions / faults.  memcpy_mcsafe() may fall back to
	 * memcpy() which lacks this handling.
	 */
	ret = __memcpy_mcsafe(to, from, len);
	__uaccess_end();
	return ret;
}

__must_check unsigned long
_copy_to_user(void __user *to, const void *from, unsigned len);
__must_check unsigned long
_copy_from_user(void *to, const void __user *from, unsigned len);
__must_check unsigned long
copy_in_user(void __user *to, const void __user *from, unsigned len);

static inline unsigned long __must_check copy_from_user(void *to,
					  const void __user *from,
					  unsigned long n)
{
	int sz = __compiletime_object_size(to);

	might_fault();
	if (likely(sz == -1 || sz >= n)) {
		check_object_size(to, n, false);
		n = _copy_from_user(to, from, n);
	}
#ifdef CONFIG_DEBUG_VM
	else
		WARN(1, "Buffer overflow detected!\n");
#endif
	return n;
}

static __always_inline __must_check
int copy_to_user(void __user *dst, const void *src, unsigned size)
{
	int sz = __compiletime_object_size(src);

	might_fault();
	if (likely(sz == -1 || sz >= size)) {
		check_object_size(src, size, true);
		size = (unsigned)_copy_to_user(dst, src, size);
	}
	return (int)size;
}

static __always_inline __must_check
int __copy_from_user(void *dst, const void __user *src, unsigned size)
{
	int ret = 0;

	might_fault();

	check_object_size(dst, size, false);

	if (!__builtin_constant_p(size))
		return copy_user_generic(dst, (__force void *)src, size);
	switch (size) {
	case 1:
		__uaccess_begin_nospec();
		__get_user_asm(*(u8 *)dst, (u8 __user *)src,
			      ret, "b", "b", "=q", 1);
		__uaccess_end();
		return ret;
	case 2:
		__uaccess_begin_nospec();
		__get_user_asm(*(u16 *)dst, (u16 __user *)src,
			      ret, "w", "w", "=r", 2);
		__uaccess_end();
		return ret;
	case 4:
		__uaccess_begin_nospec();
		__get_user_asm(*(u32 *)dst, (u32 __user *)src,
			      ret, "l", "k", "=r", 4);
		__uaccess_end();
		return ret;
	case 8:
		__uaccess_begin_nospec();
		__get_user_asm(*(u64 *)dst, (u64 __user *)src,
			      ret, "q", "", "=r", 8);
		__uaccess_end();
		return ret;
	case 10:
		__uaccess_begin_nospec();
		__get_user_asm(*(u64 *)dst, (u64 __user *)src,
			       ret, "q", "", "=r", 10);
		if (likely(!ret))
			__get_user_asm(*(u16 *)(8 + (char *)dst),
				       (u16 __user *)(8 + (char __user *)src),
				       ret, "w", "w", "=r", 2);
		__uaccess_end();
		return ret;
	case 16:
		__uaccess_begin_nospec();
		__get_user_asm(*(u64 *)dst, (u64 __user *)src,
			       ret, "q", "", "=r", 16);
		if (likely(!ret))
			__get_user_asm(*(u64 *)(8 + (char *)dst),
				       (u64 __user *)(8 + (char __user *)src),
				       ret, "q", "", "=r", 8);
		__uaccess_end();
		return ret;
	default:
		return copy_user_generic(dst, (__force void *)src, size);
	}
}

static __always_inline __must_check
int __copy_to_user(void __user *dst, const void *src, unsigned size)
{
	int ret = 0;

	might_fault();

	check_object_size(src, size, true);

	if (!__builtin_constant_p(size))
		return copy_user_generic((__force void *)dst, src, size);
	switch (size) {
	case 1:
		__uaccess_begin();
		__put_user_asm(*(u8 *)src, (u8 __user *)dst,
			      ret, "b", "b", "iq", 1);
		__uaccess_end();
		return ret;
	case 2:
		__uaccess_begin();
		__put_user_asm(*(u16 *)src, (u16 __user *)dst,
			      ret, "w", "w", "ir", 2);
		__uaccess_end();
		return ret;
	case 4:
		__uaccess_begin();
		__put_user_asm(*(u32 *)src, (u32 __user *)dst,
			      ret, "l", "k", "ir", 4);
		__uaccess_end();
		return ret;
	case 8:
		__uaccess_begin();
		__put_user_asm(*(u64 *)src, (u64 __user *)dst,
			      ret, "q", "", "er", 8);
		__uaccess_end();
		return ret;
	case 10:
		__uaccess_begin();
		__put_user_asm(*(u64 *)src, (u64 __user *)dst,
			       ret, "q", "", "er", 10);
		if (likely(!ret)) {
			asm("":::"memory");
			__put_user_asm(4[(u16 *)src], 4 + (u16 __user *)dst,
				       ret, "w", "w", "ir", 2);
		}
		__uaccess_end();
		return ret;
	case 16:
		__uaccess_begin();
		__put_user_asm(*(u64 *)src, (u64 __user *)dst,
			       ret, "q", "", "er", 16);
		if (likely(!ret)) {
			asm("":::"memory");
			__put_user_asm(1[(u64 *)src], 1 + (u64 __user *)dst,
				       ret, "q", "", "er", 8);
		}
		__uaccess_end();
		return ret;
	default:
		return copy_user_generic((__force void *)dst, src, size);
	}
}

static __always_inline __must_check
int __copy_in_user(void __user *dst, const void __user *src, unsigned size)
{
	int ret = 0;

	might_fault();
	if (!__builtin_constant_p(size))
		return copy_user_generic((__force void *)dst,
					 (__force void *)src, size);
	switch (size) {
	case 1: {
		u8 tmp;
		__uaccess_begin();
		__get_user_asm(tmp, (u8 __user *)src,
			       ret, "b", "b", "=q", 1);
		if (likely(!ret))
			__put_user_asm(tmp, (u8 __user *)dst,
				       ret, "b", "b", "iq", 1);
		__uaccess_end();
		return ret;
	}
	case 2: {
		u16 tmp;
		__uaccess_begin();
		__get_user_asm(tmp, (u16 __user *)src,
			       ret, "w", "w", "=r", 2);
		if (likely(!ret))
			__put_user_asm(tmp, (u16 __user *)dst,
				       ret, "w", "w", "ir", 2);
		__uaccess_end();
		return ret;
	}

	case 4: {
		u32 tmp;
		__uaccess_begin();
		__get_user_asm(tmp, (u32 __user *)src,
			       ret, "l", "k", "=r", 4);
		if (likely(!ret))
			__put_user_asm(tmp, (u32 __user *)dst,
				       ret, "l", "k", "ir", 4);
		__uaccess_end();
		return ret;
	}
	case 8: {
		u64 tmp;
		__uaccess_begin();
		__get_user_asm(tmp, (u64 __user *)src,
			       ret, "q", "", "=r", 8);
		if (likely(!ret))
			__put_user_asm(tmp, (u64 __user *)dst,
				       ret, "q", "", "er", 8);
		__uaccess_end();
		return ret;
	}
	default:
		return copy_user_generic((__force void *)dst,
					 (__force void *)src, size);
	}
}

static __must_check __always_inline int
__copy_from_user_inatomic(void *dst, const void __user *src, unsigned size)
{
	return copy_user_generic(dst, (__force const void *)src, size);
}

static __must_check __always_inline int
__copy_to_user_inatomic(void __user *dst, const void *src, unsigned size)
{
	return copy_user_generic((__force void *)dst, src, size);
}

extern long __copy_user_nocache(void *dst, const void __user *src,
				unsigned size, int zerorest);

extern long __copy_user_flushcache(void *dst, const void __user *src, unsigned size);
extern void memcpy_page_flushcache(char *to, struct page *page, size_t offset,
			   size_t len);

static inline int
__copy_from_user_nocache(void *dst, const void __user *src, unsigned size)
{
	might_fault();
	return __copy_user_nocache(dst, src, size, 1);
}

static inline int
__copy_from_user_inatomic_nocache(void *dst, const void __user *src,
				  unsigned size)
{
	return __copy_user_nocache(dst, src, size, 0);
}

static inline int
__copy_from_user_flushcache(void *dst, const void __user *src, unsigned size)
{
	return __copy_user_flushcache(dst, src, size);
}

unsigned long
copy_user_handle_tail(char *to, char *from, unsigned len, unsigned zerorest);

unsigned long
mcsafe_handle_tail(char *to, char *from, unsigned len);

#endif /* _ASM_X86_UACCESS_64_H */
