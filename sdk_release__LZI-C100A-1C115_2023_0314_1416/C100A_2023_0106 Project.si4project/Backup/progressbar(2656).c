#include 	"progressbar.h"
#include 	"progressbar_ui_para.h"


//创建图层
static H_LYR  __progressbar_8bpp_layer_create(RECT *rect, __s32 pipe)
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

//时间转换,提前时间
static void __time2time(__u32 ms, progressbar_cuckoo_time_t *format_time)
{
	__u32  total_minute;
	__u32 second_time;
	second_time = ms / 1000;		//秒
	total_minute = second_time / 60;	//分
	format_time->data1.second = second_time % 60;//求余
	format_time->data1.minute = total_minute % 60;
	format_time->data1.hour   = total_minute / 60;
}

//申请图片使用资源
static __s32 progressbar_ui_para_init(void)
{
	progress_uipara_t *ui_para;
	ui_para = progress_get_uipara(1);//获取进度条等控件大小和坐标

	if(!ui_para->uipara_icon.res_hdl)
	{
		ui_para->uipara_icon.res_hdl = theme_open(ID_MOVIE_PROG_BG_UF_BMP);//进度条背景图
	}
	
	if(!ui_para->uipara_icon.res_hdl1)
	{
		ui_para->uipara_icon.res_hdl1 = theme_open(ID_MOVIE_PROG_CURSOR_UF_BMP);//进度条图
	}
	return EPDK_OK;
}

//释放图片使用资源
static __s32 progressbar_ui_para_deinit(void)
{
	progress_uipara_t *ui_para;
	ui_para = progress_get_uipara(1);
	if(ui_para->uipara_icon.res_hdl)
	{
		theme_close(ui_para->uipara_icon.res_hdl);
		ui_para->uipara_icon.res_hdl = NULL;
	}

	if(ui_para->uipara_icon.res_hdl1)
	{
		theme_close(ui_para->uipara_icon.res_hdl1);
		ui_para->uipara_icon.res_hdl1 = NULL;
	}
	return EPDK_OK;
}
//进度条内部参数初始化
static __s32 progressbar_init_internal_para(progressbar_scene_t *scene_para)
{
	if(NULL == scene_para)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}
	scene_para->cur_time_index = -1;
	scene_para->prog_timmer_id = 0x18;
	scene_para->prog_hide_timmer_id = 0x19;
	return EPDK_OK;
}

