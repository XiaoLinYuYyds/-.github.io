#include	"app_new_fm.h"
#include	"new_fm.h"




//////////////////////////////////////////////////////////////////////////
// 建立图层
#define fm_layer_create(_lyr, _prt, _name)   app_com_layer_create_default(_lyr, _prt, _name)

__s32 g_is_searching = 0;


//窗口的命令发送函数
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
	msg.h_deswin = hparent;//父窗口
	msg.h_srcwin = hwin;//源窗口
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data2;
	msg.dwReserved = reserved;//存放USB或TF卡切换
	GUI_SendNotifyMessage(&msg);
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的自动搜索函数
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
	dsk_radio_get_search_flag(&search_flag);//获取搜索标志

	if(1 == search_flag)	//正在搜索过程中不允许中断，必须得搜索完后才能进行
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
*Description: 用于fm的初始化函数
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
		get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);//获取注册表指示器

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
*Description: 用于fm的初始化函数
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
	if(!dsk_radio_is_open())//如果检测radio模块没有打开
	{
		reg_fm_para_t	*para;
		dsk_radio_open();									//打开radio模块	
		dsk_radio_rcv_open();								//收音机接收端模块---打开
		dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);			//设置收音机频段 为美国频段

		dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);	//设置收音机信号强度门限值 启点阈值 为中等
		dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);	//设置收音机的音频模式 为立体声
		dsk_radio_rcv_get_search_result(para);				//从列表文件里获取搜索到的频道列表
		dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);//设置---播放列表类型
	}


	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				app_new_fm_scene_create
*
*Description: 用于fm的framewin窗口场景创建函数
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
	//////////fm的framewin图层大小///////////
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
	fm_create.fm_frmwin = msg->h_deswin;//获取
	fm_ctrl->fm_framewin = new_fm_frmwin_create(msg->h_deswin,  &fm_create);//framewin窗口创建函数

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
			app_new_fm_scene_create(msg);//
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	//1，销毁
			__wrn("new fm is start  destroy\n");
			app_new_fm_manwin_delete(msg);
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{	//2，绘制

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{		//4，退出
			__wrn("quit app_new_fm manwin...\n");
			app_new_fm_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);			
		}
		return EPDK_OK;
		
		case GUI_MSG_KEY:{		//5，按键

		}
		break;
		
		case GUI_MSG_COMMAND:{	//7，命令                                   
			switch(LOWORD(msg->dwAddData1))
			{
				case NEW_FM_ID:	//framewin窗口ID，意思是在主菜单时候进入执行下面的程序
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
	#if 0 //检查内存是否泄漏
		esKSRV_SysInfo();								//查看系统内存消息是否泄露或溢出
	#endif
	ctrl_para = (new_fm_ctrl *)My_Balloc( sizeof(new_fm_ctrl));
	eLIBs_memset(ctrl_para, 0, sizeof(new_fm_ctrl));

	ctrl_para->font			=	para->font;
	ctrl_para->root_type	=	para->root_type;//TF卡
	/****************************开始配置manwin窗口*******************/
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
*Description: fm的manwin窗口删除
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

	if(this->fm_framewin){//先删除子窗口
		GUI_FrmWinDelete(this->fm_framewin);
		__wrn("fm_framewin is delete success...\n");
		this->fm_framewin = 0;
	}

	if(this->fm_lyr){//接着删除子窗口图层
		GUI_LyrWinDelete(this->fm_lyr);
		this->fm_lyr = 0;
		__wrn("fm_lyr is delete success...\n");
	}

	if(this){//最后清除本主窗口数据
		eLIBs_memset(this, 0, sizeof(new_fm_ctrl));
		My_Bfree(this, sizeof(new_fm_ctrl));
	}

}






