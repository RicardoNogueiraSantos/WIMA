#ifndef IMAGE_H
#define IMAGE_H
#include <stdio.h>
#include <stdlib.h>

// Types of images format
typedef enum {	
	UNCOMPRESSED,
	JPG_IMG
} tImageFormat;

// Image Information
typedef struct tImage{	
	unsigned int height;
	unsigned int width;
	unsigned int bitDepth;
	unsigned int channels;
	unsigned char ** data;
//	unsigned int id; //no caso de querer identificar a imagem
} tImage;

// Image Information
typedef struct tCompressedData {
	unsigned int size; // in unsigned chars
	unsigned char * data;
} tCompressedData;

tImage* tImage_loadBmp (const char * filename);
int tImage_saveBmp (tImage* img, const char * filename);
void tImage_destroy (tImage * img);

#endif
