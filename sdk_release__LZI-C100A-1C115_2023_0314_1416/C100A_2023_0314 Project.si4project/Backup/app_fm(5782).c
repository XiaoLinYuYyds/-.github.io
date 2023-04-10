/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: app_fm.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/07/24 22:51  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "app_fm_i.h"
#include "fm_sset.h"

static fm_icon_data_t 		fm_icon_table[] =
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

/************************************************************************/
//
/************************************************************************/
H_WIN g_FMManWnd = NULL;

//////////////////////////////////////////////////////////////////////////
// 建立图层
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)

//////////////////////////////////////////////////////////////////////////
static __s32 _fm_op_notify(__s32 op_function);
#define fm_op_notify(_fun)    _fm_op_notify((__s32)_fun)

__s32 g_is_searching = 0;

//////////////////////////////////////////////////////////////////////////
//当前找到一个有效频率
static void on_fm_test_freq_ok(FM_CTRLDATA_T *fm_ctrl)
{
	if(fm_ctrl->search_mode == SRHMODE_MANUAL)//手动搜索
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//by cky
		__wrn("draw:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
	}
	else//SRHMODE_AUTO  自动搜索
	{
		if(dsk_radio_rcv_get_search_result_vn() != 0)
		{
			dsk_radio_chan_t chan_info;
			fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();
			fm_ctrl->channel_id = fm_ctrl->channel_count - 1;
			__wrn("@fm_ctrl->seach_channel_count = %d\n", fm_ctrl->channel_count);
			__wrn("@fm_ctrl->seach_channel_id = %d\n", fm_ctrl->channel_id);
			dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
			__wrn("@fm_ctrl->channel_id = %d\n", fm_ctrl->channel_id);
			fm_ctrl->cur_freq = chan_info.freq;
			__wrn("@:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
		}
	}
}

//当前找到的是无效频率
static void on_fm_test_freq_fail(FM_CTRLDATA_T *fm_ctrl)
{
	if(fm_ctrl->search_mode == SRHMODE_MANUAL)//手动搜索
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
		__wrn("draw:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
	}
	else//SRHMODE_AUTO  自动搜索
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
		__wrn("fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
	}
}


//搜索结束
static void on_fm_test_freq_end(FM_CTRLDATA_T *fm_ctrl)
{
	dsk_radio_chan_t chan_info;

	if(fm_ctrl->search_mode == SRHMODE_MANUAL)//手动搜索
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq); //by cky modify
		//dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
		//fm_ctrl->cur_freq = chan_info.freq;
		__wrn("fm_ctrl->channel_id = %d,fm_ctrl->cur_freq = %d\n", fm_ctrl->channel_id, fm_ctrl->cur_freq);
	}
	else//SRHMODE_AUTO  自动搜索
	{
		fm_ctrl->channel_id = 0;

		if(dsk_radio_rcv_get_search_result_vn() == 0)//no channel info
		{
			fm_ctrl->cur_freq = fm_ctrl->cur_min_freq;
			fm_ctrl->channel_count = 1;
			__wrn("find 0 channel, select min-freq %d as channel 1\n", fm_ctrl->cur_freq);
		}
		else
		{
			dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
			fm_ctrl->cur_freq = chan_info.freq;
			fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();
			__wrn("find %d channel(s), select %d as channel 1\n", fm_ctrl->channel_count, fm_ctrl->cur_freq);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//更新列表
	{
		reg_fm_para_t *para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
		dsk_radio_rcv_save_search_result(); //save to reg
		fm_reg_set_channel_count(para, fm_ctrl->channel_count);
		//Tarring
		dsk_reg_flush();
#ifdef __MSG
		{
			//for debug
			__s32 i, num = 0;

			for(i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				__msg("FM_channel[%d]:%d\n", i, para->FM_channel[i]);

				if(0 != para->FM_channel[i])
				{
					num++;
				}
			}

			__msg("valid_num = %d, sel channel_id:%d\n", num, para->channel_id);
		}
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	fm_ctrl->search_mode = SRHMODE_IDLE;
}

//////////////////////////////////////////////////////////////////////////
#define FM_CB_NEED_NOTIFY_FRW(_p) (_p && _p->h_main_lyr && GUI_LyrWinGetSta(_p->h_main_lyr) == GUI_LYRWIN_STA_ON)
//搜索成功回调处理函数
__s32 cb_srhch_success(void *arg_p)
{
	FM_WND_T *wnd_para;
	H_WIN hwnd = (H_WIN)arg_p;
	FM_GetWndPara(wnd_para, FM_WND_T, hwnd);
	__wrn("*******cb_srhch_success********\n");

	if(wnd_para)
	{
		on_fm_test_freq_ok(&wnd_para->fm_ctrl);
		__wrn("draw:fm_ctrl->cur_freq = %d\n", wnd_para->fm_ctrl.cur_freq);
	}

	if(FM_CB_NEED_NOTIFY_FRW(wnd_para))
	{
		NOTIFY_MSG(CMD_AUTOSRH_FINDCH, hwnd, wnd_para->h_main_wnd, 0, 0, 0);
	}

#if SP_FM_WhenAutoSearch_PlayFreq

	if(GUI_IsTimerInstalled(hwnd, ID_TIMER_FM_TestPlayFreq) == EPDK_TRUE)
	{
		GUI_ResetTimer(hwnd, ID_TIMER_FM_TestPlayFreq, ID_TIMER_FM_TestPlayFreq_Speed, 0);
	}
	else
	{
		GUI_SetTimer(hwnd, ID_TIMER_FM_TestPlayFreq, ID_TIMER_FM_TestPlayFreq_Speed, 0);
	}

	g_is_searching = 2;
	dsk_amplifier_onoff(1);
#endif
	return 0;
}
//搜索失败回调处理函数
__s32 cb_srhch_fail(void *arg_p)
{
	FM_WND_T *wnd_para;
	H_WIN hwnd = (H_WIN)arg_p;
	FM_GetWndPara(wnd_para, FM_WND_T, hwnd);
	__wrn("*******cb_srhch_fail********\n");

	if(wnd_para)
	{
		on_fm_test_freq_fail(&wnd_para->fm_ctrl);
		__wrn("draw:fm_ctrl->cur_freq = %d\n", wnd_para->fm_ctrl.cur_freq);
	}

	if(FM_CB_NEED_NOTIFY_FRW(wnd_para))
	{
		NOTIFY_MSG(CMD_AUTOSRH_FINDCHFAIL, hwnd, wnd_para->h_main_wnd, 0, 0, 0);
	}

	return 0;
}
//搜索超时回调处理函数
__s32 cb_srhch_over(void *arg_p)
{
	FM_WND_T *wnd_para;
	H_WIN hwnd = (H_WIN)arg_p;
	__s32 auto_close_scn_time;
	FM_GetWndPara(wnd_para, FM_WND_T, hwnd);
	__wrn("cb_srhch_over():begin\n");
#if SP_FM_WhenAutoSearch_PlayFreq

	if(GUI_IsTimerInstalled(hwnd, ID_TIMER_FM_TestPlayFreq) == EPDK_TRUE)
	{
		GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq);
		__wrn("GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq);\n");
	}

#endif
#if TIME_FMAutoCloseScreen
	g_set_close_scn_time(TIME_FMAutoCloseScreen);
#else
	auto_close_scn_time = get_auto_close_scn_time_fm();
	__wrn("auto_close_scn_time=%d\n", auto_close_scn_time);

	if(auto_close_scn_time > 0)//搜索完毕重新设置FM的自动关屏时间
	{
		g_set_close_scn_time(auto_close_scn_time);
	}
	else
	{
		//恢复系统自动关屏时间
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			g_set_close_scn_time(para->backlightoff);
			__msg("para->backlightoff=%d\n", para->backlightoff);
		}
	}

#endif

	if(wnd_para)
	{
		on_fm_test_freq_end(&wnd_para->fm_ctrl);
	}

	if(FM_CB_NEED_NOTIFY_FRW(wnd_para))
	{
		NOTIFY_MSG(CMD_AUTOSRH_OVER, hwnd, wnd_para->h_main_wnd, 0, 0, 0);
	}

	SEND_MSG(DSK_MSG_SCREEN_OPEN, hwnd, GUI_WinGetHandFromName("init"), 0, 0, 0);
	g_is_searching = 0;
	__wrn("cb_srhch_over():end\n");
	return 0;
}

