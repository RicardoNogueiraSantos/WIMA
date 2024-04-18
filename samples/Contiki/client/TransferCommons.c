//gcc -o client client.c commons.c
#include "TransferCommons.h"

void ConvByteToHex(unsigned char nbyte, unsigned char * str);

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
	int i;
	//TODO
	Image_FULL_MSG msg;
	Image_FULL_MSG * rec_msg = (Image_FULL_MSG *)buff;
	msg.id=rec_msg->id;
	msg.initial_blk=rec_msg->initial_blk;
	msg.nb_of_blks=rec_msg->nb_of_blks;
	for (i=0;i<FULL_MSG_DATA_LEN;i++) msg.data[i]=rec_msg->data[i];
	msg.type=rec_msg->type;
	return msg;
}

Image_FULL_REPLY_MSG decode_img_full_reply_message (unsigned char * buff){
	//TODO
	Image_FULL_REPLY_MSG msg;
	return msg;
}

Image_BLOCK_MSG decode_img_block_message (unsigned char * buff){
	int i;

	//TODO
	Image_BLOCK_MSG msg;
	Image_BLOCK_MSG * rec_msg = (Image_BLOCK_MSG *)buff;
	msg.type=rec_msg->type;
	msg.ini_blk=rec_msg->ini_blk;
	msg.nb_blk=rec_msg->nb_blk;
	for (i=0;i<MAX_DATA_IMG_BLOCK_SIZE;i++){
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
	int i;
	//TODO
	Image_CHKSYNC_REPLY_MSG msg;
	Image_CHKSYNC_REPLY_MSG * rec_msg=(Image_CHKSYNC_REPLY_MSG *)buff;
	msg.id=rec_msg->id;
	msg.id=rec_msg->status;
	msg.sync_list_len=rec_msg->sync_list_len;
	for (i=0;i<msg.sync_list_len;i++) msg.sync_list[i]=rec_msg->sync_list[i];
	return msg;
}

Image_SYNC_MSG decode_img_sync_message (unsigned char *buff){
	int i;

	Image_SYNC_MSG msg;
	Image_SYNC_MSG * rec_msg=(Image_SYNC_MSG *)buff;
	msg.id=rec_msg->id;
	msg.sync_list_len=rec_msg->sync_list_len;
	for (i=0;i<MAX_DATA_IMG_BLOCK_SIZE;i++) msg.sync_list[i]=rec_msg->sync_list[i];

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

//#define DSP_TRANSACTION_RESULT 1
//#define DSP_TX_MSG             1

char chk_reply(unsigned char * buff_snd,unsigned char * buff_rec,unsigned int len_snd,unsigned int len_rec, char reply  )
{
unsigned char snd_id;
unsigned char rec_id;
unsigned char expected_reply;
char error_cod;
	

	error_cod=0;
	snd_id=get_message_id(buff_snd[0]);
#ifdef DSP_TX_MSG
	log_msg_id("Sent message : ",snd_id);
	log_msg_array("Bytes sent : ",buff_snd,len_snd,'y');
#endif
	if (reply=='y')
	{
		if (len_rec>0)
		{
			rec_id=get_message_id(buff_rec[0]);
			expected_reply=get_expected_reply(snd_id);
			if (rec_id==expected_reply)
			{
				error_cod=0;
			}
			else error_cod=-2;
		}
		else  error_cod=-1;
	}else error_cod=1;

#ifdef DSP_TRANSACTION_RESULT
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
			printf("Unknown message(%d) received. Protocol fail \n", buff_rec[0]);
		break;

		case -3:
			printf("Received message is empty \n");
		break;

		case -4:
			printf("Not possible to send an UDP message there was a socket error, please restart application, exiting...\n");
			//free(udp);
			exit(1);
		break;

		default :

		break;

	}
#endif
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


void sync_list_full_reset(unsigned char * img_data,unsigned int len)
{
	int i;

	for ( i=0;i<len;i++)
	{
		img_data[i]=0x00;
	}
}

void sync_list_full_set(unsigned char * img_data,unsigned int len)
{
	int i;

	for ( i=0;i<len;i++)
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
	int i;
	for ( i=0;i<len;i++){
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

unsigned char put_block_image ( unsigned char * src, unsigned char * dest,tTransferSettings atr_img, unsigned int blk_nb)
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


unsigned char get_block_image(unsigned char *src,unsigned char *dest,tTransferSettings atr_img,unsigned int blk_nb)
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


void log_msg_id(char * msg, unsigned char com_id)
{
	
char com_str [11][60] =
{"Handsahek message \n",
"Handsahek message reply \n",
"Full image message \n",
"Full image message reply \n",
"Block image message \n",
"Block image message reply \n",
"Client to server sync image message \n",
"Client to server sync image message reply \n",
"Check sync image message \n",
"Check sync image message reply \n",
"Unknown message \n"};

unsigned char msg_id;

	
	switch (com_id)
	{
		case HANDSHAKE_MSG_ID : msg_id=0;
		break;
		
		case HANDSHAKE_REPLY_MSG_ID : msg_id=1;
		break;
		
		case IMAGE_FULL_MSG_ID : msg_id=2;
		break;
		
		case IMAGE_FULL_REPLY_MSG_ID : msg_id=3;
		break;
		
		case IMAGE_BLOCK_MSG_ID : msg_id=4;
		break;
		
		case IMAGE_BLOCK_REPLY_MSG_ID : msg_id=5;
		break;
		
		case IMAGE_SYNC_MSG_ID : msg_id=6;
		break;
		
		case IMAGE_SYNC_REPLY_MSG_ID : msg_id=7;
		break;
		
		case IMAGE_CHKSYNC_MSG_ID : msg_id=8;
		break;
		
		case IMAGE_CHKSYNC_REPLY_MSG_ID : msg_id=9;
		break;
		
		default : msg_id=10;
		break;
	}
	printf(msg);
	printf(" ");
	printf((char *)com_str[msg_id]);
	//strcpy(&msg_str[0],msg);
	//strcpy(&msg_str[strlen(msg_str)],nm_str);
	//strcpy(&msg_str[strlen(msg_str)]," \n");
	puts(" ");
	//printf(msg_str);
	//puts(" ");

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

void log_msg_matrix (char * msg, unsigned char * mat, unsigned int col, unsigned int row,char msg_log)
{
unsigned int i,j,idx;
	if (msg_log=='n') return;
	if (col==0 || row==0){
		puts("no data \n");
		puts(" ");
		return ;
	}
	puts(msg);
	idx=0;
	for (i=0;i<row;i++){
		for (j=0;j<col;j++){
			printf(" %A", mat[idx]);
			idx++;
		}
		printf("/n");
	}
}

void ConvByteToHex(unsigned char nbyte, unsigned char * str)
{
unsigned char wk_byte;

	// First byte to convert
	wk_byte=0x0F&(nbyte>>4);
	if (wk_byte<10)
	{
		str[0]='0'+wk_byte;
	}else str[0]='A'+(wk_byte-10);
	
	// Second byte to convert
	wk_byte=0x0F&nbyte;
	if (wk_byte<10)
	{
		str[1]='0'+wk_byte;
	}else str[1]='A'+(wk_byte-10);

}

