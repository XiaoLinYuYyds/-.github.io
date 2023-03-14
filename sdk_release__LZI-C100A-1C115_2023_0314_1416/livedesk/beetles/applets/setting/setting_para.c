/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              LiveTouch Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : ebook
* File    : ebook_uipara.c
*
* By      : Bayden.chen
* Version : v1.0
* Date    : 2009-12-25 14:53:05
**********************************************************************************************************************
*/

#ifndef __SETTING_PARA_H__
#define __SETTING_PARA_H__
#include "setting_para.h"


static tips_uipara_t setting_tips_uipara_800_480 =
{
	{0, 0},	//tips_bmp_pos//ÌáÊ¾¿ò±³¾°Í¼×ø±êÎ»ÖÃ
	{0, 0, 191, 20},	//title pos//ÌáÊ¾¿òtitle×ø±êÎ»ÖÃ
	{32, 28},	//tip_tsize_pos//×Ü´ÅÅÌÈÝÁ¿×ø±êÎ»ÖÃ
	{32, 50},	//tip_fsize_pos//Ê£Óà´ÅÅÌÈÝÁ¿×ø±êÎ»ÖÃ
	{(SET_TIPS_W -TIPS_YES_NO_BMP_WIDTH) >>1, 75}	//tip_ok_bmp_pos//OK ×Ö·ûÏÔÊ¾µ×Í¼×ø±ê

};

static set_general_para setting_general_uipara_800_480 =
{
	{0, 235, 800, 37}, //{0,203,400,37},  //toolbar_rect
	{30, 30}, //{11,30},		  //bmp_big_setting_pos			
	{10, 247, 30, 16}, //{10,215,30,16},// bmp_prompt_rect;
	{45, 244},		//  string_prompt_pos;
	{30, 552, 32 * 16, 32},		//RECT  string_detail_prompt_rect;
	{30, 552, 32 * 16, 32},		//RECT  string_detail_prompt_rect_ex;
};

static set_power_para setting_power_uipara_800_480 =
{
	{0, 235, 800, 37}, //toolbar_rect
	{30, 30},		 //bmp_big_setting_pos
	{10, 247, 30, 16}, // bmp_prompt_rect;
	{45, 244},		//  string_prompt_pos;
	{30, 440, 140, 32},		//RECT  string_detail_prompt_rect;
	{10, 440, 220, 32},		//RECT  string_detail_prompt_rect_ex;
	{
		{
			//item0;
			{150 + 120, 24},		// bmp_focus_pos;
			{150 + 120, 24},		// bmp_unfocus_pos;
			{270 + 120, 32},		//bmp_left_pos;
			{360 + 120, 32},		//bmp_right_pos;
			{157 + 120, 24},		//string_title_pos;
			{276 + 120, 30, 84 + 100, 24} //string_content_rect;
		},
		{
			//item1;
			{150 + 120, 54},		// bmp_focus_pos;
			{150 + 120, 54},		// bmp_unfocus_pos;
			{270 + 120, 62},		//bmp_left_pos;
			{360 + 120, 62},		//bmp_right_pos;
			{157 + 120, 54},		//string_title_pos;
			{276 + 120, 54, 84 + 100, 30} //string_content_rect;
		},
		{
			//item2;
			{150 + 120, 84},		// bmp_focus_pos;
			{150 + 120, 84},		// bmp_unfocus_pos;
			{270 + 120, 92},		//bmp_left_pos;
			{360 + 120, 92},		//bmp_right_pos;
			{157 + 120, 84},		//string_title_pos;
			{276 + 120, 78, 84 + 100, 30} //string_content_rect;
		}
	}

};

static tips_uipara_t setting_tips_uipara_400_240 =
{
	{0, 0},	//tips_bmp_pos
	{0, 0, 191, 20},	//title pos
	{15, 28},	//tip_tsize_pos
	{15, 50},	//tip_fsize_pos
	{61, 72}	//tip_ok_bmp_pos

};

