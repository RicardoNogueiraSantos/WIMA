#ifndef CHG_DETECT_CD1_H
#define CHG_DETECT_CD1_H


int GetChangeDetection_CD1(ImageType curr_image,ImageType old_image, tCD_Settings Settings);
 

// Image Transfer Format
typedef struct {
	
	unsigned int block_rows;
	unsigned int block_cols;
	unsigned int blocK_size;
	unsigned int full_image_blocks;
	char difpixel_threshold;
	unsigned char * custom_settings;
} tCHG_Detec_Settings_A;


#endif /* CHG_DETECT_CD1_H */

	
