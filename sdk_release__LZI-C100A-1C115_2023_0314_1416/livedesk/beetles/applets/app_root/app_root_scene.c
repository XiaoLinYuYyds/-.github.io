/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    		: app_root_win_scene.c
* By      		: Bayden
* Func		: app_root applets plugin interface define
* Version	: v1.0
* ============================================================================================================
* 2010-9-28 17:11:20  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "app_root.h"
#include "app_root_scene.h"
#include "app_root_init.h"
#include "home/app_home.h"
#include "explorer/explorer.h"
#include "movie/app_movie.h"
#include "music/music.h"
#include "photo/app_photo.h"
#include "setting/app_setting.h"
#include "fm/app_fm.h"
#include "ebook/app_ebook.h"
#include "record/app_record.h"
#include "linein/app_linein.h"
//#include "calendar/app_calendar.h"



#define AMPLIFER_CHECK_TIMMER_ID 0x37
#define AMPLIFER_CHECK_TIME 100

#define SOS_TIMER_ID 0x38
#define SOS_CHECK_TIME 1

#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByKnob)
#define TIMEOUT_ID_TIMER_GET_ECHO_LEVEL	10	// 100ms读一次混响等级值
#else
#define TIMEOUT_ID_TIMER_GET_ECHO_LEVEL 	0
#endif
#if TIMEOUT_ID_TIMER_GET_ECHO_LEVEL
#define ID_TIMER_GET_ECHO_LEVEL			0x1234
#endif

#if SP_AUX_DET
typedef struct tag_root_globe_check
{
	__u8 	globe_thread_id;
	__hdle	hdl;
	__bool	linein_In;// linein

} root_globe_check;
#endif

typedef struct tag_root_ctrl
{
	H_WIN	h_app_home;
	H_WIN	h_app_explorer;
	H_WIN	h_app_fm;
	H_WIN	h_app_movie;
	H_WIN	h_app_music;
	H_WIN	h_app_photo;
	H_WIN	h_app_ebook;
	H_WIN	h_app_setting;
	H_WIN	h_app_calendar;
	H_WIN	h_app_record;
	H_WIN	h_app_new_setting;
	H_WIN	h_app_new_explorer;
	H_WIN	h_app_new_movie;/*存放manwin窗口*/
	H_WIN	h_app_new_photo;
	H_WIN	h_app_new_music;/*存放新添加的music应用*/
	H_WIN	h_app_new_ebook;
	H_WIN	h_app_new_fm;/*存放新添加的收音机应用*/
#if SP_APP_AUX
	H_WIN	h_app_linin;
#endif
#if SP_APP_ATV
	H_WIN	h_app_atv;
#endif
	void   *volume_scene;//全局音量条 hfrm
	root_para_t *root_para;

#if SP_AUX_DET
	root_globe_check  globe_check;
#endif

	__u8 amplifer_check_id;

	__s32 sos_started;
	__s32 volume_before_sos;

	__s32 is_hbar_show_before_dlg;//静音框弹出之前hbar是否打开
	__bool	switchMediaEnable_flag;
	E_WIN_VOLUME	m_eWinVolume;
#if SP_AutoUpdateFirmware
	__u8			m_updateFirmwareLog;
#endif
} root_ctrl_t;
#if SP_AutoUpdateFirmware
	// for m_updateFirmwareLog:
	#define LOG_UpdateFirmware_By_RootCreate		1
	#define LOG_UpdateFirmware_By_FSPlugin			2
#endif

typedef enum
{
  app_root_prob_intent_switch_media = 0,
  app_root_prob_intent_switch_storage,
  app_root_prob_intent_plugin_for_play,
  app_root_prob_intent_first_play,
  app_root_prob_intent_unknown,
  app_root_prob_intent_max,
} app_root_prob_intent_t;

typedef enum
{
  app_root_prob_ok = 0,
  app_root_prob_fail,
  app_root_prob_already_running,
  app_root_prob_not_running,
  app_root_prob_no_correspond_storage,
  app_root_prob_no_meida_file,
  app_root_prob_storage_switch,
  app_root_prob_media_switch,
  app_root_prob_storage_plugined,
} app_root_prob_result_t;

typedef enum
{
	// 决定模式切换的数量和顺序
	m_eShiftMode_fm = 0,
#if SP_APP_AUX
	m_eShiftMode_linein,
#endif
	m_eShiftMode_movie,
	m_eShiftMode_music,
	m_eShiftMode_ebook,
	m_eShiftMode_photo,
#if SP_APP_ATV
	m_eShiftMode_tv,
//	m_eShiftMode_avin,
#endif
	m_eShiftMode_setting,
	m_eShiftMode_max,
	m_eShiftMode_atv,
	m_eShiftMode_dvd,
	m_eShiftMode_new_setting,
//	m_eShiftMode_tv,
} E_SHIFT_MODE;


/**********************************************************************************************************************/

#define APP_COM_DLG_ID  (APP_ROOT_ID+1)
#define APP_ROOT_TYPE_SD 0x1
#define APP_ROOT_TYPE_UD 0x2


typedef enum
{
  APP_ROOT_MSG_DO_ASYNC = GUI_MSG_ + 1,
} app_root_msg_id_t;

typedef __s32(*p_app_root_fun)(void *arg);


static void app_root_cacheon(void);
static void app_root_cacheoff(void);
static void  app_root_cmd2self(root_ctrl_t *para, __s32 lword, __s32 hword, __s32 data2, __s32 reserved);
static rat_root_t __app_root_get_rat_root_type(__s32 reg_storage_type);
static __s32 __app_root_get_reg_storage_type(unsigned char root);
static __s32 app_root_shortcut_process(root_ctrl_t *root_ctrl, app_root_prob_intent_t intent, __s32 reg_media_type, __s32 reg_storage_type, __bool bHideHBar, __bool bShowDlgFlag);
static void __app_root_delete_all_app_except_home(root_ctrl_t *para);
static void __app_root_delete_all_audio_app(root_ctrl_t *para);
static void __app_root_delete_all_app(root_ctrl_t *para);
static __s32 __app_root_delete_sos(root_ctrl_t *para);
static __s32 __app_root_to_linein(root_ctrl_t *para);
static __s32 __app_root_to_sos(root_ctrl_t *para);
static void __app_home_restore_ex(root_ctrl_t *root_ctrl);
static void __app_home_restore(__gui_msg_t *msg);
static __s32 app_root_set_music_bg_mode(__gui_msg_t *msg);
static __s32 app_root_process_before_show_dlg(root_ctrl_t *root_ctrl);
static __s32 app_root_process_after_show_dlg(root_ctrl_t *root_ctrl);

#if SP_APP_ATV
extern __bool			g_isTconOccupyLcmi_flag;
#endif
extern __s32 g_is_searching;

__s32 g_is_mute = 0;//1代表mute，0代表没mute

static __bool g_linin_is_first_read = EPDK_TRUE;//是否第一次读取

#ifndef IOCTRL__MIC_DET__IS_PLUGIN
static __bool gs_isPlugin_micDet = 0;
#endif

#if SP_AutoUpdateFirmware
#define PATH__LZ_C100_IMG				"x:\\update\\LZ_C100.img"			// 兼容以前功能
#define PATH__AUTOEXE_LZ_C100_IMG		"x:\\update\\cmd(autoexe)_LZ_C100.img"	// 优先使用此固件
const char *gc_pstrRelativePath_UpdateFirmware[] = {PATH__AUTOEXE_LZ_C100_IMG, PATH__LZ_C100_IMG};
#define SIZE_gc_pstrRelativePath_UpdateFirmware		(sizeof(gc_pstrRelativePath_UpdateFirmware) /sizeof(gc_pstrRelativePath_UpdateFirmware[0]))
char g_strAbsolutePath_UpdateFirmware[64];

__s32 CheckFileSize(const char *fullPath, __s32 fileSize)
{
	__s64	fileSize_byte;
	fileSize_byte = eLIBs_GetFileSize(fullPath);
	__wrn("fileSize_byte = %ld\n", (__s32)fileSize_byte);

	if((fileSize_byte < (fileSize + 1024 * 1024)) && (fileSize_byte > (fileSize - 1024 * 1024)))
	{
		__wrn("Check firmware <<%s>>succ!\n", fullPath);
		return EPDK_OK;
	}

	return EPDK_FAIL;
}

__s32 CheckFileSize_Firmware(char rootDir, __s32 fileSize)
{
	__wrn("rootDir=%c:/, fileSize = %ld\n", rootDir, fileSize);

	if(!rootDir)
	{
		return 0;
	}

	if(fileSize > 1024)
	{
		__s64	fileSize_byte;
		__u8	i;

		for(i = 0; i < SIZE_gc_pstrRelativePath_UpdateFirmware; i++)
		{
			memset(g_strAbsolutePath_UpdateFirmware, 0, sizeof(g_strAbsolutePath_UpdateFirmware));
			eLIBs_strcpy(g_strAbsolutePath_UpdateFirmware, gc_pstrRelativePath_UpdateFirmware[i]);
			g_strAbsolutePath_UpdateFirmware[0] = rootDir;
			__wrn("Check firmware fullPath : %s\n", g_strAbsolutePath_UpdateFirmware);

			if(CheckFileSize(g_strAbsolutePath_UpdateFirmware, fileSize) == EPDK_OK)
			{
				return rootDir; // dsk_start_fw_update(fullPath);
			}
		}
	}

	__wrn("Check firmware fail!\n");
	return 0;
}

__s32 DetectUpdateFirmware(char *rootDir)
{
	__s32 ret, firmwareSize;

	if(!rootDir)
	{
		return 0;
	}

	ret = esCFG_GetKeyValue("partition0", "size_hi", &firmwareSize, 1);

	if(ret || firmwareSize)
	{
		__wrn("fetch para: partition0 size_hi from script failed\n");
		return 0;
	}

	ret = esCFG_GetKeyValue("partition0", "size_lo", &firmwareSize, 1);

	if(ret || firmwareSize <= 0)
	{
		__wrn("fetch para: partition0 size_lo from script failed\n");
		return 0;
	}

	__wrn("partition0 size_lo = %ld\n", firmwareSize);

	switch(firmwareSize)
	{
		case 3823 :
			firmwareSize = 4643840;
			break;

		case 7919 :
			firmwareSize = 8838144;
			break;

		default :
			return 0;
	}

	if(*rootDir)
	{
		return CheckFileSize_Firmware(*rootDir, firmwareSize);
	}
	else
	{
		// 查找磁盘
		const char *c_pstrRatPartitionName[] = {RAT_SD_CARD, RAT_USB_DISK};
		__u8	i;

		for(i = 0; i < 2; i++)
		{
			char disk_name[RAT_MAX_PARTITION][4];
			eLIBs_memset(disk_name , 0 , sizeof(disk_name)) ;
			ret = rat_get_partition_name(c_pstrRatPartitionName[i], disk_name);

			if((EPDK_OK == ret) && disk_name[0][0])
			{
				*rootDir = disk_name[0][0];
				ret = CheckFileSize_Firmware(*rootDir, firmwareSize);

				if(ret)
				{
					return ret;
				}
			}
		}
	}

	return 0;
}

__s32 GetFileTotal(const char *searchPath, rat_media_type_t mediaType)
{
	HRAT 	hrat = NULL;
	__u32 	total = 0;
	__wrn("searchPath[%s]\n", searchPath);
	hrat = rat_open(searchPath, mediaType, 0);

	if(hrat == NULL)
	{
		return 0;
	}

	total = rat_get_cur_scan_cnt(hrat);
	__wrn("scan files total=%ld\n", total);
	return total;
}

__s32 GetFileTotal_Firmware(char *rootDir)
{
#ifdef PATH__LZ_C100_IMG_FOLDER
	char		path[] = PATH__LZ_C100_IMG_FOLDER;
#else
	char		path[] = "x:";
#endif

	if(*rootDir)
	{
		path[0] = *rootDir;
		return GetFileTotal(path, RAT_MEDIA_TYPE_FIRMWARE);
	}
	else
	{
		const char *c_pstrRatPartitionName[] = {RAT_SD_CARD, RAT_USB_DISK};
		__u8	i;

		for(i = 0; i < sizeof(c_pstrRatPartitionName) / sizeof(c_pstrRatPartitionName[0]); i++)
		{
			__s32	ret;
			char	disk_name[RAT_MAX_PARTITION][4];
			eLIBs_memset(disk_name , 0 , sizeof(disk_name)) ;
			ret = rat_get_partition_name(c_pstrRatPartitionName[i], disk_name);

			if((EPDK_OK == ret) && disk_name[0][0])
			{
				*rootDir = disk_name[0][0];
				path[0] = *rootDir;
				ret = GetFileTotal(path, RAT_MEDIA_TYPE_FIRMWARE);

				if(ret)
				{
					return ret;
				}
			}
		}
	}

	return 0;
}

char app_root_detect_update_firmware(root_ctrl_t *root_ctrl)
{
	char 	rootDir = 0;

	if(root_ctrl->m_updateFirmwareLog == LOG_UpdateFirmware_By_FSPlugin)
	{
		rootDir = root_ctrl->root_para->fw_update_file_disk;
	}

	{
		// 方案1
		if(DetectUpdateFirmware(&rootDir))
		{
			// 有满足以前条件的可升级固件
			root_ctrl->root_para->fw_update_file_disk = rootDir;
			__app_root_delete_all_app(root_ctrl);
			app_root_cmd2self(root_ctrl, FW_UPDATE_DIALOG_ID, ADLG_CMD_SET_UI, 0, 0);
			return rootDir;
		}
	}

	root_ctrl->m_updateFirmwareLog = 0; // 退出此函数后，以下方案暂不使用此标志
	{
		// 方案2
		if(GetFileTotal_Firmware(&rootDir))
		{
			root_para_t *root_para = root_ctrl->root_para;
			__app_root_delete_all_app(root_ctrl);
			root_para->root_type = __app_root_get_rat_root_type(__app_root_get_reg_storage_type(rootDir));
			root_para->explr_root = root_para->root_type;
			root_para->data = ID_EXPLORER_FIRMWARE;
			root_para->return_to_explorer_file_list = 0 ;
			app_root_cacheon();
			root_ctrl->h_app_explorer =  app_explorer_create(root_para);
			__msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
			GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
			app_root_cacheoff();
			return rootDir;
		}
	}
	return 0;
}

void app_root_update_firmware_giveup(root_ctrl_t *root_ctrl)
{
	__s32 ret = EPDK_FAIL;
	__s32 reg_storage_type;
	__s32 reg_media_type;
	__wrn("root_ctrl->m_updateFirmwareLog = %d\n", root_ctrl->m_updateFirmwareLog);

	switch(root_ctrl->m_updateFirmwareLog)
	{
		case LOG_UpdateFirmware_By_RootCreate :
			reg_media_type = -1;
			reg_storage_type = -1;
			ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_first_play, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_TRUE);
			break;

		case LOG_UpdateFirmware_By_FSPlugin :
			reg_storage_type = __app_root_get_reg_storage_type(g_strAbsolutePath_UpdateFirmware[0]);
			reg_media_type = -1;

			if(-1 != reg_storage_type)
			{
				ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_plugin_for_play, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_FALSE);
			}
			else		// 磁盘不存在了,切换磁盘
			{
				reg_root_para_t *para;
				para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

				if(para)
				{
					reg_media_type = para->last_play_app;
					__wrn("reg_root_para->last_play_app = %d\n", para->last_play_app);
					ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_switch_media, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_TRUE);
				}
			}

			break;

		default :
			break;
	}

	root_ctrl->m_updateFirmwareLog = 0;

	if(EPDK_FAIL == ret)
	{
		__app_home_restore_ex(root_ctrl);
	}
}
#endif

static void app_root_cacheon(void)
{
	//GUI_LyrWinCacheOn();
}

static void app_root_cacheoff(void)
{
	//GUI_LyrWinCacheOff();
}

//volume
static __s32 __app_root_volume_scene_create(root_ctrl_t *root_ctrl)
{
	__s32 ret;

	if(NULL == root_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	__wrn("root_ctrl->volume_scene=0x%x\n", root_ctrl->volume_scene);

	if(NULL == root_ctrl->volume_scene)
	{
		com_volume_create_para_t create_para;
		eLIBs_memset(&create_para, 0, sizeof(com_volume_create_para_t));
		create_para.hparent = root_ctrl->root_para->h_parent;
		create_para.scene_id = COM_VOLUME_ID;
		create_para.m_eWinVolume = root_ctrl->m_eWinVolume;

		switch(create_para.m_eWinVolume)
		{
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)

			case m_eWinVolume_echo:
				create_para.max_val = 8;
				create_para.min_val = 1;
				create_para.cur_val = dsk_echo_level_get_setting() + 1;
				break;
#endif

			case m_eWinVolume_default :
			default :
				create_para.max_val = AUDIO_DEVICE_VOLUME_MAX;
				create_para.min_val = AUDIO_DEVICE_VOLUME_MIN;
				create_para.cur_val = dsk_volume_get();
				break;
		}

		root_ctrl->volume_scene = com_volume_scene_create(&create_para);

		if(NULL == root_ctrl->volume_scene)
		{
			__msg("com_volume_scene_create fail...\n");
			return EPDK_FAIL;
		}
	}

	return EPDK_OK;
}
static __s32 app_root_restore_focus_child(root_ctrl_t *root_ctrl)
{
	if(root_ctrl->h_app_explorer)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
	}
	else if(root_ctrl->h_app_movie)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_movie);
	}
	else if(root_ctrl->h_app_new_movie)//新添加的movie视频应用程序app
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_movie);
	}
	else if(root_ctrl->h_app_photo)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_photo);
	}
	else if(root_ctrl->h_app_ebook)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_ebook);
	}
	else if(root_ctrl->h_app_new_ebook)//新添加电子书窗口选中发送按键命令
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_ebook);
	}
	else if(root_ctrl->h_app_setting)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_setting);
	}
	else if(root_ctrl->h_app_new_setting)//新添加通用设置app
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_setting);
	}
	else if(root_ctrl->h_app_record)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_record);
	}
#if SP_APP_ATV
	else if(root_ctrl->h_app_atv)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_atv);
	}
#endif
#if SP_APP_AUX
	else if(root_ctrl->h_app_linin
					&& LINEIN_MODE_NORMAL == app_linein_get_mode(root_ctrl->h_app_linin))
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_linin);
	}
#endif	
	else if(root_ctrl->h_app_music
					&& MUSICPLAYER_NORMAL == app_music_get_mode(root_ctrl->h_app_music))
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_music);
	}
	else if(root_ctrl->h_app_new_music
					&& MUSICPLAYER_NORMAL == app_music_get_mode(root_ctrl->h_app_new_music))//新添加音乐应用
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_music);
	}
	else if(root_ctrl->h_app_fm)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_fm);
	}
	else if(root_ctrl->h_app_calendar)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_calendar);
	}
	else if(root_ctrl->h_app_home
					&& MULTI_SCREEN_HOME_MODE_NORMAL == app_multi_screen_home_get_mode(root_ctrl->h_app_home))
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_home);
	}
	return EPDK_OK;
}

static __s32 __com_volume_scene_delete(root_ctrl_t *root_ctrl)
{
	__s32 ret;

	if(NULL == root_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__msg("root_ctrl->volume_scene=0x%x\n", root_ctrl->volume_scene);

		if(root_ctrl->volume_scene)
		{
			com_volume_scene_delete(root_ctrl->volume_scene);
			root_ctrl->volume_scene = NULL;
			app_root_restore_focus_child(root_ctrl);	//防止之后按键消息传不到子APP中
		}
	}

	return EPDK_OK;
}


static __s32 __com_volume_scene_set_focus(root_ctrl_t *root_ctrl)
{
	__s32 ret;

	if(NULL == root_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		if(root_ctrl->volume_scene)
		{
			com_volume_scene_set_focus(root_ctrl->volume_scene);
		}
	}

	return EPDK_OK;
}


/************************************全局检测初始化******************************************/
#if SP_AUX_DET
__s32 app_root_is_linein_insert(void)
{
	H_WIN root;
	root_ctrl_t   *root_ctrl;
	__s32 cur_read_data;
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return EPDK_FALSE;
	}

	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(root);

	if(!root_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FALSE;
	}
	
#if SP_DEV_DET_PLUGIN_BY_AD
	if(root_ctrl->globe_check.linein_In)
	{
		return EPDK_TRUE;
	}
	
#else

	if(NULL == root_ctrl->globe_check.hdl)
	{
		__wrn("root_ctrl->globe_check.hdl is null...\n");
		return EPDK_FALSE;
	}

	cur_read_data = esPINS_ReadPinData(root_ctrl->globe_check.hdl, 0);
	__wrn("cur_read_data=%d\n", cur_read_data);

	if(STA__AUX__PLUGIN == cur_read_data)
	{
		return EPDK_TRUE;
	}

#endif
	return EPDK_FALSE;
}

#else
__s32 app_root_is_linein_insert(void)
{
	return EPDK_FALSE;
}
#endif

#if SP_AUX_DET
static __s32 app_root_linein_change(root_ctrl_t *para, __s32 lineinSt)
{
	__s32 ret;
	__gui_msg_t 	msg;
	//    __wrn("lineinSt=%d\n", lineinSt);
	//__wrn("do not process linin now...\n");
	//    return EPDK_FAIL;//112350

#if (SP_AUX_DET == OP_AUX_DET_FOR_LINEIN)
	if(lineinSt == 1)  // in
	{
#if 1
		__wrn("linein plug in process...\n");

		if(NULL == para->h_app_linin)
		{
			__wrn("begin send msg to kill linein");
			app_root_cmd2self(para,
												APP_LINEIN_ID,
												SWITCH_TO_LINE_IN,
												0,
												0);
		}

#endif
	}
	else if(lineinSt == -1)  //out
	{
		__wrn("linein plug out process...\n");
		__wrn("para->h_app_linin=%x\n", para->h_app_linin);

		if(para->h_app_linin)
		{
			__wrn("begin send msg to kill linein");
			app_root_cmd2self(para,
												APP_LINEIN_ID,
												SWITCH_TO_MMENU,
												0,
												0);
		}
	}

#else
	if(lineinSt == 1)  // in
	{
		__wrn("linein plug in process...\n");

		if((para->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) != OP_RootFlags_CVBS_InputMode_AVINFromDetect)
		{
			if(!app_atv_isAVInput())
			{
				app_root_cmd2self(para,
				                  APP_ATV_ID,
				                  SWITCH_TO_ATV,
				                  m_eShiftMode_avin |(1<<17),
				                  0);
			}
		}
	}
	else if(lineinSt == -1)  //out
	{
		__wrn("linein plug out process...\n");

		if((para->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) == OP_RootFlags_CVBS_InputMode_AVINFromDetect)
		{
			if(app_atv_isAVInput())
			{
				app_root_cmd2self(para,
				                  APP_ATV_ID,
				                  SWITCH_TO_MMENU,
				                  0,
				                  0);
			}
		}
	#if SP_APP_AUX
		else if(para->h_app_linin)
		{
			__wrn("begin send msg to kill linein");
			app_root_cmd2self(para,
			                  APP_LINEIN_ID,
			                  SWITCH_TO_MMENU,
			                  0,
			                  0);
		}
	#endif
	}

#endif
	return EPDK_OK;
}

static __s32 app_root_get_linein_state(root_ctrl_t *para)
{
#if SP_DEV_DET_PLUGIN_BY_AD
	static __s8 s_curSta = -1;
	if(para->globe_check.linein_In)
	{
		if(s_curSta != 1)
		{
			s_curSta = 1;
			return 1;
		}
	}
	else
	{
		if(s_curSta != -1)
		{
			s_curSta = -1;
			return -1;
		}
	}
	
	return 0;

#else
	static __s32 recorded_cur_data = STA__AUX__PLUGOUT;//记录当前状态，刚开始默认没插入linein
	static __s32 last_read_data = 1;
	__s32 cur_read_data;
	__s32 ret = 0;//0 none, 1 in, -1 out
	static s32 cntDly = 0;
	static __s32 line_in_delay = 20; //防抖动延时

	if(NULL == para->globe_check.hdl)
	{
		return 0;
	}

	//	__msg("enter app_root_get_linein_state\n");
	cur_read_data = esPINS_ReadPinData(para->globe_check.hdl, 0);
	//	__msg("linein pin data=%d\n", cur_read_data);

	if(EPDK_TRUE == g_linin_is_first_read)//第一次试探性的读取
	{
		last_read_data = cur_read_data;
		g_linin_is_first_read = EPDK_FALSE;
		return 0;
	}

	if(-1 == cur_read_data)
	{
		return 0;
	}

	if(cur_read_data == last_read_data)//当前读取值跟上次一样，计数加1
	{
		cntDly++;
	}
	else
	{
		cntDly = 0;//重新开始第一次读取
	}

	//__wrn("cntDly=%d\n", cntDly);
	last_read_data = cur_read_data;
	ret = 0;

	if(cntDly >= line_in_delay)//连续一段时间读取值无变化，认为是有效的读取
	{
		//__wrn("cur_read_data=%d\n", cur_read_data);
		if(STA__AUX__PLUGOUT == cur_read_data)
		{
			if(STA__AUX__PLUGIN == recorded_cur_data)
			{
				para->globe_check.linein_In = EPDK_TRUE;
				line_in_delay = 20;
				ret = -1;
			}
		}
		else if(STA__AUX__PLUGIN == cur_read_data)
		{
			if(STA__AUX__PLUGOUT == recorded_cur_data)
			{
				para->globe_check.linein_In = EPDK_TRUE;
				line_in_delay = 2;
				ret = 1;
			}
		}

		recorded_cur_data = cur_read_data;//记录当前有效状态
		cntDly = 0;
	}

	return ret;
#endif
}

static __s32 app_root_globe_check_init(root_ctrl_t *para)
{
#if SP_DEV_DET_PLUGIN_BY_AD
{
	ES_FILE *pKeyFile;
	pKeyFile = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");
	
	if(!pKeyFile)
	{
		__wrn("key connot be openned!\n");
	}
	else
	{
		eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_DET_AUX_PLUG_EN, 0, 0);
		eLIBs_fclose(pKeyFile);
	}
}
#else
	__s32            ret;
	user_gpio_set_t  gpio_set[1];
	{
		//如果是debug模式，则把pa0口给jtag用
		__target_para_t target;
		esKSRV_GetTargetPara(&target);
		__wrn("target.debugenable=%d\n", target.debugenable);

		if(target.debugenable == EPDK_TRUE)
		{
			__wrn("debug mode, do not check linin now...\n");
			return EPDK_FAIL;
		}
	}
	g_linin_is_first_read = EPDK_TRUE;
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	// gpio_set->port = 1;				//PA
	// gpio_set->port_num = 0;			//0
	// gpio_set->mul_sel = 0;			//input
	// gpio_set->pull = 2;				//pull up
	// gpio_set->drv_level = 1;
	// gpio_set->data = 0;				//low
	ret = esCFG_GetKeyValue("linein_para", "linein",
	                        (int *)gpio_set, sizeof(user_gpio_set_t) / sizeof(int));

	if(EPDK_OK != ret)
	{
		__wrn("read linein para fail...\n");
		return EPDK_FAIL;
	}

	para->globe_check.hdl = esPINS_PinGrpReq(gpio_set, 1);

	if(NULL == para->globe_check.hdl)
	{
		__wrn("request linein pin fail...\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(para->globe_check.hdl, PIN_PULL_UP, NULL);

	if(ret)
	{
		__wrn("app_root_globe_check_init() pull input_row1 failed\n");
		return EPDK_FAIL;
	}

#endif
	return EPDK_OK;
}

static __s32 app_root_globe_check_uninit(root_ctrl_t *para)
{
#if SP_DEV_DET_PLUGIN_BY_AD
{
	ES_FILE *pKeyFile;
	pKeyFile = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");
	
	if(!pKeyFile)
	{
		__wrn("key connot be openned!\n");
	}
	else
	{
		eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_DET_AUX_PLUG_DIS, 0, 0);
		eLIBs_fclose(pKeyFile);
	}
}
#else
	if(NULL == para || NULL == para->globe_check.hdl)
	{
		__wrn("request linein pin fail...\n");
		return EPDK_FAIL;
	}

	esPINS_PinGrpRel(para->globe_check.hdl, 0);
	para->globe_check.hdl = NULL;

#endif
	return EPDK_OK;
}

/************************************全局检测线程******************************************/
static void app_root_globe_thread(void *arg)
{
	__bool		loopFlag = EPDK_TRUE;
	root_ctrl_t *para;
	root_para_t *root_para;
	__s32		lineinSt = 0;//0 none, 1 in, -1 out
	__wrn("******app_root_globe_thread begin********\n");

	if(arg != NULL)
	{
		para = (root_ctrl_t *)arg;
		root_para = (root_para_t *)para->root_para;
		app_root_globe_check_init(para);
	}
	else
	{
		__here__;
		loopFlag = EPDK_FALSE;
	}

	//	__wrn("loopFlag=%d\n", loopFlag);
#if 1

	while(loopFlag)
	{
		//		__here__;
		if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
		{
			//			__here__;
			loopFlag = EPDK_FALSE;
		}

		//		__here__;
		//Line in check
		lineinSt = app_root_get_linein_state(para);
		//__msg_jjl("lineinSt=%d, root_para->m_maskFlags=0x%X, app_atv_isAVInput()=%d\n", lineinSt, root_para->m_maskFlags, app_atv_isAVInput());

#if SP_APP_ATV
	#if (SP_AUX_DET == OP_AUX_DET_FOR_AVIN_LINEIN) || (SP_AUX_DET == OP_AUX_DET_FOR_AVIN)
		if((root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) != OP_RootFlags_CVBS_InputMode_AVINFromDetect)
	#endif
		{
			if(app_atv_isAVInput())
			{
				lineinSt = 0;
			}
		}
#endif
		//		__here__;
		//		__msg("lineinSt=%d\n", lineinSt);
		app_root_linein_change(para, lineinSt);
		//		__here__;
	#if SP_APP_ALARM
		//alarm check
		alarm_check_run();
	#endif
		//		__here__;
		esKRNL_TimeDly(10);
	}

#endif
	//	__here__;
	app_root_globe_check_uninit(para);
	esKRNL_TDel(OS_PRIO_SELF);
	__wrn("******app_root_globe_thread end********\n");
}
#endif


