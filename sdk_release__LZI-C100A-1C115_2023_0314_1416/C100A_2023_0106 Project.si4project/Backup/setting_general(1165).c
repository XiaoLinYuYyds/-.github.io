/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: setting_general.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "Setting_general.h"
#include "setting_para.h"

#define W_BMP_SettingOptionItem 				480
#define H_BMP_SettingOptionItem 				24

#define W_BMP_SettingOptionLeft 				6
#define H_BMP_SettingOptionLeft 				9
#define W_BMP_SettingOptionRight				6
#define H_BMP_SettingOptionRight				9

#define H_SettingOptionItemNull 					4
#define W_SettingOptionTitleNull					4

#define W_SettingOptionContent					(16*10)

#define MAX_ExploreKeyCharCnt							16
#define TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR		300
#if TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR
#define ID_TIMER_CLR_EXPLORE_KEYCHAR				0x1000
#define SN_SettingOptionDisplayVcom				"2231513"	// "vcom"
#define SN_SettingOptionDisplayHue				"8215"	// "hue"
#define SN_SettingOptionDisplaySaturability		"1912021"	// "satu"
#define SN_SettingOptionDisplayPower				"1615235"	// "powe"
#define SN_SettingOptionDisplayDebugMode				"8888" // "S008DP" (DP:debug param)//密码

#endif
#define TIMEOUT_ID_TIMER_GET_POWER					100
#if TIMEOUT_ID_TIMER_GET_POWER
#define ID_TIMER_GET_POWER							0x1001
#endif

#define STEP_DispParam							5

typedef enum
{
	SETTING_MENU_LANGUAGE = 0,
	SETTING_MENU_INFO,
	SETTING_MENU_FACTORY,
	SETTING_MENU_AVOUT_FORMAT,
	SETTING_MENU_VOLUMESET,
	SETTING_MENU_PWM_EX,
	SETTING_MENU_BL_TIME,
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
	SETTING_MENU_ECHO_SET,
#endif
	SETTING_MENU_DEBUG_MODE,
	SETTING_MENU_PWM,
	SETTING_MENU_BRIGHT,
	SETTING_MENU_CONTRAST,
	SETTING_MENU_HUE,
	SETTING_MENU_SATURABILITY,

	SETTING_MENU_MAX,
	SETTING_MENU_STYLE,
	SETTING_MENU_KEYTONE,
} settingmenu_t;

typedef enum
{
	BACKLIGHTOFF_NEVER,
	BACKLIGHTOFF_10SECOND,
	BACKLIGHTOFF_30SECOND,
	BACKLIGHTOFF_60SECOND,
	BACKLIGHTOFF_90SECOND,
	BACKLIGHTOFF_MAX,
} backlightoff_t;

typedef struct tag_setting_item_id
{
	__s32 id_string_title;
	__s32 id_string_detail_prompt_ex;
	__s32 *id_string_content, id_string_content_sizeof;
	__s32  id_bmp_left;
	__s32  id_bmp_right;
	__s32  id_bmp_focus_item;
}
setting_item_id_t;

typedef struct tag_setting_item_res
{
	char string_title[128];
	OFFSET string_title_pos;

	char string_content[128];
	RECT  string_content_rect;

	H_THEME  h_bmp_focus;
	OFFSET   bmp_focus_pos;

	H_THEME  h_bmp_unfocus;
	OFFSET   bmp_unfocus_pos;

	H_THEME  h_bmp_left;
	OFFSET   bmp_left_pos;

	H_THEME  h_bmp_right;
	OFFSET   bmp_right_pos;

	__s32 content_num;
	__s32 content_nr;
}
setting_item_res_t;

typedef enum
{
  m_eSettingGeneralPaintType_all = 0,
  m_eSettingGeneralPaintType_curFocus,
  m_eSettingGeneralPaintType_newFocus_menuItemIdx,
  m_eSettingGeneralPaintType_newFocus_dispItemIdx,
} E_SETTING_GENERAL_PAINT_TYPE;

typedef struct tag_setting_general_attr
{
	//	char string_prompt[128];
	//	char string_detail_prompt[128];
	char string_detail_prompt_ex[128];
	H_THEME 	h_bmp_big_setting;

	__s32	focus_txt_color;
	__s32	unfocus_txt_color;

	H_WIN	layer;
	GUI_FONT *font;

	__s32 new_focus, newDispFocus;
	__s32 old_focus;

	__s32 item_nr;

	setting_item_res_t		m_settingItemRes[SETTING_MENU_MAX];
	__u32				m_maskSettingItemDisable;
	char					m_exploreKeyChar[MAX_ExploreKeyCharCnt + 1];	// 探测数字键
	__u8				m_exploreKeyChar_cnt;
	__u32				m_exploreKeyNum;

	setting_general_para_t *general_para;
	set_general_para *general_uipara;

	reg_system_para_t *pRegParamBak;
}
setting_general_attr_t;
// for m_maskSettingItemDisable :
#define MASK_SettingItemDisable_Icon				(1<<SETTING_MENU_MAX)
#define MASK_SettingItemDisable_Power			(1<<(SETTING_MENU_MAX+1))


extern void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

static reg_system_para_t *setting_reg_para;

typedef enum
{
	//决定语种的数量、语种切换的顺序
	m_eShiftLanguageIdx_begin = 0,
	m_eShiftLanguageIdx_English,
	m_eShiftLanguageIdx_ChineseS,
	m_eShiftLanguageIdx_Max,
} E_SHIFT_LANGUAGE_IDX;

static __s32 content_style_id[] = //ITEM2 (设置主题风格)
{
	STRING_SET_COMMON_TOPIC_STYLE1_CON,
	STRING_SET_COMMON_TOPIC_STYLE2_CON,
	STRING_SET_COMMON_TOPIC_STYLE3_CON
};

static __s32 content_output_id[] = //输出类型
{
	STRING_SET_OUTPUT_NTSC_CON,
	STRING_SET_OUTPUT_PAL_CON
};

static __s32 content_keytone_id[] = //按键音
{
	STRING_SET_KEYTONE_ON_CON,
	STRING_SET_KEYTONE_OFF_CON
};

static __s32 content_backlight_off_id[] = //ITEM2 (关闭背光)
{
	STRING_SET_POWER_BGT_NEVER_CON,
	STRING_SET_POWER_BGT_LAST10S_CON,
	STRING_SET_POWER_BGT_LAST30S_CON,
	STRING_SET_POWER_BGT_LAST60S_CON,
	STRING_SET_POWER_BGT_LAST90S_CON
};

