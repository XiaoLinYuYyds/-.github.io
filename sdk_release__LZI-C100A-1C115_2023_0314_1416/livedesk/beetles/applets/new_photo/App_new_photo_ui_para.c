#include "App_new_photo_ui_para.h"



static new_photo_uipara_t	uipara_1024_600 =
{
	{//窗口大小
		0, 0, 1024, 600
	},
	{//图层大小
		0, 0, 1024, 600
	},

};








new_photo_uipara_t	*get_new_photo_uipara(__s32 rotate)
{
	SIZE	screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	switch(rotate){
		case 0:{
			if((screen_size.width == 1024) && (screen_size.height == 600))
			return &uipara_1024_600;
		}
		break;
		case 1:
			break;
	}
	
	return &uipara_1024_600;;
}


