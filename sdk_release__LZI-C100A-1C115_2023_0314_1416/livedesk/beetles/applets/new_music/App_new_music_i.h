#ifndef		__APP_NEW_MUSIC_I_H
#define		__APP_NEW_MUSIC_I_H

#if 1  //必须加载的两个头文件
#include "apps.h"
#include "beetles_app.h"
#endif

#define NEW_MUSIC_ID			(APP_NEWMUSIC_ID+1)	//音乐播放器的framewin窗口id地址
#define NEW_MUSIC_PLAYLIST_ID	(APP_NEWMUSIC_ID+2)	//音乐播放列表的framewin窗口id地址

__s32 app_new_music_cmd2para(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif
