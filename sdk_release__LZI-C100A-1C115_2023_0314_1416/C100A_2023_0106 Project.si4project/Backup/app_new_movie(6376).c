/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_movie.c
* By      	: Bayden.chen
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2011-05-05   Bayden.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "app_new_movie_i.h"
#include "app_new_movie.h"
#include "progressbar/progressbar.h"//进度条显示头文件
#include "spsc_sub_ctrl_menu/new_movie_spsc_ctrl_menu.h"

#define TIMEROUT_ID_TIMER_HLYR_FF_FB		400
#if TIMEROUT_ID_TIMER_HLYR_FF_FB
#define ID_TIMER_HLYR_FF_FB			0x5555
#endif



static new_movie_ctrl_t *g_new_movie_ctrl = NULL;
static __s32 is_touch_ff_rr_valid = 0;//代表判断当前是否可以进行触摸快进快退
static __krnl_event_t *g_new_movie_robin_sem = NULL;/*robin组件初始化标志*/

static __s32 g_new_is_on_tv = 0;/*息屏和；亮屏标志*/

//static __s32 new_movie_playend_scene_create(new_movie_ctrl_t *new_movie_ctrl);


static __s32 new_movie_playerror_scene_create(new_movie_ctrl_t *new_movie_ctrl);


#define GUI_MSG_SET_TVOUT_TP_HOOK_MSG (GUI_MSG_USER_DEF+1)

static __s32 new_movie_save_breakpoint_info(new_movie_ctrl_t *new_movie_ctrl);





