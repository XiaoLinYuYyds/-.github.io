#include  "new_explorer_list.h"


























/*=============================================================================================
*Description：new explorer list ui paint
*			  app应用 列表 UI 绘制 单个内容
*return		：return EPDK_OK;返回一个OK
=============================================================================================*/

static __s32 _new_explorer_list_ui_one_paint(__gui_msg_t *msg)
{


	return EPDK_OK;
}



/*=============================================================================================
*Description：new explorer list frame window proc
*			  新的资源管理器app应用 列表 framewin子窗口创建 回调处理函数
*return		：GUI_FrmWinCreate(&framewin_para)：返回一个创建好的framewin窗口
=============================================================================================*/

static __s32 _new_explorer_list_win_proc(__gui_msg_t *msg)
{
	__wrn("_new_explorer_list_win_proc msg->id=%d is...\n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE://0，创建功能
		{
			
		}
			return EPDK_OK;

		case GUI_MSG_PAINT:	//2, 绘制功能

			return EPDK_OK;
		
		case GUI_MSG_KEY:	//5，按键功能
		{

		}
			return EPDK_OK;

		default://否则直接退出switch语句
			break;
	}


	return GUI_FrmWinDefaultProc(msg);
}



/*=============================================================================================
*Description：new explorer list frame window create
*			  新的资源管理器app应用 列表 framewin子窗口创建
*return		：GUI_FrmWinCreate(&framewin_para)：返回一个创建好的framewin窗口
=============================================================================================*/

H_WIN	new_explorer_list_framewin_creat(H_WIN h_parent,new_explorer_list_para_t *para)
{
	__gui_framewincreate_para_t		framewin_para;		//framewin窗口结构体
	new_explorer_list_para_t	*new_explorer_list_para;//存放framewin窗口创建需要的结构体
	FB fb;
	GUI_LyrWinGetFB(para->list_layer, &fb);//获取FB功能块，获取列表图层大小
	new_explorer_list_para = (new_explorer_list_para_t *)esMEMS_Balloc(sizeof(new_explorer_list_para_t));	//分配内存空间到结构体
	eLIBs_memset(new_explorer_list_para, 0, sizeof(new_explorer_list_para_t));								//结构体数据初始化，数据清0
	new_explorer_list_para->list_font  = para->list_font;	//framewin窗口文本存放
	new_explorer_list_para->media_type = para->media_type;	//获取磁盘文件的文件类型
	new_explorer_list_para->view_mode  = para->view_mode;	//获取显示的模式，是list列表模式还是square正方形模式
	new_explorer_list_para->root_para  = para->root_para;	//获取root 根目录传进来的参数
	new_explorer_list_para->root_type  = para->root_type;	//存放USB设备或TF卡设备检测结果

	/******************开始操作framewin窗口的配置**********************/
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));									//framewin结构体数据初始化，数据清0
	framewin_para.name			=	"New Explorer List Window";
	framewin_para.dwExStyle		=	WS_EX_NONE;
	framewin_para.dwStyle		=	WS_NONE | WS_VISIBLE;
	framewin_para.spCaption		=	NULL;
	framewin_para.hOwner		=	NULL;
	framewin_para.id			=	NEW_NEWEXPLORER_LIST_ID;
	framewin_para.hHosting		=	h_parent;
	framewin_para.FrameWinProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_explorer_list_win_proc);
	framewin_para.rect.x		=	0;
	framewin_para.rect.y		=	0;
	framewin_para.rect.width	=	fb.size.width;
	framewin_para.rect.height	=	fb.size.height;
	framewin_para.BkColor.alpha	=	0;
	framewin_para.BkColor.blue	=	0;
	framewin_para.BkColor.green	=	255;
	framewin_para.BkColor.red	=	0;
	framewin_para.attr			=	(void *)new_explorer_list_para;
	framewin_para.hLayer		=	new_explorer_list_para->list_layer;		//图层赋值给framewin窗口图层
	
	return(GUI_FrmWinCreate(&framewin_para));//返回一个创建好的framewin窗口
}





