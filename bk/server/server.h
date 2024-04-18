#define MAX_CLIENTS 10

#include "commons.h"


typedef struct CLIENTS {
	struct sockaddr_in addr;
	//outras infos de cada cliente
} CLIENTS;

typedef struct Image_Client_Data{
	unsigned long int addr;
	unsigned char init;
	Image_Attributs attr;
	unsigned char * pixel_data;
	unsigned char * Image_sync_list;
	unsigned int sync_list_len;
} Image_Client_Data;

