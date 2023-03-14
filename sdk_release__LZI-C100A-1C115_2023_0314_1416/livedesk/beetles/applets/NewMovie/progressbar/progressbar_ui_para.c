/*#ifndef __PROGRESSBAR_UI_PARA_H
#define __PROGRESSBAR_UI_PARA_H
*/
#include "progressbar_ui_para.h"


static progress_uipara_t uipara_400_240 =
{
	1,
	{
		(480 - 364) / 2, 40, 364, 24 //(400-364)/2, 40, 364, 16 进度条图层大小，宽为364，高为24
	},
	{
		0, 0, 364, 24				//framewin窗口大小，宽为364，高为24
	},
	{
		110, (24 - 4) / 2, 146, 4 //进度条背景图片，宽为146，高为4
	},
	{
		110, (24 - 4) / 2, 5, 4 //进度条黄色移动小段宽为5，高为4
	},
};
static progress_uipara_t uipara_1024_600 =
{
	1,
	{
		(1024 - 364) / 2, 560, 364, 24 //(400-364)/2, 40, 364, 16 进度条的图层显示位置坐标和大小，宽为364，高为24
	},
	{
		0, 0, 364, 24				//在进度条图层上面的framewin窗口大小，宽为364，高为24
	},
	{
		110, (24 - 4) / 2, 146, 4 //进度条背景图片，宽为146，高为4
	},
	{
		110, (24 - 4) / 2, 5, 4 //进度条黄色移动小段宽为5，高为4
	},
	{//当前播放时间位置
		{
			5, 0, 25, 24
		},
		{
			41, 0, 25, 24
		},
		{
			77, 0, 25, 24
		},
	},
	{//当前时间分隔点位置
		{
			33, 0, 5, 24
		},
		{
			69, 0, 5, 24
		}
	},
	{//总播放时间位置
		{
			263, 0, 25, 24
		},
		{
			299, 0, 25, 24
		},
		{
			335, 0, 25, 24
		},
	},
	{//总播放时间分隔点位置
		{
			291, 0, 5, 24
		},
		{
			327, 0, 5, 24
		}
	},
	/*{ID_MOVIE_PROG_BG_UF_BMP},{ID_MOVIE_PROG_CURSOR_UF_BMP},*/
	
};

/*进度条ui坐标和大小参数获取*/
progress_uipara_t *progress_get_uipara(__s32 rotate)
{
	SIZE	screen;//屏幕大小
	dsk_display_get_size(&screen.width, &screen.height);
	//__wrn("screen.w = %d, screen.h = %d\n",screen.width,screen.height);//1024*600
	switch(rotate){
		case 1:{
			if((screen.width == 480) && (screen.height == 272)){
				return &uipara_400_240;
			}
			else if((screen.width == 720) && (screen.height == 576)){
				return NULL;
			}
			else if((screen.width == 1024) && (screen.height == 600)){
				return &uipara_1024_600;
			}

		}
		case 0:
			break;
				
	}
	return &uipara_400_240;
}
/*
#endif*/
