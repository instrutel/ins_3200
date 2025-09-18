  /** The CAN-1 is configured as follows:
    - Bit Rate   = 1 Mbit/s  
    - CAN Clock  = external clock (HSE)
    - ID Filter  = All identifiers are allowed
    - RTR = Data
    - DLC = 8 byte
	
  /** CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
	
  /** Tx Packet 
	STD-ID - 121
 	
  /** RX PACKET 
	STD_ID - 321 and 123