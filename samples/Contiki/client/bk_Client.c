//gcc -o client client.c commons.c
#include "TransferCommons.h"
#include "TransferClient.h"
#include "contiki.h"
#include "ImgApi.h"
#include  "DBImage_house.h"
#include "UdpStream.h"

/*---------------------------------------------------------------------------*/

// 250 Kbp/s ~ 2 ms by bytes + 50 ms to wait reply
#define  TM_REPLY_CMD(NB)  (20*NB*(((clock_time_t)CLOCK_SECOND)))/10000 + (50*((clock_time_t)CLOCK_SECOND))/1000

// 250 Kbp/s ~ 2 ms by bytes + 20 ms
#define  WAIT_TX_CMD(NB)  (25*NB*(((clock_time_t)CLOCK_SECOND)))/10000 + (20*((clock_time_t)CLOCK_SECOND))/1000

#define WAIT_TX_MSG(TMR,TIME_MS)  {  								\
	etimer_set(&TMR,TIME_MS);                  							\
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&TMR));   \
}



#define CLR_REC_BYTES() msg_rec_len=0;

#define UDP_CLIENT_PORT	   8765

// Image attributes
static tTransferSettings com_set;
// Image sync list
// Each bit means a block synchronized with server
// 0 -> Block not synchronized
// 1 -> Block synchronized

unsigned char img_sync_list[LEN_SYNC_BUF];
unsigned char img_sync_srv[LEN_SYNC_BUF];

unsigned char * img_buf;

unsigned int msg_rec_len;
unsigned char buff_rec[MAX_MSG_SIZE];

extern COM_SOCK * com_port;


#define ATTEMPT_COUNT 3

const unsigned char FULL_IMG_UPDATE_TYPE=UP_IMG_FULL_IN_BLOCKS;

void receive_com_data(unsigned char *data, int len);

PROCESS(img_transfer_client, "Image client");

