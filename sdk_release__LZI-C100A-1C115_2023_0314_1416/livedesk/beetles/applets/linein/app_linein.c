
#include "linein_uipara.h"
#include "app_linein.h"

#if SP_APP_AUX


#define SCREEN_WIDTH	_W_SCR_
#define SCREEN_HEIGHT	_H_SCR_

typedef struct tag_linein_ctrl
{
	linein_para_t *linein_scene_para;
	HTHEME h_bmp_loading[2];
}
linein_ctrl_t;

typedef struct
{
	H_WIN hfrm;
	H_WIN h_frm_alarm_coming;
	linein_mode_e mode;
}
linein_man_ctrl_t;

static __u8 gs_auxMode = 0;
	#define MASK_AUX_MODE_LINEIN					(1<<0)
	#define MASK_AUX_MODE_BT							(1<<1)

__s32 linein_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	char buf[128];
	msg.h_deswin = GUI_WinGetParent(hwin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	return (GUI_SendMessage(&msg));
}

/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
static H_WIN linein_layer_create(RECT *rect)
{
	FB	fb =
	{
		{0, 0},
		{0, 0, 0},
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, 0}},
	};
	__disp_layer_para_t para =
	{
		//DISP_LAYER_WORK_MODE_PALETTE,
		DISP_LAYER_WORK_MODE_NORMAL,
		0,
		0,
		0,

		0,
		0xff,
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		DISP_LAYER_OUTPUT_CHN_DE_CH1,
		NULL
	};
	__layerwincreate_para_t create_info =
	{
		"linein layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width  = rect->width;
	fb.size.height = rect->height;
	para.pipe			= 1;
	para.scn_win.x 		= rect->x;
	para.scn_win.y 		= rect->y;
	para.scn_win.width 	= rect->width;
	para.scn_win.height = rect->height;
	para.src_win.x 		= 0;
	para.src_win.y 		= 0;
	para.src_win.width 	= rect->width;
	para.src_win.height = rect->height;
	para.fb = &fb;
	create_info.lyrpara = &para;
	return(GUI_LyrWinCreate(&create_info));
}

static __s32 linein_loading_paint(H_WIN h_win, __s32 index)
{
	linein_ctrl_t *linein_ctrl;
	linein_uipara_t *linein_uipara;
	GUI_RECT gui_rect;

	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	if(index < 0 || index > 1)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	linein_ctrl = (linein_ctrl_t *)GUI_WinGetAddData(h_win);
	linein_uipara = (linein_uipara_t *)app_linein_get_uipara();
	GUI_LyrWinSel(linein_ctrl->linein_scene_para->layer);
	GUI_SetFont(linein_ctrl->linein_scene_para->font);
	GUI_UC_SetEncodeUTF8();

	if(NULL == linein_ctrl->h_bmp_loading[index])
	{
		if(linein_uipara->linein_btn_bmp[index])
		{
			linein_ctrl->h_bmp_loading[index] = dsk_theme_open(linein_uipara->linein_btn_bmp[index]);
		}
	}

	if(linein_ctrl->h_bmp_loading[index])
	{
		void *pbmp;
		pbmp = dsk_theme_hdl2buf(linein_ctrl->h_bmp_loading[index]);

		if(pbmp)
		{
			GUI_BMP_Draw(pbmp, linein_uipara->linein_bmp_rect.x, linein_uipara->linein_bmp_rect.y);
		}
	}

	return EPDK_OK;
}

