/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	music_player.h
*
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-5-3 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/




#include "beetles_app.h"
#include "music.h"
#include "music_ui.h"
#include "music_list.h"
#include "music_player.h"

#include "player_face.h"

#include "spectrum_i.h"
#include "MusicVolumeBar.h"
#include "spctrm.h"
#include "music_lyrics.h"

//#include "Music_spectrum.h"
#define  NAME_MUSIC_BKPOINT				   "MUSICINF"

#define TIMEROUT_ID_TIMER_HLYR_FF_FB		400
#if TIMEROUT_ID_TIMER_HLYR_FF_FB
#define ID_TIMER_HLYR_FF_FB			0x5555
#endif

static char last_filename = 1;
static char music_key_count = 0;
static char music_rr_ff = 0;
static char long_key_flag = 0;

__u8 music_exit = 0;

static __u32 MusicTimerId = 0x01;
static __u32 Music_FF_RR_TimerId = 0x05;
static __u32 MusicSysTimerId = 0x10;

static __u32 bat_lever = 10;
static __u32 root_type_flag = 10;
//static __hdle music_mute_hdl = 0;

static __bool music_rr_flag = 0;

static __u32 MusicCheckFocusTimerId = 0x10;				//检测当前条目是否与实际播放条目相同
#if SP_FavoriteFiles
__s32 music_play_status = 0;
#endif

H_WIN 			music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para);
static __s32 	_music_player_win_cb(__gui_msg_t *msg);
static __s32 	music_player_win_key_proc(__gui_msg_t *msg);
static __s32 	music_player_scene_create(H_WIN  music_frame_win);								//创建场景
static __s32 	music_player_win_on_close(__gui_msg_t *msg);
static __s32  	music_player_win_on_destroy(__gui_msg_t *msg);
static __s32 	music_player_win_on_paint(H_WIN  music_frame_win);


static __s32 	music_play_file(music_player_ctrl_t *player_ctrl, char *filename, __bool breakplay);

static __s32 	music_player_device_init(music_player_ctrl_t *player_ctrl);	//播放音频设备初始化
static __s32 	music_player_device_uninit(music_player_ctrl_t *player_ctrl);//关闭音频设备

static __s32 	music_player_win_on_timer(__gui_msg_t *msg);

static __s32 	music_player_check_play_status_on_timer(music_player_ctrl_t *this);
__s32 			music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
static __s32 	music_player_set_robin_EQ_mode(music_player_ctrl_t *this);


__s32 			music_player_get_last_para(music_player_ctrl_t *This);
__s32 			music_player_save_last_para(music_player_ctrl_t *This);

__s32 			create_music_lyrics_scene(music_player_ctrl_t *This);
__s32 			delete_music_lyrics_scene(music_player_ctrl_t *This);


__s32 music_player_win_on_sys_timer(music_player_ctrl_t *this);


__s32 music_restore_play_file_info(music_player_ctrl_t *this);

static void DrawMusicDirectory(__u32 focus, GUI_FONT *font);
static void DrawMusic_total(music_player_ctrl_t *this);
static void DrawMusic_playing(music_player_ctrl_t *this);
#if SP_FavoriteFiles
static void Drawmusic_play_style(music_player_ctrl_t *this);
#endif



//extern __s32 draw_music_player_background(music_player_ctrl_t *player_ctrl);

/*
************************************************************************************************************************
*Function	:          H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para)
*
*Description	: 		创建MusicPlayer framewin
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	music_player_ctrl_t *music_player_ctrl;
	FB fb;
	//app_print_memory_usage("music player win create", NULL);
	GUI_LyrWinGetFB(para->music_layer, &fb);
	music_player_ctrl = (music_player_ctrl_t *)esMEMS_Balloc(sizeof(music_player_ctrl_t));
	eLIBs_memset((void *)music_player_ctrl, 0, sizeof(music_player_ctrl_t));
	music_player_ctrl->digit_timmer_id = 0x16;
	music_player_ctrl->font = para->font;
	//music_player_ctrl->background_layer = para->background_layer;
	music_player_ctrl->music_layer = para->music_layer;
	music_player_ctrl->media_type = para->media_type;		//photo, music, movie
	music_player_ctrl->root_type = para->root_type;							//SD or USB device
	music_player_ctrl->visiable = para->visiable;				//是否背景播放
	music_player_ctrl->explorer_play_music = para->explorer_play_music;
	music_player_ctrl->music_file_name = para->music_file_name;
	//music_player_ctrl->view_mode = para->view_mode;							//list or square
	//ExplorerListWinGetSearchPath(list_para);
	//eLIBs_memcpy(music_player_ctrl->search_path,"e:", sizeof("e:"));
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name =	"Explorer List window",
	              framewin_para.dwExStyle = WS_EX_NONE;
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;
	framewin_para.id        = MUSIC_LIST_WIN_ID;
	framewin_para.hHosting = h_parent;
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_music_player_win_cb);
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width = fb.size.width;
	framewin_para.rect.height = fb.size.height;
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 255;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)music_player_ctrl;
	framewin_para.hLayer = music_player_ctrl->music_layer;			//会被listbar调用
	return (GUI_FrmWinCreate(&framewin_para));						//在framewin 的create消息中将handle,传送给music_player_ctrl->h_framewin
}

__s32 music_player_on_power_charge(__gui_msg_t *msg)
{
	music_player_ctrl_t *this;
	power_level_e power_level = 0;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(this == NULL)
	{
		return EPDK_FAIL;
	}

	if(!this->visiable)
	{
		return EPDK_FAIL;
	}

	power_level = msg->dwAddData1;
	GUI_LyrWinSel(this->music_layer);
	DrawMusicBatteryStatus(power_level);
	return EPDK_OK;
}



__s32 music_get_fullcharge(void)
{
	ES_FILE *power;
	__drv_power_fuelguage_t 		info;
	power = eLIBs_fopen("b:\\HID\\POWER", "w");

	if(!power)
	{
		__msg(" Power Driver cannot be open !\n");
		return EPDK_FALSE;
	}

	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_FUELGUAGE, 0, &info);
	eLIBs_fclose(power);

	if(info.is_full_charge)
	{
		return EPDK_TRUE;
	}
	else
	{
		return EPDK_FALSE;
	}
}
__s32 music_player_win_on_command(__gui_msg_t *msg)
{
	switch(msg->dwAddData1)
	{
		case MUSICPLAYER_BACKGROUND:
			music_player_scene_delete(msg->h_deswin);
			//send message to main menu
			music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, MUSICPLAYER_BACKGROUND);
			break;
#if SP_FavoriteFiles

		case MUSICPLAYER_FAVORPLAY:
			music_player_scene_delete(msg->h_deswin);
			__wrn("come here\n");
			//send message to main menu
			music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_FAVORPLAY, 0);
			break;
#endif

		default:
			break;
	}

	return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          static __s32 _music_player_win_cb(__gui_msg_t *msg)
*
*Description	: 		framewin 消息处理函数
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

static __s32 __music_delete_breakpoint_info(__s32 index)
{
	__s32 ret;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__cedar_tag_inf_t tag_inf;
	ret = robin_npl_index2file(index, filename);

	if(-1 == ret)
	{
		__msg("robin_npl_index2file fail...\n");
		return -1;
	}

	if(!fifo_db_has_exist(NAME_MUSIC_BKPOINT))
	{
		return -1;
	}

	ret = get_item_content_from_fifo_db(NAME_MUSIC_BKPOINT, filename, &tag_inf);

	if(-1 == ret)
	{
		return 0;
	}

	if(delete_item_from_fifo_db(NAME_MUSIC_BKPOINT, filename))
	{
		return -1;
	}

	return 0;
}

/*
************************************************************************************************************************
*Function	:            app_music_on_command
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _music_player_win_cb(__gui_msg_t *msg)
{
	__s32 ret = EPDK_OK;
	music_player_ctrl_t *this;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	//__msg("music player win msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//  ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
			__msg("----explorer frame window GUI_MSG_CREATE begin----\n");
			music_player_scene_create(msg->h_deswin);   									//on create message
			return EPDK_OK;

		case GUI_MSG_PAINT:
			__msg("----explorer frame window GUI_MSG_ONPAINT----\n");
			music_player_win_on_paint(msg->h_deswin);
			return EPDK_OK;

		case GUI_MSG_CLOSE:
			__msg("----explorer frame window GUI_MSG_CLOSE begin----\n");
			music_player_win_on_close(msg);
			return EPDK_OK;

		case GUI_MSG_DESTROY:
			__msg("----explorer frame window GUI_MSG_DESTROY begin----\n");
			music_player_win_on_destroy(msg);
			__msg("----explorer frame window GUI_MSG_DESTROY end----\n");
			return EPDK_OK;

		case GUI_MSG_COMMAND:
			music_player_win_on_command(msg);
			break;

		case GUI_MSG_KEY:
			if(this->visiable)		//有界面显示才会按键操作
			{
				music_player_win_key_proc(msg);
			}

			break;

		case DSK_MSG_VOLTAGE_CHANGE:
			music_player_on_power_charge(msg);
			return EPDK_OK;

		case GUI_MSG_TIMER:
			if(MusicTimerId != msg->dwAddData1)
			{
				__wrn("this->digit_timmer_id=%d\n", this->digit_timmer_id);
				__wrn("msg->dwAddData1=%d\n", msg->dwAddData1);
			}

			if(MusicSysTimerId == msg->dwAddData1)	//定时去更新播放状态)
			{
				power_level_e level;
				__here__;
				music_player_win_on_sys_timer(this);
				__here__;
				dsk_power_get_voltage_level(&level);
				__here__;
				GUI_LyrWinSel(this->music_layer);
				__here__;

				if(dsk_get_charge_state())
				{
					__here__;

					if(music_get_fullcharge())
					{
						__here__;
						DrawMusicBatteryStatus(5);
					}
					else
					{
						__here__;

						if(bat_lever == 10)
						{
							bat_lever = level;
							__here__;
							DrawMusicBatteryStatus(level);
						}
						else
						{
							__here__;
							DrawMusicBatteryStatus(bat_lever);
							__here__;
							bat_lever ++;

							if(bat_lever > 5)
							{
								__here__;
								bat_lever = 0;
								//bat_lever = level;
							}
						}
					}
				}
				else
				{
					__here__;
					DrawMusicBatteryStatus(level);
				}
			}
			else if(MusicCheckFocusTimerId == msg->dwAddData1)
			{
				//music_player_win_check_focus_on_timer(this);
			}
			else if(MusicTimerId == msg->dwAddData1)
			{
				__here__;
				music_player_win_on_timer(msg);
			}
			else if(this->digit_timmer_id &&
			        this->digit_timmer_id == msg->dwAddData1)
			{
				__here__;
				__wrn("timmer has come...\n");
				__wrn("this->ndigit_song_num=%d\n", this->ndigit_song_num);
				music_player_set_play_index(this, this->ndigit_song_num);
				__here__;
				music_player_delete_digit_layer(msg->h_deswin);
			}

#if TIMEROUT_ID_TIMER_HLYR_FF_FB
			else if(msg->dwAddData1 == ID_TIMER_HLYR_FF_FB)
			{
				music_ff_fb_layer_delete(msg);
			}

#endif
			break;

		case DSK_MSG_FS_PART_PLUGOUT:
			__msg("SD card plugout!");
			//music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
			break;

		default:
			ret = GUI_FrmWinDefaultProc(msg);
			return ret;
	}

	return GUI_FrmWinDefaultProc(msg);
}

//NextKey处理函数
static __s32 music_explorer_on_next_key(__gui_msg_t *msg)
{
	music_player_ctrl_t *player_ctrl = NULL;
	music_list_ctrl_t *ListCtrl = NULL;
	char *filename = NULL;
	__listbar_scene_t scene;
	player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(player_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	ListCtrl = player_ctrl->list_ctrl;

	if(ListCtrl == NULL)
	{
		return EPDK_FAIL;
	}

	filename = player_ctrl->play_file_info.filename;
	LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
	__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
	ListCtrl->OnNextKey((void *)ListCtrl);				//listbar on nextkey
	LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
	__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
	//show_music_list(ListCtrl);
	//get_music_list_focus_item_full_filename(ListCtrl, filename);
	//music_play_file(player_ctrl, filename);
	player_ctrl->CheckFocusItemCnt = 0;		//有按键清除检测焦点条目定时计数器
	return EPDK_OK;
}

//NextKey处理函数
static __s32 music_explorer_on_previous_key(__gui_msg_t *msg)
{
	music_player_ctrl_t *player_ctrl;
	music_list_ctrl_t *ListCtrl;
	//char *filename = NULL;
	player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(player_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	ListCtrl = player_ctrl->list_ctrl;

	if(ListCtrl == NULL)
	{
		return EPDK_FAIL;
	}

	//filename = player_ctrl->play_file_info.filename;
	ListCtrl->OnPreviousKey((void *)ListCtrl);			//listbar on previous key
	//show_music_list(ListCtrl);
	//get_music_list_focus_item_full_filename(ListCtrl, filename);
	//music_play_file(player_ctrl, filename);
	player_ctrl->CheckFocusItemCnt = 0;		//有按键清除检测焦点条目定时计数器
	return EPDK_OK;
}

static __s32 music_explorer_on_menu_key(__gui_msg_t *msg)
{
	music_player_ctrl_t *this;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(this->menu_mode == MUSIC_EXPLORER_MODE)
	{
		{
			music_palyer_ui_t *player_ui;
			GUI_MEMDEV_Handle	draw_mem;
			player_ui = music_get_player_ui_param();
			draw_mem = GUI_MEMDEV_Create(player_ui->contrl_BG_rect.x, player_ui->contrl_BG_rect.y, player_ui->contrl_BG_rect.width, player_ui->contrl_BG_rect.height);
			GUI_MEMDEV_Select(draw_mem);
			this->menu_mode = MUSIC_SET_PLAYER_MODE;
			GUI_UC_SetEncodeUTF8();				//UTF8字体显示
			//GUI_LyrWinCacheOn();
			ClearMusicPlayerContrlButtonBG();
			DrawMusicPlayerContrlButton(this);
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
		}
		//GUI_LyrWinCacheOff();
	}
	else
	{
		//this->menu_mode = MUSIC_EXPLORER_MODE;
		//__msg("error in music explorer");
	}

	return EPDK_OK;
}

__s32 music_save_breakpoint(music_player_ctrl_t *This)
{
	__wrn("music_save_breakpoint...........\n");
	__wrn("This=%d\n", This);
	__wrn("This->play_index=%d\n", This->play_index);
	__music_save_breakpoint_info(This->play_index);
	__wrn("This->list_ctrl=0x%x\n", This->list_ctrl);
	music_list_save_last_para(This->list_ctrl);			//保存参数
	__wrn("music_list_save_last_para.......\n");
	dsk_reg_flush();
	/*
	if(This->play_cnt < 20)//20
	{
		This->play_cnt++;
		dsk_reg_flush();
	}
	else if(This->play_cnt < 30)//5
	{
		This->play_cnt++;
		if(0 == (This->play_cnt%2))
		{
			dsk_reg_flush();
		}
	}
	else if(This->play_cnt < 50)//6
	{
		This->play_cnt++;
		if(0 == (This->play_cnt%3))
		{
			dsk_reg_flush();
		}
	}
	else
	{
		This->play_cnt++;
		if(0 == (This->play_cnt%3))
		{
			dsk_reg_flush();
		}
	}

	if(This->play_cnt >= 50)
	{
		This->play_cnt = 0;
	}
	*/
	return EPDK_OK;
}
static __s32 music_player_set_play_pause_onview(music_player_ctrl_t *this)
{
	if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
	{
		robin_set_cmd_pause();
		//__app_music_audio_mute(0);
		DrawMusicPlayStatusButton(MUSIC_PAUSE);
		//DrawMusicPlayPauseButton(PRESSED);
		//DrawMusicPlayStatusButton(MUSIC_PAUSE);
	}
	else
	{
		robin_set_cmd_play();
		//__app_music_audio_mute(1);
		DrawMusicPlayStatusButton(MUSIC_PLAY);
		//DrawMusicPlayPauseButton(PRESSED);
		//DrawMusicPlayStatusButton(MUSIC_PLAYING);
	}

	return EPDK_OK;
}