PROCESS_THREAD(img_transfer_client, ev, data)
{
	
	static struct etimer msg_timer;
	static client_states state;
	int i;
	int received;
	unsigned int attempt;
	unsigned char *msg_img_ptr;
	unsigned char *sync_source=(unsigned char*)img_sync_list;
	static unsigned int count_msg,count_block,cmd_exit,cmd;
	char res;
	static Handshake_MSG handshake_msg;
	static Image_BLOCK_MSG img_blk_msg;
	static Image_FULL_MSG img_full_msg;
	static Image_SYNC_MSG img_sync_msg;
	static Image_CHKSYNC_MSG img_chksync_msg;
	static Image_CHKSYNC_REPLY_MSG img_s2c_reply_chksync;
	static block_server_update_states  block_update_state=SYNC_FULL_BLOCK_TO_SERVER;
	tComConfig ConfigCOM;

	PROCESS_BEGIN();
	
	printf("Transfer Client running \n");
	
	// Setup COM
	com_port = (COM_SOCK*) malloc(sizeof(COM_SOCK));
	ConfigCOM.port=UDP_CLIENT_PORT;
	ConfigCOM.service_id=190;
	ConfigCOM.address="127.0.0.1";
	ConfigCOM.com_type=UDP_CON;
	ConfigCOM.com_side=CLIENT_COM_SIDE;	
	com_port->rec_msg=receive_com_data;
	UDP_ComSetup(ConfigCOM,com_port);
		
	state=WAIT_NEW_COMMAND;

	
	while(1)
	{

		if (state==UP_NOTIFY_SERVER) 
		{
			// INIT State

			printf("state: INIT\n");
			//Prepare handshake message, update image characteristics and fragmentation
			handshake_msg.id = HANDSHAKE_MSG_ID;
			handshake_msg.img_rows = com_set.rows;
			handshake_msg.img_cols = com_set.cols;
			handshake_msg.block_rows = com_set.block_cols;
			handshake_msg.block_cols = com_set.block_rows;
			handshake_msg.blocK_size = com_set.blocK_size;
			handshake_msg.full_image_blocks=com_set.full_image_blocks;
			printf("sending HANDSHAKE message...\n");
			//send the message
			res=com_port->snd_msg(&handshake_msg,sizeof(Handshake_MSG));
			// Wait for reply 
			CLR_REC_BYTES() ;
			WAIT_TX_MSG(msg_timer,TM_REPLY_CMD(sizeof(Handshake_MSG))) ;	
			printf ("Tempo de espera em ms %d , número de bytes %d",TM_REPLY_CMD(sizeof(Handshake_MSG)),sizeof(Handshake_MSG));
			res= chk_reply(&handshake_msg,&buff_rec,sizeof(Handshake_MSG),msg_rec_len,'y' );					
			if (res>=0)
			{
					state = UP_IMG_FULL;
					printf("Resposta correta recebida    \n");
			}
			else
			{
					state = WAIT_NEW_COMMAND;
					printf("Resposta errada recebida    \n");
			}

		}
		else if(state==UP_IMG_FULL)
		{
			// UP_IMG_FULL State

			//começar a mandar as imagens
			printf("state: UP_IMG_FULL \n");

			//INCIALMENTE PRECISA MANDAR A MENSAGEM INTEIRA
			//Image_FULL_MSG img_full_msg;
			// Checks if is necessary fragmentation according to the message length
			// without fragmentation, limited to UPD packet size (65535 bytes)
			if (FULL_IMG_UPDATE_TYPE==UP_IMG_FULL_ONE_BLOCK){
				img_full_msg.id = IMAGE_FULL_MSG_ID;
				img_full_msg.type = IMG_FULL_UPDATE_TYPE_FULL;
				// Gets image data
				for (i=0;i<FULL_IMG_BUF;i++) 
				{
					img_full_msg.data[i]=img_buf[i];
					printf("sending IMAGE_FULL_MSG message...\n");
					//send the message
					res=com_port->snd_msg(&img_full_msg,sizeof(Image_FULL_MSG));
					WAIT_TX_MSG(msg_timer,TM_TX_CMD(sizeof(Handshake_MSG))) ;
					res= chk_reply(&handshake_msg,&buff_rec,sizeof(Handshake_MSG),res,'n' );		
					if (res==0) 
					{			
								// Todo code
								
					}else{
							printf("Failed receiving data, exiting...\n");
					}
				}
			}
			else
			{
				// Sends the message to start image blocks transmission, transmission will be done through blocks
				img_full_msg.id = IMAGE_FULL_MSG_ID;
				img_full_msg.type = IMG_FULL_UPDATE_TYPE_IN_FRAGS;
				res=com_port->snd_msg(&img_full_msg,sizeof(Image_FULL_MSG));	
				if (res==0) 
				{
					WAIT_TX_MSG(msg_timer,TM_REPLY_CMD(sizeof(Image_FULL_MSG))) ;
					res=chk_reply(&img_full_msg,buff_rec,sizeof(Image_FULL_MSG),res,'y' );
					if (res>=0){
						// Sets next state info
						block_update_state=SYNC_FULL_BLOCK_TO_SERVER;
						state=UP_FULL_IMG_BLOCK;								
					}
				}
				else{
					printf("Failled sending message \n");
				}
			}
		}
		else if (state==UP_FULL_IMG_BLOCK)
		{
			// UP_IMG_BLOCK State

			//começar a mandar as imagens
			printf("state: UP_IMG_BLOCK\n");
			// Checks the type blocl update

			//Image_BLOCK_MSG img_blk_msg;
			img_blk_msg.id = IMAGE_BLOCK_MSG_ID;
			img_blk_msg.type = BLOCK_TYPE_CONT;
			img_blk_msg.ini_blk=0;
			count_msg=0;
			// Gets the pointer to message data
			msg_img_ptr=&img_blk_msg.data[0];
			for (count_block=0;count_block<com_set.full_image_blocks;count_block++){
					//res=get_block_image((unsigned char *)img_buf,msg_img_ptr,com_set,count_block);
					res=get_block_image(img_buf,msg_img_ptr,com_set,count_block);
					msg_img_ptr+=com_set.blocK_size;
					count_msg++;
					// Checks if a new block will fit in the current message
					// otherwise transmits message with maximum blocks possible
					if ((com_set.blocK_size*(count_msg+1))>MAX_DATA_IMG_BLOCK_SIZE)
					{
						// Send the message, and is not necessary to wait for response
						img_blk_msg.nb_blk=count_msg;
						res=com_port->snd_msg(&img_blk_msg, sizeof(img_blk_msg));						
						if(res<0) 
						{
							printf("Failled sending message, exiting...\n");
						}
						WAIT_TX_MSG(msg_timer,WAIT_TX_CMD(sizeof(img_blk_msg)));
						msg_img_ptr=&img_blk_msg.data[0];
						img_blk_msg.ini_blk+=count_msg;
						count_msg=0;
						
					}
			}
			// checks for remainder , then transmits then
			if (count_msg>0){
				img_blk_msg.nb_blk=count_msg;
				res=com_port->snd_msg(&img_blk_msg, sizeof(img_blk_msg));
				if(res<0) 
				{
					printf("Failled sending message, exiting...\n");
				}

			}
			WAIT_TX_MSG(msg_timer,WAIT_TX_CMD(sizeof(img_blk_msg)));
			state=WAIT_NEW_COMMAND;
			// After have transmitted all image blocks, consider updated in server
			sync_list_full_set((unsigned char *)img_sync_list,LEN_SYNC_BUF);
		}
		else if (state==CLIENT2SERVER_SYNC)
		{
			// CLIENT2SERVER_SYNC State

			// Inits the process to update changed blocks, notify server the
			// blocks will be updated through sync list.
			//Image_SYNC_MSG img_sync_msg;
			img_sync_msg.id=IMAGE_SYNC_MSG_ID;
			img_sync_msg.sync_list_len=LEN_SYNC_BUF;
			for (i=0;i<LEN_SYNC_BUF;i++) img_sync_msg.sync_list[i]=img_sync_list[i];
			//res=msg_snd(udp,&img_sync_msg,(unsigned char *)buff,sizeof(Image_CHKSYNC_MSG),ATTEMPT_COUNT);
			if (res>=-2)
			{
				// Sets internal states to send changes blocks
				state=UP_IMG_BLOCK;
				block_update_state=SYNC_CHANGED_BLOCKS_TO_SERVER;
				sync_source=(unsigned char *)img_sync_list;
			}
		}
		else if  (state==CHECK_SERVER_SYNC)
		{
			// CHECK_SERVER_SYNC State
			//Image_CLT2SRV_CHKSYNC_MSG img_s2c_chksync;
			//Image_CLT2SRV_CHKSYNC_REPLY_MSG img_s2c_reply_chksync;
			img_chksync_msg.id=IMAGE_CHKSYNC_MSG_ID;
			// Send message
			//res=msg_snd(udp,&img_chksync_msg,(char *)buff,sizeof(Image_SYNC_MSG),ATTEMPT_COUNT);
			// Checks reply
			if (res>=0)
			{
				img_s2c_reply_chksync=decode_img_chksync_reply_msg(buff_rec);
				if (img_s2c_reply_chksync.status==REPLY_OK){
					printf("Server up to date \n");
					// Update sync list
					sync_list_full_set((unsigned char *) img_sync_list,LEN_SYNC_BUF);
					// Finishes process
					//state=ENTER_MANUAL_CMD;
				}else{
					// Saves sync list to update
					printf("Server out of date \n");
					for(i=0;i<LEN_SYNC_BUF;i++) img_sync_srv[i]=img_s2c_reply_chksync.sync_list[i];
					// Updates internal states to sync server
					sync_source=(unsigned char *)img_sync_srv;
					//state=ENTER_MANUAL_CMD;
				}

			}
		}
		else if  (state==WAIT_NEW_COMMAND)
		{
			// WAIT_NEW_COMMAND State

			printf("Aguardando evento \n");

			PROCESS_WAIT_EVENT_UNTIL(ev==PROCESS_EVENT_CONTINUE);
						
			state=UP_NOTIFY_SERVER;
		}
	}

	PROCESS_END();

}


