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
#include "progressbar/progressbar.h"//��������ʾͷ�ļ�
#include "spsc_sub_ctrl_menu/new_movie_spsc_ctrl_menu.h"

#define TIMEROUT_ID_TIMER_HLYR_FF_FB		400
#if TIMEROUT_ID_TIMER_HLYR_FF_FB
#define ID_TIMER_HLYR_FF_FB			0x5555
#endif



static new_movie_ctrl_t *g_new_movie_ctrl = NULL;
static __s32 is_touch_ff_rr_valid = 0;//�����жϵ�ǰ�Ƿ���Խ��д����������
static __krnl_event_t *g_new_movie_robin_sem = NULL;/*robin�����ʼ����־*/

static __s32 g_new_is_on_tv = 0;/*Ϣ���ͣ�������־*/

//static __s32 new_movie_playend_scene_create(new_movie_ctrl_t *new_movie_ctrl);


static __s32 new_movie_playerror_scene_create(new_movie_ctrl_t *new_movie_ctrl);


#define GUI_MSG_SET_TVOUT_TP_HOOK_MSG (GUI_MSG_USER_DEF+1)

static __s32 new_movie_save_breakpoint_info(new_movie_ctrl_t *new_movie_ctrl);





/*��ȡ��ʾ��������ͣ���NONE,LCD,TV,HDMI,VGL�������ģʽ*/
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
/*��Ƶ���Ʋ�����ʼ��*/
static __s32 new_movie_ctrl_para_init(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("new_movie_ctrl is null...\n");
		return EPDK_FAIL;
	}

	new_movie_ctrl->hdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r");	/*����ʾ�����ļ�*/

	if(!new_movie_ctrl->hdisp)
	{
		__msg("open disp fail...\n");
	}

	__wrn("open disp success...\n");
	#if 1	/*robinģ���*/
	{
		__s32 ret;
		__s32 screen_width, screen_height;
		RECT vedio_rect;
		robin_open_arg_t arg;
		arg.cedar_stop_mode = CEDAR_STOP_MODE_UNINSTALL_PLUGINS;	/*ֹͣ����ʱ,ж�����в��*/
		arg.switch_vplay_mode = CEDAR_FILE_SWITCH_VPLY_MODE_GAP;	/*2���ļ��л�*/
		arg.reserve_mem_size = 1024 * 1024;	//512*1024;				/*Ԥ��1M�ֽ�*/
		ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);				/*��robinģ��ӿ�*/

		if(EPDK_OK != ret)
		{
			__msg("robin_open fail...\n");
			return EPDK_FAIL;
		}

		__wrn("robin_open success...\n");
		//robin_clear_feedback_msgQ();
		dsk_display_get_size(&screen_width, &screen_height);/*ͼ��ߴ��С1024*600*/
		vedio_rect.x = 0;
		vedio_rect.y = 0;
		vedio_rect.width = screen_width;
		vedio_rect.height = screen_height;
		__wrn("vedio screen=%d %d %d %d", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
		new_movie_ctrl->hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);/*������Ƶͼ����Դ*/

		if(!new_movie_ctrl->hvedio_lyr)
		{
			__msg("robin_request_vedio_layer fail...\n");
			return EPDK_FAIL;
		}

		__wrn("robin_request_vedio_layer success...\n");
	}
	#endif
	
	#if 1	/*��ȡ��Ƶ�����ļ�λ��ָʾ���ĵ�ǰֵͷ����ʼ*/
	if(new_is_on_tv())
	{
		__u32 arg[3] = {0};
		arg[0] = new_movie_ctrl->hvedio_lyr;
		eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_START, 0, (void *)arg);
	}
	#endif

	#if 1	/*��ȡ��Ƶ�����ļ�λ��ָʾ���ĵ�ǰֵ�ײ�����*/
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
/*��Ƶ���Ʋ���ɾ��*/
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
		eLIBs_fioctrl(new_movie_ctrl->hdisp, DISP_CMD_VIDEO_STOP, 0, (void *)arg);//��Ƶ���ļ�����ֹͣ��ȡ
	}
	//�ͷ�ͼ��
	if(new_movie_ctrl->hvedio_lyr)
	{
		robin_release_video_layer(new_movie_ctrl->hvedio_lyr);
		new_movie_ctrl->hvedio_lyr = NULL;
	}
	//ɾ����ʾ����
	if(new_movie_ctrl->hdisp)
	{
		eLIBs_fclose(new_movie_ctrl->hdisp);
		new_movie_ctrl->hdisp = NULL;
	}

	return EPDK_OK;
}
/*�����Զ����Ŷ�ʱ��*/
static __s32 new_movie_install_auto_play_timmer(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	__wrn("new_movie_ctrl->auto_play_timmer_id = 0x%x\n",new_movie_ctrl->auto_play_timmer_id);
	if(!GUI_IsTimerInstalled(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id))/*��ʱ������*/
	{
		GUI_SetTimer(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id
		             , 200, NULL);//0.5�룬���ö�ʱ��
		__wrn("GUI_SetTimer timmer is 1...\n");
	}

	return EPDK_OK;
}
/*��λ�Զ����Ŷ�ʱ��*/
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
		               , 200, NULL);//0.5�룬��λ��ʱ��
	}
	else
	{
		__msg("timmer already install...\n");
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*ж���Զ����Ŷ�ʱ��*/
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
		GUI_KillTimer(new_movie_ctrl->new_movie_manager, new_movie_ctrl->auto_play_timmer_id);//ɾ����ʱ��
	}


	return EPDK_OK;
}

