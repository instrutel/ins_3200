/**
  ******************************************************************************
  * @file           : Task.c
  * @author			: Instrutel application team
  * @brief          : Core Task application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Private includes ----------------------------------------------------------*/
#include<stdio.h>
#include "../MSS/UART/mss_uart.h"
#include "common.h"
#include "sw_config.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart;
static uint8_t ChannelNum = 0;
static uint32_t pTimeOut  = 0;

#define UART_RECV_BUF_SIZE  32U
static uint8_t g_uart_to_debug[UART_RECV_BUF_SIZE];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r*********************************************************************\n\r\
******* Instrutel CAN LOOP-BACK Application Example  ***********\n\r\
************************************************************************\n\r";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static uint8_t get_data_from_uart(void);
void CoreApplicationSubTask(uint8_t cmd);
void CoreApplicationInit(void);

/* Private user code ---------------------------------------------------------*/
#define PRINT_STRING(X)     HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,0); \
							MSS_UART_Transmit(&huart,(uint8_t*)X); \
							HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,1)

#define GET_STRING(X)    	MSS_UART_Receive(&huart,(uint8_t*)X)


/**
 * @brief  Perform early initialization of the board hardware.
 * @details This function initializes the basic hardware components of the board, such as peripherals clock
 *  configuration, peripherals setup and GPIO setup required before the main application starts.
 */
void BoardInit(void)
{
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MSS_UART_Init(&huart,USART1,115200);

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r****  CAN LOOP-BACK Test program  ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");


}


/**
 * @brief  Core task function for the application.
 * @details This function implements the core functionality of the application.
 * 		It typically handles the main control flow,processes inputs, updates system states,
 * 		and calls other tasks or functions as necessary.
 */
void Core_Task(void)
{
	if( COM_DeviceInit() == HAL_OK)
	{
		PRINT_STRING("\r\nCAN LOOP-BAKC Test :PASS\r\n");
	}else {
		PRINT_STRING("\r\nCAN LOOP-BAKC Test :FAIL\r\n");
	}
	/* Task entry point */
	while(1)
	{

	}
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PE8 PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,0);

}
