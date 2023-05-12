
#include "..\\functions_i.h"

#define STR__PRJ_MAIN_VER								"V1.03"


#define REG_SETTING_PARA_FILE_PATH 			"e:\\setting.bin"

#define REG_DEFAULT_BGD_PATH   BEETLES_APP_ROOT"apps\\"

#define REG_VERSION_INFO "REG1"

#define REG_VERSION_INFO_LEN (eLIBs_strlen(REG_VERSION_INFO))

typedef struct
{
	reg_system_para_t	system_para;
	reg_init_para_t	    init_para;
	reg_root_para_t	    root_para;
	reg_movie_para_t	movie_para;
	reg_music_para_t	music_para;
	reg_photo_para_t	photo_para;
	reg_fm_para_t		fm_para;
	reg_ebook_para_t	ebook_para;
	reg_record_para_t   record_para;
#if SP_APP_CALENDAR
	reg_calendar_para_t calendar_para;
#endif
#if SP_APP_ATV
	reg_atv_para_t		atv_para;
#endif
}
reg_app_para_t;

typedef struct
{
	__u32   restore_flag[REG_APP_MAX];
	reg_app_para_t para_current;//当前值
	reg_app_para_t para_default;//出厂默认值
	__u32   reserverd[4]; //保留参数
}
reg_all_para_t;

static reg_all_para_t	*all_app_para = NULL;

#define REG_HOME_ICON_POS(i, x, y) {REG_HOME_ICON_POS_X0(i, x, y),\
		REG_HOME_ICON_POS_Y0(i, x, y),\
		REG_HOME_ICON_POS_X1(i, x, y),\
		REG_HOME_ICON_POS_Y1(i, x, y)}

#define REG_HOME_ICON_POS_X0(i, x, y) (480*(i)+120*(x))
#define REG_HOME_ICON_POS_Y0(i, x, y) (24+(272-24-15)/2*(y))
#define REG_HOME_ICON_POS_X1(i, x, y) (480*(i)+120*(x)+120-1)
#define REG_HOME_ICON_POS_Y1(i, x, y) (24+(272-24-15)/2*(y)+(272-24-15)/2-1)






static void multi_screen_unit_init(pmulti_screen_t multi_screen_para)
{
	GUI_RECT desktop_iconRect[DESKTOP_UNIT_MAX] =
	{
		//固定应用

		/*    {0,        24+(272-24-15)/2,   120-1,    24+(272-24-15)/2*2-1},
		    {120,      24+(272-24-15)/2,   120*2-1,  24+(272-24-15)/2*2-1},
		    {120*2,    24+(272-24-15)/2,   120*3-1,  24+(272-24-15)/2*2-1},
		    {120*3,    24+(272-24-15)/2,   120*4-1,  24+(272-24-15)/2*2-1},

		    {480,          24,   480+120-1,      24+(272-24-15)/2-1},
		    {480+120,      24,   480+120*2-1,    24+(272-24-15)/2-1},
		    {480+120*2,    24,   480+120*3-1,    24+(272-24-15)/2-1},
		    {480+120*3,    24,   480+120*4-1,    24+(272-24-15)/2-1},

		    {480+0,        24+(272-24-15)/2,   480+120-1,    24+(272-24-15)/2*2-1},
		    {480+120,      24+(272-24-15)/2,   480+120*2-1,  24+(272-24-15)/2*2-1},

		    */

		REG_HOME_ICON_POS(0, 0, 1),
		REG_HOME_ICON_POS(0, 1, 1),
		REG_HOME_ICON_POS(0, 2, 1),
		REG_HOME_ICON_POS(0, 3, 1),

		REG_HOME_ICON_POS(1, 0, 0),
		REG_HOME_ICON_POS(1, 1, 0),
		REG_HOME_ICON_POS(1, 2, 0),
		REG_HOME_ICON_POS(1, 3, 0),

		REG_HOME_ICON_POS(1, 0, 1),
		REG_HOME_ICON_POS(1, 1, 1),

		//快捷键
		{0,   0,   99,  100},
		{0,   0,   57,  160},
		{0,   0,   64,  93 },
		{0,   0,   184, 156},

		{REG_HOME_ICON_POS_X0(0, 0, 0) + 30, REG_HOME_ICON_POS_Y0(0, 0, 0), REG_HOME_ICON_POS_X1(0, 0, 0) + 30 + 5, REG_HOME_ICON_POS_Y1(0, 0, 0)},
		{REG_HOME_ICON_POS_X0(0, 1, 0) + 30 * 2 + 5, REG_HOME_ICON_POS_Y0(0, 1, 0), REG_HOME_ICON_POS_X1(0, 1, 0) + 30 * 2 + 5, REG_HOME_ICON_POS_Y1(0, 1, 0)},
		{REG_HOME_ICON_POS_X0(0, 2, 0) + 30 * 2 + 5, REG_HOME_ICON_POS_Y0(0, 2, 0), REG_HOME_ICON_POS_X1(0, 2, 0) + 30 * 2 + 5, REG_HOME_ICON_POS_Y1(0, 2, 0)},
	};
	GUI_RECT	headbar_iconRect[HEADBAR_UNIT_MAX] =
	{
		{0,   30,  66,  96},
		{76,  30,  142, 96},
		{152, 30,  218, 96},
		{228, 30,  294, 96},

		{304, 30,  370, 96},
		{380, 30,  446, 96},
		{456, 30,  522, 96},
	};
	__s16 i = 0;
	multi_screen_para->desktop_screen_index = 0;
	multi_screen_para->desktop_unit_num     = DESKTOP_UNIT_MAX;
	multi_screen_para->headbar_unit_num     = HEADBAR_UNIT_MAX;

	for(i = 0; i < DESKTOP_UNIT_MAX; i++)
	{
		multi_screen_para->desktop_unit[i].rect.x0 = desktop_iconRect[i].x0;
		multi_screen_para->desktop_unit[i].rect.y0 = desktop_iconRect[i].y0;
		multi_screen_para->desktop_unit[i].rect.x1 = desktop_iconRect[i].x1;
		multi_screen_para->desktop_unit[i].rect.y1 = desktop_iconRect[i].y1;

		if(i < DESKTOP_UNIT_AP_MAX
		    || i >= DESKTOP_UNIT_AP_MAX + HEADBAR_INITIAL_MAX)
		{
			multi_screen_para->desktop_unit[i].isExist = EPDK_TRUE;
		}
		else
		{
			multi_screen_para->desktop_unit[i].isExist = EPDK_FALSE;
		}
	}

	for(i = 0; i < HEADBAR_UNIT_MAX; i++)
	{
		multi_screen_para->headbar_unit[i].rect.x0 = headbar_iconRect[i].x0;
		multi_screen_para->headbar_unit[i].rect.y0 = headbar_iconRect[i].y0;
		multi_screen_para->headbar_unit[i].rect.x1 = headbar_iconRect[i].x1;
		multi_screen_para->headbar_unit[i].rect.y1 = headbar_iconRect[i].y1;

		if(i < HEADBAR_INITIAL_MAX)
		{
			multi_screen_para->headbar_unit[i].isExist = EPDK_TRUE;
		}
		else
		{
			multi_screen_para->headbar_unit[i].isExist = EPDK_FALSE;
		}
	}
}

