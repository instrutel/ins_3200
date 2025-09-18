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
#include "../../Utilities/Interface/bsp_if.h"
#include "sw_config.h"
#include<string.h>

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
//
///******************************************************************************
// *  Greeting messages displayed over the UART.
// */
//const char g_greeting_msg[] =
//"\n\r******************************************************************************\n\r\
//******* Instrutel CAN  Application  Example  ***********\n\r\
//******************************************************************************\n\r\
// Example project demonstrates the using CAN communication reading LTC1867 ADC count \n\r\
//------------------------------------------------------------------------------\n\r\
//******************************************************************************\n\r";

const char g_message2[] = "\n\n\n\rSelect option: \r\n\
 Type 1  Send CAN-STD Packet\n\r\
 Type h  Display the menu options \n\r";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static uint8_t get_data_from_uart(void);
void process_packet(uint8_t *data,uint16_t PayloadLen);
void CoreApplicationInit(void);

/* Private user code ---------------------------------------------------------*/
#define PRINT_STRING(X)     HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,0); \
							MSS_UART_Transmit(&huart,(uint8_t*)X); \
							HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,1)

#define GET_STRING(X)    	MSS_UART_Receive(&huart,(uint8_t*)X)


/**
  * @brief : Board Initialization
  * @param : None
  * @retval: int
  */
void BoardInit(void)
{
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MSS_UART_Init(&huart,USART1,115200);

	/* Display greeting message */
//	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r***************  FULL-CAN Test PROGRAM  *****************\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

    PRINT_STRING(g_message2);
}


/* @brief : Core application main task
 * @param : None
 * @retval: None
 */
void Core_Task(void)
{
	uint16_t rx_bytes = 0; uint8_t rx_buff[2] ={0};
	CoreApplicationInit();
	/* Task entry point */
	while(1)
	{
		rx_bytes = GET_STRING(rx_buff);
		if (rx_bytes > 0)
		{
			switch(rx_buff[0])
			{
				case '1':
					rx_bytes = get_data_from_uart();
					if( BSP_IfWrite(1, g_uart_to_debug, rx_bytes) != 0)
					{
						PRINT_STRING("\r\[TX] :send packet failed\n");
					}else{
						PRINT_STRING("\r\n[Tx] :sent OK\n");
					}
					break;
				case '2':
					break;
				case 'h':
					PRINT_STRING(g_message2);
					break;
			}
			rx_bytes = 0;
		}
	}
}


/*@Brief : Data receive packets handler
 *@param : Received Data packet
 *@param : Data length
 */
void CoreApplicationInit(void)
{
	BSP_IfInit(&process_packet);
}

/**
 * @brief  A sample handler function that processes the received Ethernet packet.
 * @details This function is called after a packet is received , it simply prints
 * 		the data for demonstration purposes.
 * @param  data  Pointer to the received packet data.
 * @param  len   Length of the received packet.
 */
void process_packet(uint8_t *data,uint16_t PayloadLen)
{
	uint8_t lbuf[60] ={0};
	memcpy(lbuf,data,PayloadLen);
	PRINT_STRING("\n\r[Rx]:\033[32m ");
	PRINT_STRING(lbuf);
	sprintf((char*)lbuf," \033[0m Len:%d\r\n",PayloadLen);
	PRINT_STRING(lbuf);
}

/*------------------------------------------------------------------------------
  Receive the data from UART
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

    PRINT_STRING("\r\n Enter the data of 1-8 bytes:");

    count = 0u;
    while (!complete)
    {
        rx_size = GET_STRING(rx_buff);
        if(rx_size > 0u)
        {
            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_to_debug[count++] = rx_buff[0];
            }
            if (8 == count) {
                complete = 1u;
            }
        }
    }
    PRINT_STRING("\033[32m"); // Set text color to green
    g_uart_to_debug[count] = 0x00;
    PRINT_STRING(g_uart_to_debug);
    PRINT_STRING("\033[0m"); //// Reset text color to default
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
