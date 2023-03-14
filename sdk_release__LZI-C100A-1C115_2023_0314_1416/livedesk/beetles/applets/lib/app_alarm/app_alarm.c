#include "app_alarm.h"

#if SP_APP_ALARM


static H_WIN alarm_win = NULL;
const static __u32 id_alarm_bmp[ALARM_BMP_MAX] =
{
	ID_ALARM_ALARM_BG_BMP,		//����

	ID_ALARM_ALARM_ANI1_BMP,	//����1
	ID_ALARM_ALARM_ANI2_BMP,	//����2
	ID_ALARM_ALARM_ANI3_BMP,	//����3

	ID_ALARM_BUTTON_UNF_BMP,	//�޽���
	ID_ALARM_BUTTON_F_BMP,		//����

	ID_ALARM_0_BMP,				//����
	ID_ALARM_1_BMP,
	ID_ALARM_2_BMP,
	ID_ALARM_3_BMP,
	ID_ALARM_4_BMP,
	ID_ALARM_5_BMP,
	ID_ALARM_6_BMP,
	ID_ALARM_7_BMP,
	ID_ALARM_8_BMP,
	ID_ALARM_9_BMP,
	ID_ALARM_10_BMP,
};
static GUI_RECT	alarm_button_touch[2] =
{
	{14, 159, 156, 197},		//�˳�
	{167, 159, 309, 197}		//̰˯
};

static H_LYR alarm_frm_32bpp_layer_create(RECT *LayerRect)
{
	H_LYR layer = NULL;
	FB  fb =
	{
		{0, 0},                                   						/* size      */
		{0, 0, 0},                                      				/* buffer    */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    	/* fmt       */

	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_NORMAL,
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
		"APP_ALARM",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= LayerRect->width;
	fb.size.height		= LayerRect->height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= LayerRect->width;
	lstlyr.src_win.height   = LayerRect->height;
	lstlyr.scn_win.x		= LayerRect->x;
	lstlyr.scn_win.y		= LayerRect->y;
	lstlyr.scn_win.width  = LayerRect->width;
	lstlyr.scn_win.height = LayerRect->height;
	lstlyr.pipe = 1;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app alarm layer create error !\n");
	}

	return layer;
}


static __s32 alarm_res_init(__gui_msg_t *msg)
{
	__u32 i = 0;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	for(i = 0; i < ALARM_BMP_MAX; i++)
	{
		palarmFrm_attr->h_bmp_alarm[i] = theme_open(id_alarm_bmp[i]);
	}

	return EPDK_OK;
}