static __s32  linein_win_key_proc(__gui_msg_t *msg)
{
	linein_para_t *linein_para;
	static __s32 last_key = -1;
	__s32 bok;
	bok = -1;

	linein_para = (linein_para_t *)GUI_WinGetAttr(msg->h_deswin);

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_MENU == last_key
		    || GUI_MSG_KEY_ESCAPE == last_key)
		{
			if((IOCTRL__AUX_DET__IS_PLUGIN()) || (gs_auxMode & MASK_AUX_MODE_BT))
			{
				linein_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 1); //进入后台模式
				__msg_jjl("aux background.\n");
			}
			else
			{
				linein_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
			}
		}
	}
	else
	{
		if(GUI_MSG_KEY_BT == msg->dwAddData1)
		{
			if(gs_auxMode & MASK_AUX_MODE_BT)
			{
				gs_auxMode &= ~MASK_AUX_MODE_BT;
				gscene_hbar_set_bt(0);
				IOCTRL__BT_PW__SET_OFF();
			}
			else
			{
				gs_auxMode |= MASK_AUX_MODE_BT;
				gscene_hbar_set_bt(1);
				IOCTRL__BT_PW__SET_ON();
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

	return EPDK_OK;
}

static __s32  linein_win_touch_proc(__gui_msg_t *msg)
{
	__s32 x, y;
	static __bool down_point_in_rect = EPDK_FALSE;
	static __bool up_point_in_rect = EPDK_FALSE;
	linein_uipara_t *linein_uipara;
	x = LOSWORD(msg->dwAddData2);
	y = HISWORD(msg->dwAddData2);
	linein_uipara = (linein_uipara_t *)app_linein_get_uipara();

	if(NULL == linein_uipara)
	{
		__wrn("linein_uipara is null...\n");
		return EPDK_FAIL;
	}

	switch(msg->dwAddData1)
	{
		case GUI_MSG_TOUCH_DOWN:
		{
			if(GUI_WinGetCaptureWin() != msg->h_deswin)
			{
				GUI_WinSetCaptureWin(msg->h_deswin);
			}

			if(x >= linein_uipara->linein_bmp_rect.x
			    && x < linein_uipara->linein_bmp_rect.x + linein_uipara->linein_bmp_rect.width
			    && y >= linein_uipara->linein_bmp_rect.y
			    && y < linein_uipara->linein_bmp_rect.y + linein_uipara->linein_bmp_rect.height)
			{
				down_point_in_rect = EPDK_TRUE;
			}
			else
			{
				down_point_in_rect = EPDK_FALSE;
			}

			if(EPDK_TRUE == down_point_in_rect)
			{
				linein_loading_paint(msg->h_deswin, 1);
			}

			break;
		}

		case GUI_MSG_TOUCH_UP:
		{
			if(GUI_WinGetCaptureWin() == msg->h_deswin)
			{
				GUI_WinReleaseCapture();
			}

			if(x >= linein_uipara->linein_bmp_rect.x
			    && x < linein_uipara->linein_bmp_rect.x + linein_uipara->linein_bmp_rect.width
			    && y >= linein_uipara->linein_bmp_rect.y
			    && y < linein_uipara->linein_bmp_rect.y + linein_uipara->linein_bmp_rect.height)
			{
				up_point_in_rect = EPDK_TRUE;
			}
			else
			{
				up_point_in_rect = EPDK_FALSE;
			}

			linein_loading_paint(msg->h_deswin, 0);
			break;
		}

		default:
		{
			break;
		}
	}

	if(GUI_MSG_TOUCH_UP == msg->dwAddData1
	    && EPDK_TRUE == up_point_in_rect
	    && EPDK_TRUE == down_point_in_rect)
	{
		linein_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 1); //进入后台模式
	}

	return EPDK_OK;
}

static __s32 _linein_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			linein_ctrl_t *linein_ctrl;
			linein_para_t *linein_para;
			__here__;
			linein_ctrl = (linein_ctrl_t *)My_Balloc(sizeof(linein_ctrl_t));

			if(!linein_ctrl)
			{
				__msg("memory balloc fail.........\n");
				return EPDK_FAIL;
			}

			eLIBs_memset(linein_ctrl, 0, sizeof(linein_ctrl_t));
			linein_para = (linein_para_t *)GUI_WinGetAttr(msg->h_deswin);
			linein_ctrl->linein_scene_para = linein_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)linein_ctrl);
			__here__;
			//GUI_LyrWinCacheOn();
			GUI_LyrWinSetSta(linein_ctrl->linein_scene_para->layer, GUI_LYRWIN_STA_ON);
			GUI_LyrWinSetTop(linein_ctrl->linein_scene_para->layer);
			//GUI_LyrWinCacheOff();
			{
				char title[32] = {0};
				dsk_langres_get_menu_text(STRING_LINE_IN_TITLE, title, sizeof(title));
				gscene_hbar_set_title(title, sizeof(title));
				gscene_hbar_set_state(HBAR_ST_SHOW);
			}
			{
				GUI_RECT gui_rect;
				char text[256] = {0};
				linein_uipara_t *linein_uipara;
				linein_uipara = (linein_uipara_t *)app_linein_get_uipara();
				dsk_langres_get_menu_text(STRING_LINE_IN_EXTERNAL_SOUND, text, sizeof(text));

				if(linein_uipara)
				{
					gui_rect.x0 = linein_uipara->linein_tips_rect.x;
					gui_rect.y0 = linein_uipara->linein_tips_rect.y;
					gui_rect.x1 = gui_rect.x0 + linein_uipara->linein_tips_rect.width;
					gui_rect.y1 = gui_rect.y0 + linein_uipara->linein_tips_rect.height;

					if(linein_ctrl->linein_scene_para->layer)
					{
						GUI_LyrWinSel(linein_ctrl->linein_scene_para->layer);
						GUI_SetFont(linein_ctrl->linein_scene_para->font);
						GUI_SetColor(GUI_WHITE);
						GUI_DispStringInRect(text, &gui_rect, GUI_TA_BOTTOM | GUI_TA_HCENTER);
					}
				}				
			}

			if(gs_auxMode & MASK_AUX_MODE_BT)
			{
				gscene_hbar_set_bt(1);
				IOCTRL__BT_PW__SET_ON();
			}
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:
			return EPDK_OK;

		case GUI_MSG_DESTROY:
		{
			linein_ctrl_t *linein_ctrl;
			linein_ctrl = (linein_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!linein_ctrl)
			{
				__err("linein_ctrl is null.......\n");
				return EPDK_FAIL;
			}

			{
				__s32 i;

				for(i = 0 ; i < BEETLES_TBL_SIZE(linein_ctrl->h_bmp_loading) ; i++)
				{
					if(linein_ctrl->h_bmp_loading[i])
					{
						dsk_theme_close(linein_ctrl->h_bmp_loading[i]);
						linein_ctrl->h_bmp_loading[i] = 0;
					}
				}
			}

			if(linein_ctrl->linein_scene_para)
			{
				My_Bfree(linein_ctrl->linein_scene_para, sizeof(linein_para_t));
			}

			if(linein_ctrl)
			{
				My_Bfree(linein_ctrl, sizeof(linein_ctrl_t));
			}
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:
			return linein_loading_paint(msg->h_deswin, 0);

		case GUI_MSG_KEY:
			return linein_win_key_proc(msg);

		case GUI_MSG_TOUCH:
			return linein_win_touch_proc(msg);

		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

/*
	建立窗口
*/
static H_WIN linein_win_create(H_WIN h_parent, linein_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	linein_para_t *linein_scene_para;
	FB fb;

	__inf("****************************************************************************************\n");
	__inf("********  enter line in application  **************\n");
	__inf("****************************************************************************************\n");

	GUI_LyrWinGetFB(para->layer, &fb);
	linein_scene_para = (linein_para_t *)My_Balloc(sizeof(linein_para_t));

	if(!linein_scene_para)
	{
		__msg("memory balloc fail.........\n");
		return EPDK_FAIL;
	}

	linein_scene_para->font = para->font;
	linein_scene_para->layer = para->layer;
	__here__;
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name 			= APP_LINEIN;
	framewin_para.dwExStyle 	= WS_EX_NONE;
	framewin_para.dwStyle 		= WS_NONE | WS_VISIBLE;
	framewin_para.spCaption 	= NULL;
	framewin_para.hOwner		= NULL;
	framewin_para.hHosting 		= h_parent;
	framewin_para.id        	= APP_LINEIN_ID;
	framewin_para.FrameWinProc 	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_linein_proc);
	framewin_para.rect.x 		= 0;
	framewin_para.rect.y 		= 0;
	framewin_para.rect.width	= fb.size.width;
	framewin_para.rect.height 	= fb.size.height;
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red 	= 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue 	= 0;
	framewin_para.attr 			= (void *)linein_scene_para;
	framewin_para.hLayer 		= para->layer;
	return (GUI_FrmWinCreate(&framewin_para));
}

