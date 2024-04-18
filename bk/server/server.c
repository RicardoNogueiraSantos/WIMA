//gcc -o server server.c commons.c

#include "server.h"
#include <time.h>

//Image_Client_Data Client_img_data[MAX_CLIENTS];

Image_Client_Data Client_img_data;

#define MSG_RETRY_COUNT 3

#define LOG_REC_DATA 'n'

#define LOG_IMG_REC  'y'

char check_sync_status(Image_Client_Data * img_data);


int process_handshake_message(Handshake_MSG msg) {
unsigned long img_size,i;
	//process message
	Client_img_data.attr.cols=msg.img_cols;
	Client_img_data.attr.rows=msg.img_rows;
	Client_img_data.attr.block_cols=msg.block_cols;
	Client_img_data.attr.block_rows=msg.block_rows;
	Client_img_data.attr.full_image_blocks=msg.full_image_blocks;
	Client_img_data.attr.blocK_size=msg.blocK_size;
	Client_img_data.attr.image_size=msg.image_size;

	// Checks attributes
	if ((Client_img_data.attr.cols>0) && (Client_img_data.attr.rows>0) &&
		(Client_img_data.attr.block_cols>0) && (Client_img_data.attr.block_rows>0)	)
	{
		// Image data allocation
		img_size=Client_img_data.attr.rows*Client_img_data.attr.cols;
		//free(Client_img_data.pixel_data);
		free(Client_img_data.Image_sync_list);
		Client_img_data.pixel_data=(unsigned char *)malloc(img_size*(sizeof (unsigned char)));
		Client_img_data.sync_list_len=Client_img_data.attr.full_image_blocks/8;
		Client_img_data.Image_sync_list=(unsigned char *)malloc(Client_img_data.sync_list_len*(sizeof (unsigned char)));
		for (i=0;i<img_size;i++){
			Client_img_data.pixel_data[i]=0x00;
		}
		sync_list_full_reset(Client_img_data.Image_sync_list,Client_img_data.sync_list_len);
		Client_img_data.init=1;
		return 1;
	}
	else
	{
		//Client_img_data.pixel_data=NULL;
		Client_img_data.init=0;
		return -1;
	}
	return 0;
}

int process_img_full_message(Image_FULL_MSG msg) {

	//TODO
	unsigned long i,img_size;

	// Checks if client is initialized
	if (Client_img_data.init==1){

		// Gets the total image length
		// Checks if image was received or will sent in block by the server
		switch (msg.type){

			// Full image was received in the message
			case IMG_FULL_UPDATE_TYPE_FULL:

				img_size=Client_img_data.attr.rows*Client_img_data.attr.cols;
				for (i=0;i<img_size;i++) Client_img_data.pixel_data[i]=msg.data[i];
				break;

			// Message will be updated by fragments
			case IMG_FULL_UPDATE_TYPE_IN_FRAGS:
				sync_list_full_reset(Client_img_data.Image_sync_list,Client_img_data.sync_list_len);
				//Client_img_data.time_rec=clock();
				//Client_img_data.pending_img=1;
				break;
			default :
				break;
		}
	}


	return 1;	
}


int process_img_block_message(Image_BLOCK_MSG msg) {
unsigned int blk_nb,i;
unsigned char * ptr_src;

	//process message
	//TODO	
	if (msg.type==BLOCK_TYPE_CONT){
		ptr_src=&msg.data[0];
		for (i=msg.ini_blk;i<(msg.ini_blk+msg.nb_blk);i++){
			//server_set_block_image (ptr_src,&Client_img_data,i);
			(void)put_block_image (ptr_src,Client_img_data.pixel_data,Client_img_data.attr,i);
			ptr_src+=Client_img_data.attr.blocK_size;
			sync_list_set_blk(Client_img_data.Image_sync_list,i);
		}
	}else{
		ptr_src=&msg.data[0];
		for (i=0;i<msg.nb_blk;i++){
			blk_nb=ptr_src[0]+(ptr_src[1]<<8);
			ptr_src+=2;
			(void)put_block_image (ptr_src,Client_img_data.pixel_data,Client_img_data.attr,blk_nb);
			sync_list_set_blk(Client_img_data.Image_sync_list,blk_nb);
			ptr_src+=Client_img_data.attr.blocK_size;
		}
	}
	return 1;	
}

