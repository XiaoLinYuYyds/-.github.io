#ifndef __NEW_SETTING_PARA_H__
#define __NEW_SETTING_PARA_H__

#include "New_Setting_para.h"

static New_Settings_uipara_t new_number_uipara_800_480 =
{
	{0, 0},	//矩形方框背景图片绘制位置
	{0, 0, 191, 101},//{416, 290, 191, 20},	矩形位置和大小,对应显示的ID_SETTING_SET_INFO_BMP图片的像素 宽191 高101这样才能让图片全部覆盖到矩形框
	{0, 0},	
	{0, 0},	
	{272, 120},//按钮图片背景绘制位置
	{272, 120, 480, 37}//按钮矩形框大小和位置，对应ID_SETTING_SET_BOTTOM_BMP图片的像素 宽480 高37

};

static New_Settings_uipara_t new_number_uipara_400_240 =
{
	{0, 0},	
	{0, 0, 191, 20},	
	{15, 28},	
	{15, 50},	
	{61, 72},
	{0, 0, 480, 37}

};
/**********************************************************	
*app应用图片和其他控件的大小和显示坐标位置获取函数
**********************************************************/
New_Settings_uipara_t *New_Setting_get_numbers_para(SIZE screen_size)
{
	__s32			screen_width;
	__s32			screen_height;

	if((screen_size.width == 800) && (screen_size.height == 480))
	{
		return &new_number_uipara_800_480;
	}
	else if((screen_size.width == 480) && (screen_size.height == 272))
	{
		return &new_number_uipara_400_240;
	}
	else
	{
		__msg("ERR: NULL ui para..\n");
		return	&new_number_uipara_800_480;
	}
	__wrn("New_Setting_get_numbers_para CREATE\n");
}


#endif;




















