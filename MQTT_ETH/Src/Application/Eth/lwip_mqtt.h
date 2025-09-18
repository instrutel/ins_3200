/*
 * lwip_mqtt.h
 *
 *  Created on: Apr 13, 2025
 *      Author: Admin
 */

#ifndef APPLICATION_ETH_LWIP_MQTT_H_
#define APPLICATION_ETH_LWIP_MQTT_H_


#include "lwip/apps/mqtt.h"

void Mqtt_do_connect(mqtt_client_t *client, const char * topic);
void Mqqt_publish(mqtt_client_t *client, const char *topic , void *arg);

#endif /* APPLICATION_ETH_LWIP_MQTT_H_ */
