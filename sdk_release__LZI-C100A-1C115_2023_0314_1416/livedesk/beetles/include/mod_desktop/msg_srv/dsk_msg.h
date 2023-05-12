/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_message.h
* By      	: Andy.zhang
* Func		: desk message define
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_MESSAGE_H__
#define __DSK_MESSAGE_H__

typedef enum
{
  DSK_MSG_START = GUI_MSG_DESKTOP,

  DSK_MSG_HOME,				// 回到主界面
  DSK_MSG_KILL,				// 强制要求应用程序退出
  DSK_APP_RESTORE,			// 应用程序恢复前台

  DSK_MSG_SYS_START = DSK_MSG_START + 0x100,

  /* 以下消息仅为桌面系统 和 主界面 收到 */
  DSK_MSG_REFRESH_CURSOR,		// 更新鼠标位置
  DSK_MSG_HIDE_CURSOR,		// 隐藏鼠标位置
  DSK_MSG_MAIN_CMD,			// MAIN消息传输管道
  DSK_MSG_CHANG_LANGUAGE,		// 语言编码
  DSK_MSG_FW_UPDATE,			// 固件升级
  DSK_MSG_FW_START_UPDATE,	// 固件升级

  DSK_MSG_SCREEN_SWITCH,		// 屏幕切换
  DSK_MSG_SWITCH_YPBPR,		// 切换到色差
  DSK_MSG_SWITCH_HDMI,		// 切换到HDMI
  DSK_MSG_SWITCH_CVBS,		// 切换到CVBS
  DSK_MSG_ASSISTANT_SCENE_CREATE,
  DSK_MSG_ASSISTANT_SCENE_DELETE,
  DSK_MSG_SETTING_RESTORE,	// 设置复位

  DSK_MSG_STANDBY_WAKE_UP,	// 设备待机唤醒
  DSK_MSG_STANDBY,			// 设备待机

  DSK_MSG_HOLD,				// 锁键盘
  DSK_MSG_APP_EXIT,			// 一键回主界面
  DSK_MSG_POWER_OFF,			// 关机
  DSK_MSG_LOW_POWER,			// 低电

  DSK_MSG_BAN_VOLUME,			// 禁音、开音

  DSK_MSG_USBD_PLUG_IN,		// usb device connect pc
  DSK_MSG_USBD_PLUG_OUT,		// usb device plug out pc

  DSK_MSG_ORCHID_UPDATE_START,// 开始更新orchid
  DSK_MSG_ORCHID_UPDATE_END,	// 更新orchid 结束

  DSK_MSG_INSTALL_USBH,		// usb host 检测到device插入
  DSK_MSG_UNSTALL_USBH,		// usb host 检测到device

  DSK_MSG_SET_AUTO_OFF,		// 设置关机时间
  DSK_MSG_SET_SCN_CLOSE,		// 设置关屏时间

  DSK_MSG_SCREEN_OPEN,		// 通知开屏
  DSK_MSG_SCREEN_CLOSE,		// 通知关屏
  DSK_MSG_CLOSE_SCN_GATE_ON,	// 允许自动关屏
  DSK_MSG_CLOSE_SCN_GATE_OFF,	// 禁止自动关屏

  DSK_MSG_TVDAC_PLUGIN,		// tvdac plug in
  DSK_MSG_TVDAC_PLUGOUT,		// tvdac plug out
  DSK_MSG_HDMI_PLUGIN,		// hdmi plug in
  DSK_MSG_HDMI_PLUGOUT,		// hdmi plug out

  DSK_MSG_FS_PART_PLUGIN,		// 文件系统分区插入
  DSK_MSG_FS_PART_PLUGOUT,	// 文件系统分区拔出

  DSK_MSG_VOLTAGE_CHANGE,     //电池电量变化消息(dwAddData1为power_level)

  // BOX方案 特殊消息
  DSK_MSG_BOX_STANDBY_START,	// 通知进入 standby
  DSK_MSG_BOX_STANDBY_END,		// 消息对话框 待机结束
  DSK_MSG_BOX_STANDBY_EXIT,		// 消息对话框 待机退出
  DSK_MSG_SWITCH_AUDIO_IF,

  DSK_MSG_ENABLE_STANDBY,	    // 允许standby
  DSK_MSG_DISABLE_STANDBY,	// 禁止standby
  DSK_MSG_GET_STANDBY_FLAG,	// 查询是否允许standby

  DSK_MSG_ALARM,

  DSK_MSG_SCREEN_FORCE_CLOSE,	// 荧光屏强制关闭、强制关屏

  DSK_MSG_SCREENOFF_BKOFF,
  DSK_MSG_SCREENON_BUT_BKOFF,

  DSK_MSG_TCON_OCCUPY_LCMI,
  DSK_MSG_TCON_RELEASE_LCMI,
  DSK_MSG_TCON_CLOSE_SCREEN,	// 定时关闭荧光屏、定时关屏

  DSK_MSG_RELEASE_LOGO_BUF,		// logo数据释放
	DSK_MSG_RELEASE_TIMER_DET_UDISK,

  DSK_MSG_APP_MODE_ENTER,
  DSK_MSG_APP_MODE_EXIT,

  DSK_MSG_MIC_PLUGIN,			// 麦克风设备插入
  DSK_MSG_MIC_PLUGOUT,			// 麦克风设备拔出
	
  DSK_MSG_AUX_PLUGIN,			// 音频输入端口插入
  DSK_MSG_AUX_PLUGOUT,			// 音频输入端口拔出

  DSK_MSG_LAST = GUI_MSG_DESKTOP + 0x1000 - 1
} __msg_desktop_e;


#endif /* __DSK_MESSAGE_H__ */





