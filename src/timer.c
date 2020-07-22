#include "timer.h"

#include <stddef.h>

static timer_task_t* head = NULL;

void timer_update(void) {
    timer_time_t now = _timer_get_time();
    timer_task_t* current = head;
    while (current) {
        if ((timer_time_diff_t)(current->_end_time - now) <= 0) {
            timer_task_t* expired = current;
            current = current->_next;
            timer_remove(expired);
            if (expired->callback) {
                expired->callback(expired->context);
            }
        } else {
            current = current->_next;
        }
    }
}

void timer_add_new(timer_task_t* task, timer_time_t duration) {
    timer_time_t now = _timer_get_time();
    if (timer_is_pending(task)) return;

    task->_end_time = now + duration;

    if (head) {
        timer_task_t* prev_head = head;
        head = task;
        head->_next = prev_head;
    } else {
        head = task;
    }
}

bool timer_remove(timer_task_t* task) {
    if (!head) {
        return false;
    }

    if (head == task) {
        head = head->_next;
        task->_next = NULL;
        return true;
    }

    timer_task_t* current = head;
    while (current->_next) {
        if (current->_next == task) {
            timer_task_t* to_remove = current->_next;
            current->_next = to_remove->_next;
            to_remove->_next = NULL;
            return true;
        }
    }
    return false;
}

bool timer_is_pending(timer_task_t* task) {
    if (!head) {
        return false;
    }

    timer_task_t* current = head;
    while (current) {
        if (current == task) {
            return true;
        }
        current = current->_next;
    }

    return false;
}
