#include <led.h>

void led_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	led_off(0);
	led_off(1);
	led_off(2);
	led_off(3);
}

void led_on(int i)
{
	switch (i)
	{
		case 0:
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			break;

		case 1:
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);
			break;

		case 2:
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
			break;

		case 3:
			GPIO_ResetBits(GPIOB, GPIO_Pin_15);
			break;

		default:
			break;
	}

}

void led_off(int i)
{
	switch (i)
	{
		case 0:
			GPIO_SetBits(GPIOB, GPIO_Pin_12);
			break;

		case 1:
			GPIO_SetBits(GPIOB, GPIO_Pin_13);
			break;

		case 2:
			GPIO_SetBits(GPIOB, GPIO_Pin_14);
			break;

		case 3:
			GPIO_SetBits(GPIOB, GPIO_Pin_15);
			break;

		default:
			break;
	}
}
