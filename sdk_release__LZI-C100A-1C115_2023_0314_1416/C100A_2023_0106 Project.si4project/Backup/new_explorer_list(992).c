#include  "new_explorer_list.h"
#include  "new_explorer_list_ui.h"




/***********************************************************************************************************
	建立图层
************************************************************************************************************/
H_LYR New_explorer_list_32bpp_layer_create(RECT *rect,__s32 pipe)
{
	H_LYR layer = NULL;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
		0,                                              /* ck_mode   */
		0,                                              /* alpha_en  */
		0,                                              /* alpha_val */
		1,                                              /* pipe      */
		0xff,                                           /* prio      */
		{0, 0, 0, 0},                           		/* screen    */
		{0, 0, 0, 0},                               	/* source    */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
		NULL                                            /* fb        */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"New_Explorer_list layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= rect->width;
	lstlyr.src_win.height 	= rect->height;
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y;
	lstlyr.scn_win.width  	= rect->width;
	lstlyr.scn_win.height 	= rect->height;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("New_Explorer_list layer create error !\n");
	}
	__wrn("New_Explorer_list_32bpp_layer_create CREATE\n");
	return layer;
}

/******************************************************************
*Description	:		list列表图层初始化，申请使用图片资源
*******************************************************************/
static void New_explorer_list_init(new_explorer_list_ctrl_t *list_ctrl)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	__wrn("New_explorer_list_init is 1...\n");
	list_ctrl->ViewerList_uiparam = new_explorer_get_list_view_ui_param();//获取列表显示控件的大小和位置坐标
	
	__wrn("New_explorer_list_init is 2...\n");
	if(!list_ctrl->new_explorer_scroll_bg_bmp)
	{
		__wrn("New_explorer_list_init is 3...\n");
		list_ctrl->new_explorer_scroll_bg_bmp = theme_open(ID_EXPLORER_LIST_SCROLL_BG_BMP);//申请滚动条背景图片使用资源
		__wrn("New_explorer_list_init is 3 ok\n");
	}
	if(!list_ctrl->new_explorer_scroll_bmp)
	{
		__wrn("New_explorer_list_init is 4...\n");
		list_ctrl->new_explorer_scroll_bmp = theme_open(ID_EXPLORER_LIST_SCROLL_BODY_BMP);//申请滚动条白色图片使用资源
		__wrn("New_explorer_list_init is 4 ok\n");
	}
	if(!list_ctrl->new_explorer_list_item_focus_bmp)
	{
		__wrn("New_explorer_list_init is 5...\n");
		list_ctrl->new_explorer_list_item_focus_bmp = theme_open(ID_EXPLORER_LIST_ITEM_FOCUS_BG_BMP);//申请列表条目选中的背景图片使用资源
		__wrn("New_explorer_list_init is 5 ok\n");
	}

}

