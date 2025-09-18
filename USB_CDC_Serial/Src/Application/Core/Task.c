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
#include "usb_device.h"
#include "sw_config.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart;
#define UART_RECV_BUF_SIZE  32U
static uint8_t g_uart_to_debug[UART_RECV_BUF_SIZE];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
********** [Instrutel] USB Application - USB CDC to UART Serial Example **********\n\r\
******************************************************************************\n\r\
 Example project demonstrates the use of USB CDC for serial transmission and reception.\n\r\
------------------------------------------------------------------------------\n\r\
 - Reads data from UART1 and transmits it over USB as serial data.\n\r\
 - Receives USB serial data and forwards it to UART1.\n\r\
******************************************************************************\n\r";

const char g_message2[] =
"\n\n\n\rSelect an option:\r\n\
---------------------------------------------\r\n\
 Type 1 - Enter data to transmit over USB CDC\r\n\
 Type h - Display the menu options\r\n";


/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
void process_packet(uint8_t *data,uint32_t len);
static uint8_t get_data_from_uart(void);

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
	PRINT_STRING("\n\r/*******************************************************/\n\r");
	PRINT_STRING("/*                                                     */\n\r");
	PRINT_STRING("/*         USB CDC to SERIAL - Test Program            */\n\r");
	PRINT_STRING("/*                                                     */\n\r");
	PRINT_STRING("/*******************************************************/\n\r");

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
	/* USB DEVICE Init  */
	MX_USB_DEVICE_Init(process_packet);

	PRINT_STRING("Awaiting USB cable connection to initialize USB CDC interface...\r\n");

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
					CDC_Transmit_FS(g_uart_to_debug, rx_bytes);
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
void process_packet(uint8_t *data,uint32_t len)
{
	uint8_t lbuf[80] ={0};
	sprintf((char*)lbuf, "\n\rRecvd: %s size:%d\r\n",data,(uint16_t)len);
	PRINT_STRING(lbuf);
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
    uint8_t rx_buff[2] ={0};
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < UART_RECV_BUF_SIZE ; count++)
    {
    	g_uart_to_debug[count] = 0u;
    }

    PRINT_STRING("\r\n>>Enter data of 32 bytes to transmit via USB CDC:\r\n");

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
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u; count--;
            }
        }
    }
    g_uart_to_debug[count] = 0x00;
//    PRINT_STRING(g_uart_to_debug);
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
