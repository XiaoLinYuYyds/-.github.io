/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: spsc_ctrl.c
* By      	:
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "spsc_ctrl.h"
#include "spsc_ctrl_ui.h"
#include "common/common.h"
typedef struct
{
	// ê?è?2?êy
	H_LYR hlyr;
	H_WIN hparent;
	__s32 scene_id;

	//?ú2?2?êy
	H_WIN hfrm;
	__hdle long_str_handle;

	char long_string[RAT_MAX_FULL_PATH_LEN];
	char str_file_name[RAT_MAX_FULL_PATH_LEN];
	char str_file_info[128];
	char str_video_info[128];
	char str_audio_info[128];

}
movie_spsc_ctrl_scene_t;

typedef enum
{
  STR_MOVIE_AUTO = 12,             //auto
  STR_MOVIE_STRECTCH,           //拉伸
  STR_MOVIE_ORIGINAL,           //原始
  STR_MOVIE_ACTUAL,            //真实比例
  STR_MOVIE_SUB_SIZE,           //大小

} movie_screen_size_sta;


static __s32 lang_id[] =
{
	STRING_MOVIE_PLAY_PREVIOUS,
	STRING_MOVIE_PLAY_NEXT,
	STRING_MOVIE_PLAY_BACKWARD,
	STRING_MOVIE_PLAY_FORWARD,
	STRING_MOVIE_PLAY_PAUSE,
	STRING_MOVIE_SOUND_CHANEL,
	STRING_MOVIE_TRK_TRACK,
	STRING_MOVIE_PLAY_TIME,
	STRING_MOVIE_CIRCEL_MODE,
	STRING_MOVIE_SCREEN_SIZE,
	STRING_MOVIE_BRIGHT_SET,
	STRING_MOVIE_TV_OUT,
	STRING_MOVIE_AUTO,               //auto
	STRING_MOVIE_STRECTCH,           //strectch
	STRING_MOVIE_ORIGINAL,          //original
	STRING_MOVIE_ACTUAL,            //真实比例
	STRING_MOVIE_SUB_SIZE,           //大小
};

static void movie_draw_text(__u32 index);

