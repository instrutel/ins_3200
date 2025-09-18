/**
  ******************************************************************************
  * @file           : LwIP/TCP/lwip_tcp.c
  * @author			: Instrutel application team
  * @brief          : tcp client/server application using LwIP RAW API
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
#include "main.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <stdio.h>
#include <string.h>
#include "sw_config.h"
#include "../TCP/lwip_tcp.h"

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//static u8_t  recev_buf[64];
void (*tcp_packetrecv_cb)(uint8_t *data, uint16_t Len);
struct tcp_pcb *Activepcb;          /* pointer on the current tcp_pcb */

#if TCP_CLIENT
/**********************************************************************************************************************
 * 											TCP CLIENT
 *********************************************************************************************************************/

static struct tcp_pcb *client_pcb;
struct tcpclient es;

/* tcp client protocol states */
enum client_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};


/* structure to be passed as argument to the tcp callbacks */
struct tcpclient
{
  enum client_states state;     /* connection status */
  uint8_t TxPacketIndex;        /* transmit Packet count Index  */
  uint8_t RxPacketCount;        /* Number of Packets received count */
};



/* Private function prototypes -----------------------------------------------*/
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcpclient * es);
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//static void  tcp_client_send(struct tcp_pcb *tpcb, struct tcpclient * es);
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  : connects to the tcp server
* @param  : destination port number
* @param  : tcp Packet receive handler function
* @retval : tcp client connection request status
*/
int8_t tcp_client_connect(uint16_t port, void (*func)(uint8_t *data, uint16_t Len))
{
   ip_addr_t DestIPaddr;
   err_t	err = ERR_ABRT;

  /* create new tcp pcb */
   client_pcb = tcp_new();

  if (client_pcb != NULL)
  {
//	/* Binds the connection to a local port number and IP address.
//	   the connection is  bound to all local IP addresses. */
//	err = tcp_bind(client_pcb, IP_ADDR_ANY, 23);
    IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );

    /* connect to destination address/port */
    err = tcp_connect(client_pcb,&DestIPaddr,port,tcp_client_connected);

    tcp_packetrecv_cb = func;
  }
  return err;
}

/*@Brief : Send data packet to the active tcp client
 *@param : pData Pointer to data buffer (u8 data elements).
 *@param : Data length
 *@retval: tcp write err_t status
 */
int8_t tcp_client_send(uint8_t *pData, uint16_t len)
{
	err_t err ;
	if( es.state == ES_CONNECTED )
	{
		es.TxPacketIndex++;
		err = tcp_write(Activepcb, pData, len, 1);
	}
	return err;
}

/*@ Brief : tcp client port close
 *@ param : none
 *@ retval: none
 */
void tcp_client_close(void)
{
	if( Activepcb != NULL )
	{	/* close connection */
		tcp_client_connection_close(Activepcb,&es);
	}
}

/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK
  * @retval err_t: returned error
  */
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	err_t ret_err =  err;

	if ( err == ERR_OK )
	{
		es.state = ES_CONNECTED;
		Activepcb = tpcb;

		/* pass newly allocated es structure as argument to tpcb */
		tcp_arg(client_pcb, &es);

		/* initialize LwIP tcp_recv callback function */
		tcp_recv(tpcb, tcp_client_recv);

		/* initialize LwIP tcp_sent callback function */
		tcp_sent(tpcb, tcp_client_sent);

		/* initialize LwIP tcp_poll callback function */
		tcp_poll(tpcb, tcp_client_poll, 1);

		if(tcp_packetrecv_cb)
			tcp_packetrecv_cb((uint8_t*)"CONNECTED",9);
    }
    else
    {
      /* close connection */
      tcp_client_connection_close(tpcb, &es);
    }
	return ret_err;
}