/*****************************************************************************
*Description	:		list列表图层不初始化，释放申请的使用图片资源
******************************************************************************/
static void New_explorer_list_uninit(new_explorer_list_ctrl_t *list_ctrl)
{
	__wrn("New_explorer_list_uninit is 0 ok\n");
	if(list_ctrl->new_explorer_scroll_bg_bmp)
	{
		__wrn("New_explorer_list_uninit is 1 ...\n");
		theme_close(list_ctrl->new_explorer_scroll_bg_bmp);//滚动条背景图释放
		__wrn("New_explorer_list_uninit is 1 ok\n");
	}
	if(list_ctrl->new_explorer_scroll_bmp)
	{
		__wrn("New_explorer_list_uninit is 2...\n");
		theme_close(list_ctrl->new_explorer_scroll_bmp);//滚动条白色图片使用资源释放
		__wrn("New_explorer_list_uninit is 2 ok\n");
	}
	if(list_ctrl->new_explorer_list_item_focus_bmp)
	{
		__wrn("New_explorer_list_uninit is 3...\n");
		theme_close(list_ctrl->new_explorer_list_item_focus_bmp);//列表条目选中的背景图片使用资源释放
		__wrn("New_explorer_list_uninit is 3 ok\n");
	}

}
/*****************************************************************************
*Description	:		list列表项目编号次数获取函数:
******************************************************************************/
static void New_explorer_list_get_item_number_size(__gui_msg_t *msg,__u32 num)
{
	new_explorer_list_ctrl_t *new_explorer_list_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	new_explorer_list_ctrl = (new_explorer_list_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str数组内存空间初始化
	//eLIBs_strcat(str, "number:");
	snum = num;
	eLIBs_uint2str_dec(snum, numstr);//int型转char型
	 
	if(snum >= 10)
	{
		eLIBs_strcat(str, numstr);
	}
	else
	{
		//eLIBs_strcat(str, "0");
		eLIBs_strcat(str, numstr);
	}

	eLIBs_strcpy(new_explorer_list_ctrl->list_item_number_size, (const char *)str);
}
/*****************************************************************************
*Description	:		获取列表模式下的列表显示区域坐标，用于显示项目文本矩形:
******************************************************************************/
static __s32 New_explorer_get_listview_area_rect(  RECT *text_rect)
{
	New_Explorers_ViewerList_uiparam_t *ui_param;
	ui_param = new_explorer_get_list_view_ui_param();//列表控件显示 获取ui坐标参数
	__wrn("new_explorer get item text rect is...!!\n");
	//默认SD卡模式      { 156, 8, 420, 455 }/*列表显示区域范围坐标*/
	text_rect->x = ui_param->List_item_ui_param.ListBarArea.x;
	text_rect->y = ui_param->List_item_ui_param.ListBarArea.y;
	text_rect->width = ui_param->List_item_ui_param.ListBarArea.width;
	text_rect->height = ui_param->List_item_ui_param.ListBarArea.height;
	__wrn("text_rect: x=%d, y=%d, width=%d, height=%d\n",
		text_rect->x,text_rect->y,text_rect->width,text_rect->height);

	return EPDK_OK;
}

/*****************************************************************************
*Description	:		获取项目文本矩形显示区域坐标，用于显示文件名:
******************************************************************************/
__s32 New_explorer_get_item_text_rect(  RECT *text_rect)
{
	New_Explorers_ViewerList_uiparam_t *ui_param;
	ui_param = new_explorer_get_list_view_ui_param();//列表控件显示 获取ui坐标参数
	__wrn("new_explorer get item text rect is...!!\n");
	//默认SD卡模式      { 59, 0, 338, 30 }/*文件名显示范围坐标*/
	text_rect->x = ui_param->List_item_ui_param.text_rect.x;
	text_rect->y = ui_param->List_item_ui_param.text_rect.y;
	text_rect->width = ui_param->List_item_ui_param.text_rect.width;
	text_rect->height = ui_param->List_item_ui_param.text_rect.height;
	__wrn("text_rect: x=%d, y=%d, width=%d, height=%d\n",
		text_rect->x,text_rect->y,text_rect->width,text_rect->height);

	return EPDK_OK;
}
/************************************************************************************************************************
*Description	:		从全路径中获取文件名
************************************************************************************************************************/
static __s32 New_explorer_ExtractFileName(char *FileName, char *FilePath)
{
	char *pFileName;
	__wrn("get Extract File Name is...!!\n");
	pFileName = eLIBs_strchrlast(FilePath, '\\');//汉字识别
	pFileName++;
	eLIBs_strcpy(FileName, pFileName);
	return EPDK_OK;
}
/************************************************************************************************************************
*Description	:	Get List Item File Full Path	
*					根据索引号获得条目文件名全路径
************************************************************************************************************************/
static __s32 New_explorer_GetListItemFileFullPath(new_explorer_list_para_t *list_para, __s32 ItemIndex, char *FilePath)
{
	int ret;
	rat_entry_t ItemInfo;
	eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
	ret = rat_get_item_info_by_index(list_para->rat.handle, ItemIndex, &ItemInfo);//获取当前项目信息索引
	__wrn("get List Item File Full Path is...!!\n");
	if(ret == EPDK_FAIL)
	{
		__wrn("get file information form rat fail!!\n");
		return EPDK_FAIL;
	}

	eLIBs_strcpy(FilePath, ItemInfo.Path);			//不去操作rat 内存
	return EPDK_OK;
}

/************************************************************************************************************************
*Description	:		根据索引号获得条目文件名(只有文件名，供显示)
************************************************************************************************************************/
static __s32 New_explorer_GetListItemFileName(new_explorer_list_para_t *list_para, __s32 ItemIndex, char *FileName)
{
	int ret;
	char FilePath[RAT_MAX_FULL_PATH_LEN];
	eLIBs_memset(FilePath, 0, sizeof(FilePath));//数组初始化数据清0
	ret = New_explorer_GetListItemFileFullPath(list_para, ItemIndex, FilePath);//通过索引获取当前条目文件名的全路径,返回EPDK_OK
	__wrn("get List Item File Name is...!!\n");
	if(ret == EPDK_FAIL)
	{
		return EPDK_FAIL;
	}

	New_explorer_ExtractFileName(FileName, FilePath);//从全路径获取文件名
	__wrn("get List Item Extract File Name is ok ok...!!\n");
	return EPDK_OK;
}
#if 0
/*=============================================================================================
*Description：new explorer list item tetx ui paint
*			  app应用 列表 UI 绘制 列表条目文件名 文本
*return		：return EPDK_OK;返回一个OK
=============================================================================================*/
static __s32 New_explorer_draw_listview_item_text(__lbar_draw_para_t *draw_param)
{
	int ret;
	new_explorer_list_para_t	*list_para;	//列表项目条目 参数
	char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名，长度为RAT获取到磁盘文件的最大的路径长度
	char	*name =NULL;
	GUI_RECT	List_TextRect;
	RECT		TextRect;

	list_para = (new_explorer_list_para_t *)draw_param->attr;
	__wrn("New_explorer_draw_listview_item_text is...!!\n");
	eLIBs_memset(FileName, 0, sizeof(FileName));//数组数据初始化，清0
	ret = New_explorer_GetListItemFileName(list_para, draw_param->index, FileName);//获取文件名 成功
	__wrn("get List Item File Name is ok ok...!!\n");
	if(ret == EPDK_FAIL)
	{
		__wrn("get file information form rat fail!!\n");//从rat获取文件信息失败
		return EPDK_FAIL;
	}
	ret = new_explorer_get_item_text_rect(&TextRect);//绘制文本显示矩形区域，成功返回：EPDK_OK
	if(!ret)
	{
		__wrn("get item text rect fail!!\n");
	}
	#if 1	//绘制文本 信息
		GUI_SetFont(list_para->list_font);
		GUI_UC_SetEncodeUTF8();
		GUI_SetColor(GUI_WHITE);
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
		{
			List_TextRect.x0 = draw_param->rect.x + TextRect.x;
			List_TextRect.y0 = draw_param->rect.y + TextRect.x;
			List_TextRect.x1 = draw_param->rect.x + (TextRect.x + TextRect.width);
			List_TextRect.y1 = draw_param->rect.y + (TextRect.y + TextRect.height);
			__wrn("List_TextRect: x0=%d, y0=%d, x1=%d, y1=%d\n",List_TextRect.x0,List_TextRect.y0,List_TextRect.x1,List_TextRect.y1);

			__wrn("New_explorer_draw_listview_item_text FileName=%s is...!!\n",FileName);
			GUI_DispStringInRect(FileName, &List_TextRect, GUI_TA_VCENTER);//显示文本
			__wrn("FileName=%s is...!!\n",FileName);
		}
	#endif
	return EPDK_OK;
}


/*=============================================================================================
*Description：new explorer list item tetx ui paint
*			  app应用 列表 UI 绘制 列表控件项目
*return		：return EPDK_OK;返回一个OK
=============================================================================================*/
static __s32 New_explorer_draw_listview_item(__lbar_draw_para_t *draw_param)
{
	new_explorer_list_para_t	*list_para;	//列表项目条目 参数
	rat_media_type_t 			media_type;	//RAT 浏览的媒体类型

	list_para = (new_explorer_list_para_t *)draw_param->attr;
	__wrn("list_para=%s is...!!\n",list_para);
	__wrn("New explorer draw listview item is...!!\n");
	{//默认SD卡模式
		New_explorer_draw_listview_item_text(draw_param);	//绘制文本 名
	}
	{
			//media_type = get_file_list_item_file_type(list_para->);//获取文件列表清单文件类型
			media_type = RAT_MEDIA_TYPE_VIDEO;//视频文件类型
	}
	
	return EPDK_OK;
}
/*=============================================================================================
*Description：new explorer list bar ui item draw
*			  app应用 列表条目 UI 列表模式的列表项目条目显示区域绘制
=============================================================================================*/
  __s32 New_explorer_listbar_item_draw(__lbar_draw_para_t *draw_param)
{
	new_explorer_list_para_t	*list_para;	//列表项目条目 参数
	list_para = (new_explorer_list_para_t *)draw_param->attr;
	__wrn("list_para->rat.total is...= %d\n",list_para->rat.total);
	if(list_para->rat.total <= 0)//文件总数小于0时
	{
		return EPDK_FAIL;//返回错误
	}
	__wrn("\n*******New_explorer_listbar_item_draw is draw***********\n");
	__wrn("draw_param->mode is...= %d\n",draw_param->mode);
	switch(draw_param->mode)
	{
		case LBAR_MODE_NORMAL://列表模式下绘制列表显示区域
		{
			__wrn("\n*******New explorer draw listview item***********\n");
			New_explorer_draw_listview_item(draw_param);//直接绘制
		}
		break;

		//default:
			//break;
	}
	return EPDK_OK;
}

/*=============================================================================================
*Description：new explorer list bar ui init
*			  app应用 列表条目 UI 初始化 相当于是申请使用图片资源
=============================================================================================*/
static	void New_explorer_ListBar_init(H_WIN list_win)
{
#if 0
	__listbar_config_t	listbar_config;
	__s32 item_width = 0, item_height = 0;//项目条的宽和高
	new_explorer_list_ctrl_t *list_ctrl;
	new_explorer_list_para_t *list_para;	//列表项目条目 参数
	list_para = (new_explorer_list_para_t *)GUI_WinGetAttr(list_win);	//获取结构体数据

	list_ctrl->ViewerList_uiparam = new_explorer_get_viewer_ui_param();//获取矩形大小
	//explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();
	__wrn("\n~~~~~~~~~~Listbar is initializing~~~~~~~~~~~\n");
	eLIBs_memset(&listbar_config, 0, sizeof(__listbar_config_t));	//结构体初始化，数据清0
	{//默认列表模式
		/*列表显示区域里面的列表项目 显示矩形大小*/
		item_width 	= list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.width;	//获取项目矩形宽
		item_height	= list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.height;	//获取项目矩形高
		/*列表模式下的列表显示区域*/
		listbar_config.list_rect.x = list_ctrl->ViewerList_uiparam->List_item_ui_param.ListBarArea.x;
		listbar_config.list_rect.y = list_ctrl->ViewerList_uiparam->List_item_ui_param.ListBarArea.y;
		listbar_config.list_rect.width = list_ctrl->ViewerList_uiparam->List_item_ui_param.ListBarArea.width;
		listbar_config.list_rect.height = list_ctrl->ViewerList_uiparam->List_item_ui_param.ListBarArea.height;
		__wrn("listbar_config.list_rect：x= %d,y= %d,width= %d,heigh= %d\n",
			listbar_config.list_rect.x,listbar_config.list_rect.y,listbar_config.list_rect.width,listbar_config.list_rect.height);
			
	}

	listbar_config.item_width 	=	item_width;	//列表项目显示 矩形控件	宽赋值给列表模式下的列表显示区域项目 矩形
	listbar_config.item_height	=	item_height;
	__wrn("listbar_config.item_width = %d,listbar_config.item_height = %d\n",
		listbar_config.item_width,listbar_config.item_height);
	listbar_config.start_id		=	0;
	listbar_config.focus_id		=	0;
	listbar_config.bk_color		=	0;
	listbar_config.alpha_status	=	1;			//打开alpha通道开关
	listbar_config.list_attr 	= 	(void *)list_para;
	listbar_config.item_cnt		=	list_para->rat.total;			 //项目总数
	__wrn("new listbar_config.item_cnt = %d\n",listbar_config.item_cnt);
	__wrn("new list_para->rat.total = %d\n",list_para->rat.total);
	list_para->listbar_handle	=	LISTBAR_Create(list_win);		 //创建一个图层窗口控制操作句柄,创建成功则返回PDEK_OK
	__wrn("new list_para->listbar_handle = %d\n",list_para->listbar_handle);

	LISTBAR_Config(list_para->listbar_handle, New_explorer_listbar_item_draw, &listbar_config); //config parametters for listbar：列表条目控制参数

	//LISTBAR_Config(list_para->listbar_handle, _explorer_listbar_item_draw, &listbar_config);	//config parametters for listbar
	

	__wrn("new list_para->listbar_handle = %d is ok....\n",list_para->listbar_handle);
#endif

#if 1
		new_explorer_list_para_t *list_para;
		__listbar_config_t config;
		__scroll_bar_t scroll_param;						//new scroll bar
		__s32 item_width = 0, item_height = 0;
		New_Explorers_ViewerList_uiparam_t *ui_param = new_explorer_get_viewer_ui_param();			//获得整个AP的UI设计参数
		__msg("\n~~~~~~~~~~Listbar is initializing~~~~~~~~~~~\n");
		eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
		eLIBs_memset(&scroll_param, 0, sizeof(__scroll_bar_t));
		list_para = (new_explorer_list_para_t *)GUI_WinGetAttr(list_win);
	
		if(list_para->view_mode1 == NEW_EXPLR_LIST_MODE)
		{
			item_width = ui_param->List_item_ui_param.item_rect.width;						//根据list or square模式不同
			item_height = ui_param->List_item_ui_param.item_rect.height;
			config.list_rect.x = ui_param->List_item_ui_param.ListBarArea.x;			//ui_param->menu == listbar area
			config.list_rect.y = ui_param->List_item_ui_param.ListBarArea.y;
			config.list_rect.width = ui_param->List_item_ui_param.ListBarArea.width;
			config.list_rect.height = ui_param->List_item_ui_param.ListBarArea.height;
		}
	
		config.item_width = item_width;
		config.item_height = item_height;				//these are numbers only for test

		config.start_id = 0;//list_para->last_start_id;
		//config.focus_id = list_para->rat.index;				//removed by libaiao, 2010.6.6
		config.focus_id = 0;//list_para->last_focused_id;
		config.bk_color = 0;
		config.alpha_status = 1;						//打开Alpha开关
		config.list_attr = (void *)list_para;

		config.item_cnt = list_para->rat.total;   //Rat.total必须已经获得；搜索文件类型的文件总数
		__wrn("config.item_cnt = %d\n",config.item_cnt);
		
		list_para->listbar_handle = LISTBAR_Create(list_win);					//new listbar
		__wrn("list_para->listbar_handle = %d\n",list_para->listbar_handle);
		LISTBAR_Config(list_para->listbar_handle, New_explorer_listbar_item_draw, &config);	//config parametters for listbar


#endif
}
/*=============================================================================================
*Description：new explorer get list ListBar pagenum
*			  app应用 列表条目 UI  			获取每页的项目条数
*Return		：pagenum：返回一个项目条数
=============================================================================================*/
static	__s32 New_explorer_get_ListBar_pagenum(new_explorer_list_para_t	*list_para)
{
	__u32	pagenum = 0;
	__u32	page_width = 0,page_height = 0;	//也宽，高
	__s32   item_width = 0, item_height = 0;//项目条的宽和高
	__s32   listbar_width = 0, listbar_height = 0;//列表条目的宽和高
	new_explorer_list_ctrl_t *list_ctrl;
	list_ctrl->ViewerList_uiparam = new_explorer_get_viewer_ui_param();//获取矩形大小

	{	//默认列表模式
		/*列表显示区域里面的列表项目 显示矩形大小*/
		item_width 	= list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.width;	//获取项目矩形宽
		item_height	= list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.height;	//获取项目矩形高
		__wrn("item_width = %d,item_height = %d\n",item_width,item_height);
	
		/*列表模式下的列表显示区域 				 显示矩形大小*/
		listbar_width 	= list_ctrl->ViewerList_uiparam->List_item_ui_param.ListBarArea.width;	//获取列表显示区域 矩形宽
		listbar_height	= list_ctrl->ViewerList_uiparam->List_item_ui_param.ListBarArea.height;	//获取列表显示区域 矩形高
		__wrn("listbar_width = %d,listbar_height = %d\n",listbar_width,listbar_height);
	
	}
	/*每页的项目条数算法：*/
	page_width  = listbar_width  / item_width;
	page_height = listbar_height / item_height;
	pagenum		= page_width * page_height;

	return pagenum;
}

/*=============================================================================================
*Description：new explorer get list last param
*			  app应用 列表项目 UI 			获取最后一个条目参数
=============================================================================================*/
static	void New_explorer_get_last_para(new_explorer_list_para_t	*list_para)
{
	__u32 list_page_item_cnt = 0; 				//每页的条目总数
	char FileName[RAT_MAX_FULL_PATH_LEN];		//存放文件名
	int ret = 0;
	reg_root_para_t *root_para = NULL;

	list_page_item_cnt = New_explorer_get_ListBar_pagenum(list_para);	//获取页数项目条数
	__wrn("list_page_item_cnt = %d\n",list_page_item_cnt);//打印项目条数
	root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(list_para->rat.handle != NULL)	//文件搜索句柄不为空
	{
		switch(list_para->media_type)	//搜索的文件类型
		{
			case RAT_MEDIA_TYPE_VIDEO:	//视频文件类型
			{
				/*{	//默认SD卡模式
					list_para->last_start_id = root_para->last_movie_start_index_sd;
					list_para->last_focused_id = root_para->last_movie_index_sd;
					list_para->last_filename = root_para->last_movie_path_sd;
				}*/
			}
			break;

			default://否则直接退出switch语句
				break;
		}
	}
}
#endif

/*=============================================================================================
*Description：new explorer File Search module and rat_init
*			  app应用 列表项目 UI 		文件搜索模式，RAT初始化（必须RAT初始化才能获取文件名）
*Return		：返回一个EPDK_OK值
=============================================================================================*/
static	__s32 New_explorer_FileSearch_module_rat_init(new_explorer_list_para_t	*list_para)
{
	if(list_para->rat.handle == NULL)	//文件搜索句柄为空，进入一次
	{
		{//文件搜索打开
			__wrn("list_para->root_type = %d,list_para->media_type = %d\n",list_para->root_type,list_para->media_type);//打印SD卡模式 or USB模式;搜索文件的类型:3为所有的视频文件
			list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);//打开RAT进程; root_type：为SD卡或者USB模式; media_type：需要搜索的文件媒体类型; 0为搜索次数记录
		}
		if(list_para->rat.handle != NULL)
		{
			__wrn("\n!!!!!!!!!!!!! open rat File Search successed!!!!!!!!!!!!!!!!!\n");
		}
		else
		{
			__wrn("\n!!!!!!!!!!!!! open rat File Search failed!!!!!!!!!!!!!!!!!\n");
		}
	}
	else
	{
		__wrn("\n!!!!!!!!!!!!!please dont open rat again!!!!!!!!!!!!!!!!!\n");//文件搜索句柄不为空 不再进入上面执行文件搜索功能
	}

	list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle);//获取当前扫描到的文件类型的文件总数
	__wrn("list_para->rat.total 11= %d\n",list_para->rat.total);
	//rat_start_miniature_decode();	//开始缩略图线程
	return EPDK_OK;
}