static setting_item_id_t setting_general_res_id[] =
{
	{
		STRING_SET_COMMON_LANGUAGE_TITLE,
		STRING_SET_COMMON_LANGUAGE_DE,
		NULL, m_eShiftLanguageIdx_Max,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SET_COMMON_TOPIC_STYLE_TITLE,
		STRING_SET_COMMON_TOPIC_STYLE_DE,
		content_style_id, sizeof(content_style_id) / sizeof(content_style_id[0]),
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SET_COMMON_PRODUCTINFO_TITLE,
		STRING_SET_COMMON_PRODUCTINFO_DE,
		NULL, NULL,
		NULL,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SET_COMMON_FACTORY_DEFAULT_TITLE,
		STRING_SET_COMMON_FACTORY_DEFAULT_DE,
		NULL, NULL,
		NULL,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},

	{
		STRING_SET_OUTPUT_TITLE,
		STRING_SET_OUTPUT_DE,
		content_output_id, sizeof(content_output_id) / sizeof(content_output_id[0]),
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},

	{
		STRING_SET_KEYTONE_TITLE,
		STRING_SET_KEYTONE_DE,
		content_keytone_id, sizeof(content_keytone_id) / sizeof(content_keytone_id[0]),
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SET_VOLUME_TITLE,
		STRING_SET_VOLUME_DE,
		NULL, 31,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SETTING_PWM1,
		STRING_SETTING_PWM1_NOTE,
		NULL, 15,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SET_POWER_BGT_OFF_TITLE,
		STRING_SET_POWER_BGT_OFF_DE,
		content_backlight_off_id, sizeof(content_backlight_off_id) / sizeof(content_backlight_off_id[0]),
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
	{
		STRING_SETTING_REVERB_QUALITY,
		STRING_SETTING_REVERB_QUALITY_NOTE,
		NULL, 8,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
#endif
	{
		STRING_SETTING_LCD_BRIGHT,
		STRING_SETTING_LCD_BRIGHT_NOTE,
		NULL, CONST_DispBright_Max,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SETTING_LCD_CONTRAST,
		STRING_SETTING_LCD_CONTRAST_NOTE,
		NULL, CONST_DispContrast_Max,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SETTING_PWM,
		STRING_SETTING_PWM_NOTE,
		NULL, 15,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SETTING_LCD_HUE,
		STRING_SETTING_LCD_HUE_NOTE,
		NULL, CONST_DispHue_Max,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SETTING_LCD_SATURABILITY,
		STRING_SETTING_LCD_SATURABILITY_NOTE,
		NULL, CONST_DispSaturability_Max,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
	{
		STRING_SETTING_DEBUG_MODE,
		STRING_SETTING_DEBUG_MODE_NOTE,
		NULL, 2,
		ID_SETTING_SET_BUTT_LF_BMP,
		ID_SETTING_SET_BUTT_RF_BMP,
		ID_SETTING_SET_ITEM_F_BMP,
	},
};

typedef enum
{
	m_eSettingResId_language = 0,
	m_eSettingResId_style,
	m_eSettingResId_productInfo,
	m_eSettingResId_factoryDefault,
	m_eSettingResId_tvoutSys,
	m_eSettingResId_keytone,
	m_eSettingResId_volume,
	m_eSettingResId_pwm_ex,
	m_eSettingResId_backlightOffTime,
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
	m_eSettingResId_echo,
#endif
	m_eSettingResId_bright,
	m_eSettingResId_contrast,
	m_eSettingResId_pwm,
	m_eSettingResId_hue,
	m_eSettingResId_saturability,
	m_eSettingResId_debugMode,
	m_eSettingResId_total,
} E_SETTING_RES_ID;

__u32 g_settingResIdMapping[SETTING_MENU_MAX];

void _setting_general_get_res_id_mapping(__u32 *pMapping)
{
	__u16 i;

	for(i = 0; i < SETTING_MENU_MAX; i++)
	{
		switch(i)
		{
			case SETTING_MENU_LANGUAGE :
				pMapping[i] = m_eSettingResId_language;
				break;

			case SETTING_MENU_STYLE :
				pMapping[i] = m_eSettingResId_style;
				break;

			case SETTING_MENU_INFO :
				pMapping[i] = m_eSettingResId_productInfo;
				break;

			case SETTING_MENU_FACTORY :
				pMapping[i] = m_eSettingResId_factoryDefault;
				break;

			case SETTING_MENU_AVOUT_FORMAT :
				pMapping[i] = m_eSettingResId_tvoutSys;
				break;

			case SETTING_MENU_KEYTONE :
				pMapping[i] = m_eSettingResId_keytone;
				break;

			case SETTING_MENU_VOLUMESET :
				pMapping[i] = m_eSettingResId_volume;
				break;

			case SETTING_MENU_PWM_EX:
				pMapping[i] = m_eSettingResId_pwm_ex;
				break;
				
			case SETTING_MENU_BL_TIME :
				pMapping[i] = m_eSettingResId_backlightOffTime;
				break;

		#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
			case SETTING_MENU_ECHO_SET :
				pMapping[i] = m_eSettingResId_echo;
				break;
		#endif

			case SETTING_MENU_BRIGHT :
				pMapping[i] = m_eSettingResId_bright;
				break;

			case SETTING_MENU_CONTRAST :
				pMapping[i] = m_eSettingResId_contrast;
				break;

			case SETTING_MENU_PWM :
				pMapping[i] = m_eSettingResId_pwm;
				break;

			case SETTING_MENU_HUE :
				pMapping[i] = m_eSettingResId_hue;
				break;

			case SETTING_MENU_SATURABILITY :
				pMapping[i] = m_eSettingResId_saturability;
				break;

			case SETTING_MENU_DEBUG_MODE:
				pMapping[i] = m_eSettingResId_debugMode;
				break;

			default :
				break;
		}
	}
}

static void _setting_general_res_get_const(setting_general_attr_t *general_attr)
{
	__u32 	i;
	setting_item_res_t  *p_item_res;
	general_attr->m_maskSettingItemDisable |= MASK_SettingItemDisable_Power
	    |(1<<SETTING_MENU_PWM)
	    ;

	if(!(setting_reg_para->m_maskFlags & MASK_SysFlags_DebugMode))
	{
		general_attr->m_maskSettingItemDisable |= 0
			|(1<<SETTING_MENU_DEBUG_MODE)
			|(1<<SETTING_MENU_PWM)
			|(1<<SETTING_MENU_BRIGHT)
			|(1<<SETTING_MENU_CONTRAST)
			|(1<<SETTING_MENU_HUE)
			|(1<<SETTING_MENU_SATURABILITY)
			;
	}
	else
	{
		general_attr->m_maskSettingItemDisable &= ~(1<<SETTING_MENU_PWM);
	}

	general_attr->item_nr = SETTING_MENU_MAX;

	if(general_attr->h_bmp_big_setting == NULL)
	{
		general_attr->h_bmp_big_setting = theme_open(ID_SETTING_SET_BSYSSET_BMP);
	}

	for(i = 0; i < SETTING_MENU_MAX; i++)
	{
		if(general_attr->m_maskSettingItemDisable & (1 << i))
		{
			general_attr->item_nr--;
		}

		p_item_res = &general_attr->m_settingItemRes[i];
		p_item_res->content_nr = setting_general_res_id[g_settingResIdMapping[i]].id_string_content_sizeof;

		//		p_item_res->string_content_rect = general_attr->general_uipara->item[i].item_string_content_rect;
		//		p_item_res->string_title_pos = general_attr->general_uipara->item[i].item_string_title_pos;

		//		p_item_res->bmp_left_pos = general_attr->general_uipara->item[i].item_bmp_left_pos;//初始化各图片的放置位置
		//		p_item_res->bmp_right_pos = general_attr->general_uipara->item[i].item_bmp_right_pos;
		//		p_item_res->bmp_focus_pos = general_attr->general_uipara->item[i].item_bmp_focus_pos;

		if(p_item_res->h_bmp_left == NULL)
		{
			p_item_res->h_bmp_left = theme_open(setting_general_res_id[g_settingResIdMapping[i]].id_bmp_left);
		}

		if(p_item_res->h_bmp_right == NULL)
		{
			p_item_res->h_bmp_right = theme_open(setting_general_res_id[g_settingResIdMapping[i]].id_bmp_right);
		}

		if(p_item_res->h_bmp_focus == NULL)
		{
			p_item_res->h_bmp_focus = theme_open(setting_general_res_id[g_settingResIdMapping[general_attr->new_focus]].id_bmp_focus_item);
		}
	}
}

static void _setting_general_res_uninit(setting_general_attr_t *general_attr)
{
	__u32 	i;
	setting_item_res_t  *p_item_res;
	theme_close(general_attr->h_bmp_big_setting);

	for(i = 0; i < SETTING_MENU_MAX; i++)
	{
		p_item_res = &general_attr->m_settingItemRes[i];
		theme_close(p_item_res->h_bmp_left);
		theme_close(p_item_res->h_bmp_right);
		theme_close(p_item_res->h_bmp_focus);
	}
}

static void _setting_general_res_get_base(setting_general_attr_t *general_attr, __s32 idx)
{
	__u32				*content_id = NULL;
	setting_item_res_t		*p_item_res;
	p_item_res = &general_attr->m_settingItemRes[idx];
	content_id = setting_general_res_id[g_settingResIdMapping[idx]].id_string_content;

	if(content_id)
	{
		get_menu_text(content_id[p_item_res->content_num], p_item_res->string_content, 128);
	}
	else
	{
		memset(p_item_res->string_content, 0x00, sizeof(p_item_res->string_content) / sizeof(p_item_res->string_content[0]));

		if(p_item_res->content_nr)
		{
			eLIBs_sprintf(p_item_res->string_content, "%ld", p_item_res->content_num);
		}
	}
}

static void _setting_general_res_reset(setting_general_attr_t *general_attr)
{
	__u32 	i;
	setting_item_res_t  *p_item_res;
	get_menu_text(setting_general_res_id[g_settingResIdMapping[general_attr->new_focus]].id_string_detail_prompt_ex,
	              general_attr->string_detail_prompt_ex, 128);

	for(i = 0; i < SETTING_MENU_MAX; i++)
	{
		p_item_res = &general_attr->m_settingItemRes[i];
		get_menu_text(setting_general_res_id[g_settingResIdMapping[i]].id_string_title, p_item_res->string_title, 128);

		switch(i)
		{
			case SETTING_MENU_LANGUAGE:
				if(setting_reg_para)
				{
					switch(setting_reg_para->language)
					{
						case EPDK_LANGUAGE_ENM_CHINESES :
							p_item_res->content_num = m_eShiftLanguageIdx_ChineseS;
							break;

						case EPDK_LANGUAGE_ENM_ENGLISH :
							p_item_res->content_num = m_eShiftLanguageIdx_English;
							break;

						default :
							continue;
							break;
					}
				}
				else
				{
					continue;
				}

				get_menu_text(STRING_SETTING_LANGUAGE_CONTENT, p_item_res->string_content, 128);
				continue;
				break;

			case SETTING_MENU_STYLE:
				if(setting_reg_para)
				{
					p_item_res->content_num = setting_reg_para->style;

					if(p_item_res->content_num > 2)
					{
						p_item_res->content_num = 0;
					}
				}
				else
				{
					p_item_res->content_num = 0;
				}

				break;

			case SETTING_MENU_AVOUT_FORMAT:
				if(setting_reg_para)
				{
					p_item_res->content_num = setting_reg_para->pal_ntsc; //0 ntsc , 1 pal

					if(p_item_res->content_num > 1)
					{
						p_item_res->content_num = 0;
					}
				}
				else
				{
					p_item_res->content_num = 0;
				}

				break;

			case SETTING_MENU_KEYTONE:
				if(setting_reg_para)
				{
					p_item_res->content_num = setting_reg_para->keytone;

					if(setting_reg_para->keytone == 1)	//keytone on
					{
						p_item_res->content_num = 0;
					}
					else if(setting_reg_para->keytone == 0)	//keytone off
					{
						p_item_res->content_num = 1;
					}
					else
					{
						p_item_res->content_num = 0;
					}
				}
				else
				{
					p_item_res->content_num = 0;
				}

				break;

			case SETTING_MENU_VOLUMESET:
				if(setting_reg_para)
				{
					if((setting_reg_para->volume > 30) || (setting_reg_para->volume < 0))
					{
						p_item_res->content_num = 30;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->volume;
					}
				}
				else
				{
					p_item_res->content_num = 30;
				}

				break;

			case SETTING_MENU_BRIGHT:
				if(setting_reg_para)
				{
					if((setting_reg_para->bright > CONST_DispBright_Max)
					    || (setting_reg_para->bright < CONST_DispBright_Min))
					{
						p_item_res->content_num = CONST_DispBright_Default;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->bright;
					}
				}
				else
				{
					p_item_res->content_num = CONST_DispBright_Default;
				}

				break;

			case SETTING_MENU_CONTRAST:
				if(setting_reg_para)
				{
					if((setting_reg_para->contrast > CONST_DispContrast_Max)
					    || (setting_reg_para->contrast < CONST_DispContrast_Min))
					{
						p_item_res->content_num = CONST_DispContrast_Default;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->contrast;
					}
				}
				else
				{
					p_item_res->content_num = CONST_DispContrast_Default;
				}

				break;

			case SETTING_MENU_PWM:
				if(setting_reg_para)
				{
					if((setting_reg_para->pwm > 15) || (setting_reg_para->pwm < 0))
					{
						p_item_res->content_num = 10;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->pwm + 1;
					}
				}
				else
				{
					p_item_res->content_num = 10;
				}

				break;

			case SETTING_MENU_PWM_EX:
				if(setting_reg_para)
				{
					if((setting_reg_para->pwm1 > 15) || (setting_reg_para->pwm1 < 0))
					{
						p_item_res->content_num = 10;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->pwm1 + 1;
					}
				}
				else
				{
					p_item_res->content_num = 10;
				}

				break;

			case SETTING_MENU_BL_TIME://设置关闭背光时间
			{
				if(setting_reg_para)
				{
					switch(setting_reg_para->backlightoff)
					{
						case 0:
							p_item_res->content_num = 0;
							break;

						case 10:
							p_item_res->content_num = 1;
							break;

						case 30:
							p_item_res->content_num = 2;
							break;

						case 60:
							p_item_res->content_num = 3;
							break;

						case 90:
							p_item_res->content_num = 4;
							break;

						default:
							p_item_res->content_num = 0;
							break;
					}
				}
				else
				{
					p_item_res->content_num = 0;
				}
			}
			break;

		#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
			case SETTING_MENU_ECHO_SET:
				if(setting_reg_para)
				{
					if((setting_reg_para->m_echoLevel >= 8) || (setting_reg_para->m_echoLevel < 0))
					{
						p_item_res->content_num = 3;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->m_echoLevel + 1;
					}
				}
				else
				{
					p_item_res->content_num = 3;
				}

				break;
		#endif

			case SETTING_MENU_HUE :
				if(setting_reg_para)
				{
					if((setting_reg_para->hue > CONST_DispHue_Max)
					    || (setting_reg_para->hue < CONST_DispHue_Min))
					{
						p_item_res->content_num = CONST_DispHue_Default;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->hue;
					}
				}
				else
				{
					p_item_res->content_num = CONST_DispHue_Default;
				}

				break;

			case SETTING_MENU_SATURABILITY :
				if(setting_reg_para)
				{
					if((setting_reg_para->saturability > CONST_DispSaturability_Max)
					    || (setting_reg_para->saturability < CONST_DispSaturability_Min))
					{
						p_item_res->content_num = CONST_DispSaturability_Default;
					}
					else
					{
						p_item_res->content_num = setting_reg_para->saturability;
					}
				}
				else
				{
					p_item_res->content_num = CONST_DispSaturability_Default;
				}

				break;

			case SETTING_MENU_DEBUG_MODE:
				p_item_res->content_num = 0;
				if(setting_reg_para)
				{
					if(setting_reg_para->m_maskFlags & MASK_SysFlags_DebugMode)
					{
						p_item_res->content_num = 1;
					}
				}
				break;

			default:
				break;
		}

		_setting_general_res_get_base(general_attr, i);
	}
}

static void _setting_general_res_get_focus_detail_prompt(setting_general_attr_t *general_attr)
{
	setting_item_res_t  *p_item_res;
	p_item_res = &general_attr->m_settingItemRes[general_attr->new_focus];
	get_menu_text(setting_general_res_id[g_settingResIdMapping[general_attr->new_focus]].id_string_detail_prompt_ex,
	              general_attr->string_detail_prompt_ex, 128);
}

static void _setting_general_res_get_focus_content(setting_general_attr_t *general_attr)
{
	_setting_general_res_get_base(general_attr, general_attr->new_focus);
}

static __s32 setting_general_paint(__gui_msg_t *msg, E_SETTING_GENERAL_PAINT_TYPE a_eSettingGeneralPaintType)
{
	setting_general_attr_t *general_attr;
	setting_item_res_t  *p_item_res;
	__s32 menuItemIdx, dispItemcnt;
	GUI_RECT gui_rect;
	RECT rect;
	OFFSET	xyOft;
	__wrn("setting_general_paint(%d):begin\n", a_eSettingGeneralPaintType);
	general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(general_attr == NULL)
	{
		return EPDK_FAIL;
	}

	if(GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	__wrn("setting_general_paint(%d):valid\n", a_eSettingGeneralPaintType);

	switch(a_eSettingGeneralPaintType)
	{
		case m_eSettingGeneralPaintType_all :
			_setting_general_res_reset(general_attr);
			break;

		case m_eSettingGeneralPaintType_newFocus_dispItemIdx :
			for(menuItemIdx = 0, dispItemcnt = 0; menuItemIdx < SETTING_MENU_MAX; menuItemIdx++)
			{
				// 将显示索引转换为菜单选项索引
				if(general_attr->m_maskSettingItemDisable & (1 << menuItemIdx))
				{
					continue;
				}

				if(++dispItemcnt == general_attr->newDispFocus)
				{
					if(general_attr->new_focus == menuItemIdx)
					{
						return EPDK_OK;
					}

					general_attr->old_focus = general_attr->new_focus;
					general_attr->new_focus = menuItemIdx;
					a_eSettingGeneralPaintType = m_eSettingGeneralPaintType_newFocus_menuItemIdx;
					break;
				}
			}

			if(dispItemcnt > general_attr->item_nr)
			{
				return EPDK_FAIL;
			}

		case m_eSettingGeneralPaintType_newFocus_menuItemIdx :
			_setting_general_res_get_focus_detail_prompt(general_attr);
			break;

		case m_eSettingGeneralPaintType_curFocus :
			_setting_general_res_get_focus_content(general_attr);
			break;

		default :
			return EPDK_FAIL;
	}

	GUI_LyrWinSel(general_attr->layer);
	GUI_SetFont(general_attr->font);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_SetBkColor(0x00);
	GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
	GUI_LyrWinCacheOn();
	GUI_OpenAlphaBlend();

	switch(a_eSettingGeneralPaintType)
	{
		case m_eSettingGeneralPaintType_all :
			if(!(general_attr->m_maskSettingItemDisable & MASK_SettingItemDisable_Icon))
			{
				general_attr->m_maskSettingItemDisable |= MASK_SettingItemDisable_Icon;
				GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_big_setting), general_attr->general_uipara->bmp_big_setting_pos.x, general_attr->general_uipara->bmp_big_setting_pos.y);
			}

		case m_eSettingGeneralPaintType_newFocus_menuItemIdx :
			GUI_SetColor(general_attr->focus_txt_color);
			gui_rect.x0 = general_attr->general_uipara->string_detail_prompt_rect_ex.x;
			gui_rect.y0 = general_attr->general_uipara->string_detail_prompt_rect_ex.y;
			gui_rect.x1 = gui_rect.x0 + general_attr->general_uipara->string_detail_prompt_rect_ex.width;
			gui_rect.y1 = gui_rect.y0 + general_attr->general_uipara->string_detail_prompt_rect_ex.height;
			GUI_ClearRectEx(&gui_rect);
			GUI_DispStringInRect(general_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
			break;

		case m_eSettingGeneralPaintType_curFocus :
			break;

		default :
			return EPDK_FAIL;
	}

	xyOft.x = (rect.width - W_BMP_SettingOptionItem) / 2;
	xyOft.y = (rect.height - H_BMP_SettingOptionItem * SETTING_MENU_MAX - H_SettingOptionItemNull * (SETTING_MENU_MAX - 1)) / 2;
	gui_rect.y0 = xyOft.y;

	for(menuItemIdx = 0, dispItemcnt = 0; menuItemIdx < SETTING_MENU_MAX; menuItemIdx++)
	{
		if(general_attr->m_maskSettingItemDisable & (1 << menuItemIdx))
		{
			continue;
		}

		dispItemcnt++;
		gui_rect.x0 = xyOft.x;
		gui_rect.x1 = gui_rect.x0 + W_BMP_SettingOptionItem - 1;
		gui_rect.y1 = gui_rect.y0 + H_BMP_SettingOptionItem - 1;
		p_item_res = &general_attr->m_settingItemRes[menuItemIdx];

		if(menuItemIdx == general_attr->new_focus)
		{
			OFFSET xyOft_focus;
			GUI_RECT t_gui_rect = gui_rect;
			GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
			t_gui_rect.x1 = gui_rect.x0 + W_BMP_SettingOptionItem - 1 - W_SettingOptionTitleNull;
			t_gui_rect.x0 = t_gui_rect.x1 - W_SettingOptionContent;
			GUI_ClearRectEx(&gui_rect);
			xyOft_focus.x = gui_rect.x0;
			xyOft_focus.y = gui_rect.y0;
			GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_focus), xyOft_focus.x, xyOft_focus.y);
			xyOft_focus.x += W_BMP_SettingOptionItem - W_SettingOptionTitleNull;
			xyOft_focus.y += (H_BMP_SettingOptionItem - H_BMP_SettingOptionRight) / 2;
			GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_right), xyOft_focus.x - W_BMP_SettingOptionRight, xyOft_focus.y);
			GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_left), xyOft_focus.x - W_SettingOptionContent, xyOft_focus.y);
			GUI_SetColor(general_attr->focus_txt_color);
		}
		else
		{
			switch(a_eSettingGeneralPaintType)
			{
				case m_eSettingGeneralPaintType_newFocus_menuItemIdx :
					if(menuItemIdx != general_attr->old_focus)
					{
						break;
					}

				case m_eSettingGeneralPaintType_all :
					GUI_ClearRectEx(&gui_rect);
					GUI_SetColor(general_attr->unfocus_txt_color);
					break;

				case m_eSettingGeneralPaintType_curFocus :
					break;

				default :
					return EPDK_FAIL;
			}
		}

		switch(a_eSettingGeneralPaintType)
		{
			case m_eSettingGeneralPaintType_curFocus :
				if(menuItemIdx != general_attr->new_focus)
				{
					break;
				}

			case m_eSettingGeneralPaintType_newFocus_menuItemIdx :
				if((menuItemIdx != general_attr->new_focus) && (menuItemIdx != general_attr->old_focus))
				{
					break;
				}

			case m_eSettingGeneralPaintType_all :
				gui_rect.x1 = gui_rect.x0 + W_BMP_SettingOptionItem - 1 - W_SettingOptionContent - W_SettingOptionTitleNull;
				gui_rect.x0 += W_SettingOptionTitleNull;
				{
					char str[128] = "";
					sprintf(str, "%d, ", dispItemcnt);
					strcat(str, p_item_res->string_title);
					GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
				}
				gui_rect.x0 = gui_rect.x1 + 1 +W_BMP_SettingOptionLeft +W_SettingOptionTitleNull;
				gui_rect.x1 = gui_rect.x0 + W_SettingOptionContent - 1 - W_SettingOptionTitleNull*3 -W_BMP_SettingOptionRight;
				GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
				break;

			default :
				return EPDK_FAIL;
		}

		gui_rect.y0 += H_BMP_SettingOptionItem + H_SettingOptionItemNull;
	}

	if(a_eSettingGeneralPaintType == m_eSettingGeneralPaintType_all)
	{
		gui_rect.x0 = xyOft.x;
		gui_rect.x1 = gui_rect.x0 + W_BMP_SettingOptionItem - 1;
		for(; dispItemcnt < SETTING_MENU_MAX; dispItemcnt++)
		{
			gui_rect.y1 = gui_rect.y0 + H_BMP_SettingOptionItem - 1;
			GUI_ClearRectEx(&gui_rect);
			gui_rect.y0 += H_BMP_SettingOptionItem + H_SettingOptionItemNull;
		}
	}
	
	GUI_CloseAlphaBlend();
	GUI_LyrWinCacheOff();
	__wrn("setting_general_paint():end\n");
	return EPDK_OK;
}

