#ifndef GANYMEDE_H_
#define GANYMEDE_H_

#include <stdint.h>

#define TASK(_name, _setup_func, _loop_func, _stack_size)           \
    struct {                                                                   \
        task_info_t info;                                                      \
        uint8_t stack[_stack_size - sizeof(task_info_t)];                      \
    }                                                                          \
    _name __attribute__ ((section (".tasks"))) =                               \
    {                                                                          \
        .info =                                                                \
        {                                                                      \
            .magic = 0xBEEF,                                                   \
            .stack_size = _stack_size,                                         \
            .setup_func = _setup_func,                                         \
            .loop_func = _loop_func,                                           \
        }                                                                      \
    }

typedef struct semaphore_s
{
    volatile uint8_t counter;
}
semaphore_t;

typedef struct task_info_s
{
    union {
        struct {
            uint16_t magic;
            uint16_t stack_size;
        };

        void* next;
    };
    void (*setup_func)(void);
    void (*loop_func)(void);
}
task_info_t;


void semaphore_init(semaphore_t* sem, uint8_t counter);
void semaphore_wait(semaphore_t* sem);
void semaphore_signal(semaphore_t* sem);
uint8_t sched_self(void);
void sched_delay(uint16_t ms);


uint16_t timer_get_ticks(void);
uint16_t timer_ms2ticks(uint16_t ms, uint16_t* remaining_us);
uint16_t timer_ticks2ms(uint16_t ticks);

#endif /* GANYMEDE_H_ */