/************************************************************************/
//通知关闭fm
/************************************************************************/
__s32 notify_to_close_fm(H_WIN hwnd, __s32 id, __s32 para)
{
	__gui_msg_t msg;
	msg.h_deswin   = GUI_WinGetParent(FM_GetManWnd());
	msg.h_srcwin   = hwnd;
	msg.id         = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwnd), id);
	msg.dwAddData2 = para;
	msg.dwReserved = 0;
	GUI_SendNotifyMessage(&msg);
	return EPDK_OK;
}
//设置频带
static __s32 fm_set_band(FM_CTRLDATA_T *fm_ctrl, dsk_radio_band_mode_t mode)
{
	dsk_radio_set_band(mode);
	dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);
	dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);
	__msg("freq range = [%d, %d]\n", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
	return EPDK_OK;
}

#define MASK_CheckDisk_SD 	(1<<0)
#define MASK_CheckDisk_UD 	(1<<1)
//存在外部磁盘
__bool __app_fm_has_external_disk(__u32 type)
{
	__s32 ret;
	char diskname[RAT_MAX_PARTITION][4];

	if(type & MASK_CheckDisk_UD)
	{
		ret = rat_get_partition_name(RAT_USB_DISK, diskname);

		if(EPDK_OK == ret)
		{
			if(diskname[0][0])
			{
				return EPDK_TRUE;
			}
		}
	}

	if(type & MASK_CheckDisk_SD)
	{
		ret = rat_get_partition_name(RAT_SD_CARD, diskname);

		if(EPDK_OK == ret)
		{
			if(diskname[0][0])
			{
				return EPDK_TRUE;
			}
		}
	}

	return EPDK_FALSE;
}

/************************************************************************/
// 进入录音
/************************************************************************/
static __s32 fm_call_record_app(H_WIN hwnd, __s32 root_type)
{
	H_WIN dlg;
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

	if(wnd_para)
	{
		if(__app_fm_has_external_disk(MASK_CheckDisk_SD | MASK_CheckDisk_UD) == EPDK_FALSE)
		{
			__s32 lang_id[] = {STRING_HOME_CUE, STRING_HOME_NULL_DISK};

			if(wnd_para->h_sset)
			{
				SSET_Destroy(wnd_para->h_sset);
				wnd_para->h_sset = NULL;

				if(wnd_para->sset_create_para)
				{
					FM_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
					wnd_para->sset_create_para = NULL;
				}

				__msg("sset destroyed \n");
			}

			gscene_hbar_set_state(HBAR_ST_HIDE);
			default_dialog(dlg, hwnd, 0x2, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);
			app_dialog_destroy(dlg);
		}
		else
		{
			wnd_para->fm_ctrl.is_record = EPDK_TRUE;

			if(__app_fm_has_external_disk(MASK_CheckDisk_SD) == EPDK_TRUE)
			{
				notify_to_close_fm(hwnd, SWITCH_TO_OTHER_APP, MAKELONG(FM_SW_TO_RECORD, RAT_TF));
			}
			else if(__app_fm_has_external_disk(MASK_CheckDisk_UD) == EPDK_TRUE)
			{
				notify_to_close_fm(hwnd, SWITCH_TO_OTHER_APP, MAKELONG(FM_SW_TO_RECORD, RAT_USB));
			}
		}
	}

	return EPDK_OK;
}

/************************************************************************/
// 选择某个频道
//因为增加了退出一项，所以sel_id要减1
/************************************************************************/
static __s32 fm_change_channelno(FM_CTRLDATA_T *fm_ctrl, __s32 sel_id)
{
	dsk_radio_chan_t chan_info;

	if(dsk_radio_rcv_get_search_result_vn() != fm_ctrl->channel_count)
	{
		__wrn(" somewhere error,   check\n");
	}

	__wrn("  channel_count = %d,   channel_id = %d, sel %d\n",
	      fm_ctrl->channel_count, fm_ctrl->channel_id, sel_id);
	/*
		if(fm_ctrl->channel_count <= 1)
		{
			__wrn("  fm_ctrl->channel_count <= 1\n");
			return EPDK_OK;
		}
	*/
	dsk_radio_rcv_get_search_result_chaninfo(sel_id - 1, &chan_info);

	if((fm_ctrl->channel_id == sel_id - 1 && chan_info.freq == fm_ctrl->cur_freq)
	    || sel_id > fm_ctrl->channel_count)
	{
		return EPDK_OK;
	}

	if(0 == sel_id)
	{
		app_fm_notify_delete_sub_scene(g_FMManWnd);
		return EPDK_OK;
	}
	else
	{
		//dsk_radio_chan_t chan_info;
		reg_fm_para_t *para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
		//dsk_radio_rcv_get_search_result_chaninfo(sel_id, &chan_info);
		//dsk_radio_rcv_get_search_result_chaninfo(sel_id -1, &chan_info);
		__wrn(" sel freq = %d\n", chan_info.freq);
		fm_ctrl->cur_freq = chan_info.freq;

		if(dsk_radio_rcv_set_freq_play(chan_info.freq) == EPDK_OK)
		{
			fm_ctrl->channel_id = sel_id - 1;
			fm_reg_set_sel_channel_id(para, sel_id - 1);
		}

		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//just for mistake
		__wrn(" get sel freq = %d\n", fm_ctrl->cur_freq);
		//refresh ui:
		fm_op_notify(fm_change_channelno);
		__wrn("  fm_change_channelno  return\n");
	}

	return EPDK_OK;
}

//手动搜索
static __s32 fm_manual_search_start(FM_CTRLDATA_T *fm_ctrl, __s32 way)
{
	__u32 search_flag;
	dsk_radio_get_search_flag(&search_flag) ;

	if(1 == search_flag) //正在搜索过程中不允许中断，必须得搜索完后才能进行
	{
		__wrn("fm_manual_search_start:return\n");
		return EPDK_OK;
	}
	else
	{
		__wrn("fm_manual_search_start:\n");
		fm_ctrl->search_mode = SRHMODE_MANUAL;
		__wrn("FM::way = %d\n", way); //0:right;1:left
		dsk_radio_set_manual_way(way);
		dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_MANUAL);
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
		__wrn(" fm_manual:fm_ctrl->cur_freq = %d, fm_ctrl->channel_id + 1=%d\n", fm_ctrl->cur_freq, fm_ctrl->channel_id + 1);
		dsk_radio_rcv_search_all(fm_ctrl->cur_freq, fm_ctrl->channel_id + 1); //TODO:fm_ctrl->channel_id+1 ?
		return EPDK_OK;
	}
}

