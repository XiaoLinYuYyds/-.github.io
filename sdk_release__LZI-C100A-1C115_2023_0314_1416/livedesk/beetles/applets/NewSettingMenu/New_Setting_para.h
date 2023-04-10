#ifndef __NEW_SETTING_PARA_H_
#define  __NEW_SETTING_PARA_H_
//#include "number_menu_i.h"
#include "New_Setting_i.h"

typedef struct New_Setting_uipara_pos
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	__s32 reserve;
} New_Setting_uipara_pos_t;

typedef struct New_Setting_uipara
{	
	#if 1	//number：00 次数显示提示显示框
	OFFSET   numbers_bmp_pos;		//提示框背景图坐标位置
	RECT      numbers_tiltle_pos; 	//提示框title坐标位置
	OFFSET   number_tsize_pos;		//回车次数量坐标位置
	OFFSET   number_fsize_pos;		//其他坐标位置
	OFFSET   New_Settings_tiltle_bmp_pos;		//OK 字符显示底图坐标
	RECT     New_Settings_tiltle_pos; 	//按键框主题坐标位置
	#endif

} New_Settings_uipara_t;



New_Settings_uipara_t *New_Setting_get_numbers_para(SIZE screen_size);




#endif
