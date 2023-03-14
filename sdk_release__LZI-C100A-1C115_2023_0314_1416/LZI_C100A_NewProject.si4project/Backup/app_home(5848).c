/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_main.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "app_home.h"
#include "main_menu.h"
#include "sub_menu.h"
#include "home_uipara.h"
#include "msgbox.h"
#include "app_home_i.h"

#include "number_menu_tips.h"//新窗口

#define HOME_USE_HOME_LOGO 0
__s32 SUB_MENU_CUT = 0;//子菜单切换

typedef struct tag_home_para
{
	H_WIN	h_app_main;
	H_WIN 	h_mmenu;
	H_WIN 	h_submenu;

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
home_para_t;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
//主菜单图层像素点，32bpp：每个像素点占据32位双字节.屏幕色彩指标
static H_LYR home_32bpp_layer_create(RECT *rect, __s32 pipe)
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
		"main menu layer",
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
//子菜单图层像素点，8bpp：每个像素点占据8位双字节.屏幕色彩指标
static H_LYR home_8bpp_layer_create(RECT *rect, __s32 pipe)
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
		"sub menu layer",
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

//应用退出到桌面时设置回当前背景
void __app_home_change_to_home_bg(void)
{
	reg_init_para_t *para;
	para = dsk_reg_get_para_by_app(REG_APP_INIT);

	if(para)
	{
		__s32 ret;
		ret = gscene_bgd_get_flag();

		if(ret != para->bgd_flag_in_main)
		{
			gscene_bgd_set_flag(para->bgd_flag_in_main);
			gscene_bgd_refresh();
		}
		else
		{
			__msg("ret == para->bgd_flag_in_main, not need change bg...\n");
		}
	}
	else
	{
		__msg("dsk_reg_get_para_by_app fail...\n");
	}

	gscene_bgd_set_state(BGD_STATUS_SHOW);
}

void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = GUI_WinGetParent(hwin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);//GUI发送管理通知
}

