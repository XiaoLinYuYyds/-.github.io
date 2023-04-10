#include	"app_new_fm.h"
#include	"new_fm.h"


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
}AdcAnaCtl1;


//////////////////////////////////////////////////////////////////////////
// 建立图层
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)

__s32 g_is_searching1 = 0;


//窗口的命令发送函数
__s32  app_new_fm_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
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

//设置音频线输入接收
__s32 __dsk_audio_set_linein_gain(__s32 value)
{
	volatile AdcAnaCtl1* pAdcAnaCtl = (volatile AdcAnaCtl1*)(0x24 + 0Xf1C23C00);
	if(value>7||value<0)
		return -1;
	pAdcAnaCtl->LineintoLRMirerGain	=value;
	return 0;

}

/************************************************************************/
// 选择某个频道
//因为增加了退出一项，所以sel_id要减1
/************************************************************************/
static __s32 _new_fm_change_channelon()
{

}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的搜索成功回调处理函数//当前找到一个有效频率
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/

static void on_new_fm_test_freq_ok(NEW_FM_CTRLDATA_T *fm_ctrl)
{
	if(fm_ctrl->search_mode == NEW_SRHMODE_MANUAL)//手动搜索
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
//////////////////////////////////////////////////////////////////////////
#define NEW_FM_CB_NEED_NOTIFY_FRW(_p) (_p && _p->fm_lyr && GUI_LyrWinGetSta(_p->fm_lyr) == GUI_LYRWIN_STA_ON)

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的搜索成功回调处理函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 new_cb_srhch_success(void *arg_p)
{
	new_fm_ctrl *wnd_para;
	H_WIN hwnd = (H_WIN)arg_p;
	NEW_FM_GetWndPara( wnd_para, new_fm_ctrl, hwnd);
	__wrn("*******new_cb_srhch_success********\n");

	if(wnd_para)
	{
		on_new_fm_test_freq_ok(&wnd_para->fm_ctrl);//搜索成功
		__wrn("draw:fm_ctrl->cur_freq = %d\n", wnd_para->fm_ctrl.cur_freq);
	}

	if(NEW_FM_CB_NEED_NOTIFY_FRW(wnd_para))
	{
		NOTIFY_MSG(NEW_CMD_AUTOSRH_FINDCH, hwnd, wnd_para->fm_framewin, 0, 0, 0);//通告MSG
	}

#if 1
	if(GUI_IsTimerInstalled(hwnd, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
	{
		GUI_ResetTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1, ID_TIMER_FM_TestPlayFreq_Speed_1, 0);//定时器复位
	}
	else
	{
		GUI_SetTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1, ID_TIMER_FM_TestPlayFreq_Speed_1, 0);//开启定时器
	}
	g_is_searching1 = 2;
	dsk_amplifier_onoff(1);//打开功放,声音
#endif
	
	return 0;
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的自动搜索函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_auto_search_start(NEW_FM_CTRLDATA_T *fm_ctrl)
{
	__u32 search_flag;
	g_is_searching1 = 1;
	dsk_radio_get_search_flag(&search_flag);//获取搜索标志

	if(1 == search_flag)	//正在搜索过程中不允许中断，必须得搜索完后才能进行
	{
		__wrn("__new_fm_auto_search_start:return\n");
		return EPDK_OK;
	}
	else
	{
		__wrn("__new_fm_auto_search_start:\n");
		fm_ctrl->search_mode = NEW_SRHMODE_AUTO;//赋值搜索模式为自动搜索模式
		dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_AUTO);//设置为自动模式
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);//设置---播放列表类型
		dsk_radio_rcv_search_all(0, 0);//收音机接收端---搜索所有频率和通道
		esKRNL_TimeDly(5);//延时5*10ms
		SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);//发送
		return EPDK_OK;
	}
}

