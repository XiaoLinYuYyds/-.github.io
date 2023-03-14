/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: main_menu.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "app_home_i.h"
#include "home_uipara.h"
#include "main_menu.h"
#include "typedef.h"



typedef struct tag_res_id_t
{
	__s32 lang_id;
	__s32 unfocus_bmp_id;
	__s32 focus_bmp_id;
}
mm_res_id_t;

typedef struct tag_mmenu_attr
{
	HTHEME focus_bmp[MAX_ITEM_NUM];
	HTHEME unfocus_bmp[MAX_ITEM_NUM];
	char item_str[MAX_ITEM_NUM][128];
	RECT item_pos[MAX_ITEM_NUM];
	__s32 focus_item;
	__s32 first_item;
	__u8	  paint_flag;

	__s32 focus_txt_color;
	__s32 unfocus_txt_color;

	GUI_FONT *font;
	H_LYR	layer;
	H_LYR	bg_layer;
}
mmenu_attr_t;
	// for paint_flag:
	#define MASK_PaintFlag_MEMDEV_Nonuse					(1<<0)

//传入图片ID索引
static mm_res_id_t uipara_default[] =
{
	{STRING_HOME_MUSIC, ID_HOME_MAIN_MUSIC_LOSE_BMP, ID_HOME_MAIN_MUSIC_FOCUS_BMP},		//音乐；格式:第一位为语言，第二位为不选中图片，第三位为选中图片
	{STRING_HOME_MOVIE, ID_HOME_MAIN_MOVIE_LOSE_BMP, ID_HOME_MAIN_MOVIE_FOCUS_BMP},		//电影
#if 0//SP_APP_ATV
	{STRING_HOME_TV, ID_HOME_MAIN_TV_LOSE_BMP, ID_HOME_MAIN_TV_FOCUS_BMP},
	//{STRING_HOME_AVIN, ID_HOME_MAIN_AVIN_LOSE_BMP, ID_HOME_MAIN_AVIN_FOCUS_BMP},
#endif
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

static mm_res_id_t *main_get_ui_para(__s32 rotate)
{
	/*	__s32           screen_width;
		__s32 			screen_height;

		dsk_display_get_size(&screen_width, &screen_height);
		switch(rotate)
		{
		case GUI_SCNDIR_NORMAL:
		case GUI_SCNDIR_ROTATE180:
			{
				if((screen_width == 400 )&&( screen_height == 240))
					return uipara_400_240;
				else if((screen_width == 480 )&&( screen_height == 272))
					return uipara_480_272;
				else if((screen_width == 320 )&&( screen_height == 240))
					return uipara_320_240;
				else if((screen_width == 320 )&&( screen_height == 240))
					return uipara_800_480;
			}
		case GUI_SCNDIR_ROTATE90:
		case GUI_SCNDIR_ROTATE270:

			break;
		}
	*/
	return uipara_default;
}


/*
	申请使用图片资源
*/

static __s32 init_mmenu_res(mmenu_attr_t *mmenu_attr)
{
	__u32 	i;
	home_uipara_t *home_ui_para;
	mm_res_id_t *main_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());

	if(!home_ui_para)
	{
		__err("home_ui_para is null..\n");
		return EPDK_FAIL;
	}

	main_ui_para = main_get_ui_para(GUI_GetScnDir());

	if(!main_ui_para)
	{
		__err("main_ui_para is null...\n");
		return EPDK_FAIL;
	}

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(NULL == mmenu_attr->unfocus_bmp[i])
		{
			mmenu_attr->unfocus_bmp[i] = dsk_theme_open(main_ui_para[i].unfocus_bmp_id);	//不选择图片索引(unfocus_bmp_id)
		}

		if(NULL == mmenu_attr->focus_bmp[i])
		{
			mmenu_attr->focus_bmp[i] = dsk_theme_open(main_ui_para[i].focus_bmp_id);		//选择图片索引(focus_bmp_id)
		}
	}

	return EPDK_OK;
}


/*
	释放图片资源
*/

