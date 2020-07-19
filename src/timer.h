#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t timer_time_t;
typedef int32_t timer_time_diff_t;
typedef void (*timer_callback_t)(void* context);

typedef struct timer_task {
    timer_callback_t callback;
    void* context;

    timer_time_t _end_time;
    struct timer_task* _next;
} timer_task_t;

timer_time_t _timer_get_time(void);

void timer_update(void);
__attribute__((nonnull(1))) void timer_add_new(timer_task_t* task, timer_time_t duration);
__attribute__((nonnull)) bool timer_remove(timer_task_t* task);
__attribute__((nonnull)) bool timer_is_pending(timer_task_t* task);
