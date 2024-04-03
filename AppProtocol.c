
#include "AppProtocol.h"

int tAppProtocol_getMsgId(unsigned char *msg){
	if (msg != NULL) {
		return msg[0]&0x0F;
	} else {
		return-1;
	}
}

int tAppProtocol_isHandShakeMsg(tRawReceivedMsg *msg) {
	if (msg != NULL) {
		return (tAppProtocol_getMsgId(msg->buffer) == HANDSHAKE_MSG_ID);
	} else {
		printf("tAppProtocol_isHandShakeMsg return 0, there is a error!!!\n");
		return 0;
	}
}

int tAppProtocol_isHandShakeReply(tRawReceivedMsg *msg) {
	if (msg != NULL) {
		return (tAppProtocol_getMsgId(msg->buffer) == HANDSHAKE_REPLY_MSG_ID);
	} else {
		printf("tAppProtocol_isHandShakeReply return 0, there is a error!!!\n");
		return 0;
	}
}

tReplyMsg * tAppProtocol_decodeReply(tRawReceivedMsg *msg){
	tReplyMsg * rpl = (tReplyMsg*) malloc (sizeof(tReplyMsg));

	tReplyMsg * buffmsg=(tReplyMsg *)msg->buffer;

	rpl->id = buffmsg->id;
	rpl->status = buffmsg->status;

	return rpl;

}

tImgSliceHeaderMsg * tAppProtocol_decodeImgSliceHeaderMsg(tRawReceivedMsg *msg){
	tImgSliceHeaderMsg * header = (tImgSliceHeaderMsg*) malloc (sizeof(tImgSliceHeaderMsg));

	tImgSliceHeaderMsg * buffmsg=(tImgSliceHeaderMsg *)msg->buffer;

	header->id = buffmsg->id;
	header->height = buffmsg->height;
	header->width = buffmsg->width;
	header->bitDepth = buffmsg->bitDepth;
	header->channels = buffmsg->channels;
	header->slices = buffmsg->slices;
	header->sliceSize = buffmsg->sliceSize;
	header->lastSliceSize = buffmsg->lastSliceSize;

	return header;
}

tSliceDataMsg * tAppProtocol_decodeSliceDataMsg(tRawReceivedMsg *msg){
	tSliceDataMsg * sdmsg = (tSliceDataMsg*) malloc (sizeof(tSliceDataMsg));

	tSliceDataMsg * buffmsg =(tSliceDataMsg*)msg->buffer;

	sdmsg->position = buffmsg->position;
	sdmsg->dataSize = buffmsg->dataSize;
	memcpy(sdmsg->data, &buffmsg->data,sdmsg->dataSize);

	return sdmsg;
}

tHandShakeMsg * tAppProtocol_decodeHandShakeMsg(tRawReceivedMsg *msg){
	tHandShakeMsg * hs = (tHandShakeMsg*) malloc (sizeof(tHandShakeMsg));

	tHandShakeMsg * buffmsg =(tHandShakeMsg *)msg->buffer;

	hs->id = buffmsg->id;

	return hs;
}

int tAppProtocol_isReplyOK(tRawReceivedMsg *msg) {
	//printf("tAppProtocol_isReplyOK...\n");
	if (msg != NULL) {
		tReplyMsg * reply = tAppProtocol_decodeReply(msg);
		//printf("reply->id == %d && reply->status == %d\n", reply->id, reply->status);
		int ret = (reply->id == REPLY_MSG_ID && reply->status == REPLY_OK);
		free(reply);
		return ret;
	} else {
		printf("tAppProtocol_isReplyOK return 0, there is a error!!!\n");
		return 0;
	}
}


tReplyMsg * tAppProtocol_encodeReplyOk(){
	tReplyMsg * msg = (tReplyMsg*) malloc (sizeof(tReplyMsg));
	msg->id = REPLY_MSG_ID;
	msg->status = REPLY_OK;
	return msg;
}

tImgSliceHeaderMsg * tAppProtocol_encodeImgSliceHeaderMsg(tImage * img, tSlicedData * slicedData){
	tImgSliceHeaderMsg * msg = (tImgSliceHeaderMsg*) malloc (sizeof(tImgSliceHeaderMsg));

	msg->id = IMG_SLICE_HEADER_MSG_ID;
	msg->height = img->height;
	msg->width = img->width;
	msg->bitDepth = img->bitDepth;
	msg->channels = img->channels;
	msg->slices = slicedData->slices;
	msg->sliceSize = slicedData->sliceSize;
	msg->lastSliceSize = slicedData->lastSliceSize;

	return msg;
}

tSliceDataMsg * tAppProtocol_encodeSliceDataMsg(unsigned char *data, int size, int pos){
	tSliceDataMsg * msg = (tSliceDataMsg*) malloc (sizeof(tSliceDataMsg));

	msg->id = SLICE_DATA_MSG_ID;
	msg->position = pos;
	msg->dataSize = size;
	memcpy(msg->data, data,size);
	return msg;
}

tHandShakeMsg * tAppProtocol_encodeHandShakeMsg(){
	tHandShakeMsg * msg = (tHandShakeMsg*) malloc (sizeof(tHandShakeMsg));

	msg->id = HANDSHAKE_MSG_ID;
	return msg;
}

tAppProtocol * tAppProtocol_create () {
	tAppProtocol * protocol = (tAppProtocol*) malloc (sizeof(tAppProtocol));

	protocol->isHandShakeMsg = &tAppProtocol_isHandShakeMsg;
	protocol->isHandShakeReply = &tAppProtocol_isHandShakeReply;
	protocol->isReplyOK = &tAppProtocol_isReplyOK;
	protocol->getMsgId = &tAppProtocol_getMsgId;
	protocol->encodeReplyOk = &tAppProtocol_encodeReplyOk;
	protocol->encodeImgSliceHeaderMsg = &tAppProtocol_encodeImgSliceHeaderMsg;
	protocol->encodeSliceDataMsg = &tAppProtocol_encodeSliceDataMsg;
	protocol->encodeHandShakeMsg = &tAppProtocol_encodeHandShakeMsg;
	protocol->decodeImgSliceHeaderMsg = &tAppProtocol_decodeImgSliceHeaderMsg;
	protocol->decodeSliceDataMsg = &tAppProtocol_decodeSliceDataMsg;
	protocol->decodeHandShakeMsg = &tAppProtocol_decodeHandShakeMsg;
	protocol->decodeReply = &tAppProtocol_decodeReply;

	return protocol;
}

void tAppProtocol_destroy (tAppProtocol* protocol) {
	if (protocol != NULL) free(protocol);
}
