//gcc -o client client.c commons.c

#include "client.h"
#include "test_cases.h"
#include <time.h>


// Image memory buffer
unsigned char img_buf[FULL_IMG_BUF];
// Image attributes
Image_Attributs atr_img;
// Image sync list
// Each bit means a block synchronized with server
// 0 -> Block not synchronized
// 1 -> Block synchronized
unsigned char img_sync_list[LEN_SYNC_BUF];
unsigned char img_sync_srv[LEN_SYNC_BUF];


#define ATTEMPT_COUNT 3


const unsigned char FULL_IMG_UPDATE_TYPE=UP_IMG_FULL_IN_BLOCKS;


int main(void)
{
	unsigned char *msg_img_ptr;
	unsigned char *sync_source=(unsigned char*)img_sync_list;
	unsigned int count_msg,count_block,cmd_exit,cmd;
	char res;
	unsigned char buff[BUFLEN];

	Image_SYNC_MSG img_sync_msg;
	Image_CHKSYNC_MSG img_chksync_msg;
	Image_CHKSYNC_REPLY_MSG img_s2c_reply_chksync;

	client_states state = INIT;
	block_server_update_states  block_update_state=SYNC_FULL_BLOCK_TO_SERVER;

	UDP_SOCK * udp = Initialize_UDP_CLIENT(SERVER, PORT);
	printf("Connected to %s:%d\n", SERVER, PORT);
	load_img((unsigned char *)img_buf);
	// Image attributes
	atr_img.cols=IMG_COLS;
	atr_img.rows=IMG_ROWS;
	atr_img.block_cols=IMG_BLK_COLS;
	atr_img.block_rows=IMG_BLK_ROWS;
	atr_img.full_image_blocks=IMG_NB_BLK;
	atr_img.blocK_size=IMG_BLK_COLS*IMG_BLK_ROWS;
	atr_img.image_size=IMG_COLS*IMG_ROWS;
	// state=get_expected_reply(IMAGE_SYNC_MSG_ID);
	while(1)
	{
		switch (state) {
			case INIT:
				printf("state: INIT\n");
				//Prepare handshake message, update image characteristics and
				// fragmentation
				Handshake_MSG handshake_msg;
				handshake_msg.id = HANDSHAKE_MSG_ID;
				handshake_msg.img_rows = atr_img.rows;
				handshake_msg.img_cols = atr_img.cols;
				handshake_msg.block_rows = atr_img.block_cols;
				handshake_msg.block_cols = atr_img.block_rows;
				handshake_msg.blocK_size = atr_img.blocK_size;
				handshake_msg.full_image_blocks=atr_img.full_image_blocks;
				printf("sending HANDSHAKE message...\n");
				//send the message
				//snd_udp_msg(udp,&handshake_msg,sizeof(Handshake_MSG));
				//char send_udp_msg(UDP_SOCK * udp,char *buff_rec,char *buff_snd,unsigned int len)
				res=msg_snd(udp,&handshake_msg,(unsigned char *)buff,sizeof(handshake_msg),ATTEMPT_COUNT);
				if (res>=0){
					state = UP_IMG_FULL;
				}else{

					state = UP_IMG_FULL;
					//state = ENTER_MANUAL_CMD;
				}


				//if (udp->send_message(udp, &handshake_msg, sizeof(Handshake_MSG))==-1) {
				//	printf("Failled sending message, exiting...\n");
				//	exit(1);
				//}

				//printf("Waiting response...\n");
				//int received = udp->wait_message(udp, buff, MAX_MSG_SIZE, 100);
				//if (received == -1) {
				//	printf("Failed receiving data, exiting...\n");
				//	free(udp);
				//	exit(1);
				//}
				//printf("Message received\n");
				//if (received > 0) {
				//	if (get_message_id(buff[0]) == HANDSHAKE_REPLY_MSG_ID) {
				//		printf("Handshake response received\n");
				//		state = UP_IMG_FULL;
				//	} else {
				//		printf("Unknow message(%d). Protocol fail, exiting...\n", buff[0]);
				//		free(udp);
				//		exit(1);
				//	}
				//} else {
				//	printf("Received message is empty, exiting...\n");
				//	free(udp);
				//	exit(1);
				//}
				break;

			case UP_IMG_FULL:
				//começar a mandar as imagens
				printf("state: UP_IMG_FULL \n");

				//INCIALMENTE PRECISA MANDAR A MENSAGEM INTEIRA
				Image_FULL_MSG img_full_msg;
				// Checks if is necessary fragmentation according to the message length
				// without fragmentation, limited to UPD packet size (65535 bytes)
				if (FULL_IMG_UPDATE_TYPE==UP_IMG_FULL_ONE_BLOCK){
					img_full_msg.id = IMAGE_FULL_MSG_ID;
					img_full_msg.type = IMG_FULL_UPDATE_TYPE_FULL;
					// Gets image data
					for (int i=0;i<FULL_IMG_BUF;i++) img_full_msg.data[i]=img_buf[i];
					printf("sending IMAGE_FULL_MSG message...\n");
					//send the message
					if (udp->send_message(udp, &img_full_msg, sizeof(Image_FULL_MSG))==-1) {
						printf("Failled sending message, exiting...\n");
						exit(1);
					}
					printf("Waiting response...\n");
					int received = udp->wait_message(udp, buff, MAX_MSG_SIZE, 100);
					if (received == -1) {
						printf("Failed receiving data, exiting...\n");
						free(udp);
						exit(1);
					}
					printf("Message received\n");
					if (received > 0) {
						if (get_message_id(buff[0]) == IMAGE_FULL_REPLY_MSG_ID) {
							printf("Image_FULL_REPLY_MSG response received\n");
							state=ENTER_MANUAL_CMD;
						} else {
							printf("Unknow message(%d). Protocol fail, exiting...\n", buff[0]);
							free(udp);
							exit(1);
						}
					}
					else {
						printf("Received message is empty, exiting...\n");
						free(udp);
						exit(1);
					}
				}
				else{
					// Sends the message to start image blocks transmission
					// transmission will be done through blocks
					img_full_msg.id = IMAGE_FULL_MSG_ID;
					img_full_msg.type = IMG_FULL_UPDATE_TYPE_IN_FRAGS;
					if (udp->send_message(udp, &img_full_msg, sizeof(Image_FULL_MSG))==-1){
						printf("Failled sending message, exiting...\n");
						exit(1);
					}
					// Sets next state info
					block_update_state=SYNC_FULL_BLOCK_TO_SERVER;
					state=UP_IMG_BLOCK;
				}

				break;

			case UP_IMG_BLOCK:
				//começar a mandar as imagens
				printf("state: UP_IMG_BLOCK\n");
				// Checks the type blocl update

				if (block_update_state==SYNC_FULL_BLOCK_TO_SERVER){

					Image_BLOCK_MSG img_blk_msg;
					//unsigned char *img_ptr=0x00;
					//unsigned int count_data_msg;
					//unsigned int count_msg;
					//char res=0;
					img_blk_msg.id = IMAGE_BLOCK_MSG_ID;
					img_blk_msg.type = BLOCK_TYPE_CONT;
					img_blk_msg.ini_blk=0;
					count_msg=0;
					// Gets the pointer to message data
					msg_img_ptr=&img_blk_msg.data[0];
					for (count_block=0;count_block<IMG_NB_BLK;count_block++){
							res=get_block_image((unsigned char *)img_buf,msg_img_ptr,atr_img,count_block);
							msg_img_ptr+=IMG_BLK_SIZE;
							count_msg++;
							// Checks if a new block will fit in the current message
							// otherwise transmits message with maximum blocks possible
							if ((IMG_BLK_SIZE*(count_msg+1))>MAX_DATA_IMG_BLOCK_SIZE)
							{
								// Send the message, and is not necessary to wait for response
								img_blk_msg.nb_blk=count_msg;
								//log_msg_scalar("Initial block : ",img_blk_msg.ini_blk);
								//log_msg_scalar("Count block : ",img_blk_msg.nb_blk);
								//log_msg_array ("Image block sent :",&img_blk_msg,sizeof(img_blk_msg),'y');
								if (udp->send_message(udp, &img_blk_msg, sizeof(img_blk_msg))==-1)
								{
									printf("Failled sending message, exiting...\n");
									exit(1);
								}
								msg_img_ptr=&img_blk_msg.data[0];
								img_blk_msg.ini_blk+=count_msg;
								count_msg=0;
								delay(20);
							}
					}
					// checks for remainder , then transmits then
					if (count_msg>0){
						img_blk_msg.nb_blk=count_msg;
						udp->send_message(udp, &img_blk_msg, sizeof(img_blk_msg));

						//log_msg_scalar("Initial block1 : ",img_blk_msg.ini_blk);
						//log_msg_scalar("Count block1 : ",img_blk_msg.nb_blk);
						//log_msg_array ("Image block sent 1:",&img_blk_msg,sizeof(img_blk_msg),'y');
					}
					// After have transmitted all image blocks, consider updated in server
					sync_list_full_set((unsigned char *)img_sync_list,LEN_SYNC_BUF);
					state=ENTER_MANUAL_CMD;
					}else{
						Image_BLOCK_MSG img_blk_msg;
						// Updates only images blocks changed
						img_blk_msg.id = IMAGE_BLOCK_MSG_ID;
						img_blk_msg.type = IMG_FULL_UPDATE_TYPE_IN_FRAGS;
						img_blk_msg.ini_blk=0;
						count_msg=0;
						// Gets the number of blocks in each message
						msg_img_ptr=img_blk_msg.data;
						for (int count_block=0;count_block<IMG_NB_BLK;count_block++)
						{
							// Checks if the block needs to be update
							if (sync_list_check_blk((unsigned char *) img_sync_list,count_block)==0) {

								// Gets the number of the block to be transmitted
								// LSB
								msg_img_ptr[0]=(unsigned char)(0x00FF&count_block);
								// MSB
								msg_img_ptr[1]=(unsigned char)(0x00FF&(count_block>>8));
								msg_img_ptr+=2;
								// Gets the pixel
								res=get_block_image((unsigned char *)img_buf,(unsigned char *)msg_img_ptr,atr_img,count_block);
								// Adjust pointer to next element
								msg_img_ptr+=IMG_BLK_SIZE;
								count_msg++;
								// Checks if a new block will fit in the current message
								// otherwise transmits message with maximum blocks possible
								if (((IMG_BLK_SIZE+2)*(count_msg+1))>MAX_DATA_IMG_BLOCK_SIZE)
								{
									// Send the message, and is not necessary to wait for response
									img_blk_msg.nb_blk=count_msg;
									res=msg_snd(udp,&img_blk_msg,buff,sizeof(img_blk_msg),0);
									msg_img_ptr=img_blk_msg.data;
									count_msg=0;
									delay(20);
								}
							}
						}
						if (count_msg>0){
							img_blk_msg.nb_blk=count_msg;
							udp->send_message(udp, &img_blk_msg, sizeof(img_blk_msg));
						}

						state=ENTER_MANUAL_CMD;
					}
					break;

			case CLIENT2SERVER_SYNC:
				// Inits the process to update changed blocks, notify server the
				// blocks will be updated through sync list.
				//Image_SYNC_MSG img_sync_msg;
				img_sync_msg.id=IMAGE_SYNC_MSG_ID;
				img_sync_msg.sync_list_len=LEN_SYNC_BUF;
				for (int i=0;i<LEN_SYNC_BUF;i++) img_sync_msg.sync_list[i]=img_sync_list[i];
				res=msg_snd(udp,&img_sync_msg,(unsigned char *)buff,sizeof(Image_CHKSYNC_MSG),ATTEMPT_COUNT);
				if (res>=-2)
				{
					// Sets internal states to send changes blocks
					state=UP_IMG_BLOCK;
					block_update_state=SYNC_CHANGED_BLOCKS_TO_SERVER;
					sync_source=(unsigned char *)img_sync_list;
				}
				else{
					state=ENTER_MANUAL_CMD;
				}
				break;

			case CHECK_SERVER_SYNC:
				//Image_CLT2SRV_CHKSYNC_MSG img_s2c_chksync;
				//Image_CLT2SRV_CHKSYNC_REPLY_MSG img_s2c_reply_chksync;
				img_chksync_msg.id=IMAGE_CHKSYNC_MSG_ID;
				// Send message
				res=msg_snd(udp,&img_chksync_msg,(char *)buff,sizeof(Image_SYNC_MSG),ATTEMPT_COUNT);
				// Checks reply
				if (res>=0){
					img_s2c_reply_chksync=decode_img_chksync_reply_msg(buff);
					if (img_s2c_reply_chksync.status==REPLY_OK){
						printf("Server up to date \n");
						// Update sync list
						sync_list_full_set((unsigned char *) img_sync_list,LEN_SYNC_BUF);
						// Finishes process
						state=ENTER_MANUAL_CMD;
					}else{
						// Saves sync list to update
						printf("Server out of date \n");
						for(int i=0;i<LEN_SYNC_BUF;i++) img_sync_srv[i]=img_s2c_reply_chksync.sync_list[i];
						// Updates internal states to sync server
						sync_source=(unsigned char *)img_sync_srv;
						state=ENTER_MANUAL_CMD;
					}

				}

				break;

			case ENTER_MANUAL_CMD:
				cmd_exit=0;
				cmd=0;
				while (!cmd_exit){
					printf("Insert the command \n");
					printf("1: Re-Init \n");
					printf("2: Update full image on the server \n");
					printf("3: Update changed blocks on the server \n");
					printf("4: Check server sync \n");
					printf("7: Displays test image and sync list  \n");
					printf("8: Emulated changed blocks \n");
					printf("9: Finish \n");
					printf(" \n");
					scanf("%d", &cmd);
					switch (cmd){
						case 1: state=INIT;cmd_exit=1;break;
						case 2: state=UP_IMG_FULL;cmd_exit=1;break;
						case 3: state=CLIENT2SERVER_SYNC;cmd_exit=1; break;
						case 4: state=CHECK_SERVER_SYNC;cmd_exit=1; break;
						case 7:
								log_msg_matrix ("Image loaded :",(unsigned char *) img_buf,atr_img.cols,atr_img.rows,'y');
								log_msg_array ("Sync list :",(unsigned char *) img_sync_list,LEN_SYNC_BUF,'y');
								state=ENTER_MANUAL_CMD;
								cmd_exit=0;
							break;
						case 8:
								emulated_chg_blocks((unsigned char *)img_buf,(unsigned char *)img_sync_list,atr_img);
								state=ENTER_MANUAL_CMD;
								cmd_exit=0;
							break;
						case 9: state=END_APL;cmd_exit=1;break;
						default :
							cmd_exit=0;
							printf("Invalid command, please retry \n");
							break;
					}
				}
				break;

			default:
				if (state==END_APL){
					printf("Exiting application...\n");
					exit(0);

				}
				else{
					printf("Unknown state (%d), exiting...\n", state);
					exit(1);
				}
				break;
		}

	}

	udp->close_sock(udp);
	free(udp);
	return 0;
}

