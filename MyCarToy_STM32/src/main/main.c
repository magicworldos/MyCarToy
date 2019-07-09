#include <typedef.h>
#include <printf.h>
#include <led.h>
#include <timer.h>
#include <uart.h>
#include <btn.h>
#include <sysclk.h>
#include <pwm.h>
#include <nrf.h>

float btn_v_l[6] = { 0 };
float btn_v[6] = { 0 };
uint8_t btn[6] = { 0 };
uint16_t pwm[8] = { 0 };

uint32_t nrf_rx[16] = { 0 }; //发送缓冲
uint8_t status = 0;

float pwm_p[2] = { 0 };
float pwm_pl[2] = { 0 };

float filter = 0.2f;
float filter_t = 0.5f;
float turn_val = 0.3f;

uint8_t mode = 0;

void RCC_config()
{
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);

	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);

		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
}

int main(int argc, char *argv[])
{
	SystemInit();

	RCC_config();

	led_init();

	sysclk_init();

	btn_init();

	uart_init();

	timer_init();

	pwm_init();

	nrf_init();

	status = NRF_Check();
	if (status == SUCCESS)
	{
		printf("NRF MCU connect success\n");
	}
	else
	{
		printf("NRF MCU connect error\n");
	}

#ifdef __MASTER
	NRF_TX_Mode();
#else
	NRF_RX_Mode();
#endif

	uint64_t i = 0;
	uint64_t i_tx_rx = 0;

	while (1)
	{
		if (i % 10 == 0)
		{
			for (int i = 0; i < 6; i++)
			{
				float v = btn_get_value(i);
				btn_v[i] = v * 0.1f + btn_v_l[i] * 0.9f;
				btn_v_l[i] = btn_v[i];
				if (btn_v[i] > 0.5f)
				{
					btn[i] = 1;
				}
				else
				{
					btn[i] = 0;
				}
			}

			uint8_t bsum = 0;
			for (int i = 0; i < 6; i++)
			{
				bsum += btn[i];
			}
			if (bsum > 0)
			{
				led_on(1);
			}
			else
			{
				led_off(1);
			}
		}

		if (i % 1000 == 0)
		{
			led_on(0);
		}
		else if (i % 1000 == 500)
		{
			led_off(0);
		}

#ifdef __MASTER
		if (i % 50 == 0)
		{
			uint32_t btn_val = 0;
			btn_val |= btn[1];
			btn_val |= btn[0] << 8;
			btn_val |= btn[4] << 16;
			btn_val |= btn[5] << 24;
			//printf("%d %d %d %d %d %d %x\r\n", btn[0], btn[1], btn[2], btn[3], btn[4], btn[5], (unsigned int)btn_val);
			nrf_send_buff(&btn_val, 1);

			if (i_tx_rx++ % 10 == 0)
			{
				led_on(2);
			}
			if (i_tx_rx++ % 10 == 5)
			{
				led_off(2);
			}
		}
#else

//		if (btn[5])
//		{
//			mode = 0;
//			led_off(3);
//		}
//		if (btn[4])
//		{
//			mode = 1;
//			led_on(3);
//		}
//
//		if (mode == 1)
//		{
//			if (btn[3] && !btn[2])
//			{
//				pwm[0] = PWM_MAX_VALUE;
//				pwm[1] = PWM_MAX_VALUE;
//			}
//			else if (!btn[3] && btn[2])
//			{
//				pwm[0] = PWM_MIN_VALUE;
//				pwm[1] = PWM_MIN_VALUE;
//			}
//			else if (!btn[3] && !btn[2])
//			{
//				pwm[0] = PWM_MID;
//				pwm[1] = PWM_MID;
//			}
//		}

		if (mode == 0)
		{
			int recv_size = nrf_buff_get(nrf_rx);
			if (recv_size > 0)
			{
				if (i_tx_rx++ % 10 == 0)
				{
					led_on(2);
				}
				if (i_tx_rx++ % 10 == 5)
				{
					led_off(2);
				}

				uint8_t btn0 = nrf_rx[0] & 0xf;
				uint8_t btn1 = (nrf_rx[0] >> 8) & 0xf;
				uint8_t btn2 = (nrf_rx[0] >> 16) & 0xf;
				uint8_t btn3 = (nrf_rx[0] >> 24) & 0xf;

				int is_turn = 0;
				float power = 0.0f;
				float turn0 = 0.0f;
				float turn1 = 0.0f;

				if (btn1)
				{
					power = -1.0f;
				}
				if (btn0)
				{
					power = 1.0f;
				}

				if (btn2)
				{
					is_turn = 1;
					turn0 = -turn_val;
					turn1 = turn_val;
				}
				if (btn3)
				{
					is_turn = 1;
					turn0 = turn_val;
					turn1 = -turn_val;
				}
				if (btn2 + btn3 > 1)
				{
					turn0 = 0.0f;
					turn1 = 0.0f;
				}

				if (!is_turn)
				{
					pwm_p[0] = power * filter + pwm_pl[0] * (1.0f - filter);
					pwm_p[1] = power * filter + pwm_pl[1] * (1.0f - filter);
					pwm_pl[0] = pwm_p[0];
					pwm_pl[1] = pwm_p[1];
				}
				else
				{
					pwm_p[0] = turn0 * filter_t + pwm_pl[0] * (1.0f - filter_t);
					pwm_p[1] = turn1 * filter_t + pwm_pl[1] * (1.0f - filter_t);
					pwm_pl[0] = pwm_p[0];
					pwm_pl[1] = pwm_p[1];
				}

				pwm[0] = PWM_MID + PWM_SCALE * pwm_p[0];
				pwm[1] = PWM_MID + PWM_SCALE * pwm_p[1];
				pwm_set_values(pwm);
				//printf("%x %d %d\r\n", (unsigned int) nrf_rx[0], pwm[0], pwm[1]);
			}
		}
#endif

		i++;
		sysclk_delay_ms(1);
	}

	return 0;
}

