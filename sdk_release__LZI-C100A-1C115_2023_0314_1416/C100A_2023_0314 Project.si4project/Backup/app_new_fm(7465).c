#include	"app_new_fm.h"
#include	"new_fm.h"




//////////////////////////////////////////////////////////////////////////
// ����ͼ��
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)

__s32 g_is_searching = 0;


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

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
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
static __s32 __new_fm_auto_search_start()
{
	__u32 search_flag;
	g_is_searching = 1;
	dsk_radio_get_search_flag(&search_flag);//��ȡ������־

	if(1 == search_flag)	//�������������в������жϣ���������������ܽ���
	{
		__wrn("fm_auto_search_start:return\n");
		return EPDK_OK;
	}
	else
	{
		dsk_radio_set_a
	}
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm�ĳ�ʼ������
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_init_date()
{

	if(dsk_radio_is_open())
	{
		dsk_radio_chan_t chan_info;
		reg_fm_para_t	*para;
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);//��ȡע���ָʾ��

		if(1)
		{

		}
		else
		{

		}
	}
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm�ĳ�ʼ������
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_init_module()
{
	if(!dsk_radio_is_open())//������radioģ��û�д�
	{
		reg_fm_para_t	*para;
		dsk_radio_open();									//��radioģ��	
		dsk_radio_rcv_open();								//���������ն�ģ��---��
		dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);			//����������Ƶ�� Ϊ����Ƶ��

		dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);	//�����������ź�ǿ������ֵ ������ֵ Ϊ�е�
		dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);	//��������������Ƶģʽ Ϊ������
		dsk_radio_rcv_get_search_result(para);				//���б��ļ����ȡ��������Ƶ���б�
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);//����---�����б�����
	}


	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm��framewin���ڳ�����������
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32  app_new_fm_scene_create(__gui_msg_t *msg)
{
	new_fm_ctrl		*fm_ctrl;
	RECT	fm_rect;
	fm_create_para_t	fm_create;
	fm_ctrl = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
	eLIBs_memset(&fm_create, 0, sizeof(fm_create_para_t));
	//////////fm��framewinͼ���С///////////
	fm_rect.x	= 0;
	fm_rect.y	= 0;
	fm_rect.width	= 600;
	fm_rect.height	= 1024;
	fm_layer_create(fm_ctrl->fm_lyr, &fm_rect, "new_fm_lyr"); 
	if(!fm_ctrl->fm_lyr) {
		__wrn("fm_lyr is null...\n");
		return EPDK_FALSE;
	} else {
		__wrn("fm_lyr is create success...\n");
	}
	fm_create.fm_frmlyr = fm_ctrl->fm_lyr;
	fm_create.fm_frmwin = msg->h_deswin;//��ȡ
	fm_ctrl->fm_framewin = new_fm_frmwin_create(msg->h_deswin,  &fm_create);//framewin���ڴ�������

	GUI_WinSetFocusChild(fm_ctrl->fm_framewin);//���Ͱ�����������Ϣ��framewin�Ӵ���
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

			__wrn("new fm is start create\n");
			app_new_fm_scene_create(msg);//
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	//1������
			__wrn("new fm is start  destroy\n");
			app_new_fm_manwin_delete(msg);
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
						case MSG_NEW_FM_OP_EXIT:{
							__gui_msg_t mmsg;
							mmsg.h_deswin = msg->h_deswin;
							mmsg.id		= GUI_MSG_QUIT;
							GUI_SendMessage(&mmsg);
						}
						break;
					}
				}
			}
		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{	//19����ʱ��

		}
		return EPDK_OK;
		
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






