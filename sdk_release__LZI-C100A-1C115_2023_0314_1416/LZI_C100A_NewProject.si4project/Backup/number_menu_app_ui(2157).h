#ifndef __NUMBER_MENU_APP_UI_H_
#define  __NUMBER_MENU_APP_UI_H_

#include "number_menu_i.h"
#include "app_home_i.h"

#define SCN_LCD_W 				1024	//屏幕宽
#define SCN_LCD_H 				600		//屏幕高

#define APP_HBAR_H 				20		//两行app应用的列高间距
#define APP_W 					150		//没有选中的app应用图片宽
#define APP_H 					150
#define APP_FOCUS_W				190		//选中的app应用图片宽
#define APP_FOCUS_H				190

#define APP_PER_LINE 			4		//一行四个app应用
#define APP_PER_COL 			2		//两行


#define NUMBER_MASK_PaintFlag_MEMDEV_Nonuse		(1<<0)	//1左移0位，为1的时候

typedef struct number_tag_res_id_t
{
	__s32 lang_id;				//语言
	__s32 unfocus_bmp_id;		//没有选中的图片
	__s32 focus_bmp_id;			//选中的图片
}
number_res_id_t;

typedef struct tag_numbermenu_attr
{
	HTHEME number_focus_bmp[MAX_ITEM_NUM];		//存放选中的图片ID
	HTHEME number_unfocus_bmp[MAX_ITEM_NUM];	//存放没有选中的图片ID
	char  number_app_str[MAX_ITEM_NUM][128];	//存放
	RECT  number_app_pos[MAX_ITEM_NUM];			//app应用绘制 矩形 的最大数量8个矩形
	__s32 number_focus_app;						//选中的项目内容app应用
	__s32 number_first_app;						//第一个项目内容app应用
	__u8  number_paint_flag;					//绘制标志

	__s32 number_focus_txt_color;				//存放选中的文本颜色：黄色
	__s32 number_unfocus_txt_color;				//存放没有新的的文本颜色：白色

	GUI_FONT *number_font;						//文本类型
	H_LYR	number_layer;						//app应用图层
	H_LYR	number_bg_layer;					//app应用背景图层
}
numbermenu_attr_t;

#endif


