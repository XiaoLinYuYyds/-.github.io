#include "app_New_Setting.h"
#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"
//#include "number_menu_i.h"
#include "New_Setting_i.h"



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
*�������ã���APP_NEWSETTING_ID����ӵ�appӦ�ó���manwin�����ڷ��Ͱ���֪ͨ��Ϣ
***********************************************************************************/
void  app_new_setting_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = GUI_WinGetParent(hwin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	__wrn("**********app_new_setting_cmd2parent**********\n");
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);//GUI��manwin�����ڷ��͹���֪ͨ
}

/**********************************************************************************
*�������ã������appӦ��manwin���ڵİ������������������framewin����ʱ���Բ���
***********************************************************************************/
static __s32  _app_New_Setting_key_proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("app_New_Setting_key key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("app_New_Setting_key key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)//�س�
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_ENTER, 0, 0);
			//app_new_setting_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP,SETTING_SW_TO_MAIN, 0);//�س����� �����źŵ�app_root_scene.c �����case APP_NEWSETTING_ID:ִ��ֱ�ӷ��ص�homeӦ�ý��湦��
		}
		if(GUI_MSG_KEY_RIGHT == last_key)//��
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)//��
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("app_New_Setting_key key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//4Ϊup
			case GUI_MSG_KEY_LONGLEFT:
			{
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);//�������
			}
			break;

			case GUI_MSG_KEY_RIGHT: 	//Ϊdown
			case GUI_MSG_KEY_LONGRIGHT:
			{
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
			}
			break;

			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
				break;

			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
				break;

			case GUI_MSG_KEY_ESCAPE:
			case GUI_MSG_KEY_MENU:
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_EXIT, 0, 0);
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

/*********************************************************************************
*�������ã�����ӵ�appӦ��framewin���ڵ��ô�������
�βΣ�
**********************************************************************************/
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
	rect.width = screen_size.width;		//framewin���ڿ�1024
	rect.height = screen_size.height;	//framewin���ڸ�600
	__wrn("rect.x = %d , rect.y = %d,rect.width = %d , rect.heigh = %d\n",rect.x,rect.y,rect.width,rect.height);
	app_new_setting_ctrl->lry_new_frm_setting = app_New_Setting_8bpp_layer_create(&rect,1);//����ͼ�㴴��
	{//������������ӵ�ͨ������appӦ�ó���framewin����
		__wrn("root_type = 0 \n");
		__wrn("app_new_setting_win_create is... \n");
		general_para.layer = app_new_setting_ctrl->lry_new_frm_setting;//ͼ�㸳��
		if(!general_para.layer)											//�ж�ͼ���Ƿ񴴽�ͼ��ɹ�
		{
			__wrn("general_para.layer is null \n");
			return EPDK_FAIL ;
		}
		general_para.font = app_new_setting_ctrl->setting_font;
		general_para.focus_id = 0;
		app_new_setting_ctrl->h_frm_general_setting = New_Setting_app_win_create(msg->h_deswin, &general_para);//��������ӵ�appӦ�ó��򴴽�framwin�Ӵ���
		GUI_WinSetFocusChild(app_new_setting_ctrl->h_frm_general_setting);//ѡ��ָ�����ڽ�����Ϣ����ͣ�����ִ����Ҫ����
	}
	__wrn("app_new_setting_win_create  is ok\n");
	return EPDK_OK;
}

