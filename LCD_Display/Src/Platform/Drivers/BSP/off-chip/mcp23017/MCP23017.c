/**
  ******************************************************************************
  * @file    MCP23017.c
  * @author  Application Team
  * @brief   This file provides a set of functions needed to manage the MCP23017
  *          IO Expender devices.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/


#include "MCP23017.h"
#include "stm32f4xx_hal.h"


/* Private macro -------------------------------------------------------------*/
	//##############      MCP DRIVER MACROS  #################//

#define  MCP23017_ADDRESS            	(0x20)  //--  MCP Default Address
#define  MCP_I2C_CLOCKSPEED            (100000)

#define   IS_MCP23017_ADDRESS(ADDR)   ((ADDR >= 0x40) && (ADDR <= 0x4E))
#define   IS_MCP23017_PORT(PORT)       ((PORT == MCP_PORT_A) || (PORT == MCP_PORT_B))

#define  bitSet(value, bit)     ((value) |= (1UL << (bit)))
#define  bitClear(value, bit)   ((value) &= ~(1UL << (bit)))
#define  bitWrite(value, bit, bitvalue)   (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define  bitRead(value, bit)   (((value) >> (bit)) & 0x01)


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static HAL_StatusTypeDef MCP23017_PortDefaultConfig(I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,MCPPorts_e Mcp_Port);
static HAL_StatusTypeDef MCP23017_RegisterWrite(I2C_HandleTypeDef *hi2c, uint8_t SlaveAddr,uint8_t RegAddr,uint8_t RegValue);
static HAL_StatusTypeDef MCP23017_RegisterRead (I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *pValue);

/** @defgroup LAN8742_Private_Functions LAN8742 Private Functions
  * @{
  */

/* @Brief : MCP 23017 IO Expender Card Initialization
 * @param : I2C_HandleTypeDef pointer
 * @retval: Device Initialization status
 */
uint8_t MCP23017_Init(I2C_HandleTypeDef *hi2c,I2C_TypeDef *i2c)
{
	uint8_t ret = HAL_ERROR;

	/* USER CODE END I2C1_Init 1 */
	hi2c->Instance = i2c;
	hi2c->Init.ClockSpeed = MCP_I2C_CLOCKSPEED; /* clock speed tested at 100khz */
	hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c->Init.OwnAddress1 = 0;
	hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c->Init.OwnAddress2 = 0;
	hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	/* @@ I2c Init @@  */
	ret = HAL_I2C_Init(hi2c);
	return ret;
}

/*
 *
 */
void MCP23017_DeInit(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_DeInit(hi2c);
	hi2c = NULL;
}

void MCP23017_Rest(GPIO_TypeDef *GPIOPort, uint32_t GPIO_PIN)
{
	HAL_GPIO_WritePin((GPIO_TypeDef *)GPIOPort,GPIO_PIN,1);
	HAL_Delay(100);
	HAL_GPIO_WritePin((GPIO_TypeDef *)GPIOPort,GPIO_PIN,0);
	HAL_Delay(100);
	HAL_GPIO_WritePin((GPIO_TypeDef *)GPIOPort,GPIO_PIN,1);
	HAL_Delay(100);
}

/*
 *
 */
uint8_t MCP23017_PortConfig(I2C_HandleTypeDef *hi2c,uint8_t salveAddr,MCPPorts_e port,MCP2307PortConfig_t *config)
{
	uint8_t ret= 0; uint8_t pValue=0;

	if( MCP23017_RegisterRead(hi2c, salveAddr, IODIRA, &pValue) != HAL_OK )
	{
		return HAL_ERROR;
	}
	switch( port )
	{
		case MCP_PORT_A:
				MCP23017_RegisterWrite(hi2c,salveAddr,IODIRA,config->IoDir);
				MCP23017_RegisterWrite(hi2c,salveAddr,IPOLA,config->IPol);
				MCP23017_RegisterWrite(hi2c,salveAddr,GPPUA,config->GPPulUp);
				MCP23017_RegisterWrite(hi2c,salveAddr,IOCONA,config->IntCon);
				MCP23017_RegisterWrite(hi2c,salveAddr,GPINTENA,config->GPInteEn);
			break;
		case MCP_PORT_B:
				MCP23017_RegisterWrite(hi2c,salveAddr,IODIRB,config->IoDir);
				MCP23017_RegisterWrite(hi2c,salveAddr,IPOLB,config->IPol);
				MCP23017_RegisterWrite(hi2c,salveAddr,GPPUB,config->GPPulUp);
				MCP23017_RegisterWrite(hi2c,salveAddr,IOCONB,config->IntCon);
				MCP23017_RegisterWrite(hi2c,salveAddr,GPINTENB,config->GPInteEn);
			break;
	}
	return 0;

}