static __s32 uninit_mmenu_res(mmenu_attr_t *mmenu_attr)
{
	__u32 	i;

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(mmenu_attr->unfocus_bmp[i])
		{
			dsk_theme_close(mmenu_attr->unfocus_bmp[i]);
			mmenu_attr->unfocus_bmp[i] = NULL;
		}

		if(mmenu_attr->focus_bmp[i])
		{
			dsk_theme_close(mmenu_attr->focus_bmp[i]);
			mmenu_attr->focus_bmp[i]  = NULL;
		}
	}

	return EPDK_OK;
}
//图片绘制函数
static void paint_mmain_one_item(mmenu_attr_t *mmenu_attr, __s32 index, __s32 bfocus)
{
	GUI_RECT gui_rect;
	__u32 	i;
	void *pbmp;
	__s32 bmp_width;
	__s32 bmp_height;
	HBMP hbmp;
	__s32 bmp_x;
	__s32 bmp_y;
	home_uipara_t *home_ui_para;
	mm_res_id_t *main_ui_para;
	char text[256];
	GUI_MEMDEV_Handle draw_mem = NULL;
	__u8 paint_flag = mmenu_attr->paint_flag;
	
	home_ui_para = home_get_ui_para(GUI_GetScnDir());
	mmenu_attr->paint_flag = 0;

	if(!home_ui_para)
	{
		__err("home_ui_para is null..\n");
		return ;
	}

	main_ui_para = main_get_ui_para(GUI_GetScnDir());

	if(!main_ui_para)
	{
		__err("main_ui_para is null...\n");
		return ;
	}

	if(index < 0 || index >= MAX_ITEM_NUM)
	{
		__err("invalid para...\n");
		return ;
	}

	if(!(paint_flag & MASK_PaintFlag_MEMDEV_Nonuse))
	{
		__s32 screen_width, screen_height;
		/* get lcd size*/
		dsk_display_get_size(&screen_width, &screen_height);
		GUI_LyrWinSel(mmenu_attr->layer);
		GUI_SetFont(mmenu_attr->font);	//设置字体
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(0);
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
		draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height);
		__wrn("draw_mem=0x%x\n", draw_mem);
	}

	if(draw_mem)
	{
		GUI_MEMDEV_Select(draw_mem);
	}

	{
		GUI_RECT gui_rect;	//画矩形
		__u16 xOft, yOft;
		xOft = (W_ITEM_FOCUS -ITEM_W)/2;
		yOft = (H_ITEM_FOCUS -ITEM_H)/2;
		bmp_x = home_ui_para->item_pos[index].x;
		bmp_y = home_ui_para->item_pos[index].y;
		gui_rect.x0 = bmp_x-16;
		gui_rect.y0 = bmp_y +ITEM_H +24;
		gui_rect.x1 = gui_rect.x0 +ITEM_W -1 +32;
		gui_rect.y1 = gui_rect.y0+24 -1;
		

		if(bfocus)//选中图标
		{
			GUI_SetColor(mmenu_attr->focus_txt_color);
			pbmp = dsk_theme_hdl2buf(mmenu_attr->focus_bmp[index]);		//通过传入的图片索引index绘制
			bmp_x -= xOft;
			bmp_y -= yOft;
		}
		else//未选中图标
		{
			GUI_SetColor(mmenu_attr->unfocus_txt_color);
			pbmp = dsk_theme_hdl2buf(mmenu_attr->unfocus_bmp[index]);	//通过传入的图片索引index绘制
		#if (W_ITEM_FOCUS >= ITEM_W) || (H_ITEM_FOCUS >= ITEM_H)
			GUI_ClearRect(bmp_x - yOft, bmp_y - yOft, bmp_x + W_ITEM_FOCUS - 1, bmp_y + H_ITEM_FOCUS - 1);
		#endif
		}

		GUI_BMP_Draw(pbmp, bmp_x, bmp_y);	//图片绘制
		{
			dsk_langres_get_menu_text(main_ui_para[index].lang_id, text, sizeof(text));
			GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_TOP);
		}
	}

	if(draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(draw_mem);
		draw_mem = NULL;
	}
}

/*
	显示全部ITEM
*/

