/******************************************************************************************************************
* Copyright	: Instrutel - 2025                                                                                    *
*                                                                                                                 *
* Author 	: Venkatesh                                                                                           *
* Customer	: ins300                                                                                              *
*                                                                                                                 *
* Date		: 2025 FEB 01                                                                                         *
* Project	: self                                                                                                *
*                                                                                                                 *
* Developed for 						                                                                          *
* Processor	: ST407                                                                                               *
*******************************************************************************************************************
******************************************************************************************************************/
/*Description: Project is to for                                                                                 */
/*									Documentation										                         */
/******************************************************************************************************************
*                                                                                                                 *
*                                       #DEFINES                                                                  *
*                                                                                                                 *
******************************************************************************************************************/

#ifndef BOARDS_CONFIG_INC_HW_CONFIG_H_
#define BOARDS_CONFIG_INC_HW_CONFIG_H_

#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_gpio.h"
//#include"main.h"

#if !defined __HAL_LINKDMA_PTR
#define __HAL_LINKDMA_PTR(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
                        do{                                                      \
                              (__HANDLE__)->__PPP_DMA_FIELD__ = (__DMA_HANDLE__); \
                              (__DMA_HANDLE__)->Parent = (__HANDLE__);             \
                          } while(0U)
#endif

/* ETHERNET RESET GPIO  */
#ifndef APP_SW_RESET_DIGIO_LINE
#define APP_SW_RESET_DIGIO_LINE			GPIO_PIN_2 //DIGIO_66//PE2
#define APP_SW_RESET_DIGIO_PORT			GPIOE
#endif

/* ------- GLCD CONFIGURATION ----------*/
/* display GLCD related pins*/

#define ENABLE_PIN_PORT	    GPIOE
#define ENABLE_PIN          GPIO_PIN_7
#define RS_PIN_PORT			GPIOA
#define RS_PIN              GPIO_PIN_0

#define DISPLAY_DATA_PORT	 GPIOE

#define DATA_LINE_1         GPIO_PIN_8
#define DATA_LINE_2         GPIO_PIN_9
#define DATA_LINE_3         GPIO_PIN_10
#define DATA_LINE_4         GPIO_PIN_11
#define DATA_LINE_5         GPIO_PIN_12
#define DATA_LINE_6         GPIO_PIN_13
#define DATA_LINE_7         GPIO_PIN_14
#define DATA_LINE_8         GPIO_PIN_15

/* ---------  END --------*/


/* KEYPAD : G_LCD */
#ifndef DEV_KEY_PAD
#define DEV_KEY_PAD        1
#define DEV_KEYPAD_KEY1     GPIO_PIN_10//DIGIO_26//pb10
#define DEV_KEYPAD_KEY2		GPIO_PIN_13//DIGIO_29//pb13
#define DEV_KEYPAD_KEY3		GPIO_PIN_14//DIGIO_30//pb14
#define DEV_KEYPAD_KEY4		GPIO_PIN_15//DIGIO_31//pb15
#define DEV_KEYPAD_PORT     GPIOB
#define HAL_Get_DigIO(PIN)	HAL_GPIO_ReadPin(DEV_KEYPAD_PORT,PIN)
#define KEY_PAD_TASK_PERIOD 100
#endif

///////////////////////////////////      I2C LINES CONFIGURATION /////////////////////////////////////////////////////

#define I2C1_SCL				GPIO_PIN_8	//PB6
#define I2C1_SDA				GPIO_PIN_9	//PB7
#define I2C1_PORT				GPIOB

#define I2C1_DMA_TX				DMA2_Channel5
#define I2C1_DMA_RX				DMA2_Channel3
#define I2C1_DMA_TX_IRQn        DMA1_Channel5_IRQn
#define I2C1_DMA_RX_IRQn        DMA1_Channel3_IRQn

#define I2C2_SCL				GPIO_PIN_10	//PB10
#define I2C2_SDA				GPIO_PIN_11	//PB11
#define I2C2_PORT				GPIOB

#define I2C2_DMA_TX				DMA2_Channel5
#define I2C2_DMA_RX				DMA2_Channel3
#define I2C2_DMA_TX_IRQn        DMA1_Channel5_IRQn
#define I2C2_DMA_RX_IRQn        DMA1_Channel3_IRQn

