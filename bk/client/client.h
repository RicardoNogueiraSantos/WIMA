#ifndef CLIENT_H
#define CLIENT_H

#include "commons.h"


// To be define MAX_MSG_SIZE definition
//#define MAX_MSG_SIZE  512

// Image row pixels
#define IMG_ROWS 64
// Image column pixels
#define IMG_COLS 64
// Image memory buffer length
#define FULL_IMG_BUF IMG_ROWS*IMG_COLS

// Block image update group
#define IMG_BLK_ROWS  4
#define IMG_BLK_COLS  4
#define IMG_BLK_SIZE  IMG_BLK_ROWS*IMG_BLK_COLS
// Number of block in a full image
#define IMG_NB_BLK  (FULL_IMG_BUF)/(IMG_BLK_ROWS*IMG_BLK_COLS)

// Update buffer memory
#define LEN_SYNC_BUF IMG_NB_BLK/8

typedef enum  {
	INIT=0,
	// Update full image without fragmentation, limited to UPD packet size (65535 bytes)
	UP_IMG_FULL,
	UP_IMG_BLOCK,
	// Update image partially
	CLIENT2SERVER_SYNC,
	CHECK_SERVER_SYNC,
	// Console command
	ENTER_MANUAL_CMD,
	END_APL
} client_states;

typedef enum  {
	SYNC_FULL_BLOCK_TO_SERVER=0,
	SYNC_CHANGED_BLOCKS_TO_SERVER=1
} block_server_update_states;

#endif /* CLIENT_H */
