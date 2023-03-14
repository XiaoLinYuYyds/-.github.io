#ifndef __SETTING_PARA_H__
#define __SETTING_PARA_H__
#include "number_menu_para.h"


static numbers_uipara_t number_uipara_800_480 =
{
	{0, 0},				//提示框背景图坐标位置
	{0, 0, 191, 20},	//提示框title坐标位置
	{32, 28},			//回车按键点击数坐标位置
	{32, 50},			//暂时不用的坐标位置
	{(191 -37) >>1, 75}	//OK 字符显示底图坐标

};

static numbers_uipara_t number_uipara_400_240 =
{
	{0, 0},	
	{0, 0, 191, 20},	
	{15, 28},	
	{15, 50},	
	{61, 72}	

};
	

numbers_uipara_t *number_get_numbers_para(SIZE screen_size)
{
	__s32			screen_width;
	__s32			screen_height;

	if((screen_size.width == 800) && (screen_size.height == 480))
	{
		return &number_uipara_800_480;
	}
	else if((screen_size.width == 480) && (screen_size.height == 272))
	{
		return &number_uipara_400_240;
	}
	else
	{
		__msg("ERR: NULL ui para..\n");
		return	&number_uipara_800_480;
	}
	__wrn("number_get_numbers_para CREATE\n");
}


#endif;