/*��Ƶ�ļ�����*/
static __s32 new_movie_play_file(new_movie_ctrl_t *new_movie_ctrl, __s32 index, __bool breakplay)
{
	__s32 ret,amplifier_flag;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__cedar_tag_inf_t tag_inf;
	ret = robin_npl_index2file(index, filename);//ͨ��index������ȡ��ǰ�����б�Ҫ���ŵ��ļ���

	if(-1 == ret)
	{
		__wrn("robin_npl_index2file fail...\n");
		return EPDK_FAIL;
	}

	//new_movie_ff_fb_over(new_movie_ctrl);
	__wrn("breakplay=%d\n", breakplay);
	__wrn("filename=%s\n", filename);

	{
		robin_set_cmd_stop();			/*����ֹͣ��������*/
		//robin_clear_feedback_msgQ();//��մ�����Ϣ��Ϣ����
		ret = robin_play_file(filename, NULL);	/*���ŵ�ǰ�����б���ļ�*/
		__wrn("ret=%d\n", ret);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}

		robin_set_cmd_play();			//���ÿ�ʼ��������
		__wrn("ret=%d\n", ret);

		amplifier_flag = dsk_amplifier_is_on();//��ȡ����״̬
		__wrn("amplifier_flag=%d\n", amplifier_flag);
	}

	if(EPDK_OK == ret)
	{
		__wrn("new_movie_play_file ret=%d\n", ret);
		new_movie_install_auto_play_timmer(new_movie_ctrl);//�����Զ����Ŷ�ʱ��
	}

	return ret;
}


/*��Ƶ�ļ���ʼ����*/
static __s32 new_movie_start_play(new_movie_ctrl_t *new_movie_ctrl, __bool breakplay)
{
	__s32 ret;
	__s32 index;
	index = robin_npl_get_cur();//��ȡ��ǰ�����б��Ҫ�����ļ�
	__wrn("new_movie_start_play  is index = %d...\n",index);

	if(-1 == index)
	{
		__wrn("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}
	__wrn(" new_movie_start_play is...\n");
	ret = new_movie_play_file(new_movie_ctrl, index, breakplay);/*������Ƶ�ļ�*/
	return ret;
}

/*ͨ���ļ�����id�����ļ�����Ĭ�ϴӵ�һ���ļ���ʼ����*/
static __s32 new_movie_play_for_index_or_default(__gui_msg_t *msg)
{
	root_para_t *para;
	static __s32 index = 0;
	para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);//��ȡ��root�д���manwin����ʱ����Ĳ�������
					
	#if 1//ͨ�� ��������ֱ�� ������Ƶ�ļ�
		index = robin_npl_get_cur();					//��ȡ��ǰ�����б�Ҫ���ŵĵ�ǰ�����ļ�����
		__wrn("save breakpoint the index = %d\n",index);
	
		if(para->data != -1)							//������-1��ʱ���Ǵ�new_explorer�б�����ת��new_movie���ŵ�
		{
			__wrn("para->data = %d\n",para->data);		//���ŵ��ļ�������
			index = para->data;							//��ȡ��Ҫ���ŵ��ļ�����
			__wrn(" index = %d\n",index);
			robin_npl_set_cur(index);					//���õ�ǰ�����ļ�
			para->data = -1;							//�����תapp��־λ
			
		}
		else											//����-1��ʱ����ֱ�Ӵ�home�������new_movie���ŵ�
		{
			if(index != 0)								/*Ĭ�ϴ�ͷ��ʼ����*/
			{
				index = 0;
				__wrn(" index 2= %d\n",index);
				/*�������ô�ͷ��ʼ�����ļ�*/
				robin_npl_set_cur(index);				/*���õ�ǰ�����б�Ҫ���ŵĵ�ǰ�����ļ�����id*/
			}

		}
	#endif
}
/*ֹͣ����*/
static __s32 new_movie_stop_play(new_movie_ctrl_t *new_movie_ctrl)
{
	__wrn(" new_movie_stop_play is...\n");
	robin_set_cmd_stop();/*����ֹͣ��������*/
	return EPDK_OK;
}
/*��ͣ����*/
static __s32 new_movie_pause_play(new_movie_ctrl_t *new_movie_ctrl)
{
	__wrn(" new_movie_pause_play is...\n");

	robin_set_cmd_pause();/*������ͣ��������*/
	return EPDK_OK;
}
//flag:0��ʾ�˳���explorer��1��ʾ�˳���home
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
	
	//__movie_pause_play(movie_ctrl);��Ƶ���޸��ˣ�Ҫ���ȡ�ϵ���Ϣ��ʱ��ֻ����play״̬�����Բ�����ͣ
	{
		__gui_msg_t msg;
		eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
		msg.id = GUI_MSG_CLOSE;//���͹ر�����
		msg.dwAddData1 = flag;
		msg.h_deswin = movie_ctrl->new_movie_manager;
		GUI_SendMessage(&msg);
	}
	__wrn("after __movie_exit_movie\n");
	return EPDK_OK;
}