//设置频带
static __s32 __new_fm_set_band(NEW_FM_CTRLDATA_T *fm_ctrl, dsk_radio_band_mode_t mode)
{
	dsk_radio_set_band(mode);						//设置频带
	dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);	//获取最小频率
	dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);	//获取最大频率
	__wrn("freq range = [%d, %d]\n", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的初始化数据函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_init_date(NEW_FM_CTRLDATA_T *fm_ctrl)
{
	fm_ctrl->search_mode = NEW_SRHMODE_IDLE;//设置搜索模式为空
	
	if(dsk_radio_is_open())
	{
		dsk_radio_chan_t chan_info;
		reg_fm_para_t	*para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);//获取注册表指示器
		fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();//获取---有效的搜索结果--频道数量
		__wrn("channel_count = %d...\n",fm_ctrl->channel_count);
		if(fm_ctrl->channel_count <= 0)//如果没有搜索到有效结果
		{
			fm_ctrl->channel_id = 0;
			fm_ctrl->cur_freq = NEW_MIN_CHANNEL_FREQ;
			fm_ctrl->channel_count = 0;
			ZeroMemory(para, sizeof(reg_fm_para_t));//结构体初始化，清0
			//fm_reg_set_sel_channel_id(para, 0);
			// 			fm_reg_set_channel_(para, 0, fm_ctrl->cur_freq); //设置默认一个频道频率
			// 			fm_reg_set_channel_count(para, fm_ctrl->channel_count);
			
			dsk_radio_rcv_get_search_result(para);	//从列表文件里获取搜索到的频道列表
			{
				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);//设置播放频率

				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
			}

			dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
		}
		else
		{
			__wrn("channel_count is no null...\n");
			/*if(ValueBetweenRange(NEW_MIN_CHANNEL_FREQ, fm_reg_get_curfreq(para), NEW_MAX_CHANNEL_FREQ)
				&& (fm_reg_get_channel_count(para) == 1) && (fm_ctrl->channel_count == 1)
				&& (fm_reg_get_channel_(para, 0) == NEW_MIN_CHANNEL_FREQ))
			{
				//曾经有手动设置频率(没搜索过)
				fm_ctrl->cur_freq = fm_reg_get_curfreq(para);	//获取当前频率
				__wrn("fm_init:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
				__wrn("fm_init:fm_ctrl->channel_count  = %d\n", fm_ctrl->channel_count);

				if((fm_ctrl->cur_freq < NEW_MIN_CHANNEL_FREQ) && (fm_ctrl->cur_freq > NEW_MIN_CHANNEL_FREQ))
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
			}*/
		}

		dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);
		dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);
		__msg("freq range = [%d, %d]\n", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
	}
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的radio模块初始化函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_init_module(void *cb_arg, NEW_FM_CTRLDATA_T *fm_ctrl)
{
	if(!dsk_radio_is_open())//如果检测radio模块没有打开
	{
		reg_fm_para_t	*para;
		__wrn("radio_open is start...\n");
		dsk_radio_open();									//打开radio模块
		__wrn("radio_open is ok...\n");
		dsk_radio_rcv_open();								//收音机接收端模块---打开
		__wrn("radio_rcv_open is ok...\n");
		dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);			//设置收音机频段 为美国频段
		__wrn("radio_set_band is ok...\n");
		dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);	//设置收音机信号强度门限值 启点阈值 为中等
		__wrn("radio_set_threshold is ok...\n");
		dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);	//设置收音机的音频模式 为立体声
		__wrn("radio_set_audio_mode is ok...\n");

#ifdef __MSG//调试
		{
			//for debug
			__s32 i, num = 0;

			for(i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				__wrn("FM_channel[%d]:%d\n", i, para->FM_channel[i]);

				if(0 != para->FM_channel[i])
				{
					num++;//记录有效的频率
				}
			}

			__wrn("valid_num = %d, sel channel_id:%d\n", num, para->channel_id);
		}
