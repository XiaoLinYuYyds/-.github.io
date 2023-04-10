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

#include "app_movie_i.h"

#define TIMEROUT_ID_TIMER_HLYR_FF_FB		400
#if TIMEROUT_ID_TIMER_HLYR_FF_FB
#define ID_TIMER_HLYR_FF_FB			0x5555
#endif

typedef struct
{
	//ctrl scene
	void *spsc_scene;//manwin

	void *volume_scene;//frmwin

	void *bright_scene;//frmwin

	void *playsta_scene;//frmwin

	void *sub_show_scene;//frmwin
	__u32 sub_state;
	__u32 sub_pos;
	__u32 sub_color;
	__u32 sub_size;
	__u32 sub_stream;//0代表国语 1代表粤语

	void *prog_scene;//frmwin

	void *sub_set_scene;//frmwin

	H_WIN breakplay_scene;//该值直接就是frmwin句柄
	H_WIN playerror_scene;//该值直接就是frmwin句柄
	H_WIN playend_scene;//该值直接就是frmwin句柄
	H_WIN h_frm_alarm_coming ;	//闹钟定时时间到的时候弹出的对话框的窗口句柄
	H_WIN movie_manager;//movie的manwin

	ES_FILE *hdisp;//显示驱动句柄
	__hdle	 hvedio_lyr;//视频图层

	__u8	 auto_play_timmer_id;

	__bool   exit_movie;//是否退出movie
	__s32    err_cnt;//播放错误的文件个数
	__s32    rr_flag;//是否进行了快退操作，用于快退到尽头时，做判断用

	__s32 stop_cnt ;//连续stop_cnt次检测到stop，才播下一首，防止还没播起来就跳到下一首
	__s32 cur_stop_cnt;
	__s32 cur_time;//记录当前播放时间，用于制造一个视频断点信息
	char  cur_playing_file[RAT_MAX_FULL_PATH_LEN + 1];
	__bool quit_by_fs_part_plug_out;//如果退出movie了，是否拔卡造成的退出
	__u8 auto_flush_timmer_id;
	__s32 btv_out;

	//数字选取===================================
	__s32  ndigit_movie_num; //数字选取序号
	__u8   digit_timmer_id;  //数字选取timmer id号
	H_LYR  hdigit_layer;     //显示当前数字的图层
	H_LYR	m_hLyr_ff_fb;	// 显示快进、快退提示界面
	__s16	m_ff_fb_speed;
	//=============================================
#if SP_FavoriteFiles
	__s32 cur_media_type; //指定当前的媒体库收藏和非收藏播放
#endif
}
movie_ctrl_t;

static movie_icon_data_t 		movie_icon_table[] =
{
	{ ID_MOVIE_NUM0_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM1_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM2_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM3_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM4_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM5_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM6_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM7_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM8_BMP,				NULL,  			 	NULL },
	{ ID_MOVIE_NUM9_BMP,				NULL,  			 	NULL },
#ifdef ID_MOVIE_NUMLOCK_BG_BMP
	{ ID_MOVIE_NUMLOCK_BG_BMP,			NULL,				NULL },
#endif

	{ 0,                                    		NULL,  				NULL }      	//browser_icon_max
};


static movie_ctrl_t *g_movie_ctrl = NULL;
static __s32 is_touch_ff_rr_valid = 0;//代表判断当前是否可以进行触摸快进快退

static __s32 g_is_on_tv = 0;

static __s32 __movie_playend_scene_create(movie_ctrl_t *movie_ctrl);
static __s32 __movie_has_sub_scene(movie_ctrl_t *movie_ctrl, __u32 sub_id);

//删除movie的子场景
static __s32 __movie_delete_subscene_by_id(movie_ctrl_t *movie_ctrl, __u32 sub_id);

static __s32 __movie_playerror_scene_create(movie_ctrl_t *movie_ctrl);

//是否有除字幕以外的子场景
static __bool __app_has_visuable_sub_scene_except_x(movie_ctrl_t *movie_ctrl, __u32 sub_scene);

#define GUI_MSG_SET_TVOUT_TP_HOOK_MSG (GUI_MSG_USER_DEF+1)

static __s32 movie_save_breakpoint_info(movie_ctrl_t *movie_ctrl);

void movie_ff_fb_over(movie_ctrl_t *movie_ctrl);



static __s32 movie_save_breakpoint_info(movie_ctrl_t *movie_ctrl)
{
	__wrn("movie_save_breakpoint_info.............\n");
	{
		//save breakpoint
		__s32 index;
		index = robin_npl_get_cur();
		__here__;

		if(-1 != index)
		{
			__here__;
			__movie_save_breakpoint_info(movie_ctrl, index);
		}
	}
	__movie_save_last_playing_file(movie_ctrl);
	dsk_reg_flush();
	return EPDK_OK;
}


void *movie_get_icon_res(__u32 index)
{
	movie_icon_data_t *icon_table = NULL;
	icon_table = movie_icon_table;

	if(index > (sizeof(movie_icon_table) / sizeof(movie_icon_table[0])))
	{
		return NULL;
	}

	//__msg("----icon_id: %d, res_id: %d\n", index, icon_table[index].res_id);
	if(icon_table == NULL)
	{
		return NULL;
	}
	else if(icon_table[index].res_id == 0)
	{
		return NULL;
	}
	else if(icon_table[index].res_handle == NULL)
	{
#if 1
		icon_table[index].res_handle = theme_open(icon_table[index].res_id);
		icon_table[index].res_buffer = theme_hdl2buf(icon_table[index].res_handle);
		return icon_table[index].res_buffer;
#else
		{
			int i = 0;
			__msg("------------open all of the picture at first\n");
			app_print_memory_usage("new memory for picture resource", NULL);
			app_print_memory_usage("new memory for picture resource", NULL);

			for(i = 0; i < MUSIC_ICON_MAX; i++)
			{
				if(icon_table[i].res_handle == NULL)
				{
					icon_table[i].res_handle = theme_open(icon_table[i].res_id);
					icon_table[i].res_buffer = theme_hdl2buf(icon_table[i].res_handle);
				}
			}

			app_print_memory_usage("new memory for picture resource finished", NULL);
			return icon_table[index].res_buffer;
		}
#endif
	}
	else
	{
		return icon_table[index].res_buffer;
	}
}

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR movie_8bpp_layer_create(RECT *rect, __s32 pipe)
{
	H_LYR layer = NULL;
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
		"sub menu layer",
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
		__err("GUI_LyrWinCreate fail... !\n");
	}

	return layer;
}

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR movie_32bpp_layer_create(RECT *rect, __s32 pipe)
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
		"sub menu layer",
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
		__err("GUI_LyrWinCreate fail... !\n");
	}

	return layer;
}

static __bool is_on_tv(void)
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
static __s32 __movie_ctrl_para_init(movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__err("movie_ctrl is null...\n");
		return EPDK_FAIL;
	}

	movie_ctrl->hdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r");

	if(!movie_ctrl->hdisp)
	{
		__msg("open disp fail...\n");
	}

	__wrn("open disp success...\n");
	{
		__s32 ret;
		__s32 screen_width, screen_height;
		RECT vedio_rect;
		robin_open_arg_t arg;
		arg.cedar_stop_mode = CEDAR_STOP_MODE_UNINSTALL_PLUGINS;
		arg.switch_vplay_mode = CEDAR_FILE_SWITCH_VPLY_MODE_GAP;
		arg.reserve_mem_size = 1024 * 1024;	//512*1024;
		ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);

		if(EPDK_OK != ret)
		{
			__msg("robin_open fail...\n");
			return EPDK_FAIL;
		}

		__wrn("robin_open success...\n");
		//robin_clear_feedback_msgQ();
		dsk_display_get_size(&screen_width, &screen_height);
		vedio_rect.x = 0;
		vedio_rect.y = 0;
		vedio_rect.width = screen_width;
		vedio_rect.height = screen_height;
		__wrn("vedio screen=%d %d %d %d", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
		movie_ctrl->hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);

		if(!movie_ctrl->hvedio_lyr)
		{
			__msg("robin_request_vedio_layer fail...\n");
			return EPDK_FAIL;
		}

		__wrn("robin_request_vedio_layer success...\n");
	}

	if(is_on_tv())
	{
		__u32 arg[3] = {0};
		arg[0] = movie_ctrl->hvedio_lyr;
		eLIBs_fioctrl(movie_ctrl->hdisp, DISP_CMD_VIDEO_START, 0, (void *)arg);
	}

	{
		__s32 ret;
		__u32 arg[3] = {0};
		arg[0] = movie_ctrl->hvedio_lyr;
		ret = eLIBs_fioctrl(movie_ctrl->hdisp, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);

		if(0 != ret)
		{
			__msg("DISP_CMD_LAYER_BOTTOM fail...\n");
		}
		else
		{
			__msg("DISP_CMD_LAYER_BOTTOM success...\n");
		}
	}

	movie_ctrl->auto_play_timmer_id = 0x10;
	/*movie_ctrl->auto_flush_timmer_id = 0x31;
	movie_ctrl->err_cnt             = 0;
	movie_ctrl->rr_flag             = 0;
	movie_ctrl->stop_cnt            = 2;
	movie_ctrl->cur_stop_cnt        = 0;
	movie_ctrl->cur_time            = 0;
	movie_ctrl->btv_out             = 0;
	movie_ctrl->digit_timmer_id		= 0x4a;*/
	eLIBs_memset(movie_ctrl->cur_playing_file, 0, sizeof(movie_ctrl->cur_playing_file));
	//movie_ctrl->quit_by_fs_part_plug_out    = EPDK_FALSE;
	__msg("__movie_para_init success...\n");
	return EPDK_OK;
}

static __s32 __movie_ctrl_para_deinit(movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(is_on_tv())
	{
		__u32 arg[3] = {0};
		arg[0] = movie_ctrl->hvedio_lyr;
		eLIBs_fioctrl(movie_ctrl->hdisp, DISP_CMD_VIDEO_STOP, 0, (void *)arg);
	}

	if(movie_ctrl->hvedio_lyr)
	{
		robin_release_video_layer(movie_ctrl->hvedio_lyr);
		movie_ctrl->hvedio_lyr = NULL;
	}

	if(movie_ctrl->hdisp)
	{
		eLIBs_fclose(movie_ctrl->hdisp);
		movie_ctrl->hdisp = NULL;
	}

	return EPDK_OK;
}
/*配置自动播放定时器*/
static __s32 __movie_install_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	__wrn("movie_ctrl->auto_play_timmer_id = 0x%x\n",movie_ctrl->auto_play_timmer_id);
	if(!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
	{
		GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
		             , 200, NULL);//0.5秒
		__wrn("GUI_SetTimer timmer is 1...\n");
	}
/*
#if MOVIE_AUTO_FLUSH_TIME

	if(!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_flush_timmer_id))
	{
		GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_flush_timmer_id
		             , MOVIE_AUTO_FLUSH_FIRST_TIME, NULL);
		__msg("GUI_SetTimer timmer is 2...\n");
	}
	else
	{
		GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_flush_timmer_id
		               , MOVIE_AUTO_FLUSH_FIRST_TIME, NULL);
		__msg("GUI_SetTimer timmer is 3...\n");
	}

#endif*/
	return EPDK_OK;
}
/*复位自动播放定时器*/
static __s32 __movie_reset_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("__movie_reset_auto_play_timmer is 1...\n");
	if(GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
	{
		GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
		               , 200, NULL);//0.5秒
	}
	else
	{
		__msg("timmer already install...\n");
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*卸载自动播放定时器*/
static __s32 __movie_uninstall_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("__movie_uninstall_auto_play_timmer is 1...\n");
	if(GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
	{
		GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id);
	}

#if MOVIE_AUTO_FLUSH_TIME

	if(GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_flush_timmer_id))
	{
		GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_flush_timmer_id);
	}