#define MASK_EnRestoreParam_MediaType						(1<<0)
#define MASK_EnRestoreParam_VideoScreenRatio			(1<<1)
#define MASK_EnRestoreParam_Volume							(1<<2)
#define MASK_EnRestoreParam_Language						(1<<3)
#define MASK_EnRestoreParam_Style								(1<<4)
#define MASK_EnRestoreParam_Keytone							(1<<5)
#define MASK_EnRestoreParam_TvoutSys						(1<<6)
#define MASK_EnRestoreParam_LightOffTime					(1<<7)
#define MASK_EnRestoreParam_PWM								(1<<8)
#define MASK_EnRestoreParam_PWM1							(1<<9)
#define MASK_EnRestoreParam_Bright							(1<<10)
#define MASK_EnRestoreParam_Contrast						(1<<11)
#define MASK_EnRestoreParam_Hue								(1<<12)
#define MASK_EnRestoreParam_Saturability					(1<<13)
#define MASK_EnRestoreParam_TvSys							(1<<14)
#define MASK_EnRestoreParam_AvinSys							(1<<15)



#define MASK_EnRestoreParam_FromFunParam					(1<<31)

void dsk_reg_default_cur_para_from_cfg(reg_app_para_t *para, __u32 a_enRestoreParam_mask)
{
	__s32 	ret;
	int 		val;
	H_PARSER parser = NULL;

#if MASK_SysFlags_ShiftNum
	para->system_para.m_maskFlags |= MASK_SysFlags_ShiftNum;
#endif
	para->system_para.m_maskFlags &= ~MASK_SysFlags_DebugMode;
	
	parser = esCFG_Init_Ex("d:\\app_config.bin");

	if(NULL == parser)
	{
		__wrn("esCFG_Init_Ex fail...\n");
		return;
	}

	ret = esCFG_GetKeyValue_Ex(parser, "system/logoDisable", (int *)&val, 1);

	if(EPDK_OK == ret)
	{
		if(val < 0 || val > 1)
		{
			val = 0;
		}

		para->system_para.logoDisable = val;
		__wrn("default zoom_mode=%d\n", val);
	}
	else
	{
		para->system_para.logoDisable = 0;
	}

	if(!(a_enRestoreParam_mask & MASK_EnRestoreParam_FromFunParam))
	{
		ret = esCFG_GetKeyValue_Ex(parser, "system/enRestoreParam", (int *)&a_enRestoreParam_mask, 1);

		if(EPDK_OK != ret)
		{
			return;
		}
	}

	a_enRestoreParam_mask &= ~MASK_EnRestoreParam_FromFunParam;

	if((EPDK_OK == ret) && a_enRestoreParam_mask)
	{
		__wrn("a_enRestoreParam_mask=0x%X\n", a_enRestoreParam_mask);

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_MediaType)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/mediaType", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < m_eMediaTypeList_video || val > m_eMediaTypeList_max)
				{
					val = m_eMediaTypeList_video;
				}

				para->root_para.last_play_app = val;
				__wrn("\n\ndefault mediaType=%d\n", val);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_VideoScreenRatio)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/videoScreenRatio", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < 0 || val > ROBIN_ZOOM_16R9)
				{
					val = ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO;
				}

				para->movie_para.zoom_mode = val;
				__wrn("default zoom_mode=%d\n", val);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Volume)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/volume", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < AUDIO_DEVICE_VOLUME_MIN || val > AUDIO_DEVICE_VOLUME_MAX)
				{
					val = 26;
				}

				para->system_para.volume = val;
				__wrn("default volume=%d\n", para->system_para.volume);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Language)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/lang", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(
					EPDK_LANGUAGE_ENM_ENGLISH != val
				    && EPDK_LANGUAGE_ENM_CHINESES != val
				    && EPDK_LANGUAGE_ENM_CHINESET != val
				    )
				{
					val = EPDK_LANGUAGE_ENM_CHINESES;
				}

				para->system_para.language = val;
				__wrn("default lang=0x%x\n", para->system_para.language);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Style)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/style", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < 0)
				{
					val = 0;
				}

				para->init_para.default_bgd_index = val;
				__wrn("default style=0x%d\n", para->system_para.style);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Keytone)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/keytone", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < 0 || val > 1)
				{
					val = 0;
				}

				para->system_para.keytone = val;
				__wrn("default keytone=%d\n", para->system_para.keytone);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_TvoutSys)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/cvbs", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < 0 || val > 1)
				{
					val = 0;
				}

				para->system_para.pal_ntsc = val;
				__wrn("default cvbs=%d\n", para->system_para.pal_ntsc);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_LightOffTime)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/light_off", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(0 != val
				    && 10 != val
				    && 30 != val
				    && 60 != val
				    && 90 != val)
				{
					val = 60;
				}

				para->system_para.backlightoff = val;
				__wrn("default backlightoff=%d\n", para->system_para.backlightoff);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_PWM)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/pwm", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < LION_BRIGHT_LEVEL1 || val >= LION_BRIGHT_TOTAL)
				{
					val = LION_BRIGHT_LEVEL12;
				}

				para->system_para.pwm = val;
				__wrn("default pwm=%d\n", para->system_para.pwm);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_PWM1)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/pwm1", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < LION_BRIGHT_LEVEL1 || val >= LION_BRIGHT_TOTAL)
				{
					val = LION_BRIGHT_LEVEL12;
				}

				para->system_para.pwm1 = val;
				__wrn("default pwm1=%d\n", para->system_para.pwm1);
			}
		}
		
		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Bright)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/bright", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < CONST_DispBright_Min || val > CONST_DispBright_Max)
				{
					val = CONST_DispBright_Default;
				}

				para->system_para.bright = val;
				__wrn("default lcd bright=%d\n", para->system_para.bright);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Contrast)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/contrast", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < CONST_DispContrast_Min || val > CONST_DispContrast_Max)
				{
					val = CONST_DispContrast_Default;
				}

				para->system_para.contrast = val;
				__wrn("default lcd contrast=%d\n", para->system_para.contrast);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Hue)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/hue", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < CONST_DispHue_Min || val > CONST_DispHue_Max)
				{
					val = CONST_DispHue_Default;
				}

				para->system_para.hue = val;
				__wrn("default lcd hue=%d\n", para->system_para.hue);
			}
		}

		if(a_enRestoreParam_mask & MASK_EnRestoreParam_Saturability)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/saturability", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < CONST_DispSaturability_Min || val > CONST_DispSaturability_Max)
				{
					val = CONST_DispSaturability_Default;
				}

				para->system_para.saturability = val;
				__wrn("default lcd saturability=%d\n", para->system_para.saturability);
			}
		}

