#ifndef  __NEW_EXPLORER_LIST_H
#define  __NEW_EXPLORER_LIST_H


#include "app_new_explorer_i.h"
#include  "new_explorer_list_ui.h"





//定义rat 文件搜索数据结构
typedef struct tag_new_rat_ctrl
{
	HRAT  handle;				//文件搜索句柄
	__s32 index;				//当前文件索引号
	__s32 total;				//文件总数
	//rat_media_type_t media_type;//搜索文件的类型
} new_rat_ctrl_t;

typedef struct tag_new_explorer_list_para
{
	H_WIN	list_framewin;	//列表framewin窗口句柄
	H_LYR 	list_layer;		//列表窗口图层
	GUI_FONT *list_font;	//列表窗口文本 
	
	H_LBAR 	listbar_handle;						//列表条目 窗口句柄
	
	__s32	root_type;							//存放usb设备模式或sd卡设备模式变量
	char 	FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名
	__s32	focus_text_id;						//存放选中的文本ID标志位
	rat_media_type_t		 media_type; 		//搜索文件的类型
	new_explorer_view_mode_e view_mode1; 		//显示模式,List or square

	new_rat_ctrl_t 			 rat;				//文件搜索数据结构

	root_para_t  *root_para	;					//记录从root根目录 传进来的参数
} new_explorer_list_para_t;//存放列表framewin窗口的图层和文本

typedef struct new_explorer_list_ctrl
{
	new_explorer_list_para_t *new_explorer_list_para;	//存放列表窗口和图层信息参数
	
	__s32 focus_txt_color;			//存放选中的文本的颜色
	__s32 unfocus_txt_color;		//存放未选中的文本的颜色
	H_THEME new_explorer_list_item_focus_bmp;	//存放列表条目选中的背景图
	H_THEME new_explorer_scroll_bmp;			//用于存放滚动条白色的图片
	H_THEME new_explorer_scroll_bg_bmp; 		//用于存放滚动条背景的背景图

	char  list_item_txt_size[128];			//存放列表项目字符串文本数据
	char  list_item_number_size[128];		//存放列表项目编号

	New_Explorers_ViewerList_uiparam_t *ViewerList_uiparam;	//存放界面的列表视图等矩形或文本等 控件的大小和位置 结构体的结构体指针
} new_explorer_list_ctrl_t;//用于list列表整个framewin窗口需要使用的结构体控制


H_WIN	new_explorer_list_framewin_creat(H_WIN h_parent,new_explorer_list_para_t *para);
H_LYR 	New_explorer_list_32bpp_layer_create(RECT *rect,__s32 pipe);

extern	__s32 New_explorer_listbar_item_draw(__lbar_draw_para_t *draw_param);




































#endif
