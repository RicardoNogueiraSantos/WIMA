/*
Copyright 2024 Ricardo Santos

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include "TransferUDP.h"
#define LOG_MODULE "TransferUDP"
#define LOG_LEVEL LOG_LEVEL_INFO

int tTransferUDP_send(tTransferUDP * comm, unsigned char * data, unsigned int size) {
	int ret = 0;
	int count = 0;
	uip_ipaddr_t dest_ipaddr;

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Send to DAG root */
      LOG_INFO("Sending request %u to ", count);
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO_("\n");
      simple_udp_sendto(&comm->connection, data, size, &dest_ipaddr);
      count++;
    } else {
      LOG_INFO("Not reachable yet\n");
    }
	return count;
}

int tTransferUDP_register(tTransferUDP * comm, int server_port, int client_port, void (callback)(struct simple_udp_connection *, const uip_ipaddr_t *, uint16_t , const uip_ipaddr_t *,  uint16_t ,  const uint8_t *, uint16_t)) {
	return simple_udp_register(&comm->connection, server_port, NULL, client_port, callback);
}

tTransferUDP * tTransferUDP_create() {
	tTransferUDP * transferserver = (tTransferUDP*) malloc (sizeof(tTransferUDP));
	transferserver->registerComm = &tTransferUDP_register;
	transferserver->send = &tTransferUDP_send;
	
	return transferserver;
}

void tTransferUDP_destroy(tTransferUDP* server) {
	if (server != NULL) free(server);
}
