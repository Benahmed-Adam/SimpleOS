#pragma once

#include <stdint.h>

void init_timer(uint32_t frequency);
void sleep(uint32_t ms);
uint32_t uptime();