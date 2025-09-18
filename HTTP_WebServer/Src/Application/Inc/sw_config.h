/*
 * sw_config.h
 *
 *  Created on: Feb 9, 2025
 *      Author: Admin
 */

#ifndef APPLICATION_INC_SW_CONFIG_H_
#define APPLICATION_INC_SW_CONFIG_H_

/*@Add Group: LAN Network Settings Configurations
 */
#define MY_STATIC_IP_ADDRESS			"192.168.001.008\0"
#define MY_STATIC_IP_GATEWAY			"192.168.001.001\0"
#define MY_STATIC_IP_MASK				"255.255.255.000\0"
#define MY_ETH_MAC_STRING				"00:80:E1:00:00:00\0"
#define DEFAULT_IP_TYPE                 STATIC_IP

/* Destination IP */
#define  DEST_IP_ADDRESS	"192.168.001.003\0"
#define  DEST_IP_ADDR0  	192
#define  DEST_IP_ADDR1  	168
#define  DEST_IP_ADDR2  	1
#define  DEST_IP_ADDR3  	3

#define DEST_CLIENT_PORT	25
#define DEST_SERVER_PORT 	25

#define TCP_SERVER      	0
#define TCP_CLIENT      	1

#endif /* APPLICATION_INC_SW_CONFIG_H_ */
