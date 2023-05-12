#ifndef		__APP_NEW_MUSIC_H
#define		__APP_NEW_MUSIC_H

#include "App_new_music_i.h"
#include "App_new_music_ui_para.h"
#include "new_music.h"
#include "new_music_playlist.h"




typedef	struct tag_app_new_music_ctrl
{
	H_WIN	new_music_frm_win;//存放播放器framewin的窗口句柄
	H_LYR	new_music_frm_lyr;//存放播放器framewin的窗口图层

	H_WIN 	h_spectrum_win;	  //存放频谱图framewin窗口
	H_LYR	h_spectrum_lyr;	  //存放频谱图framewin窗口图层
	
	#if 1	//播放列表framewin窗口
	H_WIN	playlist_frm_win;//存放framewin的窗口句柄
	H_LYR	playlist_frm_lyr;//存放framewin的窗口图层
	GUI_FONT *playlist_font;//存放文本
	#endif
	
	GUI_FONT *font;//存放文本
	__s32    root_type;
	__s32	 play_timer_id;
	__u32	lastplay_index;	   //存放TF卡插入播放上一次播放的音乐文件索引id
}app_new_music_ctrl_t;


#endif

