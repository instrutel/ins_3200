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
#include<string.h>
#include "lwip.h"
#include "../TCP/lwip_tcp.h"
#include "../FatFs/fatfs.h"
#include "../MSS/UART/mss_uart.h"
#include "sw_config.h"

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
	F_NOP,
	F_LIST,
	F_WRITE,
	F_READ,
	F_CLOSE,
	Fs_INVALID,
	Fs_ERR,
}fs_operation;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU
#define ETH_RECV_BUF_SIZE  64U

/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;
UART_HandleTypeDef huart;

fs_operation fatfs_Req = F_NOP ;
static uint8_t filename[25];
uint8_t retSD;    /* Return value for SD */

static uint8_t gtcprecvbuff[ETH_RECV_BUF_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
void process_packet(uint8_t *data,uint16_t len);
//static uint8_t get_data_from_uart(void);
uint16_t ExtractPacket(uint8_t *pBuff, uint8_t *dest, uint8_t flag,uint16_t Len);

/* Private user code ---------------------------------------------------------*/

#define PRINT_STRING(X)     MSS_UART_Transmit(&huart,(uint8_t*)X);
#define GET_STRING(X)    	MSS_UART_Receive(&huart,(uint8_t*)X)

/******************************************************************************/

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
	MX_SDIO_SD_Init();
	MX_LWIP_Init();

	/* Display greeting message */
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r****  Ethernet TCP & Fatfs Application Test program  ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

    /* print eth ip settings */
    char info_string[36];
    sprintf(info_string, "\n\rIP: %s\n\r", MY_STATIC_IP_ADDRESS);
    PRINT_STRING(info_string);
    sprintf(info_string, "GW: %s\n\r", MY_STATIC_IP_GATEWAY);
    PRINT_STRING(info_string);
    sprintf(info_string, "NM: %s\n\r", MY_STATIC_IP_MASK);
    PRINT_STRING(info_string);

    sprintf(info_string, "tcp server listening on port:%d\n\r", DEST_SERVER_PORT);
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
	fatfs_Req = F_NOP;
	/*-- Initialize the TCP server socket -- */
	tcp_server_init(DEST_SERVER_PORT, &process_packet);

	/* Initialize the Fatfs  */
	FATFS_Init();
	/* Mount the SD Card  */
	retSD = FATFS_Mount();
	switch( retSD )
	{
		case FR_OK:
			PRINT_STRING("\r\nSD-Card mounted \r\n");
			break;
		case FR_NOT_READY:
			/* Disk Not Exist */
			PRINT_STRING("\r\nSD-Card not detected \r\n");
			break;
		case FR_INT_ERR:
		case FR_DISK_ERR:
			/* DISK Reading Failed */
			PRINT_STRING("\r\nSD-Card reading err \r\n");
			break;
	}

	/* Task entry point */
	while(1)
	{
		/* LWIP Process */
		MX_LWIP_Process();
		switch(fatfs_Req)
		{
			case F_READ:
				if( FATFS_FileOpen(filename) == FR_OK) {
					uint32_t rBytes ,size = 0;
					FATFS_FileSize(filename, &size);
					FATFS_FileRead(gtcprecvbuff, size,&rBytes);
					tcp_server_send(gtcprecvbuff, rBytes);
					FATFS_FileClose();
				}else{
					PRINT_STRING("File read Error\r\n");
					tcp_server_send((uint8_t*)"File read Error\r\n", strlen("File read Error\r\n"));
				}
				break;
			case F_WRITE:
				if( FATFS_FileCreate(filename) == FR_OK) {
					uint32_t wBytes  = 0;
					FATFS_FileWrite(gtcprecvbuff, strlen((const char*)gtcprecvbuff), &wBytes);
					sprintf((char*)gtcprecvbuff, "Number of bytes: %lu\r\n",wBytes);
					PRINT_STRING(gtcprecvbuff);
					tcp_server_send((uint8_t*)"FILE WR : OK\r\n",strlen("FILE WR : OK\r\n"));
					FATFS_FileClose();
				}else{
					PRINT_STRING("File write Error\r\n");
					tcp_server_send((uint8_t*)"FILE WR : ERR\r\n",strlen("FILE WR : ERR\r\n"));
				}
				break;
			case F_LIST:
				memset(gtcprecvbuff,0x00,ETH_RECV_BUF_SIZE);
				uint16_t numoffiles= 0;
				FATFS_GetFileList((uint8_t*)"0:/", gtcprecvbuff, &numoffiles, 10);
				if( numoffiles != 0 ) {
					tcp_server_send(gtcprecvbuff,strlen((const char*)gtcprecvbuff));
					PRINT_STRING(gtcprecvbuff);
				}else{
					strcpy((char*)gtcprecvbuff,"NO Files..!\r\n");
					tcp_server_send(gtcprecvbuff,strlen((const char*)gtcprecvbuff));
					PRINT_STRING(gtcprecvbuff);
				}
				break;
			case F_CLOSE:
				break;
			case F_NOP:
				break;
			case Fs_INVALID:
				tcp_server_send((uint8_t*)"INVALID CMD\r\n", 13);
				PRINT_STRING("INVALID CMD\r\n");
				break;
			case Fs_ERR:
				tcp_server_send((uint8_t*)"SD-Card ERR\r\n", 13);
				PRINT_STRING("SD-Card ERR\r\n");
				break;
		}
		fatfs_Req = F_NOP;
	}
}