/*获取显示的输出类型：有NONE,LCD,TV,HDMI,VGL五种输出模式*/
static __bool new_is_on_tv(void)
{
	__disp_output_type_t output;
	
	output = dsk_display_get_output_type();
	if(DISP_OUTPUT_TYPE_LCD == output || DISP_OUTPUT_TYPE_NONE == output)
	{
		__wrn("not on tv now...\n");
		return EPDK_FALSE;
	}
	else
	{
		__wrn("on tv now...\n");
		return EPDK_TRUE;
	}
}
/*视频控制参数初始化*/
static __s32 new_movie_ctrl_para_init(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("new_movie_ctrl is null...\n");
		return EPDK_FAIL;
	}

	new_movie_ctrl->hdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r");	/*打开显示驱动文件*/

	if(!new_movie_ctrl->hdisp)
	{
		__msg("open disp fail...\n");
	}

	__wrn("open disp success...\n");
	#if 1	/*robin模块打开*/
	{
		__s32 ret;
		__s32 screen_width, screen_height;
		RECT vedio_rect;
		robin_open_arg_t arg;
		arg.cedar_stop_mode = CEDAR_STOP_MODE_UNINSTALL_PLUGINS;	/*停止播放时,卸载所有插件*/
		arg.switch_vplay_mode = CEDAR_FILE_SWITCH_VPLY_MODE_GAP;	/*2个文件切换*/
		arg.reserve_mem_size = 1024 * 1024;	//512*1024;				/*预留1M字节*/
		ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);				/*打开robin模块接口*/

		if(EPDK_OK != ret)
		{
			__msg("robin_open fail...\n");
			return EPDK_FAIL;
		}

		__wrn("robin_open success...\n");
		//robin_clear_feedback_msgQ();
		dsk_display_get_size(&screen_width, &screen_height);/*图层尺寸大小1024*600*/
		vedio_rect.x = 0;
		vedio_rect.y = 0;
		vedio_rect.width = screen_width;
		vedio_rect.height = screen_height;
		__wrn("vedio screen=%d %d %d %d", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
		new_movie_ctrl->hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);/*申请视频图层资源*/

		if(!new_movie_ctrl->hvedio_lyr)
		{
			__msg("robin_request_vedio_layer fail...\n");
			return EPDK_FAIL;
		}

		__wrn("robin_request_vedio_layer success...\n");
	}
	#endif
	
	#if 1	/*获取视频流的文件位置指示器的当前值头部开始*/
	if(new_is_on_tv())
	{
		__u32 arg[3] = {0};
		arg[0] = new_movie_ctrl->hvedio_lyr;
		eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_START, 0, (void *)arg);
	}
	#endif

	#if 1	/*获取视频流的文件位置指示器的当前值底部结束*/
	{
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
	}
	#endif

	new_movie_ctrl->auto_play_timmer_id = 0x01;

	eLIBs_memset(new_movie_ctrl->cur_playing_file, 0, sizeof(new_movie_ctrl->cur_playing_file));

	__msg("new_movie_para_init success...\n");
	return EPDK_OK;
}
/*视频控制参数删除*/
static __s32 new_movie_ctrl_para_deinit(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(new_is_on_tv())
	{
		__u32 arg[3] = {0};
		arg[0] = new_movie_ctrl->hvedio_lyr;
		eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_STOP, 0, (void *)arg);//视频流文件数据停止获取
	}
	//释放图层
	if(new_movie_ctrl->hvedio_lyr)
	{
		robin_release_video_layer(new_movie_ctrl->hvedio_lyr);
		new_movie_ctrl->hvedio_lyr = NULL;
	}
	//删除显示驱动
	if(new_movie_ctrl->hdisp)
	{
		eLIBs_fclose(new_movie_ctrl->hdisp);
		new_movie_ctrl->hdisp = NULL;
	}

	return EPDK_OK;
}
/*配置自动播放定时器*/
static __s32 new_movie_install_auto_play_timmer(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	__wrn("new_movie_ctrl->auto_play_timmer_id = 0x%x\n",new_movie_ctrl->auto_play_timmer_id);
	if(!GUI_IsTimerInstalled(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id))/*定时器加载*/
	{
		GUI_SetTimer(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id
		             , 200, NULL);//0.5秒，设置定时器
		__wrn("GUI_SetTimer timmer is 1...\n");
	}

	return EPDK_OK;
}
/*复位自动播放定时器*/
static __s32 new_movie_reset_auto_play_timmer(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("new_movie_reset_auto_play_timmer is 1...\n");
	if(GUI_IsTimerInstalled(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id))
	{
		GUI_ResetTimer(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id
		               , 200, NULL);//0.5秒，复位定时器
	}
	else
	{
		__msg("timmer already install...\n");
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*卸载自动播放定时器*/
static __s32 new_movie_uninstall_auto_play_timmer(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("new_movie_uninstall_auto_play_timmer is 1...\n");
	if(GUI_IsTimerInstalled(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id))
	{
		GUI_KillTimer(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id);//删除定时器
	}


	return EPDK_OK;
}

/*视频文件播放*/
static __s32 new_movie_play_file(new_movie_ctrl_t *new_movie_ctrl, __s32 index, __bool breakplay)
{
	__s32 ret,amplifier_flag;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__cedar_tag_inf_t tag_inf;
	ret = robin_npl_index2file(index, filename);//通过index索引获取当前播放列表要播放的文件名

	if(-1 == ret)
	{
		__wrn("robin_npl_index2file fail...\n");
		return EPDK_FAIL;
	}

	//new_movie_ff_fb_over(new_movie_ctrl);
	__wrn("breakplay=%d\n", breakplay);
	__wrn("filename=%s\n", filename);

	{
		robin_set_cmd_stop();			/*设置停止播放命令*/
		//robin_clear_feedback_msgQ();//清空错误信息消息队列
		ret = robin_play_file(filename, NULL);	/*播放当前播放列表的文件*/
		__wrn("ret=%d\n", ret);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}

		robin_set_cmd_play();			//设置开始播放命令
		__wrn("ret=%d\n", ret);

		amplifier_flag = dsk_amplifier_is_on();//获取功放状态
		__wrn("amplifier_flag=%d\n", amplifier_flag);
	}

	if(EPDK_OK == ret)
	{
		__wrn("new_movie_play_file ret=%d\n", ret);
		new_movie_install_auto_play_timmer(new_movie_ctrl);//开启自动播放定时器
	}

	return ret;
}


/*视频文件开始播放*/
static __s32 new_movie_start_play(new_movie_ctrl_t *new_movie_ctrl, __bool breakplay)
{
	__s32 ret;
	__s32 index;
	index = robin_npl_get_cur();//获取当前播放列表的要播放文件
	__wrn("new_movie_start_play  is index = %d...\n",index);

	if(-1 == index)
	{
		__wrn("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}
	__wrn(" new_movie_start_play is...\n");
	ret = new_movie_play_file(new_movie_ctrl, index, breakplay);/*播放视频文件*/
	return ret;
}

/*通过文件索引id播放文件或者默认从第一个文件开始播放*/
static __s32 new_movie_play_for_index_or_default(__gui_msg_t *msg)
{
	root_para_t *para;
	static __s32 index = 0;
	para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);//获取从root中创建manwin窗口时输入的参数数据
					
	#if 1//通过 索引或者直接 播放视频文件
		index = robin_npl_get_cur();					//获取当前播放列表要播放的当前播放文件索引
		__wrn("save breakpoint the index = %d\n",index);
	
		if(para->data != -1)							//不等于-1的时候是从new_explorer列表中跳转到new_movie播放的
		{
			__wrn("para->data = %d\n",para->data);		//播放的文件索引号
			index = para->data;							//获取需要播放的文件索引
			__wrn(" index = %d\n",index);
			robin_npl_set_cur(index);					//设置当前播放文件
			para->data = -1;							//清除跳转app标志位
			
		}
		else											//等于-1的时候是直接从home界面进入new_movie播放的
		{
			if(index != 0)								/*默认从头开始播放*/
			{
				index = 0;
				__wrn(" index 2= %d\n",index);
				/*重新设置从头开始播放文件*/
				robin_npl_set_cur(index);				/*设置当前播放列表要播放的当前播放文件索引id*/
			}

		}
	#endif
}
/*停止播放*/
static __s32 new_movie_stop_play(new_movie_ctrl_t *new_movie_ctrl)
{
	__wrn(" new_movie_stop_play is...\n");
	robin_set_cmd_stop();/*设置停止播放命令*/
	return EPDK_OK;
}
/*暂停播放*/
static __s32 new_movie_pause_play(new_movie_ctrl_t *new_movie_ctrl)
{
	__wrn(" new_movie_pause_play is...\n");

	robin_set_cmd_pause();/*设置暂停播放命令*/
	return EPDK_OK;
}
//flag:0表示退出到explorer，1表示退出到home
static __s32 new_movie_exit_movie(new_movie_ctrl_t *movie_ctrl, __s32 flag)
{
	__wrn("before new_movie_exit_movie\n");

	if(!movie_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	if(movie_ctrl->exit_movie)
	{
		__wrn("movie already exit...\n");
		return EPDK_FAIL;
	}

	movie_ctrl->exit_movie = 1;

	if(!movie_ctrl->new_movie_manager)
	{
		__wrn("movie_ctrl->movie_manager is null...\n");
		return EPDK_FAIL;
	}

	new_movie_uninstall_auto_play_timmer(movie_ctrl);
	
	//__movie_pause_play(movie_ctrl);视频库修改了，要求获取断点信息的时候只能是play状态，所以不能暂停
	{
		__gui_msg_t msg;
		eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
		msg.id = GUI_MSG_CLOSE;//发送关闭命令
		msg.dwAddData1 = flag;
		msg.h_deswin = movie_ctrl->new_movie_manager;
		GUI_SendMessage(&msg);
	}
	__wrn("after __movie_exit_movie\n");
	return EPDK_OK;
}


/*检查是否播放错误*/
static __s32 new_movie_check_for_play_err(new_movie_ctrl_t *new_movie_ctrl)
{
	__s32 ret;
	ret = robin_get_feedback_msg();//获取信息消息队列

	if(ret < 0)
	{
		robin_clear_feedback_msgQ();//清空错误信息消息队列
		esKRNL_TimeDly(100);
		__wrn("new_movie_check_for_play_err exit movie...\n");
		new_movie_exit_movie(new_movie_ctrl, 1);//退出应用,回到home界面
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*检查自动播放*/
static __s32 new_movie_check_for_auto_play(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__cedar_status_t fsm_sta;
		fsm_sta = robin_get_fsm_status();/*获取fsm状态机的状态*/
		__wrn("fsm_sta= %d\n", fsm_sta);

		if(CEDAR_STAT_PLAY == fsm_sta
		    || CEDAR_STAT_PAUSE == fsm_sta)
		{
			new_movie_ctrl->cur_time = robin_get_cur_time();/*获取当前播放的时间*/
			robin_get_fsm_cur_file(new_movie_ctrl->cur_playing_file, sizeof(new_movie_ctrl->cur_playing_file));/*获取当前播放的文件名*/
			__wrn("new_movie_ctrl->cur_time = %d\n", new_movie_ctrl->cur_time);
			__wrn("new_movie_ctrl->cur_playing_file = %d\n", new_movie_ctrl->cur_playing_file);
			__wrn("fsm_sta 22= %d\n", fsm_sta);
			
		}
		else
		{
			new_movie_ctrl->cur_time = -1;
			eLIBs_strcpy(new_movie_ctrl->cur_playing_file, "");
		}

		switch(fsm_sta)
		{
			case CEDAR_STAT_PLAY:{
				dsk_amplifier_onoff(1);//打开功放
				break;
			}
			case CEDAR_STAT_PAUSE:
			case CEDAR_STAT_STOP:{
				dsk_amplifier_onoff(0);//关闭功放
				break;
			}
			default:
			{
				new_movie_ctrl->rr_flag = 0;
				__msg("fsm_sta=%d, new_movie_ctrl->rr_flag=%d\n", fsm_sta, new_movie_ctrl->rr_flag);
				break;
			}
		}
	}

	return EPDK_OK;
}

/*切换输出类型,显示输出的类型*/
static __s32 new_movie_switch_output(new_movie_ctrl_t *new_movie_ctrl)
{
	//切换输出
	{
		reg_system_para_t *para;//系统参数
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			__wrn("para->pal_ntsc = %d\n", para->pal_ntsc);

			if(0 == para->pal_ntsc)
			{
				if(para->output == LION_DISP_LCD)
				{
					new_movie_ctrl->btv_out = 1;
					__here__;
					g_new_is_on_tv = 1;
					g_display_switch_output(LION_DISP_TV_NTSC);
				}
				else
				{
					new_movie_ctrl->btv_out = 0;
					__here__;
					g_new_is_on_tv = 0;
					g_display_switch_output(LION_DISP_LCD);
				}
			}
			else
			{
				if(para->output == LION_DISP_LCD)
				{
					new_movie_ctrl->btv_out = 1;
					__here__;
					g_new_is_on_tv = 1;
					g_display_switch_output(LION_DISP_TV_PAL);
				}
				else
				{
					new_movie_ctrl->btv_out = 0;
					__here__;
					g_new_is_on_tv = 0;
					g_display_switch_output(LION_DISP_LCD);
				}
			}
		}
	}
	__wrn("new_movie_ctrl->btv_out=%d\n", new_movie_ctrl->btv_out);
	return EPDK_OK;
}
/*切换到电视机或者LCD显示屏*/
static __s32 new_movie_switch_to_tv_or_lcd(new_movie_ctrl_t *new_movie_ctrl)
{
	__wrn("new_movie_switch_to_tv_or_lcd is...\n");
	new_movie_switch_output(new_movie_ctrl);//切换显示输出类型
	
	return EPDK_OK;
}
/*开始播放进程的定时器，用于字幕显示，方便字幕显示同步播放的内容*/
static __s32 new_movie_process_after_start_play(new_movie_ctrl_t *new_movie_ctrl)
{

	__wrn("new_movie_process_after_start_play is...\n");
	new_movie_install_auto_play_timmer(new_movie_ctrl);
	//new_movie_spsc_scene_create(new_movie_ctrl);//刚进入视频时，不显示控制菜单
	return EPDK_OK;
}

/*设置播放模式*/
static void __app_new_movie_set_play_mode(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
	{
		__wrn("load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__wrn("load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		robin_set_play_mode(para->rotate_mode);
		__wrn("para->rotate_mode=%d\n", para->rotate_mode);
	}
	else
	{
		robin_set_play_mode(RAT_PLAY_MODE_ROTATE_ALL);
	}
}
/*设置缩放模式*/
static void __app_new_movie_set_zoom_mode(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
	{
		__wrn("load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__wrn("load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		robin_set_zoom(para->zoom_mode);
		__wrn("para->zoom_mode=%d\n", para->zoom_mode);
	}
}
/*注册表参数初始化*/
static __s32 __app_new_movie_reg_para_init(new_movie_ctrl_t *new_movie_ctrl)
{
	__app_new_movie_set_play_mode();
	__app_new_movie_set_zoom_mode();
	__wrn("__app_movie_reg_para_init is...\n");

	dsk_reg_set_app_restore_flag(REG_APP_MOVIE, 0);
	return EPDK_OK;
}


//声道模式
static __s32 __app_new_movie_draw_channel_mode(__s32 channel, H_LYR hlyr, GUI_RECT *gui_rect)
{
	char text[128];
	__wrn("__app_new_movie_draw_channel_mode...\n");

	if(NULL == hlyr || NULL == gui_rect)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(0 == channel)
	{
		dsk_langres_get_menu_text(STRING_MUSIC_STEREO, text, sizeof(text));
	}
	else if(1 == channel)
	{
		dsk_langres_get_menu_text(STRING_MUSIC_LEFT, text, sizeof(text));
	}
	else
	{
		dsk_langres_get_menu_text(STRING_MUSIC_RIGHT, text, sizeof(text));
	}

	GUI_LyrWinSel(hlyr);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(gui_rect);
	GUI_SetFont(SWFFont);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetColor(GUI_WHITE);
	GUI_UC_SetEncodeUTF8();
	GUI_DispStringInRect(text, gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
	return EPDK_OK;
}

//=====================================30234384

//窗口的命令发送函数
__s32  new_movie_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
{
	H_WIN hparent;
	__gui_msg_t msg;
	hparent = GUI_WinGetParent(hwin);

	if(!hparent)
	{
		__err("hparent is null...\n");
		return EPDK_FAIL;
	}
	__wrn("**********app_new_movie_cmd2parent**********\n");
	msg.h_deswin = hparent;//父窗口
	msg.h_srcwin = hwin;//源窗口
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data2;
	msg.dwReserved = reserved;//存放USB或TF卡切换
	GUI_SendNotifyMessage(&msg);
	return EPDK_OK;
}
/***********************************************************************************************************
*@Function	:          __s32 new_movie_robin_sem_init(void)
*@Description	: 		robin组件初始化
*						
*@Return     	: 	    EPDK_OK或EPDK_FAIL
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
__s32 new_movie_robin_sem_init(void)
{
	if(0 == g_new_movie_robin_sem){
		g_new_movie_robin_sem = esKRNL_SemCreate(1);//robin组件创建
	}

	if(g_new_movie_robin_sem){
		return EPDK_OK;
	}
	else{
		return EPDK_FAIL;
	}
}

/***********************************************************************************************************
*@Function	:          __s32 new_movie_robin_sem_deinit(void)
*@Description	: 		robin组件卸载不初始化
*						
*@Return     	: 	    EPDK_OK
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
__s32 new_movie_robin_sem_deinit(void)
{
	if(g_new_movie_robin_sem){
		__u8 err;
		esKRNL_SemDel(g_new_movie_robin_sem, OS_DEL_ALWAYS, &err);//robin组件删除
		g_new_movie_robin_sem = NULL;
	}

	return EPDK_OK;
}

/*快进图层绘制*/
__s32 new_movie_ff_fb_layer_draw(__gui_msg_t *msg)
{
	char     text[256];
	new_movie_ctrl_t *new_movie_ctrl;
	H_LYR	*phLyr;
	char 	t_str[] = " : X1";
	__wrn("================new_movie_ff_fb_layer_draw================== \n");
	new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	phLyr = &new_movie_ctrl->hvedio_lyr;

	if(!new_movie_ctrl)
	{
		__err("new_movie_ctrl is null...\n");
		return EPDK_OK;
	}

	if(NULL == *phLyr)
	{
		__wrn("digit layer is null...\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(*phLyr))
	{
		__wrn("digit layer is not on...\n");
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(*phLyr);
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_Clear();
	GUI_SetFont(SWFFont);
	GUI_SetColor(GUI_RED);
	GUI_CharSetToEncode(dsk_get_langres_charset());
	GUI_SetBkColor(0x0);
	{
		GUI_RECT gui_rect;
		gui_rect.x0 = 0;
		gui_rect.y0 = 0;
		gui_rect.x1 = W_NumLock_Bg - 1;
		gui_rect.y1 = H_NumLock_Num - 1;
		//GUI_BMP_Draw();
	}
	GUI_LyrWinSetTop(*phLyr);
	return EPDK_OK;
}

/***********************************************************************************************************
*@Function	:          __s32 progressbar_scene_create(__gui_msg_t *msg)
*@Description	: 		创建进度条显示framewin窗口
*						
*@Return     	: 	    EPDK_OK
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
static __s32 progressbar_scene_create(__gui_msg_t *msg)
{
	#if 0
	new_movie_ctrl_t	*new_movie_ctrl;
	movie_progressbar_create_para_t para;

	new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	{//创建绘制新添加的通用设置app应用程序framewin窗口
		__wrn("root_type = 0 \n");
		__wrn("progressbar_scene_create is... \n");
	}
	__wrn("progressbar_scene_create  is ok\n");

	
	eLIBs_memset(&para, 0, sizeof(movie_progressbar_create_para_t));
	para.hparent = new_movie_ctrl->new_movie_manager;
	para.scene_id = MOVIE_VOLUME_ID;

	new_movie_ctrl->progressbar_scene = Play_Progress_Bar_Scene_Frmwin_create(msg->h_deswin, &para);//进入新添加的app应用程序创建framwin子窗口
	//GUI_WinSetFocusChild(app_new_setting_ctrl->h_frm_general_setting);//选择指定窗口进行消息命令发送，按键执行需要调用
	#endif
	#if 0//新的进度条framewin窗口
		new_movie_ctrl_t	*new_movie_ctrl;
		movie_progressbar_create_para_t progressbar_createpara;
		RECT rect;
		SIZE screen_size;
		new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
		{//创建绘制新添加的通用设置app应用程序framewin窗口
			rect.x = 0;
			rect.y = 0;
			dsk_display_get_size(&screen_size.width, &screen_size.height);//
			rect.width = screen_size.width;		//
			rect.height = screen_size.height;	//
			__wrn("rect.x = %d , rect.y = %d,rect.width = %d , rect.heigh = %d\n",rect.x,rect.y,rect.width,rect.height);
			progressbar_createpara.hlyr = __progressbar_8bpp_layer_create(&rect,1);
			if(!progressbar_createpara.hlyr){
				__wrn("progressbar_createpara.layer is null \n");
				return EPDK_FAIL;
			}

			progressbar_createpara.time_font = new_movie_ctrl->time_font;
			progressbar_createpara.hparent = new_movie_ctrl->new_movie_manager;
			__wrn("progressbar_createpara.hparent is =%x \n",progressbar_createpara.hparent);
			progressbar_createpara.scene_id = MOVIE_VOLUME_ID;
			new_movie_ctrl->progressbar_scene = Play_Progress_Bar_Scene_Frmwin_create(msg->h_deswin, &progressbar_createpara);
			
		}
		
	#endif
	#if 1//直接绘制
		new_movie_ff_fb_layer_draw(msg);
	#endif
	
	return EPDK_OK;

}

//创建控制菜单子场景framewin窗口
static __s32 __new_movie_spsc_ctrl_menu_scene_create(new_movie_ctrl_t *scene_para)
{
	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		new_movie_spsc_ctrl_menu_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(new_movie_spsc_ctrl_menu_create_para_t));
		create_para.hparent  = scene_para->new_movie_manager;//new_spsc_ctrl_menu_manager;
		create_para.scene_id = MOVIE_SPSC_ID;//spsc_ctrl_menu_scene_id;
		//create_para.hlyr	  = scene_para->hlyr;
		scene_para->spsc_ctrl_scene = new_movie_spsc_ctrl_menu_scene_create(&create_para);//
		__wrn("__new_movie_spsc_ctrl_menu_scene_create is...\n");
		if(!scene_para->spsc_ctrl_scene)
		{
			__wrn("movie_spsc_ctrl_scene_create fail...\n");
			return EPDK_FAIL;
		}

		__wrn("new_movie_spsc_ctrl_menu_scene_set_focus success...\n");
		new_movie_spsc_ctrl_menu_scene_set_focus(scene_para->spsc_ctrl_scene);//按键信息发送
		__wrn("new_movie_spsc_ctrl_menu_scene_set_focus\n");
	}

	return EPDK_OK;
}

//进度条framewin窗口创建
static __s32 __movie_progressbar_create(new_movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("__movie_progressbar_create is start...\n");
	{
		progressbar_create_para_t create_para;
		//progressbar_scene_t *scene_para;
		eLIBs_memset(&create_para, 0, sizeof(progressbar_create_para_t));
		create_para.hparent = movie_ctrl->new_movie_manager;
		create_para.scene_id = MOVIE_PROG_ID;
		//movie_ctrl->progressbar_scene = movie_prog_scene_create(&create_para);//进度条framewin窗口创建可以用
		movie_ctrl->progressbar_scene = progressbar_frmwin_create(&create_para);//进度条framewin窗口创建
		
		if(NULL == movie_ctrl->progressbar_scene)
		{
			__wrn("__movie_progressbar_create fail...\n");
			return EPDK_FAIL;
		}
		progressbar_ctrl_scene_set_focus(movie_ctrl->progressbar_scene);//发送按键信息到进度条界面窗口图层
	}

	return EPDK_OK;
}

//按键功能处理函数
static __s32 app_new_movie_key_proc(__gui_msg_t *msg)
{
	__s32 bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
		__s32 ret;
		static __s32 last_key = -1;
		new_movie_ctrl_t *new_movie_ctrl;
		__wrn("__app_new_movie_proc: GUI_MSG_KEY begin\n");
		new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

		if(!new_movie_ctrl)
		{
			__err("new_movie_ctrl is null...\n");
			return EPDK_OK;
		}

			bok = -1;
		#if 0
			if(KEY_UP_ACTION == msg->dwAddData2)//第一次进入的按键执行功能
			{
				__wrn("last_key=%d\n", last_key);

				if(0)
				{}
				//if( GUI_MSG_KEY_LONGVIDEO == last_key )
				else if(GUI_MSG_KEY_LONGENTER == last_key)//长按回车按键，息屏
				{
					//dsk_display_lcd_off();
					//__wrn("Tarring-----send Force close---------\n");
					//SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//界面熄灭		
					bok = EPDK_OK;
				}
				else	if(GUI_MSG_KEY_TVOUT == last_key)
				{
					__wrn("before new_movie_switch_to_tv_or_lcd\n");
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);
					__msg("after new_movie_switch_to_tv_or_lcd\n");
				}
				else if(GUI_MSG_KEY_PLAY_PAUSE == last_key
				        || GUI_MSG_KEY_LONGPLAY_PAUSE == last_key)//暂停按键按下
				{
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_PLAY_PAUSE is press...\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						//new_movie_delete_subscene_by_id(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						robin_set_cmd_pause();
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
					}
					else
					{
						//new_movie_delete_subscene_by_id(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
						robin_set_cmd_play();
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_play);
					}

					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_ENTER == last_key)//回车按键按下
				{
					__wrn("GUI_MSG_KEY_ENTER is press...\n");
					/*if(new_movie_ctrl->m_ff_fb_speed)//进入快进功能
					{
						__wrn("GUI_MSG_KEY_ENTER is press 1...\n");
						switch(robin_get_fsm_status())
						{
							case CEDAR_STAT_FF :
							case CEDAR_STAT_RR :
								robin_set_cmd_play();

							default :
								break;
						}

						//new_movie_ff_fb_over(new_movie_ctrl);
						bok = EPDK_OK;
					}
					else*/
					{
						//如果没有弹出菜单，则按enter暂停或开始播放
						__s32 ret;
						ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						__wrn("GUI_MSG_KEY_ENTER is press 2...\n");

						if(EPDK_FALSE == ret)//如果没有除字幕以外的子场景，则处理暂停、继续播放
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();	//获取fsm状态机的状态

							if(CEDAR_STAT_PLAY == fsm_sta)		//播放状态
							{
								__wrn("before robin_set_cmd_pause\n");
								robin_set_cmd_pause();			//设置暂停播放
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//播放声音暂停
								__wrn("after robin_set_cmd_pause\n");
								//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
								bok = EPDK_OK;
							}
							else if(CEDAR_STAT_PAUSE == fsm_sta)//暂停状态
							{
								__wrn("before robin_set_cmd_play\n");
								robin_set_cmd_play();//设置开始播放
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);//播放声音打开
								__wrn("after robin_set_cmd_play\n");
								bok = EPDK_OK;
							}
						}
					}
				}
				else if(GUI_MSG_KEY_ESCAPE == last_key
				        || GUI_MSG_KEY_MENU == last_key//返回退出按键
				       )
				{
					__s32 ret;
					ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL
					           //                 & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID) & (~MOVIE_PROG_ID));

					if(EPDK_TRUE == ret)//如果有除字幕和playpause以外的子场景，则不退出movie，删除所有子场景
					{
						
					}
					else//退出movie
					{
						if(new_is_on_tv())
						{
							new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//唤醒界面
						}
						else
						{
							//new_movie_exit_movie(new_movie_ctrl, 0);
						}
					}

					bok = EPDK_OK;
				}
			}
			else//进入界面后的按键功能执行
			{	
				__cedar_status_t sta;
				sta = robin_get_fsm_status();//获取fsm状态机的状态
				__wrn("GUI_MSG_KEY sta=%d\n",sta);
				__wrn("msg->dwAddData1 is = %d ...\n",msg->dwAddData1);
				switch(msg->dwAddData1)
				{
					case GUI_MSG_KEY_REPEATE:
					case GUI_MSG_KEY_LONGMENU://长按退出
					case GUI_MSG_KEY_SUBMENU:
					{
						__s32 ret;
						__here__;
						if(CEDAR_STAT_PLAY == sta)
						{
							__wrn("CEDAR_STAT_PLAY sta=%d\n",sta);
							robin_set_cmd_pause();
							app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//声音暂停
							//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
						}
						
						bok = EPDK_OK;
						break;
					}
					case GUI_MSG_KEY_RR://后退
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_RR\n");

						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_rr();//发送后退命令
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_FF://快进
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_FF\n");
						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_ff();//发送快进命令
						}

						bok = EPDK_OK;
						break;
					}
					
					case GUI_MSG_KEY_MENU://返回退出按键
					{
						__wrn("GUI_MSG_KEY_MENU is press to new_movie_exit_movie...\n");
						new_movie_exit_movie(new_movie_ctrl, 1);//退出到home应用
						bok = EPDK_OK;
						break;
					}
					
					case GUI_MSG_KEY_RIGHT://下按键，快进
					case GUI_MSG_KEY_LONGRIGHT:
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_RIGHT send ff...\n");
						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_ff();//发送快进命令
						}

						bok = EPDK_OK;
						break;
					}
					
					case GUI_MSG_KEY_LEFT://上按键，后退
					case GUI_MSG_KEY_LONGLEFT:
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_LEFT send rr...\n");

						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_rr();//发送后退命令
						}

						bok = EPDK_OK;
						break;
					}

					default:
					{
						break;
					}
				}
			}
		#endif

	#if 1
		if(KEY_UP_ACTION == msg->dwAddData2){//按键松开动作获取，第一次进入的按键执行功能
			__wrn("last_key=%d\n", last_key);
			switch(last_key){
				case GUI_MSG_KEY_LONGENTER:{/*长按回车按键，息屏*/
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//界面熄灭		
					bok = EPDK_OK;
				}
				break;

				case GUI_MSG_KEY_TVOUT:{/*显示类型输出切换*/
					__wrn("before new_movie_switch_to_tv_or_lcd\n");
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);
					__msg("after new_movie_switch_to_tv_or_lcd\n");
				}
				break;

				case GUI_MSG_KEY_PLAY_PAUSE:
				case GUI_MSG_KEY_LONGPLAY_PAUSE:{/*暂停按键按下/长按*/
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_PLAY_PAUSE is press...\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						//new_movie_delete_subscene_by_id(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						robin_set_cmd_pause();//发送暂停命令
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
					}
					else
					{
						//new_movie_delete_subscene_by_id(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
						robin_set_cmd_play();//发送开始播放命令
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_play);
					}

					bok = EPDK_OK;
				}
				break;

				case GUI_MSG_KEY_ENTER:{/*回车按键按下*/
					__wrn("GUI_MSG_KEY_ENTER is press...\n");
					{
						//如果没有弹出菜单，则按enter暂停或开始播放
						__s32 ret;
						ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						__wrn("GUI_MSG_KEY_ENTER is press 2...\n");

						if(EPDK_FALSE == ret)//如果没有除字幕以外的子场景，则处理暂停、继续播放
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();	//获取fsm状态机的状态

							if(CEDAR_STAT_PLAY == fsm_sta)		//播放状态
							{
								__wrn("before robin_set_cmd_pause\n");
								robin_set_cmd_pause();			//设置暂停播放
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//播放声音暂停
								__wrn("after robin_set_cmd_pause\n");
								//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
								bok = EPDK_OK;
							}
							else if(CEDAR_STAT_PAUSE == fsm_sta)//暂停状态
							{
								__wrn("before robin_set_cmd_play\n");
								robin_set_cmd_play();//设置开始播放
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);//播放声音打开
								__wrn("after robin_set_cmd_play\n");
								bok = EPDK_OK;
							}
						}
					}
					
				}
				break;

				case GUI_MSG_KEY_ESCAPE:
				case GUI_MSG_KEY_MENU:{/*返回退出按键*/
					__s32 ret;
					ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL
					           //                 & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID) & (~MOVIE_PROG_ID));

					if(EPDK_TRUE == ret)//如果有除字幕和playpause以外的子场景，则不退出movie，删除所有子场景
					{
						
					}
					else//退出movie
					{
						if(new_is_on_tv())
						{
							new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//唤醒界面
						}
						else
						{
							//new_movie_exit_movie(new_movie_ctrl, 0);
						}
					}

					bok = EPDK_OK;

				}
				break;

				default:{/*否则直接退出当前的switch语句*/
					break;
				}

			}
		}
		else{//进入界面后的按键功能执行
			__cedar_status_t sta;
			sta = robin_get_fsm_status();//获取fsm状态机的状态
			__wrn("GUI_MSG_KEY sta=%d\n",sta);
			__wrn("msg->dwAddData1 is = %d ...\n",msg->dwAddData1);
			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_REPEATE:
				case GUI_MSG_KEY_LONGMENU://长按退出
				case GUI_MSG_KEY_SUBMENU:{
					__s32 ret;
					__here__;
					if(CEDAR_STAT_PLAY == sta)
					{
						__wrn("CEDAR_STAT_PLAY sta=%d\n",sta);
						robin_set_cmd_pause();
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//声音暂停
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
					}
					
					bok = EPDK_OK;
					
				}
				break;
				
				case GUI_MSG_KEY_RR:{//后退
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_RR\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_rr();//发送后退命令
					}
					bok = EPDK_OK;
					
				}
				break;

				case GUI_MSG_KEY_FF:{//快进
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_FF\n");
					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_ff();//发送快进命令
					}

					bok = EPDK_OK;
					
				}
				break;
				
				case GUI_MSG_KEY_MENU:{//返回退出按键
					__wrn("GUI_MSG_KEY_MENU is press to new_movie_exit_movie the home...\n");
					new_movie_exit_movie(new_movie_ctrl, 1);//退出到home应用
					bok = EPDK_OK;
				}
				break;

				case GUI_MSG_KEY_NUM1:{//按键1，返回退出到new explorer界面
					__wrn("GUI_MSG_KEY_MENU is press to new_movie_exit_movie the new_explorer...\n");
					new_movie_exit_movie(new_movie_ctrl, 0);//退出到new_explorer应用
					bok = EPDK_OK;
				}
				break;
				
				case GUI_MSG_KEY_RIGHT://下按键，快进
				case GUI_MSG_KEY_LONGRIGHT:{
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_RIGHT send ff...\n");
					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_ff();//发送快进命令
					}
					//__movie_prog_scene_create(new_movie_ctrl);//直接创建进度条framewin窗口

					bok = EPDK_OK;
					
				}
				break;
				
				case GUI_MSG_KEY_LEFT://上按键，后退
				case GUI_MSG_KEY_LONGLEFT:{
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_LEFT send rr...\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_rr();//发送后退命令
					}

					bok = EPDK_OK;
					
				}
				break;

				default:{/*否则直接退出switch语句*/
					break;
				}
			}


		}
	#endif
		
			if(KEY_UP_ACTION == msg->dwAddData2){/*检测按键松开动作为松开*/
				last_key = -1;
				__wrn("KEY_UP_ACTION is key up...\n");
			}
			else{/*按键为按下动作*/
				last_key = msg->dwAddData1;
				__wrn("key is press...\n");
			}

			__wrn("__app_new_movie_proc: GUI_MSG_KEY end\n");

			if(EPDK_OK == bok){ //如果消息已处理，则返回，否则交给子场景处理
				return EPDK_OK;
			}
			else{
				return EPDK_FAIL;
			}

}
/*视频app应用程序的回调处理函数*/
static __s32 __app_new_movie_proc(__gui_msg_t *msg)
{
	__wrn("__app_new_movie_proc: msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

	switch(msg->id)
	{
		case GUI_MSG_CREATE://创建窗口
		{
			__s32 ret;
			new_movie_ctrl_t *new_movie_ctrl;
			
			
			#if 1	/*设置背景颜色*/
				__here__;
				g_disable_close_scn();/*失能关闭SCN(屏幕，荧光屏)*/
				SEND_MSG(DSK_MSG_RELEASE_LOGO_BUF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);//通知init
				SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);//通知init
				__wrn("SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF)\n");
				com_set_backcolor(CONST_BackColor_Black);/*设置背景颜色*/
				__here__;
			#endif
			
			gscene_hbar_set_state(HBAR_ST_HIDE);
			__here__;
			gscene_bgd_set_state(BGD_STATUS_HIDE);
			__wrn("__app_new_movie_proc: GUI_MSG_CREATE begin\n");
			new_movie_ctrl = esMEMS_Balloc(sizeof(new_movie_ctrl_t));/*分配内存空间*/

			if(!new_movie_ctrl)
			{
				__wrn("mem not enough...\n");
				return EPDK_FAIL;
			}

			__here__;
			new_movie_robin_sem_init();/*robin组件初始化*/
			eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));	/*结构体数据初始化，清0*/
			GUI_WinSetAddData(msg->h_deswin, (__u32)new_movie_ctrl);	/*添加一个数据*/
			new_movie_ctrl->new_movie_manager = msg->h_deswin;			/*窗口赋值*/
			
			#if 1	/*视频图层资源申请*/
				new_movie_ctrl_para_init(new_movie_ctrl);				/*视频控制参数初始化*/
			#endif
			
			__app_new_movie_reg_para_init(new_movie_ctrl);				/*注册表参数初始化*/
			__here__;
			#if 0	/*创建进度条framewin窗口*/ 
				progressbar_scene_create(msg);
			#endif
			
			#if 1	/*调色板参数设置*/
				com_set_palette_by_id(ID_MOVIE_PAL_BMP);//设置调色板id
				__here__;
				{
					//修改调色板
					//背景色，灰白色，字幕边框颜色
					__u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
					com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));//设置调色板颜色
				}
				__here__;
			#endif
			
			__wrn("before new_movie_cur_file_has_breakpoint\n");
			
			#if 1	/*通过 索引或者直接 播放视频文件*/
				new_movie_play_for_index_or_default(msg);
			#endif
			
			#if 1	/*开始播放视频文件*/
			{
				__wrn("new_movie_start_play is start...\n");

				//默认有断点则断点播放，无断点则从头播放
				{
					new_movie_start_play(new_movie_ctrl, NULL);//开始播放
					__here__;
					
				}
			}
			#endif
			
			#if 1	/*进度条framewin窗口创建函数*/
				__movie_progressbar_create(new_movie_ctrl);
			#endif
			
			#if 1	/*视频播放控制菜单framewin窗口创建函数*/
				__new_movie_spsc_ctrl_menu_scene_create(new_movie_ctrl);
			#endif
			
			#if 1	/*配置/开启播放程序定时器*/
				new_movie_process_after_start_play(new_movie_ctrl);//程序开始播放定时器打开
				__here__;
			#endif
			
			__wrn("__app_new_movie_proc: GUI_MSG_CREATE end\n");
			return EPDK_OK;
		}
		case GUI_MSG_PAINT://绘制窗口
		{
			
			return EPDK_OK;
		}