void  main_cmd_child(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = hwin;
	msg.h_srcwin = NULL;
	msg.id = id;
	msg.dwAddData1 = data1;
	msg.dwAddData2 = data2;
	msg.dwReserved = 0;
	GUI_SendNotifyMessage(&msg);
}
//检查磁盘
static void check_disk(home_para_t *home_para)
{
	__s32 i;
	__s32 ret;
	__s32 cnt;
	char diskname[RAT_MAX_PARTITION][4];
	{
		__target_para_t target;
		esKSRV_GetTargetPara(&target);

		if(target.debugenable == EPDK_TRUE)////如果是调试，则默认为有外设//112350
		{
			home_para->root_type = RAT_TF << 8;
			return ;
		}
	}
	__msg("**********check_disk**********\n");
	cnt = 0;
	home_para->root_type = RAT_UNKNOWN;
	ret = rat_get_partition_name(RAT_USB_DISK, diskname);

	if(EPDK_OK == ret)
	{
		i = 0;
		//while (i < RAT_MAX_PARTITION)
		{
			if(diskname[i][0])
			{
				__msg("SD : diskname[%d]=%s\n", i, diskname[i]);
				home_para->root_type += RAT_USB;
				cnt++;
			}

			i++;
		}
	}

	ret = rat_get_partition_name(RAT_SD_CARD, diskname);

	if(EPDK_OK == ret)
	{
		i = 0;
		//while (i < RAT_MAX_PARTITION)
		{
			if(diskname[i][0])
			{
				__msg("SD : diskname[%d]=%s\n", i, diskname[i]);
				home_para->root_type += RAT_TF << 8;
				cnt++;
			}

			i++;
		}
	}
}
#if 1	//新的子菜单创建函数
//app新的子菜单创建函数
static void app_newsub_menu_create(home_para_t *home_para)
{	
	number_menu_number_para_t smenu_para;
	RECT	rect;
	home_uipara_t *home_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());
	if(!home_ui_para)
	{
		__err("home_ui_para is null...\n");
		return ;
	}

	rect.x = 0;//(home_para->focus_id - home_para->first_item) * home_ui_para->item_width;
	rect.y = 0;//home_ui_para->lyr_sub_item.y;
	rect.width = 200;//home_ui_para->lyr_sub_item.w;
	rect.height = 200;//..home_ui_para->lyr_sub_item.h;

	if(rect.x + rect.width >= home_ui_para->lyr_main_menu.w)
	{
		rect.x = home_ui_para->lyr_main_menu.w - rect.width;
	}
	__wrn("newsub_menu rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n",
	      rect.x, rect.y, rect.width, rect.height);
	home_para->lyr_smenu = number_menu_32bpp_layer_create(&rect,1);//图层创建
	if(!home_para->lyr_smenu)//检测图层是否创建成功
	{
		__err("home_para->lyr_smenu is null\n");
		return;
	}
	smenu_para.layer = home_para->lyr_smenu;
	smenu_para.sub_menu_id = home_para->focus_id;
	smenu_para.smenu_font = home_para->main_font;
	smenu_para.root_type = home_para->root_type;
	smenu_para.focus_item = home_para->focus_smenu_item;
	__msg("home_para->focus_smenu_item=%d\n", home_para->focus_smenu_item);
	home_para->h_submenu = number_menu_numbers_win_create(home_para->h_app_main, &smenu_para);//sub_menu_win_create(home_para->h_app_main, &smenu_para);
	__msg("home_para->h_submenu=%x\n", home_para->h_submenu);
     GUI_WinSetFocusChild(home_para->h_submenu);
	if(!home_para->h_submenu)//检测窗口是否创建成功
	{
		__err("home_para->h_submenu is null\n");
		return;
	}
}
#endif
//app子菜单创建函数
static void app_sub_menu_create(home_para_t *home_para)
{	
	#if 0//SUB_MENU_CUT
	smenunumber_para_t smenu_para;
	#else
	smenu_para_t smenu_para;
	#endif
	RECT	rect;
	home_uipara_t *home_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());

	if(!home_ui_para)
	{
		__err("home_ui_para is null...\n");
		return ;
	}

	rect.x = (home_para->focus_id - home_para->first_item) * home_ui_para->item_width;
	rect.y = home_ui_para->lyr_sub_item.y;
	rect.width = home_ui_para->lyr_sub_item.w;
	rect.height = home_ui_para->lyr_sub_item.h;

	if(rect.x + rect.width >= home_ui_para->lyr_main_menu.w)
	{
		rect.x = home_ui_para->lyr_main_menu.w - rect.width;
	}
	__wrn("sub_menu rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n",
	      rect.x, rect.y, rect.width, rect.height);
	home_para->lyr_smenu = home_8bpp_layer_create(&rect, 1);
	smenu_para.layer = home_para->lyr_smenu;
	smenu_para.sub_menu_id = home_para->focus_id;
	smenu_para.smenu_font = home_para->main_font;
	smenu_para.root_type = home_para->root_type;
	smenu_para.focus_item = home_para->focus_smenu_item;
	__msg("home_para->focus_smenu_item=%d\n", home_para->focus_smenu_item);
	#if 0//SUB_MENU_CUT	//新的
	home_para->h_submenu = sub_menu_winnumber_create(home_para->h_app_main, &smenu_para);//
	#else	//原来的
	home_para->h_submenu = sub_menu_win_create(home_para->h_app_main, &smenu_para);//	
	#endif
	__msg("home_para->h_submenu=%x\n", home_para->h_submenu);
}
//app主菜单创建函数
static void app_main_menu_create(home_para_t *home_para)
{
	RECT	rect;
	mmenu_para_t mmenu_para;
	home_uipara_t *home_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());

	if(!home_ui_para)
	{
		__err("home_ui_para is null...\n");
		return ;
	}

	rect.x = home_ui_para->lyr_main_menu.x;
	rect.y = home_ui_para->lyr_main_menu.y;
	rect.width = home_ui_para->lyr_main_menu.w;
	rect.height = home_ui_para->lyr_main_menu.h;
	__wrn("rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n",
	      rect.x, rect.y, rect.width, rect.height);
	home_para->lyr_mmenu = home_32bpp_layer_create(&rect, 0);
	home_para->lyr_forground = home_32bpp_layer_create(&rect, 1);
	mmenu_para.mmenu_font = home_para->main_font;
	mmenu_para.focus_id = home_para->focus_id;
	mmenu_para.layer = home_para->lyr_forground;
	mmenu_para.bg_layer = home_para->lyr_mmenu;
	#if 1
	home_para->h_mmenu = main_menu_win_create(home_para->h_app_main, &mmenu_para);
	#else
	home_para->h_mmenu = number_menu_numbers_win_create(home_para->h_app_main, (number_menu_number_para_t*)&mmenu_para);
	#endif
	__msg("home_para->h_mmenu=%x\n", home_para->h_mmenu);
	GUI_WinSetFocusChild(home_para->h_mmenu);
}
//home应用程序
static __s32 app_home_proc(__gui_msg_t *msg)
{
	//  __msg("app_home_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//      ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//菜单创建
		{
			home_para_t *home_para;
			root_para_t *root_para;
			root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
			home_para = (home_para_t *)esMEMS_Balloc(sizeof(home_para_t));

			if(!home_para)
			{
				return EPDK_FAIL;
			}

			eLIBs_memset((void *)home_para, 0, sizeof(home_para_t));
			home_para->main_font = root_para->font;
			home_para->h_app_main = msg->h_deswin;
			home_para->focus_id = 2;
			home_para->first_item = 0;
			//com_set_palette_by_id(ID_HOME_PAL_BMP);
			//PT_LYR_INFO;
			gscene_bgd_set_state(BGD_STATUS_HIDE);
			//主菜单
			//..app_main_menu_create(home_para);
			//建立一个子菜单
			check_disk(home_para);
			#if 0
			app_sub_menu_create(home_para);
			#else
			app_newsub_menu_create(home_para);//新添加窗口
			#endif
#if 1 == HOME_USE_HOME_LOGO
			GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_ON);
#else
#endif
			//..GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_ON);
			//..GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_ON);
			//GUI_LyrWinSetTop(home_para->lyr_mmenu);
			//..GUI_LyrWinSetTop(home_para->lyr_forground);
			gscene_hbar_set_state(HBAR_ST_HIDE);
#if 1 == HOME_USE_HOME_LOGO
			gscene_bgd_set_state(BGD_STATUS_HIDE);
#else
			gscene_bgd_set_state(BGD_STATUS_SHOW);//BGD_STATUS_SHOW
#endif

			//gscene_bgd_set_state(BGD_STATUS_SHOW);

			if(0)
			{
				GUI_LyrWinSel(home_para->lyr_mmenu);
				GUI_SetBkColor(GUI_RED);
				GUI_Clear();
			}

#if 1 == HOME_USE_HOME_LOGO

			if(1)
			{
				__s32 i;
				__u8 *pbmp;
				HTHEME logo_bmp_hand = dsk_theme_open(ID_HOME_HOMELOGO_BMP);

				if(logo_bmp_hand)
				{
					pbmp = dsk_theme_hdl2buf(logo_bmp_hand);
					__wrn("pbmp=0x%x\n", pbmp);

					for(i = 0 ; i < 54 ; i++)
					{
						__inf("0x%x ", pbmp[i]);
					}

					if(pbmp)
					{
						GUI_LyrWinSel(home_para->lyr_mmenu);
						GUI_ARGB_Draw(pbmp, 0, 0);
					}

					dsk_theme_close(logo_bmp_hand);
				}
			}

#else
#endif
			//            gscene_hbar_set_state(HBAR_ST_SHOW);
			//			gscene_hbar_set_state(HBAR_ST_HIDE);
			//gscene_hbar_set_state(HBAR_ST_SHOW);
			gscene_bgd_set_bottom();
			GUI_WinSetFocusChild(home_para->h_mmenu);
			GUI_WinSetAddData(msg->h_deswin, (__u32)home_para);
			__here__;

			//PT_LYR_INFO;

			//test volume
			if(0)
			{
				reg_system_para_t *para;
				__s32 vol;
				vol = dsk_volume_get();
				__msg("dsk_volume_get=%d\n", vol);
				para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);

				if(para)
				{
					__msg("para->volume=%d\n", para->volume);
				}
				else
				{
					__msg("para is null...\n");
				}
			}
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:		//菜单关闭
			GUI_ManWinDelete(msg->h_deswin);
			return EPDK_OK;

		case GUI_MSG_DESTROY:	//菜单销毁
		{
			home_para_t *home_para;
			home_para = (home_para_t *)GUI_WinGetAddData(msg->h_deswin);

			if(home_para->lyr_mmenu)
			{
				GUI_LyrWinDelete(home_para->lyr_mmenu);
			}

			if(home_para->lyr_smenu)
			{
				GUI_LyrWinDelete(home_para->lyr_smenu);
			}

			if(home_para->lyr_forground)
			{
				GUI_LyrWinDelete(home_para->lyr_forground);
			}

			home_para->lyr_mmenu = 0;
			home_para->lyr_smenu = 0;
			home_para->lyr_forground = 0;
			esMEMS_Bfree(home_para, sizeof(home_para_t));
		}

		return EPDK_OK;

		case GUI_MSG_KEY:		//按键
			__msg("******app_home_proc GUI_MSG_KEY*****\n");
			break;

		case GUI_MSG_TOUCH:		//触摸
			break;

		case DSK_MSG_FS_PART_PLUGIN:
		case DSK_MSG_FS_PART_PLUGOUT:
			return EPDK_OK;

		case DSK_APP_RESTORE:	//复位
		{
			home_para_t *home_para;
			home_para = (home_para_t *)GUI_WinGetAddData(msg->h_deswin);
#if 1 == HOME_USE_HOME_LOGO
			gscene_bgd_set_state(BGD_STATUS_HIDE);
#else
			gscene_bgd_set_state(BGD_STATUS_SHOW);
#endif
			g_display_switch_output(LION_DISP_LCD);
			//com_set_palette_by_id(ID_HOME_PAL_BMP);
			//__app_home_change_to_home_bg();
#if 1 == HOME_USE_HOME_LOGO
			GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_ON);
