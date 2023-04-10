/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: main_uipara.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "home_uipara.h"
#include "main_menu.h"
#include "sub_menu.h"

#define HORI_SPACE ((SCN_W-ITEM_W*ITEM_PER_LINE)/(ITEM_PER_LINE+1))		  //(1024屏幕宽 - 150*4(一行显示4个app))/(4+1)
#define VERT_SPACE ((SCN_H-HBAR_H-ITEM_H*ITEM_PER_COL)/(ITEM_PER_COL+1))  //(600屏幕高 - 150*2(一列显示2个app))/(2+1)

#define POS_X(x, y) ((HORI_SPACE+ITEM_W)*x+HORI_SPACE)
//#define POS_Y(x, y) ((VERT_SPACE+ITEM_H)*y+VERT_SPACE+HBAR_H)
#define POS_Y(x, y) ((VERT_SPACE+ITEM_H)*y+VERT_SPACE+10)

static home_uipara_t uipara_320_240 =
{
	{
		0, 0, 320, 240,
	},
	{
		0, 0, 320, 240,
	},
	{
		0, 273, 1, 1
	},
	{
		{
			POS_X(0, 0), POS_Y(0, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 0), POS_Y(1, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 0), POS_Y(2, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(0, 1), POS_Y(0, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 1), POS_Y(1, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 1), POS_Y(2, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(0, 2), POS_Y(0, 2), ITEM_W, ITEM_H
		},
		/*
		{
		    POS_X(1, 2), POS_Y(1, 2), ITEM_W, ITEM_H
		},
		*/
		{
			POS_X(2, 2), POS_Y(2, 2), ITEM_W, ITEM_H
		},
#if SP_APP_CALENDAR
		{
			POS_X(2, 2), POS_Y(2, 2), ITEM_W, ITEM_H
		},
#endif
	},

	//以下参数无效

	SCN_W / 4, //??4?
	4,//??4???
	4,//??4?
	45,//??????
	45,//??????

	118,//item width
	24,//item height
	118,//line width
	3,//line height
	118,//top width
	2,//top height
	118,//bottom width
	6//bottom height
};


static home_uipara_t uipara_400_240 =
{
	{
		0, 171, 400, 240 - 171
	},
	{
		0, 171, 400, 240 - 171
	},
	{
		0, 40, 118, 123
	},
	{
		{
			POS_X(0, 0), POS_Y(0, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 0), POS_Y(1, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 0), POS_Y(2, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(3, 0), POS_Y(3, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(4, 0), POS_Y(4, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(0, 1), POS_Y(0, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 1), POS_Y(1, 1), ITEM_W, ITEM_H
		},
		/*
		{
		    POS_X(2, 1), POS_Y(2, 1), ITEM_W, ITEM_H
		},
		*/
		{
			POS_X(3, 1), POS_Y(3, 1), ITEM_W, ITEM_H
		},
#if SP_APP_CALENDAR
		{
			POS_X(4, 1), POS_Y(4, 1), ITEM_W, ITEM_H
		},
#endif
	},
	400 / 4, //??5?
	4,
	4,//??5?
	45,//????
	45,//????

	118,//item width
	24,//item height
	118,//line width
	3,//line height
	118,//top width
	2,//top height
	118,//bottom width
	6//bottom height
};

static home_uipara_t uipara_480_272 =
{
	{
		0, 0, 480, 272//0, 185, 480, 272-185
	},
	{
		0, 0, 480, 272//0, 185, 480, 272-185
	},
	{
		0, 273, 1, 1
	},
	{
		{
			POS_X(0, 0), POS_Y(0, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 0), POS_Y(1, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 0), POS_Y(2, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(3, 0), POS_Y(3, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(0, 1), POS_Y(0, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 1), POS_Y(1, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 1), POS_Y(2, 1), ITEM_W, ITEM_H
		},
		/*
		{
		    POS_X(2, 1), POS_Y(2, 1), ITEM_W, ITEM_H
		},
		*/
		{
			POS_X(3, 1), POS_Y(3, 1), ITEM_W, ITEM_H
		},
#if SP_APP_CALENDAR
		{
			POS_X(4, 1), POS_Y(4, 1), ITEM_W, ITEM_H
		},
#endif
	},

	480 / 4, //??4?
	4,//??4???
	4,//??4?
	45,//??????
	45,//??????

	118,//item width
	24,//item height
	118,//line width
	3,//line height
	118,//top width
	2,//top height
	118,//bottom width
	6//bottom height
};

static home_uipara_t uipara_800_480 =
{
	{
            0, 0, 1024, 600
        },
        {
            0, 0, 1024, 600
	},
	{
		0, 273, 1, 1
	},
	{
		{
			POS_X(0, 0), POS_Y(0, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 0), POS_Y(1, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 0), POS_Y(2, 0), ITEM_W, ITEM_H
		},
		{
			POS_X(3, 0), POS_Y(3, 0), ITEM_W, ITEM_H
		},
//#if (SP_APP_CALENDAR || SP_APP_ATV)
#if 0
		{
			POS_X(4, 0), POS_Y(4, 0), ITEM_W, ITEM_H
		},
#endif
		{
			POS_X(0, 1), POS_Y(0, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(1, 1), POS_Y(1, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(2, 1), POS_Y(2, 1), ITEM_W, ITEM_H
		},
		{
			POS_X(3, 1), POS_Y(3, 1), ITEM_W, ITEM_H
		},
//#if (SP_APP_CALENDAR || SP_APP_ATV)
#if 0
		{
			POS_X(4, 1), POS_Y(4, 1), ITEM_W, ITEM_H
		},
#endif
	},

	600 / 4, //??4?
	4,//??4???
	4,//??4?
	45,//??????
	45,//??????

	118,//item width
	24,//item height
	118,//line width
	3,//line height
	118,//top width
	2,//top height
	118,//bottom width
	6//bottom height
};


home_uipara_t *home_get_ui_para(__s32 rotate)
{
	__s32           screen_width;
	__s32 			screen_height;
	/* get lcd size*/
	dsk_display_get_size(&screen_width, &screen_height);
	__msg("*************screen_width=%d,screen_height=%d\n", screen_width, screen_height);

	switch(rotate)
	{
	case GUI_SCNDIR_NORMAL:
	case GUI_SCNDIR_ROTATE180:
		{
			if((screen_width == 400 )&&( screen_height == 240))
				return &uipara_400_240;
			else if((screen_width == 480 )&&( screen_height == 272))
				return &uipara_480_272;
			else if((screen_width == 320 )&&( screen_height == 240))
				return &uipara_320_240;
			else if((screen_width == 800 )&&( screen_height == 480))
				return &uipara_800_480;
			else if((screen_width == 1024 )&&( screen_height == 600))
				return &uipara_800_480;
		}
	case GUI_SCNDIR_ROTATE90:
	case GUI_SCNDIR_ROTATE270:

		break;
	}

	return &uipara_800_480;
}