/*=============================================================================================
*Description：new explorer list view ui creat
*			  app应用 列表模式下的列表视图 UI 创建；直接绘制显示视频类媒体文件名
=============================================================================================*/
static	__s32 New_explorer_ListView_creat(H_WIN list_win)
{
	new_explorer_list_para_t	*list_para;	//列表项目条目 参数
	
	char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名，长度为RAT获取到磁盘文件的最大的路径长度
	GUI_RECT	List_TextRect;//指定显示的区域:作用是指定显示的起始坐标x0、y0和终止坐标x1、y1
	RECT		TextRect;//直接显示矩形:作用是只需要确定初始坐标和矩形的宽和高就可以了
	__s32 		list_item_number=0;
	list_para	=	(new_explorer_list_para_t *)GUI_WinGetAttr(list_win);//获取数据
	list_para->list_framewin = list_win;	//窗口句柄保存
	list_para->list_font	 = NULL;		//列表文本为空
	__wrn("\n~~~~~~~~~~ListView creat is initializing~~~~~~~~~~~\n");

	if(list_para->media_type = RAT_MEDIA_TYPE_VIDEO)		//文件类型为视频文件
	{
		__wrn("\n~~~~~~~~~~file search is initializing~~~~~~~~~~~\n");
		New_explorer_FileSearch_module_rat_init(list_para);	//文件搜索模式,搜索视频文件，RAT初始化
		
		__wrn("\n~~~~~~~~~~file search is OK OK OK	OK~~~~~~~~~~~\n");
	}
	//New_explorer_ListBar_init(list_win);	//列表条目初始化，列表模式下的列表显示区域 初始化
	__wrn("\n~~~~~~~~~~file search is 11111	OK~~~~~~~~~~~\n");
	#if 0//直接绘制单个文本名
		eLIBs_memset(FileName, 0, sizeof(FileName));//数组数据初始化，清0
		
		__wrn("\n~~~~~~~~~~file search is 22222	OK~~~~~~~~~~~\n");
		New_explorer_GetListItemFileName(list_para, 1, FileName);
		new_explorer_get_item_text_rect(&TextRect);//绘制文本显示矩形区域，成功返回：EPDK_OK
		__wrn("\n~~~~~~~~~~file search is 22222	OK~~~~~~~~~~~\n");

		#if 1	//绘制文本 信息
			GUI_SetFont(list_para->list_font);
			GUI_UC_SetEncodeUTF8();
			GUI_SetColor(GUI_WHITE);
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			{
				List_TextRect.x0 = 0 + TextRect.x;
				List_TextRect.y0 = 0 + TextRect.x;
				List_TextRect.x1 = 0 + (TextRect.x + TextRect.width);
				List_TextRect.y1 = 0 + (TextRect.y + TextRect.height);
				__wrn("List_TextRect: x0=%d, y0=%d, x1=%d, y1=%d\n",List_TextRect.x0,List_TextRect.y0,List_TextRect.x1,List_TextRect.y1);

				__wrn("New_explorer_draw_listview_item_text FileName=%s is...!!\n",FileName);
				GUI_DispStringInRect(FileName, &List_TextRect, GUI_TA_VCENTER);//显示文本
				GUI_DispStringAt(FileName,100,100);
				__wrn("FileName=%s is...!!\n",FileName);
			}
		#endif
	#endif
	#if 1//直接绘制视频类所有文件名
		eLIBs_memset(FileName, 0, sizeof(FileName));//数组数据初始化，清0
		__wrn("\nlist_item_number < list_para->rat.total = %d\n",list_para->rat.total);	
		for(list_item_number = 0;list_item_number < list_para->rat.total-4;list_item_number++)//绘制矩形，显示视频文件名
		{
			__wrn("\n~~~~~~~~~~file search is 22222	is~~~~~~~~~~~\n");
			New_explorer_GetListItemFileName(list_para, list_item_number, FileName);//获取列表项目文件名
			New_explorer_get_item_text_rect(&TextRect);//获取绘制文本显示矩形区域，成功返回：EPDK_OK
			__wrn("\n~~~~~~~~~~file search is 22222	OK~~~~~~~~~~~\n");
			__wrn("\nlist_item_number = %d\n",list_item_number);	
			#if 1	//在列表模式下的列表显示区域坐标（156, 8, 420, 455） 绘制文本信息存放坐标
				GUI_SetFont(list_para->list_font);		//设置文本
				GUI_UC_SetEncodeUTF8();					//设置文本编码格式
				GUI_SetColor(GUI_WHITE);				//设置颜色白色
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//设置绘制模式为正常模式
				{//文件名显示矩形坐标TextRect（59，0，338，30）
					//if(list_item_number == 0)
					/*{
						List_TextRect.x0 =  156 + TextRect.x;						//文件名显示的list条目矩形起始坐标x0 156+59=215
						List_TextRect.y0 = 	list_item_number*50 + 8 + TextRect.y;	//文件名显示的list条目矩形起始坐标y0 条数*50间距 + 8 + 0
						List_TextRect.x1 =  156 + 59 + 338;							//文件名显示的list条目矩形终止坐标x1 156+59+338=553
						List_TextRect.y1 =  list_item_number*50 + 8 + (TextRect.y + TextRect.height);	//文件名显示的list条目矩形终止坐标y1 条数*50 + 8 + 0+30
					}
					else*/
					{
						List_TextRect.x0 =	156 + TextRect.x;						//156为列表模式下的列表显示区域坐标x轴起始坐标 + 文件名显示矩形x轴坐标为59
						List_TextRect.y0 =	list_item_number*50 + 8 + TextRect.y;	//在列表项目条目显示坐标 间隔为50，项目条数量*50 + 8为列表显示区域坐标y轴起始坐标 + 0
						List_TextRect.x1 =	156 + (TextRect.x + TextRect.width);	//文本名显示的矩形宽为固定338 + 59 +156 = 553
						List_TextRect.y1 =	list_item_number*50 + 8 + (TextRect.y + TextRect.height);	//文本名显示的矩形高为固定30，项目条数量*50间隔+30为y1的y轴终止坐标
					}
					__wrn("List_TextRect: x0=%d, y0=%d, x1=%d, y1=%d\n",List_TextRect.x0,List_TextRect.y0,List_TextRect.x1,List_TextRect.y1);

					__wrn("New_explorer_draw_listview_item_text FileName=%s is...!!\n",FileName);
					GUI_DispStringInRect(FileName, &List_TextRect, GUI_TA_VCENTER);//显示视频MP4文件名文本到矩形
					//GUI_DispStringAt(FileName,100,20+list_item_number*50);
					__wrn("FileName=%s is...!!\n",FileName);
				}
			#endif
		}
	#endif
	#if 1//获取媒体类型文件的文件名，然后拷贝到结构体数组list_para->FileName中;用于下面的单个绘制的上下选择功能时打开
		eLIBs_memset(list_para->FileName, 0, sizeof(list_para->FileName));//数组数据初始化，清0
		__wrn("\n list_para->rat.total = %d\n",list_para->rat.total);	
		for(list_item_number = 0;list_item_number < list_para->rat.total-4;list_item_number++)//绘制矩形，显示9个视频文件名
		{
			__wrn("\n~~~~~~~~~~file search is	OK~~~~~~~~~~~\n");
			New_explorer_GetListItemFileName(list_para, list_item_number, list_para->FileName);//获取列表项目文件名
			New_explorer_get_item_text_rect(&TextRect);//获取绘制文本显示矩形区域，成功返回：EPDK_OK
			__wrn("\n~~~~~~~~~~file search is 33333	OK~~~~~~~~~~~\n");
			{
				__wrn("list_para->FileName=%s is...!!\n",list_para->FileName);
			}
		}
	#endif
	return 0;
}