//自动搜索
static __s32 fm_auto_search_start(FM_CTRLDATA_T *fm_ctrl)
{
	__u32 search_flag;
	g_is_searching = 1;
	//g_set_close_scn_time(1);
	dsk_radio_get_search_flag(&search_flag) ;

	if(1 == search_flag) //正在搜索过程中不允许中断，必须得搜索完后才能进行
	{
		__wrn("fm_auto_search_start:return\n");
		return EPDK_OK;
	}
	else
	{
		__wrn("fm_auto_search_start:\n");
		fm_ctrl->search_mode = SRHMODE_AUTO;
		dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_AUTO);
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
		__wrn("fm_auto_search_start:");
		dsk_radio_rcv_search_all(0, 0);
		esKRNL_TimeDly(5);
		SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
		return EPDK_OK;
	}
}

//初始化相关数据
static __s32 fm_init_data(FM_CTRLDATA_T *fm_ctrl)
{
	fm_ctrl->search_mode = SRHMODE_IDLE;
	fm_ctrl->thumb_show = FM_SHOW_THUMB_N;
	__wrn("init module#################################\n");

	if(dsk_radio_is_open())
	{
		dsk_radio_chan_t chan_info;
		reg_fm_para_t *para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
		fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();
		__wrn("fm_ctrl->channel_count = %d\n", fm_ctrl->channel_count);

		if(fm_ctrl->channel_count <= 0)//no channel info
		{
			__here__;
			fm_ctrl->channel_id = 0;
			fm_ctrl->cur_freq = MIN_CHANNEL_FREQ;
			fm_ctrl->channel_count = 0;
			//初始化reg_fm_para_t
			ZeroMemory(para, sizeof(reg_fm_para_t));
			fm_reg_set_sel_channel_id(para, 0);
			// 			fm_reg_set_channel_(para, 0, fm_ctrl->cur_freq); //设置默认一个频道频率
			// 			fm_reg_set_channel_count(para, fm_ctrl->channel_count);
			dsk_radio_rcv_get_search_result(para);
			
			{
				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);
				//esKRNL_TimeDly(50);
				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
				//esKRNL_TimeDly(50);
			}
			
			dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
		}
		else
		{
			__here__;

			if(ValueBetweenRange(MIN_CHANNEL_FREQ, fm_reg_get_curfreq(para), MAX_CHANNEL_FREQ)
			    && fm_reg_get_channel_count(para) == 1 && fm_ctrl->channel_count == 1
			    && fm_reg_get_channel_(para, 0) == MIN_CHANNEL_FREQ)
			{
				//曾经有手动设置频率(没搜索过)
				fm_ctrl->cur_freq = fm_reg_get_curfreq(para);
				__wrn("fm_init:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
				__wrn("fm_init:fm_ctrl->channel_count  = %d\n", fm_ctrl->channel_count);

				if((fm_ctrl->cur_freq < MIN_CHANNEL_FREQ) && (fm_ctrl->cur_freq > MIN_CHANNEL_FREQ))
				{
					return EPDK_FAIL;
				}
				else
				{
					//if(87500 == MIN_CHANNEL_FREQ) //只针对4702，FM play3次声音才最清晰
					{
						dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);
						dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
					}

					dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
				}
			}

			//			else
			{
				if(ValueInRange(0, para->channel_id, fm_ctrl->channel_count))
				{
					fm_ctrl->channel_id = fm_reg_get_sel_channel_id(para);
					__wrn("fm_ctrl->channel_id  = %d\n", fm_ctrl->channel_id);
				}
				else
				{
					fm_ctrl->channel_id = 0;
					fm_reg_set_sel_channel_id(para, 0);
				}

				dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
				__wrn(" fm_ctrl->channel_id  = %d\n", fm_ctrl->channel_id);

				if(ValueOutOfRange(MIN_CHANNEL_FREQ, fm_ctrl->cur_freq, MAX_CHANNEL_FREQ))
				{
					fm_ctrl->cur_freq = chan_info.freq;
				}

				__wrn(" fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);

				if((fm_ctrl->cur_freq < MIN_CHANNEL_FREQ) && (fm_ctrl->cur_freq > MIN_CHANNEL_FREQ))
				{
					return EPDK_FAIL;
				}//FM play3次声音才最清晰
				else
				{
					//if(87500 == MIN_CHANNEL_FREQ) //只针对4702，FM play3次声音才最清晰
					{
						dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);
						dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
					}

					dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
				}
			}
		}

		dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);
		dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);
		__msg("freq range = [%d, %d]\n", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
	}

	return EPDK_OK;
}

//初始化模块
static __s32 fm_init_module(void *cb_arg, FM_CTRLDATA_T *fm_ctrl)
{
	if(!dsk_radio_is_open())
	{
		reg_fm_para_t *para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
		dsk_radio_open();//打开radio模块
		dsk_radio_rcv_open();
		dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);
		//fm_set_band(fm_ctrl, DSK_RADIO_BAND_US_EUP);
		dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);
		dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);
#ifdef __MSG
		{
			//for debug
			__s32 i, num = 0;

			for(i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				__msg("FM_channel[%d]:%d\n", i, para->FM_channel[i]);

				if(0 != para->FM_channel[i])
				{
					num++;
				}
			}

			__msg("valid_num = %d, sel channel_id:%d\n", num, para->channel_id);
		}
#endif
		dsk_radio_rcv_get_search_result(para);
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
	}

	dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_SUCCESS, esKRNL_GetCallBack(cb_srhch_success), cb_arg);
	dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_FAIL, esKRNL_GetCallBack(cb_srhch_fail), cb_arg);
	dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_OVER, esKRNL_GetCallBack(cb_srhch_over), cb_arg);
	//dsk_radio_rcv_set_search_cb(esKRNL_GetCallBack(fm_auto_search_cb),(void *)fmplay_menu_attr);
	fm_init_data(fm_ctrl);
	return EPDK_OK;
}

/************************************************************************/
// fm 操作通知
/************************************************************************/
#define IsOPFunc(_op, _opFun)  ((_op) == (__s32)_opFun)
static __s32 _fm_op_notify(__s32 op_function)
{
	__wrn("fm_change_channelno\n");

	if(IsOPFunc(op_function, fm_change_channelno))
	{
		FM_WND_T *wnd_para;
		FM_GetWndPara(wnd_para, FM_WND_T, FM_GetManWnd());

		if(wnd_para)
		{
			if(wnd_para->h_main_lyr && wnd_para->h_main_wnd)
			{
				FM_UpdateUI(wnd_para->h_main_wnd);
			}
		}
	}

	__wrn("fm_change_channelno end\n");
	// 	else if (IsOPFunc(op_function, fm_change_channelno))
	// 	{
	// 	}
	return EPDK_OK;
}