#if TIMEOUT_ID_TIMER_GET_POWER
void _setting_general_paint_power_base(setting_general_attr_t *general_attr)
{
	GUI_RECT	gui_rect;
	RECT	rect;
	__u8	powerLevel = 0x80;
	__s32	voltage;
	char		str[] = "Vbat:2-3700";
	voltage = dsk_power_get_battery_level(&powerLevel);
	str[5] = (powerLevel & 0x0F) + '0';
	memset(&str[7], 0x00, 4);
	sprintf(&str[7], "%ld", voltage);
	GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
	gui_rect.x1 = rect.width - 1;
	gui_rect.x0 = gui_rect.x1 - 12 * (strlen(str));
	gui_rect.y1 = rect.height - 1;
	gui_rect.y0 = gui_rect.y1 - 24;
	GUI_ClearRectEx(&gui_rect);
	GUI_DispStringInRect(str, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
	__wrn("%s\n", str);
#if 0
	{
		ES_FILE *pKeyFile;
		pKeyFile = eLIBs_fopen("b:\\INPUT\\IR_KEY", "rb");

		if(pKeyFile)
		{
			__s32 ret = eLIBs_fioctrl(pKeyFile, DRV_IRKEY_CMD_GET_IR_KEY_VALUE, 0, 0);
			memset(str, 0x00, sizeof(str) / sizeof(str[0]));
			sprintf(str, "IR:0x%04X", ret);
			gui_rect.y1 = gui_rect.y0 - 1;
			gui_rect.y0 = gui_rect.y1 - 24;
			GUI_ClearRectEx(&gui_rect);
			GUI_DispStringInRect(str, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
			__wrn("%s\n", str);
		}
	}
#endif
}

__s32 _setting_general_paint_power(__gui_msg_t *msg)
{
	setting_general_attr_t *general_attr;
	general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(general_attr == NULL)
	{
		return EPDK_FAIL;
	}

	if(GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSel(general_attr->layer);
	GUI_SetFont(general_attr->font);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_SetBkColor(0x00);
	GUI_SetColor(general_attr->unfocus_txt_color);
	GUI_LyrWinCacheOn();
	GUI_OpenAlphaBlend();
	_setting_general_paint_power_base(general_attr);
	GUI_CloseAlphaBlend();
	GUI_LyrWinCacheOff();
	return EPDK_OK;
}
#endif

static void setting_restore_default(__gui_msg_t *msg)
{
	__u32	pwm = setting_reg_para->pwm;
	
	dsk_reg_set_app_restore_all();

	if(setting_reg_para)
	{
		dsk_langres_set_type(setting_reg_para->language);			//语言恢复出厂设置
		//gscene_bgd_set_default_bg_index(setting_reg_para->style);	//恢复默认背景图
		//gscene_bgd_refresh();
		dsk_keytone_set_state((__set_keytone_t)(setting_reg_para->keytone));//desktop模块里面1 表示on,2 表示off
		dsk_keytone_on();
		dsk_volume_set(setting_reg_para->volume);	//音量恢复出厂设置
		{
			// 重置屏参
			setting_general_attr_t *general_attr;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

			if(general_attr)
			{
				if(general_attr->m_maskSettingItemDisable & (1 << SETTING_MENU_PWM))
				{
					setting_reg_para->pwm = pwm;	// PWM不变
				}
			}
		
			dsk_display_set_lcd_pwm((__lion_bright_t)(setting_reg_para->pwm));
			dsk_display_set_lcd_pwm_ex((__lion_bright_t)(setting_reg_para->pwm1));
			dsk_set_display_global(setting_reg_para->bright, DISP_CMD_SET_BRIGHT);
			dsk_set_display_global(setting_reg_para->contrast, DISP_CMD_SET_CONTRAST);
			dsk_set_display_global(setting_reg_para->hue, DISP_CMD_SET_HUE);
			dsk_set_display_global(setting_reg_para->saturability, DISP_CMD_SET_SATURATION);	

			if(!(general_attr->m_maskSettingItemDisable & (1<<SETTING_MENU_DEBUG_MODE)))
			{
				__u32 t_maskSettingItemDisable = general_attr->m_maskSettingItemDisable;
				general_attr->m_maskSettingItemDisable |= (0
					|(1<<SETTING_MENU_DEBUG_MODE)
					|(1<<SETTING_MENU_BRIGHT)
					|(1<<SETTING_MENU_CONTRAST)
					|(1<<SETTING_MENU_HUE)
					|(1<<SETTING_MENU_SATURABILITY)
					|(1<<SETTING_MENU_PWM)
					);
				t_maskSettingItemDisable ^= general_attr->m_maskSettingItemDisable;
				while(t_maskSettingItemDisable)
				{
					if(t_maskSettingItemDisable & 1)	general_attr->item_nr--;
					t_maskSettingItemDisable >>= 1;
				}
			}			
		}
		g_set_close_scn_time(setting_reg_para->backlightoff);	//自动关屏时间
		dsk_set_auto_off_time(setting_reg_para->poweroff);		//自动关机时间

		if(SP_APP_CALENDAR)
		{
			__date_t factory_date;
			__time_t factory_time;
			factory_date = setting_reg_para->date;
			factory_time = setting_reg_para->time;
			esTIME_SetDate(&factory_date);
			esTIME_SetTime(&factory_time);
		}
	}

	setting_general_paint(msg, m_eSettingGeneralPaintType_all);
}

__s32 setting_general_key_proc_num(__gui_msg_t *msg)
{
	setting_general_attr_t *general_attr;
	general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(general_attr == NULL)
	{
		return EPDK_FAIL;
	}

#if TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR

	if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR))
	{
		GUI_SetTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR, TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR, NULL);
		memset(general_attr->m_exploreKeyChar, 0x00,
		       sizeof(general_attr->m_exploreKeyChar) / sizeof(general_attr->m_exploreKeyChar[0]));
		general_attr->m_exploreKeyChar[0] = msg->dwAddData1 - GUI_MSG_KEY_NUM0 + '0';
		general_attr->m_exploreKeyNum = msg->dwAddData1 - GUI_MSG_KEY_NUM0;
		general_attr->m_exploreKeyChar_cnt = 1;
	}
	else
	{
		if(general_attr->m_exploreKeyNum < (0xFFFFFFFF / 10 - 1))
		{
			general_attr->m_exploreKeyNum *= 10;
			general_attr->m_exploreKeyNum += msg->dwAddData1 - GUI_MSG_KEY_NUM0;
		}

		if(general_attr->m_exploreKeyChar_cnt < MAX_ExploreKeyCharCnt)
		{
			GUI_ResetTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR, TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR, NULL);
			general_attr->m_exploreKeyChar[general_attr->m_exploreKeyChar_cnt] = msg->dwAddData1 - GUI_MSG_KEY_NUM0 + '0';
			general_attr->m_exploreKeyChar_cnt++;
		}
		else
		{
			return EPDK_FAIL;;	// 无效键
		}
	}

	__msg_jjl("m_exploreKeyValue[0,%d)=\"%s\", m_exploreKeyNum=%ld\n", general_attr->m_exploreKeyChar_cnt, general_attr->m_exploreKeyChar, general_attr->m_exploreKeyNum);
	{
		__u32 t_maskSettingItemDisable = general_attr->m_maskSettingItemDisable;
#ifdef SN_SettingOptionDisplayDebugMode
		if((SETTING_MENU_DEBUG_MODE < SETTING_MENU_MAX)
						&& (strcmp(general_attr->m_exploreKeyChar, SN_SettingOptionDisplayDebugMode) == 0))
		{
			if(general_attr->m_maskSettingItemDisable & (1 << SETTING_MENU_DEBUG_MODE))
			{
				__u32 t1_maskSettingItemDisable = general_attr->m_maskSettingItemDisable;
				general_attr->m_maskSettingItemDisable &= ~(0
					|(1<<SETTING_MENU_DEBUG_MODE)
					|(1<<SETTING_MENU_BRIGHT)
					|(1<<SETTING_MENU_CONTRAST)
					|(1<<SETTING_MENU_HUE)
					|(1<<SETTING_MENU_SATURABILITY)
					|(1<<SETTING_MENU_PWM)
					);
				general_attr->old_focus = general_attr->new_focus;
				general_attr->new_focus = SETTING_MENU_DEBUG_MODE;
				t1_maskSettingItemDisable ^= general_attr->m_maskSettingItemDisable;
				while(t1_maskSettingItemDisable)
				{
					if(t1_maskSettingItemDisable & 1)	general_attr->item_nr++;
					t1_maskSettingItemDisable >>= 1;
				}
				setting_reg_para->m_maskFlags |= MASK_SysFlags_DebugMode;
			}
		}
#else
		if((SETTING_MENU_PWM < SETTING_MENU_MAX)
		    && (strcmp(general_attr->m_exploreKeyChar, SN_SettingOptionDisplayVcom) == 0))
		{
			// 此项作为焦点项显示出来
			if(general_attr->m_maskSettingItemDisable & (1 << SETTING_MENU_PWM))
			{
				general_attr->m_maskSettingItemDisable &= ~(1 << SETTING_MENU_PWM);
				general_attr->old_focus = general_attr->new_focus;
				general_attr->new_focus = SETTING_MENU_PWM;
			}
		}
		else if((SETTING_MENU_HUE < SETTING_MENU_MAX)
		        && (strcmp(general_attr->m_exploreKeyChar, SN_SettingOptionDisplayHue) == 0))
		{
			if(general_attr->m_maskSettingItemDisable & (1 << SETTING_MENU_HUE))
			{
				general_attr->m_maskSettingItemDisable &= ~(1 << SETTING_MENU_HUE);
				general_attr->old_focus = general_attr->new_focus;
				general_attr->new_focus = SETTING_MENU_HUE;
			}
		}
		else if((SETTING_MENU_SATURABILITY < SETTING_MENU_MAX)
		        && (strcmp(general_attr->m_exploreKeyChar, SN_SettingOptionDisplaySaturability) == 0))
		{
			if(general_attr->m_maskSettingItemDisable & (1 << SETTING_MENU_SATURABILITY))
			{
				general_attr->m_maskSettingItemDisable &= ~(1 << SETTING_MENU_SATURABILITY);
				general_attr->old_focus = general_attr->new_focus;
				general_attr->new_focus = SETTING_MENU_SATURABILITY;
			}
		}
#endif
#if TIMEOUT_ID_TIMER_GET_POWER
		else if(strcmp(general_attr->m_exploreKeyChar, SN_SettingOptionDisplayPower) == 0)
		{
			general_attr->m_maskSettingItemDisable &= ~MASK_SettingItemDisable_Power;

			if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_POWER))
			{
				_setting_general_paint_power(msg);
				GUI_SetTimer(msg->h_deswin, ID_TIMER_GET_POWER, TIMEOUT_ID_TIMER_GET_POWER, NULL);
			}

			return EPDK_OK;
		}

