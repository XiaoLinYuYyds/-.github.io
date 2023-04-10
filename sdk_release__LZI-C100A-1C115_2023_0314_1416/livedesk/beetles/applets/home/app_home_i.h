/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_main_i.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_HOME_I_H_
#define  __APP_HOME_I_H_

#include "apps.h"
#include "beetles_app.h"

#define MAIN_MENU_ID  	(APP_HOME_ID +1)
#define SUB_MENU_ID		(APP_HOME_ID +2)
#define MSGBOX_ID		(APP_HOME_ID +3)
#define SUB_MENUNUMBER_ID		(APP_HOME_ID +4)	//窗口ID

#define MOVIE_ID_SET_SHOWERR		(GUI_MSG_USER_DEF + 1)

#if (SP_APP_CALENDAR || SP_APP_ATV)
#define MAX_ITEM_NUM	8			
#else
#define MAX_ITEM_NUM	8
#endif

extern __s32 SUB_MENU_CUT;
void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif//__APP_HOME_I_H_