//////////////////////////////////////////////////////////////////////////
//按键消息处理,主要因为在sset起来以后FM 的frm没收到按键消息
//返回true 为不需要向下发、否则需要
//////////////////////////////////////////////////////////////////////////
static __bool fm_key_proc(__gui_msg_t *msg)
{
	FM_WND_T *wnd_para;
	static __s32 manw_lastkey = -1;
	__bool ret = EPDK_FALSE;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	__wrn("msg->dwReserved=%d\n", msg->dwReserved);

	//__wrn("---FM manwin-data 2 =%d, data1 = %d, lastkey = %d\n",msg->dwAddData2,msg->dwAddData1,manw_lastkey );

	if(wnd_para->h_sset)
	{
		if(msg->dwAddData2 == KEY_UP_ACTION)
		{
			switch(manw_lastkey)
			{
					//			case GUI_MSG_KEY_LONGMENU:
				case GUI_MSG_KEY_MENU:
				case GUI_MSG_KEY_SUBMENU:
					//			case GUI_MSG_KEY_LONGSUBMENU:
				{
					NOTIFY_MSG(FM_COMMAND, msg->h_deswin, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0, 0);
					ret = EPDK_TRUE;
					break;
				}

				default:
				{
					ret = EPDK_FALSE;
					break;
				}
			}

			manw_lastkey = -1;
		}
		else
		{
			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_LONGMENU:
				case GUI_MSG_KEY_MENU:
				case GUI_MSG_KEY_SUBMENU:
				case GUI_MSG_KEY_LONGSUBMENU:
				{
					manw_lastkey = msg->dwAddData1;
					ret = EPDK_TRUE;
					break;
				}

				default:
				{
					ret = EPDK_FALSE;
					break;
				}
			}
		}
	}
	else
	{
		ret = EPDK_FALSE;
	}

	return ret;
}

/************************************************************************/
// 处理sset命令
/************************************************************************/
static __s32 on_fm_sset_cmd(H_WIN hwnd, __s32 id, __s32 value)
{
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

	if(FM_SSET_NC_ID_UNKNOW == value)
	{
		__wrn("FM_SSET_NC_ID_UNKNOW\n");
		return EPDK_OK;
	}

	switch(id)
	{
		case FM_SSET_NC_EXIT:
		{
			NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0, 0);
			notify_to_close_fm(hwnd, SWITCH_TO_MMENU, 0);
		}
		break;

		case FM_SSET_NC_CHANNEL:
		{
			//sset自己处理
		}
		break;

		case FM_SSET_NC_MSEARCH:
		{
			fm_manual_search_start(&wnd_para->fm_ctrl, 0);
		}
		break;

		case FM_SSET_NC_ASEARCH:
		{
			NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0, 0);
			NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_SHOW_ASEARCH_DLG, 0, 0);
			//fm_auto_search_start(&wnd_para->fm_ctrl);
		}
		break;

		case FM_SSET_NC_AUDIO:
		{
			dsk_radio_set_audio_mode(value);
		}
		break;

		case FM_SSET_NC_RECORD:
		{
			fm_call_record_app(hwnd, value);
		}
		break;

		case FM_SSET_NC_SEL_CHANNEL:
		{
			__wrn("  fm_change_channelno:id = %d,    value = %d\n", id, value);
			fm_change_channelno(&wnd_para->fm_ctrl, value);
		}
		break;

		case FM_SSET_NC_SEL_AUDIO:
		{
			__gui_msg_t   mymsg;
			ASSERT(value == DSK_RADIO_AUDIO_STEREO || value == DSK_RADIO_AUDIO_MONO
			       || value == DSK_RADIO_AUDIO_EXIT);

			if(value < 3) //重刷图形，切换显示"mono"，"stereo"
			{
				__wrn("value = %d\n", value);
				dsk_radio_set_audio_mode(value);
				mymsg.h_deswin = wnd_para->h_main_wnd ;
				mymsg.id = GUI_MSG_PAINT;
				GUI_SendNotifyMessage(&mymsg);
			}
			else //退出
			{
				NOTIFY_MSG(FM_COMMAND, NULL, hwnd, ID_FM_CMD_DESTROY_SSET, 0, 0);
			}
		}
		break;

		case SSET_NC_TIMER:
		{}
		break;
	}

	return EPDK_OK;
}

/************************************************************************/
// 处理dialog命令
/************************************************************************/
static __s32 on_fm_auto_search_dlg_cmd(H_WIN hwnd, __s32 id, __s32 para)
{
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

	switch(id)
	{
		case ADLG_CMD_EXIT:
		{
			app_dialog_destroy(wnd_para->h_dlg);
			wnd_para->h_dlg = NULL;

			if(para == ADLG_IDYES)
			{
				fm_auto_search_start(&wnd_para->fm_ctrl);
			}
			else
			{}
		}
		break;
	}

	return EPDK_OK;
}