#endif
	return EPDK_OK;
}
/*加载视频断点信息*/
static __s32 __movie_load_breakpoint_info(__s32 index, __cedar_tag_inf_t *tag_inf)
{
	__s32 ret;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	ret = robin_npl_index2file(index, filename);
	__wrn("__movie_load_breakpoint_info is 1...\n");
	if(-1 == ret)
	{
		__msg("robin_npl_index2file fail...\n");
		return -1;
	}

	__here__;

	if(!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
	{
		return -1;
	}

	__here__;

	if(get_item_content_from_fifo_db(NAME_MOVIE_BKPOINT, filename, tag_inf))
	{
		return -1;
	}

	__here__;
	return 0;
}
/*制作视频标签*/
static __s32 __movie_make_tag(movie_ctrl_t *movie_ctrl, __cedar_tag_inf_t *tag_inf)
{
	char cur_playing_file[RAT_MAX_FULL_PATH_LEN + 1] = {0};

	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!tag_inf)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	cur_playing_file[0] = 0;
	robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
	__msg("cur_playing_file=%s\n", cur_playing_file);
	__msg("movie_ctrl->cur_playing_file=%s\n", movie_ctrl->cur_playing_file);

	if(0 == eLIBs_strcmp(cur_playing_file, movie_ctrl->cur_playing_file))
	{
		eLIBs_memset(tag_inf, 0, sizeof(__cedar_tag_inf_t));
		eLIBs_strncpy(tag_inf->tag_magic, "VTAG", 4);
		tag_inf->time_offset = movie_ctrl->cur_time;
		__msg("tag_inf->time_offset=%d\n", tag_inf->time_offset);
	}
	else
	{
		__here__;
		movie_ctrl->cur_time = 0;
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*保存视频播放断点信息*/
static __s32 __movie_save_breakpoint_info(movie_ctrl_t *movie_ctrl, __s32 index)
{
	__cedar_tag_inf_t tag_inf;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__s32 ret;
	__cedar_status_t  fsm_sta;

	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	fsm_sta = robin_get_fsm_status();
	__msg("fsm_sta=%d\n", fsm_sta);
	/*get the tag*/
	ret = robin_get_tag(&tag_inf);
	__wrn("robin_get_tag, ret = %d\n", ret);

	if(0 != ret) //如果视频库获取断点信息失败，则自造一个断点信息
	{
		__msg("robin_get_tag fail...\n");
		return -1;

		if(EPDK_TRUE == movie_ctrl->quit_by_fs_part_plug_out)
		{
			ret = __movie_make_tag(movie_ctrl, &tag_inf);
			__msg("__movie_make_tag, ret = %d\n", ret);

			if(0 != ret)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	__here__;
	ret = robin_npl_index2file(index, filename);

	if(-1 == ret)
	{
		__msg("robin_npl_index2file fail...\n");
		return -1;
	}

	__wrn("begin save breakpoint, index = %d, filename=%s\n", index, filename);

	if(!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
	{
		ret = create_fifo_db(NAME_MOVIE_BKPOINT, 4, sizeof(__cedar_tag_inf_t));

		if(0 != ret)
		{
			__msg("create_fifo_db fail...\n");
			return -1;
		}
	}

	__here__;
	ret = add_item_to_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);
	__msg("add_item_to_fifo_db, ret=%d\n", ret);
	return ret;
}
/*删除视频断点信息*/
static __s32 __movie_delete_breakpoint_info(movie_ctrl_t *movie_ctrl, __s32 index)
{
	__s32 ret;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__cedar_tag_inf_t tag_inf;

	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	ret = robin_npl_index2file(index, filename);

	if(-1 == ret)
	{
		__msg("robin_npl_index2file fail...\n");
		return -1;
	}

	if(!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
	{
		return -1;
	}

	ret = get_item_content_from_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);

	if(-1 == ret)
	{
		return 0;
	}

	if(delete_item_from_fifo_db(NAME_MOVIE_BKPOINT, filename))
	{
		return -1;
	}

	return 0;
}
/*视频文件播放*/
static __s32 __movie_play_file(movie_ctrl_t *movie_ctrl, __s32 index, __bool breakplay)
{
	__s32 ret;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__cedar_tag_inf_t tag_inf;
	ret = robin_npl_index2file(index, filename);

	if(-1 == ret)
	{
		__wrn("robin_npl_index2file fail...\n");
		return EPDK_FAIL;
	}

	//movie_ff_fb_over(movie_ctrl);
	__wrn("breakplay=%d\n", breakplay);
	__wrn("filename=%s\n", filename);

	/*if(EPDK_TRUE == breakplay)
	{
		ret = __movie_load_breakpoint_info(index, &tag_inf);

		if(0 == ret)
		{
			__wrn("has breakpoint info, begin breakplay... index = %d filename=%s\n", index, filename);
			robin_set_cmd_stop();
			//robin_clear_feedback_msgQ();//清空错误信息消息队列
			ret = robin_play_file(filename, &tag_inf);

			if(EPDK_FAIL == ret)
			{
				return EPDK_FAIL;
			}

			robin_set_cmd_play();
		}
		else
		{
			robin_set_cmd_stop();
			//robin_clear_feedback_msgQ();//清空错误信息消息队列
			ret = robin_play_file(filename, NULL);

			if(EPDK_FAIL == ret)
			{
				return EPDK_FAIL;
			}

			robin_set_cmd_play();
			__wrn("wrn, no breakpoint info, not breakplay... index = %d filename=%s\n", index, filename);
		}
	}
	else*/
	{
		robin_set_cmd_stop();
		//robin_clear_feedback_msgQ();//清空错误信息消息队列
		ret = robin_play_file(filename, NULL);
		__wrn("ret=%d\n", ret);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}

		robin_set_cmd_play();
		__wrn("ret=%d\n", ret);
	}

	if(EPDK_OK == ret)
	{
		__wrn("__movie_play_file ret=%d\n", ret);
		__movie_install_auto_play_timmer(movie_ctrl);
	}

	return ret;
}
/*随机播放*/
__s32 __movie_play_random(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;
	rat_play_mode_e playMode = robin_get_play_mode();

	if(playMode != RAT_PLAY_MODE_RANDOM)
	{
		return EPDK_FAIL;
	}
	__wrn("__movie_play_random ret=%d\n", ret);
	if(1 == movie_ctrl->rr_flag)
	{
		__s32 index;
		index = robin_npl_get_cur();
		__wrn("index=%d\n", index);

		if(-1 != index)
		{
			__movie_play_file(movie_ctrl, index, EPDK_FALSE);
		}

		//movie_ctrl->rr_flag = 0;
	}
	else
	{
		ret = __movie_auto_play_next(movie_ctrl);
		__wrn("ret=%d\n", ret);

		if(EPDK_OK != ret) //播放完毕,退出应用
		{
			__movie_playend_scene_create(movie_ctrl);
			esKRNL_TimeDly(100);
			__movie_exit_movie(movie_ctrl, 0);
		}
		else
		{
			{
				//通知子场景更新UI
				__gui_msg_t msg;
				eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
				__msg("movie_ctrl->spsc_scene=%x\n", movie_ctrl->spsc_scene);

				if(movie_ctrl->spsc_scene)
				{
					msg.h_deswin = movie_spsc_scene_get_hwnd(movie_ctrl->spsc_scene);

					if(msg.h_deswin)
					{
						msg.id = movie_spsc_scene_update_fileinfo;
						msg.dwAddData1 = 0;
						GUI_SendMessage(&msg);
					}
				}
			}
		}
	}

	return EPDK_OK;
}

#if SP_FavoriteFiles
static __s32 __movie_favor_play_file(movie_ctrl_t *movie_ctrl, char *filename)
{
	__s32 ret;
	robin_set_cmd_stop();
	ret = robin_play_file(filename, NULL);
	__wrn("ret=%d\n", ret);

	if(EPDK_FAIL == ret)
	{
		return EPDK_FAIL;
	}

	robin_set_cmd_play();
	__wrn("ret=%d\n", ret);

	if(EPDK_OK == ret)
	{
		__movie_install_auto_play_timmer(movie_ctrl);
	}

	return EPDK_OK;
}

static __s32 rat_get_filenamebyindex(movie_ctrl_t *movie_ctrl, __s32 index , char *filename)
{
	HRAT hrat = NULL;
	__s32 ret;
	root_para_t *root_para;
	rat_entry_t ItemInfo;

	if(movie_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	if(movie_ctrl->movie_manager == NULL)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(root_para == NULL)
	{
		return EPDK_FAIL;
	}

	rat_set_modify_flag(RAT_MEDIA_TYPE_FAVOR_VIDEO, 1);
	hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

	if(hrat == NULL)
	{
		__wrn("EPDK_FAIL\n");
		return EPDK_FAIL;
	}

	ret = rat_get_item_info_by_index(hrat, index, &ItemInfo);

	if(ret == EPDK_OK)
	{
		eLIBs_strcpy(filename, ItemInfo.Path);
	}

	rat_close(hrat);
	return ret;
}

static __s32 __movie_favor_start_play_file(movie_ctrl_t *movie_ctrl)
{
	root_para_t *root_para;
	char filename[1024] = {0};

	if(movie_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	if(movie_ctrl->movie_manager == NULL)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(root_para == NULL)
	{
		return EPDK_FAIL;
	}

	rat_get_filenamebyindex(movie_ctrl, root_para->favor_index, filename);
	__wrn("filename:%s\n", filename);
	__movie_favor_play_file(movie_ctrl, filename);
	return EPDK_OK;
}
#endif
/*播放下一个视频文件*/
static __s32 __movie_play_next(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;
	__s32 index;
	__s32 total;

	if(__movie_play_random(movie_ctrl) == EPDK_OK)
	{
		return EPDK_OK;
	}

	index = robin_npl_get_cur();
	__wrn("__movie_play_next is...\n");
	if(-1 == index)
	{
		__msg("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}

	__movie_delete_breakpoint_info(movie_ctrl, index);
	total = robin_npl_get_total_count();

	if(-1 == total)
	{
		__msg("robin_npl_get_total_count fail...\n");
		return EPDK_FAIL;
	}

	index++;

	if(index >= total)
	{
		index = 0;
	}

	robin_npl_set_cur(index);
	robin_set_cmd_stop();
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_NEXT);
	ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);

	if(EPDK_OK == ret)
	{
		//movie_save_breakpoint_info(movie_ctrl);
	}

	return ret;
}
/*播放上一个视频文件*/
static __s32 __movie_play_prev(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;
	__s32 index;
	__s32 total;

	if(__movie_play_random(movie_ctrl) == EPDK_OK)
	{
		return EPDK_OK;
	}

	index = robin_npl_get_cur();

	if(-1 == index)
	{
		__msg("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}

	total = robin_npl_get_total_count();

	if(-1 == total)
	{
		__msg("robin_npl_get_total_count fail...\n");
		return EPDK_FAIL;
	}

	index--;

	if(index < 0)
	{
		index = total - 1;
	}

	if(index < 0)
	{
		__msg("total movie count = 0\n");
		index = 0;
	}

	robin_npl_set_cur(index);
	robin_set_cmd_stop();
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PREV);
	ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);

	if(EPDK_OK == ret)
	{
		//movie_save_breakpoint_info(movie_ctrl);
	}

	return ret;
}
/*当前播放视频文件有断点*/
static __s32 __movie_cur_file_has_breakpoint(void)
{
	__s32 ret;
	__s32 index;
	__cedar_tag_inf_t tag_inf;
	index = robin_npl_get_cur();

	if(-1 == index)
	{
		__msg("robin_npl_get_cur fail...\n");
		return EPDK_FALSE;
	}
	__wrn("before __movie_cur_file_has_breakpoint\n");
	__wrn("before __movie_load_breakpoint_info\n");
	ret = __movie_load_breakpoint_info(index, &tag_inf);

	if(0 == ret)
	{
		return EPDK_TRUE;
	}
	else
	{
		return EPDK_FALSE;
	}
}
/*视频文件开始播放*/
static __s32 __movie_start_play(movie_ctrl_t *movie_ctrl, __bool breakplay)
{
	__s32 ret;
	__s32 index;
	index = robin_npl_get_cur();

	if(-1 == index)
	{
		__msg("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}
	__wrn(" __movie_start_play is...\n");
	ret = __movie_play_file(movie_ctrl, index, breakplay);
	return ret;
}
/*自动播放下一个视频文件*/
static __s32 __movie_auto_play_next(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;
	__s32 index;
	__s32 total;
	index = robin_npl_get_cur();
	__wrn(" __movie_auto_play_next is...\n");
	if(-1 != index)
	{
		__here__;
		__movie_delete_breakpoint_info(movie_ctrl, index);
		__here__;
	}

	index = robin_npl_get_next();

	if(-1 == index)
	{
		__msg("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}

	__movie_play_file(movie_ctrl, index, EPDK_FALSE);
	//__msg("ret=%d\n", ret);
	return EPDK_OK;//强制返回ok，cedar_set_play可能返回失败
}

/*停止播放*/
static __s32 __movie_stop_play(movie_ctrl_t *movie_ctrl)
{
	__wrn(" __movie_stop_play is...\n");
	robin_set_cmd_stop();
	return EPDK_OK;
}
/*暂停播放*/
static __s32 __movie_pause_play(movie_ctrl_t *movie_ctrl)
{
	__wrn(" __movie_pause_play is...\n");

	robin_set_cmd_pause();
	return EPDK_OK;
}


//flag:0表示退出到explorer，1表示退出到home
static __s32 __movie_exit_movie(movie_ctrl_t *movie_ctrl, __s32 flag)
{
	__msg("before __movie_exit_movie\n");

	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(movie_ctrl->exit_movie)
	{
		__msg("movie already exit...\n");
		return EPDK_FAIL;
	}

	movie_ctrl->exit_movie = 1;

	if(!movie_ctrl->movie_manager)
	{
		__err("movie_ctrl->movie_manager is null...\n");
		return EPDK_FAIL;
	}

	__movie_uninstall_auto_play_timmer(movie_ctrl);
	movie_save_breakpoint_info(movie_ctrl);
	//__movie_pause_play(movie_ctrl);视频库修改了，要求获取断点信息的时候只能是play状态，所以不能暂停
	{
		__gui_msg_t msg;
		eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
		msg.id = GUI_MSG_CLOSE;
		msg.dwAddData1 = flag;
		msg.h_deswin = movie_ctrl->movie_manager;
		GUI_SendMessage(&msg);
	}
	__msg("after __movie_exit_movie\n");
	return EPDK_OK;
}
/*检查是否播放错误*/
static __s32 __movie_check_for_play_err(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;
	ret = robin_get_feedback_msg();

	if(ret < 0)
	{
		robin_clear_feedback_msgQ();//清空错误信息消息队列
		/*__msg("robin_get_feedback_msg < 0, movie play err...\n");
		__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
		__movie_playerror_scene_create(movie_ctrl);
		esKRNL_TimeDly(100);
		__movie_playerror_scene_delete(movie_ctrl);
		movie_ctrl->err_cnt++;

		if(movie_ctrl->err_cnt >= robin_npl_get_total_count()
		    || RAT_PLAY_MODE_ROTATE_ONE == robin_get_play_mode())
		{
			__movie_exit_movie(movie_ctrl, 0);
		}*/

		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*检查自动播放*/
static __s32 __movie_check_for_auto_play(movie_ctrl_t *movie_ctrl)
{
	if(!movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__cedar_status_t fsm_sta;
		fsm_sta = robin_get_fsm_status();
		__msg("fsm_sta= %d\n", fsm_sta);

		if(CEDAR_STAT_PLAY == fsm_sta
		    || CEDAR_STAT_PAUSE == fsm_sta)
		{
			movie_ctrl->cur_time = robin_get_cur_time();
			robin_get_fsm_cur_file(movie_ctrl->cur_playing_file, sizeof(movie_ctrl->cur_playing_file));
		}
		else
		{
			movie_ctrl->cur_time = -1;
			eLIBs_strcpy(movie_ctrl->cur_playing_file, "");
		}

		switch(fsm_sta)
		{
			/*case CEDAR_STAT_STOP:
			{
				__s32 ret;
				movie_ctrl->cur_stop_cnt++;

				if(movie_ctrl->cur_stop_cnt < movie_ctrl->stop_cnt)
				{
					break;
				}

				movie_ctrl->cur_stop_cnt = 0;
				__msg("CEDAR_STAT_STOP, start play next...\n");
				__msg("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);

				if(1 == movie_ctrl->rr_flag)
				{
					__s32 index;
					index = robin_npl_get_cur();
					__msg("index=%d\n", index);

					if(-1 != index)
					{
						__movie_play_file(movie_ctrl, index, EPDK_FALSE);
					}

					//movie_ctrl->rr_flag = 0;
				}
				else
				{
#if SP_FavoriteFiles

					if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_FAVOR_VIDEO)
					{
						ret = movie_favor_auto_paly_next(movie_ctrl);
					}
					else if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_VIDEO)
#endif
					{
						ret = __movie_auto_play_next(movie_ctrl);
					}

					__msg("ret=%d\n", ret);

					if(EPDK_OK != ret) //播放完毕,退出应用
					{
						__movie_playend_scene_create(movie_ctrl);
						esKRNL_TimeDly(100);
						__movie_exit_movie(movie_ctrl, 0);
					}
					else
					{
						{
							//通知子场景更新UI
							__gui_msg_t msg;
							eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
							__msg("movie_ctrl->spsc_scene=%x\n", movie_ctrl->spsc_scene);

							if(movie_ctrl->spsc_scene)
							{
								msg.h_deswin = movie_spsc_scene_get_hwnd(movie_ctrl->spsc_scene);

								if(msg.h_deswin)
								{
									msg.id = movie_spsc_scene_update_fileinfo;
									msg.dwAddData1 = 0;
									GUI_SendMessage(&msg);
								}
							}
						}
					}
				}

				break;
			}

			case CEDAR_STAT_PLAY:
			{
				movie_ctrl->rr_flag = 0;
				__msg("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);
				{
					//存在能正常播放的文件，把该值复位一下
					movie_ctrl->err_cnt = 0;
				}
				break;
			}

			case CEDAR_STAT_RR:
			{
				movie_ctrl->rr_flag = 1;
				__msg("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);
				break;
			}
*/
			default:
			{
				movie_ctrl->rr_flag = 0;
				__msg("fsm_sta=%d, movie_ctrl->rr_flag=%d\n", fsm_sta, movie_ctrl->rr_flag);
				break;
			}
		}
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		movie_spsc_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_spsc_create_para_t));
		create_para.hparent = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_SPSC_ID;
		movie_ctrl->spsc_scene = movie_spsc_scene_create(&create_para);

		if(NULL == movie_ctrl->spsc_scene)
		{
			__msg("movie_spsc_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->spsc_scene)
		{
			movie_spsc_scene_delete(movie_ctrl->spsc_scene);
			movie_ctrl->spsc_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_volume_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		movie_volume_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_volume_create_para_t));
		create_para.hparent = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_VOLUME_ID;
		create_para.max_val = AUDIO_DEVICE_VOLUME_MAX;
		create_para.min_val = AUDIO_DEVICE_VOLUME_MIN;
		create_para.cur_val = dsk_volume_get();
		movie_ctrl->volume_scene = movie_volume_scene_create(&create_para);

		if(NULL == movie_ctrl->volume_scene)
		{
			__msg("movie_volume_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_volume_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->volume_scene)
		{
			movie_volume_scene_delete(movie_ctrl->volume_scene);
			movie_ctrl->volume_scene = NULL;
		}
	}

	return EPDK_OK;
}


static __s32 __movie_volume_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->volume_scene)
		{
			movie_volume_scene_set_focus(movie_ctrl->volume_scene);
		}
	}

	return EPDK_OK;
}

static __s32 __movie_bright_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		movie_bright_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_bright_create_para_t));
		create_para.hparent = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_BRIGHT_ID;
		create_para.max_val = CONST_DispBright_Max;
		create_para.min_val = CONST_DispBright_Min;
		create_para.cur_val = dsk_get_display_global(DISP_CMD_GET_BRIGHT);
		movie_ctrl->bright_scene = movie_bright_scene_create(&create_para);

		if(NULL == movie_ctrl->bright_scene)
		{
			__msg("movie_bright_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_bright_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->bright_scene)
		{
			movie_bright_scene_delete(movie_ctrl->bright_scene);
			movie_ctrl->bright_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_bright_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->bright_scene)
		{
			movie_bright_scene_set_focus(movie_ctrl->bright_scene);
		}
	}

	return EPDK_OK;
}

static __s32 __movie_playsta_scene_create(movie_ctrl_t *movie_ctrl, movie_playsta_t sta)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(NULL == movie_ctrl->playsta_scene)
	{
		movie_playsta_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_playsta_create_para_t));
		create_para.hparent = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_PLAYSTA_ID;
		create_para.playsta = sta;
		movie_ctrl->playsta_scene = movie_playsta_scene_create(&create_para);

		if(NULL == movie_ctrl->playsta_scene)
		{
			__msg("movie_playsta_scene_create fail...\n");
			return EPDK_FAIL;
		}

		movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
	}

	return EPDK_OK;
}

static __s32 __movie_playsta_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->playsta_scene)
		{
			movie_playsta_scene_delete(movie_ctrl->playsta_scene);
			movie_ctrl->playsta_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_playsta_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->playsta_scene)
		{
			movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
		}
	}

	return EPDK_OK;
}

static __s32 __movie_sub_show_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		movie_sub_show_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_sub_show_create_para_t));
		create_para.hparent  = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_SUB_SHOW_ID;
		create_para.sub_pos  = movie_ctrl->sub_pos;
		create_para.sub_color = movie_ctrl->sub_color;
		create_para.sub_size = movie_ctrl->sub_size;
		movie_ctrl->sub_show_scene = movie_sub_show_scene_create(&create_para);

		if(NULL == movie_ctrl->sub_show_scene)
		{
			__msg("movie_sub_show_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_sub_show_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->sub_show_scene)
		{
			movie_sub_show_scene_delete(movie_ctrl->sub_show_scene);
			movie_ctrl->sub_show_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_sub_set_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		movie_sub_set_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_sub_set_create_para_t));
		create_para.hparent = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_SUB_SET_ID;
		create_para.sub_state = movie_ctrl->sub_state;
		create_para.sub_color = movie_sub_set_color2index(movie_ctrl->sub_color);
		create_para.sub_pos =  movie_ctrl->sub_pos;
		create_para.sub_stream =  movie_ctrl->sub_stream;
		movie_ctrl->sub_set_scene = movie_sub_set_scene_create(&create_para);

		if(NULL == movie_ctrl->sub_set_scene)
		{
			__msg("movie_sub_set_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_sub_set_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->sub_set_scene)
		{
			movie_sub_set_scene_delete(movie_ctrl->sub_set_scene);
			movie_ctrl->sub_set_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_sub_set_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->sub_set_scene)
		{
			movie_sub_set_scene_set_focus(movie_ctrl->sub_set_scene);
		}
	}

	return EPDK_OK;
}

static __s32 __movie_prog_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		movie_prog_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(movie_prog_create_para_t));
		create_para.hparent = movie_ctrl->movie_manager;
		create_para.scene_id = MOVIE_PROG_ID;
		movie_ctrl->prog_scene = movie_prog_scene_create(&create_para);
		

		if(NULL == movie_ctrl->prog_scene)
		{
			__msg("movie_volume_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_prog_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->prog_scene)
		{
			movie_prog_scene_delete(movie_ctrl->prog_scene);
			movie_ctrl->prog_scene = NULL;
		}
	}

	return EPDK_OK;
}