#define I2C3_SCL				GPIO_PIN_8	//PA8
#define I2C3_PORT				GPIOA
#define I2C3_SDA				GPIO_PIN_9	//PC9
#define I2C3_PORT_Ex			GPIOC

#define I2C3_DMA_TX_CH			DMA2_Channel5
#define I2C3_DMA_RX_CH			DMA2_Channel3
#define I2C3_DMA_TX_IRQn        DMA1_Channel5_IRQn
#define I2C3_DMA_RX_IRQn        DMA1_Channel3_IRQn

			//-- UART LINES --//
#define UART1_TX_LINE			GPIO_PIN_9//PA9
#define UART1_RX_LINE			GPIO_PIN_10//PA10
#define UART1_PORT 				GPIOA
//-- REMAPPABLE UART LINES --//
#define UART1_REMAPING		(1)

#if defined UART1_REMAPING && (UART1_REMAPING == 1)
	#undef  UART1_TX_LINE
	#undef  UART1_RX_LINE
	#undef  UART1_PORT
	#define UART1_TX_LINE	GPIO_PIN_6 	//PB6
	#define UART1_RX_LINE	GPIO_PIN_7 	//PB7
	#define UART1_PORT 		GPIOB
#endif

#define UART2_TX_LINE		GPIO_PIN_5 //PA2
#define UART2_PORT			GPIOD
#define UART2_RX_LINE		GPIO_PIN_3 //PA3
#define UART2_PORT_EX		GPIOA

#define UART2_REMAPING		(0)   // NO REMAP PINS  FOR USART2

#define UART3_TX_LINE		GPIO_PIN_8 //PB10
#define UART3_RX_LINE		GPIO_PIN_9 //PB11
#define UART3_PORT       	GPIOD

#define UART3_REMAPING		(0)

#if(UART3_REMAPING == 1)
	#undef  UART3_TX_LINE
	#undef  UART3_RX_LINE
	#undef	UART3_PORT
	#define UART3_TX_LINE	GPIO_PIN_10 //PC10
	#define UART3_RX_LINE	GPIO_PIN_11 //PC11
	#define UART3_PORT			GPIOC
#endif

#define UART4_TX_LINE		GPIO_PIN_0 //PA0
#define UART4_RX_LINE		GPIO_PIN_1 //PA1
#define UART4_PORT       	GPIOA
#define UART4_REMAPING		(0)  // NO REMAP PINS FOR UART4
#if(UART4_REMAPING == 1)
#define UART4_TX_LINE		GPIO_PIN_10 //Pc10
#define UART4_RX_LINE		GPIO_PIN_11 //Pc11
#define UART4_PORT       	GPIOC
#endif
#define UART5_TX_LINE		GPIO_PIN_12 //PC12
#define UART5_RX_LINE		GPIO_PIN_2 //PD2
#define UART5_TX_PORT       GPIOC
#define UART5_RX_PORT       GPIOD

#define UART6_TX_LINE		GPIO_PIN_6 //PC6
#define UART6_RX_LINE		GPIO_PIN_7 //PC7
#define UART6_PORT          GPIOC

/******************************************************************
 * 			## --- SPI MACROS ---  ##						  *
 ******************************************************************/

#define SPI3_MISO_DIGIO    	GPIO_PIN_11
#define SPI3_MOSI_DIGIO		GPIO_PIN_12
#define SPI3_SCK_DIGIO		GPIO_PIN_10
#define SPI3_PORT  			GPIOC

#define SPI2_MISO_DIGIO    	GPIO_PIN_15
#define SPI2_MOSI_DIGIO		GPIO_PIN_14
#define SPI2_SCK_DIGIO		GPIO_PIN_10
#define SPI2_PORT  			GPIOB

/*  SPI2 DGIO LINES */
#define SPI1_MISO_DIGIO    	GPIO_PIN_6
#define SPI1_SCK_DIGIO		GPIO_PIN_5
#define SPI1_PORT  			GPIOA
#define SPI1_MOSI_DIGIO		GPIO_PIN_5
#define SPI1_ExPORT         GPIOB

/******************************************************************
 * 			## DMA CHANNEL ALLOCATION ##						  *
 ******************************************************************/

