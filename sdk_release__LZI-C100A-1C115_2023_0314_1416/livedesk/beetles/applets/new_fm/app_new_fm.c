#include	"app_new_fm.h"
#include	"new_fm.h"


typedef struct	{//λ��ṹ��
	
  	__u32 micPreAmpGainCtl	: 3;	//B2-B0����: 3������˼���������ֻռ��3��λ
	
  	__u32 micPreAmpEnBit		: 1;//B3
	__u32 FMtoLRMirerGain 		: 3;//B6-B4 
	__u32 paspreedselect		: 1;//b7
	__u32 adcInputSrcSel	  : 6;	//b13-b8. adc input source
	__u32 CosSlopTimeForAntipop:2;	//b15-b14
	__u32 adcInputGainCtl	  : 3;	//b18-16, adc input gain
  	__u32 reserved4			: 2;	//b20-19
  	__u32 LineintoLRMirerGain : 3;	//b23-21
  	__u32 MictoLRMirerGain	: 3; 	//b26-24
  	__u32 reserved5			: 4;	//b30-b27
  	__u32 adcEnBit			: 1;	//b31
}AdcAnaCtl1;

//////////////////////////////////////////////////////////////////////////
// ����ͼ��
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)

__s32 g_is_searching1 = 0;



//���ڵ�����ͺ���
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
	msg.h_deswin = hparent;//������
	msg.h_srcwin = hwin;//Դ����
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data2;
	msg.dwReserved = reserved;//���USB��TF���л�
	GUI_SendNotifyMessage(&msg);
	return EPDK_OK;
}

//������Ƶ���������
__s32 __dsk_audio_set_linein_gain(__s32 value)
{
	volatile AdcAnaCtl1* pAdcAnaCtl = (volatile AdcAnaCtl1*)(0x24 + 0Xf1C23C00);
	if(value>7||value<0)
		return -1;
	pAdcAnaCtl->LineintoLRMirerGain	=value;
	return 0;

}

/************************************************************************/
// ѡ��ĳ��Ƶ��
//��Ϊ�������˳�һ�����sel_idҪ��1
/************************************************************************/
static __s32 _new_fm_change_channelon()
{

}
#if 0
//��ǰ�ҵ�������ЧƵ��
static void on_new_fm_test_freq_fail(NEW_FM_CTRLDATA_T *fm_ctrl)
{
	if(fm_ctrl->search_mode == NEW_SRHMODE_MANUAL)//�ֶ�����
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
		__wrn("draw:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
	}
	else//SRHMODE_AUTO  �Զ�����
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
		__wrn("fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
	}
}

//��������
static void on_new_fm_test_freq_end(NEW_FM_CTRLDATA_T *fm_ctrl)
{
	dsk_radio_chan_t chan_info;

	if(fm_ctrl->search_mode == NEW_SRHMODE_MANUAL)//�ֶ�����
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq); //by cky modify
		//dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
		//fm_ctrl->cur_freq = chan_info.freq;
		__wrn("fm_ctrl->channel_id = %d,fm_ctrl->cur_freq = %d\n", fm_ctrl->channel_id, fm_ctrl->cur_freq);
	}
	else//SRHMODE_AUTO  �Զ�����
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
	//�����б�
	{
		reg_fm_para_t *para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
		dsk_radio_rcv_save_search_result(); //save to reg
		//fm_reg_set_channel_count(para, fm_ctrl->channel_count);
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
	fm_ctrl->search_mode = NEW_SRHMODE_IDLE;
}
#endif

