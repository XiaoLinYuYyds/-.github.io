
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: volume_ui.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __COM_VOLUME_UIPARA_H_
#define  __COM_VOLUME_UIPARA_H_

#include "beetles_app.h"

#define COM_VOLUME_MAX_ICON_RES_NUM 2

typedef struct
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	__s32 res_id[COM_VOLUME_MAX_ICON_RES_NUM];
	HTHEME res_hdl[COM_VOLUME_MAX_ICON_RES_NUM];
} com_volume_rect_t;

typedef enum
{
  com_volume_icon_lvolume = 0,
  com_volume_icon_rvolume,
  com_volume_icon_prog_bg,
  com_volume_icon_prog_cursor,
  com_volume_icon_num
} com_volume_icon_t;

typedef struct
{
	__s32 pipe;
	com_volume_rect_t uipara_lyr;
	com_volume_rect_t uipara_frm;
	com_volume_rect_t uipara_volume_bg;
	com_volume_rect_t uipara_icon[com_volume_icon_num];
	com_volume_rect_t uipara_volume_text;
} com_volume_uipara_t;

com_volume_uipara_t *com_volume_get_uipara(__s32 rotate);

#endif
