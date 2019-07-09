/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出:
 *  - 用于向标准设备显示字符内容
 */

#ifndef _PRINTF_C_
#define _PRINTF_C_

#include <typedef.h>
#include <printf.h>
#include <uart.h>

typedef union
{
	float f;
	unsigned int u;
} u32f;

/*
 * putch : 显示一个字符
 *  - char ch : 待显示字符
 * return : void
 */
void putch(char ch)
{
	uart_send(ch);
	//__asm__ volatile ("int $0x82" :: "a"(params));
}

/*
 * putchar : 显示一个字符，只显示可显字符
 *  - char ch : 待显示字符
 * return : void
 */
//void putchar(char ch)
//{
//	if ((ch >= 0x20 && ch <= 0x7e) || ch == '\n' || ch == '\t')
//	{
//		putch(ch);
//	}
//}
/*
 * puts : 显示字符串
 *  - char *str : 字符串
 * return : void
 */
int puts(char *str)
{
	int count = 0;
	int params[3];
	params[0] = 1;
	params[1] = (int) str;
	params[2] = (int) &count;
	while (*str != '\0')
	{
		putch(*str++);
	}
	return count;
}

/*
 * number_to_str : 将整数转为字符串
 *  - char *buff : 数据地址
 *  - int number : 整数
 *  - int hex : 10进制或16进制
 * return : void
 */
void number_to_str(char *buff, int number, int hex)
{
	char temp[0x200];
	char num[0x20] = "0123456789ABCDEFG";

	int i = 0;
	int length = 0;
	int rem;
	char sign = '+';

	temp[i++] = '\0';
	if (number < 0)
	{
		sign = '-';
		number = 0 - number;
	}
	else if (number == 0)
	{
		temp[i++] = '0';
	}

	while (number > 0)
	{
		rem = number % hex;
		temp[i++] = num[rem];
		number = number / hex;
	}
	if (sign == '-')
	{
		temp[i++] = sign;
	}
	length = i;

	for (i = length - 1; i >= 0; i--)
	{
		*buff++ = temp[i];
	}
}

/*
 * printf_lf_e : 以科学计数法显示浮点数
 *  - int tty_id : tty编号
 *  - int float_or_double : 单双精度
 *  - double val : 浮点数值
 *  - int e : 指数
 *  - int sign : 正负号
 * return : void
 */
void printf_lf_e(double val, int e, int sign)
{
	int lf_size = 4;
	char buff[0x200];
	int i = 0, start = 0;
	int length = 2 + lf_size;
	if (sign)
	{
		putch('-');
	}
	for (; i < length; ++i)
	{
		if (i == 1)
		{
			buff[i] = '.';
		}
		else
		{
			int n = (int) val;
			buff[i] = n + '0';
			val -= n;
			val *= 10.0;
		}
	}

	buff[i++] = 'e';
	int sign_e = 0;
	if (e >= 0)
	{
		buff[i++] = '+';
	}
	else
	{
		buff[i++] = '-';
		e = -e;
	}
	buff[i] = '\0';
	printf("%s", buff);
	printf("%d", e);
}

/*
 * printf_lf : 以小数形式显示浮点数
 *  - int tty_id : tty编号
 *  - int float_or_double : 单双精度
 *  - double val : 浮点数值
 *  - int e : 指数
 *  - int sign : 正负号
 * return : void
 */
void printf_lf(double val, int e, int sign)
{
	int lf_size = 4;
	char buff[0x200];
	if (sign)
	{
		putch('-');
	}
	int i = 0;
	if (e >= 0)
	{
		int length = e + 2 + lf_size;
		for (; i < length; ++i)
		{
			if (i == e + 1)
			{
				buff[i] = '.';
			}
			else
			{
				int n = (int) val;
				buff[i] = n + '0';
				val -= n;
				val *= 10.0;
			}
		}
	}
	else
	{
		e = -e;
		int length = e + 2 + lf_size;
		for (; i < length; ++i)
		{
			if (i == 0)
			{
				buff[i] = '0';
			}
			else if (i == 1)
			{
				buff[i] = '.';
			}
			else if (i < e + 1)
			{
				buff[i] = '0';
			}
			else
			{
				int n = (int) val;
				buff[i] = n + '0';
				val -= n;
				val *= 10;
			}
		}
	}
	buff[i] = '\0';
	printf("%s", buff);
}