#else
#endif
			GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_ON);
			GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_ON);
			//GUI_LyrWinSetTop(home_para->lyr_mmenu);
			GUI_LyrWinSetTop(home_para->lyr_forground);
			gscene_hbar_set_state(HBAR_ST_SHOW);

			/*
			#if 1 == HOME_USE_HOME_LOGO

						gscene_bgd_set_state(BGD_STATUS_HIDE);
			#else

						gscene_bgd_set_state(BGD_STATUS_SHOW);
			#endif
			*/

			if(0)
			{
				GUI_LyrWinSel(home_para->lyr_mmenu);
				GUI_SetBkColor(GUI_RED);
				GUI_Clear();
			}

#if 1 == HOME_USE_HOME_LOGO

			if(1)
			{
				void *pbmp;
				HTHEME logo_bmp_hand = dsk_theme_open(ID_HOME_HOMELOGO_BMP);

				if(logo_bmp_hand)
				{
					pbmp = dsk_theme_hdl2buf(logo_bmp_hand);
					__wrn("pbmp=0x%x\n", pbmp);

					if(pbmp)
					{
						GUI_LyrWinSel(home_para->lyr_mmenu);
						GUI_ARGB_Draw(pbmp, 0, 0);
					}

					dsk_theme_close(logo_bmp_hand);
				}
			}

