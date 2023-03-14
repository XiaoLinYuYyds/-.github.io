#ifndef		__APP_NEW_MUSIC_H
#define		__APP_NEW_MUSIC_H

#include "App_new_music_i.h"
#include "App_new_music_ui_para.h"
#include "new_music.h"




typedef	struct tag_app_new_music_ctrl
{
	H_WIN	new_music_frm_win;//存放framewin的窗口句柄
	H_LYR	new_music_frm_lyr;//存放framewin的窗口图层
	
	GUI_FONT *font;//存放文本
	__s32    root_type;
	__s32	 play_timer_id;
}app_new_music_ctrl_t;


#endif

