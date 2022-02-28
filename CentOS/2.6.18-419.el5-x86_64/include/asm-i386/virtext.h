#ifndef _ASM_I386_VIRTEX_H
#define _ASM_I386_VIRTEX_H

static inline int on_a_virt_guest(void)
{
	unsigned int eax, ebx, ecx, edx;
	char signature[13];

	cpuid(0x40000000, &eax, &ebx, &ecx, &edx);
	memcpy(signature + 0, &ebx, 4);
	memcpy(signature + 4, &ecx, 4);
	memcpy(signature + 8, &edx, 4);
	signature[12] = 0;

	if (strcmp(signature, "KVMKVMKVM") == 0)
		return 1;

	if (strcmp(signature, "XenVMMXenVMM") == 0)
		return 1;

	return 0;
}

#endif
