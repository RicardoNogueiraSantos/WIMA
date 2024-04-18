//gcc -o client client.c commons.c
#include "commons.h"

int send_message(UDP_SOCK *self, void * msg, int size) {
	//send the message
	return sendto(self->sock, msg, size , 0 , (struct sockaddr *) &(self->addr), sizeof(self->addr));
}

/* int send_message_to(UDP_SOCK *self, void * msg, int size, struct sockaddr_in addr) {
	//send the message
	return sendto(self->sock, msg, size , 0 , (struct sockaddr *) &addr, sizeof(addr));
} */

int wait_message(UDP_SOCK *self, char * buf, int buf_size, int timeout) { // timeout in miliseconds--> 0 no timeout
	struct timeval tv;
	
	if (timeout > 0) {
		tv.tv_sec = 0;
		tv.tv_usec = timeout*1000;	

		if (setsockopt(self->sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
			fprintf(stderr, "Failled to set timeout\n");
			return -2;
		}
	}

	memset(buf,'\0', buf_size);
	//try to receive some data, this is a blocking call
	int slen = sizeof(self->addr);
	return recvfrom(self->sock, buf, buf_size, 0, (struct sockaddr *) &(self->addr), &slen);
}

/* int wait_message_from(UDP_SOCK *self, char * buf, int buf_size, int timeout, struct sockaddr_in addr) { // timeout in miliseconds--> 0 no timeout
	struct timeval tv;
	
	if (timeout > 0) {
		tv.tv_sec = 0;
		tv.tv_usec = timeout*1000;	

		if (setsockopt(self->sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
			fprintf(stderr, "Failled to set timeout\n");
			return -2;
		}
	}

	memset(buf,'\0', buf_size);
	//try to receive some data, this is a blocking call
	int slen = sizeof(addr);
	int ret = recvfrom(self->sock, buf, buf_size, 0, (struct sockaddr *) &addr, &slen);
	//printf("%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	return ret;	
} */

void close_sock(UDP_SOCK *self) {
	//send the message
	close(self->sock);
}

int bind_sock(UDP_SOCK *self) {
	//bind socket to port
	return bind(self->sock , (struct sockaddr*)&(self->addr), sizeof(self->addr));
}

int get_peer_id(char * str, struct sockaddr_in addr) {
		//print details of the client/peer and the data received
		return sprintf(str, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}


UDP_SOCK * Initialize_UDP_CLIENT(char * server_host, int server_port) {
	UDP_SOCK * udp = malloc(sizeof(UDP_SOCK));
	
	if ( (udp->sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		fprintf(stderr, "Cannot create socket, exiting...\n");
		free(udp);
		exit(1);
	}
	
	
	memset((char *) &(udp->addr), 0, sizeof(udp->addr));
	udp->addr.sin_family = AF_INET;
	udp->addr.sin_port = htons(server_port);
	
	if (inet_aton(server_host, &(udp->addr.sin_addr)) == 0) {
		fprintf(stderr, "inet_aton() failed, exiting...\n");
		free(udp);
		exit(1);
	}
	
	udp->send_message = &send_message;
	udp->wait_message = &wait_message;
	//udp->send_message_to = &send_message_to;
	//udp->wait_message_from = &wait_message_from;
	udp->close_sock = &close_sock;
	udp->bind_sock = &bind_sock;
	
	return udp;
}

UDP_SOCK * Initialize_UDP_SERVER(char * server_host, int server_port) {
	UDP_SOCK * udp = Initialize_UDP_CLIENT(server_host, server_port);
	
	if (udp->bind_sock(udp) == -1) {
		fprintf(stderr, "Failled to bind to the port, exiting...\n");
		free(udp);
		exit(1);		
	}
	
	return udp;
}


unsigned char get_message_id(unsigned char msg) {
	int ret = -1;
	
	if (msg) {
		ret = msg&0x0F; //take first 4 bits
	}
	
	return ret;
}

//------ DECODE FUNCTIONS

Handshake_MSG decode_handshake_message (unsigned char * buff){
	Handshake_MSG msg;
	Handshake_MSG * rec_msg=(Handshake_MSG *)buff;
	//msg.id = buff[0]&0x0F;
	//process message
	msg.id=rec_msg->id;
	msg.img_cols=rec_msg->img_cols;
	msg.img_rows=rec_msg->img_rows;
	msg.block_cols=rec_msg->block_cols;
	msg.block_rows=rec_msg->block_rows;
	msg.full_image_blocks=rec_msg->full_image_blocks;
	msg.blocK_size=rec_msg->blocK_size;
	msg.image_size=rec_msg->image_size;
	return msg;
}

Handshake_REPLY_MSG decode_handshake_reply_message (unsigned char * buff){
	//TODO
	Handshake_REPLY_MSG msg;
	return msg;
}

Image_FULL_MSG decode_img_full_message (unsigned char * buff){
	//TODO
	Image_FULL_MSG msg;
	Image_FULL_MSG * rec_msg = (Image_FULL_MSG *)buff;
	msg.id=rec_msg->id;
	msg.initial_blk=rec_msg->initial_blk;
	msg.nb_of_blks=rec_msg->nb_of_blks;
	for (int i=0;i<FULL_MSG_DATA_LEN;i++) msg.data[i]=rec_msg->data[i];
	msg.type=rec_msg->type;
	return msg;
}

Image_FULL_REPLY_MSG decode_img_full_reply_message (unsigned char * buff){
	//TODO
	Image_FULL_REPLY_MSG msg;
	return msg;
}

Image_BLOCK_MSG decode_img_block_message (unsigned char * buff){

	//TODO
	Image_BLOCK_MSG msg;
	Image_BLOCK_MSG * rec_msg = (Image_BLOCK_MSG *)buff;
	msg.type=rec_msg->type;
	msg.ini_blk=rec_msg->ini_blk;
	msg.nb_blk=rec_msg->nb_blk;
	for (int i=0;i<MAX_DATA_IMG_BLOCK_SIZE;i++){
		msg.data[i]=rec_msg->data[i];
	}
	return msg;
}


Image_BLOCK_REPLY_MSG decode_img_block_reply_message (unsigned char * buff){
	//TODO
	Image_BLOCK_REPLY_MSG msg;
	return msg;	
}

Image_CHKSYNC_REPLY_MSG decode_img_chksync_reply_msg (unsigned char * buff){
	//TODO
	Image_CHKSYNC_REPLY_MSG msg;
	Image_CHKSYNC_REPLY_MSG * rec_msg=(Image_CHKSYNC_REPLY_MSG *)buff;
	msg.id=rec_msg->id;
	msg.id=rec_msg->status;
	msg.sync_list_len=rec_msg->sync_list_len;
	for (int i=0;i<msg.sync_list_len;i++) msg.sync_list[i]=rec_msg->sync_list[i];
	return msg;
}

Image_SYNC_MSG decode_img_sync_message (unsigned char *buff){

	Image_SYNC_MSG msg;
	Image_SYNC_MSG * rec_msg=(Image_SYNC_MSG *)buff;
	msg.id=rec_msg->id;
	msg.sync_list_len=rec_msg->sync_list_len;
	for (int i=0;i<MAX_DATA_IMG_BLOCK_SIZE;i++) msg.sync_list[i]=rec_msg->sync_list[i];

	return msg;

}

Image_CHKSYNC_MSG decode_img_chksync_message (unsigned char * buff)
{
	Image_CHKSYNC_MSG msg;
	Image_CHKSYNC_MSG  * rec_msg=(Image_CHKSYNC_MSG *)buff;
	msg.id=rec_msg->id;
	return msg;
}

//--------------- DUMP FUNCTIONS
void dump_handshake_message (Handshake_MSG msg){
	printf("Message id: HANDSHAKE_MSG_ID\n");
	//TODO - print other fields
}

void dump_handshake_reply_message (Handshake_REPLY_MSG msg){
	//TODO
}

void dump_img_full_message (Image_FULL_MSG msg){
	//TODO
}

void dump_img_full_reply_message (Image_FULL_REPLY_MSG msg){
	//TODO
}

void dump_img_block_message (Image_BLOCK_MSG msg){
	//TODO
}

void dump_img_block_reply_message (Image_BLOCK_REPLY_MSG msg){
	//TODO
}

// Other functions

char msg_snd(UDP_SOCK * udp,unsigned char * buff_snd,unsigned char * buff_rec,unsigned int len,unsigned char count_attempt )
{
unsigned char retry;
char error_cod;
	error_cod=0;
	retry=1;
	if (udp->send_message(udp,buff_snd,len)==-1){
		error_cod=-4;
		retry=0;
	}
	// Checks if response is expected
	if (count_attempt==0){
		error_cod=1;
		retry=0;
	}
	// Log message
	log_msg_id("Sending message : ",get_message_id(buff_snd[0]));
	log_msg_array("Bytes sent : ",buff_snd,len,'y');

	if (retry==0)
	{
		int received = udp->wait_message(udp, buff_rec, MAX_MSG_SIZE, 100);
	}
	while (retry)
	{
		int received = udp->wait_message(udp, buff_rec, MAX_MSG_SIZE, 100);
		log_msg_array("Bytes received : ",buff_rec,10,'y');
		if (received == -1) {
			if (udp->send_message(udp, buff_snd,len)==-1) {
				error_cod=-4;
				retry=0;
			}
			else{
				error_cod=-1;
				if (count_attempt>0){
					count_attempt--;
				}
				else retry=0;
			}
		}
		else{
			// Message received
			printf("Message received \n");
			if (received > 0) {
				if ( get_message_id(buff_rec[0]) == get_expected_reply(get_message_id(buff_snd[0]))  ) {
					error_cod=0;
					retry=0;
				}
				else{
					error_cod=-2;
					retry=0;
				}
			}
			else{
				error_cod=-2;
				if (count_attempt>0){
					count_attempt--;
				}
				else retry=0;
			}
		}
	}
	switch (error_cod)
	{
		case 1:
			printf("No response expected \n");
		break;

		case 0:
			log_msg_id("Response received : ",get_message_id(buff_rec[0]));
		break;

		case -1:
			printf("No message received \n");
		break;

		case -2:
			printf("Unknown message(%d) received. Protocol fail, exiting...\n", buff_rec[0]);
		break;

		case -3:
			printf("Received message is empty, exiting...\n");
		break;

		case -4:
			printf("Not possible to send an UDP message there was a socket error, please restart application, exiting...\n");
			free(udp);
			exit(1);
		break;

		default :

		break;


	}

	return error_cod;
}

unsigned char get_expected_reply(unsigned char msg_id)
{
unsigned char res;
	switch (msg_id){
		case HANDSHAKE_MSG_ID : res=HANDSHAKE_REPLY_MSG_ID; break;
		case	IMAGE_FULL_MSG_ID : res= IMAGE_FULL_REPLY_MSG_ID; break;
		case	IMAGE_BLOCK_MSG_ID : res=IMAGE_BLOCK_REPLY_MSG_ID; break;
		case	IMAGE_SYNC_MSG_ID : res=IMAGE_SYNC_REPLY_MSG_ID; break;
		case	IMAGE_CHKSYNC_MSG_ID : res=IMAGE_CHKSYNC_REPLY_MSG_ID; break;
		default : res=0xFF; break ;
	}
	return res;
}

void log_msg_matrix (char * msg, unsigned char * mat, unsigned int col, unsigned int row,char msg_log)
{
unsigned int x,y,idx;
	if (msg_log=='n') return;
	if (col==0 || row==0){
		puts("no data");
		puts(" ");
		return ;
	}
	idx=0;
	puts(msg);
	for (x=0;x<col;x++){
		for (y=0;y<row;y++){
			printf("%c", mat[idx++]);
		}
		printf("\n");
	}
	puts(" ");
}

void log_msg_id(unsigned char * msg, unsigned char msg_id)
{
char * nm_str;
char msg_str[100];

	switch (msg_id)
	{
		case HANDSHAKE_MSG_ID :
			nm_str="Handsahek message";
		break;
		case HANDSHAKE_REPLY_MSG_ID :
			nm_str="Handsahek message reply";
		break;
		case IMAGE_FULL_MSG_ID :
			nm_str="Full image message";
		break;
		case IMAGE_FULL_REPLY_MSG_ID :
			nm_str="Full image message reply";
		break;
		case IMAGE_BLOCK_MSG_ID :
			nm_str="Block image message";
		break;
		case IMAGE_BLOCK_REPLY_MSG_ID :
			nm_str="Block image message reply";
		break;
		case IMAGE_SYNC_MSG_ID :
			nm_str="Client to server sync image message";
		break;
		case IMAGE_SYNC_REPLY_MSG_ID :
			nm_str="Client to server sync image reply message reply";
		break;
		case IMAGE_CHKSYNC_MSG_ID :
			nm_str="Client to server check image sync message";
		break;
		case IMAGE_CHKSYNC_REPLY_MSG_ID :
			nm_str="Client to server check image sync message reply";
		break;
		default :
			nm_str="Unknown message";
		break;

	}
	strcpy(&msg_str[0],msg);
	strcpy(&msg_str[strlen(msg_str)],nm_str);
	strcpy(&msg_str[strlen(msg_str)]," \n");
	puts(" ");
	printf(msg_str);
	puts(" ");

}

void log_msg_array (char * msg, unsigned char * arr, unsigned int len,char msg_log)
{
unsigned int idx;
	if (msg_log=='n') return;
	if (len==0){
		puts("no data \n");
		puts(" ");
		return ;
	}
	puts(msg);
	for (idx=0;idx<len;idx++){
		printf("%X", arr[idx]);
		printf(" ");
	}
	puts(" \n");
}

void log_msg_scalar (char * msg, unsigned int scl)
{
	puts(msg);
	printf("%X", scl);
	printf(" ");
	puts(" \n");
}

char check_timeout(clock_t tm_rec,unsigned int tm)
{
    long tmout;
    clock_t now;

    tmout = tm*(CLOCKS_PER_SEC/1000);
    now = clock();
    if ( (now-tm_rec) < tmout )
    {
        return 'n';
    }
    else
    {
    	return 't';
    }
}

void sync_list_full_reset(unsigned char * img_data,unsigned int len)
{

	for (int i=0;i<len;i++)
	{
		img_data[i]=0x00;
	}
}

void sync_list_full_set(unsigned char * img_data,unsigned int len)
{

	for (int i=0;i<len;i++)
	{
		img_data[i]=0xFF;
	}
}

void sync_list_set_blk(unsigned char * sync_list, unsigned int blk_nb)
{
unsigned int idx;
unsigned int msk_idx;
unsigned char msk[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	// Gets byte index
	idx=(blk_nb/8);
	// Gets mask
	msk_idx=blk_nb-(8*idx);
	sync_list[idx]|=msk[msk_idx];
}

void sync_list_reset_blk(unsigned char * img_data, unsigned int blk_nb)
{
unsigned int idx;
unsigned int msk_idx;
unsigned char msk[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	// Gets byte index
	idx=(blk_nb/8);
	// Gets mask
	msk_idx=blk_nb-(8*idx);
	img_data[idx]&=(~msk[msk_idx]);
}

// Output :
// 'y' - yes all blocks received
// 'n' - no
char check_all_blocks_received(unsigned char *img_sync, unsigned int len)
{
	for (int i=0;i<len;i++){
		if (img_sync[i]!=0xFF)
		{
			return 'n';
		}
	}
	return 'y';
}

// Output :
// 0 - block changed
// 1 - block unchanged
unsigned char sync_list_check_blk(unsigned char * sync_list, unsigned int blk_nb)
{
unsigned int idx;
unsigned int msk_idx;
unsigned char msk[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	// Gets byte index
	idx=(blk_nb/8);
	// Gets mask
	msk_idx=blk_nb-(8*idx);
	if ( (sync_list[idx]&msk[msk_idx]) == 0 ) return 0;
	else return 1;
}

// Image memory format :
// An image of MxN pixels consists of an array in this way
// A[0] ->    P0,0 P0,1........P0,M-1
// A[M] ->    P1,0 P1,1........P1,M-1
// .................................
// A[M*(N-1)] PN,0 PN,1........PN,M-1
// A block of X,Y (for instance 2x) then consists
// P0,0 P01,1
// P1,0 P1,1

unsigned char put_block_image ( unsigned char * src, unsigned char * dest,Image_Attributs atr_img, unsigned int blk_nb)
{
unsigned int row;
unsigned int col;
unsigned int buf_idx;
unsigned int line_blk;
unsigned int nb_blk_col;
unsigned int col_blk;
unsigned long int img_idx;

		if (blk_nb>=atr_img.full_image_blocks)	{
			// Return error, blocks out of the limits
			return -1;
		}
		// Columns of Blocks
		nb_blk_col=atr_img.cols/atr_img.block_cols;
		// Gets the block line
		line_blk=blk_nb/nb_blk_col;
		// Gets the block columns
		col_blk=blk_nb-(line_blk*nb_blk_col);

		img_idx=col_blk*atr_img.block_cols+(line_blk*atr_img.block_rows*atr_img.cols);
		buf_idx=0;
		for (row=0;row<atr_img.block_rows;row++){
			for (col=0;col<atr_img.block_cols;col++){
				dest[img_idx]=src[buf_idx];
				buf_idx++;
				img_idx++;
			}
			// Index position in the next line
			img_idx+=(atr_img.cols-atr_img.block_cols);
		}
		return 1;
}

unsigned char get_block_image(unsigned char *src,unsigned char *dest,Image_Attributs atr_img,unsigned int blk_nb)
{
unsigned int row;
unsigned int col;
unsigned int buf_idx;
unsigned int line_blk;
unsigned int nb_blk_col;
unsigned int col_blk;
unsigned long int img_idx;

	if (blk_nb>=atr_img.full_image_blocks)	{
		// Return error, blocks out of the limits
		return -1;
	}

	// Columns of Blocks
	nb_blk_col=atr_img.cols/atr_img.block_cols;
	// Gets the block line
	line_blk=blk_nb/nb_blk_col;
	// Gets the block columns
	col_blk=blk_nb-(line_blk*nb_blk_col);

	img_idx=col_blk*atr_img.block_cols+(line_blk*atr_img.block_rows*atr_img.cols);
	buf_idx=0;
	for (row=0;row<atr_img.block_rows;row++){
		for (col=0;col<atr_img.block_cols;col++){
			dest[buf_idx]=src[img_idx];
			buf_idx++;
			img_idx++;
		}
		// Index position in the next line
		img_idx+=(atr_img.cols-atr_img.block_rows);
	}

	return 1;
}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}