#endif

		if(t_maskSettingItemDisable != general_attr->m_maskSettingItemDisable)
		{
			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR);
				general_attr->m_exploreKeyChar_cnt = 0;
			}

			general_attr->item_nr++;
			setting_general_paint(msg, m_eSettingGeneralPaintType_all);
		}
	}
#endif
	return EPDK_OK;
}

__s32 setting_general_key_proc_item_idx(__gui_msg_t *msg)
{
	setting_general_attr_t *general_attr;
	general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(general_attr == NULL)
	{
		return EPDK_FAIL;
	}

	if(general_attr->m_exploreKeyChar_cnt)
	{
		__u32 t_settingItemIdx = general_attr->m_exploreKeyNum;
		general_attr->m_exploreKeyChar_cnt = 0;
		__wrn("t_settingItemIdx=%d\n", t_settingItemIdx);

		if(t_settingItemIdx && (t_settingItemIdx <= general_attr->item_nr))
		{
			general_attr->newDispFocus = t_settingItemIdx;
			setting_general_paint(msg, m_eSettingGeneralPaintType_newFocus_dispItemIdx);
		}

		return EPDK_OK;
	}

	return EPDK_FAIL;
}

static __s32  setting_general_key_proc(__gui_msg_t *msg)
{
	setting_general_attr_t *general_attr;
	static __s32 last_key = -1;
	general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		if((GUI_MSG_KEY_LONGNUM0 <= last_key) && (last_key <= GUI_MSG_KEY_LONGNUM9))
		{
			last_key -= GUI_MSG_KEY_LONGNUM0 - GUI_MSG_KEY_NUM0;
		}

		if((GUI_MSG_KEY_NUM0 <= last_key) && (last_key <= GUI_MSG_KEY_NUM9))
		{
			msg->dwAddData1 = last_key;
			setting_general_key_proc_num(msg);
		}
		else
		{
			// 清除数字键内容
			switch(last_key)
			{
				case GUI_MSG_KEY_ENTER :
				{
					if(EPDK_FAIL == setting_general_key_proc_item_idx(msg))
						main_cmd2parent(msg->h_deswin, ID_OP_RIGHT, general_attr->new_focus, 0);
				}
				break;
				
				case GUI_MSG_KEY_LONGENTER :
				{
					if(EPDK_FAIL == setting_general_key_proc_item_idx(msg))
						main_cmd2parent(msg->h_deswin, ID_OP_RIGHT, general_attr->new_focus, 1);
				}
				break;
//					if(EPDK_FAIL == setting_general_key_proc_item_idx(msg))
//					{
#if SP_APP_SETTING_ADJUST_CONTENT_BY_KEYENTER
						main_cmd2parent(msg->h_deswin, ID_OP_ENTER, general_attr->new_focus, 0);
#else
						//main_cmd2parent(msg->h_deswin, ID_OP_DOWN, general_attr->new_focus, 0);
#endif
//					}

				default :
#if TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR
					if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR))
					{
						GUI_KillTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR);
						general_attr->m_exploreKeyChar_cnt = 0;
					}

