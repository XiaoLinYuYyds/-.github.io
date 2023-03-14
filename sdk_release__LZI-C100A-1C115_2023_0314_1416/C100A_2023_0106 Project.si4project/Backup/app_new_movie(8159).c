/***********************************************************************************************************
*Copyright © 	LJT owns it. date:2023-02-02 
*File name: 	app_new_movie.c	// 文件名
*Author： 		LJT				//作者
*Version: 		NULL			//版本号
*Description: 	new movie app manwin create
*				创建movie视频app应用程序的manwin窗口界面
*				
*Others: 		
*Log: 			2023/2/2 // 修改日志，包括修改内容，日期，修改人等
***********************************************************************************************************/

#include "app_new_movie.h"



/***********************************************************************************************************
*@Function	:          static H_LYR New_movie_32bpp_layer_create(RECT *rect,__s32 pipe)
*
*@Description	: 		建立图层，
*						注意与背景图应处于不同的pipe
*@Return     	: 	    返回layer图层句柄
*@Log			:		2023/2/2
************************************************************************************************************/
static H_LYR New_movie_32bpp_layer_create(RECT *rect,__s32 pipe)
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


/***********************************************************************************************************
*@Function	:          static H_LYR New_movie_32bpp_layer_create(RECT *rect,__s32 pipe)
*
*@Description	: 		向父窗口发送命令
*						向APP_NEWSETTING_ID新添加的app应用程序manwin父窗口发送按键通知消息
*@Return     	: 	    NULL
*@Log			:		2023/2/2
************************************************************************************************************/
void  app_new_movie_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = GUI_WinGetParent(hwin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	__wrn("**********app_new_movie_cmd2parent**********\n");
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);//GUI向manwin父窗口发送管理通知
}



























/***********************************************************************************************************
*@Function: static __s32 new_movie_install_auto_play_timer(new_movie_ctrl_t *new_movie_ctrl)
*
*@Description	: 		自动播放视频文件定时器配置
*						
*@Return     	: 	    返回EPDK_OK或EPDK_FAIL
*@Log			:		2023/2/2
************************************************************************************************************/
static	__s32	new_movie_install_auto_play_timer(new_movie_ctrl_t	*new_movie_ctrl)
{
	if(!new_movie_ctrl){
		__wrn("new_movie_ctrl is null...\n");
		return EPDK_FAIL;
	}
	if(!GUI_IsTimerInstalled(new_movie_ctrl->new_movie_win,  new_movie_ctrl->auto_play_timmer_id)){/*定时器加载*/
		GUI_SetTimer(new_movie_ctrl->new_movie_win, new_movie_ctrl->auto_play_timmer_id, 
					500, NULL);/*设置定时时间为0.5秒*/
		__wrn("GUI_SetTimer is success...\n");
	}
	return EPDK_OK;
}