#if SP_APP_ATV
		if(a_enRestoreParam_mask & MASK_EnRestoreParam_TvSys)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/tv_system", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < m_eTvSystem_PAL_DK || val >= m_eTvSystem_max)
				{
					val = m_eTvSystem_PAL_I;
				}
			
				para->atv_para.m_eTvSystem_tv = val;
				{
					__u16 i;
					for(i = 0; i < SIZE_sTunerAtvList; i++)
					{
						para->atv_para.m_sTunerAtvList[i].m_eTvSystem = para->atv_para.m_eTvSystem_tv;
					}
				}
				__msg("default tv_system=%d\n", val);
			}
		}
		
		if(a_enRestoreParam_mask & MASK_EnRestoreParam_AvinSys)
		{
			ret = esCFG_GetKeyValue_Ex(parser, "system/avin_system", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < m_eTvSystem_PAL_DK || val >= m_eTvSystem_max)
				{
					val = m_eTvSystem_PAL_I;
				}
			
				para->atv_para.m_eTvSystem_avin = val;
				__msg("default avin_system=%d\n", val);
			}
		}
#endif
	}

	if(parser)
	{
		esCFG_Exit_Ex(parser);
		parser = NULL;
	}
}

//读取配置文件默认配置
static __s32 __reg_paramter_config_init(reg_app_para_t *para)
{
	if(!para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	dsk_reg_default_cur_para_from_cfg(para, 
		MASK_EnRestoreParam_FromFunParam
		| MASK_EnRestoreParam_MediaType
		| MASK_EnRestoreParam_VideoScreenRatio
		| MASK_EnRestoreParam_Volume
		| MASK_EnRestoreParam_Language
		| MASK_EnRestoreParam_Style
		| MASK_EnRestoreParam_Keytone
		| MASK_EnRestoreParam_TvoutSys
		| MASK_EnRestoreParam_LightOffTime
		| MASK_EnRestoreParam_PWM
		| MASK_EnRestoreParam_PWM1
		| MASK_EnRestoreParam_Bright
		| MASK_EnRestoreParam_Contrast
		| MASK_EnRestoreParam_Hue
		| MASK_EnRestoreParam_Saturability
		| MASK_EnRestoreParam_TvSys
		| MASK_EnRestoreParam_AvinSys
		);
	{
		//init key val by app_config.fex
		char *path = "d:\\app_config.bin";
		H_PARSER parser;
		__s32 ret;
		__s32 i;
		char keyname[64];
		int val;
		char str_val[32];

		do
		{
			parser = NULL;
			parser = esCFG_Init_Ex(path);

			if(NULL == parser)
			{
				__wrn("esCFG_Init_Ex fail...\n");
				break;
			}

			ret = esCFG_GetKeyValue_Ex(parser, "system/realVolume_max", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(val < AUDIO_DEVICE_VOLUME_MIN || val > AUDIO_DEVICE_VOLUME_MAX)
				{
					val = 28;
				}

				para->system_para.realVolume_max = val;
				__wrn("realVolume_max=%d\n", para->system_para.realVolume_max);
			}

			eLIBs_memset(str_val, 0, sizeof(str_val));
			ret = esCFG_GetKeyValue_Ex(parser, "prog_ver/ver", (int *)str_val, sizeof(str_val) / sizeof(int));

			if(EPDK_OK == ret)
			{
				strcat(para->system_para.ver_inf, str_val);
			}

		#if 0
			ret = esCFG_GetKeyValue_Ex(parser, "system/power_off", (int *)&val, 1);

			if(EPDK_OK == ret)
			{
				if(0 != val
				    && 3 * 60 != val
				    && 5 * 60 != val
				    && 10 * 60 != val
				    && 15 * 60 != val)
				{
					val = 3 * 60;
				}

				para->system_para.poweroff = val;
				__wrn("default poweroff=%d\n", para->system_para.poweroff);
			}

			{
				__s32 i;
				__s32 size;
				__s32 ret;
				__s32 scn_no;
				__s32 x_no;
				__s32 y_no;
				size = sizeof(g_home_icon_name2enm) / sizeof(g_home_icon_name2enm[0]);

				for(i = 0 ; i < size ; i++)
				{
					eLIBs_strcpy(keyname, g_home_icon_name2enm[i].name);
					eLIBs_strcat(keyname, "/");
					eLIBs_strcat(keyname, "scn_no");
					ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)&scn_no, 1);

					if(EPDK_OK != ret)
					{
						continue;
					}

					if(scn_no < 0 || scn_no > 2)
					{
						scn_no = 0;
					}

					eLIBs_strcpy(keyname, g_home_icon_name2enm[i].name);
					eLIBs_strcat(keyname, "/");
					eLIBs_strcat(keyname, "x_no");
					ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)&x_no, 1);

					if(EPDK_OK != ret)
					{
						continue;
					}

					if(x_no < 0 || x_no > 3)
					{
						x_no = 0;
					}

					eLIBs_strcpy(keyname, g_home_icon_name2enm[i].name);
					eLIBs_strcat(keyname, "/");
					eLIBs_strcat(keyname, "y_no");
					ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)&y_no, 1);

					if(EPDK_OK != ret)
					{
						continue;
					}

					if(y_no < 0 || y_no > 1)
					{
						y_no = 0;
					}

					{
						GUI_RECT gui_rect;
						gui_rect.x0 = REG_HOME_ICON_POS_X0(scn_no, x_no, y_no);
						gui_rect.y0 = REG_HOME_ICON_POS_Y0(scn_no, x_no, y_no);
						gui_rect.x1 = REG_HOME_ICON_POS_X1(scn_no, x_no, y_no);
						gui_rect.y1 = REG_HOME_ICON_POS_Y1(scn_no, x_no, y_no);
						para->system_para.multi_screen_para.desktop_unit[g_home_icon_name2enm[i].type].rect
						  = gui_rect;
					}
				}
			}
			#endif
		}
		while(0);

		if(parser)
		{
			esCFG_Exit_Ex(parser);
			parser = NULL;
		}
	}
	return EPDK_OK;
}

