/*
Copyright 2024 Ricardo Santos

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
