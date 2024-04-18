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

#ifndef TRANSFERUDP_H
#define TRANSFERUDP_H
#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


#define TRANSFER_BUFFER_SIZE	1024

//tamanho m�ximo do bloco de dados
#define TRANSFER_DATA_BUFFER_SIZE TRANSFER_BUFFER_SIZE-16-4 //20 bytes do cabe�alho da mensagem que manda os dados tSliceDataMsg

typedef struct tRawReceivedMsg {
	unsigned char * buffer;
	int receivedSize;
} tRawReceivedMsg;

void tRawReceivedMsg_free(tRawReceivedMsg *);

typedef struct tTransferUDP {
	struct simple_udp_connection connection;
	int (*registerComm)(struct tTransferUDP *, int, int, void (callback)(struct simple_udp_connection *, const uip_ipaddr_t *, uint16_t , const uip_ipaddr_t *,  uint16_t ,  const uint8_t *, uint16_t));
	int (*send)(struct tTransferUDP *, unsigned char *, unsigned int);
} tTransferUDP;

tTransferUDP * tTransferUDP_create();

void tTransferUDP_destroy(tTransferUDP* server);

#endif
