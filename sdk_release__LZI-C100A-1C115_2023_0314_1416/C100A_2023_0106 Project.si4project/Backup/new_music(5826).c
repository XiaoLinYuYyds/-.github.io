#include "App_new_music.h"

#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR
#define COLOUR_TO_8BPP_INDEX_0          0xF0

//创建图层
static	H_LYR app_new_music_8bpp_laye_creat(RECT *rect, __s32 pipe)	
{
		H_LYR layer = NULL;
		FB	fb =
		{
			{0, 0}, 										/* size 	 */
			{0, 0, 0},										/* buffer	 */
			{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},	/* fmt		 */
		};
		__disp_layer_para_t lstlyr =
		{
			DISP_LAYER_WORK_MODE_PALETTE,					 /* mode	  */
			0,												/* ck_mode	 */
			0,												/* alpha_en  */
			0,												/* alpha_val */
			1,												/* pipe 	 */
			0xff,											/* prio 	 */
			{0, 0, 0, 0},									/* screen	 */
			{0, 0, 0, 0},									/* source	 */
			DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
			NULL											/* fb		 */
		};
		__layerwincreate_para_t lyrcreate_info =
		{
			"new music layer",
			NULL,
			GUI_LYRWIN_STA_SUSPEND,
			GUI_LYRWIN_NORMAL
		};
		fb.size.width		= rect->width;
		fb.size.height		= rect->height;
		lstlyr.src_win.x		= 0;
		lstlyr.src_win.y		= 0;
		lstlyr.src_win.width	= rect->width;
		lstlyr.src_win.height	= rect->height;
		lstlyr.scn_win.x		= rect->x;
		lstlyr.scn_win.y		= rect->y;
		lstlyr.scn_win.width	= rect->width;
		lstlyr.scn_win.height	= rect->height;
		lstlyr.pipe = pipe;
		lstlyr.fb = &fb;
		lyrcreate_info.lyrpara = &lstlyr;
		layer = GUI_LyrWinCreate(&lyrcreate_info);
	
		if(!layer)
		{
			__err("GUI_LyrWinCreate fail... !\n");
		}
	
		return layer;
}

//命令发送函数
__s32 app_new_music_cmd2para(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t mymsg;
	__wrn("\n*****************app_new_music_cmd2para********************\n");
	mymsg.h_deswin = GUI_WinGetParent(hwin);
	mymsg.h_srcwin = NULL;
	mymsg.id = GUI_MSG_COMMAND;
	mymsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	mymsg.dwAddData2 = data1;
	mymsg.dwReserved = data2;
	return (GUI_SendNotifyMessage(&mymsg));
}

static __s32 PlayCurMusicFile(void)
{
	__s32 ret;
	char Buf[FSYS_DIRNAME_MAX];
	eLIBs_memset(Buf, 0, sizeof(Buf));
	ret = robin_npl_get_cur();//获取当前播放文件

	if(-1 == ret)
	{
		__wrn("robin_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}

	ret = robin_npl_index2file(ret, Buf);//通过索引获取文件名
	__wrn("ret =%d\n", ret);

	if(0 == ret)
	{
		robin_play_file(Buf);
		__wrn("robin_play_file file=%s\n", Buf);

	}
	else
	{
		__wrn("ANOLE_PlayCurFile fail\n");
	}

	return ret;
}

static __s32 app_new_music_init(__gui_msg_t *msg)
{
	__robin_para_t 	robin_para;
	__s32			ret;
	new_music_ctrl_t *new_music_ctrl;
	new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	ret = ;

	if(ret == -1)
	{
		__wrn("robin_init failed!\n");
		return EPDK_FAIL;
	}
	/*else if(ret == 1)
	{
		__inf("robin_init failed! ret = 1\n");
		ShowErrorPic(new_music_ctrl->err_pic);
		return EPDK_FAIL;
	}*/


	PlayCurMusicFile();//播放音乐文件
	return EPDK_TRUE;
}


//回调处理函数
static __s32 __app_new_music_proc(__gui_msg_t *msg)
{
	__wrn("msg->id = %d\n",msg->id);
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			gscene_bgd_set_state(BGD_STATUS_HIDE);//背景场景隐藏
			/*g_disable_close_scn();//关闭荧光屏
			com_set_backcolor(CONST_BackColor_Black);//设置背景颜色为黑色
			com_set_palette_by_id(ID_PHOTO_COLOURTABLE_BMP);//调色板id
			{
				//背景色，文字编辑底色
				__u32 color[] = {COLOUR_BG_8BPP, COLOUR_EDIT_BG_8BPP};
				com_set_palette_color_by_index(COLOUR_TO_8BPP_INDEX_0, color, BEETLES_TBL_SIZE(color));
			}*/
			app_new_music_init(msg);
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
H_WIN	app_new_music_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	new_music_ctrl_t *new_music_ctrl;
	H_WIN	hManWin;

	new_music_ctrl = (new_music_ctrl_t *)esMEMS_Balloc(sizeof(new_music_ctrl_t));
	eLIBs_memset(new_music_ctrl, 0, sizeof(new_music_ctrl_t));
	new_music_ctrl->font = para->font;
	new_music_ctrl->root_type = para->root_type;
	/*开始操作配置manwin窗口参数*/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));//结构体初始化，数据清0
	create_info.name	=	APP_NEWPHOTO;
	create_info.hParent	=	para->h_parent;
	create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_music_proc);
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