static __s32 __reg_paramter_init(reg_app_para_t *para)
{
	__u16 i = 0;

	if(!para)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	multi_screen_unit_init(&para->system_para.multi_screen_para);

#if SP_APP_ALARM
	for(i = 0; i < REG_ALARM_NUM; i++)
	{
		eLIBs_strcpy(para->system_para.alarm[i].alarm_music_path, REG_ALARM_MUSIC_DEFAULT_PATH);
		para->system_para.alarm[i].alarm_runtime	= REG_ALARM_50SEC;
		para->system_para.alarm[i].alarm_volume		= 20;
		para->system_para.alarm[i].alarm_time.hour 	= 12;
		para->system_para.alarm[i].alarm_time.minute = 0;
		para->system_para.alarm[i].alarm_time.second = 0;
	}
#endif

	strcpy(para->system_para.ver_inf, STR__PRJ_MAIN_VER);		// 主版本
	
	para->system_para.audio_output = 0;//normal
	para->system_para.channel = 0; //立体声
	para->system_para.charset = 5; //gbk
	para->system_para.detect_gate = 1; //on
	para->system_para.saturation = 63;
	para->system_para.logoDisable = 0;
	para->system_para.realVolume_max = AUDIO_DEVICE_VOLUME_MAX;
	para->system_para.volume = AUDIO_DEVICE_VOLUME_MAX;
	para->system_para.language = EPDK_LANGUAGE_ENM_CHINESES; //简体中文
	para->system_para.style = 0;
	para->system_para.pwm = 10;
	para->system_para.pwm1 = 12;
	para->system_para.bright = CONST_DispBright_Default;
	para->system_para.contrast = dsk_get_display_global(DISP_CMD_GET_CONTRAST);

	if(para->system_para.contrast < CONST_DispContrast_Min)
	{
		para->system_para.contrast = CONST_DispContrast_Default;
	}

	para->system_para.hue = dsk_get_display_global(DISP_CMD_GET_HUE);

	if(para->system_para.hue < CONST_DispHue_Min)
	{
		para->system_para.hue = CONST_DispHue_Default;
	}

	para->system_para.saturability = dsk_get_display_global(DISP_CMD_GET_SATURATION);

	if(para->system_para.saturability < CONST_DispSaturability_Min)
	{
		para->system_para.saturability = CONST_DispSaturability_Default;
	}

	para->system_para.backlightoff = 30; //不自动关屏
	para->system_para.poweroff = 0; //不自动关机
	para->system_para.keytone = 0; //按键音是否打开
	para->system_para.gamma_r_value = 0x0a;
	para->system_para.gamma_g_value = 0x0a;
	para->system_para.gamma_b_value = 0x0a;
	para->system_para.output = LION_DISP_LCD;
	para->system_para.pal_ntsc = 1; //0代表ntsc, 1代表pal
	para->system_para.time_set = 1;
	para->system_para.date.year = 2012;
	para->system_para.date.month = 5;
	para->system_para.date.day = 1;
	para->system_para.time.hour = 12;
	para->system_para.time.minute = 0;
	para->system_para.time.second = 0;
	para->system_para.tp_adjust_finish_flag = 0;
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
	para->system_para.m_echoLevel = 2;
#endif
	para->system_para.m_ePowerOffType = m_ePowerOffType_normal;
	para->system_para.m_maskFlags = 0;
#if MASK_SysFlags_ShiftNum
	para->system_para.m_maskFlags = MASK_SysFlags_ShiftNum;
#endif
	para->init_para.default_bgd_index = 0;
	eLIBs_strcpy(para->init_para.default_bgd_path, REG_DEFAULT_BGD_PATH);
	eLIBs_strcpy(para->init_para.setting_bgd_path, "");
	para->init_para.bgd_flag = 1; //默认背景
	para->init_para.bgd_flag_in_main = 1;//默认背景
	para->root_para.cur_play_storage = -1;           //最后一次运行的ap类型，-1:空 0:音乐,1:视频
	para->root_para.last_play_app = m_eMediaTypeList_video;          //最后一次运行的ap类型，0:音乐,1:视频
	eLIBs_memset(para->root_para.last_app_play_storage, 0, sizeof(para->root_para.last_app_play_storage));
	eLIBs_memset(para->root_para.last_storage_play_app, 0, sizeof(para->root_para.last_storage_play_app));
	para->root_para.last_storage_play_app[0] = m_eMediaTypeList_video;	// for:app_root_prob_intent_plugin_for_play
	para->root_para.last_storage_play_app[1] = m_eMediaTypeList_video;
	//   para->root_para.last_app_name;
	para->root_para.last_movie_index_sd = 0;
	//  para->root_para.last_movie_path_sd;
	para->root_para.last_movie_index_ud = 0;
	//  para->root_para.last_movie_path_ud;
	para->root_para.last_music_index_sd = 0;
	//  para->root_para.last_music_path_sd;
	para->root_para.last_music_index_ud = 0;
	//  para->root_para.last_music_path_ud;
	para->root_para.last_photo_index_sd = 0;
	//  para->root_para.last_photo_path_sd;
	para->root_para.last_photo_index_ud = 0;
	//  para->root_para.last_photo_path_ud;
	para->root_para.last_ebook_index_sd = 0;
	//  para->root_para.last_ebook_path_sd;
	para->root_para.last_ebook_index_ud = 0;
	//  para->root_para.last_ebook_path_ud =0;
	para->movie_para.rotate_mode = 2; //ROTATE_ALL
	para->movie_para.zoom_mode = ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO; //VIDEO_RATIO
	para->movie_para.sub_state = 1; //on
	para->movie_para.sub_color = GUI_WHITE;
	para->movie_para.sub_stream = 0;
	para->movie_para.sub_pos = 0; //下面
	para->music_para.start_item_id = 0;
	para->music_para.focus_item_id = 0;
	para->music_para.play_mode = 2; //ROTATE_ALL
	para->music_para.EQ_mode = 0; //NORMAL
	para->photo_para.bgmusic = 0; //关闭
	para->photo_para.speedflag = 0;
	para->photo_para.interval_time = 6;
	para->photo_para.ratio = 0;
	para->photo_para.effect = 0; //默认无切换效果
	para->ebook_para.text_colour = GUI_WHITE;
	para->ebook_para.switch_time = 10;
	para->ebook_para.switch_time_flag = 0;
	para->ebook_para.bg_music_switch = 0;
	para->fm_para.mode = 0;
	para->fm_para.channel_id = 0;
	//para->ebook_para
#if SP_APP_CALENDAR
	para->calendar_para.time_format = 0;
#endif

#if SP_APP_ATV
	para->atv_para.m_psTunerAtvList_idx = IDX_sTunerAtvList_invalid;	// 频道索引: 无效
	para->atv_para.m_eTvSystem_tv = m_eTvSystem_PAL_I;
	para->atv_para.m_eTvSystem_avin = m_eTvSystem_PAL_I;
	{
		__u16 i;
		for(i = 0; i < SIZE_sTunerAtvList; i++)
		{
			para->atv_para.m_sTunerAtvList[i].m_eTvSystem = para->atv_para.m_eTvSystem_tv;
		}
	}
	
	if(para->atv_para.m_psTunerAtvList_idx <= IDX_sTunerAtvList_invalid)
	{ // 添加一个默认频道
		para->atv_para.m_psTunerAtvList_idx = 0;
		para->atv_para.m_psTunerAtvList_cnt = 1;
		para->atv_para.m_sTunerAtvList[0].m_freq = RANGE_TVFreq_Min;
		para->atv_para.m_sTunerAtvList[0].m_eTvSystem = para->atv_para.m_eTvSystem_tv; // 使用系统制式
	}

#if SP_SaveTVShortcutIdx

	for(i = 0; i < SIZE_sTunerAtvList_shortcutIdx; i++)
	{
		para->atv_para.m_psTunerAtvList_shortcutIdx_tab[i] = IDX_sTunerAtvList_invalid;
	}

#endif
#endif
	{
		__s32 ret;
		ret = __reg_paramter_config_init(para);

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}
	}
	return EPDK_OK;
}

