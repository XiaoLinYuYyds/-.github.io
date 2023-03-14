#include "number_menu_app_ui.h"
#include "home_uipara.h"
#include "typedef.h"




/*====================================================================================
*传入图片ID索引
====================================================================================*/
static number_res_id_t number_uipara_default[] =
{
	{STRING_HOME_MUSIC, ID_HOME_MAIN_MUSIC_LOSE_BMP, ID_HOME_MAIN_MUSIC_FOCUS_BMP},		//音乐；格式:第一位为语言，第二位为不选中图片，第三位为选中图片
	{STRING_HOME_MOVIE, ID_HOME_MAIN_MOVIE_LOSE_BMP, ID_HOME_MAIN_MOVIE_FOCUS_BMP},		//电影
	{STRING_HOME_PHOTO, ID_HOME_MAIN_PHOTO_LOSE_BMP, ID_HOME_MAIN_PHOTO_FOCUS_BMP},		//相片
	{STRING_HOME_EBOOK, ID_HOME_MAIN_EBOOK_LOSE_BMP, ID_HOME_MAIN_EBOOK_FOCUS_BMP},		//电子书
	{STRING_HOME_FM, ID_HOME_MAIN_FM_LOSE_BMP, ID_HOME_MAIN_FM_FOCUS_BMP},				//收音
	{STRING_HOME_RECORD, ID_HOME_MAIN_RECORD_LOSE_BMP, ID_HOME_MAIN_RECORD_FOCUS_BMP},	//录音
	//{STRING_HOME_RECORD, ID_HOME_MAIN_RECORD_LOSE_BMP, ID_HOME_MAIN_RECORD_FOCUS_BMP},	//录音
	//{STRING_HOME_RECORD, ID_HOME_1_BMP, ID_HOME_MAIN_RECORD_FOCUS_BMP},					//新添加加载的图片
	//{STRING_HOME_RECORD, ID_HOME_2_BMP, ID_HOME_MAIN_RECORD_FOCUS_BMP},					//新添加加载的图片
	{STRING_HOME_SETMAN_COMMON, ID_HOME_MAIN_GENERAL_SETTING_LOSE_BMP, ID_HOME_MAIN_GENERAL_SETTING_FOCUS_BMP},//通用设置
	//{STRING_HOME_SETMAN_POWER, ID_HOME_MAIN_POWER_SETTING_LOSE_BMP, ID_HOME_MAIN_POWER_SETTING_FOCUS_BMP},//电源管理
	{STRING_HOME_EXPLORER, ID_HOME_MAIN_EXPLORER_LOSE_BMP, ID_HOME_MAIN_EXPLORER_FOCUS_BMP},//文件管理
#if SP_APP_CALENDAR
	{STRING_HOME_CALENDAR, ID_HOME_MAIN_CALENDAR_LOSE_BMP, ID_HOME_MAIN_CALENDAR_FOCUS_BMP},//时钟日历
#endif
};

/*====================================================================================
*获取传入图片的数据 --> 传入图片ID索引
====================================================================================*/
 number_res_id_t *number_get_ui_para(__s32 rotate)
{
	
	return number_uipara_default;
}