void receive_com_data(unsigned char *data, int len)
{
unsigned int i;
	if (len<=MAX_MSG_SIZE)
	{
			for (i=0;i<len;i++)  buff_rec[i]=data[i];
			msg_rec_len=len;
	}
	else
	{
			for (i=0;i<MAX_MSG_SIZE;i++)  buff_rec[i]=data[i];
			msg_rec_len=MAX_MSG_SIZE;
	}
	process_post_synch(&img_transfer_client,PROCESS_EVENT_CONTINUE,NULL);		
	
	//printf("Data received transfer client \n");
    //printf("Length : %d \n", msg_rec_len);
    //log_msg_array ("Message contents",&buff_rec,msg_rec_len,'y');
}

void start_proc(void)
{
	process_start(&img_transfer_client,NULL);
}

void snd_img(void)
{
			// Seta aqui os dados para serem enviados, estou usando o array em  DBImage_house.h
			com_set.rows=128;
			com_set.cols=128;
			com_set.block_rows=8;
			com_set.block_cols=8;
			com_set.blocK_size=64;
			com_set.full_image_blocks=64;
			com_set.image_size=16384; 
			img_buf=&db_img_house;

	
	process_post_synch(&img_transfer_client,PROCESS_EVENT_CONTINUE,NULL);
}