#else
#endif
			//            gscene_hbar_set_state(HBAR_ST_SHOW);
			//            gscene_bgd_set_bottom();
			//            gscene_hbar_set_state(HBAR_ST_SHOW);
			//            GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_ON);
			//            GUI_LyrWinSetBottom(home_para->lyr_total_bg);
			//            GUI_BMP_Draw(dsk_theme_hdl2buf(home_para->total_bg_hdl)
			//               , main_uipara.lyr_total_bg.x, main_uipara.lyr_total_bg.y);
			GUI_WinSetFocusChild(home_para->h_mmenu);
		}

		return EPDK_OK;

		case GUI_MSG_COMMAND:	//命令输入
		{
			home_para_t *home_para;
			home_para = (home_para_t *)GUI_WinGetAddData(msg->h_deswin);

			switch(LOWORD(msg->dwAddData1))
			{
				case MAIN_MENU_ID:	//主菜单ID，意思是在主菜单时候进入执行下面的程序
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_SWITCH_ITEM:
						{
							//先挡住子菜单，解决子菜单切换时闪的问题。
							//GUI_LyrWinSetTop(home_para->lyr_mmenu);//112357
							//gscene_hbar_set_state(HBAR_ST_SHOW);//112357
							if(home_para->lyr_smenu)
							{
								GUI_LyrWinDelete(home_para->lyr_smenu);
							}

							home_para->focus_id = msg->dwAddData2;
							home_para->first_item = msg->dwReserved;
							app_sub_menu_create(home_para);
							GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SLEEP);
							//                        GUI_LyrWinSetTop(home_para->lyr_smenu);
						}
						break;

						case ID_OP_SUB_UP:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_SUB_OP_UP;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_SUB_DOWN:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_SUB_OP_DOWN;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_SUB_ENTER:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = home_para->h_submenu;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_SUB_OP_ENTER;
							mymsg.dwAddData2 = msg->dwAddData2;
							__wrn("msg->dwAddData2=%d\n", msg->dwAddData2);
							GUI_SendMessage(&mymsg);
						}
						break;

						default:
							break;
					}
				}
				break;

				case SUB_MENU_ID:	//子菜单ID，意思是在进入子菜单后进行执行下面的程序
				{
					if(HIWORD(msg->dwAddData1) == SMENU_APP_SWITCH)
					{
						__wrn("msg->dwReserved=%d\n", msg->dwReserved);
						home_para->focus_id = msg->dwReserved;
						home_para->focus_smenu_item = msg->dwAddData2;
						__wrn("msg->dwAddData2=%d\n", msg->dwAddData2);
						GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
						__wrn("home_para->focus_id = %d\n", home_para->focus_id);

						switch(home_para->focus_id)
						{
							case ID_HOME_FM:
						#if SP_APP_ATV
							case ID_HOME_ATV:
							case ID_HOME_AVIN:
							case ID_HOME_TV:
							//case ID_HOME_DVD:
						#endif
							{
								if(home_para->lyr_smenu)
								{
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								}

								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								//                            GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, home_para->focus_id, msg->dwAddData2);
							}
							break;

							case ID_HOME_PHOTO:
							/*case ID_HOME_MUSIC:*/
							case ID_HOME_MOVIE:
							case ID_HOME_EBOOK:
						#if (SP_FixedMic) || (SP_HotplugMic)
							case ID_HOME_RECORD:
						#endif
							{
								__here__;
								check_disk(home_para);
								__msg("home_para->root_type = %d", home_para->root_type);

								if(home_para->root_type != 0)
								{
									if(home_para->lyr_smenu)
									{
										GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
									}

									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
									main_menu_uninit_res(home_para->h_mmenu);

									//                                GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
									if((home_para->root_type & 0xff) //USB
									    && (home_para->root_type & 0xff00)) //SD
									{
										root_para_t *root_para;
										root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);

										switch(root_para->log & MASK_RootLog__LastFs)
										{
											case OP_RootLog__LastFs_SD:
												main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, home_para->focus_id, RAT_TF);
												break;

											case OP_RootLog__LastFs_UD:
											default :
												main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, home_para->focus_id, RAT_USB);
												break;
										}
									}
									else if(home_para->root_type & 0xff) //USB
									{
										main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP
										                , home_para->focus_id, RAT_USB);
									}
									else if(home_para->root_type & 0xff00) //TF
									{
										main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP
										                , home_para->focus_id, RAT_TF);
									}
									else
									{
										__err("wrong root type..\n");
									}
								}
								else
								{
									if(0)
									{
										__msg(" no sd or usb ,no disk insert, then start to play fm\n");

										if(home_para->lyr_smenu)
										{
											GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
										}

										GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
										GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
										main_menu_uninit_res(home_para->h_mmenu);
										//                            GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
										main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_FM, 0);
									}
								}

								__here__;
								break;
							}