__s32 dsk_reg_init_para(void)
{
	//分配内存
	{
		if(all_app_para)
		{
			__msg("para already init...\n");
			return EPDK_OK;
		}

		all_app_para = esMEMS_Balloc(sizeof(reg_all_para_t));

		if(!all_app_para)
		{
			__msg("mem not enough...\n");
			return EPDK_FAIL;
		}
	}
	//尝试加载文件
	{
		ES_FILE *fp;
		fp = eLIBs_fopen(REG_SETTING_PARA_FILE_PATH, "rb");

		if(fp == NULL)
		{
			eLIBs_format("e:\\", "fat", 0);
			fp = eLIBs_fopen(REG_SETTING_PARA_FILE_PATH, "rb");
		}

		if(fp)
		{
			__s32 file_len;
			__s32 read_num;
			__msg("file exist:%s\n", REG_SETTING_PARA_FILE_PATH);
			eLIBs_fseek(fp, 0, SEEK_END);
			file_len = eLIBs_ftell(fp);

			if(file_len == sizeof(reg_all_para_t) + REG_VERSION_INFO_LEN)
			{
				char version[5] = {0};
				eLIBs_fseek(fp, 0, SEEK_SET);
				__here__;
				read_num = eLIBs_fread(version, 1, REG_VERSION_INFO_LEN, fp);

				if(REG_VERSION_INFO_LEN == read_num)
				{
					__here__;

					if(0 == eLIBs_strncmp(version, REG_VERSION_INFO, REG_VERSION_INFO_LEN))
					{
						__here__;
						read_num = eLIBs_fread(all_app_para, 1, sizeof(reg_all_para_t), fp);

						if(sizeof(reg_all_para_t) == read_num)
						{
							__here__;
							eLIBs_fclose(fp);
							dsk_reg_default_cur_para_from_cfg(&all_app_para->para_current, 0);
							__msg("load reg para success...\n");
							return EPDK_OK;
						}
					}
					else
					{
						__msg("reg version info incorrect...\n");
					}
				}
				else
				{
					__msg("read file fail:%s\n", REG_SETTING_PARA_FILE_PATH);
				}
			}

			eLIBs_fclose(fp);
		}
		else
		{
			__msg("file not exist:%s\n", REG_SETTING_PARA_FILE_PATH);
		}
	}
	__here__;
	//文件不存在或大小不对，则重新写入
	//初始化参数
	{
		__s32 sta;
		__s32 ret;
		eLIBs_memset(all_app_para, 0, sizeof(reg_all_para_t));
		ret = EPDK_OK;
		sta = __reg_paramter_init(&all_app_para->para_current);

		if(EPDK_FAIL == sta)
		{
			__msg("__reg_paramter_init fail...\n");
			ret = EPDK_FAIL;
		}

		sta = __reg_paramter_init(&all_app_para->para_default);

		if(EPDK_FAIL == sta)
		{
			__msg("__reg_paramter_init fail...\n");
			ret = EPDK_FAIL;
		}

		if(EPDK_FAIL == ret)
		{
			return EPDK_FAIL;
		}
	}

	//写入文件
	if(1)
	{
		//
		ES_FILE *fp;
		__s32 write_num;
		fp = eLIBs_fopen(REG_SETTING_PARA_FILE_PATH, "wb");

		if(!fp)
		{
			__msg("open file fail:%s\n", REG_SETTING_PARA_FILE_PATH);
			return EPDK_FAIL;
		}

		write_num = eLIBs_fwrite(REG_VERSION_INFO, 1, REG_VERSION_INFO_LEN, fp);

		if(write_num != REG_VERSION_INFO_LEN)
		{
			eLIBs_fclose(fp);
			__msg("eLIBs_fwrite fail:%s\n", REG_SETTING_PARA_FILE_PATH);
			return EPDK_FAIL;
		}

		write_num = eLIBs_fwrite(all_app_para, 1, sizeof(reg_all_para_t), fp);

		if(write_num != sizeof(reg_all_para_t))
		{
			eLIBs_fclose(fp);
			__msg("eLIBs_fwrite fail:%s\n", REG_SETTING_PARA_FILE_PATH);
			return EPDK_FAIL;
		}
		else
		{
			__msg("write para success...\n");
			eLIBs_fclose(fp);
			return EPDK_OK;
		}
	}

	return EPDK_OK;
}

