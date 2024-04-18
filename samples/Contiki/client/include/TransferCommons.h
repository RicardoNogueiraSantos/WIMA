
#ifndef TRANSFER_COMMONS_H
#define TRANSFER_COMMONS_H

#include<stdio.h>	//printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include "cfs/cfs.h"

// To be define MAX_MSG_SIZE definition
#define MAX_MSG_SIZE  80

// Maximum data on block messages
#define MAX_DATA_IMG_BLOCK_SIZE 80

#define UP_IMG_FULL_ONE_BLOCK  0
#define UP_IMG_FULL_IN_BLOCKS  1

#define RETRY_ATTEMPTS         5
#define BLOCK_MSG_REPLY_REQ

typedef enum {
	HANDSHAKE_MSG_ID=1,
	HANDSHAKE_REPLY_MSG_ID=2,
	IMAGE_FULL_MSG_ID=3,
	IMAGE_FULL_REPLY_MSG_ID=4,
	IMAGE_BLOCK_MSG_ID=5,
	IMAGE_BLOCK_REPLY_MSG_ID=6,
	IMAGE_SYNC_MSG_ID=7,
	IMAGE_SYNC_REPLY_MSG_ID=8,
	IMAGE_CHKSYNC_MSG_ID=9,
	IMAGE_CHKSYNC_REPLY_MSG_ID=10
} Message_ID;

typedef enum {
	// Image updated at one full block
	IMG_FULL_UPDATE_TYPE_FULL=0,
	// Image updated by fragments
	IMG_FULL_UPDATE_TYPE_IN_FRAGS=1
} Full_Img_Update_Type;

typedef enum Msg_Block_Type{
	BLOCK_TYPE_CONT=0,
	BLOCK_TYPE_SPARSE=1
} Msg_Block_Type;

typedef struct Handshake_MSG {
	Message_ID id:4;
	unsigned char spare_01:4;
	unsigned int img_rows:16;
	unsigned int img_cols:16;
	unsigned int block_rows:16;
	unsigned int block_cols:16;
	unsigned int blocK_size:16;
	unsigned int full_image_blocks:16;
	unsigned long int image_size:32;
} Handshake_MSG;


typedef enum {
	REPLY_OK=1,
	REPLY_FAIL
} Reply_STATUS; 

typedef struct Handshake_REPLY_MSG{
	Message_ID id:4;
	unsigned int status:4;
} Handshake_REPLY_MSG;


#define FULL_MSG_DATA_LEN MAX_MSG_SIZE-10

typedef struct Image_FULL_MSG{
	Message_ID id:4;
	Full_Img_Update_Type type:4;
	unsigned int initial_blk:16;
	unsigned char nb_of_blks:8;
	unsigned char data[FULL_MSG_DATA_LEN];
	//other fields....
} Image_FULL_MSG;


typedef struct Image_FULL_REPLY_MSG{
	Message_ID id:4;
	unsigned int status:4;
	//other fields....
} Image_FULL_REPLY_MSG;

typedef struct Image_BLOCK_MSG{
	Message_ID id:4;
	Msg_Block_Type type:4;
	unsigned int ini_blk:16;
	unsigned char nb_blk:8;
	unsigned char data[MAX_DATA_IMG_BLOCK_SIZE];
	//other fields....
} Image_BLOCK_MSG;

typedef struct Image_BLOCK_REPLY_MSG{
	Message_ID id:4;
	unsigned int status:4;	
	//other fields....
} Image_BLOCK_REPLY_MSG;

typedef struct Image_SYNC_MSG{
	Message_ID id:4;
	unsigned char dummy:4;
	unsigned int sync_list_len;
	unsigned char sync_list[MAX_DATA_IMG_BLOCK_SIZE];
	//other fields....
} Image_SYNC_MSG;

typedef struct Image_SYNC_REPLY_MSG{
	Message_ID id:4;
	unsigned char status:4;
	//other fields....
} Image_SYNC_REPLY_MSG;

typedef struct Image_CHKSYNC_MSG{
	Message_ID id:4;
	unsigned char dummy:4;
	//other fields....
} Image_CHKSYNC_MSG;

typedef struct Image_CHKSYNC_REPLY_MSG{
	Message_ID id:4;
	unsigned char status:4;
	unsigned int sync_list_len;
	unsigned char sync_list[MAX_DATA_IMG_BLOCK_SIZE];
	//other fields....
} Image_CHKSYNC_REPLY_MSG;

typedef struct tTransferSettings{
	unsigned char  * img;
	unsigned int rows;
	unsigned int cols;
	unsigned int block_rows;
	unsigned int block_cols;
	unsigned int blocK_size;
	unsigned int full_image_blocks;
	unsigned long int image_size;
} tTransferSettings;



//---- DECODE functions

Handshake_MSG decode_handshake_message (unsigned char * buff);
Handshake_REPLY_MSG decode_handshake_reply_message (unsigned char * buff);
Image_FULL_MSG decode_img_full_message (unsigned char * buff);
Image_FULL_REPLY_MSG decode_img_full_reply_message (unsigned char * buff);
Image_BLOCK_MSG decode_img_block_message (unsigned char * buff);
Image_BLOCK_REPLY_MSG decode_img_block_reply_message (unsigned char * buff);
Image_SYNC_MSG decode_img_sync_message (unsigned char *buff);
Image_CHKSYNC_MSG decode_img_chksync_message (unsigned char * buff);
Image_CHKSYNC_REPLY_MSG decode_img_chksync_reply_msg(unsigned char * buff);


//---- DUMP functions
void dump_handshake_message (Handshake_MSG msg);
void dump_handshake_reply_message (Handshake_REPLY_MSG msg);
void dump_img_full_message (Image_FULL_MSG msg_t);
void dump_img_full_reply_message (Image_FULL_REPLY_MSG msg);
void dump_img_block_message (Image_BLOCK_MSG msg);
void dump_img_block_reply_message (Image_BLOCK_REPLY_MSG msg);

// Others
char chk_reply(unsigned char * buff_snd,unsigned char * buff_rec,unsigned int len_snd,unsigned int len_rec, char reply  );
unsigned char get_expected_reply(unsigned char msg_id);
void log_msg_id(char * msg, unsigned char msg_id);
void log_msg_array (char * msg, unsigned char * arr, unsigned int len,char log_enabled);
void log_msg_matrix (char * msg, unsigned char * mat, unsigned int col, unsigned int row,char msg_log);
void log_msg_scalar (char * msg, unsigned int scl);
void sync_list_full_reset(unsigned char * img_data,unsigned int len);
void sync_list_full_set(unsigned char * img_data,unsigned int len);
void sync_list_set_blk(unsigned char * sync_list, unsigned int blk_nb);
void sync_list_reset_blk(unsigned char * img_data, unsigned int blk_nb);
char check_all_blocks_received(unsigned char *img_sync, unsigned int len);
unsigned char sync_list_check_blk(unsigned char * sync_list, unsigned int blk_nb);
unsigned char put_block_image ( unsigned char * src, unsigned char * dest,tTransferSettings atr_img, unsigned int blk_nb);
unsigned char get_block_image (unsigned char *src,unsigned char *dest,tTransferSettings atr_img,unsigned int blk_nb);
//char msg_snd(UDP_SOCK * udp,unsigned char * buff_snd,unsigned char * buff_rec,unsigned int len,unsigned char count_attempt );
void log_img_file (char * msg, unsigned char * mat, unsigned int col, unsigned int row);

#endif  /*TRANSFER_COMMONS_H*/