#if 1
		case GUI_MSG_KEY://按键功能
		{
			app_new_movie_key_proc(msg);//按键处理函数
			break;
		}
#endif 
		case GUI_MSG_CLOSE://关闭，退出窗口
		{
			if(1 == msg->dwAddData1)//退出到home
			{
				__wrn("exit to home...\n");
				new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, SETTING_SW_TO_MAIN, 0);//退出movie到home窗口
			}
			else//退出到 new explorer
			{
				__wrn("exit to explorer...\n");
				new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_MOVIE_SW_TO_NEW_EXPLORER, 2);//退出movie到new_explorer应用程序窗口
			}


			return EPDK_OK;
		}
#if 1
		case GUI_MSG_TIMER://窗口定时器功能
		{
			new_movie_ctrl_t *new_movie_ctrl;
			__s32 ret,total;
			__u32 cur_time=0,total_time=0;
			ret = -1;
			new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(new_movie_ctrl)
			{
				if(LOWORD(msg->dwAddData1) == new_movie_ctrl->auto_play_timmer_id)
				{

					{
						__s32 play_sta,amplifier_flag;
						__wrn("before new_movie_check_for_play_err\n");
						play_sta = new_movie_check_for_play_err(new_movie_ctrl);
						__wrn("after new_movie_check_for_play_err\n");

						if(EPDK_OK == play_sta)
						{
							__wrn("before new_movie_check_for_auto_play\n");
							new_movie_check_for_auto_play(new_movie_ctrl);
							__wrn("aftr new_movie_check_for_auto_play\n");
						}
						else//如果播放错误，则重新开始检测
						{
							//new_movie_reset_auto_play_timmer(new_movie_ctrl);
							//new_movie_ctrl->cur_stop_cnt = 0;
						}
						
						cur_time = robin_get_cur_time();		/*获取当前文件的播放时间*/
						__wrn("robin_get_cur_time = %d \n",cur_time);
						total_time = robin_get_total_time();	/*获取当前播放文件的总时间*/
						__wrn("robin_get_total_time = %d \n",total_time);
						if(cur_time == total_time)				//时间相同时暂停播放，并退出//播放完毕,退出应用
						{
							static __s32 time_count=0;
							#if 1	/*继续循环播放视频文件*/
								time_count = robin_npl_get_cur();/*获取当前播放列表要播放文件的索引id号*/
								__wrn("time_count = %d\n",time_count);
								
								total = robin_npl_get_total_count();	/*获取当前播放列表的所有文件总数*/
								__wrn("file_total = %d\n",total);
								if(total < time_count){			//播放所有视频之后退出应用，回到home界面
									time_count = 0;
									esKRNL_TimeDly(100);
									new_movie_exit_movie(new_movie_ctrl, 1);//1为回到home应用界面
								}
								else{
									time_count += 1;
									robin_npl_set_cur(time_count);	/*设置当前播放列表的要播放文件索引*/
									new_movie_start_play(new_movie_ctrl, 0);/*开始播放文件。0为无断点播放*/
									__wrn("new_movie_start_play is success.....\n");
								}
							#endif
							
							
						}

						amplifier_flag = dsk_amplifier_is_on();
						__wrn("amplifier_flag 2 =%d\n", amplifier_flag);
						ret = EPDK_OK;
					}
				}
				//==========================================
		
				//==========================================
			}
			else
			{
				__err("new_movie_ctrl is null...\n");
				return EPDK_OK;
			}

			if(-1 == ret) //未处理，交给子场景处理
			{
				break;
			}
			else//已处理
			{
				return EPDK_OK;
			}
		}
