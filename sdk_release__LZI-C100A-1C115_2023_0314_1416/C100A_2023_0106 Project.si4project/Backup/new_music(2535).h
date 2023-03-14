#ifndef		__NEW_MUSIC_H
#define		__NEW_MUSIC_H

#include	"App_new_music_i.h"
#include 	"App_new_music_ui_para.h"


typedef struct new_music_rat_para
{
	HRAT   		rat_handle;											//搜索句柄
	__hdle      long_str_handle;									//长字符串滚动句柄
	__u32   	total;												//条目总数
	__u32    	play_index;											//当前播放文件索引号
}new_music_rat_para_t;

typedef struct new_music_scene_para
{
	H_THEME progressbar_bmp;//0为进度条背景，1为进度条移动黄色部分图片
	H_THEME progress_bmp[2];//测试用列	
}new_music_scene_t;

typedef	struct app_new_music_create_para
{
	H_WIN	new_music_hfrm;//存放framewin窗口内部的句柄
	H_LYR	new_music_hlyr;//存放framewin窗口内部的图层
	GUI_FONT *font;//存放文本
	__s32    root_type;//存放USB或TF卡检测标志

	__s32 	auto_play_timmer_id;//定时器id
	rat_media_type_t 	 media_type;	//存放获取到磁盘的文件类型
	root_para_t  		*root_para;		//存放root根目录结构体变量
}new_music_create_para_t;//framewin窗口创建的结构体

typedef	struct new_music_ctrl_para
{
	new_music_create_para_t	*framewin_create_para;//存放framewin窗口的创建参数
	new_music_scene_t	scene_para[2];//存放界面的控件储存变量
	new_music_scene_t	scene_uipara;//测试
	new_music_uipara_t	*new_music_uipara;

	rat_play_mode_e		play_mode;		//播放模式
	new_music_rat_para_t rat;//用于搜索文件
	HTHEME bmp[2];
	H_THEME bmp_music;
}new_music_ctrl_t;//存放framewin窗口的控件以及其他变量等

#endif