static __s32 __app_root_broadcast_msg(__gui_msg_t *msg)
{
	H_WIN root, child;
	char  winname[256];
	__gui_msg_t set_msg;
	eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
	root  = msg->h_deswin;
	child = GUI_WinGetFirstChild(root);

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		__msg("app_root_broadcast_msg msg->id=%d,winname = %s \n",
		      msg->id, winname);
		set_msg.h_deswin = child;
		GUI_SendMessage(&set_msg);
		// GUI_SendNotifyMessage(&set_msg);
		__msg("");
		child = GUI_WinGetNextBro(child);
	}

	return EPDK_OK;
}

static void  app_root_cmd2self(root_ctrl_t *para, __s32 lword, __s32 hword, __s32 data2, __s32 reserved)
{
	__gui_msg_t msg;
	eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
	msg.h_deswin = para->root_para->h_parent;
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(lword, hword);
	msg.dwAddData2 = data2;
	msg.dwReserved = reserved;
	GUI_SendMessage(&msg);
}

static __s32  __app_root_send_msg2focus_child(__gui_msg_t *msg)
{
	H_WIN root, child;
	__gui_msg_t set_msg;
	eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
	root  = msg->h_deswin;
	child = GUI_WinGetFocusChild(root);

	if(child)
	{
		GUI_SendMessage(&set_msg);
	}

	return EPDK_OK;
}

//向hwin发送异步消息，执行pfun函数
static __s32 __app_do_async(H_WIN hwin, __u32 msg_id, p_app_root_fun pfun, void *arg)
{
	if(0 == hwin)
	{
		return EPDK_FAIL;
	}

	{
		__gui_msg_t setmsg;
		setmsg.id			=  msg_id;
		setmsg.h_srcwin		=  NULL;
		setmsg.h_deswin		=  hwin;
		setmsg.dwAddData1	= (__u32)pfun;
		setmsg.dwAddData2	= (__u32)arg;
		setmsg.p_arg		=  NULL;
		__msg("setmsg.id=%x, setmsg.h_deswin=%x, setmsg.dwAddData1=%x\n, setmsg.dwAddData2=%x\n",
		      setmsg.id, setmsg.h_deswin, setmsg.dwAddData1, setmsg.dwAddData2);
		GUI_SendNotifyMessage(&setmsg);
	}

	return EPDK_OK;
}

//向hwin发送setfocus消息，syn 为1代表需要直接调用GUI_WinSetFocusChild。
static void __app_set_focus_child(H_WIN hwin, __s32 syn)
{
	if(0 == hwin)
	{
		return;
	}

	if(1 == syn)
	{
		GUI_WinSetFocusChild(hwin);
	}
	else
	{
		__gui_msg_t setmsg;
		setmsg.id			=  GUI_MSG_SET_FOCUS;
		setmsg.h_srcwin		=  NULL;
		setmsg.h_deswin		=  hwin;
		setmsg.dwAddData1	=  NULL;
		setmsg.dwAddData2	=  NULL;
		setmsg.p_arg		=  NULL;
		GUI_SendNotifyMessage(&setmsg);
	}
}

static __bool __app_root_has_forground_apps(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__wrn("__app_root_has_forground_apps\n");

	if(!para)
	{
		__err("invalid para...\n");
		return EPDK_FALSE;
	}

	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return EPDK_FALSE;
	}

	child = GUI_WinGetFirstChild(root);

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);
		__wrn("winname = %s \n", winname);

		//if(0 == eLIBs_stricmp(winname, APP_MULTI_SCREEN_HOME)
		//        ||0 == eLIBs_stricmp(winname, APP_HOME))
		if(0 == eLIBs_stricmp(winname, APP_HOME))
		{
			multi_screen_home_mode_e mode;
			mode = app_multi_screen_home_get_mode(child);

			if(MULTI_SCREEN_HOME_MODE_NORMAL == mode)
			{
				__wrn("root has child:%s\n", winname);
				return EPDK_TRUE;
			}
		}
		else if(0 == eLIBs_stricmp(winname, APP_MUSIC))
		{
			musicplayer_mode_e mode;
			mode = app_music_get_mode(child);

			if(MUSICPLAYER_NORMAL == mode)
			{
				__wrn("found forground music...\n");
				return EPDK_TRUE;
			}
		}
		else if(0 == eLIBs_stricmp(winname, APP_NEWMUSIC))//新添加音乐
		{
			musicplayer_mode_e mode;
			mode = app_music_get_mode(child);

			if(MUSICPLAYER_NORMAL == mode)
			{
				__wrn("found forground new music...\n");
				return EPDK_TRUE;
			}
		}
	#if SP_APP_AUX
		else if(0 == eLIBs_stricmp(winname, APP_LINEIN))
		{
			linein_mode_e mode;
			mode = app_linein_get_mode(child);

			if(LINEIN_MODE_NORMAL == mode)
			{
				__wrn("found forground linein...\n");
				return EPDK_TRUE;
			}
		}
	#endif
		else
		{
			__wrn("root has child:%s\n", winname);
			return EPDK_TRUE;
		}

		child = next_child;
	}

	__wrn("root has no child\n");
	return EPDK_FALSE;
}

static __s32 __app_root_is_bg_music_exist(__gui_msg_t *msg, H_WIN *hMusic)
{
	H_WIN root, child;
	char  winname[256];
	__bool exist;
	root  = GUI_WinGetHandFromName(APP_ROOT);
	child = GUI_WinGetFirstChild(root);
	exist = 0;
	*hMusic = 0;

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		__msg("winname = %s \n", winname);

		if(0 == eLIBs_strcmp(APP_MUSIC, winname))
		{
			*hMusic = child;
			exist = 1;
			break;
		}
		else if(0 == eLIBs_strcmp(APP_NEWMUSIC, winname))
		{
			*hMusic = child;
			exist = 1;
			break;
		}

		child = GUI_WinGetNextBro(child);
	}

	return exist;
}

__s32 __app_root_get_bg_music_index(__u32 *index)
{
	H_WIN root, child;
	char  winname[256];
	H_WIN  hMusic = NULL;
	root  = GUI_WinGetHandFromName(APP_ROOT);
	child = GUI_WinGetFirstChild(root);
	hMusic = NULL;

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);

		if(0 == eLIBs_strcmp(APP_MUSIC, winname))
		{
			hMusic = child;
			break;
		}
		else if(0 == eLIBs_strcmp(APP_NEWMUSIC, winname))
		{
			hMusic = child;
			break;
		}

		child = GUI_WinGetNextBro(child);
	}

	if(NULL == hMusic)
	{
		__wrn("no music app currently...\n");
		return 0;
	}

	return app_music_get_index(hMusic, index);
}

static __s32 app_root_set_music_bg_mode(__gui_msg_t *msg)
{
	__s32 ret;
	root_ctrl_t   *root_ctrl;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	ret = EPDK_FAIL;

	if(root_ctrl->h_app_music)
	{
#if (0==SUPPORT_MUSIC_BG_MODE)
		GUI_ManWinDelete(root_ctrl->h_app_music);
		root_ctrl->h_app_music = NULL;
#else
		ret = app_music_set_bg_mode(root_ctrl->h_app_music);
#endif
	}
	else if(root_ctrl->h_app_new_music)
	{
#if (0==SUPPORT_MUSIC_BG_MODE)
		GUI_ManWinDelete(root_ctrl->h_app_new_music);
		root_ctrl->h_app_new_music = NULL;
#else
		ret = app_music_set_bg_mode(root_ctrl->h_app_new_music);
#endif
	}

	return ret;
}

static void __app_root_delete_bg_linein(__gui_msg_t *msg)
{
#if SP_APP_AUX
	H_WIN root, child, next_child;
	char  winname[256];
	root_para_t *root_para;
	root_ctrl_t   *root_ctrl;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	root_para = root_ctrl->root_para;
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return ;
	}

	child = GUI_WinGetFirstChild(root);

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);
		__msg("winname = %s \n", winname);

		if(0 == eLIBs_strcmp(APP_LINEIN, winname))
		{
			GUI_ManWinDelete(child);
			root_ctrl->h_app_linin = NULL;
			break;
		}

		child = next_child;
	}
#endif
}

static void __app_root_delete_bg_music(__gui_msg_t *msg)
{
	H_WIN root, child, next_child;
	char  winname[256];
	root_para_t *root_para;
	root_ctrl_t   *root_ctrl;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	root_para = root_ctrl->root_para;
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return ;
	}

	child = GUI_WinGetFirstChild(root);

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);
		__msg("winname = %s \n", winname);

		if(0 == eLIBs_strcmp(APP_MUSIC, winname))
		{
			GUI_ManWinDelete(child);
			root_ctrl->h_app_music = NULL;
			break;
		}
		else if(0 == eLIBs_strcmp(APP_NEWMUSIC, winname))//新添加的音乐应用manwin窗口
		{
			GUI_ManWinDelete(child);
			root_ctrl->h_app_new_music = NULL;
			break;
		}

		child = next_child;
	}
}

static void __app_root_create_bg_music(__gui_msg_t *msg)
{
	H_WIN root, child;
	char  winname[256];
	__bool exist;
	root_para_t *root_para;
	root_ctrl_t   *root_ctrl;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	root_para = root_ctrl->root_para;
	root  = GUI_WinGetHandFromName(APP_ROOT);
	child = GUI_WinGetFirstChild(root);
	exist = 0;

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, 256);
		GUI_WinGetName(child, winname);
		__msg("winname = %s \n", winname);

		if(0 == eLIBs_strcmp(APP_MUSIC, winname))
		{
			exist = 1;
			break;
		}
		else if(0 == eLIBs_strcmp(APP_NEWMUSIC, winname))//新添加音乐
		{
			exist = 1;
			break;
		}

		child = GUI_WinGetNextBro(child);
	}

	if(0 == exist)
	{
		root_para->data = MUSICPLAYER_BACKGROUND;
		//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
		root_ctrl->h_app_music =  app_music_create(root_para);
	}
}


//进入应用之前把背景设置为默认背景
static void __app_root_change_to_default_bg(void)
{
#if 0
	reg_init_para_t *para;
	para = dsk_reg_get_para_by_app(REG_APP_INIT);

	if(para)
	{
		para->bgd_flag_in_main = gscene_bgd_get_flag();

		if(EPDK_TRUE == para->bgd_flag_in_main)//当前是默认背景，则不做任何事情
		{
		}
		else//当前不是默认背景，修改为默认背景
		{
			gscene_bgd_set_flag(EPDK_TRUE);
			gscene_bgd_refresh();
		}
	}
	else
	{
		__msg("dsk_reg_get_para_by_app fail...\n");
	}

#endif
}

static void __app_root_delete_all_app_except_home(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];

	if(!para)
	{
		__err("invalid para...\n");
	}

	__app_root_delete_sos(para);
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return ;
	}

	child = GUI_WinGetFirstChild(root);
	__wrn("begin delete apps except music...\n");

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, sizeof(winname));
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);

		if(0 != eLIBs_strcmp(winname, APP_HOME)
		    && 0 != eLIBs_strcmp(winname, APP_MULTI_SCREEN_HOME))
		{
			__wrn("winname=%s\n", winname);
			GUI_ManWinDelete(child);
		}

		child = next_child;
	}

	//para->h_app_home = NULL;
	para->h_app_explorer = NULL;
	para->h_app_movie    = NULL;
	para->h_app_music    = NULL;
	para->h_app_photo    = NULL;
	para->h_app_fm       = NULL;
	para->h_app_setting  = NULL;
	para->h_app_ebook    = NULL;
	para->h_app_calendar = NULL;
	para->h_app_record   = NULL;
	para->h_app_new_setting = NULL;//新添加通用设置app
	para->h_app_new_movie = NULL;//新添加的movie应用app
	para->h_app_new_ebook = NULL;//清空新添加的电子书manwin窗口句柄
#if SP_APP_AUX
	para->h_app_linin    = NULL;
#endif	
#if SP_APP_ATV
	para->h_app_atv    = NULL;
#endif
	__app_home_restore_ex(para);
	__wrn("*******__app_root_delete_all_app_except_bg_audio end*******\n");
}


static void __app_root_delete_all_app_except_bg_audio(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];

	if(!para)
	{
		__err("invalid para...\n");
	}

	__app_root_delete_sos(para);
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return ;
	}

	child = GUI_WinGetFirstChild(root);
	__wrn("begin delete apps except music...\n");

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, sizeof(winname));
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);

		if(0 != eLIBs_strcmp(winname, APP_MUSIC)
		    && 0 != eLIBs_strcmp(winname, APP_LINEIN))
		{
			__wrn("winname=%s\n", winname);
			GUI_ManWinDelete(child);
		}
		else if(0 != eLIBs_strcmp(winname, APP_NEWMUSIC)
		    && 0 != eLIBs_strcmp(winname, APP_LINEIN))
		{
			__wrn("winname=%s\n", winname);
			GUI_ManWinDelete(child);
		}

		child = next_child;
	}

	para->h_app_home = NULL;
	para->h_app_explorer = NULL;
	para->h_app_movie    = NULL;
	//para->h_app_music    = NULL;
	para->h_app_photo    = NULL;
	para->h_app_fm       = NULL;
	para->h_app_setting  = NULL;
	para->h_app_new_setting  = NULL;
	para->h_app_ebook    = NULL;
	para->h_app_calendar = NULL;
	para->h_app_record   = NULL;
	para->h_app_new_movie = NULL;//新添加的movi应用app
	para->h_app_new_ebook = NULL;
	//para->h_app_new_music = NULL;//新添加音乐
	//para->h_app_linin    = NULL;
#if SP_APP_ATV
	para->h_app_atv   = NULL;
#endif
	__wrn("*******__app_root_delete_all_app_except_bg_audio end*******\n");
}

static void __app_root_delete_all_audio_app(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];

	if(!para)
	{
		__err("invalid para...\n");
	}

	__app_root_delete_sos(para);
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return ;
	}

	child = GUI_WinGetFirstChild(root);
	__wrn("begin delete audio apps...\n");

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, sizeof(winname));
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);

		if(0 == eLIBs_strcmp(winname, APP_MUSIC)
		    || 0 == eLIBs_strcmp(winname, APP_MOVIE)
		    || 0 == eLIBs_strcmp(winname, APP_FM)
		    || 0 == eLIBs_strcmp(winname, APP_RECORD)
		#if SP_APP_AUX
		    || 0 == eLIBs_strcmp(winname, APP_LINEIN)
		#endif
		#if SP_APP_ATV
		    || 0 == eLIBs_strcmp(winname, APP_ATV)
		#endif
			|| 0 == eLIBs_strcmp(winname, APP_NEWMOVIE)
			|| 0 == eLIBs_strcmp(winname, APP_NEWMUSIC)//新添加音乐
		    )
		{
			__wrn("winname=%s\n", winname);
			GUI_ManWinDelete(child);
		}

		child = next_child;
	}

	para->h_app_movie    = NULL;
	para->h_app_music    = NULL;
	para->h_app_fm       = NULL;
	para->h_app_record   = NULL;
	para->h_app_new_movie = NULL;//新添加的movi应用app
	para->h_app_new_music = NULL;//新添加的音乐应用app
#if SP_APP_AUX
	para->h_app_linin    = NULL;
#endif
#if SP_APP_ATV
	para->h_app_atv   = NULL;
#endif
	__wrn("*******__app_root_delete_all_audio_app end*******\n");
}

static void __app_root_delete_all_app(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];

	if(!para)
	{
		__err("invalid para...\n");
	}

	__app_root_delete_sos(para);
	root  = GUI_WinGetHandFromName(APP_ROOT);

	if(!root)
	{
		__err("GUI_WinGetHandFromName fail...\n");
		return ;
	}

	child = GUI_WinGetFirstChild(root);
	__wrn("begin delete app apps...\n");

	while(child != NULL)
	{
		eLIBs_memset(winname, 0, sizeof(winname));
		GUI_WinGetName(child, winname);
		next_child = GUI_WinGetNextBro(child);
		__wrn("winname=%s\n", winname);
		GUI_ManWinDelete(child);
		child = next_child;
	}

	para->h_app_home = NULL;
	para->h_app_explorer = NULL;
	para->h_app_movie    = NULL;
	para->h_app_music    = NULL;
	para->h_app_photo    = NULL;
	para->h_app_fm       = NULL;
	para->h_app_setting  = NULL;
	para->h_app_ebook    = NULL;
	para->h_app_calendar = NULL;
	para->h_app_record   = NULL;
	para->h_app_new_setting = NULL;//新添加通用设置app
	para->h_app_new_movie = NULL;//新添加的movi应用app
	para->h_app_new_music = NULL;//新添加的音乐应用app
	para->h_app_new_ebook = NULL;//清除新添加的电子书句柄
	
#if SP_APP_AUX
	para->h_app_linin    = NULL;
#endif
#if SP_APP_ATV	
	para->h_app_atv   = NULL;
#endif
	__wrn("*******__app_root_delete_all_app end*******\n");
}

#if SP_APP_AUX
static __s32 __app_root_to_linein(root_ctrl_t *para)
{
	__u32 t_maskFlags;
	
	if(NULL == para)
	{
		return EPDK_FAIL;
	}
	
	if((NULL != para->h_app_linin) && (LINEIN_MODE_NORMAL == app_linein_get_mode(para->h_app_linin)))
	{
		__wrn("line in alreadly running...\n");
		return EPDK_FAIL;
	}

	//__app_root_delete_all_audio_app(para);
	t_maskFlags = para->root_para->m_maskFlags & MASK_RootFlags_AuxMode_BT;
	__app_root_delete_all_app(para);
	para->root_para->m_maskFlags |= t_maskFlags;
	
	{
		__s32 ret;
		linein_mode_e mode;
		//ret = __app_root_has_forground_apps(para);
		//if(EPDK_FALSE == ret)
		{
			mode = LINEIN_MODE_NORMAL;
		}
		/*else
		{
			mode = LINEIN_MODE_BACKGROUND;
		}
		*/
		__wrn("begin send msg to load linein, mode=%d\n", mode);
		app_root_cmd2self(para,
		                  APP_HOME_ID,
		                  SWITCH_TO_OTHER_APP,
		                  ID_LINEIN_SHORTCUT,
		                  mode);
		__wrn("*******__app_root_to_linein success*******\n");
	}
	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}
#endif

static __s32 __app_root_to_fm(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__wrn("*******__app_root_to_fm*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(para->h_app_fm)
	{
		__wrn("fm alreadly running...\n");
		return EPDK_OK;
	}

	__app_root_delete_all_app(para);
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_FM);
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_FM_SHORTCUT,
	                  NULL);
	__wrn("*******__app_root_to_fm success*******\n");
#if (SP_FM_IC != SP_FM_IC_CDT5218)
	IOCTRL__ATV_PW__SET_OFF();
#endif
	return EPDK_OK;
}
//root转到setting
static __s32 __app_root_to_setting(root_ctrl_t *para)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__s32 ret;
	root_para_t *root_para;
	root_para = para->root_para;
	__wrn("*******__app_root_to_setting*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(NULL != para->h_app_setting)
	{
		__wrn("setting alreadly running...\n");
		return EPDK_FAIL;
	}
	if(NULL != para->h_app_new_setting)
	{
		__wrn("new setting alreadly running...\n");
		return EPDK_FAIL;
	}
	__app_root_delete_all_app_except_bg_audio(para);
	__wrn("before set music bg mode...\n");

	if(para->h_app_music)
	{
#if (0==SUPPORT_MUSIC_BG_MODE)
		GUI_ManWinDelete(para->h_app_music);
		para->h_app_music = NULL;
#else
		ret = app_music_set_bg_mode(para->h_app_music);
#endif
	}
	else if(para->h_app_new_music)
	{
#if (0==SUPPORT_MUSIC_BG_MODE)
		GUI_ManWinDelete(para->h_app_new_music);
		para->h_app_new_music = NULL;
#else
		ret = app_music_set_bg_mode(para->h_app_new_music);
#endif
	}

	//    app_root_set_music_bg_mode(msg);
	__wrn("after set music bg mode...\n");
	
#if SP_APP_AUX
	if(para->h_app_linin)
	{
		app_linein_set_mode(para->h_app_linin, LINEIN_MODE_BACKGROUND);
	}
#endif

	para->root_para->setting_opt = 0;//通用设置
	para->h_app_setting =  app_setting_create(root_para);

	if(para->h_app_setting)
	{
		GUI_WinSetFocusChild(para->h_app_setting);
		__wrn("*******__app_root_to_setting success*******\n");
	}
	else
	{
		__wrn("create setting manwin fail...\n");
	}

	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}



static __s32 __app_root_to_audio(root_ctrl_t *para
                                 , __s32 rat_root_type)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__s32 ret;
	__wrn("*******__app_root_to_audio*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(RAT_TF != rat_root_type
	    && RAT_USB != rat_root_type)
	{
		__wrn("rat_root_type type error,do not play video...\n");
		return EPDK_FAIL;
	}

	//不要删除后台音乐
	// if(NULL != para->h_app_music
	//    && para->root_para->root_type == rat_root_type)
	//{
	//    __wrn("music alreadly running...\n");
	//    return EPDK_FAIL;
	//}
	//外部已经搜索并设置播放列表
	//ret = __app_root_set_playfile(para->quick_root_type);
	//if(EPDK_FAIL == ret)
	//{
	//    __wrn("no media file...\n");
	//    return -2;//没有媒体文件
	//}
	//如果音乐存在，并且需要播放的盘为当前播放盘，则不要删除后台音乐
	{
		__bool to_delete_music_first;
		to_delete_music_first = EPDK_TRUE;

		if(para->h_app_music)
		{
			__s32 ret;
			__u32 cur_rat_root_type;
			ret = app_music_get_root_type(para->h_app_music, &cur_rat_root_type);

			if(EPDK_OK == ret && cur_rat_root_type == rat_root_type)
			{
				to_delete_music_first = EPDK_FALSE;
			}
		}
		else if(para->h_app_new_music)
		{
			__s32 ret;
			__u32 cur_rat_root_type;
			ret = app_music_get_root_type(para->h_app_new_music, &cur_rat_root_type);

			if(EPDK_OK == ret && cur_rat_root_type == rat_root_type)
			{
				to_delete_music_first = EPDK_FALSE;
			}
		}

		if(EPDK_TRUE == to_delete_music_first)
		{
			__app_root_delete_all_app(para);
		}
		else
		{
			__app_root_delete_all_app_except_bg_audio(para);
		}
	}
	gscene_bgd_set_state(BGD_STATUS_SHOW);
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_MUSIC);
	para->root_para->explr_root = rat_root_type;
	__wrn("begin send msg to load music...\n");
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_MUSIC_SHORTCUT,
	                  rat_root_type);
	__wrn("*******__app_root_to_audio success*******\n");
	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}

static __s32 __app_root_to_sos(root_ctrl_t *para)
{
	__s32 ret;
	return EPDK_OK;
}

static __s32 __app_root_delete_sos(root_ctrl_t *para)
{
	__wrn("*******__app_root_delete_sos*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(EPDK_FALSE == para->sos_started)
	{
		__wrn("sos not running...\n");
		return EPDK_FAIL;
	}

	para->sos_started = EPDK_FALSE;
	dsk_volume_set(para->volume_before_sos);
	robin_set_cmd_stop();
	robin_close();

	if(GUI_IsTimerInstalled(para->root_para->h_parent, SOS_TIMER_ID))
	{
		GUI_KillTimer(para->root_para->h_parent, SOS_TIMER_ID);
	}

	__wrn("*******__app_root_delete_sos success*******\n");
	return EPDK_OK;
}



static __s32 __app_root_to_record(root_ctrl_t *para
                                  , __s32 rat_root_type)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__s32 ret;
	__wrn("*******__app_root_to_record*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(RAT_TF != rat_root_type
	    && RAT_USB != rat_root_type)
	{
		__wrn("rat_root_type type error,do not play video...\n");
		return EPDK_FAIL;
	}

	if(NULL != para->h_app_record
	    && para->root_para->root_type == rat_root_type)
	{
		__wrn("record alreadly running...\n");
		return EPDK_FAIL;
	}

	__app_root_delete_all_app(para);
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_RECORD);
	__wrn("begin send msg to load movie...\n");
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_RECORD_SHORTCUT,
	                  rat_root_type);
	__wrn("*******__app_root_to_record success*******\n");
	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}

#if SP_APP_ATV
__s32 __app_root_ready_to_atv(root_ctrl_t *root_ctrl, __u32 a_eShiftMode)
{
	E_SHIFT_MODE t_eShiftMode_getCur;

	switch(root_ctrl->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
	{
		case OP_RootFlags_CVBS_InputMode_TVFromHome :
			t_eShiftMode_getCur = m_eShiftMode_tv;
			break;

		case OP_RootFlags_CVBS_InputMode_DVDFromHome :
			t_eShiftMode_getCur = m_eShiftMode_dvd;
			break;
#if SP_APP_AUX
		case OP_RootFlags_CVBS_InputMode_AVINFromHome :
		case OP_RootFlags_CVBS_InputMode_AVINFromDetect :
			t_eShiftMode_getCur = m_eShiftMode_avin;
			break;
	#endif
	
		default :
			if(root_ctrl->h_app_atv)
			{
				t_eShiftMode_getCur = m_eShiftMode_atv;
			}
			else
			{
				t_eShiftMode_getCur = m_eShiftMode_max;
			}
			break;
	}

	if(t_eShiftMode_getCur == a_eShiftMode)	//不需要切换模式
	{
		return EPDK_FAIL;
	}

	root_ctrl->root_para->m_maskFlags &= ~MASK_RootFlags_CVBS_InputMode;

	switch(a_eShiftMode & 0xFFFF)
	{
		case m_eShiftMode_atv:
			break;
			
		case m_eShiftMode_tv :
			root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_TVFromHome;
			break;

		case m_eShiftMode_dvd :
			root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_DVDFromHome;
			break;

	#if SP_APP_AUX
		case m_eShiftMode_avin :
			if(a_eShiftMode & (1<<17))
			{
				root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_AVINFromDetect;
			}
			else
			{
				root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_AVINFromHome;
			}
			break;
	#endif

		default :
			return EPDK_FAIL;
	}

	return t_eShiftMode_getCur;
}

static __s32 __app_root_to_atv(root_ctrl_t *para)
{
	__u32 t_maskFlags;
	__wrn("*******__app_root_to_atv*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	t_maskFlags = para->root_para->m_maskFlags;
	__app_root_delete_all_app(para);
	para->root_para->m_maskFlags = t_maskFlags;
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_ATV_SHORTCUT,
	                  NULL);
	__wrn("*******__app_root_to_atv success*******\n");
	return EPDK_OK;
}

__s32 __app_root_to_atv_ex(root_ctrl_t *root_ctrl, __u32 a_shiftMode)
{
	if(EPDK_FAIL != __app_root_ready_to_atv(root_ctrl, a_shiftMode))
	{
		__app_root_to_atv(root_ctrl);
	}
}
#endif

static __bool is_on_tv(void)
{
	__disp_output_type_t output;
	
	output = dsk_display_get_output_type();
	if(DISP_OUTPUT_TYPE_LCD == output || DISP_OUTPUT_TYPE_NONE == output)
	{
		__wrn("not on tv now...\n");
		return EPDK_FALSE;
	}
	else
	{
		__wrn("on tv now...\n");
		return EPDK_TRUE;
	}
}

static __s32 __app_root_to_video(root_ctrl_t *para
                                 , __s32 rat_root_type)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__s32 ret;
	__movie_create_mode_e tv_mode;
	__wrn("*******__app_root_to_video*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(RAT_TF != rat_root_type
	    && RAT_USB != rat_root_type)
	{
		__wrn("rat_root_type type error,do not play video...\n");
		return EPDK_FAIL;
	}

	if(NULL != para->h_app_movie
	    && para->root_para->root_type == rat_root_type)
	{
		__wrn("movie alreadly running...\n");
		return EPDK_FAIL;
	}

	if(NULL != para->h_app_new_movie 
		&& para->root_para->root_type == rat_root_type)
	{
		__wrn("new movie alreadly running...\n");
		return EPDK_FAIL;

	}

	//外部已经搜索并设置播放列表
	//ret = __app_root_set_playfile(para->quick_root_type);
	//if(EPDK_FAIL == ret)
	//{
	//    __wrn("no media file...\n");
	//    return -2;//没有媒体文件
	//}

	if(EPDK_TRUE == is_on_tv())
	{
		tv_mode = MOVIE_MODE_TV;
	}
	else
	{
		tv_mode = MOVIE_MODE_NORMAL;
	}

	__app_root_delete_all_app(para);
	para->root_para->explr_root = rat_root_type;
	para->root_para->tv_mode = tv_mode;
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_MOVIE);
	__wrn("begin send msg to load movie...\n");
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_MOVIE_SHORTCUT,
	                  rat_root_type);
	__wrn("*******__app_root_to_video success*******\n");
	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}

static __s32 __app_root_to_ebook(root_ctrl_t *para
                                 , __s32 rat_root_type)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__s32 ret;
	__wrn("*******__app_root_to_ebook*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(RAT_TF != rat_root_type
	    && RAT_USB != rat_root_type)
	{
		__wrn("rat_root_type type error,do not play video...\n");
		return EPDK_FAIL;
	}

	if(NULL != para->h_app_ebook
	    && para->root_para->root_type == rat_root_type)
	{
		__wrn("ebook alreadly running...\n");
		return EPDK_FAIL;
	}
		
	if(NULL != para->h_app_new_ebook
	    && para->root_para->root_type == rat_root_type)//新添加电子书
	{
		__wrn("new ebook alreadly running...\n");
		return EPDK_FAIL;
	}

	//外部已经搜索并设置播放列表
	//ret = __app_root_set_playfile(para->quick_root_type);
	//if(EPDK_FAIL == ret)
	//{
	//    __wrn("no media file...\n");
	//    return -2;//没有媒体文件
	//}
	__app_root_delete_all_app_except_bg_audio(para);
	para->root_para->explr_root = rat_root_type;
	__wrn("begin send msg to load ebook...\n");
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_EBOOK_SHORTCUT,
	                  rat_root_type);
	__wrn("*******__app_root_to_ebook success*******\n");
	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}

