
#include "number_menu_tips.h"
#include "setting_para.h"
#include "string.h"



typedef struct number_menu_tips_ctrl
{
	number_menu_tip_para_t *tips_para;

	__s32 focus_txt_color;		//选中的txt文本颜色
	__s32 unfocus_txt_color;	//未选中的txt文本颜色
	H_THEME tip_bmp;
	H_THEME tip_f_bmp;

	char ok_txt[128];		//string ok
	char tsize[128];		//total size
	char fsize[128];		//free size
	char svers[128];		//soft version
	char total_size[128];
	char free_size[128];
	char title_txt[128];
	tips_uipara_t *tips_uipara;
}
number_menu_tips_ctrl_t;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
H_LYR number_menu_8bpp_layer_create(RECT *rect,__s32 pipe)
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
		"number_menu tips layer",
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
	lstlyr.scn_win.x		= (1024 -191) / 2;
	lstlyr.scn_win.y		= (600 - 75 - 101) / 2; //240为屏幕高度，60为底端高度
	lstlyr.scn_win.width  	= rect->width;
	lstlyr.scn_win.height 	= rect->height;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("tips layer create error !\n");
	}

	return layer;
}
//获取回车数值
void number_menu_tips_get_volume_size(__gui_msg_t *msg)
{

}
//初始化
static void number_menu_tip_init(number_menu_tips_ctrl_t *tips_ctrl)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	tips_ctrl->tips_uipara = setting_get_tips_para(screen_size);//获取对话框框大小

	if(!tips_ctrl->tip_bmp)
	{
		tips_ctrl->tip_bmp = theme_open(ID_SETTING_SET_INFO_BMP);
	}

	if(!tips_ctrl->tip_f_bmp)
	{
		tips_ctrl->tip_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);
	}
	//文本菜单
	get_menu_text(STRING_SET_OK, tips_ctrl->ok_txt, 128);
