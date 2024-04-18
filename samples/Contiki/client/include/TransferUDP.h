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
	struct simple_udp_connection * connection;
	int (*registerComm)(struct tTransferUDP *, int, int);
	int (*send)(struct tTransferUDP *, unsigned char *, unsigned int);
} tTransferUDP;

tTransferUDP * tTransferUDP_create();

void tTransferUDP_destroy(tTransferUDP* server);

#endif
