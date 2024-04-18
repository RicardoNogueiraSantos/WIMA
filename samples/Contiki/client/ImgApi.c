//gcc -o client client.c commons.c
#include "ImgApi.h"
#include "CD1.h"
#include "DBImage_house.h"


extern void UDP_ComSetup( tComConfig config, COM_SOCK * ports);
extern void UDP_ComServerSetup (unsigned char port);
extern int UDP_SndMsg(unsigned char *msg,unsigned int len);
extern unsigned int UDP_RecMsg(unsigned char * buff_rec, unsigned int max_size, int time);


void InitializeTransmission( tComConfig cfg, COM_SOCK * ports)
{
	ports->snd_msg=NULL;
	ports->rec_msg=NULL;
	
	// Set ip protocol type and get com ports
	if (cfg.com_side==CLIENT_COM_SIDE){
		
		switch (cfg.com_type)
		{
			case 	UDP_CON :
				UDP_ComSetup(cfg, ports);
				//ports->SndMsg=UDP_SndMsg;
				//ports->RecMsg=UDP_RecMsg;
			break;
			default :
			
			break;	
			
		}
		
	}else
	{		
		switch (cfg.com_type)
		{
			case 	UDP_CON :
					
			break;
			
			default:
			break;	
			
		}	
	}	
	
}

void InitializeChgDetect(tChangeDetectionAlgo * detec_algo, CDTypes type)
{
	
	switch (type){
		
		case CD1 : 
			detec_algo->ChangeDetect=GetChangeDetection_CD1;
			break;
		
		default :
			detec_algo->ChangeDetect=NULL;
			break;
	}	
}

void InitializeCompression(tCompressionAlgo * compress_algo, tCompressionTypes type)
{
	switch (type){
		
		case CDT_COMPRESS : 
			
			break;
		
		default :
			
			break;
	}	
}

ImageType GetDBImage(tImagesIndex idximg, tImageInfo * Info)
{
ImageType ptr_img;
		switch (idximg)
		{
				case  HOUSE_IMAGE :
					ptr_img=(ImageType)&db_img_house;
					Info->rows=128;
					Info->cols=128;
					Info->image_size=16384;
					Info->Type=RAW_GRAY_8BIT;	
					break;
				default :
					ptr_img=NULL;
					Info->rows=0;
					Info->cols=0;
					Info->image_size=0;
					Info->Type=RAW_GRAY_8BIT;	
					break;
		}
		return ptr_img;
				
}









