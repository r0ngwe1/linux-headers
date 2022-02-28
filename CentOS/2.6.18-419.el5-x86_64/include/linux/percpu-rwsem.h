#ifndef _LINUX_PERCPU_RWSEM_H
#define _LINUX_PERCPU_RWSEM_H

#include <linux/mutex.h>
#include <linux/percpu.h>
#include <linux/rcupdate.h>
#include <linux/delay.h>
#include <asm/local.h>

struct percpu_rw_semaphore {
	local_t *counters;
	bool locked;
	struct mutex mtx;
};

#define light_mb()	smp_mb()
#define heavy_mb()	smp_mb()

static inline void percpu_down_read(struct percpu_rw_semaphore *p)
{
	rcu_read_lock();
	if (unlikely(p->locked)) {
		rcu_read_unlock();
		mutex_lock(&p->mtx);
		preempt_disable();
		local_inc(per_cpu_ptr(p->counters, smp_processor_id()));
		preempt_enable();
		mutex_unlock(&p->mtx);
		return;
	}
	preempt_disable();
	local_inc(per_cpu_ptr(p->counters, smp_processor_id()));
	preempt_enable();
	rcu_read_unlock();
	light_mb(); /* A, between read of p->locked and read of data, paired with D */
}

static inline void percpu_up_read(struct percpu_rw_semaphore *p)
{
	light_mb(); /* B, between read of the data and write to p->counter, paired with C */
	preempt_disable();
	local_dec(per_cpu_ptr(p->counters, smp_processor_id()));
	preempt_enable();
}

static inline unsigned long __percpu_count(local_t *counters)
{
	unsigned long total = 0;
	int cpu;

	for_each_possible_cpu(cpu)
		total += local_read(per_cpu_ptr(counters, cpu));

	return total;
}

static inline void percpu_down_write(struct percpu_rw_semaphore *p)
{
	mutex_lock(&p->mtx);
	p->locked = true;
	synchronize_rcu();
	while (__percpu_count(p->counters))
		msleep(1);
	heavy_mb(); /* C, between read of p->counter and write to data, paired with B */
}

static inline void percpu_up_write(struct percpu_rw_semaphore *p)
{
	heavy_mb(); /* D, between write to data and write to p->locked, paired with A */
	p->locked = false;
	mutex_unlock(&p->mtx);
}

static inline int percpu_init_rwsem(struct percpu_rw_semaphore *p)
{
	int cpu;
	p->counters = alloc_percpu(local_t);
	if (unlikely(!p->counters))
		return -ENOMEM;
	for_each_possible_cpu(cpu)
		local_set(per_cpu_ptr(p->counters, cpu), 0);
	p->locked = false;
	mutex_init(&p->mtx);
	return 0;
}

static inline void percpu_free_rwsem(struct percpu_rw_semaphore *p)
{
	free_percpu(p->counters);
	p->counters = NULL; /* catch use after free bugs */
}

#endif
