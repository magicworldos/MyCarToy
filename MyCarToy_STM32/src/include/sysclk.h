#ifndef _SYSCLK
#define _SYSCLK

#include <typedef.h>

void sysclk_init();

void sysclk_start();

void sysclk_delay_ms(uint32_t ms);

void sysclk_delay_us(uint32_t us);

void sysclk_decrement();

uint64_t getTm();

#endif

