#ifndef IMGDB_H
#define IMGDB_H
#include <stdio.h>
#include <stdlib.h>
#include "include/Image.h"

typedef struct tImageDb {
		tImage ** images;
		int size;
		tImage * (*get)(struct tImageDb*, int);
		int (*load)(struct tImageDb*);
} tImageDb;

tImageDb * tImageDb_create();
int tImageDb_destroy(tImageDb * imgDb);

#endif