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



typedef struct New_Setting_para
{
	GUI_FONT *msgbox_font;
	H_LYR	  layer;

	H_WIN		layer;		//窗口
	H_WIN		bg_layer;	//背影窗口
	GUI_FONT 	*smenu_font;//文本控件
	__s32			root_type;
	__s8	 		sub_menu_id;
	__s32           focus_item;
} New_Setting_para_t;

H_WIN New_Setting_win_create(H_WIN h_parent, setting_tip_para_t *para);
H_LYR New_Setting_layer_create(RECT *rect);


#endif
