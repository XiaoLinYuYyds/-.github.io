#include "new_fm_set.h"




static new_fm_uipara_t	uipara_1024_600 = 
{
	{
		{0,0,15,11},
		{0,0,15,11}
	},
	{10,10,280,12},
	{10,10,1,18},
	{110,10,6,6},
	{10,10,65,16},
	{
		10,10,19,28
	},
	{10,10,65,16},

};


















new_fm_uipara_t *get_new_fm_uipara(__s32 rotate)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	switch(rotate) {
		case 0: {
			if((screen_size.width == 1024) && (screen_size.height == 600))
				return &uipara_1024_600;
		}
		break;
		
		default:
			break;
	}




}