/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback
  * @param tpcb: tcp connection control block
  * @param err: receive error code
  * @retval err_t: retuned error
  */
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;

	struct tcpclient *es = (struct tcpclient *)arg;

	/* if we receive an empty tcp frame from server => close connection */
	if (p == NULL)
	{
		es->state = ES_CLOSING;
	   /* we're done sending, close connection */
	   tcp_client_connection_close(tpcb, es);
	   ret_err = ERR_OK;
	}
	/* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
	else if(err != ERR_OK)
	{
		/* free received pbuf*/
		if (p != NULL)
		{
		  pbuf_free(p);
		}
		ret_err = err;
	}
	else if( es->state == ES_CONNECTED )
	{
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);

		/* increment message count */
		es->RxPacketCount++;

		/* new Packet Recv callback */
		if( tcp_packetrecv_cb != NULL )
			tcp_packetrecv_cb((uint8_t*)p->payload,p->len);

		pbuf_free(p);

		ret_err = ERR_OK;
	}
	/* data received when connection already closed */
	else
	{
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);

		/* free pbuf and do nothing */
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	return ret_err;
}

/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;

	struct tcpclient *es = (struct tcpclient*)arg;
	if (es != NULL)
	{
		Activepcb =  tpcb;
		if(es->state == ES_CLOSING)
		{
			tcp_client_connection_close(tpcb,es);
		}
		else if( es->TxPacketIndex ) {
			tcp_output(tpcb);
			es->TxPacketIndex = 0;
		}
	}else {
		/* nothing to be done */
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}
	return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @param  arg: pointer on argument passed to callback
  * @param  tcp_pcb: tcp connection control block
  * @param  len: length of data sent
  * @retval err_t: returned error code
  */
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcpclient *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcpclient *)arg;

  if( (es != NULL) && (es->TxPacketIndex > 0))
  {
	  es->TxPacketIndex --;
  }

  return ERR_OK;
}

/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on client structure
  * @retval None
  */
static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcpclient * es )
{
  es->state = ES_CLOSING;
  /* remove callbacks */
  tcp_recv(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_poll(tpcb, NULL,0);

  /* close tcp connection */
  tcp_close(tpcb);
}

#endif


/**********************************************************************************************************************
 * 											TCP SERVER
 *********************************************************************************************************************/
#if TCP_SERVER


/* tcp server protocol states */
enum tcp_server_states
{
  ES_NONE = 0,
  ES_LISTENING,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* structure for maintaing connection infos to be passed as argument
   to LwIP callbacks*/
struct tcp_server_st
{
  u8_t state;             /* current connection state */
  u8_t retries;
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  uint8_t RxPacketCnt;
  uint8_t TxPacketIndex;
};


static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_server_error(void *arg, err_t err);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//static void tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_st *es);
static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_st *es);

struct tcp_server_st *es;
struct tcp_pcb *tcp_server_pcb;

/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
int8_t tcp_server_init(uint16_t port, void (*func)(uint8_t *data, uint16_t Len))
{
	err_t err = ERR_ABRT;
	/* create new tcp server pcb */
	tcp_server_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);

	if (tcp_server_pcb != NULL)
	{
		if(es == NULL ) {
			/* allocate structure es to maintain tcp connection informations */
			es = (struct tcp_server_st *)mem_malloc(sizeof(struct tcp_server_st));
		}
		tcp_setprio(tcp_server_pcb, TCP_PRIO_MIN);

		/* pass newly allocated es structure as argument to tpcb */
		tcp_arg(tcp_server_pcb,es);

		/* bind echo_pcb to port 7 (ECHO protocol) */
		err = tcp_bind(tcp_server_pcb, IP_ADDR_ANY, port);

		if (err == ERR_OK)
		{
		  /* start tcp listening for echo_pcb */
		  tcp_server_pcb = tcp_listen(tcp_server_pcb);

		  /* initialize LwIP tcp_accept callback function */
		  tcp_accept(tcp_server_pcb, tcp_server_accept);
		  /*-- Data receive call back function --*/
		  tcp_packetrecv_cb = func;
		  es->state = ES_LISTENING;
		}
		else
		{
		  /* deallocate the pcb */
		  memp_free(MEMP_TCP_PCB, tcp_server_pcb);
		  /* delete es structure */
	      mem_free(es);
		}
	}
	return err;
}

/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
int8_t tcp_server_send(uint8_t *ptr,uint16_t Len)
{
	err_t wr_err = -1;

	if( es != NULL && es->state == ES_ACCEPTED )
	{
		/* send udp data */
		wr_err = tcp_write(Activepcb, ptr, Len, 1);
		es->TxPacketIndex++;
	}
	return wr_err;
}
/**
 * @brief : closes the tcp connection
 * @param : None
 * @retval: None
 */
