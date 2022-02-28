#ifndef _ASM_X86_MSHYPER_H
#define _ASM_X86_MSHYPER_H

#include <linux/init.h>
#include <linux/clocksource.h>

int ms_hyperv_platform(void);
void __cpuinit ms_hyperv_set_feature_bits(struct cpuinfo_x86 *c);
void __cpuinit init_hv_clocksource(void);
cycle_t read_hv_clock(void);
unsigned long hyperv_get_tsc_khz(void);

extern unsigned int use_hv_time;

#define HV_REF_COUNTER_FREQUENCY 10000000UL

#endif
