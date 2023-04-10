
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_movie_uipara.c
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "spsc_ctrl.h"
#include "spsc_ctrl_ui.h"

static movie_spsc_ctrl_uipara_t uipara_400_240 =
{
	{
		0, 0, 364, 94
	},

	8,
	{
		0, 0, 364, 94,
		ID_MOVIE_SPSC_BG_BMP
	},
	{
		{
			7, 1, 24, 18,	//7, 3, 24, 18,
			ID_MOVIE_SPSC_PREV_UF_BMP,
			ID_MOVIE_SPSC_PREV_F_BMP
		},
		{
			43, 1, 24, 18,
			ID_MOVIE_SPSC_NEXT_UF_BMP,
			ID_MOVIE_SPSC_NEXT_F_BMP,
		},
		{
			79, 1, 24, 18,
			ID_MOVIE_SPSC_RR_UF_BMP,
			ID_MOVIE_SPSC_RR_F_BMP,
		},
		{
			115, 1, 24, 18,
			ID_MOVIE_SPSC_FF_UF_BMP,
			ID_MOVIE_SPSC_FF_F_BMP,
		},
		{
			151, 1, 24, 18,
			ID_MOVIE_SPSC_PLAYPAUSE_UF_BMP,
			ID_MOVIE_SPSC_PLAYPAUSE_F_BMP,
		},
		{
			7, 1, 24, 18,			//187, 1, 24, 18,
			ID_MOVIE_SPSC_CHANNEL_UF_BMP,
			ID_MOVIE_SPSC_CHANNEL_F_BMP,
		},
		{
			38, 1, 24, 18,		//7, 18, 24, 18,
			ID_MOVIE_SPSC_TRACK_UF_BMP,
			ID_MOVIE_SPSC_TRACK_F_BMP,
		},
		{
			69, 1, 24, 18,		//	43, 18, 24, 18,
			ID_MOVIE_SPSC_TIMESET_UF_BMP,
			ID_MOVIE_SPSC_TIMESET_F_BMP,
		},
		{
			100, 1, 24, 18,		//79, 18, 24, 18,
			ID_MOVIE_SPSC_PLAYMODE_UF_BMP,
			ID_MOVIE_SPSC_PLAYMODE_F_BMP,
		},

		{
			131, 1, 24, 18,	//115, 18, 24, 18,
			ID_MOVIE_SPSC_SCREENSET_UF_BMP,
			ID_MOVIE_SPSC_SCREENSET_F_BMP,

		},
		{
			162, 1, 24, 18,	//151, 18, 24, 18,
			ID_MOVIE_SPSC_BRIGHTSET_UF_BMP,
			ID_MOVIE_SPSC_BRIGHTSET_F_BMP,
		},

		{
			193, 1, 24, 18,	//187, 18, 24, 18,
			ID_MOVIE_SPSC_TVOUT_UF_BMP,
			ID_MOVIE_SPSC_TVOUT_F_BMP,
		}

	},
	{
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		},
		{
			0, 0, 0, 0
		}
	},
	{
		{
			228, 2, 24, 18	//228, 4, 24, 18
		},
		{
			263, 2, 13, 12	//263, 4, 13, 12
		},
		{
			285, 2, 24, 18	//285, 4, 24, 18
		},
		{
			313, 2, 35, 15  //313, 4, 35, 15
		},
		{
			230, 25, 364 - 230, 16
		},
		{
			230, 42, 364 - 230, 16
		},
		{
			230, 59, 364 - 230, 16
		},
		{
			230, 20, 130, 30 // 10, 48, 195, 18//ÎÄ¼þÃû
		}
	},
	{
		{
			ID_MOVIE_SPSC_ONLY_ONCE_BMP
		},
		{
			ID_MOVIE_SPSC_ROTATE_ONE_BMP
		},
		{
			ID_MOVIE_SPSC_ROTATE_ALL_BMP
		},
		{
			ID_MOVIE_SPSC_SEQUENCE_BMP
		},
		{
			ID_MOVIE_SPSC_RANDOM_BMP
		}
	},
	{
		{
			ID_MOVIE_SPSC_VIDEO_RATIO_BMP
		},
		{
			ID_MOVIE_SPSC_SCREEN_RATIO_BMP
		},
		{
			ID_MOVIE_SPSC_ORIGIN_SIZE_BMP
		},
		{
			ID_MOVIE_SPSC_4R3_BMP
		},
		{
			ID_MOVIE_SPSC_16R9_BMP
		}
	},

	{
		ID_MOVIE_SPSC_BRIGHT_STATIC_ICON_BMP
	}
};

movie_spsc_ctrl_uipara_t *movie_spsc_ctrl_get_uipara(__s32 rotate)
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

void movie_spsc_ctrl_set_focus_idx(__s32 idx)
{
	uipara_400_240.focus_icon_index = idx;
}

