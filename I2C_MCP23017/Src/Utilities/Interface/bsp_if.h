/**
  ******************************************************************************
  * @file    bsp_if.h
  * @author  Application Team
  * @brief   This file provides a set of functions needed to manage the MCP23017
  *          IO Expender devices.
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


#ifndef UTILITIES_INTERFACE_BSP_IF_H_
#define UTILITIES_INTERFACE_BSP_IF_H_

#include <stdint.h>

typedef enum
{
	MCP_GPIO_PIN0,
	MCP_GPIO_PIN1,
	MCP_GPIO_PIN2,
	MCP_GPIO_PIN3,
	MCP_GPIO_PIN4,
	MCP_GPIO_PIN5,
	MCP_GPIO_PIN6,
	MCP_GPIO_PIN7,
}MCPPortPins_e;


uint8_t BSP_IfInit(void);
uint8_t BSP_IfRead(uint8_t cmd, uint8_t* pBuff , uint16_t len);
uint8_t BSP_IfWrite(uint8_t cmd, uint8_t* pBuff, uint16_t size);

#endif /* UTILITIES_INTERFACE_BSP_IF_H_ */
