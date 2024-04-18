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
	unsigned int * filled; //para indicar quais blocos já chegaram
	unsigned char ** data; //matriz onde cada linha tem tamanho sliceSize e contém "slices" slices
	                       //na transmissão será enviado uma slice por vez --> data[i]
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