static __s32 music_explorer_on_enter_key(__gui_msg_t *msg)
{
	__s32 ret = 0;
	music_player_ctrl_t *This;
	music_list_ctrl_t *ListCtrl;
	char *filename;
	__u32 index;
	//music_player_ctrl_t *this_t;
	//this_t = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	{
		if(This->hdigit_layer)
		{
			__s32 ndigit_song_num = This->ndigit_song_num;
			music_player_delete_digit_layer(msg->h_deswin);
			music_player_set_play_index(This, ndigit_song_num);
			return EPDK_OK;
		}
	}
	ListCtrl = This->list_ctrl;
	filename = This->play_file_info.filename;
	index = robin_npl_get_cur();//删除断点信息
	__music_delete_breakpoint_info(index);
	ret = get_music_list_focus_item_full_filename(ListCtrl, filename);

	if(ret != EPDK_FAIL)
	{
		ListCtrl->play_index = ret;
	}
	else
	{
		ListCtrl->play_index = 0;
		return EPDK_FAIL;
	}

	{
		//处理当前歌曲的暂停播放
		char cur_playing_file[RAT_MAX_FULL_PATH_LEN];
		// __cedar_status_t cedar_status;
		music_player_ctrl_t *player_ctrl;
		player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

		if(NULL == player_ctrl)
		{
			return EPDK_FAIL;
		}

		eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
		ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
		__wrn("ret = %d, cur_playing_file=%s", ret, cur_playing_file);

		//cedar_status = robin_get_fsm_status();
		//__wrn("cedar_status=%d\n", cedar_status);
		if(EPDK_OK == ret
		    && 0 == eLIBs_stricmp(filename, cur_playing_file))
		{
			if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
			{
				DrawMusicPlayStatusButton(MUSIC_PAUSE);
				robin_set_cmd_pause();
				app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
			}
			else
			{
				music_rr_flag = 0;
				This->m_ff_fb_speed = 0;
				music_ff_fb_layer_delete(msg);
				DrawMusicPlayStatusButton(MUSIC_PLAY);
				app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
				robin_set_cmd_play();
			}

			//DrawMusicPlayPauseButton(UNFOCUSED);
			return EPDK_OK;
		}
	}

	music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
	//music_play_file(This, filename, 0);
	ret = music_play_file(This, filename, 0);
	__wrn("ret=%d\n", ret);

	if(EPDK_OK == ret)
	{
		//music_save_breakpoint(This);
	}

	return EPDK_OK;
}

static __s32 music_digit_layer_draw(__gui_msg_t *msg)
{
	music_player_ctrl_t *player_ctrl;
	__s32 total_song_num;
	char text[256];
	__s32 i;
	__s32 j;
	RECT rect;
	GUI_RECT gui_rect;
	__s32 X = 0, Y = 0;
	void *pic_buf = NULL;
	H_LYR	*phLyr;
	player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	phLyr = &player_ctrl->hdigit_layer;

	if(NULL == player_ctrl)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	if(NULL == *phLyr)
	{
		__wrn("digit layer is null...\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(*phLyr))
	{
		__wrn("digit layer is not on...\n");
		return EPDK_FAIL;
	}

	total_song_num = robin_npl_get_total_count();

	if(0)//(player_ctrl->ndigit_song_num > total_song_num)//112358 如果超出范围也显示歌曲编号
	{
		dsk_langres_get_menu_text(STRING_MUSIC_NO_THIS_SONG, text, sizeof(text));
	}
	else
	{
		eLIBs_memset(text, 0, sizeof(text));
		eLIBs_int2str_dec(player_ctrl->ndigit_song_num, text);
	}

	GUI_LyrWinSel(*phLyr);
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
#ifdef ID_MOVIE_NUMLOCK_BG_BMP
	{
		pic_buf = music_get_icon_res(ID_MUSIC_NUMLOCK_BG);

		if(pic_buf != NULL)
		{
			GUI_BMP_Draw(pic_buf, 0, 0);
		}

#if MASK_SysFlags_ShiftNum
		{
			reg_system_para_t *para;
			para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

			if(para)
			{
				if(para->m_maskFlags & MASK_SysFlags_ShiftNum)
				{
					para->m_maskFlags &= ~MASK_SysFlags_ShiftNum;
				}
			}
		}
#endif
	}
#else
	{
		GUI_Clear();
	}
#endif

	if(msg->dwAddData1 != GUI_MSG_KEY_NUMLOCK)
	{
		__wrn("text=%s\n", text);
		i = eLIBs_strlen(text);
		X = (W_NumLock_Bg - (i * W_Num)) / 2;
		Y = H_NumLock_Bg - H_NumLock_Num + (H_NumLock_Num - H_Num) / 2;

		for(i = 0; i < 6; i++)
		{
			if(text[i] == 0)
			{
				break;
			}

			pic_buf = music_get_icon_res(ID_MUSIC_NUM0 + (text[i] - '0'));

			if(pic_buf != NULL)
			{
				GUI_BMP_Draw(pic_buf, X, Y);
			}

			X += W_Num;
		}
	}

	GUI_LyrWinSetTop(*phLyr);
	return EPDK_OK;
}

void music_display_digit(__gui_msg_t *msg)
{
	music_player_ctrl_t *This;
	H_LYR	*phLyr;
	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	phLyr = &This->hdigit_layer;
	music_ff_fb_layer_delete(msg);

	if(NULL == *phLyr)
	{
		RECT rect;
		__s32 width, height;
		__s32 scn_width, scn_height;
		dsk_display_get_size(&scn_width, &scn_height);
		width = W_NumLock_Bg;
		height = H_NumLock_Bg;
		rect.x = (scn_width - width);
		rect.y = scn_height - height;
		rect.width = width;
		rect.height = height;
		*phLyr = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_ON, "");

		if(NULL == *phLyr)
		{
			__wrn("digit layer create fail...\n");
		}
		else
		{
			__wrn("digit layer create ok...\n");
		}
	}

	__wrn("player_ctrl->digit_timmer_id=%d\n", This->digit_timmer_id);
	__wrn("msg->h_deswin=0x%x\n", msg->h_deswin);

	if(!GUI_IsTimerInstalled(msg->h_deswin, This->digit_timmer_id))
	{
		__wrn("create timmer...\n");
		This->ndigit_song_num = 0;
		GUI_SetTimer(msg->h_deswin, This->digit_timmer_id, TIMEOUT_ID_TIMER_DIGIT, NULL);//2.5秒
	}
	else
	{
		__wrn("reset timmer...\n");
		GUI_ResetTimer(msg->h_deswin, This->digit_timmer_id, TIMEOUT_ID_TIMER_DIGIT, NULL);//2.5秒
	}

	if(msg->dwAddData1 != GUI_MSG_KEY_NUMLOCK)
	{
		__s32 cur_val;
		cur_val = msg->dwAddData1 - GUI_MSG_KEY_NUM0;
		__wrn("cur_val=%d\n", cur_val);

		if(1)
		{
			//if(This->ndigit_song_num < 100000)
			{
				This->ndigit_song_num *= 10;
				This->ndigit_song_num += cur_val;
				This->ndigit_song_num %= 1000000;	// 低6位有效
			}
		}
		else
		{
			__wrn("song num exceed...\n");
		}
	}

	__wrn("This->ndigit_song_num=%d\n", This->ndigit_song_num);
	music_digit_layer_draw(msg);
}

__s32 music_proc_ff_fb(__gui_msg_t *msg)
{
	music_player_ctrl_t *This;
	__cedar_status_t fsm_sta;
	fsm_sta = robin_get_fsm_status();

	switch(fsm_sta)
	{
		case CEDAR_STAT_PAUSE :
		case CEDAR_STAT_STOP :
			return EPDK_FAIL;

		default :
			break;
	}

	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(msg->dwAddData1 == GUI_MSG_KEY_FF_MULTI)
	{
		if(This->m_ff_fb_speed <= 0)
		{
			This->m_ff_fb_speed = 1;
		}
		else
		{
			This->m_ff_fb_speed++;
		}

		if(This->m_ff_fb_speed > MAX_FF_FB_MULTI)
		{
			This->m_ff_fb_speed = 0;
		}
	}
	else if(msg->dwAddData1 == GUI_MSG_KEY_FB_MULTI)
	{
		if(This->m_ff_fb_speed >= 0)
		{
			This->m_ff_fb_speed = -1;
		}
		else
		{
			This->m_ff_fb_speed--;
		}

		if(This->m_ff_fb_speed < -MAX_FF_FB_MULTI)
		{
			This->m_ff_fb_speed = 0;
		}
	}

	__wrn("This->m_ff_fb_speed=%d\n", This->m_ff_fb_speed);
	{
		const __u16 c_ff_fb_speed_tab[] = {8, 16, 32};

		if(This->m_ff_fb_speed > 0)
		{
			switch(fsm_sta)
			{
				case CEDAR_STAT_RR :
					robin_set_cmd_play();

				case CEDAR_STAT_PLAY :
					music_rr_flag = 0;
					robin_set_cmd_ff();

				default :
					break;
			}

			robin_set_ff_rr_speed(c_ff_fb_speed_tab[This->m_ff_fb_speed]);
		}
		else if(This->m_ff_fb_speed < 0)
		{
			switch(fsm_sta)
			{
				case CEDAR_STAT_FF :
					robin_set_cmd_play();

				case CEDAR_STAT_PLAY :
					music_rr_flag = 1;
					robin_set_cmd_rr();

				default :
					break;
			}

			robin_set_ff_rr_speed(c_ff_fb_speed_tab[-This->m_ff_fb_speed]);
		}
		else
		{
			switch(fsm_sta)
			{
				case CEDAR_STAT_FF :
				case CEDAR_STAT_RR :
					music_rr_flag = 0;
					robin_set_cmd_play();

				default :
					break;
			}
		}
	}
	return EPDK_OK;
}

__s32 music_ff_fb_layer_draw(__gui_msg_t *msg)
{
	char     text[256];
	music_player_ctrl_t *This;
	H_LYR	*phLyr;
	char 	t_str[] = " : X1";
	__wrn("&&&&&&&& music_ff_fb_layer_draw \n");
	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	phLyr = &This->m_hLyr_ff_fb;

	if(!This)
	{
		__err("This is null...\n");
		return EPDK_OK;
	}

	if(NULL == *phLyr)
	{
		__wrn("digit layer is null...\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(*phLyr))
	{
		__wrn("digit layer is not on...\n");
		return EPDK_FAIL;
	}

	eLIBs_memset(text, 0, sizeof(text));

	if(This->m_ff_fb_speed > 0)
	{
		dsk_langres_get_menu_text(STRING_MOVIE_FF, text, sizeof(text));
		t_str[4] = '0' + This->m_ff_fb_speed;
		strcat(text, t_str);
	}
	else if(This->m_ff_fb_speed < 0)
	{
		dsk_langres_get_menu_text(STRING_MOVIE_RR, text, sizeof(text));
		t_str[4] = '0' - This->m_ff_fb_speed;
		strcat(text, t_str);
	}
	else
	{
		dsk_langres_get_menu_text(STRING_MOVIE_ST_PLAT, text, sizeof(text));
	}

	GUI_LyrWinSel(*phLyr);
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_Clear();
	GUI_SetFont(SWFFont);
	GUI_SetColor(GUI_RED);
	GUI_CharSetToEncode(dsk_get_langres_charset());
	GUI_SetBkColor(0x0);
	{
		GUI_RECT gui_rect;
		gui_rect.x0 = 0;
		gui_rect.y0 = 0;
		gui_rect.x1 = W_NumLock_Bg - 1;
		gui_rect.y1 = H_NumLock_Num - 1;
		GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	}
	GUI_LyrWinSetTop(*phLyr);
	return EPDK_OK;
}

void music_display_ff_fb(__gui_msg_t *msg)
{
	H_LYR	*phLyr;
	music_player_ctrl_t *This;

	if(music_proc_ff_fb(msg)  == EPDK_FAIL)
	{
		return;
	}

	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	phLyr = &This->m_hLyr_ff_fb;
	music_player_delete_digit_layer(msg->h_deswin);

	if(NULL == *phLyr)
	{
		RECT rect;
		__s32 width, height;
		__s32 scn_width, scn_height;
		dsk_display_get_size(&scn_width, &scn_height);
		width = W_NumLock_Bg;
		height = H_NumLock_Num;
		rect.x = (scn_width - width);
		rect.y = scn_height - height;
		rect.width = width;
		rect.height = height;
		*phLyr = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_ON, "hLyr_ff_fb");

		if(NULL == *phLyr)
		{
			__wrn("digit layer create fail...\n");
		}
		else
		{
			__wrn("digit layer create ok...\n");
		}
	}

#if TIMEROUT_ID_TIMER_HLYR_FF_FB

	if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_HLYR_FF_FB))
	{
		GUI_SetTimer(msg->h_deswin, ID_TIMER_HLYR_FF_FB, TIMEROUT_ID_TIMER_HLYR_FF_FB, NULL);
	}
	else
	{
		GUI_ResetTimer(msg->h_deswin, ID_TIMER_HLYR_FF_FB, TIMEROUT_ID_TIMER_HLYR_FF_FB, NULL);
	}

#endif
	music_ff_fb_layer_draw(msg);
}

__s32 music_ff_fb_layer_delete(__gui_msg_t *msg)
{
	music_player_ctrl_t *This;
	__wrn("&&&&&&&& music_ff_fb_layer_delete\n");
	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!This)
	{
		__err("This is null...\n");
		return EPDK_OK;
	}

	if(This->m_hLyr_ff_fb)
	{
		GUI_LyrWinDelete(This->m_hLyr_ff_fb);
		This->m_hLyr_ff_fb = NULL;
	}

#if TIMEROUT_ID_TIMER_HLYR_FF_FB

	if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_HLYR_FF_FB))
	{
		GUI_KillTimer(msg->h_deswin, ID_TIMER_HLYR_FF_FB);
	}

#endif
	__wrn("music_ff_fb_layer_delete end...\n");
	return EPDK_OK;
}

#if SP_FavoriteFiles
static __s32 music_shift_playlist(__gui_msg_t *msg)
{
	// 切换播放列表
	music_player_ctrl_t *this;
	H_WIN    h_dialog = NULL;
	char file_path[RAT_MAX_FULL_PATH_LEN];
	char filename[1024] = {0};
	HRAT hrat;
	__s32 ret;
	reg_music_para_t *music_last_para = NULL;
	__s32 index;
	H_WIN hDstWin;
	__s32 count;
	__s32 i, j;
	HRATNPL				h_rat_npl;//new+++
	hDstWin = GUI_WinGetParent(msg->h_deswin);
	//DebugPrintf("----------music_explorer_lovely_play_start---\n");
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(this->media_type == RAT_MEDIA_TYPE_AUDIO)
	{
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STROE_UP_PLAY};
			//DebugPrintf("-------default_dialog--------\n");
			default_dialog(h_dialog, hDstWin, APP_STORE_UP_PLAY_DLG_ID, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);

			if(h_dialog)
			{
				app_dialog_destroy(h_dialog);
				h_dialog = NULL;
			}
		}
		//DebugPrintf("-------start_rat_open_ex--------\n");
		rat_set_modify_flag(RAT_MEDIA_TYPE_FAVOR_AUDIO, 1);
		hrat = rat_open_ex(this->root_type, RAT_MEDIA_TYPE_FAVOR_AUDIO, 0);

		if(hrat == NULL)
		{
			//DebugPrintf("EPDK_FAIL\n");
			return NULL;
		}

		count = rat_get_cur_scan_cnt(hrat);

		if(count == 0)
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STORE_UP_PLAY_NULL};
			//DebugPrintf("-------default_dialog--------\n");
			default_dialog(h_dialog, hDstWin, APP_STORE_UP_PLAY_DLG_ID, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);

			if(h_dialog)
			{
				app_dialog_destroy(h_dialog);
				h_dialog = NULL;
			}

			rat_close(hrat);
			return EPDK_FAIL;
		}

#if 0
		//DebugPrintf("-------rat_get_cur_scan_cnt--------\n");
		{
			rat_entry_t ItemInfo;
			eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
			//DebugPrintf("-------rat_get_item_info_by_index--------\n");
			ret = rat_get_item_info_by_index(hrat, 0, &ItemInfo);

			if(ret == EPDK_OK)
			{
				eLIBs_memset(file_path, 0, sizeof(file_path));
				eLIBs_strcpy(file_path, ItemInfo.Path);
				//DebugPrintf("-------file_path=%s--------\n", file_path);
				{
					HRATNPL hRatNpl;
					hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);

					if(!hRatNpl)
					{
						//DebugPrintf("rat_npl_open fail \n");
					}

					ret = rat_npl_file2index(hRatNpl , file_path);
				}
				//DebugPrintf("-------------ret=%d", ret);

				if(ret != EPDK_FAIL)
				{
					this->list_ctrl->play_index = ret;
				}

				//DebugPrintf("---------------------\n");
				music_list_set_focus_item(this->list_ctrl, this->list_ctrl->play_index);
				MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);
				//DebugPrintf("----------filename=%s-----------\n", filename);
				music_play_file(this, filename, 0);
			}
		}
#endif
		//DebugPrintf("-------rat_close--------\n");
		rat_close(hrat);
		this->media_type = RAT_MEDIA_TYPE_FAVOR_AUDIO;
	}
	else
	{
		//DebugPrintf("-------stro_up_play_flag = 0--------\n");
		this->media_type = RAT_MEDIA_TYPE_AUDIO;
		{
			__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STROE_UP_PLAY_CANCEL};
			//DebugPrintf("-------default_dialog--------\n");
			default_dialog(h_dialog, hDstWin, APP_STORE_UP_PLAY_DLG_ID, ADLG_OK, lang_id);
			esKRNL_TimeDly(100);

			if(h_dialog)
			{
				app_dialog_destroy(h_dialog);
				h_dialog = NULL;
			}
		}
	}

	return EPDK_OK;
}