#endif
					break;
			}
		}
	}
	else
	{
#if TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR

		if(((GUI_MSG_KEY_LONGNUM0 <= msg->dwAddData1) && (msg->dwAddData1 <= GUI_MSG_KEY_LONGNUM9))
		    || ((GUI_MSG_KEY_NUM0 <= msg->dwAddData1) && (msg->dwAddData1 <= GUI_MSG_KEY_NUM9)))
		{
			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR))
			{
				GUI_ResetTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR, TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR, NULL);
			}
		}

#endif

		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_NUMLOCK :
			{
				if(KEY_DOWN_ACTION == msg->dwAddData2)
				{
#if MASK_SysFlags_ShiftNum
					reg_system_para_t *para;
					para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

					if(para)
					{
						if(para->m_maskFlags & MASK_SysFlags_ShiftNum)
						{
							para->m_maskFlags &= ~MASK_SysFlags_ShiftNum;
						}
						else
						{
							para->m_maskFlags |= MASK_SysFlags_ShiftNum;
						}
					}

#endif
				}
			}
			break;

			case GUI_MSG_KEY_LEFT:		//映射为up
			case GUI_MSG_KEY_LONGLEFT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_UP, general_attr->new_focus, 0);
			}
			break;

			case GUI_MSG_KEY_RIGHT:		//映射为down
			case GUI_MSG_KEY_LONGRIGHT:
			{
				main_cmd2parent(msg->h_deswin, ID_OP_DOWN, general_attr->new_focus, 0);
			}
			break;

			case GUI_MSG_KEY_VADD :
			case GUI_MSG_KEY_UP:
				main_cmd2parent(msg->h_deswin, ID_OP_RIGHT, general_attr->new_focus, 0);
				break;
				
			case GUI_MSG_KEY_LONGVADD :
			case GUI_MSG_KEY_LONGUP:
				main_cmd2parent(msg->h_deswin, ID_OP_RIGHT, general_attr->new_focus, 1);
				break;

			case GUI_MSG_KEY_VDEC :
			case GUI_MSG_KEY_DOWN:
				main_cmd2parent(msg->h_deswin, ID_OP_LEFT, general_attr->new_focus, 0);
				break;
				
			case GUI_MSG_KEY_LONGVDEC :
			case GUI_MSG_KEY_LONGDOWN:
				main_cmd2parent(msg->h_deswin, ID_OP_LEFT, general_attr->new_focus, 1);
				break;

			case GUI_MSG_KEY_ESCAPE:
			case GUI_MSG_KEY_MENU:
				main_cmd2parent(msg->h_deswin, ID_OP_EXIT, general_attr->new_focus, 0);
				break;

			default:
				break;
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

	return EPDK_OK;
}

