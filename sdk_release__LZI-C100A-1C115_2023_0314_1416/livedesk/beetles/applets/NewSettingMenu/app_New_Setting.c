#include "app_New_Setting.h"
#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"
//#include "number_menu_i.h"
#include "New_Setting_i.h"



/***********************************************************************************************************
	建立图层
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
	lstlyr.scn_win.y		= rect->y; //240为屏幕高度，60为底端高度
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
*函数作用：向APP_NEWSETTING_ID新添加的app应用程序manwin父窗口发送按键通知消息
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
	GUI_SendNotifyMessage(&msg);//GUI向manwin父窗口发送管理通知
}

/**********************************************************************************
*函数作用：新添加app应用manwin窗口的按键处理函数，这个在有framewin窗口时可以不用
***********************************************************************************/
static __s32  _app_New_Setting_key_proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("app_New_Setting_key key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("app_New_Setting_key key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)//回车
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_ENTER, 0, 0);
			//app_new_setting_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP,SETTING_SW_TO_MAIN, 0);//回车按下 发送信号到app_root_scene.c 里面的case APP_NEWSETTING_ID:执行直接返回到home应用界面功能
		}
		if(GUI_MSG_KEY_RIGHT == last_key)//下
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)//下
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("app_New_Setting_key key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//4为up
			case GUI_MSG_KEY_LONGLEFT:
			{
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);//命令控制
			}
			break;

			case GUI_MSG_KEY_RIGHT: 	//为down
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
*函数作用：新添加的app应用framewin窗口调用创建函数
形参：
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
	dsk_display_get_size(&screen_size.width, &screen_size.height);//获取LCD显示屏大小1024*600
	rect.width = screen_size.width;		//framewin窗口宽1024
	rect.height = screen_size.height;	//framewin窗口高600
	__wrn("rect.x = %d , rect.y = %d,rect.width = %d , rect.heigh = %d\n",rect.x,rect.y,rect.width,rect.height);
	app_new_setting_ctrl->lry_new_frm_setting = app_New_Setting_8bpp_layer_create(&rect,1);//进入图层创建
	{//创建绘制新添加的通用设置app应用程序framewin窗口
		__wrn("root_type = 0 \n");
		__wrn("app_new_setting_win_create is... \n");
		general_para.layer = app_new_setting_ctrl->lry_new_frm_setting;//图层赋予
		if(!general_para.layer)											//判断图层是否创建图层成功
		{
			__wrn("general_para.layer is null \n");
			return EPDK_FAIL ;
		}
		general_para.font = app_new_setting_ctrl->setting_font;
		general_para.focus_id = 0;
		app_new_setting_ctrl->h_frm_general_setting = New_Setting_app_win_create(msg->h_deswin, &general_para);//进入新添加的app应用程序创建framwin子窗口
		GUI_WinSetFocusChild(app_new_setting_ctrl->h_frm_general_setting);//选择指定窗口进行消息命令发送，按键执行需要调用
	}
	__wrn("app_new_setting_win_create  is ok\n");
	return EPDK_OK;
}

