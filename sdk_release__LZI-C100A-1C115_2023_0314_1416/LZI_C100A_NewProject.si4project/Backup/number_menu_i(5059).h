#ifndef __NUMBER_MENU_PARA_H_
#define  __NUMBER_MENU_PARA_H_
#include "app_setting_i.h"

typedef struct number_uipara
{
	OFFSET   number_bmp_pos;	//提示框背景图坐标位置
	RECT      number_tiltle_pos; 	//提示框title坐标位置
	OFFSET   number_tsize_pos;		//总磁盘容量坐标位置
	OFFSET   number_fsize_pos;		//剩余磁盘容量坐标位置
	OFFSET   number_ok_bmp_pos;	//OK 字符显示底图坐标

} number_uipara_t;



tips_uipara_t *number_get_tips_para(SIZE screen_size);

#endif


