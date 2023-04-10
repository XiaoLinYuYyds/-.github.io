#include "record_list_view.h"
#include "record_comm.h"

static void DrawRecordDirectory(__u32 focus);//, GUI_FONT *font);
static void DrawRecord_total();//(GUI_FONT *font);
static void DrawRecord_playing();//(GUI_FONT *font);

static __u32 root_type_flag = 10;

static void DrawRecord_playing(precord_list_view_t prec_list_view)//(GUI_FONT *font)
{
	__u32 index;
	char ucStringBuf[20] = {0};
	__u32 x1 = 150 + 190; //正在播放
	__u32 y1 = 553;
	GUI_RECT gui_rect2;
	char item_str[1][128];
	__s32 lang_id_2 = STRING_MUSIC_MUSIC_PLAYING;
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetBkColor(0x00000000);//设置颜色等
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	gui_rect2.x0 = x1;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0 + 85;
	gui_rect2.y1 = gui_rect2.y0 + 22;
	GUI_ClearRectEx(&gui_rect2);//清除区域
	dsk_langres_get_menu_text(lang_id_2, item_str, GUI_TITLE_MAX);//显示正在播放
	GUI_DispStringInRect((const char *)item_str, &gui_rect2, GUI_TA_HCENTER | GUI_TA_VCENTER);
	//index = robin_npl_get_cur()+1;//显示正在播放数字
	index = prec_list_view->precord_list_doc->playIndex + 1;
	eLIBs_sprintf(ucStringBuf, "%d", index);
	gui_rect2.x0 = x1 + 85;
	gui_rect2.y0 = y1;
	gui_rect2.x1 = gui_rect2.x0 + 80;
	gui_rect2.y1 = gui_rect2.y0 + 22;
	GUI_ClearRectEx(&gui_rect2);//清除区域
	GUI_DispStringInRect(ucStringBuf, &gui_rect2, GUI_TA_LEFT | GUI_TA_VCENTER);
}

static void DrawRecord_total(precord_list_view_t prec_list_view)//(GUI_FONT *font)
{
	__u32 song_total;
	char ucStringBuf[20] = {0};
	__u32 x1 = 150 + 190; //歌曲总数
	__u32 y1 = 578;
	GUI_RECT gui_rect;
	char item_str[1][128];
	__s32 lang_id_1 = STRING_MUSIC_SONG_TOTAL;
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetBkColor(0x00000000);//设置颜色等
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	gui_rect.x0 = x1;
	gui_rect.y0 = y1;
	gui_rect.x1 = gui_rect.x0 + 85;
	gui_rect.y1 = gui_rect.y0 + 20;
	GUI_ClearRectEx(&gui_rect);//清除区域
	dsk_langres_get_menu_text(lang_id_1, item_str, GUI_TITLE_MAX);//显示歌曲总数
	GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	//song_total = robin_npl_get_total_count();//显示歌曲数字
	song_total = prec_list_view->precord_list_doc->total;
	eLIBs_sprintf(ucStringBuf, "%d", song_total);
	gui_rect.x0 = x1 + 85;
	gui_rect.y0 = y1;
	gui_rect.x1 = gui_rect.x0 + 85;
	gui_rect.y1 = gui_rect.y0 + 20;
	GUI_ClearRectEx(&gui_rect);//清除区域
	GUI_DispStringInRect(ucStringBuf, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
}

static void DrawRecordDirectory(__u32 focus)//, GUI_FONT *font)
{
	void *fbmp;
	void *ubmp;
	__s32 directory_fcous = 0;
	__s32 directory_unfcous = 0;
	__u32 start_x = 20 + 80;//播放目录图标
	__u32 start_y = 553;
	GUI_RECT gui_rect;
	char item_str[1][128];
	__s32 lang_id = STRING_MUSIC_SONG_DIRECTORY;
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetBkColor(0x00000000);
	GUI_SetFont(SWFFont);
	GUI_UC_SetEncodeUTF8();
	gui_rect.x0 = start_x;//播放目录文字
	gui_rect.y0 = start_y;
	gui_rect.x1 = gui_rect.x0 + 8 * 12;
	gui_rect.y1 = gui_rect.y0 + 20;
	GUI_ClearRectEx(&gui_rect);//清除区域
	dsk_langres_get_menu_text(lang_id, item_str, GUI_TITLE_MAX);// 显示播放目录
	GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);

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
		GUI_BMP_Draw(fbmp, start_x, start_y + 25);
		GUI_BMP_Draw(ubmp, start_x + 42, start_y + 25);
	}
	else
	{
		GUI_BMP_Draw(ubmp, start_x, start_y + 25);
		GUI_BMP_Draw(fbmp, start_x + 42, start_y + 25);
	}

	dsk_theme_close(directory_fcous);
	dsk_theme_close(directory_unfcous);
}
static void record_list_view_time(precord_list_view_t prec_list_view)
{
	char ucDateBuf[32] = {0};
	char ucTimeBuf[16] = {0};
	__time_t time;
	__date_t date;
	eLIBs_GetDate(&date);
	eLIBs_GetTime(&time);
	record_date_to_char(ucDateBuf, date, '-');
	record_time_to_char(ucTimeBuf, time, ':');
	eLIBs_strcat(ucDateBuf, " ");
	eLIBs_strcat(ucDateBuf, ucTimeBuf);
	GUI_SetColor(APP_COLOR_WHITE);
	GUI_DispStringAt(ucDateBuf, 115, 0);
}

