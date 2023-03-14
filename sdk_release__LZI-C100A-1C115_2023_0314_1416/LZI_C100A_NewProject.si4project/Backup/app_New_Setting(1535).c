#include "app_New_Setting.h"
#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"
#include "number_menu_i.h"



/***********************************************************************************************************
	建立图层
************************************************************************************************************/
H_LYR app_New_Setting_8bpp_layer_create(RECT *rect,__s32 pipe)
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
		"app New Setting layer",
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
	lstlyr.scn_win.y		= rect->y; //240为屏幕高度，60为底端高度
	lstlyr.scn_win.width  	= rect->width;
	lstlyr.scn_win.height 	= rect->height;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app New Setting layer create error !\n");
	}
	__wrn("app_New_Setting_8bpp_layer_create CREATE\n");
	return layer;
}
/****************************************************
*新的通用设置app窗口创建函数
****************************************************/
static __s32 app_new_setting_win_create(__gui_msg_t *msg)
{
	app_new_setting_ctrl_t	*app_new_setting_ctrl;
	New_Setting_para_t general_para;
	RECT rect;
	SIZE screen_size;
	app_new_setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	rect.x = 0;
	rect.y = 0;
	dsk_display_get_size(&screen_size.width, &screen_size.height);//获取LCD显示屏大小1024*600
	rect.width = screen_size.width;//1024
	rect.height = screen_size.height;//600
	app_new_setting_ctrl->lyr_setting = app_New_Setting_8bpp_layer_create(&rect,1);//图层创建
	if(app_new_setting_ctrl->root_type == APP_NEW_SETTING_GENERAL)//等于0
	{
		__wrn("root_type = 0 \n");
		
		general_para.layer = app_new_setting_ctrl->lyr_setting;
		general_para.msgbox_font = app_new_setting_ctrl->setting_font;
		general_para.focus_id = 0;
		app_new_setting_ctrl->h_frm_general = New_Setting_app_win_create(msg->h_deswin, &general_para);//进入新的framwin
		GUI_WinSetFocusChild(app_new_setting_ctrl->h_frm_general);
	}
	__wrn("app_new_setting_win_create  is ok\n");
	return EPDK_OK;
}

/*====================================================================================
*函数作用：manwin图层窗口创建回调函数
*形参：__gui_msg_t *msg：GUI固定必须的结构体指针变量
====================================================================================*/
static __s32 _app_New_Setting_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//创建窗口
		{
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			com_set_palette_by_id(ID_SETTING_WKM_BMP);
			app_new_setting_win_create(msg);
			__wrn("_app_New_Setting_proc GUI_MSG_CREATE  is ok\n");
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//关闭窗口，然后返回上一层主窗口界面
		{
			
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //销毁窗口
		{
			
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//绘制显示所有图片内容
			__wrn("new Setting \n");

		return EPDK_OK;

		case GUI_MSG_KEY:	//按键处理函数

		return EPDK_OK;
		

		case MSG_NUMBER_MENU_OP_ENTER:	//回车确认按键
		{
			__gui_msg_t mymsg;
			key_number += 1;
			#if 0	
			if(key_number == 5)//删除app应用，然后返回上一层home显示全部app应用
			{
				key_number = 0;
				#if 1
				mymsg.id = GUI_MSG_CLOSE;//调用关闭窗口
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#else
				mymsg.id = GUI_MSG_DESTROY;//调用销毁窗口
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#endif
			}
			else
			{	
				__wrn("new setkey_number =%d  \n",key_number);
				New_Setting_paint_one_item(msg,key_number);//刷新number次数显示内容
			}
			#endif
		}
		return EPDK_OK;
		case GUI_MSG_COMMAND:	//命令输入
		{
			app_new_setting_ctrl_t *setting_ctrl;
			setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			switch(LOWORD(msg->dwAddData1))
			{
				case GENERAL_TIPS_ID:
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_NUMBER_MENU_UP:	//上按键1
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NUMBER_MENU_OP_UP;
							__wrn("MSG_NUMBER_MENU_OP_UP NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_NUMBER_MENU_DOWN:	//下按键2
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NUMBER_MENU_OP_DOWN;
							__wrn("MSG_NUMBER_MENU_OP_DOWN NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_NUMBER_MENU_ENTER:	//回车按键3
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NUMBER_MENU_OP_ENTER;
							mymsg.dwAddData2 = msg->dwAddData2;
							__wrn("msg->dwAddData1=%d\n", msg->dwAddData1);
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_NUMBER_MENU_SWITCH_TO_GENERAL:
						{
							H_LYR tips_win_layer;

							if(setting_ctrl == NULL)
							{
								__msg("setting_ctrl NEW error.........\n");
								return EPDK_FAIL;
							}

							if(setting_ctrl->h_frm_tips == NULL)
							{
								__msg(" setting_ctrl->h_frm_tips NEW is null \n");
								GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
								return EPDK_FAIL ;
							}

							tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_tips);

							if(tips_win_layer == NULL)
							{
								__msg(" tips_win layer NEW is null \n");
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

/*====================================================================================
*函数作用：manwin窗口创建函数
*形参：H_WIN h_parent：窗口定义 ；__gui_msg_t *msg：GUI固定必须的结构体指针变量
====================================================================================*/
H_WIN app_New_Setting_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	app_new_setting_ctrl_t *app_New_setting_ctrl;
	__inf("****************************************************************************************\n");
	__inf("********  enter new setting app  **************\n");
	__inf("****************************************************************************************\n");
	//__wait__;
	app_New_setting_ctrl = (app_new_setting_ctrl_t *)My_Balloc(sizeof(app_new_setting_ctrl_t));

	if(!app_New_setting_ctrl)
	{
		__msg("memory balloc fail.........\n");
		return EPDK_FAIL;
	}

	eLIBs_memset(app_New_setting_ctrl, 0, sizeof(app_new_setting_ctrl_t));
	app_New_setting_ctrl->setting_font = para->font;
	app_New_setting_ctrl->root_type = para->setting_opt;
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_NEWSETTING;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_New_Setting_proc);
	create_info.attr            = (void *)app_New_setting_ctrl;
	create_info.id			= APP_NEWSETTING_ID;
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));								//返回一个ManWin创建窗口
}
/*====================================================================================
*函数作用：窗口删除
====================================================================================*/
__s32 app_New_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
	app_new_setting_ctrl_t	*app_New_setting_ctrl;
	app_New_setting_ctrl	 = (app_new_setting_ctrl_t *)GUI_WinGetAttr(hmanwin);
	__here__ ;
	
	return EPDK_OK;
}