/*====================================================================================
*�������ã�manwinͼ�㴰�ڻص�����
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
			app_new_setting_win_create(msg);	//����app��framewin����
			__wrn("_app_New_Setting_proc GUI_MSG_CREATE  is ok\n");
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//�رմ��ڣ�Ȼ�󷵻���һ�������ڽ���
		{
			__wrn("app new Setting GUI_MSG_CLOSE...\n");
			app_new_setting_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);//���͵�app_root_scene.c ��case APP_NEWSETTING_ID:������ִ�з��ص�homeӦ�ý���
		}

		return EPDK_OK;
		
		case GUI_MSG_QUIT:	//�˳���ǰ��appӦ�ó���manwin���ڣ�Ȼ����ת��ͨ�����õ�app_setting.c�ļ���appӦ�ó���manwin���ڽ���
		{
			__wrn("app new Setting GUI_MSG_QUIT...\n");
			app_new_setting_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_SETTING, 0);//���͵�app_root_scene.c ��case APP_NEWSETTING_ID:������ִ����ת�л���app_setting.c �Ĵ���appӦ��manwin���ڽ���
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //�س����½��� ���ٴ���
		{
			app_new_setting_ctrl_t	*app_new_setting_ctrl;
			__wrn("app_new_setting_ctrl destory begin ............\n");
			app_new_setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(!app_new_setting_ctrl)
			{
				__err("app_new_setting_ctrl is null ......\n");
				return EPDK_FAIL;
			}
			#if 0//����1 ɾ��ͼ��
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
			#if 1//����2 ɾ��ͼ��
			if(app_new_setting_ctrl->lry_new_frm_setting)//����ӵ�ͨ������ͼ�㣬���
			{	
				__here__ ;
				__wrn(" lry_new_frm_setting isn't null, delete it... \n");
				GUI_LyrWinDelete(app_new_setting_ctrl->lry_new_frm_setting);//ɾ������ӵ�appӦ��ͼ��
				//GUI_LyrWinDelete(h_lyr);
				__wrn(" lry_new_frm_setting isn't null, delete it OK\n");
				__here__ ;
				app_new_setting_ctrl->lry_new_frm_setting = NULL ;//��ֵΪ��
			}
				__wrn("app_new_setting_ctrl destory begin is ok\n");
			#endif
			#if 0//����3 ɾ��ͼ��
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
			__wrn("app new Setting GUI_MSG_PAINT...\n");

		return EPDK_OK;

		case GUI_MSG_KEY:	//manwin���ڰ����������������� break �����Ϳ�����framewin���ڷ��Ͱ�����Ϣ
		{
			__wrn("app new Setting GUI_MSG_KEY...\n");
			__wrn("app new Setting GUI_MSG_KEY msg->dwAddData1=%d...\n",msg->dwAddData1);
			//_app_New_Setting_key_proc(msg);
			#if 1
			switch(msg->dwAddData1)
			{
				__wrn("new Setting GUI_MSG_KEY_ESCAPE...\n");
				#if 0//�������������Ű�ť���¹��ܣ����͹رմ���id���˻ص�home�����洰����ʾ����app
				case GUI_MSG_KEY_ENTER://���ڻس��͹رմ���
				{	
					#if 1
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_KEY_ESCAPE is...\n");
					my_msg.id = GUI_MSG_CLOSE;//�رմ���
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = 0;
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#else
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_COMMAND is...\n");
					my_msg.id = GUI_MSG_COMMAND;//��������
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = NEW_SETTING_ID;//����id
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#endif
					#if 0
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_COMMAND is...\n");
					my_msg.id = MSG_NEW_SETTING_OP_ENTER;//����س�ȷ��
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = 0;//����id
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#endif
					return EPDK_OK;
				}
				#endif
				case GUI_MSG_KEY_ESCAPE://�����ذ������£�Ȼ��رմ��ڣ��˻ص�home�����洰����ʾ����app
				{
					#if 1
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_KEY_ESCAPE is...\n");
					my_msg.id = GUI_MSG_CLOSE;//�رմ���
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = 0;
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#endif
					return EPDK_OK;
				}
			}
			#endif
		}
		break;
	//return EPDK_OK;

		case MSG_NEW_SETTING_OP_ENTER:	//�س�ȷ�ϰ���
		{
			__gui_msg_t mymsg;
			app_new_setting_ctrl_t	*app_new_setting_ctrl;
			key_number += 1;
			__wrn("app new Setting MSG_NUMBER_MENU_OP_ENTER...\n");
			#if 0	
			if(key_number == 5)//ɾ��appӦ�ã�Ȼ�󷵻���һ��home��ʾȫ��appӦ��
			{
				key_number = 0;
				#if 0
				mymsg.id = GUI_MSG_CLOSE;//���ùرմ���
				mymsg.h_deswin = app_new_setting_ctrl->lry_new_frm_setting;
				mymsg.h_srcwin = NULL;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#else
				mymsg.id = GUI_MSG_CLOSE;//GUI_MSG_DESTROY;//�������ٴ���
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = 0;
				mymsg.dwAddData2 = 0;
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
			__wrn("app_new_setting.c LOWORD(msg->dwAddData1) = 0x%x  is....\n",LOWORD(msg->dwAddData1));
			switch(LOWORD(msg->dwAddData1))
			{
				#if 1//���뵽�����New_Setting.c��framewin����
				__wrn("NEW_SETTING_ID  is....\n");
				case NEW_SETTING_ID://0x9700
				{
					__wrn("NEW_SETTING_ID  is ok\n");
					__wrn("HIWORD(msg->dwAddData1) = %d is....\n",HIWORD(msg->dwAddData1));
					switch(HIWORD(msg->dwAddData1))
					{
						__wrn("HIWORD(msg->dwAddData1)  is....\n");
						case ID_OP_NEW_SETTING_UP:	//�ϰ���1
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general_setting;//��New_Setting.c��framewin���ڷ��Ͱ����ź�
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NEW_SETTING_OP_UP;
							__wrn("MSG_NUMBER_MENU_OP_UP NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_NEW_SETTING_DOWN:	//�°���2
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general_setting;//��New_Setting.c��framewin���ڷ��Ͱ����ź�
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NEW_SETTING_OP_DOWN;
							__wrn("MSG_NUMBER_MENU_OP_DOWN NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;
						case ID_OP_NEW_SETTING_ENTER:	//�س�����3
						{
							#if 1
							__gui_msg_t mymsg;
							__wrn("---ID_OP_NUMBER_MENU_ENTER 1 is....\n");
							mymsg.h_deswin = setting_ctrl->h_frm_general_setting;//��New_Setting.c��framewin���ڷ��Ͱ����ź�
							__wrn("---ID_OP_NUMBER_MENU_ENTER 2 is....\n");
							mymsg.h_srcwin = NULL;
							__wrn("---ID_OP_NUMBER_MENU_ENTER 3 is....\n");
							mymsg.id = MSG_NEW_SETTING_OP_ENTER;				 //�س�ȷ��id
							__wrn("---ID_OP_NUMBER_MENU_ENTER 4 is....\n");
							//mymsg.dwReserved = msg->dwReserved;
							__wrn("---ID_OP_NUMBER_MENU_ENTER 5 is....\n");
							GUI_SendMessage(&mymsg);							 //GUI������Ϣ
							__wrn("---ID_OP_NUMBER_MENU_ENTER 6 is....\n");
							#endif
						}
						break;
						case ID_OP_NEW_SETTING_EXIT:	//�����˳������appӦ�ó����manwin����id��Ϣ�ź�
						{
							#if 1//���Ͱ����źŵ���ǰ��APP_NEWSETTING_ID������manwin���ڵ�д��
							__gui_msg_t my_msg;
							__wrn("new Setting ID_OP_NEW_SETTING_EXIT GUI_MSG_KEY_ESCAPE is...\n");
							my_msg.id = GUI_MSG_CLOSE;			//��ǰ�ļ�manwin���ڵĹرմ���id
							my_msg.h_srcwin = msg->h_srcwin;	//app_New_Setting.c������manwin���ڷ��͹ر��ź�
							my_msg.h_deswin = msg->h_deswin;	//app_New_Setting.c������manwin���ڷ��͹ر��ź�
							my_msg.dwAddData1 = 0;
							my_msg.dwAddData2 = 0;
							GUI_SendNotifyMessage(&my_msg);		//����֪ͨ��Ϣ
							return EPDK_OK;						//ֱ���˳����case GUI_MSG_COMMAND:���
							#endif
						}
						break;
						case ID_NEW_SETTING_SWITCH_TO_SETTING:  //�����л���תappӦ�ó����id��Ϣ�ź�
						{
							#if 1//���Ͱ����źŵ���ǰ��APP_NEWSETTING_ID������manwin���ڵ�д��
							__gui_msg_t my_msg;
							__wrn("new Setting ID_NEW_SETTING_SWITCH_TO_SETTING GUI_MSG_KEY_ESCAPE is...\n");
							my_msg.id = GUI_MSG_QUIT;			//��ǰ�ļ�manwin���ڵ��˳�����id
							my_msg.h_srcwin = msg->h_srcwin;	//app_New_Setting.c������manwin���ڷ����˳��ź�
							my_msg.h_deswin = msg->h_deswin;	//app_New_Setting.c������manwin���ڷ����˳��ź�
							my_msg.dwAddData1 = 0;
							my_msg.dwAddData2 = 0;
							GUI_SendNotifyMessage(&my_msg);		//����֪ͨ��Ϣ����ǰmanwin����
							return EPDK_OK;						//ֱ���˳����case GUI_MSG_COMMAND:���
							#endif
						}

						default:
							break;
					}
				}
				
				//return EPDK_OK;
				default:
					break ;
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
*�������ã�����ӵ�appӦ��manwin���ڴ�������
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
	create_info.name            = APP_NEWSETTING;//APP_SETTING;//�µ�����
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_New_Setting_proc);
	create_info.attr            = (void *)app_New_setting_ctrl;
	create_info.id			= APP_NEWSETTING_ID;//APP_SETTING_ID;//�µ�ID
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