__s32 dsk_reg_deinit_para(void)
{
	if(all_app_para)
	{
		esMEMS_Bfree(all_app_para, sizeof(reg_all_para_t));
		all_app_para = NULL;
	}

	return EPDK_OK;
}

__s32 dsk_reg_flush(void)
{
	__s32 result;
	ES_FILE *fp = NULL;

	if(1)//
	{
		if(!all_app_para)
		{
			__err("reg para not init...\n");
			return EPDK_FALSE;
		}

		fp = eLIBs_fopen(REG_SETTING_PARA_FILE_PATH, "wb");

		if(fp == NULL)
		{
			eLIBs_format("e:\\", "fat", 0);
			fp = eLIBs_fopen(REG_SETTING_PARA_FILE_PATH, "wb");

			if(fp == NULL)
			{
				__msg("2open file fail:%s\n", REG_SETTING_PARA_FILE_PATH);
				return EPDK_FAIL;
			}
		}

		result = eLIBs_fwrite(REG_VERSION_INFO, 1, REG_VERSION_INFO_LEN, fp);

		if(result != REG_VERSION_INFO_LEN)
		{
			eLIBs_fclose(fp);
			__msg("eLIBs_fwrite fail:%s\n", REG_SETTING_PARA_FILE_PATH);
			return EPDK_FAIL;
		}

		result = eLIBs_fwrite(all_app_para, 1, sizeof(reg_all_para_t), fp);

		if(result < sizeof(reg_all_para_t))
		{
			eLIBs_fclose(fp);
			__msg("write file fail:%s\n", REG_SETTING_PARA_FILE_PATH);
			return EPDK_FAIL;
		}

		eLIBs_fclose(fp);
		{
			//
			ES_FILE *hFile;
			//flush user disk data to NOR flash
			hFile = eLIBs_fopen("b:\\STORAGE\\SPINOR", "rb");

			if(hFile == NULL)
			{
				__wrn("open NOR file failed\n");
				return EPDK_FAIL;
			}

			eLIBs_printf("flush file begin ...\n");
			eLIBs_fioctrl(hFile, DEV_IOC_USR_FLUSH_CACHE, 0, 0);
			eLIBs_printf("flush file end .\n");
			eLIBs_fclose(hFile);
		}
	}

	return EPDK_OK;
}

