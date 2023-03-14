#include "App_new_music_ui_para.h"



static new_music_uipara_t	uipara_1024_600 =
{
	{//进度条窗口大小
		(1024-600), 60, 600, 480
	},
	{//进度条图层大小
		0, 0, 600, 480
	},
	{
		{(600-276)/2, (480-5), 276, 5},			//进度条背景
		{(600-276)/2, (480-5), 5, 5}			//进度条移动部分
	},
	{
		{70, 25, 386 , 544},					//音乐列表的图层区域
		{0, 0, 386, 544},						//音乐列表窗口的区域
		{70, 25, 386, 32},						//未选中单首音乐的区域
		{5, 9, 316, 22},						//选中音乐图片区域
		{36, 0, 148 + 136, 38}					//文本文字区域
	},
	{
		(600-276)/2, (470-150), 15, 150			//频谱图
	},

};








new_music_uipara_t	*get_new_music_uipara(__s32 rotate)
{
	SIZE	screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	switch(rotate){
		case 0:{
			if((screen_size.width == 1024) && (screen_size.height == 600))
			return &uipara_1024_600;
		}
		break;
		case 1:
			break;
	}
	
	return &uipara_1024_600;;
}