/*����Ƿ񲥷Ŵ���*/
static __s32 new_movie_check_for_play_err(new_movie_ctrl_t *new_movie_ctrl)
{
	__s32 ret;
	ret = robin_get_feedback_msg();//��ȡ��Ϣ��Ϣ����

	if(ret < 0)
	{
		robin_clear_feedback_msgQ();//��մ�����Ϣ��Ϣ����
		esKRNL_TimeDly(100);
		__wrn("new_movie_check_for_play_err exit movie...\n");
		new_movie_exit_movie(new_movie_ctrl, 1);//�˳�Ӧ��,�ص�home����
		return EPDK_FAIL;
	}

	return EPDK_OK;
}
/*����Զ�����*/
static __s32 new_movie_check_for_auto_play(new_movie_ctrl_t *new_movie_ctrl)
{
	if(!new_movie_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__cedar_status_t fsm_sta;
		fsm_sta = robin_get_fsm_status();/*��ȡfsm״̬����״̬*/
		__wrn("fsm_sta= %d\n", fsm_sta);

		if(CEDAR_STAT_PLAY == fsm_sta
		    || CEDAR_STAT_PAUSE == fsm_sta)
		{
			new_movie_ctrl->cur_time = robin_get_cur_time();/*��ȡ��ǰ���ŵ�ʱ��*/
			robin_get_fsm_cur_file(new_movie_ctrl->cur_playing_file, sizeof(new_movie_ctrl->cur_playing_file));/*��ȡ��ǰ���ŵ��ļ���*/
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
				dsk_amplifier_onoff(1);//�򿪹���
				break;
			}
			case CEDAR_STAT_PAUSE:
			case CEDAR_STAT_STOP:{
				dsk_amplifier_onoff(0);//�رչ���
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

/*�л��������,��ʾ���������*/
static __s32 new_movie_switch_output(new_movie_ctrl_t *new_movie_ctrl)
{
	//�л����
	{
		reg_system_para_t *para;//ϵͳ����
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
/*�л������ӻ�����LCD��ʾ��*/
static __s32 new_movie_switch_to_tv_or_lcd(new_movie_ctrl_t *new_movie_ctrl)
{
	__wrn("new_movie_switch_to_tv_or_lcd is...\n");
	new_movie_switch_output(new_movie_ctrl);//�л���ʾ�������
	
	return EPDK_OK;
}
/*��ʼ���Ž��̵Ķ�ʱ����������Ļ��ʾ��������Ļ��ʾͬ�����ŵ�����*/
static __s32 new_movie_process_after_start_play(new_movie_ctrl_t *new_movie_ctrl)
{

	__wrn("new_movie_process_after_start_play is...\n");
	new_movie_install_auto_play_timmer(new_movie_ctrl);
	//new_movie_spsc_scene_create(new_movie_ctrl);//�ս�����Ƶʱ������ʾ���Ʋ˵�
	return EPDK_OK;
}

/*���ò���ģʽ*/
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
/*��������ģʽ*/
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
/*ע��������ʼ��*/
static __s32 __app_new_movie_reg_para_init(new_movie_ctrl_t *new_movie_ctrl)
{
	__app_new_movie_set_play_mode();
	__app_new_movie_set_zoom_mode();
	__wrn("__app_movie_reg_para_init is...\n");

	dsk_reg_set_app_restore_flag(REG_APP_MOVIE, 0);
	return EPDK_OK;
}


//����ģʽ
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

//���ڵ�����ͺ���
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
	msg.h_deswin = hparent;//������
	msg.h_srcwin = hwin;//Դ����
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data2;
	msg.dwReserved = reserved;//���USB��TF���л�
	GUI_SendNotifyMessage(&msg);
	return EPDK_OK;
}
/***********************************************************************************************************
*@Function	:          __s32 new_movie_robin_sem_init(void)
*@Description	: 		robin�����ʼ��
*						
*@Return     	: 	    EPDK_OK��EPDK_FAIL
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
__s32 new_movie_robin_sem_init(void)
{
	if(0 == g_new_movie_robin_sem){
		g_new_movie_robin_sem = esKRNL_SemCreate(1);//robin�������
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
*@Description	: 		robin���ж�ز���ʼ��
*						
*@Return     	: 	    EPDK_OK
*@Log			:		2023/2/6 create in start
************************************************************************************************************/
__s32 new_movie_robin_sem_deinit(void)
{
	if(g_new_movie_robin_sem){
		__u8 err;
		esKRNL_SemDel(g_new_movie_robin_sem, OS_DEL_ALWAYS, &err);//robin���ɾ��
		g_new_movie_robin_sem = NULL;
	}

	return EPDK_OK;
}

/*���ͼ�����*/
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
*@Description	: 		������������ʾframewin����
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
	{//������������ӵ�ͨ������appӦ�ó���framewin����
		__wrn("root_type = 0 \n");
		__wrn("progressbar_scene_create is... \n");
	}
	__wrn("progressbar_scene_create  is ok\n");

	
	eLIBs_memset(&para, 0, sizeof(movie_progressbar_create_para_t));
	para.hparent = new_movie_ctrl->new_movie_manager;
	para.scene_id = MOVIE_VOLUME_ID;

	new_movie_ctrl->progressbar_scene = Play_Progress_Bar_Scene_Frmwin_create(msg->h_deswin, &para);//��������ӵ�appӦ�ó��򴴽�framwin�Ӵ���
	//GUI_WinSetFocusChild(app_new_setting_ctrl->h_frm_general_setting);//ѡ��ָ�����ڽ�����Ϣ����ͣ�����ִ����Ҫ����
	#endif
	#if 0//�µĽ�����framewin����
		new_movie_ctrl_t	*new_movie_ctrl;
		movie_progressbar_create_para_t progressbar_createpara;
		RECT rect;
		SIZE screen_size;
		new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
		{//������������ӵ�ͨ������appӦ�ó���framewin����
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
	#if 1//ֱ�ӻ���
		new_movie_ff_fb_layer_draw(msg);
	#endif
	
	return EPDK_OK;

}

//�������Ʋ˵��ӳ���framewin����
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
		new_movie_spsc_ctrl_menu_scene_set_focus(scene_para->spsc_ctrl_scene);//������Ϣ����
		__wrn("new_movie_spsc_ctrl_menu_scene_set_focus\n");
	}

	return EPDK_OK;
}

//������framewin���ڴ���
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
		//movie_ctrl->progressbar_scene = movie_prog_scene_create(&create_para);//������framewin���ڴ���������
		movie_ctrl->progressbar_scene = progressbar_frmwin_create(&create_para);//������framewin���ڴ���
		
		if(NULL == movie_ctrl->progressbar_scene)
		{
			__wrn("__movie_progressbar_create fail...\n");
			return EPDK_FAIL;
		}
		progressbar_ctrl_scene_set_focus(movie_ctrl->progressbar_scene);//���Ͱ�����Ϣ�����������洰��ͼ��
	}

	return EPDK_OK;
}