/*
	回调
*/
static __s32 _setting_general_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			setting_general_attr_t *general_attr;
			setting_general_para_t *general_para;
			SIZE screen_size;
			setting_reg_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
			general_para = (setting_general_para_t *)GUI_WinGetAttr(msg->h_deswin);
			general_attr = (setting_general_attr_t *)My_Balloc(sizeof(setting_general_attr_t));

			if(!general_attr)
			{
				__msg("memory balloc fail.........\n");
				return EPDK_FAIL;
			}

			eLIBs_memset(general_attr, 0, sizeof(setting_general_attr_t));
			dsk_display_get_size(&screen_size.width, &screen_size.height);
			general_attr->general_uipara = setting_get_general_para(screen_size);
			general_attr->new_focus = general_para->focus_id;
			general_attr->old_focus = general_para->focus_id;
			general_attr->general_para = general_para;
			_setting_general_get_res_id_mapping(g_settingResIdMapping);
			_setting_general_res_get_const(general_attr);
			general_attr->focus_txt_color = APP_COLOR_YELLOW;
			general_attr->unfocus_txt_color = APP_COLOR_WHITE;
			general_attr->font = general_para->font;
			general_attr->layer = general_para->layer;
			GUI_WinSetAddData(msg->h_deswin, (__u32)general_attr);
			GUI_LyrWinSetSta(general_attr->layer, GUI_LYRWIN_STA_ON);
			GUI_LyrWinSetTop(general_attr->layer);
			setting_general_paint(msg, m_eSettingGeneralPaintType_all);

			if(!general_attr->pRegParamBak)
			{
				general_attr->pRegParamBak = (reg_system_para_t *)My_Balloc(sizeof(reg_system_para_t));
				if(general_attr->pRegParamBak)
				{
					memcpy(general_attr->pRegParamBak, setting_reg_para, sizeof(reg_system_para_t));
				}
			}
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:
		{
			GUI_FrmWinDelete(msg->h_deswin);
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY:
		{
			setting_general_attr_t *general_attr;
			setting_general_para_t *general_para;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!general_attr)
			{
				__err("general_attr is null ......\n");
				return EPDK_FAIL;
			}

#if TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR);
			}

#endif
#if TIMEOUT_ID_TIMER_GET_POWER

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_POWER))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_GET_POWER);
			}

#endif
			_setting_general_res_uninit(general_attr);
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
	
		#if (MOVIE_AUTO_FLUSH_TIME || MUSIC_AUTO_FLUSH_TIME)
			{
				__bool flagWrite = 1;

				if(general_attr->pRegParamBak)
				{
					general_attr->pRegParamBak->m_knlTime = setting_reg_para->m_knlTime;
					if(EPDK_YES == eLIBs_memcmp(general_attr->pRegParamBak, setting_reg_para, sizeof(reg_system_para_t)))
					{
						flagWrite = 0;
						__msg_jjl("app_setting param unchanged.\n");
					}
				}
				
				if(flagWrite)		dsk_reg_flush();	//写进文件里面
			}
		#endif
	
			general_para = general_attr->general_para;

			if(general_attr->pRegParamBak)
			{
				My_Bfree(general_attr->pRegParamBak, sizeof(reg_system_para_t));
			}
			
			if(general_para)
			{
				My_Bfree(general_para, sizeof(setting_general_para_t));
			}

			if(general_attr)
			{
				My_Bfree(general_attr, sizeof(setting_general_attr_t));
			}
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:
			//setting_general_paint(msg, m_eSettingGeneralPaintType_all);
			return EPDK_OK;

		case GUI_MSG_KEY:
			setting_general_key_proc(msg);
			return EPDK_OK;

		case GUI_MSG_TOUCH:
			return EPDK_OK;

		case GUI_MSG_TIMER :
			switch(msg->dwAddData1)
			{
#if TIMEOUT_ID_TIMER_CLR_EXPLORE_KEYCHAR

				case ID_TIMER_CLR_EXPLORE_KEYCHAR :
					if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR))
					{
						GUI_KillTimer(msg->h_deswin, ID_TIMER_CLR_EXPLORE_KEYCHAR);
						setting_general_key_proc_item_idx(msg);
					}

					break;
#endif
#if TIMEOUT_ID_TIMER_GET_POWER

				case ID_TIMER_GET_POWER :
					_setting_general_paint_power(msg);
					break;
#endif

				default :
					break;
			}

			return EPDK_OK;

		case MSG_OP_UP:
		{
			setting_general_attr_t *general_attr;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

			if(general_attr->item_nr > 1)
			{
				__s32 cnt = SETTING_MENU_MAX;
				general_attr->old_focus = general_attr->new_focus;

				do
				{
					cnt--;

					if(general_attr->new_focus <= 0)
					{
						general_attr->new_focus = cnt;
					}
					else
					{
						general_attr->new_focus--;
					}
				}
				while((cnt) && (general_attr->m_maskSettingItemDisable & (1 << general_attr->new_focus)));

				setting_general_paint(msg, m_eSettingGeneralPaintType_newFocus_menuItemIdx);
			}
		}

		return EPDK_OK;

		case MSG_OP_DOWN:
		{
			setting_general_attr_t *general_attr;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

			if(general_attr->item_nr > 1)
			{
				__s32 cnt = SETTING_MENU_MAX;
				general_attr->old_focus = general_attr->new_focus;

				do
				{
					cnt--;

					if(general_attr->new_focus >= cnt)
					{
						general_attr->new_focus = 0;
					}
					else
					{
						general_attr->new_focus++;
					}
				}
				while((cnt) && (general_attr->m_maskSettingItemDisable & (1 << general_attr->new_focus)));

				setting_general_paint(msg, m_eSettingGeneralPaintType_newFocus_menuItemIdx);
			}
		}

		return EPDK_OK;
		
#if SP_APP_SETTING_ADJUST_CONTENT_BY_KEYENTER

		case MSG_OP_LEFT:
		{
			setting_general_attr_t *general_attr;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
			general_attr->old_focus = general_attr->new_focus;

			if(general_attr->new_focus <= 0)
			{
				general_attr->new_focus = general_attr->item_nr - 1;
			}
			else
			{
				general_attr->new_focus--;
			}

			setting_general_paint(msg, m_eSettingGeneralPaintType_newFocus_menuItemIdx);
		}

		return EPDK_OK;

		case MSG_OP_RIGHT:
		{
			setting_general_attr_t *general_attr;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
			general_attr->old_focus = general_attr->new_focus;

			if(general_attr->new_focus >= general_attr->item_nr - 1)
			{
				general_attr->new_focus = 0;
			}
			else
			{
				general_attr->new_focus++;
			}

			setting_general_paint(msg, m_eSettingGeneralPaintType_newFocus_menuItemIdx);
		}

		return EPDK_OK;