static __s32 __movie_prog_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->prog_scene)
		{
			movie_prog_scene_set_focus(movie_ctrl->prog_scene);
		}
	}

	return EPDK_OK;
}


static __s32 __movie_breakplay_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(NULL == movie_ctrl->breakplay_scene)
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_BREAKPLAY};
			default_dialog(movie_ctrl->breakplay_scene, movie_ctrl->movie_manager, MOVIE_BREAKPLAY_DIALOG_ID, ADLG_YESNO, lang_id);
		}

		//movie_ctrl->breakplay_scene = create_breakplay_dlg();
		if(NULL == movie_ctrl->breakplay_scene)
		{
			__msg("default_dialog fail...\n");
			return EPDK_FAIL;
		}
		else
		{
			if(!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
			{
				GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
				             , 500, NULL);//0.5秒
			}
		}
	}

	return EPDK_OK;
}

static __s32 __movie_breakplay_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->breakplay_scene)
		{
			app_dialog_destroy(movie_ctrl->breakplay_scene);
			movie_ctrl->breakplay_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_playerror_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(NULL == movie_ctrl->playerror_scene)
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_FILE_NOT_SUPPORT};
			default_dialog(movie_ctrl->playerror_scene, movie_ctrl->movie_manager, MOVIE_PLAYERROR_DIALOG_ID, ADLG_OK, lang_id);
		}

		//movie_ctrl->breakplay_scene = create_breakplay_dlg();
		if(NULL == movie_ctrl->playerror_scene)
		{
			__msg("default_dialog fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_playerror_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->playerror_scene)
		{
			app_dialog_destroy(movie_ctrl->playerror_scene);
			movie_ctrl->playerror_scene = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_playend_scene_create(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(NULL == movie_ctrl->playend_scene)
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_PLAY_END};
			default_dialog(movie_ctrl->playend_scene, movie_ctrl->movie_manager, MOVIE_PLAYEND_DIALOG_ID, ADLG_OK, lang_id);
		}

		if(NULL == movie_ctrl->playend_scene)
		{
			__msg("default_dialog fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_playend_scene_delete(movie_ctrl_t *movie_ctrl)
{
	__s32 ret;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(movie_ctrl->playend_scene)
		{
			app_dialog_destroy(movie_ctrl->playend_scene);
			movie_ctrl->playend_scene = NULL;
		}
	}

	return EPDK_OK;
}

//判断movie manwin是否有指定的子场景之一
static __s32 __movie_has_sub_scene(movie_ctrl_t *movie_ctrl, __u32 sub_id)
{
	H_WIN child;
	char  winname[256];
	__u32 child_id;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FALSE;
	}

	if(!movie_ctrl->movie_manager)
	{
		__err("invalid para...\n");
		return EPDK_FALSE;
	}

	child = GUI_WinGetFirstChild(movie_ctrl->movie_manager);

	while(child != NULL)
	{
		child_id = GUI_WinGetItemId(child);

		if((child_id & sub_id))
		{
			return EPDK_TRUE;
		}

		child = GUI_WinGetNextBro(child);
	}

	return EPDK_FALSE;
}

//删除movie的子场景
static __s32 __movie_delete_subscene_by_id(movie_ctrl_t *movie_ctrl, __u32 sub_id)
{
	H_WIN child, next_child;
	char  winname[256];
	__u32 child_id;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!movie_ctrl->movie_manager)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	child = GUI_WinGetFirstChild(movie_ctrl->movie_manager);

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);
		child_id = GUI_WinGetItemId(child);

		if((child_id & sub_id)) //需要删除
		{
			__msg("begin delete movie sub scene, winname = %s \n", winname);

			switch(child_id)
			{
				case MOVIE_SPSC_ID:
				{
					__movie_spsc_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_VOLUME_ID:
				{
					__movie_volume_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_BRIGHT_ID:
				{
					__movie_bright_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_PLAYSTA_ID:
				{
					__movie_playsta_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_SUB_SHOW_ID:
				{
					__movie_sub_show_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_SUB_SET_ID:
				{
					__movie_sub_set_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_PROG_ID:
				{
					__movie_prog_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_BREAKPLAY_DIALOG_ID:
				{
					__movie_breakplay_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_PLAYERROR_DIALOG_ID:
				{
					__movie_playerror_scene_delete(movie_ctrl);
					break;
				}

				case MOVIE_PLAYEND_DIALOG_ID:
				{
					__movie_playend_scene_delete(movie_ctrl);
					break;
				}

				default:
				{
					__err("unknown child id, child_id=%d\n", child_id);
					return EPDK_FAIL;
				}
			}
		}

		child = next_child;
	}

	return EPDK_OK;
}

static __s32 __movie_delete_scene_berore_switch_tv(movie_ctrl_t *movie_ctrl)
{
	//删除场景和部分资源
	__here__;
	__movie_uninstall_auto_play_timmer(movie_ctrl);
	{
		//save breakpoint
		__s32 index;
		index = robin_npl_get_cur();
		__here__;

		if(-1 != index)
		{
			__here__;
			__movie_save_breakpoint_info(movie_ctrl, index);
		}
	}
	__here__;
	__movie_stop_play(movie_ctrl);
	__here__;
	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
	__here__;
	__movie_ctrl_para_deinit(movie_ctrl);
	robin_close();
	return EPDK_OK;
}

static __s32 __movie_create_scene_after_switch_tv(movie_ctrl_t *movie_ctrl)
{
	//创建部分场景和资源
	__movie_ctrl_para_init(movie_ctrl);
	__app_movie_reg_para_init(movie_ctrl);
	__movie_start_play(movie_ctrl, EPDK_TRUE);

	if(1 == movie_ctrl->sub_state)
	{
		__movie_sub_show_scene_create(movie_ctrl);
	}

	__movie_install_auto_play_timmer(movie_ctrl);
	return EPDK_OK;
}
/*切换输出*/
static __s32 __movie_switch_output(movie_ctrl_t *movie_ctrl)
{
	//切换输出
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			__msg("para->pal_ntsc = %d\n", para->pal_ntsc);

			if(0 == para->pal_ntsc)
			{
				if(para->output == LION_DISP_LCD)
				{
					movie_ctrl->btv_out = 1;
					__here__;
					g_is_on_tv = 1;
					g_display_switch_output(LION_DISP_TV_NTSC);
				}
				else
				{
					movie_ctrl->btv_out = 0;
					__here__;
					g_is_on_tv = 0;
					g_display_switch_output(LION_DISP_LCD);
				}
			}
			else
			{
				if(para->output == LION_DISP_LCD)
				{
					movie_ctrl->btv_out = 1;
					__here__;
					g_is_on_tv = 1;
					g_display_switch_output(LION_DISP_TV_PAL);
				}
				else
				{
					movie_ctrl->btv_out = 0;
					__here__;
					g_is_on_tv = 0;
					g_display_switch_output(LION_DISP_LCD);
				}
			}
		}
	}
	__wrn("movie_ctrl->btv_out=%d\n", movie_ctrl->btv_out);
	return EPDK_OK;
}
/*切换到电视机或者LCD显示屏*/
static __s32 __movie_switch_to_tv_or_lcd(movie_ctrl_t *movie_ctrl)
{
	__movie_delete_scene_berore_switch_tv(movie_ctrl);
	__movie_switch_output(movie_ctrl);
	__movie_create_scene_after_switch_tv(movie_ctrl);
	return EPDK_OK;
}

static __s32 __spsc_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	switch(msg_id)
	{
		case spsc_scene_msg_timeout:
		{
			__s32 ret;
			__msg("spsc_scene_msg_timeout\n");
			__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			{
				//如果处于暂停状态，则创建播放状态子场景
				__cedar_status_t  status;
				status = robin_get_fsm_status();

				if(CEDAR_STAT_PAUSE == status)
				{
					__msg("spsc time out, cedar in pause status, create play/pause scene...\n");
					__movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
				}
			}
			return EPDK_OK;
		}

		case spsc_scene_msg_brightset:
		{
			if(is_on_tv())
			{
				__msg("on tv, not adjust bright scene...\n");
				return EPDK_OK;
			}

			__msg("before spsc_scene_msg_brightset\n");
			__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			__movie_bright_scene_create(movie_ctrl);
			__msg("__movie_bright_scene_create\n");
			__movie_bright_scene_set_focus(movie_ctrl);
			__msg("__movie_bright_scene_set_focus\n");
			__msg("after spsc_scene_msg_brightset\n");
			return EPDK_OK;
		}

		case spsc_scene_msg_volumeset:
		{
			__msg("before spsc_scene_msg_volumeset\n");
			__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			__movie_volume_scene_create(movie_ctrl);
			__msg("__movie_volume_scene_create\n");
			__movie_volume_scene_set_focus(movie_ctrl);
			__msg("__movie_volume_scene_set_focus\n");
			__msg("after spsc_scene_msg_volumeset\n");
			return EPDK_OK;
		}

		case spsc_scene_msg_tvout:
		{
			__msg_jjl("before __movie_switch_to_tv_or_lcd\n");
			__movie_switch_to_tv_or_lcd(movie_ctrl);
			__msg_jjl("after __movie_switch_to_tv_or_lcd\n");
			return EPDK_OK;
		}

		case spsc_scene_msg_rr:
		{
			movie_ctrl->rr_flag = 1;
			return EPDK_OK;
		}

		case spsc_scene_msg_save_breakpoint_info:
		{
			movie_save_breakpoint_info(movie_ctrl);
			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}

static __s32 __volume_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	switch(msg_id)
	{
		case movie_volume_scene_msg_timeout:
		{
			__msg("movie_volume_scene_msg_timeout\n");

			if(movie_ctrl->volume_scene)
			{
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			}

			return EPDK_OK;
		}

		case movie_volume_scene_msg_volumeset:
		{
			__msg("movie_volume_scene_msg_volumeset, dwAddData2=%d\n", dwAddData2);
			dsk_volume_set(dwAddData2);
			return EPDK_OK;
		}

		case movie_volume_scene_msg_enter:
		{
			if(movie_ctrl->volume_scene)
			{
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			}

			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}

static __s32 __bright_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	switch(msg_id)
	{
		case movie_bright_scene_msg_timeout:
		{
			__msg("movie_bright_scene_msg_timeout\n");

			if(movie_ctrl->bright_scene)
			{
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			}

			return EPDK_OK;
		}

		case movie_bright_scene_msg_brightset:
		{
			__msg("movie_bright_scene_msg_brightset, dwAddData2=%d\n", dwAddData2);
			dsk_set_display_global(dwAddData2, DISP_CMD_SET_BRIGHT);
			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}

static __s32 __playsta_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	switch(msg_id)
	{
		case movie_playsta_scene_msg_timeout:
		{
			__s32 ret;
			__msg("movie_playsta_scene_msg_timeout\n");
			ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYSTA_ID);

			if(EPDK_TRUE == ret)
			{
				__here__;
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			}

			return EPDK_OK;
		}

		case movie_playsta_scene_msg_clicked:
		{
			__msg("movie_playsta_scene_msg_playstaset\n");

			if(movie_playsta_play == dwAddData2)
			{
				app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
				robin_set_cmd_play();
			}
			else if(movie_playsta_pause == dwAddData2)
			{
				robin_set_cmd_pause();
				app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
			}
			else
			{
				__err("dwAddData2 invalid...\n");
			}

			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}

static __s32 __sub_set_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	reg_movie_para_t *para;
	para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

	switch(msg_id)
	{
		case sub_set_scene_msg_timeout:
		{
			__s32 ret;
			__msg("sub_set_scene_msg_timeout\n");
			ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SET_ID);

			if(EPDK_TRUE == ret)
			{
				__here__;
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			}

			return EPDK_OK;
		}

		case sub_set_scene_msg_sub_state:
		{
			__msg("sub_set_scene_msg_sub_state\n");

			if(0 == dwAddData2)
			{
				__s32 ret;
				ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);

				if(EPDK_TRUE == ret)
				{
					__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SHOW_ID);
				}
			}
			else if(1 == dwAddData2)
			{
				__s32 ret;
				ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);

				if(EPDK_FALSE == ret)
				{
					__movie_sub_show_scene_create(movie_ctrl);
				}
				else
				{
					__msg("sub_show scene alread on...\n");
				}
			}
			else
			{
				__err("dwAddData2 invalid...\n");
			}

			movie_ctrl->sub_state = dwAddData2;

			if(para)
			{
				para->sub_state = dwAddData2;
			}

			if(movie_ctrl->sub_set_scene)
			{
				movie_sub_set_scene_set_lyr_top(movie_ctrl->sub_set_scene);
			}

			return EPDK_OK;
		}

		case sub_set_scene_msg_sub_color:
		{
			__msg("sub_set_scene_msg_sub_color, dwAddData2=%x\n", dwAddData2);
			movie_ctrl->sub_color = dwAddData2;

			if(1 == movie_ctrl->sub_state)//如果存在字幕场景，则需删除后重键，使得其颜色变化
			{
				__s32 ret;
				ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);

				if(EPDK_TRUE == ret)
				{
					__movie_sub_show_scene_delete(movie_ctrl);
					__movie_sub_show_scene_create(movie_ctrl);
				}
			}

			if(para)
			{
				para->sub_color = dwAddData2;
			}

			if(movie_ctrl->sub_set_scene)
			{
				movie_sub_set_scene_set_lyr_top(movie_ctrl->sub_set_scene);
			}

			return EPDK_OK;
		}

		case sub_set_scene_msg_sub_pos:
		{
			__msg("sub_set_scene_msg_sub_pos, dwAddData2=%d\n", dwAddData2);

			if(movie_ctrl->sub_pos == dwAddData2)
			{
				__msg("sub pos not need change...\n");
				return EPDK_OK;
			}

			movie_ctrl->sub_pos = dwAddData2;

			if(1 == movie_ctrl->sub_state)//如果存在字幕场景，则需删除后重键，使得其颜色变化
			{
				if(dwAddData2 >= 0 || dwAddData2 < 2)
				{
					__s32 ret;
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);

					if(EPDK_TRUE == ret)
					{
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SHOW_ID);
					}

					__movie_sub_show_scene_create(movie_ctrl);
				}
				else
				{
					__err("dwAddData2 invalid...\n");
				}
			}

			if(para)
			{
				para->sub_pos = dwAddData2;
			}

			if(movie_ctrl->sub_set_scene)
			{
				movie_sub_set_scene_set_lyr_top(movie_ctrl->sub_set_scene);
			}

			return EPDK_OK;
		}

		case sub_set_scene_msg_sub_stream:
		{
			__msg("sub_set_scene_msg_sub_stream, dwAddData2=%d\n", dwAddData2);

			if(movie_ctrl->sub_stream == dwAddData2)
			{
				__msg("sub stream not need change...\n");
				return EPDK_OK;
			}

			movie_ctrl->sub_stream = dwAddData2;
			robin_select_subtitle(dwAddData2);

			if(para)
			{
				para->sub_stream = dwAddData2;
			}

			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}

static __s32 __prog_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	switch(msg_id)
	{
		case movie_prog_scene_msg_timeout:
		{
			__s32 ret;
			__msg("movie_prog_scene_msg_timeout\n");
			ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

			if(EPDK_TRUE == ret)
			{
				__here__;
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
			}

			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}

static __s32 __movie_process_after_start_play(movie_ctrl_t *movie_ctrl)
{
	/*{
		//创建字幕
		if(1 == movie_ctrl->sub_state)
		{
			__movie_sub_show_scene_create(movie_ctrl);
		}
	}*/
	__wrn("__movie_process_after_start_play is...\n");
	__movie_install_auto_play_timmer(movie_ctrl);
	//__movie_spsc_scene_create(movie_ctrl);//刚进入视频时，不显示控制菜单
	return EPDK_OK;
}

static __s32 __breakplay_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
	switch(msg_id)
	{
		case ADLG_CMD_EXIT:
		{
			if(GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
			{
				GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id);
			}

			if(ADLG_IDYES == dwAddData2)//断点播放
			{
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
				__movie_start_play(movie_ctrl, EPDK_TRUE);
				__movie_process_after_start_play(movie_ctrl);
				return EPDK_OK;
			}
			else//非断点播放
			{
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
				__movie_start_play(movie_ctrl, EPDK_FALSE);
				__movie_process_after_start_play(movie_ctrl);
				return EPDK_OK;
			}
		}

		default:
		{
			break;
		}
	}

	return EPDK_FAIL;
}
/*保存最后播放的视频文件*/
static __s32 __movie_save_last_playing_file(movie_ctrl_t *movie_ctrl)
{
	__s32 index;
	char file[RAT_MAX_FULL_PATH_LEN] = {0};
	root_para_t *root_para;

	if(NULL == movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);
	index = robin_npl_get_cur();

	if(-1 != index)
	{
		__s32 ret;
		ret = robin_npl_index2file(index, file);

		if(EPDK_OK == ret)
		{
			ret = dsk_reg_save_cur_play_info(REG_APP_MOVIE, index,
			                                 file, root_para->root_type);

			if(EPDK_OK == ret)
			{
				__msg("dsk_reg_save_cur_play_info success,index=%d, file=%s, root=%d\n",
				      index, file, root_para->root_type);
				return EPDK_OK;
			}
			else
			{
				__msg("dsk_reg_save_cur_play_info fail...\n");
				return EPDK_FAIL;
			}
		}
		else
		{
			__msg("robin_npl_index2file fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_FAIL;
}
/*设置播放模式*/
static void __app_movie_set_play_mode(void)
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
static void __app_movie_set_zoom_mode(void)
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

static __u32 __app_movie_get_sub_show_state(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
	{
		__msg("load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__msg("load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		__msg("para->sub_state=%d\n", para->sub_state);
		return para->sub_state;
	}
	else
	{
		return 1;
	}
}

static __u32 __app_movie_get_sub_stream_index(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
	{
		__msg("load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__msg("load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		__msg("para->sub_stream=%d\n", para->sub_stream);
		return para->sub_stream;
	}
	else
	{
		return 1;
	}
}

static __u32 __app_movie_get_sub_show_pos(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
	{
		__msg("load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__msg("load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		__msg("para->sub_pos=%d\n", para->sub_pos);
		return para->sub_pos;
	}
	else
	{
		return 0;
	}
}

static __u32 __app_movie_get_sub_show_color(void)
{
	reg_movie_para_t *para;

	if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
	{
		__msg("load default setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
	}
	else
	{
		__msg("load current setting...\n");
		para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
	}

	if(para)
	{
		__msg("para->sub_color=%x\n", para->sub_color);
		return para->sub_color;
	}
	else
	{
		return GUI_WHITE;
	}
}

static __s32 __app_movie_reg_para_init(movie_ctrl_t *movie_ctrl)
{
	__app_movie_set_play_mode();
	__app_movie_set_zoom_mode();
	__wrn("__app_movie_reg_para_init is...\n");
	/*movie_ctrl->sub_pos   = __app_movie_get_sub_show_pos();
	movie_ctrl->sub_color = __app_movie_get_sub_show_color();
	movie_ctrl->sub_state = __app_movie_get_sub_show_state();
	movie_ctrl->sub_stream = __app_movie_get_sub_stream_index();
	movie_ctrl->sub_size  = 16;*/
	dsk_reg_set_app_restore_flag(REG_APP_MOVIE, 0);
	return EPDK_OK;
}

static void dsk_display_set_FLICKER()
{
	ES_FILE *fp;
	fp = eLIBs_fopen("b:\\disp\\display", "r+");

	if(fp)
	{
		__u32 args[3] = {0};
		args[0] = 1;
		__msg("DISP_CMD_SET_DE_FLICKER, args[0]=%d\n", args[0]);
		eLIBs_fioctrl(fp, DISP_CMD_SET_DE_FLICKER, 0, args);
		eLIBs_fclose(fp);
	}
	else
	{
		__msg("open disp drv fail...\n");
	}
}

//声道模式
static __s32 __app_movie_draw_channel_mode(__s32 channel, H_LYR hlyr, GUI_RECT *gui_rect)
{
	char text[128];
	__wrn("__app_movie_draw_channel_mode...\n");

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
/*视频数字图层绘制*/
static __s32 movie_digit_layer_draw(__gui_msg_t *msg)
{
	__s32 	 i;
	__s32 	 j;
	RECT 	 rect;
	char     text[256];
	GUI_RECT gui_rect;
	__s32 	 total_movie_num;
	movie_ctrl_t *movie_ctrl;
	__s32 X = 0, Y = 0;
	void *pic_buf = NULL;
	H_LYR	*phLyr;
	__wrn("&&&&&&&& movie_digit_layer_draw \n");
	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	phLyr = &movie_ctrl->hdigit_layer;

	if(!movie_ctrl)
	{
		__err("movie_ctrl is null...\n");
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

	total_movie_num = robin_npl_get_total_count();
#if 0

	if(movie_ctrl->ndigit_movie_num > total_movie_num)
	{
		dsk_langres_get_menu_text(STRING_NO_THIS_MOVIE, text, sizeof(text));
	}
	else
#endif
	{
		eLIBs_memset(text, 0, sizeof(text));
		eLIBs_int2str_dec(movie_ctrl->ndigit_movie_num, text);
	}

	GUI_LyrWinSel(*phLyr);
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
#ifdef ID_MOVIE_NUMLOCK_BG_BMP
	{
		pic_buf = movie_get_icon_res(10);

		if(pic_buf != NULL)
		{
			GUI_BMP_Draw(pic_buf, 0, 0);
		}

#if MASK_SysFlags_ShiftNum
		{
			reg_system_para_t *para;
			para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

			if(para)
			{
				if(para->m_maskFlags & MASK_SysFlags_ShiftNum)
				{
					para->m_maskFlags &= ~MASK_SysFlags_ShiftNum;
				}
			}
		}
#endif
	}
#else
	{
		GUI_Clear();
	}
#endif

	if(msg->dwAddData1 != GUI_MSG_KEY_NUMLOCK)
	{
		__wrn("text=%s\n", text);
		i = eLIBs_strlen(text);
		X = (W_NumLock_Bg - (i * W_Num)) / 2;
		Y = H_NumLock_Bg - H_NumLock_Num + (H_NumLock_Num - H_Num) / 2;

		for(i = 0; i < 6; i++)
		{
			if(text[i] == 0)
			{
				break;
			}

			pic_buf = movie_get_icon_res(text[i] - '0');

			if(pic_buf != NULL)
			{
				GUI_BMP_Draw(pic_buf, X, Y);
			}

			X += W_Num;
		}
	}

	GUI_LyrWinSetTop(*phLyr);
	return EPDK_OK;
}
/*显示视频数字*/
void movie_display_digit(__gui_msg_t *msg)
{
	H_LYR	*phLyr;
	movie_ctrl_t *movie_ctrl;
	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	phLyr = &movie_ctrl->hdigit_layer;
	movie_ff_fb_layer_delete(msg);
	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

	if(NULL == *phLyr)
	{
		RECT rect;
		__s32 width, height;
		__s32 scn_width, scn_height;
		dsk_display_get_size(&scn_width, &scn_height);
		width = W_NumLock_Bg;
		height = H_NumLock_Bg;
		rect.x = (scn_width - width);
		rect.y = scn_height - height;
		rect.width = width;
		rect.height = height;
		*phLyr = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_ON, "");

		if(NULL == *phLyr)
		{
			__wrn("digit layer create fail...\n");
		}
		else
		{
			__wrn("digit layer create ok...\n");
		}
	}

	__wrn("movie_ctrl->digit_timmer_id=%d\n", movie_ctrl->digit_timmer_id);

	//__wrn("msg->h_deswin=0x%x\n", msg->h_deswin);
	if(!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->digit_timmer_id))
	{
		__wrn("===========create timmer...\n");
		movie_ctrl->ndigit_movie_num = 0;
		GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->digit_timmer_id, TIMEOUT_ID_TIMER_DIGIT, NULL);
	}
	else
	{
		__wrn("=============reset timmer...\n");
		GUI_ResetTimer(msg->h_deswin, movie_ctrl->digit_timmer_id, TIMEOUT_ID_TIMER_DIGIT, NULL);
	}

	if(msg->dwAddData1 != GUI_MSG_KEY_NUMLOCK)
	{
		__s32 cur_val;
		cur_val = msg->dwAddData1 - GUI_MSG_KEY_NUM0;
		__wrn("cur_val=%d\n", cur_val);

		if(1)
		{
			//if(movie_ctrl->ndigit_movie_num < 100000)
			{
				movie_ctrl->ndigit_movie_num *= 10;
				movie_ctrl->ndigit_movie_num += cur_val;
				movie_ctrl->ndigit_movie_num %= 1000000;	// 低6位有效
			}
		}
		else
		{
			__wrn("=============movie num exceed...\n");
		}
	}

	__wrn("movie_ctrl->ndigit_movie_num=%d\n", movie_ctrl->ndigit_movie_num);
	movie_digit_layer_draw(msg);
}

void movie_ff_fb_over(movie_ctrl_t *movie_ctrl)
{
	if(movie_ctrl->m_hLyr_ff_fb)
	{
		GUI_LyrWinDelete(movie_ctrl->m_hLyr_ff_fb);
		movie_ctrl->m_hLyr_ff_fb = NULL;
#if TIMEROUT_ID_TIMER_HLYR_FF_FB

		if(GUI_IsTimerInstalled(movie_ctrl->movie_manager, ID_TIMER_HLYR_FF_FB))
		{
			GUI_KillTimer(movie_ctrl->movie_manager, ID_TIMER_HLYR_FF_FB);
		}

#endif
	}

	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_PROG_ID);
	movie_ctrl->m_ff_fb_speed = 0;
	movie_ctrl->rr_flag = 0;
}

__s32 movie_proc_ff_fb(__gui_msg_t *msg)
{
	movie_ctrl_t *movie_ctrl;
	__cedar_status_t fsm_sta;
	fsm_sta = robin_get_fsm_status();

	switch(fsm_sta)
	{
		case CEDAR_STAT_PAUSE :
		case CEDAR_STAT_STOP :
			return EPDK_FAIL;

		default :
			break;
	}

	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(msg->dwAddData1 == GUI_MSG_KEY_FF_MULTI)
	{
		if(movie_ctrl->m_ff_fb_speed <= 0)
		{
			movie_ctrl->m_ff_fb_speed = 1;
		}
		else
		{
			movie_ctrl->m_ff_fb_speed++;
		}

		if(movie_ctrl->m_ff_fb_speed > MAX_FF_FB_MULTI)
		{
			movie_ctrl->m_ff_fb_speed = 0;
		}
	}
	else if(msg->dwAddData1 == GUI_MSG_KEY_FB_MULTI)
	{
		if(movie_ctrl->m_ff_fb_speed >= 0)
		{
			movie_ctrl->m_ff_fb_speed = -1;
		}
		else
		{
			movie_ctrl->m_ff_fb_speed--;
		}

		if(movie_ctrl->m_ff_fb_speed < -MAX_FF_FB_MULTI)
		{
			movie_ctrl->m_ff_fb_speed = 0;
		}
	}

	__wrn("movie_ctrl->m_ff_fb_speed=%d\n", movie_ctrl->m_ff_fb_speed);
	{
		const __u16 c_ff_fb_speed_tab[] = {16, 32, 64};

		if(movie_ctrl->m_ff_fb_speed > 0)
		{
			switch(fsm_sta)
			{
				case CEDAR_STAT_RR :
					robin_set_cmd_play();

				case CEDAR_STAT_PLAY :
					movie_ctrl->rr_flag = 0;
					robin_set_cmd_ff();

				default :
					break;
			}

			robin_set_ff_rr_speed(c_ff_fb_speed_tab[movie_ctrl->m_ff_fb_speed]);
		}
		else if(movie_ctrl->m_ff_fb_speed < 0)
		{
			switch(fsm_sta)
			{
				case CEDAR_STAT_FF :
					robin_set_cmd_play();

				case CEDAR_STAT_PLAY :
					movie_ctrl->rr_flag = 1;
					robin_set_cmd_rr();

				default :
					break;
			}

			robin_set_ff_rr_speed(c_ff_fb_speed_tab[-movie_ctrl->m_ff_fb_speed]);
		}
		else
		{
			switch(fsm_sta)
			{
				case CEDAR_STAT_FF :
				case CEDAR_STAT_RR :
					movie_ctrl->rr_flag = 0;
					robin_set_cmd_play();

				default :
					break;
			}
		}
	}
	return EPDK_OK;
}
/*快进图层绘制*/
__s32 movie_ff_fb_layer_draw(__gui_msg_t *msg)
{
	char     text[256];
	movie_ctrl_t *movie_ctrl;
	H_LYR	*phLyr;
	char 	t_str[] = " : X1";
	__wrn("&&&&&&&& movie_ff_fb_layer_draw \n");
	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	phLyr = &movie_ctrl->m_hLyr_ff_fb;

	if(!movie_ctrl)
	{
		__err("movie_ctrl is null...\n");
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

	eLIBs_memset(text, 0, sizeof(text));

	if(movie_ctrl->m_ff_fb_speed > 0)
	{
		dsk_langres_get_menu_text(STRING_MOVIE_FF, text, sizeof(text));
		t_str[4] = '0' + movie_ctrl->m_ff_fb_speed;
		strcat(text, t_str);
	}
	else if(movie_ctrl->m_ff_fb_speed < 0)
	{
		dsk_langres_get_menu_text(STRING_MOVIE_RR, text, sizeof(text));
		t_str[4] = '0' - movie_ctrl->m_ff_fb_speed;
		strcat(text, t_str);
	}
	else
	{
		dsk_langres_get_menu_text(STRING_MOVIE_ST_PLAT, text, sizeof(text));
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
		GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	}
	GUI_LyrWinSetTop(*phLyr);
	return EPDK_OK;
}

void movie_display_ff_fb(__gui_msg_t *msg)
{
	H_LYR	*phLyr;
	movie_ctrl_t *movie_ctrl;

	if(movie_proc_ff_fb(msg)  == EPDK_FAIL)
	{
		return;
	}

	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	phLyr = &movie_ctrl->m_hLyr_ff_fb;
	movie_delete_digit_layer(msg);
	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PROG_ID));

	if(NULL == *phLyr)
	{
		RECT rect;
		__s32 width, height;
		__s32 scn_width, scn_height;
		dsk_display_get_size(&scn_width, &scn_height);
		width = W_NumLock_Bg;
		height = H_NumLock_Num;
		rect.x = (scn_width - width);
		rect.y = scn_height - height;
		rect.width = width;
		rect.height = height;
		*phLyr = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_ON, "hLyr_ff_fb");

		if(NULL == *phLyr)
		{
			__wrn("digit layer create fail...\n");
		}
		else
		{
			__wrn("digit layer create ok...\n");
		}
	}

#if TIMEROUT_ID_TIMER_HLYR_FF_FB

	if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_HLYR_FF_FB))
	{
		GUI_SetTimer(msg->h_deswin, ID_TIMER_HLYR_FF_FB, TIMEROUT_ID_TIMER_HLYR_FF_FB, NULL);
	}
	else
	{
		GUI_ResetTimer(msg->h_deswin, ID_TIMER_HLYR_FF_FB, TIMEROUT_ID_TIMER_HLYR_FF_FB, NULL);
	}

#endif
	movie_ff_fb_layer_draw(msg);

	if(__movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID) == EPDK_FALSE)
	{
		__movie_prog_scene_create(movie_ctrl);
		__movie_prog_scene_set_focus(movie_ctrl);
	}
	else
	{
		movie_prog_scene_update_prog(movie_ctrl->prog_scene);
	}
}
/*快进图层删除*/
__s32 movie_ff_fb_layer_delete(__gui_msg_t *msg)
{
	movie_ctrl_t *movie_ctrl;
	__wrn("&&&&&&&& movie_ff_fb_layer_delete\n");
	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!movie_ctrl)
	{
		__err("movie_ctrl is null...\n");
		return EPDK_OK;
	}

	if(movie_ctrl->m_hLyr_ff_fb)
	{
		GUI_LyrWinDelete(movie_ctrl->m_hLyr_ff_fb);
		movie_ctrl->m_hLyr_ff_fb = NULL;
	}

#if TIMEROUT_ID_TIMER_HLYR_FF_FB

	if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_HLYR_FF_FB))
	{
		GUI_KillTimer(msg->h_deswin, ID_TIMER_HLYR_FF_FB);
	}

