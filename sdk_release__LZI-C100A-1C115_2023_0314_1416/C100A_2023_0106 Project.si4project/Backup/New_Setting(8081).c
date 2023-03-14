#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"




/***********************************************************************************************************
	建立图层
************************************************************************************************************/
H_LYR New_Setting_8bpp_layer_create(RECT *rect,__s32 pipe)
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
		"New_Setting layer",
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
		__err("New_Setting layer create error !\n");
	}
	__wrn("New_Setting_8bpp_layer_create CREATE\n");
	return layer;
}
/****************************************************
*回车按键次数获取函数
****************************************************/
void New_Setting_get_volume_size(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str数组内存空间初始化
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

	eLIBs_strcpy(New_Setting_ctrl->new_number_size, (const char *)str);


}
/******************************************************************
*初始化，申请使用图片资源
*******************************************************************/
static void New_Setting_init(New_Setting_ctrl_t *New_Setting_ctrl)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	New_Setting_ctrl->New_Settings_uipara = New_Setting_get_numbers_para(screen_size);//获取控件的大小和显示位置
	__wrn("New_Setting_init is ok\n");
	if(!New_Setting_ctrl->New_Setting_bmp)
	{
		New_Setting_ctrl->New_Setting_bmp = theme_open(ID_SETTING_SET_INFO_BMP);//方框背景图
	}
	#if 1
	if(!New_Setting_ctrl->New_Setting_Bottom_bmp)
	{
		New_Setting_ctrl->New_Setting_Bottom_bmp = theme_open(ID_SETTING_SET_BOTTOM_BMP);//按钮矩形框背景图
	}
	#endif

	if(!New_Setting_ctrl->New_Setting_f_bmp)
	{
		New_Setting_ctrl->New_Setting_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);//确认背景图
	}
	get_menu_text(STRING_SET_SATURDAY, New_Setting_ctrl->Saturday_txt, 128);//星期六文字获取
	__wrn("New_Setting_init CREATE\n");
}
/**********************************************************************************
*释放申请的使用图片资源
**********************************************************************************/
static void New_Setting_uninit(New_Setting_ctrl_t *New_Setting_ctrl)
{
	__u32 	i;

	{
		__wrn("New_Setting_uninit is 1 ok\n");
		if(New_Setting_ctrl->New_Setting_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_bmp);//方框背景图
			New_Setting_ctrl->New_Setting_bmp = NULL;
		}
		__wrn("New_Setting_uninit is 2 ok\n");
		if(!New_Setting_ctrl->New_Setting_Bottom_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_Bottom_bmp);//按钮矩形框背景图
			New_Setting_ctrl->New_Setting_Bottom_bmp = NULL;
		}
		__wrn("New_Setting_uninit is 3 ok\n");
		if(New_Setting_ctrl->New_Setting_f_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_f_bmp);//确认背景图
			New_Setting_ctrl->New_Setting_f_bmp  = NULL;
		}
		__wrn("New_Setting_uninit is 4 ok\n");
	}

	return;
}
/****************************************************************
*绘制单个应用
****************************************************************/
 __s32 New_Setting_paint_one_item(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	GUI_RECT gui_rect;		//存放GUI矩形控件变量
	GUI_RECT gui_bottom_rect;		//存放GUI按钮矩形控件变量
	char	 *txt_character;//存放文字的字符串：
	void  	*number_theme;	//存放获取的图片使用资源数据
	void	*New_Setting_Bottom_theme;//存放获取的按键图片使用资源数据
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//添加一个数据到msg结构体
	GUI_LyrWinSel(New_Setting_ctrl->New_Settings_para->layer);//选择新添加的app应用framewin子窗口图层
	
	#if 0		//直接在图层上面显示绘制文本字符串
				GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);	//设置对话框字体
				GUI_UC_SetEncodeUTF8(); 								//设置编码格式
				__wrn("number_menu_numbers_paint_all 3 CREATE\n");
				GUI_SetColor(GUI_BLACK);								//设置文本颜色，黑色
				New_Setting_get_volume_size(msg,num);			//获取回车按键点击次数 字符串
				txt_character = New_Setting_ctrl->new_number_size;		//需要显示的文字字符串获取
				__wrn("number_menu_numbers_paint_all 4 CREATE\n");
				GUI_DispStringAt(txt_character,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				{
					__wrn("txt_character = %s\n",txt_character);		//直接显示文字字符串，文本绘制成功
				}
				
				//number_theme = theme_hdl2buf(numbers_ctrl->number_bmp);
				//GUI_BMP_Draw(number_theme, numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//绘制文本控件number:00显示函数
				__wrn("New_Setting_paint_one_item CREATE\n");
				__wrn("New_Setting_ctrl->new_number_size = %s\n",New_Setting_ctrl->new_number_size);
				__wrn("number_tsize_pos.x = %d , number_tsize_pos.y = %d\n",New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				//GUI_DispStringAt(numbers_ctrl->number_total_size, numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);			//在文本控件上显示字符串
				//GUI_DispStringAt("01",32,28);//字符串显示和坐标
				__wrn("txt paint is CREATE\n");//文本绘制成功
	#endif
	#if 1		//绘制矩形 然后在矩形框显示绘制文字字符串---> 显示 "number:00"
		GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);		//设置对话框字体
		GUI_UC_SetEncodeUTF8();										//设置编码格式
		
		GUI_SetColor(APP_COLOR_YELLOW);									//GUI_WHITE设置文本颜色，黑色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//设置画图模式为正常
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形框控件的背景颜色为黑色
		New_Setting_get_volume_size(msg,num);						//获取回车按键点击次数 字符串
		txt_character = New_Setting_ctrl->new_number_size;			//需要显示的文字字符串获取
		#if 0//手动直接修改矩形大小
		gui_rect.x0 = 0;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;		//矩形x 100 /*416*/
		gui_rect.y0 = 0;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;		//矩形y 80    /*290*/
		gui_rect.x1 = 800;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//矩形宽191
		gui_rect.y1 = 480;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height;  //矩形高20
		#else//获取申请图片资源的矩形大小
		gui_rect.x0 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;		//矩形x 0 /*416*/
		gui_rect.y0 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;		//矩形y 0	/*290*/
		gui_rect.x1 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//矩形宽191
		gui_rect.y1 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height; //矩形高101
		#endif
		{
			number_theme = theme_hdl2buf(New_Setting_ctrl->New_Setting_bmp);			//获取图片使用资源数据
			GUI_BMP_Draw(number_theme, New_Setting_ctrl->New_Settings_uipara->numbers_bmp_pos.x, New_Setting_ctrl->New_Settings_uipara->numbers_bmp_pos.y);//绘制文本控件number:00显示函数矩形背景图和显示坐标，防止number的字符串被覆盖问题
			//GUI_BMP_Draw(number_theme, 0, 0);//绘制文本控件number:00显示函数，防止number的字符串被覆盖问题
	
			__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			GUI_DispStringInRect(txt_character , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);//将文字字符串显示到矩形控件上面，并水平居中
			
			//GUI_DispStringAt(txt_character, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x + 20);
			__wrn("txt_character = %s \n",txt_character);//打印文本显示信息
			__wrn("New_Setting_paint_one_item ok ok \n");
		}
	#endif
	#if 1		//绘制按键矩形 然后在按键矩形框显示绘制文字字符串 ---> 显示中文 "星期六 2023/1/17 16:09"
		GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);		//设置对话框字体
		GUI_UC_SetEncodeUTF8();										//设置编码格式
		
		GUI_SetColor(APP_COLOR_YELLOW);								//GUI_WHITE设置文本颜色，黑色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//设置画图模式为正常
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形框控件的背景颜色为黑色
		#if 1
			gui_bottom_rect.x0 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.x;		//按钮矩形x 0 
			gui_bottom_rect.y0 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.y;		//按钮矩形y 0	
			gui_bottom_rect.x1 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.width;	//按钮矩形宽480
			gui_bottom_rect.y1 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.height; //按钮矩形高37
		#endif
		{
			__wrn("gui_bottom_rect.x0 = %d , gui_bottom_rect.y0 = %d,gui_bottom_rect.width = %d , gui_bottom_rect.heigh = %d\n",
						gui_bottom_rect.x0,gui_bottom_rect.y0,gui_bottom_rect.x1,gui_bottom_rect.y1);
			New_Setting_Bottom_theme = theme_hdl2buf(New_Setting_ctrl->New_Setting_Bottom_bmp);  		//获取按钮图片使用资源数据
			GUI_BMP_Draw(New_Setting_Bottom_theme ,
				New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.x , 
				New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.y);	 				//按钮图片绘制
				{
					__wrn("New_Settings_tiltle_bmp_pos.x = %d , New_Settings_tiltle_bmp_pos.y = \n",
						New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.x,New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.y);
					__wrn("Saturday_txt = %s \n",New_Setting_ctrl->Saturday_txt);						//打印文本显示信息
					GUI_DispStringInRect(New_Setting_ctrl->Saturday_txt, &gui_bottom_rect , GUI_TA_HCENTER | GUI_TA_VERTICAL);//显示星期六
					GUI_DispStringAt(New_Setting_ctrl->Saturday_txt,278,125);//单独显示
					GUI_DispStringAt("2023/1/17 16:09",332,125);//单独显示
				}
		}
		
	#endif
	return EPDK_OK;
}
//绘制所有图片
static __s32 New_Setting_paint_all(__gui_msg_t *msg)
{

	return EPDK_OK;
}
/****************************************************************
*函数功能：app应用framewin窗口的按键处理函数
****************************************************************/
static __s32  New_Setting_key_proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("New_Setting_key key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("New_Setting_key key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_ENTER, 0, 0);//向父窗口app_New_Setting.c文件的manwin窗口发送"case GUI_MSG_COMMAND:	//命令输入"进入到case NEW_SETTING_ID:里面执行功能
			//app_new_setting_cmd2parent(msg->h_deswin, NEW_SETTING_ID, ID_OP_NEW_SETTING_ENTER, 0);
		}
		if(GUI_MSG_KEY_RIGHT == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("New_Setting_key key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//为up
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

/*====================================================================================
*函数作用：图层窗口创建回调函数
*形参：__gui_msg_t *msg：GUI固定必须的结构体指针变量
====================================================================================*/
static __s32 _New_Setting_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	__wrn("_New_Setting_proc msg->id=%d is...\n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//创建窗口
		{
			New_Setting_ctrl_t *New_Setting_ctrl;
			new_setting_para_t *New_Settings_para;
			New_Settings_para = (new_setting_para_t *)GUI_WinGetAttr(msg->h_deswin);		//获取窗口的属性
			New_Setting_ctrl = (New_Setting_ctrl_t *)My_Balloc(sizeof(New_Setting_ctrl_t)); //内存分配
			if(!New_Setting_ctrl)
			{
				__err("memory balloc fail........\n");
				return EPDK_FAIL;
			}
			__wrn("New_Setting GUI_MSG_CREATE 1 is...\n");
			eLIBs_memset(New_Setting_ctrl, 0, sizeof(New_Setting_ctrl_t));			//给结构体number_menu_numbers_ctrl_t内存初始化
			New_Setting_ctrl->focus_txt_color = APP_COLOR_YELLOW;//GUI_YELLOW;//				//选中的文本颜色
			New_Setting_ctrl->unfocus_txt_color = APP_COLOR_WHITE;//GUI_WHITE;//				//未选中的文本颜色
			New_Setting_ctrl->New_Settings_para = New_Settings_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)New_Setting_ctrl);						//添加一个数据
			__wrn("New_Setting GUI_MSG_CREATE 2 is...\n");
			New_Setting_init(New_Setting_ctrl);												//number:00显示提示图片资源申请和窗口显示大小
			GUI_LyrWinSetSta(New_Setting_ctrl->New_Settings_para->layer, GUI_LYRWIN_STA_ON);//窗口图层设置开启状态
			GUI_LyrWinSetTop(New_Setting_ctrl->New_Settings_para->layer);					//窗口图层设置停止状态
			//number_menu_numbers_paint_all(msg,0);											//绘制所有图层
			__wrn("New_Setting GUI_MSG_CREATE 3 is...\n");
			//New_Setting_paint_one_item(msg,0);				//绘制图层												//绘制单个图层
			__wrn("New_Setting GUI_MSG_CREATE 4 is ok\n");
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//关闭窗口，然后返回上一层主窗口界面
		{
			#if 0
			New_Setting_ctrl_t *New_Setting_ctrl;
			New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!New_Setting_ctrl)
			{
				__msg("New_Setting_ctrl_t is null \n");
				return EPDK_FAIL ;
			}
			__wrn("New_Setting_ctrl_t GUI_MSG_CLOSE is...  \n");
			
			//app_new_setting_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_SWITCH_TO_GENERAL, 0, 0);//返回上一层
			//app_new_setting_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_OP_EXIT, 0, 0);//直接退出
			//app_new_setting_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, NULL);//直接进入ID_HOME_NEW_SETTING_NUMBER,然后进入到新添加的通用设置界面
			#endif
			#if 0//直接释放图片资源
			New_Setting_ctrl_t *New_Setting_ctrl;
			New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//获取这个结构体的数据
			New_Setting_uninit(New_Setting_ctrl);//释放图片资源
			if(New_Setting_ctrl)//释放结构体内存
			{
				__msg("New_Setting_ctrl_t win free New_Setting_ctrl_t.....\n");
				My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
				New_Setting_ctrl = NULL ;
			}
			#endif
			#if 1//向app_New_Setting.c父窗口manwin窗口的case NEW_SETTING_ID:发送退出信号
			__wrn("New_Setting_ctrl_t APP_NEWSETTING_ID is... ok\n");
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_EXIT, 0, 0);
			//app_new_setting_cmd2parent(msg->h_deswin, APP_NEWSETTING_ID, GUI_MSG_CLOSE, 0);//向app_New_Setting.c父窗口manwin发送关闭信号
			#endif
			__wrn("New_Setting_ctrl_t GUI_MSG_CLOSE is... ok\n");
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //销毁窗口
		{
			#if 0	//方法1，释放图片使用资源
				New_Setting_ctrl_t *New_Setting_ctrl;
				New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//获取这个结构体的数据
				New_Setting_uninit(New_Setting_ctrl);//释放图片资源
				if(New_Setting_ctrl->New_Settings_para)//释放图层
				{
					__wrn("New_Setting_ctrl win free New_Settings_para.....\n");
					My_Bfree(New_Setting_ctrl->New_Settings_para, sizeof(new_setting_para_t));
					New_Setting_ctrl->New_Settings_para = NULL ;
				}

				if(New_Setting_ctrl)
				{
					__wrn("New_Setting_ctrl win free New_Setting_ctrl.....\n");
					My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
					New_Setting_ctrl = NULL ;
				}
			#endif
			#if 1	//方法2，释放图片资源
				New_Setting_ctrl_t *New_Setting_ctrl;
				New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//获取这个结构体的数据

				if(!New_Setting_ctrl)//释放结构体内存
				{
					__err("New_Setting_ctrl is null........\n");
					return EPDK_FAIL;
				}
				

				__wrn("New_Setting_ctrl win destroy...\n");
				theme_close(New_Setting_ctrl->New_Setting_bmp);	//释放矩形方框图片资源
				__wrn("New_Setting_ctrl win close theme...\n");
				theme_close(New_Setting_ctrl->New_Setting_Bottom_bmp);	//释放按钮图片资源
				__wrn("New_Setting_ctrl win close theme...\n");
				theme_close(New_Setting_ctrl->New_Setting_f_bmp);//释放确认图片资源
				__wrn("New_Setting_ctrl win close theme ok\n");

				if(New_Setting_ctrl->New_Settings_para)//释放图层
				{
					__wrn("New_Setting_ctrl win free New_Settings_para.....\n");
					My_Bfree(New_Setting_ctrl->New_Settings_para, sizeof(new_setting_para_t));
					New_Setting_ctrl->New_Settings_para = NULL ;
				}

				if(New_Setting_ctrl)
				{
					__wrn("New_Setting_ctrl win free New_Setting_ctrl.....\n");
					My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
					New_Setting_ctrl = NULL ;
				}
			#endif
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//绘制显示所有图片内容
			__wrn("one  paint new Setting \n");
			New_Setting_paint_one_item(msg,0);
		return EPDK_OK;

		case GUI_MSG_KEY:	//按键处理函数
		__wrn("GUI_MSG_KEY new Setting \n");
			New_Setting_key_proc(msg);
		return EPDK_OK;

		case MSG_NEW_SETTING_OP_ENTER:	//回车确认按键
		{
			__gui_msg_t mymsg;
			key_number += 1;
			__wrn("new setkey_number start =%d  \n",key_number);
			#if 1	
			if(key_number == 5)//删除app应用，然后返回上一层app_New_Setting.c的manwin窗口显示全部app应用
			{
				key_number = 0;
				#if 1
				mymsg.id = GUI_MSG_CLOSE;		//framewin窗口的关闭窗口id
				mymsg.h_deswin = msg->h_deswin;	//在当前图层发送按键信息
				mymsg.h_srcwin = msg->h_srcwin; //在当前图层发送按键信息
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
				__wrn("new setkey_number stop =%d  \n",key_number);
				New_Setting_paint_one_item(msg,key_number);//刷新number次数显示内容
			}
			#endif
		}
		return EPDK_OK;
			
		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

/*====================================================================================
*函数作用：新的app应用framewin窗口创建函数
*形参：H_WIN h_parent：窗口定义 ；__gui_msg_t *msg：GUI固定必须的结构体指针变量
====================================================================================*/
H_WIN New_Setting_app_win_create(H_WIN h_parent, new_setting_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;		//定义一个 framewin窗口的创建 结构体变量
	new_setting_para_t *New_Setting_para;
	FB fb;
	GUI_LyrWinGetFB(para->layer, &fb);				//图层窗口获取FB
	New_Setting_para = (new_setting_para_t *)My_Balloc(sizeof(new_setting_para_t));	//给结构体指针变量分配内存空间

	if(!New_Setting_para)	//判断结构体是否分配内存成功
	{
		__err("memory balloc fail........\n");
		return EPDK_FAIL;
	}

	New_Setting_para->font = para->font;									//获取消息对话框文本
	New_Setting_para->layer = para->layer;									//图层
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));	//初始化结构体，清0
	framewin_para.name =	"New_Setting_win";								//窗口名称
	framewin_para.dwExStyle = WS_EX_NONE;									//窗口扩展风格为无
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;							//framewin子窗口的样式为可见或无
	framewin_para.spCaption =  NULL;										//framewin子窗口的控件标题
	framewin_para.hOwner	= NULL;											//framewin子窗口的拥有者句柄
	framewin_para.hHosting = h_parent;										//framewin的主窗口句柄
	framewin_para.id         = NEW_SETTING_ID;								//framewin的窗口ID号
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_New_Setting_proc);	//framewin的消息处理过程，回调函数处理
	framewin_para.rect.x = 0;												//framewin在图层窗口中的位置x
	framewin_para.rect.y = 0;												//framewin在图层窗口中的位置y
	framewin_para.rect.width =  fb.size.width;								//framewin在图层窗口中的位置宽1024
	framewin_para.rect.height =  fb.size.height;							//framewin在图层窗口中的位置高600
	framewin_para.BkColor.alpha =  0;										//framewin的背景色 32-bit (ARGB) color  
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)New_Setting_para;							//framewin的窗口私有属性信息
	framewin_para.hLayer = para->layer;										//framewin的图层窗口
	return (GUI_FrmWinCreate(&framewin_para));								//返回一个framewin创建窗口
}
/*====================================================================================
*函数作用：窗口删除
====================================================================================*/
void  new_setting_app_win_delete(H_WIN New_Setting_win)
{
	GUI_FrmWinDelete(New_Setting_win);
}