static __s32 __app_root_to_photo(root_ctrl_t *para
                                 , __s32 rat_root_type)
{
	H_WIN root, child, next_child;
	char  winname[256];
	__s32 ret;
	__wrn("*******__app_root_to_photo*******\n");

	if(!para)
	{
		__err("invalid para...\n");
	}

	if(RAT_TF != rat_root_type
	    && RAT_USB != rat_root_type)
	{
		__wrn("rat_root_type type error,do not play video...\n");
		return EPDK_FAIL;
	}

	if(NULL != para->h_app_photo
	    && para->root_para->root_type == rat_root_type)
	{
		__wrn("photo alreadly running...\n");
		return EPDK_FAIL;
	}

	//外部已经搜索并设置播放列表
	//ret = __app_root_set_playfile(para->quick_root_type);
	//if(EPDK_FAIL == ret)
	//{
	//    __wrn("no media file...\n");
	//    return -2;//没有媒体文件
	//}
	__app_root_delete_all_app_except_bg_audio(para);
	para->root_para->explr_root = rat_root_type;
	__wrn("begin send msg to load photo...\n");
	app_root_cmd2self(para,
	                  APP_HOME_ID,
	                  SWITCH_TO_OTHER_APP,
	                  ID_PHOTO_SHORTCUT,
	                  rat_root_type);
	__wrn("*******__app_root_to_photo success*******\n");
	IOCTRL__ATV_PW__SET_OFF();
	return EPDK_OK;
}

static void __app_home_restore_ex(root_ctrl_t *root_ctrl)
{
	IOCTRL__ATV_PW__SET_OFF();

	if(NULL == root_ctrl)
	{
		__err("invalid para...\n");
		return;
	}

	if(root_ctrl->h_app_home)
	{
		__gui_msg_t mymsg;
		mymsg.h_deswin = root_ctrl->h_app_home;
		mymsg.id = DSK_APP_RESTORE;
		mymsg.dwAddData1 = 0;
		mymsg.dwAddData2 = 0;
		GUI_SendNotifyMessage(&mymsg);
		mymsg.h_deswin = root_ctrl->h_app_home;
		mymsg.id = DSK_MSG_FS_PART_PLUGOUT;
		mymsg.dwAddData1 = 0;
		mymsg.dwAddData2 = 0;
		GUI_SendNotifyMessage(&mymsg);
		__app_set_focus_child(root_ctrl->h_app_home, 0);
	}
	else
	{
		app_root_cacheon();
		root_ctrl->h_app_home = app_home_create(root_ctrl->root_para);
		__app_set_focus_child(root_ctrl->h_app_home, 0);
		app_root_cacheoff();
	}

	__wrn("app home restore...\n");
}

static void __app_home_restore(__gui_msg_t *msg)
{
	root_ctrl_t   *root_ctrl;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	__app_home_restore_ex(root_ctrl);
}

//return EPDK_OK, 表示成功,
//return EPDK_FAIL,表示盘符中没有视频\音频文件,则返回到浏览器,告知客户没有文件
static __s32 __app_root_set_playfile(rat_root_t rat_root_type, rat_media_type_t rat_media_type)
{
	char file_path[RAT_MAX_FULL_PATH_LEN];
	HRAT hrat = NULL;
	__u32 total = 0;
	__u32 last_file_index = 0;
	char *last_file_path = NULL;
	reg_root_para_t *para;
	int ret;
	rat_entry_t ItemInfo;

	if(RAT_UNKNOWN == rat_root_type || RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
	{
		return EPDK_FAIL;
	}

	eLIBs_memset(file_path, 0, sizeof(file_path));
	ret = __app_root_get_search_path(rat_root_type, file_path);

	if(EPDK_FAIL == ret || eLIBs_strlen(file_path) == 0)
	{
		return EPDK_FAIL;
	}

	//hrat = rat_open(file_path, rat_media_type, 0);
	hrat = rat_open_ex(rat_root_type, rat_media_type, 0);

	if(hrat == NULL)
	{
		return EPDK_FAIL;
	}

	total = rat_get_cur_scan_cnt(hrat);

	if(total == 0)
	{
		return EPDK_FAIL;
	}

	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(para == NULL)
	{
		return EPDK_FAIL;
	}

	if(RAT_MEDIA_TYPE_VIDEO == rat_media_type)
	{
		if(rat_root_type == RAT_TF)
		{
			last_file_index = para->last_movie_index_sd;
			last_file_path = para->last_movie_path_sd;
		}
		else
		{
			last_file_index = para->last_movie_index_ud;
			last_file_path = para->last_movie_path_ud;
		}
	}
	else if(RAT_MEDIA_TYPE_AUDIO == rat_media_type)
	{
		if(rat_root_type == RAT_TF)
		{
			last_file_index = para->last_music_index_sd;
			last_file_path = para->last_music_path_sd;
		}
		else
		{
			last_file_index = para->last_music_index_ud;
			last_file_path = para->last_music_path_ud;
		}
	}
	else if(RAT_MEDIA_TYPE_EBOOK == rat_media_type)
	{
		if(rat_root_type == RAT_TF)
		{
			last_file_index = para->last_ebook_index_sd;
			last_file_path = para->last_ebook_path_sd;
		}
		else
		{
			last_file_index = para->last_ebook_index_ud;
			last_file_path = para->last_ebook_path_ud;
		}
	}
	else if(RAT_MEDIA_TYPE_PIC == rat_media_type)
	{
		if(rat_root_type == RAT_TF)
		{
			last_file_index = para->last_photo_index_sd;
			last_file_path = para->last_photo_path_sd;
		}
		else
		{
			last_file_index = para->last_photo_index_ud;
			last_file_path = para->last_photo_path_ud;
		}
	}

	eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
	ret = rat_get_item_info_by_index(hrat, last_file_index, &ItemInfo);

	if(ret == EPDK_OK)
	{
		eLIBs_memset(file_path, 0, sizeof(file_path));
		eLIBs_strcpy(file_path, ItemInfo.Path);
		ret = eLIBs_strcmp(file_path, last_file_path);

		if(ret == 0)		//相等,则播放上一次的文件
		{
			rat_set_file_for_play(hrat, last_file_path);
			return EPDK_OK;
		}
	}

	ret = rat_get_item_info_by_index(hrat, 0, &ItemInfo);	//直接播放第一首

	if(ret == EPDK_OK)
	{
		eLIBs_memset(file_path, 0, sizeof(file_path));
		eLIBs_strcpy(file_path, ItemInfo.Path);
		rat_set_file_for_play(hrat, file_path);
		return EPDK_OK;
	}

	return EPDK_FAIL;
}

static rat_root_t __app_root_get_rat_root_type(__s32 reg_storage_type)
{
	rat_root_t rat_root_type;
	rat_root_type = RAT_UNKNOWN;

	if(0 == reg_storage_type)
	{
		rat_root_type = RAT_TF;
	}
	else if(1 == reg_storage_type)
	{
		rat_root_type = RAT_USB;
	}
	else
	{
		__wrn("reg_storage_type type error, change to 0...\n");
		rat_root_type = RAT_TF;
	}

	return rat_root_type;
}

static rat_media_type_t __app_root_get_rat_media_type(__s32 reg_media_type)
{
	rat_media_type_t rat_media_type;
	rat_media_type = RAT_MEDIA_TYPE_UNKNOWN;

	switch(reg_media_type)
	{
		case m_eMediaTypeList_video :
			rat_media_type = RAT_MEDIA_TYPE_VIDEO;
			break;

		case m_eMediaTypeList_audio:
			rat_media_type = RAT_MEDIA_TYPE_AUDIO;
			break;

		case m_eMediaTypeList_ebook:
			rat_media_type = RAT_MEDIA_TYPE_EBOOK;
			break;

		case m_eMediaTypeList_photo:
			rat_media_type = RAT_MEDIA_TYPE_PIC;
			break;

		default :
			rat_media_type = RAT_MEDIA_TYPE_AUDIO;
			break;
	}

	return rat_media_type;
}

static __s32 __app_root_get_reg_storage_type(unsigned char root)
{
	__s32 i;
	__s32 ret;
	char disk_name[RAT_MAX_PARTITION][4];
	char cur_disk_name[2];
	eLIBs_memset(cur_disk_name, 0, sizeof(cur_disk_name));
	cur_disk_name[0] = root;
	__wrn("cur_disk_name=%s\n", cur_disk_name);
	eLIBs_memset(disk_name , 0 , sizeof(disk_name)) ;
	ret = rat_get_partition_name(RAT_SD_CARD, disk_name);

	if(EPDK_OK == ret)
	{
		i = 0;

		while(i < RAT_MAX_PARTITION && disk_name[i][0])
		{
			__wrn("disk_name[%d]=%s\n", i, disk_name[i]);

			if(0 == eLIBs_strnicmp(disk_name[i], cur_disk_name, 1))
			{
				return 0;
			}

			i++;
		}
	}

	eLIBs_memset(disk_name , 0 , sizeof(disk_name)) ;
	ret = rat_get_partition_name(RAT_USB_DISK, disk_name);

	if(EPDK_OK == ret)
	{
		i = 0;

		while(i < RAT_MAX_PARTITION && disk_name[i][0])
		{
			__wrn("disk_name[i]=%s\n", disk_name[i]);

			if(0 == eLIBs_strnicmp(disk_name[i], cur_disk_name, 1))
			{
				return 1;
			}

			i++;
		}
	}

	return -1;
}

static __s32 __app_root_get_reg_storage_type2(rat_root_t rat_root_type)
{
	__s32 reg_root_type;
	reg_root_type = 0;

	if(RAT_TF == rat_root_type)
	{
		reg_root_type = 0;
	}
	else if(RAT_USB == rat_root_type)
	{
		reg_root_type = 1;
	}

	return reg_root_type;
}

static __s32  __app_root_get_search_path(rat_root_t rat_root_type, char *search_path)
{
	__s32 ret;
	char str_type[50];
	char disk_name[RAT_MAX_PARTITION][4];
	eLIBs_memset(str_type, 0, sizeof(str_type));
	eLIBs_memset(disk_name, 0, sizeof(disk_name));
	eLIBs_memset(search_path, 0, sizeof(search_path));

	switch(rat_root_type)
	{
		case RAT_LOCAL:
			eLIBs_strcpy(str_type, RAT_LOCAL_DISK);
			break;

		case RAT_TF:
			eLIBs_strcpy(str_type, RAT_SD_CARD);
			break;

		case RAT_USB:
			eLIBs_strcpy(str_type, RAT_USB_DISK);
			break;

		default:
			return EPDK_FAIL;
	}

	__wrn("str_type=%s\n", str_type);
	ret = rat_get_partition_name(str_type, disk_name);
	eLIBs_strcpy(search_path, disk_name[0]);
	__wrn("rat root type=%d\n", rat_root_type);
	__wrn("search_path=%s\n", search_path);

	if(0 == search_path[0])
	{
		return EPDK_FAIL;
	}

	return ret;
}

static __bool __app_root_has_external_disk(__u32 type)
{
	__s32 ret;
	char diskname[RAT_MAX_PARTITION][4];
	{
		__target_para_t target;
		esKSRV_GetTargetPara(&target);

		if(target.debugenable == EPDK_TRUE)
		{
			return EPDK_TRUE;  //112350 如果是调试，则默认为有外设
		}
	}

	if(type & APP_ROOT_TYPE_UD)
	{
		ret = rat_get_partition_name(RAT_USB_DISK, diskname);

		if(EPDK_OK == ret)
		{
			if(diskname[0][0])
			{
				return EPDK_TRUE;
			}
		}
	}

	if(type & APP_ROOT_TYPE_SD)
	{
		ret = rat_get_partition_name(RAT_SD_CARD, diskname);

		if(EPDK_OK == ret)
		{
			if(diskname[0][0])
			{
				return EPDK_TRUE;
			}
		}
	}

	return EPDK_FALSE;
}

static __s32 app_root_process_before_show_dlg(root_ctrl_t *root_ctrl)
{
	HBarState sta;

	if(!root_ctrl)
	{
		return EPDK_FAIL;
	}

	gscene_hbar_get_state(&sta);

	if(HBAR_ST_SHOW == sta)
	{
		root_ctrl->is_hbar_show_before_dlg = 1;
	}
	else
	{
		root_ctrl->is_hbar_show_before_dlg = 0;
	}

	gscene_hbar_set_state(HBAR_ST_HIDE);

	if(root_ctrl->h_app_movie)
	{
		app_movie_notify_delete_sub_scene(root_ctrl->h_app_movie);
	}

	if(root_ctrl->h_app_new_movie)
	{
		app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_new_movie);
	}
	
	if(root_ctrl->h_app_photo)
	{
		app_photo_notify_delete_sub_scene(root_ctrl->h_app_photo);
	}

	if(root_ctrl->h_app_ebook)
	{
		app_ebook_notify_delete_sub_scene(root_ctrl->h_app_ebook);
	}

	if(root_ctrl->h_app_new_ebook)
	{
		app_ebook_notify_delete_sub_scene(root_ctrl->h_app_new_ebook);
	}

	if(root_ctrl->h_app_fm)
	{
		app_fm_notify_delete_sub_scene(root_ctrl->h_app_fm);
	}

	if(root_ctrl->h_app_setting)
	{
		app_setting_notify_delete_sub_dlg_scene(root_ctrl->h_app_setting);
	}
	if(root_ctrl->h_app_new_setting)
	{
		app_setting_notify_delete_sub_dlg_scene(root_ctrl->h_app_new_setting);
	}
	if(root_ctrl->h_app_record)
	{
		app_record_notify_delete_sub_scene(root_ctrl->h_app_record);
	}

	return EPDK_OK;
}

static __s32 app_root_process_after_show_dlg(root_ctrl_t *root_ctrl)
{
	if(!root_ctrl)
	{
		return EPDK_FAIL;
	}

	if(1 == root_ctrl->is_hbar_show_before_dlg)
	{
		gscene_hbar_set_state(HBAR_ST_SHOW);
	}
	else
	{
		gscene_hbar_set_state(HBAR_ST_HIDE);
	}

	//重设前台程序为焦点
	if(root_ctrl->h_app_explorer)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
	}
	else if(root_ctrl->h_app_movie)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_movie);
	}
	else if(root_ctrl->h_app_new_movie)//新添加的movie应用app
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_movie);
	}
	else if(root_ctrl->h_app_photo)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_photo);
	}
	else if(root_ctrl->h_app_ebook)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_ebook);
	}
	else if(root_ctrl->h_app_new_ebook)//新添加的电子书应用app
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_ebook);
	}
	else if(root_ctrl->h_app_setting)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_setting);
	}
	else if(root_ctrl->h_app_new_setting)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_setting);
	}
	else if(root_ctrl->h_app_record)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_record);
	}
#if SP_APP_ATV
	else if(root_ctrl->h_app_atv)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_atv);
	}
#endif
#if SP_APP_AUX
	else if(root_ctrl->h_app_linin
	        && LINEIN_MODE_NORMAL == app_linein_get_mode(root_ctrl->h_app_linin))
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_linin);
	}
#endif	
	else if(root_ctrl->h_app_music
	        && MUSICPLAYER_NORMAL == app_music_get_mode(root_ctrl->h_app_music))
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_music);
	}
	else if(root_ctrl->h_app_new_music
    && MUSICPLAYER_NORMAL == app_music_get_mode(root_ctrl->h_app_new_music))//新添加音乐
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_new_music);
	}
	else if(root_ctrl->h_app_fm)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_fm);
	}
	else if(root_ctrl->h_app_calendar)
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_calendar);
	}
	else if(root_ctrl->h_app_home
	        && MULTI_SCREEN_HOME_MODE_NORMAL == app_multi_screen_home_get_mode(root_ctrl->h_app_home))
	{
		GUI_WinSetFocusChild(root_ctrl->h_app_home);
	}

	return EPDK_OK;
}

static __s32 app_root_show_dlg(H_WIN hwin, __s32 text_id, __s32 title_id, __s32 time)
{
	H_WIN dlg;
	__s32 lang_id[2] = {STRING_ROOT_CUE, STRING_ROOT_NULL_DISK};

	if(title_id)
	{
		lang_id[0] = title_id;
	}

	if(text_id)
	{
		lang_id[1] = text_id;
	}

	default_dialog(dlg, hwin, APP_COM_DLG_ID, ADLG_OK, lang_id);
	GUI_LyrWinCacheOff();

	if(0 != time)
	{
		esKRNL_TimeDly(time);
	}
	else
	{
		esKRNL_TimeDly(120);
	}

	app_dialog_destroy(dlg);
}

