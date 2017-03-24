#ifndef SCHED_H_
#define SCHED_H_

#include <setjmp.h>
#include <stdint.h>

void sched_init(void);
void sched_loop(void);

void platform_set_sp(jmp_buf jmpb, void* sp);
void platform_irq_get_enabled(uint8_t* irq_status);

#endif /* SCHED_H_ */