static __s32 __movie_spsc_ctrl_init_ui(void)
{
	movie_spsc_ctrl_uipara_t *ui_para;
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		__msg("movie_spsc_get_uipara fail...\n");
		return EPDK_FAIL;
	}

	{
		int j;

		for(j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
		{
			if(ui_para->uipara_bg.res_id[j])
			{
				ui_para->uipara_bg.res_hdl[j] = dsk_theme_open(ui_para->uipara_bg.res_id[j]);

				if(!ui_para->uipara_bg.res_hdl[j])
				{
					__msg("dsk_theme_open fail...\n");
				}
				else//?è??×??ú′?
				{
					//  dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[j]);
				}
			}
		}
	}

	{
		int i;
		int j;

		for(i = 0 ; i < movie_spsc_ctrl_icon_num ; i++)
		{
			for(j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
			{
				if(ui_para->uipara_icon[i].res_id[j])
				{
					ui_para->uipara_icon[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_icon[i].res_id[j]);

					if(!ui_para->uipara_icon[i].res_hdl[j])
					{
						__msg("dsk_theme_open fail...\n");
					}
					else//?è??×??ú′?
					{
						//  dsk_theme_hdl2buf(ui_para->uipara_icon[i].res_hdl[j]);
					}
				}
			}
		}
	}

	{
		int i;

		for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_playmode_res) ; i++)
		{
			if(ui_para->uipara_playmode_res[i].res_id)
			{
				ui_para->uipara_playmode_res[i].res_hdl = dsk_theme_open(ui_para->uipara_playmode_res[i].res_id);

				if(!ui_para->uipara_playmode_res[i].res_hdl)
				{
					__msg("dsk_theme_open fail...\n");
				}
				else//?è??×??ú′?
				{
					// dsk_theme_hdl2buf(ui_para->uipara_playmode_res[i].res_hdl);
				}
			}
		}
	}

	{
		int i;

		for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_screenmode_res) ; i++)
		{
			if(ui_para->uipara_screenmode_res[i].res_id)
			{
				ui_para->uipara_screenmode_res[i].res_hdl = dsk_theme_open(ui_para->uipara_screenmode_res[i].res_id);

				if(!ui_para->uipara_screenmode_res[i].res_hdl)
				{
					__msg("dsk_theme_open fail...\n");
				}
				else//?è??×??ú′?
				{
					//  dsk_theme_hdl2buf(ui_para->uipara_screenmode_res[i].res_hdl);
				}
			}
		}
	}

	{
		if(ui_para->uipara_bright_icon_res.res_id)
		{
			ui_para->uipara_bright_icon_res.res_hdl = dsk_theme_open(ui_para->uipara_bright_icon_res.res_id);

			if(!ui_para->uipara_bright_icon_res.res_hdl)
			{
				__msg("dsk_theme_open fail...\n");
			}
			else//?è??×??ú′?
			{
				//dsk_theme_hdl2buf(ui_para->uipara_bright_icon_res.res_hdl);
			}
		}
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_deinit_ui(void)
{
	movie_spsc_ctrl_uipara_t *ui_para;
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		__msg("movie_spsc_get_uipara fail...\n");
		return EPDK_FAIL;
	}

	{
		int j;

		for(j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
		{
			if(ui_para->uipara_bg.res_hdl[j])
			{
				dsk_theme_close(ui_para->uipara_bg.res_hdl[j]);
				ui_para->uipara_bg.res_hdl[j] = NULL;
			}
		}
	}

	{
		int i;
		int j;

		for(i = 0 ; i < movie_spsc_ctrl_icon_num ; i++)
		{
			for(j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
			{
				if(ui_para->uipara_icon[i].res_hdl[j])
				{
					dsk_theme_close(ui_para->uipara_icon[i].res_hdl[j]);
					ui_para->uipara_icon[i].res_hdl[j] = NULL;
				}
			}
		}
	}

	{
		int i;

		for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_playmode_res) ; i++)
		{
			if(ui_para->uipara_playmode_res[i].res_hdl)
			{
				dsk_theme_close(ui_para->uipara_playmode_res[i].res_hdl);
				ui_para->uipara_playmode_res[i].res_hdl = NULL;
			}
		}
	}

	{
		int i;

		for(i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_screenmode_res) ; i++)
		{
			if(ui_para->uipara_screenmode_res[i].res_hdl)
			{
				if(ui_para->uipara_screenmode_res[i].res_hdl)
				{
					dsk_theme_close(ui_para->uipara_screenmode_res[i].res_hdl);
					ui_para->uipara_screenmode_res[i].res_hdl = NULL;
				}
			}
		}
	}

	{
		if(ui_para->uipara_bright_icon_res.res_hdl)
		{
			dsk_theme_close(ui_para->uipara_bright_icon_res.res_hdl);
			ui_para->uipara_bright_icon_res.res_hdl = NULL;
		}
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_icon_draw_item(movie_spsc_ctrl_scene_t *scene_para, __s32 index, __s32 focus)
{
	movie_spsc_ctrl_uipara_t *ui_para;
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		__msg("movie_spsc_get_uipara fail...\n");
		return EPDK_FAIL;
	}

	if(index < 0 || index >= movie_spsc_ctrl_icon_num)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(focus < 0 || focus > 1)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr)
	{
		__msg("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
	{
		__msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	{
		void *pbmp;

		if(!ui_para->uipara_icon[index].res_hdl[focus])
		{
			__msg("res_hdl is null...\n");
			return EPDK_FAIL;
		}

		pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[index].res_hdl[focus]);

		if(!pbmp)
		{
			__msg("pbmp is null...\n");
			return EPDK_FAIL;
		}

		if(0/*0 == index*/)
		{
			ES_FILE *fp;
			fp = eLIBs_fopen("f:\\1.bin", "wb+");

			if(fp)
			{
				eLIBs_fwrite(pbmp, 1, dsk_theme_hdl2size(ui_para->uipara_icon[index].res_hdl[focus])
				             , fp);
				eLIBs_fclose(fp);
			}
		}

		//draw icon
		{
			__s32 ret;
			ret = GUI_BMP_Draw(pbmp, ui_para->uipara_icon[index].x, ui_para->uipara_icon[index].y);

			if(0 != ret)
			{
				__msg("GUI_BMP_Draw fail...\n");
				return EPDK_FAIL;
			}
		}
		//draw text
		{
			// 			if (ui_para->uipara_icon_text[index])
			// 			{
			// 			}
		}
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_bg_draw(movie_spsc_ctrl_scene_t *scene_para)
{
	movie_spsc_ctrl_uipara_t *ui_para;
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		__msg("movie_spsc_get_uipara fail...\n");
		return EPDK_FAIL;
	}

	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr)
	{
		__msg("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
	{
		__msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	{
		__s32 j;
		void *pbmp;

		for(j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
		{
			if(ui_para->uipara_bg.res_hdl[j])
			{
				pbmp = dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[j]);

				if(!pbmp)
				{
					__msg("dsk_theme_hdl2buf fail...\n");
					continue;
				}

				GUI_BMP_Draw(pbmp, ui_para->uipara_bg.x, ui_para->uipara_bg.y);
			}
		}
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_static_icon_draw(movie_spsc_ctrl_scene_t *scene_para, movie_spsc_ctrl_static_icon_t type)
{
	movie_spsc_ctrl_uipara_t *ui_para;
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		__msg("movie_spsc_get_uipara fail...\n");
		return EPDK_FAIL;
	}

	if(type < 0 || type >= movie_spsc_ctrl_static_icon_num)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr)
	{
		__msg("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
	{
		__msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	switch(type)
	{
		case movie_spsc_ctrl_static_icon_playmode:
		{
			void *pbmp;
			orc_play_mode_e playmode;
			playmode = robin_get_play_mode();

			if(playmode >= BEETLES_TBL_SIZE(ui_para->uipara_playmode_res))
			{
				playmode = 0;
			}

			if(!ui_para->uipara_playmode_res[playmode].res_hdl)
			{
				__msg("ui_para->uipara_playmode_res[playmode].res_hdl is null...\n");
				return EPDK_FAIL;
			}

			pbmp = dsk_theme_hdl2buf(ui_para->uipara_playmode_res[playmode].res_hdl);

			if(!pbmp)
			{
				__msg("dsk_theme_hdl2buf fail...\n");
				return EPDK_OK;
			}

			GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_playmode].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_playmode].y);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_bright:
		{
			void *pbmp;

			if(!ui_para->uipara_bright_icon_res.res_hdl)
			{
				__msg("ui_para->uipara_bright_icon_res.res_hdl is null...\n");
				return EPDK_FAIL;
			}

			pbmp = dsk_theme_hdl2buf(ui_para->uipara_bright_icon_res.res_hdl);

			if(!pbmp)
			{
				__msg("dsk_theme_hdl2buf fail...\n");
				return EPDK_OK;
			}

			GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_bright].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_bright].y);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_bright_text:
		{
			__s32 bright;
			char str[32] = {0};
			GUI_RECT gui_rect;
			bright = dsk_get_display_global(DISP_CMD_GET_BRIGHT);
			eLIBs_sprintf(str, "%d", bright);

			if(SWFFont)
			{
				GUI_SetFont(SWFFont);
			}
			else
			{
				GUI_SetFont(GUI_GetDefaultFont());
			}

			GUI_SetColor(APP_COLOR_WHITE);
			GUI_SetBkColor(0xF0);
			GUI_UC_SetEncodeUTF8();
			gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_bright_text].x;
			gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_bright_text].y;
			gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_bright_text].w;
			gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_bright_text].h;
			GUI_ClearRectEx(&gui_rect);
			GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_screen_mode:
		{
			void *pbmp;
			robin_zoom_e zoom;
			zoom = robin_get_zoom();

			if(zoom >= BEETLES_TBL_SIZE(ui_para->uipara_screenmode_res))
			{
				zoom = 0;
			}

			if(!ui_para->uipara_screenmode_res[zoom].res_hdl)
			{
				__msg("ui_para->uipara_screenmode_res[zoom].res_hdl is null...\n");
				return EPDK_FAIL;
			}

			pbmp = dsk_theme_hdl2buf(ui_para->uipara_screenmode_res[zoom].res_hdl);

			if(!pbmp)
			{
				__msg("dsk_theme_hdl2buf fail...\n");
				return EPDK_OK;
			}

			GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_screen_mode].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_screen_mode].y);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_file_info:
		{
#if 0
			__s32 ret;
			__s32 index;
			char file[RAT_MAX_FULL_PATH_LEN] = {0};
			char str[RAT_MAX_FULL_PATH_LEN] = {0};
			char strtmp[32];
			//μè′ycedar°????t2￥?eà′
			ret = movie_wait_for_in_cedar_play_sta();

			if(EPDK_FAIL == ret)
			{
				return EPDK_OK;
			}

			if(scene_para->str_file_info[0])
			{
				eLIBs_strcpy(str, scene_para->str_file_info);
			}
			else
			{
				index = robin_npl_get_cur();

				if(-1 == index)
				{
					__msg("robin_npl_get_cur fail...\n");
					return EPDK_FAIL;
				}

				ret = robin_npl_index2file(index, file);

				if(EPDK_FAIL == ret)
				{
					__msg("robin_npl_index2file fail...\n");
					return EPDK_FAIL;
				}

				ret = eLIBs_GetFileSize(file);
				filesize2str(ret, strtmp);
				dsk_langres_get_menu_text(STRING_MOVIE_FILE_INFO, str, sizeof(str));
				eLIBs_strcat(str, strtmp);
				__msg("str=%s\n", str);
				eLIBs_strcpy(scene_para->str_file_info, str);
			}

			{
				GUI_RECT gui_rect;

				if(SWFFont)
				{
					GUI_SetFont(SWFFont);
				}
				else
				{
					GUI_SetFont(GUI_GetDefaultFont());
				}

				GUI_SetColor(APP_COLOR_WHITE);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].x;
				gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].h;
				GUI_ClearRectEx(&gui_rect);
				GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER | GUI_TA_LEFT);
			}