/*=============================================================================================
*Description：new explorer list ui paint
*			  app应用 列表 UI 绘制 单个项目条的视频MP4文件名内容
*return		：return EPDK_OK;返回一个OK
=============================================================================================*/
#if 1
static __s32 _new_explorer_list_ui_one_paint(__gui_msg_t *msg, __s32 key_num)
{
	new_explorer_list_para_t	*list_para;	//列表项目条目 参数
	
	RECT		TextRect;//直接显示矩形:作用是只需要确定初始坐标和矩形的宽和高就可以了
	GUI_RECT	List_TextRect;
	GUI_RECT	List_TextRect1;
	static __s32 unfoucs_id=0;
	static char  FileName_backups[RAT_MAX_FULL_PATH_LEN];//用于保存上一次的文件名
	list_para	=	(new_explorer_list_para_t *)GUI_WinGetAttr(msg->h_deswin);//获取数据
	
	New_explorer_GetListItemFileName(list_para, key_num, list_para->FileName);//获取列表项目文件名
	New_explorer_get_item_text_rect(&TextRect);//获取绘制文本显示矩形区域，成功返回：EPDK_OK
	
	
	#if 1	//在列表模式下的列表显示区域坐标（156, 8, 420, 455） 绘制文本信息存放坐标
		GUI_SetFont(list_para->list_font);		//设置文本
		GUI_UC_SetEncodeUTF8();					//设置文本编码格式
		GUI_SetColor(GUI_WHITE);				//设置颜色白色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//设置绘制模式为正常模式
		{//文件名显示矩形坐标TextRect（59，0，338，30）
			__wrn("unfoucs_id =%d is...!!\n",unfoucs_id);
			//unfoucs_id = list_para->focus_text_id;
			if(unfoucs_id != key_num)//为未选择的文件名显示
			{
				//GUI_SetColor(GUI_WHITE);				//设置颜色白色
				List_TextRect1.x0 =  156 + TextRect.x;						//文件名显示的list条目矩形起始坐标x0 156+59=215
				List_TextRect1.y0 =  unfoucs_id*50 + 8 + TextRect.y;		//文件名显示的list条目矩形起始坐标y0 条数*50间距 + 8 + 0
				List_TextRect1.x1 =  156 + 59 + 338;						//文件名显示的list条目矩形终止坐标x1 156+59+338=553
				List_TextRect1.y1 =  unfoucs_id*50 + 8 + (TextRect.y + TextRect.height);	//文件名显示的list条目矩形终止坐标y1 条数*50 + 8 + 0+30
				GUI_DispStringInRect(FileName_backups, &List_TextRect1, GUI_TA_VCENTER);//显示视频MP4文件名文本到矩形
				__wrn("List_TextRect:1 x0=%d, y0=%d, x1=%d, y1=%d\n",List_TextRect1.x0,List_TextRect1.y0,List_TextRect1.x1,List_TextRect1.y1);
				__wrn("FileName_backups 1=%s is...!!\n",FileName_backups);	
			}
				//GUI_ClearRect(List_TextRect.x0,List_TextRect.y0,List_TextRect.x1,List_TextRect.y1);
			eLIBs_memset(FileName_backups, 0, sizeof(FileName_backups));//数组初始化，数据清0
			{
				GUI_SetColor(GUI_YELLOW);				//设置颜色白色
				List_TextRect.x0 =  156 + TextRect.x;						//文件名显示的list条目矩形起始坐标x0 156+59=215
				List_TextRect.y0 = 	key_num*50 + 8 + TextRect.y;			//文件名显示的list条目矩形起始坐标y0 条数*50间距 + 8 + 0
				List_TextRect.x1 =  156 + 59 + 338;							//文件名显示的list条目矩形终止坐标x1 156+59+338=553
				List_TextRect.y1 =  key_num*50 + 8 + (TextRect.y + TextRect.height);	//文件名显示的list条目矩形终止坐标y1 条数*50 + 8 + 0+30
			}
			unfoucs_id = key_num;
			//list_para->focus_text_id = key_num;
			__wrn("List_TextRect: x0=%d, y0=%d, x1=%d, y1=%d\n",List_TextRect.x0,List_TextRect.y0,List_TextRect.x1,List_TextRect.y1);

			__wrn("list_para-> FileName=%s is...!!\n",list_para->FileName);
			GUI_DispStringInRect(list_para->FileName, &List_TextRect, GUI_TA_VCENTER);//显示视频MP4文件名文本到矩形
			eLIBs_strcpy((char *)FileName_backups,(const char *)list_para->FileName);//拷贝到数组
			__wrn("FileName_backups=%s is...!!\n",FileName_backups);
		}
	#endif



}
#endif
/*=============================================================================================
*Description：new explorer list ui paint
*			  app应用 列表 UI 绘制 所有内容
*return		：return EPDK_OK;返回一个OK
=============================================================================================*/
static __s32 _new_explorer_list_ui_all_paint(__gui_msg_t *msg)
{
	new_explorer_list_ctrl_t *new_explorer_list_ctrl;
	new_explorer_list_para_t *listpara;
	RECT	gui_rect;				//存放滚动条背景图显示矩形
	RECT	gui_scroll_rect;		//存放滚动条白色图显示矩形
	RECT	gui_list_item_rect;		//存放列表项目条显示矩形
	RECT	text_rect;				//
	void	*scroll_bg_bmp;			//存放滚动条背景图数据
	void	*scroll_bmp;			//存放滚动条白色图数据
	void	*list_item_focus_bmp;	//存放列表项目条背景图数据
	__s32	list_item_number=0;
	__wrn("_new_explorer_list_ui_all_paint 0 is....\n");
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}
	new_explorer_list_ctrl = (new_explorer_list_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);	//获取添加一个数据到msg结构体
	__wrn("_new_explorer_list_ui_all_paint 1 is....\n");
	GUI_LyrWinSel(new_explorer_list_ctrl->new_explorer_list_para->list_layer);				//选择新添加的app应用framewin子窗口图层
	__wrn("list_layer = %d\n",new_explorer_list_ctrl->new_explorer_list_para->list_layer);
	
		#if 0	//直接在图层上面显示绘制文本字符串
				GUI_SetFont(new_explorer_list_ctrl->new_explorer_list_para->list_font);	//设置对话框字体
				GUI_UC_SetEncodeUTF8(); 								//设置编码格式
				__wrn("_new_explorer_list_ui_one_paint 3 CREATE\n");
				GUI_SetColor(GUI_BLACK);								//设置文本颜色，黑色
				//New_Setting_get_volume_size(msg,num);			//获取回车按键点击次数 字符串
				//txt_character = New_Setting_ctrl->new_number_size;		//需要显示的文字字符串获取
				__wrn("_new_explorer_list_ui_one_paint 4 CREATE\n");
				//GUI_DispStringAt(txt_character,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				{
					//__wrn("txt_character = %s\n",txt_character);		//直接显示文字字符串，文本绘制成功
				}
				
				//number_theme = theme_hdl2buf(numbers_ctrl->number_bmp);
				//GUI_BMP_Draw(number_theme, numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//绘制文本控件number:00显示函数
				__wrn("_new_explorer_list_ui_one_paint CREATE\n");
				
				//GUI_DispStringAt(numbers_ctrl->number_total_size, numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);			//在文本控件上显示字符串
				GUI_DispStringAt("01",32,28);//字符串显示和坐标
				__wrn("txt paint is CREATE\n");//文本绘制成功
		#endif
		#if 0	//绘制滚动条背景图
			
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//绘制模式为正常模式
			gui_rect.x		=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_bg_rect.x;		//620
			gui_rect.y		=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_bg_rect.y;		//10
			gui_rect.width	=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_bg_rect.width;	//7
			gui_rect.height	=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_bg_rect.height;	//450
			__wrn("gui_rect 1：x= %d,y= %d,width= %d,heigh= %d\n",
				gui_rect.x,gui_rect.y,gui_rect.width,gui_rect.height);
			{
				scroll_bg_bmp = theme_hdl2buf(new_explorer_list_ctrl->new_explorer_scroll_bg_bmp);//获取滚动条背景图片数据
				if(!scroll_bg_bmp)
				{
					__wrn("scroll_bg_bmp is null \n");
					return EPDK_FAIL ;
				}
				GUI_BMP_Draw(scroll_bg_bmp, gui_rect.x, gui_rect.y);
				__wrn("_new_explorer_list_ui_one_paint ok ok \n");
			}
			
		#endif
		#if 0	//绘制滚动条白色滚动部分图片
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//绘制模式为正常模式
			//GUI_ClearRectEx(&gui_rect);//先删除前面的矩形，然后在下面重新画
			gui_scroll_rect.x		=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_rect.x;
			gui_scroll_rect.y		=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_rect.y;
			gui_scroll_rect.width	=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_rect.width;
			gui_scroll_rect.height	=	new_explorer_list_ctrl->ViewerList_uiparam->ViewerList_scroll_rect.height;
			__wrn("gui_scroll_rect 2：x= %d,y= %d,width= %d,heigh= %d\n",
				gui_scroll_rect.x,gui_scroll_rect.y,gui_scroll_rect.width,gui_scroll_rect.height);
			{
				scroll_bmp = theme_hdl2buf(new_explorer_list_ctrl->new_explorer_scroll_bmp);//获取滚动条白色图片数据
				if(!scroll_bmp)
				{
					__wrn("scroll_bmp is null \n");
					return EPDK_FAIL ;
				}
				GUI_BMP_Draw(scroll_bmp, gui_scroll_rect.x, gui_scroll_rect.y);
				__wrn("_new_explorer_list_ui_one_paint scroll ok ok \n");
			}
		#endif
		#if 1	//绘制列表条目
		for(list_item_number = 0;list_item_number < 10;list_item_number++)
		{
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//绘制模式为正常模式
			gui_list_item_rect.x		=	new_explorer_list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.x;
			gui_list_item_rect.y		=	new_explorer_list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.y + list_item_number*50;
			gui_list_item_rect.width	=	new_explorer_list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.width;
			gui_list_item_rect.height	=	new_explorer_list_ctrl->ViewerList_uiparam->List_item_ui_param.item_rect.height;
			__wrn("gui_list_item_rect 2：x= %d,y= %d,width= %d,heigh= %d\n",
				gui_list_item_rect.x,gui_list_item_rect.y,gui_list_item_rect.width,gui_list_item_rect.height);
			New_explorer_list_get_item_number_size(msg,list_item_number);	//获取项目编号
			{
				list_item_focus_bmp = theme_hdl2buf(new_explorer_list_ctrl->new_explorer_list_item_focus_bmp);//获取滚动条白色图片数据
				if(!list_item_focus_bmp)			//判断是否获取图片资源数据
				{
					__wrn("list_item_focus_bmp is null \n");
					return EPDK_FAIL;
				}
				GUI_BMP_Draw(list_item_focus_bmp, gui_list_item_rect.x, gui_list_item_rect.y);//绘制图片
				GUI_SetColor(GUI_YELLOW);			//设置文本颜色
				GUI_DispStringAt(new_explorer_list_ctrl->list_item_number_size,gui_list_item_rect.x+5,gui_list_item_rect.y + 3);//项目编号 字符串显示和坐标
				__wrn("_new_explorer_list_ui_one_paint list item number ok ok \n");
			}
		}
		#endif
		#if 1	//绘制列表项目视频MP4文件文本
			New_explorer_ListView_creat(msg->h_deswin);
			//New_explorer_draw_listview_item(__lbar_draw_para_t *draw_param);
		#endif
	return EPDK_OK;
}


