#ifndef		_NEW_MUSIC_PLAYLIST_H
#define		_NEW_MUSIC_PLAYLIST_H

#include	"App_new_music_i.h"
#include 	"App_new_music_ui_para.h"


#define		MEMDEV_ONOFF	1	//伪内存设备创建开关；1开，0关

#if 1	//发送到父窗口的按键操作控制命令宏定义
#define		send_cmd_play_prev_to_PlayDev	0//从播放列表发送播放音乐命令
#define		send_cmd_play_next_to_PlayDev	1//从播放列表发送播放音乐命令

#endif
#if 1	//本窗口的按键操作控制命令宏定义
#define	playlist_key_prev	105
#define	playlist_key_next	106

#endif
typedef enum	//用户播放列表按键操作控制命令
{
  MSG_PLAYLIST_OP_UP = GUI_MSG_USER_DEF,//0X3000
  MSG_PLAYLIST_OP_DOWN,
  MSG_PLAYLIST_OP_ENTER,
} playlist_msg_t;

typedef struct playlist_rat_para
{
	HRAT   		rat_handle;											//搜索句柄
	__hdle      long_str_handle;									//长字符串滚动句柄
	__u32   	total;												//条目总数
	__u32    	play_index;											//当前播放文件索引号
}playlist_rat_para_t;//rat模块搜索文件用的结构体

typedef struct playlist_scene_para
{
	H_THEME focus_bmp;//选中部分图片
	H_THEME progress_bmp[2];//测试用列	
}playlist_scene_t;//界面图片资源结构体

typedef	struct app_playlist_create_para
{
	H_WIN	playlist_hfrm;	//存放framewin窗口内部的句柄
	H_LYR	playlist_hlyr;	//存放framewin窗口内部的图层
	GUI_FONT *font;			//存放文本
	__s32    root_type;		//存放USB或TF卡检测标志

	//__s32 	auto_play_timmer_id;//定时器id
	#if 1//播放列表的音乐文件选择操作
	__s32	CurFocus_id;	//当前选中的音乐文件索引id
	__s32	LastFocus_id;	//上一次选中的音乐文件索引id
	char 	last_filename[RAT_MAX_FULL_PATH_LEN];//上一次选中的音乐文件名
	char 	file_number[10];	//存放音乐文件的文件序号
	#endif
	
	rat_media_type_t 	 media_type;	//存放获取到磁盘的文件类型
	root_para_t  		*root_para;		//存放root根目录结构体变量
}playlist_create_para_t;//framewin窗口创建的结构体

typedef	struct playlist_ctrl_para
{
	playlist_create_para_t	*framewin_create_para;//存放framewin窗口的创建参数
	playlist_scene_t	scene_para[2];//存放界面的控件储存变量
	playlist_scene_t	scene_uipara;//测试
	new_music_uipara_t	*playlist_uipara;//ui界面控件的大小和位置参数

	rat_play_mode_e		play_mode;		//播放模式
	playlist_rat_para_t rat;//用于搜索文件

}playlist_ctrl_t;//存放framewin窗口的控件以及其他变量等





H_WIN	playlist_frmwin_create(H_WIN h_parent, playlist_create_para_t *para);





#endif