//显示错误信息
static __s32 app_root_show_err_info(root_ctrl_t *root_ctrl
                                    , __s32 init_media_type
                                    , __s32 init_storage_type
                                    , app_root_prob_intent_t intent
                                    , app_root_prob_result_t err_code, __bool bHideHBar)
{
	__s32 text_id;
	reg_root_para_t *para;
	//参数检查
	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(!para)
	{
		__wrn("para is null...\n");
		goto fail;
	}

	if(
	  (init_media_type < 0
	   && init_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage)
	  )
	)
	{
		__wrn("media type error...\n");
		goto fail;
	}

	if(
	  (init_storage_type < 0
	   && init_storage_type >= BEETLES_TBL_SIZE(para->last_storage_play_app)
	  )
	)
	{
		__wrn("storage type is null...\n");
		goto fail;
	}

	if(intent < 0 || intent >= app_root_prob_intent_max)
	{
		__wrn("intent type error...\n");
		goto fail;
	}

	text_id = 0;

	switch(intent)
	{
		case app_root_prob_intent_first_play:
		{
			if(app_root_prob_storage_plugined == err_code)
			{
				if(0 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_SD_VIDEO;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_SD_AUDIO;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_SD_EBOOK;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_SD_PHOTO;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else if(1 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_USB_VIDEO;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_USB_AUDIO;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_USB_EBOOK;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_USB_PHOTO;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}

			break;
		}

		case app_root_prob_intent_plugin_for_play:
		{
			if(0)//app_root_prob_storage_plugined == err_code)
			{
				if(0 == init_storage_type)
				{
					text_id = STRING_ROOT_SD_PLUGIN;
				}
				else if(1 == init_storage_type)
				{
					text_id = STRING_ROOT_UD_PLUGIN;
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}
			else if(app_root_prob_no_meida_file == err_code)
			{
				if(0 == init_storage_type)
				{
					text_id = STRING_ROOT_NO_FILE_IN_SD;
				}
				else if(1 == init_storage_type)
				{
					text_id = STRING_ROOT_NO_FILE_IN_USB;
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}
			else if(app_root_prob_storage_plugined == err_code)
			{
				if(0 == init_storage_type)
				{
					text_id = STRING_ROOT_SD_PLUGIN;
				}
				else if(1 == init_storage_type)
				{
					text_id = STRING_ROOT_UD_PLUGIN;
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}

			break;
		}

		case app_root_prob_intent_switch_media:
		{
			if(app_root_prob_no_correspond_storage == err_code)
			{
				text_id = STRING_ROOT_NO_ENY_STORAGE;
			}
			else if(app_root_prob_no_meida_file == err_code)
			{
				switch(init_media_type)
				{
					case m_eMediaTypeList_video :
						text_id = STRING_ROOT_NO_ENY_VIDEO_FILE;
						break;

					case m_eMediaTypeList_audio:
						text_id = STRING_ROOT_NO_ENY_AUDIO_FILE;
						break;

					case m_eMediaTypeList_ebook:
						text_id = STRING_ROOT_NO_ENY_EBOOK_FILE;
						break;

					case m_eMediaTypeList_photo:
						text_id = STRING_ROOT_NO_ENY_PHOTO_FILE;
						break;

					default :
						__wrn("init_media_type type error...\n");
						break;
				}
			}
			else if(app_root_prob_media_switch == err_code)
			{
				if(0 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_SWITCH_TO_SD_VIDEO;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_SWITCH_TO_SD_AUDIO;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_SWITCH_TO_SD_EBOOK;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_SWITCH_TO_SD_PHOTO;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else if(1 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_SWITCH_TO_USB_VIDEO;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_SWITCH_TO_USB_AUDIO;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_SWITCH_TO_USB_EBOOK;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_SWITCH_TO_USB_PHOTO;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}

			break;
		}

		case app_root_prob_intent_switch_storage:
		{
			if(app_root_prob_no_correspond_storage == err_code)
			{
				if(0 == init_storage_type)
				{
					text_id = STRING_ROOT_NO_SD;
				}
				else if(1 == init_storage_type)
				{
					text_id = STRING_ROOT_NO_USB;
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}
			else if(app_root_prob_no_meida_file == err_code)
			{
				if(0 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_NO_VIDEO_FILE_IN_SD;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_NO_AUDIO_FILE_IN_SD;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_NO_EBOOK_FILE_IN_SD;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_NO_PHOTO_FILE_IN_SD;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else if(1 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_NO_VIDEO_FILE_IN_USB;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_NO_AUDIO_FILE_IN_USB;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_NO_EBOOK_FILE_IN_USB;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_NO_PHOTO_FILE_IN_USB;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}
			else if(app_root_prob_storage_switch == err_code)
			{
				if(0 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_SWITCH_TO_SD_VIDEO;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_SWITCH_TO_SD_AUDIO;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_SWITCH_TO_SD_EBOOK;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_SWITCH_TO_SD_PHOTO;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else if(1 == init_storage_type)
				{
					switch(init_media_type)
					{
						case m_eMediaTypeList_video :
							text_id = STRING_ROOT_SWITCH_TO_USB_VIDEO;
							break;

						case m_eMediaTypeList_audio:
							text_id = STRING_ROOT_SWITCH_TO_USB_AUDIO;
							break;

						case m_eMediaTypeList_ebook:
							text_id = STRING_ROOT_SWITCH_TO_USB_EBOOK;
							break;

						case m_eMediaTypeList_photo:
							text_id = STRING_ROOT_SWITCH_TO_USB_PHOTO;
							break;

						default :
							__wrn("init_media_type type error...\n");
							break;
					}
				}
				else
				{
					__wrn("init_storage_type type error...\n");
				}
			}

			break;
		}

		case app_root_prob_intent_unknown:
		{
			if(app_root_prob_no_correspond_storage == err_code)
			{
				text_id = STRING_ROOT_NO_ENY_STORAGE;
			}

			break;
		}

		default:
		{
			__wrn("intent type error...\n");
			goto fail;
			break;
		}
	}

	__wrn("text_id=%d\n", text_id);

	if(text_id)
	{
		HBarState sta;
		app_root_process_before_show_dlg(root_ctrl);//弹框之前先删除多余的子场景
		__app_root_change_to_default_bg();
		//gscene_bgd_set_flag(EPDK_TRUE);
		//gscene_bgd_refresh();

		if(bHideHBar)
		{
			gscene_hbar_get_state(&sta);
			gscene_hbar_set_state(HBAR_ST_HIDE);
		}

		app_root_show_dlg(root_ctrl->root_para->h_parent
		                  , text_id, 0, 0);

		if(bHideHBar)
		{
			gscene_hbar_set_state(sta);
		}

		app_root_process_after_show_dlg(root_ctrl);//弹框之重设焦点
	}

	__wrn("app_root_show_err_info ok...\n");
	return EPDK_OK;
fail:
	;
	return EPDK_FAIL;
}
//储存设备拔出
static __s32 __app_root_play_other_storages_on_plugout(root_ctrl_t *root_ctrl)
{
	__s32 ret;
	__s32 reg_media_type;
	__s32 reg_storage_type;
	reg_root_para_t *para;
	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(!para)
	{
		__wrn("para is null...\n");
		return EPDK_FAIL;
	}

	if(NULL == root_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	reg_media_type = -1;
	reg_storage_type = para->cur_play_storage;

	if(root_ctrl->h_app_ebook)
	{
		reg_media_type = m_eMediaTypeList_ebook;
	}
	if(root_ctrl->h_app_new_ebook)
	{
		reg_media_type = m_eMediaTypeList_ebook;
	}
	else if(root_ctrl->h_app_photo)
	{
		reg_media_type = m_eMediaTypeList_photo;
	}
	else if(root_ctrl->h_app_movie)
	{
		reg_media_type = m_eMediaTypeList_video;
	}
	else if(root_ctrl->h_app_new_movie)//新添加的movie应用app
	{
		reg_media_type = m_eMediaTypeList_video;
	}
	else if(root_ctrl->h_app_music)
	{
		reg_media_type = m_eMediaTypeList_audio;
	}
	else if(root_ctrl->h_app_new_music)
	{
		reg_media_type = m_eMediaTypeList_audio;
	}
	else
	{
		reg_media_type = -1;
		__wrn("no media playing...\n");
		return EPDK_FAIL;
	}

	root_ctrl->switchMediaEnable_flag = 1;
	ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_switch_storage, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_FALSE);
	root_ctrl->switchMediaEnable_flag = 0;
	return ret;
}

//媒体探测
//init_media_type:用于存放探测得到的媒体类型0:音乐 1:视频
//init_storage_type:用于存放探测得到的媒体介质类型0:卡 1:u盘
//intent:探测方式
//返回:EPDK_OK，探测成功，EPDK_FAIL，探测失败，*err_code存放错误代号
static __s32 app_root_prob_media_file_for_play(root_ctrl_t *root_ctrl
    , __s32 *init_media_type
    , __s32 *init_storage_type
    , app_root_prob_intent_t intent
    , app_root_prob_result_t *err_code)
{
	__bool bhas_disk;
	__s32 reg_storage_type;
	__s32 reg_media_type;
	reg_root_para_t *para;
	rat_media_type_t rat_media_type;
	__u8 storageShiftCnt = 0;
	__wrn("***********app_root_prob_media_file_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d***********\n",
	      intent, *init_media_type, *init_storage_type, *err_code);
	//参数检查
	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(!para)
	{
		__wrn("para is null...\n");
		goto fail;
	}

	if(!init_media_type ||
	    (*init_media_type < 0
	     && *init_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage)
	    )
	  )
	{
		__wrn("media type error...\n");
		goto fail;
	}

	if(!init_storage_type)
	{
		__wrn("storage type is null...\n");
		goto fail;
	}

	if(intent < 0 || intent >= app_root_prob_intent_max)
	{
		__wrn("intent type error...\n");
		goto fail;
	}

	reg_storage_type = *init_storage_type;
	reg_media_type = *init_media_type;
	bhas_disk = EPDK_FALSE;

	do
	{
		__s32 ret;
		rat_root_t rat_root_type;
		HRAT hrat;
		__u32 media_cnt;
		__s32 cur_reg_root_type;
		char path[RAT_MAX_FULL_PATH_LEN];
		__wrn("begin search media file, reg_storage_type=%d, reg_media_type=%d\n"
		      , reg_storage_type, reg_media_type);
		rat_root_type = __app_root_get_rat_root_type(reg_storage_type);

		if(RAT_UNKNOWN == rat_root_type)
		{
			__wrn("rat_root_type error...\n");
			goto fail;
		}

		rat_media_type = __app_root_get_rat_media_type(reg_media_type);

		if(RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
		{
			__wrn("rat_media_type error...\n");
			goto fail;
		}

		eLIBs_memset(path, 0, sizeof(path));
		ret = __app_root_get_search_path(rat_root_type, path);

		if(EPDK_FAIL == ret)//无磁盘
		{
			__wrn("no such disk, reg_storage_type=%d\n", reg_storage_type);

			if(app_root_prob_intent_first_play == intent)
			{
				__wrn("intent is first_play, not need switch storage, intent=%d...\n", intent);
				goto next_storage;	//no_storage;
			}
			else if(app_root_prob_intent_plugin_for_play == intent)
			{
				__wrn("may not run here...\n");
				goto no_media_file;
			}
			else if(app_root_prob_intent_switch_media == intent)
			{
				__wrn("intent is switch_media, goto next_storage\n");
				goto next_storage;
			}
			else if(app_root_prob_intent_switch_storage == intent)
			{
				__wrn("intent is switch_storage, goto next_storage\n");
				goto next_storage;
			}
			else
			{
				__wrn("intent type error...\n");
				goto fail;
			}
		}

		bhas_disk = EPDK_TRUE;
		media_cnt = 0;
		//hrat = rat_open(path, rat_media_type, 0);
		hrat = rat_open_ex(rat_root_type, rat_media_type, 0);

		if(hrat)
		{
			media_cnt = rat_get_cur_scan_cnt(hrat);
		}

		if(NULL == hrat || 0 == media_cnt)
		{
			__wrn("no such media file, reg_media_type=%d\n"
			      , reg_media_type);

			if(app_root_prob_intent_first_play == intent)
			{
				__wrn("intent is first_play, not need switch storage, intent=%d...\n", intent);
				goto next_media_type;	//no_media_file;
			}
			else if(app_root_prob_intent_plugin_for_play == intent)
			{
				__wrn("intent is plugin_for_play, goto no_media_file...\n");
				goto next_media_type;
			}
			else if(app_root_prob_intent_switch_media == intent)
			{
				__wrn("intent is switch_media, goto next_storage\n");
				goto next_storage;
			}
			else if(app_root_prob_intent_switch_storage == intent)
			{
				__wrn("intent is switch_storage, goto next_storage\n");

				if(root_ctrl->switchMediaEnable_flag == 1)
				{
					goto next_media_type;
				}
				else
				{
					goto next_storage;
				}
			}
			else
			{
				__wrn("intent type error...\n");
				goto fail;
			}
		}

		//探测成功
		__wrn("search media file success, media_cnt=%d\n", media_cnt);
		break;
	next_storage:
		;
		__wrn("app_root_prob_media_file_for_play: next_storage\n");
		reg_storage_type++;

		if(reg_storage_type >= BEETLES_TBL_SIZE(para->last_storage_play_app))
		{
			reg_storage_type = 0;
		}

		//计算当前播放盘符，盘符切换不会切换到当前盘符
		cur_reg_root_type = para->cur_play_storage;
		__wrn("reg_storage_type=%d\n", reg_storage_type);
		__wrn("cur_reg_root_type=%d\n", cur_reg_root_type);
		__wrn("init_storage_type=%d\n", *init_storage_type);
		/*		if(reg_storage_type == *init_storage_type||
				       (cur_reg_root_type == *init_storage_type
				         &&app_root_prob_intent_switch_storage == intent)
				  )*/
		{
			if(++storageShiftCnt >= BEETLES_TBL_SIZE(para->last_storage_play_app))
			{
				if(EPDK_FALSE == bhas_disk)
				{
					__wrn("search finish, not any reg root type\n");
					goto no_storage;
				}
				else
				{
					__wrn("search finish, no more reg root type\n");
					goto no_media_file;
				}
			}
		}
		__wrn("try to switch to reg root type:%d\n", reg_storage_type);
		continue;
	next_media_type:
		;
		__wrn("app_root_prob_media_file_for_play: next_media_type\n");
		reg_media_type++;

		if(reg_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage))
		{
			reg_media_type = 0;
		}

		__wrn("reg_media_type=%d\n", reg_media_type);

		if(reg_media_type == *init_media_type)
		{
			if(EPDK_FALSE == bhas_disk)
			{
				__wrn("EPDK_FALSE == bhas_disk, may not run here...\n");
				goto no_storage;//一般不会跑到这里
			}
			else
			{
				__wrn("no ");
				goto no_media_file;
			}
		}

		__wrn("try to search new media type, reg_media_type=%d\n", reg_media_type);
		continue;
	}
	while(1);

	__wrn("***********prob media file success********,*init_media_type=%d, *init_storage_type=%d\n"
	      , *init_media_type, *init_storage_type);
ok:
	;

	if(err_code)
	{
		if(init_media_type)
		{
			*init_media_type = reg_media_type;
		}

		if(init_storage_type)
		{
			*init_storage_type = reg_storage_type;
		}

		if(app_root_prob_intent_switch_media == intent)
		{
			*err_code = app_root_prob_media_switch;
		}
		else if(app_root_prob_intent_switch_storage == intent)
		{
			*err_code = app_root_prob_storage_switch;
		}
		else if(app_root_prob_intent_plugin_for_play == intent)
		{
			*err_code = app_root_prob_storage_plugined;
		}
		else if(app_root_prob_intent_first_play == intent)
		{
			*err_code = app_root_prob_storage_plugined;
		}
		else
		{
			__wrn("intent type error...\n");
			*err_code = app_root_prob_ok;
		}
	}

	return EPDK_OK;
no_storage:
	;

	if(err_code)
	{
		*err_code = app_root_prob_no_correspond_storage;
	}

	return EPDK_FAIL;
no_media_file:
	;

	if(err_code)
	{
		*err_code = app_root_prob_no_meida_file;
	}

	return EPDK_FAIL;
fail:
	;

	if(err_code)
	{
		*err_code = app_root_prob_fail;
	}

	return EPDK_FAIL;
}


//盘符探测
//init_media_type:用于存放探测得到的媒体类型
//init_storage_type:用于存放探测得到的媒体介质类型
//intent:探测方式
//返回:EPDK_OK，探测成功，EPDK_FAIL，探测失败，*err_code存放错误代号
static __s32 app_root_prob_storage_type_for_play(root_ctrl_t *root_ctrl
    , __s32 *init_media_type
    , __s32 *init_storage_type
    , app_root_prob_intent_t intent
    , app_root_prob_result_t *err_code)
{
	reg_root_para_t *para;
	__msg("***********app_root_prob_storage_type_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d***********\n",
	      intent, *init_media_type, *init_storage_type);
	//参数检查
	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(!para)
	{
		__wrn("para is null...\n");
		goto fail;
	}

	if(!init_media_type ||
	    (*init_media_type < 0
	     && *init_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage)
	    )
	  )
	{
		if(app_root_prob_intent_first_play != intent
		    && app_root_prob_intent_plugin_for_play != intent)
		{
			__msg("media type error...\n");
			goto fail;
		}
	}

	if(!init_storage_type)
	{
		if(app_root_prob_intent_first_play != intent)
		{
			__wrn("storage type is null...\n");
			goto fail;
		}
	}

	if(intent < 0 || intent >= app_root_prob_intent_max)
	{
		__wrn("intent type error...\n");
		goto fail;
	}

	if(app_root_prob_intent_first_play == intent)
	{
		if(para->last_play_app < 0
		    || para->last_play_app >= BEETLES_TBL_SIZE(para->last_app_play_storage))
		{
			para->last_play_app = 0;
		}

		if(init_media_type)
		{
			*init_media_type = para->last_play_app;
		}

		if(init_storage_type)
		{
			*init_storage_type = para->last_app_play_storage[para->last_play_app];
		}

		__wrn("app_root_prob_intent_first_play, *init_media_type=%d, *init_storage_type=%d\n"
		      , *init_media_type, *init_storage_type);
	}
	else if(app_root_prob_intent_switch_media == intent)
	{
		__wrn("app_root_prob_intent_switch_media\n");

		switch(*init_media_type)
		{
			case m_eMediaTypeList_audio ://音频
			{
				//music已经运行
				if(root_ctrl->h_app_music)
				{
					__wrn("music alreadly running\n");
					goto alreadly_running;
				}
				else if(root_ctrl->h_app_new_music)
				{
					__wrn("new music alreadly running\n");
					goto alreadly_running;
				}

				__wrn("music not running\n");
				goto from_nearest_storage;
				break;
			}

			case m_eMediaTypeList_video ://视频
			{
				if(root_ctrl->h_app_movie)
				{
					__wrn("movie alreadly running\n");
					goto alreadly_running;
				}
				else if(root_ctrl->h_app_new_movie)
				{
					__wrn("new movie alreadly running\n");
					goto alreadly_running;
				}

				__wrn("movie or new_movie not running\n");
				goto from_nearest_storage;
				break;
			}

			case m_eMediaTypeList_ebook ://电子书
			{
				if(root_ctrl->h_app_ebook)
				{
					__wrn("ebook alreadly running\n");
					goto alreadly_running;
				}

				if(root_ctrl->h_app_new_ebook)
				{
					__wrn("new ebook alreadly running\n");
					goto alreadly_running;
				}

				__wrn("ebook not running\n");
				goto from_nearest_storage;
				break;
			}

			case m_eMediaTypeList_photo ://图片
			{
				if(root_ctrl->h_app_photo)
				{
					__wrn("photo alreadly running\n");
					goto alreadly_running;
				}

				__wrn("photo not running\n");
				goto from_nearest_storage;
				break;
			}

			default:
			{
				__wrn("media type error...\n");
				goto fail;
				break;
			}
		}
	}
	else if(app_root_prob_intent_switch_storage == intent)
	{
		__wrn("app_root_prob_intent_switch_storage\n");

		switch(*init_media_type)
		{
			case m_eMediaTypeList_audio ://音频
			{
				//music尚未运行
				if(!root_ctrl->h_app_music)
				{
					__wrn("music not running\n");
					goto not_running;
				}
				else if(!root_ctrl->h_app_new_music)
				{
					__wrn("new music not running\n");
					goto not_running;
				}

				__wrn("music is running\n");
				goto from_next_storage;
				break;
			}

			case m_eMediaTypeList_video ://视频
			{
				//视频尚未运行
				if(!root_ctrl->h_app_movie)
				{
					__wrn("movie not running\n");
					goto not_running;
				}
				else if(!root_ctrl->h_app_new_movie)
				{
					__wrn("new movie not running\n");
					goto not_running;
				}

				__wrn("movie is running\n");
				goto from_next_storage;
				break;
			}

			case m_eMediaTypeList_ebook ://电子书
			{
				//电子书尚未运行
				if(!root_ctrl->h_app_ebook)
				{
					__wrn("ebook not running\n");
					goto not_running;
				}
				
				if(!root_ctrl->h_app_new_ebook)
				{
					__wrn("new ebook not running\n");
					goto not_running;
				}

				__wrn("ebook is running\n");
				goto from_next_storage;
				break;
			}

			case m_eMediaTypeList_photo ://图片
			{
				//图片尚未运行
				if(!root_ctrl->h_app_photo)
				{
					__wrn("photo not running\n");
					goto not_running;
				}

				__wrn("photo is running\n");
				goto from_next_storage;
				break;
			}

			default:
			{
				__wrn("media type error...\n");
				goto fail;
				break;
			}
		}
	}
	else if(app_root_prob_intent_plugin_for_play == intent)
	{
		__wrn("app_root_prob_intent_plugin_for_play\n");

		if(!init_storage_type)
		{
			__wrn("init_storage_type is null...\n");
			goto fail;
		}

		if(*init_storage_type < 0 || *init_storage_type >= BEETLES_TBL_SIZE(para->last_storage_play_app))
		{
			__wrn("storage type invalid...\n");
			goto fail;
		}

		if(init_media_type)
		{
			*init_media_type = para->last_storage_play_app[*init_storage_type];
		}

		__wrn("get last storage play media, reg media type=%d\n", *init_media_type);
		goto storage_plugin;
	}
	else
	{
		__wrn("intent type error...\n");
		goto fail;
	}

	__wrn("prob storage type success,*init_media_type=%d, *init_storage_type=%d\n"
	      , *init_media_type, *init_storage_type);
	goto ok;
from_next_storage:
	;
	__wrn("app_root_prob_storage_type_for_play: from_next_storage\n");

	if(root_ctrl->switchMediaEnable_flag == 0)
	{
		//切换盘符
		__s32 ret;
		HRAT hrat;
		char path[RAT_MAX_FULL_PATH_LEN];
		rat_root_t rat_root_type;
		__s32 to_root_type;
		rat_media_type_t rat_media_type;
		__u32 media_cnt;

		if(-1 == para->cur_play_storage
		    || para->cur_play_storage >= BEETLES_TBL_SIZE(para->last_app_play_storage))
		{
			__wrn("para->cur_play_storage error, change to 0...\n");
			para->cur_play_storage = 0;
		}

		//获取当前播放盘符
		to_root_type = para->cur_play_storage;
		__wrn("current reg disk type is:%d\n", para->cur_play_storage);
		//执行盘符切换
		to_root_type++;

		if(to_root_type >= BEETLES_TBL_SIZE(para->last_storage_play_app))
		{
			to_root_type = 0;
		}

		if(to_root_type == para->cur_play_storage)
		{
			__wrn("there is only one disk currently, reg root type=%d\n", to_root_type);
			goto no_storage;
		}

		if(init_storage_type)
		{
			*init_storage_type = to_root_type;
		}

		rat_root_type = __app_root_get_rat_root_type(to_root_type);

		if(RAT_UNKNOWN == rat_root_type)
		{
			__wrn("rat_root_type type error...\n");
			goto no_storage;
		}

		rat_media_type = __app_root_get_rat_media_type(*init_media_type);

		if(RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
		{
			__wrn("rat_media_type type error...\n");
			goto no_media_file;
		}

		__wrn("try switch to reg root type:%d\n", to_root_type);
		eLIBs_memset(path, 0, sizeof(path));
		ret = __app_root_get_search_path(rat_root_type, path);

		if(EPDK_FAIL == ret)//无磁盘
		{
			__wrn("no such reg root type:%d\n", to_root_type);
			goto no_storage;
		}

		{
			//hrat = rat_open(path, rat_media_type, 0);
			hrat = rat_open_ex(rat_root_type, rat_media_type, 0);

			if(NULL == hrat)
			{
				__wrn("no corresponden media file, reg root type=%d\n", to_root_type);
				goto no_media_file;
			}

			media_cnt = rat_get_cur_scan_cnt(hrat);

			if(0 == media_cnt)
			{
				__wrn("no corresponden media file, reg root type=%d\n", to_root_type);
				goto no_media_file;
			}

			if(init_storage_type)
			{
				*init_storage_type = to_root_type;
			}

			__wrn("success to switch reg root to %d, reg media type=%d, media cnt=%d\n"
			      , to_root_type, *init_media_type, media_cnt);
		}

		goto ok;
	}
	else
	{
		goto ok;
	}

from_nearest_storage:
	;
	__wrn("app_root_prob_storage_type_for_play: from_nearest_storage\n");
	{
		//获取当前播放盘
		__wrn("try to get current playing root type\n");

		if(-1 != para->cur_play_storage)//如果当前有盘符正在播放
		{
			if(init_storage_type)
			{
				*init_storage_type = para->cur_play_storage;
			}

			__wrn("current playing reg root type is:%d\n", para->cur_play_storage);
			goto ok;
		}
		else//获取最近播放该ap的盘
		{
			__wrn("current playing root type is empty...\n");

			if(init_storage_type)
			{
				*init_storage_type = para->last_app_play_storage[*init_media_type];
			}

			__wrn("get last disk which play this media,reg media type=%d, reg root type=%d",
			      *init_media_type, para->last_app_play_storage[*init_media_type]);
			goto ok;
		}
	}
ok:
	;
	__wrn("app_root_prob_storage_type_for_play: ok\n");

	if(err_code)
	{
		*err_code = app_root_prob_ok;
	}

	return EPDK_OK;
storage_plugin:
	;
	__wrn("app_root_prob_storage_type_for_play: storage_plugin\n");

	if(err_code)
	{
		*err_code = app_root_prob_ok;
	}

	return EPDK_OK;
alreadly_running:
	;
	__wrn("app_root_prob_storage_type_for_play: alreadly_running\n");

	if(init_storage_type)
	{
		*init_storage_type = para->cur_play_storage;
	}

	if(err_code)
	{
		*err_code = app_root_prob_already_running;
	}

	return EPDK_FAIL;
not_running:
	;
	__wrn("app_root_prob_storage_type_for_play: not_running\n");

	if(err_code)
	{
		*err_code = app_root_prob_not_running;
	}

	return EPDK_FAIL;
no_storage:
	;
	__wrn("app_root_prob_storage_type_for_play: no_storage\n");

	if(err_code)
	{
		*err_code = app_root_prob_no_correspond_storage;
	}

	return EPDK_FAIL;
no_media_file:
	;
	__wrn("app_root_prob_storage_type_for_play: no_media_file\n");

	if(err_code)
	{
		*err_code = app_root_prob_no_meida_file;
	}

	return EPDK_FAIL;
fail:
	;
	__msg("app_root_prob_storage_type_for_play: fail\n");

	if(err_code)
	{
		*err_code = app_root_prob_fail;
	}

	return EPDK_FAIL;
}

static __s32 app_root_shortcut_process(root_ctrl_t *root_ctrl, app_root_prob_intent_t intent, __s32 reg_media_type, __s32 reg_storage_type, __bool bHideHBar, __bool bShowDlgFlag)
{
	__s32 reg_storage_type_bk = reg_storage_type, reg_media_type_bk = reg_media_type;
	char wav_file[1024];
	__msg("***********app_root_shortcut_process, intent=%d, reg_media_type=%d, reg_storage_type=%d***********\n",
	      intent, reg_media_type, reg_storage_type);

	//检查处理类型
	if(intent >= app_root_prob_intent_max)
	{
		__wrn("intent type error...\n");
		goto err;
	}

	{
		//如果类型正确，则进行盘符探测
		__s32 ret;
		app_root_prob_result_t err_code;
		ret = app_root_prob_storage_type_for_play(root_ctrl
		      , &reg_media_type
		      , &reg_storage_type
		      , intent
		      , &err_code);
		__msg("***********after app_root_prob_storage_type_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d, ret=%d***********\n",
		      intent, reg_media_type, reg_storage_type, err_code, ret);

		if(1)
		{
			//盘符探测后的预处理
			if(app_root_prob_already_running == err_code)
			{
				if(m_eMediaTypeList_audio == reg_media_type)//如果是后台音乐，则直接恢复播放
				{
					if(root_ctrl->h_app_music)
					{
						musicplayer_mode_e mode;
						mode = app_music_get_mode(root_ctrl->h_app_music);

						if(-1 == reg_storage_type)//如果第一次探测盘符失败，则获取当前播放盘符
						{
							__s32 ret;
							__u32 rat_root_type;
							ret = app_music_get_root_type(root_ctrl->h_app_music, &rat_root_type);

							if(EPDK_OK == ret)
							{
								reg_storage_type = __app_root_get_reg_storage_type2(rat_root_type);
							}
						}

						if(MUSICPLAYER_BACKGROUND == mode || MUSICPLAYER_BACKGROUND_HOME == mode)
						{
							__wrn("background music is running, reseum it directly...\n");
							goto set_play_file;
						}
					}
					else if(root_ctrl->h_app_new_music)//新添加的音乐
					{
						musicplayer_mode_e mode;
						mode = app_music_get_mode(root_ctrl->h_app_new_music);

						if(-1 == reg_storage_type)//如果第一次探测盘符失败，则获取当前播放盘符
						{
							__s32 ret;
							__u32 rat_root_type;
							ret = app_music_get_root_type(root_ctrl->h_app_new_music, &rat_root_type);

							if(EPDK_OK == ret)
							{
								reg_storage_type = __app_root_get_reg_storage_type2(rat_root_type);
							}
						}

						if(MUSICPLAYER_BACKGROUND == mode || MUSICPLAYER_BACKGROUND_HOME == mode)
						{
							__wrn("background new music is running, reseum it directly...\n");
							goto set_play_file;
						}
					}
				}
			}
		}

		if(bShowDlgFlag)
		{
			app_root_show_err_info(root_ctrl
			                       , reg_media_type
			                       , reg_storage_type
			                       , intent
			                       , err_code
			                       , bHideHBar);
		}

		if(EPDK_OK != ret)
		{
			goto err;
		}

		//探测成功，继续进行媒体探测
		ret = app_root_prob_media_file_for_play(root_ctrl
		                                        , &reg_media_type
		                                        , &reg_storage_type
		                                        , intent
		                                        , &err_code);
		__wrn("***********after app_root_prob_media_file_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d, ret=%d***********\n",
		      intent, reg_media_type, reg_storage_type, err_code, ret);

		if(bShowDlgFlag)
		{
			app_root_show_err_info(root_ctrl
			                       , reg_media_type
			                       , reg_storage_type
			                       , intent
			                       , err_code
			                       , bHideHBar);
		}

		if(EPDK_OK != ret)
		{
			goto err;
		}

	set_play_file:
		;
		if(app_root_prob_intent_switch_storage == intent)
		{
			if((reg_storage_type == reg_storage_type_bk) && (reg_media_type == reg_media_type_bk))
			{
				return EPDK_FAIL;
			}
		}
		
		{
			//探测成功，设置播放列表
			__s32 ret;
			rat_root_t rat_root_type;
			rat_media_type_t rat_media_type;
			rat_media_type = __app_root_get_rat_media_type(reg_media_type);
			rat_root_type = __app_root_get_rat_root_type(reg_storage_type);

			if(RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
			{
				__wrn("media type error...\n");
				goto err;
			}

			if(RAT_UNKNOWN == rat_root_type)
			{
				__wrn("rat root type error...\n");
				goto err;
			}

			ret = __app_root_set_playfile(rat_root_type, rat_media_type);

			if(EPDK_FAIL == ret)
			{
				__wrn("set file for play fail...\n");
				goto err;
			}

			__wrn("set file for play success...\n");
		}
	begin_play:
		;
		//探测成功，则开始播放
		{
			rat_root_t rat_root_type;
			rat_root_type = __app_root_get_rat_root_type(reg_storage_type);

			if(RAT_UNKNOWN == rat_root_type)
			{
				__wrn("storage type unknown...\n");
				goto err;
			}

			switch(reg_media_type)
			{
				case m_eMediaTypeList_video :
					//__app_root_to_video(root_ctrl, rat_root_type);//自动进入播放视频
					break;

				case m_eMediaTypeList_audio:
					__app_root_to_audio(root_ctrl, rat_root_type);
					break;

				case m_eMediaTypeList_ebook:
					__app_root_to_ebook(root_ctrl, rat_root_type);
					break;

				case m_eMediaTypeList_photo:
					__app_root_to_photo(root_ctrl, rat_root_type);
					break;

				default :
					__wrn("reg_media_type type unknown...\n");
					goto err;
					break;
			}
		}
	}

	__msg("***********app_root_shortcut_process success***********\n");
	return EPDK_OK;
err:
	;
	__msg("***********app_root_shortcut_process fail***********\n");
	return EPDK_FAIL;
}

static __s32 app_root_check_volume_key(__gui_msg_t *msg)
{
	__s32 bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
	static __s32 longdown_cnt = 0;
	static __s32 last_key = -1;
	root_ctrl_t   *root_ctrl;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!root_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	if(root_ctrl->h_app_setting)
	{
		return EPDK_FAIL;
	}
	if(root_ctrl->h_app_new_setting)
	{
		return EPDK_FAIL;
	}
	if(KEY_DOWN_ACTION == msg->dwAddData2 || KEY_REPEAT_ACTION == msg->dwAddData2)
	{
		switch(msg->dwAddData1)
		{
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)

			case GUI_MSG_KEY_ECHOUP :
			case GUI_MSG_KEY_LONGECHOUP :
				root_ctrl->m_eWinVolume = m_eWinVolume_echo;
#endif

			case GUI_MSG_KEY_VADD :
			case GUI_MSG_KEY_LONGVADD :
			{
				if(root_ctrl->h_app_home 
				    &&(!(root_ctrl->h_app_explorer)) 
				    &&(!(root_ctrl->h_app_fm)) 
				    &&(!(root_ctrl->h_app_movie)) 
				    //	&&(!(root_ctrl->h_app_music)) 
				    &&(!(root_ctrl->h_app_photo)) 
				    &&(!(root_ctrl->h_app_ebook)) 
				    &&(!(root_ctrl->h_app_setting))
				    &&(!(root_ctrl->h_app_new_setting))
				    &&(!(root_ctrl->h_app_record))
				    //	&&(!(root_ctrl->h_app_linin)) &&
				    &&(!(root_ctrl->h_app_new_movie)) //新添加的movie应用app
				    &&(!(root_ctrl->h_app_new_music))//新添加音乐
				    &&(!(root_ctrl->h_app_new_ebook)) 
				#if SP_APP_ATV
				    &&(!(root_ctrl->h_app_atv))
				#endif
				  )
				{
					multi_screen_home_mode_e mode;
					mode = app_multi_screen_home_get_mode(root_ctrl->h_app_home);

					if(MULTI_SCREEN_HOME_MODE_NORMAL == mode)
					{
						__wrn("MULTI_SCREEN_HOME_MODE_NORMAL == mode\n");
						goto end;
					}
				}

				if(root_ctrl->h_app_movie)
				{
					app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_movie);
				}

				if(root_ctrl->h_app_new_movie)//新添加的movie应用app
				{
					app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_new_movie);
				}

			#if 0	//SP_APP_ATV
				if(g_isTconOccupyLcmi_flag)
				{
					if(root_ctrl->m_eWinVolume == m_eWinVolume_default)
					{
						dsk_volume_inc();
					}
				}
				else if(!(root_ctrl->h_app_atv))
			#endif
				{
					__app_root_volume_scene_create(root_ctrl);
					__com_volume_scene_set_focus(root_ctrl);
				}
			}
			break;
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)

			case GUI_MSG_KEY_ECHODOWN:
			case GUI_MSG_KEY_LONGECHODOWN:
				root_ctrl->m_eWinVolume = m_eWinVolume_echo;
#endif

			case GUI_MSG_KEY_VDEC:
			case GUI_MSG_KEY_LONGVDEC:
			{
				if(root_ctrl->h_app_home 
				    &&(!(root_ctrl->h_app_explorer)) 
				    &&(!(root_ctrl->h_app_fm)) 
				    &&(!(root_ctrl->h_app_movie)) 
				    //	&&(!(root_ctrl->h_app_music)) 
				    &&(!(root_ctrl->h_app_photo)) 
				    &&(!(root_ctrl->h_app_ebook)) 
				    &&(!(root_ctrl->h_app_setting)) 
				    &&(!(root_ctrl->h_app_new_setting))
				    &&(!(root_ctrl->h_app_record))
				    //	&&(!(root_ctrl->h_app_linin)) &&
				    &&(!(root_ctrl->h_app_new_movie)) //新添加的movie应用app
				    &&(!(root_ctrl->h_app_new_music))//新添加音乐
				    &&(!(root_ctrl->h_app_new_ebook)) 
				#if SP_APP_ATV
				    &&(!(root_ctrl->h_app_atv))
				#endif
				  )
				{
					multi_screen_home_mode_e mode;
					mode = app_multi_screen_home_get_mode(root_ctrl->h_app_home);

					if(MULTI_SCREEN_HOME_MODE_NORMAL == mode)
					{
						__wrn("MULTI_SCREEN_HOME_MODE_NORMAL == mode\n");
						goto end;
					}
				}

				if(root_ctrl->h_app_movie)
				{
					app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_movie);
				}

				if(root_ctrl->h_app_new_movie)//新添加的movie应用app
				{
					app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_new_movie);
				}

			#if 0	//SP_APP_ATV
				if(g_isTconOccupyLcmi_flag)
				{
					if(root_ctrl->m_eWinVolume == m_eWinVolume_default)
					{
						dsk_volume_dec();
					}
				}
				else if(!(root_ctrl->h_app_atv))
			#endif
				{
					__app_root_volume_scene_create(root_ctrl);
					__com_volume_scene_set_focus(root_ctrl);
				}

				//gscene_hbar_set_state(HBAR_ST_SHOW_WIDTH_VOLUME);
			}
			break;

			default :
				break;
		}
	}

end:
	;//快捷操作处理完毕

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		last_key = -1;
		longdown_cnt = 0;
	}
	else
	{
		last_key = msg->dwAddData1;
	}

	//判断是否需要过滤快捷键
	bok = EPDK_FAIL;

	switch(msg->dwAddData1)
	{
		case GUI_MSG_KEY_VADD:
		case GUI_MSG_KEY_LONGVADD:
		case GUI_MSG_KEY_VDEC:
		case GUI_MSG_KEY_LONGVDEC:
		{
			bok = EPDK_OK;
			break;
		}

		default:
		{
			break;
		}
	}

	//    __wrn("app_root_check_short_key end\n");
	return bok;
}

__s32 app_root_shift_mode(root_ctrl_t *root_ctrl, __u32 type)
{
	static __s32	s_shiftMode = m_eShiftMode_max;
	__u32 		shiftMode_cnt;

	// 获取当前模式
	if(root_ctrl->h_app_setting)
	{
		s_shiftMode = m_eShiftMode_setting;
	}
	else if(root_ctrl->h_app_new_setting)
	{
		s_shiftMode = m_eShiftMode_new_setting;
	}
	else if(root_ctrl->h_app_ebook)
	{
		s_shiftMode = m_eShiftMode_ebook;
	}
	else if(root_ctrl->h_app_new_ebook)
	{
		s_shiftMode = m_eShiftMode_ebook;
	}
	else if(root_ctrl->h_app_photo)
	{
		s_shiftMode = m_eShiftMode_photo;
	}
#if SP_APP_ATV
	else if(root_ctrl->h_app_atv)
	{
		switch(root_ctrl->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
		{
			case OP_RootFlags_CVBS_InputMode_TVFromHome :
				s_shiftMode = m_eShiftMode_tv;
				break;
#if 1		
			case OP_RootFlags_CVBS_InputMode_DVDFromHome :
				s_shiftMode = m_eShiftMode_dvd;
				break;
	
			case OP_RootFlags_CVBS_InputMode_AVINFromHome :
			case OP_RootFlags_CVBS_InputMode_AVINFromDetect :
				s_shiftMode = m_eShiftMode_avin;
				break;
		#endif
		
			default :
				s_shiftMode = m_eShiftMode_atv;
				break;
		}
	}
#endif
	else if(root_ctrl->h_app_movie)
	{
		s_shiftMode = m_eShiftMode_movie;
	}
	else if(root_ctrl->h_app_new_movie)//新添加的movie应用app
	{
		s_shiftMode = m_eShiftMode_movie;
	}
	else if(root_ctrl->h_app_fm)
	{
		s_shiftMode = m_eShiftMode_fm;
	}
#if SP_APP_AUX	
	else if(root_ctrl->h_app_linin)
	{
		s_shiftMode = m_eShiftMode_linein;  // 最后检测可能会背景播放的app.
	}
#endif
	else if(root_ctrl->h_app_music)
	{
		s_shiftMode = m_eShiftMode_music;
	}
	else if(root_ctrl->h_app_new_music)//新添加的音乐
	{
		s_shiftMode = m_eShiftMode_music;
	}
#if SP_APP_ATV
	if((type == GUI_MSG_KEY_SUBMODE) && (!root_ctrl->h_app_atv))
	{
		s_shiftMode = m_eShiftMode_max;
	}
#endif

	for(shiftMode_cnt = 0; shiftMode_cnt < m_eShiftMode_max; shiftMode_cnt++)
	{
		__s32		ret = EPDK_FAIL, media = -1;


		if(type == GUI_MSG_KEY_SUBMODE)
		{
			if(shiftMode_cnt >= 3)
			{
				return -1;
			}

			if(++s_shiftMode > m_eShiftMode_dvd)
			{
				//s_shiftMode = m_eShiftMode_tv;
			}
		}
		else
		{
			if(++s_shiftMode >= m_eShiftMode_max)
			{
				s_shiftMode = m_eShiftMode_fm;
			}
		}

		switch(s_shiftMode)
		{
			case m_eShiftMode_fm :
				__app_root_to_fm(root_ctrl);
				ret = EPDK_OK;
				break;
				
		#if SP_APP_AUX
			case m_eShiftMode_linein :
				__app_root_to_linein(root_ctrl);
				ret = EPDK_OK;
				break;
		#endif

			case m_eShiftMode_music :
				media = m_eMediaTypeList_audio;

			case m_eShiftMode_movie :
				if(media < 0)
				{
					media = m_eMediaTypeList_video;
				}

			case m_eShiftMode_ebook :
				if(media < 0)
				{
					media = m_eMediaTypeList_ebook;
				}

			case m_eShiftMode_photo :
				if(media < 0)
				{
					media = m_eMediaTypeList_photo;
				}

				ret = app_root_shortcut_process(root_ctrl,
				                                app_root_prob_intent_switch_media,
				                                media,
				                                -1,
				                                EPDK_TRUE, EPDK_FALSE);
				break;

			case m_eShiftMode_setting :
				__app_root_to_setting(root_ctrl);
				ret = EPDK_OK;
				break;

		#if SP_APP_ATV
			case m_eShiftMode_atv :
			case m_eShiftMode_tv :
		#if SP_APP_AUX
			case m_eShiftMode_avin :
		#endif
			//case m_eShiftMode_dvd :
				__app_root_to_atv_ex(root_ctrl, s_shiftMode);
				ret = EPDK_OK;
				break;
		#endif
		
			default :
				break;
		}

		if(ret == EPDK_OK)
		{
			return s_shiftMode;
		}
	}

	__app_root_to_fm(root_ctrl);
	return m_eShiftMode_fm;
}

__s32 app_root__explorer_switch_disk(root_ctrl_t   *root_ctrl)
{
	if(root_ctrl->h_app_explorer)
	{
		__s32 hasDisk = EPDK_FALSE;
		__u32 rootType;

		if(root_ctrl->root_para->data != ID_EXPLORER_ALL)
		{
			if(root_ctrl->root_para->root_type == RAT_TF)
			{
				hasDisk = __app_root_has_external_disk(APP_ROOT_TYPE_UD);
				rootType = RAT_USB;
			}
			else if(root_ctrl->root_para->root_type == RAT_USB)
			{
				hasDisk = __app_root_has_external_disk(APP_ROOT_TYPE_SD);
				rootType = RAT_TF;
			}
		}

		if(hasDisk == EPDK_TRUE)
		{
			GUI_ManWinDelete(root_ctrl->h_app_explorer);
			root_ctrl->h_app_explorer = NULL;
			gscene_hbar_set_state(HBAR_ST_HIDE);
			__app_root_change_to_default_bg();
			root_ctrl->root_para->root_type = rootType;
			root_ctrl->root_para->explr_root = root_ctrl->root_para->root_type;
			//root_para->data = ID_EXPLORER_EBOOK;
			//root_para->return_to_explorer_file_list = 0 ;
			app_root_cacheon();
			//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
			root_ctrl->h_app_explorer = app_explorer_create(root_ctrl->root_para);
			GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
			app_root_cacheoff();
			return EPDK_OK;
		}
	}

	return EPDK_FAIL;
}

static __s32 app_root_check_mode_key(__gui_msg_t *msg)
{
	__s32 ret;
	__s32 bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
	root_ctrl_t   *root_ctrl;
	reg_root_para_t *para;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!root_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(!para)
	{
		__wrn("para is null...\n");
		return EPDK_FAIL;
	}

	if(KEY_UP_ACTION == msg->dwAddData2
	    || KEY_REPEAT_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_LONGMODE == msg->dwAddData1)
		{
			__s32 media;
			static __s32 cnt = 0;
			cnt++;

			if(cnt < 5)
			{
				goto end;
			}

			cnt = 0;

			if(app_root__explorer_switch_disk(root_ctrl) == EPDK_FAIL)
			{
				if(root_ctrl->h_app_music)
				{
					media = m_eMediaTypeList_audio;
				}
				else if(root_ctrl->h_app_new_music)//新添加音乐用于
				{
					media = m_eMediaTypeList_audio;
				}
				else if(root_ctrl->h_app_movie)
				{
					media = m_eMediaTypeList_video;
				}
				else if(root_ctrl->h_app_new_movie)//新添加的movie应用app
				{
					media = m_eMediaTypeList_video;
				}
				else if(root_ctrl->h_app_ebook)
				{
					media = m_eMediaTypeList_ebook;
				}
				else if(root_ctrl->h_app_new_ebook)
				{
					media = m_eMediaTypeList_ebook;
				}
				else if(root_ctrl->h_app_photo)
				{
					media = m_eMediaTypeList_photo;
				}
				else
				{
					goto end;
				}

				ret = app_root_shortcut_process(root_ctrl,
				                                app_root_prob_intent_switch_storage,
				                                media,
				                                para->cur_play_storage,
				                                EPDK_TRUE, EPDK_FALSE);
				goto end;
			}
		}
		else if((GUI_MSG_KEY_MODE == msg->dwAddData1) || (GUI_MSG_KEY_SUBMODE == msg->dwAddData1))
		{
			__s32 retMode = app_root_shift_mode(root_ctrl, msg->dwAddData1);
			__msg_jjl("retMode=%d\n", retMode);
		}
	}

end:
	;//快捷操作处理完毕
	//判断是否需要过滤快捷键
	bok = EPDK_FAIL;

	switch(msg->dwAddData1)
	{
		case GUI_MSG_KEY_MODE:
		case GUI_MSG_KEY_LONGMODE:
		{
			bok = EPDK_OK;
			break;
		}

		default:
		{
			break;
		}
	}

	return bok;
}

static __s32 app_root_check_short_key(__gui_msg_t *msg)
{
	__s32 bok = EPDK_FAIL;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
	__s32 ret;
	static __s32 last_key = -1;
	root_ctrl_t   *root_ctrl;
	reg_root_para_t *para;
	app_root_prob_intent_t intent;
	__s32 media_type;
	__s32 storage_type;
	static __s32 longdown_cnt = 0;
	char wav_file[1024];
	para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(!para)
	{
		__wrn("para is null...\n");
		return EPDK_OK;
	}

	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!root_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	//处理SOS快捷
	if(KEY_DOWN_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_SOS == msg->dwAddData1)
		{
			if(1 == root_ctrl->sos_started)
			{
				__app_root_delete_sos(root_ctrl);
			}
			else
			{
				__app_root_to_sos(root_ctrl);
			}

			goto end;
		}
	}

	//处理RECORD快捷
	if(KEY_DOWN_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_RECORD == msg->dwAddData1)
		{
			__s32 ret;
			ret = __app_root_has_external_disk(APP_ROOT_TYPE_SD | APP_ROOT_TYPE_UD);

			if(EPDK_FALSE == ret)
			{
				app_root_show_err_info(root_ctrl, 0, 0, app_root_prob_intent_unknown,
				                       app_root_prob_no_correspond_storage, 1);
				goto end;
			}

			ret = __app_root_has_external_disk(APP_ROOT_TYPE_SD);

			if(EPDK_TRUE == ret)
			{
				__app_root_to_record(root_ctrl, RAT_TF);
				goto end;
			}

			ret = __app_root_has_external_disk(APP_ROOT_TYPE_UD);

			if(EPDK_TRUE == ret)
			{
				__app_root_to_record(root_ctrl, RAT_USB);
				goto end;
			}

			__wrn("should not run here...\n");
		}
	}

#if SP_APP_ATV
	//处理ATV快捷
	if(KEY_DOWN_ACTION == msg->dwAddData2)
	{
		E_SHIFT_MODE	t_eShiftMode = m_eShiftMode_max;

		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_ATV :
				t_eShiftMode = m_eShiftMode_atv;
				break;

			case GUI_MSG_KEY_TV :
				t_eShiftMode = m_eShiftMode_tv;
				break;
		#if SP_APP_AUX
			case GUI_MSG_KEY_AVIN :
				t_eShiftMode = m_eShiftMode_avin;
				break;
		#endif

			case GUI_MSG_KEY_DVD :
				t_eShiftMode = m_eShiftMode_dvd;
				break;
#if 1
			case GUI_MSG_KEY_BT:
				root_ctrl->root_para->m_maskFlags |= MASK_RootFlags_AuxMode_BT;
				bok = __app_root_to_linein(root_ctrl);
				break;
		#endif

			default :
				break;
		}

		if(t_eShiftMode != m_eShiftMode_max)
		{
			__app_root_to_atv_ex(root_ctrl, t_eShiftMode);
			goto end;
		}
	}
#endif

	//处理FM快捷
	if(KEY_DOWN_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_FM == msg->dwAddData1)
		{
			__app_root_to_fm(root_ctrl);
			goto end;
		}
	}

	//处理SETTING快捷
	if(KEY_DOWN_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_SETTING == msg->dwAddData1)
		{
			__app_root_to_setting(root_ctrl);
			app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_SETTING);
			goto end;
		}
	}

	//处理音频和视频快捷
	{
		//得到媒体类型和处理方式
		intent = app_root_prob_intent_unknown;
		media_type = -1;
		storage_type = para->cur_play_storage;

		if(KEY_DOWN_ACTION == msg->dwAddData2
		    || KEY_REPEAT_ACTION == msg->dwAddData2)
		{
			if(GUI_MSG_KEY_AUDIO == msg->dwAddData1)
			{
				intent = app_root_prob_intent_switch_media;
				media_type = m_eMediaTypeList_audio;//音频
			}
			else if(GUI_MSG_KEY_LONGAUDIO == msg->dwAddData1)
			{
				longdown_cnt++;

				if(10 == longdown_cnt)
				{
					intent = app_root_prob_intent_switch_storage;
					media_type = m_eMediaTypeList_audio;//音频
				}
			}
			else if(GUI_MSG_KEY_VIDEO == msg->dwAddData1)
			{
				intent = app_root_prob_intent_switch_media;
				media_type = m_eMediaTypeList_video;//视频
			}
			else if(GUI_MSG_KEY_LONGVIDEO == msg->dwAddData1)
			{
				longdown_cnt++;

				if(10 == longdown_cnt)
				{
					intent = app_root_prob_intent_switch_storage;
					media_type = m_eMediaTypeList_video;//视频
				}
			}
			else if(GUI_MSG_KEY_EBOOK == msg->dwAddData1)
			{
				intent = app_root_prob_intent_switch_media;
				media_type = m_eMediaTypeList_ebook;//电子书
			}
			else if(GUI_MSG_KEY_LONGEBOOK == msg->dwAddData1)
			{
				longdown_cnt++;

				if(10 == longdown_cnt)
				{
					intent = app_root_prob_intent_switch_storage;
					media_type = m_eMediaTypeList_ebook;//电子书
				}
			}
			else if(GUI_MSG_KEY_PHOTO == msg->dwAddData1)
			{
				intent = app_root_prob_intent_switch_media;
				media_type = m_eMediaTypeList_photo;//图片
			}
			else if(GUI_MSG_KEY_LONGPHOTO == msg->dwAddData1)
			{
				longdown_cnt++;

				if(10 == longdown_cnt)
				{
					intent = app_root_prob_intent_switch_storage;
					media_type = m_eMediaTypeList_photo;//图片
				}
			}
			else if(GUI_MSG_KEY_SWITCH == msg->dwAddData1)
			{
				intent = app_root_prob_intent_switch_storage;

				if(root_ctrl->h_app_music)
				{
					media_type = m_eMediaTypeList_audio;//音频
				}
				else if(root_ctrl->h_app_new_music)
				{
					media_type = m_eMediaTypeList_audio;//新添加音频
				}
				else if(root_ctrl->h_app_movie)
				{
					media_type = m_eMediaTypeList_video;//视频
				}
				else if(root_ctrl->h_app_new_movie)//新添加的movie应用app
				{
					media_type = m_eMediaTypeList_video;//视频
				}
				else if(root_ctrl->h_app_photo)
				{
					media_type = m_eMediaTypeList_photo;//图片
				}
				else if(root_ctrl->h_app_ebook)
				{
					media_type = m_eMediaTypeList_ebook;//电子书
				}
				else if(root_ctrl->h_app_new_ebook)
				{
					media_type = m_eMediaTypeList_ebook;//新添加的电子书
				}
			}
		}
	}

	if(intent < app_root_prob_intent_unknown)
	{
		if(root_ctrl->h_app_home
		    && MULTI_SCREEN_HOME_MODE_NORMAL == app_multi_screen_home_get_mode(root_ctrl->h_app_home))
		{
			app_root_shortcut_process(root_ctrl, intent, media_type, storage_type, EPDK_TRUE, EPDK_FALSE);
		}
		else
		{
			app_root_shortcut_process(root_ctrl, intent, media_type, storage_type, EPDK_FALSE, EPDK_FALSE);
		}
	}