/*=============================================================================================
*Description：new explorer list View ui the key proc
*			  app应用 列表视图 UI 按键处理函数
*return		：return EPDK_OK;返回一个OK
=============================================================================================*/
static __s32  New_explorer_ListView_Key_Proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)//进入当前界面时的第一次按键信息处理
	{
		__wrn(" New_explorer_ListView_Key msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn(" key1 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_ENTER, 0, 0);//向manwin窗口发送回车确认按键消息信号
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_ENTER://2，确认按键消息发送
			case GUI_MSG_KEY_LONGENTER:{
				app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_ENTER, 0, 0);
			}
			break;

			case GUI_MSG_KEY_RIGHT://3，右移
			case GUI_MSG_KEY_LONGRIGHT:{
				app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_RIGHT, 0, 0);
			}
			break;
			
			case GUI_MSG_KEY_LEFT://4，左移
			case GUI_MSG_KEY_LONGLEFT:{
				app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_LEFT, 0, 0);
			}
			break;

			case GUI_MSG_KEY_ESCAPE://5，退出
			case GUI_MSG_KEY_MENU:{//返回按键退出
				app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_EXIT, 0, 0);
			}
			break;
				
			default:
				break;
				
		}
	}

	if(KEY_UP_ACTION == msg->dwAddData2)//按键松开动作标志
	{
		__wrn(" key2 last_key = %d\n",last_key);
		last_key = -1;
	}
	else	//储存最后一次的按键值，界面退出前的按键标志
	{
		last_key = msg->dwAddData1;
	}

	return EPDK_OK;
	#endif
}