#endif
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_video_info:
		{
#if 0
			__s32 ret;
			char str[256] = {0};
			char strtmp[32];
			//μè′ycedar°????t2￥?eà′
			ret = movie_wait_for_in_cedar_play_sta();

			if(EPDK_FAIL == ret)
			{
				return EPDK_OK;
			}

			if(scene_para->str_video_info[0])
			{
				eLIBs_strcpy(str, scene_para->str_video_info);
			}
			else
			{
				__msg("***********before robin_get_video_bps*****\n");
				ret = robin_get_video_bps();
				//ret = 123;
				__msg("************robin_get_video_bps=%d*******\n", ret);

				if(0 == ret)
				{
					eLIBs_strcpy(strtmp, "");
				}
				else
				{
					eLIBs_sprintf(strtmp, "%dkbps", (ret + 1023) / 1024);
				}

				dsk_langres_get_menu_text(STRING_MOVIE_VIDEO_INFO, str, sizeof(str));
				eLIBs_strcat(str, strtmp);
				__msg("str=%s\n", str);
				eLIBs_strcpy(scene_para->str_video_info, str);
			}

			{
				GUI_RECT gui_rect;

				if(SWFFont)
				{
					GUI_SetFont(SWFFont);
				}
				else
				{
					GUI_SetFont(GUI_GetDefaultFont());
				}

				GUI_SetColor(APP_COLOR_WHITE);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].x;
				gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].h;
				GUI_ClearRectEx(&gui_rect);
				GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER | GUI_TA_LEFT);
			}

#endif
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_audio_info:
		{
#if 0
			__s32 ret;
			char str[256] = {0};
			char strtmp[32];
			__s32 bps;
			//μè′ycedar°????t2￥?eà′
			ret = movie_wait_for_in_cedar_play_sta();

			if(EPDK_FAIL == ret)
			{
				return EPDK_OK;
			}

			if(scene_para->str_audio_info[0])
			{
				eLIBs_strcpy(str, scene_para->str_audio_info);
			}
			else
			{
				ret = robin_get_audio_bps();
				__msg("robin_get_audio_bps=%d\n", ret);

				if(0 == ret)
				{
					eLIBs_strcpy(strtmp, "");
				}
				else
				{
					eLIBs_sprintf(strtmp, "%dkbps", (ret + 1023) / 1024);
				}

				dsk_langres_get_menu_text(STRING_MOVIE_AUDIO_INFO, str, sizeof(str));
				eLIBs_strcat(str, strtmp);
				__msg("str=%s\n", str);
				eLIBs_strcpy(scene_para->str_audio_info, str);
			}

			{
				GUI_RECT gui_rect;

				if(SWFFont)
				{
					GUI_SetFont(SWFFont);
				}
				else
				{
					GUI_SetFont(GUI_GetDefaultFont());
				}

				GUI_SetColor(APP_COLOR_WHITE);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_audio_info].x;
				gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_audio_info].y;
				gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_audio_info].w;
				gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_audio_info].h;
				GUI_ClearRectEx(&gui_rect);
				GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER | GUI_TA_LEFT);
			}

#endif
			return EPDK_OK;
		}

		case movie_spsc_ctrl_static_icon_filename:
		{
			__s32 ret;
			__s32 index;
			char src_string[RAT_MAX_FULL_PATH_LEN] = {0};
			char dst_string[RAT_MAX_FULL_PATH_LEN] = {0};
			char *pstr;
			movie_spsc_ctrl_uipara_t *ui_para;
			GUI_RECT gui_rect;

			//return EPDK_OK;//3¤×?·?′?1??ˉò??-?-á??￡

			if(scene_para->str_file_name[0])
			{
				eLIBs_strcpy(src_string, scene_para->str_file_name);
			}
			else
			{
				index = robin_npl_get_cur();

				if(-1 == index)
				{
					__msg("robin_npl_get_cur fail...\n");
					return EPDK_FAIL;
				}

				ret = robin_npl_index2file(index, src_string);

				if(-1 == ret)
				{
					__msg("robin_npl_index2file fail, index=%d...\n", index);
					return EPDK_FAIL;
				}

				eLIBs_strcpy(scene_para->str_file_name, src_string);
			}

			pstr = eLIBs_strchrlast(src_string, '\\');

			if(!pstr)
			{
				__msg("eLIBs_strchrlast fail...\n");
				return EPDK_FAIL;
			}

			pstr++;
			ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

			if(!ui_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].x;
			gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].y;
			gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].w - 1;
			gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].h - 1;
			GUI_SetFont(SWFFont);
			GUI_SetColor(APP_COLOR_WHITE);
			GUI_SetBkColor(0xF0);
			GUI_UC_SetEncodeUTF8();
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			GUI_ClearRectEx(&gui_rect);
			GetClippedString(&gui_rect, pstr, dst_string, "  ");
			GUI_DispStringInRect(dst_string, &gui_rect, GUI_TA_LEFT | GUI_TA_BOTTOM);
			return EPDK_OK;
		}
	}

	return EPDK_FAIL;
}

