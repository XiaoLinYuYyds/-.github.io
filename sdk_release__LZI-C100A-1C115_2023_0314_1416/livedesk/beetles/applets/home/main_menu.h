/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: main_menu.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __MAIN_MENU_H_
#define __MAIN_MENU_H_

#include "app_home_i.h"
#include "home_uipara.h"


#define SCN_W 					1024
#define SCN_H 					600

#define HBAR_H 				20
#define ITEM_W 					150
#define ITEM_H 					150
#define W_ITEM_FOCUS			190
#define H_ITEM_FOCUS			190

#if SP_APP_ATV
#define ITEM_PER_LINE 4
#else
#define ITEM_PER_LINE 4
#endif
#define ITEM_PER_COL 2


typedef struct tag_mmenu_uipara_pos
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	__s32 reserve;
} mmenu_uipara_pos_t;

#define MAX_PARTITION		11		//可以有11 个盘

#define ID_SWITCH_ITEM		0
#define ID_OP_SUB_UP			1
#define ID_OP_SUB_DOWN		2
#define ID_OP_SUB_ENTER	3

typedef struct tag_main_menu
{
	__s32 	focus_id;
	H_WIN	layer;
	H_WIN	bg_layer;
	GUI_FONT *mmenu_font;
} mmenu_para_t;

__s32 main_menu_win_create(H_WIN h_parent, mmenu_para_t *para);
void main_menu_win_delete(H_WIN mm_win);
void main_menu_uninit_res(H_WIN mm_win);

#endif