#define UART1_DMA_TX        DMA2_Stream7
#define UART1_DMA_TX_CH     DMA_CHANNEL_4
#define UART1_DMA_TX_IRQn   DMA2_Stream7_IRQn
#define UART1_DMA_RX     	DMA2_Stream5
#define UART1_DMA_RX_CH     DMA_CHANNEL_4
#define UART1_DMA_RX_IRQn   DMA2_Stream5_IRQn

#define UART2_DMA_TX        DMA1_Stream6
#define UART2_DMA_TX_CH     DMA_CHANNEL_4
#define UART2_DMA_TX_IRQn   DMA1_Stream6_IRQn
#define UART2_DMA_RX     	DMA1_Stream5
#define UART2_DMA_RX_CH     DMA_CHANNEL_4
#define UART2_DMA_RX_IRQn   DMA1_Stream5_IRQn

#define UART3_DMA_TX        DMA1_Stream3
#define UART3_DMA_TX_CH     DMA_CHANNEL_4
#define UART3_DMA_TX_IRQn   DMA1_Stream3_IRQn
#define UART3_DMA_RX     	DMA1_Stream1
#define UART3_DMA_RX_CH     DMA_CHANNEL_4
#define UART3_DMA_RX_IRQn   DMA1_Stream1_IRQn

#define UART4_DMA_TX        DMA1_Stream4
#define UART4_DMA_TX_CH     DMA_CHANNEL_4
#define UART4_DMA_TX_IRQn   DMA1_Stream4_IRQn
#define UART4_DMA_RX     	DMA1_Stream2
#define UART4_DMA_RX_CH     DMA_CHANNEL_4
#define UART4_DMA_RX_IRQn   DMA1_Stream2_IRQn

#define UART5_DMA_TX        DMA1_Stream7
#define UART5_DMA_TX_CH     DMA_CHANNEL_4
#define UART5_DMA_TX_IRQn   DMA1_Stream7_IRQn
#define UART5_DMA_RX     	DMA1_Stream0
#define UART5_DMA_RX_CH     DMA_CHANNEL_4
#define UART5_DMA_RX_IRQn   DMA1_Stream0_IRQn

#define UART6_DMA_TX        DMA2_Stream6
#define UART6_DMA_TX_CH     DMA_CHANNEL_5
#define UART6_DMA_TX_IRQn   DMA2_Stream6_IRQn
#define UART6_DMA_RX     	DMA2_Stream1
#define UART6_DMA_RX_CH     DMA_CHANNEL_5
#define UART6_DMA_RX_IRQn   DMA2_Stream1_IRQn

#define SPI1_DMA_TX         DMA2_Stream3
#define SPI1_DMA_TX_CH      DMA_CHANNEL_3
#define SPI1_DMA_TX_IRQn    DMA2_Stream3_IRQn
#define SPI1_DMA_RX			DMA2_Stream2
#define SPI1_DMA_RX_CH      DMA_CHANNEL_3
#define SPI1_DMA_RX_IRQn    DMA2_Stream2_IRQn

#define SPI2_DMA_TX         DMA1_Stream4
#define SPI2_DMA_TX_CH      DMA_CHANNEL_0
#define SPI2_DMA_TX_IRQn    DMA1_Stream4_IRQn
#define SPI2_DMA_RX			DMA1_Stream3
#define SPI2_DMA_RX_CH      DMA_CHANNEL_0
#define SPI2_DMA_RX_IRQn    DMA1_Stream3_IRQn

#define SPI3_DMA_TX         DMA1_Stream7
#define SPI3_DMA_TX_CH      DMA_CHANNEL_0
#define SPI3_DMA_TX_IRQn    DMA1_Stream7_IRQn
#define SPI3_DMA_RX			DMA1_Stream2
#define SPI3_DMA_RX_CH      DMA_CHANNEL_0
#define SPI3_DMA_RX_IRQn    DMA1_Stream2_IRQn

#define ADC1_DMA_TX			DMA2_Stream0
#define ADC1_DMA_TX_CH      DMA_CHANNEL_0
#define ADC1_DMA_TX_IRQn    DMA2_Stream0_IRQn

#ifndef ADC_DEV_ADDRESS
#define ADC_DEV_ADDRESS 0
#endif

#endif /* BOARDS_CONFIG_INC_HW_CONFIG_H_ */