#if SP_APP_CALENDAR

							case ID_HOME_CALENDAR:
							{
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_OTHERS, 3);
								break;
							}

#endif

							case ID_HOME_EXPORER:
							{
								check_disk(home_para);
								__msg("home_para->root_type = %d", home_para->root_type);

								if(home_para->root_type != 0)
								{
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
									//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
									main_menu_uninit_res(home_para->h_mmenu);
									main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_OTHERS, 2);
								}
								else
								{
									__wrn("no disk currently...\n");
								}

								break;
							}

							case ID_HOME_GENERAL_SETTING:
							{
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_OTHERS, 0);
								break;
							}
							case ID_HOME_NUMBER://新添加
							{
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NUMBER, 0);//进入通用设置子菜单
								break;
							}

							/*
							case ID_HOME_POWER_SETTING:
								{
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
									//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
									main_menu_uninit_res(home_para->h_mmenu);
									main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_OTHERS, 1);
									break;
								}
							*/
						}
					}
				}
				break;
				#if 0
				case SUB_MENUNUMBER_ID://进入新窗口ID
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_OP_LEFT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_LEFT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_RIGHT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_RIGHT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_UP:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_UP;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_DOWN:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_DOWN;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_ENTER:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_ENTER;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_OP_EXIT:
						{
							__gui_msg_t mymsg;
							mymsg.h_deswin = setting_ctrl->h_frm_tips;
							mymsg.h_srcwin = NULL;
							mymsg.id = MSG_OP_EXIT;
							GUI_SendMessage(&mymsg);
						}
						break;

						case ID_SWITCH_TO_GENERAL:
						{
							H_LYR tips_win_layer;

							if(setting_ctrl == NULL)
							{
								__msg("setting_ctrl error.........\n");
								return EPDK_FAIL;
							}

							if(setting_ctrl->h_frm_tips == NULL)
							{
								__msg(" setting_ctrl->h_frm_tips is null \n");
								GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
								return EPDK_FAIL ;
							}

							tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_tips);

							if(tips_win_layer == NULL)
							{
								__msg(" tips_win layer is null \n");
								return EPDK_FAIL ;
							}

							GUI_LyrWinDelete(tips_win_layer);
							setting_ctrl->h_frm_tips = NULL ;
							GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
						}
						break;

						default:
							break;
					}
				}
				break;
				#endif
				
				case MSGBOX_ID:
				{
					if(HIWORD(msg->dwAddData1) == CMD_MSGBOX_CLOSE)
					{}
				}

				default:
					break;
			}
		}

		return EPDK_OK;
	}

	return GUI_ManWinDefaultProc(msg);
}

