#include "contiki.h"
#include "net/routing/routing.h"
//#include "random.h"  --> *** remover este comment
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

#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

tTransferUDP * udpComm;
tAppProtocol * protocol;
tSlicer * slicer;

// Types of images format
typedef enum {
	WAITING_HANDSHAKE_ACK,
	WAITING_HEADER_ACK,
	WAITING_DATA_ACK
} tClientStates;

tClientStates clientState;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
	//transforma em raw message pra depois usar no protocolo
	tRawReceivedMsg * msg = (tRawReceivedMsg*) malloc (sizeof(tRawReceivedMsg));
	msg->buffer = (unsigned char *) data;
	msg->receivedSize = datalen;
	if (clientState == WAITING_HANDSHAKE_ACK) {
		if (protocol->isHandShakeReply(msg)) {
			LOG_INFO("Recebeu handshake ACK...\n");
		} else {
			LOG_INFO("Erro de protocolo, aguardando handshake...\n");
		}
	
	} else if (clientState == WAITING_HEADER_ACK) {

	} else if (clientState == WAITING_DATA_ACK) {

	} else {
		  LOG_INFO("Protocol error, received msg\n");
		  LOG_INFO_6ADDR(sender_addr);
	}
#if LLSEC802154_CONF_ENABLED
  //LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  //LOG_INFO_("\n");

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static char str[32];

  PROCESS_BEGIN();
	printf("iniciando...\n");

  /* Initialize UDP connection */
	udpComm = tTransferUDP_create();
	
	if (udpComm != NULL) {
		udpComm->registerComm(udpComm,UDP_SERVER_PORT, UDP_CLIENT_PORT);

		//Create protocol
		tAppProtocol * protocol = tAppProtocol_create();

		//Create slicer
		tSlicer * slicer = tSlicer_create();

	  //etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL); --> *** remover este comment
	  while(1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

		udpComm->send(udpComm, (unsigned char*)protocol->encodeHandShakeMsg(), sizeof(tHandShakeMsg));

		clientState = WAITING_HANDSHAKE_ACK;

		/* Add some jitter */
		//etimer_set(&periodic_timer, SEND_INTERVAL
		//  - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND))); --> *** remover este comment
	  }
	} else {
	  		printf("Error creating transferUDP!!!\n");
	  		return 1;
	}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
