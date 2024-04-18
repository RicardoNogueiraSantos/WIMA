#include "client.h"
#include "commons.h"

const char tst_array [64][64]={
"11111111AAAAAAAAIIIIIIIIzzzzzzzz11111111AAAAAAAAIIIIIIIIzzzzzzzz",
"22222222BBBBBBBBHHHHHHHHyyyyyyyy22222222BBBBBBBBHHHHHHHHyyyyyyyy",
"33333333CCCCCCCCJJJJJJJJxxxxxxxx33333333CCCCCCCCJJJJJJJJxxxxxxxx",
"44444444DDDDDDDDKKKKKKKKvvvvvvvv44444444DDDDDDDDKKKKKKKKvvvvvvvv",
"55555555EEEEEEEELLLLLLLLuuuuuuuu55555555EEEEEEEELLLLLLLLuuuuuuuu",
"66666666FFFFFFFFMMMMMMMMtttttttt66666666FFFFFFFFMMMMMMMMtttttttt",
"77777777GGGGGGGGNNNNNNNNssssssss77777777GGGGGGGGNNNNNNNNssssssss",
"88888888HHHHHHHHOOOOOOOOrrrrrrrr88888888HHHHHHHHOOOOOOOOrrrrrrrr",
"aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd",
"eeeeeeeeffffffffgggggggghhhhhhhheeeeeeeeffffffffgggggggghhhhhhhh",
"iiiiiiiijjjjjjjjkkkkkkkklllllllliiiiiiiijjjjjjjjkkkkkkkkllllllll",
"mmmmmmmmnnnnnnnnooooooooppppppppmmmmmmmmnnnnnnnnoooooooopppppppp",
"qqqqqqqgrrrrrrrrssssssssttttttttqqqqqqqgrrrrrrrrsssssssstttttttt",
"11111111AAAAAAAAIIIIIIIIzzzzzzzz11111111AAAAAAAAIIIIIIIIzzzzzzzz",
"22222222BBBBBBBBHHHHHHHHyyyyyyyy22222222BBBBBBBBHHHHHHHHyyyyyyyy",
"33333333CCCCCCCCJJJJJJJJxxxxxxxx33333333CCCCCCCCJJJJJJJJxxxxxxxx",
"44444444DDDDDDDDKKKKKKKKvvvvvvvv44444444DDDDDDDDKKKKKKKKvvvvvvvv",
"55555555EEEEEEEELLLLLLLLuuuuuuuu55555555EEEEEEEELLLLLLLLuuuuuuuu",
"66666666FFFFFFFFMMMMMMMMtttttttt66666666FFFFFFFFMMMMMMMMtttttttt",
"77777777GGGGGGGGNNNNNNNNssssssss77777777GGGGGGGGNNNNNNNNssssssss",
"88888888HHHHHHHHOOOOOOOOrrrrrrrr88888888HHHHHHHHOOOOOOOOrrrrrrrr",
"aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd",
"eeeeeeeeffffffffgggggggghhhhhhhheeeeeeeeffffffffgggggggghhhhhhhh",
"iiiiiiiijjjjjjjjkkkkkkkklllllllliiiiiiiijjjjjjjjkkkkkkkkllllllll",
"mmmmmmmmnnnnnnnnooooooooppppppppmmmmmmmmnnnnnnnnoooooooopppppppp",
"qqqqqqqgrrrrrrrrssssssssttttttttqqqqqqqgrrrrrrrrsssssssstttttttt",
"uuuuuuuuvvvvvvvvxxxxxxxxyyyyyyyyuuuuuuuuvvvvvvvvxxxxxxxxyyyyyyyy",
"11111111AAAAAAAAIIIIIIIIzzzzzzzz11111111AAAAAAAAIIIIIIIIzzzzzzzz",
"22222222BBBBBBBBHHHHHHHHyyyyyyyy22222222BBBBBBBBHHHHHHHHyyyyyyyy",
"33333333CCCCCCCCJJJJJJJJxxxxxxxx33333333CCCCCCCCJJJJJJJJxxxxxxxx",
"44444444DDDDDDDDKKKKKKKKvvvvvvvv44444444DDDDDDDDKKKKKKKKvvvvvvvv",
"55555555EEEEEEEELLLLLLLLuuuuuuuu55555555EEEEEEEELLLLLLLLuuuuuuuu",
"66666666FFFFFFFFMMMMMMMMtttttttt66666666FFFFFFFFMMMMMMMMtttttttt",
"77777777GGGGGGGGNNNNNNNNssssssss77777777GGGGGGGGNNNNNNNNssssssss",
"88888888HHHHHHHHOOOOOOOOrrrrrrrr88888888HHHHHHHHOOOOOOOOrrrrrrrr",
"aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd",
"eeeeeeeeffffffffgggggggghhhhhhhheeeeeeeeffffffffgggggggghhhhhhhh",
"iiiiiiiijjjjjjjjkkkkkkkklllllllliiiiiiiijjjjjjjjkkkkkkkkllllllll",
"mmmmmmmmnnnnnnnnooooooooppppppppmmmmmmmmnnnnnnnnoooooooopppppppp",
"qqqqqqqgrrrrrrrrssssssssttttttttqqqqqqqgrrrrrrrrsssssssstttttttt",
"11111111AAAAAAAAIIIIIIIIzzzzzzzz11111111AAAAAAAAIIIIIIIIzzzzzzzz",
"22222222BBBBBBBBHHHHHHHHyyyyyyyy22222222BBBBBBBBHHHHHHHHyyyyyyyy",
"33333333CCCCCCCCJJJJJJJJxxxxxxxx33333333CCCCCCCCJJJJJJJJxxxxxxxx",
"44444444DDDDDDDDKKKKKKKKvvvvvvvv44444444DDDDDDDDKKKKKKKKvvvvvvvv",
"55555555EEEEEEEELLLLLLLLuuuuuuuu55555555EEEEEEEELLLLLLLLuuuuuuuu",
"66666666FFFFFFFFMMMMMMMMtttttttt66666666FFFFFFFFMMMMMMMMtttttttt",
"77777777GGGGGGGGNNNNNNNNssssssss77777777GGGGGGGGNNNNNNNNssssssss",
"88888888HHHHHHHHOOOOOOOOrrrrrrrr88888888HHHHHHHHOOOOOOOOrrrrrrrr",
"aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd",
"eeeeeeeeffffffffgggggggghhhhhhhheeeeeeeeffffffffgggggggghhhhhhhh",
"iiiiiiiijjjjjjjjkkkkkkkklllllllliiiiiiiijjjjjjjjkkkkkkkkllllllll",
"mmmmmmmmnnnnnnnnooooooooppppppppmmmmmmmmnnnnnnnnoooooooopppppppp",
"qqqqqqqgrrrrrrrrssssssssttttttttqqqqqqqgrrrrrrrrsssssssstttttttt",
"uuuuuuuuvvvvvvvvxxxxxxxxyyyyyyyyuuuuuuuuvvvvvvvvxxxxxxxxyyyyyyyy",
"11111111AAAAAAAAIIIIIIIIzzzzzzzz11111111AAAAAAAAIIIIIIIIzzzzzzzz",
"22222222BBBBBBBBHHHHHHHHyyyyyyyy22222222BBBBBBBBHHHHHHHHyyyyyyyy",
"33333333CCCCCCCCJJJJJJJJxxxxxxxx33333333CCCCCCCCJJJJJJJJxxxxxxxx",
"44444444DDDDDDDDKKKKKKKKvvvvvvvv44444444DDDDDDDDKKKKKKKKvvvvvvvv",
"55555555EEEEEEEELLLLLLLLuuuuuuuu55555555EEEEEEEELLLLLLLLuuuuuuuu",
"66666666FFFFFFFFMMMMMMMMtttttttt66666666FFFFFFFFMMMMMMMMtttttttt",
"aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd",
"eeeeeeeeffffffffgggggggghhhhhhhheeeeeeeeffffffffgggggggghhhhhhhh",
"iiiiiiiijjjjjjjjkkkkkkkklllllllliiiiiiiijjjjjjjjkkkkkkkkllllllll",
"mmmmmmmmnnnnnnnnooooooooppppppppmmmmmmmmnnnnnnnnoooooooopppppppp",
//"qqqqqqqgrrrrrrrrssssssssttttttttqqqqqqqgrrrrrrrrsssssssstttttttt"
};

int changed_elements[]=
{0,3,45,50,67,79,90,91,92,108,121,133,150,165,173,188,194,202,220,255,-1};
unsigned char changed_pattern[]=
{'/','/','/','/','/','/','/','/','/','/','/','/','/','/','/','/','/','/','/','/'};

void load_img(unsigned char * img)
{
unsigned int x;
unsigned int y;
unsigned long int idx;

	idx=0;
	for (x=0;x<IMG_ROWS;x++){
		for (y=0;y<IMG_COLS;y++){
			img[idx]=tst_array[x][y];
			idx++;
		}
	}
}


void emulated_chg_blocks(unsigned char * img,unsigned char * sync_list,Image_Attributs img_data)
{
int i;
	i=0;
	while(changed_elements[i]>=0)
	{
		sync_list_reset_blk(sync_list,changed_elements[i]);
		put_block_image ((unsigned char *) changed_pattern,(unsigned char *) img,img_data,changed_elements[i]);
		i++;
	}
}
