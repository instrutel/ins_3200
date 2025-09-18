/**
  ******************************************************************************
  * @file    bsp_if.c
  * @author  Application Team
  * @brief   This file provides a set of functions needed to performs
  * 		 hardware-level initialization for external/off-chip.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 Instrutel systems.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "../Interface/bsp_if.h"
#include "stm32f4xx_hal.h"
#include "ltc1867.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/** @defgroup off-chip Device Common Functions
  * @{
  */

/**
  * @brief  Initialize off-chip device interface.
  * @details This function performs hardware-level initialization for external/off-chip
  *          interfaces such as SPI, I2C, or GPIO required to communicate with external
  *          peripherals. It may configure pins, clocks, and peripheral drivers.
  * @retval uint8_t Returns 0 on success, or non-zero error code on failure.
  * @note   This function must be called before any off-chip device communication occurs.
  */
uint8_t BSP_IfInit(void)
{
	return LTC1867_Init(&hspi1, SPI1);
}



/**
  * @brief  Read data from an off-chip device.
  * @details This function communicates with an external (off-chip) peripheral using a specified command,
  *          and reads the response data into a provided buffer. The communication may occur over I2C, SPI, or
  *          any other interface depending on the board configuration.
  * @param  cmd   The command or register address to send to the off-chip device before reading.
  * @param  pBuff Pointer to the buffer where the received data will be stored.
  * @param  len   The number of bytes to read from the off-chip device.
  * @retval uint8_t Returns 0 if the read operation is successful, or a non-zero value indicating an error.
  * @note   Ensure that the communication interface (e.g., I2C or SPI) is initialized before calling this function.
  */
uint8_t BSP_IfRead(uint8_t cmd, uint8_t* pBuff , uint16_t len)
{
	uint8_t chnlNum = 0; uint16_t adccount = 0;
	for( ; chnlNum < 8 ; ++chnlNum)
	{
		LTC1867_ChannelContinuousRead(&hspi1, ADC_CHANNEL_SINGLE_ENDED, chnlNum, &adccount);
		pBuff[chnlNum*2] = (uint8_t)(adccount >> 8);
		pBuff[(chnlNum*2)+1] = (uint8_t)(adccount & 0xff);
	}
	return 0;
}

/**
  * @brief  Write data to an off-chip device.
  * @details This function sends a command followed by a block of data to an external (off-chip)
  *          peripheral over the board's communication interface (e.g., I2C, SPI).
  * @param  cmd    Command or register address to send before data (device-specific).
  * @param  pBuff  Pointer to the data buffer containing the payload to write.
  * @param  size   Number of bytes to send from the buffer.
  * @retval uint8_t Returns 0 if the write operation is successful, or a non-zero error code otherwise.
  * @note   The communication interface (e.g., I2C or SPI) must be properly initialized before calling this function.
  */
uint8_t BSP_IfWrite(uint8_t cmd, uint8_t* pBuff, uint16_t size)
{
	UNUSED(cmd);
	UNUSED(pBuff);
	UNUSED(size);

	return 0;
}

