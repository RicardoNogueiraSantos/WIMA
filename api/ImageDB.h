#ifndef IMGDB_H
#define IMGDB_H
#include <stdio.h>
#include <stdlib.h>

typedef struct tImageDb {
		tImage ** images;
		int size;
		tImage * (*get)(struct tImageDb*, int);
		int (*load)(struct tImageDb*);
} tImageDb;

tImageDb * tImageDb_create();
int tImageDb_destroy(tImageDb * imgDb);

#endif