// An image of MxN pixels consists of an array in this way
// A[0] ->    P0,0 P0,1........P0,M-1
// A[M] ->    P1,0 P1,1........P1,M-1
// .................................
// A[M*(N-1)] PN,0 PN,1........PN,M-1
// A block of X,Y (for instance 2x) then consists
// P0,0 P01,1
// P1,0 P1,1
/*
unsigned int client_get_block_image(unsigned int blk_nb, unsigned char *buf_msg)
{
unsigned int row;
unsigned int col;
unsigned int buf_idx;
unsigned int line_blk;
unsigned int nb_blk_col;
unsigned int col_blk;
unsigned long int img_idx;

	if (blk_nb>=IMG_NB_BLK)	{
		// Return error, blocks out of the limits
		return -1;
	}

	// Columns of Blocks
	nb_blk_col=IMG_COLS/IMG_BLK_COLS;
	// Gets the block line
	line_blk=blk_nb/nb_blk_col;
	// Gets the block columns
	col_blk=blk_nb-(line_blk*nb_blk_col);

	img_idx=col_blk*IMG_BLK_COLS+(line_blk*IMG_BLK_ROWS*IMG_COLS);
	buf_idx=0;
	for (row=0;row<IMG_BLK_ROWS;row++){
		for (col=0;col<IMG_BLK_COLS;col++){
			buf_msg[buf_idx]=img_buf[img_idx];
			buf_idx++;
			img_idx++;
		}
		// Index position in the next line
		img_idx+=(IMG_COLS-IMG_BLK_COLS);
	}

	return 1;
}*/

