/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    :  ebook_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "app_loading_uipara.h"
#define SCREEN_WIDTH	800//320
#define SCREEN_HEIGHT	480//240

static app_loading_uipara_t app_loading_uipara;

app_loading_uipara_t *get_app_loading_uipara(void)
{
	app_loading_uipara.loading_bmp_pos.x = (SCREEN_WIDTH - 30) / 2;
	app_loading_uipara.loading_bmp_pos.y = (SCREEN_HEIGHT - 30) / 2;
	app_loading_uipara.loading_bmp_pos.w = 30;
	app_loading_uipara.loading_bmp_pos.h = 30;
	app_loading_uipara.loading_txt_pos.x = (SCREEN_WIDTH - 30) / 2 - 30;
	app_loading_uipara.loading_txt_pos.y = (SCREEN_HEIGHT - 30) / 2 + 30;
	app_loading_uipara.loading_txt_pos.w = 100;
	app_loading_uipara.loading_txt_pos.h = 38;
	return &app_loading_uipara;
}