#if 1 //�������
/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm�������ɹ�����//��ǰ�ҵ�һ����ЧƵ��
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
	if(fm_ctrl->search_mode == NEW_SRHMODE_MANUAL)//�ֶ�����
	{
		dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//by cky
		__wrn("draw:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
	}
	else//SRHMODE_AUTO  �Զ�����
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

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm������ʧ�ܺ���//��ǰû���ҵ�һ����ЧƵ��
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static void on_new_fm_test_freq_fail(NEW_FM_CTRLDATA_T *this)
{
	if(this->search_mode == NEW_SRHMODE_MANUAL)
	{
		dsk_radio_rcv_get_cur_freq(&this->cur_freq);//��ȡ---���������ն˵�ǰ�Ĳ���Ƶ��
		__wrn("@cur_freq = %d\n",this->cur_freq);
	}
	else
	{
		dsk_radio_rcv_get_cur_freq(&this->cur_freq);//��ȡ---���������ն˵�ǰ�Ĳ���Ƶ��
		__wrn("@cur_freq = %d\n",this->cur_freq);
	}
}
#endif

//////////////////////////////////////////////////////////////////////////ͼ��״̬��
#define NEW_FM_CB_NEED_NOTIFY_FRW(_p) (_p && _p->fm_lyr && GUI_LyrWinGetSta(_p->fm_lyr) == GUI_LYRWIN_STA_ON)


#if 1	//��������ص�����
/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm��---�����ɹ�---�ص�������
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
		on_new_fm_test_freq_ok(&wnd_para->fm_ctrl);	//�����ɹ�
		__wrn("draw:fm_ctrl->cur_freq = %d\n", wnd_para->fm_ctrl.cur_freq);
	}

	if(NEW_FM_CB_NEED_NOTIFY_FRW(wnd_para))			//ͼ��״̬�Ƿ��
	{
		NOTIFY_MSG(NEW_CMD_AUTOSRH_FINDCH, hwnd, wnd_para->fm_framewin, 0, 0, 0);//ͨ��MSG��������Ϣ��framewin���ڣ�����--�Զ������ɹ�--�������---����UI����
	}

#if 1	//���� �� �ָ� ������ʱ��
	if(GUI_IsTimerInstalled(hwnd, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
	{
		__wrn("GUI_ResetTimer ---> timer reset...\n");
		GUI_ResetTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1, ID_TIMER_FM_TestPlayFreq_Speed_1, 0);//��ʱ����λ
	}
	else
	{
		__wrn("GUI_SetTimer ---> timer set...\n");
		GUI_SetTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1, ID_TIMER_FM_TestPlayFreq_Speed_1, 0);//������ʱ��
	}
	g_is_searching1 = 2;
	dsk_amplifier_onoff(1);//�򿪹���,����
#endif
	
	return 0;
}

/*
************************************************************************************************************************
*                       				new_cb_srhch_fail
*
*Description: ����fm��---����ʧ��--�ص�������
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 new_cb_srhch_fail(void *arg_p)
{
	new_fm_ctrl *wnd_para;
	H_WIN hwnd = (H_WIN)arg_p;
	NEW_FM_GetWndPara(wnd_para, new_fm_ctrl, hwnd);
	__wrn("*******new_cb_srhch_fail********\n");

	if(wnd_para)
	{
		on_new_fm_test_freq_fail(&wnd_para->fm_ctrl);//����ʧ�ܺ���
		__wrn("draw:fm_ctrl->cur_freq = %d\n", wnd_para->fm_ctrl.cur_freq);
	}

	if(NEW_FM_CB_NEED_NOTIFY_FRW(wnd_para))
	{
		NOTIFY_MSG(NEW_CMD_AUTOSRH_FINDCHFAIL, hwnd, wnd_para->fm_framewin, 0, 0, 0);//������Ϣ��framewin�Ӵ��ڣ��Զ�����ʧ������
	}

	return 0;
}
/*
************************************************************************************************************************
*                       				new_cb_srhch_fail
*
*Description: ����fm��---������ʱ--�ص�������
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 new_cb_srhch_over(void *arg_p)
{
	new_fm_ctrl *wnd_para;
	H_WIN hwnd = (H_WIN)arg_p;
	__s32 auto_close_scn_time;
	NEW_FM_GetWndPara(wnd_para, new_fm_ctrl, hwnd);
	__wrn("cb_srhch_over():begin\n");
#if SP_FM_WhenAutoSearch_PlayFreq

	if(GUI_IsTimerInstalled(hwnd, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
	{
		GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1);
		__wrn("GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq);\n");
	}

#endif
/*
#if TIME_FMAutoCloseScreen
	g_set_close_scn_time(TIME_FMAutoCloseScreen);
#else
	auto_close_scn_time = get_auto_close_scn_time_fm();
	__wrn("auto_close_scn_time=%d\n", auto_close_scn_time);

	if(auto_close_scn_time > 0)//���������������FM���Զ�����ʱ��
	{
		g_set_close_scn_time(auto_close_scn_time);
	}
	else
	{
		//�ָ�ϵͳ�Զ�����ʱ��
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			g_set_close_scn_time(para->backlightoff);
			__msg("para->backlightoff=%d\n", para->backlightoff);
		}
	}

#endif*/

	if(wnd_para)
	{
		//on_new_fm_test_freq_end(&wnd_para->fm_ctrl);
	}

	if(NEW_FM_CB_NEED_NOTIFY_FRW(wnd_para))
	{
		NOTIFY_MSG(NEW_CMD_AUTOSRH_OVER, hwnd, wnd_para->fm_framewin, 0, 0, 0);//������Ϣ��framewin�Ӵ��ڣ������Զ������������
	}

	SEND_MSG(DSK_MSG_SCREEN_OPEN, hwnd, GUI_WinGetHandFromName("init"), 0, 0, 0);//������Ϣ��init��Ŀ¼������Ļ����
	g_is_searching1 = 0;
	__wrn("cb_srhch_over():end\n");
	return 0;
}
#endif

