/**
  ******************************************************************************
  * @file    LwIP/UDP/lwip_udp.c
  * @author  Application Team
  * @version V1.3.1
  * @date    09-October-2024
  * @brief   UDP server/client application
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "sw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

static uint16_t UDPClinetPort = 99; /* define the UDP remote connection port */
struct udp_pcb *hupcb;

static char  rx_buf[50];
//static char  tx_buf[50];

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void (*Packetrecvcb)(uint8_t *data, uint16_t Len);

void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udp_client_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/


/******************************************************************************************************************
									##  UDP Server  ##
 ******************************************************************************************************************/


/**
  * @brief  Initialize the UDP server application.
  * @param  : the UDP remote connection port
  * @param  : CallBacl Functoin
  * @retval : none
  */
uint8_t udp_server_init(uint16_t cPort,void (*func)(uint8_t *data, uint16_t Len))
{
   struct udp_pcb *upcb;
   err_t err; ip_addr_t MyIPaddr;

   /* Create a new UDP control block  */
   upcb = udp_new();

   if (upcb)
   {
//		/*assign destination IP address */
		IP4_ADDR(&MyIPaddr, LOCAL_IP_ADDR0, LOCAL_IP_ADDR1, LOCAL_IP_ADDR2, LOCAL_IP_ADDR3 );
//		upcb->remote_port = 43;
		/* Bind the upcb to the UDP_PORT port */
		/* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
		err = udp_bind(upcb, &MyIPaddr, cPort);

		if(err == ERR_OK)
		{
			/* Set a receive callback for the upcb */
			udp_recv(upcb, udp_server_receive_callback, NULL);
			Packetrecvcb = func;
		}
   }
   return err;
}

uint8_t udp_server_send(uint8_t*buf, uint16_t len)
{
	uint8_t ret = 0;
	if( hupcb == NULL ) {
		return ret;
	}

	/* allocate pbuf from pool*/
	struct pbuf *newpbuf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_POOL);

	if (newpbuf != NULL)
	{
		/* copy data to pbuf */
		pbuf_take(newpbuf, (char*)buf, len);

		/* send udp data */
		err_t err = udp_send(hupcb, newpbuf);
		if(err != ERR_OK)
		{
			udp_disconnect(hupcb);
			hupcb =  NULL;
		}
		/* free pbuf */
		pbuf_free(newpbuf);
		/* success ret status */
		ret= 1;
	}
	return ret;
}

/* Cleanup function
 *
 */
void udp_server_cleanup(void)
{
	udp_disconnect(hupcb);
	udp_remove(hupcb);
	hupcb =  NULL;
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	if( hupcb == NULL || hupcb != upcb ) {
		if(hupcb != NULL)
		{
			udp_server_cleanup();
		}
		/* Connect to the remote client */
		udp_connect(upcb, addr, port);
		hupcb =  upcb;
	}
	uint8_t Index = sprintf(rx_buf,"[%d]:",port);
	/* Copy the data to local buffer */
	memcpy(rx_buf+Index,p->payload,p->len);

	/* new Packet Recv callback */
	if( Packetrecvcb != NULL )
		Packetrecvcb((uint8_t*)rx_buf,p->len);

	/* Free the p buffer */
	pbuf_free(p);
}

/******************************************************************************************************************
									##  UDP Client  ##
 ******************************************************************************************************************/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
  */
uint8_t udp_client_connect(uint16_t cPort,void (*func)(uint8_t *data, uint16_t Len))
{
  ip_addr_t MyIPaddr,DestIPaddr;

  err_t err = ERR_ABRT;

  /* Create a new UDP control block  */
  hupcb = udp_new();

  if (hupcb !=NULL)
  {
	/*assign destination IP address */
	IP4_ADDR(&MyIPaddr, LOCAL_IP_ADDR0, LOCAL_IP_ADDR1, LOCAL_IP_ADDR2, LOCAL_IP_ADDR3 );

	/* Bind the upcb to the UDP_PORT port */
	/* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
	err = udp_bind(hupcb, &MyIPaddr, LOCAL_PORT);

    /*assign destination IP address */
	IP4_ADDR(&DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );
    /* configure destination IP address and port */
    err= udp_connect(hupcb, (const ip_addr_t*)&DestIPaddr, DEST_SERVER_PORT);

    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(hupcb, udp_client_receive_callback, NULL);
      Packetrecvcb = func;
    }
  }
  return err;
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
uint8_t udp_client_send(uint8_t *pdata,uint16_t Len)
{
  struct pbuf *p;
  err_t err = ERR_ABRT;

  /* allocate pbuf from pool*/
  p = pbuf_alloc(PBUF_TRANSPORT, Len, PBUF_POOL);

  if (p != NULL)
  {
    /* copy data to pbuf */
    pbuf_take(p, (char*)pdata, Len);

    /* send udp data */
    err = udp_send(hupcb, p);

    /* free pbuf */
    pbuf_free(p);
  }
  return err;
}

/* Cleanup function
 *
 */
void udp_client_cleanup(void)
{
	udp_disconnect(hupcb);
	udp_remove(hupcb);
	hupcb =  NULL;
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_client_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	uint8_t Index = sprintf(rx_buf,"[%d]:",port);
	/* Copy the data to local buffer */
	memcpy(rx_buf+Index,p->payload,p->len);

	/* new Packet Recv callback */
	if( Packetrecvcb != NULL )
		Packetrecvcb((uint8_t*)rx_buf,p->len);
	/* Free receive pbuf */
	pbuf_free(p);
}