#endif
		case GUI_MSG_DESTROY://摧毁窗口功能
		{
			new_movie_ctrl_t *new_movie_ctrl;
			__wrn("__app_new_movie_proc: GUI_MSG_DESTROY begin\n");
			new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(new_movie_ctrl)
			{
				if(new_is_on_tv())//切换显示类型
				{
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//切换显示类型
				}

				__here__;
				#if 1//先释放定时器，之后再释放图片资源
					new_movie_uninstall_auto_play_timmer(new_movie_ctrl);//自动播放定时器释放
				#endif
				/*{
					//save breakpoint
					__s32 index;
					index = robin_npl_get_cur();
					__here__;

				}*/
				__here__;
				new_movie_stop_play(new_movie_ctrl);//停止播放，发送停止播放命令
				__here__;
				
				#if 1//删除进度条图层,删除前需要先停止播放
					progressbar_ui_scene_delete(new_movie_ctrl->progressbar_scene);
					if(new_movie_ctrl->progressbar_scene){//需要再清除一下这个储存进度条framewin窗口的句柄
						__wrn("new_movie_ctrl->progressbar_scene is free null...\n");
						//eLIBs_memset(new_movie_ctrl->progressbar_scene, 0, sizeof(new_movie_ctrl_t));
						new_movie_ctrl->progressbar_scene = NULL;
					}
				#endif
				
				#if 1//删除视频播放控制菜单图层
					new_movie_spsc_ctrl_menu_ui_scene_delete(new_movie_ctrl->spsc_ctrl_scene);
					if(new_movie_ctrl->spsc_ctrl_scene){//需要再清除一下这个储存控制菜单framewin窗口的句柄
						__wrn("new_movie_ctrl->spsc_ctrl_scene is free null...\n");
						//eLIBs_memset(new_movie_ctrl->spsc_ctrl_scene, 0, sizeof(new_movie_ctrl_t));
						new_movie_ctrl->spsc_ctrl_scene = NULL;
					}
				#endif
				
				__here__;
				new_movie_ctrl_para_deinit(new_movie_ctrl);//视频控制参数删除，包括图层释放，显示驱动删除等
				eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));
				esMEMS_Bfree(new_movie_ctrl, sizeof(new_movie_ctrl_t));//释放内存空间
				__here__;
			}

			g_display_switch_output(LION_DISP_LCD);	//movie 退出时恢复到小机显示
			g_enable_close_scn();					//关闭荧光屏
			robin_close();							//robin模块关闭
			new_movie_robin_sem_deinit();			//robin组件卸载不初始化
				
			com_set_backcolor(CONST_BackColor);		//设置背景颜色
			SEND_MSG(DSK_MSG_SCREENOFF_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);//发送背景关闭
			__wrn("__app_new_movie_proc: GUI_MSG_DESTROY end\n");
			return EPDK_OK;
		}

		default:
		{
			__msg("__app_new_movie_proc: default\n");
			break;
		}
	}

	return GUI_ManWinDefaultProc(msg);
}
/*视频app应用程序manwin窗口创建*/
H_WIN app_new_movie_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	H_WIN hManWin;
	__inf("****************************************************************************************\n");
	__inf("********  enter new movie app  **************\n");
	__inf("****************************************************************************************\n");
	//esKSRV_SysInfo();//检查内存是否泄漏
	/*******************开始操作manwin窗口配置*********************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_NEWMOVIE;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_movie_proc);
	create_info.attr            = (void *)para;
	create_info.id				= APP_NEWMOVIE_ID;//地址0x9801
	create_info.hHosting        = NULL;
	hManWin = GUI_ManWinCreate(&create_info);
	__wrn("app_new_new_movie_create, hManWin=%x\n", hManWin);
	#if 0
	if(hManWin)
	{
		reg_root_para_t *root_reg_para;
		__s32 reg_storage_type;
		reg_storage_type = 0;

		if(para)
		{
			if(RAT_TF == para->root_type)
			{
				reg_storage_type = 0;
			}
			else if(RAT_USB == para->root_type)
			{
				reg_storage_type = 1;
			}
			else
			{
				__wrn("para->root_para->root_type invalid...\n");
			}
		}

		root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

		if(root_reg_para)
		{
			__s32 reg_app_type;
			__wrn("root_reg_para...\n");
			reg_app_type = m_eMediaTypeList_video;
			root_reg_para->cur_play_storage = reg_storage_type;
			root_reg_para->last_play_app = reg_app_type;
			root_reg_para->last_app_play_storage[reg_app_type] = reg_storage_type;
			root_reg_para->last_storage_play_app[reg_storage_type] = reg_app_type;
		}
		else
		{
			__wrn("para is null...\n");
		}
	}
	#endif

	return hManWin;
}

__s32 app_new_movie_is_tv_out(H_WIN hmanwin)
{
#if 1
	return g_new_is_on_tv;
#endif
}



