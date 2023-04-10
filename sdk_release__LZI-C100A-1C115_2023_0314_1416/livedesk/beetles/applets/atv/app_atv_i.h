/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_atv_i.h
* By      	: Andy.zhang
* Func		: desk atv thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_ATV_I_H_
#define  __APP_ATV_I_H_

#include "apps.h"
#include "beetles_app.h"

#define MAIN_MENU_ID  				(APP_ATV_ID +1)
#define SUB_MENU_ID					(APP_ATV_ID +2)
#define APP_ATV_DLG_ID				(APP_ATV_ID +0x10)

#define GUI_MSG_ATV_CUE_INSERT_AVIN		(GUI_MSG_USER_DEF +1)
#define GUI_MSG_ATV_TO_LINEIN						(GUI_MSG_USER_DEF +2)

#define MASK_ATVFlags_AVINFromHome		(1<<0)
#define MASK_ATVFlags_AVINFromDetect		(1<<1)


void  atv_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif//__APP_HOME_I_H_