void tcp_server_close(void)
{
	tcp_server_connection_close(Activepcb,es);
}


/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: tcp_server_st pointer
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used
  * @retval err_t: error status
  */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	err_t ret_err;
	struct tcp_server_st *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);

	es = (struct tcp_server_st *)arg;
	/* set priority for the newly accepted tcp connection newpcb */
	tcp_setprio(newpcb, TCP_PRIO_MIN);

	if (es != NULL)
	{
		es->state = ES_ACCEPTED;
		Activepcb = newpcb;
		es->retries = 0;

		/* pass newly allocated es structure as argument to newpcb */
		tcp_arg(newpcb, es);

		/* initialize lwip tcp_recv callback function for newpcb  */
		tcp_recv(newpcb, tcp_server_recv);

		/* initialize lwip tcp_err callback function for newpcb  */
		tcp_err(newpcb, tcp_server_error);

		/* initialize lwip tcp_sent callback function for newpcb  */
		tcp_sent(newpcb,tcp_server_sent);

		/* initialize lwip tcp_poll callback function for newpcb */
		tcp_poll(newpcb, tcp_server_poll, 0);

		if(tcp_packetrecv_cb)
			tcp_packetrecv_cb("CONNECTED",9);

		ret_err = ERR_OK;
	}
	else
	{
		/*  close tcp connection */
		tcp_server_connection_close(newpcb, es);
		/* return memory error */
		ret_err = ERR_MEM;
	}
	return ret_err;
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	struct tcp_server_st *es;
	err_t ret_err;

	LWIP_ASSERT("arg != NULL",arg != NULL);

	es = (struct tcp_server_st *)arg;
	/* Acknowledge data reception */
	tcp_recved(tpcb, p->tot_len);

	if((err == ERR_OK) && (p != NULL))
	{
		/* increment message count */
		es->RxPacketCnt++;
		/* new Packet Recv callback */
		if( tcp_packetrecv_cb != NULL )
			tcp_packetrecv_cb((uint8_t*)p->payload,p->len);

		pbuf_free(p);
		ret_err = ERR_OK;
	}
	/* if we receive an empty tcp frame from client => close connection */
	if ( ( p == NULL ) || (err != ERR_OK ))
	{
		/* remote host closed connection */
		es->state = ES_CLOSING;
		ret_err = ERR_OK;
		/* else : a non empty frame was received from client but for some reason err != ERR_OK */
		/* free received pbuf*/
		if (p != NULL)
		{
			ret_err = err;
			pbuf_free(p);
		}
		tcp_server_connection_close(tpcb, es);
	}
	return ret_err;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs.
  * @param  arg: pointer on argument parameter
  * @param  err: not used
  * @retval None
  */
static void tcp_server_error(void *arg, err_t err)
{
  struct tcp_server_st *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_server_st *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_server_st *es;

  es = (struct tcp_server_st *)arg;
  if (es != NULL)
  {
		if( tpcb->state == CLOSED || tpcb->state == CLOSE_WAIT  )
		{
			es->state = ES_NONE;
			/* connection closed state */
			if( tcp_packetrecv_cb != NULL )
				tcp_packetrecv_cb((uint8_t*)"CLOSED",6);
		}
      else if(es->TxPacketIndex)
      {
    	  tcp_output(tpcb);
    	  es->TxPacketIndex = 0;
      }
      ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @param  None
  * @retval None
  */
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_st *es;

	LWIP_UNUSED_ARG(len);

	es = (struct tcp_server_st *)arg;
	es->retries = 0;
	if((es != NULL) && (es->TxPacketIndex > 0))
	{
		es->TxPacketIndex = 0;
	}
	/* if no more data to send and client closed connection*/
	if(es->state == ES_CLOSING)
	  tcp_server_connection_close(tpcb, es);

	return ERR_OK;
}


/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_st *es)
{

  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);

  /* close tcp connection */
  tcp_close(tpcb);
}
#endif /* TCP_SERVER */
#endif /* LWIP_TCP */

/**************************************************** END OF FILE ****************************************************/
