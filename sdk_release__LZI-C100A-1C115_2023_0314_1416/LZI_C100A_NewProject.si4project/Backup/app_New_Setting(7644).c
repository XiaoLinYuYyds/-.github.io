#include "app_New_Setting.h"
#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"
#include "number_menu_i.h"



/***********************************************************************************************************
	����ͼ��
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
	lstlyr.scn_win.y		= rect->y; //240Ϊ��Ļ�߶ȣ�60Ϊ�׶˸߶�
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
/**********************************************************************************
//����������
***********************************************************************************/
static __s32  _app_New_Setting_key_proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("app_New_Setting_key key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("app_New_Setting_key key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_ENTER, 0, 0);
			//main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP,SETTING_SW_TO_MAIN, 0);//ֱ�ӷ��ص�homeӦ�ý���
		}
		if(GUI_MSG_KEY_RIGHT == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("number_menu_numbers key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//Ϊup
			case GUI_MSG_KEY_LONGLEFT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_UP, 0, 0);//�������
			}
			break;

			case GUI_MSG_KEY_RIGHT: 	//Ϊdown
			case GUI_MSG_KEY_LONGRIGHT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_DOWN, 0, 0);
			}
			break;

			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_UP, 0, 0);
				break;

			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_DOWN, 0, 0);
				break;

			case GUI_MSG_KEY_ESCAPE:
			case GUI_MSG_KEY_MENU:
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_EXIT, 0, 0);
				break;

			default:
				break;
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
	#endif
}

/****************************************************
*�µ�ͨ������app���ڴ�������
****************************************************/
static __s32 app_new_setting_win_create(__gui_msg_t *msg)
{
	app_new_setting_ctrl_t	*app_new_setting_ctrl;
	new_setting_para_t general_para;
	RECT rect;
	SIZE screen_size;
	app_new_setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	rect.x = 0;
	rect.y = 0;
	dsk_display_get_size(&screen_size.width, &screen_size.height);//��ȡLCD��ʾ����С1024*600
	rect.width = screen_size.width;//1024
	rect.height = screen_size.height;//600
	__wrn("rect.x = %d , rect.y = %d,rect.width = %d , rect.heigh = %d\n",rect.x,rect.y,rect.width,rect.height);
	app_new_setting_ctrl->lry_new_frm_setting = app_New_Setting_8bpp_layer_create(&rect,1);//����ͼ�㴴��
	{//������������ӵ�ͨ������appӦ�ó���
		__wrn("root_type = 0 \n");
		__wrn("app_new_setting_win_create is... \n");
		general_para.layer = app_new_setting_ctrl->lry_new_frm_setting;//ͼ�㸳��
		if(!general_para.layer)//�ж�ͼ���Ƿ񴴽�
		{
			__wrn("general_para.layer is null \n");
			return EPDK_FAIL ;
		}
		general_para.font = app_new_setting_ctrl->setting_font;
		general_para.focus_id = 0;
		app_new_setting_ctrl->h_frm_general_setting = New_Setting_app_win_create(msg->h_deswin, &general_para);//��������ӵ�ͨ�����ô���framwin����
		GUI_WinSetFocusChild(app_new_setting_ctrl->h_frm_general_setting);//����ͣ�����ִ����Ҫ����
	}
	__wrn("app_new_setting_win_create  is ok\n");
	return EPDK_OK;
}