static void paint_mmain_item_all(mmenu_attr_t *mmenu_attr)
{
	__s32 i;
	GUI_MEMDEV_Handle	draw_mem;
	__s32           screen_width;
	__s32 			screen_height;

	GUI_LyrWinSel(mmenu_attr->layer);
	GUI_SetFont(mmenu_attr->font);
	GUI_UC_SetEncodeUTF8();
	GUI_SetBkColor(0);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	
	/* get lcd size*/
	dsk_display_get_size(&screen_width, &screen_height);
	draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height);
	__wrn("draw_mem=0x%x\n", draw_mem);

	if(draw_mem)
	{
		GUI_MEMDEV_Select(draw_mem);
	}

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(i == mmenu_attr->focus_item)
		{
			mmenu_attr->paint_flag |= MASK_PaintFlag_MEMDEV_Nonuse;	//标志的位操作
			paint_mmain_one_item(mmenu_attr, i, 1);
		}
		else
		{
			mmenu_attr->paint_flag |= MASK_PaintFlag_MEMDEV_Nonuse;	//标志的位操作
			paint_mmain_one_item(mmenu_attr, i, 0);
		}
	}

	if(draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(draw_mem);
		draw_mem = NULL;
	}
}
//主菜单按键响应处理函数
static __s32  mmenu_key_proc(__gui_msg_t *msg)
{
	static __s32 last_key = -1;
	static __s32 KEY_ENTER_NUMBER = 0;
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	//__gui_msg_t mmsg;
	//setting_ctrl_t *setting_ctrl;
	//setting_ctrl = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	//number_menu_number_para_t* tips_para;
	//GUI_RECT rect;
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		/*if (GUI_MSG_KEY_ENTER == last_key)
		{
			main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
		}*/
		__wrn("_main_menu_Proc msg->dwAddData2=%d\n", msg->dwAddData2);
		__wrn("_main_menu_Proc last_key=%d\n", last_key);
		mmenu_attr->first_item = 0;
		switch(last_key)
		{
			case GUI_MSG_KEY_ENTER:		//回车确认按键
			{
				/*if(mmenu_attr->focus_item == 6)	//ID_HOME_NUMBER播放确认按键点击音乐图标次数记录
				{
					KEY_ENTER_NUMBER += 1;
					if(KEY_ENTER_NUMBER == 3)
					{
						SUB_MENU_CUT = 1;
						//main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);//跳转进入选中的子窗口
						#if 0// 直接显示回车按键点击次数信息提示框
								number_menu_tip_para_t tips_para;
								RECT rect;
								rect.x = 0;
								rect.y = 0;
								rect.width = 191;
								rect.height = 101;
								tips_para.msgbox_font = setting1_ctrl->setting_font;
								tips_para.layer = number_menu_tips_layer_create(&rect,1);//图层创建
								setting1_ctrl->h_frm_tips = number_menu_tips_win_create(GUI_WinGetHandFromName(APP_SETTING), &tips_para);//窗口创建
								GUI_WinSetFocusChild(setting1_ctrl->h_frm_tips);
						#endif
						#if 0 //直接显示回车按键点击次数提示对话框
							mmsg->h_deswin = setting1_ctrl->h_frm_general;
							mmsg->h_srcwin = NULL;
							mmsg->id = ID_SWITCH_TO_TIPS;
							mmsg->dwReserved = msg->dwReserved;
							GUI_SendMessage(&mmsg);
						#endif
						#if 0
								rect.x0 = 0;
								rect.y0 = 0;
								rect.x1 = 191;
								rect.y1 = 101;
								//mmenu_attr.font = setting_ctrl->setting_font;
								mmenu_attr.layer = number_menu_8bpp_layer_create(&rect,1);
								mmenu_attr->h_parent = number_menu_numbers_win_create(GUI_WinGetHandFromName(APP_SETTING), &tips_para);
								GUI_WinSetFocusChild(mmenu_attr->h_parent);
						#endif 
						//number_menu_numbers_paint_all(msg,0);
						#if 0//直接发送新添加的通用设置app的id创建mainwin窗口
						__wrn("SUB_MENUNUMBER_ID is...ok \n");
						__wrn("MSG_NUMBER_MENU_OP_ENTER is... \n");
						__wrn("send ID_OP_NUMBER_MENU_ENTER is... \n");
						//main_cmd2parent(msg->h_deswin, ID_HOME_NEW_SETTING_NUMBER, 0, 0);//向上发一个进入ID_HOME_NEW_SETTING_NUMBER
						main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, NULL);//直接进入ID_HOME_NEW_SETTING_NUMBER,然后进入到新添加的通用设置界面
						#endif
					}
					if(KEY_ENTER_NUMBER == 3)//10次就销毁窗口
					{
						KEY_ENTER_NUMBER = 0;
						SUB_MENU_CUT = 0;
						//msg.id = GUI_MSG_DESTROY;//
					}
					__wrn("KEY_ENTER_NUMBER=%d\n", KEY_ENTER_NUMBER);
				}
				else*/
				{
					main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);//进入选中的子窗口
					__wrn("_main_menu_Proc mmenu_attr->focus_item=%d\n", mmenu_attr->focus_item);
				}
				__wrn("_main_menu_Proc mmenu_attr->focus_item=%d\n", mmenu_attr->focus_item);
				//#endif
				break;
			}

			case GUI_MSG_KEY_MODE:
			case GUI_MSG_KEY_AUDIO:		//音乐按键
			case GUI_MSG_KEY_LONGAUDIO:	//长按音乐按键
				__msg("++++++++main_menu to music++++++++++\n");
				mmenu_attr->focus_item = ID_HOME_MUSIC;
				main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
#if 0

			case GUI_MSG_KEY_VIDEO:
			case GUI_MSG_KEY_LONGVIDEO: //长按电影按键
				mmenu_attr->focus_item = ID_HOME_MOVIE;
				main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;

			case GUI_MSG_KEY_FMTUNER:	//收音按键
				//mmenu_attr->focus_item = ID_HOME_FM;
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				//main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
#endif

		#if (SP_FixedMic) || (SP_HotplugMic)
			case GUI_MSG_KEY_RECORD:	//录音按键
				__msg("++++++++main_menu to record++++++++++\n");
				mmenu_attr->focus_item = ID_HOME_RECORD;
				main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
		#endif

				/*case GUI_MSG_KEY_SETTING:
					mmenu_attr->focus_item = ID_HOME_OTHERS;
					main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
					main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
					break;*/
			default:
				break;
		}
	}
	else
	{
	__wrn("_main_menu_Proc msg->dwAddData1=%d\n", msg->dwAddData1);
		switch(msg->dwAddData1)
		{
#if 0

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:
			{
				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
				mmenu_attr->focus_item -= 1;

				if(0 == ((mmenu_attr->focus_item + 1) % ITEM_PER_LINE))
				{
					mmenu_attr->focus_item += ITEM_PER_LINE;
				}

				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
				//                paint_mmain_item_all(mmenu_attr);
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
			}

			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:
			{
				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
				mmenu_attr->focus_item += 1;

				if(0 == ((mmenu_attr->focus_item) % ITEM_PER_LINE))
				{
					mmenu_attr->focus_item -= ITEM_PER_LINE;
				}

				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
				//                paint_mmain_item_all(mmenu_attr);
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
			}

#else

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:		//长按左键
			{
				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
				mmenu_attr->focus_item--;

				if(mmenu_attr->focus_item < 0)
				{
					mmenu_attr->focus_item = MAX_ITEM_NUM - 1;
				}

				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
				//                paint_mmain_item_all(mmenu_attr);
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
			}
			break;

			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:		//长按右键
			{
				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
				mmenu_attr->focus_item++;

				if(mmenu_attr->focus_item >= MAX_ITEM_NUM)
				{
					mmenu_attr->focus_item = 0;
				}

				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
				//                paint_mmain_item_all(mmenu_attr);
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
			}
			break;
#endif

			case GUI_MSG_KEY_VADD:
			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:	//长按上按键
			{
				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
				mmenu_attr->focus_item -= ITEM_PER_LINE;

				if(mmenu_attr->focus_item < 0)
				{
					mmenu_attr->focus_item += MAX_ITEM_NUM;
				}

				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
				//                paint_mmain_item_all(mmenu_attr);
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
			}

			case GUI_MSG_KEY_VDEC:
			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:	//长按下按键
			{
				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
				mmenu_attr->focus_item += ITEM_PER_LINE;

				if(mmenu_attr->focus_item > MAX_ITEM_NUM - 1)
				{
					mmenu_attr->focus_item -= MAX_ITEM_NUM;
				}

				paint_mmain_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
				//                paint_mmain_item_all(mmenu_attr);
				//main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
				break;
			}
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
}

/*
主菜单应用程序
*/
static __s32 _main_menu_Proc(__gui_msg_t *msg)
{
/*	home_uipara_t *home_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());

	if(!home_ui_para)
	{
		__err("home_ui_para is null..\n");
		return ;
	}*/

	//__msg("_main_menu_Proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//    ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	__wrn("_main_menu_Proc msg->id=%d\n", msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//GUI主窗口创建应用程序
		{
			mmenu_attr_t *mmenu_attr;
			mmenu_para_t *mmenu_para;
			mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
			mmenu_attr = (mmenu_attr_t *)esMEMS_Balloc(sizeof(mmenu_attr_t));

			if(!mmenu_attr)
			{
				return EPDK_FAIL;
			}

			eLIBs_memset((void *)mmenu_attr, 0, sizeof(mmenu_attr_t));
			mmenu_attr->font = mmenu_para->mmenu_font;
			mmenu_attr->layer = mmenu_para->layer;
			mmenu_attr->bg_layer = mmenu_para->bg_layer;
			mmenu_attr->focus_item = 0;//mmenu_para->focus_id;
			mmenu_attr->first_item = 0;
			mmenu_attr->focus_txt_color = GUI_YELLOW;//APP_COLOR_YELLOW;
			mmenu_attr->unfocus_txt_color = GUI_WHITE;//APP_COLOR_WHITE;
			GUI_WinSetAddData(msg->h_deswin, (__s32)mmenu_attr);
			init_mmenu_res(mmenu_attr);	//申请使用图片资源
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:		//GUI主窗口关闭应用程序
		{
			GUI_FrmWinDelete(msg->h_deswin);
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY:	//1 GUI主窗口销毁应用程序
		{
			mmenu_para_t *mmenu_para;
			mmenu_attr_t *mmenu_attr;

			mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
			mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
			uninit_mmenu_res(mmenu_attr);	//释放申请使用的图片资源
			esMEMS_Bfree(mmenu_attr, sizeof(mmenu_attr_t));
			esMEMS_Bfree(mmenu_para, sizeof(mmenu_para_t));
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:		//GUI主窗口绘制应用程序图片
		{
			mmenu_attr_t *mmenu_attr;

			if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)))
			{
				__msg("layer not on, not need paint...\n");
				__msg("layer not on, not need paint...\n");
				__msg("layer not on, not need paint...\n");
				__msg("layer not on, not need paint...\n");
				return EPDK_OK;
			}

			mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!mmenu_attr)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			init_mmenu_res(mmenu_attr);
			paint_mmain_item_all(mmenu_attr);//显示全部图标
			return EPDK_OK;
		}

		case GUI_MSG_KEY:		//GUI主窗口应用程序按键功能
		{
			__here__;
			mmenu_key_proc(msg);
			return EPDK_OK;
		}

		case GUI_MSG_TOUCH:		//GUI主窗口应用程序触摸功能
		{
			//mmenu_touch_proc(msg);//触摸函数
			return EPDK_OK;
		}

		case GUI_MSG_TIMER:		//GUI主窗口应用程序定时器功能
		{
			return EPDK_OK;
		}

		case GUI_MSG_COMMAND:	//GUI主窗口应用程序命令输入功能
			return EPDK_OK;

		case GUI_MSG_WIN_WAKEUP:	//GUI主窗口唤醒功能
		{
			GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
			return EPDK_OK;
		}
	}

	return GUI_FrmWinDefaultProc(msg);
}
//主菜单framewin窗口创建函数
__s32 main_menu_win_create(H_WIN h_parent, mmenu_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	mmenu_para_t *mmenu_para;
	FB fb;
	GUI_LyrWinGetFB(para->layer, &fb);
	mmenu_para = (mmenu_para_t *)esMEMS_Balloc(sizeof(mmenu_para_t));

	if(!mmenu_para)
	{
		return EPDK_FAIL;
	}

	eLIBs_memset((void *)mmenu_para, 0, sizeof(mmenu_para_t));
	mmenu_para->mmenu_font = para->mmenu_font;
	mmenu_para->layer = para->layer;
	mmenu_para->bg_layer = para->bg_layer;
	mmenu_para->focus_id = para->focus_id;
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name =	"main_menu_win",
	              framewin_para.dwExStyle = WS_EX_NONE;
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;
	framewin_para.id         = MAIN_MENU_ID;
	framewin_para.hHosting = h_parent;
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_main_menu_Proc);
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width = fb.size.width;
	framewin_para.rect.height = fb.size.height;
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)mmenu_para;
	framewin_para.hLayer = para->layer;
	return (GUI_FrmWinCreate(&framewin_para));
}
//主菜单framewin窗口删除函数
void main_menu_win_delete(H_WIN mm_win)
{
	GUI_FrmWinDelete(mm_win);
}
//主菜单窗口不初始化函数
void main_menu_uninit_res(H_WIN mm_win)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(mm_win);

	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return ;
	}

	uninit_mmenu_res(mmenu_attr);
}