/*
************************************************************************************************************************
*                       				__new_fm_auto_search_start
*
*Description: ����fm���Զ���������
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
	dsk_radio_get_search_flag(&search_flag);	//��ȡ������־
	__wrn("search_flag = %d:\n",search_flag);

	if(1 == search_flag)						//�������������в������жϣ���������������ܽ���
	{
		__wrn("__new_fm_auto_search_start:return\n");
		return EPDK_OK;
	}
	else
	{
		__wrn("__new_fm_auto_search_start:\n");
		fm_ctrl->search_mode = NEW_SRHMODE_AUTO;					//��ֵ����ģʽΪ�Զ�����ģʽ
		dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_AUTO);		//����Ϊ�Զ�ģʽ
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);	//����---�����б�����
		dsk_radio_rcv_search_all(0, 0);								//���������ն�---��������Ƶ�ʺ�ͨ��
		esKRNL_TimeDly(5);//��ʱ5*10ms
		//SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);//���͹ر���Ļ��Ϣ���ini��Ŀ¼
		return EPDK_OK;
	}
}
/*
************************************************************************************************************************
*                       				__new_fm_manual_search_start
*
*Description: ����fm���ֶ���������
*
*Arguments  : 
*             way��0
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_manual_search_start(NEW_FM_CTRLDATA_T *fm_ctrl, __u32 way)
{
	__u32 search_flag;
	static __s32 freq;
	dsk_radio_get_search_flag(&search_flag);	//��ȡ������־
	__wrn("search_flag = %d:\n",search_flag);
	
	if(1 == search_flag)						//�������������в������жϣ���������������ܽ���
	{
		__wrn("__new_fm_auto_search_start:return\n");
		return EPDK_OK;
	}
	else
	{
		__wrn("__new_fm_auto_search_start:\n");
		fm_ctrl->search_mode = NEW_SRHMODE_MANUAL;					//��ֵ����ģʽΪ�ֶ�����ģʽ
		dsk_radio_set_manual_way(way);								//�����ֶ�;����0Ϊ����������1Ϊ��������
		dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_MANUAL);		//����Ϊ�ֶ�ģʽ
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);	//����---�����б�����
		__wrn("fm_ctrl->cur_freq = %d, fm_ctrl->channel_id+1 = %d\n",fm_ctrl->cur_freq, fm_ctrl->channel_id+1);
	
		dsk_radio_rcv_search_all(fm_ctrl->cur_freq, fm_ctrl->channel_id+1);	//�������е�Ƶ�ʺ�ͨ��
		//SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);//���͹ر���Ļ��Ϣ���ini��Ŀ¼
		return EPDK_OK;
	}

}

//����Ƶ��
static __s32 __new_fm_set_band(NEW_FM_CTRLDATA_T *fm_ctrl, dsk_radio_band_mode_t mode)
{
	dsk_radio_set_band(mode);						//����Ƶ��
	dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);	//��ȡ��СƵ��
	dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);	//��ȡ���Ƶ��
	__wrn("freq range = [%d, %d]\n", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
	return EPDK_OK;
}

#if 1//radioģ���ʼ��

/*
************************************************************************************************************************
*                       				__new_fm_init_date
*
*Description: ����fm�ĳ�ʼ�����ݺ���
*
*Arguments  : fm_ctrl������������ݽṹ��
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_init_date(NEW_FM_CTRLDATA_T *fm_ctrl)
{
	fm_ctrl->search_mode = NEW_SRHMODE_IDLE;			//��������ģʽΪ��
	
	if(dsk_radio_is_open())
	{
		dsk_radio_chan_t chan_info;
		reg_fm_para_t	*para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);//��ȡע���ָʾ��
		fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();//��ȡ---��Ч���������--Ƶ������
		__wrn("channel_count = %d...\n",fm_ctrl->channel_count);
		
		if(fm_ctrl->channel_count <= 0)//���û����������Ч���
		{
			fm_ctrl->channel_id = 0;
			fm_ctrl->cur_freq = NEW_MIN_CHANNEL_FREQ;
			fm_ctrl->channel_count = 0;
			ZeroMemory(para, sizeof(reg_fm_para_t));//�ṹ���ʼ������0
			//fm_reg_set_sel_channel_id(para, 0);
			// 			fm_reg_set_channel_(para, 0, fm_ctrl->cur_freq); //����Ĭ��һ��Ƶ��Ƶ��
			// 			fm_reg_set_channel_count(para, fm_ctrl->channel_count);
			
			dsk_radio_rcv_get_search_result(para);					 //���б��ļ����ȡ��������Ƶ���б�
			{
				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);//���ò���Ƶ��

				dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);//���ò���Ƶ��-100
			}

			dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
		}
		else	//�Ѿ�����������Ч�Ľ��
		{
			__wrn("channel_count is not null...\n");
			/*if(ValueBetweenRange(NEW_MIN_CHANNEL_FREQ, fm_reg_get_curfreq(para), NEW_MAX_CHANNEL_FREQ)
				&& (fm_reg_get_channel_count(para) == 1) && (fm_ctrl->channel_count == 1)
				&& (fm_reg_get_channel_(para, 0) == NEW_MIN_CHANNEL_FREQ))
			{
				//�������ֶ�����Ƶ��(û������)
				fm_ctrl->cur_freq = fm_reg_get_curfreq(para);	//��ȡ��ǰƵ��
				__wrn("fm_init:fm_ctrl->cur_freq = %d\n", fm_ctrl->cur_freq);
				__wrn("fm_init:fm_ctrl->channel_count  = %d\n", fm_ctrl->channel_count);

				if((fm_ctrl->cur_freq < NEW_MIN_CHANNEL_FREQ) && (fm_ctrl->cur_freq > NEW_MIN_CHANNEL_FREQ))
				{
					return EPDK_FAIL;
				}
				else
				{
					//if(87500 == MIN_CHANNEL_FREQ) //ֻ���4702��FM play3��������������
					{
						dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);
						dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
					}

					dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
				}
			}*/
		}
		//////////////Ƶ�ʷ�Χ��87MHz~108MHz////////////////////
		dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);//��ȡ��СƵ�ʣ�87000
		dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);//��ȡ���Ƶ�ʣ�108000
		__wrn("freq range = [%d, %d]\n", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
	}
}