//进度条绘制
static __s32 __progressbar_update_prog_ui(progressbar_scene_t *scene_para, __u32 ui_type)
{
	progress_uipara_t *ui_para;
	ui_para = progress_get_uipara(1);//获取进度条矩形大小和坐标

	if(NULL == ui_para)
	{
		__wrn("uipara fail...\n");
		return EPDK_FAIL;
	}

	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr)
	{
		__wrn("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
	{
		__wrn("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(scene_para->hlyr);//选择图层
	com_memdev_create(scene_para->hlyr);//创建伪内存设备

	//进度条图层背景
	if(ui_type & PROGRESSBAR_UI_TYPE_BG)
	{
		GUI_RECT gui_rect;
		GUI_SetBkColor(0xF0);
		gui_rect.x0 = 0;
		gui_rect.y0 = 0;
		gui_rect.x1 = gui_rect.x0 + ui_para->uipara_lyr.w;
		gui_rect.y1 = gui_rect.y0 + ui_para->uipara_lyr.h;
		GUI_ClearRectEx(&gui_rect);
		__wrn("MOVIE_PROG_UI_TYPE_BG is create...\n");
	}
	
	//当前时间
	__here__;

	if(ui_type & PROGRESSBAR_UI_TYPE_CUR_TIME)
	{
		char str_val[32] = {0};
		GUI_RECT gui_rect;
		progressbar_cuckoo_time_t format_time;
		__u32 t;
		__here__;

		if(-1 == scene_para->cur_time_index)//普通更新，从robin获取时间
		{
			t = robin_get_cur_time();//获取当前播放时间
			__time2time(t, &format_time);//时间转换
			eLIBs_memcpy(&scene_para->cur_time, &format_time, sizeof(progressbar_cuckoo_time_t));//备份一次时间，给用于返回到外部
		}
		else if(scene_para->cur_time_index >= 0 || scene_para->cur_time_index < 3)//设置时间，从外部获取时间
		{
			eLIBs_memcpy(&format_time, &scene_para->cur_time, sizeof(progressbar_cuckoo_time_t));
		}
		else
		{
			__err("invalid para...\n");
			com_memdev_delete();//删除伪内存设置
			return EPDK_FAIL;
		}

		if(SWFFont)
		{
			GUI_SetFont(SWFFont);//设置文本
		}
		else
		{
			GUI_SetFont(GUI_GetDefaultFont());
		}

		GUI_SetColor(APP_COLOR_WHITE);//设置文本颜色
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		{//绘制文本显示
			__s32 i;
			__here__;

			for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_cur_time) ; i++)
			{
				if(i == scene_para->cur_time_index)
				{
					GUI_SetBkColor(0xF1);//亮选该时间位置
				}
				else
				{
					GUI_SetBkColor(0xF0);
				}
				//当前播放时间显示位置
				gui_rect.x0 = ui_para->uipara_prog_cur_time[i].x;
				gui_rect.y0 = ui_para->uipara_prog_cur_time[i].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_cur_time[i].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_cur_time[i].h;
				GUI_ClearRectEx(&gui_rect);
				eLIBs_sprintf(str_val, "%.2d", format_time.data2[i]);
				GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
			}
		}
		{
			__s32 i;
			__here__;

			for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_cur_time_dot) ; i++)
			{
				gui_rect.x0 = ui_para->uipara_prog_cur_time_dot[i].x;
				gui_rect.y0 = ui_para->uipara_prog_cur_time_dot[i].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_cur_time_dot[i].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_cur_time_dot[i].h;
				eLIBs_sprintf(str_val, ":");
				GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
			}
		}
	}
	//总时间
	__here__;

	if(ui_type & PROGRESSBAR_UI_TYPE_CUR_TIME)
	{
		char str_val[32] = {0};
		GUI_RECT gui_rect;
		progressbar_cuckoo_time_t format_time;
		__u32 t;
		__here__;

		{
			t = robin_get_total_time();//获取总播放时间
			__time2time(t, &format_time);//时间转换
		}

		if(SWFFont)
		{
			GUI_SetFont(SWFFont);//设置文本
		}
		else
		{
			GUI_SetFont(GUI_GetDefaultFont());
		}

		GUI_SetColor(APP_COLOR_WHITE);//设置文本颜色
		GUI_SetBkColor(0xF0);//设置背景颜色
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		{//绘制总时间文本显示
			__s32 i;
			__here__;

			for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_total_time) ; i++)
			{

				//当前播放时间显示位置
				gui_rect.x0 = ui_para->uipara_prog_total_time[i].x;
				gui_rect.y0 = ui_para->uipara_prog_total_time[i].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_total_time[i].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_total_time[i].h;
				GUI_ClearRectEx(&gui_rect);
				eLIBs_sprintf(str_val, "%.2d", format_time.data2[i]);
				GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
			}
		}
		{//绘制总时间的分隔点
			__s32 i;
			__here__;

			for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_total_time_dot) ; i++)
			{
				gui_rect.x0 = ui_para->uipara_prog_total_time_dot[i].x;
				gui_rect.y0 = ui_para->uipara_prog_total_time_dot[i].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_total_time_dot[i].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_total_time_dot[i].h;
				eLIBs_sprintf(str_val, ":");
				GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
			}
		}
	}
	
	//画进度条显示
	__here__;

	if(ui_type & PROGRESSBAR_UI_TYPE_PROG)
	{
		//画进度条背景
		__here__;
		{
			void *pbmp;
			
			if(!ui_para->uipara_icon.res_hdl)
			{
				__wrn("ui_para->uipara_icon.res_hdl is null...\n");
				com_memdev_delete();
				return EPDK_FAIL;
			}

			pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon.res_hdl);//获取图片数据

			if(!pbmp)
			{
				__wrn("pbmp is null...\n");
				com_memdev_delete();
				return EPDK_FAIL;
			}

			GUI_BMP_Draw(pbmp, ui_para->uipara_icon.x, ui_para->uipara_icon.y);//绘制图片
		}
		//画进度条移动部分
		__here__;
		{
			void *pbmp;
			__u32 cur_time;
			__u32 total_time;
			cur_time = robin_get_cur_time();
			total_time = robin_get_total_time();

			if(!ui_para->uipara_icon.res_hdl1)
			{
				__wrn("ui_para->uipara_icon is null...\n");
				com_memdev_delete();
				return EPDK_FAIL;
			}

			pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon.res_hdl1);//进度条移动部分图片数据

			if(!pbmp)
			{
				__wrn("pbmp is null...\n");
				com_memdev_delete();
				return EPDK_FAIL;
			}

			{
				
				__s32 i=0;
				__s32 n;
				__wrn("cur_time=%d total_time=%d\n", cur_time, total_time);
				if(cur_time <= 0 || total_time <= 0)
				{
					n = 0;
				}
				else
				{
					__here__;
					n = (ui_para->uipara_progressbar_icon.w * cur_time)
					    / (ui_para->uipara_progressbar_icon.w * total_time);
					__here__;
				}

				__msg("n= %d \n", n);
				__here__;
				
					
				for(i = 0 ; i < n ; i++)
				{
					__here__;
					GUI_BMP_Draw(pbmp, ui_para->uipara_icon.x + i * ui_para->uipara_icon.w
					             , ui_para->uipara_icon.y);//绘制图片
					__here__;
				}
			}
		}
	}

	__here__;
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
	com_memdev_delete();//删除伪内存设备
	return EPDK_OK;
}