/***********************************************************************************************************
*@Function: static __s32 new_movie_play_file(new_movie_ctrl_t *new_movie_ctrl, __s32 index, __bool breakplay)
*
*@Description	: 		播放视频文件
*						
*@Return     	: 	    返回一个ret：EPDK_OK或EPDK_FAIL
*@Log			:		2023/2/2
************************************************************************************************************/
static	__s32	new_movie_play_file(new_movie_ctrl_t	*new_movie_ctrl, __s32 index, __bool breakplay)
{
	__s32 ret,ret1;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};	/*存放文件名*/
	__cedar_tag_inf_t	tag_inf;
	__cedar_status_t	status;
	__u32	cur_time,total_time;
	ret = robin_npl_index2file(index, filename);/*在当前播放列表中获取与其索引相对应的文件*/
	if(ret == -1){
		__wrn("robin_npl_index2file is fail...\n");
		return EPDK_FAIL;
	}
	__wrn("breakplay=%d\n",breakplay);			/*断点播放*/
	__wrn("filename=%s\n",filename);			/*打印获取到的文件名*/
	
	#if 1										/*直接播放文件*/
		robin_set_cmd_stop();					/*设置停止命令，该操作将向FSM发送stop命令，并等待FSM处于“停止”状态；用于停止*/
		ret = robin_play_file(filename, NULL);	/*直接播放文件*/
		__wrn("ret = %d\n",ret);
		if(ret == EPDK_FAIL){
			__wrn("robin_play_file is fail...\n");
			return EPDK_FAIL;
		}
		cur_time = robin_get_cur_time();		/*获取当前文件的播放时间*/
		__wrn("robin_get_cur_time = %d \n",cur_time);
		total_time = robin_get_total_time();	/*获取当前播放文件的总时间*/
		__wrn("robin_get_total_time = %d \n",total_time);
		//robin_clear_feedback_msgQ();			//FSM创建了一个消息队列，提供即时错误消息。这操作可以清除队列
		status = robin_get_fsm_status();				/*获取FSM当前的状态*/
		__wrn("robin_get_fsm_status = %d \n",status);

		robin_set_cmd_play_file(filename, NULL);	/*设置播放文件命令*/
		ret1 = robin_set_cmd_play();				/*设置播放命令，该操作将向FSM发送play命令，并等待FSM处于“播放”状态；用于播放*/
		if(ret1 == -1){
			__wrn("robin_set_cmd_play is fail...\n");
		}
		
	#endif
	if(ret == EPDK_OK){							/*配置自动播放定时器*/
		new_movie_install_auto_play_timer(new_movie_ctrl);
	}
	return ret;
}


/***********************************************************************************************************
*@Function	:          static	__s32	new_movie_start_play(new_movie_ctrl_t	*new_movie_ctrl)
*
*@Description	: 		视频开始播放函数
*						
*@Return     	: 	    返回一个ret：EPDK_OK或EPDK_FAIL
*@Log			:		2023/2/2
************************************************************************************************************/
static	__s32	new_movie_start_play(new_movie_ctrl_t	*new_movie_ctrl, __bool breakplay, __s32 set_index)
{
	__s32	ret;
	__s32	index;
	robin_npl_set_cur(set_index);	/*设置当前播放的文件索引*/
	index	=	robin_npl_get_cur();/*获取当前播放列表中要播放的当前播放文件的索引*/
	//index	=	robin_npl_get_prev();/*获取当前播放列表中要播放的上一个播放文件的索引*/
	__wrn("index = %d\n",index);
	if(index == -1){
		__wrn("robin_npl_get_cur if fail...\n");
		return EPDK_FAIL;
	}
	
	ret = new_movie_play_file(new_movie_ctrl, index, breakplay);/*播放文件*/
	return ret;
}







