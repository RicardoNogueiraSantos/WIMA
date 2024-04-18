#include "Image.h"

tImage* tImage_loadBmp (const char * filename) {
	//Load images
	FILE *fIn = fopen(filename,"r");
	unsigned char byte[54];

	if(fIn==NULL) {											
		printf("File does not exist.\n");
	}
	for(int i=0;i<54;i++) {									
		byte[i] = getc(fIn); //Get the header								
	}
		
	//Read first Image
	tImage* img = (tImage*)malloc(sizeof(tImage));
	img->height = *(int*)&byte[18];
	img->width = *(int*)&byte[22];
	img->bitDepth = *(int*)&byte[28];
	img->channels = 3;
//	img->id = k;
	img->data = (unsigned char**) malloc (img->channels * sizeof(unsigned char*));
	for (int i = 0; i < img->channels; i++) {
		img->data[i] = (unsigned char *)malloc(img->height * img->width * img->channels * sizeof(unsigned char));
	}

	for(int i=0;i<img->width;i++) {
		for(int j=0;j<img->height;j++) {
			img->data[0][j*img->height+i]=(uint32_t)getc(fIn);// blue
			img->data[1][j*img->height+i]=(uint32_t)getc(fIn);// green
			img->data[2][j*img->height+i]=(uint32_t)getc(fIn);// red
		}
	}
	fclose(fIn);
	return img;
}

//apenas para teste da leitura e dos dados da imagem
int tImage_saveBmp (tImage* img, const char * filename) {
	FILE *fIn = fopen("images/lena_color.bmp","r");	// Usa para pegar o header
	unsigned char byte[54];

	if(fIn==NULL)							// check if the input file has not been opened succesfully.
	{											
		printf("File does not exist.\n");
	}

	for(int i=0;i<54;i++)						// read the 54 byte header from fIn
	{									
		byte[i] = getc(fIn);								
	}

	
	FILE *fOut = fopen(filename,"w+");	// Output File name
	
	fwrite(byte,sizeof(unsigned char),54,fOut);			// write the header back

	for(int i=0;i<img->width;i++) {
		for(int j=0;j<img->height;j++) {
			putc((unsigned char)img->data[0][j*img->height+i],fOut);
			putc((unsigned char)img->data[1][j*img->height+i],fOut);
			putc((unsigned char)img->data[2][j*img->height+i],fOut);
		}
	}
		
	fclose(fOut);
	fclose(fIn);
	return 0;	
}

void tImage_destroy (tImage * img) {
	if (img != NULL) {
		for (int j = 0; j < img->channels; j++) {
			if (img->data != NULL) {
				free(img->data[j]);
			}
		}
		free(img);
	}
}
