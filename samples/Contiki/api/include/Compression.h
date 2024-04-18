#ifndef COMPRESSION_H
#define COMPRESSION_H
#include "Image.h"

typedef struct tCompression {
	tCompressedData * (*compressChangeDetectionData)(struct tCompression*, tCompressedData*);      
	tCompressedData * (*compressImage)(struct tCompression*, tImage*);  
} tCompression;

tCompression * tCompression_create();
void tCompression_destroy(tCompression *compression);
tCompressedData * tCompression_compressChangeDetectionData(tCompression*, tCompressedData *);      
tCompressedData * tCompression_compressImage(tCompression*, tImage* img);
	
#endif