/***********************************************************************************************************
*@Function	:          static __s32	new_movie_ctrl_para_init(new_movie_ctrl_t	*new_movie_ctrl)
*
*@Description	: 		视频应用控制参数初始化，申请使用robin模块资源
*						
*@Return     	: 	    返回EPDK_OK
*@Log			:		2023/2/2
************************************************************************************************************/
static	__s32	new_movie_ctrl_para_init(new_movie_ctrl_t	*new_movie_ctrl, RECT *vedio_rect)
{
	if(!new_movie_ctrl){
		__wrn("new_movie_ctrl is null...\n");
		return EPDK_FAIL;
	}
	new_movie_ctrl->hdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r");
	__wrn("disp is =%s\n",new_movie_ctrl->hdisp);
	if(!new_movie_ctrl->hdisp){
		__msg("open disp fail...\n");
	}
	
	{
		__s32 ret;
		robin_open_arg_t	arg;
		arg.cedar_stop_mode	  =	CEDAR_STOP_MODE_UNINSTALL_PLUGINS;	/*停止播放时,卸载所有插件*/
		arg.reserve_mem_size  =	1024*1024;							/*预留1M字节*/
		arg.switch_vplay_mode =	CEDAR_FILE_SWITCH_VPLY_MODE_GAP;	/*2个文件切换*/
		ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);				/*打开robin模块接口*/
		if(ret != EPDK_OK){
			__wrn("robin_open is fail...\n");
			return EPDK_FAIL;
		}
		__wrn("robin_open is success...\n");
	}
	//new_movie_ctrl->hvedio_lyr = robin_request_video_layer(vedio_rect, 1, 0xff);/*申请robin模块接口视频图层资源*/
	__wrn("vedio_rect:para x=%d, y=%d, width=%d, height=%d\n",vedio_rect->x, vedio_rect->y, vedio_rect->width, vedio_rect->height);
	/*if(!new_movie_ctrl->hvedio_lyr){
			__wrn("new_movie_ctrl->hvedio_lyr layer is null.....\n");
			return EPDK_FAIL;
	}
		__wrn("hvedio_lyr layer is success...\n");*/	
	/*{
		__s32 ret;
		__u32 arg[3] = {0};
		arg[0] = new_movie_ctrl->hvedio_lyr;
		ret = eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
		
		if(0 != ret)
		{
			__msg("DISP_CMD_LAYER_BOTTOM fail...\n");
		}
		else
		{
			__msg("DISP_CMD_LAYER_BOTTOM success...\n");
		}
	}*/
	//new_movie_ctrl->auto_play_timmer_id = 0x10;/*自动播放定时器id*/
	return EPDK_OK;
}




/***********************************************************************************************************
*@Function	:          static __s32	new_movie_scene_create(__gui_msg_t *msg)
*
*@Description	: 		视频播放图层等场景创建
*						
*@Return     	: 	    
*@Log			:		2023/2/2
************************************************************************************************************/
static	__s32	new_movie_scene_create(__gui_msg_t *msg)
{
	new_movie_ctrl_t	*new_movie_ctrl;
	SIZE screen_size;
	RECT vedio_rect;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	new_movie_ctrl = (new_movie_ctrl_t *)My_Balloc(sizeof(new_movie_ctrl_t));
	if(!new_movie_ctrl){
		__wrn("mem not enough...\n");
		return EPDK_FAIL;
	}
/*
	__here__;
	g_disable_close_scn();
	SEND_MSG(DSK_MSG_RELEASE_LOGO_BUF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
	SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
	__wrn("SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF)\n");
	com_set_backcolor(CONST_BackColor_Black);
	__here__;
	__movie_robin_sem_init();
	*/
	__here__;
	eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));
	__here__;
	GUI_WinSetAddData(msg->h_deswin, (__u32)new_movie_ctrl);
	__here__;
	new_movie_ctrl->new_movie_win		= msg->h_deswin;
	__here__;
	/*com_set_palette_by_id(ID_MOVIE_PAL_BMP);//设置调色板
	__here__;
	{
		//修改调色板
		//背景色，灰白色，字幕边框颜色
		__u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
		com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
	}
	__here__;*/
	__wrn("before __movie_cur_file_has_breakpoint\n");
	#if 1	/*图层显示矩形大小1024*600*/
		vedio_rect.x = 0;
		vedio_rect.y = 0;
		vedio_rect.width	=	screen_size.width;
		vedio_rect.height	=	screen_size.height;
		__wrn("vedio_rect: x=%d, y=%d, width=%d, height=%d\n",vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
	#endif
	#if 1	/*视频图层资源申请*/
		new_movie_ctrl_para_init(new_movie_ctrl,&vedio_rect);
		new_movie_ctrl->hvedio_lyr	=	robin_request_video_layer(&vedio_rect, 0, 0xff);/*申请robin接口视频图层资源*/
		//new_movie_ctrl->new_movie_lyr	=	New_movie_32bpp_layer_create(&vedio_rect, 1);/*普通图层*/
		if(!new_movie_ctrl->hvedio_lyr){
			__wrn("new_movie_ctrl->hvedio_lyr layer is null.....\n");
			return EPDK_FAIL;
		}
		__wrn("hvedio_lyr layer is success...\n");
		/*if(!new_movie_ctrl->new_movie_lyr){
			__wrn("new_movie_ctrl layer is null.....\n");
			return EPDK_FAIL;
		}*/

		/*{
			__s32 ret;
			__u32 arg[3] = {0};
			arg[0] = new_movie_ctrl->hvedio_lyr;
			ret = eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
			
			if(0 != ret)
			{
				__msg("DISP_CMD_LAYER_BOTTOM fail...\n");
			}
			else
			{
				__msg("DISP_CMD_LAYER_BOTTOM success...\n");
			}
		}*/
		new_movie_ctrl->auto_play_timmer_id = 0x10;
		
	#endif
	#if 1	/*播放视频文件*/
		new_movie_start_play(new_movie_ctrl, 0,0);
		__wrn("new_movie_start_play is success.....\n");
	#endif

	return EPDK_OK;
}

