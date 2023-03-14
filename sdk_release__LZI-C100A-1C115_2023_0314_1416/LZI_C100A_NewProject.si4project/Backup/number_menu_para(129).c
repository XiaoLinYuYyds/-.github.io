#ifndef __SETTING_PARA_H__
#define __SETTING_PARA_H__
#include "number_menu_para.h"
#include "number_menu_app_ui.h"

#define APP_HORI_SPACE ((SCN_LCD_W-(APP_W*APP_PER_LINE))/(APP_PER_LINE+1))		  	//(1024屏幕宽 - 150*4(一行显示4个app))/(4+1):“4+1为5个间隙” =每个app应用的x轴间距:x轴间距为84.8
#define APP_VERT_SPACE ((SCN_LCD_H-APP_HBAR_H-(APP_H*APP_PER_COL))/(APP_PER_COL+1))   //(600屏幕高 - 20 - 150*2(一列显示2个app))/(2+1):“2+1为3个间隙” =每个app应用的y轴间距：y轴间距为93

#define APP_POS_X(x, y) ((APP_HORI_SPACE+APP_W)*x+APP_HORI_SPACE)		//app应用图片的x轴坐标：(84.8+app应用图片150宽 * x) + 84.8 =app应用图片显示的x轴坐标
#define APP_POS_Y(x, y) ((APP_VERT_SPACE+APP_H)*y+APP_VERT_SPACE+10)	//app应用图片的y轴坐标：(93+app应用图片150宽 * y) + 93 + 10=app应用图片显示的y轴坐标

static numbers_uipara_t number_uipara_800_480 =
{
	{0, 0},				//提示框背景图坐标位置
	{0, 0, 60, 60},		//提示框控件坐标位置大小
	{32, 28},			//回车按键点击数坐标位置
	{32, 50},			//暂时不用的坐标位置
	{(191 -37) >>1, 75},//OK 字符显示底图坐标
	{0,0,1024,600},		//x,y,宽，高，（窗口的大小和坐标）
	{
		{APP_POS_X(0, 0), APP_POS_Y(0, 0) , APP_W, APP_H},//{200, 200, 150, 150}//单独坐标//第一个app应用图片显示的坐标和大小：(x:84.8  y:103)
	#if 1
		{APP_POS_X(1, 0), APP_POS_Y(0, 0) ,150 ,150},//2
		{APP_POS_X(2, 0), APP_POS_Y(0, 0) ,150 ,150},//3
		{APP_POS_X(3, 0), APP_POS_Y(0, 0) ,150 ,150},//4
		{APP_POS_X(0, 0), APP_POS_Y(0, 1) ,150 ,150},//5
		{APP_POS_X(1, 0), APP_POS_Y(0, 1) ,150 ,150},//6
		{APP_POS_X(2, 0), APP_POS_Y(0, 1) ,150 ,150},//7
		{APP_POS_X(3, 0), APP_POS_Y(0, 1) ,150 ,150},//8
	#endif
	}, //app应用图片8个的显示坐标
	//下面的参数用到再设置
	#if 0
	{}, //主菜单app应用项目宽
	{}, //主菜单app应用项目总数
	{}, //主菜单app应用项目
	{}, //主菜单app应用项目图片宽
	{}, //主菜单app应用项目图片高
	#endif
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

