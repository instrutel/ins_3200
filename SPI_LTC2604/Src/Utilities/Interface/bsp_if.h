/*
 * bsp_if.h
 *
 *  Created on: Mar 19, 2025
 *      Author: Admin
 */

#ifndef UTILITIES_INTERFACE_BSP_IF_H_
#define UTILITIES_INTERFACE_BSP_IF_H_

#include <stdint.h>

uint8_t BSP_IfInit(void);
uint8_t BSP_IfRead(uint8_t cmd, uint8_t* pBuff , uint16_t len);
uint8_t BSP_IfWrite(uint8_t cmd, uint8_t* pBuff, uint16_t size);

#endif /* UTILITIES_INTERFACE_BSP_IF_H_ */