end:
	;//快捷操作处理完毕

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		last_key = -1;
		longdown_cnt = 0;
	}
	else
	{
		last_key = msg->dwAddData1;
	}

	//判断是否需要过滤快捷键
	switch(msg->dwAddData1)
	{
		case GUI_MSG_KEY_FM:

			//case GUI_MSG_KEY_LONGFM:
		case GUI_MSG_KEY_RECORD:
		case GUI_MSG_KEY_LONGRECORD:
		case GUI_MSG_KEY_VIDEO:

			//case GUI_MSG_KEY_LONGVIDEO:
		case GUI_MSG_KEY_AUDIO:
		case GUI_MSG_KEY_LONGAUDIO:
		case GUI_MSG_KEY_EBOOK:
		case GUI_MSG_KEY_LONGEBOOK:
		case GUI_MSG_KEY_PHOTO:
		case GUI_MSG_KEY_LONGPHOTO:
		case GUI_MSG_KEY_SOS:
		case GUI_MSG_KEY_LONGSOS:
		case GUI_MSG_KEY_ATV :
		case GUI_MSG_KEY_LONGATV :
		case GUI_MSG_KEY_TV :
		case GUI_MSG_KEY_LONGTV :
		case GUI_MSG_KEY_AVIN :
		case GUI_MSG_KEY_LONGAVIN :
		//case GUI_MSG_KEY_DVD :
		//case GUI_MSG_KEY_LONGDVD :
		{
			bok = EPDK_OK;
			break;
		}

		default:
		{
			break;
		}
	}

	//    __wrn("app_root_check_short_key end\n");
	return bok;
}

void app_root_show_standby(void)
{
	H_LYR t_hlyr = NULL;
	__s32 scn_width, scn_height;
	dsk_display_get_size(&scn_width, &scn_height);

	if(NULL == t_hlyr)
	{
		RECT  rect;
		rect.x		= 0;
		rect.y		= 0;		//- 20;
		rect.width	= scn_width;
		rect.height 	= scn_height;
		t_hlyr = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_ON, "");

		if(NULL == t_hlyr)
		{
			__wrn("**********digit layer create fail...\n");
			return;
		}
	}

	if(t_hlyr)
	{
		GUI_RECT gui_rect;
		char text[128] = "";
		gui_rect.x0 = 0;
		gui_rect.x1 = scn_width - 1;
		gui_rect.y0 = 0;
		gui_rect.y1 = scn_height - 1;
		gscene_bgd_set_state(BGD_STATUS_HIDE);
		GUI_LyrWinSel(t_hlyr);
		GUI_SetBkColor(0);
		GUI_ClearRectEx(&gui_rect);
		GUI_SetFont(SWFFont);
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
		GUI_SetColor(GUI_WHITE);
		GUI_UC_SetEncodeUTF8();
		dsk_langres_get_menu_text(STRING_ROOT_STANDBY, text, sizeof(text));
		strcat(text, "......");
		GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
		esKRNL_TimeDly(300);
		SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
		GUI_LyrWinDelete(t_hlyr);
	}
}

void app_root_check_key_power(__gui_msg_t *msg)
{
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		if((GUI_MSG_KEY_POWER == msg->dwAddData1) || (GUI_MSG_KEY_LONGPOWER == msg->dwAddData1))
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			gscene_hbar_set_state(HBAR_ST_HIDE);
			__app_root_change_to_default_bg();
			/*
			//删除后台linein
			__app_root_delete_bg_linein(msg);


			//删除后台音乐
			__app_root_delete_bg_music(msg);
			*/
			__app_root_delete_all_app(root_ctrl);
			app_root_show_standby();
			SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
			app_root_cacheon();
			__app_home_restore(msg);
			app_root_cacheoff();
		}
	}
}

#if SP_Software_Echo
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByKnob)
void app_root_echo_proc(void)
{
	if(dsk_audio_echo_is_open() && (!app_record_is_recording()))
	{
		__u32	echoLevel;
		echoLevel = dsk_echo_level_get_setting();

		if(echoLevel == EPDK_FAIL)
		{
			return;
		}

		dsk_echo_level_set(echoLevel);
	}
}
#endif
#endif

static __s32 app_root_command_proc(__gui_msg_t *msg)
{
	root_para_t *root_para;
	root_ctrl_t   *root_ctrl;
	__gui_msg_t mymsg;
	root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	root_para = root_ctrl->root_para;
#if SP_FavoriteFiles

	if(music_play_status != 0)
	{
		root_para->enter_media_type[m_eMediaTypeList_audio] = music_play_status;
	}

#endif

	switch(LOWORD(msg->dwAddData1))
	{
		__msg("LOWORD(msg->dwAddData1) =%d\n",LOWORD(msg->dwAddData1));
		case COM_VOLUME_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case com_volume_scene_msg_timeout:
				{
					__com_volume_scene_delete(root_ctrl);
					//app_root_process_after_show_dlg(root_ctrl);
					break;
				}

				default:
				{
					break;
				}
			}

			break;
		}
#if SP_APP_AUX
		case APP_LINEIN_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_MMENU:
				{
					__s32 ret;
					__s32 dwReserved;
					dwReserved = msg->dwReserved;//0代表全部删除，1代表进入linin后台模式
					__wrn("root_ctrl->h_app_linin=%x\n", root_ctrl->h_app_linin);

					if(0 == dwReserved)
					{
						if((root_ctrl->h_app_linin) &&(!app_linein_has_bt()))
						{
							__wrn("before GUI_ManWinDelete(root_ctrl->h_app_linin)\n");
							GUI_ManWinDelete(root_ctrl->h_app_linin);
							__wrn("after GUI_ManWinDelete(root_ctrl->h_app_linin)\n");
							root_ctrl->h_app_linin = 0;
						}
					}
					else
					{
						if(root_ctrl->h_app_linin)
						{
							app_linein_set_mode(root_ctrl->h_app_linin, LINEIN_MODE_BACKGROUND);
						}
					}

					ret = __app_root_has_forground_apps(root_ctrl);
					__wrn("ret=%d\n", ret);

					if(EPDK_FALSE == ret)
					{
						__app_home_restore(msg);
					}

					break;
				}

				case SWITCH_TO_LINE_IN:
				{
					__app_root_to_linein(root_ctrl);
					break;
				}

				default:
				{
					break;
				}
			}
		}
		break;