static __s32 music_process_favor_file_for_paly(__gui_msg_t *msg, __s32 next_or_prev)
{
	__s32 next_index;
	HRAT hrat;
	HRATNPL hnpl;
	char filename[1024] = {0};
	__s32 index = 0, ret = 0;
	music_player_ctrl_t *this;
	__wrn("*********************RAT_MEDIA_TYPE_FAVOR_AUDIO_for_play******************\n");
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	hrat = rat_open_ex(this->list_ctrl->root_type, RAT_MEDIA_TYPE_FAVOR_AUDIO, 0);

	if(hrat == NULL)
	{
		//DebugPrintf("EPDK_FAIL\n");
		return NULL;
	}

	robin_get_fsm_cur_file(filename , sizeof(filename));
	__wrn("********filename=%s******************\n", filename);
	rat_set_file_for_play(hrat, filename);
	hnpl = rat_npl_open(RAT_MEDIA_TYPE_FAVOR_AUDIO);

	if(hnpl == NULL)
	{
		//DebugPrintf("EPDK_FAIL\n");
		return NULL;
	}

	rat_npl_set_play_mode(hnpl, RAT_PLAY_MODE_ROTATE_ALL);

	if(next_or_prev == 1)
	{
		next_index = rat_npl_get_next(hnpl);
		//DebugPrintf("********next_index=%d******************\n", next_index);
	}
	else
	{
		next_index = rat_npl_get_prev(hnpl);
		//DebugPrintf("********next_index=%d******************\n", next_index);
	}

	rat_npl_index2file(hnpl, next_index, filename);
	__wrn("********filename=%s******************\n", filename);
	index = rat_npl_file2index(this->list_ctrl->rat_handle, filename);
	__wrn("********index=%d******************\n", index);
	__wrn("********filename=%s******************\n", filename);
	rat_set_modify_flag(RAT_MEDIA_TYPE_FAVOR_AUDIO, 1);
	hrat = rat_open_ex(this->root_type, RAT_MEDIA_TYPE_FAVOR_AUDIO, 0);

	if(hrat == NULL)
	{
		//DebugPrintf("EPDK_FAIL\n");
		return NULL;
	}

	{
		rat_entry_t ItemInfo;
		eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
		__wrn("-------rat_get_item_info_by_index--------\n");
		ret = rat_get_item_info_by_index(hrat, next_index, &ItemInfo);

		if(ret == EPDK_OK)
		{
			eLIBs_memset(filename, 0, sizeof(filename));
			eLIBs_strcpy(filename, ItemInfo.Path);
			__wrn("-------file_path=%s--------\n", filename);
			{
				HRATNPL hRatNpl;
				hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);

				if(!hRatNpl)
				{
					__msg("rat_npl_open fail \n");
				}

				ret = rat_npl_file2index(hRatNpl , filename);
			}
			__wrn("-------------ret=%d", ret);

			if(ret != EPDK_FAIL)
			{
				this->list_ctrl->play_index = ret;
			}

			__wrn("---------------------\n");
			music_list_set_focus_item(this->list_ctrl, this->list_ctrl->play_index);
			MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);
			__wrn("----------filename=%s-----------\n", filename);
			music_play_file(this, filename, 0);
		}
	}

	rat_npl_close(hnpl);
	rat_close(hrat);
}
//lvoely处理函数

static __s32 music_favoriefile__add_or_del(__gui_msg_t *msg)
{
	// 收藏
	music_player_ctrl_t *player_ctrl = NULL;
	music_list_ctrl_t *ListCtrl = NULL;
	char file_path[RAT_MAX_FULL_PATH_LEN];
	H_WIN hDstWin;
	char *filename = NULL;
	HRAT hrat;
	__listbar_scene_t scene;
	H_WIN    h_dialog = NULL;
	int ret;
	rat_entry_t ItemInfo;
	eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
	hDstWin = GUI_WinGetParent(msg->h_deswin);
	player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(player_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	ListCtrl = player_ctrl->list_ctrl;

	if(ListCtrl == NULL)
	{
		return EPDK_FAIL;
	}

	//__wrn("music_explorer_lovely_start-\n");
	//filename = player_ctrl->play_file_info.filename;
	__wrn("player_ctrl->play_file_info.filename=%s\n", player_ctrl->play_file_info.filename);
	/*{
		LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
		//DebugPrintf("listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
		hrat = rat_open_ex(player_ctrl->root_type, RAT_MEDIA_TYPE_AUDIO, 0);
		if( !hrat )
		{
			//DebugPrintf( "rat_npl_open fail \n" );
		}
		MusicGetListItemFileFullPath(hrat,scene.focus_id, filename);
	}*/
	LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
	__wrn("listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
	{
		/*HRATNPL hRatNpl;
		hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);
		if( !hRatNpl )
		{
			__msg( "rat_npl_open fail \n" );
		}*/
		//rat_npl_index2file(hRatNpl ,scene.focus_id, filename);
		//list_ctrl->last_focused_id = rat_npl_get_cur(hRatNpl);
	}
	rat_set_modify_flag(RAT_MEDIA_TYPE_AUDIO, 1);
	hrat = rat_open_ex(player_ctrl->root_type, RAT_MEDIA_TYPE_AUDIO, 0);

	if(hrat == NULL)
	{
		__wrn("EPDK_FAIL\n");
		return NULL;
	}

	{
		__wrn("-------rat_get_item_info_by_index--------\n");
		ret = rat_get_item_info_by_index(hrat, scene.focus_id, &ItemInfo);

		if(ret == EPDK_OK)
		{
			__msg("-------rat_get_item_info_by_index_ok--------\n");
			eLIBs_memset(file_path, 0, sizeof(file_path));
			eLIBs_strcpy(file_path, ItemInfo.Path);
			__wrn("-------file_path=%s--------\n", file_path);
		}
	}

	rat_close(hrat);
	__wrn("filename=%s\n", file_path);
	filename = file_path;
	{
		hrat = rat_open_ex(player_ctrl->root_type, RAT_MEDIA_TYPE_FAVOR_AUDIO, 0);

		if(hrat == NULL)
		{
			//DebugPrintf("EPDK_FAIL\n");
			return NULL;
		}

		__wrn("FileName=%s\n", filename);
		ret = rat_is_favor_file(hrat, filename);

		if(ret == 1) //已经收藏 则删除收藏
		{
			{
				__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DEL};
				////DebugPrintf("default_dialog\n");
				default_dialog(h_dialog, hDstWin, APP_STORE_UP_DLG_ID, ADLG_OK, lang_id);
				esKRNL_TimeDly(100);

				if(h_dialog)
				{
					app_dialog_destroy(h_dialog);
					h_dialog = NULL;
				}
			}
			{
				//__wrn("-------------------start_rat_del_favor_file\n");
				__wrn("FileName=%s\n", filename);
				ret = rat_del_favor_file(hrat, filename);

				if(ret == EPDK_FAIL)
				{
					__wrn("EPDK_FAIL\n");
					rat_close(hrat);
					return EPDK_FAIL;
				}

				rat_close(hrat);
				show_music_list(player_ctrl->list_ctrl);
				return EPDK_OK;
			}
		}
		else //  还没收藏则向下执行收藏
		{
			{
				__wrn("start_rat_add_favor_file\n");
				{
					__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_STROE_UP};
					////DebugPrintf("default_dialog\n");
					default_dialog(h_dialog, hDstWin, APP_STORE_UP_DLG_ID, ADLG_OK, lang_id);
					esKRNL_TimeDly(100);

					if(h_dialog)
					{
						app_dialog_destroy(h_dialog);
						h_dialog = NULL;
					}
				}
				__wrn("filename=%s\n", filename);
				ret = rat_add_favor_file(hrat, filename);
				__wrn("rat_add_favor_file_OVER...\n");

				if(ret == EPDK_FAIL)
				{
					__wrn("EPDK_FAIL\n");
					rat_close(hrat);
					return EPDK_FAIL;
				}

				rat_close(hrat);
			}
			show_music_list(player_ctrl->list_ctrl);
			return EPDK_OK;
		}
	}
}
#endif

static __s32 music_explorer_key_proc(__gui_msg_t *msg)
{
	music_list_ctrl_t *ListCtrl;
	music_player_ctrl_t *This;
	static __u32 last_key = 0;
	This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	//music_player_ctrl_t *player_ctrl = NULL;
	//player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
	{
		//GUI_ResetTimer(msg->h_deswin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);
		switch(msg->dwAddData1)
		{
				//case GUI_MSG_KEY_VDEC:
				//ExplorerListWinOnPreviousPage(list_para);
				//		break;
			case GUI_MSG_KEY_ENTER:
				music_explorer_on_enter_key(msg);
				break;

				//			case GUI_MSG_KEY_VADD:
				//				last_key = GUI_MSG_KEY_VADD;
				//				break;
			case GUI_MSG_KEY_MENU:
				last_key = GUI_MSG_KEY_MENU;
				//case GUI_MSG_KEY_LONGMENU:
				//music_explorer_on_menu_key(msg);
				//music_send_command(msg->h_deswin, 0, EXPLR_SW_TO_MAIN, 0);
				break;

			case GUI_MSG_KEY_EQ:
				music_player_set_EQ_mode(This);
				break;

			case GUI_MSG_KEY_REPEATE:
				music_player_set_play_mode(This);
				break;

			case GUI_MSG_KEY_ESCAPE:
				music_send_command(msg->h_deswin, 0, SWITCH_TO_MMENU, 0);
				break;

			case GUI_MSG_KEY_RISE:
				break;

			case GUI_MSG_KEY_RR:
			case GUI_MSG_KEY_LONGLEFT:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_PLAY == sta)
				{
					robin_set_cmd_rr();
					music_rr_flag = 1;
					This->m_ff_fb_speed = -1;
				}

				break;
			}

			case GUI_MSG_KEY_SUBMENU:
			{
				music_key_count = 5;

				if(music_key_count == 5)
				{
					music_key_count = 0;
					music_explorer_on_menu_key(msg);
				}

				break;
			}

			case GUI_MSG_KEY_LONGMENU:
			{
				music_key_count++;

				if(music_key_count == 5)
				{
					music_key_count = 0;
					music_explorer_on_menu_key(msg);
				}

				break;
			}

			case GUI_MSG_KEY_FF:
			case GUI_MSG_KEY_LONGRIGHT:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_PLAY == sta)
				{
					robin_set_cmd_ff();
					music_rr_flag = 0;
					This->m_ff_fb_speed = 1;
				}

				break;
			}

			case GUI_MSG_KEY_NUMLOCK:
			{
				if((KEY_DOWN_ACTION == msg->dwAddData2) && (NULL == This->hdigit_layer))
				{
					music_display_digit(msg);
				}

				break;
			}

			case GUI_MSG_KEY_FF_MULTI :
			case GUI_MSG_KEY_FB_MULTI :
			{
				if(KEY_DOWN_ACTION == msg->dwAddData2)
				{
					music_display_ff_fb(msg);
				}

				break;
			}

#if SP_FavoriteFiles

			case GUI_MSG_KEY_LONGNUM1:
			case GUI_MSG_KEY_FAVORFILE:
			{
				if(last_key == GUI_MSG_KEY_NUM1 || msg->dwAddData1 == GUI_MSG_KEY_FAVORFILE)
				{
					__wrn("favor File\n");

					if(This->media_type == RAT_MEDIA_TYPE_FAVOR_AUDIO)
					{
						break;
					}

					music_favoriefile__add_or_del(msg);
				}

				break;
			}

			case GUI_MSG_KEY_LONGNUM0:
			case GUI_MSG_KEY_FAVORPLAY:
			{
				if(last_key == GUI_MSG_KEY_NUM0 || msg->dwAddData1 == GUI_MSG_KEY_FAVORPLAY)
				{
					__s32 ret;
					__wrn("favor play\n");
					ret = music_shift_playlist(msg);

					if(ret == EPDK_FAIL)
					{
						break;
					}

					if(This->media_type == RAT_MEDIA_TYPE_FAVOR_AUDIO)
					{
						music_play_status = RAT_MEDIA_TYPE_FAVOR_AUDIO;
					}
					else
					{
						music_play_status = RAT_MEDIA_TYPE_AUDIO;
					}

					{
						__gui_msg_t my_msg;
						//my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
						my_msg.h_deswin = msg->h_deswin;
						my_msg.h_srcwin = msg->h_deswin;
						my_msg.id = GUI_MSG_COMMAND;
						my_msg.dwAddData1 = MUSICPLAYER_FAVORPLAY;//发送消息给音乐AP要进入背景播放状态，释放场景
						//GUI_SendNotifyMessage(&msg);
						GUI_SendMessage(&my_msg);
					}
				}

				break;
			}

#endif

			default:
				break;
		}

		last_key = msg->dwAddData1;
	}
	else if(KEY_UP_ACTION == msg->dwAddData2)
	{
		switch(msg->dwAddData1)
		{
				//case GUI_MSG_KEY_ENTER:	//因为现在不支持定时长按键，因此
				//只能判断Menu Up时才会响应close消息
			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_DOWN:
				//case GUI_MSG_KEY_LONGDOWN:
				//case GUI_MSG_KEY_VDEC:
				__msg("----explorer list window on UP key----\n");
#if 1

				if(GUI_MSG_KEY_VADD == msg->dwAddData1
				    || GUI_MSG_KEY_VDEC == msg->dwAddData1)
				{
					if(1 == msg->dwReserved)
					{
						//                            break;
					}
				}

				music_player_set_play_next(This);//下一曲直接播放
#else
				music_explorer_on_next_key(msg);
#endif
				break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_UP:
				//case GUI_MSG_KEY_LONGUP:
				//case GUI_MSG_KEY_VADD:
#if 1
				if(GUI_MSG_KEY_VADD == msg->dwAddData1
				    || GUI_MSG_KEY_VDEC == msg->dwAddData1)
				{
					if(1 == msg->dwReserved)
					{
						//break;
					}
				}

				music_player_set_play_previous(This);//下一曲直接播放
#else
				music_explorer_on_previous_key(msg);
#endif
				break;

			case GUI_MSG_KEY_PLAY_PAUSE:
			case GUI_MSG_KEY_LONGPLAY_PAUSE:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
				{
					DrawMusicPlayStatusButton(MUSIC_PAUSE);
					robin_set_cmd_pause();
					app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
				}
				else
				{
					DrawMusicPlayStatusButton(MUSIC_PLAY);
					app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
					robin_set_cmd_play();
				}

				break;
			}

			case GUI_MSG_KEY_LONGLEFT:
#if SP_LPressUpForFFRR
				break;
#endif

			case GUI_MSG_KEY_RR:
			case GUI_MSG_KEY_LONGRR:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_RR == sta)
				{
					robin_set_cmd_play();
				}

				break;
			}

			case GUI_MSG_KEY_LONGRIGHT:
#if SP_LPressUpForFFRR
				break;
#endif

			case GUI_MSG_KEY_FF:
			case GUI_MSG_KEY_LONGFF:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_FF == sta)
				{
					robin_set_cmd_play();
				}

				break;
			}

			case GUI_MSG_KEY_NEXT:
			{
#if 1
				music_player_set_play_next(This);//下一曲直接播放
#else
				music_explorer_on_next_key(msg);
#endif
				break;
			}

			case GUI_MSG_KEY_PREV:
			{
#if 1
				music_player_set_play_previous(This);//上一曲直接播放
#else
				music_explorer_on_previous_key(msg);
#endif
				break;
			}

			case GUI_MSG_KEY_LR:
			case GUI_MSG_KEY_LONGLR:
			{
				__wrn("msg->dwAddData2=%d\n", msg->dwAddData2);
				{
					if(This->music_layer)
					{
						__here__;

						if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(This->music_layer))
						{
							__s32 channel;
							GUI_LyrWinSel(This->music_layer);
							channel = robin_get_channel();
							channel = (channel + 1) % 3;
							__wrn("channel=%d\n", channel);
							robin_set_channel(channel);
							DrawMusicAcompStatus(channel);
						}
					}
				}
				break;
			}

			case GUI_MSG_KEY_MENU:
				if(last_key == GUI_MSG_KEY_MENU)
				{
#if 1
#if (0==SUPPORT_MUSIC_BG_MODE)
					robin_set_cmd_pause();
#endif

					if(robin_get_fsm_status() != CEDAR_STAT_PLAY)			//关闭音乐
					{
						music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
					}
					else
					{
						//退出时，正在播放则进入背景播放状?
						//__wait__;
						//music_send_command(msg->h_deswin, GUI_MSG_COMMAND, EXPLR_SW_TO_MAIN, MUSICPLAYER_BACKGROUND);
						{
							__gui_msg_t my_msg;
							//my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
							my_msg.h_deswin = msg->h_deswin;
							my_msg.h_srcwin = msg->h_deswin;
							my_msg.id = GUI_MSG_COMMAND;
							my_msg.dwAddData1 = MUSICPLAYER_BACKGROUND;//发送消息给音乐AP要进入背景播放状态，释放场景
							//GUI_SendNotifyMessage(&msg);
							GUI_SendMessage(&my_msg);
						}
					}

#else
					{
						__gui_msg_t my_msg;
						eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
						my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
						my_msg.h_srcwin = msg->h_deswin;
						my_msg.id = DSK_MSG_HOME;
						my_msg.dwAddData1 = 0;
						//GUI_SendNotifyMessage(&msg);
						GUI_SendMessage(&my_msg);
					}
#endif
				}

				break;

			case GUI_MSG_KEY_LONGMENU:
				if(last_key == GUI_MSG_KEY_MENU)
				{
					music_explorer_on_menu_key(msg);
					__msg("start volume bar\n");
				}

				break;

			case -1://GUI_MSG_KEY_LONGVADD:
				if(last_key == GUI_MSG_KEY_VADD)
				{
					music_player_ctrl_t *player_ctrl = NULL;
					player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
					MusicVolumeBarCreate(player_ctrl);
				}

				break;

			case GUI_MSG_KEY_NUM0:
			case GUI_MSG_KEY_NUM1:
			case GUI_MSG_KEY_NUM2:
			case GUI_MSG_KEY_NUM3:
			case GUI_MSG_KEY_NUM4:
			case GUI_MSG_KEY_NUM5:
			case GUI_MSG_KEY_NUM6:
			case GUI_MSG_KEY_NUM7:
			case GUI_MSG_KEY_NUM8:
			case GUI_MSG_KEY_NUM9:
			{
				music_display_digit(msg);
				break;
			}
		}

		last_key = 0;
	}

	return EPDK_OK;
}