static __s32 alarm_res_uninit(__gui_msg_t *msg)
{
	__u32 i = 0;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	for(i = 0; i < ALARM_BMP_MAX; i++)
	{
		if(palarmFrm_attr->h_bmp_alarm[i])
		{
			theme_close(palarmFrm_attr->h_bmp_alarm[i]);
			palarmFrm_attr->h_bmp_alarm[i] = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 alarm_frm_alarm_change_draw(__gui_msg_t *msg)
{
	char 			str[GUI_NAME_MAX];
	GUI_RECT		rect = {146, 25, 146 + 80, 25 + 16};
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(palarmFrm_attr->layer);
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(0xDB000000);
	GUI_ClearRectEx(&rect);

	if(palarmFrm_attr->isDelay == EPDK_FALSE)
	{
		dsk_langres_get_menu_text(STRING_ALARM_RUNNING, str, GUI_NAME_MAX);
		GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
	}
	else
	{
		dsk_langres_get_menu_text(STRING_ALARM_DELAYING, str, GUI_NAME_MAX);
		GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
	}

	return EPDK_OK;
}
static __s32 alarm_frm_bg_draw(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(palarmFrm_attr->layer);
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetBkColor(0);
	GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_ALARM_BG_BMP]), 0, 0);
	alarm_frm_alarm_change_draw(msg);
	return EPDK_OK;
}
static __s32 alarm_frm_button_draw(__gui_msg_t *msg)
{
	char 		str[GUI_NAME_MAX];
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(palarmFrm_attr->layer);
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetBkColor(0);

	if(palarmFrm_attr->buttonF == ALARM_EXIT_BUTTON)
	{
		GUI_SetColor(GUI_YELLOW);
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_BUTTON_F_BMP]), 14, 159);
	}
	else
	{
		GUI_SetColor(GUI_WHITE);
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_BUTTON_UNF_BMP]), 14, 159);
	}

	dsk_langres_get_menu_text(STRING_ALARM_EXIT, str, GUI_NAME_MAX);
	GUI_DispStringInRect(str, &alarm_button_touch[0], GUI_TA_VCENTER | GUI_TA_HCENTER);

	if(palarmFrm_attr->buttonF == ALARM_DELAY_BUTTON)
	{
		GUI_SetColor(GUI_YELLOW);
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_BUTTON_F_BMP]), 167, 159);
	}
	else
	{
		GUI_SetColor(GUI_WHITE);
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_BUTTON_UNF_BMP]), 167, 159);
	}

	dsk_langres_get_menu_text(STRING_ALARM_DELAY, str, GUI_NAME_MAX);
	GUI_DispStringInRect(str, &alarm_button_touch[1], GUI_TA_VCENTER | GUI_TA_HCENTER);
	return EPDK_OK;
}
static __s32 alarm_frm_alarmSt_draw(__gui_msg_t *msg)
{
	__time_t 		time;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(palarmFrm_attr->layer);
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

	//�滭����
	if(palarmFrm_attr->isDelay)
	{
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_ALARM_ANI1_BMP]), 33, 43);
		palarmFrm_attr->ani_fp = 0;
	}
	else
	{
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_ALARM_ANI1_BMP + (palarmFrm_attr->ani_fp % 3)]), 33, 43);
		palarmFrm_attr->ani_fp++;
	}

	//�滭ʱ��
	eLIBs_GetTime(&time);
	GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_0_BMP + ((time.hour / 10) % 10)]), 150, 70);
	GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_0_BMP + ((time.hour) % 10)]), 177, 70);

	if(esKRNL_TimeGet() % 100 < 50)
	{
		GUI_SetBkColor(0xDB000000);
		GUI_ClearRect(204, 70, 221, 110);
	}
	else
	{
		GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_10_BMP]), 204, 70);
	}

	GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_0_BMP + ((time.minute / 10) % 10)]), 221, 70);
	GUI_BMP_Draw(theme_hdl2buf(palarmFrm_attr->h_bmp_alarm[ALARM_0_BMP + ((time.minute) % 10)]), 248, 70);
	return EPDK_OK;
}
static __s32 alarm_frm_music_play(__gui_msg_t *msg)
{
	__cedar_status_t play_status;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	play_status = robin_get_fsm_status();

	if(palarmFrm_attr->isDelay == EPDK_FALSE && play_status == CEDAR_STAT_PAUSE)
	{
		robin_set_cmd_play();
	}
	else if(palarmFrm_attr->isDelay == EPDK_FALSE && play_status == CEDAR_STAT_STOP)
	{
		robin_set_cmd_play();
	}
	else if(palarmFrm_attr->isDelay == EPDK_TRUE && play_status != CEDAR_STAT_STOP)
	{
		robin_set_cmd_stop();
	}

	return EPDK_OK;
}

static __u32 alarm_get_runtime_cnt(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	if(palarmFrm_attr->reg_alarm->alarm_runtime == REG_ALARM_20SEC)
	{
		return 20;
	}
	else if(palarmFrm_attr->reg_alarm->alarm_runtime == REG_ALARM_30SEC)
	{
		return 30;
	}
	else if(palarmFrm_attr->reg_alarm->alarm_runtime == REG_ALARM_50SEC)
	{
		return 50;
	}
	else if(palarmFrm_attr->reg_alarm->alarm_runtime == REG_ALARM_90SEC)
	{
		return 90;
	}
	else
	{
		return 90;
	}
}
static __u32 alarm_get_delay_cnt(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	if(palarmFrm_attr->reg_alarm->alarm_delay == REG_ALARM_5MIN)
	{
		return 5 * 60;
	}
	else if(palarmFrm_attr->reg_alarm->alarm_delay == REG_ALARM_10MIN)
	{
		return 10 * 60;
	}
	else if(palarmFrm_attr->reg_alarm->alarm_delay == REG_ALARM_15MIN)
	{
		return 15 * 60;
	}
	else
	{
		return 15 * 60;
	}
}
static __u32 alarm_get_cur_second_cnt(__gui_msg_t *msg)
{
	__time_t 		time;
	eLIBs_GetTime(&time);
	return ((time.hour * 60 * 60) + (time.minute * 60) + time.second);
}