void *Fm_get_icon_res(__u32 index)
{
	fm_icon_data_t *icon_table = NULL;
	icon_table = fm_icon_table;

	if(index > (sizeof(fm_icon_table) / sizeof(fm_icon_table[0])))
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


static __s32 FM_digit_layer_draw(__gui_msg_t *msg)
{
	__s32 	 i;
	__s32 	 j;
	RECT 	 rect;
	char     text[256];
	GUI_RECT gui_rect;
	__s32 	 total_fm_num;
	__s32 X = 0, Y = 0;
	void *pic_buf = NULL;
	FM_WND_T *wnd_para;
	H_LYR	*phLyr;
	__wrn("&&&&&&&& fm_digit_layer_draw \n");
	//wnd_para = (FM_WND_T*)GUI_WinGetAddData(msg->h_deswin);
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	phLyr = &wnd_para->fm_ctrl.hdigit_layer;

	//__wrn("fm_ctrl0 = %d", fm_ctrl0);
	if(wnd_para == NULL)
	{
		__err("fm_ctrl is null...\n");
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

	total_fm_num = dsk_radio_rcv_get_search_result_vn();
	{
		eLIBs_memset(text, 0, sizeof(text));
		eLIBs_int2str_dec(wnd_para->fm_ctrl.ndigit_fm_num, text);
	}
	GUI_LyrWinSel(*phLyr);
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
#ifdef ID_MOVIE_NUMLOCK_BG_BMP
	{
		pic_buf = Fm_get_icon_res(10);

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

	if(msg->dwAddData2 != GUI_MSG_KEY_NUMLOCK)
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

			pic_buf = Fm_get_icon_res(text[i] - '0');

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

void fm_display_digit(__gui_msg_t *msg)
{
	H_LYR	*phLyr;
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	phLyr = &wnd_para->fm_ctrl.hdigit_layer;

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

	__wrn("movie_ctrl->digit_timmer_id=%d\n", wnd_para->fm_ctrl.digit_timmer_id);

	//__wrn("msg->h_deswin=0x%x\n", msg->h_deswin);
	if(!GUI_IsTimerInstalled(msg->h_deswin, wnd_para->fm_ctrl.digit_timmer_id))
	{
		__wrn("===========create timmer...\n");
		wnd_para->fm_ctrl.ndigit_fm_num = 0;
		GUI_SetTimer(msg->h_deswin, wnd_para->fm_ctrl.digit_timmer_id, TIMEOUT_ID_TIMER_DIGIT, NULL);
	}
	else
	{
		__wrn("=============reset timmer...\n");
		GUI_ResetTimer(msg->h_deswin, wnd_para->fm_ctrl.digit_timmer_id, TIMEOUT_ID_TIMER_DIGIT, NULL);
	}

	if(msg->dwAddData2 != GUI_MSG_KEY_NUMLOCK)
	{
		__s32 cur_val;
		__s32 total_song_num;
		cur_val = msg->dwAddData2 - GUI_MSG_KEY_NUM0;
		__wrn("&&&&&&&&&&& cur_val=%d \n", cur_val);

		if(1)
		{
			//if(wnd_para->fm_ctrl.ndigit_fm_num < 100000)
			{
				wnd_para->fm_ctrl.ndigit_fm_num *= 10;
				wnd_para->fm_ctrl.ndigit_fm_num += cur_val;
				wnd_para->fm_ctrl.ndigit_fm_num %= 1000000;	// 低6位有效
			}
		}
		else
		{
			__wrn("=============fm num exceed...\n");
		}
	}

	__wrn("movie_ctrl->ndigit_fm_num=%d\n", wnd_para->fm_ctrl.ndigit_fm_num);
	FM_digit_layer_draw(msg);
}

__s32 FM_delete_digit_layer(__gui_msg_t *msg)
{
	FM_WND_T *wnd_para;
	__wrn("&&&&&&&& fm_delete_digit_layer\n");
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

	if(wnd_para == NULL)
	{
		__err("wnd_para is null...\n");
		return EPDK_OK;
	}

	if(wnd_para->fm_ctrl.hdigit_layer)
	{
		GUI_LyrWinDelete(wnd_para->fm_ctrl.hdigit_layer);
		wnd_para->fm_ctrl.hdigit_layer = NULL;
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

	if(wnd_para->fm_ctrl.digit_timmer_id)
	{
		if(GUI_IsTimerInstalled(msg->h_deswin, wnd_para->fm_ctrl.digit_timmer_id))
		{
			GUI_KillTimer(msg->h_deswin, wnd_para->fm_ctrl.digit_timmer_id);
		}
	}

	wnd_para->fm_ctrl.ndigit_fm_num = 0;
	__wrn("fm_delete_digit_layer end...\n");
	return EPDK_OK;
}

__s32 fm_proc_digital(__gui_msg_t *msg)
{
	FM_WND_T *wnd_para;
	__s32 freq = 0;
	__s32 channelid = 0;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	__wrn("this->ndigit_fm_num=%d\n", wnd_para->fm_ctrl.ndigit_fm_num);

	if(wnd_para->fm_ctrl.ndigit_fm_num <= wnd_para->fm_ctrl.channel_count)
	{
		//todo :  根据索引跳转有效?
		fm_change_channelno(&wnd_para->fm_ctrl, wnd_para->fm_ctrl.ndigit_fm_num);
	}
	else if(wnd_para->fm_ctrl.ndigit_fm_num >= 870 && wnd_para->fm_ctrl.ndigit_fm_num <= 1080)
	{
		//todo: 跳转到相应频率电台
		__wrn("************************************1");
		freq = wnd_para->fm_ctrl.ndigit_fm_num * 100;
		channelid = dsk_radio_rcv_search_matching_frep(freq);

		if(channelid != -1)
		{
			wnd_para->fm_ctrl.channel_id = channelid;
			fm_change_channelno(&wnd_para->fm_ctrl, channelid);
		}
		else
		{
			wnd_para->fm_ctrl.cur_freq = freq;
			dsk_radio_rcv_set_freq_play(freq);
			FM_UpdateUI(wnd_para->h_main_wnd);
		}
	}
	else
	{
		FM_delete_digit_layer(msg);
		return EPDK_FALSE;
	}

	FM_delete_digit_layer(msg);
	return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//命令消息处理 FM_COMMAND
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_command(__gui_msg_t *msg)
{
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

	switch(msg->dwAddData1)
	{
		case ID_FM_CMD_CREATE_SSET:
		{
			gscene_hbar_set_state(HBAR_ST_SHOW);

			if(wnd_para->h_sset)
			{
				__msg("err, sset created \n");
			}
			else
			{
				wnd_para->sset_create_para = FM_SSET_Build_CreatePara(FM_GetManWnd());

				if(wnd_para->sset_create_para)
				{
					//创建sset
					wnd_para->h_sset = SSET_Create(wnd_para->sset_create_para);
					SSET_Set_Nodelist("fm", NODELIST_ON);
					__msg("sset created \n");
				}
			}
		}
		break;

		case ID_FM_CMD_DESTROY_SSET:
		{
			if(wnd_para->h_sset)
			{
				SSET_Destroy(wnd_para->h_sset);
				wnd_para->h_sset = NULL;

				if(wnd_para->sset_create_para)
				{
					FM_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
					wnd_para->sset_create_para = NULL;
				}

				__msg("sset destroyed \n");
			}
			else
			{
				__msg("err, sset destroyed \n");
			}

			gscene_hbar_set_state(HBAR_ST_HIDE);
		}
		break;

		case ID_FM_CMD_SHOW_ASEARCH_DLG:
		{
			__s32 str[] = {STRING_FM_CHSRH_TITLE, STRING_FM_CHSRH_TIPS};
			default_dialog(wnd_para->h_dlg, FM_GetManWnd(), ID_FM_IS_AUTO_SEARCH_DLG, ADLG_YESNO, str);
		}
		break;
	}

	return EPDK_OK;
}

static __s32 fmplay_finetune(__gui_msg_t *msg, __u8 ucChMod, __u8  playmode)
{
	__u32 uFreq = 0;
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

	if(wnd_para->fm_ctrl.hdigit_layer)
	{
		fm_proc_digital(msg);
		return EPDK_OK;
	}

	if(1 == ucChMod) //	1:减
	{
		uFreq = wnd_para->fm_ctrl.cur_freq - LARK_SEARCH_STEP_US;

		if(uFreq < FM_SEARCH_CHN_MIN_FREQ)
		{
			if(playmode == 1)  // 循环
			{
				uFreq = FM_SEARCH_CHN_MAX_FREQ;
			}
			else			//不循环
			{
				uFreq = FM_SEARCH_CHN_MIN_FREQ;
			}
		}
	}
	else			//0: 加
	{
		uFreq = wnd_para->fm_ctrl.cur_freq + LARK_SEARCH_STEP_US;

		if(uFreq > FM_SEARCH_CHN_MAX_FREQ)
		{
			if(playmode == 1)  // 循环
			{
				uFreq = FM_SEARCH_CHN_MIN_FREQ;
			}
			else			//不循环
			{
				uFreq = FM_SEARCH_CHN_MAX_FREQ;
			}
		}
	}

	__msg("uFreq = %d\n", uFreq);
	wnd_para->fm_ctrl.cur_freq = uFreq;
	dsk_radio_rcv_set_freq_play(uFreq);
	//dek_radio_set_FM1_3(fmplay_menu_attr->curFM1_3AM1_2_id,fmplay_menu_attr->curFM1_3AM1_2_index, uFreq);		//add by Kingvan
	//dsk_radio_rcv_save_search_result();
	//refresh ui:
	__wrn("fm_change_channelno");
	fm_op_notify(fm_change_channelno);
	return EPDK_OK;
}

//bincr: 1增加，0:减小
static __s32 __app_fm_channel_channel(__gui_msg_t *msg, __s32 bincr)
{
	H_WIN hwnd = msg->h_deswin;
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

	if(1 == bincr)
	{
		if(wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
		    || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
		{}
		else
		{
			__s32  channel_id = wnd_para->fm_ctrl.channel_id + 1;
			__wrn("channel_id = %d\n", channel_id);

			if(channel_id >= (__s32)(wnd_para->fm_ctrl.channel_count))
			{
				channel_id = 0;
			}

			__wrn("fm_change_channelno\n");
			fm_change_channelno(&wnd_para->fm_ctrl, channel_id + 1);
		}
	}
	else
	{
		if(wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
		    || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
		{}
		else
		{
			__s32  channel_id = wnd_para->fm_ctrl.channel_id - 1;

			if(channel_id < 0)
			{
				channel_id = wnd_para->fm_ctrl.channel_count - 1;
			}

			if(channel_id < 0)
			{
				channel_id = 0;
			}

			if(channel_id >= (__s32)(wnd_para->fm_ctrl.channel_count))
			{
				channel_id = 0;
			}

			fm_change_channelno(&wnd_para->fm_ctrl, channel_id + 1);
		}
	}

	return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//命令消息处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_manwnd_command(__gui_msg_t *msg)
{
	H_WIN hwnd = msg->h_deswin;
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	APP_DEBUG_CBMSGEx;

	switch(LOSWORD(msg->dwAddData1))
	{
			__wrn("msg->dwAddData1 == %d\n", msg->dwAddData1);

		case ID_FM_SCENE_SSET:
			return on_fm_sset_cmd(msg->h_deswin, HISWORD(msg->dwAddData1), msg->dwAddData2);

		case ID_FM_IS_AUTO_SEARCH_DLG:
			return on_fm_auto_search_dlg_cmd(msg->h_deswin, HISWORD(msg->dwAddData1), msg->dwAddData2);

		case FM_MAINWND_ID:
		{
			switch(HISWORD(msg->dwAddData1))
			{
				case ID_FM_MAINWND_NOTIFY_TOUCHUP:
				{
					if(wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
					    || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
					{}
					else
					{
						if(wnd_para->h_sset)
						{
							NOTIFY_MSG(FM_COMMAND, msg->h_deswin, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0, 0);
						}
						else
						{
							NOTIFY_MSG(FM_COMMAND, msg->h_deswin, FM_GetManWnd(), ID_FM_CMD_CREATE_SSET, 0, 0);
						}
					}
				}
				break;

				case  ID_FM_MAINWND_CLICKUP:
				{
					switch(msg->dwAddData2)
					{
#if 1

						case GUI_MSG_KEY_LONGMENU:
						{
							reg_fm_para_t *para;
							__gui_msg_t   mymsg;
							get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
							dsk_radio_rcv_save_cur_freq();
							wnd_para->fm_ctrl.channel_count = dsk_radio_rcv_get_search_result_vn();
							__wrn("wnd_para->fm_ctrl.channel_count=%d\n", wnd_para->fm_ctrl.channel_count);
							__wrn("wnd_para->fm_ctrl.cur_freq = %d, para->channel_id=%d\n", wnd_para->fm_ctrl.cur_freq, para->channel_id);
							wnd_para->fm_ctrl.channel_id = para->channel_id;
							__wrn("wnd_para->fm_ctrl.channel_id=%d\n", wnd_para->fm_ctrl.channel_id);
							//重刷FM主界面图层，更新保存手动搜索的台数
							mymsg.h_deswin = wnd_para->h_main_wnd ;
							mymsg.id = GUI_MSG_PAINT;
							GUI_SendNotifyMessage(&mymsg);
							app_fm_notify_delete_sub_scene(msg->h_deswin);
							{
								H_WIN dlg;
								__s32 lang_id[] = {STRING_FM_CUE, STRING_FM_SAVE_FREQ_OK};
								default_dialog(dlg, msg->h_deswin, ID_FM_SAVE_FREQ_DLG, ADLG_OK, lang_id);
								esKRNL_TimeDly(100);
								app_dialog_destroy(dlg);
							}
							break;
						}

#endif

						case GUI_MSG_KEY_MENU:
						{
							__wrn("on_fm_manwnd_command():GUI_MSG_KEY_MENU, g_is_searching=%d, wnd_para->h_sset=%X\n", g_is_searching, wnd_para->h_sset);

							if(g_is_searching)
							{
								dsk_radio_rcv_autosearch_resume();
								dsk_radio_rcv_search_stop();
								break;
							}

							if(wnd_para->h_sset)
							{
								NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0, 0);
							}
							else
							{
								gscene_hbar_set_state(HBAR_ST_HIDE);
							}

							notify_to_close_fm(hwnd, SWITCH_TO_MMENU, 0);
							break;
						}

#if 1

						case GUI_MSG_KEY_ENTER:
						{
#if 0

							if(wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
							    || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
							{}
							else
							{
								__s32  channel_id = wnd_para->fm_ctrl.channel_id + 1;
								__wrn("channel_id = %d\n", channel_id);

								if(channel_id >= wnd_para->fm_ctrl.channel_count)
								{
									channel_id = 0;
								}

								__wrn("fm_change_channelno\n");
								fm_change_channelno(&wnd_para->fm_ctrl, channel_id + 1);
							}

#else
							//SEND_MSG(GUI_MSG_KEY, msg->h_deswin, GUI_WinGetHandFromName(APP_ROOT), GUI_MSG_KEY_MUTE, KEY_DOWN_ACTION, 0);
							fmplay_finetune(msg, 0, 1);
#endif
							break;
						}

#endif

						case GUI_MSG_KEY_RIGHT:
						case GUI_MSG_KEY_PADD:
						case GUI_MSG_KEY_UP:
						case GUI_MSG_KEY_NEXT:
						{
							__wrn("GUI_MSG_KEY_RIGHT...\n");
#if 0
							fmplay_finetune(msg, 0, 1);
#else
							__wrn("msg->dwReserved=%d\n", msg->dwReserved);

							if(0 == msg->dwReserved)//面板按键
							{
								__app_fm_channel_channel(msg, 1);
							}
							else//遥控按键
							{
								__app_fm_channel_channel(msg, 1);
							}

#endif
							break;
						}

#if 1

						case GUI_MSG_KEY_LONGENTER:
						case GUI_MSG_KEY_LONGFM:
						case GUI_MSG_KEY_SEARCHFREQ:
						{
							//fm_auto_search_start(&wnd_para->fm_ctrl);
							NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_SHOW_ASEARCH_DLG, 0, 0);
							break;
						}

						/*
						case GUI_MSG_KEY_LONGENTER:
							{
								//fm_auto_search_start(&wnd_para->fm_ctrl);
								SEND_MSG(DSK_MSG_SCREEN_OPEN, hwnd, GUI_WinGetHandFromName("init"), 0, 0, 0);
								break;

							}
							//#else
						*/
						case GUI_MSG_KEY_LONGRIGHT:
						{
							if(wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
							    || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
							{}
							else
							{
								fm_manual_search_start(&wnd_para->fm_ctrl, 0);
								//dsk_radio_rcv_set_search_cb(esKRNL_GetCallBack(fm_auto_search_cb),(void *)fmplay_menu_attr);
							}

							break;
						}

						case GUI_MSG_KEY_LONGLEFT:
						{
							if(wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
							    || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
							{}
							else
							{
								fm_manual_search_start(&wnd_para->fm_ctrl, 1);
								//dsk_radio_rcv_set_search_cb(esKRNL_GetCallBack(fm_auto_search_cb),(void *)fmplay_menu_attr);
							}

							break;
						}

#endif

						case GUI_MSG_KEY_LEFT:
						case GUI_MSG_KEY_PSUB:
						case GUI_MSG_KEY_DOWN:
						case GUI_MSG_KEY_PREV:
						{
							__wrn("GUI_MSG_KEY_LEFT...\n");
#if 0
							fmplay_finetune(msg, 1, 1);
#else
							__wrn("msg->dwReserved=%d\n", msg->dwReserved);

							if(0 == msg->dwReserved)//面板按键
							{
								__app_fm_channel_channel(msg, 0);
							}
							else//遥控按键
							{
								__app_fm_channel_channel(msg, 0);
							}

#endif
							break;
						}

						case GUI_MSG_KEY_NUMLOCK:
							if(NULL == wnd_para->fm_ctrl.hdigit_layer)
							{
								fm_display_digit(msg);
							}

							break;

						case GUI_MSG_KEY_NUM0:
						case GUI_MSG_KEY_NUM1:
						case GUI_MSG_KEY_NUM2:
						case GUI_MSG_KEY_NUM3:
						case GUI_MSG_KEY_NUM4:
						case GUI_MSG_KEY_NUM5:
						case GUI_MSG_KEY_NUM6:
						case GUI_MSG_KEY_NUM7:
						case GUI_MSG_KEY_NUM8:
						case GUI_MSG_KEY_NUM9:
						{
							fm_display_digit(msg);
							break;
						}

						//===========================================================
						//++++++++++++++++++++++++++//
						default:
							break;
					}
				}
				break;
			}
		}

		return EPDK_OK;
	}

	return EPDK_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//主窗口销毁处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_destory(__gui_msg_t *msg)
{
	FM_WND_T *wnd_para;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	__wrn("on_fm_destory():begin\n");

#if SP_FM_WhenAutoSearch_PlayFreq	
		if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_FM_TestPlayFreq) == EPDK_TRUE)
		{
			GUI_KillTimer(msg->h_deswin, ID_TIMER_FM_TestPlayFreq);
			__wrn("GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq);\n");
		}	
#endif

	g_is_searching = 0;
	//允许自动关屏
	//	g_enable_close_scn();
	{
		//恢复系统自动关屏时间
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			g_set_close_scn_time(para->backlightoff);
			__msg("para->backlightoff=%d\n", para->backlightoff);
		}
	}
	g_enable_standby();
	//	if (dsk_radio_rcv_get_search_result_vn() <= 1)
	{
		reg_fm_para_t *para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
		//		ASSERT(wnd_para->fm_ctrl.channel_count == 1);
		//		ASSERT(ValueBetweenRange(wnd_para->fm_ctrl.cur_min_freq, wnd_para->fm_ctrl.cur_freq, wnd_para->fm_ctrl.cur_max_freq));
		//		fm_reg_set_channel_(para, 0, wnd_para->fm_ctrl.cur_freq);
		fm_reg_set_curfreq(para, wnd_para->fm_ctrl.cur_freq);
		__msg("cur_freq = %d, channel_count = %d\n", fm_reg_get_curfreq(para), fm_reg_get_channel_count(para));
	}
	//刷新注册表数据
	update_to_reg();

	if(! wnd_para->fm_ctrl.is_record)
	{
		dsk_radio_close();
	}

	//TODO:release resource

	if(wnd_para->h_sset)
	{
		SSET_Destroy(wnd_para->h_sset);
		wnd_para->h_sset = NULL;
	}

	if(wnd_para->sset_create_para)
	{
		FM_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
		wnd_para->sset_create_para = NULL;
	}

	if(wnd_para->h_dlg)
	{
		app_dialog_destroy(wnd_para->h_dlg);
		wnd_para->h_dlg = NULL;
	}

#if SP_APP_ALARM
	if(wnd_para->h_frm_alarm_coming)
	{
		app_alarm_frm_destory(wnd_para->h_frm_alarm_coming);
		wnd_para->h_frm_alarm_coming = NULL ;
	}
#endif

	app_com_layer_destroy(wnd_para->h_main_lyr);
	BFreeType(wnd_para, FM_WND_T);
	EndOfDebugAPPMEM();
	__wrn("on_fm_destory:over\n");
	return 0;
}

static __s32 get_auto_close_scn_time_fm(void)
{
	__s32 bok;
	__s32 close_scn_time;
	{
		//init key val by app_config.fex
		char *path = "d:\\app_config.bin";
		H_PARSER parser;
		__s32 ret;
		__s32 i;
		char keyname[64];
		int val;
		char str_val[32];
		bok = 1;

		do
		{
			parser = NULL;
			parser = esCFG_Init_Ex(path);

			if(NULL == parser)
			{
				__wrn("esCFG_Init_Ex fail...\n");
				bok = 0;
				break;
			}

			__wrn("before esCFG_GetKeyValue_Ex\n");
			eLIBs_strcpy(keyname, "fm");
			eLIBs_strcat(keyname, "/");
			eLIBs_strcat(keyname, "auto_close_scn");
			ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)&val, 1);
			__wrn("after esCFG_GetKeyValue_Ex, ret=%d\n", ret);

			if(EPDK_OK == ret)
			{
				bok = 1;
				close_scn_time = val;
			}
			else
			{
				bok = 0;
			}
		}
		while(0);

		if(parser)
		{
			esCFG_Exit_Ex(parser);
			parser = NULL;
		}
	}

	if(0 == bok)//default is not auto close scn
	{
		close_scn_time = 0;
	}

	__wrn("close_scn_time=%d\n", close_scn_time);
	return close_scn_time;
}
typedef struct	{
	
  __u32 micPreAmpGainCtl	: 3;//B2-B0
	
  __u32 micPreAmpEnBit		: 1;//B3
	__u32	FMtoLRMirerGain 	: 3;//B6-B4 
	__u32 paspreedselect		: 1;//b7
	__u32 adcInputSrcSel	  : 6;//b13-b8. adc input source
	__u32 CosSlopTimeForAntipop:2;//b15-b14
	__u32 adcInputGainCtl	  : 3;//b18-16, adc input gain
  __u32 reserved4			: 2;//b20-19
  __u32 LineintoLRMirerGain : 3;//b23-21
  __u32 MictoLRMirerGain	: 3; //b26-24
  __u32 reserved5			: 4;//b30-b27
  __u32 adcEnBit			: 1;//b31
}AdcAnaCtl;

__s32 dsk_audio_set_linein_gain(__s32 value)
 {
	volatile AdcAnaCtl* pAdcAnaCtl = (volatile AdcAnaCtl*)(0x24 + 0Xf1C23C00);
	if(value>7||value<0)
		return -1;
	pAdcAnaCtl->LineintoLRMirerGain	=value;
	return 0;

 }

//////////////////////////////////////////////////////////////////////////
//主窗口创建处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_create(__gui_msg_t *msg)
{
	FM_UI_PARA_T *ui;
	FM_WND_T *wnd_para;
	__s32 auto_close_scn_time;
	FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
	//不显示headbar
	//gscene_hbar_set_state(HBAR_ST_HIDE);
	//禁止自动关屏
	//	g_disable_close_scn();
	g_disable_standby();
	//设置调色板
	com_set_palette_by_id(ID_FM_COLOURTABLE_BMP);
	//初始化radio
	fm_init_module((void *)msg->h_deswin, &wnd_para->fm_ctrl);
	wnd_para->fm_ctrl.digit_timmer_id = 0x21;
	//TODO:init wnd_para
	//gscene_bgd_set_state(BGD_STATUS_HIDE);
	gscene_bgd_set_state(BGD_STATUS_SHOW);
	ui = get_fm_uipara();
	fm_layer_create(wnd_para->h_main_lyr, &ui->rt, "fm_main");
	wnd_para->h_main_wnd = fm_mainfrw_create(msg->h_deswin, wnd_para->h_main_lyr, (void *)&wnd_para->fm_ctrl);
	__msg("mainwnd = %x, lyr = %x\n", wnd_para->h_main_wnd, wnd_para->h_main_lyr);

	if(NULL == wnd_para->h_main_wnd)
	{
		return EPDK_FAIL;
	}

	_app_show_focus_wnd(wnd_para->h_main_wnd, wnd_para->h_main_lyr);
	GUI_LyrWinSetTop(wnd_para->h_main_lyr);
#if TIME_FMAutoCloseScreen
	g_set_close_scn_time(TIME_FMAutoCloseScreen);
#else
	auto_close_scn_time = get_auto_close_scn_time_fm();
	__msg("auto_close_scn_time=%d\n", auto_close_scn_time);

	if(auto_close_scn_time > 0)//设置FM的自动关屏时间
	{
		g_set_close_scn_time(auto_close_scn_time);
	}

#endif
	{
	ES_FILE *paudio;
	__u32 ret;
		paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");
	
		if(!paudio)
		{
			__wrn("Open Audio Dev File Failed\n");
			ret = EPDK_FAIL;
		//	break;
		}
	
		eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_START, 0xff, 0);
		eLIBs_fclose(paudio);
		dsk_audio_set_linein_gain(2);
}
	//关背景
	//gscene_bgd_set_state(BGD_STATUS_HIDE);
	return 0;
}

//主消息处理函数
static __s32 _app_fm_Proc(__gui_msg_t *msg)
{
	APP_DEBUG_CBMSGEx;

	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			esPWM_LockCpuFreq();//禁止CPU自动调节频率，防止FM杂音
			dsk_set_auto_off_time(0);
			//pull_down_gpio(4, 15);//拉低LCD15，选择AB类功放
			//dsk_amplifier_onoff(1);//打开功放
			g_FMManWnd = msg->h_deswin;
			return on_fm_create(msg);
		}

		case GUI_MSG_DESTROY:
		{
			__wrn("_app_fm_Proc:GUI_MSG_DESTROY:begin\n");

			esPWM_UnlockCpuFreq();//允许CPU自动调节频率
			{
				reg_system_para_t *para;
				para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

				if(para)
				{
					dsk_set_auto_off_time(para->poweroff);
					__wrn("para->poweroff=%d\n", para->poweroff);
				}
			}
			//dsk_amplifier_onoff(0);//关闭功放
			//set_input_gpio(4, 15, 0);//选择D类功放
			FM_delete_digit_layer(msg);
			IOCTRL__PA_MODE__SET_D();
			return on_fm_destory(msg);
		}

		case DSK_MSG_HOME:
		{
			notify_to_close_fm(msg->h_deswin, SWITCH_TO_MMENU, 0);
		}

		return EPDK_OK;

		case DSK_MSG_KILL:
		case GUI_MSG_CLOSE:
		{
			GUI_ManWinDelete(msg->h_deswin);
		}
		break;

		case GUI_MSG_COMMAND:
		{
			if(on_fm_manwnd_command(msg) == EPDK_OK)
			{
				return EPDK_OK;
			}
		}
		break;

		case FM_COMMAND:
			return on_fm_command(msg);

		case GUI_MSG_KEY:
		{
			fm_key_proc(msg);
		}
		break;

		case GUI_MSG_TIMER :
		{
			FM_WND_T *wnd_para;
			FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

			if(msg->dwAddData1 == wnd_para->fm_ctrl.digit_timmer_id)
			{
				__wrn("timmer has come...\n");
				return fm_proc_digital(msg);
			}

#if SP_FM_WhenAutoSearch_PlayFreq
			else if(msg->dwAddData1 == ID_TIMER_FM_TestPlayFreq)
			{
				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_FM_TestPlayFreq) == EPDK_TRUE)
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_FM_TestPlayFreq);
				}

				g_is_searching = 1;
				dsk_amplifier_onoff(0);
				dsk_radio_rcv_autosearch_resume();
				return EPDK_OK;
			}

#endif
		}
		break;

#if SP_APP_ALARM
		case DSK_MSG_ALARM:
		{
			FM_WND_T *wnd_para;
			FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

			if(wnd_para->h_dlg)
			{
				app_dialog_destroy(wnd_para->h_dlg);
				wnd_para->h_dlg = NULL ;
			}

			if(wnd_para->h_sset)
			{
				SSET_Destroy(wnd_para->h_sset);
				wnd_para->h_sset = NULL ;
			}

			wnd_para->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
			return EPDK_OK ;
		}

		case ALARM_MSG_CLOSE:
		{
			FM_WND_T *wnd_para;
			FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

			if(wnd_para->h_frm_alarm_coming)
			{
				wnd_para->h_frm_alarm_coming = NULL ;
			}

			__wrn("@@ALARM_MSG_CLOSE begin\n");
			GUI_WinSetFocusChild(msg->h_deswin);
			//	            dsk_amplifier_onoff(1);//闹钟关闭后会关功放，这里把它打开
			__wrn("@@ALARM_MSG_CLOSE end\n");
			return EPDK_OK ;
		}
#endif

		default:
			break;
	}

	return GUI_ManWinDefaultProc(msg);
}


__s32 app_fm_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	FM_WND_T *wnd_para;
#if (SP_FM_IC == SP_FM_IC_CDT5218)
	IOCTRL__ATV_PW__SET_ON();
#endif
	StartDebugAPPMEM();
	__inf("**************************************\n");
	__inf("********  enter fm app  **************\n");
	__inf("**************************************\n");
	app_set_title(STRING_HOME_FM);
	gscene_bgd_set_state(BGD_STATUS_SHOW);
	BallocType(wnd_para, FM_WND_T);
	ZeroTypeMem(wnd_para, FM_WND_T);
	wnd_para->font      = para->font;
	wnd_para->root_type = para->root_type;
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_FM;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_fm_Proc);
	create_info.attr            = (void *)wnd_para;
	create_info.id              = APP_FM_ID;
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));
}

__s32 app_fm_notify_delete_sub_scene(H_WIN hmanwin)
{
	if(NULL == hmanwin)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	SEND_MSG(FM_COMMAND, NULL, hmanwin, ID_FM_CMD_DESTROY_SSET, 0, 0);
	return EPDK_OK;
}

__s32 app_fm_get_auto_search_state(void)
{
	return (g_is_searching == 1) ? 1 : 0;
}

