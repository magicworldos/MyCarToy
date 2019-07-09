/*
 * stdio.h
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#ifndef _PRINTF_H_
#define _PRINTF_H_

#include <stdarg.h>
#include <math.h>
#include <string.h>


void putch(char ch);

//void putchar(char ch);

int puts(char *str);

void backspace();

void gets(char *str);

void backspace();

void number_to_str(char *buff, int number, int hex);

void printf_lf_e(double val, int e, int sign);

void printf_lf(double val, int e, int sign);

void printf_vf(float val, int with_e);

void printf_vlf(double val, int with_e);

int printf(char *fmt, ...);

#endif /* INCLUDE_SHELL_STDIO_H_ */
