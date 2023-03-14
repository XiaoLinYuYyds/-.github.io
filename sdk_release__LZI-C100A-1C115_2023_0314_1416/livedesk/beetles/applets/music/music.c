/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	Explorer.c,
*			Explorer.h
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-4-14 15:24:52  Libaiao  create this file, implements the fundemental
interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "music.h"
#include "music_player.h"
#include "music_ui.h"
#include "spctrm.h"


__s32 app_music_create(root_para_t  *para);
__s32 app_music_delete(void  *para);
static __s32 music_scene_create(__gui_msg_t *msg);							//����music����
static __s32 _music_manager_win_cb(__gui_msg_t *msg);

__s32 app_music_on_close(__gui_msg_t *msg);
__s32 app_music_on_destroy(__gui_msg_t *msg);


static H_LYR music_8bpp_layer_create(__u8 pipe);
static H_LYR music_32bpp_layer_create(__u8 pipe);

static __s32 music_layer_create(music_ctrl_t *music_ctrl);
static __s32 music_layer_delete(music_ctrl_t *music_ctrl);
static __s32 music_scene_delete(__gui_msg_t *msg);


/*
************************************************************************************************************************
*Function	:           __s32 app_music_create(root_para_t  *para)
*
*Description	: 		����MusicPlayerӦ�ó���
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 app_music_create(root_para_t  *para)
{
	H_WIN hwin;
	__gui_manwincreate_para_t create_info;
	music_ctrl_t *music_ctrl = NULL;
	__inf("****************************************************************************************\n");
	__inf("********  enter music player application  **************\n");
	__inf("****************************************************************************************\n");
	g_disable_standby();

	if(MUSICPLAYER_NORMAL == para->data)
	{
		gscene_hbar_set_state(HBAR_ST_HIDE);
		gscene_bgd_set_state(BGD_STATUS_SHOW);
	}

	//__wait__;
	music_ctrl = (music_ctrl_t *)esMEMS_Balloc(sizeof(music_ctrl_t));						//Ϊ˽�����������ڴ�
	eLIBs_memset(music_ctrl, 0, sizeof(music_ctrl_t));
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	music_ctrl->auto_flush_timmer_id = 0x57;
	music_ctrl->font = para->font;
	music_ctrl->root_type = para->root_type;
	__msg("*********************root_para -> root_type = %d\n", music_ctrl->root_type);
	music_ctrl->mode = para->data;
#if SP_FavoriteFiles
	music_ctrl->media_type = para->enter_media_type[m_eMediaTypeList_audio];
#endif

	if(para->explorer_play_music != 0)
	{
		music_ctrl->explorer_play_music = para->explorer_play_music;
		//eLIBs_strcpy(music_ctrl->music_file_name, para->music_file_name);
		music_ctrl->music_file_name = para->music_file_name;
	}

	create_info.name            = APP_MUSIC;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_music_manager_win_cb);
	create_info.attr            = (void *)music_ctrl;														//����Managerwin ˽������
	create_info.id							= APP_MUSIC_ID;
	create_info.hHosting        = NULL;
	hwin = GUI_ManWinCreate(&create_info);

	if(hwin)
	{
		reg_root_para_t *root_reg_para;
		__s32 reg_storage_type;
		reg_storage_type = 0;

		if(para)
		{
			if(RAT_TF == para->root_type)
			{
				reg_storage_type = 0;
			}
			else if(RAT_USB == para->root_type)
			{
				reg_storage_type = 1;
			}
			else
			{
				__wrn("para->root_para->root_type invalid...\n");
			}
		}

		root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

		if(root_reg_para)
		{
			__s32 reg_app_type;
			reg_app_type = m_eMediaTypeList_audio;
			root_reg_para->cur_play_storage = reg_storage_type;
			root_reg_para->last_play_app = reg_app_type;
			root_reg_para->last_app_play_storage[reg_app_type] = reg_storage_type;
			root_reg_para->last_storage_play_app[reg_storage_type] = reg_app_type;
		}
		else
		{
			__wrn("para is null...\n");
		}
	}

	return hwin;
}

__s32 app_music_restore(H_WIN music_man_win, root_para_t *root_para)
{
	__s32 ret = 0;
	__u32 cur_time = 0;
	music_ctrl_t *music_ctrl = NULL;
	__s32 root_type = root_para->root_type;
	char cur_file_name[RAT_MAX_FULL_PATH_LEN];

	if(music_man_win == NULL)
	{
		__here__;
		return EPDK_FAIL;
	}

	if(root_para->explorer_play_music != 0)			//���ļ���������
	{
		eLIBs_memset(cur_file_name, 0, sizeof(cur_file_name));	//�ж��Ƿ��뵱ǰ�����ļ���ͬ����ͬ��ָ���̨����
		robin_get_fsm_cur_file(cur_file_name, sizeof(cur_file_name));
		__msg("cur_file_name = %s\n", cur_file_name);
		__msg("root_para->music_file_name = %s\n", root_para->music_file_name);
		ret = eLIBs_strcmp(cur_file_name, root_para->music_file_name);

		if(ret != 0)
		{
			__here__;
			return EPDK_FAIL;
		}
	}

	if(robin_get_fsm_status() == CEDAR_STAT_STOP)
	{
		//return EPDK_FAIL;
	}

	//__wait__;
	{
		//cur_time = robin_get_cur_time();
		//cur_time = robin_get_cur_time();
		//__msg("cur_time= d%\n", cur_time);
	}
	{
		//cur_time = robin_get_cur_time();
		//cur_time = robin_get_cur_time();
		//__msg("cur_time= d%\n", cur_time);
	}
	//GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
	{
		//cur_time = robin_get_cur_time();
		//cur_time = robin_get_cur_time();
	}
	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);
	__msg("music_ctrl=%d\n", music_ctrl);
	__msg("music_ctrl->root_type=%d\n", music_ctrl->root_type);
	__msg("root_type=%d\n", root_type);
	__msg("para->root_type=%d\n", root_para->root_type);

	if(music_ctrl->root_type != root_type)//��������̷����򷵻�ʧ�ܣ�����ܻ��ؽ�һ��music
	{
		__here__;
		__msg("disk changed\n");
		return EPDK_FAIL;
	}

#if SP_FavoriteFiles
	music_ctrl->media_type = root_para->enter_media_type[m_eMediaTypeList_audio];
#endif
	music_ctrl->root_type = root_type;
	music_ctrl->explorer_play_music = root_para->explorer_play_music;
	music_ctrl->music_file_name = root_para->music_file_name;
	__msg("root_para->explorer_play_music=%d\n", root_para->explorer_play_music);
	{
		music_player_ctrl_t *music_player_ctrl;
		music_player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_ctrl->h_framewin);

		if(music_player_ctrl == NULL)
		{
			__here__;
			return EPDK_FAIL;
		}

		music_player_ctrl->explorer_play_music = music_ctrl->explorer_play_music;
		music_player_ctrl->music_file_name = music_ctrl->music_file_name;
	}
	GUI_WinSetFocusChild(music_man_win);

	if(music_ctrl->mode == MUSICPLAYER_BACKGROUND)
	{
		music_ctrl->mode = 	MUSICPLAYER_NORMAL;
	}

	{
		//cur_time = robin_get_cur_time();
		//cur_time = robin_get_cur_time();
	}

	gscene_hbar_set_state(HBAR_ST_HIDE);
	gscene_bgd_set_state(BGD_STATUS_SHOW);
	ret = GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
	//clear_music_list_area();
	__msg("--------------music player set layer status on, ret = %d\n", ret);
	{
		//cur_time = robin_get_cur_time();
		//cur_time = robin_get_cur_time();
	}
	//__wait__;
	ret = music_player_restore(music_ctrl->h_framewin, music_ctrl->root_type);
	GUI_WinSetFocusChild(music_ctrl->h_framewin);		//frame win get focus.
	//ret = GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
	return ret;
}
/*
************************************************************************************************************************
*Function	:           __s32 app_music_delete(root_para_t  *para)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 app_music_delete(void *para)
{
	return EPDK_OK;
}



__s32 app_music_on_key_proc(__gui_msg_t *msg)
{
	if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:
			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:
			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				break;

			case GUI_MSG_KEY_VDEC:
				break;

			case GUI_MSG_KEY_ENTER:
			case GUI_MSG_KEY_MENU:
				break;

			case GUI_MSG_KEY_ESCAPE:
				//music_send_command(msg->h_deswin, 0, 0, 0);
				break;

			case GUI_MSG_KEY_RISE:
				break;

			default:
				break;
		}
	}

	return EPDK_OK;
	//return EPDK_OK;
}


__s32 app_music_on_command(__gui_msg_t *msg)
{
	switch(msg->dwAddData1)
	{
		case MUSICPLAYER_BACKGROUND:
			music_scene_delete(msg);
			break;
	}

	return EPDK_OK;
}

//����һ�����������������
__s32 app_music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	char buf[128];
	H_WIN hDstWin;
	//H_WIN hManagerWin;
	//robin_set_cmd_stop();
	//hManagerWin = GUI_WinGetParent(hwin);
	hDstWin = GUI_WinGetParent(hwin);
	msg.h_deswin = hDstWin;
	msg.h_srcwin = hwin;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);//
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_WinGetName(msg.h_deswin, buf);
	__msg("%s\n", buf);
	GUI_SendNotifyMessage(&msg);
	//GUI_SendMessage(&msg);
	return EPDK_OK;
}


static __s32 __music_install_auto_play_timmer(__gui_msg_t *msg)
{
	music_ctrl_t    *music_ctrl;
	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!music_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

#if MUSIC_AUTO_FLUSH_TIME

	if(!GUI_IsTimerInstalled(msg->h_deswin, music_ctrl->auto_flush_timmer_id))
	{
		GUI_SetTimer(msg->h_deswin, music_ctrl->auto_flush_timmer_id
		             , MUSIC_AUTO_FLUSH_FIRST_TIME, NULL);
	}
	else
	{
		GUI_ResetTimer(msg->h_deswin, music_ctrl->auto_flush_timmer_id
		               , MUSIC_AUTO_FLUSH_FIRST_TIME, NULL);
	}

#endif
	return EPDK_OK;
}

static __s32 __music_uninstall_auto_play_timmer(__gui_msg_t *msg)
{
	music_ctrl_t    *music_ctrl;
	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!music_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

#if MUSIC_AUTO_FLUSH_TIME

	if(GUI_IsTimerInstalled(msg->h_deswin, music_ctrl->auto_flush_timmer_id))
	{
		GUI_KillTimer(msg->h_deswin, music_ctrl->auto_flush_timmer_id);
	}

#endif
	return EPDK_OK;
}



/*
************************************************************************************************************************
*Function	:           static __s32 _music_manager_win_cb(__gui_msg_t *msg)
*
*Description	: 		manager window contrl block
*					gets message from system, & dispatch command to child
*Arguments  	: 		get a message from system
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _music_manager_win_cb(__gui_msg_t *msg)
{
	//__msg("music manwin msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
			music_scene_create(msg);
			return EPDK_OK;

		case GUI_MSG_CLOSE:
			__msg("----music manager window GUI_MSG_CLOSE begin----\n");
			//app_explorer_on_close(msg);
			app_music_on_close(msg);
			return EPDK_OK;

		case GUI_MSG_DESTROY:
			__msg("----music manager window GUI_MSG_DESTROY begin----\n");
			__music_uninstall_auto_play_timmer(msg);
			//app_explorer_on_destroy(msg->h_deswin);			//release  layer,memory
			app_music_on_destroy(msg);
			{
				reg_root_para_t *root_reg_para;
				root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

				if(root_reg_para)
				{
					root_reg_para->cur_play_storage = -1;
				}
				else
				{
					__wrn("para is null...\n");
				}
			}
			return EPDK_OK;

		case GUI_MSG_KEY:
			app_music_on_key_proc(msg);
			break;

		case GUI_MSG_TOUCH:
			break;

		case GUI_MSG_COMMAND:
			__msg("----music manager window GUI_MSG_CLOSE begin----\n");
			app_music_on_command(msg);
			//return EPDK_OK;
			break;

		case DSK_MSG_VOLTAGE_CHANGE:
		{
			music_ctrl_t	*music_ctrl = NULL;
			music_ctrl	 = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(music_ctrl && music_ctrl->h_framewin)
			{
				__gui_msg_t set_msg;
				eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
				set_msg.h_deswin = music_ctrl->h_framewin;
				GUI_SendMessage(&set_msg);
			}

			return EPDK_OK;
		}

		case DSK_MSG_FS_PART_PLUGIN:
			break;

		case DSK_MSG_FS_PART_PLUGOUT:
		{
			char diskname[2] = {0};
			music_ctrl_t	*music_ctrl = NULL;
			music_ctrl	 = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			__msg("SD card plugout!");
			diskname[0] = (char)(msg->dwAddData2);
			diskname[1] = 0;

			if(0 == eLIBs_strnicmp(music_ctrl->root, diskname, 1))
			{
				if(MUSICPLAYER_BACKGROUND == music_ctrl->mode)
				{
					app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, CLOSE_BG_MUSIC, music_ctrl->mode);
				}
				else
				{
					app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, music_ctrl->mode);
				}
			}

			return EPDK_OK;
		}

		case GUI_MSG_TIMER:
		{
			music_ctrl_t *music_ctrl;
			__s32 ret;
			ret = -1;
			music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(!music_ctrl)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			if(LOWORD(msg->dwAddData1) == music_ctrl->auto_flush_timmer_id)
			{
				//Tarring
				music_player_ctrl_t *music_player_ctrl;
				music_player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_ctrl->h_framewin);
				__wrn("*********************music flush*********************\n");
#if 1
				//Tarring
				music_save_breakpoint(music_player_ctrl);
				//dsk_reg_flush();
#endif
				GUI_ResetTimer(msg->h_deswin, music_ctrl->auto_flush_timmer_id
				               , MUSIC_AUTO_FLUSH_TIME, NULL);
				return EPDK_OK;
			}

			break;
		}

		default:
			break;
	}

	return GUI_ManWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*Function	:           __s32 app_music_on_close(__gui_msg_t *msg)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 app_music_on_close(__gui_msg_t *msg)
{
	//music_ctrl_t	*music_ctrl = NULL;
	//music_ctrl	 = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	GUI_ManWinDelete(msg->h_deswin);	//it will send detroy message after call this function.
	return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           __s32 app_music_on_destroy(__gui_msg_t *msg)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 app_music_on_destroy(__gui_msg_t *msg)
{
	music_ctrl_t	*music_ctrl = NULL;
	music_ctrl	 = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	g_enable_standby();
	delete_music_player_volume_scene(music_ctrl->h_framewin);
	__msg("before music_layer_delete\n");
	music_layer_delete(music_ctrl);
	__msg("after music_layer_delete\n");
	__here__;
	//if(music_ctrl->mode != MUSICPLAYER_BACKGROUND)
	{
		UnRegisterSpectrumControl();					//delete spectrum class
	}
	__here__;
	esMEMS_Bfree(music_ctrl, sizeof(music_ctrl_t));
	__here__;
	return EPDK_OK;
}

static __s32  MusicGetSearchPath(__s32 root_type, char *path)
{
	__s32 ret = 0;
	char str_root[50];
	char disk_name[RAT_MAX_PARTITION][4];
	eLIBs_memset(str_root, 0, sizeof(str_root));
	eLIBs_memset(disk_name, 0, sizeof(disk_name));

	switch(root_type)
	{
		case RAT_LOCAL:
			eLIBs_strcpy(str_root, RAT_LOCAL_DISK);
			break;

		case RAT_TF:
			eLIBs_strcpy(str_root, RAT_SD_CARD);
			break;

		case RAT_USB:
			eLIBs_strcpy(str_root, RAT_USB_DISK);
			break;

		default:
			break;
	}

	__msg("root_type=%d\n", root_type);
	ret = rat_get_partition_name(str_root, disk_name);
	eLIBs_strcpy(path, disk_name[0]);

	if(path[0])
	{
		return EPDK_OK;
	}
	else
	{
		return EPDK_FAIL;
	}
}


/*
************************************************************************************************************************
*Function	:           static __s32 music_scene_create(__gui_msg_t *msg)
*
*Description	: 		�������ֲ�����������ListBar
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_scene_create(__gui_msg_t *msg)
{
	music_ctrl_t	*music_ctrl;
	__s32 ret;
	music_player_ctrl_t player_para;
	eLIBs_memset(&player_para, 0, sizeof(music_player_ctrl_t));
	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	player_para.root_type = music_ctrl->root_type;
	MusicGetSearchPath(music_ctrl->root_type, music_ctrl->root);
#if SP_FavoriteFiles
	player_para.media_type = music_ctrl->media_type;
#else
	player_para.media_type = RAT_MEDIA_TYPE_AUDIO;
#endif
	//player_para.view_mode = music_ctrl->view_mode;		//view mode,
	player_para.font = SWFFont;

	switch(music_ctrl->mode)
	{
		case MUSICPLAYER_NORMAL:
			player_para.visiable = EPDK_TRUE;
			break;

		case MUSICPLAYER_BACKGROUND:
			player_para.visiable = EPDK_FALSE;
			break;

		default:
			player_para.visiable = EPDK_TRUE;
			break;
	}

	//app_print_memory_usage("music scene create", NULL);
	//player_para.view_mode = music_ctrl->view_mode;		//view mode,
	ret = music_layer_create(music_ctrl);								//����ͼ��

	if(ret == EPDK_FAIL)
	{
		__wrn("create layer for music player failed!");
		return EPDK_FAIL;
	}

	//app_print_memory_usage("music layer create finished", NULL);
	//if(player_para.visiable)
	{
		#if 1//ԭ����
		if(!RegisterSpectrumControl())
		{
			__wrn("!!!!!!!!register spectrum ocx error!!!\n");
		}

		music_ctrl->h_spectrum_win = SPCTRM_SceneCreate((H_WIN)(msg->h_deswin), (H_WIN)(music_ctrl->music_layer));

		if(music_ctrl->h_spectrum_win == NULL)
		{
			__msg("creat spectrum frame win error!");
		}
		#else//�µ�
		if(!NEW_RegisterSpectrumControl())
		{
			__wrn("!!!!!!!!register spectrum ocx error!!!\n");
		}

		music_ctrl->h_spectrum_win = NEW_SPCTRM_SceneCreate((H_WIN)(msg->h_deswin), (H_WIN)(music_ctrl->music_layer));

		if(music_ctrl->h_spectrum_win == NULL)
		{
			__wrn("creat spectrum frame win error!");
		}
		#endif
		
	}
	//app_print_memory_usage("spectrum scene create finished!", NULL);
	//player_para.background_layer = 	music_ctrl->background_layer;
	player_para.music_layer = 	music_ctrl->music_layer;
	player_para.explorer_play_music = music_ctrl->explorer_play_music;
	player_para.music_file_name = music_ctrl->music_file_name;
	music_ctrl->h_framewin = music_player_win_create(msg->h_deswin, &player_para);

	if(MUSICPLAYER_NORMAL == music_ctrl->mode)
	{
		GUI_WinSetFocusChild(music_ctrl->h_framewin);		//frame win get focus.					//set picture layer as top level
	}

	if(player_para.visiable)
	{
		GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
	}
	else
	{
		GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_SLEEP);
		__msg("music layer is off for background play mode\n");
	}

	__music_install_auto_play_timmer(msg);
#if MUSIC_ENTER_FLUSH
	__wrn("**********music flush**************\n");
	dsk_reg_flush();
#endif
	return EPDK_OK;
}


static __s32 music_scene_delete(__gui_msg_t *msg)
{
	music_ctrl_t	*This = NULL;
	__s32 			screen_width;
	__s32 			screen_height;
	This = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(This == NULL)
	{
		return EPDK_FAIL;
	}

	if(This->music_layer == NULL)
	{
		return EPDK_FAIL;
	}

	This->mode = MUSICPLAYER_BACKGROUND;

	if(This->h_spectrum_win != NULL)
	{
		//robin_disable_spectrum();
		//GUI_FrmWinDelete(This->h_spectrum_win);				//ɾ��Ƶ��ͼ����
		//UnRegisterSpectrumControl();
		//GUI_FrmWinDelete(This->h_spectrum_win);
		//This->h_spectrum_win = NULL;
	}

	delete_music_player_volume_scene(This->h_framewin);
	{
		/* get lcd size*/
		dsk_display_get_size(&screen_width, &screen_height);
		GUI_ClearRect(0, 0, screen_width, screen_height);
	}
	music_player_delete_digit_layer(This->h_framewin);
	GUI_LyrWinSetSta(This->music_layer, GUI_LYRWIN_STA_SLEEP);
	return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:          static H_LYR music_8bpp_layer_create(void)