/*
************************************************************************************************************************
*                       				__new_fm_init_radio_module
*
*Description: ����fm��radioģ���ʼ������
*
*Arguments  : cb_arg������Ĵ��ھ��
*             fm_ctrl������������ݽṹ��
*
*Return     : ����EPDK_OK
*
************************************************************************************************************************
*/
static __s32 __new_fm_init_radio_module(void *cb_arg, NEW_FM_CTRLDATA_T *fm_ctrl)
{
	if(!dsk_radio_is_open())//������radioģ��û�д�
	{
		reg_fm_para_t	*para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);	//��ȡע���
		__wrn("radio_open is start...\n");
		dsk_radio_open();									//��radioģ��
		__wrn("radio_open is ok...\n");
		dsk_radio_rcv_open();								//���������ն�ģ��---��--->�ᴴ���Զ������߳�
		__wrn("radio_rcv_open is ok...\n");
		dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);			//����������Ƶ�� Ϊ����Ƶ��
		__wrn("radio_set_band is ok...\n");
		dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);	//�����������ź�ǿ������ֵ ������ֵ Ϊ�е�
		__wrn("radio_set_threshold is ok...\n");
		dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);	//��������������Ƶģʽ Ϊ������
		__wrn("radio_set_audio_mode is ok...\n");

#ifdef __MSG//����
		{
			//for debug
			__s32 i, num = 0;

			for(i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				__wrn("FM_channel[%d]:%d\n", i, para->FM_channel[i]);

				if(0 != para->FM_channel[i])
				{
					num++;//��¼��Ч��Ƶ��
				}
			}

			__wrn("valid_num = %d, sel channel_id:%d\n", num, para->channel_id);
		}
