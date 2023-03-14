/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: msgbox.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __NEW_SETTING_H_
#define  __NEW_SETTING_H_

#include "New_Setting_para.h"
#include "number_menu_i.h"
#if 0//旧的结构体
typedef struct New_Setting_para
{
	GUI_FONT *msgbox_font;
	H_LYR	  layer1;

	H_WIN		layer;		//窗口
	H_WIN		bg_layer;	//背影窗口
	GUI_FONT 	*smenu_font;//文本控件
	__s32			root_type;
	__s8	 		sub_menu_id;
	__s32           focus_item;
	__s32 			focus_id;
} New_Setting_para_t;
typedef struct New_Setting_ctrl
{
	New_Setting_para_t *New_Settings_para;

	__s32 focus_txt_color;
	__s32 unfocus_txt_color;
	H_THEME New_Setting_bmp;
	H_THEME New_Setting_f_bmp;

	char  new_number_size[128];
	New_Settings_uipara_t *New_Settings_uipara;
}
New_Setting_ctrl_t;
#endif
typedef struct new_tag_setting_para
{
	GUI_FONT *font;

	H_LYR layer;
	__s32 focus_id;
	__s32 old_focus;

} new_setting_para_t;
typedef struct New_Setting_ctrl
{
	new_setting_para_t *New_Settings_para;

	__s32 focus_txt_color;
	__s32 unfocus_txt_color;
	H_THEME New_Setting_bmp;
	H_THEME New_Setting_f_bmp;

	char  new_number_size[128];
	New_Settings_uipara_t *New_Settings_uipara;
}
New_Setting_ctrl_t;


H_WIN New_Setting_app_win_create(H_WIN h_parent, new_setting_para_t *para);//窗口创建函数
H_LYR New_Setting_app_layer_create(RECT *rect);//图层创建函数，


#endif