/*====================================================================================
*申请使用图片资源
=====================================================================================*/
__s32 init_numbermenu_res(numbermenu_attr_t *number_menu_attr)
{
	__u32 	i;
	home_uipara_t *home_ui_para;
	number_res_id_t *main_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());//获取窗口大小

	if(!home_ui_para)
	{
		__err("home_ui_para is null..\n");
		return EPDK_FAIL;
	}

	main_ui_para = number_get_ui_para(GUI_GetScnDir());//获取传入图片索引数据

	if(!main_ui_para)		//传入失败
	{
		__err("main_ui_para is null...\n");
		return EPDK_FAIL;
	}

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(NULL == number_menu_attr->number_unfocus_bmp[i])
		{
			number_menu_attr->number_unfocus_bmp[i] = dsk_theme_open(main_ui_para[i].unfocus_bmp_id);	//不选择的图片索引
		}

		if(NULL == number_menu_attr->number_focus_bmp[i])
		{
			number_menu_attr->number_focus_bmp[i] = dsk_theme_open(main_ui_para[i].focus_bmp_id);		//选择的图片索引
		}
	}
	__wrn("number_menu_number_init CREATE\n");
	return EPDK_OK;
}
/*====================================================================================
*释放使用图片资源
=====================================================================================*/
static __s32 uninit_numbermenu_res(numbermenu_attr_t *number_menu_attr)
{
	__u32 	i;

	for(i = 0; i < MAX_ITEM_NUM; i++)							//最大8个app应用
	{
		if(number_menu_attr->number_unfocus_bmp[i])
		{
			dsk_theme_close(number_menu_attr->number_unfocus_bmp[i]);	//关闭图片
			number_menu_attr->number_unfocus_bmp[i] = NULL;			//空值显示
		}

		if(number_menu_attr->number_focus_bmp[i])
		{
			dsk_theme_close(number_menu_attr->number_focus_bmp[i]);	//关闭图片
			number_menu_attr->number_focus_bmp[i]  = NULL;			//空值显示
		}
	}

	return EPDK_OK;
}
/*====================================================================================
*图片绘制单个app应用函数
=====================================================================================*/
void paint_numbermain_one_app(numbermenu_attr_t *number_menu_attr, __s32 index, __s32 bfocus)
{
	GUI_RECT  app_gui_rect;		//定义矩形创建变量
	__u32 	i;
	void *app_pbmp;				//空指针app应用控件主题
	__s32 app_bmp_width;		//app应用控件图片宽
	__s32 app_bmp_height;		//app应用控件图片高
	HBMP  app_hbmp;				//hbmp格式变量
	__s32 app_bmp_x;			//app应用图片显示x轴
	__s32 app_bmp_y;			//app应用图片显示y轴
	char  app_text[256];		//存放app应用显示字体字符串文本数据
	home_uipara_t 		*home_ui_para;
	number_res_id_t 	*number_ui_para;
	GUI_MEMDEV_Handle 	app_draw_mem = NULL;	
	__u8 app_paint_flag = number_menu_attr->number_paint_flag;	//app应用的绘制标志位
	
	home_ui_para = home_get_ui_para(GUI_GetScnDir());			//获取主窗口的大小
	number_menu_attr->number_paint_flag = 0;					//app应用绘制标志位置0

	if(!home_ui_para)											//检测是否获取到主窗口的大小
	{
		__err("home_ui_para is null..\n");
		return ;
	}

	number_ui_para = number_get_ui_para(GUI_GetScnDir());		//获取app应用的图片索引ID
	__wrn("paint_numbermain_one_app GUI_MSG_CREATE\n");
	if(!number_ui_para)											//检测是否获取到传入图片ID数据
	{
		__err("number_ui_para is null...\n");
		return ;
	}

	if(index < 0 || index >= MAX_ITEM_NUM)						//app应用索引数量小于0或者大于等于8个，无效
	{
		__err("invalid para...\n");
		return ;
	}

	if(!(app_paint_flag & NUMBER_MASK_PaintFlag_MEMDEV_Nonuse))	//
	{
		__s32 screen_width, screen_height;
		/* get lcd size*/
		dsk_display_get_size(&screen_width, &screen_height);	//获取lcd显示屏的大小1024*600
		__wrn("screen_width = %d  screen_height = %d \n",screen_width,screen_height);
		GUI_LyrWinSel(number_menu_attr->number_layer);			//图层窗口选择
		GUI_SetFont(number_menu_attr->number_font);				//设置字体
		GUI_UC_SetEncodeUTF8();									//设置编码格式
		GUI_SetBkColor(0);										//设置app应用控件的背景颜色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);					//设置绘图模式为正常模式
		app_draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height); //伪内存创建，大小
		__wrn("app_draw_mem 1 = 0x%x\n", app_draw_mem);
	}

	if(app_draw_mem)											//伪内存存在
	{
		GUI_MEMDEV_Select(app_draw_mem);						//选择伪内存
		__wrn("app_draw_mem ok CREATE\n");
	}
	#if 1	//绘制矩形
	{
		GUI_RECT app_gui_rect;	//画矩形
		__u16 xOft, yOft;
		xOft = (APP_FOCUS_W -APP_W)/2;						//选中的app应用图片宽190 - 没有选中的app应用图片宽 /2 = 矩形的x轴坐标
		yOft = (APP_FOCUS_H -APP_H)/2;						//选中的app应用图片高190 - 没有选中的app应用图片高 /2 = 矩形的y轴坐标
		app_bmp_x = home_ui_para->item_pos[index].x;			//app应用图片的显示坐标x轴，index：索引第几个图片显示
		app_bmp_y = home_ui_para->item_pos[index].y;			//app应用图片的显示坐标y轴
		app_gui_rect.x0 = app_bmp_x-16;							//app应用显示矩形x轴坐标 - 16(app之间的间隔大小) = app应用矩形的起坐标x0
		app_gui_rect.y0 = app_bmp_y +APP_H +24;				//app应用显示矩形y轴坐标 + 150(ITEM_H是没有选中的图片高)+ 24(与第二行的间隔高) = app应用矩形的起坐标y0
		app_gui_rect.x1 = app_gui_rect.x0 +APP_W -1 +32;		//app应用矩形的起坐标x0 + 150(ITEM_W是没有选中的图片宽) - 1 + 32 = app应用矩形的末尾坐标x1
		app_gui_rect.y1 = app_gui_rect.y0+24 -1;
		__wrn("app_gui_rect is CREATE\n");

		if(bfocus)	//等于1选中图标
		{
			__wrn("bfocus is ..\n");
			GUI_SetColor(number_menu_attr->number_focus_txt_color);	//设置颜色，选中的文本颜色黄色
			__wrn("number_focus_txt_color is ..\n");
			app_pbmp = dsk_theme_hdl2buf(number_menu_attr->number_focus_bmp[index]);		//通过传入的图片索引index数据赋值给app_pbmp空指针
			__wrn("app_pbmp is ..\n");
			app_bmp_x -= xOft;
			app_bmp_y -= yOft;
			__wrn("bfocus is 1\n");
		}
		else		//未选中图标
		{
			__wrn("bfocus is 0\n");
			GUI_SetColor(number_menu_attr->number_unfocus_txt_color);
			app_pbmp = dsk_theme_hdl2buf(number_menu_attr->number_unfocus_bmp[index]);	//通过传入的图片索引index绘制
		#if (APP_FOCUS_W >= APP_W) || (APP_FOCUS_H >= APP_H)
			GUI_ClearRect(app_bmp_x - yOft, app_bmp_y - yOft, app_bmp_x + APP_FOCUS_W - 1, app_bmp_y + APP_FOCUS_H - 1);	//删除选中过的app应用矩形显示
		#endif
		}

		GUI_BMP_Draw(app_pbmp, app_bmp_x, app_bmp_y);	//传入app_pbmp图片数据进行绘制
		{
			__wrn("GUI_BMP_Draw is succes\n");
			dsk_langres_get_menu_text(number_ui_para[index].lang_id, app_text, sizeof(app_text));	//文本显示函数
			__wrn("GUI_BMP_Draw  is 2 succes\n");
			//GUI_DispStringInRect(app_text, &app_gui_rect, GUI_TA_HCENTER | GUI_TA_TOP);					//字符串显示到app应用矩形
			__wrn("GUI_BMP_Draw  is 3 succes\n");
		}
	}
	#endif

	if(app_draw_mem)
	{
		__wrn("app_draw_mem is ...\n");
		GUI_MEMDEV_CopyToLCD(app_draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(app_draw_mem);
		app_draw_mem = NULL;
	}

}
/*====================================================================================
*显示全部共8个APP应用函数
=====================================================================================*/
void paint_numbermain_app_all(numbermenu_attr_t *number_menu_attr)
{
	__s32 i;
	GUI_MEMDEV_Handle	app_draw_mem;		//伪内存句柄变量
	__s32           screen_width;
	__s32 			screen_height;

	GUI_LyrWinSel(number_menu_attr->number_layer);	//图层窗口选择，需要显示到那个图层界面的意思
	GUI_SetFont(number_menu_attr->number_font);		//设置文本
	GUI_UC_SetEncodeUTF8();							//设置文本为utf8编码格式
	GUI_SetBkColor(0);								//设置文本控件的背景颜色
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);			//设置绘制的模式为正常绘画模式
	
	/* get lcd size*/
	dsk_display_get_size(&screen_width, &screen_height);					//获取lcd显示屏的大小，1024*600
	app_draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height);	//伪内存大小创建
	__wrn("app_draw_mem=0x%x\n", app_draw_mem);

	if(app_draw_mem)								//伪内存大小创建成功
	{
		GUI_MEMDEV_Select(app_draw_mem);			//伪内存选择
		__wrn("app_draw_mem is CREATE\n");
	}

	for(i = 0; i < MAX_ITEM_NUM; i++)				//最多显示8个app应用
	{
		if(i == number_menu_attr->number_focus_app)										//选中的app应用
		{
			number_menu_attr->number_paint_flag |= NUMBER_MASK_PaintFlag_MEMDEV_Nonuse;	//绘制标志的位操作
			paint_numbermain_one_app(number_menu_attr, i, 1);							//单个app应用绘制函数
		}
		else																			//没有选中的app应用
		{
			number_menu_attr->number_paint_flag |= NUMBER_MASK_PaintFlag_MEMDEV_Nonuse;	//绘制标志的位操作
			paint_numbermain_one_app(number_menu_attr, i, 0);							//单个app应用绘制函数
		}
	}

	if(app_draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(app_draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(app_draw_mem);
		app_draw_mem = NULL;
	}
	__wrn("paint_numbermain_app_all 8 CREATE\n");
}
























