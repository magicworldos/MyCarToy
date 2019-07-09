#ifndef __TIMER_H
#define __TIMER_H

#include <typedef.h>

void timer_init();

void timer_delay_ms(uint64_t ms);

void timer_decrement();

#endif