#endif
	__wrn("movie_ff_fb_layer_delete end...\n");
	return EPDK_OK;
}
/*通过索引播放*/
static __s32 __movie_play_by_index(movie_ctrl_t *movie_ctrl, __u32 movie_index)
{
	__s32 ret;
	__s32 index;
	__s32 total;
#if SP_FavoriteFiles
	HRAT hrat = NULL;
	HRATNPL hnpl = NULL;
	char filename[1024] = {0};
	root_para_t *root_para;
	__s32 normal_index = 0;

	if(movie_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	if(movie_ctrl->movie_manager == NULL)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(root_para == NULL)
	{
		return EPDK_FAIL;
	}

	if(movie_ctrl->cur_media_type != RAT_MEDIA_TYPE_VIDEO)
	{
		rat_set_modify_flag(RAT_MEDIA_TYPE_FAVOR_VIDEO, 1);
		hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

		if(hrat == NULL)
		{
			//DebugPrintf("EPDK_FAIL\n");
			return EPDK_FAIL;
		}

		total = rat_get_cur_scan_cnt(hrat);
		index = movie_index;

		if(index > total)
		{
			rat_close(hrat);
			return EPDK_FAIL;
		}
		else if(0 == index)
		{
			return EPDK_FAIL; //如果输入第0  个,直接返回
		}
		else
		{
			index --;
		}

		//rat_npl_index2file(hnpl, index, filename);
		rat_get_filenamebyindex(movie_ctrl, index, filename);
#if 0
		__wrn("filename:%s\n", filename);
		index = robin_npl_file2index(filename);

		if(index != -1)
		{
			__movie_play_file(movie_ctrl, index, EPDK_FALSE);
		}

#else
		ret = __movie_favor_play_file(movie_ctrl, filename);
#endif
		rat_close(hrat);
		normal_index = robin_npl_file2index(filename);
		robin_npl_set_cur(normal_index);
	}
	else
#endif
	{
		index = robin_npl_get_cur();

		if(-1 == index)
		{
			__msg("robin_npl_get_cur fail...\n");
			return EPDK_FAIL;
		}

		__movie_delete_breakpoint_info(movie_ctrl, index);
		total = robin_npl_get_total_count();

		if(-1 == total)
		{
			__msg("robin_npl_get_total_count fail...\n");
			return EPDK_FAIL;
		}

		//index++;
		index = movie_index;

		if(index > total)
		{
			//index = 0;
			return EPDK_FAIL;
		}
		else if(0 == index)
		{
			return EPDK_FAIL; //如果输入第0  个,直接返回
		}
		else
		{
			index --;
		}

		robin_npl_set_cur(index);
		ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);
	}

	return ret;
}

