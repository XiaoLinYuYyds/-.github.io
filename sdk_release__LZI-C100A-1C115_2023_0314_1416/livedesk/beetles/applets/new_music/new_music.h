#ifndef		__NEW_MUSIC_H
#define		__NEW_MUSIC_H

#include	"App_new_music_i.h"
#include 	"App_new_music_ui_para.h"

//用于设置绘制文本时的固定配置
#define			NewMusicSetTransColor()	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	\
	GUI_SetBkColor(0x00000000);\
	GUI_SetColor(GUI_WHITE)

#if 1//按键控制宏定义
#define	new_music_play_key_exit		0//播放器退出
#endif

typedef enum	//用户播放器按键操作控制命令
{
  MSG_PLAY_OP_UP = GUI_MSG_USER_DEF,//0X3000
  MSG_PLAY_OP_DOWN,
  MSG_PLAY_OP_ENTER,
  MSG_PLAY_OP_PLAY_BY_INDEX,	//通过播放列表索引播放音乐文件
} play_msg_t;//用户播放器按键操作控制命令结构体

typedef struct new_music_rat_para
{
	HRAT   		rat_handle;					//搜索句柄
	__hdle      long_str_handle;			//长字符串滚动句柄
	__u32   	total;						//条目总数
	__u32    	play_index;					//当前播放文件索引号
}new_music_rat_para_t;//rat模块搜索文件用的结构体

typedef struct new_music_scene_para
{
	H_THEME progressbar_bmp;//0为进度条背景，1为进度条移动黄色部分图片
	H_THEME progress_bmp[2];//测试频谱图用列	
}new_music_scene_t;//界面图片资源结构体

typedef	struct app_new_music_create_para
{
	H_WIN	new_music_hfrm;		//存放framewin窗口内部的句柄
	H_LYR	new_music_hlyr;		//存放framewin窗口内部的图层
	GUI_FONT *font;				//存放文本
	__s32    root_type;			//存放USB或TF卡检测标志，TF卡为2

	__u8 	auto_play_timmer_id;//播放器定时器id，0x01
	__u8 	key_timmer_id;		//按键检测定时器id，0x02
	__u8 	ff_rr_timmer_id;	//快进或后退定时器id，0x03
	
	rat_media_type_t 	 media_type;	//存放获取到磁盘的媒体文件类型
	root_para_t  		*root_para;		//存放传入的root根目录结构体变量
	__u32	curplay_index;				//存放当前播放的音乐文件索引id
	__s32	lastplay_index;				//存放上一次播放的音乐文件索引id
}new_music_create_para_t;//framewin窗口创建的结构体

typedef	struct new_music_ctrl_para
{
	new_music_create_para_t	*framewin_create_para;	//存放播放器framewin窗口创建的参数
	new_music_scene_t	scene_para[2];				//存放界面的进度条控件储存变量
	new_music_scene_t	scene_uipara;				//用于测试的频谱图
	new_music_uipara_t	*new_music_uipara;			//ui界面控件的大小和位置参数

	rat_play_mode_e		play_mode;					//存放音乐的播放模式，循环播放所有、单曲循环、顺序播放、逆序播放等
	new_music_rat_para_t rat;						//用于搜索文件
	HTHEME bmp[2];
	H_THEME bmp_music;
}new_music_ctrl_t;//存放framewin窗口的控件以及其他变量等


H_WIN	new_music_frmwin_create(H_WIN h_parent, new_music_create_para_t *para);//播放器framewin窗口创建

__s32 new_music_ui_scene_delete(void *handle);

#endif

