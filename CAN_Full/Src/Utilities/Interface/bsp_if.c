/**
  ******************************************************************************
  * @file    bsp_if.c
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

#include "bsp_if.h"
#include<string.h>
#include "stm32f4xx_hal.h"

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void(*PacketHandler)(uint8_t* pBuff , uint16_t len );

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
uint8_t BSP_IfInit(void(*func)(uint8_t* pBuff , uint16_t len ))
{
	CAN_FilterTypeDef  sFilterConfig;

    /*##-1- Configure the CAN peripheral #######################################*/
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 6;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if ( HAL_CAN_Init(&hcan1) != HAL_OK )
	{
//		Error_Handler();
	}
	 /*##-2- Configure the CAN Filter ###########################################*/
	  sFilterConfig.FilterBank = 0;
	  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	  sFilterConfig.FilterIdHigh = 0x0000;
	  sFilterConfig.FilterIdLow = 0x0000;
	  sFilterConfig.FilterMaskIdHigh = 0x0000;
	  sFilterConfig.FilterMaskIdLow = 0x0000;
	  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	  sFilterConfig.FilterActivation = ENABLE;
	  sFilterConfig.SlaveStartFilterBank = 14;

	  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	  {
	    /* Filter configuration Error */
	    Error_Handler();
	  }

	  /*##-3- Start the CAN peripheral ###########################################*/
	  if (HAL_CAN_Start(&hcan1) != HAL_OK)
	  {
	    /* Start Error */
	    Error_Handler();
	  }

	  /*##-4- Activate CAN RX notification #######################################*/
	  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	  {
	    /* Notification Error */
	    Error_Handler();
	  }
	  PacketHandler = func;
	  return HAL_OK; /* Test Passed */
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
	uint8_t Len = size % 8;
	/*##-5- Configure Transmission process #####################################*/
	TxHeader.StdId = 0x121;
	TxHeader.ExtId = 0x121;

	if( cmd == 1 )
		TxHeader.IDE = CAN_ID_STD;
	else TxHeader.IDE = CAN_ID_EXT;

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = Len;
	TxHeader.TransmitGlobalTime = DISABLE;

	/* Set the data to be transmitted */
	memcpy(TxData,pBuff,Len);

	/* Start the Transmission process */
	if( HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK )
	{
	    /* Transmission request Error */
		return 1;
	}
	return 0;
}

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }

  /* Print received data  */
  if ((RxHeader.StdId == 0x321) && (RxHeader.IDE == CAN_ID_STD))
  {
	  if(PacketHandler)
		  PacketHandler(RxData,RxHeader.DLC);
  }else if((RxHeader.StdId == 0x123) && (RxHeader.IDE == CAN_ID_STD))
  {
	  if(PacketHandler)
		  PacketHandler(RxData,RxHeader.DLC);
  }
}
