#ifndef SCHED_H_
#define SCHED_H_

#include <stdint.h>
#include <stddef.h>

typedef struct sched_context_s* sched_context_t;

void sched_init(void);
void sched_loop(void);

void platform_irq_get_enabled(uint8_t* irq_status);
void platform_idle(void);
void platform_context_create(sched_context_t ctx, void (*func)(), void* stack, size_t stack_size);
void platform_context_swap(sched_context_t save, sched_context_t load);


#endif /* SCHED_H_ */