//检测播放状态,并根据playmode获得相关的文件,启动播放器
static __s32 music_player_check_play_status_on_timer(music_player_ctrl_t *this)
{
	__cedar_status_t  status;
	__s32 index;
	char *filename;
	music_list_ctrl_t *ListCtrl = NULL;
	__listbar_scene_t scene;
	status = robin_get_fsm_status();

	if(status == CEDAR_STAT_STOP)
	{
		if(this->menu_mode == MUSIC_EXPLORER_MODE)
		{
			if(this->h_lyrics != NULL)
			{}
			else
			{
				SPCTRM_Clear(0);		//显示最大值
			}
		}

		DrawMusicPlayStatusButton(MUSIC_PAUSE);

		if(this->play_mode == RAT_PLAY_MODE_ONLY_ONCE)
		{
			return EPDK_OK;
		}

		//if(this->play_mode == RAT_PLAY_MODE_RANDOM)
		{
			//return EPDK_OK;
		}
#if SP_FavoriteFiles

		if(this->media_type == RAT_MEDIA_TYPE_AUDIO)
		{
#endif

			if(music_rr_flag)
			{
				index = robin_npl_get_cur();
			}
			else
			{
				{
					__u32 index2;
					index2 = robin_npl_get_cur();
					__music_delete_breakpoint_info(index2);//删除上一首断点信息
				}
				index = robin_npl_get_next();
			}

#if SP_FavoriteFiles
		}
		else
		{
			music_player_set_play_next(this);
			return EPDK_OK;
		}

#endif
		__msg("\n\n\n--------------------------------robin npl get index = %d\n", index);

		if(index != -1)
		{
#if 0

			if(this->list_ctrl != NULL)
			{
				ListCtrl = this->list_ctrl;
				ListCtrl->play_index = index;
			}

			filename = this->play_file_info.filename;
			robin_npl_index2file(index, filename);
			__msg("\n\n\n--------------------------------robin npl get filename= %s\n", filename);
			//music_list_set_item_scene(ListCtrl, &(ListCtrl->listbar_play_scene));
			music_play_file(this, filename, 0);
			DrawMusic_playing(this->font);//更新播放歌曲

			if(this->play_mode == RAT_PLAY_MODE_RANDOM)
			{
				//music_play_file(this, filename);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				music_list_set_focus_item(ListCtrl, index);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				ListCtrl->play_index = scene.focus_id;
				return EPDK_OK;
			}

			if(this->visiable)		//背景播放则不显示listbar
			{
				if((this->play_mode == RAT_PLAY_MODE_ROTATE_ALL)
				    || (this->play_mode == RAT_PLAY_MODE_SEQUENCE))
				{
					{
						LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
						__msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
						__msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);

						if(ListCtrl->play_index != scene.focus_id + 1)
						{
							music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
						}
						else
						{
							ListCtrl->OnNextKey((void *)ListCtrl);	//listbar on previous key
						}

						LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
						__msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
						ListCtrl->play_index = scene.focus_id;
						__msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
					}
				}
			}

#else

			if(this->list_ctrl != NULL)
			{
				ListCtrl = this->list_ctrl;
				ListCtrl->play_index = index;
			}
			else
			{
				this->play_index = index;
			}

			filename = this->play_file_info.filename;
			robin_npl_index2file(index, filename);
			__msg("\n\n\n--------------------------------robin npl get filename= %s\n", filename);
			//music_list_set_item_scene(ListCtrl, &(ListCtrl->listbar_play_scene));
			music_play_file(this, filename, 0);

			if(this->visiable)		//背景播放则不显示listbar
			{
				DrawMusic_playing(this);
#if SP_FavoriteFiles
				Drawmusic_play_style(this);
#endif
			}

			if(this->play_mode == RAT_PLAY_MODE_RANDOM && ListCtrl != NULL)
			{
				//music_play_file(this, filename);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				music_list_set_focus_item(ListCtrl, index);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				ListCtrl->play_index = scene.focus_id;
				return EPDK_OK;
			}

			if(this->visiable)		//背景播放则不显示listbar
			{
				if((this->play_mode == RAT_PLAY_MODE_ROTATE_ALL)
				    || (this->play_mode == RAT_PLAY_MODE_SEQUENCE))
				{
					{
						LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
						__msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
						__msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);

						if(ListCtrl->play_index != scene.focus_id + 1)
						{
							music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
						}
						else
						{
							ListCtrl->OnNextKey((void *)ListCtrl);	//listbar on previous key
						}

						LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
						__msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
						ListCtrl->play_index = scene.focus_id;
						__msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
					}
				}
			}

#endif
			//music_play_file(this, filename);
		}

		//robin_npl_get_cur(void);
	}
	else if(status == CEDAR_STAT_PLAY)
	{
		music_rr_flag = 0;
	}
	else if(status == CEDAR_STAT_RR)
	{
		music_rr_flag = 1;
	}

	return EPDK_OK;
}
__s32 music_player_get_last_para(music_player_ctrl_t *This)
{
	__s32 ret = 0;
	reg_music_para_t *music_last_para = NULL;
	reg_system_para_t *system_last_para = NULL;
	//__wait__;
	music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);
	system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
	//This->play_index = music_last_para->play_index;		//获得实际播放的索引号
	This->EQ_mode = music_last_para->EQ_mode;
	This->play_mode = music_last_para->play_mode;
	//This->BL_time = music_last_para->BL_time;

	switch(This->play_mode)
	{
		case RAT_PLAY_MODE_SEQUENCE:
			break;

		case RAT_PLAY_MODE_ROTATE_ONE:
			break;

		case RAT_PLAY_MODE_ROTATE_ALL:
			break;

		case RAT_PLAY_MODE_ONLY_ONCE:
			break;

		case RAT_PLAY_MODE_RANDOM:
			break;

		default:
			This->play_mode = RAT_PLAY_MODE_SEQUENCE;
			break;
	}

	switch(This->EQ_mode)
	{
		case MUSIC_EQ_NORMAL:
			break;

		case MUSIC_EQ_ROCK:
			break;

		case MUSIC_EQ_CLASSIC:
			break;

		case MUSIC_EQ_JAZZ:
			break;

		case MUSIC_EQ_POP:
			break;

		default:
			This->EQ_mode = MUSIC_EQ_NORMAL;
			break;
	}

	switch(system_last_para->backlightoff)
	{
		case 0:
			This->BL_time = MUSIC_BL_ON;
			break;

		case 10:
			This->BL_time = MUSIC_BL_LAST_10S;
			break;

		case 30:
			This->BL_time = MUSIC_BL_LAST_30S;
			break;

		case 60:
			This->BL_time = MUSIC_BL_LAST_60S;
			break;

		case 90:
			This->BL_time = MUSIC_BL_LAST_90S;
			break;

		default:
			This->BL_time = MUSIC_BL_ON;
			break;
	}

	return EPDK_OK;
}

//保存注册表信息直接操作结构体
__s32 music_player_save_last_para(music_player_ctrl_t *This)
{
	__s32 ret = 0;
	reg_music_para_t *music_last_para;
	reg_system_para_t *system_last_para = NULL;
	system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
	music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);
	music_last_para->EQ_mode = This->EQ_mode;
	music_last_para->play_mode = This->play_mode;
	//music_last_para->BL_time = ;
#if 0

	switch(This->BL_time)
	{
		case MUSIC_BL_ON:
			system_last_para->backlightoff = 0;
			break;

		case MUSIC_BL_LAST_10S:
			system_last_para->backlightoff = 10;
			break;

		case MUSIC_BL_LAST_30S:
			system_last_para->backlightoff = 30;
			break;

		case MUSIC_BL_LAST_60S:
			system_last_para->backlightoff = 60;
			break;

		case MUSIC_BL_LAST_90S:
			system_last_para->backlightoff = 90;
			break;

		default:
			system_last_para->backlightoff = 0;
			break;
	}

#endif
	return EPDK_OK;
}

static __s32 music_player_set_play_mode(music_player_ctrl_t *this)
{
	switch(this->play_mode)
	{
		case RAT_PLAY_MODE_SEQUENCE:
			this->play_mode = RAT_PLAY_MODE_ROTATE_ONE;
			break;

		case RAT_PLAY_MODE_ROTATE_ONE:
			this->play_mode = RAT_PLAY_MODE_ROTATE_ALL;
			break;

		case RAT_PLAY_MODE_ROTATE_ALL:
			this->play_mode = RAT_PLAY_MODE_ONLY_ONCE;
			break;

		case RAT_PLAY_MODE_ONLY_ONCE:
			this->play_mode = RAT_PLAY_MODE_RANDOM;
			//this->play_mode = RAT_PLAY_MODE_SEQUENCE;
			break;

		case RAT_PLAY_MODE_RANDOM:
			this->play_mode = RAT_PLAY_MODE_SEQUENCE;
			break;

		default:
			break;
	}

#if 0
	DrawMusicPlayModeButton(this->play_mode, PRESSED);
#endif
	DrawMusicPlayModeStatus(this->play_mode);
	robin_set_play_mode(this->play_mode);
	return EPDK_OK;
}

static __s32 music_player_set_EQ_mode(music_player_ctrl_t *this)
{
	__cedar_audio_eq_t eq_mode;

	switch(this->EQ_mode)
	{
		case MUSIC_EQ_NORMAL:
			this->EQ_mode = MUSIC_EQ_ROCK;
			eq_mode = CEDAR_AUD_EQ_TYPE_ROCK;
			break;

		case MUSIC_EQ_ROCK:
			this->EQ_mode = MUSIC_EQ_CLASSIC;
			eq_mode = CEDAR_AUD_EQ_TYPE_CLASSIC;
			break;

		case MUSIC_EQ_CLASSIC:
			this->EQ_mode = MUSIC_EQ_JAZZ;
			eq_mode = CEDAR_AUD_EQ_TYPE_JAZZ;
			break;

		case MUSIC_EQ_JAZZ:
			this->EQ_mode = MUSIC_EQ_POP;
			eq_mode = CEDAR_AUD_EQ_TYPE_POP;
			break;

		case MUSIC_EQ_POP:
			this->EQ_mode = MUSIC_EQ_NORMAL;
			eq_mode = CEDAR_AUD_EQ_TYPE_NORMAL;
			break;
	}

	DrawMusicEQModeStatus(this);
#if 0
	DrawMusicEQModeButton(this->EQ_mode, PRESSED);
#endif
	robin_set_eq_mode(eq_mode);
	return EPDK_OK;
}
static __s32 music_player_set_BL_time(music_player_ctrl_t *this)
{
	__u32 time = 0;

	switch(this->BL_time)
	{
		case MUSIC_BL_ON:
			this->BL_time = MUSIC_BL_LAST_10S;
			time = 10;
			break;

		case MUSIC_BL_LAST_10S:
			this->BL_time = MUSIC_BL_LAST_30S;
			time = 30;
			break;

		case MUSIC_BL_LAST_30S:
			this->BL_time = MUSIC_BL_LAST_60S;
			time = 60;
			break;

		case MUSIC_BL_LAST_60S:
			this->BL_time = MUSIC_BL_LAST_90S;
			time = 90;
			break;

		case MUSIC_BL_LAST_90S:
			this->BL_time = MUSIC_BL_ON;
			time = 0;
			break;

		default:
			time = 0;
			break;
	}

	g_set_close_scn_time(time);
	DrawMusicBLTimeButton(this->BL_time, PRESSED);
#if SP_APP_CALENDAR
#else
	DrawMusicBLLastTimeStatus(this->BL_time);			//RTC 不存在显示背光时间
#endif
	return EPDK_OK;
}

char *music_player_set_play_random(music_player_ctrl_t *this)
{
	__s32 index;
	char *filename = NULL;
	music_list_ctrl_t *ListCtrl = NULL;
	__listbar_scene_t scene;

	if(this->play_mode == RAT_PLAY_MODE_RANDOM && this->list_ctrl != NULL)
	{
		index = robin_npl_get_next();
		__msg("\n\n\n--------------------------------robin npl get index = %d\n", index);

		if(index != -1)
		{
#if 0

			if(this->list_ctrl != NULL)
			{
				ListCtrl = this->list_ctrl;
				ListCtrl->play_index = index;
			}

			filename = this->play_file_info.filename;
			robin_npl_index2file(index, filename);
			__msg("\n\n\n--------------------------------robin npl get filename= %s\n", filename);
			music_play_file(this, filename, 0);
			DrawMusic_playing(this->font);//更新播放歌曲

			if(this->play_mode == RAT_PLAY_MODE_RANDOM)
			{
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				music_list_set_focus_item(ListCtrl, index);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				ListCtrl->play_index = scene.focus_id;
				return EPDK_OK;
			}

			if(this->visiable)		//背景播放则不显示listbar
			{
				if((this->play_mode == RAT_PLAY_MODE_ROTATE_ALL)
				    || (this->play_mode == RAT_PLAY_MODE_SEQUENCE))
				{
					{
						LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
						__msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
						__msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);

						if(ListCtrl->play_index != scene.focus_id + 1)
						{
							music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
						}
						else
						{
							ListCtrl->OnNextKey((void *)ListCtrl);	//listbar on previous key
						}

						LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
						__msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
						ListCtrl->play_index = scene.focus_id;
						__msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
					}
				}
			}

#else

			if(this->list_ctrl != NULL)
			{
				ListCtrl = this->list_ctrl;
				ListCtrl->play_index = index;
			}
			else
			{
				this->play_index = index;
			}

			filename = this->play_file_info.filename;
			robin_npl_index2file(index, filename);
			__msg("\n\n\n--------------------------------robin npl get filename= %s\n", filename);
			{
				//music_play_file(this, filename);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				music_list_set_focus_item(ListCtrl, index);
				LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
				__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
				ListCtrl->play_index = scene.focus_id;
			}
#endif
		}
	}

	return filename;
}

static __s32 music_player_set_play_previous(music_player_ctrl_t *this)
{
	__s32 ret;
	music_list_ctrl_t *ListCtrl;
	char *filename;
	__listbar_scene_t scene;
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	robin_set_cmd_stop();
	//DrawMusicPlayPreButton(PRESSED);
	filename = music_player_set_play_random(this);

	if(filename == NULL)
	{
		ListCtrl = this->list_ctrl;
		filename = this->play_file_info.filename;
		__msg("------------play previous file\n");
		//__wait__;
		{
			LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);

			if(ListCtrl->play_index != scene.focus_id)
			{
				__msg("------------------current focus item has been moved!\n");
				music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
			}
		}					//
		ListCtrl->OnPreviousKey((void *)ListCtrl);					//listbar on previous key
		//music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
		LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
		ListCtrl->play_index = scene.focus_id;
		ret = get_music_list_focus_item_full_filename(ListCtrl, filename);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}
	}

	robin_set_cmd_stop();
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PREV_SONG);
	//music_play_file(this, filename, 0);
	ret = music_play_file(this, filename, 0);
	DrawMusic_playing(this);
#if SP_FavoriteFiles
	Drawmusic_play_style(this);
