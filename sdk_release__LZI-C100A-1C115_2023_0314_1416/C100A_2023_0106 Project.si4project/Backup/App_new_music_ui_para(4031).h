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

typedef	struct 
{
	/*这里面添加存放UI控件的变量*/
	RECT	playlist_area_hlyr;	//播放列表framewin图层大小区域
	RECT	playlist_hrm;		//播放列表framewin窗口大小
	RECT	list_unfocus;		//未选中列表音乐条控件
	RECT	list_focus;			//选中列表音乐条控件
	RECT	txt_rect;			//文本显示控件
}playlist_uipara_t;/*播放列表界面内容的ui坐标和矩形大小参数*/


typedef	struct tag_new_music_uipara_t
{
	/*这里面添加存放UI控件的变量*/
	new_music_rect_t	uipara_hrm;//进度条framewin窗口大小
	new_music_rect_t	uipara_hlyr;//进度条framewin图层大小
	new_music_rect_t	progressbar[2];//进度条控件
	
	playlist_uipara_t	playlist_uipara;//播放列表framewin窗口UI参数
	
	new_music_rect_t	spectrogram;//频谱图
	
}new_music_uipara_t;/*界面内容的ui坐标和矩形大小参数*/
		
new_music_uipara_t	*get_new_music_uipara(__s32 rotate);

#endif
