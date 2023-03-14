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

/**********************************************************************************
*函数作用：向APP_NEWSETTING_ID新添加的app应用程序manwin父窗口发送按键通知消息
***********************************************************************************/
void  app_new_explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = GUI_WinGetParent(hwin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	__wrn("**********app_new_explorer_cmd2parent**********\n");
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);//GUI向manwin父窗口发送管理通知
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
	#if 0	//通过获取lcd显示屏大小，用于绘制图层大小1024*600
	RECT rect;
	SIZE screen_size;
	rect.x = 0;
	rect.y = 0;
	dsk_display_get_size(&screen_size.width, &screen_size.height);//获取LCD显示屏大小1024*600
	rect.width = screen_size.width; 	//framewin窗口宽1024
	rect.height = screen_size.height;	//framewin窗口高600
	__wrn("rect.x = %d , rect.y = %d,rect.width = %d , rect.heigh = %d\n",rect.x,rect.y,rect.width,rect.height);
	#endif
	
	eLIBs_memset(&new_explorer_ListPara, 0, sizeof(new_explorer_list_para_t));	//结构体数据初始化，数据清0
	new_explorer_ctrl = (new_explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);	//获取属性
	/*RETC LayerRect:矩形图层*/
	__wrn("new_explorer_ctrl->root_type = %d \n",new_explorer_ctrl->root_type);	//等于2为TF卡
	new_explorer_ListPara.root_type 	= new_explorer_ctrl->root_type;		//检测USB设备或者是TF卡设备
	new_explorer_ListPara.media_type    = new_explorer_ctrl->media_type;	//获取磁盘的文件类型：比如音乐文件，视频文件，电子书文件等类型
	new_explorer_ListPara.view_mode1	= new_explorer_ctrl->view_mode;		//使用liest列表显示模式还是正方形显示模式
	new_explorer_ListPara.root_para		= new_explorer_ctrl->root_para;		//获取root根目录传入进来的参数

	__wrn("----app new explorer manager window new_list_layer creat begin is 1----\n");
	#if 1	//自定义设置图层大小，绘制图层大小640*480
	new_explorer_ctrl->new_list_layer = new_explorer_32bpp_layer_create();		//列表框使用32位bpp 图层创建 大小为640*480
	#else	//通过获取lcd显示屏大小，绘制图层大小1024*600
	new_explorer_ctrl->new_list_layer = New_explorer_list_32bpp_layer_create(&rect,1);//列表框使用32位bpp 图层创建 大小为1024*600
	#endif
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
	__wrn("_new_explorer_manager_win_proc msg->id = %d \n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//窗口创建功能
		{
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			__wrn("----app new explorer manager window GUI_MSG_CREATE begin is----\n");
			new_explorer_scene_create(msg);
			__wrn("----app new explorer manager window GUI_MSG_CREATE begin ok----\n");
		}
		return EPDK_OK;//只需要创建一次的直接退出整个回调函数
		case GUI_MSG_DESTROY: //回车按下进入 销毁窗口
		{
			
			#if 1//方法2 删除图层
			new_explorer_ctrl_t *new_explorer_ctrl;
			__wrn("_new_explorer_manager_win_proc GUI_MSG_DESTROY\n");
			new_explorer_ctrl = (new_explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			if(new_explorer_ctrl->new_list_layer)//新添加的通用设置图层，清空
			{	
				__here__ ;
				__wrn(" new_list_layer isn't null, delete it... \n");
				GUI_LyrWinDelete(new_explorer_ctrl->new_list_layer);//删除新添加的app应用图层
				//GUI_LyrWinDelete(h_lyr);
				__wrn(" new_list_layer isn't null, delete it OK\n");
				__here__ ;
				new_explorer_ctrl->new_list_layer = NULL ;//赋值为空
			}
				__wrn("new_explorer_ctrl destory begin is ok\n");
			#endif
			#if 0//方法3 删除图层
				if(app_new_setting_ctrl->h_frm_general_setting)
				{	
					__wrn(" h_frm_general_setting isn't null, delete it OK\n");
					app_dialog_destroy(app_new_setting_ctrl->h_frm_general_setting);//摧毁图层
					__wrn(" h_frm_general_setting isn't null, delete it OK\n");
					app_new_setting_ctrl->h_frm_general_setting = NULL;
				}
			#endif
			My_Bfree(new_explorer_ctrl, sizeof(new_explorer_ctrl_t));//结构体数组释放内存空间
			
		}
		return EPDK_OK;
		case GUI_MSG_QUIT:
		{
			__wrn("ID_OP_NEW_EXPLORER_EXIT  GUI_MSG_QUIT is....\n");
			app_new_setting_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);//发送到app_root_scene.c 的case APP_NEWSETTING_ID:条件下执行跳转切换到app_setting.c 的创建app应用manwin窗口界面
		}
		return EPDK_OK;
		case GUI_MSG_KEY:		//manwin窗口按键功能
		{

		}
		break;//按键功能需要往下面子窗口framewin窗口发送按键消息信号，所有使用break只退出当前switch语句
		case GUI_MSG_COMMAND:	//接收到framewin窗口发送的命令
		{
			new_explorer_ctrl_t *new_explorer_ctrl;
			__wrn("_new_explorer_manager_win_proc GUI_MSG_COMMAND\n");
			new_explorer_ctrl = (new_explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			
			__wrn("app_new_explorer.c GUI_MSG_COMMAND  is....\n");
			__wrn("app_new_explorer.c LOWORD(msg->dwAddData1) = 0x%x  is....\n",LOWORD(msg->dwAddData1));
			switch(LOWORD(msg->dwAddData1))
			{
				#if 1//接收到来自new_explorer_list.c的framewin窗口命令
				__wrn("NEW_NEWEXPLORER_LIST_ID  is....\n");
				case NEW_NEWEXPLORER_LIST_ID://0x9700
				{
					__wrn("NEW_SETTING_ID  is ok\n");
					__wrn("HIWORD(msg->dwAddData1) = %d is....\n",HIWORD(msg->dwAddData1));
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_NEW_EXPLORER_EXIT://发送关闭命令
						{
							
							#if 1//发送按键信号到当前的APP_NEWSETTING_ID父窗口manwin窗口的写法
							__gui_msg_t my_msg;
							__wrn("new explorer ID_NEW_SETTING_SWITCH_TO_MAIN GUI_MSG_KEY_ESCAPE is...\n");
							my_msg.id = GUI_MSG_QUIT;			//当前文件manwin窗口的退出窗口id
							my_msg.h_srcwin = msg->h_srcwin;	//app_new_explorer.c父窗口manwin窗口发送退出信号
							my_msg.h_deswin = msg->h_deswin;	//app_new_explorer.c父窗口manwin窗口发送退出信号
							my_msg.dwAddData1 = 0;
							my_msg.dwAddData2 = 0;
							GUI_SendNotifyMessage(&my_msg);		//发送通知信息到当前manwin窗口
							return EPDK_OK;						//直接退出这个case GUI_MSG_COMMAND:语句
							#endif
							__wrn("ID_OP_NEW_EXPLORER_EXIT  is....\n");
						}
						break;

						case ID_OP_NEW_EXPLORER_RIGHT://右移
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = new_explorer_ctrl->new_list_layer;//向new_explorer_list.c的framewin窗口图层发送按键信号
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NEW_EXPLORER_OP_RIGHT;
							__wrn("MSG_NUMBER_MENU_OP_UP NEW \n");
							GUI_SendMessage(&mymsg);//向new_list_layer图层发送消息命令
						}
						break;

						case ID_OP_NEW_EXPLORER_LEFT://左移
						{

						}
						break;
					}
				}
				#endif
			}
		}
		return EPDK_OK;
		
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
			new_explorer_ctrl->view_mode  = NEW_EXPLR_LIST_MODE;//EXPLR_LIST_MODE;
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