//�������ܴ�����
static __s32 app_new_movie_key_proc(__gui_msg_t *msg)
{
	__s32 bok;// ������Ϣ�Ƿ��Ѵ�����ϣ�-1�������´�������ֱ������
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
			if(KEY_UP_ACTION == msg->dwAddData2)//��һ�ν���İ���ִ�й���
			{
				__wrn("last_key=%d\n", last_key);

				if(0)
				{}
				//if( GUI_MSG_KEY_LONGVIDEO == last_key )
				else if(GUI_MSG_KEY_LONGENTER == last_key)//�����س�������Ϣ��
				{
					//dsk_display_lcd_off();
					//__wrn("Tarring-----send Force close---------\n");
					//SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//����Ϩ��		
					bok = EPDK_OK;
				}
				else	if(GUI_MSG_KEY_TVOUT == last_key)
				{
					__wrn("before new_movie_switch_to_tv_or_lcd\n");
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);
					__msg("after new_movie_switch_to_tv_or_lcd\n");
				}
				else if(GUI_MSG_KEY_PLAY_PAUSE == last_key
				        || GUI_MSG_KEY_LONGPLAY_PAUSE == last_key)//��ͣ��������
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
				else if(GUI_MSG_KEY_ENTER == last_key)//�س���������
				{
					__wrn("GUI_MSG_KEY_ENTER is press...\n");
					/*if(new_movie_ctrl->m_ff_fb_speed)//����������
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
						//���û�е����˵�����enter��ͣ��ʼ����
						__s32 ret;
						ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						__wrn("GUI_MSG_KEY_ENTER is press 2...\n");

						if(EPDK_FALSE == ret)//���û�г���Ļ������ӳ�����������ͣ����������
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();	//��ȡfsm״̬����״̬

							if(CEDAR_STAT_PLAY == fsm_sta)		//����״̬
							{
								__wrn("before robin_set_cmd_pause\n");
								robin_set_cmd_pause();			//������ͣ����
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//����������ͣ
								__wrn("after robin_set_cmd_pause\n");
								//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
								bok = EPDK_OK;
							}
							else if(CEDAR_STAT_PAUSE == fsm_sta)//��ͣ״̬
							{
								__wrn("before robin_set_cmd_play\n");
								robin_set_cmd_play();//���ÿ�ʼ����
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);//����������
								__wrn("after robin_set_cmd_play\n");
								bok = EPDK_OK;
							}
						}
					}
				}
				else if(GUI_MSG_KEY_ESCAPE == last_key
				        || GUI_MSG_KEY_MENU == last_key//�����˳�����
				       )
				{
					__s32 ret;
					ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL
					           //                 & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID) & (~MOVIE_PROG_ID));

					if(EPDK_TRUE == ret)//����г���Ļ��playpause������ӳ��������˳�movie��ɾ�������ӳ���
					{
						
					}
					else//�˳�movie
					{
						if(new_is_on_tv())
						{
							new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//���ѽ���
						}
						else
						{
							//new_movie_exit_movie(new_movie_ctrl, 0);
						}
					}

					bok = EPDK_OK;
				}
			}
			else//��������İ�������ִ��
			{	
				__cedar_status_t sta;
				sta = robin_get_fsm_status();//��ȡfsm״̬����״̬
				__wrn("GUI_MSG_KEY sta=%d\n",sta);
				__wrn("msg->dwAddData1 is = %d ...\n",msg->dwAddData1);
				switch(msg->dwAddData1)
				{
					case GUI_MSG_KEY_REPEATE:
					case GUI_MSG_KEY_LONGMENU://�����˳�
					case GUI_MSG_KEY_SUBMENU:
					{
						__s32 ret;
						__here__;
						if(CEDAR_STAT_PLAY == sta)
						{
							__wrn("CEDAR_STAT_PLAY sta=%d\n",sta);
							robin_set_cmd_pause();
							app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//������ͣ
							//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
						}
						
						bok = EPDK_OK;
						break;
					}
					case GUI_MSG_KEY_RR://����
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_RR\n");

						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_rr();//���ͺ�������
						}

						bok = EPDK_OK;
						break;
					}

					case GUI_MSG_KEY_FF://���
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_FF\n");
						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_ff();//���Ϳ������
						}

						bok = EPDK_OK;
						break;
					}
					
					case GUI_MSG_KEY_MENU://�����˳�����
					{
						__wrn("GUI_MSG_KEY_MENU is press to new_movie_exit_movie...\n");
						new_movie_exit_movie(new_movie_ctrl, 1);//�˳���homeӦ��
						bok = EPDK_OK;
						break;
					}
					
					case GUI_MSG_KEY_RIGHT://�°��������
					case GUI_MSG_KEY_LONGRIGHT:
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_RIGHT send ff...\n");
						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_ff();//���Ϳ������
						}

						bok = EPDK_OK;
						break;
					}
					
					case GUI_MSG_KEY_LEFT://�ϰ���������
					case GUI_MSG_KEY_LONGLEFT:
					{
						__cedar_status_t sta;
						sta = robin_get_fsm_status();
						__wrn("GUI_MSG_KEY_LEFT send rr...\n");

						if(CEDAR_STAT_PLAY == sta)
						{
							robin_set_cmd_rr();//���ͺ�������
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
		if(KEY_UP_ACTION == msg->dwAddData2){//�����ɿ�������ȡ����һ�ν���İ���ִ�й���
			__wrn("last_key=%d\n", last_key);
			switch(last_key){
				case GUI_MSG_KEY_LONGENTER:{/*�����س�������Ϣ��*/
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//����Ϩ��		
					bok = EPDK_OK;
				}
				break;

				case GUI_MSG_KEY_TVOUT:{/*��ʾ��������л�*/
					__wrn("before new_movie_switch_to_tv_or_lcd\n");
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);
					__msg("after new_movie_switch_to_tv_or_lcd\n");
				}
				break;

				case GUI_MSG_KEY_PLAY_PAUSE:
				case GUI_MSG_KEY_LONGPLAY_PAUSE:{/*��ͣ��������/����*/
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_PLAY_PAUSE is press...\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						//new_movie_delete_subscene_by_id(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						robin_set_cmd_pause();//������ͣ����
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
					}
					else
					{
						//new_movie_delete_subscene_by_id(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
						robin_set_cmd_play();//���Ϳ�ʼ��������
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_play);
					}

					bok = EPDK_OK;
				}
				break;

				case GUI_MSG_KEY_ENTER:{/*�س���������*/
					__wrn("GUI_MSG_KEY_ENTER is press...\n");
					{
						//���û�е����˵�����enter��ͣ��ʼ����
						__s32 ret;
						ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
						__wrn("GUI_MSG_KEY_ENTER is press 2...\n");

						if(EPDK_FALSE == ret)//���û�г���Ļ������ӳ�����������ͣ����������
						{
							__cedar_status_t fsm_sta;
							fsm_sta = robin_get_fsm_status();	//��ȡfsm״̬����״̬

							if(CEDAR_STAT_PLAY == fsm_sta)		//����״̬
							{
								__wrn("before robin_set_cmd_pause\n");
								robin_set_cmd_pause();			//������ͣ����
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//����������ͣ
								__wrn("after robin_set_cmd_pause\n");
								//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
								bok = EPDK_OK;
							}
							else if(CEDAR_STAT_PAUSE == fsm_sta)//��ͣ״̬
							{
								__wrn("before robin_set_cmd_play\n");
								robin_set_cmd_play();//���ÿ�ʼ����
								app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);//����������
								__wrn("after robin_set_cmd_play\n");
								bok = EPDK_OK;
							}
						}
					}
					
				}
				break;

				case GUI_MSG_KEY_ESCAPE:
				case GUI_MSG_KEY_MENU:{/*�����˳�����*/
					__s32 ret;
					ret = 0;//new_movie_has_sub_scene(new_movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL
					           //                 & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID) & (~MOVIE_PROG_ID));

					if(EPDK_TRUE == ret)//����г���Ļ��playpause������ӳ��������˳�movie��ɾ�������ӳ���
					{
						
					}
					else//�˳�movie
					{
						if(new_is_on_tv())
						{
							new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//���ѽ���
						}
						else
						{
							//new_movie_exit_movie(new_movie_ctrl, 0);
						}
					}

					bok = EPDK_OK;

				}
				break;

				default:{/*����ֱ���˳���ǰ��switch���*/
					break;
				}

			}
		}
		else{//��������İ�������ִ��
			__cedar_status_t sta;
			sta = robin_get_fsm_status();//��ȡfsm״̬����״̬
			__wrn("GUI_MSG_KEY sta=%d\n",sta);
			__wrn("msg->dwAddData1 is = %d ...\n",msg->dwAddData1);
			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_REPEATE:
				case GUI_MSG_KEY_LONGMENU://�����˳�
				case GUI_MSG_KEY_SUBMENU:{
					__s32 ret;
					__here__;
					if(CEDAR_STAT_PLAY == sta)
					{
						__wrn("CEDAR_STAT_PLAY sta=%d\n",sta);
						robin_set_cmd_pause();
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//������ͣ
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
					}
					
					bok = EPDK_OK;
					
				}
				break;
				
				case GUI_MSG_KEY_RR:{//����
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_RR\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_rr();//���ͺ�������
					}
					bok = EPDK_OK;
					
				}
				break;

				case GUI_MSG_KEY_FF:{//���
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_FF\n");
					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_ff();//���Ϳ������
					}

					bok = EPDK_OK;
					
				}
				break;
				
				case GUI_MSG_KEY_MENU:{//�����˳�����
					__wrn("GUI_MSG_KEY_MENU is press to new_movie_exit_movie the home...\n");
					new_movie_exit_movie(new_movie_ctrl, 1);//�˳���homeӦ��
					bok = EPDK_OK;
				}
				break;

				case GUI_MSG_KEY_NUM1:{//����1�������˳���new explorer����
					__wrn("GUI_MSG_KEY_MENU is press to new_movie_exit_movie the new_explorer...\n");
					new_movie_exit_movie(new_movie_ctrl, 0);//�˳���new_explorerӦ��
					bok = EPDK_OK;
				}
				break;
				
				case GUI_MSG_KEY_RIGHT://�°��������
				case GUI_MSG_KEY_LONGRIGHT:{
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_RIGHT send ff...\n");
					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_ff();//���Ϳ������
					}
					//__movie_prog_scene_create(new_movie_ctrl);//ֱ�Ӵ���������framewin����

					bok = EPDK_OK;
					
				}
				break;
				
				case GUI_MSG_KEY_LEFT://�ϰ���������
				case GUI_MSG_KEY_LONGLEFT:{
					__cedar_status_t sta;
					sta = robin_get_fsm_status();
					__wrn("GUI_MSG_KEY_LEFT send rr...\n");

					if(CEDAR_STAT_PLAY == sta)
					{
						robin_set_cmd_rr();//���ͺ�������
					}

					bok = EPDK_OK;
					
				}
				break;

				default:{/*����ֱ���˳�switch���*/
					break;
				}
			}


		}
	#endif
		
			if(KEY_UP_ACTION == msg->dwAddData2){/*��ⰴ���ɿ�����Ϊ�ɿ�*/
				last_key = -1;
				__wrn("KEY_UP_ACTION is key up...\n");
			}
			else{/*����Ϊ���¶���*/
				last_key = msg->dwAddData1;
				__wrn("key is press...\n");
			}

			__wrn("__app_new_movie_proc: GUI_MSG_KEY end\n");

			if(EPDK_OK == bok){ //�����Ϣ�Ѵ����򷵻أ����򽻸��ӳ�������
				return EPDK_OK;
			}
			else{
				return EPDK_FAIL;
			}

}
/*��ƵappӦ�ó���Ļص�������*/
static __s32 __app_new_movie_proc(__gui_msg_t *msg)
{
	__wrn("__app_new_movie_proc: msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

	switch(msg->id)
	{
		case GUI_MSG_CREATE://��������
		{
			__s32 ret;
			new_movie_ctrl_t *new_movie_ctrl;
			
			
			#if 1	/*���ñ�����ɫ*/
				__here__;
				g_disable_close_scn();/*ʧ�ܹر�SCN(��Ļ��ӫ����)*/
				SEND_MSG(DSK_MSG_RELEASE_LOGO_BUF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);//֪ͨinit
				SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);//֪ͨinit
				__wrn("SEND_MSG(DSK_MSG_SCREENON_BUT_BKOFF)\n");
				com_set_backcolor(CONST_BackColor_Black);/*���ñ�����ɫ*/
				__here__;
			#endif
			
			gscene_hbar_set_state(HBAR_ST_HIDE);
			__here__;
			gscene_bgd_set_state(BGD_STATUS_HIDE);
			__wrn("__app_new_movie_proc: GUI_MSG_CREATE begin\n");
			new_movie_ctrl = esMEMS_Balloc(sizeof(new_movie_ctrl_t));/*�����ڴ�ռ�*/

			if(!new_movie_ctrl)
			{
				__wrn("mem not enough...\n");
				return EPDK_FAIL;
			}

			__here__;
			new_movie_robin_sem_init();/*robin�����ʼ��*/
			eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));	/*�ṹ�����ݳ�ʼ������0*/
			GUI_WinSetAddData(msg->h_deswin, (__u32)new_movie_ctrl);	/*���һ������*/
			new_movie_ctrl->new_movie_manager = msg->h_deswin;			/*���ڸ�ֵ*/
			
			#if 1	/*��Ƶͼ����Դ����*/
				new_movie_ctrl_para_init(new_movie_ctrl);				/*��Ƶ���Ʋ�����ʼ��*/
			#endif
			
			__app_new_movie_reg_para_init(new_movie_ctrl);				/*ע��������ʼ��*/
			__here__;
			#if 0	/*����������framewin����*/ 
				progressbar_scene_create(msg);
			#endif
			
			#if 1	/*��ɫ���������*/
				com_set_palette_by_id(ID_MOVIE_PAL_BMP);//���õ�ɫ��id
				__here__;
				{
					//�޸ĵ�ɫ��
					//����ɫ���Ұ�ɫ����Ļ�߿���ɫ
					__u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
					com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));//���õ�ɫ����ɫ
				}
				__here__;
			#endif
			
			__wrn("before new_movie_cur_file_has_breakpoint\n");
			
			#if 1	/*ͨ�� ��������ֱ�� ������Ƶ�ļ�*/
				new_movie_play_for_index_or_default(msg);
			#endif
			
			#if 1	/*��ʼ������Ƶ�ļ�*/
			{
				__wrn("new_movie_start_play is start...\n");

				//Ĭ���жϵ���ϵ㲥�ţ��޶ϵ����ͷ����
				{
					new_movie_start_play(new_movie_ctrl, NULL);//��ʼ����
					__here__;
					
				}
			}
			#endif
			
			#if 1	/*������framewin���ڴ�������*/
				__movie_progressbar_create(new_movie_ctrl);
			#endif
			
			#if 1	/*��Ƶ���ſ��Ʋ˵�framewin���ڴ�������*/
				__new_movie_spsc_ctrl_menu_scene_create(new_movie_ctrl);
			#endif
			
			#if 1	/*����/�������ų���ʱ��*/
				new_movie_process_after_start_play(new_movie_ctrl);//����ʼ���Ŷ�ʱ����
				__here__;
			#endif
			
			__wrn("__app_new_movie_proc: GUI_MSG_CREATE end\n");
			return EPDK_OK;
		}
		case GUI_MSG_PAINT://���ƴ���
		{
			
			return EPDK_OK;
		}
