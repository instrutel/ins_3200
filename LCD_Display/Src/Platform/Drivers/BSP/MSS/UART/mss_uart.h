/*
 * hal_uart.h
 *
 *  Created on: Feb 9, 2025
 *      Author: Admin
 */

#ifndef PLATFORM_DRIVERS_BSP_MSS_UART_MSS_UART_H_
#define PLATFORM_DRIVERS_BSP_MSS_UART_MSS_UART_H_

#include<stdint.h>
#include "stm32f4xx_hal.h"

void MSS_UART_Init(UART_HandleTypeDef *huart,USART_TypeDef *uart,uint32_t br);
uint8_t MSS_UART_Transmit(UART_HandleTypeDef *huart, uint8_t*data,uint16_t Len);
uint8_t MSS_UART_TransmitString(UART_HandleTypeDef *huart,uint8_t *data);
uint8_t MSS_UART_Receive(UART_HandleTypeDef *huart,uint8_t*pData);

#endif /* PLATFORM_DRIVERS_BSP_MSS_UART_MSS_UART_H_ */
