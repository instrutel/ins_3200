/*
 * lwip_udp.h
 *
 *  Created on: Feb 8, 2025
 *      Author: Admin
 */

#ifndef UTILITIES_LWIP_UDP_LWIP_UDP_H_
#define UTILITIES_LWIP_UDP_LWIP_UDP_H_

#include "stdint.h"



uint8_t udp_server_init(uint16_t cPort,void (*func)(uint8_t *data, uint16_t Len));

uint8_t udp_server_send(uint8_t*buf, uint16_t len);

void udp_server_cleanup(void);

uint8_t udp_client_connect(void);

// This function is called when an UDP datagrm has been received on the port UDP_PORT.
uint8_t udp_client_send(uint8_t *pdata,uint16_t Len);

void udp_client_cleanup(void);

#endif /* UTILITIES_LWIP_UDP_LWIP_UDP_H_ */