*
*Description	: 		����ͼ�㣬��Ҫ������ʾ8bpp ����ͼƬ
*					ע����ǰ��ͼӦ���ڲ�ͬ��pipe
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

static H_LYR music_8bpp_layer_create(__u8 pipe)
{
	H_LYR layer = NULL;
	RECT LayerRect;
	//int ret;
	//__layerwincreate_para_t lyrcreate_info;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		//{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
		//{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
		{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
		//DISP_LAYER_WORK_MODE_NORMAL,
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
		"APP_MUSIC",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	music_get_screen_rect(&LayerRect);
	fb.size.width		= LayerRect.width;
	fb.size.height		= LayerRect.height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= LayerRect.x;
	lstlyr.src_win.y  		= LayerRect.y;
	lstlyr.src_win.width 	= LayerRect.width;
	lstlyr.src_win.height 	= LayerRect.height;
	lstlyr.scn_win.x		= LayerRect.x;
	lstlyr.scn_win.y		= LayerRect.y;
	lstlyr.scn_win.width  	= LayerRect.width;
	lstlyr.scn_win.height 	= LayerRect.height;
	lstlyr.pipe = pipe;		//0;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("music layer create error !\n");
	}

	return layer;
}

/*
************************************************************************************************************************
*Function	:          static H_LYR music_32bpp_layer_create(__u8 pipe)
*
*Description	: 		ǰ��ͼ�㣬��Ҫ������ʾǰ��ͼƬ��ListBar���item, ����ͼ
*					ע���뱳��ͼӦ���ڲ�ͬ��pipe
*
*Arguments  	:
*
*Return     	: 	     ͼ����
*
************************************************************************************************************************
*/
static H_LYR music_32bpp_layer_create(__u8 pipe)
{
	H_LYR layer = NULL;
	RECT LayerRect;
	//	int ret;
	//__layerwincreate_para_t lyrcreate_info;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		//{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */

	};
	__disp_layer_para_t lstlyr =
	{
		//DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
		DISP_LAYER_WORK_MODE_NORMAL,
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
		"APP_MUSIC",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	music_get_screen_rect(&LayerRect);
	fb.size.width		= LayerRect.width;
	fb.size.height		= LayerRect.height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= LayerRect.x;
	lstlyr.src_win.y  		= LayerRect.y;
	lstlyr.src_win.width 	= LayerRect.width;
	lstlyr.src_win.height 	= LayerRect.height;
	lstlyr.scn_win.x		= LayerRect.x;
	lstlyr.scn_win.y		= LayerRect.y;
	lstlyr.scn_win.width  	= LayerRect.width;
	lstlyr.scn_win.height 	= LayerRect.height;
	lstlyr.pipe = pipe;		//1
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app music layer create error !\n");
	}

	return layer;
}