static void record_list_view_Top(precord_list_view_t prec_list_view)
{
	char vol[5] = {0};
	power_level_e batterySt = DSK_POWER_LEVEL_0;
	draw_record_bmp(prec_list_view->pui, RECORD_BAR_TOP_BMP);
	draw_record_lang(prec_list_view->pui, STRING_RECORD_PLAY, APP_COLOR_WHITE);
	//if( dsk_volume_is_on() )
	//	draw_record_bmp( prec_list_view->pui, RECORD_VOL_ON_BMP );
	//else
	//	draw_record_bmp( prec_list_view->pui, RECORD_VOL_OFF_BMP );
	//eLIBs_int2str_dec(dsk_volume_get(), vol);
	GUI_SetColor(APP_COLOR_WHITE);
	//GUI_DispStringAt( vol, 300,0 );
	//dsk_power_get_voltage_level(&batterySt);
	//draw_record_bmp( prec_list_view->pui, RECORD_BATTERY_0_BMP+batterySt );
	record_list_view_time(prec_list_view);
}

static void record_list_view_MusicList(precord_list_view_t prec_list_view)
{
	__s32 		cnt = 0, curIndex, topIndex, total;
	__s32		scrollStartY, scrollEndY, height = 388;
	GUI_RECT 	rect = {0 + 80 + 100, 42 + 60, 160 + 80 + 100, 58 + 60}; //录音列表music_progress_focus
	char 		str[50];
	//音乐卷轴
	draw_record_bmp(prec_list_view->pui, RECORD_MUSIC_PROGRESS_BG_BMP);
	draw_record_bmp(prec_list_view->pui, RECORD_MUSIC_PROGRESS_UP_BMP);
	draw_record_bmp(prec_list_view->pui, RECORD_MUSIC_PROGRESS_DOWN_BMP);

	//歌曲
	if(prec_list_view->precord_list_doc == NULL)
	{
		return;
	}

	curIndex 	= prec_list_view->precord_list_doc->curIndex;
	topIndex 	= prec_list_view->precord_list_doc->topIndex;
	total    	= prec_list_view->precord_list_doc->total;

	for(cnt = topIndex;	cnt < total && cnt < RECORD_MUSIC_LIST_NUM + topIndex; cnt++)
	{
		record_list_getFileName(prec_list_view->precord_list_doc, cnt, str);

		if(curIndex == cnt)
		{
			draw_record_bmp_coor(prec_list_view->pui, RECORD_MUSIC_PROGRESS_FOCUS_BMP, rect.x0, rect.y0 - 2);
		}

		draw_record_str(prec_list_view->pui, str, APP_COLOR_WHITE, &rect);
		rect.y0 += RECORD_MUSIC_NAME_HEIGHT;
		rect.y1 += RECORD_MUSIC_NAME_HEIGHT;
	}

	if(total > RECORD_MUSIC_LIST_NUM)
	{
		scrollStartY = height * topIndex / total;
		scrollEndY = height * (RECORD_MUSIC_LIST_NUM + topIndex) / total;

		for(; scrollStartY < scrollEndY; scrollStartY++)
		{
			draw_record_bmp_coor(prec_list_view->pui, RECORD_MUSIC_PROGRESS_FLAG_BMP, 183 + 169 + 100, scrollStartY + 33 + 60);
		}
	}
}

