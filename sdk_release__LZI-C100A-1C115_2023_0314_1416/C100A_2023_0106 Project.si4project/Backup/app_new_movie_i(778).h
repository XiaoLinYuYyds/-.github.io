#ifndef		_APP_NEW_MOVIE_I_H
#define		_APP_NEW_MOVIE_I_H

#if 1  //必须加载的两个头文件
#include "apps.h"
#include "beetles_app.h"
#endif











//定义movie的子场景的ID
#define 	MOVIE_SPSC_ID			            0x1
#define 	MOVIE_VOLUME_ID			            (0x9801+100)//0x2
#define 	MOVIE_BRIGHT_ID			            0x4
#define 	MOVIE_PLAYSTA_ID		            0x8
#define 	MOVIE_SUB_SHOW_ID		            0x10
#define 	MOVIE_SUB_SET_ID		            0x20
#define 	MOVIE_PROG_ID       	    		0x40
#define 	MOVIE_BREAKPLAY_DIALOG_ID			0x80
#define 	MOVIE_PLAYERROR_DIALOG_ID			0x100
#define 	MOVIE_PLAYEND_DIALOG_ID	    		0x200
#define     MOVIE_SSET_ID						0x400





//所有子场景id
#define		MOVIE_SUB_SCENE_TYPE_ALL   (MOVIE_SPSC_ID|MOVIE_VOLUME_ID|MOVIE_BRIGHT_ID|MOVIE_PLAYSTA_ID|MOVIE_SUB_SHOW_ID|MOVIE_SUB_SET_ID\
                                      |MOVIE_PROG_ID|MOVIE_BREAKPLAY_DIALOG_ID|MOVIE_PLAYERROR_DIALOG_ID|MOVIE_PLAYEND_DIALOG_ID)

//除了spsc场景和sub_show场景以外的场景id
#define		MOVIE_SUB_SCENE_TYPE_OTHER (MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SPSC_ID)\
                                      &(~MOVIE_SUB_SHOW_ID))













#endif
