#include	"app_new_fm.h"




//////////////////////////////////////////////////////////////////////////
// ����ͼ��
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)




/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: ����fm��framewin���ڴ�������
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
	fm_ctrl = (new_fm_ctrl *)GUI_WinGetAttr(msg->h_deswin);
	//////////fm��framewinͼ���С///////////
	fm_rect.x	= 0;
	fm_rect.y	= 0;
	fm_rect.width	= 600;
	fm_rect.height	= 1024;
	fm_layer_create(fm_ctrl->fm_lyr, fm_rect, "new fm lyr"); 
	if(!fm_ctrl->fm_lyr) {
		__wrn("fm_lyr is null...\n");
		return EPDK_FALSE;
	} else {
		__wrn("fm_lyr is create success...\n");
	}

	fm_ctrl->fm_framewin = ;

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
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	//1������
			__wrn("new fm is start  destroy\n");
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{	//2������

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{		//4���˳�

		}
		return EPDK_OK;
		
		case GUI_MSG_KEY:{		//5������

		}
		break;
		
		case GUI_MSG_COMMAND:{	//7������                                   

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

	ctrl_para = (new_fm_ctrl *)My_Balloc( sizeof(new_fm_ctrl));
	eLIBs_memset(ctrl_para, 0, sizeof(new_fm_ctrl));

	ctrl_para->font			=	para->font;
	ctrl_para->root_type	=	para->root_type;//TF��
	/****************************��ʼ����manwin����*******************/
	eLIBs_memset(create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name	=	APP_NEWFM;
	create_info.hParent	=	para->h_parent;
	create_info.id		=	APP_NEWFM_ID;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t app_new_fm_proc) );;
	create_info.attr	=	(void *)ctrl_para;

	return (GUI_ManWinCreate(&create_info));
}









