#include "number_menu_tips.h"
#include "number_menu_para.h"
#include "string.h"
#include "number_menu_app_ui.h"//app应用显示图层
#include "typedef.h"



typedef struct number_menu_numbers_ctrl
{
	number_menu_number_para_t *numbers_para;

	__s32 focus_txt_color;		//选中的txt文本颜色
	__s32 unfocus_txt_color;	//未选中的txt文本颜色
	H_THEME number_bmp;
	H_THEME number_f_bmp;

	char number_ok_txt[128];		//string ok
	char number_tsize[128];		//total size
	char number_fsize[128];		//free size
	char number_svers[128];		//soft version
	char number_total_size[128];
	char number_free_size[128];
	char number_title_txt[128];
	numbers_uipara_t *numbers_uipara;
}
number_menu_numbers_ctrl_t;

/***********************************************************************************************************
	32位建立图层
************************************************************************************************************/
H_LYR number_menu_32bpp_layer_create(RECT *rect, __s32 pipe)
{
	H_LYR layer = NULL;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
		0,                                              /* ck_mode   */
		0,                                              /* alpha_en  */
		0,                                              /* alpha_val */
		0,                                              /* pipe      */
		0xff,                                           /* prio      */
		{0, 0, 0, 0},                           		/* screen    */
		{0, 0, 0, 0},                               	/* source    */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
		NULL                                            /* fb        */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"number menu layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= rect->width;
	lstlyr.src_win.height 	= rect->height;
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y;
	lstlyr.scn_win.width  	= rect->width;
	lstlyr.scn_win.height 	= rect->height;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app bar layer create error !\n");
	}

	return layer;
}

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
		"number_menu numbers layer",
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
		__err("numbers layer create error !\n");
	}
	__wrn("number_menu_8bpp_layer_create CREATE\n");
	return layer;
}
//获取回车按键点击数值
void number_menu_numbers_get_volume_size(__gui_msg_t *msg,__u32 num)
{
	number_menu_numbers_ctrl_t *numbers_ctrl;
	char		str[768];
	char		numstr[32];
	__u64		size;
	__u32		gnum;
	__u32		 snum;
	numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);
	eLIBs_strcat(str, numbers_ctrl->number_tsize);
	eLIBs_strcat(str, "number:");
	snum = num;
	eLIBs_uint2str_dec(snum, numstr);//int型转char型
	 
	if(snum >= 10)
	{
		eLIBs_strcat(str, numstr);
	}
	else
	{
		eLIBs_strcat(str, "0");
		eLIBs_strcat(str, numstr);
	}
	eLIBs_strcpy(numbers_ctrl->number_total_size, (const char *)str);
	__wrn("number_menu_numbers_get_volume_size CREATE\n");
}
//初始化，申请使用图片资源
static void number_menu_number_init(number_menu_numbers_ctrl_t *numbers_ctrl)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);		//获取LCD屏幕显示尺寸字节大小
	numbers_ctrl->numbers_uipara = number_get_numbers_para(screen_size);//获取对话框矩形大小

	if(!numbers_ctrl->number_bmp)
	{
		numbers_ctrl->number_bmp = theme_open(ID_SETTING_SET_INFO_BMP);//传入图片ID，
	}

	if(!numbers_ctrl->number_f_bmp)
	{
		numbers_ctrl->number_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);//
	}
	__wrn("number_menu_number_init CREATE\n");
	//文本菜单
	//get_menu_text(STRING_SET_OK, numbers_ctrl->number_ok_txt, 128);

}
//绘制所有图层
 __s32 number_menu_numbers_paint_all(__gui_msg_t *msg,__u32 num)
{
	number_menu_numbers_ctrl_t *numbers_ctrl;
	GUI_RECT gui_rect;

	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)//图层窗口创建失败
	{
		return EPDK_FAIL;
	}

	numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!numbers_ctrl)
	{
		__msg(" is null \n ");
		return EPDK_FAIL ;
	}
	__wrn("number_menu_numbers_paint_all CREATE\n");
	GUI_LyrWinSel(numbers_ctrl->numbers_para->layer);		//窗口事件
	GUI_SetFont(numbers_ctrl->numbers_para->msgbox_font);	//设置对话框字体
	GUI_UC_SetEncodeUTF8();									//设置编码格式
	number_menu_numbers_get_volume_size(msg,num);			//获取回车按键点击次数
	__wrn("number_menu_numbers_paint_all3 CREATE\n");
	GUI_SetColor(GUI_BLACK);								//设置文本颜色
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);					//设置画图模式为正常
	__wrn("number_menu_numbers_paint_all4 CREATE\n");
	GUI_BMP_Draw(theme_hdl2buf(numbers_ctrl->number_bmp), numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//绘制显示函数
	__wrn("number_menu_numbers_paint_all5 CREATE\n");
	__wrn("numbers_ctrl->number_total_size = %s\n",numbers_ctrl->number_total_size);
	__wrn("number_tsize_pos.x = %d , number_tsize_pos.y = %d\n",numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);
	GUI_DispStringAt(numbers_ctrl->number_total_size, numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);			//显示字符串
	//GUI_DispStringAt("01",32,28);//字符串显示和坐标
	__wrn("number_menu_numbers_paint_all6 CREATE\n");
	//..GUI_SetBkColor(BEETLES_BLACK_COLOR);			//设置窗口背景颜色
	__wrn("number_menu_numbers_paint_all7 CREATE\n");
	//..GUI_ClearRectEx(&gui_rect);						//矩形删除
	__wrn("number_menu_numbers_paint_all 2 CREATE\n");
	return EPDK_OK;
}
//按键处理函数
static __s32  number_menu_numbers_key_proc(__gui_msg_t *msg)
{
#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("number_menu_numbers key msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("number_menu_numbers key last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_ENTER, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("number_menu_numbers key msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//为up
			case GUI_MSG_KEY_LONGLEFT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_UP, 0, 0);//命令控制
			}
			break;

			case GUI_MSG_KEY_RIGHT: 	//为down
			case GUI_MSG_KEY_LONGRIGHT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_DOWN, 0, 0);
			}
			break;

			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_UP, 0, 0);
				break;

			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_DOWN, 0, 0);
				break;

			case GUI_MSG_KEY_ESCAPE:
			case GUI_MSG_KEY_MENU:
				main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_EXIT, 0, 0);
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
static __s32 number_menu_numbers_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	switch(msg->id)
	{
		__wrn("number_menu_numbers msg->id = %d\n",msg->id);
		case GUI_MSG_CREATE:	//创建菜单窗口
		{	
			#if 1
				number_menu_numbers_ctrl_t *numbers_ctrl;	//number：次数显示提示
			//#else
				numbermenu_attr_t *numbers_attr;			//app应用显示
			#endif
			number_menu_number_para_t *numbers_para;
			numbers_ctrl = (number_menu_numbers_ctrl_t *)My_Balloc(sizeof(number_menu_numbers_ctrl_t));//内存分配
			numbers_attr = (numbermenu_attr_t *)esMEMS_Balloc(sizeof(numbermenu_attr_t));
			if(!numbers_ctrl)
			{
				__err("memory balloc fail........\n");
				return EPDK_FAIL;
			}
			if(!numbers_attr)	//检测mmenu_attr_t
			{
				__err("mmenu_attr_t balloc fail........\n");
				return EPDK_FAIL;
			}
			eLIBs_memset(numbers_ctrl, 0, sizeof(number_menu_numbers_ctrl_t));			//内存初始化
			numbers_para = (number_menu_number_para_t *)GUI_WinGetAttr(msg->h_deswin);	//获取窗口的属性
			#if 1	//app应用窗口创建
				eLIBs_memset((void *)numbers_attr, 0, sizeof(numbers_attr));
				numbers_attr->number_font = numbers_para->smenu_font;					//app应用文本
				numbers_attr->number_layer = numbers_para->layer;						//app应用图层，选中的图片显示
				numbers_attr->number_bg_layer = numbers_para->bg_layer;					//app应用背影图层，就是没有选中的图片显示
				numbers_attr->number_focus_app = 0;										//选中的app应用
				numbers_attr->number_first_app = 0;										//第一个app应用
				numbers_attr->number_focus_txt_color = APP_COLOR_YELLOW;				//选中app应用的文本颜色
				numbers_attr->number_unfocus_txt_color = APP_COLOR_WHITE;				//未选中app应用的文本颜色
				GUI_WinSetAddData(msg->h_deswin, (__s32)numbers_attr);
				__wrn("number_menu_numbers GUI_MSG_CREATE\n");
				init_numbermenu_res(numbers_attr);										//app应用图片申请使用资源
				//paint_numbermain_app_all(numbers_attr);			//绘制所有app应用
				paint_numbermain_one_app(numbers_attr, 1, 1);	//绘制单个app
				__wrn("paint_numbermain_one_app GUI_MSG_CREATE\n");
				GUI_LyrWinSetSta(numbers_attr->number_layer, GUI_LYRWIN_STA_ON);		//窗口设置开启状态
				GUI_LyrWinSetTop(numbers_attr->number_layer);						//窗口设置停止状态
			#endif
			#if 0	//number数值显示窗口创建
			numbers_ctrl->focus_txt_color = APP_COLOR_YELLOW;							//选中的文本颜色
			numbers_ctrl->unfocus_txt_color = APP_COLOR_WHITE;							//未选中的文本颜色
			numbers_ctrl->numbers_para = numbers_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)numbers_ctrl);						//添加一个数据
			number_menu_number_init(numbers_ctrl);										//number:00显示提示图片资源申请和窗口显示大小
		//	GUI_LyrWinCacheOn();	//窗口检查打开
			GUI_LyrWinSetSta(numbers_ctrl->numbers_para->layer, GUI_LYRWIN_STA_ON);		//窗口设置开启状态
			GUI_LyrWinSetTop(numbers_ctrl->numbers_para->layer);						//窗口设置停止状态
			//number_menu_numbers_paint_all(msg,0);										//绘制所有图层
			//GUI_LyrWinCacheOff();	//窗口检查关闭
			#endif
			__wrn("number_menu_numbers GUI_MSG_CREATE\n");
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//关闭窗口
		{
			number_menu_numbers_ctrl_t *numbers_ctrl;
			numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!numbers_ctrl)
			{
				__msg("number_menu_numbers_ctrl is null \n");
				return EPDK_FAIL ;
			}
			__wrn("GUI_MSG_CLOSE is  \n");
			main_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_SWITCH_TO_GENERAL, 0, 0);//返回上一层
			//main_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_OP_EXIT, 0, 0);//直接退出
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY:	//销毁窗口
		{
			number_menu_numbers_ctrl_t *numbers_ctrl = NULL ;
			numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!numbers_ctrl)
			{
				__err("numbers_ctrl is null........\n");
				return EPDK_FAIL;
			}

			__msg("number_menu_numbers win destroy\n");
			__msg("number_menu_numbers win close theme\n");
			theme_close(numbers_ctrl->number_bmp);	//图片关闭
			theme_close(numbers_ctrl->number_f_bmp);	//图片关闭

			if(numbers_ctrl->numbers_para)
			{
				__msg("number_menu_numbers win free number_menu_numbers_para.....\n");
				My_Bfree(numbers_ctrl->numbers_para, sizeof(number_menu_number_para_t));//释放内存
				numbers_ctrl->numbers_para = NULL ;
			}

			if(numbers_ctrl)
			{
				__msg("number_menu_numbers win free number_menu_numbers_ctrl.....\n");
				My_Bfree(numbers_ctrl, sizeof(number_menu_numbers_ctrl_t));
				numbers_ctrl = NULL ;
			}
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//绘制显示所有图片内容
			__wrn("one  paint_all \n");
			number_menu_numbers_paint_all(msg,0);
			return EPDK_OK;

		case GUI_MSG_KEY:	//按键处理函数
			number_menu_numbers_key_proc(msg);
			return EPDK_OK;

		case MSG_NUMBER_MENU_OP_ENTER:	//回车确认按键
		{
			__gui_msg_t mymsg;
			key_number += 1;
			if(key_number == 5)//删除app应用，然后显示全部app应用
			{
				key_number = 0;
				#if 1
				mymsg.id = GUI_MSG_CLOSE;//调用关闭窗口
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#else
				mymsg.id = GUI_MSG_DESTROY;//调用销毁窗口
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#endif
				
			}
			else
			{	
				__wrn("key_number =%d  \n",key_number);
				number_menu_numbers_paint_all(msg,key_number);//刷新显示内容
			}
		}

		return EPDK_OK;

		case MSG_NUMBER_MENU_OP_EXIT:	//退出返回按键，也是调用关闭窗口
		{
			__gui_msg_t mymsg;
			__wrn("MSG_NUMBER_MENU_OP_EXIT  \n");
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
H_WIN number_menu_numbers_win_create(H_WIN h_parent, number_menu_number_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	number_menu_number_para_t *numbers_para;
	FB fb;
	GUI_LyrWinGetFB(para->layer, &fb);
	//numbers_para = (number_menu_number_para_t *)My_Balloc(sizeof(number_menu_number_para_t));
	numbers_para = (number_menu_number_para_t *)esMEMS_Balloc(sizeof(number_menu_number_para_t));//内存空间分配
	
	if(!numbers_para)
	{
		__err("memory balloc fail........\n");
		return EPDK_FAIL;
	}
	__wrn("number1_menu_numbers\n");
	//numbers_para->sub_menu_id = para->sub_menu_id;	//子菜单ID 可以不用
	//numbers_para->root_type = para->root_type;		//
	//numbers_para->focus_item = para->focus_item;	//子菜单内容
	
	numbers_para->msgbox_font = para->smenu_font;	//消息对话框
	numbers_para->layer = para->layer;				//图层
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t)); //分配内存空间
	framewin_para.name =	"number_menu_numbers_win";	//窗口名字
	framewin_para.dwExStyle = WS_EX_NONE;
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;
	framewin_para.hHosting = h_parent;				//窗口句柄
	framewin_para.id         = SUB_MENUNUMBER_ID;	//GENERAL_TIPS_ID;//GENERAL_TIPS_ID;//	//窗口ID,作用返回上一层需要用到
	__wrn("number2_menu_numbers\n");
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)number_menu_numbers_proc);//窗口控件回调函数
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width =  fb.size.width;
	framewin_para.rect.height =  fb.size.height;
	__wrn("number3_menu_numbers\n");
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)numbers_para;	//窗口的属性
	framewin_para.hLayer = para->layer;		//窗口图层
	return (GUI_FrmWinCreate(&framewin_para));	//返回控件窗口创建成功
}

void  number_menu_numbers_win_delete(H_WIN numbers_win)
{
	GUI_FrmWinDelete(numbers_win);
}




