/**
 * @brief  A sample handler function that processes the received Ethernet packet.
 * @details This function is called after a packet is received , it simply prints
 * 		the data for demonstration purposes.
 * @param  data  Pointer to the received packet data.
 * @param  len   Length of the received packet.
 */
void process_packet(uint8_t *data, uint16_t len)
{
    // processing the received data (for demo purposes)
	if(data[0] == '$' && data[len-1] == '$')
	{
		if( retSD != FR_OK ) { fatfs_Req = Fs_ERR; }
		else if(strncmp((const char*)data+1,"F_LIST",6) == 0)  {
			fatfs_Req = F_LIST;
		}else if(strncmp((const char*)data+1,"F_READ",6) == 0) {
			fatfs_Req = F_READ;
			ExtractPacket(data+8,filename,'$',len);
		}else if(strncmp((const char*)data+1,"F_WRITE",7) == 0) {
			fatfs_Req = F_WRITE;
			uint16_t index = ExtractPacket(data+9,filename,',',len);
			if(index == 0 ) {
				fatfs_Req = Fs_INVALID;
				return ;
			}
			memset(gtcprecvbuff, 0x00,ETH_RECV_BUF_SIZE );
			ExtractPacket(data+index+10,gtcprecvbuff,'$',len);
		}
	}else {
		fatfs_Req = Fs_INVALID;
	}
	return ;
}

/**
 * @brief  Extract a packet from the received data buffer.
 * @details This function extracts a packet from the input buffer `pBuff`, based on the provided length and a condition flag.
 *          The extracted data is then copied to the destination buffer `dest`.
 *          The flag might specify certain extraction conditions or packet type filtering.
 * @param  pBuff  A pointer to the source buffer containing the received data.
 * @param  dest   A pointer to the destination buffer where the extracted packet will be stored.
 * @param  flag   A flag used to indicate the condition or type of packet to be extracted.
 * @param  Len    The length of the packet to be extracted from the source buffer.
 * @return uint16_t The length of the extracted packet. Returns 0 if no packet was extracted or if an error occurred.
 * @note   The function assumes that the destination buffer `dest` has enough space to hold the extracted packet.
 *         It also assumes that the source buffer `pBuff` contains at least `Len` bytes of data.
 */
uint16_t ExtractPacket(uint8_t *pBuff, uint8_t *dest, uint8_t flag,uint16_t Len)
{
	if(pBuff ==  NULL ) return 0;

	uint16_t index =0;
	do {
		dest[index] = pBuff[index];
		index++; if( index >= Len ){
			index = 0;
			break;
		}
	}while(pBuff[index] != flag);
	dest[index] = 0x00;
	return index;
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{
  /*  SDIO_Init  */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;//SDIO_INIT_CLK_DIV;  // for 400khz
  /*  SDIO_Init  */
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
