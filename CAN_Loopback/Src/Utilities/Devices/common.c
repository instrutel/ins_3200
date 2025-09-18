/**
  ******************************************************************************
  * @file    common.c
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

#include "common.h"
#include "stm32f4xx_hal.h"
//#include "ltc1867.h"

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/** @defgroup offchip Device Common Functions
  * @{
  */

/*
 *
 */
uint8_t COM_DeviceInit(void)
{
	CAN_FilterTypeDef  sFilterConfig;

    /*##-1- Configure the CAN peripheral #######################################*/
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 6;
	hcan1.Init.Mode = CAN_MODE_LOOPBACK;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if ( HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		return HAL_ERROR;
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
		  return HAL_ERROR;
	  }

	  /*##-3- Start the CAN peripheral ###########################################*/
	  if (HAL_CAN_Start(&hcan1) != HAL_OK)
	  {
	    /* Start Error */
		  return HAL_ERROR;
	  }

	  /*##-4- Start the Transmission process #####################################*/
	  TxHeader.StdId = 0x11;
	  TxHeader.RTR = CAN_RTR_DATA;
	  TxHeader.IDE = CAN_ID_STD;
	  TxHeader.DLC = 2;
	  TxHeader.TransmitGlobalTime = DISABLE;
	  TxData[0] = 0xCA;
	  TxData[1] = 0xFE;

	  /* Request transmission */
	  if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	  {
	    /* Transmission request Error */
		  return HAL_ERROR;
	  }

	  /* Wait transmission complete */
	  while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) {}

	  /*##-5- Start the Reception process ########################################*/
	  if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 1)
	  {
	    /* Reception Missing */
		  return HAL_ERROR;
	  }

	  if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	  {
	    /* Reception Error */
		  return HAL_ERROR;
	  }

	  if((RxHeader.StdId != 0x11)                     ||
	     (RxHeader.RTR != CAN_RTR_DATA)               ||
	     (RxHeader.IDE != CAN_ID_STD)                 ||
	     (RxHeader.DLC != 2)                          ||
	     ((RxData[0]<<8 | RxData[1]) != 0xCAFE))
	  {
	    /* Rx message Error */
	    return HAL_ERROR;
	  }

	  return HAL_OK; /* Test Passed */
}


/*@Brief : offchip device data read
 *@param : specific commands for offchip driver if required
 *@param : received data buffer
 *@param : buffer len
 */
uint8_t COM_DeviceRead(uint8_t cmd, uint8_t* pBuff , uint16_t len)
{

}

uint8_t COMDeviceWrite(uint8_t cmd, uint8_t* pBuff, uint16_t size)
{

}

