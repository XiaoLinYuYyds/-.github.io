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
#if 1
#include "number_menu_tips.h"//�´���
#include "New_Setting.h"//�������Ӵ���
#endif
#define HOME_USE_HOME_LOGO 0
__s32 SUB_MENU_CUT = 0;//�Ӳ˵��л�

typedef struct tag_home_para
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
home_para_t;

/***********************************************************************************************************
	����ͼ��
************************************************************************************************************/
//���˵�ͼ�����ص㣬32bpp��ÿ�����ص�ռ��32λ˫�ֽ�.��Ļɫ��ָ��
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
//�Ӳ˵�ͼ�����ص㣬8bpp��ÿ�����ص�ռ��8λ˫�ֽ�.��Ļɫ��ָ��
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

//Ӧ���˳�������ʱ���ûص�ǰ����
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
	__wrn("**********main_cmd2parent**********\n");
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);//GUI���͹���֪ͨ
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
//������
static void check_disk(home_para_t *home_para)
{
	__s32 i;
	__s32 ret;
	__s32 cnt;
	char diskname[RAT_MAX_PARTITION][4];
	{
		__target_para_t target;
		esKSRV_GetTargetPara(&target);

		if(target.debugenable == EPDK_TRUE)////����ǵ��ԣ���Ĭ��Ϊ������//112350
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
#if 0	//�µ��Ӳ˵���������
//app�µ��Ӳ˵���������
static void app_newsetting_sub_menu_create(home_para_t *home_para)
{	
	#if 0//ԭ����
	New_Setting_para_t new_smenu_para;
	#else//�µĽṹ��
	new_setting_para_t new_smenu_para;
	#endif
	RECT	rect;
	home_uipara_t *home_ui_para;
	//numbers_uipara_t *home_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());//��ȡ���ڴ�С number_get_numbers_para(GUI_GetScnDir());//
	if(!home_ui_para)
	{
		__err("home_ui_para is null...\n");
		return ;
	}
	#if 1	//����ȷ�Ͼ��εĴ�С
	rect.x = 0;//(home_para->focus_id - home_para->first_item) * home_ui_para->item_width;
	rect.y = 0;//home_ui_para->lyr_sub_item.y;
	rect.width = 800;//home_ui_para->lyr_sub_item.w;
	rect.height = 480;//home_ui_para->lyr_sub_item.h;
	#else	//ͨ����ȡlcd��ʾ���ߴ�Ĵ�С
	rect.x = home_ui_para->lyr_sub_item.x;		//����������x������ 0
	rect.y = home_ui_para->lyr_sub_item.y;		//����������y������ 0
	rect.width = home_ui_para->lyr_sub_item.w;	//���������ڿ�	ԭʼ��1024
	rect.height = home_ui_para->lyr_sub_item.h;//���������ڸ� ԭʼ��600
	__wrn("rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n", rect.x, rect.y, rect.width, rect.height);
	#endif
	if(rect.x + rect.width >= home_ui_para->lyr_sub_item.w)
	{
		//rect.x = home_ui_para->lyr_main_menu.w - rect.width;
	}
	__wrn("newsub_menu rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n",
	      rect.x, rect.y, rect.width, rect.height);
	//home_para->lyr_smenu = number_menu_32bpp_layer_create(&rect,1);//ͼ�㴴��
	home_para->lyr_smenu = home_8bpp_layer_create(&rect,1);//ͼ�㴴��
	if(!home_para->lyr_smenu)//���ͼ���Ƿ񴴽��ɹ�
	{
		__err("home_para->lyr_smenu is null\n");
		return;
	}
	new_smenu_para.layer = home_para->lyr_smenu;				//һͼ�㸳ֵ��û��ѡ��appӦ��ǰ��ͼ��
	#if 0
	new_smenu_para.sub_menu_id = home_para->focus_id;			//ѡ�е�appӦ�ô���ID
	new_smenu_para.smenu_font = home_para->main_font;			//����
	new_smenu_para.root_type = home_para->root_type;			//
	new_smenu_para.focus_item = home_para->focus_smenu_item;	//ѡ�е�����
	new_smenu_para.bg_layer = home_para->lyr_smenu;				//��ͼ�㣬ѡ��appӦ�õĵı�Ӱͼ��
	#endif
	__msg("home_para->focus_smenu_item=%d\n", home_para->focus_smenu_item);
	home_para->h_submenu = New_Setting_app_win_create(home_para->h_app_main, &new_smenu_para);
	__msg("home_para->h_submenu=%x\n", home_para->h_submenu);
     GUI_WinSetFocusChild(home_para->h_submenu);			//����ͣ�����ִ����Ҫ����
	if(!home_para->h_submenu)								//��ⴰ���Ƿ񴴽��ɹ�
	{
		__err("home_para->h_submenu is null\n");
		return;
	}
}
#endif

#if 0	//�µ����˵���������
//app�µ����˵���������
static void app_newsub_menu_create(home_para_t *home_para)
{	
	number_menu_number_para_t smenu_para;
	RECT	rect;
	home_uipara_t *home_ui_para;
	//numbers_uipara_t *home_ui_para;
	home_ui_para = home_get_ui_para(GUI_GetScnDir());//��ȡ���ڴ�С number_get_numbers_para(GUI_GetScnDir());//
	if(!home_ui_para)
	{
		__err("home_ui_para is null...\n");
		return ;
	}
	#if 0	//����ȷ�Ͼ��εĴ�С
	rect.x = 0;//(home_para->focus_id - home_para->first_item) * home_ui_para->item_width;
	rect.y = 0;//home_ui_para->lyr_sub_item.y;
	rect.width = 800;//home_ui_para->lyr_sub_item.w;
	rect.height = 480;//home_ui_para->lyr_sub_item.h;
	#else	//ͨ����ȡlcd��ʾ���ߴ�Ĵ�С
	rect.x = home_ui_para->lyr_main_menu.x;		//����������x������
	rect.y = home_ui_para->lyr_main_menu.y;		//����������y������
	rect.width = home_ui_para->lyr_main_menu.w;	//���������ڿ�	ԭʼ��1024
	rect.height = home_ui_para->lyr_main_menu.h;//���������ڸ� ԭʼ��600
	__wrn("rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n", rect.x, rect.y, rect.width, rect.height);
	#endif
	if(rect.x + rect.width >= home_ui_para->lyr_main_menu.w)
	{
		//rect.x = home_ui_para->lyr_main_menu.w - rect.width;
	}
	__wrn("newsub_menu rect.x=%d, rect.y=%d, rect.width=%d, rect.height=%d\n",
	      rect.x, rect.y, rect.width, rect.height);
	home_para->lyr_mmenu = number_menu_32bpp_layer_create(&rect,1);//ͼ�㴴��
	if(!home_para->lyr_mmenu)//���ͼ���Ƿ񴴽��ɹ�
	{
		__err("home_para->lyr_smenu is null\n");
		return;
	}
	smenu_para.layer = home_para->lyr_mmenu;				//һͼ�㸳ֵ��û��ѡ��appӦ��ǰ��ͼ��
	smenu_para.sub_menu_id = home_para->focus_id;			//ѡ�е�appӦ�ô���ID
	smenu_para.smenu_font = home_para->main_font;			//����
	smenu_para.root_type = home_para->root_type;			//
	smenu_para.focus_item = home_para->focus_smenu_item;	//ѡ�е�����
	smenu_para.bg_layer = home_para->lyr_mmenu;				//��ͼ�㣬ѡ��appӦ�õĵı�Ӱͼ��
	
	__msg("home_para->focus_smenu_item=%d\n", home_para->focus_smenu_item);
	home_para->h_mmenu = number_menu_numbers_win_create(home_para->h_app_main, &smenu_para);//sub_menu_win_create(home_para->h_app_main, &smenu_para);
	__msg("home_para->h_mmenu=%x\n", home_para->h_mmenu);
     GUI_WinSetFocusChild(home_para->h_mmenu);			//����ͣ�����ִ����Ҫ����
	if(!home_para->h_mmenu)								//��ⴰ���Ƿ񴴽��ɹ�
	{
		__err("home_para->h_mmenu is null\n");
		return;
	}
}
#endif
//app�Ӳ˵���������
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
	#if 0//SUB_MENU_CUT	//�µ�
	home_para->h_submenu = sub_menu_winnumber_create(home_para->h_app_main, &smenu_para);//
	#else	//ԭ����
	home_para->h_submenu = sub_menu_win_create(home_para->h_app_main, &smenu_para);//	
	#endif
	__msg("home_para->h_submenu=%x\n", home_para->h_submenu);
}
//app���˵���������
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
//homeӦ�ó���
static __s32 app_home_proc(__gui_msg_t *msg)
{
	//  __msg("app_home_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//      ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//�˵�����
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
			//���˵�
			#if 1//ԭ����
			app_main_menu_create(home_para);
			#else//�µ�������
			app_newsub_menu_create(home_para);//������������
			#endif
			//����һ���Ӳ˵�
			//check_disk(home_para);
			#if 1//ԭ����
			app_sub_menu_create(home_para);
			#else//�µ��Ӵ���
			//app_newsub_menu_create(home_para);//�����Ӵ���
			//app_newsetting_sub_menu_create(home_para);//������ͨ������app�Ӵ���
			#endif
#if 1 == HOME_USE_HOME_LOGO
			GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_ON);	//���˵�ͼ�㴰��״̬��
#else
#endif
			#if 1//ԭ����
			GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_ON);		//�����Ӳ˵�ͼ�㴰��״̬
			GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_ON);
			GUI_LyrWinSetTop(home_para->lyr_mmenu);
			GUI_LyrWinSetTop(home_para->lyr_forground);
			gscene_hbar_set_state(HBAR_ST_HIDE);
			#else//�µ�ͨ������app�Ӵ���ͼ��״̬
			//GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_ON);		//�����Ӳ˵�ͼ�㴰��״̬
			//gscene_hbar_set_state(HBAR_ST_HIDE);
			#endif
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

		case GUI_MSG_CLOSE:		//�˵��ر�
			GUI_ManWinDelete(msg->h_deswin);
			return EPDK_OK;

		case GUI_MSG_DESTROY:	//�˵�����
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

		case GUI_MSG_KEY:		//����
			__msg("******app_home_proc GUI_MSG_KEY*****\n");
			break;

		case GUI_MSG_TOUCH:		//����
			break;

		case DSK_MSG_FS_PART_PLUGIN:
		case DSK_MSG_FS_PART_PLUGOUT:
			return EPDK_OK;

		case DSK_APP_RESTORE:	//��λ
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

		case GUI_MSG_COMMAND:	//��������
		{
			home_para_t *home_para;
			home_para = (home_para_t *)GUI_WinGetAddData(msg->h_deswin);
			__wrn("home GUI_MSG_COMMAND msg->dwAddData1=%d\n", msg->dwAddData1);
			switch(LOWORD(msg->dwAddData1))
			{
				case MAIN_MENU_ID:	//���˵�ID����˼�������˵�ʱ�����ִ������ĳ���
				{
					switch(HIWORD(msg->dwAddData1))
					{
						case ID_SWITCH_ITEM:
						{
							//�ȵ�ס�Ӳ˵�������Ӳ˵��л�ʱ�������⡣
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

				case SUB_MENU_ID:	//�Ӳ˵�ID����˼���ڽ����Ӳ˵������ִ������ĳ���
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
								check_disk(home_para);//����Ƿ���ڴ���
								__wrn("home_para->root_type = %d\n", home_para->root_type);
								__wrn("ID_HOME_MOVIE is...\n");

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

							case ID_HOME_CALENDAR://��������appӦ��
							{
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								__wrn("ID_HOME_CALENDAR is...\n");
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_OTHERS, 3);
								break;
							}

#endif

							case ID_HOME_EXPORER:
							{
								check_disk(home_para);//����Ƿ���ڴ���
								__msg("home_para->root_type = %d", home_para->root_type);
								__wrn("home_para->root_type is... = %d\n",home_para->root_type);

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
									__wrn("no disk currently is...\n");
								}

								break;
							}

							case ID_HOME_GENERAL_SETTING://����ͨ������appӦ�ó��򴴽�manwin����
							{
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_OTHERS, 0);
								break;
							}
							case ID_HOME_NUMBER://������
							{
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NUMBER, 0);//����ͨ�������Ӳ˵�
								break;
							}
							case ID_HOME_NEW_SETTING_NUMBER://��������ת��app_root_scene.cȥ����APPӦ��manwin����
							{
								__wrn("ID_HOME_NEW_SETTING_NUMBER is...\n");
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);	//ͼ�㴰����ͣ״̬
								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, 0);//�����µ�appӦ���Ӳ˵�
								break;
							}
							case ID_HOME_NEW_EXPORER://��������ת��app_root_scene.cȥ����new_explorer��APPӦ��manwin����
							{
								check_disk(home_para);//����Ƿ���ڴ���
								__wrn("ID_HOME_NEW_EXPORER\n");
								__wrn("home_para->root_type is... = %d\n",home_para->root_type);

								if(home_para->root_type != 0)//����2ΪTF��,2<<8����512:������Ϊ 0010 0000 0000
								{
									__wrn("send ID_HOME_NEW_EXPORER to app_root_scene.c is...\n");
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);		//sub_menu�Ӳ˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND); 	//main_menu���˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);	//main_menu���˵�ǰ��ͼ�㴰�ڹ���״̬
									main_menu_uninit_res(home_para->h_mmenu);							//���˵������ͷ�ͼƬ��Դ
									main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_EXPORER, 2);	//���͵�app_root_scene.c�ļ������µ�new_explorer����manwin���ڹ��ܣ�2ΪTF��
									__wrn("send ID_HOME_NEW_EXPORER is ok\n");
								}
								else
								{
									__wrn("ID_HOME_NEW_EXPORER no disk currently is...\n");
								}
								break;
							}
							case ID_HOME_NEW_MOVIE:/*��������ת��app_root_scene.cȥ����new_movie��APPӦ��manwin����*/
							{
								check_disk(home_para);//����Ƿ���ڴ���
								__wrn("ID_HOME_NEW_MOVIE\n");
								__wrn("home_para->root_type is... = %d\n",home_para->root_type);

								if(home_para->root_type != 0)//����2ΪTF��,2<<8����512:������Ϊ 0010 0000 0000
								{
									__wrn("send ID_HOME_NEW_MOVIE to app_root_scene.c is...\n");
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);		//sub_menu�Ӳ˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND); 	//main_menu���˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);	//main_menu���˵�ǰ��ͼ�㴰�ڹ���״̬
									main_menu_uninit_res(home_para->h_mmenu);							//���˵������ͷ�ͼƬ��Դ
									main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_MOVIE, 2);	//���͵�app_root_scene.c�ļ������µ�new_movie����manwin���ڹ��ܣ�2ΪTF��
									__wrn("send ID_HOME_NEW_MOVIE is ok\n");
								}
								else
								{
									__wrn("ID_HOME_NEW_MOVIE no disk currently is...\n");
								}
								break;
							}

							case ID_HOME_NEW_PHOTO:{//���������appӦ��
								check_disk(home_para);//����Ƿ���ڴ���
								__wrn("ID_HOME_NEW_PHOTO\n");
								__wrn("home_para->root_type is... = %d\n",home_para->root_type);

								if(home_para->root_type != 0)//����2ΪTF��,2<<8����512:������Ϊ 0010 0000 0000
								{
									__wrn("send ID_HOME_NEW_PHOTO to app_root_scene.c is...\n");
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);		//sub_menu�Ӳ˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND); 	//main_menu���˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);	//main_menu���˵�ǰ��ͼ�㴰�ڹ���״̬
									main_menu_uninit_res(home_para->h_mmenu);							//���˵������ͷ�ͼƬ��Դ
									main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_PHOTO, 2);	//���͵�app_root_scene.c�ļ������µ�new_movie����manwin���ڹ��ܣ�2ΪTF��
									__wrn("send ID_HOME_NEW_PHOTO is ok\n");
								}
								else
								{
									__wrn("ID_HOME_NEW_PHOTO no disk currently is...\n");
								}
								break;
							}

							case ID_HOME_NEWMUSIC://�����ӵ�musicӦ��
							{
								check_disk(home_para);//����Ƿ���ڴ���
								__wrn("ID_HOME_NEW_MUSIC\n");
								__wrn("home_para->root_type is... = %d\n",home_para->root_type);

								if(home_para->root_type != 0)//����2ΪTF��,2<<8����512:������Ϊ 0010 0000 0000
								{
									__wrn("send ID_HOME_NEW_MUSIC to app_root_scene.c is...\n");
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);		//sub_menu�Ӳ˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND); 	//main_menu���˵�ͼ�㴰�ڹ���״̬
									GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);	//main_menu���˵�ǰ��ͼ�㴰�ڹ���״̬
									main_menu_uninit_res(home_para->h_mmenu);							//���˵������ͷ�ͼƬ��Դ
									main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEWMUSIC, 2);	//���͵�app_root_scene.c�ļ������µ�new_movie����manwin���ڹ��ܣ�2ΪTF��
									__wrn("send ID_HOME_NEW_MUSIC is ok\n");
								}
								else
								{
									__wrn("ID_HOME_NEW_MUSIC no disk currently is...\n");
								}
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
				#if 1	//���������˵����Ӳ˵�����
					#if 0
					case SUB_MENUNUMBER_ID://���������˵��հ״���ID
					{
						#if 1
						__wrn("SUB_MENUNUMBER_ID is...\n");
						switch(HIWORD(msg->dwAddData1))
						{	
							#if 1
							case ID_NUMBER_MENU_SWITCH_ITEM://�Ӳ˵�����0
							{
								//�ȵ�ס�Ӳ˵�������Ӳ˵��л�ʱ�������⡣
								//GUI_LyrWinSetTop(home_para->lyr_mmenu);//112357
								//gscene_hbar_set_state(HBAR_ST_SHOW);//112357
								if(home_para->lyr_smenu)
								{
									GUI_LyrWinDelete(home_para->lyr_smenu);
								}
								__wrn("ID_NUMBER_MENU_SWITCH_ITEM \n");
								home_para->focus_id = msg->dwAddData2;
								home_para->first_item = msg->dwReserved;
								//app_sub_menu_create(home_para);								 //��home���˵����洰���ڴ����Ӳ˵�
								app_newsetting_sub_menu_create(home_para);					//�����ӵ��Ӳ˵�
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SLEEP);//ͼ�㴰������Ϊ����״̬
								//GUI_LyrWinSetTop(home_para->lyr_smenu);
							}
							break;
							#endif
							case ID_OP_NUMBER_MENU_UP:	//�ϰ���1
							{
								__gui_msg_t mymsg;
								mymsg.h_deswin = home_para->h_mmenu;
								mymsg.h_srcwin = NULL;
								mymsg.id = MSG_NUMBER_MENU_OP_UP;
								__wrn("MSG_NUMBER_MENU_OP_UP \n");
								GUI_SendMessage(&mymsg);
							}
							break;

							case ID_OP_NUMBER_MENU_DOWN:	//�°���2
							{
								__gui_msg_t mymsg;
								mymsg.h_deswin = home_para->h_mmenu;
								mymsg.h_srcwin = NULL;
								mymsg.id = MSG_NUMBER_MENU_OP_DOWN;
								__wrn("MSG_NUMBER_MENU_OP_DOWN \n");
								GUI_SendMessage(&mymsg);
							}
							break;

							case ID_OP_NUMBER_MENU_ENTER:	//�س�����3
							{
								#if 1	//���ͻس�����
								__gui_msg_t mymsg;
								mymsg.h_deswin = home_para->h_mmenu;
								mymsg.h_srcwin = NULL;
								mymsg.id = MSG_NUMBER_MENU_OP_ENTER;
								mymsg.dwAddData2 = msg->dwAddData2;
								__wrn("SUB_MENUNUMBER_ID ID_OP_NUMBER_MENU_ENTER msg->dwAddData1=%d\n", msg->dwAddData1);
								GUI_SendMessage(&mymsg);
								#else	//ֱ�ӷ��������ӵ�ͨ������app��id����mainwin����
								__wrn("ID_OP_NUMBER_MENU_ENTER is \n");
								//main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, NULL);//ֱ�ӽ���ID_HOME_NEW_SETTING_NUMBER,Ȼ����뵽�����ӵ�ͨ�����ý���
								#endif
							}
							break;

							case ID_HOME_NEW_SETTING_NUMBER://����ֱ�ӷ��������ӵ�ͨ������app��id����mainwin����
							{
								#if 1
								__wrn("ID_OP_NUMBER_MENU_ENTER is... ok\n");
								//main_menu_uninit_res(home_para->h_mmenu);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, NULL);//ֱ�ӽ���ID_HOME_NEW_SETTING_NUMBER,Ȼ����뵽�����ӵ�ͨ�����ý���
								#endif
							}
							break;

							default:
								break;
						}
						#endif
					}
					break;
					#endif
					#if 0 //�µ�ͨ�������Ӳ˵�����ID
					case NEW_SETTING_ID://�������Ӳ˵��հ״���ID
					{	
						#if 1
						switch(HIWORD(msg->dwAddData1))
						{	
							#if 1
							__wrn("NEW_SETTING_ID msg->dwAddData1=%d\n", msg->dwAddData1);
							case ID_NUMBER_MENU_SWITCH_ITEM://�Ӳ˵�����0
							{
								//�ȵ�ס�Ӳ˵�������Ӳ˵��л�ʱ�������⡣
								//GUI_LyrWinSetTop(home_para->lyr_mmenu);//112357
								//gscene_hbar_set_state(HBAR_ST_SHOW);//112357
								if(home_para->lyr_smenu)
								{
									GUI_LyrWinDelete(home_para->lyr_smenu);
								}
								__wrn("app new_setting ID_NUMBER_MENU_SWITCH_ITEM \n");
								home_para->focus_id = msg->dwAddData2;
								home_para->first_item = msg->dwReserved;
								app_sub_menu_create(home_para);								 //��home���˵����洰���ڴ����Ӳ˵�
								GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SLEEP);//ͼ�㴰������Ϊ����״̬
								//GUI_LyrWinSetTop(home_para->lyr_smenu);
							}
							break;
							#endif
							case ID_OP_NUMBER_MENU_UP:	//�ϰ���1
							{
								__gui_msg_t mymsg;
								mymsg.h_deswin = home_para->h_submenu;
								mymsg.h_srcwin = NULL;
								mymsg.id = MSG_NUMBER_MENU_OP_UP;
								__wrn("app new_setting MSG_NUMBER_MENU_OP_UP \n");
								GUI_SendMessage(&mymsg);
							}
							break;

							case ID_OP_NUMBER_MENU_DOWN:	//�°���2
							{
								__gui_msg_t mymsg;
								mymsg.h_deswin = home_para->h_submenu;
								mymsg.h_srcwin = NULL;
								mymsg.id = MSG_NUMBER_MENU_OP_DOWN;
								__wrn("app new_setting MSG_NUMBER_MENU_OP_DOWN \n");
								GUI_SendMessage(&mymsg);
							}
							break;

							case ID_OP_NUMBER_MENU_ENTER:	//�س�����3
							{
								__gui_msg_t mymsg;
								mymsg.h_deswin = home_para->h_submenu;
								mymsg.h_srcwin = NULL;
								mymsg.id = MSG_NUMBER_MENU_OP_ENTER;
								mymsg.dwAddData2 = msg->dwAddData2;
								__wrn("app new_setting msg->dwAddData1=%d\n", msg->dwAddData1);
								GUI_SendMessage(&mymsg);
							}
							break;
							
							case ID_HOME_NEW_SETTING_NUMBER://������ͨ������app��ID
							{
								if(home_para->lyr_smenu)
								{
									GUI_LyrWinSetSta(home_para->lyr_smenu, GUI_LYRWIN_STA_SUSPEND);
								}

								GUI_LyrWinSetSta(home_para->lyr_mmenu, GUI_LYRWIN_STA_SUSPEND);
								GUI_LyrWinSetSta(home_para->lyr_forground, GUI_LYRWIN_STA_SUSPEND);
								main_menu_uninit_res(home_para->h_mmenu);
								//GUI_LyrWinSetSta(home_para->lyr_total_bg, GUI_LYRWIN_STA_SUSPEND);
								__wrn("ID_HOME_NEW_SETTING_NUMBER msg->dwAddData1=%d\n", msg->dwAddData1);
								main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, home_para->focus_id, msg->dwAddData2);
							}
							break;
							default:
								break;
						}
						#endif
					}
					break;
					#endif
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
//home���ڴ���
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