#endif

		dsk_radio_rcv_get_search_result(para);						//���б��ļ����ȡ��������Ƶ���б�
		__wrn("radio_rcv_get_search_result is ok...\n");
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);	//����---�����б�����
		__wrn("radio_rcv_set_play_list_type is ok...\n");
	}
	__wrn("dsk_radio_set_cb is start...\n");
	dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_SUCCESS, esKRNL_GetCallBack((__pCBK_t)new_cb_srhch_success), cb_arg);	//��������������Ƶ�ʣ���ת�����ɹ��ص�������
	__wrn("dsk_radio_set_cb is start...\n");
	dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_FAIL, esKRNL_GetCallBack((__pCBK_t)new_cb_srhch_fail), cb_arg);			//��������������Ƶ�ʣ���ת����ʧ�ܻص�������
	__wrn("__new_fm_init_date is start...\n");
	__new_fm_init_date(fm_ctrl);							//��ʼ������
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__new_fm_uninit_module
*
*Description: ����fm��radioģ�鲻��ʼ���������ر�radioģ��
*
*Arguments  : cb_arg������Ĵ��ھ��
*             fm_ctrl������������ݽṹ��
*
*Return     : ����EPDK_OK
*
************************************************************************************************************************
*/
static __s32 __new_fm_uninit_radio_module(void)
{
	__wrn("uninit radio moudle start...\n");
	if(dsk_radio_is_open())					//������radioģ���
	{
		__wrn("uninit radio moudle under way...\n");
		if(dsk_radio_rcv_is_open())			//�����ն�---�Ƿ��
		{
			dsk_radio_rcv_search_stop();	//���������ն�--����ֹͣ
			dsk_radio_rcv_close();			//���������ն�ģ��---�رա��ر��Զ������߳�
		}
		dsk_radio_close();					//������radioģ��ر�
	}
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__new_fm_next_freq_play
*
*Description: ����fm��radioģ�� ����---��һ��Ƶ�� ����
*
*Arguments  : null
*             
*
*Return     : ����EPDK_OK
*
************************************************************************************************************************
*/
static __s32 __new_fm_next_freq_play(void)
{
	__u32 cur_freq = 0;
	__wrn("next freq play...\n");
	dsk_radio_rcv_get_cur_freq(&cur_freq);//
	__msg("cur_freq 0 = %d\n", cur_freq);
	
	dsk_radio_rcv_next_freq_play();			//���������ն�---������һ��Ƶ��Ƶ��
	
	dsk_radio_rcv_get_cur_freq(&cur_freq);	//��ȡ---���������ն˵�ǰ�Ĳ���Ƶ��
	__msg("cur_freq 1 = %d\n", cur_freq);
	dsk_radio_rcv_set_freq_play(cur_freq);	//����---���������ն�--���ŵ�ǰƵ��

	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__new_fm_init_radio_module
*
*Description: ����fm��radioģ�� ����---��һ��Ƶ�� ����
*
*Arguments  : null
*             
*
*Return     : ����EPDK_OK
*
************************************************************************************************************************
*/
static __s32 __new_fm_pre_freq_play(void)
{
	__u32 cur_freq = 0;
	__wrn("prev freq play...\n");
	dsk_radio_rcv_get_cur_freq(&cur_freq);//
	__msg("cur_freq 0 = %d\n", cur_freq);
	
	dsk_radio_rcv_pre_freq_play();			//���������ն�---������һ��Ƶ��Ƶ��
	
	dsk_radio_rcv_get_cur_freq(&cur_freq);	//��ȡ---���������ն˵�ǰ�Ĳ���Ƶ��
	__msg("cur_freq 1 = %d\n", cur_freq);
	dsk_radio_rcv_set_freq_play(cur_freq);	//����---���������ն�--���ŵ�ǰƵ�ʣ������������ǰ���һ��Ƶ������Ϊ��ǰ����Ƶ��

	return EPDK_OK;
}

#endif

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm��framewin���ڳ�����������
*
*Arguments  : 
*            
*
*Return     : ����EPDK_OK\EPDK_FALSE
*
************************************************************************************************************************
*/
static __s32  app_new_fm_scene_create(__gui_msg_t *msg)
{
	new_fm_ctrl		*this;
	RECT	fm_rect;
	fm_create_para_t	fm_create;
	this = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
	eLIBs_memset(&fm_create, 0, sizeof(fm_create_para_t));
	//////////fm��framewinͼ���С///////////
	fm_rect.x	= 0;
	fm_rect.y	= 0;
	fm_rect.width	= 1024;
	fm_rect.height	= 600;
	fm_layer_create(this->fm_lyr, &fm_rect, "new_fm_lyr"); //����ͼ��
	////////////////////////////////////////
	if(!this->fm_lyr) {
		__wrn("fm_lyr is null...\n");
		return EPDK_FALSE;
	} else {
		__wrn("fm_lyr is create success...\n");
	}
	fm_create.fm_frmlyr = this->fm_lyr;
	fm_create.fm_frmwin = msg->h_deswin;	//��ֵ���ھ��
	fm_create.fm_ctrl1  = &this->fm_ctrl;
	this->fm_framewin = new_fm_frmwin_create(msg->h_deswin,  &fm_create);//framewin���ڴ�������
	//_app_show_focus_wnd(fm_ctrl->fm_framewin, fm_ctrl->fm_lyr);
	//GUI_LyrWinSetTop(fm_ctrl->fm_lyr);
	GUI_WinSetFocusChild(this->fm_framewin);//���Ͱ�����������Ϣ��framewin�Ӵ���
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_proc
*
*Description: fm��manwin���ڴ����ص�������
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
		case GUI_MSG_CREATE:{	//0������
			new_fm_ctrl	*wnd_para;
			//FM_GetWndPara(wnd_para, new_fm_ctrl, msg->h_deswin);//��ȡATTr����
			wnd_para = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
			__wrn("new fm is start create\n");
			esPWM_LockCpuFreq();//��ֹCPU�Զ�����Ƶ�ʣ���ֹFM����
			//���õ�ɫ��
			com_set_palette_by_id(ID_FM_COLOURTABLE_BMP);
			//��ʼ��radioģ��
			__new_fm_init_radio_module((void *)msg->h_deswin, &wnd_para->fm_ctrl);//radioģ���ʼ��
			__wrn("__new_fm_init_module create\n");
			app_new_fm_scene_create(msg);									//����FM�Ӵ���

			gscene_bgd_set_state(BGD_STATUS_SHOW);							//����ӫ������ʾ״̬����ʾ״̬
			//��ͼ��״̬
			//_app_show_focus_wnd(wnd_para->fm_framewin, wnd_para->fm_lyr);	//ͼ��״̬��
			//GUI_LyrWinSetTop(wnd_para->fm_lyr);								//ͼ���ö�

			/*{
				ES_FILE *paudio;
				__u32 ret;
					paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");//����Ƶ�ļ���������������
				
					if(!paudio)
					{
						__wrn("Open Audio Dev File Failed\n");
						ret = EPDK_FAIL;
					//	break;
					}
				
					eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_START, 0xff, 0);//�ļ�����������������Ƶ�豸
					eLIBs_fclose(paudio);//�ر���Ƶ�豸�ļ�
					__dsk_audio_set_linein_gain(2);//������Ƶ������
			}*/
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	//1������
			__wrn("new fm is start  destroy\n");
			esPWM_UnlockCpuFreq();//����CPU�Զ�����Ƶ��
			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1);
				__wrn("GUI_KillTimer(hwnd, ID_TIMER_FM_TestPlayFreq_1);\n");
			}
			__new_fm_uninit_radio_module();//�ر�radioģ��
			app_new_fm_manwin_delete(msg); //ɾ��framewin����
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{	//2������

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{		//4���˳�
			__wrn("quit app_new_fm manwin...\n");
			app_new_fm_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);			
		}
		return EPDK_OK;

		case MSG_NEW_FM_OP_ENTER:{//�س����£������Զ�����
			new_fm_ctrl *this;
			this = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
			if(msg->dwAddData2 == 1){	//�������Զ�����
				__new_fm_auto_search_start(&this->fm_ctrl);//�Զ�������ʼ
			}
			else{	//�̰����ֶ�����
				__new_fm_manual_search_start(&this->fm_ctrl, 0);//�ֶ�������ʼ
			}
			__wrn("g_is_searching1 = %d...\n", g_is_searching1);
			/*if(g_is_searching1)
			{
				__wrn("autosearch start...\n");
				dsk_radio_rcv_autosearch_resume();//�ָ��Զ�����
				dsk_radio_rcv_search_stop();//��ͣ����
				//fm_ctrl->cur_freq = 93900;
				//dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
				break;
			}*/
		}
		return EPDK_OK;

		case MSG_NEW_FM_OP_RIGHT:{//��һ�װ��£�������һ��Ƶ��
				__new_fm_next_freq_play();//������һ��Ƶ��
				//__new_fm_manual_search_start(fm_ctrl, 1);//�ֶ�������ʼ
		}
		return EPDK_OK;
		
		case MSG_NEW_FM_OP_LEFT:{//��һ�װ��£�������һ��Ƶ��
			__new_fm_pre_freq_play();//������һ��Ƶ��
			//__new_fm_manual_search_start(fm_ctrl, 0);//�ֶ�������ʼ
		}
		return EPDK_OK;
		
		
		case GUI_MSG_KEY:{		//5������
			
		}
		break;
		
		case GUI_MSG_COMMAND:{	//7������                                   
			switch(LOWORD(msg->dwAddData1))
			{
				case NEW_FM_ID:	//framewin����ID����˼�������˵�ʱ�����ִ������ĳ���
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case MSG_NEW_FM_OP_EXIT:{//�˳�Ӧ�õ�home����
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		  = GUI_MSG_QUIT;
							GUI_SendMessage(&mmsg);//�򱾴��ڻ�ָ�����ڷ�����Ϣ����
						}
						break;

						case MSG_NEW_FM_OP_ENTER:{//���Զ�����
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		  = MSG_NEW_FM_OP_ENTER;
							mmsg.dwAddData2 = msg->dwAddData2;// �ֶ�/�Զ�����ģʽ�л���1�Զ���2�ֶ�
							GUI_SendMessage(&mmsg);
						}
						break;

						case MSG_NEW_FM_OP_RIGHT:{//��һ��Ƶ�ʲ���
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		  = MSG_NEW_FM_OP_RIGHT;
							GUI_SendMessage(&mmsg);
						}
						break;

						case MSG_NEW_FM_OP_LEFT:{//��һ��Ƶ�ʲ���
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		  = MSG_NEW_FM_OP_LEFT;
							GUI_SendMessage(&mmsg);
						}
						break;
					}
				}
			}
		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{	//19����ʱ��
					new_fm_ctrl	*hwnd;
					hwnd = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
					
					__wrn("ID_TIMER_FM_TestPlayFreq_1 timer0...\n");
					if(msg->dwAddData1 == ID_TIMER_FM_TestPlayFreq_1)//������һ����Ч��Ƶ��
					{
						__wrn("ID_TIMER_FM_TestPlayFreq_1 timer\n");
						if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1) == EPDK_TRUE)
						{
							GUI_KillTimer(msg->h_deswin, ID_TIMER_FM_TestPlayFreq_1);
						}
						g_is_searching1 = 1;
						dsk_amplifier_onoff(0);				//�رչ�������
						dsk_radio_rcv_autosearch_resume();	//�ָ��Զ�����,��������
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
*Description: fm��manwin���ڴ���
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
	#if 0 //����ڴ��Ƿ�й©
		esKSRV_SysInfo();								//�鿴ϵͳ�ڴ���Ϣ�Ƿ�й¶�����
	#endif
	ctrl_para = (new_fm_ctrl *)My_Balloc( sizeof(new_fm_ctrl));
	eLIBs_memset(ctrl_para, 0, sizeof(new_fm_ctrl));

	ctrl_para->font			=	para->font;
	ctrl_para->root_type	=	para->root_type;//TF��
	/****************************��ʼ����manwin����*******************/
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
*Description: fm��manwin����ɾ��
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

	if(this->fm_framewin){//��ɾ���Ӵ���
		GUI_FrmWinDelete(this->fm_framewin);
		__wrn("fm_framewin is delete success...\n");
		this->fm_framewin = 0;
	}

	if(this->fm_lyr){//����ɾ���Ӵ���ͼ��
		GUI_LyrWinDelete(this->fm_lyr);
		this->fm_lyr = 0;
		__wrn("fm_lyr is delete success...\n");
	}

	if(this){//������������������
		eLIBs_memset(this, 0, sizeof(new_fm_ctrl));
		My_Bfree(this, sizeof(new_fm_ctrl));
	}

}