static __s32 alarm_frm_close(__gui_msg_t *msg)
{
	H_WIN		musicWin = NULL;
	__gui_msg_t sendMsg;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	sendMsg.h_deswin 	= GUI_WinGetParent(msg->h_deswin);
	sendMsg.h_srcwin 	= NULL;
	sendMsg.id 			= ALARM_MSG_CLOSE;
	sendMsg.dwAddData1 	= 0;
	sendMsg.dwAddData2 	= 0;
	sendMsg.dwReserved 	= 0;
	GUI_PostMessage(&sendMsg);
	GUI_LyrWinDelete(palarmFrm_attr->layer);

	if(is_app_exist(APP_MUSIC))
	{
		musicWin = GUI_WinGetHandFromName(APP_MUSIC);

		if(musicWin != sendMsg.h_deswin)
		{
			sendMsg.h_deswin 	= musicWin;
			sendMsg.h_srcwin 	= NULL;
			sendMsg.id 			= ALARM_MSG_CLOSE;
			sendMsg.dwAddData1 	= 0;
			sendMsg.dwAddData2 	= 0;
			sendMsg.dwReserved 	= 0;
			GUI_PostMessage(&sendMsg);
		}
	}

	if(is_app_exist(APP_NEWMUSIC))
	{
		musicWin = GUI_WinGetHandFromName(APP_NEWMUSIC);

		if(musicWin != sendMsg.h_deswin)
		{
			sendMsg.h_deswin 	= musicWin;
			sendMsg.h_srcwin 	= NULL;
			sendMsg.id 			= ALARM_MSG_CLOSE;
			sendMsg.dwAddData1 	= 0;
			sendMsg.dwAddData2 	= 0;
			sendMsg.dwReserved 	= 0;
			GUI_PostMessage(&sendMsg);
		}
	}

	return EPDK_OK;
}

static __s32 _alarm_frm_on_paint(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	//���û滭����
	GUI_LyrWinSel(palarmFrm_attr->layer);
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetBkColor(0);
	return EPDK_OK;
}

static __s32 _alarm_frm_on_create(__gui_msg_t *msg)
{
	ES_FILE *file_hdle;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSetSta(palarmFrm_attr->layer, GUI_LYRWIN_STA_ON);
	palarmFrm_attr->sysVolume		= dsk_volume_get();
	palarmFrm_attr->buttonF			= ALARM_EXIT_BUTTON;
	palarmFrm_attr->isDelay			= EPDK_FALSE;
	palarmFrm_attr->alarmTimeStart	= alarm_get_cur_second_cnt(msg);
	palarmFrm_attr->alarmTimeID 	= 0x71;
	GUI_SetTimer(msg->h_deswin, palarmFrm_attr->alarmTimeID, ALARM_TIMER_TIMES, NULL);
	//��Դ��ʼ��
	alarm_res_init(msg);
	//�滭����
	alarm_frm_bg_draw(msg);
	alarm_frm_alarm_change_draw(msg);
	alarm_frm_alarmSt_draw(msg);
	alarm_frm_button_draw(msg);

	//������ʼ��
	if(robin_is_open() == EPDK_FALSE)
	{
		robin_open(ROBIN_MODE_AUDIO_MIN, NULL);
		robin_set_play_mode(RAT_PLAY_MODE_ONLY_ONCE);
		palarmFrm_attr->extRobinOpen = EPDK_FALSE;
	}
	else
	{
		palarmFrm_attr->extRobinOpen = EPDK_TRUE;
	}

	dsk_volume_set(palarmFrm_attr->reg_alarm->alarm_volume);
	file_hdle = eLIBs_fopen(palarmFrm_attr->reg_alarm->alarm_music_path, "rb");

	if(file_hdle == NULL)
	{
		robin_play_file(REG_ALARM_MUSIC_DEFAULT_PATH, NULL);					//   play default
		__msg("\n\n1: %s  play alarm music!\n\n", REG_ALARM_MUSIC_DEFAULT_PATH);
	}
	else
	{
		eLIBs_fclose(file_hdle);
		robin_play_file(palarmFrm_attr->reg_alarm->alarm_music_path, NULL);   	//play music
		__msg("\n\n2: %s  play alarm music!\n\n", palarmFrm_attr->reg_alarm->alarm_music_path);
	}

	GUI_LyrWinSetTop(palarmFrm_attr->layer);
	g_disable_standby();
	g_disable_close_scn();
	return EPDK_OK;
}
static __s32 _alarm_frm_on_close(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_FrmWinDelete(msg->h_deswin);
	return EPDK_OK;
}
static __s32 _alarm_frm_on_destory(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	//��������
	robin_set_cmd_stop();

	if(palarmFrm_attr->extRobinOpen == EPDK_FALSE)
	{
		robin_close();

		//robin_close��رչ��ţ�������ں�̨linein����Ҫ�رչ���
		if(EPDK_TRUE == is_app_exist(APP_LINEIN)
		    || EPDK_TRUE == is_app_exist(APP_RECORD)
		    || EPDK_TRUE == is_app_exist(APP_FM))
		{
			//            dsk_amplifier_onoff(1);
		}
	}

	dsk_volume_set(palarmFrm_attr->sysVolume);

	if(GUI_WinGetCaptureWin() == msg->h_deswin)
	{
		GUI_WinReleaseCapture();
	}

	GUI_KillTimer(msg->h_deswin, palarmFrm_attr->alarmTimeID);
	alarm_res_uninit(msg);
	esMEMS_Bfree(palarmFrm_attr, sizeof(app_alarm_attr));
	GUI_WinSetAttr(msg->h_deswin, NULL);
	alarm_exit_app();
	alarm_win = NULL;
	g_enable_standby();
	g_enable_close_scn();
	return EPDK_OK;
}

