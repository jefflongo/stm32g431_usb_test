#include "stm32_assert.h"

static volatile char const* _assert_file = 0;
static volatile unsigned _assert_line = 0;

void assert_failed(char const* file, unsigned line) {
    _assert_file = file;
    _assert_line = line;

    while (1)
        ;
}
