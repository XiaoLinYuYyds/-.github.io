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


static __krnl_event_t *g_new_movie_robin_sem = NULL;

/***********************************************************************************************************
*@Function	:          static H_LYR New_movie_32bpp_layer_create(RECT *rect,__s32 pipe)
*
*@Description	: 		建立图层，
*						注意与背景图应处于不同的pipe管道
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
	lstlyr.src_win.x  		= 0;		/*源窗口大小*/
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= rect->width;
	lstlyr.src_win.height 	= rect->height;
	lstlyr.scn_win.x		= rect->x;	/*屏幕，荧光屏大小*/
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
*@Function	:          __s32 __new_movie_robin_sem_init(void)
*@Description	: 		robin组件初始化
*						
*@Return     	: 	    EPDK_OK或EPDK_FAIL
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
__s32 __new_movie_robin_sem_init(void)
{
	if(0 == g_new_movie_robin_sem){
		g_new_movie_robin_sem = esKRNL_SemCreate(1);
	}

	if(g_new_movie_robin_sem){
		return EPDK_OK;
	}
	else{
		return EPDK_FAIL;
	}
}

/***********************************************************************************************************
*@Function	:          __s32 __new_movie_robin_sem_deinit(void)
*@Description	: 		robin组件卸载不初始化
*						
*@Return     	: 	    EPDK_OK
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
__s32 __new_movie_robin_sem_deinit(void)
{
	if(g_new_movie_robin_sem){
		__u8 err;
		esKRNL_SemDel(g_new_movie_robin_sem, OS_DEL_ALWAYS, &err);
		g_new_movie_robin_sem = NULL;
	}

	return EPDK_OK;
}

/***********************************************************************************************************
*@Function	:          static void __app_new_movie_set_play_mode(void)
*@Description	: 		设置播放模式
*						
*@Return     	: 	    null
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
static void __app_new_movie_set_play_mode(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE)){/*获取到注册表app复位标志*/
		__wrn("play mode load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);/*获取*/
	}
	else{
		__wrn("play mode load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para){
		robin_set_play_mode(para->rotate_mode);/*默认设置为2，循环播放所有模式*/
		__wrn("para->rotate_mode=%d\n", para->rotate_mode);
	}
	else{
		robin_set_play_mode(RAT_PLAY_MODE_ROTATE_ALL);/*设置为2，循环播放模式*/
		__wrn("robin_set_play_mode is RAT_PLAY_MODE_ROTATE_ALL\n");
	}
}
/***********************************************************************************************************
*@Function	:          static void __app_new_movie_set_zoom_mode(void)
*@Description	: 		设置缩放模式
*						
*@Return     	: 	    null
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
static void __app_new_movie_set_zoom_mode(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))/*获取app复位标志*/
	{
		__wrn("zoom mode load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__wrn("zoom mode load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		robin_set_zoom(para->zoom_mode);
		__wrn("para->zoom_mode=%d\n", para->zoom_mode);
	}
}

/***********************************************************************************************************
*@Function	:          static __s32 __app_new_movie_reg_para_init(new_movie_ctrl_t *new_movie_ctrl)
*@Description	: 		注册表参数初始化
*						
*@Return     	: 	    EPDK_OK
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
static __s32 __app_new_movie_reg_para_init(new_movie_ctrl_t *new_movie_ctrl)
{
	__app_new_movie_set_play_mode();/*设置播放模式*/
	__app_new_movie_set_zoom_mode();/*设置缩放模式*/
	__wrn("__app_new_movie_reg_para_init is...\n");
	/*注册表设置app复位标志*/
	dsk_reg_set_app_restore_flag(REG_APP_MOVIE, 0);
	return EPDK_OK;
}