#else

		case MSG_OP_LEFT:
		{
			setting_general_attr_t *general_attr;
			setting_item_res_t  *p_item_res;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
			p_item_res = &general_attr->m_settingItemRes[general_attr->new_focus];

			switch(general_attr->new_focus)
			{
				case SETTING_MENU_LANGUAGE:
				{
					if(p_item_res->content_num == (m_eShiftLanguageIdx_begin+1))
					{
						p_item_res->content_num = p_item_res->content_nr - 1;
					}
					else
					{
						p_item_res->content_num--;
					}

					if(setting_reg_para)
					{
						switch(p_item_res->content_num)
						{
							case m_eShiftLanguageIdx_ChineseS :
								setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
								break;

							case m_eShiftLanguageIdx_English :
								setting_reg_para->language = EPDK_LANGUAGE_ENM_ENGLISH;
								break;

							default :
								return EPDK_FAIL;
								break;
						}

						dsk_langres_set_type(setting_reg_para->language);
						setting_general_paint(msg, m_eSettingGeneralPaintType_all);
					}
				}
				break;

				case SETTING_MENU_STYLE:
				{
					if(!p_item_res->content_num)
					{
						p_item_res->content_num = p_item_res->content_nr - 1;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->style = p_item_res->content_num;

						if(gscene_bgd_get_default_bg_index() != setting_reg_para->style)
						{
							gscene_bgd_set_flag(1);
							gscene_bgd_set_default_bg_index(setting_reg_para->style);
						}
					}
				}
				break;

				case SETTING_MENU_INFO:
				{
					main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_PRODUCT_INFO, 0);
				}
				break;

				case SETTING_MENU_FACTORY:				//恢复出厂设置
				{
					main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 0);
				}
				break;

				case SETTING_MENU_AVOUT_FORMAT:
				{
					if(!p_item_res->content_num)
					{
						p_item_res->content_num = p_item_res->content_nr - 1;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->pal_ntsc = p_item_res->content_num;
					}
				}
				break;

				case SETTING_MENU_KEYTONE:
				{
					if(!p_item_res->content_num)
					{
						p_item_res->content_num = p_item_res->content_nr - 1;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->keytone = p_item_res->content_num + 1;
						dsk_keytone_set_state((__set_keytone_t)(setting_reg_para->keytone));//desktop模块里面1 表示on,2 表示off
					}

					dsk_keytone_on();
				}
				break;

				case SETTING_MENU_VOLUMESET:
				{
					if(!p_item_res->content_num)
					{
						p_item_res->content_num = p_item_res->content_nr - 1;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->volume = p_item_res->content_num;
						dsk_volume_set((__lion_bright_t)(setting_reg_para->volume));
					}
				}
				break;

				case SETTING_MENU_BRIGHT:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num <= (CONST_DispBright_Min + STEP_DispParam))
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num -= STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num <= CONST_DispBright_Min)
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num--;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->bright = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->bright, DISP_CMD_SET_BRIGHT);
					}

					break;

				case SETTING_MENU_CONTRAST:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num <= (CONST_DispContrast_Min + STEP_DispParam))
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num -= STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num <= CONST_DispContrast_Min)
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num--;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->contrast = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->contrast, DISP_CMD_SET_CONTRAST);
					}

					break;

				case SETTING_MENU_PWM:
					if(p_item_res->content_num <= 1)
					{
						p_item_res->content_num = p_item_res->content_nr;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->pwm = p_item_res->content_num - 1;
						dsk_display_set_lcd_pwm((__lion_bright_t)(setting_reg_para->pwm));
					}

					break;

				case SETTING_MENU_PWM_EX:
					if(p_item_res->content_num <= 1)
					{
						p_item_res->content_num = p_item_res->content_nr;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->pwm1 = p_item_res->content_num - 1;
						dsk_display_set_lcd_pwm_ex((__lion_bright_t)(setting_reg_para->pwm1));
					}

					break;
					
				case SETTING_MENU_BL_TIME:
					if(!p_item_res->content_num)
					{
						p_item_res->content_num = p_item_res->content_nr - 1;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);
					{
						__u32  close_scn_time ;

						switch(p_item_res->content_num)
						{
							case BACKLIGHTOFF_NEVER:
								g_set_close_scn_time(0);
								close_scn_time = 0;
								break;

							case BACKLIGHTOFF_10SECOND:
								g_set_close_scn_time(10);//10s
								close_scn_time = 10;
								break;

							case BACKLIGHTOFF_30SECOND:
								g_set_close_scn_time(30);// 30s
								close_scn_time = 30;
								break;

							case BACKLIGHTOFF_60SECOND:
								g_set_close_scn_time(60);
								close_scn_time = 60;
								break;

							case BACKLIGHTOFF_90SECOND:
								g_set_close_scn_time(90);
								close_scn_time = 90;
								break;

							default:
								g_set_close_scn_time(0);
								close_scn_time = 0;
								break;
						}

						if(setting_reg_para)
						{
							setting_reg_para->backlightoff = close_scn_time;
						}
					}
					break;

			#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
				case SETTING_MENU_ECHO_SET:
					if(p_item_res->content_num <= 1)
					{
						p_item_res->content_num = p_item_res->content_nr;
					}
					else
					{
						p_item_res->content_num--;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->m_echoLevel = p_item_res->content_num - 1;

						if(dsk_audio_echo_is_open())
						{
							dsk_echo_level_set(setting_reg_para->m_echoLevel);
						}
					}

					break;
			#endif

				case SETTING_MENU_HUE:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num <= (CONST_DispHue_Min + STEP_DispParam))
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num -= STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num <= CONST_DispHue_Min)
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num--;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->hue = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->hue, DISP_CMD_SET_HUE);
					}

					break;

				case SETTING_MENU_SATURABILITY:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num <= (CONST_DispSaturability_Min + STEP_DispParam))
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num -= STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num <= CONST_DispSaturability_Min)
						{
							p_item_res->content_num = p_item_res->content_nr;
						}
						else
						{
							p_item_res->content_num--;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->saturability = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->saturability, DISP_CMD_SET_SATURATION);
					}

					break;

				case SETTING_MENU_DEBUG_MODE:
					if(!p_item_res->content_num)
					{
						p_item_res->content_num = p_item_res->content_nr -1;
					}
					else
					{
						p_item_res->content_num--;
					}

					if(setting_reg_para)
					{
						__u32 t_maskSettingItemDisable = general_attr->m_maskSettingItemDisable;
						__u32 t_item_nr = general_attr->item_nr;
						if(p_item_res->content_num)
						{
							setting_reg_para->m_maskFlags |= MASK_SysFlags_DebugMode;
							general_attr->m_maskSettingItemDisable &= ~(0
								//|(1<<SETTING_MENU_DEBUG_MODE)
								|(1<<SETTING_MENU_BRIGHT)
								|(1<<SETTING_MENU_CONTRAST)
								|(1<<SETTING_MENU_HUE)
								|(1<<SETTING_MENU_SATURABILITY)
								|(1<<SETTING_MENU_PWM)
								);
						}
						else
						{
							setting_reg_para->m_maskFlags &= ~MASK_SysFlags_DebugMode;
							general_attr->m_maskSettingItemDisable |= (0
								//|(1<<SETTING_MENU_DEBUG_MODE)
								|(1<<SETTING_MENU_BRIGHT)
								|(1<<SETTING_MENU_CONTRAST)
								|(1<<SETTING_MENU_HUE)
								|(1<<SETTING_MENU_SATURABILITY)
								|(1<<SETTING_MENU_PWM)
								);
						}
						
						t_maskSettingItemDisable ^= general_attr->m_maskSettingItemDisable;
						while(t_maskSettingItemDisable)
						{
							if(t_maskSettingItemDisable & 1)
							{
								if(p_item_res->content_num)
								{
									general_attr->item_nr++;
								}
								else
								{
									general_attr->item_nr--;
								}
							}
							t_maskSettingItemDisable >>= 1;
						}
						
						if(t_item_nr != general_attr->item_nr)
						{
							setting_general_paint(msg, m_eSettingGeneralPaintType_all);
						}
					}
					else
					{
						setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);
					}
					
					break;

				default:
					break;
			}
		}

		return EPDK_OK;

		case MSG_OP_RIGHT:
