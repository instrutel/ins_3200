/**
  ******************************************************************************
  * @file    bsp_if.h
  * @author  Application Team
  * @brief   This file provides a set of functions needed to manage the for
  * 		 external/off-chip devices.
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

uint8_t BSP_IfInit(void);
uint8_t BSP_IfRead(uint8_t cmd, uint8_t* pBuff , uint16_t len);
uint8_t BSP_IfWrite(uint8_t cmd, uint8_t* pBuff, uint16_t size);

#endif /* UTILITIES_INTERFACE_BSP_IF_H_ */