//配置定时器
static __s32 progressbar_install_prog_timmer(progressbar_scene_t *scene_para)
{
	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("progressbar timmer  install...\n");

	if(!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
	{
		GUI_SetTimer(scene_para->hfrm, scene_para->prog_timmer_id
		             , 400 * 0.025, NULL);
	}
	else
	{
		__msg("timmer already install...\n");
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
//释放定时器
static __s32 progressbar_uninstall_prog_timmer(progressbar_scene_t *scene_para)
{
	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
	{
		GUI_KillTimer(scene_para->hfrm, scene_para->prog_timmer_id);
	}

	return EPDK_OK;
}

//进度条回调处理函数
static __s32 __progressbar_proc(__gui_msg_t *msg)
{
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			progressbar_scene_t *scene_para;
			__wrn("__prog_proc GUI_MSG_CREATE begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);
			if(!scene_para)
			{
				__wrn("invalid para...\n");
				return EPDK_FAIL;
			}
			scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
			progressbar_init_internal_para(scene_para);//进度条内部参数初始化，包括定时器
			progressbar_ui_para_init();//进度条图片使用资源申请
			__progressbar_update_prog_ui(scene_para, PROGRESSBAR_UI_TYPE_ALL);//绘制进度条函数
			progressbar_install_prog_timmer(scene_para);//开启定时器
			GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);//图层窗口状态打开
			GUI_LyrWinSetTop(scene_para->hlyr);
			__wrn("__prog_proc GUI_MSG_CREATE end\n");
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1,销毁*/
			progressbar_scene_t *scene_para;
			__wrn("__prog_proc GUI_MSG_DESTROY begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}


			__wrn("before __prog_deinit_ui\n");
			progressbar_ui_para_deinit();
			progressbar_uninstall_prog_timmer(scene_para);//释放定时器
			__wrn("after __prog_deinit_ui\n");
			__wrn("__prog_proc GUI_MSG_DESTROY end\n");
		}
		return EPDK_OK;
		
		case GUI_MSG_PAINT:{/*2，绘制*/
			progressbar_scene_t *scene_para;
			__wrn("__prog_proc  begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			if(GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
			{
				__wrn("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
				return EPDK_OK;
			}

			__progressbar_update_prog_ui(scene_para, PROGRESSBAR_UI_TYPE_ALL);
			
			__wrn("__prog_proc GUI_MSG_PAINT begin\n");
		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{/*3，关闭*/

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{/*4，退出*/

		}
		return EPDK_OK;
		
		case GUI_MSG_KEY:{/*5，按键*/

		}
		break;
		
		case GUI_MSG_TIMER:{/*定时器*/
			__s32 ret;
			progressbar_scene_t *scene_para;
			__wrn("__prog_proc GUI_MSG_TIMER begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);
			ret = -1;

			if(scene_para)
			{
				if(LOWORD(msg->dwAddData1) == scene_para->prog_timmer_id)
				{
					static __u32 cnt = 0;
					__progressbar_update_prog_ui(scene_para, PROGRESSBAR_UI_TYPE_CUR_TIME);//当前时间

					if((cnt % 30) == 0)
					{
						__wrn("before __progressbar_update_prog_ui\n");
						__progressbar_update_prog_ui(scene_para, PROGRESSBAR_UI_TYPE_TOTAL_TIME);//总时间
						__wrn("after __progressbar_update_prog_ui\n");
					}

					if((cnt % 100) == 0)
					{
						__wrn("before __progressbar_update_prog_ui\n");
						__progressbar_update_prog_ui(scene_para, PROGRESSBAR_UI_TYPE_PROG);//进度条
						__wrn("after __progressbar_update_prog_ui\n");
					}

					cnt++;
					ret = EPDK_OK;
				}
				/*else if(LOWORD(msg->dwAddData1) == scene_para->prog_hide_timmer_id)
				{
					movie_cmd2parent(msg->h_deswin, movie_prog_scene_msg_timeout, 0, 0);
					ret = EPDK_OK;
				}*/
			}
			else
			{
				__err("scene_para is null...\n");
			}

			__wrn("__prog_proc GUI_MSG_TIMER end\n");

			if(-1 == ret) //未处理，交给子场景处理
			{
				break;
			}
			else//已处理
			{
				return EPDK_OK;
			}
		}
		break;

		

		default:{
			break;
		}
		
	}

	return GUI_FrmWinDefaultProc(msg);
}

//进度条framewin窗口创建
void	*progressbar_frmwin_create(progressbar_create_para_t *create_para)
{
	__gui_framewincreate_para_t	framewin_para;
	progressbar_scene_t *sence_para;
	progress_uipara_t *ui_para;
	RECT lyr_rect;
	sence_para = (progressbar_scene_t *)esMEMS_Balloc(sizeof(progressbar_scene_t));

	if(!sence_para)
	{
		__wrn("mem not enough...\n");
		return NULL;
	}

	eLIBs_memset(sence_para, 0, sizeof(progressbar_scene_t));
	sence_para->hparent = create_para->hparent;//传入窗口
	sence_para->scene_id = create_para->scene_id;//传入窗口id
	ui_para = progress_get_uipara(1);//进度条ui坐标参数获取

	if(NULL == ui_para)//获取进度条UI坐标和大小参数失败
	{
		esMEMS_Bfree(sence_para, sizeof(progressbar_scene_t));//释放内存空间
		sence_para = NULL;
		__wrn("movie_spsc_get_uipara fail...\n");
		return NULL;
	}
	//进度条图层大小 58，40，364，24
	lyr_rect.x		= ui_para->uipara_lyr.x;
	lyr_rect.y		= ui_para->uipara_lyr.y;
	lyr_rect.width	= ui_para->uipara_lyr.w;
	lyr_rect.height = ui_para->uipara_lyr.h;
	sence_para->hlyr = __progressbar_8bpp_layer_create(&lyr_rect, ui_para->pipe);//创建进度条图层
	__wrn("lyr_rect: x=%d, y=%d, w=%d, h=%d...\n",lyr_rect.x, lyr_rect.y,lyr_rect.width, lyr_rect.height);

	if(NULL == sence_para->hlyr)//图层创建失败
	{
		esMEMS_Bfree(sence_para, sizeof(progressbar_scene_t));//释放内存空间
		sence_para = NULL;
		__wrn("sence_para->hlyr is null...\n");
		return NULL;
	}
	/*=====================开始进度条framwin窗口配置======================*/
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name			=	"progressbar_win",
					 framewin_para.dwExStyle		= WS_EX_NONE;
	framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
	framewin_para.spCaption 	= NULL;
	framewin_para.hOwner		= NULL;
	framewin_para.id			= sence_para->scene_id;//窗口id
	framewin_para.hHosting		=  sence_para->hparent;//窗口
	framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__progressbar_proc);
	framewin_para.rect.x		= ui_para->uipara_frm.x;
	framewin_para.rect.y		= ui_para->uipara_frm.y;
	framewin_para.rect.width	= ui_para->uipara_frm.w;
	framewin_para.rect.height	= ui_para->uipara_frm.h;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red	= 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue	= 0;
	framewin_para.attr			= (void *)sence_para;
	framewin_para.hLayer		= sence_para->hlyr;
	sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);
	__wrn("sence_para->hlyr = %x ok...\n",sence_para->hlyr);

	if(NULL == sence_para->hfrm)//创建framewin窗口失败
	{
		esMEMS_Bfree(sence_para, sizeof(progressbar_scene_t));//释放内存
		sence_para = NULL;
		__wrn("GUI_FrmWinCreate fail...\n");
		return NULL;
	}

	if(sence_para->hlyr)//图层创建成功
	{
		__wrn("sence_para->hlyr...\n");
		GUI_LyrWinSetSta(sence_para->hlyr, GUI_LYRWIN_STA_ON);//图层窗口打开
		GUI_LyrWinSetTop(sence_para->hlyr);
	}
	else
	{
		__err("invalid para...\n");
	}

	return sence_para;

}