/***********************************************************************************************************
*@Function	:          	static __s32	_new_movie_manager_win_proc(__gui_msg_t *msg)
*
*@Description	: 		管理窗口回调处理函数，
*						功能有：创建、绘制、关闭、销毁、退出、按键、命令信号
*@Return     	: 	    GUI_ManWinDefaultProc(msg)：返回一个manwin窗口回调信号
*@Log			:		2023/2/2
************************************************************************************************************/
static __s32	_new_movie_manager_win_proc(__gui_msg_t *msg)
{
	static __s32  enter_key_num = 0;
	switch(msg->id){
	case GUI_MSG_CREATE:/*0,创建*/
		gscene_hbar_set_state(HBAR_ST_HIDE);
		gscene_bgd_set_state(BGD_STATUS_SHOW);
		new_movie_scene_create(msg);
		return EPDK_OK;
	case GUI_MSG_DESTROY:{/*1,销毁*/
		new_movie_ctrl_t 	*new_movie_ctrl;
		
		//new_movie_ctrl	=	(new_movie_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
		new_movie_ctrl	=	(new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);/*获取manwin窗口属性存放的结构体数据*/
		if(new_movie_ctrl->hvedio_lyr)//新添加的通用设置图层，清空
		{	
			//robin_release_video_layer(new_movie_ctrl->hvedio_lyr);/*释放robin模块申请的视频图层*/
			__here__ ;
			__wrn(" hvedio_lyr isn't null, delete it... \n");
			//GUI_LyrWinDelete(new_movie_ctrl->hvedio_lyr);//删除新添加的app应用图层
			//GUI_LyrWinDelete(h_lyr);
			__wrn(" hvedio_lyr isn't null, delete it OK\n");
			__here__ ;
			//new_movie_ctrl->hvedio_lyr = NULL ;//赋值为空
		}
			__wrn("new_movie_ctrl destory begin is ok\n");

		My_Bfree(new_movie_ctrl, sizeof(new_movie_ctrl_t));					 	 /*释放该manwin窗口属性存放的结构体内存空间*/
		__wrn("new_movie_ctrl is free success.....\n");
	}
		return EPDK_OK;
	case GUI_MSG_PAINT:	 /*2,绘制*/
		
		return EPDK_OK;
	case GUI_MSG_CLOSE:	 /*3,关闭*/
		app_new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_MOVIE_SW_TO_NEW_EXPLORER, 0);	/*发送命令，跳转到new explorer资源管理器应用程序的manwin窗口*/
		return EPDK_OK;
	case GUI_MSG_QUIT:	 /*4,退出*/
		app_new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);		/*发送命令，跳转到home应用界面的manwin窗口*/
		return EPDK_OK;
	case GUI_MSG_KEY:	 /*5,按键*/

		#if 1
			switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{/*返回按键按下，发送退出信号*/
					__gui_msg_t	my_msg;
					__wrn("GUI_MSG_KEY_MENU is press....\n");
					my_msg.h_deswin	 =	msg->h_deswin;
					my_msg.h_srcwin	 =	msg->h_srcwin;
					my_msg.id		 =	GUI_MSG_QUIT;/*发送退出信号*/
					my_msg.dwAddData1 = NULL;
					my_msg.dwAddData2 = NULL;
					GUI_SendNotifyMessage(&my_msg);
				}
					break;
				case GUI_MSG_KEY_CLEAR:{/*发送关闭信号*/
					__gui_msg_t my_msg;
					__wrn("GUI_MSG_KEY_CLEAR is press....\n");
					my_msg.h_deswin	 = msg->h_deswin;
					my_msg.h_srcwin	 =	msg->h_srcwin;
					my_msg.id		 = GUI_MSG_CLOSE;/*发送关闭信号*/
					my_msg.dwAddData1 = NULL;
					my_msg.dwAddData2 = NULL;
					GUI_SendNotifyMessage(&my_msg);
				}
					break;
				case GUI_MSG_KEY_ENTER:{/*发送关闭信号*/
					__gui_msg_t my_msg;
					enter_key_num += 1;
					__wrn("GUI_MSG_KEY_ENTER is press %d count....\n",enter_key_num);
					if(enter_key_num == 3){
						enter_key_num = 0;
						my_msg.h_deswin	 = msg->h_deswin;
						my_msg.h_srcwin	 =	msg->h_srcwin;
						my_msg.id		 = GUI_MSG_CLOSE;/*发送关闭信号*/
						my_msg.dwAddData1 = NULL;
						my_msg.dwAddData2 = NULL;
						GUI_SendNotifyMessage(&my_msg);
					}
				}
					break;
			}
		#endif

		break;
	case GUI_MSG_COMMAND:/*7,命令*/

		return EPDK_OK;
	
	default:/*否则直接退出switch语句*/
		break;
	}
	return GUI_ManWinDefaultProc(msg);
}