/*
************************************************************************************************************************
*Function	:           static __s32 music_layer_create(music_ctrl_t *music_ctrl)
*
*Description	: 			��������ͼ��
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_layer_create(music_ctrl_t *music_ctrl)
{
	/*
	if(music_ctrl->spectrum_layer == NULL)
	{
		music_ctrl->spectrum_layer = spectrum_32bpp_layer_create(1);
		if(music_ctrl->spectrum_layer == NULL)
		{
			__err("spectrum layer create error !\n");
			return EPDK_FAIL;
		}
	}
	*/
	if(music_ctrl->music_layer == NULL)
	{
		music_ctrl->music_layer = music_32bpp_layer_create(1);

		if(music_ctrl->music_layer == NULL)
		{
			__err("music layer create error !\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__err("music layer create error !\n");
	}

	/*
	if(music_ctrl->background_layer == NULL)
	{
			music_ctrl->background_layer = music_8bpp_layer_create(0);
			if(music_ctrl->background_layer == NULL)
			{
				__err("music layer create error !\n");
				return EPDK_FAIL;
			}
	}
	else
	{
		__err("music layer create error !\n");
		return EPDK_FAIL;
	}
	*/
	return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           static __s32 music_layer_delete(H_LYR layer)
*
*Description	: 		ɾ��ͼ��
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_layer_delete(music_ctrl_t *music_ctrl)
{
	if(music_ctrl->music_layer != NULL)
	{
		GUI_LyrWinDelete(music_ctrl->music_layer);
	}

	return EPDK_OK;
}

__s32 app_music_get_root_type(H_WIN music_man_win, __u32 *root_type)
{
	music_ctrl_t 			*music_ctrl = NULL;
	music_player_ctrl_t	*music_player_ctrl = NULL;

	if(music_man_win == NULL)
	{
		return EPDK_FAIL;
	}

	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

	if(music_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	if(root_type)
	{
		*root_type = music_ctrl->root_type;
	}

	return EPDK_OK;
}

__s32 app_music_get_mode(H_WIN music_man_win)
{
	music_ctrl_t 			*music_ctrl = NULL;
	musicplayer_mode_e   mode;
	mode = -1;

	if(music_man_win == NULL)
	{
		goto end;
	}

	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

	if(music_ctrl == NULL)
	{
		goto end;
	}

	mode = music_ctrl->mode;
end:
	;
	return mode;
}

__s32 app_music_set_bg_mode(H_WIN music_man_win)
{
	__s32 ret;
	music_ctrl_t 			*music_ctrl = NULL;
	__gui_msg_t my_msg;

	if(music_man_win == NULL)
	{
		goto fail;
	}

	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

	if(music_ctrl == NULL)
	{
		goto fail;
	}

	if(MUSICPLAYER_NORMAL != music_ctrl->mode)
	{
		__wrn("music is not in background mode...\n");
		goto fail;
	}

	if(NULL == music_ctrl->h_framewin)
	{
		goto fail;
	}

	__wrn("music_ctrl->h_framewin=%x\n", music_ctrl->h_framewin);
	ret = music_player_scene_delete(music_ctrl->h_framewin);

	if(EPDK_FAIL == ret)
	{
		goto fail;
	}

	eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
	my_msg.h_deswin = music_man_win;
	ret = music_scene_delete(&my_msg);
	__wrn("set music bg msg ok...\n");
	return EPDK_OK;
fail:
	;
	__wrn("set music bg msg fail...\n");
	return EPDK_FAIL;
}

__s32 app_music_get_index(H_WIN music_man_win, __u32 *index)
{
	music_ctrl_t 			*music_ctrl = NULL;
	music_player_ctrl_t	*music_player_ctrl = NULL;

	if(music_man_win == NULL)
	{
		*index = 0;
		return EPDK_FAIL;
	}

	music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

	if(music_ctrl == NULL)
	{
		*index = 0;
		return EPDK_FAIL;
	}

	music_player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_ctrl->h_framewin);

	if(music_player_ctrl == NULL)
	{
		*index = 0;
		return EPDK_FAIL;
	}

	if(music_player_ctrl->list_ctrl != NULL)
	{
		*index = music_player_ctrl->list_ctrl->play_index;
	}
	else
	{
		//*index = music_player_ctrl->play_index;
		*index = 0;
	}

	return music_ctrl->root_type;
}

