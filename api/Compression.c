#include "Compression.h"

tCompression * tCompression_create() {
	//printf("tCompression.tCompression_create\n");
	tCompression *  compression = (tCompression *) malloc (sizeof(tCompression));
	compression->compressChangeDetectionData = &tCompression_compressChangeDetectionData; 
	compression->compressImage = &tCompression_compressImage;
	
	return compression;	
}

void tCompression_destroy(tCompression *compression) {
	printf("tCompression.tCompression_destroy\n");
	free(compression);
}

//compress change detection data, if necessary
tCompressedData * tCompression_compressChangeDetectionData(tCompression*, tCompressedData * cData) {
	printf("tCompression.compressChangeDetectionData\n");

	return cData;
}


//compress image data	
tCompressedData * tCompression_compressImage(tCompression*, tImage* img) {

	
	return NULL;
}	
