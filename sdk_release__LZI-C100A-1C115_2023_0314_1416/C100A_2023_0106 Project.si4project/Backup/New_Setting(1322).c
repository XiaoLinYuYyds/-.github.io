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
*回车按键次数获取函数:number:00
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
/****************************************************
*时间计时次数获取函数：time
****************************************************/
void New_Setting_get_Time_size(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str数组内存空间初始化
	eLIBs_strcat(str, "time:");
	snum = num;
	eLIBs_uint2str_dec(snum, numstr);//int型转char型
	 
	if(snum >= 10)
	{
		eLIBs_strcat(str, numstr);
	}
	else if(snum >59)//1分钟计时结束
	{
		eLIBs_strcat(str, "0");//字符串存到str数组
		eLIBs_strcat(str, "0");
	}
	else
	{
		eLIBs_strcat(str, "0");
		eLIBs_strcat(str, numstr);
	}

	eLIBs_strcpy(New_Setting_ctrl->time_size, (const char *)str);//将str数组内容拷贝到结构体的time_size字符型数组


}
/***********************************************************
*函数作用：时间转字符串
参	   数：char *buff：传入一个char型数组，
		   __time_t time：esTIME_GetTime()函数获取到的时间
描	   述：把获取到的时间存放到数组
***********************************************************/
static void new_time_to_char(char *buff, __time_t time)
{
	__wrn("time: %d:%d:%d \n",time.hour,time.minute,time.second);
	buff[0] = time.hour / 10 + '0';//十位的显示，+'0'是把十进制转换成ascii码
	buff[1] = time.hour % 10 + '0';//个位的显示
	buff[2] = ':';
	buff[3] = time.minute / 10 + '0';//十位的显示
	buff[4] = time.minute % 10 + '0';//个位的显示
	//buff[5] = '\0';
	buff[5] = ':';

	buff[6] = time.second/10 + '0';//十位的显示
	buff[7] = time.second%10 + '0';//个位的显示

	buff[8] = '\0';//反斜杠0为结束
}
/***********************************************************
*日期转字符串
***********************************************************/
static void new_date_to_char(char *buff, __date_t date)
{
	__wrn("date: %d/%d/%d \n",date.year,date.month,date.day);
		buff[0] = date.year/1000 + '0';
		buff[1] = (date.year%1000)/100 + '0';
		buff[2] = ((date.year%1000)%100)/10 + '0';
		buff[3] = (date.year%1000)%100%10 + '0';
	
		buff[4] = '-';
		buff[5] = date.month/10 + '0';
		buff[6] = date.month%10 + '0';
	
		buff[7] = '-';
		buff[8] = date.day/10 + '0';
		buff[9] = date.day%10 + '0';
	//buff[0] = date.month / 10 + '0';
	//buff[1] = date.month % 10 + '0';
	//buff[2] = '-';
	//buff[3] = date.day / 10 + '0';
	//buff[4] = date.day % 10 + '0';
	//buff[5] = '\0';
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
	new_setting_time_para_t *time_para;//存放时间和日期结构体
	new_setting_time_data_t set_time_para;//存放时间和日期结构体
	GUI_RECT gui_rect;		//存放GUI矩形控件变量
	GUI_RECT gui_bottom_rect;		//存放GUI按钮矩形控件变量
	char	 *txt_character;//存放文字的字符串：
	void  	*number_theme;	//存放获取的图片使用资源数据
	void	*New_Setting_Bottom_theme;//存放获取的按键图片使用资源数据
	__s32  sys_time = 10;		  //存放获取的系统时间
	static __s32  minute_count = 0;		  //时间分的计数
	static __s32  hour_count = 0;		  	  //时间时的计数

	GUI_MEMDEV_Handle draw_mem = NULL;//存放伪内存	
	__s32 screen_width, screen_height;//获取lcd显示屏的宽和高
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//获取添加一个数据到msg结构体
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
	#if 0//创建伪内存，使得绘制更快

		/* get lcd size*/
		dsk_display_get_size(&screen_width, &screen_height);
		draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height);
		__wrn("draw_mem=0x%x\n", draw_mem);

		
		if(draw_mem)//选择伪内存
		{
			GUI_MEMDEV_Select(draw_mem);
		}
		if(draw_mem)//伪内存删除
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
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
		#else//获取New_Setting_para.c文件设置的矩形大小和位置
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
	//while(sys_time--)
	{
	#if 1		//绘制按键矩形 然后在按键矩形框显示绘制文字字符串 ---> 显示中文 "星期六 2023/1/17 16:09"
		GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);		//设置对话框字体
		GUI_UC_SetEncodeUTF8();										//设置编码格式
		
		GUI_SetColor(APP_COLOR_YELLOW);								//GUI_WHITE设置文本颜色，黑色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//设置画图模式为正常
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形框控件的背景颜色为黑色
		#if 1
			gui_bottom_rect.x0 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.x;		//按钮矩形x 272 
			gui_bottom_rect.y0 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.y;		//按钮矩形y 120	
			gui_bottom_rect.x1 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.width;	//按钮矩形宽480
			gui_bottom_rect.y1 = 480;//New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.height; //按钮矩形高37
		#endif
		{
			__wrn("gui_bottom_rect.x0 = %d , gui_bottom_rect.y0 = %d,gui_bottom_rect.width = %d , gui_bottom_rect.heigh = %d\n",
						gui_bottom_rect.x0,gui_bottom_rect.y0,gui_bottom_rect.x1,gui_bottom_rect.y1);
			New_Setting_Bottom_theme = theme_hdl2buf(New_Setting_ctrl->New_Setting_Bottom_bmp);  		//获取按钮图片使用资源数据
			GUI_BMP_Draw(New_Setting_Bottom_theme ,
				New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.x , 
				New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.y);	 				//按钮图片绘制
				{
					__wrn("New_Settings_tiltle_bmp_pos.x = %d , New_Settings_tiltle_bmp_pos.y = %d\n",
						New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.x,New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.y);
					__wrn("Saturday_txt 0 is... = %s \n",New_Setting_ctrl->Saturday_txt);						//打印文本显示信息
					GUI_DispStringInRect(New_Setting_ctrl->Saturday_txt, &gui_bottom_rect , GUI_TA_LEFT | GUI_TA_TOP);//显示星期六
					
					//GUI_DispStringInRect(txt_character, &gui_bottom_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);//显示星期六
					__wrn("txt_character 1 is... = %s \n",txt_character);
					__wrn("Saturday_txt 1 is... = %s \n",New_Setting_ctrl->Saturday_txt);
					//GUI_DispStringAt(New_Setting_ctrl->Saturday_txt,278,125);//单独显示
					//GUI_DispStringAt("2023/1/10 10:09",332,125);//单独显示
					GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形框控件的背景颜色为黑色
					#if 1	//获取系统时间
						__wrn("sys_time 1 = %d \n",sys_time);
						#if 0//结构体1 set_time_para 正常结构体操作 设置时间和日期

							set_time_para.time.hour = 9;
							set_time_para.time.minute = 59;
							set_time_para.time.second = 24;
							esTIME_SetTime(&(set_time_para.time));						// 重新设置时间
							__wrn("0 set_time_para.time = %d \n",set_time_para.time);
							set_time_para.date.year = 2023;
							set_time_para.date.month = 01;
							set_time_para.date.day = 10;
							esTIME_SetDate(&(set_time_para.date));						// 重新设置日期
							__wrn("0 set_time_para.date = %d \n",set_time_para.date);
						#endif
						#if 0//结构体2 *time_para 结构体指针操作 设置时间和日期

							time_para->data.time.hour = 10;
							time_para->data.time.minute = 52;
							time_para->data.time.second = 24;
							esTIME_SetTime(&(time_para->data.time));						// 重新设置时间
							__wrn("0 time_para->data.time = %d \n",time_para->data.time);
							time_para->data.date.year = 2023;
							time_para->data.date.month = 01;
							time_para->data.date.day = 10;
							esTIME_SetDate(&(set_time_para.date));						// 重新设置日期
							__wrn("0 time_para->data.date = %d \n",time_para->data.date);
						#endif
						#if 0//结构体3 New_Setting_ctrl->set_time_para 结构体指针调用结构体指针操作写法 设置时间和日期 只需要设置一次
							New_Setting_ctrl->set_time_para->data.time.hour = 9;
							New_Setting_ctrl->set_time_para->data.time.minute = 58;
							New_Setting_ctrl->set_time_para->data.time.second = 24;
							esTIME_SetTime(&(New_Setting_ctrl->set_time_para->data.time));						// 重新设置时间
							__wrn("New_Setting_ctrl->set_time_para.data.time is... = %d \n",New_Setting_ctrl->set_time_para->data.time);
							__wrn("set time OK set_time_para->data.time.hour = %d:%d:%d \n",New_Setting_ctrl->set_time_para->data.time.hour,New_Setting_ctrl->set_time_para->data.time.minute,New_Setting_ctrl->set_time_para->data.time.second);
							New_Setting_ctrl->set_time_para->data.date.year = 2023;
							New_Setting_ctrl->set_time_para->data.date.month = 01;
							New_Setting_ctrl->set_time_para->data.date.day = 10;
							esTIME_SetDate(&(New_Setting_ctrl->set_time_para->data.date));						// 重新设置日期
							__wrn("New_Setting_ctrl->set_time_para->data.date is... = %d \n",New_Setting_ctrl->set_time_para->data.date);
							__wrn("set date OK set_time_para->data.date.year = %d-%d-%d \n",New_Setting_ctrl->set_time_para->data.date.year,New_Setting_ctrl->set_time_para->data.date.month,New_Setting_ctrl->set_time_para->data.date.day);
						#endif
						//while(sys_time--)
						{
						//sys_time = esTIME_GetDate(&(time_para->data.date));
						__wrn("sys_time 2 = %d \n",sys_time);
						#if 0//时间运行计算
							if(sys_time == 0)//s等于0的时候
							{
								
								minute_count += 1;
								//if(minute_count == 9)
								{
									hour_count += 1;
									if(New_Setting_ctrl->set_time_para->data.time.minute >= 59)//60分，进入清0
									{
										New_Setting_ctrl->set_time_para->data.time.hour += 1;//小时加1
										New_Setting_ctrl->set_time_para->data.time.minute = 0;//分清0
										New_Setting_ctrl->set_time_para->data.time.second = sys_time;//秒清0
									}
									else if(New_Setting_ctrl->set_time_para->data.time.hour > 23)
									{
										New_Setting_ctrl->set_time_para->data.time.hour = 0;
										New_Setting_ctrl->set_time_para->data.time.minute = 0;
										New_Setting_ctrl->set_time_para->data.time.second = sys_time;//秒清0
									}
									else
									{
										New_Setting_ctrl->set_time_para->data.time.minute += 1;
										New_Setting_ctrl->set_time_para->data.time.second = sys_time;

									}
									if(hour_count > 10)
									{
										hour_count = 0;
									}
									minute_count = 0;
								}
															
							}
							else
							{
								//New_Setting_ctrl->set_time_para->data.time.hour += hour_count;//时
								//New_Setting_ctrl->set_time_para->data.time.minute += minute_count;//分
								New_Setting_ctrl->set_time_para->data.time.second += 1;//秒
							}
							esTIME_SetTime(&(New_Setting_ctrl->set_time_para->data.time));//重新设置时间
						#endif
						#if 0//结构体2 *time_para结构体指针操作 获取时间和日期
							__wrn("get time start time_para->data.time.hour = %d:%d:%d \n",time_para->data.time.hour,time_para->data.time.minute,time_para->data.time.second);
							__wrn("get date start time_para->data.date.year = %d:%d:%d \n",time_para->data.date.year,time_para->data.date.month,time_para->data.date.day);
							esTIME_GetTime(&(time_para->data.time));						// 时间
							__wrn("time_para->data.time = %d \n",time_para->data.time);
							eLIBs_memset(time_para->time_buf, 0, 32);
							new_time_to_char(time_para->time_buf, time_para->data.time);		//时间 赋值到 time_para->time_buf数组
							__wrn("time_para->time_buf = %s \n",time_para->time_buf);
							GUI_DispStringAt(time_para->time_buf,480,125);//单独显示时间
							
							esTIME_GetDate(&(time_para->data.date));						// 日期
							__wrn("time_para->data.date = %d \n",time_para->data.date);
							eLIBs_memset(time_para->date_buf, 0, 32);
							new_date_to_char(time_para->date_buf, time_para->data.date);		//日期 赋值到 time_para->data_buf数组
							__wrn("time_para->date_buf = %s \n",time_para->date_buf);
							GUI_DispStringAt(time_para->date_buf,680,125);//单独显示日期
						#endif
						#if 0//结构体1 set_time_para正常结构体调用操作 获取时间和日期
							__wrn("get time start set_time_para.time.hour = %d:%d:%d \n",set_time_para.time.hour,set_time_para.time.minute,set_time_para.time.second);
							__wrn("get date start set_time_para.date.year = %d:%d:%d \n",set_time_para.date.year,set_time_para.date.month,set_time_para.date.day);
							esTIME_GetTime(&(set_time_para.time));						// 时间
							__wrn("set_time_para.time = %d \n",set_time_para.time);
							eLIBs_memset(time_para->time_buf, 0, 32);
							new_time_to_char(time_para->time_buf, set_time_para.time);		//时间 赋值到 time_para->time_buf数组
							__wrn("time_para->time_buf = %s \n",time_para->time_buf);
							GUI_DispStringAt(time_para->time_buf,480,125);//单独显示时间
							
							esTIME_GetDate(&(set_time_para.date));						// 日期
							__wrn("set_time_para.date = %d \n",set_time_para.date);
							eLIBs_memset(time_para->date_buf, 0, 32);
							new_date_to_char(time_para->date_buf, set_time_para.date);		//日期 赋值到 time_para->data_buf数组
							__wrn("time_para->date_buf = %s \n",time_para->date_buf);
							GUI_DispStringAt(time_para->date_buf,680,125);//单独显示日期
						#endif
						#if 1//结构体3 New_Setting_ctrl->set_time_para 结构体指针调用结构体指针操作写法 获取时间和日期
							__wrn("get time start New_Setting_ctrl->set_time_para->data.time.hour = %d:%d:%d \n",
								New_Setting_ctrl->set_time_para->data.time.hour,New_Setting_ctrl->set_time_para->data.time.minute,New_Setting_ctrl->set_time_para->data.time.second);
							__wrn("get date start New_Setting_ctrl->set_time_para->data.date.year = %d:%d:%d \n",
								New_Setting_ctrl->set_time_para->data.date.year,New_Setting_ctrl->set_time_para->data.date.month,New_Setting_ctrl->set_time_para->data.date.day);
							esTIME_GetTime(&(New_Setting_ctrl->set_time_para->data.time));						// 时间
							__wrn("New_Setting_ctrl->set_time_para->data.time = %d \n",New_Setting_ctrl->set_time_para->data.time);
							eLIBs_memset(time_para->time_buf, 0, 32);
							new_time_to_char(time_para->time_buf, New_Setting_ctrl->set_time_para->data.time);		//时间 赋值到 time_para->time_buf数组
							__wrn("time_para->time_buf = %s \n",time_para->time_buf);
							//GUI_DispStringAt(time_para->time_buf,480,125);//单独显示时间
							GUI_DispStringInRect(time_para->time_buf , &gui_bottom_rect , GUI_TA_RIGHT | GUI_TA_TOP);
							
							esTIME_GetDate(&(New_Setting_ctrl->set_time_para->data.date));						// 日期
							__wrn("set_time_para->data.date = %d \n",New_Setting_ctrl->set_time_para->data.date);
							eLIBs_memset(time_para->date_buf, 0, 32);
							new_date_to_char(time_para->date_buf, New_Setting_ctrl->set_time_para->data.date);		//日期 赋值到 time_para->data_buf数组
							__wrn("time_para->date_buf = %s \n",time_para->date_buf);
							//GUI_DispStringAt(time_para->date_buf,680,125);//单独显示日期
							GUI_DispStringInRect(time_para->date_buf , &gui_bottom_rect , GUI_TA_CENTER | GUI_TA_TOP);	//GUI_TA_VCENTER
							
						#endif
						esKRNL_TimeDly(20);		// 10 * 5ms = 50ms
						}
					#endif
				}
		}
		
	#endif
	}
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
	new_setting_time_para_t *set_time_para;//存放时间和日期结构体
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
			#if 1//New_Setting_ctrl->set_time_para 结构体指针调用结构体指针操作写法 设置时间和日期 只需要设置一次
				New_Setting_ctrl->set_time_para->data.time.hour = 9;
				New_Setting_ctrl->set_time_para->data.time.minute = 58;
				New_Setting_ctrl->set_time_para->data.time.second = 24;
				esTIME_SetTime(&(New_Setting_ctrl->set_time_para->data.time));						// 重新设置时间
				__wrn("New_Setting_ctrl->set_time_para.data.time is... = %d \n",New_Setting_ctrl->set_time_para->data.time);
				__wrn("set time OK set_time_para->data.time.hour = %d:%d:%d \n",New_Setting_ctrl->set_time_para->data.time.hour,New_Setting_ctrl->set_time_para->data.time.minute,New_Setting_ctrl->set_time_para->data.time.second);
				New_Setting_ctrl->set_time_para->data.date.year = 2023;
				New_Setting_ctrl->set_time_para->data.date.month = 01;
				New_Setting_ctrl->set_time_para->data.date.day = 10;
				esTIME_SetDate(&(New_Setting_ctrl->set_time_para->data.date));						// 重新设置日期
				__wrn("New_Setting_ctrl->set_time_para->data.date is... = %d \n",New_Setting_ctrl->set_time_para->data.date);
				__wrn("set date OK set_time_para->data.date.year = %d-%d-%d \n",New_Setting_ctrl->set_time_para->data.date.year,New_Setting_ctrl->set_time_para->data.date.month,New_Setting_ctrl->set_time_para->data.date.day);
			#endif
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
				//app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_EXIT, 0, 0);//发送新添加的app应用退出ID消息
				app_new_setting_cmd2parent(msg->h_deswin, ID_NEW_SETTING_SWITCH_TO_SETTING, 0, 0);//发送新添加的app应用跳转到通用设置app应用程序消息;两个app应用程序manwin窗口之间切换跳转
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
	__wrn("esKSRV_SysInfo   is...\n");				//打印内存是否泄露
	esKSRV_SysInfo();								//查看系统内存消息是否泄露或溢出
	//esMEMS_FreeMemSize();							//剩余内存大小以KB为单位
	//esMEMS_TotalMemSize();						//总的内存大小
	__wrn("esKSRV_SysInfo | esMEMS_FreeMemSize = %d \n",esMEMS_FreeMemSize());//打印剩余内存
	__wrn("esKSRV_SysInfo | esMEMS_TotalMemSize = %d \n",esMEMS_TotalMemSize());//打印总内存
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