#endif	

		case APP_HOME_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				__msg("APP_HOME_ID HIWORD(msg->dwAddData1) =%d\n", HIWORD(msg->dwAddData1));
				case SWITCH_TO_OTHER_APP:
				{
					//esKSRV_SysInfo();
					__wrn("SWITCH_TO_OTHER_APP msg->dwAddData2 =%d\n", msg->dwAddData2);
					switch(msg->dwAddData2)
					{
						case ID_HOME_FM:
						case ID_FM_SHORTCUT:
						{
							__bool is_linein_insert;
							//gscene_hbar_set_state(HBAR_ST_HIDE);
							//                          __app_root_change_to_default_bg();
							//删除后台linein
							__app_root_delete_bg_linein(msg);
							//删除后台音乐
							__app_root_delete_bg_music(msg);
							__msg("**********enter fm**********\n");
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							is_linein_insert = app_root_is_linein_insert();
							__wrn("is_linein_insert=%d\n", is_linein_insert);

							if(EPDK_FALSE == is_linein_insert)
								//Tarring
							{
								IOCTRL__PA_MODE__SET_AB();
								app_root_cacheon();
								root_ctrl->h_app_fm =  app_fm_create(root_para);
								GUI_WinSetFocusChild(root_ctrl->h_app_fm);
								app_root_cacheoff();
							}
							else
							{
								gscene_hbar_set_state(HBAR_ST_HIDE);
								app_root_show_dlg(msg->h_deswin, STRING_ROOT_UNPLUG_LINEIN, 0, 0);
								app_root_cacheon();
								__app_home_restore(msg);
								app_root_cacheoff();
							}

							break;
						}
						break;
						case ID_HOME_MOVIE://视频app进入到资源管理器manwin窗口创建
						{
							//gscene_hbar_set_state(HBAR_ST_HIDE);
							//                            __app_root_change_to_default_bg();
							__msg("**********enter movie explorer**********\n");
							__wrn("**********enter movie explorer**********\n");
							__msg("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
#if SP_FavoriteFiles

							if(root_para->enter_media_type[m_eMediaTypeList_video] == RAT_MEDIA_TYPE_VIDEO)
							{
								root_para->data = ID_EXPLORER_MOVIE;
								__wrn("**********ID_EXPLORER_MOVIE**********\n");
							}
							else if(root_para->enter_media_type[m_eMediaTypeList_video] == RAT_MEDIA_TYPE_FAVOR_VIDEO)
							{
								root_para->data = ID_EXPLORER_MOVIEFAVOR;
								__wrn("**********ID_EXPLORER_MOVIEFAVOR**********\n");
							}

#else
							root_para->data = ID_EXPLORER_MOVIE;
#endif
							root_para->return_to_explorer_file_list = 0 ;
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							app_root_cacheon();
							root_ctrl->h_app_explorer =  app_explorer_create(root_para);
							__msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
							GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
							app_root_cacheoff();
							break;
						}
						break;

						case ID_MOVIE_SHORTCUT://视频文件播放app应用窗口创建
						{
					#if 1
							//删除后台linein
							__wrn("before __app_root_delete_bg_linein...\n");
							__app_root_delete_bg_linein(msg);
							__wrn("after __app_root_delete_bg_linein...\n");
							//删除后台音乐
							__wrn("before __app_root_delete_bg_music...\n");
							__app_root_delete_bg_music(msg);
							__wrn("after __app_root_delete_bg_music...\n");
							__msg("**********shortcut to movie**********\n");
							__msg("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
							root_para->data = 0;
							root_para->tv_mode = MOVIE_MODE_NORMAL;
							root_para->return_to_explorer_file_list = 1 ;
							app_root_cacheon();
							__wrn("before app_movie_create...\n");
							root_ctrl->h_app_movie = app_movie_create(root_para);
							__wrn("after app_movie_create...\n");
							GUI_WinSetFocusChild(root_ctrl->h_app_movie);
							app_root_cacheoff();
					#endif
						}
						break;

						case ID_HOME_PHOTO:
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__msg("**********enter photo explorer**********\n");
							__msg("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;
							root_para->explr_root = msg->dwReserved;
							root_para->data = ID_EXPLORER_PHOTO;
							root_para->return_to_explorer_file_list = 0 ;
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							app_root_cacheon();
							root_ctrl->h_app_explorer =  app_explorer_create(root_para);
							__msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
							GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
							app_root_cacheoff();
							break;
						}
						break;

						case ID_PHOTO_SHORTCUT:
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__msg("**********shortcut to photo**********\n");
							__wrn("before set music bg mode...\n");
							app_root_set_music_bg_mode(msg);
							__wrn("after set music bg mode...\n");

						#if SP_APP_AUX
							__wrn("root_ctrl->h_app_linin=0x%x\n", root_ctrl->h_app_linin);
							if(root_ctrl->h_app_linin)
							{
								app_linein_set_mode(root_ctrl->h_app_linin, LINEIN_MODE_BACKGROUND);
							}
						#endif

							app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PHOTO);
							__msg("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
							root_para->data = 0;
							root_para->return_to_explorer_file_list = 1 ;
							app_root_cacheon();
							root_ctrl->h_app_photo = app_photo_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_photo);
							app_root_cacheoff();
							break;
						}
						break;

						case ID_HOME_MUSIC:
						{
#if 0
							//gscene_hbar_set_state(HBAR_ST_HIDE);
							//                            __app_root_change_to_default_bg();
							__msg("**********enter music explorer**********\n");
							__msg("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;
							root_para->explr_root = msg->dwReserved;
							root_para->data = ID_EXPLORER_MUSIC;
							root_para->return_to_explorer_file_list = 0 ;
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							root_ctrl->h_app_explorer =  app_explorer_create(root_para);
							__msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
							GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
#else
							{
								app_root_prob_intent_t intent;
								__s32 media_type;
								__s32 storage_type;
								__s32 ret = EPDK_FAIL;
								intent = app_root_prob_intent_switch_media;
								media_type = m_eMediaTypeList_audio;//音频
								storage_type = -1;

								if(root_ctrl->h_app_home
								    && MULTI_SCREEN_HOME_MODE_NORMAL == app_multi_screen_home_get_mode(root_ctrl->h_app_home))
								{
									ret = app_root_shortcut_process(root_ctrl, intent, media_type, storage_type, EPDK_TRUE, EPDK_TRUE);
								}
								else
								{
									ret = app_root_shortcut_process(root_ctrl, intent, media_type, storage_type, EPDK_FALSE, EPDK_TRUE);
								}

								if(ret == EPDK_FAIL)
								{
									__app_home_restore_ex(root_ctrl);
								}
							}
#endif
							break;
						}
						break;

						case ID_MUSIC_SHORTCUT:
						{
							__s32 ret;
							__s32 exist;
							H_WIN hmusic;
							//删除后台linein
							__app_root_delete_bg_linein(msg);
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							root_para->explorer_play_music = 0;
							root_para->root_type = msg->dwReserved;
							root_para->data = MUSICPLAYER_NORMAL;
							root_para->return_to_explorer_file_list = 0 ;
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							app_root_cacheon();
							exist = __app_root_is_bg_music_exist(msg, &hmusic);

							if(1 == exist)//重建后台音乐
							{
								ret = app_music_get_root_type(hmusic, &root_para->root_type);

								if(EPDK_FAIL == ret)
								{
									root_para->root_type = RAT_TF;
								}

								ret = app_music_restore(hmusic, root_para);

								if(EPDK_OK == ret)
								{
									if(root_ctrl->h_app_music != hmusic)
									{
										__msg("*******root_ctrl->h_app_music != hmusic********\n");
									}

									root_ctrl->h_app_music = hmusic;
								}
								else
								{
									__msg("*****app_music_restore fail*******\n");
									__app_root_delete_bg_music(msg);
									__here__;
									root_ctrl->h_app_music =  app_music_create(root_para);
									__here__;
								}
							}
							else
							{
								root_para->root_type = msg->dwReserved;
								root_ctrl->h_app_music =  app_music_create(root_para);
							}

							GUI_WinSetFocusChild(root_ctrl->h_app_music);
							app_root_cacheoff();
							break;
						}

						case ID_HOME_EBOOK:
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							root_para->root_type = msg->dwReserved;
							root_para->explr_root = msg->dwReserved;
							root_para->data = ID_EXPLORER_EBOOK;
							root_para->return_to_explorer_file_list = 0 ;
							app_root_cacheon();
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							root_ctrl->h_app_explorer =  app_explorer_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
							app_root_cacheoff();
						}
						break;

						case ID_EBOOK_SHORTCUT:
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__msg("**********shortcut to ebook**********\n");
							__wrn("before set music bg mode...\n");
							app_root_set_music_bg_mode(msg);
							__wrn("after set music bg mode...\n");

						#if SP_APP_AUX
							__wrn("root_ctrl->h_app_linin=0x%x\n", root_ctrl->h_app_linin);
							if(root_ctrl->h_app_linin)
							{
								app_linein_set_mode(root_ctrl->h_app_linin, LINEIN_MODE_BACKGROUND);
							}
						#endif

							app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_EBOOK);
							__msg("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
							root_para->data = 0;
							root_para->return_to_explorer_file_list = 1 ;
							app_root_cacheon();
							root_ctrl->h_app_ebook = app_ebook_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_ebook);
							app_root_cacheoff();
							break;
						}
						break;

					#if (SP_FixedMic) || (SP_HotplugMic)
						case ID_HOME_RECORD:
					#endif
						case ID_RECORD_SHORTCUT:
							if(1)
							{
								__s32 mic_in;
								gscene_hbar_set_state(HBAR_ST_HIDE);
								__app_root_change_to_default_bg();
								//删除后台linein
								__app_root_delete_bg_linein(msg);
								//删除后台音乐
								__app_root_delete_bg_music(msg);
#if (!SP_FixedMic)
							#ifdef SP_APP_RECORD_ENTER_FORCE
								if(0)
							#else
								if(IOCTRL__MIC_DET__IS_PLUGIN())
							#endif
#endif
								{
									__msg("**********ID_HOME_RECORD**********\n");
									root_ctrl->root_para->root_type = msg->dwReserved;//RAT_USB或者RAT_TF
									root_ctrl->root_para->record_opt = 0;//正常录音
									//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
									app_root_cacheon();
									root_ctrl->h_app_record =  app_record_create(root_para);
									GUI_WinSetFocusChild(root_ctrl->h_app_record);
									app_root_cacheoff();
								}

#if (!SP_FixedMic)
								else
								{
									//gscene_hbar_set_state(HBAR_ST_HIDE);
									app_root_show_dlg(msg->h_deswin, STRING_ROOT_INSERT_MIC, 0, 0);
									app_root_cacheon();
									__app_home_restore(msg);
									app_root_cacheoff();
								}

#endif
							}

							break;

					#if SP_APP_ATV
						//case ID_HOME_DVD:
						case ID_HOME_TV:
					//	case ID_HOME_AVIN:
						case ID_HOME_ATV:
						{
							root_ctrl->root_para->m_maskFlags &= ~MASK_RootFlags_CVBS_InputMode;
						
							switch(msg->dwAddData2)
							{
								case ID_HOME_TV:
									root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_TVFromHome;
									break;
#if 0
								case ID_HOME_DVD:
									root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_DVDFromHome;
									break;
							#endif
						
								case ID_HOME_AVIN:
									root_ctrl->root_para->m_maskFlags |= OP_RootFlags_CVBS_InputMode_AVINFromHome;
									break;
						
								default :
									break;
							}
						}
					
						case ID_ATV_SHORTCUT:
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__app_root_delete_bg_linein(msg);	//删除后台linein
							__app_root_delete_bg_music(msg);	//删除后台音乐
							app_root_cacheon();
							root_ctrl->h_app_atv =  app_atv_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_atv);
							app_root_cacheoff();
							break;
						}
					#endif

						case ID_LINEIN_SHORTCUT:
						{
						#if SP_APP_AUX
							if(root_ctrl->h_app_linin)
							{
								__wrn("linein alreadly exist...\n");
								break;
							}

							app_root_cacheon();
							__wrn("********line in shortcut created********\n");
							root_para->data = msg->dwReserved;//linein_mode_e
							__wrn("root_para->data=%d\n",  root_para->data);
							root_ctrl->h_app_linin = app_linein_create(root_para);
							__wrn("root_ctrl->h_app_linin=%x\n", root_ctrl->h_app_linin);
							GUI_WinSetFocusChild(root_ctrl->h_app_linin);
							__wrn("after GUI_WinSetFocusChild(root_ctrl->h_app_linin);\n");
							app_root_cacheoff();
						#endif
						
							break;
						}

						case ID_HOME_AUX:
						{
						#if SP_APP_AUX
							__bool is_linein_insert;
							__wrn("ID_HOME_AUX\n");
							//删除后台音乐
							__app_root_delete_bg_music(msg);
							is_linein_insert = app_root_is_linein_insert();
							__wrn("is_linein_insert=%d\n", is_linein_insert);

							if(EPDK_FALSE == is_linein_insert)
							{
								gscene_hbar_set_state(HBAR_ST_HIDE);
								app_root_show_dlg(msg->h_deswin, STRING_ROOT_LINEIN_NOT_INSERT, 0, 0);
							}

							app_root_cacheon();

							if(root_ctrl->h_app_linin)
							{
								app_linein_set_mode(root_ctrl->h_app_linin, LINEIN_MODE_NORMAL);
							}
							else
							{
								__wrn("********line in shortcut created********\n");
								root_para->data = LINEIN_MODE_NORMAL;//linein_mode_e
								__wrn("root_para->data=%d\n",  root_para->data);
								root_ctrl->h_app_linin = app_linein_create(root_para);
								__wrn("root_ctrl->h_app_linin=%x\n", root_ctrl->h_app_linin);
								GUI_WinSetFocusChild(root_ctrl->h_app_linin);
								__wrn("after GUI_WinSetFocusChild(root_ctrl->h_app_linin);\n");
							}

							app_root_cacheoff();
						#endif
						
							break;
						}

						case ID_HOME_ALARM:
						{
							__wrn("ID_HOME_ALARM\n");
							app_root_cacheon();
							__msg("**********enter setting alarm**********\n");
							root_ctrl->root_para->setting_opt = SETTING_ALARM;//设置
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							root_ctrl->h_app_setting =  app_setting_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_setting);
							app_root_cacheoff();
							break;
						}

						case ID_HOME_OTHERS:
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							//删除后台音乐
							//__app_root_delete_bg_music(msg);
							__msg("**********enter other apps**********\n");

							if(0 == msg->dwReserved)
							{
								__app_root_change_to_default_bg();
								__msg("**********enter setting**********\n");
								root_ctrl->root_para->setting_opt = 0;//通用设置
								//								root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_setting =  app_setting_create(root_para);
								GUI_WinSetFocusChild(root_ctrl->h_app_setting);
								break;
							}
							else if(1 == msg->dwReserved)
							{
								__app_root_change_to_default_bg();
								__msg("**********enter setting**********\n");
								root_ctrl->root_para->setting_opt = 1;//电源管理
								//								root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_setting =  app_setting_create(root_para);
								GUI_WinSetFocusChild(root_ctrl->h_app_setting);
								break;
							}
							else if(2 == msg->dwReserved)
							{
								__msg("**********enter explorer manage**********\n");
								gscene_hbar_set_state(HBAR_ST_HIDE);
								__app_root_change_to_default_bg();
								__msg("**********enter explorer**********\n");
								__msg("root type=%d\n", msg->dwReserved);
								root_para->root_type = RAT_TF;//msg->dwReserved;//RAT_TF;
								root_para->explr_root = RAT_TF;//msg->dwReserved;
								root_para->data = ID_EXPLORER_ALL;//进入文件管理manwin窗口
								//root_ctrl->root_para->h_parent= GUI_WinGetHandFromName(APP_ROOT);
								root_ctrl->h_app_explorer =  app_explorer_create(root_para);
								__msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
								GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
								break;
							}
							else if(3 == msg->dwReserved)//进入日历app应用程序
							{
#if SP_APP_CALENDAR
								__msg("**********enter calendar**********\n");
								__wrn("**********enter calendar is**********\n");
								__app_root_change_to_default_bg();
								//root_ctrl->root_para->h_parent= GUI_WinGetHandFromName(APP_ROOT);
								root_ctrl->h_app_calendar =  app_calendar_create(root_para);
								GUI_WinSetFocusChild(root_ctrl->h_app_calendar);
#else
								__err("sub menu focus item invalid...\n");
#endif
								break;
							}
							else
							{
								__err("sub menu focus item invalid...\n");
								break;
							}
						}
						break;
						case ID_HOME_NUMBER:	//新添加窗口
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							//删除后台音乐
							//__app_root_delete_bg_music(msg);							
							__wrn("**********enter other new apps**********\n");

							if(0 == msg->dwReserved)//通用设置子菜单创建进入
							{
								__app_root_change_to_default_bg();
								__msg("**********enter setting**********\n");
								root_ctrl->root_para->setting_opt = 0;
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_setting =  app_setting_create(root_para);//子菜单创建调用,//app_number_create(root_para);//
								GUI_WinSetFocusChild(root_ctrl->h_app_setting);
								break;
							}
							else if(1 == msg->dwReserved)//电子书子菜单创建进入
							{
								__app_root_change_to_default_bg();
								root_para->root_type = msg->dwReserved;
								root_para->explr_root = msg->dwReserved;
								root_para->data = ID_EXPLORER_EBOOK;
								root_para->return_to_explorer_file_list = 0 ;
								app_root_cacheon();
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_explorer =  app_explorer_create(root_para);
								GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
								app_root_cacheoff();
							}
							else if(2 == msg->dwReserved)//创建新的manwin窗口
							{
								__app_root_change_to_default_bg();
								__msg("**********enter new setting**********\n");
								root_ctrl->root_para->setting_opt = 0;
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_new_setting =  app_New_Setting_manwin_create(root_para);//新的app通用设置子菜单创建调用
								GUI_WinSetFocusChild(root_ctrl->h_app_new_setting);//创建manwin窗口的储存
								break;
							}
							else
							{
								__err("sub menu focus item invalid...\n");
								break;
							}
						}
						break;
						
						case ID_HOME_NEW_SETTING_NUMBER://在home应用界面新添加的app应用程序创建mainwin窗口
						{
							if(root_ctrl->h_app_home)//不等于空,删除home应用界面manwin窗口
							{
								__wrn("root_ctrl->h_app_home IS delete....\n");
								GUI_ManWinDelete(root_ctrl->h_app_home);
								root_ctrl->h_app_home=NULL;
							}
							__wrn("APP_HOME_ID to new app IS....\n");
							__wrn("ID_OP_NUMBER_MENU_ENTER to new app IS....\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							//if(0 == msg->dwReserved)//通用设置子菜单创建进入
							{
								__app_root_change_to_default_bg();
								__msg("**********enter new setting**********\n");
								//root_ctrl->root_para->setting_opt = 0;
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_new_setting =  app_New_Setting_manwin_create(root_para);//新的app通用设置子菜单创建调用
								GUI_WinSetFocusChild(root_ctrl->h_app_new_setting);//创建manwin窗口的消息信号发送选择窗口
								break;
							}
					//
						}
						break;
						case ID_HOME_NEW_EXPORER://在home应用界面新添加的new explorer 资源管理器app应用程序创建mainwin窗口
						{
							/*if(root_ctrl->h_app_home)//不等于空,删除home应用界面manwin窗口
							{
								__wrn("root_ctrl->h_app_home IS delete....\n");
								GUI_ManWinDelete(root_ctrl->h_app_home);
								root_ctrl->h_app_home=NULL;
							}*/
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							__app_root_change_to_default_bg();
							__wrn("**********enter new explorer create is...**********\n");
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type  = RAT_TF;//msg->dwReserved;//2，RAT_TF;
							root_para->explr_root = RAT_TF;//msg->dwReserved;
							
							root_para->data = ID_NEW_EXPLORER_MOVIE;	//单独显示视频文件内容数据，一起存放到结构体root_para的data里面
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							{
								app_root_cacheon();
								root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);//创建new explorer 应用的manwin窗口
								__wrn("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_new_explorer);
								GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);//按键消息信号发送到当前创建的manwin窗口
								app_root_cacheoff();
								__wrn("**********enter new explorer create ok  **********\n");
								__wrn("ID_HOME_NEW_EXPORER msg->dwAddData2 =%d\n", msg->dwAddData2);
								break;
							}
						}
						break;
						case ID_HOME_NEW_MOVIE:/*在home应用界面新添加一个movie视频app应用程序，创建manwin窗口*/
						{
							#if 0
							//删除后台linein
							__app_root_delete_bg_linein(msg);
							//删除后台音乐
							__app_root_delete_bg_music(msg);
							__wrn("********** to movie**********\n");
							//root_para->data = 0;
							//root_para->tv_mode = MOVIE_MODE_NORMAL;
							
							
							gscene_hbar_set_state(HBAR_ST_HIDE);/*设置为隐藏*/
							__app_root_change_to_default_bg();
							__wrn("**********enter new movie create is...**********\n");
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
							root_para->root_type = root_para->explr_root;
							root_para->data = 0;
							root_para->tv_mode = MOVIE_MODE_NORMAL;/*视频模式为正常模式*/
							{
								//app_root_cacheon();
								root_ctrl->h_app_new_movie = app_new_movie_manwin_create(root_para);
								__wrn("root_ctrl->h_app_new_movie = %x\n", root_ctrl->h_app_new_movie);
								GUI_WinSetFocusChild(root_ctrl->h_app_new_movie);//按键消息信号发送到当前创建的manwin窗口
								//app_root_cacheoff();
								__wrn("**********enter new movie create ok  **********\n");
								break;
							}
							#endif
							#if 1/*第一次进入修改后的*/
							//删除后台linein
							__app_root_delete_bg_linein(msg);
							//删除后台音乐
							__app_root_delete_bg_music(msg);
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							__app_root_change_to_default_bg();
							if(root_ctrl->h_app_new_explorer)//如果explorer的manwin窗口存在
							{
								GUI_ManWinDelete(root_ctrl->h_app_new_explorer);//删除这个manwin窗口
								root_ctrl->h_app_new_explorer = NULL;
								__wrn("**********delete the new explorer**********\n");
							}
							__wrn("**********home to new movie**********\n");
							app_root_cacheon();
							

							root_para->data = -1;//默认直接播放
							root_para->tv_mode = MOVIE_MODE_NORMAL;
							root_para->explr_root = RAT_TF;//msg->dwReserved;//2,RAT_TF
							root_para->root_type = root_para->explr_root;
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							root_ctrl->h_app_new_movie = app_new_movie_manwin_create(root_para);
							__wrn("after app_new_movie_create...\n");
							GUI_WinSetFocusChild(root_ctrl->h_app_new_movie);
							app_root_cacheoff();
							#endif
							break;
						}
						break;

						case ID_HOME_NEW_PHOTO:/*在home应用界面新添加一个相册app应用程序，创建manwin窗口*/
						{
							#if 0//直接进入显示相册app应用
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__wrn("**********shortcut to new photo**********\n");
							__wrn("before set music bg mode...\n");
							app_root_set_music_bg_mode(msg);
							__wrn("after set music bg mode...\n");


							app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PHOTO);
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
							root_para->data = 0;
							root_para->return_to_explorer_file_list = 1 ;
							app_root_cacheon();
							root_ctrl->h_app_new_photo = app_new_photo_manwin_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_new_photo);
							app_root_cacheoff();
							break;
							#else//进入资源管理器列表，然后显示所有图片文件

							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							__app_root_change_to_default_bg();
							__wrn("**********enter new explorer create is... to new photo**********\n");
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type  = RAT_TF;//msg->dwReserved;//2，RAT_TF;
							root_para->explr_root = RAT_TF;//msg->dwReserved;
							
							root_para->data = ID_NEW_EXPLORER_PHOTO;	//单独显示图片文件内容数据，一起存放到结构体root_para的data里面
							__wrn("root_para->data=%d\n", root_para->data);
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							{
								app_root_cacheon();
								root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);//创建new explorer 应用的manwin窗口
								__wrn("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_new_explorer);
								GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);//按键消息信号发送到当前创建的manwin窗口
								app_root_cacheoff();
								__wrn("**********enter new explorer create ok  **********\n");
								__wrn("ID_HOME_NEW_EXPORER msg->dwAddData2 =%d\n", msg->dwAddData2);
								break;
							}
							#endif
						}
						break;

						case ID_HOME_NEWMUSIC:/*新添加的音乐播放器app程序*/
						{
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__wrn("\n********************home switch to new music ***************\n");
							root_para->root_type = msg->dwReserved;//2，TF卡
							root_para->explr_root = msg->dwReserved;
							root_para->data	=	ID_NEW_EXPLORER_MUSIC;//搜索音乐文件的id索引
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							root_ctrl->h_app_new_music = app_new_music_manwin_create(root_para);
							app_root_cacheon();

							GUI_WinSetFocusChild(root_ctrl->h_app_new_music);//按键消息信号发送到当前创建的manwin窗口
							app_root_cacheoff();

							
						}
						break;

						case ID_HOME_NEW_EBOOK:/*新添加的电子书app程序*/
						{
							#if 0//直接打开
							gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__wrn("\n********************home switch to new ebook ***************\n");
							root_para->root_type = msg->dwReserved;//2，TF卡
							root_para->explr_root = msg->dwReserved;
							root_para->data	=	ID_NEW_EXPLORER_EBOOK;//搜索电子书文件的id索引
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							root_ctrl->h_app_new_ebook = app_new_ebook_manwin_create(root_para);//创建电子书manwin窗口
							app_root_cacheon();

							GUI_WinSetFocusChild(root_ctrl->h_app_new_ebook);//按键消息信号发送到当前创建的manwin窗口
							app_root_cacheoff();
							#else//通过资源管理器列表打开
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							__app_root_change_to_default_bg();
							__wrn("**********enter new explorer create is... to new photo**********\n");
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type  = RAT_TF;//msg->dwReserved;//2，RAT_TF;
							root_para->explr_root = RAT_TF;//msg->dwReserved;
							
							root_para->data = ID_NEW_EXPLORER_EBOOK;	//单独显示电子书文件内容数据，一起存放到结构体root_para的data里面
							__wrn("root_para->data=%d\n", root_para->data);
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							{
								app_root_cacheon();
								root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);//创建new explorer 应用的manwin窗口
								__wrn("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_new_explorer);
								GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);//按键消息信号发送到当前创建的manwin窗口
								app_root_cacheoff();
								__wrn("**********enter new explorer create ok  **********\n");
								__wrn("ID_HOME_NEW_EXPORER msg->dwAddData2 =%d\n", msg->dwAddData2);
								break;
							}
							#endif
						}
						break;

						case ID_HOME_NEW_FM:
						{
							#if 1//直接打开
								gscene_hbar_set_state(HBAR_ST_HIDE);
								__app_root_change_to_default_bg();
								__wrn("\n********************home switch to new fm ***************\n");
								root_para->root_type = msg->dwReserved;//2，TF卡
								root_para->explr_root = msg->dwReserved;
								root_ctrl->h_app_new_fm = app_new_fm_manwin_create(root_para);//创建收音机manwin窗口
								app_root_cacheon();

								GUI_WinSetFocusChild(root_ctrl->h_app_new_fm);//按键消息信号发送到当前创建的manwin窗口
								app_root_cacheoff();
							#endif
						}
						break;

						default:
							break;
					}
				}
				break;

				default:
					break;
			}
		}
		break;
		
		case APP_NEWSETTING_ID://进入新添加的通用设置app的mainwin窗口id
		{
			__wrn("----APP_NEWSETTING_ID to new app IS-----\n");
			switch(HIWORD(msg->dwAddData1))
			{
				case NEW_SWITCH_TO_OTHER_APP:
				{
					__wrn("----SWITCH_TO_OTHER_APP-----\n");
					switch(msg->dwAddData2)
					{
						case NEW_SETTING_SW_TO_MAIN://直接返回到home界面
						{
							__wrn("**********new setting to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_setting);//删除这个manwin窗口
							root_ctrl->h_app_new_setting = NULL;
							app_root_cacheon();
							__app_home_restore(msg);//创建显示home应用界面manwin窗口
							app_root_cacheoff();
						}
						break;

						case NEW_SETTING_SW_TO_SETTING://新添加的app应用程序manwin窗口切换到通用设置app应用程序manwin窗口
						{
							__wrn("**********new setting to setting**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							GUI_ManWinDelete(root_ctrl->h_app_new_setting);	//删除当前新添加app应用程序的manwin窗口
							root_ctrl->h_app_new_setting = 0;
							app_root_cacheon();
							//__app_home_restore(msg);
							#if 1//跳转到通用设置app应用程序的manwin窗口
								__app_root_change_to_default_bg();
								__msg("**********enter setting**********\n");
								root_ctrl->root_para->setting_opt = 0;
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_setting =  app_setting_create(root_para);//重新创建通用设置app应用程序的manwin窗口
								GUI_WinSetFocusChild(root_ctrl->h_app_setting);			  //按键命令消息发送，到通用设置的app应用程序的manwin窗口
							#endif
							app_root_cacheoff();
						}
						break;
						
						case SETTING_SW_TO_MULTIMEDIA:
						{}
						break;

						case SETTING_SW_TO_PROMPT_UPGRADE:
						{}
						break;

						case SETTING_SW_TO_PROMPT_DEFAULT:
						{}
						break;

						case EXPLR_SW_TO_UPGRADE:
						{}
						break;

						default:
							break;
					}

					//root_ctrl->h_app_setting = NULL;
				}
				break;

				default:
					break;
			}

			break;
		}
		break;
		case APP_NEWEXPLORER_ID://资源管理器
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case NEW_SWITCH_TO_OTHER_APP:
				{
					__wrn("----SWITCH_TO_OTHER_APP-----\n");
					switch(msg->dwAddData2)
					{
						case NEW_SETTING_SW_TO_MAIN://直接返回到home界面
						{
							__wrn("**********new explorer to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_explorer);//删除这个manwin窗口
							root_ctrl->h_app_new_explorer = NULL;
							app_root_cacheon();
							__app_home_restore(msg);//创建显示home应用界面manwin窗口
							app_root_cacheoff();
						}
						break;

						case NEW_SETTING_SW_TO_SETTING://新添加的app应用程序manwin窗口切换到通用设置app应用程序manwin窗口
						{
							__wrn("**********new explorer to setting**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							GUI_ManWinDelete(root_ctrl->h_app_new_explorer);	//删除当前新添加app应用程序的manwin窗口
							root_ctrl->h_app_new_explorer = NULL;
							app_root_cacheon();
							//__app_home_restore(msg);
							#if 1//跳转到通用设置app应用程序的manwin窗口
								__app_root_change_to_default_bg();
								__msg("**********enter setting**********\n");
								root_ctrl->root_para->setting_opt = 0;
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_setting =  app_setting_create(root_para);//重新创建通用设置app应用程序的manwin窗口
								GUI_WinSetFocusChild(root_ctrl->h_app_setting);			  //按键命令消息发送，到通用设置的app应用程序的manwin窗口
							#endif
							app_root_cacheoff();
						}
						break;

						case NEW_EXPLORER_SW_TO_NEW_MOVIE:{/*跳转到new movie视频应用manwin窗口*/
							__wrn("**********new movie to new movie**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							GUI_ManWinDelete(root_ctrl->h_app_new_explorer);	//删除当前新添加app应用程序的manwin窗口
							root_ctrl->h_app_new_explorer = NULL;
							__wrn("play file index=%d\n", msg->dwReserved);//播放文件的索引id
							root_para->root_type = RAT_TF;//msg->dwReserved;//RAT_TF;
							root_para->data = msg->dwReserved;//播放文件的索引id赋值给数据变量储存
							app_root_cacheon();
							#if 1//跳转到new explorer的app应用程序的manwin窗口
								__app_root_change_to_default_bg();
								__msg("**********enter new movie**********\n");
								root_ctrl->h_app_new_movie =  app_new_movie_manwin_create(root_para);//重新创建new movie视频app应用程序的manwin窗口
								GUI_WinSetFocusChild(root_ctrl->h_app_new_movie);			  //按键命令消息发送，到new movie视频的app应用程序的manwin窗口
							#endif
							app_root_cacheoff();

						}
						break;

						case NEW_EXPLORER_SW_TO_NEW_PHOTO://跳转到新添加的相册图片显示app
						{
							//删除后台音乐
							//__app_root_delete_bg_music(msg);
							__msg("**********explorer to new photo**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							GUI_ManWinDelete(root_ctrl->h_app_new_explorer);	//删除当前新添加app资源管理器应用程序的manwin窗口
							root_ctrl->h_app_new_explorer = NULL;
							__wrn("play photo file index=%d\n", msg->dwReserved);//播放图片文件的索引id
							app_root_cacheon();
							//root_para->root_type = msg->dwReserved;
							root_para->data = msg->dwReserved;//播放文件的索引id
							root_para->explr_root = RAT_TF;
							root_para->root_type = root_para->explr_root;
							#if 1//新添加的相册app
							root_ctrl->h_app_new_photo = app_new_photo_manwin_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_new_photo);
							#endif
							app_root_cacheoff();
						}
						break;

						case NEW_EXPLORER_SW_TO_NEW_EBOOK://跳转到新添加的电子书应用app
						{
							/*gscene_hbar_set_state(HBAR_ST_HIDE);
							__app_root_change_to_default_bg();
							__wrn("\n********************new explorer switch to new ebook ***************\n");
							root_para->root_type = RAT_TF;//msg->dwReserved;//2，TF卡
							root_para->explr_root = RAT_TF;//msg->dwReserved;
							root_para->data	=	ID_NEW_EXPLORER_EBOOK;//搜索电子书文件的id索引
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							root_ctrl->h_app_new_ebook = app_new_ebook_manwin_create(root_para);//创建电子书manwin窗口
							app_root_cacheon();

							GUI_WinSetFocusChild(root_ctrl->h_app_new_ebook);//按键消息信号发送到当前创建的manwin窗口
							app_root_cacheoff();*/

							__msg("**********new explorer to new ebook**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							GUI_ManWinDelete(root_ctrl->h_app_new_explorer);	//删除当前新添加app资源管理器应用程序的manwin窗口
							root_ctrl->h_app_new_explorer = NULL;
							__wrn("play ebook file index=%d\n", msg->dwReserved);//播放图片文件的索引id
							app_root_cacheon();
							//root_para->root_type = msg->dwReserved;
							//root_para->data = 0;
							root_para->data = msg->dwReserved;//播放文件的索引id
							root_para->explr_root = RAT_TF;
							root_para->root_type = root_para->explr_root;
							root_ctrl->h_app_new_ebook = app_new_ebook_manwin_create(root_para);//创建新的电子书manwin窗口
							__here__;
							GUI_WinSetFocusChild(root_ctrl->h_app_new_ebook);
							app_root_cacheoff();
							__here__;
						}
						break;
						
						case SETTING_SW_TO_MULTIMEDIA:
						{}
						break;

						case SETTING_SW_TO_PROMPT_UPGRADE:
						{}
						break;

						case SETTING_SW_TO_PROMPT_DEFAULT:
						{}
						break;

						case EXPLR_SW_TO_UPGRADE:
						{}
						break;

						default:
							break;
					}

					//root_ctrl->h_app_setting = NULL;
				}
				break;

				default:
					break;
			}

			break;
		}
		break;
		case APP_NEWMOVIE_ID:/*新添加的movie视频app应用程序manwin窗口id*/
		{
			switch(HIWORD(msg->dwAddData1)){
				case NEW_SWITCH_TO_OTHER_APP:{
					switch(msg->dwAddData2){
						case SETTING_SW_TO_MAIN:{							/*跳转到home应用manwin窗口创建*/
							__wrn("**********new movie to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_movie);	/*删除新添加的movie视频应用manwin窗口*/
							root_ctrl->h_app_new_movie = NULL;
							__here__;
							app_root_cacheon();
							__app_home_restore(msg);
							app_root_cacheoff();
							break;
						}
						
						
						case NEW_MOVIE_SW_TO_NEW_EXPLORER:{					/*跳转到new explorer应用manwin窗口*/
							#if 0
							__wrn("**********new movie to new explorer**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);			/*设置为隐藏*/
							GUI_ManWinDelete(root_ctrl->h_app_new_movie);	/*删除当前新添加app应用程序的manwin窗口*/
							root_ctrl->h_app_new_movie = NULL;

							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;			/*RAT_TF*/
							root_para->data = ID_NEW_EXPLORER_MOVIE;		/*单独显示视频文件内容数据，一起存放到结构体root_para的data里面*/
							root_para->return_to_explorer_file_list = 0;	/*列表模式*/
							app_root_cacheon();
							#if 1//跳转到new explorer的app应用程序的manwin窗口
								__app_root_change_to_default_bg();
								__msg("**********enter new explorer**********\n");
								root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);	/*重新创建new explorer资源管理器app应用程序的manwin窗口*/
								GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);			  			/*按键命令消息发送，到new explorer资源管理器的app应用程序的manwin窗口*/
							#endif
							app_root_cacheoff();
							#endif
							
							#if 1
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							__wrn("**********new movie to new explorer**********\n");
							__app_root_change_to_default_bg();
							GUI_ManWinDelete(root_ctrl->h_app_new_movie);	/*删除当前新添加app应用程序的manwin窗口*/
							root_ctrl->h_app_new_movie = NULL;
							__wrn("********** new explorer create is...**********\n");
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF;
							root_para->explr_root = msg->dwReserved;
							
							root_para->data = ID_NEW_EXPLORER_MOVIE;	//单独显示视频文件内容数据，一起存放到结构体root_para的data里面
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							{
								app_root_cacheon();
								root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);//创建new explorer 应用的manwin窗口
								__wrn("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_new_explorer);
								GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);//按键消息信号发送到当前创建的manwin窗口
								app_root_cacheoff();
								__wrn("**********enter new explorer create ok  **********\n");
								__wrn("ID_HOME_NEW_EXPORER msg->dwAddData2 =%d\n", msg->dwAddData2);
								break;
							}
							#endif

							
						}
						break;
						
						default:/*否则直接退出当前switch语句*/
							break;
					}
					
				}



			}

		}
		break;
		case APP_NEWMUSIC_ID:{/*新添加的music音乐应用manwin窗口id*/
			switch(HIWORD(msg->dwAddData1)){
				case NEW_SWITCH_TO_OTHER_APP:{
					switch(msg->dwAddData2){
						case NEW_SETTING_SW_TO_MAIN:{							/*跳转到home应用manwin窗口创建*/
							__wrn("**********new music to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_music);	/*删除新添加的music音乐应用manwin窗口*/
							root_ctrl->h_app_new_music = NULL;
							__here__;
							app_root_cacheon();
							__app_home_restore(msg);						/*重新创建home应用的manwin窗口*/
							app_root_cacheoff();
							break;
						}
					}
				}
			}
		}
		break;
		case APP_NEWPHOTO_ID://新添加的相册app应用程序manwin窗口
		{
			switch(HIWORD(msg->dwAddData1)){
				case NEW_SWITCH_TO_OTHER_APP:{
					switch(msg->dwAddData2){
						case NEW_SETTING_SW_TO_MAIN:{
							__wrn("**********new photo to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_photo);	/*删除新添加的photo相册应用manwin窗口*/
							root_ctrl->h_app_new_photo = NULL;
							__here__;
							app_root_cacheon();
							__app_home_restore(msg);						/*重新创建home应用的manwin窗口*/
							app_root_cacheoff();
							break;
						}
						break;

						case NEW_MOVIE_SW_TO_NEW_EXPLORER:{					/*跳转到new explorer应用manwin窗口*/
							
							#if 1
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							__wrn("**********new photo to new explorer**********\n");
							__app_root_change_to_default_bg();
							GUI_ManWinDelete(root_ctrl->h_app_new_photo);	/*删除当前新添加photo相册app应用程序的manwin窗口*/
							root_ctrl->h_app_new_photo = NULL;
							__wrn("********** new explorer create is...**********\n");
							__wrn("root type=%d\n", msg->dwReserved);
							root_para->root_type = msg->dwReserved;//RAT_TF，2;
							root_para->explr_root = msg->dwReserved;
							
							root_para->data = ID_NEW_EXPLORER_PHOTO;	//单独显示相册文件内容数据，一起存放到结构体root_para的data里面
							root_para->return_to_explorer_file_list = 0 ;//列表模式
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							{
								app_root_cacheon();
								root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);//创建new explorer 应用的manwin窗口
								__wrn("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_new_explorer);
								GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);//按键消息信号发送到当前创建的manwin窗口
								app_root_cacheoff();
								__wrn("**********enter new explorer create ok  **********\n");
								__wrn("ID_HOME_NEW_EXPORER msg->dwAddData2 =%d\n", msg->dwAddData2);
								break;
							}
							#endif

							
						}
						break;

						default:
							break;
					}
				}
				break;
				
			}
		}
		break;
		case APP_NEWEBOOK_ID://新添加的电子书app应用程序manwin窗口
		{
			switch(HIWORD(msg->dwAddData1)){
				case NEW_SWITCH_TO_OTHER_APP:{
					switch(msg->dwAddData2){
						case NEW_SETTING_SW_TO_MAIN:{
							__wrn("**********new ebook to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_ebook);	/*删除新添加的photo相册应用manwin窗口*/
							root_ctrl->h_app_new_ebook = NULL;
							__here__;
							app_root_cacheon();
							__app_home_restore(msg);						/*重新创建home应用的manwin窗口*/
							app_root_cacheoff();
							break;
						}
						break;

						case NEW_EBOOK_SW_TO_NEW_EXPLORER:{					/*跳转到new explorer应用manwin窗口*/
						#if 1
						gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
						__wrn("**********new ebook to new explorer**********\n");
						__app_root_change_to_default_bg();
						GUI_ManWinDelete(root_ctrl->h_app_new_ebook);	/*删除当前新添加photo相册app应用程序的manwin窗口*/
						root_ctrl->h_app_new_ebook = NULL;
						__wrn("********** new explorer create is...**********\n");
						__wrn("root type=%d\n", msg->dwReserved);
						root_para->root_type = msg->dwReserved;//RAT_TF，2;
						root_para->explr_root = msg->dwReserved;
						
						root_para->data = ID_NEW_EXPLORER_EBOOK;	//单独显示电子书文件内容数据，一起存放到结构体root_para的data里面
						root_para->return_to_explorer_file_list = 0 ;//列表模式
						{
							app_root_cacheon();
							root_ctrl->h_app_new_explorer =  app_new_explorer_manwin_create(root_para);//创建new explorer 应用的manwin窗口
							__wrn("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_new_explorer);
							GUI_WinSetFocusChild(root_ctrl->h_app_new_explorer);//按键消息信号发送到当前创建的manwin窗口
							app_root_cacheoff();
							__wrn("**********enter new explorer create ok  **********\n");
							__wrn("ID_HOME_NEW_EXPORER msg->dwAddData2 =%d\n", msg->dwAddData2);
							break;
						}
						#endif
						}
						break;

						case SWITCH_TO_EXPLORER://跳转到资源管理器列表
						{
							__s32	ret = 0 ;
							__wrn("**********new ebook to explorer**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_ebook);
							root_ctrl->h_app_new_ebook = 0;
							__wrn("**********explorer create is...**********\n");
							root_para->root_type = root_para->explr_root;
							__wrn("root_para->root_type=%d\n", root_para->root_type);
							root_para->data = ID_EXPLORER_EBOOK;
							app_root_cacheon();
							ret =  app_explorer_create(root_para);
							app_root_cacheoff();
							app_root_cacheon();

							if(EPDK_FAIL == ret)
							{
								__msg("create explorer fail , home restore\n") ;
								__app_home_restore(msg);
							}
							else
							{
								root_ctrl->h_app_explorer = ret ;
								GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
							}

							app_root_cacheoff();
							break;
						}
					}
				}
			}
				
		}
		break;
		case APP_NEWFM_ID:
		{
			switch(HIWORD(msg->dwAddData1)){
				case NEW_SWITCH_TO_OTHER_APP:{
					switch(msg->dwAddData2){
						case NEW_SETTING_SW_TO_MAIN:{
							__wrn("**********new fm to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_fm);	/*删除新添加的fm收音机应用manwin窗口*/
							root_ctrl->h_app_new_fm = NULL;
							__here__;
							app_root_cacheon();
							__app_home_restore(msg);						/*重新创建home应用的manwin窗口*/
							app_root_cacheoff();
							break;
						}
						break;
					}
				}
			}
		}
		break;
		
		case APP_EXPLORER_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_EXPLORER:
				{
					app_root__explorer_switch_disk(root_ctrl);
					break;
				}

				case SWITCH_TO_OTHER_APP:
				{
					switch(msg->dwAddData2)
					{
						case EXPLR_SW_TO_MAIN:
						{
							if(root_ctrl->h_app_explorer)
							{
								GUI_ManWinDelete(root_ctrl->h_app_explorer);
								root_ctrl->h_app_explorer = 0;
							}

							app_root_cacheon();
							__app_home_restore(msg);
							app_root_cacheoff();
						}
						break;

						case EXPLR_SW_TO_MOVIE:
						{
						#if 1//原来的movie应用程序
							//删除后台linein
							__app_root_delete_bg_linein(msg);
							//删除后台音乐
							__app_root_delete_bg_music(msg);
							__msg("**********explorer to movie**********\n");
							app_root_cacheon();
							//root_para->root_type = msg->dwReserved;
#if SP_FavoriteFiles

							if(msg->dwReserved == 0)
							{
								root_para->enter_media_type[m_eMediaTypeList_video] = RAT_MEDIA_TYPE_VIDEO;
							}
							else if(msg->dwReserved == 1)
							{
								root_para->enter_media_type[m_eMediaTypeList_video] = RAT_MEDIA_TYPE_FAVOR_VIDEO;
							}

#endif
							root_para->data = 0;
							root_para->tv_mode = MOVIE_MODE_NORMAL;
							root_para->root_type = root_para->explr_root  ;
							root_ctrl->h_app_movie = app_movie_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_movie);
							app_root_cacheoff();
#if SP_FavoriteFiles
							root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
#endif
						#endif
						#if 0//进入新的movie应用程序
							//删除后台linein
							__app_root_delete_bg_linein(msg);
							//删除后台音乐
							__app_root_delete_bg_music(msg);
							__wrn("**********explorer to new movie**********\n");
							app_root_cacheon();
							

							root_para->data = 0;
							root_para->tv_mode = MOVIE_MODE_NORMAL;
							root_para->root_type = root_para->explr_root  ;
							root_ctrl->h_app_new_movie = app_new_movie_manwin_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_new_movie);
							app_root_cacheoff();
#if SP_FavoriteFiles
							root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
#endif
						#endif
						}
						break;

						case EXPLR_SW_TO_MUSIC:
						{
							{
								__s32 ret;
								__s32 exist;
								H_WIN hmusic;
								//删除后台linein
								__app_root_delete_bg_linein(msg);

								//                                __app_root_change_to_default_bg();

								if(root_para->music_file_name[0] == '\0')
								{
									__msg("\n\n\n       %s\n", root_para->music_file_name);
									root_para->explorer_play_music = 0;
								}
								else
								{
									__msg("\n\n\n       %s\n", root_para->music_file_name);
									root_para->explorer_play_music = 1;
								}

								root_para->data = MUSICPLAYER_NORMAL;
								root_para->root_type = root_para->explr_root;
#if SP_FavoriteFiles
								root_para->enter_media_type[m_eMediaTypeList_audio] = RAT_MEDIA_TYPE_AUDIO;//在文件管理里始终会转到正常列表里
#endif
								exist = __app_root_is_bg_music_exist(msg, &hmusic);

								if(1 == exist)//重建后台音乐
								{
									__u32	last_music_index;
									char	last_music_path[768];
									__u32	last_music_start_index;
									reg_root_para_t *rootpara = NULL;
									rootpara = dsk_reg_get_para_by_app(REG_APP_ROOT);

									if(root_para->root_type == RAT_TF)
									{
										last_music_start_index  = rootpara->last_music_start_index_sd;
										last_music_index 		= rootpara->last_music_index_sd;		//上一次播放的序列号
										eLIBs_strcpy(last_music_path, rootpara->last_music_path_sd);
									}
									else if(root_para->root_type == RAT_USB)
									{
										last_music_start_index  = rootpara->last_music_start_index_ud;
										last_music_index 		= rootpara->last_music_index_ud;		//上一次播放的序列号
										eLIBs_strcpy(last_music_path, rootpara->last_music_path_ud);
									}

									__app_root_delete_bg_music(msg);

									if(root_para->root_type == RAT_TF)
									{
										rootpara->last_music_start_index_sd  = last_music_start_index;
										rootpara->last_music_index_sd = last_music_index;		//上一次播放的序列号
										eLIBs_strcpy(rootpara->last_music_path_sd, last_music_path);
									}
									else if(root_para->root_type == RAT_USB)
									{
										rootpara->last_music_start_index_ud  = last_music_start_index;
										rootpara->last_music_index_ud = last_music_index ;		//上一次播放的序列号
										eLIBs_strcpy(rootpara->last_music_path_ud, last_music_path);
									}
								}

								{
									root_ctrl_t   *root_ctrl;
									root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

									if(root_ctrl)
									{
										__app_root_delete_all_app(root_ctrl);
									}
								}

								app_root_cacheon();
								root_ctrl->h_app_music =  app_music_create(root_para);
								GUI_WinSetFocusChild(root_ctrl->h_app_music);
								app_root_cacheoff();
							}
#if SP_FavoriteFiles
							root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
#endif
						}
						break;
#if SP_FavoriteFiles

						case EXPLR_SW_TO_FAVOR_EXPLR:
						{
							__s32 ret;

							if(root_ctrl->h_app_explorer)
							{
								GUI_ManWinDelete(root_ctrl->h_app_explorer);
								root_ctrl->h_app_explorer = 0;
							}

							root_para->root_type = root_para->explr_root;

							if(msg->dwReserved == 1)
							{
								root_para->data = ID_EXPLORER_MOVIEFAVOR;
							}
							else
							{
								root_para->data = ID_EXPLORER_MOVIE;
							}

							root_para->return_to_explorer_file_list = 0 ;
							app_root_cacheon();
							root_ctrl->h_app_explorer =  app_explorer_create(root_para);
							app_root_cacheoff();

							if(EPDK_FAIL == root_ctrl->h_app_explorer)
							{
								__msg("create explorer fail , home restore\n") ;
								__app_home_restore(msg);
							}
							else
							{
								//root_ctrl->h_app_explorer = ret ;
								GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
							}
						}
						break;
#endif

						case EXPLR_SW_TO_PHOTO:
						{
							//删除后台音乐
							//__app_root_delete_bg_music(msg);
							__msg("**********explorer to photo**********\n");
							app_root_cacheon();
							//root_para->root_type = msg->dwReserved;
							root_para->data = 0;
							root_para->root_type = root_para->explr_root  ;
							#if 0//旧的
							root_ctrl->h_app_photo = app_photo_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_photo);
							#else//新添加的相册app
							root_ctrl->h_app_new_photo = app_new_photo_manwin_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_new_photo);
							#endif
							app_root_cacheoff();
#if SP_FavoriteFiles
							root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
#endif
						}
						break;

						case EXPLR_SW_TO_EBOOK:
						{
							#if 1
							//删除后台音乐
							//__app_root_delete_bg_music(msg);
							__msg("**********explorer to ebook**********\n");
							app_root_cacheon();
							//root_para->root_type = msg->dwReserved;
							root_para->data = 0;
							root_para->root_type = root_para->explr_root  ;
							root_ctrl->h_app_ebook = app_ebook_create(root_para);
							__here__;
							GUI_WinSetFocusChild(root_ctrl->h_app_ebook);
							app_root_cacheoff();
							__here__;
							#else//新添加的电子书app应用
							//删除后台音乐
							//__app_root_delete_bg_music(msg);
							__msg("**********explorer to new ebook**********\n");
							app_root_cacheon();
							//root_para->root_type = msg->dwReserved;
							//root_para->data = 0;
							root_para->data = -1;
							root_para->root_type = root_para->explr_root;
							root_ctrl->h_app_new_ebook = app_new_ebook_manwin_create(root_para);//创建新的电子书manwin窗口
							__here__;
							GUI_WinSetFocusChild(root_ctrl->h_app_new_ebook);
							app_root_cacheoff();
							__here__;
							#endif
#if SP_FavoriteFiles
							root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
#endif
						}
						break;

						default:
						{
							__err("media type err...\n");
						}
						break;
					}

#if !SP_FavoriteFiles
					root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
#endif
				}
				break;

				default:
					break;
			}
		}
		break;

		case APP_MOVIE_ID:
		{
			__s32	ret = 0 ;
			__wrn("**********movie to explorer**********\n");
			__wrn("msg->dwAddData1=%d\n", msg->dwAddData1);
			__wrn("msg->dwAddData2=%d\n", msg->dwAddData2);
			__wrn("msg->dwReserved=%d\n", msg->dwReserved);

			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_EXPLORER:
				{
					GUI_ManWinDelete(root_ctrl->h_app_movie);
					root_ctrl->h_app_movie = 0;
					root_para->root_type = root_para->explr_root;
					__wrn("root_para->root_type=%d\n", root_para->root_type);
					root_para->data = ID_EXPLORER_MOVIE;
					app_root_cacheon();
					ret =  app_explorer_create(root_para);
					app_root_cacheoff();
					app_root_cacheon();

					if(EPDK_FAIL == ret)
					{
						__msg("create explorer fail , home restore\n") ;
						__app_home_restore(msg);
					}
					else
					{
						root_ctrl->h_app_explorer = ret ;
						GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
					}

					app_root_cacheoff();
					break;
				}

				case SWITCH_TO_MMENU:
				{
					GUI_ManWinDelete(root_ctrl->h_app_movie);
					root_ctrl->h_app_movie = 0;
					__here__;
					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

#if SP_FavoriteFiles

				case SWITCH_TO_FAVORPLAY:
				{
					GUI_ManWinDelete(root_ctrl->h_app_movie);
					root_ctrl->h_app_movie = 0;
					root_para->root_type = root_para->explr_root;
					__wrn("root_para->root_type=%d\n", root_para->root_type);
					root_para->data = ID_EXPLORER_MOVIEFAVOR;
					app_root_cacheon();
					ret =  app_explorer_create(root_para);
					app_root_cacheoff();
					app_root_cacheon();

					if(EPDK_FAIL == ret)
					{
						__msg("create explorer fail , home restore\n") ;
						__app_home_restore(msg);
					}
					else
					{
						root_ctrl->h_app_explorer = ret ;
						GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
					}

					app_root_cacheoff();
				}
				break;
#endif

				default:
				{
					break;
				}
			}

			break;
		}

		case APP_MUSIC_ID:
		{
			__msg("**********receive msg from music,msg->dwAddData2=%d**********\n", msg->dwAddData2);

			switch(msg->dwAddData2)
			{
				case SWITCH_TO_MMENU:
				{
					if(MUSICPLAYER_BACKGROUND == LOWORD(msg->dwReserved))//音乐后台，不删除音乐
					{
						__msg("music background...\n");
					}
					else
					{
						__msg("delete music...\n");
						GUI_ManWinDelete(root_ctrl->h_app_music);
						root_ctrl->h_app_music = 0;
					}

					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

#if SP_FavoriteFiles

				case SWITCH_TO_FAVORPLAY:
				{
					__s32 ret;
					__s32 exist;
					H_WIN hmusic;
					root_para->explorer_play_music = 0;
					root_para->root_type = msg->dwReserved;
					root_para->data = MUSICPLAYER_NORMAL;
					root_para->return_to_explorer_file_list = 0 ;

					if(music_play_status == RAT_MEDIA_TYPE_FAVOR_AUDIO)
					{
						root_para->enter_media_type[m_eMediaTypeList_audio] = RAT_MEDIA_TYPE_FAVOR_AUDIO;
					}
					else
					{
						root_para->enter_media_type[m_eMediaTypeList_audio] = RAT_MEDIA_TYPE_AUDIO;
					}

					exist = __app_root_is_bg_music_exist(msg, &hmusic);

					if(1 == exist)//重建后台音乐
					{
						ret = app_music_get_root_type(hmusic, &root_para->root_type);

						if(EPDK_FAIL == ret)
						{
							root_para->root_type = RAT_TF;
						}

						ret = app_music_restore(hmusic, root_para);

						if(EPDK_OK == ret)
						{
							if(root_ctrl->h_app_music != hmusic)
							{
								__msg("*******root_ctrl->h_app_music != hmusic********\n");
							}

							root_ctrl->h_app_music = hmusic;
						}
						else
						{
							__msg("*****app_music_restore fail*******\n");
							__app_root_delete_bg_music(msg);
							__here__;
							root_ctrl->h_app_music =  app_music_create(root_para);
							__here__;
						}
					}

					GUI_WinSetFocusChild(root_ctrl->h_app_music);
				}
				break;
#endif

				case SWITCH_TO_EXPLORER:
				{
					__s32	ret = 0 ;
					__msg("**********photo to explorer**********\n");

					//                   gscene_bgd_set_flag(EPDK_TRUE);
					//                    gscene_bgd_refresh();

					if(MUSICPLAYER_BACKGROUND == msg->dwReserved)//音乐后台，不删除音乐
					{
						__msg("music background...\n");
					}
					else
					{
						__msg("delete music...\n");
						GUI_ManWinDelete(root_ctrl->h_app_music);
						root_ctrl->h_app_music = 0;
					}

					__here__;
					app_root_cacheon();
					root_para->root_type = root_para->explr_root;
					root_para->data = ID_EXPLORER_MUSIC;
					//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
					ret =  app_explorer_create(root_para);
					app_root_cacheoff();
					app_root_cacheon();

					if(EPDK_FAIL == ret)
					{
						__msg("create explorer fail , home restore\n") ;
						__app_home_restore(msg);
					}
					else
					{
						root_ctrl->h_app_explorer =  ret;
						GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
					}

					app_root_cacheoff();
					break;
				}

				case CLOSE_BG_MUSIC:
				{
					__msg("delete background music...\n");

					if(root_ctrl->h_app_music)
					{
						GUI_ManWinDelete(root_ctrl->h_app_music);
						root_ctrl->h_app_music = 0;
					}

					break;
				}

				case SWITCH_DISK:
				{
					__s32 ret;
					__u32 cur_root_type;
					__u32 reg_root_type;
					rat_root_t rat_root_type;

					if(NULL == root_ctrl->h_app_music)
					{
						__wrn("music is not running...\n");
						break;
					}

					ret = app_music_get_root_type(root_ctrl->h_app_music, &cur_root_type);

					if(EPDK_FAIL == ret)
					{
						__wrn("get cur music root type fail...\n");
						break;
					}
					else
					{
						__wrn("get cur music root type success...\n");
					}

					reg_root_type = msg->dwReserved;
					rat_root_type = __app_root_get_rat_root_type(reg_root_type);

					if(RAT_UNKNOWN == rat_root_type)
					{
						__wrn("get music rat root type fail...\n");
						break;
					}
					else
					{
						__wrn("get music rat root type success...\n");
					}

					if(rat_root_type == cur_root_type)
					{
						__wrn("root type is cur root type, not need switch...\n");
						break;
					}

					{
						//begin switch storage to reg_root_type
						__s32 ret;
						__s32 reg_storage_type;
						__s32 reg_media_type;
						reg_media_type = m_eMediaTypeList_audio;
						reg_storage_type = reg_root_type;
						ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_switch_storage, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_TRUE);

						if(EPDK_FAIL == ret)
						{
							__wrn("try to switch storage fail...\n");
						}
						else
						{
							__wrn("try to switch storage success...\n");
						}
					}

					break;
				}

				default:
				{
					break;
				}
			}
		}
		break;

		case APP_PHOTO_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_EXPLORER:
				{
					//删除后台音乐
					// __app_root_delete_bg_music(msg);
					__s32 ret = 0 ;
					__msg("**********photo to explorer**********\n");
					GUI_ManWinDelete(root_ctrl->h_app_photo);
					root_ctrl->h_app_photo = 0;
					//                   gscene_bgd_set_flag(EPDK_TRUE);
					//                    gscene_bgd_refresh();
					__here__;
					app_root_cacheon();
					root_para->root_type = root_para->explr_root;
					root_para->data = ID_EXPLORER_PHOTO;
					//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
					ret =  app_explorer_create(root_para);
					app_root_cacheoff();
					app_root_cacheon();

					if(EPDK_FAIL == ret)
					{
						__msg("create explorer fail , home restore\n") ;
						__app_home_restore(msg);
					}
					else
					{
						root_ctrl->h_app_explorer = ret ;
						GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
					}

					app_root_cacheoff();
					break;
				}

				case OPEN_BG_MUSIC:
				{
					__app_root_delete_bg_linein(msg);
					__app_root_create_bg_music(msg);
					break;
				}

				case CLOSE_BG_MUSIC:
				{
					__app_root_delete_bg_music(msg);
					break;
				}

				case SWITCH_TO_MMENU:
				{
					GUI_ManWinDelete(root_ctrl->h_app_photo);
					root_ctrl->h_app_photo = 0;
					__here__;
					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

				default:
				{
					break;
				}
			}

			break;
		}

		case APP_EBOOK_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_EXPLORER:
				{
					//删除后台音乐
					//__app_root_delete_bg_music(msg);
					__s32 ret ;
					__msg("**********ebook to explorer**********\n");
					GUI_ManWinDelete(root_ctrl->h_app_ebook);
					root_ctrl->h_app_ebook = 0;
					//                   gscene_bgd_set_flag(EPDK_TRUE);
					//                   gscene_bgd_refresh();
					app_root_cacheon();
					root_para->root_type = root_para->explr_root;
					root_para->data = ID_EXPLORER_EBOOK;
					ret =  app_explorer_create(root_para);
					app_root_cacheoff();
					app_root_cacheon();

					if(EPDK_FAIL == ret)
					{
						__msg("create explorer fail , home restore\n") ;
						__app_home_restore(msg);
					}
					else
					{
						root_ctrl->h_app_explorer = ret ;
						GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
					}

					app_root_cacheoff();
					break;
				}

				case OPEN_BG_MUSIC:
				{
					__app_root_delete_bg_linein(msg);
					__app_root_create_bg_music(msg);
					break;
				}

				case CLOSE_BG_MUSIC:
				{
					__app_root_delete_bg_music(msg);
					break;
				}

				case CLOSE_AUDIO_APP:
				{
					__app_root_delete_all_audio_app(root_ctrl);
					break;
				}

				case SWITCH_TO_MMENU:
				{
					GUI_ManWinDelete(root_ctrl->h_app_ebook);
					root_ctrl->h_app_ebook = 0;
					__here__;
					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

				default:
				{
					break;
				}
			}

			break;
		}

		case APP_FM_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_MMENU:
				{
					__msg("**********fm to home**********\n");
					GUI_ManWinDelete(root_ctrl->h_app_fm);
					root_ctrl->h_app_fm = 0;
					app_root_cacheon();
					__app_home_restore(msg);
					{
						__set_keytone_t state;
						dsk_keytone_get_state(&state);
						dsk_keytone_exit();
						dsk_keytone_init(BEETLES_APP_ROOT"res\\sounds\\chord.wav");
						dsk_keytone_set_state(state);
					}
					app_root_cacheoff();
					break;
				}

				case SWITCH_TO_OTHER_APP:
				{
					switch(LOWORD(msg->dwAddData2))
					{
						case FM_SW_TO_RECORD:
						{
							__here__;
							GUI_ManWinDelete(root_ctrl->h_app_fm);
							root_ctrl->h_app_fm = 0;
							app_root_cacheon();
							__wrn("**********FM_SW_TO_RECORD**********\n");
							root_ctrl->root_para->root_type = HIWORD(msg->dwAddData2);//RAT_USB或者RAT_TF
							root_ctrl->root_para->record_opt = 1;//fm录音
#if (SP_Software_Echo)
							{
								// 关闭混响功能
								dsk_audio_echo_close();
								{
									ES_FILE *t_fp_rec = NULL;
									t_fp_rec = eLIBs_fopen("b:\\AUDIO\\REC", "r+");
									eLIBs_fioctrl(t_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO_OUTPUT, 0, 0);
									eLIBs_fclose(t_fp_rec);
								}
							}
#endif
							root_ctrl->h_app_record =  app_record_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_record);
							if(0)
							{
								__set_keytone_t state;
								dsk_keytone_get_state(&state);
								dsk_keytone_exit();
								dsk_keytone_init(BEETLES_APP_ROOT"res\\sounds\\chord.wav");
								dsk_keytone_set_state(state);
							}
							app_root_cacheoff();
							break;
						}

						default:
						{
							break;
						}
					}
				}

				default:
				{
					break;
				}
			}

			break;
		}

		case APP_RECORD_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_MMENU:
				{
					__msg("**********record to home**********\n");
					GUI_ManWinDelete(root_ctrl->h_app_record);
					root_ctrl->h_app_record = 0;
					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

				case SWITCH_TO_OTHER_APP:
				{
					switch(LOWORD(msg->dwAddData2))
					{
						case RECORD_SW_TO_FM:
						{
							__here__;
							//__app_root_change_to_default_bg();//这里不是通过主界面进入的，不记录进入ap之前背景的状态。
							GUI_ManWinDelete(root_ctrl->h_app_record);
							root_ctrl->h_app_record = 0;
							root_ctrl->root_para->record_opt = 0;
							IOCTRL__PA_MODE__SET_AB();
							app_root_cacheon();
							__msg("**********RECORD_SW_TO_FM**********\n");
							//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
							root_ctrl->h_app_fm = app_fm_create(root_para);
							GUI_WinSetFocusChild(root_ctrl->h_app_fm);
							app_root_cacheoff();
							break;
						}

						default:
						{
							break;
						}
					}
				}

				default:
				{
					break;
				}
			}

			break;
		}

		case APP_CALENDAR_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_MMENU:
				{
					__msg("**********calendar to home**********\n");
					GUI_ManWinDelete(root_ctrl->h_app_calendar);
					root_ctrl->h_app_calendar = 0;
					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

				default:
				{
					break;
				}
			}

			break;
		}