void *dsk_reg_get_para_by_app(reg_app_e eApp)
{
	if(!all_app_para)
	{
		__wrn("reg para not init...\n");
		return EPDK_FALSE;
	}

	switch(eApp)
	{
		case REG_APP_SYSTEM:
			return &all_app_para->para_current.system_para;

		case REG_APP_INIT:
			return &all_app_para->para_current.init_para;

		case REG_APP_ROOT:
			return &all_app_para->para_current.root_para;

		case REG_APP_MOVIE:
			return &all_app_para->para_current.movie_para;

		case REG_APP_MUSIC:
			return &all_app_para->para_current.music_para;

		case REG_APP_PHOTO:
			return &all_app_para->para_current.photo_para;

		case REG_APP_EBOOK:
			return &all_app_para->para_current.ebook_para;

		case REG_APP_FM:
			return &all_app_para->para_current.fm_para;

		case REG_APP_RECORD:
			return &all_app_para->para_current.record_para;
			
	#if SP_APP_CALENDAR
		case REG_APP_CALENDAR:
			return &all_app_para->para_current.calendar_para;
	#endif
	
	#if SP_APP_ATV
		case REG_APP_ATV:
			return &all_app_para->para_current.atv_para;
	#endif
	}

	__err("invalid para...\n");
	return NULL;
}