#endif

	if(EPDK_OK == ret)
	{
		//music_save_breakpoint(this);
	}

	return EPDK_OK;
}
static __s32 music_player_set_play_next(music_player_ctrl_t *this)
{
	__s32 ret;
	music_list_ctrl_t *ListCtrl;
	char *filename;
	__listbar_scene_t scene;
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	robin_set_cmd_stop();
	//DrawMusicPlayNextButton(PRESSED);
	filename = music_player_set_play_random(this);

	if(filename == NULL)
	{
		ListCtrl = this->list_ctrl;
		filename = this->play_file_info.filename;
		//music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
		//show_music_list(ListCtrl);
		__msg("------------play next file\n");
		{
			LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);

			if(ListCtrl->play_index != scene.focus_id)
			{
				music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
			}
		}
		ListCtrl->OnNextKey((void *)ListCtrl);					//listbar on previous key
		LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
		ListCtrl->play_index = scene.focus_id;
		//music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
		ret = get_music_list_focus_item_full_filename(ListCtrl, filename);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}
	}

	robin_set_cmd_stop();
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_NEXT_SONG);
	//music_play_file(this, filename, 0);
	ret = music_play_file(this, filename, 0);
	DrawMusic_playing(this);
#if SP_FavoriteFiles
	Drawmusic_play_style(this);
#endif

	if(EPDK_OK == ret)
	{
		//music_save_breakpoint(this);
	}

	return EPDK_OK;
}


static __s32 music_player_set_play_index(music_player_ctrl_t *this, __s32 index)
{
	__s32 ret;
	__s32 total_count;
	music_list_ctrl_t *ListCtrl;
	char *filename;
	__listbar_scene_t scene;
#if SP_FavoriteFiles
	HRATNPL hRatNpl;
	{
		if(this->media_type == RAT_MEDIA_TYPE_AUDIO)
		{
			hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);
		}
		else
		{
			hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_FAVOR_AUDIO);
		}
	}
	total_count =  rat_get_cur_scan_cnt(hRatNpl);
#else
	total_count = robin_npl_get_total_count();
#endif
	__wrn("total_count=%d\n", total_count);
	__wrn("index=%d\n", index);
	index--;

	if(index < 0 || index >= total_count)
	{
		return EPDK_FAIL;
	}

	ret = robin_npl_index2file(index, this->play_file_info.filename);

	if(EPDK_FAIL == ret)
	{
		return EPDK_FAIL;
	}

	__wrn("this->play_file_info.filename=%s\n", this->play_file_info.filename);
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	robin_set_cmd_stop();
	//	DrawMusicPlayNextButton(PRESSED);
	ListCtrl = this->list_ctrl;
	filename = this->play_file_info.filename;
	ListCtrl->play_index = index;
	//music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
	//show_music_list(ListCtrl);
	//__msg("------------play next file\n");
	{
		LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
		__wrn("ListCtrl->play_index=%d\n", ListCtrl->play_index);

		if(ListCtrl->play_index != scene.focus_id)
		{
			music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
		}
	}
	music_listbar_on_index((void *)ListCtrl, ListCtrl->play_index);
	LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
	ListCtrl->play_index = scene.focus_id;
	__wrn("scene.focus_id=%d\n", scene.focus_id);
	__wrn("ListCtrl->play_index=%d\n", ListCtrl->play_index);
	//music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
	get_music_list_focus_item_full_filename(ListCtrl, filename);
	__wrn("filename=%s\n", filename);
	music_play_file(this, filename, 0);
	DrawMusic_playing(this);
#if SP_FavoriteFiles
	Drawmusic_play_style(this);
#endif
	return EPDK_OK;
}


static __s32 music_player_set_play_pause(music_player_ctrl_t *this, music_btn_status_e status)
{
	DrawMusicPlayPauseButton(status);

	if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
	{
		robin_set_cmd_pause();
		DrawMusicPlayStatusButton(MUSIC_PAUSE);
	}
	else
	{
		robin_set_cmd_play();
		DrawMusicPlayStatusButton(MUSIC_PLAY);
	}

	return EPDK_OK;
}

//不管在什么模式，对是否存在歌词进行检测
__s32  music_player_check_lyrics(music_player_ctrl_t *this, __bool bUpdate)
{
	if(this == NULL)
	{
		return EPDK_FAIL;
	}

	delete_music_lyrics_scene(this);
	//if(this->menu_mode == MUSIC_EXPLORER_MODE)
	{
		if(music_lyrics_exist())		//检查是否有歌词
		{
			//有歌词则不显示频谱
			if(create_music_lyrics_scene(this) != EPDK_FAIL)
			{
				__msg("---------------create music lyrics scene successfully!\n");
			}
			else
			{
				__wrn("---------------create music lyrics scene failed!\n");
			}
		}
	}
	return EPDK_OK;
}
static __s32 music_player_set_play_backward(music_player_ctrl_t *this)
{
	__s32 ret = 0;
	__u32 total_time = 0, cur_time = 0;
	__cedar_status_t play_status;
	DrawMusicPlayBackwardButton(PRESSED);
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	__here__;
	DrawMusicPlayStatusButton(MUSIC_PLAY);
	total_time = robin_get_total_time();
	play_status = robin_get_fsm_status();

	if(play_status == CEDAR_STAT_PAUSE)
	{
		robin_set_cmd_play();			//后退失败,则继续播放
		music_player_check_lyrics(this, EPDK_FALSE);
		cur_time = robin_get_cur_time();

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		DrawMusicPlayingTime(cur_time);
		__msg("**********************************DrawMusicPlayingTime\n");
		DrawMusicProgressBar(total_time, cur_time);
		__msg("**********************************DrawMusicProgressBar\n");
		return EPDK_OK;
	}
	else if(play_status == CEDAR_STAT_STOP)
	{
		__here__;
		this->CheckFocusItemCnt = 0;
		__msg("2------------check focus item on timer\n");
		music_player_win_check_focus_on_timer(this);	//检测当前焦点是否播放条目
		robin_set_cmd_play();			//后退失败,则继续播放
		music_player_check_lyrics(this, EPDK_FALSE);
		cur_time = robin_get_cur_time();

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		DrawMusicPlayingTime(cur_time);
		DrawMusicProgressBar(total_time, cur_time);
		return EPDK_OK;
	}
	else if(play_status == CEDAR_STAT_RR)
	{
		cur_time = robin_get_cur_time();

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		DrawMusicPlayingTime(cur_time);
		DrawMusicProgressBar(total_time, cur_time);
		return EPDK_OK;
	}

	robin_set_ff_rr_speed(MUSIC_RR_SPEED);
	//robin_set_cmd_play();
	ret = robin_set_cmd_rr();

	if(ret == -1)
	{
		robin_set_cmd_play();			//后退失败,则继续播放
		music_player_check_lyrics(this, EPDK_FALSE);
		DrawMusicProgressBar(0, 0);
		GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
		return EPDK_OK;
	}

	this->setting_status = CEDAR_STAT_RR;
	cur_time = robin_get_cur_time();

	if(cur_time > total_time)
	{
		cur_time = total_time;
	}

	DrawMusicPlayingTime(cur_time);
	DrawMusicProgressBar(total_time, cur_time);
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	return EPDK_OK;
}

static __s32 music_player_set_play_forward(music_player_ctrl_t *this)
{
	__s32 ret = 0;
	__u32 total_time = 0, cur_time = 0;
	__cedar_status_t play_status;
	DrawMusicPlayForwardButton(PRESSED);
	DrawMusicPlayStatusButton(MUSIC_PLAY);
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	total_time = robin_get_total_time();
	__here__;
	play_status = robin_get_fsm_status();

	if(play_status == CEDAR_STAT_PAUSE)
	{
		robin_set_cmd_play();
		music_player_check_lyrics(this, EPDK_FALSE);
		cur_time = robin_get_cur_time();
		__here__;

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		DrawMusicPlayingTime(cur_time);
		DrawMusicProgressBar(total_time, cur_time);
		return EPDK_OK;
	}
	else if(play_status == CEDAR_STAT_STOP)
	{
		__here__;
		robin_set_cmd_stop();
		this->CheckFocusItemCnt = 0;
		__msg("2------------check focus item on timer\n");
		music_player_win_check_focus_on_timer(this);	//检测当前焦点是否播放条目
		music_player_check_lyrics(this, EPDK_FALSE);
		cur_time = robin_get_cur_time();

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		DrawMusicPlayingTime(cur_time);
		DrawMusicProgressBar(total_time, cur_time);
		this->setting_status = CEDAR_STAT_PLAY;
		return EPDK_OK;
	}
	else if(play_status == CEDAR_STAT_FF)
	{
		cur_time = robin_get_cur_time();
		__here__;

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		DrawMusicPlayingTime(cur_time);
		DrawMusicProgressBar(total_time, cur_time);
		return EPDK_OK;
	}

	robin_set_ff_rr_speed(MUSIC_FF_SPEED);
	__here__;
	//robin_set_cmd_play();
	__here__;
	ret = robin_set_cmd_ff();
	__here__;

	if(ret == -1)
	{
		robin_set_cmd_play();			//后退失败,则继续播放
		music_player_check_lyrics(this, EPDK_FALSE);
		DrawMusicProgressBar(total_time, total_time);
		GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
		return EPDK_OK;
	}

	__here__;
	this->setting_status = CEDAR_STAT_FF;
	cur_time = robin_get_cur_time();
	__here__;

	if(cur_time > total_time)
	{
		cur_time = total_time;
	}

	//GUI_LyrWinCacheOn();
	DrawMusicPlayingTime(cur_time);
	//DrawMusicTotalTime(total_time);
	//DrawMusicProgressBarPoint(total_time, cur_time);
	DrawMusicProgressBar(total_time, cur_time);
	//GUI_LyrWinCacheOff();
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	return EPDK_OK;
}

static __s32 music_player_on_setting(music_player_ctrl_t *this)
{
	switch(this->menu_id)
	{
		case SET_PLAY_MODE:
			music_player_set_play_mode(this);
			break;

		case SET_EQ_MODE:
			music_player_set_EQ_mode(this);
			break;
			//case SET_BL_TIME:
			//music_player_set_BL_time(this);
			//break;
			//case SET_PLAY_PREVIOUS:
			//music_player_set_play_previous(this);
			//break;
			//case SET_PLAY_NEXT:
			//music_player_set_play_next(this);
			//break;
			//case SET_PLAY_PAUSE:
			//music_player_set_play_pause(this);
			//break;
			//case SET_PLAY_BACKWARD:
			//music_player_set_play_backward(this);
			//break;
			//case SET_PLAY_FORWARD:
			//music_player_set_play_forward(this);
			//break;
	}

	return EPDK_OK;
}
//设置播放器按键处理函数
static __s32 music_setting_player_key_proc(__gui_msg_t *msg)
{
	static __u32 last_key = 0;
	music_player_ctrl_t *this;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
	{
		this->AutoExitMenuCnt = 0;

		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				DrawMusicPlayerContrlButtonByID(this, this->menu_id, UNFOCUSED);

				if(this->menu_id == SET_EQ_MODE)
				{
					this->menu_id = SET_PLAY_MODE;
				}
				else
				{
					this->menu_id++;
				}

				DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
				break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				DrawMusicPlayerContrlButtonByID(this, this->menu_id, UNFOCUSED);

				if(this->menu_id == SET_PLAY_MODE)
				{
					this->menu_id = SET_EQ_MODE;
				}
				else
				{
					this->menu_id--;
				}

				DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
				break;

			case GUI_MSG_KEY_LONGLEFT:
			case GUI_MSG_KEY_LONGRIGHT:
				if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
				{
					//DrawMusicPlayerContrlButton(this);
					ClearMusicPlayerContrlButtonBG();

					if(this->h_lyrics != NULL)
					{
						//if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
						{
							this->RefreshLyricsTimerCnt = 0;
							update_music_lyrics(this->h_lyrics);
						}
					}
					else
					{
						__msg("+++++++++SPCTRM_Clear++++++++\n");
						SPCTRM_Clear(0);
						SPCTRM_Update((H_WIN)(this->h_framewin));
						//draw spectrum();//画频谱图
					}

					this->menu_mode = MUSIC_EXPLORER_MODE;
				}

				break;

			case GUI_MSG_KEY_VDEC:
				break;

				//case GUI_MSG_KEY_ENTER:
			case GUI_MSG_KEY_MENU:
				last_key = GUI_MSG_KEY_MENU;
				break;
				//			case GUI_MSG_KEY_VADD:
				//				last_key = GUI_MSG_KEY_VADD;
				//				break;

			case GUI_MSG_KEY_ENTER:
				last_key = GUI_MSG_KEY_ENTER;

				if(KEY_DOWN_ACTION == msg->dwAddData2)
				{
					GUI_UC_SetEncodeUTF8();
					music_player_on_setting(this);
				}

				break;

				//case GUI_MSG_KEY_VADD:
			case GUI_MSG_KEY_ESCAPE:
				//music_send_command(msg->h_deswin, 0, SWITCH_TO_MMENU, 0);
				break;

			case GUI_MSG_KEY_RISE:
				break;

			case GUI_MSG_KEY_RR:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_PLAY == sta)
				{
					robin_set_cmd_rr();
				}

				break;
			}

			case GUI_MSG_KEY_FF:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_PLAY == sta)
				{
					robin_set_cmd_ff();
				}

				break;
			}

			default:
				break;
		}
	}
	else if(KEY_UP_ACTION == msg->dwAddData2)
	{
		switch(msg->dwAddData1)
		{
				//case GUI_MSG_KEY_ENTER:
			case GUI_MSG_KEY_MENU:
			case GUI_MSG_KEY_LONGMENU:
				if(last_key == GUI_MSG_KEY_MENU)
				{
					if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
					{
						//DrawMusicPlayerContrlButton(this);
						ClearMusicPlayerContrlButtonBG();

						if(this->h_lyrics != NULL)
						{
							//if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
							{
								this->RefreshLyricsTimerCnt = 0;
								update_music_lyrics(this->h_lyrics);
							}
						}
						else
						{
							SPCTRM_Clear(0);
							SPCTRM_Update((H_WIN)(this->h_framewin));
							//draw spectrum();//画频谱图
						}

						this->menu_mode = MUSIC_EXPLORER_MODE;
					}
				}

				break;

			case GUI_MSG_KEY_ENTER:
				if(last_key == GUI_MSG_KEY_ENTER)
				{
					GUI_UC_SetEncodeUTF8();
					DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);	//短按键抬起
				}

				break;

			case GUI_MSG_KEY_RR:
			case GUI_MSG_KEY_LONGRR:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_RR == sta)
				{
					robin_set_cmd_play();
				}

				break;
			}

			case GUI_MSG_KEY_FF:
			case GUI_MSG_KEY_LONGFF:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(CEDAR_STAT_FF == sta)
				{
					robin_set_cmd_play();
				}

				break;
			}

			case GUI_MSG_KEY_LR:
			case GUI_MSG_KEY_LONGLR:
			{
				{
					__here__;

					if(this->music_layer)
					{
						__here__;

						if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
						{
							__s32 channel;
							__here__;
							GUI_LyrWinSel(this->music_layer);
							channel = robin_get_channel();
							channel = (channel + 1) % 3;
							__wrn("channel=%d\n", channel);
							robin_set_channel(channel);
							DrawMusicAcompStatus(channel);
						}
					}
				}
				break;
			}

			case GUI_MSG_KEY_PLAY_PAUSE:
			case GUI_MSG_KEY_LONGPLAY_PAUSE:
			{
				__cedar_status_t sta;
				sta = robin_get_fsm_status();

				if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
				{
					robin_set_cmd_pause();
					app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);
				}
				else
				{
					app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);
					robin_set_cmd_play();
				}

				break;
			}
		}

		last_key = 0;
	}

	return EPDK_OK;
}

//按键消息处理函数
static __s32 music_player_win_key_proc(__gui_msg_t *msg)
{
	music_player_ctrl_t *this;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	switch(this->menu_mode)
	{
		case MUSIC_EXPLORER_MODE:
			music_explorer_key_proc(msg);			//浏览器on key process
			break;

		case MUSIC_SET_PLAYER_MODE:
			music_setting_player_key_proc(msg);		//setting player on key process
			break;

		default:
			break;
	}

	return EPDK_OK;
}

__s32 music_player_win_check_focus_on_timer(music_player_ctrl_t *this)
{
	__listbar_scene_t scene;

	if(this == NULL)
	{
		return EPDK_FAIL;
	}

	if(!this->visiable)
	{
		return EPDK_FAIL;
	}

	if(this->list_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	LISTBAR_GetScene(this->list_ctrl->listbar_handle, &scene);
	__msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);

	if(this->list_ctrl->play_index != scene.focus_id)
	{
		music_list_set_focus_item(this->list_ctrl, this->list_ctrl->play_index);
		__msg("----------------------------------------set focus item to play index\n");
	}

	return EPDK_OK;
}


