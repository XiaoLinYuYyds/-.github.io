/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_settings.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "number_menu.h"
#include "app_setting_i.h"
#include "setting_para.h"
#include "setting_general.h"
#include "setting_power.h"
#include "setting_tips.h"
/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR number_layer_palette_create(RECT *rect)
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
		"number layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= 500;//rect->width;
	lstlyr.src_win.height 	= 300;//rect->height;
	lstlyr.scn_win.x		= 30;//rect->x;
	lstlyr.scn_win.y		= 20;//rect->y;
	lstlyr.scn_win.width  	= 500;//rect->width;
	lstlyr.scn_win.height 	= 300;//rect->height;
	lstlyr.pipe = 1;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app bar layer create error !\n");
	}

	return layer;
}
/****************************************************************
*子菜单内容创建
****************************************************************/
static __s32 number_win_create(__gui_msg_t *msg)
{
	number_ctrl_t	*number_ctrl;
	RECT rect;
	SIZE screen_size;
	number_ctrl = (number_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	rect.x = 0;
	rect.y = 0;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	rect.width = 600;//screen_size.width;
	rect.height = 400;//screen_size.height;
	number_ctrl->lyr_number = number_layer_palette_create(&rect);//创建图层

	if(number_ctrl->root_type == NUMBER_GENERAL)
	{
		setting_general_para_t general_para;
		general_para.layer = number_ctrl->lyr_number;//setting_ctrl->lyr_setting;
		general_para.font = number_ctrl->number_font;//setting_ctrl->setting_font;
		general_para.focus_id = 0;
		number_ctrl->h_frm_general = setting_general_win_create(msg->h_deswin, &general_para);
		GUI_WinSetFocusChild(number_ctrl->h_frm_general);
	}
#if SP_APP_SETTING_POWER
	else if(number_ctrl->root_type == NUMBER_POWER_MANAGE)
	{
		setting_power_para_t power_para;
		power_para.layer = 0;//setting_ctrl->lyr_setting;
		power_para.font = 0;//setting_ctrl->setting_font;
		power_para.focus_id = 0;
		number_ctrl->h_frm_power = setting_power_win_create(msg->h_deswin, &power_para);
		GUI_WinSetFocusChild(number_ctrl->h_frm_power);
	}
#endif

	return EPDK_OK;
}
/*******************************************************
*菜单应用程序回调函数
*******************************************************/
static __s32 _app_number_Proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//创建子菜单
		{
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			com_set_palette_by_id(ID_SETTING_WKM_BMP);
			number_win_create(msg);
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:		//关闭
		{
			main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, SETTING_SW_TO_MAIN, 0);
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY:	//销毁
		{
			number_ctrl_t	*number_ctrl;
			__msg("setting destory begin ............\n");
			number_ctrl	 = (number_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(!number_ctrl)
			{
				__err("number_ctrl is null ......\n");
				return EPDK_FAIL;
			}

			__here__;

			if(number_ctrl->lyr_number)
			{
				__here__ ;
				__msg(" lyr_number isn't null, delete it \n");
				GUI_LyrWinDelete(number_ctrl->lyr_number);
				__here__ ;
				number_ctrl->lyr_number = NULL ;
			}

			My_Bfree(number_ctrl, sizeof(number_ctrl_t));
			//	esKSRV_SysInfo();
		}

		return EPDK_OK;

		case GUI_MSG_KEY:
		{
			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_ESCAPE:
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
			}
		}
		break;

		case DSK_MSG_FS_PART_PLUGIN:
		case DSK_MSG_FS_PART_PLUGOUT:
		{
			number_ctrl_t *setting_ctrl;
			setting_ctrl = (number_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			__here__;

			if(setting_ctrl->h_frm_tips)
			{
				__gui_msg_t set_msg;
				eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
				set_msg.h_deswin = setting_ctrl->h_frm_tips;
				__here__;
				GUI_SendMessage(&set_msg);
				__here__;
			}

			return EPDK_OK;
		}

		case GUI_MSG_COMMAND:
		{
			number_ctrl_t *setting_ctrl;
			setting_ctrl = (number_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			switch(LOWORD(msg->dwAddData1))
			{
				case GENERAL_MENU_ID:
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_LEFT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_LEFT;
							mymsg.dwReserved = msg->dwReserved;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_RIGHT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_RIGHT;
							mymsg.dwReserved = msg->dwReserved;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_UP:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_UP;
							mymsg.dwReserved = msg->dwReserved;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_DOWN:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_DOWN;
							mymsg.dwReserved = msg->dwReserved;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_ENTER:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_ENTER;
							mymsg.dwReserved = msg->dwReserved;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_EXIT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_EXIT;
							mymsg.dwReserved = msg->dwReserved;
							GUI_SendMessage(&mymsg);
						}
						break;

						default:
							break;
					}
				}

				return EPDK_OK;

				case POWER_MANAGE_MENU_ID:
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_LEFT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_power;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_LEFT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_RIGHT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_power;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_RIGHT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_UP:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_power;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_UP;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_DOWN:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_power;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_DOWN;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_ENTER:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_power;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_ENTER;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_EXIT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_power;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_EXIT;
							GUI_SendMessage(&mymsg);
						}
						break;

						default:
							break;
					}
				}

				return EPDK_OK;

				case GENERAL_TIPS_ID:
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_LEFT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_LEFT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_RIGHT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_RIGHT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_UP:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_UP;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_DOWN:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_DOWN;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_ENTER:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_ENTER;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_EXIT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_EXIT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_SWITCH_TO_GENERAL:
						{
							H_LYR tips_win_layer;

							if(setting_ctrl == NULL)
							{
								__msg("setting_ctrl error.........\n");
								return EPDK_FAIL;
							}

							if(setting_ctrl->h_frm_tips == NULL)
							{
								__msg(" setting_ctrl->h_frm_tips is null \n");
								GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
								return EPDK_FAIL ;
							}

							tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_tips);

							if(tips_win_layer == NULL)
							{
								__msg(" tips_win layer is null \n");
								return EPDK_FAIL ;
							}

							GUI_LyrWinDelete(tips_win_layer);
							setting_ctrl->h_frm_tips = NULL ;
							GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
						}
						break;

						default:
							break;
					}
				}

				return EPDK_OK;


				default:
					break ;
			}

			return EPDK_OK;
		}

		default:
			break;
	}

	return GUI_ManWinDefaultProc(msg);
}
/****************************************************
*创建子菜单的窗口
****************************************************/
H_WIN app_number_create(root_para_t *para)