/*	get_menu_text(STRING_SET_COMMON_PRODUCTINFO_TITLE , tips_ctrl->title_txt , 128);
	get_menu_text(STRING_SET_TIPS_TSIZE, tips_ctrl->tsize, 128);
	get_menu_text(STRING_SET_TIPS_FSIZE, tips_ctrl->fsize, 128);
	get_menu_text(STRING_SET_TIPS_SVERS, tips_ctrl->svers, 128);
*/
	{
		reg_system_para_t *pRegSysParam = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
		strcat(tips_ctrl->svers, pRegSysParam->ver_inf);
	}
}
//绘制所有图层
static __s32 number_menu_tips_paint_all(__gui_msg_t *msg)
{
	number_menu_tips_ctrl_t *tips_ctrl;
	GUI_RECT gui_rect;

	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	tips_ctrl = (number_menu_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!tips_ctrl)
	{
		__msg(" is null \n ");
		return EPDK_FAIL ;
	}

	GUI_LyrWinSel(tips_ctrl->tips_para->layer);		//窗口事件
	GUI_SetFont(tips_ctrl->tips_para->msgbox_font);	//设置对话框字体
	GUI_UC_SetEncodeUTF8();							//设置编码格式
	number_menu_tips_get_volume_size(msg);			//获取回车按键点击次数
	GUI_SetColor(tips_ctrl->focus_txt_color);		//设置文本颜色
	GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_bmp), tips_ctrl->tips_uipara->tips_bmp_pos.x, tips_ctrl->tips_uipara->tips_bmp_pos.y);//绘制显示函数
	//GUI_DispStringAt(tips_ctrl->total_size, tips_ctrl->tips_uipara->tip_tsize_pos.x, tips_ctrl->tips_uipara->tip_tsize_pos.y);		//显示字符串
	//GUI_DispStringAt(tips_ctrl->free_size, tips_ctrl->tips_uipara->tip_fsize_pos.x, tips_ctrl->tips_uipara->tip_fsize_pos.y);
	GUI_SetBkColor(BEETLES_BLACK_COLOR);			//设置窗口背景颜色
	gui_rect.x0 = tips_ctrl->tips_uipara->tip_ok_bmp_pos.x ;	//矩形的起坐标
	gui_rect.y0 = tips_ctrl->tips_uipara->tip_ok_bmp_pos.y;
	gui_rect.x1 = gui_rect.x0 + TIPS_YES_NO_BMP_WIDTH - 1;		//矩形的止坐标
	gui_rect.y1 = gui_rect.y0 + TIPS_YES_NO_BMP_HEIGHT - 1;
	GUI_ClearRectEx(&gui_rect);						//矩形删除
	GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), tips_ctrl->tips_uipara->tip_ok_bmp_pos.x, tips_ctrl->tips_uipara->tip_ok_bmp_pos.y);//绘制显示OK函数
	GUI_DispStringInRect(tips_ctrl->ok_txt , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);//字符串显示到矩形窗口
	gui_rect.x0 = tips_ctrl->tips_uipara->tips_tiltle_pos.x + 8 ;
	gui_rect.y0 = tips_ctrl->tips_uipara->tips_tiltle_pos.y ;
	gui_rect.x1 = gui_rect.x0 + tips_ctrl->tips_uipara->tips_tiltle_pos.width - 1;
	gui_rect.y1 = gui_rect.y0 + tips_ctrl->tips_uipara->tips_tiltle_pos.height - 1 ;
	GUI_DispStringInRect(tips_ctrl->svers, &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);
	return EPDK_OK;
}
//按键处理函数
static __s32  number_menu_tips_key_proc(__gui_msg_t *msg)
{
#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_ENTER, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_LEFT:		//映射为up
			case GUI_MSG_KEY_LONGLEFT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_UP, 0, 0);
			}
			break;

			case GUI_MSG_KEY_RIGHT: 	//映射为down
			case GUI_MSG_KEY_LONGRIGHT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_DOWN, 0, 0);
			}
			break;

			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				main_cmd2parent(msg->h_deswin, ID_OP_UP, 0, 0);
				break;

			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				main_cmd2parent(msg->h_deswin, ID_OP_DOWN, 0, 0);
				break;

			case GUI_MSG_KEY_ESCAPE:
			case GUI_MSG_KEY_MENU:
				main_cmd2parent(msg->h_deswin, ID_OP_EXIT, 0, 0);
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
#endif
	return EPDK_OK;
}