/*删除视频数字图层*/
__s32 movie_delete_digit_layer(__gui_msg_t *msg)
{
	movie_ctrl_t *movie_ctrl;
	__wrn("&&&&&&&& movie_delete_digit_layer\n");
	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!movie_ctrl)
	{
		__err("movie_ctrl is null...\n");
		return EPDK_OK;
	}

	if(movie_ctrl->hdigit_layer)
	{
		GUI_LyrWinDelete(movie_ctrl->hdigit_layer);
		movie_ctrl->hdigit_layer = NULL;
#if MASK_SysFlags_ShiftNum
		{
			reg_system_para_t *para;
			para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

			if(para)
			{
				para->m_maskFlags |= MASK_SysFlags_ShiftNum;
			}
		}
#endif
	}

	if(movie_ctrl->digit_timmer_id)
	{
		if(GUI_IsTimerInstalled(msg->h_deswin, movie_ctrl->digit_timmer_id))
		{
			GUI_KillTimer(msg->h_deswin, movie_ctrl->digit_timmer_id);
		}
	}

	movie_ctrl->ndigit_movie_num = 0;
	__wrn("movie_delete_digit_layer end...\n");
	return EPDK_OK;
}

#if SP_FavoriteFiles
static __s32 movie_shift_playlist(__gui_msg_t *msg)
{
	H_WIN    h_dialog = NULL;
	char file_path[RAT_MAX_FULL_PATH_LEN];
	char filename[1024] = {0};
	movie_ctrl_t *movie_ctrl;
	HRAT hrat;
	__s32 ret;
	__s32 index;
	H_WIN hDstWin;
	__s32 count;
	__s32 i, j;
	__s32 normal_index = 0;
	root_para_t *root_para;
	HRATNPL		h_rat_npl;//new+++
	hDstWin = GUI_WinGetParent(msg->h_deswin);

	if(hDstWin == NULL)
	{
		return EPDK_FAIL;
	}

	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(movie_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(root_para == NULL)
	{
		return EPDK_FAIL;
	}

	if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_VIDEO)
	{
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STROE_UP_PLAY};
			//DebugPrintf("-------default_dialog--------\n");
			default_dialog(h_dialog, hDstWin, APP_STORE_UP_PLAY_DLG_ID, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);

			if(h_dialog)
			{
				app_dialog_destroy(h_dialog);
				h_dialog = NULL;
			}
		}
		__wrn("-------start_rat_open_ex--------\n");
		rat_set_modify_flag(RAT_MEDIA_TYPE_FAVOR_VIDEO, 1);
		hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

		if(hrat == NULL)
		{
			//DebugPrintf("EPDK_FAIL\n");
			return EPDK_FAIL;
		}

		count = rat_get_cur_scan_cnt(hrat);

		if(count == 0)
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STORE_UP_PLAY_NULL};
			//DebugPrintf("-------default_dialog--------\n");
			default_dialog(h_dialog, hDstWin, APP_STORE_UP_PLAY_DLG_ID, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);

			if(h_dialog)
			{
				app_dialog_destroy(h_dialog);
				h_dialog = NULL;
			}

			rat_close(hrat);
			return EPDK_FAIL;
		}

		__wrn("count:%d\n", count);
		eLIBs_memset(filename, 0, sizeof(filename));
		index = robin_npl_get_cur();

		if(index != -1)
		{
			ret = robin_npl_index2file(index, filename);

			if(ret == EPDK_FAIL)
			{
				return EPDK_FAIL;
			}
			else
			{
				ret = rat_is_favor_file(hrat, filename);

				if(ret == 1)
				{
					__wrn("ret is 1\n");
				}
				else
				{
					__wrn("ret is 0\n");
					//rat_npl_index2file(h_rat_npl, 0, filename);
					rat_get_filenamebyindex(movie_ctrl, 0, filename);
					__wrn("filename:%s\n", filename);
#if 0
					index = robin_npl_file2index(filename);

					if(index != -1)
					{
						__movie_play_file(movie_ctrl, index, EPDK_FALSE);
					}

#else
					__movie_favor_play_file(movie_ctrl, filename);
					normal_index = robin_npl_file2index(filename);
					robin_npl_set_cur(normal_index);
#endif
				}
			}
		}

		rat_close(hrat);
		__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
		movie_ctrl->cur_media_type = RAT_MEDIA_TYPE_FAVOR_VIDEO;
	}
	else
	{
		//DebugPrintf("-------stro_up_play_flag = 0--------\n");
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STROE_UP_PLAY_CANCEL};
			//DebugPrintf("-------default_dialog--------\n");
			default_dialog(h_dialog, hDstWin, APP_STORE_UP_PLAY_DLG_ID, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);

			if(h_dialog)
			{
				app_dialog_destroy(h_dialog);
				h_dialog = NULL;
			}
		}
		__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
		movie_ctrl->cur_media_type = RAT_MEDIA_TYPE_VIDEO;
	}

	return EPDK_OK;
}