static __s32 __movie_spsc_ctrl_static_icon_draw_all(movie_spsc_ctrl_scene_t *scene_para)
{
	__s32 ret;

	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr)
	{
		__msg("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
	{
		__msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	ret = EPDK_OK;
	{
		__s32 i;
		GUI_SetBkColor(0xF0);
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

		for(i = 0 ; i < movie_spsc_ctrl_static_icon_num ; i++)
		{
			if(EPDK_FAIL == __movie_spsc_ctrl_static_icon_draw(scene_para, i))
			{
				__msg("__movie_spsc_ctrl_static_icon_draw fail, i= %d\n", i);
				ret = EPDK_FAIL;
			}
		}
	}
	return ret;
}

static void movie_draw_text(__u32 index)
{
	char item_str[128] = {0};
	GUI_RECT gui_rect;
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetColor(APP_COLOR_YELLOW);
	GUI_SetBkColor(0xF0);
	GUI_UC_SetEncodeUTF8();
	gui_rect.x0 = 40;
	gui_rect.y0 = 23;
	gui_rect.x1 = 160;
	gui_rect.y1 = 46;
	GUI_ClearRectEx(&gui_rect);
	dsk_langres_get_menu_text(lang_id[index], item_str, GUI_TITLE_MAX);
	GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}
static __s32 __movie_spsc_ctrl_icon_draw_all(movie_spsc_ctrl_scene_t *scene_para)
{
	movie_spsc_ctrl_uipara_t *ui_para;
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		__msg("movie_spsc_get_uipara fail...\n");
		return EPDK_FAIL;
	}

	if(!scene_para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hlyr)
	{
		__msg("scene_para->hlyr is null..., do not paint\n");
		return EPDK_FAIL;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
	{
		__msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
		return EPDK_FAIL;
	}

	{
		int i;

		for(i = 5 ; i < movie_spsc_ctrl_icon_num; i++)
		{
			if(ui_para->focus_icon_index == i)
			{
				__movie_spsc_ctrl_icon_draw_item(scene_para, i, 1);
			}
			else
			{
				__movie_spsc_ctrl_icon_draw_item(scene_para, i, 0);
			}
		}

		movie_draw_text(ui_para->focus_icon_index);// ??ê?2?μ￥1|?ü??±?
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_init(movie_spsc_ctrl_scene_t *scene_para)
{
	if(scene_para->long_str_handle == NULL)
	{
		__show_info_move_t show_info;
		movie_spsc_ctrl_uipara_t *ui_para;
		GUI_RECT gui_rect;
		ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

		if(!ui_para)
		{
			__err("invalid para...\n");
			return EPDK_FAIL;
		}

		eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
		show_info.hlayer = scene_para->hlyr;
		show_info.alpha_en = EPDK_FALSE;
		show_info.pFont = SWFFont;
		show_info.string = "";
		show_info.bkColor = 0xF0;
		show_info.fontColor = APP_COLOR_WHITE;
		show_info.encode_id = EPDK_CHARSET_ENM_UTF8;
		show_info.align = GUI_TA_HCENTER | GUI_TA_VCENTER;
		show_info.bmp = NULL;
		show_info.bmp_pos.x = 0;
		show_info.bmp_pos.y = 0;
		scene_para->long_str_handle = GUI_LongStringCreate(&show_info);

		if(scene_para->long_str_handle == NULL)
		{
			__wrn("create long string failed!\n");
		}
	}
	else
	{
		__msg("long string scroll has already started!\n");
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_uninit(movie_spsc_ctrl_scene_t *scene_para)
{
	if(scene_para->long_str_handle != NULL)
	{
		GUI_LongStringDelete(scene_para->long_str_handle);
		scene_para->long_str_handle = NULL;
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_start_roll(movie_spsc_ctrl_scene_t *scene_para)
{
	__show_info_move_t show_info;
	__s32 ret;
	__s32 index;
	char *pstr;
	movie_spsc_ctrl_uipara_t *ui_para;
	GUI_RECT gui_rect;
	__here__;
	//μè′ycedar°????t2￥?eà′
	ret = movie_wait_for_in_cedar_play_sta();
	__here__;

	if(EPDK_FAIL == ret)
	{
		return EPDK_OK;
	}

	__here__;

	if(scene_para->long_str_handle != NULL)
	{
		__here__;
		GUI_LongStringStop(scene_para->long_str_handle);
		__here__;
		eLIBs_memset(scene_para->long_string, 0, sizeof(scene_para->long_string));
		__here__;
		index = robin_npl_get_cur();
		__here__;

		if(-1 == index)
		{
			__msg("robin_npl_get_cur fail...\n");
			return EPDK_FAIL;
		}

		__here__;
		ret = robin_npl_index2file(index, scene_para->long_string);
		__here__;

		if(-1 == ret)
		{
			__msg("robin_npl_index2file fail, index=%d...\n", index);
			return EPDK_FAIL;
		}

		__here__;
		pstr = eLIBs_strchrlast(scene_para->long_string, '\\');

		if(!pstr)
		{
			__msg("eLIBs_strchrlast fail...\n");
			return EPDK_FAIL;
		}

		pstr++;
		__here__;
		ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

		if(!ui_para)
		{
			__err("invalid para...\n");
			return EPDK_FAIL;
		}

		__here__;
		gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].x;
		gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].y;
		gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].w;
		gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].h;
		__here__;
		GUI_SetFont(SWFFont);
		__here__;
		GUI_SetColor(APP_COLOR_WHITE);
		__here__;
		GUI_SetBkColor(0xF0);
		__here__;
		GUI_UC_SetEncodeUTF8();
		__here__;
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
		__here__;
		GUI_ClearRectEx(&gui_rect);
		__here__;
		gui_rect.x1--;
		gui_rect.y1--;
		//GUI_DispStringInRect(pstr, &gui_rect, GUI_TA_LEFT| GUI_TA_TOP);
		GUI_LyrWinSel(scene_para->hlyr);
		__here__;
		__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_filename);
		__msg("pstr=%s\n", pstr);
		eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
		show_info.hlayer = scene_para->hlyr;
		show_info.region.x = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].x;
		show_info.region.y = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].y;
		show_info.region.width = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].w;
		show_info.region.height = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].h;
		show_info.alpha_en = EPDK_FALSE;
		show_info.pFont = SWFFont;
		show_info.string = pstr;
		show_info.bkColor = 0xF0;
		show_info.fontColor = APP_COLOR_WHITE;
		show_info.encode_id = EPDK_CHARSET_ENM_UTF8;
		show_info.align = GUI_TA_LEFT | GUI_TA_BOTTOM;
		show_info.bmp = NULL;
		show_info.bmp_pos.x = 0;
		show_info.bmp_pos.y = 0;
		__msg("show_info.region.x=%d\n", show_info.region.x);
		__msg("show_info.region.y=%d\n", show_info.region.y);
		__msg("show_info.region.width=%d\n", show_info.region.width);
		__msg("show_info.region.height=%d\n", show_info.region.height);
		__msg("show_info.pFont=%x\n", show_info.pFont);
		__msg("show_info.string=%s\n", show_info.string);
		//GUI_LongStringStop(scene_para->long_str_handle);
		__here__;
		GUI_LongStringReset(scene_para->long_str_handle, &show_info);
		__here__;
		GUI_LongStringStart(scene_para->long_str_handle);
		__here__;
	}

	return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_stop_roll(movie_spsc_ctrl_scene_t *scene_para)
{
	if(scene_para->long_str_handle != NULL)
	{
		return GUI_LongStringStop(scene_para->long_str_handle);
	}

	return EPDK_FAIL;
}

