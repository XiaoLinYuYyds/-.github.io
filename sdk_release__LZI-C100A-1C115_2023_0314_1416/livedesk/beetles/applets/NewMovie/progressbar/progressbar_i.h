#ifndef		__PROGRESSBAR_I_H
#define		__PROGRESSBAR_I_H


#if 1  //必须加载的两个头文件
#include "apps.h"
#include "beetles_app.h"
#endif



//定义PROGRESSBAR子场景的ui类型
#define PROGRESSBAR_UI_TYPE_PROG		    0x01//进度条图片
#define PROGRESSBAR_UI_TYPE_CUR_TIME	    0x02//进度条当前时间
#define PROGRESSBAR_UI_TYPE_TOTAL_TIME	    0x04//进度条总时间
#define PROGRESSBAR_UI_TYPE_BG	        	0x08//进度条背景颜色
//PROGRESSBAR的所有ui类型
#define PROGRESSBAR_UI_TYPE_ALL	(PROGRESSBAR_UI_TYPE_PROG\
								 |PROGRESSBAR_UI_TYPE_CUR_TIME\
								 |PROGRESSBAR_UI_TYPE_TOTAL_TIME\
                                 |PROGRESSBAR_UI_TYPE_BG)

//添加按键宏定义功能


__s32  new_movie_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved);


#endif