static __s32 movie_favor_get_cur_index(movie_ctrl_t *movie_ctrl)
{
	char filename[1024] = {0};
	__s32 index = 0;
	root_para_t *root_para;
	HRAT hrat = NULL;
	HRATNPL hnpl = NULL;

	if(!movie_ctrl->movie_manager)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(!root_para)
	{
		return EPDK_FAIL;
	}

	hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

	if(hrat == NULL)
	{
		__wrn("EPDK_FAIL\n");
		return EPDK_FAIL;
	}

	hnpl = rat_npl_open(RAT_MEDIA_TYPE_FAVOR_VIDEO);

	if(hnpl == NULL)
	{
		__wrn("EPDK_FAIL\n");
		rat_close(hrat);
		return EPDK_FAIL;
	}

	index = rat_npl_get_cur(hnpl);
	__wrn("index = %d\n", index);
	rat_npl_close(hnpl);
	rat_close(hrat);
	return index;
}
static __s32 movie_favor_auto_paly_next(movie_ctrl_t *movie_ctrl)
{
	__s32 next_index;
	HRAT hrat;
	HRATNPL hnpl;
	__s32 normal_index = 0;
	char filename[1024] = {0};
	__s32 index = 0, ret = 0;
	root_para_t *root_para;

	if(!movie_ctrl)
	{
		return EPDK_FAIL;
	}

	if(!movie_ctrl->movie_manager)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(!root_para)
	{
		return EPDK_FAIL;
	}

	hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

	if(hrat == NULL)
	{
		__wrn("EPDK_FAIL\n");
		return EPDK_FAIL;
	}

	eLIBs_memset(filename, 0, sizeof(filename));
	robin_get_fsm_cur_file(filename , sizeof(filename));
	rat_set_file_for_play(hrat, filename);
	hnpl = rat_npl_open(RAT_MEDIA_TYPE_FAVOR_VIDEO);

	if(hnpl == NULL)
	{
		__wrn("EPDK_FAIL\n");
		rat_close(hrat);
		return EPDK_FAIL;
	}

	rat_npl_set_play_mode(hnpl, RAT_PLAY_MODE_ROTATE_ALL);
	next_index = rat_npl_get_next(hnpl);
	__wrn("********next_index=%d******************\n", next_index);
	//rat_npl_index2file(hnpl,next_index, filename);
	rat_get_filenamebyindex(movie_ctrl, next_index, filename);
#if 0
	index = robin_npl_file2index(filename);

	if(index != -1)
	{
		__movie_play_file(movie_ctrl, index, EPDK_FALSE);
	}

#else
	__movie_favor_play_file(movie_ctrl, filename);
#endif
	rat_npl_close(hnpl);
	rat_close(hrat);
	normal_index = robin_npl_file2index(filename);
	robin_npl_set_cur(normal_index);
	return EPDK_OK;
}

static __s32 movie_favor_play_next(movie_ctrl_t *movie_ctrl, __s32 next_or_prev)
{
	__s32 next_index;
	HRAT hrat;
	HRATNPL hnpl;
	__s32 count = 0;
	char filename[1024] = {0};
	__s32 index = 0, ret = 0;
	__s32 normal_index = 0;
	root_para_t *root_para;

	if(!movie_ctrl->movie_manager)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(!root_para)
	{
		return EPDK_FAIL;
	}

	hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

	if(hrat == NULL)
	{
		__wrn("EPDK_FAIL\n");
		return EPDK_FAIL;
	}

	count = rat_get_cur_scan_cnt(hrat);
	robin_get_fsm_cur_file(filename , sizeof(filename));
	rat_set_file_for_play(hrat, filename);
	hnpl = rat_npl_open(RAT_MEDIA_TYPE_FAVOR_VIDEO);

	if(hnpl == NULL)
	{
		__wrn("EPDK_FAIL\n");
		rat_close(hrat);
		return EPDK_FAIL;
	}

	rat_npl_set_play_mode(hnpl, RAT_PLAY_MODE_ROTATE_ALL);

	if(next_or_prev == 1)
	{
		next_index = rat_npl_get_next(hnpl);
		//next_index ++;
		//if (next_index >= count)
		//{
		//	next_index = 0;
		//}
	}
	else
	{
		//next_index = rat_npl_get_prev(hnpl);//这个接口没有达到目的
		next_index = rat_npl_get_cur(hnpl);
		next_index --;

		if(next_index < 0)
		{
			next_index = count - 1;
		}
	}

	//next_index = rat_npl_get_prev(hnpl);
	__wrn("********next_index=%d******************\n", next_index);
	//rat_npl_index2file(hnpl,next_index, filename);
	rat_get_filenamebyindex(movie_ctrl, next_index, filename);
#if 0
	index = robin_npl_file2index(filename);

	if(index != -1)
	{
		__movie_play_file(movie_ctrl, index, EPDK_FALSE);
	}

#else
	__movie_favor_play_file(movie_ctrl, filename);
#endif
	rat_npl_close(hnpl);
	rat_close(hrat);
	normal_index = robin_npl_file2index(filename);
	robin_npl_set_cur(normal_index);
	return EPDK_OK;
}


static __s32 movie_favoriefile__add_or_del(__gui_msg_t *msg)
{
	movie_ctrl_t *movie_ctrl;
	char file_path[RAT_MAX_FULL_PATH_LEN];
	H_WIN hDstWin;
	char *filename = NULL;
	HRAT hrat;
	root_para_t *root_para;
	__s32 index;
	H_WIN    h_dialog = NULL;
	int ret;
	//rat_entry_t ItemInfo;
	//eLIBs_memset(&ItemInfo,0,sizeof(rat_entry_t));
	hDstWin = GUI_WinGetParent(msg->h_deswin);
	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(movie_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(root_para == NULL)
	{
		return EPDK_FAIL;
	}

	rat_set_modify_flag(RAT_MEDIA_TYPE_FAVOR_VIDEO, 1);
	eLIBs_memset(file_path, 0, sizeof(file_path));
	index = robin_npl_get_cur();

	if(-1 != index)
	{
		__s32 ret;
		ret = robin_npl_index2file(index, file_path);

		if(EPDK_OK == ret)
		{
			filename = file_path;
		}
		else
		{
			return EPDK_FAIL;
		}
	}
	else
	{
		return EPDK_FAIL;
	}

	__wrn("filename=%s\n", file_path);
	filename = file_path;
	{
		hrat = rat_open_ex(root_para->root_type, RAT_MEDIA_TYPE_FAVOR_VIDEO, 0);

		if(hrat == NULL)
		{
			//DebugPrintf("EPDK_FAIL\n");
			return NULL;
		}

		__wrn("FileName=%s\n", filename);
		ret = rat_is_favor_file(hrat, filename);

		if(ret == 1) //已经收藏 则删除收藏
		{
			{
				__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DEL};
				////DebugPrintf("default_dialog\n");
				default_dialog(h_dialog, hDstWin, APP_STORE_UP_DLG_ID, ADLG_OK, lang_id);
				esKRNL_TimeDly(100);

				if(h_dialog)
				{
					app_dialog_destroy(h_dialog);
					h_dialog = NULL;
				}
			}
			{
				__wrn("FileName=%s\n", filename);
				ret = rat_del_favor_file(hrat, filename);

				if(ret == EPDK_FAIL)
				{
					__wrn("EPDK_FAIL\n");
					rat_close(hrat);
					return EPDK_FAIL;
				}

				rat_close(hrat);
				return EPDK_OK;
			}
		}
		else //  还没收藏则向下执行收藏
		{
			{
				__wrn("start_rat_add_favor_file\n");
				{
					__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STROE_UP};
					////DebugPrintf("default_dialog\n");
					default_dialog(h_dialog, hDstWin, APP_STORE_UP_DLG_ID, ADLG_OK, lang_id);
					esKRNL_TimeDly(100);

					if(h_dialog)
					{
						app_dialog_destroy(h_dialog);
						h_dialog = NULL;
					}
				}
				__wrn("filename=%s\n", filename);
				ret = rat_add_favor_file(hrat, filename);
				__wrn("rat_add_favor_file_OVER...\n");

				if(ret == EPDK_FAIL)
				{
					__wrn("EPDK_FAIL\n");
					rat_close(hrat);
					return EPDK_FAIL;
				}

				rat_close(hrat);
			}
			return EPDK_OK;
		}
	}
}

static __s32 movie_set_root_para(movie_ctrl_t *movie_ctrl, __s32 status)
{
	root_para_t *root_para;

	if(!movie_ctrl)
	{
		return EPDK_FAIL;
	}

	if(!movie_ctrl->movie_manager)
	{
		return EPDK_FAIL;
	}

	root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);

	if(!root_para)
	{
		return EPDK_FAIL;
	}

	root_para->enter_media_type[m_eMediaTypeList_video] = status;
	return EPDK_TRUE;
}
#endif
/*视频app应用程序的回调处理函数*/
static __s32 __app_movie_proc(__gui_msg_t *msg)
{
	__wrn("__app_movie_proc: msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			__s32 ret;
			movie_ctrl_t *movie_ctrl;

			__here__;
			g_disable_close_scn();
			SEND_MSG(DSK_MSG_RELEASE_LOGO_BUF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
			SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
			__wrn("SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF)\n");
			com_set_backcolor(CONST_BackColor_Black);
			__here__;
			//__movie_set_title();
			//gscene_hbar_set_state(HBAR_ST_SHOW);
			gscene_hbar_set_state(HBAR_ST_HIDE);
			__here__;
			gscene_bgd_set_state(BGD_STATUS_HIDE);
			__wrn("__app_movie_proc: GUI_MSG_CREATE begin\n");
			movie_ctrl = esMEMS_Balloc(sizeof(movie_ctrl_t));

			if(!movie_ctrl)
			{
				__wrn("mem not enough...\n");
				return EPDK_FAIL;
			}

			__here__;
			__movie_robin_sem_init();
			__here__;
			eLIBs_memset(movie_ctrl, 0, sizeof(movie_ctrl_t));
			__here__;
			GUI_WinSetAddData(msg->h_deswin, (__u32)movie_ctrl);
			__here__;
			movie_ctrl->movie_manager = msg->h_deswin;
			__here__;
/*#if SP_FavoriteFiles
			{
				root_para_t *root_para;
				root_para = (root_para_t *)GUI_WinGetAttr(movie_ctrl->movie_manager);
				__wrn("enter_media_type:%d\n", root_para->enter_media_type);
				movie_ctrl->cur_media_type = root_para->enter_media_type[m_eMediaTypeList_video];
				__wrn("cur_media_type:%d\n", movie_ctrl->cur_media_type);
			}
#endif*/
			__movie_ctrl_para_init(movie_ctrl);
			__here__;
			__app_movie_reg_para_init(movie_ctrl);
			__here__;
			com_set_palette_by_id(ID_MOVIE_PAL_BMP);//设置调色板
			__here__;
			{
				//修改调色板
				//背景色，灰白色，字幕边框颜色
				__u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
				com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
			}
			__here__;
			__wrn("before __movie_cur_file_has_breakpoint\n");
/*#if SP_FavoriteFiles

			if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_FAVOR_VIDEO)
			{
				//ret =  __movie_favor_cur_file_has_breakpoint(movie_ctrl);
				__movie_favor_start_play_file(movie_ctrl);//暂时不设断点播放
			}
			else
#endif*/
			{
				//ret = __movie_cur_file_has_breakpoint();
				__wrn("__movie_cur_file_has_breakpoint=%d\n", ret);

				/*if(1 == MOVIE_ENABLE_BREAKPLAY && EPDK_TRUE == ret)//弹框让用户选择是否断点播放
				{
					//弹框
					__here__;
					ret = __movie_breakplay_scene_create(movie_ctrl);
					__here__;

					if(EPDK_FAIL == ret)
					{
						__wrn("__movie_breakplay_scene_create fail, exit movie...\n");
						__movie_exit_movie(movie_ctrl, 0);
						__here__;
						return EPDK_OK;
					}
				}
				else*///不弹框，默认有断点则断点播放，无断点则从头播放
				{
					__here__;
					__movie_start_play(movie_ctrl, NULL);
					__here__;
					__movie_process_after_start_play(movie_ctrl);
					__here__;
				}
			}
/*
#if MOVIE_ENTER_FLUSH
			__wrn("**********movie flush**************\n");
			//dsk_reg_flush();
			movie_save_breakpoint_info(movie_ctrl);
#endif*/
			__wrn("__app_movie_proc: GUI_MSG_CREATE end\n");
			return EPDK_OK;
		}
#if 1
		case GUI_MSG_KEY:
		{
			__s32 bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
			__s32 ret;
			static __s32 last_key = -1;
			movie_ctrl_t *movie_ctrl;
			__wrn("__app_movie_proc: GUI_MSG_KEY begin\n");
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!movie_ctrl)
			{
				__err("movie_ctrl is null...\n");
				return EPDK_OK;
			}

			{
				//如果有断点播放对话框的子场景，则让按键消息全部往下传
				ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);

				if(EPDK_TRUE == ret)
				{
					if(GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
					{
						GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
						               , 500, NULL);//0.5秒
						               __wrn("__app_movie_proc: GUI_MSG_KEY begin GUI_ResetTimer...\n");
					}

					break;
				}
			}

			bok = -1;

			if(KEY_UP_ACTION == msg->dwAddData2)
			{
				__wrn("last_key=%d\n", last_key);

				if(0)
				{}

#if 0
				else if(/*GUI_MSG_KEY_LONGENTER == last_key
    				    				    					||*/GUI_MSG_KEY_LONGMENU == last_key
				  || GUI_MSG_KEY_SUBMENU == last_key)
					//if( GUI_MSG_KEY_SUBMENU == last_key )
				{
					__s32 ret;
					__here__;
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

					if(EPDK_FALSE == ret)
					{
						__here__;
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						__here__;
						__movie_spsc_scene_create(movie_ctrl);
						__here__;
					}
					else
					{
						__here__;
						//__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SUB_SHOW_ID));
						__here__;
					}

					bok = EPDK_OK;
				}

#endif
				else if(last_key >= GUI_MSG_KEY_NUM0 && last_key <= GUI_MSG_KEY_NUM9)
				{
					movie_display_digit(msg);
					bok = EPDK_OK;
					break;
				}
				//if( GUI_MSG_KEY_LONGVIDEO == last_key )
				else if(GUI_MSG_KEY_LONGENTER == last_key)
				{
					//dsk_display_lcd_off();
					//__wrn("Tarring-----send Force close---------\n");
					//SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
					__movie_switch_to_tv_or_lcd(movie_ctrl);		
					bok = EPDK_OK;
				}
				else	if(GUI_MSG_KEY_TVOUT == last_key)
				{
					__msg("before __movie_switch_to_tv_or_lcd\n");
					__movie_switch_to_tv_or_lcd(movie_ctrl);
					__msg("after __movie_switch_to_tv_or_lcd\n");
				}
				else if(GUI_MSG_KEY_PLAY_PAUSE == last_key
				        || GUI_MSG_KEY_LONGPLAY_PAUSE == last_key)
				{
					__cedar_status_t sta;
					sta = robin_get_fsm_status();

					if(CEDAR_STAT_PLAY == sta)
					{
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						robin_set_cmd_pause();
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
						__movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
					}
					else
					{
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
						robin_set_cmd_play();
						__movie_playsta_scene_create(movie_ctrl, movie_playsta_play);
					}

					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_RR == last_key
				        || GUI_MSG_KEY_LONGRR == last_key)
				{
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

					if(EPDK_TRUE == ret)
					{
						movie_prog_scene_update_prog(movie_ctrl->prog_scene);
					}

					{
						__cedar_status_t fsm_sta;
						fsm_sta = robin_get_fsm_status();

						if(CEDAR_STAT_RR == fsm_sta)
						{
							robin_set_cmd_play();
						}
						else
						{
							__msg("fsm not in ff/rr status, do not play\n");
						}
					}

					__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_FF == last_key
				        || GUI_MSG_KEY_LONGFF == last_key)
				{
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

					if(EPDK_TRUE == ret)
					{
						movie_prog_scene_update_prog(movie_ctrl->prog_scene);
					}

					{
						__cedar_status_t fsm_sta;
						fsm_sta = robin_get_fsm_status();

						if(CEDAR_STAT_FF == fsm_sta)
						{
							robin_set_cmd_play();
						}
						else
						{
							__msg("fsm not in ff/rr status, do not play\n");
						}
					}

					__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_NEXT == last_key
				        || GUI_MSG_KEY_LONGNEXT == last_key)
				{
					__movie_play_next(movie_ctrl);
					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_PREV == last_key
				        || GUI_MSG_KEY_LONGPREV == last_key)
				{
					__movie_play_prev(movie_ctrl);
					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_ENTER == last_key)
				{
					if(movie_ctrl->m_ff_fb_speed)
					{
						switch(robin_get_fsm_status())
						{
							case CEDAR_STAT_FF :
							case CEDAR_STAT_RR :
								robin_set_cmd_play();

							default :
								break;
						}

						movie_ff_fb_over(movie_ctrl);
						bok = EPDK_OK;
					}
					else if(movie_ctrl->hdigit_layer)
					{
						__s32 ndigit_movie_num = movie_ctrl->ndigit_movie_num;
						movie_delete_digit_layer(msg);
						__movie_play_by_index(movie_ctrl, ndigit_movie_num);
						bok = EPDK_OK;
					}
					else
					{
						//如果没有弹出菜单，则按enter暂停或开始播放
						__s32 ret;
						ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

						if(EPDK_FALSE == ret)//如果没有除字幕以外的子场景，则处理暂停、继续播放
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();

							if(CEDAR_STAT_PLAY == fsm_sta)
							{
								__msg("before robin_set_cmd_pause\n");
								robin_set_cmd_pause();
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
								__msg("after robin_set_cmd_pause\n");
								__movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
								bok = EPDK_OK;
							}
							else if(CEDAR_STAT_PAUSE == fsm_sta)
							{
								__msg("before robin_set_cmd_play\n");
								robin_set_cmd_play();
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
								__msg("after robin_set_cmd_play\n");
								bok = EPDK_OK;
							}
						}
					}
				}

#if 0
				else if(0)//(GUI_MSG_KEY_VADD == last_key)
				{
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

					if(EPDK_FALSE == ret) //如果没有spsc场景则创建字幕设置场景(VADD按键有冲突，只能这么做)
					{
						ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SET_ID);

						if(EPDK_FALSE == ret)
						{
							__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
							__movie_sub_set_scene_create(movie_ctrl);
							__movie_sub_set_scene_set_focus(movie_ctrl);
						}
						else
						{
							//__movie_sub_set_scene_delete(movie_ctrl);
						}

						bok = EPDK_OK;
					}
				}
				else if(0) //(GUI_MSG_KEY_LONGVADD == last_key)
				{
					__s32 ret;
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_VOLUME_ID);

					if(EPDK_FALSE == ret)
					{
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						__movie_volume_scene_create(movie_ctrl);
						__msg("__movie_volume_scene_create\n");
						__movie_volume_scene_set_focus(movie_ctrl);
						__msg("__movie_volume_scene_set_focus\n");
						bok = EPDK_OK;
					}
				}