/*检查是否播放错误*/
static __s32 __new_movie_check_for_play_err(new_movie_ctrl_t *movie_ctrl)
{
	__s32 ret;
	ret = robin_get_feedback_msg();/*获取信息消息队列*/

	if(ret < 0)
	{
		robin_clear_feedback_msgQ();//清空错误信息消息队列
		__wrn("robin_get_feedback_msg < 0, movie play err...\n");
		//__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
		//__movie_playerror_scene_create(movie_ctrl);
		esKRNL_TimeDly(100);
		//__movie_playerror_scene_delete(movie_ctrl);
		//movie_ctrl->err_cnt++;

		/*if(movie_ctrl->err_cnt >= robin_npl_get_total_count()
		    || RAT_PLAY_MODE_ROTATE_ONE == robin_get_play_mode())
		{
			__movie_exit_movie(movie_ctrl, 0);
		}*/

		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*检查自动播放*/
static __s32 __new_movie_check_for_auto_play(new_movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__cedar_status_t fsm_sta;
		fsm_sta = robin_get_fsm_status();/*获取fsm状态机的状态*/
		__wrn("fsm_sta= %d\n", fsm_sta);

		if(CEDAR_STAT_PLAY == fsm_sta
		    || CEDAR_STAT_PAUSE == fsm_sta)
		{
			movie_ctrl->cur_time = robin_get_cur_time();/*获取当前播放的时间*/
			robin_get_fsm_cur_file(movie_ctrl->cur_playing_file, sizeof(movie_ctrl->cur_playing_file));/*获取当前的播放文件*/
			__wrn("movie_ctrl->cur_time = %s\n",movie_ctrl->cur_time);
			__wrn("movie_ctrl->cur_playing_file = %s\n",movie_ctrl->cur_playing_file);
		}
		else
		{
			movie_ctrl->cur_time = -1;
			eLIBs_strcpy(movie_ctrl->cur_playing_file, "");
		}

		switch(fsm_sta)
		{
			case CEDAR_STAT_STOP:
			{
				__s32 ret;
				/*movie_ctrl->cur_stop_cnt++;

				if(movie_ctrl->cur_stop_cnt < movie_ctrl->stop_cnt)
				{
					break;
				}*/

				//movie_ctrl->cur_stop_cnt = 0;
				__wrn("CEDAR_STAT_STOP, start play next...\n");
				//__wrn("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);

				{
					
					__wrn("ret=%d\n", ret);

					/*if(EPDK_OK != ret) //播放完毕,退出应用
					{
						__movie_playend_scene_create(movie_ctrl);
						esKRNL_TimeDly(100);
						__movie_exit_movie(movie_ctrl, 0);
					}*/
				}

				break;
			}

			case CEDAR_STAT_PLAY:
			{
				//movie_ctrl->rr_flag = 0;
				__wrn("movie_ctrl->rr_flag =\n");
				/*{
					//存在能正常播放的文件，把该值复位一下
					movie_ctrl->err_cnt = 0;
				}*/
				break;
			}


			default:
			{
				//movie_ctrl->rr_flag = 0;
				__wrn("fsm_sta=%d \n", fsm_sta);
				break;
			}
		}
	}

	return EPDK_OK;
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
	__wrn("new_movie_ctrl->auto_play_timmer_id = 0x%x\n",new_movie_ctrl->auto_play_timmer_id);
	if(!GUI_IsTimerInstalled(new_movie_ctrl->new_movie_win,  new_movie_ctrl->auto_play_timmer_id)){/*定时器加载*/
		GUI_SetTimer(new_movie_ctrl->new_movie_win, new_movie_ctrl->auto_play_timmer_id, 
					200, NULL);/*设置定时时间为0.5秒*/
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
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};				/*存放当前播放列表的文件名*/
	char cur_playing_file[RAT_MAX_FULL_PATH_LEN + 1] = {0};	/*存放当前FSM状态机播放的文件*/
	__cedar_tag_inf_t	tag_inf;
	__cedar_status_t	status;
	__u32	cur_time,total_time,fsm_busy;
	__s32	file_total;
	__disp_output_type_t output;/*显示驱动输出的类型*/
	ret = robin_npl_index2file(index, filename);/*在当前播放列表中获取与其索引相对应的文件*/
	if(ret == -1){
		__wrn("robin_npl_index2file is fail...\n");
		return EPDK_FAIL;
	}
	__wrn("breakplay=%d\n",breakplay);			/*断点播放*/
	__wrn("filename=%s\n",filename);			/*打印获取到的文件名*/

	#if 0	/*删除视频断点信息*/
		ret = get_item_content_from_fifo_db("MOVIEINF", filename, &tag_inf);/*获取断点信息*/
		__wrn("ret = %d\n",ret);
		if(-1 == ret)
		{
			__wrn("get_item_content_from_fifo_db is fail...\n");
			return 0;
		}

		if(delete_item_from_fifo_db("MOVIEINF", filename))/*删除断点信息*/
		{
			__wrn("get_item_content_from_fifo_db is fail...\n");
			//return -1;
		}
	#endif
	#if 1	/*不使用断点，直接播放文件*/
		//app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_MOVIE);/*app播放声音*/
		//robin_clear_feedback_msgQ();				/*清除消息队列*/
		file_total = robin_npl_get_total_count();	/*获取当前播放列表的所有文件总数*/
		__wrn("file_total = %d\n",file_total);
		
		robin_set_cmd_stop();					/*设置停止命令，该操作将向FSM发送stop命令，并等待FSM处于“停止”状态；用于停止*/
		ret = robin_play_file(filename, NULL);	/*直接播放文件*/
		__wrn("ret = %d\n",ret);
		if(ret == EPDK_FAIL){
			__wrn("robin_play_file is fail...\n");
			return EPDK_FAIL;
		}
		//robin_set_cmd_play_file(filename, NULL);
		robin_set_cmd_play();					/*设置播放命令，该操作将向FSM发送play命令，并等待FSM处于“播放”状态；用于播放*/
		#if 0 /*其他操作*/
		fsm_busy = robin_fsm_is_busy();			/*检查fsm状态机是否繁忙*/
		__wrn("fsm_busy = %d \n",fsm_busy);		/*1为忙，0为不忙*/

		robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));/*获取FSM当前播放的文件名*/
		__wrn("cur_playing_file = %s\n",cur_playing_file);
		
		cur_time = robin_get_cur_time();		/*获取当前文件的播放时间*/
		__wrn("robin_get_cur_time = %d \n",cur_time);
		total_time = robin_get_total_time();	/*获取当前播放文件的总时间*/
		__wrn("robin_get_total_time = %d \n",total_time);
		//robin_clear_feedback_msgQ();			/*FSM创建了一个消息队列，提供即时错误消息。这操作可以清除队列*/
		status = robin_get_fsm_status();		/*获取FSM当前的状态*/
		__wrn("robin_get_fsm_status = %d \n",status);
		 
		//robin_set_cmd_play_file(filename, NULL);	/*设置播放文件命令，不会立刻执行play命令*/
		//ret1 = robin_set_cmd_play();				/*设置播放命令，该操作将向FSM发送play命令，并等待FSM处于“播放”状态；用于播放*/
		//if(ret1 == -1){
		//	__wrn("robin_set_cmd_play is fail...\n");
		//}
		#endif
	#endif
	#if 1	/*获取显示驱动的输出类型*/
	output = dsk_display_get_output_type();
	__wrn("output = %d\n",output);
	#endif
	if(ret == EPDK_OK){							/*配置自动播放定时器*/
		new_movie_install_auto_play_timer(new_movie_ctrl);
		__wrn("new_movie_install_auto_play_timer is success...\n");
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
	robin_npl_set_cur(set_index);		/*设置当前播放的文件索引*/
	index =	robin_npl_get_cur();		/*获取当前播放列表中要播放的当前播放文件的索引*/
	//index	=	robin_npl_get_prev();	/*获取当前播放列表中要播放的上一个播放文件的索引*/
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
	new_movie_ctrl->hdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r");	/*打开显示驱动文件*/
	__wrn("disp is =%s\n",new_movie_ctrl->hdisp);
	if(!new_movie_ctrl->hdisp){
		__msg("open disp fail...\n");
	}
	
	#if 1	/*robin模块打开*/
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
	#endif
	#if 1	/*申请视频图层资源，大小1024*600*/
	new_movie_ctrl->hvedio_lyr = robin_request_video_layer(vedio_rect, 0, 0xff);/*申请robin模块接口视频图层资源*/
	__wrn("vedio_rect:para x=%d, y=%d, width=%d, height=%d\n",vedio_rect->x, vedio_rect->y, vedio_rect->width, vedio_rect->height);
	if(!new_movie_ctrl->hvedio_lyr){
			__wrn("new_movie_ctrl->hvedio_lyr layer is null.....\n");
			return EPDK_FAIL;
	}
	__wrn("robin_request_video_layer layer is success...\n");	
	#endif
	#if 1	/*获取视频流的文件位置指示器的当前值头部开始*/
	{
		__u32 arg[3] = {0};
		arg[0] = new_movie_ctrl->hvedio_lyr;
		eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_START, 0, (void *)arg);/*视频流开启模式*/
	}
	#endif
	#if 1	/*获取视频流的文件位置指示器的当前值底部结束*/
	{
		__s32 ret;
		__u32 arg[3] = {0};
		arg[0] = new_movie_ctrl->hvedio_lyr;
		ret = eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);/*视频流到底部模式*/
		
		if(0 != ret)
		{
			__msg("DISP_CMD_LAYER_BOTTOM fail...\n");
		}
		else
		{
			__msg("DISP_CMD_LAYER_BOTTOM success...\n");
		}
	}
	#endif
	new_movie_ctrl->auto_play_timmer_id = 0x10;/*自动播放定时器id*/
	
	new_movie_ctrl->auto_flush_timmer_id = 0x31;
	eLIBs_memset(new_movie_ctrl->cur_playing_file, 0, sizeof(new_movie_ctrl->cur_playing_file));
	#if 0
	movie_ctrl->err_cnt             = 0;
	movie_ctrl->rr_flag             = 0;
	movie_ctrl->stop_cnt            = 2;
	movie_ctrl->cur_stop_cnt        = 0;
	movie_ctrl->cur_time            = 0;
	movie_ctrl->btv_out             = 0;
	movie_ctrl->digit_timmer_id		= 0x4a;
	
	movie_ctrl->quit_by_fs_part_plug_out    = EPDK_FALSE;
	__msg("__movie_para_init success...\n");
	#endif
	
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
	#if 1
		g_disable_close_scn();/*失能关闭SCN(屏幕，荧光屏)*/
		SEND_MSG(DSK_MSG_RELEASE_LOGO_BUF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
		SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
		__wrn("SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF)\n");
		com_set_backcolor(CONST_BackColor_Black);/*设置背景颜色*/
	#endif
	
	__here__;
	__new_movie_robin_sem_init();/*robin组件初始化*/
	__here__;
	
	eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));	/*结构体初始化，数据清0*/
	GUI_WinSetAddData(msg->h_deswin, (__u32)new_movie_ctrl);	/*添加一个数据*/
	new_movie_ctrl->new_movie_win		= msg->h_deswin;		/*窗口赋值*/
	if(!new_movie_ctrl->new_movie_win){
		__wrn("new_movie_win is fail...\n");
		return EPDK_FAIL;
	}

	
	#if 1	/*图层显示矩形大小1024*600*/
		vedio_rect.x = 0;
		vedio_rect.y = 0;
		vedio_rect.width	=	screen_size.width;
		vedio_rect.height	=	screen_size.height;
		__wrn("vedio_rect: x=%d, y=%d, width=%d, height=%d\n",vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
	#endif
	#if 1	/*视频图层资源申请*/
		new_movie_ctrl_para_init(new_movie_ctrl,&vedio_rect);	/*视频应用控制参数初始化，申请使用robin模块资源*/						/*控制参数初始化*/
		#if 0
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
		#endif
		#if 0
		{
			__s32 ret;
			__u32 arg[3] = {0};
			arg[0] = new_movie_ctrl->hvedio_lyr;
			__wrn("disp 111 =%s\n",new_movie_ctrl->hdisp);
			ret = eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_START, 0, (void *)arg);/*获取视频流的文件位置指示器的当前值*/
			
			if(0 != ret)
			{
				__msg("DISP_CMD_LAYER_BOTTOM fail...\n");
			}
			else
			{
				__msg("DISP_CMD_LAYER_BOTTOM success...\n");
			}
		}
		new_movie_ctrl->auto_play_timmer_id = 0x10;
		#endif
	#endif
	#if 1	/*注册表参数初始化*/
		__app_new_movie_reg_para_init(new_movie_ctrl);				/*reg参数初始化*/
	#endif
	#if 1	/*调色板参数设置*/
		com_set_palette_by_id(ID_MOVIE_PAL_BMP);//设置调色板
		__here__;
		{
			//修改调色板
			//背景色，灰白色，字幕边框颜色
			__u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
			com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));/*通过索引设置调色板颜色*/
		}
		__here__;
	#endif
	#if 1	/*播放视频文件*/
		new_movie_start_play(new_movie_ctrl, 0,4);
		__wrn("new_movie_start_play is success.....\n");
		
		
	#endif
	#if 1	/*配置/开启自动播放定时器*/
		new_movie_install_auto_play_timer(new_movie_ctrl);
		__wrn("new_movie_install_auto_play_timer is 2 success...\n");
	#endif
	//dsk_reg_flush();/*注册表flush*/
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
	case GUI_MSG_CREATE: /*0,创建*/
		gscene_hbar_set_state(HBAR_ST_HIDE);
		gscene_bgd_set_state(BGD_STATUS_HIDE);
		new_movie_scene_create(msg);
		return EPDK_OK;
	case GUI_MSG_DESTROY:{/*1,销毁*/
		new_movie_ctrl_t 	*new_movie_ctrl;
		
		new_movie_ctrl	=	(new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);/*获取manwin窗口属性存放的结构体数据*/
		if(!new_movie_ctrl){
			__wrn("new_movie_ctrl is null...\n");
			return EPDK_OK;
		}
		
		#if 1	/*释放定时器*/
			if(GUI_IsTimerInstalled(new_movie_ctrl->new_movie_win, new_movie_ctrl->auto_play_timmer_id))
			{
				__wrn("GUI_IsTimerInstalled destory begin is ok...\n");
				GUI_KillTimer(new_movie_ctrl->new_movie_win, new_movie_ctrl->auto_play_timmer_id);
			}
		#endif
		robin_set_cmd_stop();/*摧毁时先停止播放*/
		
		#if 1	/*停止视频流获取*/
			{
				__u32 arg[3] = {0};
				arg[0] = new_movie_ctrl->hvedio_lyr;
				eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_STOP, 0, (void *)arg);
			}
		#endif
		#if 1	/*图层释放*/
			if(new_movie_ctrl->hvedio_lyr)//新添加的通用设置图层，清空
			{	
				robin_release_video_layer(new_movie_ctrl->hvedio_lyr);/*释放robin模块申请的视频图层*/
				__here__ ;
				__wrn(" hvedio_lyr isn't null, delete it... \n");
				//GUI_LyrWinDelete(new_movie_ctrl->hvedio_lyr);//删除新添加的app应用图层
				//GUI_LyrWinDelete(h_lyr);
				__wrn(" hvedio_lyr isn't null, delete it OK...\n");
				__here__ ;
				new_movie_ctrl->hvedio_lyr = NULL ;//赋值为空
			}
			__wrn("new_movie_ctrl destory begin is ok...\n");
		#endif 
		
		#if 0	/*删除显示驱动句柄*/
			if(new_movie_ctrl->hdisp)/*删除显示驱动句柄*/
			{
				eLIBs_fclose(new_movie_ctrl->hdisp);
				__wrn("new_movie_ctrl->hdisp destory begin is ok...\n");
				new_movie_ctrl->hdisp = NULL;
			}
		#endif
		#if 1	/*删除赋值的manwin窗口结构体数据*/
		if(new_movie_ctrl->new_movie_win)/*删除赋值的manwin窗口结构体数据*/
		{
			GUI_ManWinDelete(new_movie_ctrl->new_movie_win);
			__wrn("new_movie_ctrl->new_movie_win destory begin is ok...\n");
			new_movie_ctrl->new_movie_win = NULL;
		}
		#endif
		eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));				 /*结构体初始化，数据清0*/
		My_Bfree(new_movie_ctrl, sizeof(new_movie_ctrl_t));					 	 /*释放该manwin窗口属性存放的结构体内存空间*/
		
		
		
		#if 1	/*robin模块关闭*/
			g_display_switch_output(LION_DISP_LCD);	/*movie 退出时恢复到小机显示*/
			g_enable_close_scn();					/*使能关闭屏幕，荧光屏*/
			robin_close();							/*robin模块关闭*/
			__new_movie_robin_sem_deinit();			/*robin组件卸载不初始化*/
		#endif 
		#if 0	/*注册表*/
		{
				reg_root_para_t *root_reg_para;
				root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

				if(root_reg_para)
				{
					root_reg_para->cur_play_storage = -1;
				}
				else
				{
					__wrn("para is null...\n");
				}
		}
		#endif
		com_set_backcolor(CONST_BackColor);/*设置背景颜色为蓝色*/
		SEND_MSG(DSK_MSG_SCREENOFF_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);/*发送MSG命令荧光屏/屏幕关闭，背景关闭*/
		__wrn("new_movie_ctrl is free success.....\n");
	}
		return EPDK_OK;
	case GUI_MSG_PAINT:	 /*2,绘制*/
		
		return EPDK_OK;
	case GUI_MSG_CLOSE:	 /*3,关闭*/
		//app_new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_MOVIE_SW_TO_NEW_EXPLORER, 0);	/*发送命令，跳转到new explorer资源管理器应用程序的manwin窗口*/
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
	#if 1
	case GUI_MSG_TIMER:
	{
				new_movie_ctrl_t *new_movie_ctrl;
				__s32 ret;
				ret = -1;
				new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	
				if(new_movie_ctrl)
				{
					__wrn("LOWORD(msg->dwAddData1) = %d\n",LOWORD(msg->dwAddData1));
					if(LOWORD(msg->dwAddData1) == new_movie_ctrl->auto_play_timmer_id)/*0x10*/
					{

						{
							__s32 play_sta;
							__wrn("before __new_movie_check_for_play_err\n");
							play_sta = __new_movie_check_for_play_err(new_movie_ctrl);
							__wrn("after __new_movie_check_for_play_err\n");
	
							if(EPDK_OK == play_sta)
							{
								__wrn("before __new_movie_check_for_auto_play\n");
								__new_movie_check_for_auto_play(new_movie_ctrl);
								__wrn("aftr __new_movie_check_for_auto_play\n");
							}
							else//如果播放错误，则重新开始检测
							{
								//__new_movie_reset_auto_play_timmer(new_movie_ctrl);
								//new_movie_ctrl->cur_stop_cnt = 0;
							}
							
							ret = EPDK_OK;
						}
					}
					GUI_ResetTimer(new_movie_ctrl->new_movie_win, new_movie_ctrl->auto_play_timmer_id
									   , MOVIE_AUTO_FLUSH_TIME, NULL);
					//else if(LOWORD(msg->dwAddData1) == new_movie_ctrl->auto_flush_timmer_id)
					/*{
						__wrn("*********************new_movie flush*********************\n");
#if 1
						//dsk_reg_flush();
						//movie_save_breakpoint_info(new_movie_ctrl);
#endif
						GUI_ResetTimer(new_movie_ctrl->new_movie_win, new_movie_ctrl->auto_flush_timmer_id
									   , MOVIE_AUTO_FLUSH_TIME, NULL);
						ret = EPDK_OK;
					}*/
					//==========================================
					//==========================================
				}
				else
				{
					__wrn("movie_ctrl is null...\n");
					return EPDK_OK;
				}
	
				/*if(-1 == ret) //未处理，交给子场景处理
				{
					break;
				}
				else//已处理*/
				{
					return EPDK_OK;
				}
	}
	#endif
	
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
	H_WIN	h_manwin;
	//new_movie_ctrl_t 	*new_movie_ctrl;

	#if 0	//检测manwin窗口是否内存泄漏-->查看打印的Pages free的大小反复切换manwin界面是否会恢复
	__wrn("esKSRV_SysInfo  manwin is...\n");				//打印内存是否泄露
	esKSRV_SysInfo();								//查看系统内存消息是否泄露或溢出
	//esMEMS_FreeMemSize();							//剩余内存大小以KB为单位
	//esMEMS_TotalMemSize();						//总的内存大小
	__wrn("esKSRV_SysInfo | esMEMS_FreeMemSize = %d \n",esMEMS_FreeMemSize());//打印剩余内存
	__wrn("esKSRV_SysInfo | esMEMS_TotalMemSize = %d \n",esMEMS_TotalMemSize());//打印总内存
	#endif
	/*
	new_movie_ctrl = (new_movie_ctrl_t *)My_Balloc(sizeof(new_movie_ctrl_t));
	if(!new_movie_ctrl){
		__wrn("new_movie_ctrl memory balloc fail.........");
		return EPDK_FAIL;
	}
	eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));
	
	new_movie_ctrl->root_para	=	para;
	new_movie_ctrl->root_type	=	para->root_type;
	*/
	
	/*************开始操作manwin窗口配置**********************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name	 		=	"app_new_movie";
	create_info.hParent	 		=	para->h_parent;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_movie_manager_win_proc);
	create_info.id				=	APP_NEWMOVIE_ID;//0x9800地址
	create_info.attr			=	(void *)para;
	create_info.hHosting 		=	NULL;
	h_manwin = GUI_ManWinCreate(&create_info);
	__wrn("new movie h_manwin=%d...\n",h_manwin);
	#if 0
	if(h_manwin){
		reg_root_para_t *root_reg_para;
		__s32 reg_storage_type;
		reg_storage_type = 0;
		
		if(para){
			reg_storage_type = 0;/*默认TF卡模式*/
			__wrn("new movie para->root_para->root_type invalid...\n");
		}
		root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);/*获取注册表app参数*/
		if(root_reg_para){
			__s32 reg_app_type;
			__wrn("new movie root_reg_para...\n");
			reg_app_type = m_eMediaTypeList_video;
			root_reg_para->cur_play_storage = reg_storage_type;	/*当前播放储存*/
			root_reg_para->last_play_app = reg_app_type;		/*上一次播放的类型*/
			root_reg_para->last_app_play_storage[reg_app_type] = reg_storage_type;
			root_reg_para->last_storage_play_app[reg_storage_type] = reg_app_type;
		}
		else{
			__wrn("new movie para is null...\n");
		}
	}
	#endif
	return(GUI_ManWinCreate(&create_info));/*返回一个创建好的manwin窗口句柄*/
}





