/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "/home/user/WIMA/samples/Contiki/client/include/TransferUDP.h"
#include "/home/user/WIMA/samples/Contiki/client/include/AppProtocol.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

// Types of images format
typedef enum {
	WAITING_HANDSHAKE,
	WAITING_HEADER,
	WAITING_DATA
} tServerStates;

tServerStates serverState;

tTransferUDP * udpComm;

tAppProtocol * protocol ;



/*---------------------------------------------------------------------------*/
static void udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen) {
	
	//transforma em raw message pra depois usar no protocolo
	tRawReceivedMsg * msg = (tRawReceivedMsg*) malloc (sizeof(tRawReceivedMsg));
	msg->buffer = (unsigned char *) data;
	msg->receivedSize = datalen;
	
	//LOG_INFO("Received request '%.*s' from ", msg->receivedSize, (char *) msg->buffer);
	//LOG_INFO_6ADDR(msg->sender_addr);  --> *** remover este comment
	//LOG_INFO_("\n");
	/* send back the same string to the client as an echo reply */
	//LOG_INFO("Sending response.\n");

	if (serverState == WAITING_HANDSHAKE) {
		if (protocol->isHandShakeMsg(msg)) {
			LOG_INFO("Recebeu handshake...\n");
			LOG_INFO("Informa que recebeu...\n");
			udpComm->send(udpComm, (unsigned char *)protocol->encodeReplyOk(), sizeof(tReplyMsg));
		} else {
			LOG_INFO("Erro de protocolo, aguardando handshake...\n");
		}

	} else if (serverState == WAITING_HEADER) {

	} else if (serverState == WAITING_DATA) {

	} else {
		  LOG_INFO("Protocol error, received msg\n");
		  LOG_INFO_6ADDR(sender_addr);
	}
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
	PROCESS_BEGIN();

	/* Initialize DAG root */
	NETSTACK_ROUTING.root_start(); 

	udpComm = tTransferUDP_create();
	
	protocol =	tAppProtocol_create();
	
	udpComm->registerComm(udpComm,UDP_SERVER_PORT, UDP_CLIENT_PORT);
	


	serverState = WAITING_HANDSHAKE;

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