/*====================================================================================
*函数作用：manwin图层窗口回调函数
*形参：__gui_msg_t *msg：GUI固定必须的结构体指针变量
====================================================================================*/
static __s32 _app_New_Setting_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	__wrn("_app_New_Setting_proc msg->id = %d \n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//创建窗口
		{
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			//com_set_palette_by_id(ID_SETTING_WKM_BMP);
			app_new_setting_win_create(msg);	//创建app的framewin窗口
			__wrn("_app_New_Setting_proc GUI_MSG_CREATE  is ok\n");
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//关闭窗口，然后返回上一层主窗口界面
		{
			__wrn("app new Setting GUI_MSG_CLOSE...\n");
			app_new_setting_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);//发送到app_root_scene.c 的case APP_NEWSETTING_ID:条件下执行返回到home应用界面
		}

		return EPDK_OK;
		
		case GUI_MSG_QUIT:	//退出当前的app应用程序manwin窗口，然后跳转到通用设置的app_setting.c文件的app应用程序manwin窗口界面
		{
			__wrn("app new Setting GUI_MSG_QUIT...\n");
			app_new_setting_cmd2parent(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_SETTING, 0);//发送到app_root_scene.c 的case APP_NEWSETTING_ID:条件下执行跳转切换到app_setting.c 的创建app应用manwin窗口界面
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //回车按下进入 销毁窗口
		{
			app_new_setting_ctrl_t	*app_new_setting_ctrl;
			__wrn("app_new_setting_ctrl destory begin ............\n");
			app_new_setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(!app_new_setting_ctrl)
			{
				__err("app_new_setting_ctrl is null ......\n");
				return EPDK_FAIL;
			}
			#if 0//方法1 删除图层
			if(app_new_setting_ctrl->h_frm_general_setting)//新添加的通用设置图层，清空
			{	
				H_LYR h_lyr ;//存放图层变量
				h_lyr = GUI_WinGetLyrWin(app_new_setting_ctrl->h_frm_general_setting);//获取窗口的图层
				if(!h_lyr)
				{
					__wrn("h_lyr is null \n");
					return EPDK_FAIL ;
				}
				__wrn(" lry_new_frm_setting isn't null, delete it \n");
				//GUI_LyrWinDelete(app_new_setting_ctrl->lry_new_frm_setting);//删除新添加的通用设置图层
				GUI_LyrWinDelete(h_lyr);
				__wrn(" lry_new_frm_setting isn't null, delete it OK\n");
				app_new_setting_ctrl->h_frm_general_setting = NULL ;//赋值为空
			}
				__wrn("app_new_setting_ctrl destory begin is ok\n");
			#endif
			#if 1//方法2 删除图层
			if(app_new_setting_ctrl->lry_new_frm_setting)//新添加的通用设置图层，清空
			{	
				__here__ ;
				__wrn(" lry_new_frm_setting isn't null, delete it... \n");
				GUI_LyrWinDelete(app_new_setting_ctrl->lry_new_frm_setting);//删除新添加的app应用图层
				//GUI_LyrWinDelete(h_lyr);
				__wrn(" lry_new_frm_setting isn't null, delete it OK\n");
				__here__ ;
				app_new_setting_ctrl->lry_new_frm_setting = NULL ;//赋值为空
			}
				__wrn("app_new_setting_ctrl destory begin is ok\n");
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
			My_Bfree(app_new_setting_ctrl, sizeof(app_new_setting_ctrl_t));//结构体数组释放内存空间
			
			
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//绘制显示所有图片内容
			__wrn("app new Setting GUI_MSG_PAINT...\n");

		return EPDK_OK;

		case GUI_MSG_KEY:	//manwin窗口按键处理函数，结束用 break 这样就可以往framewin窗口发送按键信息
		{
			__wrn("app new Setting GUI_MSG_KEY...\n");
			__wrn("app new Setting GUI_MSG_KEY msg->dwAddData1=%d...\n",msg->dwAddData1);
			//_app_New_Setting_key_proc(msg);
			#if 1
			switch(msg->dwAddData1)
			{
				__wrn("new Setting GUI_MSG_KEY_ESCAPE...\n");
				#if 0//测试物理按键播放按钮按下功能，发送关闭窗口id，退回到home主界面窗口显示所有app
				case GUI_MSG_KEY_ENTER://等于回车就关闭窗口
				{	
					#if 1
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_KEY_ESCAPE is...\n");
					my_msg.id = GUI_MSG_CLOSE;//关闭窗口
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = 0;
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#else
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_COMMAND is...\n");
					my_msg.id = GUI_MSG_COMMAND;//进入命令
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = NEW_SETTING_ID;//窗口id
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#endif
					#if 0
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_COMMAND is...\n");
					my_msg.id = MSG_NEW_SETTING_OP_ENTER;//进入回车确认
					my_msg.h_srcwin = msg->h_srcwin;
					my_msg.h_deswin = msg->h_deswin;
					my_msg.dwAddData1 = 0;//窗口id
					my_msg.dwAddData2 = 0;
					GUI_SendNotifyMessage(&my_msg);
					#endif
					return EPDK_OK;
				}
				#endif
				case GUI_MSG_KEY_ESCAPE://物理返回按键按下，然后关闭窗口，退回到home主界面窗口显示所有app
				{
					#if 1
					__gui_msg_t my_msg;
					__wrn("new Setting GUI_MSG_KEY_ESCAPE is...\n");
					my_msg.id = GUI_MSG_CLOSE;//关闭窗口
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

		case MSG_NEW_SETTING_OP_ENTER:	//回车确认按键
		{
			__gui_msg_t mymsg;
			app_new_setting_ctrl_t	*app_new_setting_ctrl;
			key_number += 1;
			__wrn("app new Setting MSG_NUMBER_MENU_OP_ENTER...\n");
			#if 0	
			if(key_number == 5)//删除app应用，然后返回上一层home显示全部app应用
			{
				key_number = 0;
				#if 0
				mymsg.id = GUI_MSG_CLOSE;//调用关闭窗口
				mymsg.h_deswin = app_new_setting_ctrl->lry_new_frm_setting;
				mymsg.h_srcwin = NULL;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#else
				mymsg.id = GUI_MSG_CLOSE;//GUI_MSG_DESTROY;//调用销毁窗口
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
				New_Setting_paint_one_item(msg,key_number);//刷新number次数显示内容
			}
			#endif
		}
		return EPDK_OK;
		case GUI_MSG_COMMAND:	//命令输入
		{
			app_new_setting_ctrl_t *setting_ctrl;
			setting_ctrl = (app_new_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			#if 1
			__wrn("app_new_setting.c GUI_MSG_COMMAND  is....\n");
			__wrn("app_new_setting.c LOWORD(msg->dwAddData1) = 0x%x  is....\n",LOWORD(msg->dwAddData1));
			switch(LOWORD(msg->dwAddData1))
			{
				#if 1//进入到新添加New_Setting.c的framewin窗口
				__wrn("NEW_SETTING_ID  is....\n");
				case NEW_SETTING_ID://0x9700
				{
					__wrn("NEW_SETTING_ID  is ok\n");
					__wrn("HIWORD(msg->dwAddData1) = %d is....\n",HIWORD(msg->dwAddData1));
					switch(HIWORD(msg->dwAddData1))
					{
						__wrn("HIWORD(msg->dwAddData1)  is....\n");
						case ID_OP_NEW_SETTING_UP:	//上按键1
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general_setting;//向New_Setting.c的framewin窗口发送按键信号
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NEW_SETTING_OP_UP;
							__wrn("MSG_NUMBER_MENU_OP_UP NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_NEW_SETTING_DOWN:	//下按键2
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_general_setting;//向New_Setting.c的framewin窗口发送按键信号
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_NEW_SETTING_OP_DOWN;
							__wrn("MSG_NUMBER_MENU_OP_DOWN NEW \n");
							GUI_SendMessage(&mymsg);
						}
						break;
						case ID_OP_NEW_SETTING_ENTER:	//回车按键3
						{
							#if 1
							__gui_msg_t mymsg;
							__wrn("---ID_OP_NUMBER_MENU_ENTER 1 is....\n");
							mymsg.h_deswin = setting_ctrl->h_frm_general_setting;//向New_Setting.c的framewin窗口发送按键信号
							__wrn("---ID_OP_NUMBER_MENU_ENTER 2 is....\n");
							mymsg.h_srcwin = NULL;
							__wrn("---ID_OP_NUMBER_MENU_ENTER 3 is....\n");
							mymsg.id = MSG_NEW_SETTING_OP_ENTER;				 //回车确认id
							__wrn("---ID_OP_NUMBER_MENU_ENTER 4 is....\n");
							//mymsg.dwReserved = msg->dwReserved;
							__wrn("---ID_OP_NUMBER_MENU_ENTER 5 is....\n");
							GUI_SendMessage(&mymsg);							 //GUI发送信息
							__wrn("---ID_OP_NUMBER_MENU_ENTER 6 is....\n");
							#endif
						}
						break;
						case ID_OP_NEW_SETTING_EXIT:	//发送退出新添加app应用程序的manwin窗口id消息信号
						{
							#if 1//发送按键信号到当前的APP_NEWSETTING_ID父窗口manwin窗口的写法
							__gui_msg_t my_msg;
							__wrn("new Setting ID_OP_NEW_SETTING_EXIT GUI_MSG_KEY_ESCAPE is...\n");
							my_msg.id = GUI_MSG_CLOSE;			//当前文件manwin窗口的关闭窗口id
							my_msg.h_srcwin = msg->h_srcwin;	//app_New_Setting.c父窗口manwin窗口发送关闭信号
							my_msg.h_deswin = msg->h_deswin;	//app_New_Setting.c父窗口manwin窗口发送关闭信号
							my_msg.dwAddData1 = 0;
							my_msg.dwAddData2 = 0;
							GUI_SendNotifyMessage(&my_msg);		//发送通知信息
							return EPDK_OK;						//直接退出这个case GUI_MSG_COMMAND:语句
							#endif
						}
						break;
						case ID_NEW_SETTING_SWITCH_TO_SETTING:  //发送切换跳转app应用程序的id消息信号
						{
							#if 1//发送按键信号到当前的APP_NEWSETTING_ID父窗口manwin窗口的写法
							__gui_msg_t my_msg;
							__wrn("new Setting ID_NEW_SETTING_SWITCH_TO_SETTING GUI_MSG_KEY_ESCAPE is...\n");
							my_msg.id = GUI_MSG_QUIT;			//当前文件manwin窗口的退出窗口id
							my_msg.h_srcwin = msg->h_srcwin;	//app_New_Setting.c父窗口manwin窗口发送退出信号
							my_msg.h_deswin = msg->h_deswin;	//app_New_Setting.c父窗口manwin窗口发送退出信号
							my_msg.dwAddData1 = 0;
							my_msg.dwAddData2 = 0;
							GUI_SendNotifyMessage(&my_msg);		//发送通知信息到当前manwin窗口
							return EPDK_OK;						//直接退出这个case GUI_MSG_COMMAND:语句
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
*函数作用：新添加的app应用manwin窗口创建函数
*形参：H_WIN h_parent：窗口定义 ；__gui_msg_t *msg：GUI固定必须的结构体指针变量
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
	create_info.name            = APP_NEWSETTING;//APP_SETTING;//新的名称
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_New_Setting_proc);
	create_info.attr            = (void *)app_New_setting_ctrl;
	create_info.id			= APP_NEWSETTING_ID;//APP_SETTING_ID;//新的ID
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));								//返回一个ManWin创建窗口
}
/*====================================================================================
*函数作用：窗口删除
====================================================================================*/
__s32 app_New_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
	app_new_setting_ctrl_t	*app_New_setting_ctrl;
	app_New_setting_ctrl	 = (app_new_setting_ctrl_t *)GUI_WinGetAttr(hmanwin);
	__here__ ;
	
	return EPDK_OK;
}