static void record_list_view_MusicMenu(precord_list_view_t prec_list_view)
{
	FOCUSE_STATE musicList[MUSIC_MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL};
	RECORD_LIST_FOCUSE focuse;

	if(prec_list_view->curPart == RECORD_MUSIC_VOLUME)
	{
		focuse = prec_list_view->oldPart;
	}
	else
	{
		focuse = prec_list_view->curPart;
	}

	if(focuse == RECORD_MUSIC_LIST)
	{
		__s16 xCoor = 210 + 210 + 50 + 100, yCoor = 50 + 100 + 60, i, imgH = 60; //频谱
		__u32 height;

		for(i = 0; i < RECORD_LIST_SPCTRM_LEVEL; i++)
		{
			height = prec_list_view->precord_list_doc->spctrmLevel[i] * imgH / 0xF;

			if(prec_list_view->precord_list_doc->spctrmLevel[i] > 0xF)
			{
				height = imgH;
			}

			draw_record_bmp_cut(prec_list_view->pui, RECORD_MUSIC_PARTICLE, xCoor, yCoor, height);
			height = prec_list_view->precord_list_doc->oldSpctrmLevel[i] * imgH / 0xF;

			if(prec_list_view->precord_list_doc->oldSpctrmLevel[i] > 0xF)
			{
				height = imgH;
			}

			draw_record_bmp_coor(prec_list_view->pui, RECORD_MUSIC_BOUY, xCoor, yCoor + imgH - height);
			xCoor += 11;
		}
	}
	else
	{
		musicList[prec_list_view->curMenu] = prec_list_view->curMenuSt;
		draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_BACK,  musicList[MUSIC_MENU_BACK]);
		draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_BACKWARD,   musicList[MUSIC_MENU_BACKWARD]);
		draw_record_music_mapping(prec_list_view->pui, prec_list_view->playMenu,	musicList[prec_list_view->playMenu]);
		draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_FORWARD,	musicList[MUSIC_MENU_FORWARD]);
		draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_DEL, 	musicList[MUSIC_MENU_DEL]);
	}
}

static void record_list_view_MusicPlayer(precord_list_view_t prec_list_view)
{
	char times[10] = {0};
	__u32 cnt = 0, len = 0;
	__s16 offsetX = 0, offsetY = 0;
	//播放进度背景
	draw_record_bmp(prec_list_view->pui, RECORD_66_BMP);

	if(prec_list_view->precord_list_doc->musicMaxTimes == 0)
	{
		return;
	}

	if(prec_list_view->playMenu == MUSIC_MENU_PAUSE)
	{
		draw_record_lang(prec_list_view->pui, STRING_PLAYING, APP_COLOR_WHITE);
	}
	else if(prec_list_view->playMenu == MUSIC_MENU_PLAY)
	{
		draw_record_lang(prec_list_view->pui, STRING_PAUSE, APP_COLOR_WHITE);
	}

	GUI_SetColor(APP_COLOR_WHITE);
	record_ms_to_char(times, prec_list_view->precord_list_doc->musicMaxTimes, ':');
	GUI_DispStringAt(times, 374 + 210 + 50 + 100, 155 + 200); //播放时间
	record_ms_to_char(times, prec_list_view->precord_list_doc->musicCurTimes, ':');
	GUI_DispStringAt(times, 210 + 210 + 50 + 100, 155 + 200); //总时间
	//42 = 170/4
	len = (prec_list_view->precord_list_doc->musicCurTimes * 42) / prec_list_view->precord_list_doc->musicMaxTimes;

	for(cnt = 0; cnt < len; cnt++)
	{
		if(cnt == 0)
		{
			draw_record_bmp_ext(prec_list_view->pui, RECORD_66_LEFT_BMP,  offsetX, offsetY);
		}
		else if(cnt == 42)
		{
			draw_record_bmp_ext(prec_list_view->pui, RECORD_66_MID_BMP,  offsetX, offsetY);
			offsetX += 2;
			draw_record_bmp_ext(prec_list_view->pui, RECORD_66_RIGHT_BMP,  offsetX, offsetY);
		}
		else
		{
			draw_record_bmp_ext(prec_list_view->pui, RECORD_66_MID_BMP,  offsetX, offsetY);
		}

		offsetX += 4;
	}
}