static __s32 __movie_spsc_ctrl_notify_parent(movie_spsc_ctrl_scene_t *scene_para, __gui_msg_t *msg, movie_spsc_ctrl_icon_t icon_id)
{
	switch(icon_id)
	{
		case movie_spsc_ctrl_icon_prev:
		{
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_prev, 0, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_next:
		{
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_next, 0, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_rr:
		{
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_rr, 0, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_ff:
		{
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_ff, 0, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_playpause:
		{
			__cedar_status_t fsm_sta;
			fsm_sta = robin_get_fsm_status();

			if(CEDAR_STAT_PLAY == fsm_sta)
			{
				movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_playpause, 0, 0);
			}
			else if(CEDAR_STAT_PAUSE == fsm_sta
			        || CEDAR_STAT_FF == fsm_sta
			        || CEDAR_STAT_RR == fsm_sta)
			{
				movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_playpause, 1, 0);
			}
			else
			{
				__msg("cedar not in stop/jump status, not need process...\n");
			}

			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_channelset:
		{
			__s32 channel;
			channel = robin_get_channel();
			channel++;

			if(channel >= 3)
			{
				channel = 0;
			}

			{
				char strtmp[16] = {0};
				char item_str[128] = {0};
				GUI_RECT gui_rect;
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				GUI_SetColor(APP_COLOR_YELLOW);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = 40;
				gui_rect.y0 = 23;
				gui_rect.x1 = 160;
				gui_rect.y1 = 46;
				GUI_ClearRectEx(&gui_rect);
				dsk_langres_get_menu_text(lang_id[movie_spsc_ctrl_icon_channelset], item_str, GUI_TITLE_MAX);

				if(AUDIO_DEV_CHANNEL_STEREO == channel)
				{
					dsk_langres_get_menu_text(STRING_MOVIE_CHANEL_STEREO, item_str, GUI_TITLE_MAX);
					//eLIBs_sprintf(strtmp, ": stereo");
				}
				else if(AUDIO_DEV_CHANNEL_LEFT == channel)
				{
					dsk_langres_get_menu_text(STRING_MOVIE_CHANEL_LEFT, item_str, GUI_TITLE_MAX);
					//eLIBs_sprintf(strtmp, ": left");
				}
				else
				{
					dsk_langres_get_menu_text(STRING_MOVIE_CHANEL_RIGHT, item_str, GUI_TITLE_MAX);
					//eLIBs_sprintf(strtmp, ": right");
				}

				//eLIBs_strcat(item_str, strtmp);
				GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			}

			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_channelset, channel, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_trackset:
		{
			__s32 track;
			__audstream_profile_t   track_para;
			robin_get_track_info(&track_para);
			__msg("track_para.nAudStrmNum=%d\n", track_para.nAudStrmNum);

			if(track_para.nAudStrmNum == 0)
			{
				return EPDK_OK;
			}

			track = (robin_get_track_index() + 1) % track_para.nAudStrmNum;
			{
				char strtmp[16] = {0};
				char item_str[128] = {0};
				GUI_RECT gui_rect;
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				GUI_SetColor(APP_COLOR_YELLOW);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = 40;
				gui_rect.y0 = 23;
				gui_rect.x1 = 160;
				gui_rect.y1 = 46;
				GUI_ClearRectEx(&gui_rect);
				dsk_langres_get_menu_text(lang_id[movie_spsc_ctrl_icon_trackset], item_str, GUI_TITLE_MAX);
				eLIBs_sprintf(strtmp, ": %d", track + 1);
				eLIBs_strcat(item_str, strtmp);
				GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			}
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_trackset, track, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_timeset:
		{
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_timeset, 0, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_playmode:
		{
			__s32 playmode;
			playmode = robin_get_play_mode();
			playmode++;

			if(playmode >= 5)
			{
				playmode = 0;
			}

			{
				// 播放模式更改字符描述 : lincoat
				char strtmp[16] = {0};
				char item_str[128] = {0};
				GUI_RECT gui_rect;
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				GUI_SetColor(APP_COLOR_YELLOW);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = 40;
				gui_rect.y0 = 23;
				gui_rect.x1 = 160;
				gui_rect.y1 = 46;
				GUI_ClearRectEx(&gui_rect);

				switch(playmode)
				{
					case RAT_PLAY_MODE_RANDOM :
						dsk_langres_get_menu_text(STRING_MOVIE_PLAY_RANDOM, item_str, GUI_TITLE_MAX);
						break;

					case RAT_PLAY_MODE_ROTATE_ALL:
						dsk_langres_get_menu_text(STRING_MOVIE_PLAY_ROTATE_ALL, item_str, GUI_TITLE_MAX);
						break;

					case RAT_PLAY_MODE_ROTATE_ONE:
						dsk_langres_get_menu_text(STRING_MOVIE_PLAY_ROTATE_ONE, item_str, GUI_TITLE_MAX);
						break;

					case RAT_PLAY_MODE_SEQUENCE:
						dsk_langres_get_menu_text(STRING_MOVIE_PLAY_ALL, item_str, GUI_TITLE_MAX);
						break;

					case RAT_PLAY_MODE_ONLY_ONCE :
					default :
						dsk_langres_get_menu_text(STRING_MOVIE_PLAY_ONCE, item_str, GUI_TITLE_MAX);
						break;
				}

				eLIBs_strcat(item_str, strtmp);
				GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			}

			movie_cmd2parent_syn(msg->h_deswin, spsc_ctrl_scene_msg_playmode, playmode, 0);//oó??Dèòaá￠?′?üD?ui￡?Dè·￠?íí?2????￠
			GUI_LyrWinSel(scene_para->hlyr);
			__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_playmode);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_brightset:
		{
			movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_brightset, 0, 0);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_screenset:
		{
#if 0
			movie_cmd2parent_syn(msg->h_deswin, spsc_ctrl_scene_msg_screenset, 0, 0);//oó??Dèòaá￠?′?üD?ui￡?Dè·￠?íí?2????￠
			GUI_LyrWinSel(scene_para->hlyr);
			__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_screen_mode);
			return EPDK_OK;
#endif
			__s32 zoom;
			zoom = robin_get_zoom();
			zoom++;

			if(zoom > 5)
			{
				zoom = 0;
			}

			{
				char strtmp[48] = {0};
				char item_str[128] = {0};
				char temp_str[32] = {0};
				GUI_RECT gui_rect;
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				GUI_SetColor(APP_COLOR_YELLOW);
				GUI_SetBkColor(0xF0);
				GUI_UC_SetEncodeUTF8();
				gui_rect.x0 = 40;
				gui_rect.y0 = 23;
				gui_rect.x1 = 160;
				gui_rect.y1 = 46;
				GUI_ClearRectEx(&gui_rect);

				if(0 == zoom)
				{
					dsk_langres_get_menu_text(lang_id[STR_MOVIE_STRECTCH], temp_str, GUI_TITLE_MAX);
					eLIBs_strcat(strtmp, temp_str);
					eLIBs_strcat(strtmp, "1 ");
				}
				else if(1 == zoom)
				{
					dsk_langres_get_menu_text(lang_id[STR_MOVIE_STRECTCH], temp_str, GUI_TITLE_MAX);
					eLIBs_strcat(strtmp, temp_str);
					eLIBs_strcat(strtmp, "2 ");
				}
				else if(2 == zoom)
				{
					dsk_langres_get_menu_text(lang_id[STR_MOVIE_ORIGINAL], temp_str, GUI_TITLE_MAX);
					eLIBs_strcat(strtmp, temp_str);
				}
				else if(3 == zoom)
				{
					eLIBs_strcat(strtmp, temp_str);
					eLIBs_strcat(strtmp, "4:3");
				}
				else if(4 == zoom)
				{
					eLIBs_strcat(strtmp, temp_str);
					eLIBs_strcat(strtmp, "16:9");
				}
				else if(5 == zoom)
				{
					dsk_langres_get_menu_text(lang_id[STR_MOVIE_AUTO], temp_str, GUI_TITLE_MAX);
					eLIBs_strcat(strtmp, temp_str);
					eLIBs_strcat(strtmp, "TV");
				}

				eLIBs_strcat(item_str, strtmp);
				GUI_DispStringInRect((const char *)item_str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			}

			movie_cmd2parent_syn(msg->h_deswin, spsc_ctrl_scene_msg_screenset, zoom, 0);
			GUI_LyrWinSel(scene_para->hlyr);
			__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_screen_mode);
			return EPDK_OK;
		}

		case movie_spsc_ctrl_icon_tvout:
		{
			movie_cmd2parent_syn(msg->h_deswin, spsc_ctrl_scene_msg_tvout, 0, 0);
			return EPDK_OK;
		}
	}

	__msg("unknown message...\n");
	return EPDK_FAIL;
}

static __s32 __movie_spsc_ctrl_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			movie_spsc_ctrl_scene_t *scene_para;
			__msg("__movie_spsc_ctrl_proc GUI_MSG_CREATE begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			scene_para->hfrm = msg->h_deswin;//±?D??ú′?3?ê??ˉ￡?òò?a′°?ú′′?¨?′·μ??￡?·??ò???μè?è?ê???
			__movie_spsc_ctrl_init_ui();
			__msg("__movie_spsc_ctrl_proc GUI_MSG_CREATE end\n");
			return EPDK_OK;
		}

		case GUI_MSG_DESTROY:
		{
			movie_spsc_ctrl_scene_t *scene_para;
			__msg("__movie_spsc_ctrl_proc GUI_MSG_DESTROY begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			__movie_spsc_ctrl_long_string_stop_roll(scene_para);
			__movie_spsc_ctrl_long_string_uninit(scene_para);
			__movie_spsc_ctrl_deinit_ui();
			__msg("__movie_spsc_ctrl_proc GUI_MSG_DESTROY end\n");
			return EPDK_OK;
		}

		case GUI_MSG_KEY:
		{
			static __s32 last_key = -1;
			movie_spsc_ctrl_scene_t *scene_para;
			movie_spsc_ctrl_uipara_t *ui_para;
			__msg("__movie_spsc_ctrl_proc GUI_MSG_KEY begin\n");
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

			if(NULL == ui_para)
			{
				__msg("movie_spsc_get_uipara fail...\n");
				return EPDK_FAIL;
			}

			if(KEY_UP_ACTION == msg->dwAddData2)
			{
				switch(last_key)
				{
					case GUI_MSG_KEY_LEFT:
					{
						__msg("draw unfocus icon..\n");
						GUI_LyrWinSel(scene_para->hlyr);
						com_memdev_create(scene_para->hlyr);

						if(ui_para->focus_icon_index < 5 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
						ui_para->focus_icon_index--;

						if(ui_para->focus_icon_index < 5)
						{
							ui_para->focus_icon_index = movie_spsc_ctrl_icon_num - 1;
						}

						__msg("draw focus icon..\n");
						movie_draw_text(ui_para->focus_icon_index);
						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
						GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
						com_memdev_delete();
						break;
					}

					case GUI_MSG_KEY_RIGHT:
					{
						__msg("draw unfocus icon..\n");
						GUI_LyrWinSel(scene_para->hlyr);
						com_memdev_create(scene_para->hlyr);

						if(ui_para->focus_icon_index < 5 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
						ui_para->focus_icon_index++;

						if(ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						__msg("draw focus icon..\n");
						movie_draw_text(ui_para->focus_icon_index);
						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
						GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
						com_memdev_delete();
						break;
					}

					case GUI_MSG_KEY_REPEATE:
						ui_para->focus_icon_index = movie_spsc_ctrl_icon_playmode;

					case GUI_MSG_KEY_ENTER:
					{
						if(ui_para->focus_icon_index < 5 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						GUI_LyrWinSel(scene_para->hlyr);
						com_memdev_create(scene_para->hlyr);
						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
						GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
						com_memdev_delete();
						__movie_spsc_ctrl_notify_parent(scene_para, msg, ui_para->focus_icon_index);
						break;
					}

					default :
						break;
				}
			}
			else
			{
				switch(msg->dwAddData1)
				{
					case GUI_MSG_KEY_LONGLEFT:
					{
						__msg("draw unfocus icon..\n");
						GUI_LyrWinSel(scene_para->hlyr);
						com_memdev_create(scene_para->hlyr);

						if(ui_para->focus_icon_index < 5 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
						ui_para->focus_icon_index--;

						if(ui_para->focus_icon_index < 5)
						{
							ui_para->focus_icon_index = movie_spsc_ctrl_icon_num - 1;
						}

						__msg("draw focus icon..\n");
						movie_draw_text(ui_para->focus_icon_index);
						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
						GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
						com_memdev_delete();
						break;
					}

					case GUI_MSG_KEY_LONGRIGHT:
					{
						__msg("draw unfocus icon..\n");
						GUI_LyrWinSel(scene_para->hlyr);
						com_memdev_create(scene_para->hlyr);

						if(ui_para->focus_icon_index < 5 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
						ui_para->focus_icon_index++;

						if(ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						__msg("draw focus icon..\n");
						movie_draw_text(ui_para->focus_icon_index);
						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
						GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
						com_memdev_delete();
						break;
					}

					case GUI_MSG_KEY_ENTER:
					case GUI_MSG_KEY_REPEATE:
					{
						if(ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
						{
							ui_para->focus_icon_index = 5;
						}

						GUI_LyrWinSel(scene_para->hlyr);
						com_memdev_create(scene_para->hlyr);
						__movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
						GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
						com_memdev_delete();
						break;
					}

					default:
					{
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

			__msg("__movie_spsc_ctrl_proc GUI_MSG_KEY end\n");
			return EPDK_OK;
		}

		case GUI_MSG_TOUCH:
		{
			__msg("__movie_spsc_ctrl_proc GUI_MSG_TOUCH begin\n");
			__msg("__movie_spsc_ctrl_proc GUI_MSG_TOUCH end\n");
			return EPDK_OK;
		}

		case GUI_MSG_TIMER:
		{
			__msg("__movie_spsc_ctrl_proc GUI_MSG_TIMER begin\n");
			__msg("__movie_spsc_ctrl_proc GUI_MSG_TIMER end\n");
			return EPDK_OK;
		}

		case GUI_MSG_CLOSE:
		{
			__msg("__movie_spsc_ctrl_proc GUI_MSG_CLOSE begin\n");
			__msg("__movie_spsc_ctrl_proc GUI_MSG_CLOSE end\n");
			return EPDK_OK;
		}

		case GUI_MSG_PAINT:
		{
			movie_spsc_ctrl_scene_t *scene_para;
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			if(GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
			{
				__msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
				return EPDK_OK;
			}

			GUI_LyrWinSel(scene_para->hlyr);
			com_memdev_create(scene_para->hlyr);
			__msg("before __movie_spsc_ctrl_bg_draw\n");
			__movie_spsc_ctrl_bg_draw(scene_para);
			__msg("after __movie_spsc_ctrl_bg_draw\n");
			__msg("before __movie_spsc_ctrl_icon_draw_all\n");
			__movie_spsc_ctrl_icon_draw_all(scene_para);
			__msg("after __movie_spsc_ctrl_icon_draw_all\n");
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			com_memdev_delete();
			{
				__gui_msg_t notify_msg;
				eLIBs_memset(&notify_msg, 0, sizeof(__gui_msg_t));
				notify_msg.h_deswin = msg->h_deswin;
				notify_msg.id = movie_spsc_ctrl_scene_draw_difuse;
				GUI_SendNotifyMessage(&notify_msg);
			}
			return EPDK_OK;
		}

		case movie_spsc_ctrl_scene_update_fileinfo:
		{
			movie_spsc_ctrl_scene_t *scene_para;
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para || !scene_para->hlyr)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			if(GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
			{
				__msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
				return EPDK_OK;
			}

			eLIBs_memset(scene_para->str_file_name, 0, sizeof(scene_para->str_file_name));
			eLIBs_memset(scene_para->str_file_info, 0, sizeof(scene_para->str_file_info));
			eLIBs_memset(scene_para->str_video_info, 0, sizeof(scene_para->str_video_info));
			eLIBs_memset(scene_para->str_audio_info, 0, sizeof(scene_para->str_audio_info));
			GUI_LyrWinSel(scene_para->hlyr);
			com_memdev_create(scene_para->hlyr);
			//__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_filename);//112357
			__here__;
			__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_file_info);
			__here__;
			__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_video_info);
			__here__;
			__movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_audio_info);
			__here__;
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			com_memdev_delete();
			__msg("before __movie_spsc_ctrl_long_string_start_roll\n");
			__movie_spsc_ctrl_long_string_start_roll(scene_para);
			__msg("after __movie_spsc_ctrl_long_string_start_roll\n");
			return EPDK_OK;
		}

		case movie_spsc_ctrl_scene_draw_difuse:
		{
			movie_spsc_ctrl_scene_t *scene_para;
			scene_para = GUI_WinGetAttr(msg->h_deswin);

			if(!scene_para || !scene_para->hlyr)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			if(GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
			{
				__msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
				return EPDK_OK;
			}

			GUI_LyrWinSel(scene_para->hlyr);
			com_memdev_create(scene_para->hlyr);
			__msg("before __movie_spsc_ctrl_bg_draw\n");
			//__movie_spsc_ctrl_bg_draw(scene_para);
			__msg("after __movie_spsc_ctrl_bg_draw\n");
			__msg("before __movie_spsc_ctrl_icon_draw_all\n");
			//__movie_spsc_ctrl_icon_draw_all(scene_para);
			__msg("after __movie_spsc_ctrl_icon_draw_all\n");
			__msg("before __movie_spsc_ctrl_static_icon_draw_all\n");
			__movie_spsc_ctrl_static_icon_draw_all(scene_para);
			__msg("after __movie_spsc_ctrl_static_icon_draw_all\n");
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			com_memdev_delete();
			__msg("before __movie_spsc_ctrl_long_string_init\n");
			__movie_spsc_ctrl_long_string_init(scene_para);
			__msg("after __movie_spsc_ctrl_long_string_init\n");
			__msg("before __movie_spsc_ctrl_long_string_start_roll\n");
			__movie_spsc_ctrl_long_string_start_roll(scene_para);
			__msg("after __movie_spsc_ctrl_long_string_start_roll\n");
			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return GUI_FrmWinDefaultProc(msg);
}

void *movie_spsc_ctrl_scene_create(movie_spsc_ctrl_create_para_t *create_para)
{
	__gui_framewincreate_para_t framewin_para;
	movie_spsc_ctrl_scene_t *sence_para;
	movie_spsc_ctrl_uipara_t *ui_para;
	sence_para = (movie_spsc_ctrl_scene_t *)esMEMS_Balloc(sizeof(movie_spsc_ctrl_scene_t));

	if(!sence_para)
	{
		__msg("mem not enough...\n");
		return NULL;
	}

	eLIBs_memset(sence_para, 0, sizeof(movie_spsc_ctrl_scene_t));
	//éè??ê?è?2?êy
	sence_para->hparent = create_para->hparent;
	sence_para->scene_id = create_para->scene_id;
	sence_para->hlyr = create_para->hlyr;

	if(NULL == sence_para->hlyr)
	{
		esMEMS_Bfree(sence_para, sizeof(movie_spsc_ctrl_scene_t));
		sence_para = NULL;
		__msg("sence_para->hlyr is null...\n");
		return NULL;
	}

	//??μ?UI×?±ê
	ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

	if(NULL == ui_para)
	{
		esMEMS_Bfree(sence_para, sizeof(movie_spsc_ctrl_scene_t));
		sence_para = NULL;
		__msg("movie_spsc_get_uipara fail...\n");
		return NULL;
	}

	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name			=	"movie_spsc_win",
	                 framewin_para.dwExStyle		= WS_EX_NONE;
	framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
	framewin_para.spCaption		= NULL;
	framewin_para.hOwner		= NULL;
	framewin_para.id			= sence_para->scene_id;
	framewin_para.hHosting		=  sence_para->hparent;
	framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__movie_spsc_ctrl_proc);
	framewin_para.rect.x		= ui_para->uipara_frm.x;
	framewin_para.rect.y		= ui_para->uipara_frm.y;
	framewin_para.rect.width	= ui_para->uipara_frm.w;
	framewin_para.rect.height	= ui_para->uipara_frm.h;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red	= 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue	= 0;
	framewin_para.attr = (void *)sence_para;
	framewin_para.hLayer = sence_para->hlyr;
	sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);

	if(NULL == sence_para->hfrm)
	{
		esMEMS_Bfree(sence_para, sizeof(movie_spsc_ctrl_scene_t));
		sence_para = NULL;
		__msg("GUI_FrmWinCreate fail...\n");
		return NULL;
	}

	return sence_para;
}

__s32 movie_spsc_ctrl_scene_set_focus(void *handle)
{
	movie_spsc_ctrl_scene_t *scene_para;
	scene_para = (movie_spsc_ctrl_scene_t *)handle;
	__here__;

	if(NULL == handle)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	__here__;

	if(!scene_para->hfrm)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	__here__;
	GUI_WinSetFocusChild(scene_para->hfrm);
	__here__;
	return EPDK_OK;
}

__s32 movie_spsc_ctrl_scene_delete(void *handle)
{
	movie_spsc_ctrl_scene_t *scene_para;
	scene_para = (movie_spsc_ctrl_scene_t *)handle;

	if(NULL == handle)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hfrm)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	GUI_FrmWinDelete(scene_para->hfrm);
	eLIBs_memset(scene_para, 0, sizeof(movie_spsc_ctrl_scene_t));
	esMEMS_Bfree(scene_para, sizeof(movie_spsc_ctrl_scene_t));
	return EPDK_OK;
}

H_WIN movie_spsc_ctrl_scene_get_hwnd(void *handle)
{
	movie_spsc_ctrl_scene_t *scene_para;
	scene_para = (movie_spsc_ctrl_scene_t *)handle;

	if(NULL == handle)
	{
		__err("invalid para..\n");
		return NULL;
	}

	return scene_para->hfrm;
}