void *dsk_reg_get_default_para_by_app(reg_app_e eApp)
{
	if(!all_app_para)
	{
		__err("reg para not init...\n");
		return EPDK_FALSE;
	}

	switch(eApp)
	{
		case REG_APP_SYSTEM:
			return &all_app_para->para_default.system_para;

		case REG_APP_INIT:
			return &all_app_para->para_default.init_para;

		case REG_APP_ROOT:
			return &all_app_para->para_default.root_para;

		case REG_APP_MOVIE:
			return &all_app_para->para_default.movie_para;

		case REG_APP_MUSIC:
			return &all_app_para->para_default.music_para;

		case REG_APP_PHOTO:
			return &all_app_para->para_default.photo_para;

		case REG_APP_EBOOK:
			return &all_app_para->para_default.ebook_para;

		case REG_APP_FM:
			return &all_app_para->para_default.fm_para;

		case REG_APP_RECORD:
			return &all_app_para->para_default.record_para;

	#if SP_APP_CALENDAR
		case REG_APP_CALENDAR:
			return &all_app_para->para_default.calendar_para;
	#endif
	
	#if SP_APP_ATV
		case REG_APP_ATV:
			return &all_app_para->para_default.atv_para;
	#endif
	}

	__err("invalid para...\n");
	return NULL;
}

__bool dsk_reg_get_app_restore_flag(reg_app_e eApp)
{
	if(!all_app_para)
	{
		__err("reg para not init...\n");
		return EPDK_FALSE;
	}

	if(eApp >= REG_APP_MAX)
	{
		__err("invalid para...\n");
		return NULL;
	}

	return (__bool)(all_app_para->restore_flag[eApp]);
}

__s32 dsk_reg_set_app_restore_flag(reg_app_e eApp, __bool flag)
{
	if(!all_app_para)
	{
		__err("reg para not init...\n");
		return EPDK_FALSE;
	}

	if(eApp >= REG_APP_MAX)
	{
		__err("invalid para...\n");
		return NULL;
	}

	all_app_para->restore_flag[eApp] = flag;
	return EPDK_OK;
}
//保存当前播放文件消息
__s32 dsk_reg_save_cur_play_info(reg_app_e eApp, __s32 index, char *filename, rat_root_t root_type)
{
	if(!all_app_para)
	{
		__err("reg para not init...\n");
		return EPDK_FAIL;
	}
	__wrn(" save cur play index = %d...",index);
	switch(eApp)
	{
		case REG_APP_MOVIE:
		{
			if(RAT_TF == root_type)
			{
				all_app_para->para_current.root_para.last_movie_index_sd = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_movie_path_sd, filename);
			}
			else if(RAT_USB == root_type)
			{
				all_app_para->para_current.root_para.last_movie_index_ud = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_movie_path_ud, filename);
			}

			break;
		}

		case REG_APP_MUSIC:
		{
			if(RAT_TF == root_type)
			{
				all_app_para->para_current.root_para.last_music_index_sd = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_music_path_sd, filename);
			}
			else if(RAT_USB == root_type)
			{
				all_app_para->para_current.root_para.last_music_index_ud = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_music_path_ud, filename);
			}

			break;
		}

		case REG_APP_PHOTO:
		{
			if(RAT_TF == root_type)
			{
				all_app_para->para_current.root_para.last_photo_index_sd = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_photo_path_sd, filename);
			}
			else if(RAT_USB == root_type)
			{
				all_app_para->para_current.root_para.last_photo_index_ud = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_photo_path_ud, filename);
			}

			break;
		}

		case REG_APP_EBOOK:
		{
			if(RAT_TF == root_type)
			{
				all_app_para->para_current.root_para.last_ebook_index_sd = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_ebook_path_sd, filename);
			}
			else if(RAT_USB == root_type)
			{
				all_app_para->para_current.root_para.last_ebook_index_ud = index;
				eLIBs_strcpy(all_app_para->para_current.root_para.last_ebook_path_ud, filename);
			}

			break;
		}

		default:
			__err("invalid app type...\n");
			return EPDK_FAIL;
	}

	return EPDK_OK;
}


__s32 dsk_reg_set_app_restore_all(void)
{
	if(!all_app_para)
	{
		__err("reg para not init...\n");
		return EPDK_FAIL;
	}

	eLIBs_memcpy(&all_app_para->para_current, (const void *)(&all_app_para->para_default), sizeof(reg_app_para_t));
	return EPDK_OK;
}

