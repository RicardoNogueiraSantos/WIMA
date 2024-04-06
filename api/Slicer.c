#include "Slicer.h"

tSlicedData* tSlicer_sliceImage(tImage * img) {
	tSlicedData * sdata = NULL;
	printf("Slice Image...\n");
	if (img != NULL) {
		sdata = (tSlicedData*) malloc (sizeof(tSlicedData));

		//calcula tamanho dos dados em unsigned chars
		int datasize = img->height*img->width*img->channels;

		if (datasize%TRANSFER_DATA_BUFFER_SIZE != 0) {
			sdata->slices = datasize/TRANSFER_DATA_BUFFER_SIZE + 1; //slices
			sdata->sliceSize = TRANSFER_DATA_BUFFER_SIZE;
			sdata->lastSliceSize = datasize%TRANSFER_DATA_BUFFER_SIZE; //é o que falta pra completar os dados
		} else {
			//sem sobra no final, dados encaixam perfeito nos slices
			sdata->slices = datasize/TRANSFER_DATA_BUFFER_SIZE; //slices
			sdata->sliceSize = TRANSFER_DATA_BUFFER_SIZE;
			sdata->lastSliceSize = sdata->sliceSize; //o último é do mesmo tamanho dos outros
		}

		//Aloca memória para os dados
		sdata->data = (unsigned char**) malloc (sdata->slices * sizeof(unsigned char*));
		sdata->filled = (unsigned int *) malloc (sdata->slices * sizeof(unsigned int *));
		for (int i = 0; i < sdata->slices; i++) {
			sdata->data[i] = (unsigned char *)malloc(sdata->sliceSize * sizeof(unsigned char));
		}

		//printf("datasize:%d\n", datasize);
		//printf("TRANSFER_DATA_BUFFER_SIZE:%d\n", TRANSFER_DATA_BUFFER_SIZE);
		//printf("Slices:%d\n", sdata->slices);
		//printf("SliceSize:%d\n", sdata->sliceSize);
		//printf("LastSliceSize:%d\n", sdata->lastSliceSize);

		int sliceIndex = 0; //marca em qual slice está
		int dataIndex = 0; //marca em qual indice dentro do slice está
		//Percorre imagem e vai colocando nos slices
		for(int i=0;i<img->channels;i++) {
			for(int j=0;j<img->width;j++) {
				for(int k=0;k<img->height;k++) {
					sdata->data[sliceIndex][dataIndex++] = img->data[i][k*img->height+j];
					//printf("sliceIndex=%d/dataIndex=%d/i=%d/j=%d/k=%d\n", sliceIndex, dataIndex, i, j, k);
					if (dataIndex == sdata->sliceSize) {
						sdata->filled[sliceIndex] = 1;
						sliceIndex++;
						dataIndex = 0;
					}
				}
			}
		}

	}

	return sdata;
}

unsigned char ** tSlicer_toImgData(tSlicedData * sdata, int channels, int height, int width) {
	unsigned char ** imgdata = NULL;

	if (sdata != NULL) {
		imgdata = (unsigned char**) malloc (channels * sizeof(unsigned char*));
		for (int i = 0; i < channels; i++) {
			imgdata[i] = (unsigned char *)malloc(height * width * channels * sizeof(unsigned char));
		}

		int sliceIndex = 0; //marca em qual slice está
		int dataIndex = 0; //marca em qual indice dentro do slice está
		//Percorre imagem e vai colocando nos slices
		for(int i=0;i<channels;i++) {
			for(int j=0;j<width;j++) {
				for(int k=0;k<height;k++) {
					imgdata[i][k*height+j] = sdata->data[sliceIndex][dataIndex++];
					if (dataIndex == sdata->sliceSize) {
						sliceIndex++;
						dataIndex = 0;
					}
				}
			}
		}
	}

	return imgdata;
}

