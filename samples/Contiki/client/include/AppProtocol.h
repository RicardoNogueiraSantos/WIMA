#ifndef APPPROTOCOL_H
#define APPPROTOCOL_H
#include <stdio.h>
#include <stdlib.h>

#include "Slicer.h"

typedef enum {
	HANDSHAKE_MSG_ID=1,
	HANDSHAKE_REPLY_MSG_ID=2,
	IMG_SLICE_HEADER_MSG_ID=3,
	SLICE_HEADER_MSG_ID=4,
	SLICE_DATA_MSG_ID=5,
	REPLY_MSG_ID=6,
} Message_ID;

typedef enum {
	REPLY_OK=1,
	REPLY_FAIL
} Reply_STATUS;


typedef struct tImgSliceHeaderMsg {
	Message_ID id:4;
	unsigned int height:16;
	unsigned int width:16;
	unsigned int bitDepth:16;
	unsigned int channels:16;
	unsigned int slices:16;
	unsigned int sliceSize:16;
	unsigned int lastSliceSize:16;
} tImgSliceHeaderMsg;


typedef struct tSliceDataMsg {
	Message_ID id:4;
	unsigned int position:16;
	unsigned int dataSize:16;
	unsigned char data[TRANSFER_DATA_BUFFER_SIZE];
} tSliceDataMsg;

typedef struct tHandShakeMsg {
	Message_ID id:4;
} tHandShakeMsg;

typedef struct tReplyMsg{
	Message_ID id:4;
	unsigned int status:4;
} tReplyMsg;

typedef struct tAppProtocol {
	int (*isHandShakeMsg)(tRawReceivedMsg *);
	int (*isHandShakeReply)(tRawReceivedMsg *);
	int (*isReplyOK)(tRawReceivedMsg *);
	int (*getMsgId)(unsigned char *);
	tImgSliceHeaderMsg * (*encodeImgSliceHeaderMsg)(tImage *, tSlicedData *);
	tSliceDataMsg * (*encodeSliceDataMsg)(unsigned char *, int, int);
	tHandShakeMsg * (*encodeHandShakeMsg)();
	tReplyMsg * (*encodeReplyOk)();
	tImgSliceHeaderMsg * (*decodeImgSliceHeaderMsg)(tRawReceivedMsg *msg);
	tSliceDataMsg * (*decodeSliceDataMsg)(tRawReceivedMsg *msg);
	tHandShakeMsg * (*decodeHandShakeMsg)(tRawReceivedMsg *msg);
	tReplyMsg * (*decodeReply)(tRawReceivedMsg *msg);
} tAppProtocol;

tAppProtocol * tAppProtocol_create ();
void tAppProtocol_destroy(tAppProtocol*);
#endif
