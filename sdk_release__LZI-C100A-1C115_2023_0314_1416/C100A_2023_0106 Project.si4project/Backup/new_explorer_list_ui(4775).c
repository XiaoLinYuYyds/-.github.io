#ifndef		__NEW_EXPLORER_LIST_H
#define		__NEW_EXPLORER_LIST_H

#include  "new_explorer_list_ui.h"




/*======================================================================
*Description：new explorer  list view ui param set
*			  
*				列表模式下 列表控件显示 获取ui坐标参数
*return		：	空
======================================================================*/

static New_Explorers_ViewerList_uiparam_t new_explorer_list_viewer_ui_param_800_480 =
{
	{						/*framewin窗口图层大小*/
		PIXEL_COLOR_ARGB8888,	/*图层格式*/
		{ 640, 480},			/*framebuffer大小,framewin窗口图层大小*/
		{ 0, 0, 640, 480},		/*src rect:src矩形大小*/
		{ 0, 0, 640, 480},		/*scn rect:scn矩形大小*/
		{ 0 },					/*reserved:预留位*/
	},
		
	{ 0, 0, 640, 480 },		/*列表视图坐标*/
	
	{ 620, 10, 7, 450 },	/*滚动条背景图片显示坐标*/
	{ 623, 24, 5, 419 },	/*滚动条图片显示坐标*/
	9,	4,	9,				/*scroll head/body/tail height flag:滚动条的显示高度位置标志*/

	/*list item widget show 列表项目控件显示 ui坐标参数*/
	{
		{ 156, 8, 420, 455},	/*列表模式下的列表显示区域坐标*/
		{ 156, 8, 420, 35 }, 		/*列表项目显示坐标 矩形控件	用于列表区域的显示文件 行数*/

		{ 59, 0, 338, 30 },  	/*text rect文件名显示范围坐标*/
	}

};

/*======================================================================
*Description：new explorer get viewer ui param
*			  
*				获得整个explorer APP设计的UI参数
*return		：&new_explorer_list_viewer_ui_param_800_480：返回一个地址
======================================================================*/
New_Explorers_ViewerList_uiparam_t *new_explorer_get_viewer_ui_param(void)
{
	__s32			screen_width;
	__s32			screen_height;
	#if 1
	/* get lcd size*/
	dsk_display_get_size(&screen_width, &screen_height);//1024*600
	__wrn("screen_width=%d\n", screen_width);
	__wrn("screen_height=%d\n", screen_height);
	new_explorer_list_viewer_ui_param_800_480.layer.scn_rect.x = (screen_width - 640) / 2;	//x轴192
	new_explorer_list_viewer_ui_param_800_480.layer.scn_rect.y = (screen_height - 480) / 2;	//y轴60

	__wrn("explr_viewer_ui_800_480.layer.scn_rect.x=%d\n",
	      new_explorer_list_viewer_ui_param_800_480.layer.scn_rect.x);
	__wrn("explr_viewer_ui_800_480.layer.scn_rect.y=%d\n",
	      new_explorer_list_viewer_ui_param_800_480.layer.scn_rect.y);
	#endif
	return &(new_explorer_list_viewer_ui_param_800_480);
}

/*======================================================================
*Description：new explorer get list view ui param
*			  
*				列表模式下 列表控件显示 获取ui坐标参数
*return		：&new_explorer_list_viewer_ui_param_800_480：返回一个地址
======================================================================*/
New_Explorers_ViewerList_uiparam_t	*new_explorer_get_list_view_ui_param(void)
{ 	
	
	return &(new_explorer_list_viewer_ui_param_800_480);	//返回列表项目控件的地址
}

//获得Explorer 窗口layer size
__s32 new_explorer_get_screen_rect(RECT *screen_rect)
{
	New_Explorers_ViewerList_uiparam_t *ui_param;
	ui_param = new_explorer_get_viewer_ui_param();
	screen_rect->x = ui_param->layer.scn_rect.x;
	screen_rect->y = ui_param->layer.scn_rect.y;
	screen_rect->width = ui_param->layer.scn_rect.width;
	screen_rect->height = ui_param->layer.scn_rect.height;
	__wrn("screen_rect: x=%d, y=%d, width=%d, height=%d\n",screen_rect->x,screen_rect->y,screen_rect->width,screen_rect->height);
	return EPDK_OK;
}






#endif

