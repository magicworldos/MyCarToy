/*
 * uart.h
 *
 *  Created on: Jun 25, 2017
 *      Author: lidq
 */

#ifndef SRC_UART_H_
#define SRC_UART_H_

#include <typedef.h>

void uart_init();

uint8_t uart_send(uint8_t ch);

void uart_send_buff(uint8_t *buff, int size);

#endif /* SRC_UART_H_ */
