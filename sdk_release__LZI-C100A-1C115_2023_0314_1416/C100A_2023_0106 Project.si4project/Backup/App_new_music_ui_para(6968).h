#ifndef		__APP_NEW_MUSIC_UI_PARA_H
#define 	__APP_NEW_MUSIC_UI_PARA_H

#include	"App_new_music_i.h"

typedef	struct 
{
	__s32 x;
	__s32 y;
	__s32 width;
	__s32 heght;
}new_music_rect_t;

typedef	struct tag_new_music_uipara_t
{
	/*这里面添加存放UI控件的变量*/
	new_music_rect_t	uipara_hrm;//framewin窗口大小
	new_music_rect_t	uipara_hlyr;//framewin图层大小
	new_music_rect_t	progressbar[2];//进度条控件
	new_music_rect_t	spectrogram;//频谱图
	
}new_music_uipara_t;/*界面内容的ui坐标和矩形大小参数*/
		
new_music_uipara_t	*get_new_music_uipara(__s32 rotate);

#endif