/*=============================================================================================
*Description：new explorer list frame window proc
*			  新的资源管理器app应用 列表 framewin子窗口创建 回调处理函数
*return		：GUI_FrmWinDefaultProc(msg)：返回一个默认的处理程序
=============================================================================================*/
static __s32 _new_explorer_list_win_proc(__gui_msg_t *msg)
{
	static __s32 key_num=0;
	__wrn("_new_explorer_list_win_proc msg->id=%d is...\n",msg->id);
	
	switch(msg->id)
	{
		case GUI_MSG_CREATE://0，创建功能
		{
			new_explorer_list_ctrl_t *new_explorer_list_ctrl;
			new_explorer_list_para_t *new_explorer_list_para;
			new_explorer_list_para = (new_explorer_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
			new_explorer_list_ctrl = (new_explorer_list_ctrl_t *)My_Balloc(sizeof(new_explorer_list_ctrl_t));	//内存空间分配

			if(!new_explorer_list_ctrl)
			{
				__err("new_explorer_list_ctrl memory balloc fail........\n");
				return EPDK_FAIL;
			}
			//New_explorer_ListView_creat(msg->h_deswin);//列表模型下的列表视图创建
			__wrn("\n**************New_explorer_ListView_creat is******************\n");

			__wrn("_new_explorer_list_win_proc GUI_MSG_CREATE 0 is...\n");
			eLIBs_memset(new_explorer_list_ctrl, 0, sizeof(new_explorer_list_ctrl_t));//结构体数据初始化，数据清0
			new_explorer_list_ctrl->new_explorer_list_para = new_explorer_list_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)new_explorer_list_ctrl);			//添加一个数据								//添加一个数据	
			
			__wrn("_new_explorer_list_win_proc GUI_MSG_CREATE 1 is...\n");
			New_explorer_list_init(new_explorer_list_ctrl);			//图片资源申请
			GUI_LyrWinSetSta(new_explorer_list_ctrl->new_explorer_list_para->list_layer, GUI_LYRWIN_STA_ON);//窗口图层设置开启状态
			GUI_LyrWinSetTop(new_explorer_list_ctrl->new_explorer_list_para->list_layer);					//窗口图层设置停止状态
			__wrn("_new_explorer_list_win_proc GUI_MSG_CREATE ok ok is...\n");

			__wrn("new_explorer_list_para->root_type = %d,list_para->media_type = %d\n",new_explorer_list_para->root_type,new_explorer_list_para->media_type);//打印SD卡模式 or USB模式;搜索文件的类型:3为所有的视频文件
			
			
		}
			return EPDK_OK;
		case GUI_MSG_DESTROY: //1, 回车按下进入 销毁窗口
		{
			new_explorer_list_ctrl_t *new_explorer_list_ctrl;
			new_explorer_list_para_t *new_explorer_list_para;
			new_explorer_list_para = (new_explorer_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
			
			__wrn("_new_explorer_list_win_proc GUI_MSG_DESTROY\n");
			new_explorer_list_ctrl = (new_explorer_list_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//获取这个结构体的数据
			New_explorer_list_uninit(new_explorer_list_ctrl);//释放图片使用资源
			#if 1	//释放结构体的内存空间
				if(new_explorer_list_ctrl->new_explorer_list_para)//释放列表framewin窗口的图层和文本结构体的内存空间
				{
					__wrn("new_explorer_list_ctrl win free new_explorer_list_para.....\n");
					
					__wrn("new_explorer_ListPara 33 size = %d\n",sizeof(new_explorer_list_para_t));
					My_Bfree(new_explorer_list_ctrl->new_explorer_list_para, sizeof(new_explorer_list_para_t));//释放结构体内存空间
					new_explorer_list_ctrl->new_explorer_list_para = NULL ;
				}

				if(new_explorer_list_ctrl)//释放列表整个framewin窗口需要使用的结构体的内存空间
				{
					__wrn("new_explorer_list_ctrl win free new_explorer_list_ctrl.....\n");
					
					__wrn("new_explorer_list_ctrl  size = %d\n",sizeof(new_explorer_list_ctrl_t));
					My_Bfree(new_explorer_list_ctrl, sizeof(new_explorer_list_ctrl_t));	
					new_explorer_list_ctrl = NULL ;
				}
			#endif
			__wrn("new_explorer_list_ctrl->new_explorer_list_para free size = %d\n",new_explorer_list_ctrl->new_explorer_list_para);
			__wrn("new_explorer_list_ctrl free size = %d\n",new_explorer_list_ctrl);
			__wrn("new_explorer_ListPara  free size = %d\n",new_explorer_list_para);//打印结构体是否释放内存
			
		}
			return EPDK_OK;

		case GUI_MSG_PAINT:	//2, 绘制功能
			__wrn("_new_explorer_list_win_proc GUI_MSG_PAINT\n");
			_new_explorer_list_ui_all_paint(msg);
			_new_explorer_list_ui_one_paint(msg,key_num);//用于绘制显示选中文本
			//New_explorer_draw_listview_item_text();//绘制列表 项目文件名
			return EPDK_OK;
		case GUI_MSG_QUIT:	//4，退出按键功能
		{
			__wrn("_new_explorer_list_win_proc GUI_MSG_QUIT is press...\n");
			app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_EXIT, 0, 0);//向manwin窗口发送退出消息信号
		}
			return EPDK_OK;
		case GUI_MSG_KEY:	//5，按键功能
		{
			New_explorer_ListView_Key_Proc(msg);//按键处理
			#if 0//直接发送退出信号
				__gui_msg_t mymsg;
				key_num += 1;
				if(key_num  == 3)
				{
					mymsg.id = GUI_MSG_QUIT;		//framewin窗口的退出窗口id，执行退出按键功能
					mymsg.h_deswin = msg->h_deswin;	//在当前图层发送按键信息
					mymsg.h_srcwin = msg->h_srcwin; //在当前图层发送按键信息
					mymsg.dwAddData1 = mymsg.dwAddData1;
					mymsg.dwAddData2 = mymsg.dwAddData2;
					GUI_SendMessage(&mymsg);		//发送到当前framewin窗口
					key_num = 0;
				}
			#endif 
			#if 0//直接按返回按键发送退出信号
				switch(msg->dwAddData1)
				{
					case GUI_MSG_KEY_MENU://物理返回按键按下，向manwin窗口发送退出信号
						__wrn("_new_explorer_list_win_proc GUI_MSG_KEY_MENU is press ...\n");
						app_new_explorer_cmd2parent(msg->h_deswin, ID_OP_NEW_EXPLORER_EXIT, 0, 0);
					break;
				}
			#endif
			__wrn("_new_explorer_list_win_proc GUI_MSG_KEY is press %d...\n",key_num);
		}
			return EPDK_OK;
		case MSG_NEW_EXPLORER_OP_RIGHT://按键上功能，执行项目条的上一条显示
		{
			key_num += 1;
			if(key_num > 9)
			{
				key_num = 0;
			}
			__wrn("_new_explorer_list_win_proc MSG_NEW_EXPLORER_OP_RIGHT is press %d...\n",key_num);
			_new_explorer_list_ui_one_paint(msg,key_num);
		}
			return EPDK_OK;
		case MSG_NEW_EXPLORER_OP_LEFT://按键下功能，执行项目条的下一条显示
		{
			key_num -= 1;
			if(key_num < 0)
			{
				key_num = 9;
			}
			__wrn("_new_explorer_list_win_proc MSG_NEW_EXPLORER_OP_LEFT is press %d...\n",key_num);
			_new_explorer_list_ui_one_paint(msg,key_num);
			
		}
			return EPDK_OK;

		default://否则直接退出switch语句
			break;
	}


	return GUI_FrmWinDefaultProc(msg);
}



