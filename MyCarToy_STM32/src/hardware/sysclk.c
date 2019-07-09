#include <sysclk.h>

__IO uint64_t sysclk_delay = 0;
__IO uint64_t sysclk_tick = 0;

void sysclk_init()
{
	if (SysTick_Config(72))
	{
		while (1)
		{
		}
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

	sysclk_start();
}

void sysclk_start()
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void sysclk_delay_ms(uint32_t ms)
{
	sysclk_delay = ms * 1000;
	while (sysclk_delay != 0)
	{
	}
}

void sysclk_decrement()
{
	if (sysclk_delay != 0)
	{
		sysclk_delay--;
	}
	sysclk_tick++;
}

void SysTick_Handler(void)
{
	sysclk_decrement();
}