static void record_list_view_delBox(precord_list_view_t prec_list_view)
{
	__s16 offsetX = 0, offsetY = 0;

	if(prec_list_view->delState == RECORD_DEL_NONE)
	{
		return;
	}

	draw_record_bmp(prec_list_view->pui, RECORD_DEL_BOX_BMP);
	draw_record_bmp_ext(prec_list_view->pui, RECORD_DEL_SEL0_0_BMP + (prec_list_view->delState == RECORD_DEL_OK), offsetX, offsetY);
	offsetX += 71;
	draw_record_bmp_ext(prec_list_view->pui, RECORD_DEL_SEL0_0_BMP + (prec_list_view->delState == RECORD_DEL_CAL), offsetX, offsetY);
	draw_record_lang(prec_list_view->pui, STRING_YES, APP_COLOR_WHITE);
	draw_record_lang(prec_list_view->pui, STRING_NO, APP_COLOR_WHITE);
	draw_record_lang(prec_list_view->pui, STRING_DELETE, APP_COLOR_WHITE);
	draw_record_lang_wrap(prec_list_view->pui, STRING_MSG4, APP_COLOR_WHITE);
}

static void record_list_view_volBar(precord_list_view_t prec_list_view)
{
	char volStr[5] = {0};
	__s16 volume = 0, i = 0, volX = 76, volY = 228;

	if(prec_list_view->curPart == RECORD_MUSIC_VOLUME)
	{
		draw_record_bmp(prec_list_view->pui, RECORD_VOL_BAR_BG);
		draw_record_bmp(prec_list_view->pui, RECORD_VOL_BAR_BODY);
		draw_record_bmp(prec_list_view->pui, RECORD_VOL_SPEAKER_RIGHT);
		draw_record_bmp_coor(prec_list_view->pui, RECORD_VOL_ON_BMP, 39, 224);
		volume = dsk_volume_get();
		eLIBs_int2str_dec(volume, volStr);
		GUI_SetColor(APP_COLOR_WHITE);
		GUI_DispStringAt(volStr, 363, 222);

		for(i = 0; i < (volume * 240 / 30) / 5; i++)
		{
			if(i == 0)
			{
				draw_record_bmp_coor(prec_list_view->pui, RECORD_VOL_LEFT, volX, volY);
			}
			else if(i == 47)
			{
				draw_record_bmp_coor(prec_list_view->pui, RECORD_VOL_RIGHT, volX, volY);
			}
			else
			{
				draw_record_bmp_coor(prec_list_view->pui, RECORD_VOL_MID, volX, volY);
			}

			volX += 5;
		}
	}
}

void record_list_view_show(precord_list_view_t prec_list_view, H_WIN layer)
{
	GUI_MEMDEV_Handle	draw_mem;
	__s16 x1, y1;
	__s16 x0 = 0, y0 = 0, w = 1024, h = 600;

	if(GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
	{
		__msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
		return;
	}

	x1 = w + x0 - 1;
	y1 = h + y0 - 1;
	GUI_LyrWinSel(layer);
	draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

	if(draw_mem == 0)
	{
		__msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
		return;
	}

	GUI_MEMDEV_Select(draw_mem);
	GUI_SetBkColor(0x00);
	GUI_ClearRect(x0, y0, x1,  y1);
	record_list_view_Top(prec_list_view);
	record_list_view_MusicMenu(prec_list_view);
	record_list_view_MusicPlayer(prec_list_view);
	record_list_view_MusicList(prec_list_view);
	record_list_view_delBox(prec_list_view);
	record_list_view_volBar(prec_list_view);
	__msg("prec_list_view->precord_list_doc->curRootType =  %d\n", prec_list_view->precord_list_doc->curRootType);

	if(prec_list_view->precord_list_doc->curRootType == RAT_TF)
	{
		DrawRecordDirectory(1);
	}
	else if(prec_list_view->precord_list_doc->curRootType == RAT_USB)
	{
		DrawRecordDirectory(0);
	}

	DrawRecord_total(prec_list_view);
	DrawRecord_playing(prec_list_view);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_MEMDEV_CopyToLCD(draw_mem);
	GUI_MEMDEV_Select(NULL);
	GUI_MEMDEV_Delete(draw_mem);
	draw_mem = NULL;
}


























