/*
 * uart_helper.h
 *
 *  Created on: Mar 16, 2025
 *      Author: Admin
 */

#ifndef UTILITIES_MISC_UART_HELPER_H_
#define UTILITIES_MISC_UART_HELPER_H_

/**
  ******************************************************************************
  * @file           : DebugLog.c
  * @brief          : Print debug messages
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"


void Uart_PortInit(USART_TypeDef *uart);
uint8_t  Uart_PutString(uint8_t *str);
uint8_t  Uart_PutChar(uint8_t ch);
uint16_t Uart_GetLine(uint8_t *pBuff, uint16_t Len);
uint8_t  Uart_GetChar(uint8_t *ch);





#endif /* UTILITIES_MISC_UART_HELPER_H_ */
