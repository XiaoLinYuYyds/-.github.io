/*
************************************************************************************************************************
*                                                        CUCKOO
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   :ResDecode.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2008.11.08
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.11.08       1.1.0        build the file
*
*/

#ifndef __BEETLES_APP_H_
#define __BEETLES_APP_H_

#include "apps.h"
#include "lib/common/palette.h"
#include "lib/common/misc.h"
#include "lib/common/mem_dev.h"
#include "lib/sset/toolbar/GlideCurve.h"
#include "lib/sset/toolbar/tbar_com.h"
#include "lib/sset/sset_e.h"
#include "lib/sset/sset_uipara.h"

#include "lib/app_dialog/app_dialog.h"
#include "lib/app_loading/app_loading.h"
#include "lib/fifo_db/fifo_db.h"
#if SP_APP_ALARM
#include "lib/app_alarm/app_alarm.h"
#endif

#include "lib/com_volume/com_volume.h"
#include "lib/com_menu/com_menu.h"


#define SUPPORT_MUSIC_BG_MODE 1

#define MOVIE_ENTER_FLUSH 0
#define MUSIC_ENTER_FLUSH 0
#define MOVIE_AUTO_FLUSH_FIRST_TIME 	(200*5)		///10秒钟
#define MUSIC_AUTO_FLUSH_FIRST_TIME		(200*5)		///10秒钟

#if SP_DEV_HW_PW_OFF
	#define MOVIE_AUTO_FLUSH_TIME 				(100*60*10)			///10分钟
	#define MUSIC_AUTO_FLUSH_TIME 				(100*60*3)			///3分钟
#else
	#define MOVIE_AUTO_FLUSH_TIME 0		//(200*60*10)			///10分钟
	#define MUSIC_AUTO_FLUSH_TIME 0		//(200*60*3)			///3分钟
#endif

#define CONST_BackColor_Blue			0xff771a00	//GUI_BLUE
#define CONST_BackColor_Black			0xff000000	//GUI_BLACK
#define CONST_BackColor					CONST_BackColor_Blue



extern GUI_FONT    *SWFFont;

#define APP_ROOT		"beetles"
#define APP_HOME		"app_home"
#define APP_NUMBER		"app_number" //新添加窗口
#define APP_EXPLORER	"app_explorer"
#define APP_MOVIE		"app_movie"
#define APP_MUSIC		"app_music"
#define APP_PHOTO		"app_photo"
#define APP_FM			"app_fm"
#define APP_SETTING		"app_setting"
#define APP_EBOOK		"app_ebook"
#define APP_CALENDAR	"app_calendar"
#define APP_RECORD	    	"app_record"
#define APP_LINEIN		"app_linein"
#define APP_ATV			"app_atv"
#define APP_NEWSETTING		"app_newsetting" //新添加通用设置名称
#define APP_NEWMOVIE		"app_new_movie"
#define APP_NEWPHOTO		"app_new_photo"
#define APP_NEWMUSIC		"app_new_music"
#define APP_NEWEBOOK		"app_new_ebook"
#define	APP_NEWFM			"app_new_ebook"//收音机


#define APP_HOME_ID			0x1001
#define APP_EXPLORER_ID		0x2001
#define APP_MOVIE_ID		0x3001
#define APP_MUSIC_ID		0x4001
#define APP_PHOTO_ID		0x5001
#define APP_FM_ID			0x6001
#define APP_SETTING_ID		0x7001
#define APP_EBOOK_ID		0x8001
#define APP_CALENDAR_ID		0x9001
#define APP_RECORD_ID		0x9002
#define APP_LOADING_ID		0x9100
#define APP_LINEIN_ID		0x9200
#define APP_ALARM_ID		0x9300
#define APP_ROOT_ID 		0x9400
#define COM_VOLUME_ID       0x9500
#define APP_ATV_ID			0xA001
#define APP_NEWSETTING_ID			0x9600	//新添加app应用程序的id存放地址
#define APP_NEWEXPLORER_ID			0x9700	//新添加资源管理器app应用程序的id存放地址
#define APP_NEWMOVIE_ID				0x9801	//新添加视频APP应用程序的id存放地址
#define APP_NEWPHOTO_ID				0x9901	//新添加相册APP应用程序的id存放地址
#define APP_NEWMUSIC_ID				0x9A01	//新添加音乐APP应用程序的id存放地址
#define APP_NEWEBOOK_ID				0x9B01	//新添加电子书APP应用程序的id存放地址
#define	APP_NEWFM_ID				0x9C01	//新添加收音机APP应用程序的id存放地址


#if SP_FavoriteFiles
#define 	APP_DEL_DLG_ID			0x60
#define 	APP_STORE_UP_DLG_ID			0x70
#define 	APP_STORE_UP_PLAY_DLG_ID			0x80
#endif

typedef enum
{
  SWITCH_TO_OTHER_APP = 0,
  SWITCH_TO_MMENU,
  SWITCH_TO_EXPLORER,
  OPEN_BG_MUSIC,
  CLOSE_BG_MUSIC,
  SWITCH_DISK,
  SWITCH_TO_LINE_IN,
  CLOSE_AUDIO_APP,
  SWITCH_TO_ATV,
#if SP_FavoriteFiles
  SWITCH_TO_FAVORPLAY,
#endif
  NEW_SWITCH_TO_OTHER_APP,//新添加app应用程序跳转到其他app的标志
} app2root_cmd_t;