__s32 music_player_win_on_sys_timer(music_player_ctrl_t *this)
{
#if SP_APP_CALENDAR
	{
		//更新系统时间
		__time_t sys_time = {0, 0, 0};

		if(this == NULL)
		{
			return EPDK_FAIL;
		}

		if(this->visiable)
		{
			this->RefreshSystimeCnt++;
			__msg("\n1-----music on systime....tick = %d\n", this->RefreshSystimeCnt);

			if(this->RefreshSystimeCnt > MUSIC_REFRESH_SYSTIME_CNT)
			{
				this->RefreshSystimeCnt = 0;
				esTIME_GetTime(&sys_time);
				__msg("\n2------------------------------music display systime, minute = %d\n", sys_time.minute);
				DrawMusicSystemTime(&sys_time);
			}
		}
	}
#endif
	{
		if(!this->visiable)
		{
			return EPDK_FAIL;
		}

		this->CheckFocusItemCnt++;
		__msg("\n1--------------check focus item on timer, tick = %d\n", this->CheckFocusItemCnt);

		if(this->CheckFocusItemCnt > MUSIC_CHECK_FOCUS_TIMER_CNT)
		{
			this->CheckFocusItemCnt = 0;
			__msg("2------------check focus item on timer\n");
			music_player_win_check_focus_on_timer(this);
		}
	}
	return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:           static __s32 music_player_win_on_timer(msg)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_player_win_on_timer(__gui_msg_t *msg)
{
	music_player_ctrl_t *this;
	__u32 total_time = 0, cur_time = 0;
	__cedar_status_t play_status;
	GUI_MEMDEV_Handle	draw_mem;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(this->visiable)
	{}

	//当后退超过0时，会进入stop状态，此时应重新play

	if((this->setting_status == CEDAR_STAT_FF) || (this->setting_status == CEDAR_STAT_RR))
	{
		play_status = robin_get_fsm_status();

		if((play_status == CEDAR_STAT_FF)
		    || (play_status == CEDAR_STAT_RR))
		{
			robin_set_cmd_play();
		}

		if(play_status == CEDAR_STAT_STOP)
		{
			total_time = robin_get_total_time();

			if(this->visiable)
			{
				DrawMusicProgressBar(total_time, total_time);
				//__msg("FF OR RR stopped\n");
				//__wait__;
			}

			if(this->setting_status == CEDAR_STAT_RR)
			{
				robin_set_cmd_play();
			}

			this->setting_status = CEDAR_STAT_PLAY;
			return EPDK_OK;
		}

#if 0
		{
			draw_mem = GUI_MEMDEV_Create(C_LAYER_CACHE_AREA_X, C_LAYER_CACHE_AREA_Y, C_LAYER_CACHE_AREA_W, C_LAYER_CACHE_AREA_H);
			GUI_MEMDEV_Select(draw_mem);
			//__msg("apply for memory %d\n", draw_mem);
			//__wait__;
			this->RefreshPlayTimeCnt = 0;
			total_time = robin_get_total_time();
			cur_time = robin_get_cur_time();

			if(cur_time > total_time)
			{
				cur_time = total_time;
			}

			//GUI_SetColor(GUI_WHITE);
			//GUI_LyrWinCacheOff();
			DrawMusicPlayingTime(cur_time);
			DrawMusicTotalTime(total_time);
			DrawMusicProgressBar(total_time, cur_time);
			//GUI_LyrWinCacheOn();
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
#endif
		this->setting_status = CEDAR_STAT_PLAY;
		return EPDK_OK;
	}
	else
	{
		music_player_check_play_status_on_timer(this);	//check status for play mode
	}

	if(this->visiable)
	{
		__msg("--------------------music player on timer\n");

		if(this->list_ctrl != NULL)
		{
			if(this->list_ctrl->total == 0)
			{
				return EPDK_FAIL;
			}
		}
		else
		{
			return EPDK_FAIL;
		}

		GUI_LyrWinSel(this->music_layer);

		if(this->RefreshPlayTimeCnt++ >= MUSIC_REFRESH_PLAYTIME_CNT)
		{
#if SP_APP_CALENDAR
			{
				//__time_t sys_time;
				//esTIME_GetTime(&sys_time);
				//DrawMusicSystemTime(&sys_time);
			}
#endif
			draw_mem = GUI_MEMDEV_Create(C_LAYER_CACHE_AREA_X, C_LAYER_CACHE_AREA_Y, C_LAYER_CACHE_AREA_W, C_LAYER_CACHE_AREA_H);
			GUI_MEMDEV_Select(draw_mem);
			//__msg("apply for memory %d\n", draw_mem);
			//__wait__;
			this->RefreshPlayTimeCnt = 0;
			total_time = robin_get_total_time();
			cur_time = robin_get_cur_time();

			if(cur_time > total_time)
			{
				cur_time = total_time;
			}

			//GUI_SetColor(GUI_WHITE);
			//GUI_LyrWinCacheOff();
			DrawMusicPlayingTime(cur_time);
			DrawMusicTotalTime(total_time);
			DrawMusicProgressBar(total_time, cur_time);
			//DrawMusicProgressBarPoint(total_time, cur_time);
			//GUI_LyrWinCacheOn();
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}

		if(this->menu_mode == MUSIC_EXPLORER_MODE)
		{
			{
				if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
				{
					SPCTRM_Update((H_WIN)(this->h_framewin));
				}
			}
			//if(this->h_lyrics != NULL)
			{
				if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
				{
					this->RefreshLyricsTimerCnt = 0;
					update_music_lyrics(this->h_lyrics);
				}
			}
			//else
		}
		else
		{
			this->AutoExitMenuCnt++;

			if(this->AutoExitMenuCnt > C_AUTO_EXIT_MENU_TIME)
			{
				this->menu_mode = MUSIC_EXPLORER_MODE;
				this->AutoExitMenuCnt = 0;
				//__wait__;
				ClearMusicPlayerContrlButtonBG();

				if(this->h_lyrics != NULL)
				{
					//if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
					{
						this->RefreshLyricsTimerCnt = 0;
						update_music_lyrics(this->h_lyrics);
					}
				}
				else
				{
					SPCTRM_Clear(0);		//显示最大值
					SPCTRM_Update((H_WIN)(this->h_framewin));
				}
			}
		}
	}

	//MusicSpectrumUpdate(this);				//
	//GUI_LyrWinFlushFB(player_ctrl->music_layer);
	return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           static __s32 music_player_win_on_close(__gui_msg_t *msg)
*
*Description	:				OnClose处理函数，释放FrameWin
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_player_win_on_close(__gui_msg_t *msg)
{
	return GUI_FrmWinDelete(msg->h_deswin);			//send destroy message
}

/*
************************************************************************************************************************
*Function	:           static __s32  music_player_win_on_destroy(__gui_msg_t *msg)
*
*Description	:				OnDestroy处理函数，释放framewin 所有申请的资源
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 music_player_delete_digit_layer(H_WIN       music_frame_win)
{
	music_player_ctrl_t *this;
	__wrn("music_player_delete_digit_layer begin...\n");
	this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

	if(NULL == this)
	{
		return EPDK_FAIL;
	}

	if(this->hdigit_layer)
	{
		GUI_LyrWinDelete(this->hdigit_layer);
		this->hdigit_layer = NULL;
#if MASK_SysFlags_ShiftNum
		{
			reg_system_para_t *para;
			para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

			if(para)
			{
				para->m_maskFlags |= MASK_SysFlags_ShiftNum;
			}
		}
#endif
	}

	if(this->digit_timmer_id)
	{
		if(GUI_IsTimerInstalled(music_frame_win, this->digit_timmer_id))
		{
			GUI_KillTimer(music_frame_win, this->digit_timmer_id);
		}
	}

	__wrn("music_player_delete_digit_layer end...\n");
	return EPDK_OK;
}
static __s32  music_player_win_on_destroy(__gui_msg_t *msg)
{
	music_player_ctrl_t *this;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(this == NULL)
	{
		return EPDK_FAIL;
	}

	if(NULL == this->h_framewin)
	{
		return EPDK_FAIL;
	}

	if(GUI_IsTimerInstalled(this->h_framewin, MusicSysTimerId))
	{
		GUI_KillTimer(this->h_framewin, MusicSysTimerId);
		__msg("--------------------------system time timer has been killed\n");
	}

	if(GUI_IsTimerInstalled(this->h_framewin, MusicCheckFocusTimerId))
	{
		GUI_KillTimer(this->h_framewin, MusicCheckFocusTimerId);
		__msg("--------------------------check focus item timer has been killed\n");
	}

	//app_print_memory_usage("music player on destroy", NULL);
#if SP_FavoriteFiles

	if(this->media_type == RAT_MEDIA_TYPE_FAVOR_AUDIO)
	{
		music_play_status  = RAT_MEDIA_TYPE_FAVOR_AUDIO;
	}
	else
	{
		music_play_status = RAT_MEDIA_TYPE_AUDIO;
	}

#endif
	delete_music_lyrics_scene(this);
	music_player_save_last_para(this);

	if(this->list_ctrl)
	{
		delete_music_list(this->list_ctrl);
	}
	else
	{
		dsk_reg_save_cur_play_info(REG_APP_MUSIC, this->play_index, this->play_file_info.filename, this->root_type);
	}

	__here__;
	__music_save_breakpoint_info(this->play_index);
	music_list_save_last_para(this->list_ctrl);
	__here__;
	//MusicSpectrumSceneDelete(this);
	music_player_delete_digit_layer(msg->h_deswin);
	music_ff_fb_layer_delete(msg);
	music_player_device_uninit(this);
	__here__;
	eLIBs_memset(this, 0, sizeof(music_player_ctrl_t));
	esMEMS_Bfree(this, sizeof(music_player_ctrl_t));
	__here__;
	music_free_icon_res();
	__here__;
	//app_print_memory_usage("music player on destroy finished", NULL);
	return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:          static __s32 music_player_scene_create(H_WIN  music_frame_win)
*
*Description	: 		music player 场景创建
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_player_scene_create(H_WIN  music_frame_win)
{
	__u32 song_total;
	music_player_ctrl_t *this;
	music_list_para_t list_para;
	__bool ret;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);
	this->h_framewin = music_frame_win;										//保存窗口句柄
	__msg("music player is initializing!\n");
	//__wait__;
	list_para.framewin = this->h_framewin;
	list_para.layer = this->music_layer;
	list_para.visiable = this->visiable;
	list_para.root_type = this->root_type;
	list_para.media_type = this->media_type;
	list_para.font = this->font;
	list_para.explorer_play_music = this->explorer_play_music;
	list_para.music_file_name = this->music_file_name;
	//app_print_memory_usage("new music list", NULL);

	if(list_para.visiable == EPDK_TRUE)
	{
		this->list_ctrl = new_music_list(&list_para);

		if(NULL == this->list_ctrl)
		{
			return EPDK_FAIL;
		}
	}
	else
	{
		this->list_ctrl = NULL;
		GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_SUSPEND);
	}

	music_player_get_last_para(this);

	if(this->list_ctrl != NULL)
	{
		char *filename = this->play_file_info.filename;
		//get_music_list_focus_item_full_filename(this->list_ctrl, filename);
		ret = MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}

		rat_set_file_for_play(this->list_ctrl->rat_handle, filename);
		this->play_index = this->list_ctrl->play_index;
	}
	else
	{
		music_list_ctrl_t list_ctrl;
		char *filename = this->play_file_info.filename;
		init_bg_music_list(&list_ctrl, &list_para);
		ret = MusicGetListItemFileFullPath(list_ctrl.rat_handle, list_ctrl.play_index, filename);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}

		rat_set_file_for_play(list_ctrl.rat_handle, filename);
		this->play_index = list_ctrl.play_index;
	}

	//this->play_mode = RAT_PLAY_MODE_SEQUENCE;
	//this->EQ_mode = MUSIC_EQ_NORMAL;
	music_player_device_init(this);

	//app_print_memory_usage("music player win on create message end", NULL);

	/*
		{
			char *filename = player_ctrl->play_file_info.filename;
			get_music_list_focus_item_full_filename(player_ctrl->list_ctrl, filename);
			music_play_file(player_ctrl, filename);
		}
	*/

	if(this->visiable)
	{
		ret = GUI_SetTimer(music_frame_win, MusicSysTimerId, MUSIC_SYSTIMER_INTERVAL, NULL);						//系统时间刷新定时器

		if(!ret)
		{
			__wrn("creat timer for system time failed\n");
		}
		else
		{
			__msg("creat timer for system time successfully\n");
		}

		/*
		ret = GUI_SetTimer(music_frame_win, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);		//检测是否foucus item 与当前播放index一致
		if(!ret)
		{
			__wrn("creat timer for check focus item failed\n");
		}
		else
		{
			__msg("creat timer for check focus item successfully\n");
		}
		*/
	}

	return EPDK_OK;
}


/*
************************************************************************************************************************
*Function	:          static __s32 music_player_win_on_paint(H_WIN  music_frame_win)
*
*Description	: 		on paint message
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

static void DrawMusicDirectory(__u32 focus, GUI_FONT *font)
{
	void *fbmp;
	void *ubmp;
	__s32 directory_fcous = 0;
	__s32 directory_unfcous = 0;
	__u32 start_x = 240 + 200 - 20 + 100;	//TF或USB目录播放图标的位置
	__u32 start_y = 95 + 20 + 280 + 110;
	GUI_RECT gui_rect;

	/*	char item_str[1][128];
		__s32 lang_id = STRING_MUSIC_SONG_DIRECTORY;

		MusicSetTransColor();//设置颜色等
	    GUI_SetFont(font);
	    GUI_UC_SetEncodeUTF8();

		gui_rect.x0 = 3;
		gui_rect.y0 = start_y;
		gui_rect.x1 = 85;
		gui_rect.y1 = gui_rect.y0 + 20;

		GUI_ClearRectEx(&gui_rect);//清除区域


		dsk_langres_get_menu_text(lang_id, item_str, GUI_TITLE_MAX);// 显示播放目录
		GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER|GUI_TA_VCENTER);
		*/
	if(focus == 1)
	{
		directory_fcous = theme_open(ID_MUSIC_SD_MUSIC_FOCUSED_BMP);
		directory_unfcous = theme_open(ID_MUSIC_USB_MUSIC_UNFOCUSED_BMP);
		root_type_flag = 1;
	}
	else if(focus == 0)
	{
		directory_fcous = theme_open(ID_MUSIC_USB_MUSIC_FOCUSED_BMP);
		directory_unfcous = theme_open(ID_MUSIC_SD_MUSIC_UNFOCUSED_BMP);
		root_type_flag = 0;
	}

	fbmp = theme_hdl2buf(directory_fcous);
	ubmp = theme_hdl2buf(directory_unfcous);

	if(focus == 1)
	{
		GUI_BMP_Draw(fbmp, start_x, start_y + 26);
		GUI_BMP_Draw(ubmp, start_x + 42, start_y + 26);
	}
	else
	{
		GUI_BMP_Draw(ubmp, start_x, start_y + 26);
		GUI_BMP_Draw(fbmp, start_x + 42, start_y + 26);
	}

	dsk_theme_close(directory_fcous);
	dsk_theme_close(directory_unfcous);
}

#if SP_FavoriteFiles
static void Drawmusic_play_style(music_player_ctrl_t *this)
{
	__u32 index;
	char ucStringBuf[20] = {0};
	__u32 x1 = 90 + 230 + 200;
	__u32 y1 = 100 + 8 + 280;
	GUI_RECT gui_rect2;
	char item_str[1][128];
	//__s32 lang_id_2 = STRING_MUSIC_MUSIC_PLAYING;
	//MusicSetTransColor();//设置颜色等
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetColor(GUI_YELLOW);
	GUI_SetFont(this->font);
	GUI_UC_SetEncodeUTF8();
	//显示正在播放区域
	gui_rect2.x0 = x1;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0 + 96;
	gui_rect2.y1 = gui_rect2.y0 + 22;
	GUI_ClearRectEx(&gui_rect2);//清除区域

	if(this->media_type == RAT_MEDIA_TYPE_AUDIO)
	{
		dsk_langres_get_menu_text(STRING_MUSIC_PLAY_FOR_NORMAL, item_str, GUI_TITLE_MAX);//显示 正常播放
	}
	else
	{
		dsk_langres_get_menu_text(STRING_MUSIC_PLAY_FOR_FAVOR, item_str, GUI_TITLE_MAX);//显示收藏播放
	}

	GUI_DispStringInRect((const char *)item_str, &gui_rect2, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

#endif
static void DrawMusic_playing(music_player_ctrl_t *this)
{
	/*__u32 index;
	char ucStringBuf[20] = {0};
	__u32 x1=90;
	__u32 y1=225;
	GUI_RECT gui_rect2;
	char item_str[1][128];
	__s32 lang_id_2 = STRING_MUSIC_MUSIC_PLAYING;

	MusicSetTransColor();//设置颜色等
	   GUI_SetFont(font);
	   GUI_UC_SetEncodeUTF8();

	gui_rect2.x0 = x1;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0+85;
	gui_rect2.y1 = gui_rect2.y0+22;

	GUI_ClearRectEx(&gui_rect2);//清除区域

	dsk_langres_get_menu_text(lang_id_2, item_str, GUI_TITLE_MAX);//显示正在播放
	GUI_DispStringInRect((const char *)item_str, &gui_rect2, GUI_TA_HCENTER|GUI_TA_VCENTER);

	index = robin_npl_get_cur()+1;//显示正在播放数字
	eLIBs_sprintf(ucStringBuf, "%d", index);

	gui_rect2.x0 = x1+85;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0+80;
	gui_rect2.y1 = gui_rect2.y0+22;

	GUI_ClearRectEx(&gui_rect2);//清除区域
	GUI_DispStringInRect(ucStringBuf, &gui_rect2, GUI_TA_LEFT|GUI_TA_VCENTER);
	*/
	__u32 index;
	char ucStringBuf[20] = {0};
	__u32 x1 = 90 + 230 + 200 + 70  + 200;
	__u32 y1 = 100 + 8 + 40 + 280 + 95;
	GUI_RECT gui_rect2;
	char item_str[1][128];
	__s32 lang_id_2 = STRING_MUSIC_MUSIC_PLAYING;
#if SP_FavoriteFiles
	music_list_ctrl_t *ListCtrl;
	char *filename;
	__listbar_scene_t scene;
#endif
	MusicSetTransColor();//设置颜色等
	GUI_SetFont(this->font);
	GUI_UC_SetEncodeUTF8();
	//显示正在播放区域
	gui_rect2.x0 = x1;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0 + 96;
	gui_rect2.y1 = gui_rect2.y0 + 22 ;   
	GUI_ClearRectEx(&gui_rect2);//清除区域
	dsk_langres_get_menu_text(lang_id_2, item_str, GUI_TITLE_MAX);//显示正在播放
	GUI_DispStringInRect((const char *)item_str, &gui_rect2, GUI_TA_HCENTER | GUI_TA_VCENTER);
#if SP_FavoriteFiles
	ListCtrl = this->list_ctrl;
	filename = this->play_file_info.filename;
	__msg("------------play previous file\n");
	//__wait__;
	{
		LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);

		if(ListCtrl->play_index != scene.focus_id)
		{
			__msg("------------------current focus item has been moved!\n");
			music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
		}
	}
	eLIBs_sprintf(ucStringBuf, "%d", ListCtrl->play_index + 1);
#else
	index = robin_npl_get_cur() + 1; //显示正在播放数字
	eLIBs_sprintf(ucStringBuf, "%d", index);
#endif
	//显示播放数字区域
	gui_rect2.x0 = x1 + 85 + 15;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0 + 85;
	gui_rect2.y1 = gui_rect2.y0 + 22;
	GUI_ClearRectEx(&gui_rect2);//清除区域
	GUI_DispStringInRect(ucStringBuf, &gui_rect2, GUI_TA_LEFT | GUI_TA_VCENTER);
}

static void DrawMusic_total(music_player_ctrl_t *this)
{
	__u32 song_total;
	char ucStringBuf[20] = {0};
	__u32 x1 = 90 + 230 + 200 + 70 + 200;
	__u32 y1 = 100 + 26 + 4 + 40 + 280 + 95;
	GUI_RECT gui_rect;
#if SP_FavoriteFiles
	HRATNPL hRatNpl;
#endif
	char item_str[1][128];
	__s32 lang_id_1 = STRING_MUSIC_SONG_TOTAL;
	MusicSetTransColor();//设置颜色等
	GUI_SetFont(this->font);
	GUI_UC_SetEncodeUTF8();
	//显示歌曲总数区域
	gui_rect.x0 = x1;
	gui_rect.y0 = y1;
	gui_rect.x1 = gui_rect.x0 + 96;
	gui_rect.y1 = gui_rect.y0 + 22;
	GUI_ClearRectEx(&gui_rect);//清除区域
	dsk_langres_get_menu_text(lang_id_1, item_str, GUI_TITLE_MAX);//显示歌曲总数
	GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#if SP_FavoriteFiles
	{
		if(this->media_type == RAT_MEDIA_TYPE_AUDIO)
		{
			hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);
		}
		else
		{
			hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_FAVOR_AUDIO);
		}
	}
	song_total =  rat_get_cur_scan_cnt(hRatNpl);
#else
	song_total = robin_npl_get_total_count();//显示歌曲数字
#endif
	eLIBs_sprintf(ucStringBuf, "%d", song_total);
	//显示歌曲数字区域
	gui_rect.x0 = x1 + 85 + 15;
	gui_rect.y0 = y1;
	gui_rect.x1 = gui_rect.x0 + 85;
	gui_rect.y1 = gui_rect.y0 + 22;
	GUI_ClearRectEx(&gui_rect);//清除区域
	GUI_DispStringInRect(ucStringBuf, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
}
static void DrawMusic_name(char *filename)
{
	GUI_RECT gui_rect;
	char musicname[512] = {0};
	MusicSetTransColor();//设置颜色等
	GUI_UC_SetEncodeUTF8();
	//__msg("this->music_file_name = %s\n", filename);
	//正在播放歌曲名显示区域
	gui_rect.x0 = 350 + 140;
	gui_rect.y0 = 200 + 80 + 100;
	gui_rect.x1 = gui_rect.x0 + 450;
	gui_rect.y1 = gui_rect.y0 + 100;
	GUI_ClearRectEx(&gui_rect);//清除区域
	gui_rect.y1 = gui_rect.y0 + 100;
	eLIBs_strcpy(musicname, (void *)(eLIBs_strchrlast(filename, '\\') + 1));
	//musicname = eLIBs_strchrlast(filename, '//');
	__msg("this->music_file_name = %s\n", musicname);
	//GUI_DispStringInRect(musicname, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringInRectWrap(musicname, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
}

static __s32 music_player_win_on_paint(H_WIN  music_frame_win)
{
	__s32 ret = 0;
	music_player_ctrl_t *player_ctrl;
	player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

	if(GUI_LyrWinGetSta(player_ctrl->music_layer) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	__msg("music player on paint message\n");

	//__wait__;

	if(player_ctrl->visiable)
	{
		GUI_LyrWinSetTop(player_ctrl->music_layer);
		MusicSetTransColor();
		//if(player_ctrl->list_ctrl != NULL)
		{
			show_music_list(player_ctrl->list_ctrl);
		}
		DrawMusicHeadbar(player_ctrl);

		if(player_ctrl->root_type == RAT_TF)
		{
			DrawMusicDirectory(1, player_ctrl->font);
		}
		else if(player_ctrl->root_type == RAT_USB)
		{
			DrawMusicDirectory(0, player_ctrl->font);
		}

		DrawMusic_total(player_ctrl);
		DrawMusic_playing(player_ctrl);
	}

	__msg("test volume bar\n");
	//__wait__;
	//app_print_memory_usage("music player on paint message, & ready to play mp3 file", NULL);
	//__here__;
	__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);

	if(player_ctrl->list_ctrl != NULL)
	{
		music_list_get_item_scene(player_ctrl->list_ctrl, &(player_ctrl->list_ctrl->listbar_play_scene));
	}

	__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);
	//DrawMusicPlayer(player_ctrl);
	{
		char *filename = player_ctrl->play_file_info.filename;
		__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);

		if(player_ctrl->list_ctrl != NULL)
		{
			__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);
			ret = MusicGetListItemFileFullPath(player_ctrl->list_ctrl->rat_handle, player_ctrl->list_ctrl->play_index, filename);

			if(EPDK_FAIL == ret)
			{
				return EPDK_FAIL;
			}
		}

		__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);
		__wrn("filename=%s\n", filename);
		//if(robin_get_fsm_status() != CEDAR_STAT_PLAY)
		{
#ifdef C_MUSIC_DEBUG
#else
			__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);
			ret = music_play_file(player_ctrl, filename, 1);
			__wrn("player_ctrl->visiable=%d\n", player_ctrl->visiable);
			
