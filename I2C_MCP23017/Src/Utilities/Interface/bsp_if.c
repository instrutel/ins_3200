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
//#include "stm32f4xx_hal.h"
#include "MCP23017.h"
#include "../MSS/GPIO/mss_gpio.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#define  MCP_RESET_PORT          GPIOE
#define  MCP_RESET_LINE          GPIO_PIN_0
#define  MCP_CARD_1_ADDR		 0x07
#define  MCP_CARD_2_ADDR		 0x00

/** @defgroup offchip Device Common Functions
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
	uint8_t ret = 0;
	/* -- MCP Port Init -- */
	MCP23017_Init(&hi2c1, I2C1);

	/* -- MCP Reset Pin Configuration -- */
	MSS_GPIO_ConfigPin(MCP_RESET_PORT,MCP_RESET_LINE,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MCP23017_Rest(MCP_RESET_PORT,MCP_RESET_LINE);

	/* MCP Mode Configurations */
	MCP2307PortConfig_t IOConfig = {
			.IoDir = 0x00, .IPol = 0xff,
			.GPPulUp = 0x00, .IntCon = 0x00 ,
			.GPInteEn = 0x00 };

	IOConfig.IoDir = 0xff;    /* GPIO Input Configuration */
	IOConfig.GPInteEn = 0x00; /* Disable interrupt for all GPIOs */
	ret = MCP23017_PortConfig(&hi2c1,MCP_CARD_1_ADDR, MCP_PORT_A, &IOConfig);
	ret = MCP23017_PortConfig(&hi2c1,MCP_CARD_1_ADDR, MCP_PORT_B, &IOConfig);

	IOConfig.IoDir = 0x00;    /* GPIO Output Configuration */
	IOConfig.GPInteEn = 0x00;  IOConfig.IPol = 0x00;
	ret = MCP23017_PortConfig(&hi2c1,MCP_CARD_2_ADDR, MCP_PORT_A, &IOConfig);
	ret = MCP23017_PortConfig(&hi2c1,MCP_CARD_2_ADDR, MCP_PORT_B, &IOConfig);
	return ret;
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
#if 0
	MCPPorts_e port = (MCPPorts_e) pBuff[0];
	uint8_t pin = pBuff[1];

	switch(cmd)
	{
		case 1: /* GPIO Pin Read */
			ret = MCP23017_PortPinRead(&hi2c1,MCP_SLAVE_ADDR, port,pin,&pBuff[0]);
			break;
		case 2: /* GPIO Port A/B read */
			ret = MCP23017_PortRegRead(&hi2c1,MCP_SLAVE_ADDR, port,&pBuff[0]);
			break;
		case 3:/* GPIO Port A & B read */
			ret = MCP23017_PortRegRead(&hi2c1,MCP_SLAVE_ADDR, MCP_PORT_A,&pBuff[0]);
			ret = MCP23017_PortRegRead(&hi2c1,MCP_SLAVE_ADDR, MCP_PORT_B,&pBuff[1]);
			break;
	}
#endif
	uint8_t ret = 0;
	ret = MCP23017_PortRegRead(&hi2c1,MCP_CARD_1_ADDR, MCP_PORT_A,&pBuff[0]);
	ret = MCP23017_PortRegRead(&hi2c1,MCP_CARD_1_ADDR, MCP_PORT_B,&pBuff[1]);

	ret = MCP23017_PortRegRead(&hi2c1,MCP_CARD_2_ADDR, MCP_PORT_A,&pBuff[2]);
	ret = MCP23017_PortRegRead(&hi2c1,MCP_CARD_2_ADDR, MCP_PORT_B,&pBuff[3]);

	return ret;
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
#if 0
	MCPPorts_e port = (MCPPorts_e) pBuff[0];
	uint8_t pin = pBuff[2]; uint8_t Value = pBuff[1];
	uint8_t ret = 0;
	switch(cmd)
	{
		case 1: /* GPIO Pin Read */
			ret = MCP23017_PortPinWrite(&hi2c1,MCP_SLAVE_ADDR, port,pin,Value);
			break;
		case 2: /* GPIO Port A/B read */
			ret = MCP23017_PortRegWrite(&hi2c1,MCP_SLAVE_ADDR, port,Value);
			break;
		case 3:/* GPIO Port A & B read */
			ret = MCP23017_PortRegRead(&hi2c1,MCP_SLAVE_ADDR, MCP_PORT_A,&pBuff[0]);
			ret = MCP23017_PortRegRead(&hi2c1,MCP_SLAVE_ADDR, MCP_PORT_B,&pBuff[1]);
			break;
	}
#endif
	uint8_t ret = 0;
	uint8_t Value = pBuff[0];

	MCP23017_PortRegWrite(&hi2c1, MCP_CARD_2_ADDR,cmd,Value);
	return ret;
}
