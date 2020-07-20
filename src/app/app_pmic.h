#pragma once

#include <stdbool.h>

bool pmic_init(void);

void bq24292i_on_interrupt(void);
void max17048_on_interrupt(void);
void stusb4500_on_interrupt(void);
