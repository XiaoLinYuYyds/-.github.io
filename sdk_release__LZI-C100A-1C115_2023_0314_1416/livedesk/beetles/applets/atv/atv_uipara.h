/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:atv_uipara.h
* By      	: Andy.zhang
* Func		: desk atv thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __ATV_UIPARA_H_
#define __ATV_UIPARA_H_

#include "app_atv_i.h"


typedef enum
{
  IDX_ATV_MENU_TV_PLAY = 0,
  IDX_ATV_MENU_TV_SEARCH,
  IDX_ATV_MENU_TV_SETTING,
  IDX_ATV_MENU_TV_AVIN,
  IDX_ATV_MENU_MAX
} E_IDX_ATV_MENU;

#define MAX_ITEM_NUM				IDX_ATV_MENU_MAX


typedef struct tag_atv_uipara_pos
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	__s32 reserve;
} atv_uipara_pos_t;

typedef struct tag_atv_uipara_t
{
	atv_uipara_pos_t lyr_atv_menu;
	atv_uipara_pos_t lyr_forground;
	atv_uipara_pos_t lyr_sub_item;

	RECT item_pos[MAX_ITEM_NUM];

	//atv menu
	__s32 item_width;
	__s32 total_item_num;
	__s32 item_per_screen;
	__s32 max_atv_bmp_width;
	__s32 max_atv_bmp_height;

	//sub menu
	__s32 bmp_item_width;
	__s32 bmp_item_height;
	__s32 bmp_line_width;
	__s32 bmp_line_height;
	__s32 bmp_top_width;
	__s32 bmp_top_height;
	__s32 bmp_bottom_width;
	__s32 bmp_bottom_height;
} atv_uipara_t;

atv_uipara_t *atv_get_ui_para(__s32 rotate);


#endif