#define BMP_LINE_ROUND(x)	(((__u32)x+3)/4*4)

#define BEETLES_TBL_SIZE(a)	(sizeof(a)/sizeof(a[0]))

typedef enum
{
	ID_HOME_NEWMUSIC = 0,/*ID_HOME_MUSIC*/
	
	ID_HOME_NEW_FM,					/*收音机的manwin窗口创建标志*/
#if SP_APP_ATV
	ID_HOME_TV,
	//ID_HOME_AVIN,
#endif
	
	//ID_HOME_DVD,
	
	ID_HOME_NEW_PHOTO,/*新添加的相册app应用程序，manwin窗口创建标志*/
	ID_HOME_NEW_EBOOK,				/*新添加的电子书app应用manwin窗口*/
	ID_HOME_FM,
	ID_HOME_RECORD,
	ID_HOME_GENERAL_SETTING,
	//ID_HOME_POWER_SETTING,
	ID_HOME_EXPORER,
#if SP_APP_CALENDAR
	ID_HOME_CALENDAR,
#endif
	ID_MAX_NUM,
	ID_HOME_OTHERS,
	ID_HOME_AUX,
	ID_HOME_ALARM,
	ID_MUSIC_SHORTCUT,
	ID_MOVIE_SHORTCUT,
	ID_EBOOK_SHORTCUT,
	ID_PHOTO_SHORTCUT,
	ID_FM_SHORTCUT,
	ID_LINEIN_SHORTCUT,
	ID_RECORD_SHORTCUT,
	ID_ATV_SHORTCUT,
	ID_HOME_ATV,
//	ID_HOME_TV,
	ID_HOME_AVIN,
	ID_HOME_MUSIC,
	ID_HOME_PHOTO,
	ID_HOME_EBOOK,
	ID_HOME_NUMBER,//新添加窗口ID
	ID_HOME_NEW_MOVIE,			  /*新添加的movie视频app应用程序，manwin窗口创建标志*/
	ID_HOME_NEW_SETTING_NUMBER,		/*新添加的空白的manwin窗口*/
	ID_HOME_NEW_EXPORER,		   /*ID_HOME_MOVIE,*/
	ID_HOME_MOVIE,
} root_home_id_t;

typedef enum
{
  ID_FM_FMPLAY = 0,
  ID_FM_FMSRH,
} root_fm_id_t;

typedef enum
{
  ID_EXPLORER_ALL = 0,
  ID_EXPLORER_MOVIE,
  ID_EXPLORER_PHOTO,
  ID_EXPLORER_MUSIC,
  ID_EXPLORER_EBOOK,
  ID_EXPLORER_FIRMWARE,
  ID_EXPLORER_RESTORE,
#if SP_FavoriteFiles
  ID_EXPLORER_MOVIEFAVOR,
#endif
} root_explorer_id_t;
typedef enum
{
  ID_NEW_EXPLORER_ALL = 0,//显示所有的列表内容
  ID_NEW_EXPLORER_MOVIE,  	//单独显示的列表视频文件内容
  ID_NEW_EXPLORER_PHOTO,	//单独显示的列表图片文件内容
  ID_NEW_EXPLORER_MUSIC,	//单独显示的列表音乐文件内容
  ID_NEW_EXPLORER_EBOOK,	//单独显示的列表电子书文件内容
  ID_NEW_EXPLORER_FIRMWARE,	//单独显示的列表固件文件内容
  ID_NEW_EXPLORER_RESTORE,	//单独显示的列表修复文件内容
} root_new_explorer_id_t;

typedef enum
{
  EXPLR_SW_TO_MAIN = 0,
  EXPLR_SW_TO_MOVIE,
  EXPLR_SW_TO_PHOTO,
  EXPLR_SW_TO_MUSIC,
  EXPLR_SW_TO_EBOOK,
#if SP_FavoriteFiles
  EXPLR_SW_TO_FAVOR_EXPLR,
#endif
} explorer_sw_to_t;

typedef enum
{
  ID_OTHERS_EXPLORER = 0,
  ID_OTHERS_EBOOK,
  ID_OTHERS_CALENDAR,
} root_others_id_t;

typedef enum
{
  ID_SETTING_COMMON = 0,
  ID_SETTING_POWER,
  ID_SETTING_TIME,
  ID_SETTING_MULTIMEDIA,
} root_setting_id_t;

enum
{
  MENU_PAINT_ALL,//draw all
  MENU_PAINT_ITEMSW,//item switch
  MENU_PAINT_VALUESW,//value change
};