/*******************************************************************************************************
*********************************************************************************************************/
//home窗口创建
H_WIN app_home_create(root_para_t *para)
{
	__gui_manwincreate_para_t create_info;
	__inf("****************************************************************************************\n");
	__inf("********  enter home app  **************\n");
	__inf("****************************************************************************************\n");
	//__wait__;
	g_display_switch_output(LION_DISP_LCD);
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name            = APP_HOME;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_home_proc);
	create_info.attr            = (void *)para;
	create_info.id				= APP_HOME_ID;
	create_info.hHosting        = NULL;
	return(GUI_ManWinCreate(&create_info));
}
//
multi_screen_home_mode_e app_multi_screen_home_get_mode(H_WIN hwin)
{
	home_para_t *home_para;

	if(NULL == hwin)
	{
		__err("invalid para...\n");
		return MULTI_SCREEN_HOME_MODE_NORMAL;
	}

	home_para = (home_para_t *)GUI_WinGetAddData(hwin);

	if(NULL == home_para || home_para->lyr_mmenu)
	{
		__msg("home_para is null...\n");
		return MULTI_SCREEN_HOME_MODE_NORMAL;
	}

	if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(home_para->lyr_mmenu))
	{
		return MULTI_SCREEN_HOME_MODE_NORMAL;
	}
	else
	{
		return MULTI_SCREEN_HOME_MODE_BACKGROUND;
	}
}


