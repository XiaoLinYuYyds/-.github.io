/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: fm_uipara.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/07/25 10:31  create this file, implements the fundamental interface;
**************************************************************************************************************
*/

#include "fm_uipara.h"

//////////////////////////////////////////////////////////////////////////
static FM_UI_PARA_T fm_ui_800x480 =
{
	{
		//TOUCH_UI_RECT
		0, 0, 1024, 600
	},
	{
		//FM_UI_MAINWND
		{
			// rt
			0, 0, 1024, 600,
		},
		{
			// scale_bg 标尺背景位置
			280, 302, 440, 12
		},
		{
			//scale, 刻度x值范围
			279, 699
		},
		{
			//btn_thumb; // 按钮大小
			15, 11
		},
		0, //cursor_y, 游标y值(相对于刻度图片)
		24, //文本y值(相对于刻度图片)
		APP_COLOR_WHITE,
	},
};


#if 0
static FM_UI_PARA_T fm_ui_480x272 =
{
	{
		//TOUCH_UI_RECT
		0, 0, 480, 272
	},
	{
		//FM_UI_MAINWND
		{
			// rt
			0, 0, 480, 272,
		},
		{
			// scale_bg 标尺背景位置
			//0, 60, 480, 45
			//29, 172, 420, 12
			30, 142, 420, 12
		},
		{
			//scale, 刻度x值范围
			//29, 449
			29, 449
		},
		{
			//btn_thumb; // 按钮大小
			//28, 17
			15, 11
		},
		0, //cursor_y, 游标y值(相对于刻度图片)
		24, //文本y值(相对于刻度图片)
		APP_COLOR_WHITE,
	},
};

//
#endif
FM_UI_PARA_T *get_fm_uipara(void)
{
	return &fm_ui_800x480;
}

//////////////////////////////////////////////////////////////////////////
//end of file


