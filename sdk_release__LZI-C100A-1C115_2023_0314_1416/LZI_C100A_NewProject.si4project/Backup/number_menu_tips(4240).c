#include "number_menu_tips.h"
#include "number_menu_para.h"	//app应用图片显示坐标大小
#include "string.h"
#include "number_menu_app_ui.h"//app应用显示图层绘制
#include "typedef.h"
#include "number_menu_i.h"

//new 子菜单
#include "New_Setting.h"
#include "New_Setting_para.h"

#include "app_home_i.h"
#include "home_uipara.h"
#include "beetles_app.h"


typedef struct number_menu_numbers_ctrl
{
	number_menu_number_para_t *numbers_para;

	__s32 focus_txt_color;		//选中的txt文本颜色
	__s32 unfocus_txt_color;	//未选中的txt文本颜色
	H_THEME number_bmp;		//选中的主题，number:00次数显示图片资源
	H_THEME h_number_f_bmp;	//未选中的主题，暂时不需要
	H_THEME app_focus_bmp[APP_MAX_NUM];			//选中的app应用图片主题
	H_THEME app_unfocus_bmp[APP_MAX_NUM];		//未选中的app应用图片主题

	char number_ok_txt[128];		//string ok
	char number_tsize[128];			// size
	char number_fsize[128];			// size
	char number_svers[128];			// 
	char number_total_size[128];
	char number_free_size[128];
	char number_title_txt[128];
	numbers_uipara_t *numbers_uipara;
}
number_menu_numbers_ctrl_t;
typedef struct tag_home1_para	//用于调用子窗口
{
	H_WIN	h_app_main;
	H_WIN 	h_mmenu;
	H_WIN 	h_submenu;
	H_WIN 	h_newsubmenu;

	H_WIN	lyr_mmenu;
	H_WIN	lyr_forground;
	H_WIN	lyr_smenu;

	__s32	focus_id;
	__s32   first_item;
	GUI_FONT *main_font;

	__s32   focus_smenu_item;
	void		*core;
	__u32	root_type;
}
home1_para_t;

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
/*****************************************************************************
*获取回车按键点击数值字符串显示
*****************************************************************************/
void number_menu_numbers_get_volume_size(__gui_msg_t *msg,__u32 num)
{
	number_menu_numbers_ctrl_t *numbers_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str数组内存空间初始化
	//eLIBs_strcat(str, numbers_ctrl->number_tsize);
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
	eLIBs_strcpy(numbers_ctrl->number_total_size, (const char *)str);	//把字符串拷贝到numbers_ctrl结构体变量
	__wrn("number_menu_numbers_get_volume_size CREATE\n");
}
/**********************************************************************************
*初始化，申请使用图片资源
**********************************************************************************/
static void number_menu_number_init(number_menu_numbers_ctrl_t *numbers_ctrl)
{
	SIZE screen_size;
	__s32 i;
	number_res_id_t *number_ui_para;
	numbermenu_attr_t *numbermenu_attr;
	dsk_display_get_size(&screen_size.width, &screen_size.height);		//获取LCD屏幕显示尺寸字节大小
	numbers_ctrl->numbers_uipara = number_get_numbers_para(screen_size);//获取对话框矩形大小，(自定义的ui控件（文本控件，图片，按钮控件等）窗口坐标位置)
	//number_ui_para = number_get_ui_para(screen_size);//获取app应用传入图片id索引数据
	number_ui_para = number_get_app_ui_para(screen_size);//获取app应用传入图片id索引数据
	if(!number_ui_para)		//传入失败
	{
		__err("main_ui_para is null...\n");
		return;
	}
	if(!numbers_ctrl->number_bmp)//用于number:00次数显示图片申请资源
	{
		numbers_ctrl->number_bmp = theme_open(ID_SETTING_SET_INFO_BMP);//直接传入图片ID，获取其句柄
	}
	#if 0 //number次数图片，直接传入图片ID索引
		if(!numbers_ctrl->number_bmp)
		{
			numbers_ctrl->number_bmp = theme_open(ID_SETTING_SET_INFO_BMP);//直接传入图片ID，获取其句柄
		}

		if(!numbers_ctrl->number_f_bmp)
		{
			numbers_ctrl->number_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);//直接传入图片ID，获取其句柄
		}
	#else	//app应用图片通过结构体方式传入图片ID索引
		#if 1  //全部app应用图片id索引资源申请
			for(i = 0; i<APP_MAX_NUM;i++)	//申请8次，8个app应用图片
			{
				if(!numbers_ctrl->app_focus_bmp[i])
				{
					//numbers_ctrl->number_bmp = theme_open(ID_HOME_MAIN_MUSIC_LOSE_BMP);//直接传入图片ID，获取其句柄
					numbers_ctrl->app_focus_bmp[i] = dsk_theme_open(number_ui_para[i].app_focus_bmp_id);//通过结构体传入app应用图片ID
				}

				if(!numbers_ctrl->app_unfocus_bmp[i])
				{
					//numbers_ctrl->number_f_bmp = theme_open(ID_HOME_MAIN_MUSIC_FOCUS_BMP);//直接传入图片ID，获取其句柄
					numbers_ctrl->app_unfocus_bmp[i] = dsk_theme_open(number_ui_para[i].app_unfocus_bmp_id);//通过结构体传入app应用图片ID
				}
				__wrn("number_menu_number_init [%d] CREATE\n",i);
			}
		#else	//单个app应用图片ID索引资源申请
			{
				if(!numbers_ctrl->app_focus_bmp[0])
				{
					//numbers_ctrl->number_bmp = theme_open(ID_HOME_MAIN_MUSIC_LOSE_BMP);//直接传入图片ID，获取其句柄
					numbers_ctrl->app_focus_bmp[0] = dsk_theme_open(number_ui_para[0].app_focus_bmp_id);//通过结构体传入app应用图片ID
				}
			
				if(!numbers_ctrl->app_unfocus_bmp[0])
				{
					//numbers_ctrl->number_f_bmp = theme_open(ID_HOME_MAIN_MUSIC_FOCUS_BMP);//直接传入图片ID，获取其句柄
					numbers_ctrl->app_unfocus_bmp[0] = dsk_theme_open(number_ui_para[0].app_unfocus_bmp_id);//通过结构体传入app应用图片ID
				}
			}
		#endif
	
	#endif 
	__wrn("number_menu_number_init CREATE\n");
	__wrn("number_menu_number_init CREATE\n");
	//文本菜单
	//get_menu_text(STRING_SET_OK, numbers_ctrl->number_ok_txt, 128);

}
/**********************************************************************************
*释放申请的使用图片资源
**********************************************************************************/
static void number_menu_number_uninit(number_menu_numbers_ctrl_t *numbers_ctrl)
{
	__u32 	i;

	for(i = 0; i < APP_MAX_NUM; i++)
	{
		if(numbers_ctrl->app_unfocus_bmp[i])
		{
			dsk_theme_close(numbers_ctrl->app_unfocus_bmp[i]);
			numbers_ctrl->app_unfocus_bmp[i] = NULL;
		}

		if(numbers_ctrl->app_focus_bmp[i])
		{
			dsk_theme_close(numbers_ctrl->app_focus_bmp[i]);
			numbers_ctrl->app_focus_bmp[i]  = NULL;
		}
	}

	return;
}
/*****************************************************************************
*函数作用：绘制单个app应用图层
*形参：__gui_msg_t *msg：GUI固定结构体指针变量；
	   __s32 index：app应用图片显示索引号；
	   __s32 focus_flage：app应用选中和未选中的标志位
*****************************************************************************/
static __s32 app_menu_paint_one(__gui_msg_t *msg,__u32 num,__s32 index,__s32 focus_flage)
{
	number_menu_numbers_ctrl_t *numbers_ctrl;
	numbermenu_attr_t *numbermenu_attr;
	number_res_id_t *app_ui_id;//app应用的文字和图片索引ID结构体
	void  	*app_theme;//app应用图片矩形控件主题
	__s32    app_bmp_x;//app应用图片矩形控件显示坐标x轴
	__s32    app_bmp_y;//app应用图片矩形控件显示坐标y轴
	char     app_txt[256];  //app应用的文字字符串
	GUI_RECT gui2_rect;	//画矩形,存放app应用图片
	__u16 	 app_focus_x, app_focus_y;
	__s32 	 i=0;
	
	numbermenu_attr = (numbermenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);	  //结构体里面添加一个存放app应用图片绘制数据
	app_ui_id = number_get_ui_para(GUI_GetScnDir());			//获取app应用传入图片和文字的id索引数据
	
	numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//结构体里面添加一个数据,作用是确保下面的数据能正常访问
	
		if(!numbers_ctrl)
		{
			__msg(" is null \n ");
			return EPDK_FAIL ;
		}
		__wrn("app_menu_paint_one CREATE\n");
		GUI_LyrWinSel(numbers_ctrl->numbers_para->layer);			//先选择图层窗口事件，选择窗口图层然后在图层上面绘制，numbers_ctrl->numbers_para->layer
		GUI_SetFont(numbers_ctrl->numbers_para->msgbox_font);		//设置对话框字体
		GUI_UC_SetEncodeUTF8();										//设置编码格式
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//设置画图模式为正常
#if  1		//绘制矩形	然后在矩形控件里面绘制单个或全部app应用图片
	//for(i = 0; i < APP_MAX_NUM;i++)
	{
		__wrn("GUI_SetColor CREATE\n");
	#if 1	//app应用图片选中的x,y坐标
			app_focus_x = (APP_FOCUS_W -APP_W)/2;						//(选中的app应用图片宽190 - 没有选中的app应用图片宽150)/2
			app_focus_y = (APP_FOCUS_H -APP_H)/2;						//(选中的app应用图片高190 - 没有选中的app应用图片高150)/2
	#endif
	#if 0	//单个app应用图片显示位置
			app_bmp_x = numbers_ctrl->numbers_uipara->numbers_app_pos[0].x;//先获取一个app图片显示的x轴坐标
			app_bmp_y = numbers_ctrl->numbers_uipara->numbers_app_pos[0].y;//先获取一个app图片显示的y轴坐标	
	#else	//所有app应用图片显示位置 APP_MAX_NUM共8个
			app_bmp_x = numbers_ctrl->numbers_uipara->numbers_app_pos[index].x;//先获取一个app图片显示的x轴坐标
			app_bmp_y = numbers_ctrl->numbers_uipara->numbers_app_pos[index].y;//先获取一个app图片显示的y轴坐标	
	#endif
		
		__wrn("app_bmp_x = %d , app_bmp_y = %d\n",app_bmp_x,app_bmp_y);
		__wrn("numbers_ctrl->numbers_uipara->numbers_app_pos[1].x = %d , numbers_ctrl->numbers_uipara->numbers_app_pos[1].y = %d\n",numbers_ctrl->numbers_uipara->numbers_app_pos[1].x,numbers_ctrl->numbers_uipara->numbers_app_pos[1].y);
		
		__wrn("numbers_ctrl->numbers_uipara->numbers_app_pos[0].w = %d , numbers_ctrl->numbers_uipara->numbers_app_pos[0].h = %d\n",numbers_ctrl->numbers_uipara->numbers_app_pos[0].width,numbers_ctrl->numbers_uipara->numbers_app_pos[0].height);
		__wrn("numbers_ctrl->numbers_uipara->lyr_numbers_menu.x = %d , numbers_ctrl->numbers_uipara->lyr_numbers_menu.y = %d\n",numbers_ctrl->numbers_uipara->lyr_numbers_menu.x,numbers_ctrl->numbers_uipara->lyr_numbers_menu.y);
		
	#if 1	//按照app应用图片大小 绘制矩形的大小
			gui2_rect.x0 = app_bmp_x-16;						 //矩形控件的x0坐标	第一个app应用矩形x坐标：84.8-16
			gui2_rect.y0 = app_bmp_y +APP_H +24;				 //矩形控件的y0坐标	第一个app应用矩形y坐标：103+150+24
			gui2_rect.x1 = gui2_rect.x0 +APP_W -1 +32;			 //矩形控件的宽			第一个app应用矩形宽坐标：68+150-1+32	
			gui2_rect.y1 = gui2_rect.y0+24 -1;					 //矩形控件的高			第一个app应用矩形高坐标：177+24-1
	#else	//直接绘制矩形大小，自定义大小
			gui_rect.x0 = 200+numbers_ctrl->numbers_uipara->numbers_tiltle_pos.x;		//矩形x
			gui_rect.y0 = 200+numbers_ctrl->numbers_uipara->numbers_tiltle_pos.y;		//矩形y
			gui_rect.x1 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.width;	//矩形宽150
			gui_rect.y1 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.height;  //矩形高150
			{
				__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			}
	#endif
		__wrn("111 gui2_rect.x0 = %d , gui2_rect.y0 = %d,gui2_rect.width = %d , gui2_rect.heigh = %d\n",gui2_rect.x0,gui2_rect.y0,gui2_rect.x1,gui2_rect.y1);
	#if 0	//直接传入图片ID的句柄
			//app_theme = theme_hdl2buf(numbers_ctrl->number_bmp);					//直接传入的图片句柄得到其buffer地址，					（必须的语句）
			//app_bmp_x = 100;//numbers_ctrl->numbers_uipara->numbers_bmp_pos.x;	//app应用图片矩形控件显示的x轴坐标，		（坐标也是必须）
			//app_bmp_y = 90;//numbers_ctrl->numbers_uipara->numbers_bmp_pos.y; 	//app应用图片矩形控件显示的y轴坐标，
	#else	//通过结构体获取图片传入的ID句柄
		#if 1	//执行app应用图片选中和未选中的显示图片句柄逻辑
				if(focus_flage == 1)//选中图标
				{
					GUI_SetColor(numbers_ctrl->focus_txt_color);
					app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_focus_bmp[index]);		//通过传入的图片索引index绘制
					app_bmp_x -= app_focus_x;
					app_bmp_y -= app_focus_y;
				}
				else//未选中图标
				{
					GUI_SetColor(numbers_ctrl->unfocus_txt_color);
					app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_unfocus_bmp[index]);	//通过传入的图片索引index绘制
				#if (APP_FOCUS_W >= APP_W) || (APP_FOCUS_H >= APP_H)
						GUI_ClearRect(app_bmp_x - app_focus_y, app_bmp_y - app_focus_y, app_bmp_x + APP_FOCUS_W - 1, app_bmp_y + APP_FOCUS_H - 1);
				#endif
				}
		#else	//单独显示选中或者未选中的app应用图片句柄
					{
				#if 0	//单个app应用图片句柄获取
						app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_unfocus_bmp[0]);					//获取未选中的app应用图片句柄得到其buffer地址，					（必须的语句）
				#else	//全部app应用图片句柄获取
						app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_unfocus_bmp[i]);					//获取未选中的app应用图片句柄得到其buffer地址，					（必须的语句）
				#endif
					}
		#endif
	#endif
		
		GUI_BMP_Draw(app_theme,app_bmp_x,app_bmp_y);							//app应用图片绘制到矩形控件上面
		{
			//__wrn("gui2_rect.x0 = %d , gui2_rect.y0 = %d,gui2_rect.width = %d , gui2_rect.heigh = %d\n",gui2_rect.x0,gui2_rect.y0,gui2_rect.x1,gui2_rect.y1);
			__wrn("app_bmp_x = %d app_bmp_x = %d\n",app_bmp_x,app_bmp_x);
			dsk_langres_get_menu_text(app_ui_id[index].app_lang_id, app_txt, sizeof(app_txt));//把结构体文本文字ID索引赋值给app_txt数组
			GUI_DispStringInRect(app_txt, &gui2_rect, GUI_TA_HCENTER | GUI_TA_BOTTOM);		  //把app_txt字符串显示到矩形gui2_rect上面水平居中，放置到底部
		}
	}