{
	__gui_manwincreate_para_t create_info;
	number_ctrl_t *number_ctrl;
	__inf("****************************************************************************************\n");
	__inf("********  enter number app  **************\n");
	__inf("****************************************************************************************\n");
	//__wait__;
	number_ctrl = (number_ctrl_t *)My_Balloc(sizeof(number_ctrl_t));

	if(!number_ctrl)
	{
		__msg("memory balloc fail.........\n");
		return EPDK_FAIL;
	}

	eLIBs_memset(number_ctrl, 0, sizeof(number_ctrl_t));
	number_ctrl->number_font = para->font;
	number_ctrl->root_type = para->setting_opt;
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_NUMBER;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_number_Proc);
	create_info.attr            = (void *)number_ctrl;
	create_info.id			= APP_SETTING_ID;
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));
}
/**********************************************************************
*删除子菜单的窗口
***********************************************************************/
__s32 app_number_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
	number_ctrl_t	*number_ctrl;
	number_ctrl	 = (number_ctrl_t *)GUI_WinGetAttr(hmanwin);
	__here__ ;
	/*
		if(!setting_ctrl)
		{
			__wrn("setting_ctrl is null ......\n");
			return EPDK_FAIL;
		}

		// 为防止图层不够
		if(setting_ctrl->h_frm_product_info_win)
		{
			H_LYR tips_win_layer ;
			tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_product_info_win);
			GUI_LyrWinDelete(tips_win_layer);
			setting_ctrl->h_frm_product_info_win = NULL ;
		}
		__here__ ;

		if(setting_ctrl->h_frm_dafault_dialog_win)
		{
			app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
			setting_ctrl->h_frm_dafault_dialog_win = NULL;
		}
		__here__ ;
		if(setting_ctrl->h_frm_alarm_select_song)
		{
			app_alarm_sel_song_delete(setting_ctrl->h_frm_alarm_select_song);
			setting_ctrl->h_frm_alarm_select_song = NULL ;
		}
		__here__ ;

		if(setting_ctrl->h_frm_alarm_clock)
		{
			setting_alarm_clock_frm_destory(setting_ctrl->h_frm_alarm_clock);
			setting_ctrl->h_frm_alarm_clock = NULL ;
		}

	    */
	return EPDK_OK;
}