typedef enum
{
  SETTING_SW_TO_MAIN = 0,
  SETTING_SW_TO_PROMPT_UPGRADE,
  SETTING_SW_TO_PROMPT_DEFAULT,
  EXPLR_SW_TO_UPGRADE,
  SETTING_SW_TO_MULTIMEDIA,
  SETTING_SW_TO_NEW_SETTING,//通用设置app的manwin窗口切换到新添加app的manwin窗口
} setting_sw_to_t;
//新添加的app应用程序的跳转枚举
typedef enum
{
  NEW_SETTING_SW_TO_MAIN = 0,//跳转到home主界面的manwin窗口
  NEW_SETTING_SW_TO_PROMPT_UPGRADE,
  NEW_SETTING_SW_TO_PROMPT_DEFAULT,
  NEW_EXPLR_SW_TO_UPGRADE,
  NEW_SETTING_SW_TO_MULTIMEDIA,
  NEW_SETTING_SW_TO_SETTING,//跳转到通用设置app应用程序manwin窗口

  NEW_MOVIE_SW_TO_NEW_EXPLORER,/*NEW_MOVIE应用 跳转到 NEW_EXPLORER应用*/
  NEW_EBOOK_SW_TO_NEW_EXPLORER,
  NEW_EXPLORER_SW_TO_NEW_MOVIE,
  NEW_EXPLORER_SW_TO_NEW_PHOTO,/*跳转到new_photo应用*/
  NEW_EXPLORER_SW_TO_NEW_EBOOK,/*跳转到new_ebook应用*/
} new_setting_sw_to_t;

typedef enum
{
  FM_SW_TO_RECORD = 0,
} fm_sw_to_t;

typedef enum
{
  RECORD_SW_TO_FM = 0,
} record_sw_to_t;


//
//MUSICPLAYER_NORMAL: 		正常进入音乐播放器
//MUSICPLAYER_BACKGROUND: 	音乐播放器作为后台播放
typedef enum
{
  MUSICPLAYER_NORMAL = 0,
  MUSICPLAYER_BACKGROUND = 1,
  MUSICPLAYER_BACKGROUND_HOME = 2,
#if SP_FavoriteFiles
  MUSICPLAYER_FAVORPLAY = 3,
#endif
  MUSICPLAYER_MUSIC_PAUSE,
  MUSICPLAYER_MUSIC_PLAY,
} musicplayer_mode_e;

typedef enum
{
  LINEIN_MODE_NORMAL = 0,
  LINEIN_MODE_BACKGROUND,
  LINEIN_MODE_INVALID
} linein_mode_e;

typedef enum
{
  MOVIE_MODE_NORMAL = 0,//
  MOVIE_MODE_TV,
  MOVIE_MODE_DESKTOP,
} __movie_create_mode_e;

typedef struct tag_root_para
{
	GUI_FONT *font;
	H_WIN	h_parent;
	H_WIN	src_win;			//
	__u32	root_type;
	rat_root_t explr_root;
	__u32 setting_opt;//为0代表通用设置，为1代表电源管理
	__u32 record_opt;//为0代表正常进入录音，为1代表fm录音
	__s32	data;
	__s32  explorer_play_music;	//从文件管理里面播放音乐标志, 1 表示从文件管理中播放
	char  music_file_name[RAT_MAX_FULL_PATH_LEN];
	__bool  return_to_explorer_file_list ;		// 1 表示回退到浏览器中先前播放的媒体文件列表，0 表示回退到sd 卡/ usb 选择窗口中
	__movie_create_mode_e tv_mode;//视频模式
	__s32 amplify_open_delay_cnt;//延时打开的次数
#if SP_AutoUpdateFirmware
	H_WIN		h_fw_update_dlg;		//固件升级对话框
	char			fw_update_file_disk;		//固件升级文件磁盘
#endif
	__u32	m_maskFlags;
	__u32 log;
#if SP_FavoriteFiles
	__s32 enter_media_type[m_eMediaTypeList_max];
	__s32 favor_index;
#endif
} root_para_t;
// for m_maskFlags:
#define MASK_RootFlags_CVBS_InputMode									(0x7UL <<0)
	#define OP_RootFlags_CVBS_InputMode_TVFromHome				(1<<0)
	#define OP_RootFlags_CVBS_InputMode_AVINFromHome			(2<<0)
	#define OP_RootFlags_CVBS_InputMode_AVINFromDetect		(3<<0)
	#define OP_RootFlags_CVBS_InputMode_DVDFromHome			(4<<0)
#define MASK_RootFlags_AuxMode_BT										(1<<3)

// for log:
#define MASK_RootLog__LastFs								(0x03<<0)
#define OP_RootLog__LastFs_invalid						0
#define OP_RootLog__LastFs_SD								1
#define OP_RootLog__LastFs_UD								2

#define W_NumLock_Bg				240
#define H_NumLock_Bg				120
#define H_NumLock_Num				71
#define W_Num						37
#define H_Num						48
#define TIMEOUT_ID_TIMER_DIGIT			400

#define MAX_FF_FB_MULTI				3

#define H_THEME HTHEME
#define get_menu_text   dsk_langres_get_menu_text
#define get_menu_len    dsk_langres_get_menu_len
#define theme_open      dsk_theme_open
#define theme_hdl2buf   dsk_theme_hdl2buf
#define theme_hdl2size  dsk_theme_hdl2size
#define theme_close     dsk_theme_close

#endif// __BEETLES_APP_H_