#if SP_FavoriteFiles
					Drawmusic_play_style(player_ctrl);//new add
#endif

			if(player_ctrl->visiable)
			{
				//               __wrn("before DrawMusicPlayerInfo\n");
				//		    	DrawMusicPlayerInfo(player_ctrl, -1);
#if 0
				{
					//后台恢复后，重设当前焦点
					music_list_ctrl_t *ListCtrl = NULL;
					__listbar_scene_t scene;
					__s32 index;
					__here__;

					if(player_ctrl)
					{
						__here__;
						ListCtrl = player_ctrl->list_ctrl;

						if(ListCtrl)
						{
							__here__;
							LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
							__wrn("scene.focus_id = %d, scene.start_id = %d\n", scene.focus_id, scene.start_id);
							index = robin_npl_get_cur();
							__wrn("index=%d\n", index);

							if(-1 != index)
							{
								//music_list_set_focus_item(ListCtrl, index);
							}
						}
					}
				}
#endif
			}

#endif
		}
#ifdef C_MUSIC_DEBUG
#else
		GUI_SetTimer(music_frame_win, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);//play之后才可以打开timer
#endif
	}
	//app_print_memory_usage("player is playing", NULL);
	return EPDK_OK;
}

//extern __s32 music_list_long_string_stop_roll(music_list_ctrl_t *this);
__s32 music_player_scene_delete(H_WIN  music_frame_win)
{
	__s32 ret = 0;
	music_player_ctrl_t *this;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

	if(!this->visiable)
	{
		return EPDK_FAIL;
	}

	this->visiable = EPDK_FALSE;
	/*
	if (GUI_LyrWinGetSta(this->music_layer) == GUI_LYRWIN_STA_ON)
	{
		GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_OFF);
	}
	else
	{
		//return EPDK_OK;
	}
	*/

	if(GUI_IsTimerInstalled(music_frame_win, MusicSysTimerId))
	{
		GUI_KillTimer(music_frame_win, MusicSysTimerId);
		__msg("--------------------------system time timer has been killed\n");
	}

	if(GUI_IsTimerInstalled(music_frame_win, MusicCheckFocusTimerId))
	{
		GUI_KillTimer(music_frame_win, MusicCheckFocusTimerId);
		__msg("--------------------------check focus item timer has been killed\n");
	}

	delete_music_lyrics_scene(this);
	music_player_save_last_para(this);
	//clear_music_list_area();
	//delete_music_list(this->list_ctrl);
	//this->list_ctrl = NULL;
	music_list_long_string_stop(this->list_ctrl);
	//music_list_long_string_stop_roll(this->list_ctrl);
	//esMEMS_Bfree(this, sizeof(music_player_ctrl_t));
	music_free_icon_res();
	return EPDK_OK;
}

__s32  musicplayer_rat_get_search_path(__s32 root_type, char *search_path)
{
	__s32 ret = 0;
	char s_root_type[50];
	char disk_name[RAT_MAX_PARTITION][4];
	eLIBs_memset(s_root_type, 0, sizeof(s_root_type));
	eLIBs_memset(disk_name, 0, sizeof(disk_name));
	//eLIBs_memset(list_ctrl->search_path, 0, sizeof(list_ctrl->search_path));
	__msg("root_type=%d\n", root_type);

	switch(root_type)
	{
		case RAT_LOCAL:
			eLIBs_strcpy(s_root_type, RAT_LOCAL_DISK);
			break;

		case RAT_TF:
			eLIBs_strcpy(s_root_type, RAT_SD_CARD);
			break;

		case RAT_USB:
			eLIBs_strcpy(s_root_type, RAT_USB_DISK);
			break;

		default:
			eLIBs_strcpy(s_root_type, RAT_SD_CARD);
			break;
	}

	ret = rat_get_partition_name(s_root_type, disk_name);//获取盘符信息
	eLIBs_strcpy(search_path, disk_name[0]);
	__msg("================================disk_name[0]=%s\n", disk_name[0]);
	return ret;
}

__s32 music_player_restore(H_WIN  music_frame_win, __s32 root_type)
{
	music_player_ctrl_t *this;
	music_list_para_t list_para;
	__s32 ret = 0;
	GUI_MEMDEV_Handle	draw_mem;

	if(music_frame_win == NULL)
	{
		return EPDK_FAIL;
	}

	//__wait__;
	this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

	//GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_ON);

	if(robin_get_fsm_status() == CEDAR_STAT_STOP)
	{
		//return EPDK_FAIL;
	}

	this->visiable = EPDK_TRUE;

	if(GUI_LyrWinGetSta(this->music_layer) == GUI_LYRWIN_STA_SUSPEND)
	{
		__here__;
		return EPDK_FAIL;
	}

	ret = GUI_LyrWinSetTop(this->music_layer);

	if(ret != EPDK_OK)
	{
		__msg("Set music layer as top failed\n");
		__wait__;
		return EPDK_FAIL;
	}
	else
	{
		__msg("--------------music player set layer status on, ret = %d\n", ret);
	}

	GUI_LyrWinSel(this->music_layer);
	GUI_SetBkColor(0x0);
	GUI_ClearRect(0, 0, 320, 50);
	//clear_music_list_area();
#if 0
	draw_mem = GUI_MEMDEV_Create(0, 0, 320, 240);
	GUI_MEMDEV_Select(draw_mem);
#endif
	this->visiable = EPDK_TRUE;

	if(this->list_ctrl != NULL)
	{
		this->list_ctrl->visiable = EPDK_TRUE;
	}

	music_player_get_last_para(this);

	if(this->list_ctrl != NULL)				//先删除列表，防止闪烁
	{
		delete_music_list(this->list_ctrl);
		this->list_ctrl = NULL;
	}

	{
		char filename[RAT_MAX_FULL_PATH_LEN];
		char search_path[RAT_MAX_FULL_PATH_LEN];
		int index = 0;
		eLIBs_memset(filename, 0, sizeof(filename));
		musicplayer_rat_get_search_path(root_type, search_path);

		if(robin_get_fsm_cur_file(filename, sizeof(filename)) != -1)
		{
			__msg("==========================robin get cur play file = %s!\n", filename);

			if(this->list_ctrl->rat_handle != 0)
			{
				//this->list_ctrl->rat_handle = rat_open(search_path, RAT_MEDIA_TYPE_AUDIO, 0);
#if SP_FavoriteFiles
				this->list_ctrl->rat_handle = rat_open_ex(root_type, this->list_ctrl->media_type, 0);
#else
				this->list_ctrl->rat_handle = rat_open_ex(root_type, RAT_MEDIA_TYPE_AUDIO, 0);
#endif
				__msg("==========================this->list_ctrl->rat_handle = %d!\n", this->list_ctrl->rat_handle);

				if(rat_set_file_for_play(this->list_ctrl->rat_handle, filename) == -1)
				{
					return EPDK_FAIL;
				}

				index = robin_npl_get_cur();
				__msg("=================robin npl get cur index = %d!\n", index);
				__here__;
			}
			else
			{
				return EPDK_FAIL;
			}
		}
		else
		{
			__wrn("======================robin get cur play file failed!\n");
			return EPDK_FAIL;
		}
	}

	{
		music_list_para_t list_para;
		__msg("music list is initializing!\n");
		this->root_type = root_type;
		list_para.framewin = this->h_framewin;
		list_para.layer = this->music_layer;
		list_para.visiable = this->visiable;
		list_para.root_type = this->root_type;
		list_para.media_type = this->media_type;
		list_para.font = this->font;
		//赋值，用于创建list时检测是否从文件管理恢复后台播放
		list_para.explorer_play_music = this->explorer_play_music;
		list_para.music_file_name = this->music_file_name;
		__msg("list_para.explorer_play_music = %d\n", list_para.explorer_play_music);
		__msg("new_music_list(&list_para);\n");
		this->list_ctrl = new_music_list(&list_para);

		if(this->list_ctrl != NULL)
		{
			show_music_list(this->list_ctrl);
		}
		else
		{
#if 0
			//GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
#endif
			return EPDK_FAIL;
		}
	}

	//__wait__;
	MusicSetTransColor();
	{
		//__u32 cur_time = 0;
		//cur_time = robin_get_cur_time();
	}
	DrawMusicHeadbar(this);
	{
		//__u32 cur_time = 0;
		//cur_time = robin_get_cur_time();
	}

	if(root_type == RAT_TF)
	{
		DrawMusicDirectory(1, this->font);
	}
	else if(root_type == RAT_USB)
	{
		DrawMusicDirectory(0, this->font);
	}

	DrawMusic_total(this);
	DrawMusic_playing(this);
#if SP_FavoriteFiles
	Drawmusic_play_style(this);
#endif
#if 0

	if(robin_get_fsm_status() == CEDAR_STAT_STOP)		//音乐已停止，则重新播放
	{
		{
			char *filename = this->play_file_info.filename;

			if(this->list_ctrl != NULL)
			{
				MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);
			}

			{
				ret = music_play_file(this, filename);
			}
		}
	}

#endif
	music_restore_play_file_info(this);
#if 0
	GUI_MEMDEV_CopyToLCD(draw_mem);
	GUI_MEMDEV_Select(NULL);
	GUI_MEMDEV_Delete(draw_mem);
	draw_mem = NULL;
