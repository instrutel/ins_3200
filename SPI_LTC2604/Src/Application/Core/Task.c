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
#include "BSP_If.h"
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
"\n\r******************************************************************************\n\r\
*************** Instrutel SPI Application LTC1867 ADC Example  *******************\n\r\
***********************************************************************************\n\r\
 Example project demonstrates the using SPI Serial communication reading LTC186X ADC\n\r\
-----------------------------------------------------------------------------------\n\r\
************************************************************************************\n\r";

const char g_message2[] = "\n\n\n\rSelect option: \r\n \
 Type 1  Reading Signal analog channel\n\r \
 Type 2  Reading all the analog channels\n\r \
 Type h  Display the menu options \n\r";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static uint8_t get_data_from_uart(void);
void CoreApplicationSubTask(uint8_t cmd);

/* Private user code ---------------------------------------------------------*/
#define PRINT_STRING(X)     MSS_UART_Transmit(&huart,(uint8_t*)X);
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
	PRINT_STRING("\n\r/*********************************************************/\n\r");
    PRINT_STRING("\n\r**** LTC186X Analog to Digital Converter Test program ****\n\r");
    PRINT_STRING("\n\r/*********************************************************/\n\r");

    PRINT_STRING(g_message2);

}

/**
 * @brief  Core task function for the application.
 * @details This function implements the core functionality of the application.
 * 		It typically handles the main control flow,processes inputs, updates system states,
 * 		and calls other tasks or functions as necessary.
 */
void Core_Task(void)
{
	uint16_t rx_bytes = 0; uint8_t rx_buff[2] ={0};
	static uint8_t ChnnlReadType = 2;

	/* Initialize off-chip device interface */
    BSP_IfInit();

	/* Task entry point */
	while(1)
	{
		rx_bytes = GET_STRING(rx_buff);
		if (rx_bytes > 0)
		{
			switch(rx_buff[0])
			{
				case '1':
					/* single Channel read */
					ChnnlReadType = 1;
					rx_bytes = get_data_from_uart();
					pTimeOut  = HAL_GetTick() ;
					break;
				case '2':
					/* read all the callen data */
					ChnnlReadType = 2;
					pTimeOut  = HAL_GetTick();
					break;
				case 'h':
					PRINT_STRING(g_message2);
					break;
			}
			rx_bytes = 0;
		}
		CoreApplicationSubTask(ChnnlReadType);
	}
}


/**
  * @brief  Execute a core application subtask based on command.
  * @details This function handles a specific subtask or operation within the core application,
  *          depending on the provided command code. Each command corresponds to a predefined
  *          functionality or feature within the application layer.
  * @param  cmd  The command identifier indicating which subtask to perform.
  * @retval None
  */
void CoreApplicationSubTask(uint8_t cmd)
{
	if(HAL_GetTick() > pTimeOut)
	{
		char TxBuff[130] ={0};
		switch(cmd)
		{
			case 2:
				PRINT_STRING("\n\r/********** READING ADC CHANNELS COUNT **********/\r\n");
				/*  SUB task */
				BSP_IfRead(0, g_uart_to_debug, UART_RECV_BUF_SIZE);
				uint16_t Chnl[8] = {0};
				for(uint8_t i = 0; i < 8 ; ++i)
				{
					Chnl[i] = (uint16_t)((g_uart_to_debug[(i*2)] << 8) + g_uart_to_debug[(i*2)+1]);
				}
				sprintf(TxBuff,"\n\rCH1 :%d\r\nCH2 :%d\r\nCH3 :%d\r\nCH4 :%d\r\nCH5 :%d\r\nCH6 :%d\r\nCH7 :%d\r\nCH8 :%d\n\r",
						(uint16_t)Chnl[0],(uint16_t)Chnl[1],(uint16_t)Chnl[2], \
						(uint16_t)Chnl[3],(uint16_t)Chnl[4],(uint16_t)Chnl[5], \
						(uint16_t)Chnl[6],(uint16_t)Chnl[7]);
				PRINT_STRING(TxBuff);
				pTimeOut = HAL_GetTick() + 5000;
				break;
			case 1:
				/*  SUB task */
				BSP_IfRead(0, g_uart_to_debug, UART_RECV_BUF_SIZE);
				uint16_t ch = (uint16_t)((g_uart_to_debug[ChannelNum*2] << 8) + g_uart_to_debug[(ChannelNum*2)+1]);;
				sprintf(TxBuff, "\r\nCH%d - %d", ChannelNum, ch);
				PRINT_STRING(TxBuff);
				pTimeOut = HAL_GetTick() + 1000;
				break;
		}

	}
}

/*------------------------------------------------------------------------------*/

/**
 * @brief  Retrieve one byte of data from the UART receive buffer store it in dest buffer.
 * @details This function reads bytes of data from the UART peripheral.
 * @return uint8_t  The byte of data received from the UART.
 */
static uint8_t get_data_from_uart(void)
{
    uint8_t complete = 0;
    uint8_t rx_buff[3] ={0};
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < 30u; count++)
    {
        g_uart_to_debug[count] = 0u;
    }

    PRINT_STRING("\r\n Enter the channel number(0 to 7) to read : ");

    count = 0u;
    while (!complete)
    {
        rx_size = GET_STRING(rx_buff);
        if(rx_size > 0u)
        {
        	PRINT_STRING( rx_buff );

            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else if(rx_buff[0] >= '0' &&  rx_buff[0] <= '7'){
            	ChannelNum = (rx_buff[0] - '0');
            	complete = 1u;
            }
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u;
            }
        }
    }
    return(count);
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
