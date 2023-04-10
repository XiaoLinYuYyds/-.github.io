#ifndef  __NEW_EXPLORER_LIST_UI_H
#define  __NEW_EXPLORER_LIST_UI_H


#include "app_new_explorer_i.h"



typedef struct new_explorer_framewin_layer_ui_param_s
{
	__pixel_rgbfmt_t fb_fmt;        //图层格式
	SIZE fb_size;                   //framebuffer大小
	RECT src_rect;                  //srcwin区域：framebuffer source window，缓冲区域
	RECT scn_rect;                  //scnwin区域
	__u32 reserved[32];             //预留
} framewin_layer_ui_param_t;	//framewin窗口图层大小配置

typedef	struct	List_item
{
	RECT	ListBarArea;	//列表模式下的列表显示区域
	RECT	item_rect;		//列表项目显示 矩形控件	用于列表区域的显示文件 行数

	RECT	text_rect;		//列表文本显示 矩形控件 用于文件名显示
}List_item_ui_param_t;	//列表项目条目 显示矩形大小配置


//整个浏览器列表UI控件显示位置和大小参数
typedef struct New_Explorers_ViewerList_uiparam
{	
	#if 1	//framewin窗口图层 显示矩形大小
	framewin_layer_ui_param_t	layer;		//layer frame window;framewin窗口图层大小设置
	#endif
	#if 1	//列表视图区域 显示控件
	RECT	ListView;						//列表视图，用于整个所有文件的显示区域

	RECT 	ViewerList_scroll_bg_rect;		//滚动条背景图片显示 矩形控件
	RECT 	ViewerList_scroll_rect;			//滚动条图片显示 		     矩形控件
	__u32 	ViewerList_scroll_head_height;	//滚动条显示的标志，头部
	__u32 	ViewerList_scroll_body_height;	//滚动条显示的 身体中间部分
	__s32 	ViewerList_scroll_tail_height;	//滚动条显示的 尾部
	
	/**************************列表模式 显示控件*****************************/
	List_item_ui_param_t	List_item_ui_param;	//列表项目 矩形控件 ui显示参数
	#endif
}
New_Explorers_ViewerList_uiparam_t;
















New_Explorers_ViewerList_uiparam_t 	*new_explorer_get_viewer_ui_param(void);				//framewin窗口图层矩形大小
New_Explorers_ViewerList_uiparam_t	*new_explorer_get_list_view_ui_param(void);

extern __s32 new_explorer_get_screen_rect(RECT *screen_rect);//framewin窗口图层矩形大小获取






















#endif
