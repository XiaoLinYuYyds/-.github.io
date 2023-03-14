
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: spsc_prog_ui.c
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include "spsc_prog_ui.h"

static movie_spsc_prog_uipara_t uipara_400_240 =
{
	{
		0, 77, 364, 16
	},
	{
		{
			115, 55, 15, 15,//70, 75, 30, 15,
			ID_MOVIE_SPSC_PROG_PLAYSTA_PAUSE_BMP,
			ID_MOVIE_SPSC_PROG_PLAYSTA_PLAY_BMP,
		},
		{
			115, 75, 146, 4,
			ID_MOVIE_SPSC_PROG_BG_UF_BMP
		},
		{
			115, 75, 5, 4,
			ID_MOVIE_SPSC_PROG_CURSOR_UF_BMP
		}
	},
	{
		{
			5,  65, 25, 18
		},
		{
			41, 65, 25, 18
		},
		{
			77, 65, 25, 18
		},
	},
	{
		{
			33, 65, 5, 18
		},
		{
			69, 65, 5, 18
		}
	},
	{
		{
			268, 65, 25, 18
		},
		{
			304, 65, 25, 18
		},
		{
			340, 65, 25, 18
		},
	},
	{
		{
			296, 65, 5, 18
		},
		{
			332, 65, 5, 18
		}
	},
};

movie_spsc_prog_uipara_t *movie_spsc_prog_get_uipara(__s32 rotate)
{
	__s32 			screen_width;
	__s32 			screen_height;
	/* get lcd size*/
	dsk_display_get_size(&screen_width, &screen_height);

	switch(rotate)
	{
		case GUI_SCNDIR_NORMAL:
		case GUI_SCNDIR_ROTATE180:
		{
			if((screen_width == 480) && (screen_height == 272))
			{
				return &uipara_400_240;
			}
		}

		case GUI_SCNDIR_ROTATE90:
		case GUI_SCNDIR_ROTATE270:
			break;
	}

	return &uipara_400_240;
}