static __s32 _alarm_frm_on_key(__gui_msg_t *msg)
{
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_LONGENTER:
			case GUI_MSG_KEY_ENTER:
			{
				if(palarmFrm_attr->buttonF == ALARM_EXIT_BUTTON)
				{
					alarm_frm_close(msg);
				}
				else
				{
					palarmFrm_attr->isDelay = EPDK_TRUE;
					palarmFrm_attr->alarmTimeStart	= alarm_get_cur_second_cnt(msg);
					alarm_frm_alarm_change_draw(msg);
				}
			}
			break;

			case GUI_MSG_KEY_LONGMENU:
			case GUI_MSG_KEY_MENU:
				alarm_frm_close(msg);
				break;
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:
			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:
				if(palarmFrm_attr->buttonF == ALARM_EXIT_BUTTON)
				{
					palarmFrm_attr->buttonF = ALARM_DELAY_BUTTON;
				}
				else
				{
					palarmFrm_attr->buttonF = ALARM_EXIT_BUTTON;
				}

				alarm_frm_button_draw(msg);
				break;
		}
	}

	return EPDK_OK;
}

static __s32 _alarm_frm_on_touch(__gui_msg_t *msg)
{
	__s32 speed = 0, direct = 0;
	__u16 x = 0, y = 0;
	papp_alarm_attr palarmFrm_attr;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	x = LOSWORD(msg->dwAddData2);
	y = HISWORD(msg->dwAddData2);
	speed = LOSWORD(msg->dwReserved);
	direct = HISWORD(msg->dwReserved);

	if(x > alarm_button_touch[0].x0 && x < alarm_button_touch[0].x1 &&
	    y > alarm_button_touch[0].y0 && y < alarm_button_touch[0].y1)
	{
		//�˳�
		alarm_frm_close(msg);
	}
	else if(x > alarm_button_touch[1].x0 && x < alarm_button_touch[1].x1 &&
	        y > alarm_button_touch[1].y0 && y < alarm_button_touch[1].y1)
	{
		//̰˯
		palarmFrm_attr->alarmTimeStart = alarm_get_cur_second_cnt(msg);
		palarmFrm_attr->isDelay = EPDK_TRUE;
		palarmFrm_attr->buttonF = ALARM_DELAY_BUTTON;
		alarm_frm_alarm_change_draw(msg);
		alarm_frm_button_draw(msg);
	}

	return EPDK_OK;
}