#endif

		dsk_radio_rcv_get_search_result(para);				//从列表文件里获取搜索到的频道列表
		__wrn("radio_rcv_get_search_result is ok...\n");
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);//设置---播放列表类型
		__wrn("radio_rcv_set_play_list_type is ok...\n");
	}
	__wrn("dsk_radio_set_cb is start...\n");
	dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_SUCCESS, esKRNL_GetCallBack((__pCBK_t)new_cb_srhch_success), cb_arg);//收音机设置搜索频率
	__wrn("__new_fm_init_date is start...\n");
	__new_fm_init_date(fm_ctrl);//初始化数据
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的framewin窗口场景创建函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32  app_new_fm_scene_create(__gui_msg_t *msg)
{
	new_fm_ctrl		*fm_ctrl;
	RECT	fm_rect;
	fm_create_para_t	fm_create;
	fm_ctrl = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
	eLIBs_memset(&fm_create, 0, sizeof(fm_create_para_t));
	//////////fm的framewin图层大小///////////
	fm_rect.x	= 0;
	fm_rect.y	= 0;
	fm_rect.width	= 600;
	fm_rect.height	= 1024;
	fm_layer_create(fm_ctrl->fm_lyr, &fm_rect, "new_fm_lyr"); 
	if(!fm_ctrl->fm_lyr) {
		__wrn("fm_lyr is null...\n");
		return EPDK_FALSE;
	} else {
		__wrn("fm_lyr is create success...\n");
	}
	fm_create.fm_frmlyr = fm_ctrl->fm_lyr;
	fm_create.fm_frmwin = msg->h_deswin;//获取
	fm_ctrl->fm_framewin = new_fm_frmwin_create(msg->h_deswin,  &fm_create);//framewin窗口创建函数

	GUI_WinSetFocusChild(fm_ctrl->fm_framewin);//发送按键等命令信息到framewin子窗口
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_proc
*
*Description: fm的manwin窗口创建回调处理函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static	__s32	app_new_fm_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{	
		case GUI_MSG_CREATE:{	//0，创建
			new_fm_ctrl	*wnd_para;
			//FM_GetWndPara(wnd_para, new_fm_ctrl, msg->h_deswin);//获取ATTr数据
			wnd_para = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
			__wrn("new fm is start create\n");
			esPWM_LockCpuFreq();//禁止CPU自动调节频率，防止FM杂音
			//设置调色板
			com_set_palette_by_id(ID_FM_COLOURTABLE_BMP);
			//初始化radio模块
			__new_fm_init_module((void *)msg->h_deswin, &wnd_para->fm_ctrl);//radio模块初始化
			__wrn("__new_fm_init_module create\n");
			app_new_fm_scene_create(msg);//创建FM子窗口

			gscene_bgd_set_state(BGD_STATUS_SHOW);//显示
			//打开图层状态
			_app_show_focus_wnd(wnd_para->fm_framewin, wnd_para->fm_lyr);//图层状态打开
			GUI_LyrWinSetTop(wnd_para->fm_lyr);//图层置顶

			{
				ES_FILE *paudio;
				__u32 ret;
					paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");//打开音频文件驱动，开启声音
				
					if(!paudio)
					{
						__wrn("Open Audio Dev File Failed\n");
						ret = EPDK_FAIL;
					//	break;
					}
				
					eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_START, 0xff, 0);//文件控制启动，启动音频设备
					eLIBs_fclose(paudio);//关闭音频设备文件
					__dsk_audio_set_linein_gain(2);//设置音频线输入
			}
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	//1，销毁
			__wrn("new fm is start  destroy\n");
			esPWM_UnlockCpuFreq();//允许CPU自动调节频率
			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1);
				__wrn("GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1);\n");
			}
			app_new_fm_manwin_delete(msg);
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{	//2，绘制

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{		//4，退出
			__wrn("quit app_new_fm manwin...\n");
			app_new_fm_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);			
		}
		return EPDK_OK;

		case MSG_NEW_FM_OP_ENTER:{
			NEW_FM_CTRLDATA_T *fm_ctrl;
			fm_ctrl = (NEW_FM_CTRLDATA_T *)GUI_WinGetAttr(msg->h_deswin);
			__new_fm_auto_search_start(fm_ctrl);
			__wrn("g_is_searching1 = %d...\n", g_is_searching1);
			if(g_is_searching1)
			{
				__wrn("autosearch start...\n");
				dsk_radio_rcv_autosearch_resume();
				dsk_radio_rcv_search_stop();
				fm_ctrl->cur_freq = 93900;
				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
				break;
			}
		}
		return EPDK_OK;
		
		case GUI_MSG_KEY:{		//5，按键
			
		}
		break;
		
		case GUI_MSG_COMMAND:{	//7，命令                                   
			switch(LOWORD(msg->dwAddData1))
			{
				case NEW_FM_ID:	//framewin窗口ID，意思是在主菜单时候进入执行下面的程序
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case MSG_NEW_FM_OP_EXIT:{//退出应用到home界面
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		= GUI_MSG_QUIT;
							GUI_SendMessage(&mmsg);
						}
						break;

						case MSG_NEW_FM_OP_ENTER:{//打开自动搜索
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		= MSG_NEW_FM_OP_ENTER;
							GUI_SendMessage(&mmsg);
						}
						break;
					}
				}
			}
		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{	//19，定时器
					//new_fm_ctrl	*hwnd;
					//hwnd = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
					__wrn("ID_TIMER_FM_TestPlayFreq_1 timer0...\n");
					if(msg->dwAddData1 == ID_TIMER_FM_TestPlayFreq_1)
					{
						__wrn("ID_TIMER_FM_TestPlayFreq_1 timer\n");
						if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
						{
							GUI_KillTimer(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1);
						}
						g_is_searching1 = 1;
						dsk_amplifier_onoff(0);//关闭功放声音
						dsk_radio_rcv_autosearch_resume();//恢复自动搜索
						return EPDK_OK;
					}
		}
		break;
		
	}

	return GUI_ManWinDefaultProc(msg);
}





