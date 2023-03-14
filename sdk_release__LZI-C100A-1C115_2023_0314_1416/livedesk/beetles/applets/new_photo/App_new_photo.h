#ifndef		__APP_NEW_PHOTO_H
#define		__APP_NEW_PHOTO_H

#include	"App_new_photo_i.h"


//////new for bmp res
typedef load_bmp_res_t                         NEW_PHOTO_BMP_RES_T;

typedef struct new_photo_rat_para
{
	HRAT   		rat_handle;											//搜索句柄
	__hdle      long_str_handle;									//长字符串滚动句柄
	__u32   	total;												//条目总数
	__u32    	play_index;											//当前播放文件索引号
}new_photo_rat_para_t;//rat模块搜索文件用的结构体

typedef	struct app_new_photo_ctrl
{
	H_WIN    lyr_photo_spsc;
	H_WIN    h_photo_spsc;
	NEW_PHOTO_BMP_RES_T err_pic;//void     *err_pic;
	GUI_FONT *font;//存放文本
	__s32    root_type;//存放SD卡或USB类型，2为SD卡
	__s32	photo_index;//列表传入的图片文件索引id
	new_photo_rat_para_t rat;//媒体类型文件搜索
}new_photo_ctrl_t;


#endif

