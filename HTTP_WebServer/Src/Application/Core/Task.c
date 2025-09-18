/**
  ******************************************************************************
  * @file           : Task.c
  * @author			: Instrutel application team
  * @brief          : Core Task application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 Instrutel Systems.
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
#include "lwip.h"
#include "../WEB_SERVER/webserver.h"
#include "sw_config.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart;
static uint8_t g_uart_to_tcp[30];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
******* Instrutel Ethernet Application Example  ***********\n\r\
******************************************************************************\n\r";


/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
void process_packet(uint8_t *data,uint16_t len);
static uint8_t get_data_from_uart(void);

/* Private user code ---------------------------------------------------------*/

#define PRINT_STRING(X)     MSS_UART_Transmit(&huart,(uint8_t*)X);
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
	MX_LWIP_Init();

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r****  Ethernet WEB-SERVER Test program  ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

    /* print eth ip settings */
    char info_string[36];
    sprintf(info_string, "\n\rIP: %s\n\r", MY_STATIC_IP_ADDRESS);
    PRINT_STRING(info_string);
    sprintf(info_string, "GW: %s\n\r", MY_STATIC_IP_GATEWAY);
    PRINT_STRING(info_string);
    sprintf(info_string, "NM: %s\n\r", MY_STATIC_IP_MASK);
    PRINT_STRING(info_string);
}

/**
 * @brief  Core task function for the application.
 * @details This function implements the core functionality of the application.
 * 		It typically handles the main control flow,processes inputs, updates system states,
 * 		and calls other tasks or functions as necessary.
 */
void Core_Task(void)
{
	/* web server init */
    http_server_init();

	/* Task entry point */
	while(1)
	{
		MX_LWIP_Process();
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
    uint8_t rx_buff[1];
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < 32u; count++)
    {
        g_uart_to_tcp[count] = 0u;
    }

    PRINT_STRING("\r\n Enter the data to send over tcp\r\n");

    count = 0u;
    while (!complete)
    {
        rx_size = GET_STRING(rx_buff);
        if(rx_size > 0u)
        {
        	PRINT_STRING( rx_buff );

            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_to_tcp[count++] = rx_buff[0];
            }
            if (32u == count) {
                complete = 1u; count--;
            }
        }
    }
    g_uart_to_tcp[count] = 0x00;
    PRINT_STRING(g_uart_to_tcp);
	PRINT_STRING( "\r\n" );
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
