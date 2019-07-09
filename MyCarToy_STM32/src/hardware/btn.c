#include <btn.h>
#include <led.h>

void btn_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
	GPIO_SetBits(GPIOA, GPIO_Pin_12);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	GPIO_SetBits(GPIOC, GPIO_Pin_10);
	GPIO_SetBits(GPIOC, GPIO_Pin_11);
}

int btn_get_value(int i)
{
	switch (i)
	{
		case 0:
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_RESET)
			{
				return 1;
			}
			else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_SET)
			{
				return 0;
			}
			break;

		case 1:
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_RESET)
			{
				return 1;
			}
			else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_SET)
			{
				return 0;
			}
			break;

		case 2:
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET)
			{
				return 1;
			}
			else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_SET)
			{
				return 0;
			}
			break;

		case 3:
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_RESET)
			{
				return 1;
			}
			else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_SET)
			{
				return 0;
			}
			break;

		case 4:
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == Bit_RESET)
			{
				return 1;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == Bit_SET)
			{
				return 0;
			}
			break;

		case 5:
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_RESET)
			{
				return 1;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET)
			{
				return 0;
			}
			break;

		default:
			break;
	}
	return 0;
}
