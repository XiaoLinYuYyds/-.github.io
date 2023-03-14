#ifndef __NUMBER_MENU_PARA_H_
#define  __NUMBER_MENU_PARA_H_
//#include "app_setting_i.h"
#include "number_menu_i.h"

typedef struct numbers_uipara
{
	OFFSET   numbers_bmp_pos;		//提示框背景图坐标位置
	RECT      numbers_tiltle_pos; 	//提示框title坐标位置
	OFFSET   number_tsize_pos;		//总磁量坐标位置
	OFFSET   number_fsize_pos;		//剩余容量坐标位置
	OFFSET   number_ok_bmp_pos;		//OK 字符显示底图坐标

} numbers_uipara_t;



numbers_uipara_t *number_get_numbers_para(SIZE screen_size);

#endif