#if SP_APP_ATV
		case APP_ATV_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_MMENU:
				{
					__msg("**********calendar to home**********\n");
					GUI_ManWinDelete(root_ctrl->h_app_atv);
					root_ctrl->h_app_atv = 0;
					app_root_cacheon();
					__app_home_restore(msg);
					app_root_cacheoff();
					break;
				}

				case SWITCH_TO_LINE_IN:
				#if SP_APP_AUX
					__wrn("SWITCH_TO_LINE_IN\n");
					__app_root_to_linein(root_ctrl);
				#else
					__app_home_restore(msg);
				#endif
					break;

				case SWITCH_TO_ATV:
					__app_root_to_atv_ex(root_ctrl, msg->dwAddData2);
					break;

				default:
				{
					break;
				}
			}

			break;
		}
#endif
		case APP_SETTING_ID:
		{
			switch(HIWORD(msg->dwAddData1))
			{
				case SWITCH_TO_OTHER_APP:
				{
					switch(msg->dwAddData2)
					{
						#if 1//原来通用设置的manwin窗口界面删除
						case SETTING_SW_TO_MAIN:
						{
							__wrn("**********setting to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_setting);
							root_ctrl->h_app_setting = 0;
							app_root_cacheon();
							__app_home_restore(msg);
							app_root_cacheoff();
						}
						break;
						#else//新添加的manwin窗口界面关闭删除
						case SETTING_SW_TO_MAIN://直接返回到home界面
						{
							__wrn("**********APP_SETTING_ID new setting to home**********\n");
							GUI_ManWinDelete(root_ctrl->h_app_new_setting);//删除这个manwin窗口
							root_ctrl->h_app_new_setting = NULL;
							app_root_cacheon();
							__app_home_restore(msg);//创建显示home应用界面manwin窗口
							app_root_cacheoff();
						}
						break;
						#endif
						case SETTING_SW_TO_NEW_SETTING://通用设置app应用程序manwin窗口切换到新添加的app应用程序manwin窗口
						{
							__wrn("**********new setting to setting**********\n");
							gscene_hbar_set_state(HBAR_ST_HIDE);//设置为隐藏
							GUI_ManWinDelete(root_ctrl->h_app_setting);	//删除当前新添加app应用程序的manwin窗口
							root_ctrl->h_app_setting = 0;
							app_root_cacheon();
							//__app_home_restore(msg);
							#if 1//转到新添加的app应用程序manwin窗口
								__app_root_change_to_default_bg();
								__msg("**********enter setting**********\n");
								//root_ctrl->root_para->setting_opt = 0;
								//root_ctrl->root_para->h_parent= GUI_WinGetParent(root_ctrl->h_app_home);
								root_ctrl->h_app_new_setting =  app_New_Setting_manwin_create(root_para);//重新创建新添加的app应用程序的manwin窗口
								GUI_WinSetFocusChild(root_ctrl->h_app_new_setting);			  		     //按键命令消息发送，到新添加的app应用程序的manwin窗口
							#endif
							app_root_cacheoff();
						}
						break;

						case SETTING_SW_TO_MULTIMEDIA:
						{}
						break;

						case SETTING_SW_TO_PROMPT_UPGRADE:
						{}
						break;

						case SETTING_SW_TO_PROMPT_DEFAULT:
						{}
						break;

						case EXPLR_SW_TO_UPGRADE:
						{}
						break;

						default:
							break;
					}

					//root_ctrl->h_app_setting = NULL;
				}
				break;

				default:
					break;
			}

			break;
		}

#if SP_AutoUpdateFirmware

		case FW_UPDATE_DIALOG_ID :
		{
			__s32 ret = EPDK_FAIL;

			switch(HIWORD(msg->dwAddData1))
			{
				case ADLG_CMD_SET_UI :
				{
					char		rootDir = root_ctrl->root_para->fw_update_file_disk;

					if(DetectUpdateFirmware(&rootDir))
					{
						char t_strAbsolutePath_UpdateFirmware[] = PATH__AUTOEXE_LZ_C100_IMG;
						__wrn("&g_strAbsolutePath_UpdateFirmware[1]=%s\n", &g_strAbsolutePath_UpdateFirmware[1]);

						if(strcmp(&g_strAbsolutePath_UpdateFirmware[1], &t_strAbsolutePath_UpdateFirmware[1]) == 0)
						{
							// 快速升级的固件
							__wrn("fw update, g_strAbsolutePath_UpdateFirmware=%s\n", g_strAbsolutePath_UpdateFirmware);
							ret = dsk_start_fw_update(g_strAbsolutePath_UpdateFirmware);
						}
						else
						{
							__s32 str[] = {0, STRING_EXPLR_FW_UPDATE_CONFIRM};
							default_dialog(root_ctrl->root_para->h_fw_update_dlg, root_ctrl->root_para->h_parent,
							               FW_UPDATE_DIALOG_ID, ADLG_YESNO, str);

							if(root_ctrl->root_para->h_fw_update_dlg)
							{
								ret = EPDK_OK;
							}
						}
					}
				}
				break;

				case ADLG_CMD_EXIT:
				{
					if(root_para ->h_fw_update_dlg != NULL)
					{
						app_dialog_destroy(root_para->h_fw_update_dlg); 		//to delete dialog
						root_para->h_fw_update_dlg = NULL;

						if(msg->dwAddData2 == ADLG_IDYES)
						{
							// 再检测一遍，防止磁盘已移除
							char		rootDir = root_ctrl->root_para->fw_update_file_disk;

							if(DetectUpdateFirmware(&rootDir))
							{
								__wrn("fw update, g_strAbsolutePath_UpdateFirmware=%s\n", g_strAbsolutePath_UpdateFirmware);
								ret = dsk_start_fw_update(g_strAbsolutePath_UpdateFirmware);
							}
						}
						else//ADLG_IDNO	// 考虑自动播放媒体
						{
							__msg("fw not update...\n");
						}
					}
				}
				break;

				default:
					break;
			}

			if(ret == EPDK_FAIL)
			{
				__msg("fw not update...\n");
				app_root_update_firmware_giveup(root_ctrl);
			}
		}
		break;
#endif

		default:
			break;
	}

	return EPDK_OK;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ app root  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
