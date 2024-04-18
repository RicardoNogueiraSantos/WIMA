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