#endif
		case MSG_OP_ENTER:
		{
			setting_general_attr_t *general_attr;
			setting_item_res_t  *p_item_res;
			general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
			p_item_res = &general_attr->m_settingItemRes[general_attr->new_focus];

			switch(general_attr->new_focus)
			{
				case SETTING_MENU_LANGUAGE:
				{
					if(p_item_res->content_num >= p_item_res->content_nr - 1)
					{
						p_item_res->content_num = m_eShiftLanguageIdx_begin+1;
					}
					else
					{
						p_item_res->content_num++;
					}

					if(setting_reg_para)
					{
						switch(p_item_res->content_num)
						{
							case m_eShiftLanguageIdx_ChineseS :
								setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
								break;
						
							case m_eShiftLanguageIdx_English :
								setting_reg_para->language = EPDK_LANGUAGE_ENM_ENGLISH;
								break;
						
							default :
								return EPDK_FAIL;
								break;
						}

						dsk_langres_set_type(setting_reg_para->language);
						setting_general_paint(msg, m_eSettingGeneralPaintType_all);
					}
				}
				break;

				case SETTING_MENU_STYLE:
				{
					if(p_item_res->content_num >= p_item_res->content_nr - 1)
					{
						p_item_res->content_num = 0;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->style = p_item_res->content_num;

						if(gscene_bgd_get_default_bg_index() != setting_reg_para->style)
						{
							gscene_bgd_set_flag(1);
							gscene_bgd_set_default_bg_index(setting_reg_para->style);
						}
					}
				}
				break;

				case SETTING_MENU_INFO:
				{
					main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_PRODUCT_INFO, 0);
				}
				break;

				case SETTING_MENU_FACTORY:				//恢复出厂设置
				{
					main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 0);
				}
				break;

				case SETTING_MENU_AVOUT_FORMAT:
				{
					if(p_item_res->content_num >= p_item_res->content_nr - 1)
					{
						p_item_res->content_num = 0;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->pal_ntsc = p_item_res->content_num;
					}
				}
				break;

				case SETTING_MENU_KEYTONE:
				{
					if(p_item_res->content_num >= p_item_res->content_nr - 1)
					{
						p_item_res->content_num = 0;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->keytone = p_item_res->content_num + 1;
						dsk_keytone_set_state((__set_keytone_t)(setting_reg_para->keytone));//desktop模块里面1 表示on,2 表示off
					}

					dsk_keytone_on();
				}
				break;

				case SETTING_MENU_VOLUMESET:
					if(p_item_res->content_num >= p_item_res->content_nr - 1)
					{
						p_item_res->content_num = 0;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->volume = p_item_res->content_num;
						dsk_volume_set((__lion_bright_t)(setting_reg_para->volume));
					}

					break;

				case SETTING_MENU_BRIGHT:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num >= (p_item_res->content_nr - STEP_DispParam))
						{
							p_item_res->content_num = CONST_DispBright_Min;
						}
						else
						{
							p_item_res->content_num += STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num >= p_item_res->content_nr)
						{
							p_item_res->content_num = CONST_DispBright_Min;
						}
						else
						{
							p_item_res->content_num++;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->bright = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->bright, DISP_CMD_SET_BRIGHT);
					}

					break;
					break;

				case SETTING_MENU_CONTRAST:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num >= (p_item_res->content_nr - STEP_DispParam))
						{
							p_item_res->content_num = CONST_DispContrast_Min;
						}
						else
						{
							p_item_res->content_num += STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num >= p_item_res->content_nr)
						{
							p_item_res->content_num = CONST_DispContrast_Min;
						}
						else
						{
							p_item_res->content_num++;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->contrast = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->contrast, DISP_CMD_SET_CONTRAST);
					}

					break;

				case SETTING_MENU_PWM:
					if(p_item_res->content_num >= p_item_res->content_nr)
					{
						p_item_res->content_num = 1;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->pwm = p_item_res->content_num - 1;
						dsk_display_set_lcd_pwm((__lion_bright_t)(setting_reg_para->pwm));
					}

					break;

				case SETTING_MENU_PWM_EX:
					if(p_item_res->content_num >= p_item_res->content_nr)
					{
						p_item_res->content_num = 1;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->pwm1 = p_item_res->content_num - 1;
						dsk_display_set_lcd_pwm_ex((__lion_bright_t)(setting_reg_para->pwm1));
					}

					break;
					
				case SETTING_MENU_BL_TIME:
					if(p_item_res->content_num >= p_item_res->content_nr - 1)
					{
						p_item_res->content_num = 0;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);
					{
						__u32  close_scn_time ;

						switch(p_item_res->content_num)
						{
							case BACKLIGHTOFF_NEVER:
								g_set_close_scn_time(0);
								close_scn_time = 0;
								break;

							case BACKLIGHTOFF_10SECOND:
								g_set_close_scn_time(10);//10s
								close_scn_time = 10;
								break;

							case BACKLIGHTOFF_30SECOND:
								g_set_close_scn_time(30);// 30s
								close_scn_time = 30;
								break;

							case BACKLIGHTOFF_60SECOND:
								g_set_close_scn_time(60);
								close_scn_time = 60;
								break;

							case BACKLIGHTOFF_90SECOND:
								g_set_close_scn_time(90);
								close_scn_time = 90;
								break;

							default:
								g_set_close_scn_time(0);
								close_scn_time = 0;
								break;
						}

						if(setting_reg_para)
						{
							setting_reg_para->backlightoff = close_scn_time;
						}
					}
					break;

			#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
				case SETTING_MENU_ECHO_SET:
					if(p_item_res->content_num >= p_item_res->content_nr)
					{
						p_item_res->content_num = 1;
					}
					else
					{
						p_item_res->content_num++;
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->m_echoLevel = p_item_res->content_num - 1;

						if(dsk_audio_echo_is_open())
						{
							dsk_echo_level_set(setting_reg_para->m_echoLevel);
						}
					}

					break;
			#endif

				case SETTING_MENU_HUE:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num >= (p_item_res->content_nr - STEP_DispParam))
						{
							p_item_res->content_num = CONST_DispHue_Min;
						}
						else
						{
							p_item_res->content_num += STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num >= p_item_res->content_nr)
						{
							p_item_res->content_num = CONST_DispHue_Min;
						}
						else
						{
							p_item_res->content_num++;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->hue = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->hue, DISP_CMD_SET_HUE);
					}

					break;

				case SETTING_MENU_SATURABILITY:
					if(msg->dwReserved == 1)
					{
						if(p_item_res->content_num >= (p_item_res->content_nr - STEP_DispParam))
						{
							p_item_res->content_num = CONST_DispSaturability_Min;
						}
						else
						{
							p_item_res->content_num += STEP_DispParam;
						}
					}
					else
					{
						if(p_item_res->content_num >= p_item_res->content_nr)
						{
							p_item_res->content_num = CONST_DispSaturability_Min;
						}
						else
						{
							p_item_res->content_num++;
						}
					}

					setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);

					if(setting_reg_para)
					{
						setting_reg_para->saturability = p_item_res->content_num;
						dsk_set_display_global(setting_reg_para->saturability, DISP_CMD_SET_SATURATION);
					}

					break;

				case SETTING_MENU_DEBUG_MODE:
					if(p_item_res->content_num >= p_item_res->content_nr -1)
					{
						p_item_res->content_num = 0;
					}
					else
					{
						p_item_res->content_num++;
					}
					
					if(setting_reg_para)
					{
						__u32 t_maskSettingItemDisable = general_attr->m_maskSettingItemDisable;
						__u32 t_item_nr = general_attr->item_nr;
						if(p_item_res->content_num)
						{
							setting_reg_para->m_maskFlags |= MASK_SysFlags_DebugMode;
							general_attr->m_maskSettingItemDisable &= ~(0
								//|(1<<SETTING_MENU_DEBUG_MODE)
								|(1<<SETTING_MENU_BRIGHT)
								|(1<<SETTING_MENU_CONTRAST)
								|(1<<SETTING_MENU_HUE)
								|(1<<SETTING_MENU_SATURABILITY)
								|(1<<SETTING_MENU_PWM)
								);
						}
						else
						{
							setting_reg_para->m_maskFlags &= ~MASK_SysFlags_DebugMode;
							general_attr->m_maskSettingItemDisable |= (0
								//|(1<<SETTING_MENU_DEBUG_MODE)
								|(1<<SETTING_MENU_BRIGHT)
								|(1<<SETTING_MENU_CONTRAST)
								|(1<<SETTING_MENU_HUE)
								|(1<<SETTING_MENU_SATURABILITY)
								|(1<<SETTING_MENU_PWM)
								);
						}
						
						t_maskSettingItemDisable ^= general_attr->m_maskSettingItemDisable;
						while(t_maskSettingItemDisable)
						{
							if(t_maskSettingItemDisable & 1)
							{
								if(p_item_res->content_num)
								{
									general_attr->item_nr++;
								}
								else
								{
									general_attr->item_nr--;
								}
							}
							t_maskSettingItemDisable >>= 1;
						}
						
						if(t_item_nr != general_attr->item_nr)
						{
							setting_general_paint(msg, m_eSettingGeneralPaintType_all);
						}
					}
					else
					{
						setting_general_paint(msg, m_eSettingGeneralPaintType_curFocus);
					}
					
					break;

				default:
					break;
			}
		}

		return EPDK_OK;

		case MSG_RESTORE_FACTORY:
			setting_restore_default(msg);
			return EPDK_OK;

		case MSG_OP_EXIT:
		{
			__gui_msg_t my_msg;
			my_msg.id = GUI_MSG_CLOSE;
			my_msg.h_srcwin = msg->h_deswin;
			my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
			my_msg.dwAddData1 = 0;
			my_msg.dwAddData2 = 0;
			GUI_SendNotifyMessage(&my_msg);
		}

		return EPDK_OK;

		case GUI_MSG_COMMAND:
			break;

		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_general_win_create(H_WIN h_parent, setting_general_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	setting_general_para_t  *general_para;
	FB fb;
	GUI_LyrWinGetFB(para->layer, &fb);
	general_para = (setting_general_para_t *)My_Balloc(sizeof(setting_general_para_t));

	if(!general_para)
	{
		__msg("memory balloc fail.........\n");
		return EPDK_FAIL;
	}

	eLIBs_memset((void *)general_para, 0, sizeof(setting_general_para_t));
	general_para->font = para->font;
	general_para->layer = para->layer;
	general_para->focus_id = para->focus_id;
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name =	"general setting win",
	              framewin_para.dwExStyle = WS_EX_NONE;
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;
	framewin_para.id         = GENERAL_MENU_ID;
	framewin_para.hHosting = h_parent;
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_general_proc);
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width = fb.size.width;
	framewin_para.rect.height = fb.size.height;
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)general_para;
	framewin_para.hLayer = para->layer;
	return (GUI_FrmWinCreate(&framewin_para));
}


__s32 setting_general_win_delete(H_WIN list_win)
{
	GUI_FrmWinDelete(list_win);
	return EPDK_OK;
}

