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

#include<string.h>
#include "uart_helper.h"
#include "../MSS/UART/mss_uart.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
enum
{
	false,
	true,
};
/* Private macro -------------------------------------------------------------*/
#define UART_HELPER_MAX_GETLINE  100

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The debug port initialization.
  * @param  USART_TypeDef *
  * @retval none
  */
void Uart_PortInit(USART_TypeDef *uart)
{
	MSS_UART_Init(&huart, uart, 115200);
}

/**
  * @brief  The debug port initialization.
  * @param  USART_TypeDef *
  * @retval none
  */
uint8_t Uart_PutString(uint8_t *str)
{
   return  MSS_UART_TransmitString(&huart, (uint8_t *)str);
}

uint8_t Uart_PutChar(uint8_t ch)
{
	return MSS_UART_Transmit(&huart, &ch, 1u);
}

uint16_t Uart_GetLine(uint8_t *pBuffer, uint16_t Len)
{
	uint16_t result = 0; _Bool finished = false;
    static char myBuffer[UART_HELPER_MAX_GETLINE]; // static to be stack friendly
    const size_t bufferLen = UART_HELPER_MAX_GETLINE;

    memset(myBuffer, 0, UART_HELPER_MAX_GETLINE);

    uint8_t cBuf[1];
    while (!finished) {
        while (0 == MSS_UART_Receive(&huart, cBuf));

        switch (cBuf[0]) {
        case '\r':
        	MSS_UART_Transmit(&huart, cBuf, 1u);
            finished = true;
            break;

        case '\n':
        	MSS_UART_Transmit(&huart, cBuf, 1u);
            finished = true;
            break;

        case 0x7Fu: // delete
            if (result) {
                result--;
                MSS_UART_Transmit(&huart, (uint8_t *)"\033[D \033[D", 7u);
                myBuffer[result] = 0;
            }
            break;

        case 0x08u: // backspace - ^H
            if (result) {
                result--;
                MSS_UART_Transmit(&huart, (uint8_t *)" \033[D", 4u);
                myBuffer[result] = 0;
            }
            break;

        case 0x03u: // intr - ^C
            result = -1;
            myBuffer[0] = 0;
            finished = true;
            break;

        case 0x1Bu: // ESC
            result = -1;
            myBuffer[0] = 0;
            finished = true;
            break;

        case 0x04u: // ^D
            if (result == 0) {
                result = -1;
                myBuffer[0] = 0;
                finished = true;
            }
            break;

        default:
            if (result < bufferLen) {
            	MSS_UART_Transmit(&huart, cBuf, 1u);
                myBuffer[result] = cBuf[0];
                result++;
            }
            break;
        }
    }

    const char crlf[] = "\n";
    MSS_UART_TransmitString(&huart, (uint8_t *)crlf);

    if (result > 0) {
        *pBuffer = myBuffer;
//        *pBufLen = (size_t)result;
    } else {
        *pBuffer = NULL;
    }

    return result;
}

uint8_t Uart_GetChar(uint8_t *ch)
{
	return MSS_UART_Receive(&huart, ch);
}
