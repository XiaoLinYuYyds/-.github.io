#include "App_new_photo.h"

#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR
#define COLOUR_TO_8BPP_INDEX_0          0xF0

//创建图层
#define		app_new_photo_8bpp_laye_creat(_lyr, _prt)	app_com_layer_create_default(_lyr, _prt, "new photo  layer")


//命令发送函数
__s32 app_new_photo_cmd2para(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t mymsg;
	__wrn("\n*****************app_new_photo_cmd2para********************\n");
	mymsg.h_deswin = GUI_WinGetParent(hwin);
	mymsg.h_srcwin = NULL;
	mymsg.id = GUI_MSG_COMMAND;
	mymsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	mymsg.dwAddData2 = data1;
	mymsg.dwReserved = data2;
	return (GUI_SendNotifyMessage(&mymsg));
}
static __bool SetShowMode(__s32 index)
{
	__bool ret;
	//DEBUG_CallFuncHere(ANOLE_SetShowMode);
	__wrn("ANOLE_SetShowMode(%d)\n", index);
	ret = anole_set_show_mode(index);
	//ret = 0;
	__here__;
	return ret;
}
static __s32 ANOLE_Init(__anole_para_t *para)
{
	__s32 ret;
	ret = anole_init();

	if(ret == 0)
	{
		//init ok
		anole_set_switch_mode_array_for_play(para->switchmodearray, para->switchnum);
		anole_set_switch_mode(para->SwitchMode);
		anole_set_play_order(para->PlayOrder);

		if(para->interval > 15) //for debug
		{
			para->interval = 15;
		}

		anole_set_play_interval(para->interval);
		//anole_set_show_mode(0);//temp
		PlayCurFile();
	}

	return ret;
}
__s32 PlayCurFile(void)
{
	__s32 ret;
	char Buf[FSYS_DIRNAME_MAX];
	eLIBs_memset(Buf, 0, sizeof(Buf));
	ret = anole_npl_get_cur();//获取当前播放文件

	if(-1 == ret)
	{
		__wrn("anole_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}

	ret = anole_npl_index2file(ret, Buf);//
	__wrn("ret =%d\n", ret);

	if(/*Buf[0] != 0 && */0 == ret)
	{
		anole_play_file(Buf);
		__wrn("anole_play_file file=%s\n", Buf);
		//ANOLE_ResetRotate();
	}
	else
	{
		__wrn("ANOLE_PlayCurFile fail\n");
	}

	return ret;
}

static __s32 app_new_photo_init(__gui_msg_t *msg)
{
	__anole_para_t 	anole_para;
	__s32			ret;
	new_photo_ctrl_t *new_photo_ctrl;
	new_photo_ctrl = (new_photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	//	notify_bg_music(msg->h_deswin, msg->h_deswin, get_photo_bgmusic());
	//PHOTO_LoadData(&anole_para);
	//create_bmp_res(ID_PHOTO_ERROR_BMP, new_photo_ctrl->err_pic);
	//anole_set_cb(ANOLE_EVENT_SHOW_PIC_FAIL, esKRNL_GetCallBack(__cb_show_pic_fail), (void *)(msg->h_deswin));
	//anole_set_cb(ANOLE_EVENT_SHOW_PIC_NEXT, esKRNL_GetCallBack(__cb_show_next_pic), (void *)(msg->h_deswin));
	/*initial anole core*/
	ret = ANOLE_Init(&anole_para);

	if(ret == -1)
	{
		__wrn("anole_init failed!\n");
		return EPDK_FAIL;
	}
	/*else if(ret == 1)
	{
		__inf("anole_init failed! ret = 1\n");
		ShowErrorPic(new_photo_ctrl->err_pic);
		return EPDK_FAIL;
	}*/

	//ANOLE_SetShowMode(get_photo_ratio());
	anole_set_show_mode(WILLOW_IMAGE_SHOW_ORISIZE);//设置显示模式
	PlayCurFile();//播放图片文件
	return EPDK_TRUE;
}


//回调处理函数
static __s32 __app_new_photo_proc(__gui_msg_t *msg)
{
	__wrn("msg->id = %d\n",msg->id);
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			gscene_bgd_set_state(BGD_STATUS_HIDE);//背景场景隐藏
			g_disable_close_scn();//关闭荧光屏
			com_set_backcolor(CONST_BackColor_Black);//设置背景颜色为黑色
			com_set_palette_by_id(ID_PHOTO_COLOURTABLE_BMP);//调色板id
			{
				//背景色，文字编辑底色
				__u32 color[] = {COLOUR_BG_8BPP, COLOUR_EDIT_BG_8BPP};
				com_set_palette_color_by_index(COLOUR_TO_8BPP_INDEX_0, color, BEETLES_TBL_SIZE(color));
			}
			app_new_photo_init(msg);
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1，销毁*/

		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2，绘制*/


		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{/*3，关闭*/

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{/*4，退出*/

		}
		return EPDK_OK;

		case GUI_MSG_KEY:{/*5，按键*/

		}
		break;

		case GUI_MSG_COMMAND:{/*7，命令*/

		}
		return EPDK_OK;
		
		default:
			break;

	}


	return GUI_ManWinDefaultProc(msg);
}

//创建manwin窗口
H_WIN	app_new_photo_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	new_photo_ctrl_t *new_photo_ctrl;
	H_WIN	hManWin;

	new_photo_ctrl = (new_photo_ctrl_t *)esMEMS_Balloc(sizeof(new_photo_ctrl_t));
	eLIBs_memset(new_photo_ctrl, 0, sizeof(new_photo_ctrl_t));
	new_photo_ctrl->font = para->font;
	new_photo_ctrl->root_type = para->root_type;
	/*开始操作配置manwin窗口参数*/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));//结构体初始化，数据清0
	create_info.name	=	APP_NEWPHOTO;
	create_info.hParent	=	para->h_parent;
	create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_photo_proc);
	create_info.attr	=	(void *)para;
	create_info.id		=	APP_NEWPHOTO_ID;
	create_info.hHosting = NULL;

	hManWin = GUI_ManWinCreate(&create_info);//返回一个创建好的manwin窗口
	if(!hManWin){
		__wrn("manwin create is false...");
		return EPDK_FALSE;
	}

	
	return hManWin;
}




