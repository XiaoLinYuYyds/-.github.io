#ifndef __NEW_SETTING_H_
#define  __NEW_SETTING_H_

#include "New_Setting_para.h"
//#include "number_menu_i.h"
#include "New_Setting_i.h"

#if 0//旧的结构体
typedef struct New_Setting_para
{
	GUI_FONT *msgbox_font;
	H_LYR	  layer1;

	H_WIN		layer;		//窗口
	H_WIN		bg_layer;	//背影窗口
	GUI_FONT 	*smenu_font;//文本控件
	__s32			root_type;
	__s8	 		sub_menu_id;
	__s32           focus_item;
	__s32 			focus_id;
} New_Setting_para_t;
typedef struct New_Setting_ctrl
{
	New_Setting_para_t *New_Settings_para;

	__s32 focus_txt_color;
	__s32 unfocus_txt_color;
	H_THEME New_Setting_bmp;
	H_THEME New_Setting_f_bmp;

	char  new_number_size[128];
	New_Settings_uipara_t *New_Settings_uipara;
}
New_Setting_ctrl_t;
#endif
typedef struct new_tag_setting_para
{
	GUI_FONT *font;

	H_LYR layer;
	__s32 focus_id;
	__s32 old_focus;

} new_setting_para_t;//存放framewin窗口的图层和文本
typedef struct
{
	__date_t 	date;				// 存放日期数据变量
	__time_t	time;				// 存放时间数据变量

}
new_setting_time_data_t;			//存放时间日期数据

typedef struct new_setting_time_para
{
	char date_buf[32];//存放日期的字符串的数组
	char time_buf[32];//存放时间的字符串的数组
	new_setting_time_data_t  data;
}new_setting_time_para_t;			//存放时间日期数组
typedef struct New_Setting_ctrl
{
	new_setting_para_t *New_Settings_para;	//存放
	new_setting_time_para_t *set_time_para;//存放时间和日期结构体
	__s32 focus_txt_color;			//存放选中的文本的颜色
	__s32 unfocus_txt_color;		//存放未选中的文本的颜色
	H_THEME New_Setting_bmp;		//存放number:00的矩形背景图
	H_THEME New_Setting_f_bmp;
	H_THEME New_Setting_Bottom_bmp; //用于存放按钮的背景图
	H_THEME New_Setting_progressbar_bmp; //用于存放进度条的背景图

	char  new_number_size[128];	//存放number:00字符串文本数据
	char Saturday_txt[128];		//存放显示星期六的中文
	char  time_size[128];		//存放时间计时字符串文本数据
	New_Settings_uipara_t *New_Settings_uipara;	//存放界面矩形或文本等控件的大小和位置结构体的结构体指针
}
New_Setting_ctrl_t;//用于整个framewin窗口需要使用的结构体控制


H_WIN New_Setting_app_win_create(H_WIN h_parent, new_setting_para_t *para);//framewon窗口创建函数
H_LYR New_Setting_8bpp_layer_create(RECT *rect,__s32 pipe);//图层创建函数，


#endif