/*
	回调函数
*/
static __s32 number_menu_tips_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//创建菜单窗口
		{
			number_menu_tips_ctrl_t *tips_ctrl;
			number_menu_tip_para_t *tips_para;
			tips_ctrl = (number_menu_tips_ctrl_t *)My_Balloc(sizeof(number_menu_tips_ctrl_t));

			if(!tips_ctrl)
			{
				__err("memory balloc fail........\n");
				return EPDK_FAIL;
			}

			eLIBs_memset(tips_ctrl, 0, sizeof(number_menu_tips_ctrl_t));	//内存分配
			tips_para = (number_menu_tip_para_t *)GUI_WinGetAttr(msg->h_deswin);
			tips_ctrl->focus_txt_color = APP_COLOR_YELLOW;		//选中的文本颜色
			tips_ctrl->unfocus_txt_color = APP_COLOR_WHITE;		//未选中的文本颜色
			tips_ctrl->tips_para = tips_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)tips_ctrl);	//添加一个数据
			number_menu_tip_init(tips_ctrl);					//
			GUI_LyrWinCacheOn();	//窗口检查打开
			GUI_LyrWinSetSta(tips_ctrl->tips_para->layer, GUI_LYRWIN_STA_ON);	//窗口设置开启状态
			GUI_LyrWinSetTop(tips_ctrl->tips_para->layer);						//窗口设置停止状态
			GUI_LyrWinCacheOff();	//窗口检查关闭
		}

		return EPDK_OK;

		case DSK_MSG_FS_PART_PLUGIN:
		case DSK_MSG_FS_PART_PLUGOUT:
		{
			//number_menu_tips_get_volume_size(msg);	//获取回车按键点击次数
			__here__;
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//关闭窗口
		{
			number_menu_tips_ctrl_t *tips_ctrl;
			tips_ctrl = (number_menu_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!tips_ctrl)
			{
				__msg("number_menu_tips_ctrl is null \n");
				return EPDK_FAIL ;
			}

			//main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_GENERAL, 0, 0);//返回上一层
			main_cmd2parent(msg->h_deswin, ID_OP_EXIT, 0, 0);//直接退出
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY:	//销毁窗口
		{
			number_menu_tips_ctrl_t *tips_ctrl = NULL ;
			tips_ctrl = (number_menu_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!tips_ctrl)
			{
				__err("tips_ctrl is null........\n");
				return EPDK_FAIL;
			}

			__msg("number_menu_tips win destroy\n");
			__msg("number_menu_tips win close theme\n");
			theme_close(tips_ctrl->tip_bmp);	//图片关闭
			theme_close(tips_ctrl->tip_f_bmp);	//图片关闭

			if(tips_ctrl->tips_para)
			{
				__msg("number_menu_tips win free number_menu_tips_para.....\n");
				My_Bfree(tips_ctrl->tips_para, sizeof(number_menu_tip_para_t));//释放内存
				tips_ctrl->tips_para = NULL ;
			}

			if(tips_ctrl)
			{
				__msg("number_menu_tips win free number_menu_tips_ctrl.....\n");
				My_Bfree(tips_ctrl, sizeof(number_menu_tips_ctrl_t));
				tips_ctrl = NULL ;
			}
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//绘制显示所有图片内容
			number_menu_tips_paint_all(msg);
			return EPDK_OK;

		case GUI_MSG_KEY:	//按键处理函数
			number_menu_tips_key_proc(msg);
			return EPDK_OK;

		case MSG_OP_ENTER:	//回车确认按键
		{
//			tips_ctrl_t *tips_ctrl;
//			tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			{
				__gui_msg_t mymsg;
				mymsg.id = GUI_MSG_CLOSE;//调用关闭窗口
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
			}
		}

		return EPDK_OK;

		case MSG_OP_EXIT:	//退出返回按键，也是调用关闭窗口
		{
			__gui_msg_t mymsg;
			mymsg.id = GUI_MSG_CLOSE;
			mymsg.h_deswin = msg->h_deswin;
			mymsg.h_srcwin = msg->h_srcwin;
			mymsg.dwAddData1 = msg->dwAddData1;
			mymsg.dwAddData2 = msg->dwAddData2;
			GUI_SendMessage(&mymsg);
		}

		return EPDK_OK;

		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

/*
	建立窗口
*/
H_WIN number_menu_tips_win_create(H_WIN h_parent, number_menu_tip_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	number_menu_tip_para_t *tips_para;
	FB fb;
	GUI_LyrWinGetFB(para->layer, &fb);
	tips_para = (number_menu_tip_para_t *)My_Balloc(sizeof(number_menu_tip_para_t));

	if(!tips_para)
	{
		__err("memory balloc fail........\n");
		return EPDK_FAIL;
	}

	tips_para->msgbox_font = para->msgbox_font;	//消息对话框
	tips_para->layer = para->layer;				//图层
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t)); //分配内存空间
	framewin_para.name =	"number_menu_tips_win";	//窗口名字
	framewin_para.dwExStyle = WS_EX_NONE;
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;
	framewin_para.hHosting = h_parent;
	framewin_para.id         = GENERAL_TIPS_ID;		//窗口ID
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)number_menu_tips_proc);//窗口控件回调函数
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width =  fb.size.width;
	framewin_para.rect.height =  fb.size.height;
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)tips_para;	//窗口的属性
	framewin_para.hLayer = para->layer;		//窗口图层
	return (GUI_FrmWinCreate(&framewin_para));	//返回控件窗口创建成功
}

void  number_menu_tips_win_delete(H_WIN tips_win)
{
	GUI_FrmWinDelete(tips_win);
}




