#endif
				else if(GUI_MSG_KEY_ESCAPE == last_key
				        || GUI_MSG_KEY_MENU == last_key
				       )
				{
					__s32 ret;
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL
					                            & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID) & (~MOVIE_PROG_ID));

					if(EPDK_TRUE == ret)//如果有除字幕和playpause以外的子场景，则不退出movie，删除所有子场景
					{
						__cedar_status_t fsm_sta;
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						fsm_sta = robin_get_fsm_status();

						if(CEDAR_STAT_PAUSE == fsm_sta)
						{
							ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYSTA_ID);

							if(EPDK_FALSE == ret)
							{
								robin_set_cmd_play();
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
								__movie_playsta_scene_create(movie_ctrl, movie_playsta_play);  //correct movie_playsta_pause ago
							}
						}
					}
					else//退出movie
					{
						if(is_on_tv())
						{
							__movie_switch_to_tv_or_lcd(movie_ctrl);
						}
						else
						{
							__movie_exit_movie(movie_ctrl, 0);
						}
					}

					bok = EPDK_OK;
				}
				else if(GUI_MSG_KEY_LONGLEFT == last_key
				        || GUI_MSG_KEY_LONGRIGHT == last_key)
				{
					__here__;
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)
					                            & (~MOVIE_PROG_ID));
					__msg("ret=%d\n", ret);

					if(EPDK_FALSE == ret)//如果没有除字幕和进度以外的子场景，则恢复快退快进后的正常播放
					{
#if (!SP_LPressUpForFFRR)
						ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);
						__msg("ret=%d\n", ret);

						if(EPDK_TRUE == ret)
						{
							movie_prog_scene_update_prog(movie_ctrl->prog_scene);
						}

						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();

							if(CEDAR_STAT_RR == fsm_sta
							    || CEDAR_STAT_FF == fsm_sta)
							{
								robin_set_cmd_play();
							}
							else
							{
								__msg("fsm not in ff/rr status, do not play\n");
							}
						}

						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
#endif
						bok = EPDK_OK;
					}
				}
				else if((GUI_MSG_KEY_LEFT == last_key) || (GUI_MSG_KEY_UP == last_key))
				{
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID));

					if(EPDK_FALSE == ret)//如果没有除字幕和playpause以外的子场景，则处理上一个下一个
					{
						__cedar_status_t fsm_sta;
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						fsm_sta = robin_get_fsm_status();

						if(CEDAR_STAT_PLAY == fsm_sta
						    || CEDAR_STAT_PAUSE == fsm_sta
						    || CEDAR_STAT_STOP == fsm_sta)
						{
#if SP_FavoriteFiles

							if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_FAVOR_VIDEO)
							{
								movie_favor_play_next(movie_ctrl, 0);
							}
							else if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_VIDEO)
#endif
							{
								__msg("before __movie_play_prev\n");
								__movie_play_prev(movie_ctrl);
								__msg("after __movie_play_prev\n");
							}

							bok = EPDK_OK;
						}
						else
						{
							__msg("fsm in ff/rr status, do not play prev\n");
						}
					}
				}
				else if((GUI_MSG_KEY_RIGHT == last_key) || (GUI_MSG_KEY_DOWN == last_key))
				{
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID));

					if(EPDK_FALSE == ret)//如果没有除字幕和playpause以外的子场景，则处理上一个下一个
					{
						__cedar_status_t fsm_sta;
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						fsm_sta = robin_get_fsm_status();

						if(CEDAR_STAT_PLAY == fsm_sta
						    || CEDAR_STAT_PAUSE == fsm_sta
						    || CEDAR_STAT_STOP == fsm_sta)
						{
							__msg("before __movie_play_prev\n");
#if SP_FavoriteFiles

							if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_FAVOR_VIDEO)
							{
								movie_favor_play_next(movie_ctrl, 1);
							}
							else if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_VIDEO)
#endif
							{
								__movie_play_next(movie_ctrl);
							}

							__msg("after __movie_play_prev\n");
							bok = EPDK_OK;
						}
						else
						{
							__msg("fsm in ff/rr status, do not play next\n");
						}
					}
				}
			}
			else
			{					__cedar_status_t sta;
					sta = robin_get_fsm_status();
				switch(msg->dwAddData1)
				{
					case GUI_MSG_KEY_REPEATE:
					case GUI_MSG_KEY_LONGMENU:
					case GUI_MSG_KEY_SUBMENU:
					{
						__s32 ret;
						__here__;
					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_pause();
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
						__movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
					}
						ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

						if(EPDK_FALSE == ret)
						{
							__here__;

							if(msg->dwAddData1 == GUI_MSG_KEY_REPEATE)
							{
								movie_spsc_ctrl_set_focus_idx(8);
							}

							__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
							__here__;
							__movie_spsc_scene_create(movie_ctrl);
							__here__;
						}
						else
						{
							__here__;
							//__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SUB_SHOW_ID));
							__here__;

							if(msg->dwAddData1 == GUI_MSG_KEY_REPEATE)
							{
								break;
							}
						}

						bok = EPDK_OK;
						break;
					}

					/*
					case GUI_MSG_KEY_LONGVIDEO:
						{
							//dsk_display_lcd_off();
							__wrn("Tarring-----send Force close---------\n");
							SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
							bok = EPDK_OK;
							break;
						}
					*/
					case GUI_MSG_KEY_LR:
					{
						__s32 width;
						__s32 height;
						__s32 channel;
						RECT rect;
						GUI_RECT gui_rect;
						H_LYR hlyr;
						movie_delete_digit_layer(msg);
						movie_ff_fb_layer_delete(msg);
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						dsk_display_get_size(&width, &height);
						rect.x = width - 16 * 6;
						rect.y = 25;
						rect.width = 16 * 4;
						rect.height = 20;
						hlyr = movie_32bpp_layer_create(&rect, 1);

						if(NULL == hlyr)
						{
							bok = EPDK_OK;
							break;
						}

						gui_rect.x0 = 0;
						gui_rect.y0 = 0;
						gui_rect.x1 = rect.width - 1;
						gui_rect.y1 = rect.height - 1;
						channel = robin_get_channel();
						channel = (channel + 1) % 3;
						__wrn("channel=%d\n", channel);
						robin_set_channel(channel);
						GUI_LyrWinSetSta(hlyr, GUI_LYR_STA_ON);
						GUI_LyrWinSetTop(hlyr);
						__app_movie_draw_channel_mode(channel, hlyr, &gui_rect);
						esKRNL_TimeDly(100);

						if(hlyr)
						{
							GUI_LyrWinDelete(hlyr);
							hlyr = NULL;
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_RR:
					case GUI_MSG_KEY_LEFT:
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();

						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_rr();
							__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
							ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

							if(EPDK_FALSE == ret)
							{
								__movie_prog_scene_create(movie_ctrl);
								__movie_prog_scene_set_focus(movie_ctrl);
							}
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_FF:
					case GUI_MSG_KEY_RIGHT:
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();

						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_ff();
							__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
							ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

							if(EPDK_FALSE == ret)
							{
								__movie_prog_scene_create(movie_ctrl);
								__movie_prog_scene_set_focus(movie_ctrl);
							}
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_LONGLEFT:
					{
						ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)
						                            & (~MOVIE_PROG_ID));

						if(EPDK_FALSE == ret)//如果没有除字幕和进度以外的子场景，则快退快进
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();

							if(CEDAR_STAT_PLAY == fsm_sta)
							{
								__msg("before robin_set_cmd_rr\n");
								robin_set_cmd_rr();
								movie_ctrl->rr_flag = 1;
								movie_ctrl->m_ff_fb_speed = -1;
								__msg("after robin_set_cmd_rr\n");
								ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

								if(EPDK_FALSE == ret)
								{
									__movie_prog_scene_create(movie_ctrl);
									__movie_prog_scene_set_focus(movie_ctrl);
								}
							}
							else
							{
								__msg("fsm not in play status, do not rr\n");
							}

							bok = EPDK_FAIL;//消息往下传给prog scene
						}

						break;
					}

					case GUI_MSG_KEY_LONGRIGHT:
					{
						ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)
						                            & (~MOVIE_PROG_ID));

						if(EPDK_FALSE == ret)//如果没有除字幕和进度以外的子场景，则快退快进
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();

							if(CEDAR_STAT_PLAY == fsm_sta)
							{
								__msg("before robin_set_cmd_ff\n");
								robin_set_cmd_ff();
								movie_ctrl->rr_flag = 0;
								movie_ctrl->m_ff_fb_speed = 1;
								__msg("after robin_set_cmd_ff\n");
								ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

								if(EPDK_FALSE == ret)
								{
									__movie_prog_scene_create(movie_ctrl);
									__movie_prog_scene_set_focus(movie_ctrl);
								}
							}
							else
							{
								__msg("fsm not in play status, do not ff\n");
							}

							bok = EPDK_FAIL;//消息往下传给prog scene
						}

						break;
					}

					case GUI_MSG_KEY_NUMLOCK:
					{
						if((KEY_DOWN_ACTION == msg->dwAddData2) && (NULL == movie_ctrl->hdigit_layer))
						{
							movie_display_digit(msg);
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_FF_MULTI :
					case GUI_MSG_KEY_FB_MULTI :
					{
						if(KEY_DOWN_ACTION == msg->dwAddData2)
						{
							movie_display_ff_fb(msg);
						}

						bok = EPDK_OK;
						break;
					}

#if SP_FavoriteFiles

					case GUI_MSG_KEY_LONGNUM0:
					case GUI_MSG_KEY_FAVORPLAY:
					{
						if(last_key == GUI_MSG_KEY_NUM0 || msg->dwAddData1 == GUI_MSG_KEY_FAVORPLAY)
						{
							ret = movie_shift_playlist(msg);
							__wrn("ret = %d\n", ret);

							if(ret == EPDK_FAIL)
							{
								__msg("movie_shift_playlist():FAIL!");
							}
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_LONGNUM1:
					case GUI_MSG_KEY_FAVORFILE:
					{
						if(last_key == GUI_MSG_KEY_NUM1)
						{
							__s32 index;
							__wrn("cur_media_type:%d\n", movie_ctrl->cur_media_type);

							if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_FAVOR_VIDEO)
							{
								__movie_exit_movie(movie_ctrl, 2);
							}
							else
							{
								movie_favoriefile__add_or_del(msg);
							}
						}

						bok = EPDK_OK;
						break;
					}

#endif

					default:
					{
						break;
					}
				}
			}

			if(KEY_UP_ACTION == msg->dwAddData2)
			{
				last_key = -1;
			}
			else
			{
				last_key = msg->dwAddData1;
			}

			__msg("__app_movie_proc: GUI_MSG_KEY end\n");

			if(EPDK_OK == bok) //如果消息已处理，则返回，否则交给子场景处理
			{
				return EPDK_OK;
			}
			else
			{
				break;
			}
		}