/*
************************************************************************************************************************
*                       				app_new_fm_manwin_create
*
*Description: fm的manwin窗口创建
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 app_new_fm_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	new_fm_ctrl		*ctrl_para;
	#if 0 //检查内存是否泄漏
		esKSRV_SysInfo();								//查看系统内存消息是否泄露或溢出
	#endif
	ctrl_para = (new_fm_ctrl *)My_Balloc( sizeof(new_fm_ctrl));
	eLIBs_memset(ctrl_para, 0, sizeof(new_fm_ctrl));

	ctrl_para->font			=	para->font;
	ctrl_para->root_type	=	para->root_type;//TF卡
	/****************************开始配置manwin窗口*******************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name	=	APP_NEWFM;
	create_info.hParent	=	para->h_parent;
	create_info.id		=	APP_NEWFM_ID;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_new_fm_proc);
	create_info.attr	=	(void *)ctrl_para;

	return (GUI_ManWinCreate(&create_info));
}


/*
************************************************************************************************************************
*                       				app_new_fm_manwin_delete
*
*Description: fm的manwin窗口删除
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 app_new_fm_manwin_delete(__gui_msg_t *msg)
{
	new_fm_ctrl	*this;

	this = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);

	if(this->fm_framewin){//先删除子窗口
		GUI_FrmWinDelete(this->fm_framewin);
		__wrn("fm_framewin is delete success...\n");
		this->fm_framewin = 0;
	}

	if(this->fm_lyr){//接着删除子窗口图层
		GUI_LyrWinDelete(this->fm_lyr);
		this->fm_lyr = 0;
		__wrn("fm_lyr is delete success...\n");
	}

	if(this){//最后清除本主窗口数据
		eLIBs_memset(this, 0, sizeof(new_fm_ctrl));
		My_Bfree(this, sizeof(new_fm_ctrl));
	}

}






