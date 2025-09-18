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
#include "lwip.h"
#include "lwip_mqtt.h"
//#include "../TCP/lwip_tcp.h"
#include "sw_config.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU

/* Private variables ---------------------------------------------------------*/
mqtt_client_t *client;

UART_HandleTypeDef huart;
#define UART_RECV_BUF_SIZE  32U
static uint8_t g_uart_to_tcp[UART_RECV_BUF_SIZE];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
******* Instrutel Ethernet Application MQTT Example  ***********\n\r\
******************************************************************************\n\r";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
void recv_packet(uint8_t *data,uint16_t len);
static uint8_t get_data_from_uart(void);
void CoreApplicationSubTask(void);

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
	MX_LWIP_Init();

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r****  Ethernet LwIp MQTT Test program  ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

    /* print eth ip settings */
    char info_string[36];
    sprintf(info_string, "\n\rMQTT IP: %s\n\r", MQTT_IP_ADDRESS);
    PRINT_STRING(info_string);
    sprintf(info_string, "PUB TOPIC: %s\n\r", MQTT_PUB_TOPIC_NAME);
    PRINT_STRING(info_string);
    sprintf(info_string, "SUB TOPIC: %s\n\r", MQTT_SUB_TOPIC_NAME);
    PRINT_STRING(info_string);

}


/* @brief : Core application main task
 * @param : None
 * @retval: None
 */
void Core_Task(void)
{
	/* MQTT Client */
	client = mqtt_client_new();

	/* Task entry point */
	while(1)
	{
		MX_LWIP_Process();
		CoreApplicationSubTask();
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
void CoreApplicationSubTask(void)
{
	static uint32_t pTimeOut_l = 0;
	static uint8_t state_l = 1;
	static uint16_t PacketCnt = 1;

	uint8_t ip_state = DHCP_GetStatus();
	switch(state_l)
	{
	 case 1:
		ip_state =  DHCP_GetStatus();
		if(ip_state == 3 || ip_state == 4) {
			state_l = 2;
		}
		break;
	 case 2:
		  Mqtt_do_connect(client, "Instrutel");
		  state_l = 3;
		break;
	 case 3:
		if(HAL_GetTick() > pTimeOut_l )
		{
			sprintf(g_uart_to_tcp,"Packet Count :%d ",PacketCnt++);
			Mqtt_publish(client, MQTT_PUB_TOPIC_NAME, g_uart_to_tcp);
			pTimeOut_l =  HAL_GetTick() + 5000;
		}
		break;
	}
}


/*------------------------------------------------------------------------------
  Receive the data from UART
 */
static uint8_t get_data_from_uart(void)
{
    uint8_t complete = 0;
    uint8_t rx_buff[2] ={0};
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < 30u; count++)
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
//        	PRINT_STRING( rx_buff );

            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_to_tcp[count++] = rx_buff[0];
            }
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u;
            }
        }
    }
    return(count);
}

/*
 *
 */
void PrintDebug(uint8_t *ptr)
{
	PRINT_STRING(ptr);
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
