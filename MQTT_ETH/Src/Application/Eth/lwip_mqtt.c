/*
 * lwip_mqtt.c
 *
 *  Created on: Apr 13, 2025
 *      Author: Admin
 */
#include "lwip/apps/mqtt.h"
#include <string.h>
#include "sw_config.h"


#define MQTT_DEBUG(X,Y,Z)  PrintDebug(X)

char buffer[300];

/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/
static int inpub_id;

static void mqtt_pub_request_cb(void *arg, err_t result);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);

/*
 *
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
  sprintf(buffer,"Incoming publish at topic %s with total length %u\n\r", topic, (unsigned int)tot_len);
  MQTT_DEBUG(buffer,strlen(buffer),1000);
  /* Decode topic string into a user defined reference */
  if(strcmp(topic, "print_payload") == 0) {
    inpub_id = 0;
  } else if(topic[0] == 'A') {
    /* All topics starting with 'A' might be handled at the same way */
    inpub_id = 1;
  }
  else {
    /* For all other topics */
    inpub_id = 9;
  }

}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  if(flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */

    /* Call function or do action depending on reference, in this case inpub_id */
    /* Don't trust the publisher, check zero termination */
	  MQTT_DEBUG("[Rx Mqtt]: ",strlen(buffer),1000);
	  strncpy(buffer,(const char*)data,len); buffer[len] = '\r'; buffer[len+1] = '\n';
	  buffer[len+2] = 0x00;
	  MQTT_DEBUG(buffer,strlen(buffer),1000);

  }
}

static void mqtt_sub_request_cb(void *arg, err_t result)
{
  /* Just print the result code here for simplicity,
     normal behaviour would be to take some action if subscribe fails like
     notifying user, retry subscribe or disconnect from server */
  sprintf(buffer,"Subscribe result: %d\n\r", result);
  MQTT_DEBUG(buffer,strlen(buffer),1000);

}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  err_t err;
  if(status == MQTT_CONNECT_ACCEPTED) {
    sprintf(buffer,"mqtt_connection_cb: Successfully connected\n");
	  MQTT_DEBUG(buffer,strlen(buffer),1000);

    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

    /* Subscribe to a topic named "placa" with QoS level 0, call mqtt_sub_request_cb with result */
    err = mqtt_subscribe(client, MQTT_SUB_TOPIC_NAME, 0, mqtt_sub_request_cb, arg);

    if(err != ERR_OK) {
      sprintf(buffer,"mqtt_subscribe return: %d\n", err);
	  MQTT_DEBUG(buffer,strlen(buffer),1000);

    }
  } else {
    sprintf(buffer,"mqtt_connection_cb: Disconnected, reason: %d\n", status);
	  MQTT_DEBUG(buffer,strlen(buffer),1000);

    /* trying to reconnect */
    Mqtt_do_connect(client);
  }
}


/* Called when publish is complete either with sucess or failure */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
  if(result != ERR_OK) {
    sprintf(buffer,"Publish result: %d\n", result);
	  MQTT_DEBUG(buffer,strlen(buffer),1000);
  }
}

/*
 *
 */

void Mqtt_do_connect(mqtt_client_t *client, const char *topic)
{
  struct mqtt_connect_client_info_t ci;
  err_t err;

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, so set it here */
  ci.client_id = "ins3200";
  ci.client_user = MQTT_USR_NAME;
  ci.client_pass = MQTT_PSW;

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting
     to establish a connection with the server.
     For now MQTT version 3.1.1 is always used */
  ip_addr_t mqttServerIP;
  IP4_ADDR(&mqttServerIP, MQTT_IP_ADDR0, MQTT_IP_ADDR1, MQTT_IP_ADDR2, MQTT_IP_ADDR3);

  err = mqtt_client_connect(client, &mqttServerIP, MQTT_PORT, mqtt_connection_cb, topic, &ci);

  /* For now just print the result code if something goes wrong */
  if(err != ERR_OK) {
    sprintf(buffer,"mqtt_connect return %d\n\r", err);
	  MQTT_DEBUG(buffer,strlen(buffer),1000);
  }
}

/*
 *
 */
void Mqtt_publish(mqtt_client_t *client, const char *topic , void *arg)
{
  //const char *pub_payload= "Hola mundo de mierda!";
  const char *pub_payload= arg;
  err_t err;
  u8_t qos = 0; /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such crappy payload... */
  err = mqtt_publish(client, topic, pub_payload, strlen(pub_payload), qos, retain, mqtt_pub_request_cb, arg);
  if(err != ERR_OK) {
    sprintf(buffer,"Publish err: %d\n\r", err);
	  MQTT_DEBUG(buffer,strlen(buffer),1000);
  }
}