#if 1
		case GUI_MSG_KEY://��������
		{
			app_new_movie_key_proc(msg);//����������
			break;
		}
#endif 
		case GUI_MSG_CLOSE://�رգ��˳�����
		{
			if(1 == msg->dwAddData1)//�˳���home
			{
				__wrn("exit to home...\n");
				new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, SETTING_SW_TO_MAIN, 0);//�˳�movie��home����
			}
			else//�˳��� new explorer
			{
				__wrn("exit to explorer...\n");
				new_movie_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_MOVIE_SW_TO_NEW_EXPLORER, 2);//�˳�movie��new_explorerӦ�ó��򴰿�
			}


			return EPDK_OK;
		}
#if 1
		case GUI_MSG_TIMER://���ڶ�ʱ������
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
						else//������Ŵ��������¿�ʼ���
						{
							//new_movie_reset_auto_play_timmer(new_movie_ctrl);
							//new_movie_ctrl->cur_stop_cnt = 0;
						}
						
						cur_time = robin_get_cur_time();		/*��ȡ��ǰ�ļ��Ĳ���ʱ��*/
						__wrn("robin_get_cur_time = %d \n",cur_time);
						total_time = robin_get_total_time();	/*��ȡ��ǰ�����ļ�����ʱ��*/
						__wrn("robin_get_total_time = %d \n",total_time);
						if(cur_time == total_time)				//ʱ����ͬʱ��ͣ���ţ����˳�//�������,�˳�Ӧ��
						{
							static __s32 time_count=0;
							#if 1	/*����ѭ��������Ƶ�ļ�*/
								time_count = robin_npl_get_cur();/*��ȡ��ǰ�����б�Ҫ�����ļ�������id��*/
								__wrn("time_count = %d\n",time_count);
								
								total = robin_npl_get_total_count();	/*��ȡ��ǰ�����б�������ļ�����*/
								__wrn("file_total = %d\n",total);
								if(total < time_count){			//����������Ƶ֮���˳�Ӧ�ã��ص�home����
									time_count = 0;
									esKRNL_TimeDly(100);
									new_movie_exit_movie(new_movie_ctrl, 1);//1Ϊ�ص�homeӦ�ý���
								}
								else{
									time_count += 1;
									robin_npl_set_cur(time_count);	/*���õ�ǰ�����б��Ҫ�����ļ�����*/
									new_movie_start_play(new_movie_ctrl, 0);/*��ʼ�����ļ���0Ϊ�޶ϵ㲥��*/
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

			if(-1 == ret) //δ���������ӳ�������
			{
				break;
			}
			else//�Ѵ���
			{
				return EPDK_OK;
			}
		}
#endif
		case GUI_MSG_DESTROY://�ݻٴ��ڹ���
		{
			new_movie_ctrl_t *new_movie_ctrl;
			__wrn("__app_new_movie_proc: GUI_MSG_DESTROY begin\n");
			new_movie_ctrl = (new_movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(new_movie_ctrl)
			{
				if(new_is_on_tv())//�л���ʾ����
				{
					new_movie_switch_to_tv_or_lcd(new_movie_ctrl);//�л���ʾ����
				}

				__here__;
				#if 1//���ͷŶ�ʱ����֮�����ͷ�ͼƬ��Դ
					new_movie_uninstall_auto_play_timmer(new_movie_ctrl);//�Զ����Ŷ�ʱ���ͷ�
				#endif
				/*{
					//save breakpoint
					__s32 index;
					index = robin_npl_get_cur();
					__here__;

				}*/
				__here__;
				new_movie_stop_play(new_movie_ctrl);//ֹͣ���ţ�����ֹͣ��������
				__here__;
				
				#if 1//ɾ��������ͼ��,ɾ��ǰ��Ҫ��ֹͣ����
					progressbar_ui_scene_delete(new_movie_ctrl->progressbar_scene);
					if(new_movie_ctrl->progressbar_scene){//��Ҫ�����һ��������������framewin���ڵľ��
						__wrn("new_movie_ctrl->progressbar_scene is free null...\n");
						//eLIBs_memset(new_movie_ctrl->progressbar_scene, 0, sizeof(new_movie_ctrl_t));
						new_movie_ctrl->progressbar_scene = NULL;
					}
				#endif
				
				#if 1//ɾ����Ƶ���ſ��Ʋ˵�ͼ��
					new_movie_spsc_ctrl_menu_ui_scene_delete(new_movie_ctrl->spsc_ctrl_scene);
					if(new_movie_ctrl->spsc_ctrl_scene){//��Ҫ�����һ�����������Ʋ˵�framewin���ڵľ��
						__wrn("new_movie_ctrl->spsc_ctrl_scene is free null...\n");
						//eLIBs_memset(new_movie_ctrl->spsc_ctrl_scene, 0, sizeof(new_movie_ctrl_t));
						new_movie_ctrl->spsc_ctrl_scene = NULL;
					}
				#endif
				
				__here__;
				new_movie_ctrl_para_deinit(new_movie_ctrl);//��Ƶ���Ʋ���ɾ��������ͼ���ͷţ���ʾ����ɾ����
				eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_ctrl_t));
				esMEMS_Bfree(new_movie_ctrl, sizeof(new_movie_ctrl_t));//�ͷ��ڴ�ռ�
				__here__;
			}

			g_display_switch_output(LION_DISP_LCD);	//movie �˳�ʱ�ָ���С����ʾ
			g_enable_close_scn();					//�ر�ӫ����
			robin_close();							//robinģ��ر�
			new_movie_robin_sem_deinit();			//robin���ж�ز���ʼ��
				
			com_set_backcolor(CONST_BackColor);		//���ñ�����ɫ
			SEND_MSG(DSK_MSG_SCREENOFF_BKOFF, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);//���ͱ����ر�
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
/*��ƵappӦ�ó���manwin���ڴ���*/
H_WIN app_new_movie_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	H_WIN hManWin;
	__inf("****************************************************************************************\n");
	__inf("********  enter new movie app  **************\n");
	__inf("****************************************************************************************\n");
	//esKSRV_SysInfo();//����ڴ��Ƿ�й©
	/*******************��ʼ����manwin��������*********************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_NEWMOVIE;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_movie_proc);
	create_info.attr            = (void *)para;
	create_info.id				= APP_NEWMOVIE_ID;//��ַ0x9801
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