static set_general_para setting_general_uipara_400_240 =
{
	{0, 235, 480, 37}, //{0,203,400,37},  //toolbar_rect
	{30, 30}, //{11,30},		  //bmp_big_setting_pos
	{10, 247, 30, 16}, //{10,215,30,16},// bmp_prompt_rect;
	{45, 244},		//  string_prompt_pos;
	{30, 162, 100, 32},		//RECT  string_detail_prompt_rect;
	{0, 162, 140, 32},		//RECT  string_detail_prompt_rect_ex;
	{
		{
			//item0;
			{150 + 20, 24},		// bmp_focus_pos;
			{150 + 20, 24},		// bmp_unfocus_pos;
			{270 + 20, 32},		//bmp_left_pos;
			{360 + 20, 32},		//bmp_right_pos;
			{157 + 20, 24},		//string_title_pos;
			{276 + 20, 25, 84, 30} //string_content_rect;
		},
		{
			//item1;
			{150 + 20, 54},		// bmp_focus_pos;
			{150 + 20, 54},		// bmp_unfocus_pos;
			{270 + 20, 62},		//bmp_left_pos;
			{360 + 20, 62},		//bmp_right_pos;
			{157 + 20, 54},		//string_title_pos;
			{276 + 20, 54, 84, 30} //string_content_rect;
		},
		{
			//item2;
			{150 + 20, 84},		// bmp_focus_pos;
			{150 + 20, 84},		// bmp_unfocus_pos;
			{270 + 20, 92},		//bmp_left_pos;
			{360 + 20, 92},		//bmp_right_pos;
			{157 + 20, 84},		//string_title_pos;
			{276, 84, 84, 30} //string_content_rect;
		},
		{
			//item3;
			{150 + 20, 114},		// bmp_focus_pos;
			{150 + 20, 114},		// bmp_unfocus_pos;
			{270 + 20, 114 + 8},		//bmp_left_pos;
			{360 + 20, 114 + 8},		//bmp_right_pos;
			{157 + 20, 114},		//string_title_pos;
			{276 + 20, 114, 100, 30} //string_content_rect;
		},

		{
			//item4;
			{150 + 20, 144},		// bmp_focus_pos;
			{150 + 20, 144},		// bmp_unfocus_pos;
			{270 + 20, 144 + 8},		//bmp_left_pos;
			{360 + 20, 144 + 8},		//bmp_right_pos;
			{157 + 20, 144},		//string_title_pos;
			{276 + 20, 144, 100, 30} //string_content_rect;
		},
		{
			//item5;
			{150 + 20, 174},		// bmp_focus_pos;
			{150 + 20, 174},		// bmp_unfocus_pos;
			{270 + 20, 174 + 8},		//bmp_left_pos;
			{360 + 20, 174 + 8},		//bmp_right_pos;
			{157 + 20, 174},		//string_title_pos;
			{276 + 20, 174, 100, 30} //string_content_rect;
		},
		{
			//item6;
			{150 + 20, 205},		// bmp_focus_pos;
			{150 + 20, 205},		// bmp_unfocus_pos;
			{270 + 20, 205 + 8},		//bmp_left_pos;
			{360 + 20, 205 + 8},		//bmp_right_pos;
			{157 + 20, 205},		//string_title_pos;
			{276 + 20, 205, 100, 30} //string_content_rect;
		},
		{
			//item6;
			{150 + 20, 235},		// bmp_focus_pos;
			{150 + 20, 235},		// bmp_unfocus_pos;
			{270 + 20, 235 + 8},		//bmp_left_pos;
			{360 + 20, 235 + 8},		//bmp_right_pos;
			{157 + 20, 235},		//string_title_pos;
			{276 + 20, 235, 100, 30} //string_content_rect;
		}
	}
};

static set_power_para setting_power_uipara_400_240 =
{
	{0, 235, 480, 37}, //toolbar_rect
	{30, 30},		 //bmp_big_setting_pos
	{10, 247, 30, 16}, // bmp_prompt_rect;
	{45, 244},		//  string_prompt_pos;
	{30, 162, 100, 32},		//RECT  string_detail_prompt_rect;
	{0, 162, 220, 32},		//RECT  string_detail_prompt_rect_ex;
	{
		{
			//item0;
			{150 + 20, 24},		// bmp_focus_pos;
			{150 + 20, 24},		// bmp_unfocus_pos;
			{270 + 20, 32},		//bmp_left_pos;
			{360 + 20, 32},		//bmp_right_pos;
			{157 + 20, 24},		//string_title_pos;
			{276 + 20, 30, 84, 24} //string_content_rect;
		},
		{
			//item1;
			{150 + 20, 54},		// bmp_focus_pos;
			{150 + 20, 54},		// bmp_unfocus_pos;
			{270 + 20, 62},		//bmp_left_pos;
			{360 + 20, 62},		//bmp_right_pos;
			{157 + 20, 54},		//string_title_pos;
			{276 + 20, 54, 84, 30} //string_content_rect;
		},
		{
			//item2;
			{150 + 20, 84},		// bmp_focus_pos;
			{150 + 20, 84},		// bmp_unfocus_pos;
			{270 + 20, 92},		//bmp_left_pos;
			{360 + 20, 92},		//bmp_right_pos;
			{157 + 20, 84},		//string_title_pos;
			{276 + 20, 78, 84, 30} //string_content_rect;
		}
	}
};

set_general_para *setting_get_general_para(SIZE screen_size)
{
	if((screen_size.width == 800) && (screen_size.height == 480))
	{
		return &setting_general_uipara_800_480;
	}
	else if((screen_size.width == 480) && (screen_size.height == 272))
	{
		return &setting_general_uipara_400_240;
	}
	else
	{
		__msg("ERR: NULL ui para..\n");
		return  &setting_general_uipara_800_480;
	}
}
set_power_para *setting_get_power_para(SIZE screen_size)
{
	__s32 			screen_width;
	__s32 			screen_height;

	if((screen_size.width == 800) && (screen_size.height == 480))
	{
		return &setting_power_uipara_800_480;
	}
	else if((screen_size.width == 480) && (screen_size.height == 272))
	{
		return &setting_power_uipara_400_240;
	}
	else
	{
		__msg("ERR: NULL ui para..\n");
		return  &setting_power_uipara_800_480;
	}
}

tips_uipara_t *setting_get_tips_para(SIZE screen_size)
{
	__s32			screen_width;
	__s32			screen_height;

	if((screen_size.width == 800) && (screen_size.height == 480))
	{
		return &setting_tips_uipara_800_480;
	}
	else if((screen_size.width == 480) && (screen_size.height == 272))
	{
		return &setting_tips_uipara_400_240;
	}
	else
	{
		__msg("ERR: NULL ui para..\n");
		return	&setting_tips_uipara_800_480;
	}
}


#endif;