/*
 * printf_vf : 显示单精度浮点数
 *  - int tty_id : tty编号
 *  - float val : 浮点数值
 *  - int with_e : 是否使用科学计数法
 * return : void
 */
void printf_vf(float val, int with_e)
{
	int sign = 0;
	u32f uf;
	uf.f = val;

	if (uf.f < 0)
	{
		uf.f = -uf.f;
		sign = 1;
	}
	int e2 = (uf.u >> 23 & 0xff);
	u32 fp = uf.u & 0x7FFFFF;
	if (e2 == 0 && fp == 0)
	{
		printf("0.0");
		return;
	}
	else if (e2 == 0 && fp != 0)
	{
		e2 -= 126;
	}
	else
	{
		e2 -= 127;
	}
	int e10 = log10(pow(2, e2));
	float val_f = uf.f / pow(10, e10);
	int val_fs = (int) val_f;
	if (val_fs >= 10)
	{
		val_f /= 10.0;
		e10++;
	}
	else if (val_fs < 1)
	{
		val_f *= 10.0;
		e10--;
	}

	if (with_e)
	{
		printf_lf_e(val_f, e10, sign);
	}
	else
	{
		printf_lf(val_f, e10, sign);
	}
}

/*
 * printf : 标准设备显示函数
 *  - char *fmt : 显示格式
 *  - ... : 动态参数
 * return : int显示字符数
 */
int printf(char *fmt, ...)
{
	//显示数字缓冲区
	char buff[0x200];
	//显示字符串指针
	char *str;
	//显示字符变量
	char ch;
	//显示字符总数
	int count = 0;

	//动态参数
	va_list args;
	//初始化动态参数
	va_start(args, fmt);

	//读到\0为结束
	while (*fmt != '\0')
	{
		//格式化标记%
		if (*fmt == '%')
		{
			//显示一个字符
			if ('c' == *(fmt + 1))
			{
				ch = va_arg(args, int);
				putch(ch);
				count++;
				fmt += 2;
			}
			//显示字符串
			else if ('s' == *(fmt + 1))
			{
				str = va_arg(args, char*);
				count += puts(str);
				fmt += 2;
			}
			//显示8进制整数
			else if ('o' == *(fmt + 1))
			{
				number_to_str(buff, va_arg(args, int), 8);
				count += puts(buff);
				fmt += 2;
			}
			//显示10进制整数
			else if ('d' == *(fmt + 1))
			{
				number_to_str(buff, va_arg(args, int), 10);
				count += puts(buff);
				fmt += 2;
			}
			//显示无符号16进制整数
			else if ('x' == *(fmt + 1))
			{
				u32 num = va_arg(args, u32);
				u32 nl = num & 0xffff;
				u32 nh = (num >> 16) & 0xffff;
				count += puts("0x");
				if (nh == 0)
				{
					number_to_str(buff, nl, 16);
					count += puts(buff);
				}
				else
				{
					number_to_str(buff, nh, 16);
					count += puts(buff);

					number_to_str(buff, nl, 16);
					int zero = 4 - strlen(buff);
					for (int i = 0; i < zero; i++)
					{
						putch('0');
					}
					count += puts(buff);
				}
				fmt += 2;
			}
			else if ('f' == *(fmt + 1))
			{
				float val = va_arg(args, double);
				printf_vf(val, 0);
				fmt += 2;
			}
			else if ('e' == *(fmt + 1))
			{
				float val = va_arg(args, double);
				printf_vf(val, 1);
				fmt += 2;
			}
		}
		//显示普通字符
		else
		{
			putch(*fmt++);
			count++;
		}
	}

	va_end(args);

	return count;
}
#endif
