#ifndef	__APP_NEW_EBOOK_H
#define	__APP_NEW_EBOOK_H


#include "App_new_ebook_i.h"
#include "App_new_ebook_ui_para.h"

#include "mbook_decode.h"















typedef struct new_ebook_rat_para
{
	HRAT   		rat_handle;											//搜索句柄
	__hdle      long_str_handle;									//长字符串滚动句柄
	__u32   	total;												//条目总数
	__u32    	play_index;											//当前播放文件索引号
}new_ebook_rat_para_t;//rat模块搜索文件用的结构体


typedef struct tag_app_new_ebook_ctrl
{
	H_WIN	h_new_ebook_frmwin;	//存放电子书显示的framewin窗口句柄
	H_LYR	new_ebook_frmlyr;	//存放电子书显示的framewin窗口图层

	GUI_FONT	*font;//存放文本
	__s32 	root_type;//存放TF卡或USB
	new_ebook_rat_para_t rat;

	#if 1
	H_DECODE            mbook;//解码
	__decode_config_t   config;		/* monkey core show config，模块核心显示控制*/
	__s32	cur_page;	//当前页数
	__s32	total_page;	//总页数
	
	#endif
}app_new_ebook_ctrl_t;










#endif

