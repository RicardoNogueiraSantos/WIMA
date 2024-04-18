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


#ifndef SLICER_H
#define SLICER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Image.h"
#include "TransferUDP.h"

typedef struct tSlicedData {
	unsigned int slices;
	unsigned int sliceSize;
	unsigned int lastSliceSize;
	unsigned int * filled; //para indicar quais blocos j� chegaram
	unsigned char ** data; //matriz onde cada linha tem tamanho sliceSize e cont�m "slices" slices
	                       //na transmiss�o ser� enviado uma slice por vez --> data[i]
} tSlicedData;

typedef struct tSlicer {	
	tSlicedData* (*sliceImage)(tImage *);
	tSlicedData* (*sliceCompressedData)(tCompressedData *);
	int (*isFilled)(tSlicedData*); //indica que todos os blocos chegaram
	unsigned char ** (*toImgData)(tSlicedData *, int, int, int);
	unsigned char * (*toCompressedData)(tSlicedData *, int);
	unsigned char * (*getSlice)(tSlicedData *, int);
	int (*addSlice) (tSlicedData *, unsigned char *, int);
} tSlicer;

tSlicer * tSlicer_create();
void tSlicer_destroy(tSlicer * slicer);

#endif
