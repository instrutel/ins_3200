/*
 * common.h
 *
 *  Created on: Mar 14, 2025
 *      Author: Admin
 */

#ifndef UTILITIES_DEVICES_COMMON_H_
#define UTILITIES_DEVICES_COMMON_H_

#include <stdint.h>

uint8_t COM_DeviceInit(void);
uint8_t COM_DeviceRead(uint8_t cmd, uint8_t* pBuff , uint16_t len);
uint8_t COMDeviceWrite(uint8_t cmd, uint8_t* pBuff, uint16_t size);


#endif /* UTILITIES_DEVICES_COMMON_H_ */
