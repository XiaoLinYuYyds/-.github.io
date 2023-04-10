#ifndef __NUMBER_MENU_PARA_H_
#define  __NUMBER_MENU_PARA_H_
//#include "app_setting_i.h"
#include "number_menu_i.h"
#include "app_home_i.h"

typedef struct numbers_uipara_pos
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	__s32 reserve;
} numbers_uipara_pos_t;

typedef struct numbers_uipara
{	
	#if 1	//number：00 次数显示提示显示框
	OFFSET   numbers_bmp_pos;		//提示框背景图坐标位置
	RECT      numbers_tiltle_pos; 	//提示框title坐标位置
	OFFSET   number_tsize_pos;		//回车次数量坐标位置
	OFFSET   number_fsize_pos;		//其他坐标位置
	OFFSET   number_ok_bmp_pos;		//OK 字符显示底图坐标
	#endif

	#if 1 //新添加菜单内容
	numbers_uipara_pos_t lyr_numbers_menu;
	

	RECT numbers_app_pos[APP_MAX_NUM];	//存放app应用的矩形数量，APP_MAX_NUM app应用的最大数量

	//numbers main menu 新的菜单内容
	__s32 numbers_item_width;			//主菜单app应用项目宽
	__s32 numbers_total_item_num;		//主菜单app应用项目总数
	__s32 numbers_item_per_screen;		//主菜单app应用项目
	__s32 numbers_max_main_bmp_width;	//主菜单app应用项目图片宽
	__s32 numbers_max_main_bmp_height;	//主菜单app应用项目图片高
	#endif 
} numbers_uipara_t;



numbers_uipara_t *number_get_numbers_para(SIZE screen_size);

#endif