/*====================================================================================
*�������ã�manwinͼ�㴰�ڴ����ص�����
*�βΣ�__gui_msg_t *msg��GUI�̶�����Ľṹ��ָ�����
====================================================================================*/
static __s32 _app_New_Setting_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	__wrn("_app_New_Setting_proc msg->id = %d \n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//��������
		{
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			//com_set_palette_by_id(ID_SETTING_WKM_BMP);
			app_new_setting_win_create(msg);
			__wrn("_app_New_Setting_proc GUI_MSG_CREATE  is ok\n");
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//�رմ��ڣ�Ȼ�󷵻���һ�������ڽ���
		{
			__wrn("new Setting GUI_MSG_CLOSE...\n");
			main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, SETTING_SW_TO_MAIN, 0);//���͵�app_root_scene.c ��APP_NEWSETTING_ID������ִ�з��ص�homeӦ�ý���
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //���ٴ���
		{
			app_new_setting_ctrl_t	*app_new_setting_ctrl;
			__wrn("app_new_setting_ctrl destory begin ............\n");
			app_new_setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(!app_new_setting_ctrl)
			{
				__err("app_new_setting_ctrl is null ......\n");
				return EPDK_FAIL;
			}
			#if 0//����1
			if(app_new_setting_ctrl->h_frm_general_setting)//����ӵ�ͨ������ͼ�㣬���
			{	
				H_LYR h_lyr ;//���ͼ�����
				h_lyr = GUI_WinGetLyrWin(app_new_setting_ctrl->h_frm_general_setting);//��ȡ���ڵ�ͼ��
				if(!h_lyr)
				{
					__wrn("h_lyr is null \n");
					return EPDK_FAIL ;
				}
				__wrn(" lry_new_frm_setting isn't null, delete it \n");
				//GUI_LyrWinDelete(app_new_setting_ctrl->lry_new_frm_setting);//ɾ������ӵ�ͨ������ͼ��
				GUI_LyrWinDelete(h_lyr);
				__wrn(" lry_new_frm_setting isn't null, delete it OK\n");
				app_new_setting_ctrl->h_frm_general_setting = NULL ;//��ֵΪ��
			}
				__wrn("app_new_setting_ctrl destory begin is ok\n");
			#endif
			#if 1//����2
			if(app_new_setting_ctrl->lry_new_frm_setting)//����ӵ�ͨ������ͼ�㣬���
			{	
				__here__ ;
				__wrn(" lry_new_frm_setting isn't null, delete it... \n");
				GUI_LyrWinDelete(app_new_setting_ctrl->lry_new_frm_setting);//ɾ������ӵ�ͨ������ͼ��
				//GUI_LyrWinDelete(h_lyr);
				__wrn(" lry_new_frm_setting isn't null, delete it OK\n");
				__here__ ;
				app_new_setting_ctrl->lry_new_frm_setting = NULL ;//��ֵΪ��
			}
				__wrn("app_new_setting_ctrl destory begin is ok\n");
			#endif
			#if 0//����3
				if(app_new_setting_ctrl->h_frm_general_setting)
				{	
					__wrn(" h_frm_general_setting isn't null, delete it OK\n");
					app_dialog_destroy(app_new_setting_ctrl->h_frm_general_setting);//�ݻ�ͼ��
					__wrn(" h_frm_general_setting isn't null, delete it OK\n");
					app_new_setting_ctrl->h_frm_general_setting = NULL;
				}
			#endif
			My_Bfree(app_new_setting_ctrl, sizeof(app_new_setting_ctrl_t));//�ṹ�������ͷ��ڴ�ռ�
			
			
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//������ʾ����ͼƬ����
			__wrn("new Setting GUI_MSG_PAINT...\n");

		return EPDK_OK;

		case GUI_MSG_KEY:	//����������
			__wrn("new Setting GUI_MSG_KEY...\n");
			_app_New_Setting_key_proc(msg);
			#if 0
			switch(msg->dwAddData1)
			{
				__wrn("new Setting GUI_MSG_KEY_ESCAPE...\n");
				case GUI_MSG_KEY_ESCAPE:
				{
					__gui_msg_t my_msg;
					app_new_setting_ctrl_t	*app_new_setting_ctrl;
					__wrn("new Setting GUI_MSG_KEY_ESCAPE is...\n");
					my_msg.id = GUI_MSG_CLOSE;//�رմ���
					my_msg.h_srcwin = app_new_setting_ctrl->lry_new_frm_setting;
					my_msg.h_deswin = app_new_setting_ctrl->lry_new_frm_setting;
					my_msg.dwAddData1 = 0;
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					return EPDK_OK;
				}
			}
			#endif
		return EPDK_OK;
		

		case MSG_NUMBER_MENU_OP_ENTER:	//�س�ȷ�ϰ���
		{
			__gui_msg_t mymsg;
			key_number += 1;
			__wrn("new Setting GUI_MSG_KEY...\n");
			#if 0	
			if(key_number == 5)//ɾ��appӦ�ã�Ȼ�󷵻���һ��home��ʾȫ��appӦ��
			{
				key_number = 0;
				#if 1
				mymsg.id = GUI_MSG_CLOSE;//���ùرմ���
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#else
				mymsg.id = GUI_MSG_DESTROY;//�������ٴ���
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
				New_Setting_paint_one_item(msg,key_number);//ˢ��number������ʾ����
			}
			#endif
		}
		return EPDK_OK;
		case GUI_MSG_COMMAND:	//��������
		{
			app_new_setting_ctrl_t *setting_ctrl;
			setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			#if 1
			__wrn("app_new_setting.c GUI_MSG_COMMAND  is....\n");
			switch(LOWORD(msg->dwAddData1))
			{
				#if 0
				case GENERAL_TIPS_ID:
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_NUMBER_MENU_UP:	//�ϰ���1
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NUMBER_MENU_OP_UP;
							__wrn("MSG_NUMBER_MENU_OP_UP NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_NUMBER_MENU_DOWN:	//�°���2
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NUMBER_MENU_OP_DOWN;
							__wrn("MSG_NUMBER_MENU_OP_DOWN NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_NUMBER_MENU_ENTER:	//�س�����3
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
								GUI_WinSetFocusChild(setting_ctrl->h_frm_general_setting);
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
							GUI_WinSetFocusChild(setting_ctrl->h_frm_general_setting);
						}
						break;

						default:
							break;
					}
				}

				return EPDK_OK;
				#endif
				#if 0//�����ͨ������framwin����
				case NEW_SETTING_ID:
				{
					__wrn("NEW_SETTING_ID  is....\n");
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_NUMBER_MENU_ENTER:	//�س�����3
						{
							__gui_msg_t mymsg;
							__wrn("---ID_OP_NUMBER_MENU_ENTER  is....\n");
							mymsg.h_deswin = setting_ctrl->lry_new_frm_setting;//������ӵ�ͨ������Ӧ�ó�������ͼ��
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NUMBER_MENU_OP_ENTER;
							mymsg.dwAddData2 = msg->dwAddData2;
							__wrn("msg->dwAddData1=%d\n", msg->dwAddData1);
							GUI_SendMessage(&mymsg);
						}
						break;
						#if 0//��ת��home����
						case ID_NUMBER_MENU_SWITCH_TO_GENERAL://��ת��GENERAL����
						{
							H_LYR new_setting_win_layer;

							if(setting_ctrl == NULL)
							{
								__msg("setting_ctrl NEW error.........\n");
								return EPDK_FAIL;
							}

							if(setting_ctrl->lry_new_frm_setting == NULL)
							{
								__msg(" setting_ctrl->h_frm_tips NEW is null \n");
								GUI_WinSetFocusChild(setting_ctrl->h_frm_general_setting);//ѡ����Ҫ��ʾ��ͼ��
								return EPDK_FAIL ;
							}

							new_setting_win_layer = GUI_WinGetLyrWin(setting_ctrl->lry_new_frm_setting);

							if(new_setting_win_layer == NULL)
							{
								__msg(" new_setting_win_layer layer NEW is null \n");
								return EPDK_FAIL ;
							}

							GUI_LyrWinDelete(new_setting_win_layer);	//ɾ������ӵ�ͨ������appӦ�ó����ͼ��
							setting_ctrl->lry_new_frm_setting = NULL ;
							GUI_WinSetFocusChild(setting_ctrl->h_frm_general_setting);//ѡ����Ҫ��ʾ��ͼ��
						}
						break;
						#endif
						default:
							break;
					}
				}
				
				return EPDK_OK;
				#endif
			}
			#endif
			return EPDK_OK;
		}
		
		default:
			break;
	}

	return GUI_ManWinDefaultProc(msg);
}

/*====================================================================================
*�������ã�manwin���ڴ�������
*�βΣ�H_WIN h_parent�����ڶ��� ��__gui_msg_t *msg��GUI�̶�����Ľṹ��ָ�����
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
	create_info.name            = APP_SETTING;//APP_NEWSETTING;//�µ�����
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_New_Setting_proc);
	create_info.attr            = (void *)app_New_setting_ctrl;
	create_info.id			= APP_SETTING_ID;//APP_NEWSETTING_ID;//�µ�ID
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));								//����һ��ManWin��������
}
/*====================================================================================
*�������ã�����ɾ��
====================================================================================*/
__s32 app_New_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
	app_new_setting_ctrl_t	*app_New_setting_ctrl;
	app_New_setting_ctrl	 = (app_new_setting_ctrl_t *)GUI_WinGetAttr(hmanwin);
	__here__ ;
	
	return EPDK_OK;
}


