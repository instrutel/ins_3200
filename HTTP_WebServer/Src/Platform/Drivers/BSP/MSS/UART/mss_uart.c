/*
 * hal_uart.c
 *
 *  Created on: Feb 9, 2025
 *      Author: Admin
 */

#include "../MSS/UART/mss_uart.h"
#include <string.h>

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MSS_UART_Init(UART_HandleTypeDef *huart,USART_TypeDef *uart,uint32_t br)
{
  /* USER CODE END USART_Init 1 */
  huart->Instance = uart;
  huart->Init.BaudRate = br;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
//    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/*
 *
 */
uint8_t MSS_UART_Transmit(UART_HandleTypeDef *huart,uint8_t *data)
{
	uint16_t Len = strlen((const char*)data);
	return HAL_UART_Transmit(huart, data, Len, 0xfffff);
}

void MSS_UART_Transmit_char(UART_HandleTypeDef *huart,uint8_t *data)
{
	HAL_UART_Transmit(huart, data, 1, 0xff);
}
/*
 *
 */
uint8_t MSS_UART_Receive(UART_HandleTypeDef *huart,uint8_t *pData)
{
	if( HAL_UART_Receive(huart, pData, 0x01, 0xff) == HAL_OK )
	{
		return 1;
	}else { return 0;  }
}