/*=============================================================================================
*Description：new explorer list frame window create
*			  新的资源管理器app应用 列表 framewin子窗口创建
*return		：GUI_FrmWinCreate(&framewin_para)：返回一个创建好的framewin窗口
=============================================================================================*/

H_WIN	new_explorer_list_framewin_creat(H_WIN h_parent,new_explorer_list_para_t *para)
{
	__gui_framewincreate_para_t		framewin_para;		//framewin窗口结构体
	new_explorer_list_para_t	*new_explorer_list_para;//存放framewin窗口创建需要的结构体
	FB fb;
	#if 1	//检测framewin窗口是否内存泄漏-->查看打印的Pages free的大小反复切换manwin界面是否会恢复
	__wrn("esKSRV_SysInfo   is...\n");				//打印内存是否泄露
	esKSRV_SysInfo();								//查看系统内存消息是否泄露或溢出
	//esMEMS_FreeMemSize();							//剩余内存大小以KB为单位
	//esMEMS_TotalMemSize();						//总的内存大小
	__wrn("esKSRV_SysInfo | esMEMS_FreeMemSize = %d \n",esMEMS_FreeMemSize());//打印剩余内存
	__wrn("esKSRV_SysInfo | esMEMS_TotalMemSize = %d \n",esMEMS_TotalMemSize());//打印总内存
	#endif
	
	GUI_LyrWinGetFB(para->list_layer, &fb);//获取FB功能块，获取列表图层大小
	//new_explorer_list_para = (new_explorer_list_para_t *)esMEMS_Balloc(sizeof(new_explorer_list_para_t));	//分配内存空间到结构体
	new_explorer_list_para = (new_explorer_list_para_t *)My_Balloc(sizeof(new_explorer_list_para_t));	//分配内存空间到结构体
	if(!new_explorer_list_para)	//判断结构体是否分配内存成功
	{
		__err("memory balloc fail........\n");
		return EPDK_FAIL;
	}
	eLIBs_memset(new_explorer_list_para, 0, sizeof(new_explorer_list_para_t));								//结构体数据初始化，数据清0
	new_explorer_list_para->list_font  = para->list_font;	//从manwin窗口传入进framewin窗口的文本存放
	new_explorer_list_para->media_type = para->media_type;	//获取磁盘文件的文件类型
	new_explorer_list_para->view_mode1  = para->view_mode1;	//获取显示的模式，是list列表模式还是square正方形模式
	new_explorer_list_para->root_para  = para->root_para;	//获取root 根目录传进来的参数
	new_explorer_list_para->root_type  = para->root_type;	//存放USB设备或TF卡设备检测结果
	new_explorer_list_para->list_layer = para->list_layer;	//从manwin窗口传入进来的图层 获取
	__wrn("new_explorer_list_para->list_layer = %d is....\n",new_explorer_list_para->list_layer);
	__wrn("new_explorer_ListPara 22 size = %d\n",sizeof(new_explorer_list_para_t));
	/******************开始操作framewin窗口的配置**********************/
	__wrn("new_explorer_list_framewin_creat is...\n");
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));									//framewin结构体数据初始化，数据清0
	framewin_para.name			=	"New Explorer List Window";
	framewin_para.dwExStyle		=	WS_EX_NONE;
	framewin_para.dwStyle		=	WS_NONE | WS_VISIBLE;
	framewin_para.spCaption		=	NULL;
	framewin_para.hOwner		=	NULL;
	framewin_para.id			=	NEW_NEWEXPLORER_LIST_ID;
	framewin_para.hHosting		=	h_parent;
	framewin_para.FrameWinProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_explorer_list_win_proc);
	framewin_para.rect.x		=	0;
	framewin_para.rect.y		=	0;
	framewin_para.rect.width	=	fb.size.width;
	framewin_para.rect.height	=	fb.size.height;
	framewin_para.BkColor.alpha	=	0;
	framewin_para.BkColor.blue	=	0;
	framewin_para.BkColor.green	=	255;
	framewin_para.BkColor.red	=	0;
	framewin_para.attr			=	(void *)new_explorer_list_para;
	__wrn("framewin_para.hLayer is...\n");
	framewin_para.hLayer		=	para->list_layer;		//获取manwin窗口传入的图层然后赋值给framewin窗口图层
	
	return (GUI_FrmWinCreate(&framewin_para));//返回一个创建好的framewin窗口
}