#endif
	__wrn("app_menu_paint_one 2 CREATE\n");
	//GUI_SetBkColor(BEETLES_BLACK_COLOR);

}
/*********************************************************************************
*函数作用：绘制所有app应用图层
*********************************************************************************/
static __s32 number_menu_numbers_paint_all(__gui_msg_t *msg,__u32 num)
{
	number_menu_numbers_ctrl_t *numbers_ctrl;

	GUI_RECT gui_rect;

	char	 *txt_character;//存放文字的字符串：“number:00”
	void  	*number_theme;
	static __s32 i = 0;
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)//图层窗口创建失败
	{
		return EPDK_FAIL;
	}

	numbers_ctrl = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//结构体里面添加一个数据,作用是确保下面的数据能正常访问

	if(!numbers_ctrl)
	{
		__msg(" is null \n ");
		return EPDK_FAIL ;
	}
	__wrn("number_menu_numbers_paint_all CREATE\n");
	GUI_LyrWinSel(numbers_ctrl->numbers_para->layer);			//先选择图层窗口事件，选择窗口图层然后在图层上面绘制，numbers_ctrl->numbers_para->layer
	#if 0		//直接在图层上面显示绘制文本字符串
		GUI_SetFont(numbers_ctrl->numbers_para->msgbox_font);	//设置对话框字体
		GUI_UC_SetEncodeUTF8();									//设置编码格式
		__wrn("number_menu_numbers_paint_all 3 CREATE\n");
		GUI_SetColor(GUI_BLACK);								//设置文本颜色，黑色
		number_menu_numbers_get_volume_size(msg,num);			//获取回车按键点击次数 字符串
		txt_character = numbers_ctrl->number_total_size;		//需要显示的文字字符串获取
		__wrn("number_menu_numbers_paint_all 4 CREATE\n");
		GUI_DispStringAt(txt_character,numbers_ctrl->numbers_uipara->number_tsize_pos.x,numbers_ctrl->numbers_uipara->number_tsize_pos.y);
		{
			__wrn("txt_character = %s\n",txt_character);		//直接显示文字字符串，文本绘制成功
		}
		
		//number_theme = theme_hdl2buf(numbers_ctrl->number_bmp);
		//GUI_BMP_Draw(number_theme, numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//绘制文本控件number:00显示函数
		__wrn("number_menu_numbers_paint_all5 CREATE\n");
		__wrn("numbers_ctrl->number_total_size = %s\n",numbers_ctrl->number_total_size);
		__wrn("number_tsize_pos.x = %d , number_tsize_pos.y = %d\n",numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);
		//GUI_DispStringAt(numbers_ctrl->number_total_size, numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);			//在文本控件上显示字符串
		//GUI_DispStringAt("01",32,28);//字符串显示和坐标
		__wrn("txt paint is CREATE\n");//文本绘制成功
	#endif

	#if	1		//绘制矩形 然后在矩形框显示绘制文字字符串---> 显示 "number:00"
		GUI_SetFont(numbers_ctrl->numbers_para->msgbox_font);		//设置对话框字体
		GUI_UC_SetEncodeUTF8();										//设置编码格式
		
		GUI_SetColor(GUI_GREEN);									//设置文本颜色，黑色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);					//设置画图模式为正常
		//GUI_SetBkColor(BEETLES_GRAY_COLOR);						//设置矩形框控件的背景颜色为黑色
		number_menu_numbers_get_volume_size(msg,num);			//获取回车按键点击次数 字符串
		txt_character = numbers_ctrl->number_total_size;		//需要显示的文字字符串获取
		gui_rect.x0 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.x;		//矩形x 0
		gui_rect.y0 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.y;		//矩形y 0
		gui_rect.x1 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.width;	//矩形宽80
		gui_rect.y1 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.height;  //矩形高20
		{
			number_theme = theme_hdl2buf(numbers_ctrl->number_bmp);
			GUI_BMP_Draw(number_theme, numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//绘制文本控件number:00显示函数，防止number的字符串被覆盖问题
	
			__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			GUI_DispStringInRect(txt_character , &gui_rect , GUI_TA_LEFT | GUI_TA_VCENTER);//将文字字符串显示到矩形控件上面，并水平居中

		}
	#endif

	#if  0 		//绘制矩形  然后在矩形控件里面绘制单个或全部app应用图片
	for(i = 0; i < APP_MAX_NUM;i++)
	{
		__wrn("GUI_SetColor CREATE\n");
		#if 1	//app应用图片选中的x,y坐标
			app_focus_x = (APP_FOCUS_W -APP_W)/2;						//(选中的app应用图片宽190 - 没有选中的app应用图片宽150)/2
			app_focus_y = (APP_FOCUS_H -APP_H)/2;						//(选中的app应用图片高190 - 没有选中的app应用图片高150)/2
		#endif
		#if 0	//单个app应用图片显示位置
			app_bmp_x = numbers_ctrl->numbers_uipara->numbers_app_pos[0].x;//先获取一个app图片显示的x轴坐标
			app_bmp_y = numbers_ctrl->numbers_uipara->numbers_app_pos[0].y;//先获取一个app图片显示的y轴坐标	
		#else	//所有app应用图片显示位置 APP_MAX_NUM共8个
			app_bmp_x = numbers_ctrl->numbers_uipara->numbers_app_pos[i].x;//先获取一个app图片显示的x轴坐标
			app_bmp_y = numbers_ctrl->numbers_uipara->numbers_app_pos[i].y;//先获取一个app图片显示的y轴坐标	
		#endif
		
		__wrn("app_bmp_x = %d , app_bmp_y = %d\n",app_bmp_x,app_bmp_y);
		__wrn("numbers_ctrl->numbers_uipara->numbers_app_pos[1].x = %d , numbers_ctrl->numbers_uipara->numbers_app_pos[1].y = %d\n",numbers_ctrl->numbers_uipara->numbers_app_pos[1].x,numbers_ctrl->numbers_uipara->numbers_app_pos[1].y);
		
		__wrn("numbers_ctrl->numbers_uipara->numbers_app_pos[0].w = %d , numbers_ctrl->numbers_uipara->numbers_app_pos[0].h = %d\n",numbers_ctrl->numbers_uipara->numbers_app_pos[0].width,numbers_ctrl->numbers_uipara->numbers_app_pos[0].height);
		__wrn("numbers_ctrl->numbers_uipara->lyr_numbers_menu.x = %d , numbers_ctrl->numbers_uipara->lyr_numbers_menu.y = %d\n",numbers_ctrl->numbers_uipara->lyr_numbers_menu.x,numbers_ctrl->numbers_uipara->lyr_numbers_menu.y);
		
		#if 1	//按照app应用图片大小 绘制矩形的大小
			gui2_rect.x0 = app_bmp_x-16;						 //矩形控件的x0坐标	第一个app应用矩形x坐标：84.8-16
			gui2_rect.y0 = app_bmp_y +APP_H +24;				 //矩形控件的y0坐标	第一个app应用矩形y坐标：103+150+24
			gui2_rect.x1 = gui2_rect.x0 +APP_W -1 +32;			 //矩形控件的宽			第一个app应用矩形宽坐标：68+150-1+32	
			gui2_rect.y1 = gui2_rect.y0+24 -1;					 //矩形控件的高			第一个app应用矩形高坐标：177+24-1
		#else	//直接绘制矩形大小，自定义大小
			gui_rect.x0 = 200+numbers_ctrl->numbers_uipara->numbers_tiltle_pos.x;		//矩形x
			gui_rect.y0 = 200+numbers_ctrl->numbers_uipara->numbers_tiltle_pos.y;		//矩形y
			gui_rect.x1 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.width;	//矩形宽150
			gui_rect.y1 = numbers_ctrl->numbers_uipara->numbers_tiltle_pos.height;  //矩形高150
			{
				__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			}
		#endif
		__wrn("111 gui2_rect.x0 = %d , gui2_rect.y0 = %d,gui2_rect.width = %d , gui2_rect.heigh = %d\n",gui2_rect.x0,gui2_rect.y0,gui2_rect.x1,gui2_rect.y1);
		#if 0	//直接传入图片ID的句柄
			//app_theme = theme_hdl2buf(numbers_ctrl->number_bmp);					//直接传入的图片句柄得到其buffer地址，					（必须的语句）
			//app_bmp_x = 100;//numbers_ctrl->numbers_uipara->numbers_bmp_pos.x;	//app应用图片矩形控件显示的x轴坐标，		（坐标也是必须）
			//app_bmp_y = 90;//numbers_ctrl->numbers_uipara->numbers_bmp_pos.y; 	//app应用图片矩形控件显示的y轴坐标，
		#else	//通过结构体获取图片传入的ID句柄
			#if 1	//执行app应用图片选中和未选中的显示图片句柄逻辑
				if(i == 1)//选中图标
				{
					GUI_SetColor(numbers_ctrl->focus_txt_color);
					app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_focus_bmp[i]);		//通过传入的图片索引index绘制
					app_bmp_x -= app_focus_x;
					app_bmp_y -= app_focus_y;
				}
				else//未选中图标
				{
					GUI_SetColor(numbers_ctrl->unfocus_txt_color);
					app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_unfocus_bmp[i]);	//通过传入的图片索引index绘制
					#if (APP_FOCUS_W >= APP_W) || (APP_FOCUS_H >= APP_H)
						GUI_ClearRect(app_bmp_x - app_focus_y, app_bmp_y - app_focus_y, app_bmp_x + APP_FOCUS_W - 1, app_bmp_y + APP_FOCUS_H - 1);
					#endif
				}
			#else	//单独显示选中或者未选中的app应用图片句柄
					{
					#if 0	//单个app应用图片句柄获取
						app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_unfocus_bmp[0]);					//获取未选中的app应用图片句柄得到其buffer地址，					（必须的语句）
					#else	//全部app应用图片句柄获取
						app_theme = dsk_theme_hdl2buf(numbers_ctrl->app_unfocus_bmp[i]);					//获取未选中的app应用图片句柄得到其buffer地址，					（必须的语句）
					#endif
					}
			#endif
		#endif
		
		GUI_BMP_Draw(app_theme,app_bmp_x,app_bmp_y);							//app应用图片绘制到矩形控件上面
		{
			//__wrn("gui2_rect.x0 = %d , gui2_rect.y0 = %d,gui2_rect.width = %d , gui2_rect.heigh = %d\n",gui2_rect.x0,gui2_rect.y0,gui2_rect.x1,gui2_rect.y1);
			__wrn("app_bmp_x = %d app_bmp_x = %d\n",app_bmp_x,app_bmp_x);
			dsk_langres_get_menu_text(app_ui_id[i].app_lang_id, app_txt, sizeof(app_txt));//不结构体文本文字ID索引赋值给app_txt数组
			GUI_DispStringInRect(app_txt, &gui2_rect, GUI_TA_HCENTER | GUI_TA_BOTTOM);		  //把app_txt字符串显示到矩形gui2_rect上面水平居中，放置到底部
		}
	}
	#endif
	#if 0		//绘制矩形  然后在矩形控件里面绘制所有app应用图片
		for(i = 0; i < APP_MAX_NUM;i++)
		{
			if(i==1)
			{
				app_menu_paint_one(msg, 0,i, 1);//第一个显示选中
			}
			else
			{
				app_menu_paint_one(msg,0,i,0);	//单个app应用图片绘制函数
			}
		}
	#endif
	__wrn("number_menu_numbers_paint_all 6 CREATE\n");
	//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形控件背景颜色为黑色
	__wrn("number_menu_numbers_paint_all 7 CREATE\n");
	//..GUI_ClearRectEx(&gui_rect);								//矩形删除
	__wrn("number_menu_numbers_paint_all 2 CREATE\n");
	return EPDK_OK;
}
/*********************************************************************************
*函数作用：按键处理函数
*形参：__gui_msg_t *msg：GUI固定必须的结构体指针变量
*********************************************************************************/
static __s32  number_menu_numbers_key_proc(__gui_msg_t *msg)
{
#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("number_menu_numbers key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("number_menu_numbers key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_ENTER, 0, 0);
		}
		if(GUI_MSG_KEY_RIGHT == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_NUMBER_MENU_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("number_menu_numbers key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
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
/*====================================================================================
*函数作用：图层窗口回调函数
*形参：__gui_msg_t *msg：GUI固定必须的结构体指针变量
====================================================================================*/
static __s32 number_menu_numbers_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	static __s32  app_flage = 1;
	static __s32  key_up_number=0;
	static __s32  key_down_number=0;
	static __s32  j;
	switch(msg->id)
	{
		__wrn("number_menu_numbers msg->id = %d\n",msg->id);
		case GUI_MSG_CREATE:	//创建菜单窗口
		{	
			
			#if 1
				number_menu_numbers_ctrl_t *numbers_ctrl;	//number：次数显示提示
			#else
				numbermenu_attr_t *numbers_attr;			//app应用显示
			#endif
			number_menu_number_para_t *numbers_para;
			numbers_para = (number_menu_number_para_t *)GUI_WinGetAttr(msg->h_deswin);	//获取窗口的属性
			#if 1	//number数值显示窗口创建
				numbers_ctrl = (number_menu_numbers_ctrl_t *)My_Balloc(sizeof(number_menu_numbers_ctrl_t));//内存分配
				if(!numbers_ctrl)
				{
					__err("memory balloc fail........\n");
					return EPDK_FAIL;
				}
				eLIBs_memset(numbers_ctrl, 0, sizeof(number_menu_numbers_ctrl_t));			//给结构体number_menu_numbers_ctrl_t内存初始化
				numbers_ctrl->focus_txt_color = GUI_YELLOW;//APP_COLOR_YELLOW;							//选中的文本颜色
				numbers_ctrl->unfocus_txt_color = GUI_WHITE;//APP_COLOR_WHITE;							//未选中的文本颜色
				numbers_ctrl->numbers_para = numbers_para;
				GUI_WinSetAddData(msg->h_deswin, (__u32)numbers_ctrl);						//添加一个数据
				number_menu_number_init(numbers_ctrl);										//number:00显示提示图片资源申请和窗口显示大小
				//GUI_LyrWinCacheOn();	//窗口检查打开
				GUI_LyrWinSetSta(numbers_ctrl->numbers_para->layer, GUI_LYRWIN_STA_ON);		//窗口图层设置开启状态
				GUI_LyrWinSetTop(numbers_ctrl->numbers_para->layer);						//窗口图层设置停止状态
				//number_menu_numbers_paint_all(msg,0);										//绘制所有图层
				//GUI_LyrWinCacheOff();	//窗口检查关闭
			#else			//app应用窗口创建
				numbers_attr = (numbermenu_attr_t *)My_Balloc(sizeof(numbermenu_attr_t));	//分配内存
				if(!numbers_attr)	//检测mmenu_attr_t
				{
					__err("mmenu_attr_t balloc fail........\n");
					return EPDK_FAIL;
				}
				
				eLIBs_memset(numbers_attr, 0, sizeof(numbermenu_attr_t));				//给结构体初始化
				numbers_attr->number_font = numbers_para->smenu_font;					//app应用文本
				numbers_attr->number_layer = numbers_para->layer;						//app应用图层，选中的图片显示
				numbers_attr->number_bg_layer = numbers_para->bg_layer;					//app应用背影图层，就是没有选中的图片显示
				numbers_attr->number_focus_app = 0;										//选中的app应用
				numbers_attr->number_first_app = 0;										//第一个app应用
				numbers_attr->number_focus_txt_color = GUI_YELLOW;//APP_COLOR_YELLOW;				//选中app应用的文本颜色
				numbers_attr->number_unfocus_txt_color = GUI_WHITE;//APP_COLOR_WHITE;				//未选中app应用的文本颜色
				GUI_WinSetAddData(msg->h_deswin, (__s32)numbers_attr);					//在msg->h_deswin结构体新添加一个numbers_attr结构体变量
				__wrn("number_menu_numbers GUI_MSG_CREATE\n");
				init_numbermenu_res(numbers_attr);										//app应用图片申请使用资源
				GUI_LyrWinSetSta(numbers_attr->number_layer, GUI_LYRWIN_STA_ON);		//窗口设置开启状态
				GUI_LyrWinSetTop(numbers_attr->number_layer);						//窗口设置停止状态
				//paint_numbermain_app_all(numbers_attr);			//绘制所有app应用
				//paint_numbermain_one_app(numbers_attr, 1, 1);		//绘制单个app
				__wrn("paint_numbermain_one_app GUI_MSG_CREATE\n");
				
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
			#if 0
			theme_close(numbers_ctrl->number_bmp);		//图片关闭
			theme_close(numbers_ctrl->number_f_bmp);	//图片关闭
			#else
			//dsk_theme_close(numbers_ctrl->app_focus_bmp[APP_MAX_NUM]);		//选中额app应用图片关闭
			//dsk_theme_close(numbers_ctrl->app_unfocus_bmp[APP_MAX_NUM]);		//未选中的app应用图片关闭
			number_menu_number_uninit(numbers_ctrl);//释放资源函数
			#endif

			if(numbers_ctrl->numbers_para)
			{
				__msg("number_menu_numbers win free number_menu_numbers_para.....\n");
				My_Bfree(numbers_ctrl->numbers_para, sizeof(number_menu_number_para_t));//释放窗口结构体内存
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
			#if 1
			number_menu_numbers_paint_all(msg,0);//number次数显示所有
			#else	//绘制app应用单个app

			#endif
			
			return EPDK_OK;

		case GUI_MSG_KEY:	//按键处理函数
			number_menu_numbers_key_proc(msg);
			return EPDK_OK;

		case MSG_NUMBER_MENU_OP_ENTER:	//回车确认按键
		{
			__gui_msg_t mymsg;
			number_menu_numbers_ctrl_t *numbers_ctrl1 = NULL ;
			New_Setting_para_t new_settings_para;
			#if 1//用于新的子窗口
			home1_para_t *home1_para;	//用于调用子窗口创建结构体
			root_para_t *root_para;
			SIZE screen_size;			//存放LCD显示屏大小
			//
			New_Setting_para_t new_smenu_para;	 //new设置子窗口结构体
			New_Setting_ctrl_t *New_Setting_ctrl;//用于获取窗口尺寸
			#endif
			
			numbers_ctrl1 = (number_menu_numbers_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//添加一个数据

			//key_number += 1;
			#if 0	
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
				number_menu_numbers_paint_all(msg,key_number);//刷新number次数显示内容
			}
			#endif

			#if 0   //回车按键功能执行
				if(key_number == 5)//删除app应用
				{
					#if 0	//调用销毁窗口
					mymsg.id = GUI_MSG_DESTROY;//调用销毁窗口
					mymsg.h_deswin = msg->h_deswin;
					mymsg.h_srcwin = msg->h_srcwin;
					mymsg.dwAddData1 = mymsg.dwAddData1;
					mymsg.dwAddData2 = mymsg.dwAddData2;
					GUI_SendMessage(&mymsg);
					#endif
					#if 0	//方法1，直接关闭app应用图片显示
						for(j=0;j<APP_MAX_NUM;j++)
						{
							dsk_theme_close(numbers_ctrl1->app_unfocus_bmp[j]);	//单独释放一个未选中的app应用图片资源
							numbers_ctrl1->app_unfocus_bmp[j] = NULL;
							dsk_theme_close(numbers_ctrl1->app_focus_bmp[j]);	//单独释放一个选中的app应用图片资源
							numbers_ctrl1->app_focus_bmp[j] = NULL;	
						}
					#endif
					#if 1	//方法2，直接释放相应的app应用图片资源,numbers_ctrl->app_focus_bmp[i]
					dsk_theme_close(numbers_ctrl1->app_unfocus_bmp[0]);	//单独释放一个未选中的app应用图片资源
					numbers_ctrl1->app_unfocus_bmp[0] = NULL;
					dsk_theme_close(numbers_ctrl1->app_focus_bmp[0]);	//单独释放一个选中的app应用图片资源
					numbers_ctrl1->app_focus_bmp[0] = NULL;
					//dsk_theme_exit();				//退出图片资源模块
					if(numbers_ctrl1)
					{
						__msg("number_menu_numbers win free number_menu_numbers_ctrl 1.....\n");
						//My_Bfree(numbers_ctrl1, sizeof(number_menu_numbers_ctrl_t));
						esMEMS_Bfree(numbers_ctrl1, sizeof(number_menu_numbers_ctrl_t));//清除结构体内存
						numbers_ctrl1 = NULL ;
					}
					app_menu_paint_one(msg,0,0,0);	//单个app应用图片绘制函数，刷新显示
					app_menu_paint_one(msg,0,0,1);	//单个app应用图片绘制函数，刷新显示
					#endif
				}
				if(key_number == 10)//然后显示全部app应用
				{
					key_number = 0;
					#if 0	//方法1，重新创建窗口
					mymsg.id = GUI_MSG_CREATE;//调用创建窗口
					mymsg.h_deswin = msg->h_deswin;
					mymsg.h_srcwin = msg->h_srcwin;
					mymsg.dwAddData1 = mymsg.dwAddData1;
					mymsg.dwAddData2 = mymsg.dwAddData2;
					GUI_SendMessage(&mymsg);
					#endif
					#if 1	//方法2，重新申请使用图片资源
						number_menu_number_init(numbers_ctrl1);//重新申请图片资源
					#endif
					#if 1		//绘制矩形  然后在矩形控件里面绘制所有app应用图片
						for(j = 0; j < APP_MAX_NUM;j++)
						{
							if(j == 0)
							{
								app_menu_paint_one(msg, 0,j, 1);//第一个显示选中
							}
							else
							{
								app_menu_paint_one(msg,0,j,0);	//单个app应用图片绘制函数
							}
						}
					#endif
				}
				if(key_number == 1)//显示单个app应用
				{
					app_menu_paint_one(msg,0,0,1);	//单个app应用图片绘制函数
				}
				__wrn("key_number =%d  \n",key_number);
				number_menu_numbers_paint_all(msg,key_number);//刷新number次数显示内容
			#endif
			#if 1	//当app选中设置app应用时，回车按键按下创建new通用设置app子菜单窗口并进入子菜单窗口
				if(key_up_number == 6)//通用设置app
				{
					key_number += 1;
				}
				if(key_number == 3)//按三次回车进入新的通用设置子窗口
				{
					
					#if 0//新建通用设置app的子窗口
						
						#if 1	//单独确认子菜单图层矩形的大小
						RECT rect;
						rect.x = 0;
						rect.y = 0;
						rect.width = 800;
						rect.height = 480;
						//1
						root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
						home1_para = (home1_para_t *)esMEMS_Balloc(sizeof(home1_para_t));
						//home1_para = (home1_para_t *)My_Balloc(sizeof(home1_para_t));//分配内存
						dsk_display_get_size(&screen_size.width, &screen_size.height);//获取LCD显示屏大小:1024*600
						New_Setting_ctrl->New_Settings_uipara = New_Setting_get_numbers_para(screen_size);//获取子窗口：number:00显示的矩形大小和位置
			
						if(!home1_para)
						{
							return EPDK_FAIL;
						}
						#if 0	//通过获取lcd显示屏尺寸的大小
						rect.x = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;	//0
						rect.y = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;	//0
						rect.width = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//200
						rect.height = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height;	//100
						#endif	
						eLIBs_memset((void *)home1_para, 0, sizeof(home1_para_t));
						home1_para->main_font = root_para->font;
						home1_para->h_app_main = msg->h_deswin;
						__wrn("home1_para is ok ok ok\n");
						//1
						//
						//new_settings_para.msgbox_font = numbers_ctrl1->numbers_uipara->numbers_tiltle_pos;//文本
						home1_para->lyr_smenu = number_menu_8bpp_layer_create(&rect,1);//图层
						if(!home1_para->lyr_smenu)
						{
							__err("home1_para->lyr_smenu is null...\n");
							return ;
						}
						new_smenu_para.layer = home1_para->lyr_smenu;				//一图层赋值，没有选中app应用前的图层

						new_smenu_para.sub_menu_id = home1_para->focus_id;			//选中的app应用窗口ID
						new_smenu_para.smenu_font = home1_para->main_font;			//字体
						new_smenu_para.root_type = home1_para->root_type;			//
						new_smenu_para.focus_item = home1_para->focus_smenu_item;	//选中的内容
						new_smenu_para.bg_layer = home1_para->lyr_smenu;			//二图层，选中app应用的的背影图层
	
						home1_para->h_submenu = New_Setting_app_win_create(home1_para->h_app_main, &new_settings_para);//(GUI_WinGetHandFromName(APP_SETTING), &new_settings_para);
						__wrn("New_Setting_app_win_create is ok ok ok\n");
						GUI_WinSetFocusChild(home1_para->h_submenu);				//命令发送，按键执行需要调用
						#endif
						//
						//开启图层窗口状态
						GUI_LyrWinSetSta(home1_para->lyr_smenu, GUI_LYRWIN_STA_ON);		//设置子菜单图层窗口状态
						//..GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_ON);
						//GUI_LyrWinSetTop(home_para->lyr_mmenu);
					#endif
					#if 0//直接绘制新的通用设置app的子窗口
					__wrn("one  paint_ \n");
					New_Setting_paint_one_item(msg,key_number);
					#endif
					key_number = 0;
				}
				else
				{	
					__wrn("key_number =%d  \n",key_number);
					number_menu_numbers_paint_all(msg,key_number);//刷新number次数显示内容
				}
			#endif
			
		}

		return EPDK_OK;
		case MSG_NUMBER_MENU_OP_DOWN:		//下
		{
			__wrn("key_down_number =%d  \n",key_up_number);
			#if 1
				app_menu_paint_one(msg, 0,key_up_number,0);//先把前面一个选中的清除为未选中
				key_up_number++;

				if(key_up_number >= APP_MAX_NUM)			 //大于等于8，就从头开始选择
				{
					key_up_number = 0;
				}

				app_menu_paint_one(msg, 0,key_up_number, 1); //然后显示选中app应用
			#endif
		}
		return EPDK_OK;
		case MSG_NUMBER_MENU_OP_UP:		//上
		{
			__wrn("key_up_number =%d  \n",key_up_number);
			#if 1
				app_menu_paint_one(msg, 0,key_up_number,0); //先把前面一个选中的清除为未选中
				key_up_number--;

				if(key_up_number < 0)						//小于0时，从最后一个开始选择
				{
					key_up_number = APP_MAX_NUM - 1;
				}

				app_menu_paint_one(msg, 0,key_up_number, 1); //然后显示选中app应用
			#endif
		}
		return EPDK_OK;
		case MSG_NUMBER_MENU_OP_EXIT:	//退出返回按键，也是调用关闭窗口
		{
			#if 0
			__gui_msg_t mymsg;
			__wrn("MSG_NUMBER_MENU_OP_EXIT  \n");
			mymsg.id = GUI_MSG_CLOSE;
			mymsg.h_deswin = msg->h_deswin;
			mymsg.h_srcwin = msg->h_srcwin;
			mymsg.dwAddData1 = msg->dwAddData1;
			mymsg.dwAddData2 = msg->dwAddData2;
			GUI_SendMessage(&mymsg);
			#endif
		}

		return EPDK_OK;

		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}
/*====================================================================================
*图层窗口建立
====================================================================================*/
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
	numbers_para->sub_menu_id = para->sub_menu_id;	//子菜单ID 可以不用
	numbers_para->root_type = para->root_type;		//
	numbers_para->focus_item = para->focus_item;	//子菜单内容
	
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
	framewin_para.hLayer = para->layer;			//窗口图层
	return (GUI_FrmWinCreate(&framewin_para));	//返回控件窗口创建成功
}

void  number_menu_numbers_win_delete(H_WIN numbers_win)
{
	GUI_FrmWinDelete(numbers_win);
}