#endif 
		case GUI_MSG_CLOSE:
		{
			if(1 == msg->dwAddData1)//退出到home
			{
				movie_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);//退出movie
			}

#if SP_FavoriteFiles
			else if(0 == msg->dwAddData1) //退出到explorer
			{
				movie_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);//退出movie
			}
			else if(2 == msg->dwAddData1)
			{
				movie_cmd2parent(msg->h_deswin, SWITCH_TO_FAVORPLAY, 0, 0);
			}

#else
			else//退出到explorer
			{
				movie_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);//退出movie
			}

#endif
			return EPDK_OK;
		}

		case GUI_MSG_TOUCH:
		{
			__s32 ret;
			movie_ctrl_t *movie_ctrl;
			__msg("__app_movie_proc: GUI_MSG_TOUCH begin\n");
			ret = -1;
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!movie_ctrl)
			{
				__err("movie_ctrl is null...\n");
				return EPDK_OK;
			}

			{
				//如果有断点播放对话框的子场景，则让触摸消息全部往下传(避免下面的对话框没有set capture)
				ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);

				if(EPDK_TRUE == ret)
				{
					break;
				}
			}

			__msg("__app_movie_proc: GUI_MSG_TOUCH end\n");
			break;
		}
#if 0
		case GUI_MSG_TIMER:
		{
			movie_ctrl_t *movie_ctrl;
			__s32 ret;
			ret = -1;
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(movie_ctrl)
			{
				if(LOWORD(msg->dwAddData1) == movie_ctrl->auto_play_timmer_id)
				{
/*#if MOVIE_ENABLE_BREAKPLAY
					ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);

					if(EPDK_TRUE == ret)
					{
						__gui_msg_t mymsg;
						mymsg.h_deswin = msg->h_deswin;
						mymsg.id = GUI_MSG_COMMAND;
						mymsg.dwAddData1 = MAKELONG(MOVIE_BREAKPLAY_DIALOG_ID, ADLG_CMD_EXIT);
						mymsg.dwAddData2 = ADLG_IDYES;
						GUI_SendMessage(&mymsg);
						ret = EPDK_OK;
					}
					else
#endif*/
					/*{
						__s32 play_sta;
						__msg("before __movie_check_for_play_err\n");
						play_sta = __movie_check_for_play_err(movie_ctrl);
						__msg("after __movie_check_for_play_err\n");

						if(EPDK_OK == play_sta)
						{
							__msg("before __movie_check_for_auto_play\n");
							__movie_check_for_auto_play(movie_ctrl);
							__msg("aftr __movie_check_for_auto_play\n");
						}
						else//如果播放错误，则重新开始检测
						{
							//__movie_reset_auto_play_timmer(movie_ctrl);
							//movie_ctrl->cur_stop_cnt = 0;
						}

						ret = EPDK_OK;
					}*/
				}
				/*else if(LOWORD(msg->dwAddData1) == movie_ctrl->auto_flush_timmer_id)
				{
					__wrn("*********************movie flush*********************\n");
#if 1
					//dsk_reg_flush();
					movie_save_breakpoint_info(movie_ctrl);
#endif
					GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_flush_timmer_id
					               , MOVIE_AUTO_FLUSH_TIME, NULL);
					ret = EPDK_OK;
				}*/
				//==========================================
				/*else if(LOWORD(msg->dwAddData1) == movie_ctrl->digit_timmer_id)  // 30234384
					//else if(msg->dwAddData1 == this->digit_timmer_id )
				{
					__wrn("timmer has come...\n");
					__wrn("this->ndigit_movie_num=%d\n", movie_ctrl->ndigit_movie_num);
					__movie_play_by_index(movie_ctrl, movie_ctrl->ndigit_movie_num);
					movie_delete_digit_layer(msg);
					ret = EPDK_OK;
				}*/
/*
#if TIMEROUT_ID_TIMER_HLYR_FF_FB
				else if(LOWORD(msg->dwAddData1) == ID_TIMER_HLYR_FF_FB)
				{
					movie_ff_fb_layer_delete(msg);

					if(!movie_ctrl->m_ff_fb_speed)
					{
						__movie_delete_subscene_by_id(movie_ctrl, MOVIE_PROG_ID);
					}

					ret = EPDK_OK;
				}

#endif*/
				//==========================================
			}
			else
			{
				__err("movie_ctrl is null...\n");
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
		case GUI_MSG_DESTROY:
		{
			movie_ctrl_t *movie_ctrl;
			__msg("__app_movie_proc: GUI_MSG_DESTROY begin\n");
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(movie_ctrl)
			{
				if(is_on_tv())
				{
					__movie_switch_to_tv_or_lcd(movie_ctrl);
				}

#if SP_FavoriteFiles

				if(movie_ctrl->cur_media_type == RAT_MEDIA_TYPE_VIDEO)
				{
					movie_set_root_para(movie_ctrl, RAT_MEDIA_TYPE_VIDEO);
				}
				else
				{
					movie_set_root_para(movie_ctrl, RAT_MEDIA_TYPE_FAVOR_VIDEO);
				}

#endif
				__here__;
				__movie_uninstall_auto_play_timmer(movie_ctrl);
				{
					//save breakpoint
					__s32 index;
					index = robin_npl_get_cur();
					__here__;

					if(-1 != index)
					{
						__here__;
						__movie_save_breakpoint_info(movie_ctrl, index);
					}
				}
				__movie_save_last_playing_file(movie_ctrl);
				{
					//save last playing file
					__s32 ret;
					//ret = __movie_save_last_playing_file(msg);
					ret = __movie_save_last_playing_file(movie_ctrl);

					if(EPDK_OK == ret)
					{
						__msg("__movie_save_last_playing_file success...\n");
					}
					else
					{
						__msg("__movie_save_last_playing_file fail...\n");
					}
				}
				__here__;
				__movie_stop_play(movie_ctrl);
				__here__;
				__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
				movie_delete_digit_layer(msg);// 30234383
				movie_ff_fb_layer_delete(msg);
				__here__;
				__movie_ctrl_para_deinit(movie_ctrl);
				eLIBs_memset(movie_ctrl, 0, sizeof(movie_ctrl_t));
				esMEMS_Bfree(movie_ctrl, sizeof(movie_ctrl_t));
				__here__;
			}

			g_display_switch_output(LION_DISP_LCD);//movie 退出时恢复到小机显示
			g_enable_close_scn();
			robin_close();
			__movie_robin_sem_deinit();
			//dsk_reg_flush();
			//gscene_bgd_set_state(BGD_STATUS_SHOW);
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
			com_set_backcolor(CONST_BackColor);
			SEND_MSG(DSK_MSG_SCREENOFF_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
			__msg("__app_movie_proc: GUI_MSG_DESTROY end\n");
			return EPDK_OK;
		}
/*
		case GUI_MSG_COMMAND:
		{
			__s32 ret;
			movie_ctrl_t *movie_ctrl;
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__msg("__app_movie_proc: GUI_MSG_COMMAND begin\n");

			if(movie_ctrl)
			{
				switch(LOWORD(msg->dwAddData1))
				{
					case MOVIE_SPSC_ID:
					{
						ret = __spsc_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					case MOVIE_VOLUME_ID:
					{
						ret = __volume_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					case MOVIE_BRIGHT_ID:
					{
						ret = __bright_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					case MOVIE_PLAYSTA_ID:
					{
						ret = __playsta_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					case MOVIE_SUB_SHOW_ID:
					{
						break;
					}

					case MOVIE_SUB_SET_ID:
					{
						ret = __sub_set_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					case MOVIE_PROG_ID:
					{
						ret = __prog_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					case MOVIE_BREAKPLAY_DIALOG_ID:
					{
						ret = __breakplay_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
						break;
					}

					default:
					{
						break;
					}
				}
			}
			else
			{
				__msg("movie_ctrl is null...\n");
			}

			__msg("__app_movie_proc: GUI_MSG_COMMAND end\n");
			return EPDK_OK;
		}

		case DSK_MSG_TVDAC_PLUGOUT:
		case DSK_MSG_HDMI_PLUGOUT:
		{
			movie_ctrl_t *movie_ctrl;
			__msg("************DSK_MSG_TVDAC_PLUGOUT/DSK_MSG_HDMI_PLUGOUT*******\n");
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!movie_ctrl)
			{
				__err("movie_ctrl is null...\n");
				return EPDK_OK;
			}

			__msg("before __movie_switch_to_tv_or_lcd\n");
			__movie_switch_to_tv_or_lcd(movie_ctrl);
			__msg("after __movie_switch_to_tv_or_lcd\n");
			return EPDK_OK;
		}

		case DSK_MSG_FS_PART_PLUGOUT:
		{
			__bool to_exist;//用于记录是否退出
			__s32 ret;
			char diskname[2] = {0};
			char cur_play_file[RAT_MAX_FULL_PATH_LEN] = {0};
			movie_ctrl_t *movie_ctrl;
			__msg("************DSK_MSG_FS_PART_PLUGOUT*******\n");
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!movie_ctrl)
			{
				__msg("movie_ctrl is null...\n");
				return EPDK_OK;
			}

			diskname[0] = (char)(msg->dwAddData2);
			diskname[1] = 0;
			to_exist = 1;//默认需要退出
			ret = robin_npl_get_cur();

			if(-1 != ret)
			{
				ret = robin_npl_index2file(ret, cur_play_file);

				if(EPDK_OK == ret)
				{
					if(0 != eLIBs_strnicmp(cur_play_file, diskname, 1))
					{
						to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
					}
				}
			}

			if(1 == to_exist)//需要退出
			{
				movie_ctrl->quit_by_fs_part_plug_out = EPDK_TRUE;

				if(is_on_tv())
				{
					__movie_switch_to_tv_or_lcd(movie_ctrl);
				}

				__movie_exit_movie(movie_ctrl, 1);
			}

			return EPDK_OK;
		}

		case DSK_MSG_HOME:
		{
			movie_ctrl_t *movie_ctrl;
			__msg("************DSK_MSG_HOME*******\n");
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!movie_ctrl)
			{
				__msg("movie_ctrl is null...\n");
				return EPDK_OK;
			}

			if(is_on_tv())
			{
				__movie_switch_to_tv_or_lcd(movie_ctrl);
			}

			__movie_exit_movie(movie_ctrl, 1);
			return EPDK_OK;
		}

#if SP_VideoPauseWhenScreenClose

		case DSK_MSG_SCREEN_CLOSE :
		case DSK_MSG_SCREEN_FORCE_CLOSE :
		{
			movie_ctrl_t *movie_ctrl;
			movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			{
				__cedar_status_t fsm_sta;
				fsm_sta = robin_get_fsm_status();

				if(CEDAR_STAT_PLAY == fsm_sta)
				{
					__msg("before robin_set_cmd_pause\n");
					robin_set_cmd_pause();
					app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
					__msg("after robin_set_cmd_pause\n");
					__movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
				}
			}
			return EPDK_OK;
		}

#endif
*/
		default:
		{
			__msg("__app_movie_proc: default\n");
			break;
		}
	}

	return GUI_ManWinDefaultProc(msg);
}
/*视频app应用程序manwin窗口创建*/
H_WIN app_movie_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	H_WIN hManWin;
	__inf("****************************************************************************************\n");
	__inf("********  enter movie app  **************\n");
	__inf("****************************************************************************************\n");
	esKSRV_SysInfo();
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_MOVIE;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_movie_proc);
	create_info.attr            = (void *)para;
	create_info.id				= APP_MOVIE_ID;
	create_info.hHosting        = NULL;
	hManWin = GUI_ManWinCreate(&create_info);
	__wrn("app_movie_create, hManWin=%x\n", hManWin);

	/*if(hManWin)
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
	}*/

	return hManWin;
}

__s32 app_movie_notify_delete_sub_scene(H_WIN hmanwin)
{
	movie_ctrl_t *movie_ctrl;

	if(!hmanwin)
	{
		__err("invalid para...\n");
	}

	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(hmanwin);

	if(!movie_ctrl)
	{
		__wrn("movie_ctrl is null...\n");
		return EPDK_FAIL;
	}

#if SP_APP_ALARM

	if(movie_ctrl->h_frm_alarm_coming)
	{
		app_alarm_frm_destory(movie_ctrl->h_frm_alarm_coming);
		movie_ctrl->h_frm_alarm_coming = NULL ;
	}

#endif
	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
	//    gscene_hbar_set_state(HBAR_ST_HIDE);
	return EPDK_OK;
}

__s32 app_movie_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
	movie_ctrl_t *movie_ctrl;

	if(!hmanwin)
	{
		__err("invalid para...\n");
	}

	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(hmanwin);

	if(!movie_ctrl)
	{
		__wrn("movie_ctrl is null...\n");
		return EPDK_FAIL;
	}

#if SP_APP_ALARM

	if(movie_ctrl->h_frm_alarm_coming)
	{
		app_alarm_frm_destory(movie_ctrl->h_frm_alarm_coming);
		movie_ctrl->h_frm_alarm_coming = NULL ;
	}

#endif
	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&
	                              (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID) & (~MOVIE_SPSC_ID));
	return EPDK_OK;
}

__s32 app_movie_is_tv_out(H_WIN hmanwin)
{
#if 1
	return g_is_on_tv;
#else
	movie_ctrl_t *movie_ctrl;

	if(NULL == hmanwin)
	{
		__err("invalid para...\n");
		return EPDK_FALSE;
	}

	movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(hmanwin);

	if(!movie_ctrl)
	{
		__wrn("movie_ctrl is null...\n");
		return EPDK_FALSE;
	}

	__wrn("movie_ctrl->btv_out=%d\n", movie_ctrl->btv_out);
	return movie_ctrl->btv_out;
#endif
}