tSlicedData* tSlicer_sliceCompressedData(tCompressedData * compressedData) {
	tSlicedData * sdata = NULL;

	if (compressedData != NULL) {
		sdata = (tSlicedData*) malloc (sizeof(tSlicedData));

		if (compressedData->size%TRANSFER_DATA_BUFFER_SIZE != 0) {
			sdata->slices = compressedData->size/TRANSFER_DATA_BUFFER_SIZE + 1; //slices
			sdata->sliceSize = TRANSFER_DATA_BUFFER_SIZE;
			sdata->lastSliceSize = sdata->slices*sdata->sliceSize - compressedData->size; //é o que falta pra completar os dados
		} else {
			//sem sobra no final, dados encaixam perfeito nos slices
			sdata->slices = compressedData->size/TRANSFER_DATA_BUFFER_SIZE; //slices
			sdata->sliceSize = TRANSFER_DATA_BUFFER_SIZE;
			sdata->lastSliceSize = sdata->sliceSize; //o último é do mesmo tamanho dos outros
		}

		//Aloca memória para os dados
		sdata->data = (unsigned char**) malloc (sdata->slices * sizeof(unsigned char*));
		sdata->filled = (unsigned int *) malloc (sdata->slices * sizeof(unsigned int *));
		for (int i = 0; i < sdata->slices; i++) {
			sdata->data[i] = (unsigned char *)malloc(sdata->sliceSize * sizeof(unsigned char));
		}

		int sliceIndex = 0; //marca em qual slice está
		int dataIndex = 0; //marca em qual indice dentro do slice está
		//Percorre imagem e vai colocando nos slices
		for(int i=0;i<compressedData->size;i++) {
			sdata->data[sliceIndex][dataIndex++] = compressedData->data[i];
			if (dataIndex == sdata->sliceSize) {
				sdata->filled[sliceIndex] = 1;
				sliceIndex++;
				dataIndex = 0;
			}
		}
	}

	return sdata;
}

unsigned char * tSlicer_toCompressedData(tSlicedData * sdata, int size) {
	unsigned char * compressed = NULL;
	if (sdata != NULL) {
		compressed = (unsigned char*) malloc (size * sizeof(unsigned char));

		int sliceIndex = 0; //marca em qual slice está
		int dataIndex = 0; //marca em qual indice dentro do slice está
		//Percorre imagem e vai colocando nos slices
		for(int i=0;i<size;i++) {
			compressed[i] = sdata->data[sliceIndex][dataIndex++];
			if (dataIndex == sdata->sliceSize) {
				sliceIndex++;
				dataIndex = 0;
			}
		}
	}
	return compressed;
}


unsigned char * tSlicer_getSlice(tSlicedData * sdata, int index) {
	unsigned char * slice = NULL;

	if (sdata != NULL) {
		if (index < sdata->slices) {
			slice = sdata->data[index];
		}
	}

	return slice;
}

int tSlicer_addSlice(tSlicedData * sdata, unsigned char * slice, int index) {
	int ret = 1;

	if (sdata != NULL) {
		if (index < sdata->slices) {
			sdata->data[index] = slice;
			sdata->filled[index] = 1;
		}
	}

	return ret;
}

int tSlicer_isFilled(tSlicedData* sdata ) {
	int ret = 1;

	if (sdata != NULL) {
		for (int i = 0; i < sdata->slices; i++) {
			if (sdata->filled[i] == 0) return 0;
		}
	}

	return ret;
}

tSlicer * tSlicer_create() {
	tSlicer * slicer = (tSlicer *) malloc (sizeof(tSlicer));

	slicer->sliceImage = &tSlicer_sliceImage;
	slicer->sliceCompressedData  = &tSlicer_sliceCompressedData;
	slicer->isFilled  = &tSlicer_isFilled;
	slicer->toImgData  = &tSlicer_toImgData;
	slicer->toCompressedData  = &tSlicer_toCompressedData;
	slicer->getSlice  = &tSlicer_getSlice;
	slicer->addSlice  = &tSlicer_addSlice;
	slicer->isFilled  = &tSlicer_isFilled;

	return slicer;
}

void tSlicer_destroy(tSlicer * slicer) {
	if(slicer != NULL) free(slicer);
}
