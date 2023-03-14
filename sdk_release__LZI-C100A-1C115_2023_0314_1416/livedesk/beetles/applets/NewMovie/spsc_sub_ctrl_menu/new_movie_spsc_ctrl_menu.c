
#include	"new_movie_spsc_ctrl_menu.h"
#include	"new_movie_spsc_ctrl_menu_ui_para.h"

//创建8bpp图层
static H_LYR  __new_spsc_ctrl_menu_8bpp_layer_create(RECT *rect, __s32 pipe)
{
	H_LYR	layer;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
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
		"progress layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
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
		__err("app bar layer create error !\n");
	}

	return layer;
}

//申请图片使用资源
static __s32 new_movie_spsc_ctrl_menu_ui_para_init(void)
{
	new_movie_spsc_ctrl_menu_uipara_t *ui_para;
	__s32 i=0;
	ui_para = new_movie_spsc_ctrl_menu_get_uipara(0);//获取进度条等控件大小和坐标

	if(!ui_para->uipara_bg_theme.res_theme)//图层背景图片资源申请
	{
		__wrn("ui_para->uipara_bg_theme.res_theme is...\n");
		ui_para->uipara_bg_theme.res_theme = theme_open(ID_MOVIE_SPSC_BG_BMP);//图层背景图
	}
	__wrn("ui_para->uipara_bg_theme.res_theme = %x...\n",ui_para->uipara_bg_theme.res_theme);
	/*for(i=0;i<movie_spsc_ctrl_menu_icon_num;i++)
	{
		if(ui_para->uipara_icon[i].res_theme_id)//进度条移动图片资源申请
		{
			__wrn("ui_para->uipara_icon[i].res_theme is...\n");
			ui_para->uipara_icon[i].res_theme = dsk_theme_open(ui_para->uipara_icon[i].res_theme_id);//进度条图
		}
	}*/
	#if 1//单独显示
		if(!ui_para->uipara_icon[i].res_theme)
		{
			ui_para->uipara_icon[i].res_theme = theme_open(ID_MOVIE_SPSC_PREV_UF_BMP);
		}
		if(!ui_para->uipara_icon[i+1].res_theme)
		{
			ui_para->uipara_icon[i+1].res_theme = theme_open(ID_MOVIE_SPSC_CHANNEL_UF_BMP);
		}
	#endif
	#if 0//通过数组申请图片资源
	for(i=1;i<movie_spsc_ctrl_menu_icon_num;i++)
	{
		if(ui_para->uipara_icon[i].res_theme_id)//进度条移动图片资源申请
		{
			__wrn("ui_para->uipara_icon[i].res_theme is...\n");
			ui_para->uipara_icon[i].res_theme = dsk_theme_open(ui_para->uipara_icon[i].res_theme_id);//进度条图
		}
	}
	#endif
	
	return EPDK_OK;
}

//释放图片使用资源
static __s32 new_movie_spsc_ctrl_menu_ui_para_deinit(void)
{
	new_movie_spsc_ctrl_menu_uipara_t *ui_para;
	__s32 i=0;
	ui_para = new_movie_spsc_ctrl_menu_get_uipara(0);//获取进度条等控件大小和坐标参数

	__wrn("ui_para->uipara_icon.prog_bg_bmp = %x...\n",ui_para->uipara_bg_theme.res_theme);
	if(ui_para->uipara_bg_theme.res_theme)//进度条背景图片释放
	{
		__wrn("prog_bg_bmp win close theme...\n");
		theme_close(ui_para->uipara_bg_theme.res_theme);
		ui_para->uipara_bg_theme.res_theme = NULL;
	}
	/*for(i=0;i<movie_spsc_ctrl_menu_icon_num;i++)
	{
		if(ui_para->uipara_icon[i].res_theme)//进度条移动图片资源申请
		{
			__wrn("ui_para->uipara_icon[i].res_theme is...\n");
			dsk_theme_close(ui_para->uipara_icon[i].res_theme);//进度条图
			ui_para->uipara_icon[i].res_theme = NULL;
		}
	}*/
	#if 1//单独释放显示
		if(ui_para->uipara_icon[i].res_theme)
		{
			dsk_theme_close(ui_para->uipara_icon[i].res_theme);
		}
		if(ui_para->uipara_icon[i+1].res_theme)
		{
			dsk_theme_close(ui_para->uipara_icon[i+1].res_theme);
		}
	#endif

	return EPDK_OK;
}

