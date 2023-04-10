#ifndef  _APP_NEW_EXPLORER_H
#define  _APP_NEW_EXPLORER_H

#include "app_new_explorer_i.h"



















typedef	struct	tag_new_explorer_ctrl
{
	H_WIN	h_new_explore;		//存放new_explore资源管理器的manwin父窗口
	H_LYR	new_list_layer;		//存放new_explore资源管理器的列表图层

	GUI_FONT	*new_explorer_font;//存放manwin窗口显示的文本文字

	__s32	root_type;					//存放usb设备模式或sd卡设备模式变量
	new_explorer_view_mode_e view_mode;		//显示模式
	rat_media_type_t 	 media_type;	//存放获取到磁盘的文件类型
	root_para_t  		*root_para;		//存放root根目录结构体变量
}new_explorer_ctrl_t;





















H_WIN app_new_explorer_manwin_create(root_para_t *para);

#endif
