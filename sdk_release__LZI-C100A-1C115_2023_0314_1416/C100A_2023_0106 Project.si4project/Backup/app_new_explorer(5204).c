#include "app_new_explorer.h"
#include  "new_explorer_list.h"
#include "string.h"
#include "typedef.h"
#include "app_new_explorer_i.h"
#include  "new_explorer_list_ui.h"



/*
************************************************************************************************************************
*Function	:          static H_LYR new_explorer_32bpp_layer_create(void)
*
*Description	: 		前景图层，主要用来显示前景图片，ListBar组件item, 缩略图
*						注意与背景图应处于不同的pipe
*Return     	: 	     layer图层句柄
*
************************************************************************************************************************
*/
static H_LYR new_explorer_32bpp_layer_create(void)
{
	H_LYR layer = NULL;
	RECT LayerRect;
	//	int ret;
	//__layerwincreate_para_t lyrcreate_info;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		//{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */

	};
	__disp_layer_para_t lstlyr =
	{
		//DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
		DISP_LAYER_WORK_MODE_NORMAL,
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
		"APP_NEW_EXPLORER",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	//explorer_get_screen_rect(&LayerRect);
	new_explorer_get_screen_rect(&LayerRect);//设置framewin窗口图层矩形大小和显示位置
	fb.size.width		= LayerRect.width;
	fb.size.height		= LayerRect.height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= LayerRect.width;
	lstlyr.src_win.height 	= LayerRect.height;
	lstlyr.scn_win.x		= LayerRect.x;
	lstlyr.scn_win.y		= LayerRect.y;
	__wrn("lstlyr.scn_win.x=%d\n", lstlyr.scn_win.x);
	__wrn("lstlyr.scn_win.y=%d\n", lstlyr.scn_win.y);
	lstlyr.scn_win.width  	= LayerRect.width;
	lstlyr.scn_win.height 	= LayerRect.height;
	__wrn("lstlyr.scn_win.width=%d\n", lstlyr.scn_win.width);
	__wrn("lstlyr.scn_win.height=%d\n", lstlyr.scn_win.height);
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

/*======================================================================
*Description：new explorer manager window to framewin
*			  manwin窗口调用创建列表framewin窗口函数 ListBar
*				创建浏览器场景
*return		：GUI_ManWinDefaultProc(msg)：返回一个manwin窗口回调信号
======================================================================*/
static __s32 new_explorer_scene_create(__gui_msg_t *msg)
{
	new_explorer_ctrl_t *new_explorer_ctrl;			  //存放manwin窗口下的结构体
	new_explorer_list_para_t	new_explorer_ListPara;//存放framewin窗口创建需要的结构体
	eLIBs_memset(&new_explorer_ListPara, 0, sizeof(new_explorer_list_para_t));	//结构体数据初始化，数据清0
	new_explorer_ctrl = (new_explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);	//获取属性
	
	/*RETC LayerRect:矩形图层*/
	__wrn("new_explorer_ctrl->root_type = %d \n",new_explorer_ctrl->root_type);	//等于2为TF卡
	new_explorer_ListPara.root_type 	= new_explorer_ctrl->root_type;		//检测USB设备或者是TF卡设备
	new_explorer_ListPara.media_type    = new_explorer_ctrl->media_type;	//获取磁盘的文件类型：比如音乐文件，视频文件，电子书文件等类型
	new_explorer_ListPara.view_mode1	= new_explorer_ctrl->view_mode;		//使用liest列表显示模式还是正方形显示模式
	new_explorer_ListPara.root_para		= new_explorer_ctrl->root_para;		//获取root根目录传入进来的参数

	__wrn("----app new explorer manager window new_list_layer creat begin is 1----\n");
	new_explorer_ctrl->new_list_layer = new_explorer_32bpp_layer_create();		//列表框使用32位bpp 图层创建
	__wrn("----app new explorer manager window new_list_layer creat begin is 2----\n");
	
	new_explorer_ListPara.list_layer  =	new_explorer_ctrl->new_list_layer;		//创建的图层重新赋值给结构体new_explorer_ListPara的list_layer图层变量
	__wrn("new_explorer_ListPara.list_layer = %d\n",new_explorer_ListPara.list_layer);
	if(!new_explorer_ListPara.list_layer)											//检测是否创建图层成功
	{
		__wrn("new_explorer_ListPara.list_layer is null \n");
		return EPDK_FAIL ;
	}
	new_explorer_ListPara.list_font	  = new_explorer_ctrl->new_explorer_font;	//manwin窗口文本赋值给framewin窗口结构体文本变量
	new_explorer_ctrl->h_new_explore  = new_explorer_list_framewin_creat(msg->h_deswin,&new_explorer_ListPara);//创建framewin子窗口
	__wrn("----app new explorer manager window new_list_layer creat begin is 3----\n");

	//下面必须添加
	GUI_WinSetFocusChild(new_explorer_ctrl->h_new_explore);//选择framewin子窗口并向其发送按键消息信号
	//GUI_LyrWinSetSta(new_explorer_ctrl->new_list_layer, GUI_LYRWIN_STA_ON);//manwin窗口当前图层开启
	//GUI_LyrWinSetTop(new_explorer_ctrl->new_list_layer);				   //manwin窗口当前图层停止
	return EPDK_OK;
}

/*======================================================================
*Description：new explorer manager window proc
*			  新的资源管理器manwin窗口回调处理函数
*return		：GUI_ManWinDefaultProc(msg)：返回一个manwin窗口回调信号
======================================================================*/
static __s32 _new_explorer_manager_win_proc(__gui_msg_t *msg)
{
	__wrn("_app_New_Setting_proc msg->id = %d \n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//窗口创建功能
		{
			__wrn("----app new explorer manager window GUI_MSG_CREATE begin is----\n");
			new_explorer_scene_create(msg);
			__wrn("----app new explorer manager window GUI_MSG_CREATE begin ok----\n");
		}
		return EPDK_OK;//只需要创建一次的直接退出整个回调函数

		case GUI_MSG_KEY:		//manwin窗口按键功能
		{

		}
		break;//按键功能需要往下面子窗口framewin窗口发送按键消息信号，所有使用break只退出当前switch语句

		default://否则退出当前switch语句
			break;
	}
	return GUI_ManWinDefaultProc(msg);
}

/*======================================================================
*Description：new explorer manager window create
*			  新的资源管理器manwin窗口创建
*return		：GUI_ManWinCreate(&create_info)：返回一个创建好的manwin窗口
======================================================================*/
H_WIN app_new_explorer_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	new_explorer_ctrl_t 	*new_explorer_ctrl;
	__wrn("****************************************************************************************\n");
	__wrn("********  enter new explorer app  **************\n");
	__wrn("****************************************************************************************\n");

	new_explorer_ctrl = (new_explorer_ctrl_t *)esMEMS_Balloc(sizeof(new_explorer_ctrl_t));//为私有属性结构体申请内存空间
	eLIBs_memset(new_explorer_ctrl, 0, sizeof(new_explorer_ctrl_t));					 //结构体内存初始化,数据清0
	
	new_explorer_ctrl->root_para = para;
	new_explorer_ctrl->new_explorer_font = para->font;		//文本
	__wrn("para->root_type = %d\n", para->root_type);		//等于2为TF卡
	switch(para->data)
	{
		__wrn("ID_NEW_EXPLORER_MOVIE para->data =%d\n", para->data);
		case ID_NEW_EXPLORER_MOVIE://1时，执行搜索磁盘的视频文件功能
		{
			new_explorer_ctrl->view_mode  = EXPLR_LIST_MODE;
			new_explorer_ctrl->media_type = RAT_MEDIA_TYPE_VIDEO;//获取磁盘的视频文件
			new_explorer_ctrl->root_type  = para->root_type;	 //检测是USB还是SD卡磁盘
		}
		break;

		default://否则直接退出当前switch语句
			break;
	}
	/******************开始操作manwin窗口的配置**********************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));//结构体内存初始化,数据清0
	create_info.name			=	"app_new_explorer";			/*manwin窗口的名称*/
	create_info.hParent			=	para->h_parent;				/*manwin窗口的句柄*/
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_explorer_manager_win_proc);/*manwin主窗口的消息回调函数处理过程*/
	create_info.attr			=	(void *)new_explorer_ctrl;	/*manwin主窗口的私有属性*/
	create_info.id				=	APP_NEWEXPLORER_ID;			/*manwin窗口的id*/
	create_info.hHosting		=	NULL;						/*manwin窗口的拥有者*/
	return(GUI_ManWinCreate(&create_info));						/*最后返回一个创建好的manwin窗口*/
}