void  linein_win_delete(H_WIN linein_win)
{
	if(linein_win != NULL)
	{
		GUI_LyrWinDelete(GUI_WinGetLyrWin(linein_win));
	}
}

static H_WIN linein_frame_create(H_WIN hparent)
{
	linein_para_t linein_scene_para;
	linein_uipara_t *linein_uipara;
	RECT rect;
	H_WIN h_wnd;

	gscene_bgd_set_state(BGD_STATUS_SHOW);
	rect.x = 0;
	rect.y = 0;
	rect.width = SCREEN_WIDTH;
	rect.height = SCREEN_HEIGHT;
	linein_scene_para.layer = linein_layer_create(&rect);
	linein_scene_para.font = SWFFont;
	h_wnd = linein_win_create(hparent, &linein_scene_para);
	return h_wnd;
}

static __s32 __app_linein_start(void)
{
	ES_FILE *paudio;
	paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");

	if(paudio)
	{
		__wrn("\n              LINE IN START succeed!\n");
		eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_START, 0, 0);
		eLIBs_fclose(paudio);
		*(volatile unsigned long *)(0xf1c23c00 + 0x10) |= (0x5 << 23);
		__wrn("*(volatile unsigned long*)(0xf1c23c00 + 0x10)=0x%x\n", *(volatile unsigned long *)(0xf1c23c00 + 0x10));
	}

	return EPDK_OK;
}

