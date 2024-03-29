/*
************************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						          (c) Copyright 2007-2010, Jackie, China
*										All Rights Reserved
*
* File    		: init_server.c
* By      		: Jackie.chen
* Func		: init server
* Version	: v1.0
* ======================================================================================================================
* 2010-10-25  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/
#ifndef __INIT_SERVER_H__
#define __INIT_SERVER_H__

#include "epdk.h"

#if (SP_KB_TYPE == DEV_LED_EFFECT1)
#define TIMEOUT_ID_TIMER_KEYLED_OFF			800
#else
#define TIMEOUT_ID_TIMER_KEYLED_OFF			0
#endif

#if TIMEOUT_ID_TIMER_KEYLED_OFF
#define ID_TIMER_KEYLED_OFF					0x2000
#endif

typedef struct init_ctl_t
{
	/* 关机 timer */
	__s32 					 autooff_timeout;	/* 自动关机时间 	*/
	__s32					 auto_off_time_id;		/* 自动关机timer id */

	/* 关屏 timer */
	__s32 					 closescn_timeout;	/* 自动关屏时间 	*/
	__bool					 closescn_gate_on;		/* 是否禁止自动关屏 */
	__s32					 close_scn_time_id;		/* 自动关屏timer id */

	/* 低电查询 */
	__s32 					 low_power_check_timeout;
	__s32					 low_power_check_time_id;

	/*电源灯闪动控制*/
	__s32					led_flash_timeout;
	__s32					led_flash_time_id;

	/* 信号量 */
	__krnl_event_t			*state_sem;			/* 查询状态信号量  */

	/* cursor */
	__s32					cursor_timeout;		/* cursor time out */
	__s32 					cursor_time_id;			/* cursor time id  */
	/* cursor 创建信息 */
	__u8					*cursor_bmp;
	__gui_cursor_t   		cursor_info;

	/* 桌面事件状态标识位*/
	__bool 					 usb_connect;		/* usb 是否连接到 pc */
	__bool 					 screen_close_done;	/* 屏幕是否已经关闭  */

	/* msg enble */
	__bool					key_msg_enble;
	__bool 					tp_msg_enble;

	__u8 					sys_msg_handle_tid;

	__bool                  autooff_timer_exist;
	__bool                  closescn_timer_exist;
	//__bool                  usb_host_is_working_before_standy;
	__bool                  bdec_freq;
	__bool                  power_off;
	
	__hdle					hdleOwi;
} __init_ctl_t;

typedef enum
{
  POWEROFF_DIALOG,
  LOWPOWER_DIALOG,
  USBD_DIALOG,
  ORCHID_DIALOG,
  FW_UPDATA_DIALOG,
  NONE_DIALOG
} main_dialog_t;

typedef struct
{
	/* 图层句柄 */
	//H_LYR hbar_layer;			// 标题栏图层
	H_LYR dialog_layer;			// 弹框图层

	/* 窗口句柄 */
	H_WIN init_win;
	H_WIN headbar_win;			// 标题栏窗口
	H_WIN dialog_win;			// 对话框窗口
	H_WIN scnlock_win;
	H_WIN assistant_win;

	char app_title[32];			// headbar show app_name

	/* 当前headbar 现场 */
	//__hbar_format_t hbar_format;
	//char hbar_title[32];

	/* 系统状态 dialog 相关 */
	main_dialog_t cur_dialog;
	__bool usb_plug_in;//usb刚刚连接，用于防止低电或关机对话框关闭后，程序被关闭。
	__bool usb_connect;
	__bool orchid_update;
	__bool low_power;
	__bool power_off;
	__bool fw_update;
} init_scene_t;

void init_lock(__gui_msg_t *msg);

void init_unlock(__gui_msg_t *msg);

H_WIN init_mainwin_create(void);

#endif

