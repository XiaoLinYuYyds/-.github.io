#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"




/***********************************************************************************************************
	����ͼ��
************************************************************************************************************/
H_LYR New_Setting_8bpp_layer_create(RECT *rect,__s32 pipe)
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
		"New_Setting layer",
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
		__err("New_Setting layer create error !\n");
	}
	__wrn("New_Setting_8bpp_layer_create CREATE\n");
	return layer;
}
/****************************************************
*�س�����������ȡ����
****************************************************/
void New_Setting_get_volume_size(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str�����ڴ�ռ��ʼ��
	eLIBs_strcat(str, "number:");
	snum = num;
	eLIBs_uint2str_dec(snum, numstr);//int��תchar��
	 
	if(snum >= 10)
	{
		eLIBs_strcat(str, numstr);
	}
	else
	{
		eLIBs_strcat(str, "0");
		eLIBs_strcat(str, numstr);
	}

	eLIBs_strcpy(New_Setting_ctrl->new_number_size, (const char *)str);


}
/******************************************************************
*��ʼ��������ʹ��ͼƬ��Դ
*******************************************************************/
static void New_Setting_init(New_Setting_ctrl_t *New_Setting_ctrl)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	New_Setting_ctrl->New_Settings_uipara = New_Setting_get_numbers_para(screen_size);
	__wrn("New_Setting_init is ok\n");
	if(!New_Setting_ctrl->New_Setting_bmp)
	{
		New_Setting_ctrl->New_Setting_bmp = theme_open(ID_SETTING_SET_INFO_BMP);
	}

	if(!New_Setting_ctrl->New_Setting_f_bmp)
	{
		New_Setting_ctrl->New_Setting_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);
	}
	__wrn("New_Setting_init CREATE\n");
}
/**********************************************************************************
*�ͷ������ʹ��ͼƬ��Դ
**********************************************************************************/
static void New_Setting_uninit(New_Setting_ctrl_t *New_Setting_ctrl)
{
	__u32 	i;

	{
		__wrn("New_Setting_uninit is 1 ok\n");
		if(New_Setting_ctrl->New_Setting_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_bmp);
			New_Setting_ctrl->New_Setting_bmp = NULL;
		}
		__wrn("New_Setting_uninit is 2 ok\n");
		if(New_Setting_ctrl->New_Setting_f_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_f_bmp);
			New_Setting_ctrl->New_Setting_f_bmp  = NULL;
		}
		__wrn("New_Setting_uninit is 3 ok\n");
	}

	return;
}
/****************************************************************
*���Ƶ���Ӧ��
****************************************************************/
 __s32 New_Setting_paint_one_item(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	GUI_RECT gui_rect;		//���GUI���οؼ�����
	char	 *txt_character;//������ֵ��ַ�����
	void  	*number_theme;	//��Ż�ȡ��ͼƬʹ����Դ����
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//����һ�����ݵ�msg�ṹ��
	GUI_LyrWinSel(New_Setting_ctrl->New_Settings_para->layer);//ѡ���µ�ͼ��
	
	#if 0		//ֱ����ͼ��������ʾ�����ı��ַ���
				GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);	//���öԻ�������
				GUI_UC_SetEncodeUTF8(); 								//���ñ����ʽ
				__wrn("number_menu_numbers_paint_all 3 CREATE\n");
				GUI_SetColor(GUI_BLACK);								//�����ı���ɫ����ɫ
				New_Setting_get_volume_size(msg,num);			//��ȡ�س������������ �ַ���
				txt_character = New_Setting_ctrl->new_number_size;		//��Ҫ��ʾ�������ַ�����ȡ
				__wrn("number_menu_numbers_paint_all 4 CREATE\n");
				GUI_DispStringAt(txt_character,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				{
					__wrn("txt_character = %s\n",txt_character);		//ֱ����ʾ�����ַ������ı����Ƴɹ�
				}
				
				//number_theme = theme_hdl2buf(numbers_ctrl->number_bmp);
				//GUI_BMP_Draw(number_theme, numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//�����ı��ؼ�number:00��ʾ����
				__wrn("New_Setting_paint_one_item CREATE\n");
				__wrn("New_Setting_ctrl->new_number_size = %s\n",New_Setting_ctrl->new_number_size);
				__wrn("number_tsize_pos.x = %d , number_tsize_pos.y = %d\n",New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				//GUI_DispStringAt(numbers_ctrl->number_total_size, numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);			//���ı��ؼ�����ʾ�ַ���
				//GUI_DispStringAt("01",32,28);//�ַ�����ʾ������
				__wrn("txt paint is CREATE\n");//�ı����Ƴɹ�
	#endif
	#if 1		//���ƾ��� Ȼ���ھ��ο���ʾ���������ַ���---> ��ʾ "number:00"
		GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);		//���öԻ�������
		GUI_UC_SetEncodeUTF8();										//���ñ����ʽ
		
		GUI_SetColor(APP_COLOR_YELLOW);									//GUI_WHITE�����ı���ɫ����ɫ
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//���û�ͼģʽΪ����
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//���þ��ο�ؼ��ı�����ɫΪ��ɫ
		New_Setting_get_volume_size(msg,num);						//��ȡ�س������������ �ַ���
		txt_character = New_Setting_ctrl->new_number_size;			//��Ҫ��ʾ�������ַ�����ȡ
		#if 0//�ֶ�ֱ���޸ľ��δ�С
		gui_rect.x0 = 0;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;		//����x 100 /*416*/
		gui_rect.y0 = 0;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;		//����y 80    /*290*/
		gui_rect.x1 = 800;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//���ο�191
		gui_rect.y1 = 480;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height;  //���θ�20
		#else//��ȡ����ͼƬ��Դ�ľ��δ�С
		gui_rect.x0 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;		//����x 0 /*416*/
		gui_rect.y0 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;		//����y 0	/*290*/
		gui_rect.x1 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//���ο�200
		gui_rect.y1 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height; //���θ�100
		#endif
		{
			number_theme = theme_hdl2buf(New_Setting_ctrl->New_Setting_bmp);			//��ȡͼƬʹ����Դ����
			GUI_BMP_Draw(number_theme, New_Setting_ctrl->New_Settings_uipara->numbers_bmp_pos.x, New_Setting_ctrl->New_Settings_uipara->numbers_bmp_pos.y);//�����ı��ؼ�number:00��ʾ��������ֹnumber���ַ�������������
			//GUI_BMP_Draw(number_theme, 0, 0);//�����ı��ؼ�number:00��ʾ��������ֹnumber���ַ�������������
	
			__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			GUI_DispStringInRect(txt_character , &gui_rect , GUI_TA_LEFT | GUI_TA_VCENTER);//�������ַ�����ʾ�����οؼ����棬��ˮƽ����
			
			//GUI_DispStringAt(txt_character, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x + 20);
			__wrn("txt_character = %s \n",txt_character);//��ӡ�ı���ʾ��Ϣ
			__wrn("New_Setting_paint_one_item ok ok \n");
		}
	#endif

	return EPDK_OK;
}
//��������ͼƬ
static __s32 New_Setting_paint_all(__gui_msg_t *msg)
{

	return EPDK_OK;
}
//������������
static __s32  New_Setting_key_proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("New_Setting_key key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("New_Setting_key key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_ENTER, 0, 0);//�򸸴���app_New_Setting.c�ļ���manwin���ڷ���"case GUI_MSG_COMMAND:	//��������"���뵽case NEW_SETTING_ID:����ִ�й���
			//app_new_setting_cmd2parent(msg->h_deswin, NEW_SETTING_ID, ID_OP_NEW_SETTING_ENTER, 0);
		}
		if(GUI_MSG_KEY_RIGHT == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("New_Setting_key key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//Ϊup
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

/*====================================================================================
*�������ã�ͼ�㴰�ڴ����ص�����
*�βΣ�__gui_msg_t *msg��GUI�̶�����Ľṹ��ָ�����
====================================================================================*/
static __s32 _New_Setting_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	__wrn("_New_Setting_proc msg->id=%d is...\n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//��������
		{
			New_Setting_ctrl_t *New_Setting_ctrl;
			new_setting_para_t *New_Settings_para;
			New_Settings_para = (new_setting_para_t *)GUI_WinGetAttr(msg->h_deswin);		//��ȡ���ڵ�����
			New_Setting_ctrl = (New_Setting_ctrl_t *)My_Balloc(sizeof(New_Setting_ctrl_t)); //�ڴ����
			if(!New_Setting_ctrl)
			{
				__err("memory balloc fail........\n");
				return EPDK_FAIL;
			}
			__wrn("New_Setting GUI_MSG_CREATE 1 is...\n");
			eLIBs_memset(New_Setting_ctrl, 0, sizeof(New_Setting_ctrl_t));			//���ṹ��number_menu_numbers_ctrl_t�ڴ��ʼ��
			New_Setting_ctrl->focus_txt_color = APP_COLOR_YELLOW;//GUI_YELLOW;//				//ѡ�е��ı���ɫ
			New_Setting_ctrl->unfocus_txt_color = APP_COLOR_WHITE;//GUI_WHITE;//				//δѡ�е��ı���ɫ
			New_Setting_ctrl->New_Settings_para = New_Settings_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)New_Setting_ctrl);						//����һ������
			__wrn("New_Setting GUI_MSG_CREATE 2 is...\n");
			New_Setting_init(New_Setting_ctrl);												//number:00��ʾ��ʾͼƬ��Դ����ʹ�����ʾ��С
			GUI_LyrWinSetSta(New_Setting_ctrl->New_Settings_para->layer, GUI_LYRWIN_STA_ON);//����ͼ�����ÿ���״̬
			GUI_LyrWinSetTop(New_Setting_ctrl->New_Settings_para->layer);					//����ͼ������ֹͣ״̬
			//number_menu_numbers_paint_all(msg,0);											//��������ͼ��
			__wrn("New_Setting GUI_MSG_CREATE 3 is...\n");
			//New_Setting_paint_one_item(msg,0);				//����ͼ��												//���Ƶ���ͼ��
			__wrn("New_Setting GUI_MSG_CREATE 4 is ok\n");
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//�رմ��ڣ�Ȼ�󷵻���һ�������ڽ���
		{
			#if 0
			New_Setting_ctrl_t *New_Setting_ctrl;
			New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!New_Setting_ctrl)
			{
				__msg("New_Setting_ctrl_t is null \n");
				return EPDK_FAIL ;
			}
			__wrn("New_Setting_ctrl_t GUI_MSG_CLOSE is...  \n");
			
			//app_new_setting_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_SWITCH_TO_GENERAL, 0, 0);//������һ��
			//app_new_setting_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_OP_EXIT, 0, 0);//ֱ���˳�
			//app_new_setting_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, NULL);//ֱ�ӽ���ID_HOME_NEW_SETTING_NUMBER,Ȼ����뵽�����ӵ�ͨ�����ý���
			#endif
			#if 0//ֱ���ͷ�ͼƬ��Դ
			New_Setting_ctrl_t *New_Setting_ctrl;
			New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ����ṹ�������
			New_Setting_uninit(New_Setting_ctrl);//�ͷ�ͼƬ��Դ
			if(New_Setting_ctrl)//�ͷŽṹ���ڴ�
			{
				__msg("New_Setting_ctrl_t win free New_Setting_ctrl_t.....\n");
				My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
				New_Setting_ctrl = NULL ;
			}
			#endif
			#if 1//��app_New_Setting.c������manwin���ڵ�case NEW_SETTING_ID:�����˳��ź�
			__wrn("New_Setting_ctrl_t APP_NEWSETTING_ID is... ok\n");
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_EXIT, 0, 0);
			//app_new_setting_cmd2parent(msg->h_deswin, APP_NEWSETTING_ID, GUI_MSG_CLOSE, 0);//��app_New_Setting.c������manwin���͹ر��ź�
			#endif
			__wrn("New_Setting_ctrl_t GUI_MSG_CLOSE is... ok\n");
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //���ٴ���
		{
			#if 0	//����1���ͷ�ͼƬʹ����Դ
				New_Setting_ctrl_t *New_Setting_ctrl;
				New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ����ṹ�������
				New_Setting_uninit(New_Setting_ctrl);//�ͷ�ͼƬ��Դ
				if(New_Setting_ctrl->New_Settings_para)//�ͷ�ͼ��
				{
					__wrn("New_Setting_ctrl win free New_Settings_para.....\n");
					My_Bfree(New_Setting_ctrl->New_Settings_para, sizeof(new_setting_para_t));
					New_Setting_ctrl->New_Settings_para = NULL ;
				}

				if(New_Setting_ctrl)
				{
					__wrn("New_Setting_ctrl win free New_Setting_ctrl.....\n");
					My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
					New_Setting_ctrl = NULL ;
				}
			#endif
			#if 1	//����2���ͷ�ͼƬ��Դ
				New_Setting_ctrl_t *New_Setting_ctrl;
				New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ����ṹ�������

				if(!New_Setting_ctrl)//�ͷŽṹ���ڴ�
				{
					__err("New_Setting_ctrl is null........\n");
					return EPDK_FAIL;
				}
				

				__wrn("New_Setting_ctrl win destroy...\n");
				theme_close(New_Setting_ctrl->New_Setting_bmp);	//�ͷ�ͼƬ��Դ
				__wrn("New_Setting_ctrl win close theme...\n");
				theme_close(New_Setting_ctrl->New_Setting_f_bmp);//�ͷ�ͼƬ��Դ
				__wrn("New_Setting_ctrl win close theme ok\n");

				if(New_Setting_ctrl->New_Settings_para)//�ͷ�ͼ��
				{
					__wrn("New_Setting_ctrl win free New_Settings_para.....\n");
					My_Bfree(New_Setting_ctrl->New_Settings_para, sizeof(new_setting_para_t));
					New_Setting_ctrl->New_Settings_para = NULL ;
				}

				if(New_Setting_ctrl)
				{
					__wrn("New_Setting_ctrl win free New_Setting_ctrl.....\n");
					My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
					New_Setting_ctrl = NULL ;
				}
			#endif
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//������ʾ����ͼƬ����
			__wrn("one  paint new Setting \n");
			New_Setting_paint_one_item(msg,0);
		return EPDK_OK;

		case GUI_MSG_KEY:	//������������
		__wrn("GUI_MSG_KEY new Setting \n");
			New_Setting_key_proc(msg);
		return EPDK_OK;

		case MSG_NEW_SETTING_OP_ENTER:	//�س�ȷ�ϰ���
		{
			__gui_msg_t mymsg;
			key_number += 1;
			__wrn("new setkey_number start =%d  \n",key_number);
			#if 1	
			if(key_number == 5)//ɾ��appӦ�ã�Ȼ�󷵻���һ��app_New_Setting.c��manwin������ʾȫ��appӦ��
			{
				key_number = 0;
				#if 1
				mymsg.id = GUI_MSG_CLOSE;		//framewin���ڵĹرմ���id
				mymsg.h_deswin = msg->h_deswin;	//�ڵ�ǰͼ�㷢�Ͱ�����Ϣ
				mymsg.h_srcwin = msg->h_srcwin; //�ڵ�ǰͼ�㷢�Ͱ�����Ϣ
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
				__wrn("new setkey_number stop =%d  \n",key_number);
				New_Setting_paint_one_item(msg,key_number);//ˢ��number������ʾ����
			}
			#endif
		}
		return EPDK_OK;
			
		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

/*====================================================================================
*�������ã��µ�appӦ��framewin���ڴ�������
*�βΣ�H_WIN h_parent�����ڶ��� ��__gui_msg_t *msg��GUI�̶�����Ľṹ��ָ�����
====================================================================================*/
H_WIN New_Setting_app_win_create(H_WIN h_parent, new_setting_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;		//����һ�� framewin���ڵĴ��� �ṹ�����
	new_setting_para_t *New_Setting_para;
	FB fb;
	GUI_LyrWinGetFB(para->layer, &fb);				//ͼ�㴰�ڻ�ȡFB
	New_Setting_para = (new_setting_para_t *)My_Balloc(sizeof(new_setting_para_t));	//���ṹ��ָ����������ڴ�ռ�

	if(!New_Setting_para)	//�жϽṹ���Ƿ�����ڴ�ɹ�
	{
		__err("memory balloc fail........\n");
		return EPDK_FAIL;
	}

	New_Setting_para->font = para->font;						//��ȡ��Ϣ�Ի����ı�
	New_Setting_para->layer = para->layer;									//ͼ��
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));	//��ʼ���ṹ�壬��0
	framewin_para.name =	"New_Setting_win";								//��������
	framewin_para.dwExStyle = WS_EX_NONE;									//������չ���
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;											//framewin��ӵ���߾��
	framewin_para.hHosting = h_parent;										//framewin�������ھ��
	framewin_para.id         = NEW_SETTING_ID;								//framewin��ID
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_New_Setting_proc);	//framewin����Ϣ�������̣��ص���������
	framewin_para.rect.x = 0;												//framewin��ͼ�㴰���е�λ��x
	framewin_para.rect.y = 0;												//framewin��ͼ�㴰���е�λ��y
	framewin_para.rect.width =  fb.size.width;								//framewin��ͼ�㴰���е�λ�ÿ�1024
	framewin_para.rect.height =  fb.size.height;							//framewin��ͼ�㴰���е�λ�ø�600
	framewin_para.BkColor.alpha =  0;										//framewin�ı���ɫ 32-bit (ARGB) color  
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)New_Setting_para;							//framewin�Ĵ���˽��������Ϣ
	framewin_para.hLayer = para->layer;										//framewin��ͼ�㴰��
	return (GUI_FrmWinCreate(&framewin_para));								//����һ��framewin��������
}
/*====================================================================================
*�������ã�����ɾ��
====================================================================================*/
void  new_setting_app_win_delete(H_WIN New_Setting_win)
{
	GUI_FrmWinDelete(New_Setting_win);
}