static __s32 __app_linein_stop(void)
{
	ES_FILE *paudio;
	paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");

	if(paudio)
	{
		__inf("\n              LINE IN STOP succeed!\n");
		eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_STOP, 0, 0);
		eLIBs_fclose(paudio);
		//change to default
		*(volatile unsigned long *)(0xf1c23c00 + 0x10) |= (0x3 << 23);
		__wrn("*(volatile unsigned long*)(0xf1c23c00 + 0x10)=0x%x\n", *(volatile unsigned long *)(0xf1c23c00 + 0x10));
	}

	return EPDK_OK;
}


static __s32 __app_linein_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			root_para_t *para;
			linein_man_ctrl_t *man_ctrl;
			para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);

			if(NULL == para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			__wrn("para->data=%d\n", para->data);
			man_ctrl = esMEMS_Balloc(sizeof(linein_man_ctrl_t));

			if(NULL == man_ctrl)
			{
				__wrn("mem not enough...\n");
				return EPDK_FAIL;
			}

			eLIBs_memset(man_ctrl, 0, sizeof(linein_man_ctrl_t));
			GUI_WinSetAddData(msg->h_deswin, (__u32)man_ctrl);
			__wrn("para->data=%d\n", para->data);
			man_ctrl->mode = para->data;
			
			if(para->m_maskFlags & MASK_RootFlags_AuxMode_BT)
			{
				para->m_maskFlags &= ~MASK_RootFlags_AuxMode_BT;
				gs_auxMode |= MASK_AUX_MODE_BT;
			}
			else
			{
				gs_auxMode &= ~MASK_AUX_MODE_BT;
			}			

			if(LINEIN_MODE_NORMAL == man_ctrl->mode)
			{
				SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
				__wrn("linin create as forground...\n");
				man_ctrl->hfrm = linein_frame_create(msg->h_deswin);
				GUI_WinSetFocusChild(man_ctrl->hfrm);
			}
			else if(LINEIN_MODE_BACKGROUND == man_ctrl->mode)
			{
				__wrn("linin create as background...\n");
			}
			else
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			__app_linein_start();
			//            dsk_amplifier_onoff(1);//打开功放
			dsk_set_auto_off_time(0);
			//gscene_hbar_set_music_state(BG_AUX);
			g_disable_standby();
			return EPDK_OK;
		}

		case GUI_MSG_DESTROY:
		{
			root_para_t *para;
			linein_man_ctrl_t *man_ctrl;
			para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);

			__wrn("__app_linein_proc: GUI_MSG_DESTROY begin\n");
			man_ctrl = (linein_man_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(NULL == man_ctrl)
			{
				return EPDK_FAIL;
			}

			g_enable_standby();
			//            dsk_amplifier_onoff(0);//关闭功放
			{
				reg_system_para_t *para;
				para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

				if(para)
				{
					dsk_set_auto_off_time(para->poweroff);
					__wrn("para->poweroff=%d\n", para->poweroff);
				}
			}
			__app_linein_stop();

			gscene_hbar_set_bt(0);
			IOCTRL__BT_PW__SET_OFF();

			if(man_ctrl->hfrm)
			{
				linein_win_delete(man_ctrl->hfrm);
				man_ctrl->hfrm = NULL;
			}

#if SP_APP_ALARM
			if(man_ctrl->h_frm_alarm_coming)
			{
				app_alarm_frm_destory(man_ctrl->h_frm_alarm_coming);
				man_ctrl->h_frm_alarm_coming = NULL;
			}
#endif

			//gscene_hbar_set_music_state(BG_NONE);
			__wrn("__app_linein_proc: GUI_MSG_DESTROY end\n");
			SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
			return EPDK_OK;
		}

		case DSK_MSG_HOME:
		{
			linein_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 1); //进入后台模式
			return EPDK_OK;
		}

