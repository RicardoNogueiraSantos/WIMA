#include "Image.h"
#include "ImageDB.h"

tImage * tImageDb_get (tImageDb* imgDb, int index) {
	tImage * ret = NULL;
	
	if (imgDb != NULL) {
		if (index >= 0 && imgDb->size > index) {
			ret = imgDb->images[index];
		}
	}
	printf("tImageDb_get return %d\n", ret!=NULL);
	return ret;
}

int tImageDb_load (tImageDb* imgDb) {
	int ret = 0;

	char fNames[10][40];
	
	//inicializa com a mesma imagem, somente para teste
	sprintf(fNames[0], "images/lena_color.bmp\0");
	sprintf(fNames[1], "images/lena_color.bmp\0");
	sprintf(fNames[2], "images/lena_color.bmp\0");
	sprintf(fNames[3], "images/lena_color.bmp\0");
	sprintf(fNames[4], "images/lena_color.bmp\0");
	sprintf(fNames[5], "images/lena_color.bmp\0");
	sprintf(fNames[6], "images/lena_color.bmp\0");
	sprintf(fNames[7], "images/lena_color.bmp\0");
	sprintf(fNames[8], "images/lena_color.bmp\0");
	sprintf(fNames[9], "images/lena_color.bmp\0");
	imgDb->size = 10;

	//Cria espaço para 10 imagens
	imgDb->images = (tImage**) malloc (10*sizeof(tImage*));

	for (int k = 0; k < 10; k++) {
		imgDb->images[k] = tImage_loadBmp(fNames[k]);
	}
	
	tImage_saveBmp(imgDb->images[0],"lena_saved.bmp");
	
	return ret;
}

tImageDb * tImageDb_create() {
	tImageDb *  imgDb = (tImageDb *)malloc(sizeof(tImageDb));
	//Inicializa ponteiro para função get
	imgDb->get = &tImageDb_get;

	//Inicializa ponteiro para função get
	imgDb->load = &tImageDb_load;

}

int tImageDb_destroy(tImageDb * imgDb) {
	//printf("imgDb_destroy entrando\n");
	if (imgDb != NULL) {
		for (int i = 0 ; i < imgDb->size ; i++) {
				//printf("imgDb->get(%d)\n", i);
				tImage * img = imgDb->get(imgDb, i);
				tImage_destroy(img);
				/*
				if (img != NULL) {
					for (int j = 0; j < img->channels; j++) {
						//printf("imgDb->get(%d)[%d]\n", i,j);
						if (img->data != NULL) {
							free(img->data[j]);
						}
					}
					free(img);
				}
				*/
		}
		free(imgDb);
	}
	return 0;
}
