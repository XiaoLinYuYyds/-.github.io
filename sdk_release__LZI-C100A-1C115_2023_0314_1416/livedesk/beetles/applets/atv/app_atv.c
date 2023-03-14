/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_atv.c
* By      	: Andy.zhang
* Func		: desk atv thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "app_atv.h"
#include "atv_menu.h"
#include "sub_menu.h"
#include "atv_uipara.h"
#include "app_atv_i.h"

#if SP_APP_ATV


typedef struct tag_atv_para
{
	H_WIN	h_app_atv;
	H_WIN hWin_menu;
	H_WIN	hLyr_frame;

	GUI_FONT *atv_font;

	__u32	m_maskFlags;
}
atv_para_t;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/

static H_LYR atv_32bpp_layer_create(RECT *rect, __s32 pipe)
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
		0,                                              /* pipe      */
		0xff,                                           /* prio      */
		{0, 0, 0, 0},                           		/* screen    */
		{0, 0, 0, 0},                               	/* source    */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
		NULL                                            /* fb        */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"atv menu layer",
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
		__err("app bar layer create error !\n");
	}

	return layer;
}

void  atv_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = GUI_WinGetParent(hwin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);
}

static void app_atv_menu_create(atv_para_t *atv_para)
{
	RECT	rect;
	mmenu_para_t mmenu_para;
	atv_uipara_t *atv_ui_para;
	atv_ui_para = atv_get_ui_para(GUI_GetScnDir());

	if(!atv_ui_para)
	{
		__err("atv_ui_para is null...\n");
		return ;
	}

	rect.x = atv_ui_para->lyr_atv_menu.x;
	rect.y = atv_ui_para->lyr_atv_menu.y;
	rect.width = atv_ui_para->lyr_atv_menu.w;
	rect.height = atv_ui_para->lyr_atv_menu.h;
	__wrn("rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n",
	      rect.x, rect.y, rect.width, rect.height);
	atv_para->hLyr_frame = atv_32bpp_layer_create(&rect, 1);
	mmenu_para.hLyr_frame = atv_para->hLyr_frame;
	mmenu_para.mmenu_font = atv_para->atv_font;
	mmenu_para.m_maskFlags = atv_para->m_maskFlags;
	atv_para->hWin_menu = atv_menu_win_create(atv_para->h_app_atv, &mmenu_para);
	__msg("atv_para->hWin_menu=%x\n", atv_para->hWin_menu);
	GUI_WinSetFocusChild(atv_para->hWin_menu);
}

static __s32 app_atv_proc(__gui_msg_t *msg)
{
	//  __msg("app_atv_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//      ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			atv_para_t *atv_para;
			root_para_t *root_para;
			root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
			atv_para = (atv_para_t *)esMEMS_Balloc(sizeof(atv_para_t));

			if(!atv_para)
			{
				return EPDK_FAIL;
			}

			eLIBs_memset((void *)atv_para, 0, sizeof(atv_para_t));
			atv_para->atv_font = root_para->font;
			atv_para->h_app_atv = msg->h_deswin;

			atv_para->m_maskFlags |= root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode;

			//主菜单
			IOCTRL__ATV_PW__SET_ON();
			app_atv_menu_create(atv_para);

			if(atv_para->hLyr_frame && !(atv_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode))
			{
				GUI_LyrWinSetSta(atv_para->hLyr_frame, GUI_LYRWIN_STA_ON);
				GUI_LyrWinSetTop(atv_para->hLyr_frame);
			}
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			gscene_bgd_set_bottom();
			GUI_WinSetFocusChild(atv_para->hWin_menu);
			GUI_WinSetAddData(msg->h_deswin, (__u32)atv_para);
			__here__;
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:
			//GUI_ManWinDelete(msg->h_deswin);
			atv_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
		return EPDK_OK;

		case GUI_MSG_DESTROY:
			__wrn("app_atv_proc:GUI_MSG_DESTROY:begin\n");
			{
				atv_para_t *atv_para;
				root_para_t *root_para;
				root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
				atv_para = (atv_para_t *)GUI_WinGetAddData(msg->h_deswin);

				if(atv_para->hLyr_frame)
				{
					GUI_LyrWinDelete(atv_para->hLyr_frame);
					atv_para->hLyr_frame = 0;
				}
				
				esMEMS_Bfree(atv_para, sizeof(atv_para_t));
				
				root_para->m_maskFlags &= ~MASK_RootFlags_CVBS_InputMode;
			}
			__wrn("app_atv_proc:GUI_MSG_DESTROY:over\n");
			return EPDK_OK;

		case GUI_MSG_KEY:
			__msg("******app_atv_proc GUI_MSG_KEY*****\n");

			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_MENU:
					__wrn("....GUI_MSG_KEY_ESCAPE.........\n");

					if(0)
					{
						__gui_msg_t my_msg;
						my_msg.id = GUI_MSG_CLOSE;
						my_msg.h_srcwin = msg->h_deswin;
						my_msg.h_deswin = msg->h_deswin;
						my_msg.dwAddData1 = 0;
						my_msg.dwAddData2 = 0;
						GUI_SendNotifyMessage(&my_msg);
						return EPDK_OK;
					}

					break;
			}

			break;

		case GUI_MSG_TOUCH:
			break;

		case DSK_MSG_FS_PART_PLUGIN:
		case DSK_MSG_FS_PART_PLUGOUT:
			return EPDK_OK;


		case GUI_MSG_COMMAND:
		{
			atv_para_t *atv_para;
			atv_para = (atv_para_t *)GUI_WinGetAddData(msg->h_deswin);

			switch(msg->dwAddData1)
			{
				case GUI_MSG_ATV_CUE_INSERT_AVIN :
				{
					H_WIN dlg;
					__s32 lang_id[] = {STRING_ROOT_CUE, STRING_ATV_INSERT_AVIN};
					default_dialog(dlg, atv_para->h_app_atv, APP_ATV_DLG_ID, ADLG_OK, lang_id);
					esKRNL_TimeDly(120);
					app_dialog_destroy(dlg);
					gscene_bgd_set_bottom();
				}
				break;

				case GUI_MSG_ATV_TO_LINEIN :
					__wrn("GUI_MSG_ATV_TO_LINEIN\n");
					atv_cmd2parent(msg->h_deswin, SWITCH_TO_LINE_IN, 0, 0);
					break;

				default :
					break;
			}
		}

		return EPDK_OK;

		case DSK_MSG_SCREEN_OPEN :
		{
			atv_para_t *atv_para;
			atv_para = (atv_para_t *)GUI_WinGetAddData(msg->h_deswin);

			if(atv_para->hWin_menu)
			{
				__gui_msg_t mymsg;
				mymsg.h_deswin = atv_para->hWin_menu;
				mymsg.h_srcwin = NULL;
				mymsg.id = DSK_MSG_SCREEN_OPEN;
				GUI_SendMessage(&mymsg);
			}
			return EPDK_OK;
		}

		default :
			break;
	}

	return GUI_ManWinDefaultProc(msg);
}

/*******************************************************************************************************
*********************************************************************************************************/
H_WIN app_atv_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	__inf("****************************************************************************************\n");
	__inf("********  enter atv app  **************\n");
	__inf("****************************************************************************************\n");
	//__wait__;
	g_display_switch_output(LION_DISP_LCD);
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_ATV;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_atv_proc);
	create_info.attr            = (void *)para;
	create_info.id				= APP_ATV_ID;
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));
}


#endif
