/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:app_main.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_HOME_H_
#define  __APP_HOME_H_

#define APP_MULTI_SCREEN_HOME    				APP_HOME

typedef enum
{
  MULTI_SCREEN_HOME_MODE_BACKGROUND = 0,
  MULTI_SCREEN_HOME_MODE_NORMAL,
  MULTI_SCREEN_HOME_MODE_INVALID,
} multi_screen_home_mode_e;

H_WIN app_home_create(root_para_t *para);

//???????????????
void __app_home_change_to_home_bg(void);

extern multi_screen_home_mode_e app_multi_screen_home_get_mode(H_WIN hwin);

#endif// __APP_HOME_H_