/*
 *
 */
uint8_t MCP23017_PortPinRead(I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,MCPPorts_e port,uint8_t pin, uint8_t* pValue)
{
	HAL_StatusTypeDef Status_l = HAL_ERROR;
	uint8_t  REG_ADDR = 0x00; uint8_t  Port_Data = 0x00;
	REG_ADDR = (port == MCP_PORT_A)?GPIO_A:GPIO_B;
	/* Port A/B register Value */
	Status_l = MCP23017_RegisterRead(hi2c,SlaveAddr,REG_ADDR,&Port_Data);
	*pValue = bitRead(Port_Data,pin);
	return Status_l;
}

/*
 *
 */
uint8_t MCP23017_PortPinWrite(I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,MCPPorts_e port,uint8_t Pin, uint8_t pValue)
{
	HAL_StatusTypeDef Status_l = HAL_ERROR;
	uint8_t  REG_ADDR = 0x00; uint8_t  Port_Data = 0x00;
	REG_ADDR = (port == MCP_PORT_A)?GPIO_A:GPIO_B;

	/* Port A/B register Value */
	Status_l = MCP23017_RegisterRead(hi2c,SlaveAddr,REG_ADDR,&Port_Data);
	if( Status_l != HAL_OK )
	{
		return Status_l;
	}
	bitWrite(Port_Data,Pin,pValue);
	/* Port register Value */
	return MCP23017_RegisterWrite(hi2c,SlaveAddr,REG_ADDR,Port_Data);
}

/*
 *
 */
uint8_t MCP23017_PortRegWrite(I2C_HandleTypeDef *hi2c,uint8_t SalveAddr,MCPPorts_e port,uint8_t pValue)
{
	uint8_t RegAddr = (port == MCP_PORT_A)?GPIO_A:GPIO_B;
	/* Port register Value */
	return MCP23017_RegisterWrite(hi2c,SalveAddr,RegAddr,pValue);
}

/*
 *
 */
uint8_t MCP23017_PortRegRead(I2C_HandleTypeDef *hi2c,uint8_t SalveAddr,MCPPorts_e port,uint8_t* pdata)
{
	/* Port A/B register Value */
	uint8_t RegAddr = (port == MCP_PORT_A)?GPIO_A:GPIO_B;
	return MCP23017_RegisterRead(hi2c,SalveAddr,RegAddr,pdata);
}


/**************************************************************************************************************/

/* @
 *
 */
static HAL_StatusTypeDef MCP23017_PortDefaultConfig(I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,MCPPorts_e Port)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	if( hi2c != NULL )
	{
		SlaveAddr = ( MCP23017_ADDRESS | SlaveAddr ) << 1;
		assert_param(IS_MCP23017_ADDRESS(SlaveAddr));  	 	/* ADDRESS CHECKING */
		uint8_t pLoop = (Port == MCP_PORT_A)?0:1;

		for( ; pLoop <= 21 ; pLoop += 2 )
		{
			ret = HAL_I2C_Mem_Write(hi2c, SlaveAddr, pLoop, I2C_MEMADD_SIZE_8BIT, 0x00, 0x01, 0xffffff);//- Default IOEXPCARD PORT Configuration
			if( ret != HAL_OK )
			{
				break;
			}
		}
	}
	return ret;
}

static HAL_StatusTypeDef MCP23017_RegisterWrite(I2C_HandleTypeDef *hi2c, uint8_t SlaveAddr, uint8_t RegAddr,uint8_t RegValue)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	if( hi2c != NULL )
	{
		SlaveAddr = ( MCP23017_ADDRESS | SlaveAddr ) << 1;
		ret = HAL_I2C_Mem_Write(hi2c, SlaveAddr, RegAddr, I2C_MEMADD_SIZE_8BIT, &RegValue, 0x01, 0xffffff);//- Default IOEXPCARD PORT Configuration
	}
	return ret;
}

static HAL_StatusTypeDef MCP23017_RegisterRead (I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr, uint8_t RegAddr,uint8_t *pValue)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	if( hi2c != NULL )
	{
		SlaveAddr = ( MCP23017_ADDRESS | SlaveAddr ) << 1;
		ret = HAL_I2C_Mem_Read(hi2c, SlaveAddr, RegAddr, I2C_MEMADD_SIZE_8BIT, pValue, 0x01, 0xffffff);//- Default IOEXPCARD PORT Configuration
	}
	return ret;
}
