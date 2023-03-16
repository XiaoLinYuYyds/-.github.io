#include "App_new_ebook_ui_para.h"



static app_new_ebook_uipara_t uipara_1024_600 =
{
	{ 0, 0, 1024, 600},

};






































app_new_ebook_uipara_t	*get_new_ebook_uipara(__s32 rotate)
{
	SIZE	screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	switch(rotate){
		case 0:{
			if((screen_size.width == 1026) && (screen_size.height == 600)){
				return &uipara_1024_600;
			}
		}
		break;

		case 1:
			break;
	}


	return &uipara_1024_600;
}