#if SP_APP_ALARM
		case DSK_MSG_ALARM:
		{
			linein_man_ctrl_t *man_ctrl;
			man_ctrl = (linein_man_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(NULL == man_ctrl)
			{
				return EPDK_FAIL;
			}

			if(LINEIN_MODE_NORMAL == man_ctrl->mode)
			{
				__app_linein_stop();
				man_ctrl->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
			}

			return EPDK_OK;
		}

		case ALARM_MSG_CLOSE:
		{
			linein_man_ctrl_t *man_ctrl;
			man_ctrl = (linein_man_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(NULL == man_ctrl)
			{
				return EPDK_FAIL;
			}

			if(man_ctrl->h_frm_alarm_coming)
			{
				man_ctrl->h_frm_alarm_coming = NULL ;
			}

			__app_linein_start();
			GUI_WinSetFocusChild(msg->h_deswin);
			return EPDK_OK;
		}
#endif
		default:
		{
			__msg("__app_line_proc: default\n");
			break;
		}
	}

	return GUI_ManWinDefaultProc(msg);
}

H_WIN app_linein_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	H_WIN hManWin;
	__inf("****************************************************************************************\n");
	__inf("********  enter linein app  **************\n");
	__inf("****************************************************************************************\n");
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_LINEIN;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_linein_proc);
	create_info.attr            = (void *)para;
	create_info.id				= APP_LINEIN_ID;
	create_info.hHosting        = NULL;
	__wrn("para->data=%d\n", para->data);
	hManWin = GUI_ManWinCreate(&create_info);
	return hManWin;
}

//mode 0:background 1:forground
__s32 app_linein_set_mode(H_WIN hwin, linein_mode_e mode)
{
	linein_man_ctrl_t *man_ctrl;
	__wrn("app_linein_set_mode, mode=%d\n", mode);

	if(NULL == hwin)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	man_ctrl = (linein_man_ctrl_t *)GUI_WinGetAddData(hwin);

	if(NULL == man_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	__wrn("man_ctrl->mode=%d\n", man_ctrl->mode);

	if(man_ctrl->mode == mode)
	{
		__wrn("already in mode:%s\n", LINEIN_MODE_BACKGROUND == mode ? "background" : "forground");
		return EPDK_OK;
	}

	__here__;

	if(LINEIN_MODE_BACKGROUND == mode)
	{
		__here__;

		if(man_ctrl->hfrm)
		{
			__here__;
			linein_win_delete(man_ctrl->hfrm);
			man_ctrl->hfrm = NULL;
		}

#if SP_APP_ALARM
		if(man_ctrl->h_frm_alarm_coming)
		{
			__here__;
			app_alarm_frm_destory(man_ctrl->h_frm_alarm_coming);
			man_ctrl->h_frm_alarm_coming = NULL;
		}
#endif
	}
	else
	{
		man_ctrl->hfrm = linein_frame_create(hwin);

		if(NULL == man_ctrl->hfrm)
		{
			__wrn("linein_frame_create fail...\n");
			return EPDK_FAIL;
		}

		GUI_WinSetFocusChild(man_ctrl->hfrm);
	}

	man_ctrl->mode = mode;
	return EPDK_OK;
}

//mode 0:background 1:forground
linein_mode_e app_linein_get_mode(H_WIN hwin)
{
	linein_man_ctrl_t *man_ctrl;

	if(NULL == hwin)
	{
		__wrn("invalid para...\n");
		return LINEIN_MODE_INVALID;
	}

	man_ctrl = (linein_man_ctrl_t *)GUI_WinGetAddData(hwin);

	if(NULL == man_ctrl)
	{
		__wrn("invalid para...\n");
		return LINEIN_MODE_INVALID;
	}

	return man_ctrl->mode;
}

__bool app_linein_has_bt(void)
{
	if(gs_auxMode & MASK_AUX_MODE_BT)		return 1;
	return 0;
}

#endif
