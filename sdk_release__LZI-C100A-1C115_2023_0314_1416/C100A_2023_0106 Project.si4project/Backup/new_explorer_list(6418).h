#ifndef  __NEW_EXPLORER_LIST_H
#define  __NEW_EXPLORER_LIST_H


#include "app_new_explorer_i.h"





typedef struct tag_new_explorer_list_para
{
	H_WIN	list_framewin;	//列表窗口句柄
	H_LYR 	list_layer;		//列表窗口图层
	GUI_FONT *list_font;	//列表窗口文本 
	

	__s32	root_type;							//存放usb设备模式或sd卡设备模式变量
	rat_media_type_t	media_type; 			//搜索文件的类型
	new_explorer_view_mode_e view_mode; 			//显示模式,List or square

	root_para_t  *root_para	;					//记录从root根目录 传进来的参数
} new_explorer_list_para_t;//存放列表framewin窗口的图层和文本









































#endif
