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

#include "net/ipv6/simple-udp.h"
#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "/home/user/WIMA/samples/Contiki/client/include/ImageDB.h"
#include "/home/user/WIMA/samples/Contiki/client/include/TransferUDP.h"

#define LOG_MODULE "Client "
#define LOG_LEVEL LOG_LEVEL_INFO



//#include "include/TransferClient.h"

//#include "/home/user/WIMA/samples/Contiki/include/DBImage_house.h"


/*---------------------------------------------------------------------------*/



// 100 ms interval for communication tasks
#define COM_INTERVAL (CLOCK_SECOND)/10  


#define UDP_CLIENT_PORT 100
#define UDP_SERVER_PORT 100




//unsigned char img_sync_list[LEN_SYNC_BUF];
//unsigned char img_sync_srv[LEN_SYNC_BUF];

//unsigned char * img_buf;

//unsigned int msg_rec_len;
//unsigned char buff_rec[MAX_MSG_SIZE];


/*---------------------------------------------------------------------------*/
PROCESS(wima_client_process, "WIMA client");
AUTOSTART_PROCESSES(&wima_client_process);
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(wima_client_process, ev, data)
{

	static struct etimer periodic_timer;
	int conn_sts;


	//simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
    //                  UDP_SERVER_PORT, udp_rx_callback);

	PROCESS_BEGIN();
	
	LOG_INFO("WIMA sample code\n");
	LOG_INFO("Transfers an image from client to server\n");

	// Initializes tImageDb, to get images to be transfered 
	struct tImageDb * DBImage=tImageDb_create();

	// Initializes network
	
	tTransferUDP  * UDPTransfer=tTransferUDP_create();

	UDPTransfer->registerComm(UDPTransfer,UDP_CLIENT_PORT,UDP_SERVER_PORT);


	while(1)
	{
		// Contiki is a non preemptive OS, so the strategy to manage the communication will be call
		// communication task every 100 ms

		 etimer_set(&periodic_timer, COM_INTERVAL);
         PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		 LOG_INFO("Communication proccess\n");


	}

	PROCESS_END();

}

