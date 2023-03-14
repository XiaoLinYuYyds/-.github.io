/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: atv_menu.h
* By      	: Andy.zhang
* Func		: desk atv thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __ATV_MENU_H_
#define __ATV_MENU_H_

#include "app_atv_i.h"
#include "atv_uipara.h"

#define SCN_W 				_W_SCR_
#define SCN_H				_H_SCR_

#define HBAR_H 				20
#define ITEM_W 					90
#define ITEM_H 					90
#define W_ITEM_FOCUS			130
#define H_ITEM_FOCUS			130


#define ID_SWITCH_ITEM		0
#define ID_OP_SUB_UP			1
#define ID_OP_SUB_DOWN		2
#define ID_OP_SUB_ENTER	3

typedef struct tag_atv_menu
{
	H_WIN h_parent;
	H_WIN	hLyr_frame;
	GUI_FONT *mmenu_font;
	__u32	m_maskFlags;
} mmenu_para_t;

typedef enum
{
  m_eMenuDepth_0 = 0,
  m_eMenuDepth_1,
  m_eMenuDepth_2,
  m_eMenuDepth_3,
  m_eMenuDepth_max
} E_MENU_DEPTH;

typedef enum
{
	m_eAtvMenuLyr_tvInfo = 0,
	m_eAtvMenuLyr_menuList,
	m_eAtvMenuLyr_num,
	m_eAtvMenuLyr_searchTv,
	m_eAtvMenuLyr_tvSys,
	m_eAtvMenuLyr_total
}E_ATV_MENU_LYR;

typedef struct tag_mmenu_attr
{
	H_WIN 		h_parent;
	H_LYR			hLyr_frame;
	H_LYR			hLyr_sub[m_eAtvMenuLyr_total];
	GUI_FONT 	*font;
	
	__s32 			focus_txt_color;
	__s32 			unfocus_txt_color;
	
	HTHEME 	focus_bmp[MAX_ITEM_NUM];
	HTHEME 	unfocus_bmp[MAX_ITEM_NUM];
	char 		item_str[MAX_ITEM_NUM][128];
	RECT 		item_pos[MAX_ITEM_NUM];

	HTHEME	hTheme_MHz, hTheme_point, hTheme_div, hTheme_percent,
	            	hTheme_procbarBody, hTheme_procbarPoint,
	            	hTheme_TVLogo;
	HTHEME	hTheme_num[10];
	HTHEME	hTheme_numEx[10];
	HTHEME	hTheme_audioSys[m_eTvSystem_max][2];
	HTHEME	hTheme_tvSys_bg, hTheme_tvSys_finger, hTheme_tvSys_fingerClr;

	__u8 		paint_flag;
	__s32 		focus_item;
	__s8			menuDepth;
	__bool		drawTvLogo;
	
	__bool		m_closeScreenDone_tcon;
	
	S_TUNER_INFO 	sTunerInfo;
	reg_atv_para_t	*pRegParamBak;
}
mmenu_attr_t;
	// for paint_flag:
	#define MASK_PaintFlag_MEMDEV_Nonuse					(1<<0)
	#define MASK_PaintFlag_RTL										(1<<1)	// Right To Left
	

typedef enum
{
	m_eTvMenuIdx_system = 0,
	m_eTvMenuIdx_autoSearch,
	m_eTvMenuIdx_max,
	m_eTvMenuIdx_manualSearch,
	m_eTvMenuIdx_addFreq,
} E_TV_MENU_IDX;

#define MASK_PaintTVMenu_TV 		((1<<m_eTvMenuIdx_system)|(1<<m_eTvMenuIdx_autoSearch)|(1<<m_eTvMenuIdx_addFreq))
#define MASK_PaintTVMenu_AVIN 	(1<<m_eTvMenuIdx_system)
	


__s32 atv_menu_win_create(H_WIN h_parent, mmenu_para_t *para);

#endif