/***********************************************************************************************************
*Function	:          	H_WIN app_new_movie_manwin_create(root_para_t *para)
*
*Description	: 		建立manwin窗口，
*						
*Return     	: 	    返回一个创建好的manwin窗口句柄
*Log			:		2023/2/2
************************************************************************************************************/
H_WIN app_new_movie_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	/*new_movie_ctrl_t 	*new_movie_ctrl;

	#if 0	//检测manwin窗口是否内存泄漏-->查看打印的Pages free的大小反复切换manwin界面是否会恢复
	__wrn("esKSRV_SysInfo  manwin is...\n");				//打印内存是否泄露
	esKSRV_SysInfo();								//查看系统内存消息是否泄露或溢出
	//esMEMS_FreeMemSize();							//剩余内存大小以KB为单位
	//esMEMS_TotalMemSize();						//总的内存大小
	__wrn("esKSRV_SysInfo | esMEMS_FreeMemSize = %d \n",esMEMS_FreeMemSize());//打印剩余内存
	__wrn("esKSRV_SysInfo | esMEMS_TotalMemSize = %d \n",esMEMS_TotalMemSize());//打印总内存
	#endif
	
	new_movie_ctrl = (new_movie_ctrl_t *)My_Balloc(sizeof(new_movie_ctrl_t));
	if(!new_movie_ctrl){
		__wrn("new_movie_ctrl memory balloc fail.........");
		return EPDK_FAIL;
	}
	eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));
	
	new_movie_ctrl->root_para	=	para;
	new_movie_ctrl->root_type	=	para->root_type;*/
	
	/*************开始操作manwin窗口配置**********************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name	 		=	"app_new_movie";
	create_info.hParent	 		=	para->h_parent;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_movie_manager_win_proc);
	create_info.id				=	APP_NEWMOVIE_ID;//0x9800地址
	create_info.attr			=	(void *)para;
	create_info.hHosting 		=	NULL;
	
	return(GUI_ManWinCreate(&create_info));/*返回一个创建好的manwin窗口句柄*/
}





