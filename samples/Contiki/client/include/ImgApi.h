
#ifndef IMG_API_H
#define IMG_API_H

#include<stdio.h>	
#include<string.h> 
#include<stdlib.h>


/*Precisa ver exatamente qual a melhor maneira de definir uma image*/
/*Precisamos de uma definição pra usar a mesma coisa em todas as funções */
typedef unsigned char * ImageType;
typedef unsigned char * CompressedType;

// Types of connections
typedef enum {	
	UDP_CON,
	TCP_CON
} TransportTypes;

// Types of images format
typedef enum {	
	RAW_GRAY_8BIT,
	JPG_IMG
} tImageSourceFormat;

// Client ro server device
typedef enum {	
	CLIENT_COM_SIDE,
	SERVER_COM_SIDE
} ComSideTypes;

typedef struct {
    unsigned int  port;
	unsigned int service_id;
	char * address ;
	TransportTypes com_type;
	ComSideTypes com_side;
}tComConfig;

typedef enum {	
	 NOT_USED,
	CDT_COMPRESS
} tCompressionTypes;

typedef enum {	
	 HOUSE_IMAGE,
	ZZ_IMAGE
} tImagesIndex;

typedef enum {	
	CD1,
	CD2
} CDTypes;


struct COM_SOCK {
	int (*snd_msg )(unsigned char *msg,unsigned int len);
	void (*rec_msg )( unsigned char * buff_rec, unsigned int len);
};

typedef struct COM_SOCK COM_SOCK;

// Image Information
typedef struct tImageInfo{
	unsigned int rows;
	unsigned int cols;
	unsigned long int image_size;
	tImageSourceFormat Type;
} tImageInfo;

typedef struct {
	int (*size)();
    ImageType imgdata ;
	tImageInfo Info;
    ImageType (* get )(tImagesIndex idx, tImageInfo * Info);
} tImageDb;


typedef struct {
	tImageInfo info;
	unsigned char * custom_settings;
}tCD_Settings;

typedef struct {
	unsigned int block_rows;
	unsigned int block_cols;
} tImgBlocksInfo; 

typedef struct {
    int (*init)(CDTypes type);
    int (*ChangeDetect)(ImageType curr_image,ImageType old_image, tCD_Settings Settings);
	tCD_Settings Settings;
	unsigned char * blockchanged_list;
} tChangeDetectionAlgo;


typedef struct {
	tImageInfo info;
	unsigned char * custom_settings;
}tCompress_Settings;

typedef struct {
    void (*init)(tCompressionTypes type);
    CompressedType (* compress)(ImageType img_in,  tCompress_Settings Settings);
    ImageType (* uncompress)(CompressedType comp_img, tCompress_Settings Settings);
	tCompress_Settings Settings;
} tCompressionAlgo;

// Image Transfer Format
typedef struct tTransSettings{
	
	tImageInfo img_info;
	tImgBlocksInfo blk_info;
	unsigned char * custom_settings;
} tTransSettings;

typedef struct  {
	int (*init) (  tComConfig cfgcom, COM_SOCK * ports );
    COM_SOCK *  ports;
	tTransSettings settings;
    // And data goes here.
} tTransmissionSession ;

typedef struct {
    tChangeDetectionAlgo * changeDetectionAlgo;
    tCompressionAlgo * compressionAlgo;
    tTransmissionSession transmissionSession;
    
 	void (*setChangeDetection)(tChangeDetectionAlgo*);
	void (*setCompressionAlgo)(tCompressionAlgo*);
	//void (*settTransmissionSessionTransport)(tTransmissionSession*);

    //O send precisa implementar toda a lógica de mandar a imagem pro outro lado,
    //considerando os settings de change detection, compression e transporte que o cara setar
    void (*send)(ImageType img);
    
    //O receive precisa implementar toda a lógica de receber a imagem no cliente
    ImageType* (*receive)();
} tWSLImagePipe;



// Initializations functions

void InitializeChgDetect(tChangeDetectionAlgo * detec_algo, CDTypes type);
void InitializeCompression(tCompressionAlgo * compress_algo, tCompressionTypes type);
void InitializeTransmission( tComConfig cfgcom, COM_SOCK * ports);
ImageType GetDBImage(tImagesIndex idximg, tImageInfo * Info);

#endif /* COMMONS_H */