static __s32 g_mute_status = 0;
static __s32 _alarm_frm_manager_proc(__gui_msg_t *msg)
{
	__msg("_alarm_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

	switch(msg->id)
	{
		case GUI_MSG_CREATE:
			g_mute_status = dsk_amplifier_is_on();
			__wrn("g_mute_status=%d\n", g_mute_status);
			//        dsk_amplifier_onoff(1);//�򿪹���
			return _alarm_frm_on_create(msg);

		case GUI_MSG_CLOSE:
			return _alarm_frm_on_close(msg);

		case GUI_MSG_DESTROY:
			__wrn("g_mute_status=%d\n", g_mute_status);

			if(0 == g_mute_status)
			{
				//            dsk_amplifier_onoff(1);//�򿪹���
			}
			else
			{
				//            dsk_amplifier_onoff(0);//�رչ���
			}

			return _alarm_frm_on_destory(msg);

		case GUI_MSG_PAINT:
			return _alarm_frm_on_paint(msg);

		case GUI_MSG_KEY:
			return _alarm_frm_on_key(msg);

		case GUI_MSG_TOUCH:
			return _alarm_frm_on_touch(msg);

		case GUI_MSG_TIMER:
		{
			papp_alarm_attr palarmFrm_attr;
			palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(msg->h_deswin);

			if(palarmFrm_attr == NULL)
			{
				return EPDK_FAIL;
			}

			if(palarmFrm_attr->isDelay == EPDK_FALSE)
			{
				if(alarm_get_cur_second_cnt(msg) - palarmFrm_attr->alarmTimeStart > alarm_get_runtime_cnt(msg))
				{
					palarmFrm_attr->alarmTimeStart = alarm_get_cur_second_cnt(msg);
					palarmFrm_attr->isDelay = EPDK_TRUE;
					alarm_frm_alarm_change_draw(msg);
				}
			}
			else
			{
				if(alarm_get_cur_second_cnt(msg) - palarmFrm_attr->alarmTimeStart > alarm_get_delay_cnt(msg))
				{
					palarmFrm_attr->alarmTimeStart = alarm_get_cur_second_cnt(msg);
					palarmFrm_attr->isDelay = EPDK_FALSE;
					alarm_frm_alarm_change_draw(msg);
				}
			}

			//�滭
			alarm_frm_alarmSt_draw(msg);
			//����
			alarm_frm_music_play(msg);
		}

		return EPDK_OK;

		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

H_WIN app_alarm_frm_create(H_WIN parentWin)
{
	H_WIN retWin = NULL;
	__gui_framewincreate_para_t framewin_para;
	papp_alarm_attr				palarmFrm_attr;
	RECT winRect				= {0, 0, 326, 210};
	RECT layerRect				= {77, 31, 326, 210};
	__s32 width, height;
	dsk_display_get_size(&width, &height);
	layerRect.x = (width - 326) / 2;
	layerRect.y = (height - 210) / 2;

	if(alarm_win != NULL)
	{
		__wrn("alarm is exist\n");
		return NULL;
	}

	//����ռ�
	palarmFrm_attr = (papp_alarm_attr)esMEMS_Balloc(sizeof(app_alarm_attr));

	if(palarmFrm_attr == NULL)
	{
		__msg("esMEMS_Balloc fail\n");
		return NULL;
	}

	eLIBs_memset(palarmFrm_attr, 0, sizeof(app_alarm_attr));
	palarmFrm_attr->reg_alarm = alarm_get_happening();

	if(palarmFrm_attr->reg_alarm == NULL)
	{
		__msg("get alarm fail\n");
		esMEMS_Bfree(palarmFrm_attr, sizeof(app_alarm_attr));
		return NULL;
	}

	//����ͼ��
	palarmFrm_attr->layer = alarm_frm_32bpp_layer_create(&layerRect);

	if(palarmFrm_attr->layer == NULL)
	{
		__msg("app_alarm_frm_create fail\n");
		esMEMS_Bfree(palarmFrm_attr, sizeof(app_alarm_attr));
		return NULL;
	}

	//��������
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name 			= "alarm_frame";
	framewin_para.dwExStyle 	= WS_EX_NONE;
	framewin_para.dwStyle 		= WS_NONE | WS_VISIBLE;
	framewin_para.spCaption 	= NULL;
	framewin_para.hOwner		= NULL;
	framewin_para.id         	= APP_ALARM_ID;
	framewin_para.hHosting 		= parentWin;
	framewin_para.FrameWinProc 	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_alarm_frm_manager_proc);
	framewin_para.rect.x 		= winRect.x;
	framewin_para.rect.y 		= winRect.y;
	framewin_para.rect.width	= winRect.width;
	framewin_para.rect.height 	= winRect.height;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red 	= 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue 	= 0;
	framewin_para.attr 			= (void *)palarmFrm_attr;
	framewin_para.hLayer 		= palarmFrm_attr->layer;
	retWin = GUI_FrmWinCreate(&framewin_para);

	if(retWin == NULL)
	{
		__msg("create alarm frame fail\n");
		GUI_LyrWinDelete(palarmFrm_attr->layer);
		esMEMS_Bfree(palarmFrm_attr, sizeof(app_alarm_attr));
	}

	GUI_WinSetCaptureWin(retWin);
	GUI_WinSetFocusChild(retWin);
	alarm_enter_app();
	alarm_win = retWin;
	return retWin;
}

__s32 app_alarm_frm_destory(H_WIN alarmWin)
{
	__gui_msg_t msg;
	papp_alarm_attr palarmFrm_attr;
	msg.h_deswin = alarmWin;
	palarmFrm_attr = (papp_alarm_attr)GUI_WinGetAttr(alarmWin);

	if(palarmFrm_attr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinDelete(palarmFrm_attr->layer);
	return EPDK_TRUE;
}



#endif
