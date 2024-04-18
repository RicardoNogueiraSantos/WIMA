#include "include/TransferUDP.h"

#define LOG_MODULE "Transfer UDP"
#define LOG_LEVEL LOG_LEVEL_INFO


struct tTransferUDP TransferUDP;

int tTransferUDP_send(tTransferUDP * comm, unsigned char * data, unsigned int size) {
	int ret = 0;
	int count = 0;
	uip_ipaddr_t dest_ipaddr;

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Send to DAG root */
      LOG_INFO("Sending request %u to ", count);
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO_("\n");
      simple_udp_sendto(comm->connection, data, size, &dest_ipaddr);
      count++;
    } else {
      LOG_INFO("Not reachable yet\n");
    }
	return count;
}

static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}

int tTransferUDP_register(tTransferUDP * UDPCom , int server_port, int client_port) {
  return simple_udp_register(UDPCom->connection, server_port, NULL, client_port, udp_rx_callback);
}

tTransferUDP * tTransferUDP_create() {
	tTransferUDP * transferserver = (tTransferUDP*) malloc (sizeof(tTransferUDP));
  transferserver->connection = (struct simple_udp_connection *) malloc (sizeof(struct simple_udp_connection)); 
	transferserver->registerComm = &tTransferUDP_register;
	transferserver->send = &tTransferUDP_send;
	
	return transferserver;
}

void tTransferUDP_destroy(tTransferUDP* server) {
	if (server != NULL) free(server);
}
