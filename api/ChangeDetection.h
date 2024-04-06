#ifndef CHANGEDETECTION_H
#define CHANGEDETECTION_H
#include <stdio.h>
#include <stdlib.h>
#include "Image.h"

typedef struct tChangeDetection {
	tCompressedData* (*changeDetect)(struct tChangeDetection*, tImage*);    
} tChangeDetection;

tCompressedData* changeDetect(tChangeDetection* changeDetection, tImage* img);
#endif