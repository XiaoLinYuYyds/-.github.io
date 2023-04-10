/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "photo_uipara.h"

//////////////////////////////////////////////////////////////////////////
static photo_uipara_t photo_uipara =
{
	{
		//spsc_layer屏幕图层{0, 0, 1024, 600}
		0, 0, LCD_WIDTH, LCD_HEIGHT,
	},
	{
		//sset_main_layer主菜单图层{0, 600-59, 1024, 59}
		0, SSET_MAIN_LAYER_Y, SSET_MAIN_LAYER_W, SSET_MAIN_LAYER_H,
	},
	#if SP_APP_PHOTO_BG
	{
		//sset_origin_layer背景菜单{4, 0, 0, 0}
		4, (MENU_LIST_BOTTOM - SSET_ORIGIN_LAYER_H), SSET_ORIGIN_LAYER_W, SSET_ORIGIN_LAYER_H,
	},
	#endif
	{
		//sset_bg_muisc_layer音乐菜单{0, 0, 0, 0}
		185, (MENU_LIST_BOTTOM - SSET_BG_MUSIC_LAYER_H), SSET_BG_MUSIC_LAYER_W, SSET_BG_MUSIC_LAYER_H,
	},
	{
		//sset_zoom_layer放大菜单{0, 0, 0, 0}
		530, 190, SSET_ZOOM_LAYER_W, SSET_ZOOM_LAYER_H,
	},
	{
		//sset_rotate_layer旋转菜单{0, 0, 0, 0}
		530, 112, SSET_ROTATE_LAYER_W, SSET_ROTATE_LAYER_H,
	},
	{
		//sset_brightness_layer亮度菜单{0, 0, 0, 0}
		530, 190, SSET_BRIGHTNESS_LAYER_W, SSET_BRIGHTNESS_LAYER_H,
	},
	{
		//sset_slideshow_layer特效菜单{0, 0, 0, 0}
		283, (MENU_LIST_BOTTOM - SSET_SLIDESHOW_LAYER_H), SSET_SLIDESHOW_LAYER_W, SSET_SLIDESHOW_LAYER_H,
	},
	{
		//sset_interval_layer速度菜单{0, 0, 0, 0}
		378, (MENU_LIST_BOTTOM - SSET_INTERVAL_LAYER_H), SSET_INTERVAL_LAYER_W, SSET_INTERVAL_LAYER_H,
	},
	{
		//sset_scale_layer比例菜单{0, 0, 0, 0}
		378, (MENU_LIST_BOTTOM - SSET_SCALE_LAYER_H), SSET_SCALE_LAYER_W, SSET_SCALE_LAYER_H,
	},
	{
		//bmp_big_pause暂停
		0, 0, BMP_BIG_PAUSE_W, BMP_BIG_PAUSE_H,
	},
	{
		//bmp_play_status播放
		0, 0, BMP_BIG_PAUSE_W, BMP_BIG_PAUSE_H,
	},
	#if SP_APP_PHOTO_BG
	{
		//bmp_set_bg
		0, 0, 400, 59,
	},
	#endif
	{
		//bmp_set_item菜单图标大小
		0, 0, MENU_BTN_W, MENU_BTN_H,
	},
	{
		//bmp_set_bar_top设置菜单条顶部图标
		0, 0, MENU_LIST_BTN_W, 3,
	},
	{
		//bmp_set_bar_bottom设置菜单条底部图标
		0, 0, MENU_LIST_BTN_W, 3,
	},
	{
		//bmp_set_bar设置菜单条主体图标
		0, 0, MENU_LIST_BTN_W, MENU_LIST_BTN_H,
	},
	{
		//bmp_set_bar_up
		0, 0, MENU_LIST_BTN_W, 5,
	},
	{
		//bmp_set_bar_down
		0, 0, MENU_LIST_BTN_W, 5,
	},
	{
		//bmp_set_line
		0, 0, 0, 0
	},
	{
		//bmp_set_block
		0, 0, 30, 30,//???
	},
	{
		//bmp_set_vertical_line
		0, 0, 1, 86,//???
	},
	{
		//bmp_set_horizontal_line
		0, 0, 230, 1,//???
	},
	{
		//bmp_zoom_bar放大尺子
		0, 0, 22, 104,
	},
	{
		//bmp_zoom_block放大刻度
		0, 0, 10, 1,
	},
	{
		//bmp_rotate旋转图标
		0, 0, 66, 66,
	},
	{
		//bmp_bright_bar亮度尺子
		0, 0, 12, 108,
	},
	{
		//bmp_bright_block亮度刻度
		0, 0, 11, 9,
	},
	{
		//bmp_volume音量
		0, 0, 15, 13,
	},
	{
		//bmp_slide_bg
		10, 10, 356, 24,//???
	},
	{
		//bmp_slide_block
		0, 0, 5, 4,
	},
	{
		//bmp_slide_bar
		0, 0, 255, 4,
	}
};

photo_uipara_t *get_photo_uipara(void)
{
	return (&photo_uipara);
}