//控制菜单UI控件绘制
static __s32 __new_movie_spsc_ctrl_menu_update_ui(new_movie_spsc_ctrl_menu_scene_t *scene_para, __u32 ui_type)
{
	new_movie_spsc_ctrl_menu_uipara_t *ui_para;
	ui_para = new_movie_spsc_ctrl_menu_get_uipara(0);//获取进度条矩形大小和坐标
	__wrn("__new_movie_spsc_ctrl_menu_update_ui is begin...\n");
	if(NULL == ui_para){//进度条ui参数获取失败
		__wrn("uipara fail...\n");
		return EPDK_FAIL;
	}

	if(!scene_para){//结构体参数为空
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr){//进度条图层创建失败
		__wrn("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr)){//进度条图层窗口状态是否为打开
		__wrn("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(scene_para->hlyr);	//选择图层
	com_memdev_create(scene_para->hlyr);//创建伪内存设备，使图片绘制的更快

	//图层背景：0，0，364，94
	if(ui_type & PROGRESSBAR_UI_TYPE_BG){
		GUI_RECT gui_rect;
		GUI_SetBkColor(0xF0);//背景颜色为透明
		gui_rect.x0 = 0;
		gui_rect.y0 = 0;
		gui_rect.x1 = gui_rect.x0 + ui_para->uipara_bg.w;
		gui_rect.y1 = gui_rect.y0 + ui_para->uipara_bg.h;
		GUI_ClearRectEx(&gui_rect);
		__wrn("MOVIE_PROG_UI_TYPE_BG is create...\n");
		__wrn("gui_rect:x0=%d,y0=%d,x1=%d,y1=%d ...\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
	}
	
	
	//画背景显示
	if(ui_type & PROGRESSBAR_UI_TYPE_PROG)
	{
		//画背景
		{
			void *pbmp;

			if(!ui_para->uipara_bg_theme.res_theme){
				__wrn("ui_para->uipara_icon.prog_bg_bmp is null...\n");
				com_memdev_delete();//删除伪内存设置
				return EPDK_FAIL;
			}
			pbmp = dsk_theme_hdl2buf(ui_para->uipara_bg_theme.res_theme);//获取图片数据

			if(!pbmp){
				__wrn("pbmp is null...\n");
				com_memdev_delete();//删除伪内存设置
				return EPDK_FAIL;
			}


			GUI_BMP_Draw(pbmp, ui_para->uipara_bg.x, ui_para->uipara_bg.y);//绘制进度条背景图片
		}
			{

		}
		{//单独显示一个icon
			void *pbmp;

			__s32 i=0;
			for(i=0;i<2;i++){
				__wrn("i=%d\n",i);
				if(!ui_para->uipara_icon[i].res_theme){//图片资源打开失败
					__wrn("ui_para->uipara_progressbar_icon.prog_bmp is null...\n");
					com_memdev_delete();//删除伪内存设备
					return EPDK_FAIL;
				}
				pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[i].res_theme);//进度条移动部分图片数据

			
				
				if(!pbmp){
					__wrn("pbmp is null...\n");
					com_memdev_delete();//删除伪内存设置
					return EPDK_FAIL;
				}

				{//绘制进度条移动
					{
						__here__;
						GUI_BMP_Draw(pbmp, ui_para->uipara_icon_retc[i].x, ui_para->uipara_icon_retc[i].y);//绘制进度条移动图片
						__wrn(" ui_para->uipara_icon_retc[i].x= %d,ui_para->uipara_icon_retc[i].y=%d \n",  
								ui_para->uipara_icon_retc[i].x,ui_para->uipara_icon_retc[i].y);
					}
				}
			}
		}
		//画icon部分
		/*{
			void *pbmp;
			__u32 cur_time;
			__u32 total_time;
			__s32 i=0;
			//for(i =0;i<movie_spsc_ctrl_menu_icon_num;i++)
			{
				if(!ui_para->uipara_icon[i].res_theme){//图片资源打开失败
					__wrn("ui_para->uipara_progressbar_icon.prog_bmp is null...\n");
					com_memdev_delete();//删除伪内存设备
					return EPDK_FAIL;
				}
				pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[i].res_theme);//进度条移动部分图片数据

			
				
				if(!pbmp){
					__wrn("pbmp is null...\n");
					com_memdev_delete();//删除伪内存设置
					return EPDK_FAIL;
				}

				{//绘制进度条移动
					{
						__here__;
						GUI_BMP_Draw(pbmp, ui_para->uipara_icon_retc[i].x, ui_para->uipara_icon_retc[i].y);//绘制进度条移动图片
						__wrn(" ui_para->uipara_icon_retc[i].x= %d,ui_para->uipara_icon_retc[i].y=%d \n",  
								ui_para->uipara_icon_retc[i].x,ui_para->uipara_icon_retc[i].y);
					}
				}
			}
		}*/
	}

	__here__;
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
	com_memdev_delete();//删除伪内存设备
	return EPDK_OK;
}


//视频播放控制菜单回调处理函数
static __s32 __new_movie_spsc_ctrl_menu_proc(__gui_msg_t *msg)
{

	switch(msg->id){
		case GUI_MSG_CREATE:{/*0,创建*/
			new_movie_spsc_ctrl_menu_scene_t *scene_para;
			__wrn("__new_movie_spsc_ctrl_menu_proc GUI_MSG_CREATE begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);
			if(!scene_para)
			{
				__wrn("invalid para...\n");
				return EPDK_FAIL;
			}
			scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
			__wrn("scene_para->hfrm para = %x...\n",scene_para->hfrm);
			new_movie_spsc_ctrl_menu_ui_para_init();
			__new_movie_spsc_ctrl_menu_update_ui(scene_para,PROGRESSBAR_UI_TYPE_ALL);
			GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);//窗口图层状态打开
			GUI_LyrWinSetTop(scene_para->hlyr);
		}
		return EPDK_OK;//只需要进入一次

		case GUI_MSG_DESTROY:{/*1,销毁*/
			new_movie_spsc_ctrl_menu_scene_t *scene_para;
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			__wrn("before new_movie_spsc_ctrl_menu_ui_para_deinit\n");
			new_movie_spsc_ctrl_menu_ui_para_deinit();//释放图片资源
			__wrn("after new_movie_spsc_ctrl_menu_ui_para_deinit\n");
			
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2,绘制*/
			new_movie_spsc_ctrl_menu_scene_t *scene_para;
			__wrn("__new_movie_spsc_ctrl_menu_proc GUI_MSG_PAINT begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);
			
			__new_movie_spsc_ctrl_menu_update_ui(scene_para,PROGRESSBAR_UI_TYPE_ALL);//重新绘制控制菜单
			
		}
		return EPDK_OK;
		
		case GUI_MSG_QUIT:{

		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{

		}
		return EPDK_OK;

		case GUI_MSG_KEY:{

		}
		break;//需要重复进入操作

		case GUI_MSG_COMMAND:{

		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{

		}
		break;//需要重复进入操作
	}

	return GUI_FrmWinDefaultProc(msg);
}

//视频播放的控制菜单framewin窗口创建
void *new_movie_spsc_ctrl_menu_scene_create(new_movie_spsc_ctrl_menu_create_para_t *create_para)
{
	__gui_framewincreate_para_t framewin_para;//framewin创建结构体
	new_movie_spsc_ctrl_menu_scene_t	*scene_para;/*视频播放控制菜单场景结构体*/
	new_movie_spsc_ctrl_menu_uipara_t *ui_para;
	RECT lyr_rect;//视频播放控制菜单图层矩形大小
	#if 1	//检测内存是否泄漏
		esKSRV_SysInfo();//检查内存是否泄漏
	#endif
	__wrn("new_movie_spsc_ctrl_menu_scene_create is begin...\n");
	scene_para = (new_movie_spsc_ctrl_menu_scene_t *)esMEMS_Balloc(sizeof(new_movie_spsc_ctrl_menu_scene_t));//分配内存空间

	if(!scene_para)
	{
		__wrn("mem not enough...\n");
		return NULL;
	}

	eLIBs_memset(scene_para, 0, sizeof(new_movie_spsc_ctrl_menu_scene_t));
	scene_para->hparent = create_para->hparent;		//传入的视频播放控制菜单源窗口
	scene_para->scene_id = create_para->scene_id;	//传入的视频播放控制菜单窗口的控件id
	scene_para->hlyr	=	create_para->hlyr;		//传入的视频播放控制菜单窗口图层
	ui_para	= new_movie_spsc_ctrl_menu_get_uipara(0);
	#if 1//控制菜单的图层大小:(1024-364)/2, 465, 364, 94
		lyr_rect.x		=	ui_para->uipara_lyr.x;
		lyr_rect.y		=	ui_para->uipara_lyr.y;
		lyr_rect.width	=	ui_para->uipara_lyr.w;
		lyr_rect.height	=	ui_para->uipara_lyr.h;
		__wrn("lyr_rect: x=%d, y=%d, w=%d, h=%d...\n",lyr_rect.x, lyr_rect.y,lyr_rect.width, lyr_rect.height);
		scene_para->hlyr = __new_spsc_ctrl_menu_8bpp_layer_create(&lyr_rect, 1);//创建视频播放控制菜单图层
		if(scene_para->hlyr == NULL){//图层创建失败
			esMEMS_Bfree(scene_para, sizeof(new_movie_spsc_ctrl_menu_scene_t));//释放内存空间
			scene_para = NULL;
			__wrn("scene_para->hlyr is null...\n");
			return NULL;
		}
	#endif

	/*=====================开始进度条framwin窗口配置======================*/
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name			=	"new_movie_spsc_strl_menu_win",
	framewin_para.dwExStyle		= WS_EX_NONE;
	framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
	framewin_para.spCaption 	= NULL;
	framewin_para.hOwner		= NULL;
	framewin_para.id			= scene_para->scene_id;//窗口id
	framewin_para.hHosting		=  scene_para->hparent;//窗口句柄
	framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__new_movie_spsc_ctrl_menu_proc);//回调函数
	framewin_para.rect.x		= ui_para->uipara_frm.x;//在framewin图层窗口中的大小和位置: 0，0，364，94
	framewin_para.rect.y		= ui_para->uipara_frm.y;
	framewin_para.rect.width	= ui_para->uipara_frm.w;
	framewin_para.rect.height	= ui_para->uipara_frm.h;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red	= 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue	= 0;
	framewin_para.attr			= (void *)scene_para;//framewin窗口属性
	framewin_para.hLayer		= scene_para->hlyr;	//framewin图层创建赋值
	scene_para->hfrm = GUI_FrmWinCreate(&framewin_para);
	__wrn("scene_para->hlyr = %x ok...\n",scene_para->hlyr);

	if(scene_para->hfrm == NULL){//framewin窗口创建失败
		esMEMS_Bfree(scene_para, sizeof(new_movie_spsc_ctrl_menu_scene_t));//释放内存空间
		scene_para = NULL;
		__wrn("scene_para->hfrm is null...\n");
		return NULL;
	}
	if(scene_para->hlyr)//图层创建成功
	{
		__wrn("scene_para->hlyr...\n");
		GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);//窗口图层状态打开
		GUI_LyrWinSetTop(scene_para->hlyr);
	}
	else
	{
		__err("invalid para...\n");
	}

	return scene_para;//返回framewin窗口创建的地址	
}
//视频控制菜单图层删除函数
__s32 new_movie_spsc_ctrl_menu_ui_scene_delete(void *handle)
{
	new_movie_spsc_ctrl_menu_scene_t *scene_para;
	scene_para = (new_movie_spsc_ctrl_menu_scene_t *)handle;//manwin父窗口中用于创建视频控制菜单的framewin句柄指针

	if(NULL == handle)//判断是否创建有视频控制菜单的framewin窗口句柄
	{
		__wrn("invalid para..\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hfrm)//视频控制菜单内部的famewin窗口句柄是否创建成功
	{
		__wrn("invalid para..\n");
		return EPDK_FAIL;
	}
	__wrn("scene_para->hfrm = %x ok...\n",scene_para->hfrm);
	GUI_FrmWinDelete(scene_para->hfrm);//删除本视频控制菜单framewin窗口句柄
	__wrn("scene_para->hlyr = %x ok...\n",scene_para->hlyr);
	if(!scene_para->hlyr)//视频控制菜单内部的famewin窗口图层是否创建成功
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	GUI_LyrWinDelete(scene_para->hlyr);//删除当前的视频控制菜单framewin窗口图层
	eLIBs_memset(scene_para, 0, sizeof(new_movie_spsc_ctrl_menu_scene_t));//清空结构体数据
	esMEMS_Bfree(scene_para, sizeof(new_movie_spsc_ctrl_menu_scene_t));//释放该结构体的空间内存
	return EPDK_OK;
}

//按键信息下发到视频控制菜单界面函数
__s32 new_movie_spsc_ctrl_menu_scene_set_focus(void *handle)
{
	new_movie_spsc_ctrl_menu_scene_t *scene_para;
	scene_para = (new_movie_spsc_ctrl_menu_scene_t *)handle;//传入的创建好framewin窗口的地址
	__here__;

	if(NULL == handle)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	__here__;

	if(!scene_para->hfrm)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	__here__;
	GUI_WinSetFocusChild(scene_para->hfrm);//发送按键信息
	__here__;
	return EPDK_OK;
}