#endif
	{
		__bool timer_ret;

		if(!GUI_IsTimerInstalled(this->h_framewin, MusicSysTimerId))
		{
			timer_ret = GUI_SetTimer(this->h_framewin, MusicSysTimerId, MUSIC_SYSTIMER_INTERVAL, NULL);						//系统时间刷新定时器

			if(!timer_ret)
			{
				__wrn("creat timer for system time failed\n");
			}
			else
			{
				__msg("--------------creat timer for system time successfully\n");
			}
		}

		/*
		if(!GUI_IsTimerInstalled(this->h_framewin, MusicCheckFocusTimerId))
		{
			timer_ret = GUI_SetTimer(this->h_framewin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);		//检测是否foucus item 与当前播放index一致
			if(!timer_ret)
			{
				__wrn("creat timer for check focus item failed\n");
			}
			else
			{
				__msg("----------------creat timer for check focus item successfully\n");
			}
		}
		*/
	}
	return EPDK_OK;
}



//获得媒体文件信息
static __s32 music_get_audio_file_info(music_player_ctrl_t *this, char *filename)
{
	robin_audio_info_t audio_file_info;
	eLIBs_memset(&audio_file_info, 0, sizeof(robin_audio_info_t));
	robin_get_audio_info(filename, &audio_file_info);
	eLIBs_strcpy(this->play_file_info.artist, audio_file_info.artist);
	eLIBs_strcpy(this->play_file_info.album, audio_file_info.album);
	this->play_file_info.bitrate = audio_file_info.bps;
	this->play_file_info.filesize = eLIBs_GetFileSize(filename);
	this->play_file_info.artist_charset = audio_file_info.artist_charset ;
	this->play_file_info.album_charset = audio_file_info.album_charset ;
	return EPDK_OK;
}


__s32 __music_load_breakpoint_info(__s32 index, __cedar_tag_inf_t *tag_inf)
{
	__s32 ret;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	ret = robin_npl_index2file(index, filename);

	if(-1 == ret)
	{
		__msg("robin_npl_index2file fail...\n");
		return -1;
	}

	__here__;

	if(!fifo_db_has_exist(NAME_MUSIC_BKPOINT))
	{
		return -1;
	}

	__here__;

	if(get_item_content_from_fifo_db(NAME_MUSIC_BKPOINT, filename, tag_inf))
	{
		return -1;
	}

	__here__;
	return 0;
}

__s32 __music_save_breakpoint_info(__s32 index)
{
	__cedar_tag_inf_t tag_inf;
	char filename[RAT_MAX_FULL_PATH_LEN] = {0};
	__s32 ret;

	/*get the tag*/
	if(EPDK_FALSE == robin_is_open())
	{
		__here__;
		return EPDK_FAIL;
	}

	__here__;
	ret = robin_get_tag(&tag_inf);

	if(0 != ret)
	{
		__msg("robin_get_tag fail...\n");
		return -1;
	}

	__here__;
	ret = robin_npl_index2file(index, filename);

	if(-1 == ret)
	{
		__msg("robin_npl_index2file fail...\n");
		return -1;
	}

	__msg("begin save breakpoint, index = %d, filename=%s\n", index, filename);

	if(!fifo_db_has_exist(NAME_MUSIC_BKPOINT))
	{
		ret = create_fifo_db(NAME_MUSIC_BKPOINT, 4, sizeof(__cedar_tag_inf_t));

		if(0 != ret)
		{
			__msg("create_fifo_db fail...\n");
			return -1;
		}
	}

	__here__;
	ret = add_item_to_fifo_db(NAME_MUSIC_BKPOINT, filename, &tag_inf);
	__msg("add_item_to_fifo_db, ret=%d\n", ret);
	return ret;
}


static __s32 music_play_file(music_player_ctrl_t *this, char *filename, __bool breakplay)
{
	__u32 total_time = 0, cur_time = 0;
	__s32 ret = 0;

	//__wait__;
	//char *filename = player_ctrl->play_file_info.filename;
	if(this  == NULL)
	{
		return EPDK_FAIL;
	}

	music_rr_flag = 0;
	this->m_ff_fb_speed = 0;

	if(this->m_hLyr_ff_fb)
	{
		GUI_LyrWinDelete(this->m_hLyr_ff_fb);
		this->m_hLyr_ff_fb = NULL;
	}

	//app_print_memory_usage("check memory in function of music_play_file", NULL);

	if(this->list_ctrl != NULL)
	{
		if((eLIBs_strlen(filename) == 0) || (this->list_ctrl->total == 0))
		{
			if(this->visiable)
			{
				DrawMusicPlayingTime(0);//显示当前播放时间
				eLIBs_printf("\n\n==DrawMusicPlayingTime==\n\n");
				CleanMusicTotalTime();//清除显示当前播放时间
				eLIBs_printf("\n\n==CleanMusicTotalTime==\n\n");
				DrawMusicTotalTime(0);//显示音乐播放总时间
				eLIBs_printf("\n\n==DrawMusicTotalTime==\n\n");
				DrawMusicProgressBar(0, 0);	//显示进度条
				eLIBs_printf("\n\n==DrawMusicProgressBar==\n\n");
				eLIBs_memset(&(this->play_file_info), 0, sizeof(audio_file_info_t));
				DrawMusicPlayerFileInfo(&(this->play_file_info));	//显示文件信息
				DrawMusicPlayStatusButton(MUSIC_PLAY);//绘制play backward button, focused or not

				if(this->menu_mode == MUSIC_EXPLORER_MODE)
				{
					SPCTRM_Clear((__u32)0x000f);		//显示最大值
				}
			}

			return EPDK_FAIL;
		}
	}

	this->CheckFocusItemCnt = 0;		//有按键清除检测焦点条目定时计数器
	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	//GUI_ResetTimer(this->h_framewin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);

	if(this->list_ctrl != NULL)
	{
		music_list_get_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));//获得当前listbar场景
		__wrn("this->list_ctrl->listbar_play_scene.focus_id:%d\n", this->list_ctrl->listbar_play_scene.focus_id);
		__wrn("this->list_ctrl->listbar_play_scene.start_id:%d\n", this->list_ctrl->listbar_play_scene.start_id);
		robin_npl_set_cur(this->list_ctrl->play_index);
		this->play_index = this->list_ctrl->play_index;
		__wrn("this->play_index=%d\n", this->play_index);
	}
	else
	{
		robin_npl_set_cur(this->play_index);
		__wrn("this->play_index=%d\n", this->play_index);
	}

	//app_print_memory_usage("robin start to play file", NULL);
	if(1 == breakplay)
	{
		char cur_playing_file[RAT_MAX_FULL_PATH_LEN];
		__cedar_tag_inf_t tag_info;
		__cedar_status_t cedar_status;
		eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
		ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
		__wrn("ret = %d, cur_playing_file=%s", ret, cur_playing_file);
		cedar_status = robin_get_fsm_status();
		__wrn("cedar_status=%d\n", cedar_status);
		__wrn("filename=%s\n", filename);

		if((CEDAR_STAT_PLAY == cedar_status
		    || CEDAR_STAT_FF == cedar_status
		    || CEDAR_STAT_RR == cedar_status
		    || CEDAR_STAT_JUMP == cedar_status
		   )
		    && EPDK_OK == ret
		    && 0 == eLIBs_stricmp(filename, cur_playing_file))
		{
			__wrn("cur music is playing, not need replay...\n");
		}
		else
		{
			ret = __music_load_breakpoint_info(this->play_index, &tag_info);

			if(EPDK_OK == ret)
			{
				ret = robin_play_file(filename, &tag_info);
			}
			else
			{
				ret = robin_play_file(filename, NULL);
			}

			if(ret == EPDK_OK)
			{
				__msg("index: %d\n", this->play_index);
			}
		}
	}
	else
	{
		ret = robin_play_file(filename, NULL);
	}

	__msg("===================music  filename is %s, return value is %d===================\n", filename, ret);
	__msg("===================robin start to play ,  return value is %d===================\n", ret);

	//app_print_memory_usage("robin start to play file finished", NULL);
	if(this->visiable)
	{
		music_get_audio_file_info(this, filename);
		total_time = robin_get_total_time();
		cur_time = robin_get_cur_time();
		__msg("======================================current time = %d, total time = %d\n", cur_time, total_time);

		if(cur_time > total_time)
		{
			cur_time = total_time;
		}

		//	robin_set_cmd_stop();
		//	robin_set_cmd_play_file(filename, NULL);
		//	robin_set_cmd_play();
		//GUI_LyrWinCacheOff();
		DrawMusicPlayingTime(cur_time);
		CleanMusicTotalTime();
		DrawMusicTotalTime(total_time);
		DrawMusicProgressBar(0, 0);					//显示进度条
		DrawMusicPlayerFileInfo(&(this->play_file_info));					//显示文件信息
		DrawMusicPlayStatusButton(MUSIC_PLAY);
		//GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	}

	if(this->visiable)
	{
		delete_music_lyrics_scene(this);

		if(this->menu_mode == MUSIC_EXPLORER_MODE)
		{
			ClearMusicPlayerContrlButtonBG();
			__msg("+++++++++SPCTRM_Clear++++++++\n");
			SPCTRM_Clear(0);		//显示最大值
			SPCTRM_Update((H_WIN)(this->h_framewin));

			if(music_lyrics_exist())		//检查是否有歌词
			{
				//有歌词则不显示频谱
				if(create_music_lyrics_scene(this) != EPDK_FAIL)
				{
					__msg("+++++++++update_music_lyrics++++++++\n");
					update_music_lyrics(this->h_lyrics);
				}
			}
			else
			{
				DrawMusic_name(filename);
			}

			/*else
			{
				SPCTRM_Clear(0);		//显示最大值
				SPCTRM_Update((H_WIN)(this->h_framewin));
			}*/
		}
		else if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
		{
			if(music_lyrics_exist())		//检查是否有歌词
			{
				//有歌词则不显示频谱
				if(create_music_lyrics_scene(this) != EPDK_FAIL)		//只创建，不显示
				{
					//update_music_lyrics(this->h_lyrics);
					__msg("create lyrics successed!\n");
				}
			}
			else
			{
				DrawMusic_name(filename);
			}
		}
	}

	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	//app_print_memory_usage("check memory in function of music_play_file finished", NULL);
	//GUI_LyrWinCacheOn();
#if MOVIE_AUTO_FLUSH_TIME
	//music_save_breakpoint(this);
#endif
	return EPDK_OK;
	//robin_set_cmd_play();
}


__s32 music_restore_play_file_info(music_player_ctrl_t *this)
{
	__s32 ret;
	__u32 total_time = 0, cur_time = 0;
	char *filename;

	if(this == NULL)
	{
		return EPDK_FAIL;
	}

	filename = this->play_file_info.filename;
	cur_time = robin_get_cur_time();
	ret = MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);

	if(EPDK_FAIL == ret)
	{
		return EPDK_FAIL;
	}

	cur_time = robin_get_cur_time();
	music_get_audio_file_info(this, filename);
	total_time = robin_get_total_time();
	cur_time = robin_get_cur_time();

	if(cur_time > total_time)
	{
		cur_time = total_time;
	}

	DrawMusicPlayingTime(cur_time);
	CleanMusicTotalTime();
	DrawMusicTotalTime(total_time);
	DrawMusicProgressBar(total_time, cur_time);					//显示进度条
	DrawMusicPlayerFileInfo(&(this->play_file_info));					//显示文件信息

	if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
	{
		DrawMusicPlayStatusButton(MUSIC_PLAY);
	}
	else
	{
		DrawMusicPlayStatusButton(MUSIC_PAUSE);
	}

	delete_music_lyrics_scene(this);

	if(this->menu_mode == MUSIC_EXPLORER_MODE)
	{
		ClearMusicPlayerContrlButtonBG();
		__msg("+++++++++SPCTRM_Clear++++++++\n");
		SPCTRM_Clear(0);		//显示最大值
		SPCTRM_Update((H_WIN)(this->h_framewin));

		if(music_lyrics_exist())		//检查是否有歌词
		{
			//有歌词则不显示频谱
			if(create_music_lyrics_scene(this) != EPDK_FAIL)
			{
				__msg("+++++++++update_music_lyrics++++++++\n");
				update_music_lyrics(this->h_lyrics);
				update_music_lyrics(this->h_lyrics);
			}
		}
		else
		{
			DrawMusic_name(filename);
		}

		/*else
		{
			SPCTRM_Clear(0);		//显示最大值
			SPCTRM_Update((H_WIN)(this->h_framewin));
		}*/
	}
	else if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
	{
		if(music_lyrics_exist())		//检查是否有歌词
		{
			//有歌词则不显示频谱
			if(create_music_lyrics_scene(this) != EPDK_FAIL)		//只创建，不显示
			{
				//update_music_lyrics(this->h_lyrics);
				__msg("create lyrics successed!\n");
			}
		}
		else
		{
			DrawMusic_name(filename);
		}
	}

	GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	return EPDK_OK;
}

//初始化音频设备
static __s32 music_player_device_init(music_player_ctrl_t *this)
{
	//__bool ret;
	__inf("_init begining!\n");
	//app_print_memory_usage("robin create", NULL);
	/*open display*/
	//ADE_Open();
	robin_open(ROBIN_MODE_AUDIO_MIN, NULL);
	robin_enable_spectrum();
	robin_set_eq_mode(this->EQ_mode);
	//music_player_set_robin_EQ_mode(this);
	robin_set_ff_rr_speed(MUSIC_FF_SPEED);
	//this->play_mode = RAT_PLAY_MODE_SEQUENCE;
	robin_set_play_mode(this->play_mode);
	//GUI_SetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	//app_print_memory_usage("robin create finished", NULL);
	return EPDK_OK;
}


//卸载音频设备
static __s32 music_player_device_uninit(music_player_ctrl_t *player_ctrl)
{
	//app_print_memory_usage("delete robin ", NULL);
	if(NULL == player_ctrl || NULL == player_ctrl->h_framewin)
	{
		return EPDK_FAIL;
	}

	__here__;

	if(GUI_IsTimerInstalled(player_ctrl->h_framewin, MusicTimerId))
	{
		GUI_KillTimer(player_ctrl->h_framewin, MusicTimerId);
	}

	if(EPDK_FALSE == robin_is_open())
	{
		__here__;
		return EPDK_FALSE;
	}

	__here__;
	robin_set_cmd_stop();
	__here__;
	//GUI_KillTimer(player_ctrl->h_framewin, MusicSpectrumTimerId);
	/*close robin*/
	robin_close();
	__here__;
	/*close de*/
	//ADE_Close();
	//app_print_memory_usage("delete robin finished", NULL);
	return EPDK_OK;
}

//


//发送一个命令参数给主界面
__s32 music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	char buf[128] = {0};
	H_WIN hDstWin;
	H_WIN hManagerWin;
	//robin_set_cmd_stop();
	hManagerWin = GUI_WinGetParent(hwin);
	hDstWin = GUI_WinGetParent(hManagerWin);
	msg.h_deswin = hDstWin;
	msg.h_srcwin = hwin;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hManagerWin), id);//
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_WinGetName(msg.h_deswin, buf);
	__msg("%s\n", buf);
	GUI_SendNotifyMessage(&msg);
	//GUI_SendMessage(&msg);
	return EPDK_OK;
}

__s32 create_music_lyrics_scene(music_player_ctrl_t *This)
{
	music_lyrics_para_t para;
	para.layer = This->music_layer;

	if(This->h_lyrics != NULL)
	{
		delete_music_lyrics_scene(This);
	}

	//app_print_memory_usage("new music lyrics", NULL);
	/*para.area.x = 250;//210;
	para.area.y = 40;	//35;
	para.area.width = 479-250;
	para.area.height= 125-40;	//125 - 35; 	//显示4行歌词*/
	//歌词显示区域
	para.area.x = 350 + 150;
	para.area.y = 200 + 80 + 40 + 85;
	para.area.width = 450;
	para.area.height =  100;	//显示4行歌?
	para.font = This->font;
	para.font_color = GUI_WHITE;
	para.hight_light_color = GUI_LIGHTRED;	//BEETLES_GRAY_COLOR;
	This->h_lyrics = new_music_lyrics(&para);

	if(This->h_lyrics == NULL)
	{
		return EPDK_FAIL;
	}

	//app_print_memory_usage("new music lyrics finished", NULL);
	return EPDK_OK;
}

__s32 delete_music_lyrics_scene(music_player_ctrl_t *This)
{
	//app_print_memory_usage("delete music lyrics", NULL);
	if(This->h_lyrics != NULL)
	{
		delete_music_lyrics(This->h_lyrics);
		This->h_lyrics = NULL;
	}

	//app_print_memory_usage("delete music lyrics finished", NULL);
	return EPDK_OK;
}

__s32 delete_music_player_volume_scene(H_WIN hfrm)
{
	music_player_ctrl_t *player_ctrl = NULL;

	if(!hfrm)
	{
		return EPDK_FAIL;
	}

	player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(hfrm);

	if(player_ctrl && player_ctrl->hVolumeBar)
	{
		VolumeBarDelete(player_ctrl->hVolumeBar);
		player_ctrl->hVolumeBar = NULL;
	}

	return EPDK_TRUE;
}


