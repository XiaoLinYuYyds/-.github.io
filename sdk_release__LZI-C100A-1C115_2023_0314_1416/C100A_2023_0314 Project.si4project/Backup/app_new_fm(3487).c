#include	"app_new_fm.h"




//////////////////////////////////////////////////////////////////////////
// 建立图层
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)




/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的framewin窗口创建函数
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
	//////////fm的framewin图层大小///////////
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

	GUI_WinSetFocusChild(fm_ctrl->fm_framewin);//发送按键等命令信息到framewin子窗口
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_proc
*
*Description: fm的manwin窗口创建回调处理函数
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
		case GUI_MSG_CREATE:{	//0，创建

			__wrn("new fm is start create\n");
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	//1，销毁
			__wrn("new fm is start  destroy\n");
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{	//2，绘制

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{		//4，退出

		}
		return EPDK_OK;
		
		case GUI_MSG_KEY:{		//5，按键

		}
		break;
		
		case GUI_MSG_COMMAND:{	//7，命令                                   

		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{	//19，定时器

		}
		return EPDK_OK;
		
	}

	return GUI_ManWinDefaultProc(msg);
}





/*
************************************************************************************************************************
*                       				app_new_fm_manwin_create
*
*Description: fm的manwin窗口创建
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
	ctrl_para->root_type	=	para->root_type;//TF卡
	/****************************开始配置manwin窗口*******************/
	eLIBs_memset(create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name	=	APP_NEWFM;
	create_info.hParent	=	para->h_parent;
	create_info.id		=	APP_NEWFM_ID;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t app_new_fm_proc) );;
	create_info.attr	=	(void *)ctrl_para;

	return (GUI_ManWinCreate(&create_info));
}