__s32 app_root_win_proc(__gui_msg_t *msg)
{
	__msg("app_root_win_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			root_para_t *root_para;
			root_ctrl_t   *root_ctrl;
			//set_input_gpio(4, 15, 0);//选择D类功放
			root_para = esMEMS_Balloc(sizeof(root_para_t));

			if(!root_para)
			{
				return EPDK_FAIL;
			}

			eLIBs_memset(root_para, 0, sizeof(root_para_t));
			root_ctrl = esMEMS_Balloc(sizeof(root_ctrl_t));

			if(!root_ctrl)
			{
				esMEMS_Bfree(root_para, sizeof(root_para_t));
				return EPDK_FAIL;
			}

			dsk_keytone_set_key_type(KEYTONE_KEY_TYPE_DOWN);
			//            dsk_amplifier_onoff(0);//关闭功放
			eLIBs_memset(root_ctrl, 0, sizeof(root_ctrl_t));
			root_para->h_parent		= msg->h_deswin;
			root_para->font			= SWFFont;
			root_para->root_type	= 0;
#if SP_FavoriteFiles
			root_para->enter_media_type[m_eMediaTypeList_audio] = RAT_MEDIA_TYPE_AUDIO;
			root_para->enter_media_type[m_eMediaTypeList_video] = RAT_MEDIA_TYPE_VIDEO;
#endif
			root_ctrl->root_para = root_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)root_ctrl);
			{
				com_set_backcolor(CONST_BackColor);		//GUI_BLUE
				//gscene_bgd_set_flag(EPDK_TRUE);
				//gscene_bgd_refresh();
				GUI_LyrWinCacheOff();
				gscene_bgd_set_state(BGD_STATUS_SHOW);
#if SP_AutoUpdateFirmware
				root_ctrl->m_updateFirmwareLog = LOG_UpdateFirmware_By_RootCreate;

				if(!app_root_detect_update_firmware(root_ctrl))
#endif
				{
					__s32 ret = EPDK_FAIL;
					__s32 reg_storage_type;
					__s32 reg_media_type;
					reg_media_type = -1;
					reg_storage_type = -1;
					ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_first_play, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_TRUE);

					if(EPDK_FAIL == ret)
					{
						SEND_MSG(DSK_MSG_RELEASE_TIMER_DET_UDISK, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
						__wrn("try to first play fail, load home app...\n");
						root_ctrl->h_app_home	= app_home_create(root_para);
						__wrn("root_ctrl->h_app_home = %x\n", root_ctrl->h_app_home);
						GUI_WinSetFocusChild(root_ctrl->h_app_home);
					}
					else
					{
						__wrn("try to first play success, not need load home app...\n");
					}
				}
			}
			//GUI_LyrWinCacheOff();
			dsk_display_lcd_on();

		#if SP_AUX_DET
			root_ctrl->globe_check.globe_thread_id =  esKRNL_TCreate(app_root_globe_thread, (root_ctrl), 0x800, KRNL_priolevel5);

			if(root_ctrl->globe_check.globe_thread_id == NULL)
			{
				__msg("esKRNL_TCreate  globe_thread_id fail! \n");
			}
		#endif

			#if SP_DEV_DET_PLUGIN_BY_AD		
			{
				ES_FILE *pKeyFile;
				pKeyFile = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");
				
				if(!pKeyFile)
				{
					__wrn("key connot be openned!\n");
				}
				else
				{
					eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_DET_MIC_PLUG_EN, 0, 0);
					eLIBs_fclose(pKeyFile);
				}
			}
			#endif

				GUI_SetTimer(msg->h_deswin, AMPLIFER_CHECK_TIMMER_ID, AMPLIFER_CHECK_TIME, NULL);
	#if TIMEOUT_ID_TIMER_GET_ECHO_LEVEL
				GUI_SetTimer(msg->h_deswin, ID_TIMER_GET_ECHO_LEVEL, TIMEOUT_ID_TIMER_GET_ECHO_LEVEL, NULL);
	#endif
				root_ctrl->root_para->amplify_open_delay_cnt = 0;
			}

		return EPDK_OK;

		case GUI_MSG_TIMER:
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!root_ctrl)
			{
				__err("root_ctrl is null...\n");
				return EPDK_OK;
			}

			if(LOWORD(msg->dwAddData1) == AMPLIFER_CHECK_TIMMER_ID)
			{
				__s32 mic_in;
				__s32 earphone_in;
				__s32 playing;
				__s32 mute;
				__s32 sta;
				__s32 amplifer_onoff;
				__s32 is_fm_searching;
				__s32 btv_out;

				do
				{
					amplifer_onoff = 0;
					mic_in = 0;
					earphone_in = 0;
#if (SP_FixedMic)
					mic_in = 0;
#elif (SP_HotplugMic)
					{
						if(IOCTRL__MIC_DET__IS_PLUGIN())
						{
							mic_in = 1;
						}
						else
						{
							mic_in = 0;
						}

#if SP_Software_Echo
						{
							if(!root_ctrl->root_para->record_opt) // 正常进入录音
							{
								if(mic_in)
								{
									if(!dsk_audio_echo_is_open())
									{
										if(app_record_is_recording())
										{
											// 此处只适用于正在录音时，话筒插入的情况。
											record_mic_param_set(1, root_ctrl->root_para->record_opt);
										}
										else
										{
											dsk_echo_level_init();
										}
									}
								}
								else
								{
									dsk_audio_echo_close();
								}
							}

							//__msg_jjl("dsk_echo_level_get(%d), mic_sta=%d\n", dsk_echo_level_get(), mic_in);
						}
#endif
					}
#endif
				#if 1
					if((root_ctrl->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
						!= OP_RootFlags_CVBS_InputMode_DVDFromHome)
				#endif
					{
						H_WIN winHdl = GUI_WinGetHandFromName("app_fm");
						if(//(!mic_in) &&
							(winHdl || root_ctrl->root_para->record_opt ||
							((root_ctrl->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) == OP_RootFlags_CVBS_InputMode_TVFromHome))
							)
						{
							IOCTRL__PA_MODE__SET_AB();
						}
						else
						{
							IOCTRL__PA_MODE__SET_D();
						}					
					}
					//mic_in = 1;//硬件mic自动检测，默认插入
					//__msg("mic_in=%d\n", mic_in);
					//耳机
					/*
					             sta = get_gpio_status(3, 10);
					             if(0 == sta)
					             {
					                 earphone_in = 1;
					             }
					             else
					             {
					                 earphone_in = 0;
					             }
					*/
					earphone_in = 0;//硬件耳机检测自动检测，默认没有插入
					//__msg("earphone_in=%d\n", earphone_in);

					//__msg("root_ctrl->h_app_movie=0x%x\n", root_ctrl->h_app_movie);
					if(root_ctrl->h_app_movie)
					{
						btv_out = app_movie_is_tv_out(root_ctrl->h_app_movie);
					}
					if(root_ctrl->h_app_new_movie)//新添加的movie应用app
					{
						btv_out = app_movie_is_tv_out(root_ctrl->h_app_new_movie);
					}
					else
					{
						btv_out = 0;
					}

					if(1 == root_ctrl->sos_started)
					{
						amplifer_onoff = 1;
						break;
					}

					//__msg("btv_out=%d\n", btv_out);

					if(1 == btv_out)
					{
						amplifer_onoff = 0;
						break;
					}

					//mute
					mute = g_is_mute;

					//__msg("mute=%d\n", mute);

					if(1 == mute)
					{
						amplifer_onoff = 0;
						break;
					}

					if(1 == mic_in || root_ctrl->root_para->record_opt)
					{
						//amplifer_onoff = 1;
						amplifer_onoff = 2;
						break;
					}

					//playing
					playing = 0;

					if(0 == playing)
					{
						//__here__;
						if(root_ctrl->h_app_fm)
						{
							//__here__;
							is_fm_searching = app_fm_get_auto_search_state();

							if(1 == is_fm_searching)
							{
								amplifer_onoff = 0;
								break;
							}

							playing = 1;
						}
					}

				#if SP_APP_AUX
					if(0 == playing)
					{
						//__here__;
						if(root_ctrl->h_app_linin)
						{
							//__here__;
							playing = 1;
						}
					}
				#endif

					//__msg("playing=%d\n", playing);
					if(0 == playing)
					{
						//__msg("root_ctrl->sos_started=%d\n", root_ctrl->sos_started);
						if(EPDK_TRUE == root_ctrl->sos_started)
						{
							playing = 1;
						}
					}

					if(0 == playing)
					{
						//__here__;
						if(root_ctrl->h_app_movie)
						{
							__cedar_status_t sta;
							sta = robin_get_fsm_status();

							//__msg("sta=%d\n", sta);
							if(CEDAR_STAT_PLAY == sta)
							{
								__here__;
								playing = 1;
							}
						}
						else if(root_ctrl->h_app_new_movie)//新添加的movie应用app
						{
							__cedar_status_t sta;
							sta = robin_get_fsm_status();

							//__msg("sta=%d\n", sta);
							if(CEDAR_STAT_PLAY == sta)
							{
								__here__;
								playing = 1;
							}
						}
					}

					if(0 == playing)
					{
						//__here__;
						if(root_ctrl->h_app_record)
						{
							__cedar_status_t sta;
							sta = robin_get_fsm_status();
							__wrn("sta=%d\n", sta);

							if(CEDAR_STAT_PLAY == sta)
							{
								__here__;
								playing = 1;
							}
						}
					}

					if(0 == playing)
					{
						//__here__;
						if(root_ctrl->h_app_music)
						{
							__cedar_status_t sta;
							sta = robin_get_fsm_status();

							//__wrn("sta=%d\n", sta);
							if(CEDAR_STAT_PLAY == sta)
							{
								//__here__;
								playing = 1;
							}
						}
						else if(root_ctrl->h_app_new_music)//新添加的音乐应用
						{
							__cedar_status_t sta;
							sta = robin_get_fsm_status();//获取fsm状态机的播放状态

							//__wrn("sta=%d\n", sta);
							if(CEDAR_STAT_PLAY == sta)//播放状态
							{
								//__here__;
								playing = 1;//播放标志置1
							}
						}
					}

				#if SP_APP_ATV
					if(0 ==  playing)
					{
						if(root_ctrl->h_app_atv)
						{
							if((root_ctrl->root_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
								!= OP_RootFlags_CVBS_InputMode_DVDFromHome)
							{
								playing = 1;
							}
							else if(IOCTRL__DVD_STA__IS_PA_ON() && app_atv_enIoctrlDetPaForDVD())
							{
								playing = 1;
							}
						}
					}
				#endif

					//__msg("playing=%d\n", playing);
					//__msg("earphone_in=%d\n", earphone_in);
					if(1 == playing && 0 == earphone_in)//播放状态下
					{
						if(amplifer_onoff == 0)
						{
							amplifer_onoff = 1;//功放开关标志置1
						}

						break;
					}

					amplifer_onoff = 0;
					break;
				}
				while(0);

				//__msg("amplifer_onoff=%d\n", amplifer_onoff);
				//if(1 == amplifer_onoff)
				if(0 != amplifer_onoff)//功放打开
				{
					root_ctrl->root_para->amplify_open_delay_cnt++;

					if(root_ctrl->root_para->amplify_open_delay_cnt >= 2)
					{
						root_ctrl->root_para->amplify_open_delay_cnt = 2;
					}

					if(root_ctrl->root_para->amplify_open_delay_cnt >= 2)
					{
						dsk_amplifier_onoff(amplifer_onoff);
					}
				}
				else
				{
					dsk_amplifier_onoff(0);
					root_ctrl->root_para->amplify_open_delay_cnt = 0;
				}

				return EPDK_OK;
			}
			else if(LOWORD(msg->dwAddData1) == SOS_TIMER_ID)
			{
				//顺便处理SOS
				if(EPDK_TRUE == root_ctrl->sos_started)
				{
					__cedar_status_t fsm_sta;
					fsm_sta = robin_get_fsm_status();
					__msg("fsm_sta= %d\n", fsm_sta);

					if(CEDAR_STAT_PLAY != fsm_sta)
					{
						//robin_play_file_no_delay(g_sos_filename[0], NULL);
					}
				}
			}

#if TIMEOUT_ID_TIMER_GET_ECHO_LEVEL
			else if(LOWORD(msg->dwAddData1) == ID_TIMER_GET_ECHO_LEVEL)
			{
				app_root_echo_proc();
			}

#endif
			break;
		}

		case GUI_MSG_CLOSE:
		{
			GUI_ManWinDelete(msg->h_deswin);
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY:
		{
			root_para_t *root_para;
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!root_ctrl)
			{
				__msg("******err****\n");
				return EPDK_OK;
			}

			root_para = (root_para_t *)root_ctrl->root_para;

			if(!root_para)
			{
				__msg("******err****\n");
				return EPDK_OK;
			}

			dsk_amplifier_onoff(0);
			__com_volume_scene_delete(root_ctrl);
			__app_root_delete_sos(root_ctrl);

			if(GUI_IsTimerInstalled(msg->h_deswin, SOS_TIMER_ID))
			{
				GUI_KillTimer(msg->h_deswin, SOS_TIMER_ID);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, AMPLIFER_CHECK_TIMMER_ID))
			{
				GUI_KillTimer(msg->h_deswin, AMPLIFER_CHECK_TIMMER_ID);
			}

#if TIMEOUT_ID_TIMER_GET_ECHO_LEVEL

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_ECHO_LEVEL))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_GET_ECHO_LEVEL);
			}

#endif
#if SP_AutoUpdateFirmware

			if(root_ctrl->root_para->h_fw_update_dlg)
			{
				app_dialog_destroy(root_ctrl->root_para->h_fw_update_dlg);		//to delete dialog
				root_ctrl->root_para->h_fw_update_dlg = NULL;
			}

#endif

		#if SP_AUX_DET
			if(root_ctrl->globe_check.globe_thread_id != NULL)
			{
				while(esKRNL_TDelReq(root_ctrl->globe_check.globe_thread_id) != OS_TASK_NOT_EXIST)
				{
					esKRNL_TimeDly(5);
				}

				root_ctrl->globe_check.globe_thread_id = NULL;
				__msg("\n thread has been deleted.\n");
			}
		#endif

			esMEMS_Bfree(root_para, sizeof(root_para_t));
			esMEMS_Bfree(root_ctrl, sizeof(root_ctrl_t));
			__wrn("app root win destroy end...\n");
		}

		return EPDK_OK;

		case GUI_MSG_COMMAND:
		{
			app_root_command_proc(msg);
			return EPDK_OK;
		}

		case GUI_MSG_KEY:
		{
			__bool audio_engine_is_open;
			root_ctrl_t   *root_ctrl;
			__msg("app_root_win_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
			      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__msg("msg->dwReserved=%d\n", msg->dwReserved);

			if(!root_ctrl)
			{
				__msg("******err****\n");
				return EPDK_OK;
			}

#if SP_AutoUpdateFirmware

			if(root_ctrl->root_para->h_fw_update_dlg)
			{
				if(
				  (msg->dwAddData1 != GUI_MSG_KEY_LEFT) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_RIGHT) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_ENTER) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_MENU) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_LONGLEFT) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_LONGRIGHT) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_LONGENTER) &&
				  (msg->dwAddData1 != GUI_MSG_KEY_LONGMENU)
				)
				{
					return EPDK_OK;
				}
			}

#endif

			#if(0)//112350
			{
				if(KEY_UP_ACTION == msg->dwAddData2
				    && GUI_MSG_KEY_VADD == msg->dwAddData1)
				{
					static __u8 reg = 0x80;
					ES_FILE *pfile;
					pfile = eLIBs_fopen("b:\\HWSC\\RTC", "r+");

					if(NULL != pfile)
					{
						__wrn("dsk_misc_open_rtc_clk reg=0x%x...\n", reg);
						eLIBs_fioctrl(pfile, RTC_CMD_SETCLOCKOUT, reg, NULL);
						eLIBs_fclose(pfile);
						reg++;

						if(reg >= 0x84)
						{
							reg = 0x80;
						}
					}
				}
			}
			#endif

			#if(0)
			{
				__s32 ret;
				static __s32 mode = 0;

				if(KEY_UP_ACTION == msg->dwAddData2
				    && GUI_MSG_KEY_VADD == msg->dwAddData1)
				{
					ret = robin_play_aux_file("d:\\res\\sounds\\sound_short.wav", mode);
					__wrn("********robin_play_aux_file, ret=%d, mode=%d********\n", ret, mode);
				}
				else if(KEY_UP_ACTION == msg->dwAddData2
				        && GUI_MSG_KEY_VDEC == msg->dwAddData1)
				{
					mode++;

					if(mode >= CEDAR_PLAY_AUX_WAV_MODE_MAX)
					{
						mode = 0;
					}

					//                    esMEMS_Info();
					__wrn("*********mode=%d********\n", mode);
				}
			}
			#endif

#if 0
			if(0)//112350
			{
				static __s32 jump_time[] =
				{
					0, 4 * 1000 + 7, 7 * 1000 + 4, 8 * 1000 + 6,
					9 * 1000 + 7, 11 * 1000 + 0, 12 * 1000 + 1,
					14 * 1000 + 8,
					16 * 1000 + 9, 18 * 1000 + 1, 20 * 1000 + 7,
					23 * 1000 + 7, 25 * 1000 + 7, 27 * 1000 + 9
				};
				static __s32 i = 0;

				if(KEY_UP_ACTION == msg->dwAddData2
				    && GUI_MSG_KEY_VADD == msg->dwAddData1)
				{
					eLIBs_printf("i=%d, jump_time[i]=%d\n", i, jump_time[i]);
					robin_set_cmd_jump(jump_time[i]);
					i++;

					if(i >= sizeof(jump_time) / sizeof(jump_time[0]))
					{
						i = 0;
					}
				}
			}

			if(0)//112350
			{
				if(KEY_UP_ACTION == msg->dwAddData2
				    && GUI_MSG_KEY_VADD == msg->dwAddData1)
				{
					dsk_send_insmod_usbh_cmd();
				}
			}

			if(0)//112350
			{
				if(msg->dwAddData1 == GUI_MSG_KEY_FM)
				{
					if(msg->dwAddData2 == KEY_UP_ACTION)
					{
						esKSRV_SysInfo();
					}

					return EPDK_OK;
				}
			}

			if(0)//测试tv_en口 //112350
			{
				if(msg->dwAddData2 == KEY_UP_ACTION)
				{
					static __s32 status = 0;

					if(status)
					{
						eLIBs_printf("__app_pullup_tv_en\n");
						__app_pullup_tv_en();
					}
					else
					{
						eLIBs_printf("__app_pulldown_tv_en\n");
						__app_pulldown_tv_en();
					}

					status = !status;
				}
			}
#endif

			__msg("**********************before app_root_win_proc GUI_MSG_KEY**************************\n");
			audio_engine_is_open = 0;

			if(!audio_engine_is_open)
			{
				//__here__;
				if(EPDK_TRUE == dsk_radio_rcv_is_open())
				{
					//__here__;
					audio_engine_is_open = 1;
				}
			}

			__here__;

			if(!audio_engine_is_open)
			{
				//__here__;
				if(EPDK_TRUE == robin_is_open())
				{
					//__cedar_status_t status;
					//__here__;
					//status = robin_get_fsm_status();
					//__msg("status = %d\n", status);
					//if(CEDAR_STAT_PAUSE != status && CEDAR_STAT_STOP != status)
					{
						//__here__;
						audio_engine_is_open = 1;
					}
				}
			}

			//__here__;

			if(!audio_engine_is_open)
			{
				if(EPDK_TRUE == dsk_wkm_is_open())
				{
					//__cedar_status_t status;
					//status = dsk_wkm_get_fsm_state();
					//if(CEDAR_STAT_PAUSE != status && CEDAR_STAT_STOP != status)
					{
						audio_engine_is_open = 1;
					}
				}
			}

			if(!audio_engine_is_open)
			{
			#if SP_APP_AUX
				if(root_ctrl->h_app_linin)
				{
					audio_engine_is_open = 1;
				}
			#endif

				if(root_ctrl->h_app_record)
				{
					audio_engine_is_open = 1;
				}
			}

			//__here__;
			if(!audio_engine_is_open)
			{
				__u32 key_type;
				//__here__;
				key_type = dsk_keytone_get_key_type();

				if(KEY_DOWN_ACTION == msg->dwAddData2)
				{
					//__here__;
					if(KEYTONE_KEY_TYPE_DOWN & key_type)
					{
						//__here__;
						dsk_keytone_on();
						//__here__;
					}
				}
				else if(KEY_UP_ACTION == msg->dwAddData2)
				{
					//__here__;
					if(KEYTONE_KEY_TYPE_UP & key_type)
					{
						//__here__;
						dsk_keytone_on();
						//__here__;
					}
				}
				else if(KEY_REPEAT_ACTION == msg->dwAddData2)
				{
					//	__here__;
					if(KEYTONE_KEY_TYPE_REPEAT & key_type)
					{
						//__here__;
						dsk_keytone_on();
						//__here__;
					}
				}
			}

			if(GUI_MSG_KEY_SOS != msg->dwAddData1
			    && GUI_MSG_KEY_LONGSOS != msg->dwAddData1)
			{
				//__app_root_delete_sos(root_ctrl);//112358 SOS模式按其它按键无效
			}

			switch(root_ctrl->m_eWinVolume)
			{
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)

				case m_eWinVolume_echo :
				{
					if(
					  (GUI_MSG_KEY_ECHOUP != msg->dwAddData1)
					  && (GUI_MSG_KEY_LONGECHOUP != msg->dwAddData1)
					  && (GUI_MSG_KEY_ECHODOWN != msg->dwAddData1)
					  && (GUI_MSG_KEY_LONGECHODOWN != msg->dwAddData1)
					)
					{
						__com_volume_scene_delete(root_ctrl);
						root_ctrl->m_eWinVolume = m_eWinVolume_default;
					}
				}
				break;
#endif

				case m_eWinVolume_default :
				default :
				{
					if(
					  (GUI_MSG_KEY_VADD != msg->dwAddData1)
					  && (GUI_MSG_KEY_LONGVADD != msg->dwAddData1)
					  && (GUI_MSG_KEY_VDEC != msg->dwAddData1)
					  && (GUI_MSG_KEY_LONGVDEC != msg->dwAddData1)
					)
					{
						__com_volume_scene_delete(root_ctrl);
					}
				}
				break;
			}

			//处理静音
			if(KEY_DOWN_ACTION == msg->dwAddData2)
			{
				if(GUI_MSG_KEY_MUTE == msg->dwAddData1)
				{
					g_is_mute = !g_is_mute;
				#if SP_APP_ATV
					//if(!(root_ctrl->h_app_atv))
				#endif
					{
						app_root_process_before_show_dlg(root_ctrl);

						if(1 == g_is_mute)
						{
							dsk_volume_off();
							app_root_show_dlg(msg->h_deswin, STRING_ROOT_MUTE, STRING_ROOT_CUE, 100);
						}
						else
						{
							dsk_volume_on();
							app_root_show_dlg(msg->h_deswin, STRING_ROOT_NOT_MUTE, STRING_ROOT_CUE, 100);
						}

						app_root_process_after_show_dlg(root_ctrl);
					}
				}
			}

			if(1)
			{
				//处理VADD\VDEC快捷键
				__s32 ret;
				ret = app_root_check_volume_key(msg);

				if(EPDK_OK == ret)
				{
					if(root_ctrl->m_eWinVolume == m_eWinVolume_default)
					{
						g_is_mute = 0;
						dsk_volume_on();
					}

					//break;//还是让消息往下发，传给音量条场景
					//__wrn("check volume key ok...\n");
					//return EPDK_OK;
				}
			}

			if(1)//处理模式键
			{
				__s32 ret;
				ret = app_root_check_mode_key(msg);

				if(EPDK_OK == ret)
				{
					return EPDK_OK;
				}
			}

			if((msg->dwAddData1 == GUI_MSG_KEY_LOCKSCREEN) ||
			    (msg->dwAddData1 == GUI_MSG_KEY_LONGLOCKSCREEN))
			{
				__app_root_broadcast_msg(msg);
				return EPDK_OK;
			}

			{
				//处理VIDEO\AUDIO\FM\EBOOK\PHOTO快捷键
				__s32 ret;
				ret = app_root_check_short_key(msg);

				if(EPDK_OK == ret)
				{
					__wrn("check short key ok...\n");
					return EPDK_OK;
				}
			}

			if(1)
			{
				app_root_check_key_power(msg);
			}

			__msg("after app_root_win_proc GUI_MSG_KEY\n");
			break;
		}

		case GUI_MSG_TOUCH:
		{
			__msg("app_root_win_proc GUI_MSG_TOUCH\n");
			break;
		}

		case DSK_MSG_FS_PART_PLUGOUT:
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__app_root_delete_sos(root_ctrl);
			__wrn("app_root_win_proc, DSK_MSG_FS_PART_PLUGOUT\n");
			{
				H_WIN root, child, next_child;
				char  winname[256];
				__s32 ret;
#if SP_AutoUpdateFirmware

				if(root_ctrl->root_para->h_fw_update_dlg)
				{
					char		rootDir = root_ctrl->root_para->fw_update_file_disk;

					if(!DetectUpdateFirmware(&rootDir))
					{
						// 升级固件文件所在磁盘不在
						app_dialog_destroy(root_ctrl->root_para->h_fw_update_dlg);		//to delete dialog
						root_ctrl->root_para->h_fw_update_dlg = NULL;
						app_root_update_firmware_giveup(root_ctrl);
					}
				}
				else
#endif
				{
					ret = __app_root_play_other_storages_on_plugout(root_ctrl);//如果拔盘，则播放另外一个盘，

					if(EPDK_FAIL == ret)
					{
						//如果播放失败，则消息放下发，否则截获消息
						__wrn("app_root_broadcast_msg, msg->dwAddData2=%d\n", msg->dwAddData2);
						__app_root_broadcast_msg(msg);
					}
				}

				__wrn("********before __app_do_async***********\n");
				__wrn("rat_clear_all_list_by_root=%x,msg->dwAddData2=%x\n", (__u32)rat_clear_all_list_by_root, msg->dwAddData2);
				__app_do_async(msg->h_deswin, APP_ROOT_MSG_DO_ASYNC, rat_clear_all_list_by_root, (void *)(msg->dwAddData2));
				__wrn("********after __app_do_async***********\n");
			}
			return EPDK_OK;
		}

		case DSK_MSG_FS_PART_PLUGIN:
		{
			char diskname[4];
			__u32 last_lun;
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			//return EPDK_OK;//112350
			diskname[0] = (__u8)(msg->dwAddData2);
			diskname[1] = '\0';
			last_lun = (__u8)(msg->dwAddData2 >> 8);
			__wrn("*********DSK_MSG_FS_PART_PLUGIN********\n");
			__wrn("last_lun=%d\n", last_lun);
			__wrn("msg->dwAddData2=%x\n", msg->dwAddData2);
			__wrn("app_root_win_proc, DSK_MSG_FS_PART_PLUGIN\n");
			msg->dwAddData2 = diskname[0];

		#if (!SP_DEV_HW_PW_OFF)
			if(IOCTRL__PWOFF_DET__IS_OFF()
			    //				|| (root_ctrl->h_app_fm && g_is_searching)
			  )
			{
				return EPDK_OK;
			}
		#endif

			__app_root_delete_sos(root_ctrl);

			//if(1 == last_lun)
			{
#if 0
				__wrn("app_root_broadcast_msg, msg->dwAddData2=%d\n", msg->dwAddData2);
				__app_root_broadcast_msg(msg);
#else
#if SP_AutoUpdateFirmware

				if(root_ctrl->root_para->h_fw_update_dlg)
				{
					return EPDK_OK;
				}

				root_ctrl->m_updateFirmwareLog = LOG_UpdateFirmware_By_FSPlugin;
				root_ctrl->root_para->fw_update_file_disk = diskname[0];

				if(!app_root_detect_update_firmware(root_ctrl))
#endif
				{
					__s32 reg_storage_type;
					__s32 reg_media_type;
					reg_storage_type = __app_root_get_reg_storage_type(diskname[0]);
					root_ctrl->root_para->log &= ~MASK_RootLog__LastFs;

					switch(reg_storage_type)
					{
						case 0:
							root_ctrl->root_para->log |= OP_RootLog__LastFs_SD;
							break;

						case 1:
							root_ctrl->root_para->log |= OP_RootLog__LastFs_UD;
							break;

						default :
							break;
					}

					reg_media_type = -1;

					if(-1 != reg_storage_type)
					{
						if(root_ctrl->h_app_home
						    && MULTI_SCREEN_HOME_MODE_NORMAL == app_multi_screen_home_get_mode(root_ctrl->h_app_home))
						{
							app_root_shortcut_process(root_ctrl, app_root_prob_intent_plugin_for_play, reg_media_type, reg_storage_type, EPDK_TRUE, EPDK_TRUE);
						}
						else
						{
							app_root_shortcut_process(root_ctrl, app_root_prob_intent_plugin_for_play, reg_media_type, reg_storage_type, EPDK_FALSE, EPDK_TRUE);
						}
					}
					else
					{
						__wrn("reg_storage_type type error...\n");
					}
				}

#endif
			}

			return EPDK_OK;
		}

		case DSK_MSG_VOLTAGE_CHANGE:
		{
			__msg("app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
			__app_root_broadcast_msg(msg);
			return EPDK_OK;
		}

		case DSK_MSG_HOME:
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__msg("app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
			__app_root_delete_sos(root_ctrl);
			msg->dwReserved = 0;
			__app_root_broadcast_msg(msg);
			return EPDK_OK;
		}

		case DSK_MSG_ALARM:
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__msg("app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
			__app_root_delete_sos(root_ctrl);
			__app_root_broadcast_msg(msg);
			return EPDK_OK;
		}

		case DSK_MSG_TVDAC_PLUGOUT:
		case DSK_MSG_HDMI_PLUGOUT:
		case DSK_MSG_SCREEN_OPEN :
		case DSK_MSG_SCREEN_CLOSE :
		case DSK_MSG_SCREEN_FORCE_CLOSE :
		{
			__wrn("__app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
			__app_root_broadcast_msg(msg);
			return EPDK_OK;
		}

		case APP_ROOT_MSG_DO_ASYNC:
		{
			p_app_root_fun pfun;
			__msg("msg->dwAddData1=%x, msg->dwAddData2=%x\n",
			      msg->dwAddData1, msg->dwAddData2);

			if(!msg->dwAddData1)
			{
				return EPDK_OK;
			}

			pfun = (p_app_root_fun)(msg->dwAddData1);
			__msg("pfun=%x, msg->dwAddData2=%x\n", pfun, msg->dwAddData2);
			__here__;
			pfun((void *)(msg->dwAddData2));
			__here__;
			return EPDK_OK;
		}

	#if SP_DEV_DET_PLUGIN_BY_AD
		case DSK_MSG_MIC_PLUGIN:
		{
			gs_isPlugin_micDet = 1;
			__msg_jjl("DSK_MSG_MIC_PLUGIN\n");
			return EPDK_OK;
		}
		
		case DSK_MSG_MIC_PLUGOUT:
		{
			gs_isPlugin_micDet = 0;
			__msg_jjl("DSK_MSG_MIC_PLUGOUT\n");
			return EPDK_OK;
		}

		case DSK_MSG_AUX_PLUGIN:
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			root_ctrl->globe_check.linein_In = 1;
			__msg_jjl("DSK_MSG_AUX_PLUGIN\n");
			return EPDK_OK;
		}
		
		case DSK_MSG_AUX_PLUGOUT:
		{
			root_ctrl_t   *root_ctrl;
			root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			root_ctrl->globe_check.linein_In = 0;
			__msg_jjl("DSK_MSG_AUX_PLUGOUT\n");
			return EPDK_OK;
		}
	#endif
	}

	__here__;
	return GUI_ManWinDefaultProc(msg);
}

/* 创建根窗口 */
H_WIN app_root_wincreate(Activity *activity)
{
	H_WIN  						hManWin;
	__gui_manwincreate_para_t  	create_info;
	//esKSRV_SysInfo();
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.attr            = activity;
	create_info.hParent			= activity_get_root_win(activity) ;
	__wrn("create_info.hParent=%x\n", create_info.hParent);
	create_info.hHosting 		= NULL;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_root_win_proc);
	create_info.name            = APP_ROOT;
	hManWin 					= GUI_ManWinCreate(&create_info);
	__msg("root win = %x\n", hManWin);

	if(hManWin == NULL)
	{
		__wrn("home: create movie windows failed!\n");
		return NULL;
	}

	return hManWin;
}

#ifndef IOCTRL__MIC_DET__IS_PLUGIN
__bool IOCTRL__MIC_DET__IS_PLUGIN(void)
{
	return gs_isPlugin_micDet;
}
#endif
