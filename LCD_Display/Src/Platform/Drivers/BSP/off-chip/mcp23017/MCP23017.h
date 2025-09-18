#ifndef _MCP23017_H_
#define _MCP23017_H_

#include "stm32f4xx_hal.h"


// -- ###################  MCP23017 BANK = 0 REG  ###############

#define IODIRA  	0x00   // -- IO configuration
#define IPOLA    	0x02   // -- Input Polart portA
#define GPINTENA 	0x04   // -- INTR ON Change Pin
#define DEFVALA 	0x06   // -- Default value
#define INTCONA 	0x08   // -- INTR Configuration
#define IOCONA 		0x0A   // -- I/O EXPANDER CONFIGURATION REGISTER
#define GPPUA 		0x0C   // -- PULL UP Configuration
#define INTFA 		0x0E   // -- INTR Flag (Read Only)
#define INTCAPA 	0x10   // -- INTERRUPT CAPTURED VALUE FOR PORT REGISTER
#define GPIO_A 		0x12   // -- GENERAL PURPOSE I/O PORT REGISTER
#define OLATA 		0x14   // -- OUTPUT LATCH REGISTER

#define IODIRB 		0x01
#define IPOLB 		0x03
#define GPINTENB 	0x05
#define DEFVALB 	0x07
#define INTCONB 	0x09
#define IOCONB 		0x0B
#define GPPUB 		0x0D
#define INTFB 		0x0F
#define INTCAPB 	0x11
#define GPIO_B 		0x13
#define OLATB 		0x15

enum
{
	PORT_A = 1,
	PORT_B,
	BANK_0 = 1,  // ---  BANK_0 Default Config
	BANK_1,
	INPUT = 0,
	OUTPUT,
	LOW  = 0 ,
	HIGH ,
};

typedef enum
{
	MCP_GPIO_PIN0,
	MCP_GPIO_PIN1,
	MCP_GPIO_PIN2,
	MCP_GPIO_PIN3,
	MCP_GPIO_PIN4,
	MCP_GPIO_PIN5,
	MCP_GPIO_PIN6,
	MCP_GPIO_PIN7,
}MCPPortPins_e;

typedef enum
{
	MCP_PORT_A = 0x01,
	MCP_PORT_B = 0x02
}MCPPorts_e;

typedef enum
{
	Input =0x01,
	Output = 0x02,
	PullUP = 0x04,
	NnPullUp=0x08,
	NoInterrupt=0x10,
	InterruptOnChange=0x20,
	InterruptOnDef=0x40,
	InputPolRev=0x80
}PortPinConfig_e;


typedef enum
{
	BankConBit   	 =0x01,
	IntPinConBit 	 =0x02,
	SeqOpBit     	 = 0x04,
	SlewRateConBit	 =0x08,
	HAEnBit     	 = 0x10,
	INTODBit 		 = 0x20,
	INTPOLBit 		 = 0x40
}IOConReg_e;

// ################################   Functions  ############################################## //

typedef struct
{
	uint8_t IoDir;    /* Io Direction Register */
	uint8_t IPol;     /*Polarity reversal */
	uint8_t GPInteEn;  /* interrupt enable */
	uint8_t DefVal;   /* default value */
	uint8_t IntCon;   /* Interrupt Control Register */
	uint8_t GPPulUp;  /*PullUp Register Config*/
	uint8_t IntF;	  /* Interrupt Flag Register */
	uint8_t IntCap;   /* Interrupt Capture Register */
	uint8_t Olat;     /* Output Latch Value */
}MCP2307PortConfig_t;


/* Exported functions --------------------------------------------------------*/
/** @defgroup MCP23017_Exported_Functions MCP23017 Exported Functions
  * @{
  */

uint8_t MCP23017_Init(I2C_HandleTypeDef *hi2c,I2C_TypeDef *i2c);
void 	MCP23017_DeInit(I2C_HandleTypeDef *hi2c);
void 	MCP23017_Rest(GPIO_TypeDef *GPIOPort, uint32_t GPIO_PIN);
uint8_t MCP23017_PortConfig(I2C_HandleTypeDef *hi2c,uint8_t   SlaveAddr,MCPPorts_e port,MCP2307PortConfig_t *config);
uint8_t MCP23017_PortPinRead(I2C_HandleTypeDef *hi2c,uint8_t  SlaveAddr,MCPPorts_e port,uint8_t Pin, uint8_t* pdata);
uint8_t MCP23017_PortPinWrite(I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,MCPPorts_e port,uint8_t Pin, uint8_t pValue);
uint8_t MCP23017_PortRegWrite(I2C_HandleTypeDef *hi2c,uint8_t SlaveAddr,MCPPorts_e port,uint8_t pValue);
uint8_t MCP23017_PortRegRead(I2C_HandleTypeDef *hi2c,uint8_t  SlaveAddr,MCPPorts_e port,uint8_t* pdata);

#endif