int process_img_sync_message(Image_SYNC_MSG img_sync_msg){

	for (int i=0;i<Client_img_data.sync_list_len;i++){
		Client_img_data.Image_sync_list[i]=img_sync_msg.sync_list[i];
	}
	return 0;

}

int process_img_chksync_message (void)
{
	if (check_all_blocks_received(Client_img_data.Image_sync_list,Client_img_data.sync_list_len)=='y'){
		return 0;
	}else{
		return -1;
	}
}

int main(void)
{
	CLIENTS clients[MAX_CLIENTS];
	
	unsigned char buff[MAX_MSG_SIZE];

	unsigned char client_count=1;


	UDP_SOCK * udp = Initialize_UDP_SERVER(SERVER, PORT);
	
	printf("Waiting connections in %s:%d\n", SERVER, PORT);	
	
	//keep listening for data
	while(1)
	{

		printf("Waiting for data...\n");
		// Checks receiving an image

		//printf("Input Q or q to exit \n");
		//key_cmd=fgetc();
		//if (key_cmd=='Q' || key_cmd=='q' )
		//{
		//	exit(1);
		//}

		//try to receive some data, this is a blocking call
		//struct sockaddr_in addr;
		int received = udp->wait_message(udp, buff, MAX_MSG_SIZE, 0);
		printf("%s:%d\n", inet_ntoa(udp->addr.sin_addr), ntohs(udp->addr.sin_port)); // id do cliente que tá mandando
		if (received == -1) {
			printf("Failed receiving data, exiting...\n");
			free(udp);
			exit(1);			
		}
		//output_array_msg("Message received",buff,received);
		if (received > 0) {
			printf("Received(%d) from %s:%d\n", buff[0], inet_ntoa(udp->addr.sin_addr), ntohs(udp->addr.sin_port));

			log_msg_array ("Bytes received -> :",buff,received,LOG_REC_DATA);

			switch(get_message_id(buff[0])) { //read first 4 bits
				case HANDSHAKE_MSG_ID:
					printf("HANDSHAKE_MSG received, return OK...\n");
					Handshake_MSG handshake_msg = decode_handshake_message (buff);
					dump_handshake_message(handshake_msg);
					
					Handshake_REPLY_MSG hsreply_msg;
					hsreply_msg.id = HANDSHAKE_REPLY_MSG_ID;

					if(process_handshake_message(handshake_msg) > 0) {
						hsreply_msg.status = REPLY_OK;						
					} else {
						hsreply_msg.status = REPLY_FAIL;						
					}
					if (udp->send_message(udp, &hsreply_msg, sizeof(struct Handshake_REPLY_MSG))==-1) {
						printf("Failled sending message, exiting...\n");
						exit(1);
					}
					break;
					
				case IMAGE_FULL_MSG_ID:
					printf("IMAGE_FULL_MSG received\n");
					Image_FULL_MSG img_full_msg = decode_img_full_message((unsigned char *)buff);
					dump_img_full_message(img_full_msg);
					Image_FULL_REPLY_MSG ifreply_msg;
					ifreply_msg.id = IMAGE_FULL_REPLY_MSG_ID;

					if(process_img_full_message(img_full_msg) > 0) {
						ifreply_msg.status = REPLY_OK;						
					} else {
						ifreply_msg.status = REPLY_FAIL;						
					}
					if (img_full_msg.type==IMG_FULL_UPDATE_TYPE_FULL) log_msg_matrix("Received image",Client_img_data.pixel_data,Client_img_data.attr.rows,Client_img_data.attr.cols,'y');
					if (udp->send_message(udp, &ifreply_msg, sizeof(struct Image_FULL_REPLY_MSG))==-1) {
						printf("Failled sending message, exiting...\n");
						exit(1);
					}

					break;
					
				case IMAGE_BLOCK_MSG_ID:
					printf("IMAGE_BLOCK_MSG received\n");
					Image_BLOCK_MSG img_block_msg = decode_img_block_message ((unsigned char *)buff);
					
					//dump_img_block_message(img_block_msg);

					//log_msg_array("Block received :",buff,sizeof(img_block_msg),'y');
					Image_BLOCK_REPLY_MSG ibreply_msg;
					ibreply_msg.id = IMAGE_BLOCK_REPLY_MSG_ID;

					process_img_block_message(img_block_msg);

					if(process_img_block_message(img_block_msg) > 0) {
						ibreply_msg.status = REPLY_OK;
					} else {
						ibreply_msg.status = REPLY_FAIL;
					}
					#ifdef BLOCK_MSG_REPLY_REQ
						if (udp->send_message(udp, &ibreply_msg, sizeof(struct Image_BLOCK_REPLY_MSG))==-1) {
							printf("Failled sending message, exiting...\n");
							exit(1);
						}
					#endif
					// Checks for pending images
					char res=check_sync_status(&Client_img_data);
					if (res=='c')
					{
						log_msg_matrix ("Image received",Client_img_data.pixel_data,Client_img_data.attr.cols, Client_img_data.attr.rows,LOG_IMG_REC);
					}


					break;
					
				case IMAGE_SYNC_MSG_ID:
					printf("IMAGE_SYNC_MSG received\n");
					Image_SYNC_MSG img_sync_msg = decode_img_sync_message ((unsigned char *)buff);
					Image_SYNC_REPLY_MSG sync_rep_msg;


					if( process_img_sync_message(img_sync_msg) > 0) {
						sync_rep_msg.status = REPLY_OK;
					} else {
						sync_rep_msg.status = REPLY_FAIL;
					}
					sync_rep_msg.id=IMAGE_SYNC_REPLY_MSG_ID;
					log_msg_array("Sync Replay : ",&sync_rep_msg,10,'y');
					if (udp->send_message(udp, &sync_rep_msg, sizeof(struct Image_SYNC_REPLY_MSG))==-1) {
							printf("Failled sending message, exiting...\n");
							exit(1);
					}
					break;


				case IMAGE_CHKSYNC_MSG_ID:
					printf("IMAGE_CHKSYNC_MSG received\n");
					Image_CHKSYNC_MSG img_chksync_msg = decode_img_chksync_message ((unsigned char *)buff);
					Image_CHKSYNC_REPLY_MSG chksync_rep_msg;


					if( process_img_chksync_message() > 0) {
						chksync_rep_msg.status = REPLY_OK;
					} else {
						chksync_rep_msg.status = REPLY_FAIL;
						for (int i=0;i<Client_img_data.sync_list_len;i++){
							chksync_rep_msg.sync_list[i]=Client_img_data.pixel_data[i];
						}
					}
					chksync_rep_msg.id=IMAGE_CHKSYNC_REPLY_MSG_ID;
					//log_msg_array("Sync Replay : ",&sync_rep_msg,10,'y');
					if (udp->send_message(udp, &chksync_rep_msg, sizeof(struct Image_CHKSYNC_REPLY_MSG))==-1) {
							printf("Failled sending message, exiting...\n");
							exit(1);
					}

					break;

				default:
					printf("Unknonw message: %d\n", buff[0]);				
			}
		}
	}
	
	udp->close_sock(udp);
	free(udp);
	return 0;
}


// Output
// 't' time-out occurred
// 'n' no time-out
// 'c' image received
char check_sync_status(Image_Client_Data * img_data){
char res='n';
	// Checks all blocks were received
	if (check_all_blocks_received(img_data->Image_sync_list,Client_img_data.sync_list_len)=='y')
	{
		// All image blocks received
		res='c';
	}
	else{
			//Checks time-out to received
			res='n';
	}
	return res;
}
