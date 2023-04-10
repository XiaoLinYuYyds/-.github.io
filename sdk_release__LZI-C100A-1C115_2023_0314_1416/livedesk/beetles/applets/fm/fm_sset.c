/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: fm_sset.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/07/22 22:00  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "fm_sset.h"

// audio list
static __sset_item_para_t fm_audio_list[] =
{
	{
		// 单声道
		"fm\\audio\\0",
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{0, 0},
			0,
			0,
			0,
			0,
			"MONO",
		},
		{FM_SSET_NC_SEL_AUDIO, DSK_RADIO_AUDIO_MONO},
		0,
		NULL,
	},
	{
		// 立体声
		"fm\\audio\\1",
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{0, 0},
			0,
			0,
			0,
			0,
			"STEREO",
		},
		{FM_SSET_NC_SEL_AUDIO, DSK_RADIO_AUDIO_STEREO},
		0,
		NULL,
	},
	{
		// 退出
		"fm\\audio\\2",
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{STRING_ROOT_EXIT, 0},
			0,
			0,
			0,
			0,
			NULL,
		},
		{FM_SSET_NC_SEL_AUDIO, DSK_RADIO_AUDIO_EXIT},
		0,
		NULL,
	},
};

// channel list
#define FM_CHANNEL_ITEM_NAME "fm\\channel\\CH"

const __sset_item_para_t fm_channel_list_empty_item[2 ] =
  //const __sset_item_para_t fm_channel_list_empty_item =
{

	{
		// 退出
		FM_CHANNEL_ITEM_NAME,
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{STRING_ROOT_EXIT, 0},
			0,
			0,
			0,
			0,
			NULL,	//"exit",		//"exit",
		},
		{FM_SSET_NC_SEL_CHANNEL, 0},
		0,
		NULL,
	},

	{
		// channel 0
		FM_CHANNEL_ITEM_NAME, //"00",
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{0, 0},
			0,
			0,
			0,
			0,
			"empty",
		},
		//{FM_SSET_NC_SEL_CHANNEL, 0},
		{FM_SSET_NC_SEL_CHANNEL, 1},
		0,
		NULL,
	},
};

static __sset_item_para_t fm_channel_list[ID_MAX_CHANNEL_COUNT] =
{

	{
		// 退出
		FM_CHANNEL_ITEM_NAME,
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{STRING_ROOT_EXIT, 0}, //
			0,
			0,
			0,
			0,
			NULL,//"exit",
		},
		{FM_SSET_NC_SEL_CHANNEL, 0},
		0,
		NULL,
	},

	{
		// channel 0
		FM_CHANNEL_ITEM_NAME, //"00",
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{0, 0},
			0,
			0,
			0,
			0,
			"empty",
		},
		//{FM_SSET_NC_SEL_CHANNEL, 0},
		{FM_SSET_NC_SEL_CHANNEL, 1},
		0,
		NULL,
	},
	{
		// channel 1
		FM_CHANNEL_ITEM_NAME, //"01",
		1,
		TYPE_OPT,
		{
			{0, 0, ID_FM_ICON_LIST_SEL_BMP, 0},
			{0, 0},
			0,
			0,
			0,
			0,
			"empty",
		},
		//{FM_SSET_NC_SEL_CHANNEL, 1},
		{FM_SSET_NC_SEL_CHANNEL, 2},
		0,
		NULL,
	},
};

//主菜单
static __sset_item_para_t fm_mainmenu[] =
{
	{
		// ID_FM_MMENU_EXIT返回
		"fm\\exit",
		1,
		TYPE_TBAR,
		{
			{ID_FM_BTN_EXIT_F_BMP, ID_FM_BTN_EXIT_N_BMP, 0, 0, 0, 0, 0, 0},
			{STRING_FM_RETURN, STRING_FM_RETURN},
			0,
			0,
			0,
			0,
			NULL,
		},
		{FM_SSET_NC_EXIT, 0},

		0,
		NULL,
	},
	{
		// ID_FM_MMENU_CHANNEL频道
		"fm\\channel",
		1,
		TYPE_TBAR,
		{
			{ID_FM_BTN_CHANNEL_F_BMP, ID_FM_BTN_CHANNEL_N_BMP, 0, 0, 0, 0, 0, 0},
			{STRING_FM_CHANNEL, STRING_FM_CHANNEL},
			0,
			0,
			0,
			0,
			NULL,
		},
		{FM_SSET_NC_CHANNEL, FM_SSET_NC_ID_UNKNOW},

		BEETLES_TBL_SIZE(fm_channel_list),
		fm_channel_list,
	},
	{
		// ID_FM_MMENU_MSEARCH手动搜索
		"fm\\msearch",
		1,
		TYPE_TBAR,
		{
			{ID_FM_BTN_MANUALSEARCH_F_BMP, ID_FM_BTN_MANUALSEARCH_N_BMP, 0, 0, 0, 0, 0, 0},
			{STRING_FM_MANUAL, STRING_FM_MANUAL},
			0,
			0,
			0,
			0,
			NULL,
		},
		{FM_SSET_NC_MSEARCH, 0},

		0,
		NULL,
	},
	{
		// ID_FM_MMENU_ASEARCH自动搜索
		"fm\\asearch",
		1,
		TYPE_TBAR,
		{
			{ID_FM_BTN_AUTOSEARCH_F_BMP, ID_FM_BTN_AUTOSEARCH_N_BMP, 0, 0, 0, 0, 0, 0},
			{STRING_FM_AUTO, STRING_FM_AUTO},
			0,
			0,
			0,
			0,
			NULL,
		},
		{FM_SSET_NC_ASEARCH, 0},

		0,
		NULL,
	},
	/*
	{// ID_FM_MMENU_AUDIO
	    "fm\\audio",
	    1,
	    TYPE_TBAR,
	    {
	        {ID_FM_BTN_AUDIO_F_BMP, ID_FM_BTN_AUDIO_N_BMP,0,0,0,0,0,0},
	        {STRING_FM_TRACK,STRING_FM_TRACK},
	        0,
	        0,
	        0,
	        0,
	        NULL,
	    },
	    {FM_SSET_NC_AUDIO,FM_SSET_NC_ID_UNKNOW},

	    BEETLES_TBL_SIZE(fm_audio_list),
	    fm_audio_list,
	},*/
	{
		// ID_FM_MMENU_RECORD录音
		"fm\\record",
		1,
		TYPE_TBAR,
		{
			{ID_FM_BTN_RECORD_F_BMP, ID_FM_BTN_RECORD_N_BMP, 0, 0, 0, 0, 0, 0},
			{STRING_FM_RECORD, STRING_FM_RECORD},
			0,
			0,
			0,
			0,
			NULL,
		},
		{FM_SSET_NC_RECORD, 0},

		0,
		NULL,
	},
};


//sset配置入口
static __sset_item_para_t fm_sset_root[] =
{
	{
		"fm",                    // 名称
		1,	                     // 是否有效
		TYPE_NORMAL,             // 风格
		{
			{0, 0, 0, 0, 0, 0, 0, 0}, // 图标资源ID
			{0, 0},              // 文本资源ID
			0,                   // 当前值
			0,                   // 最大值
			0,                   // 最小值
			0,                   // OPT选中标示
			NULL,                // 文本数据
		},
		{0, 0},                  // 消息码

		BEETLES_TBL_SIZE(fm_mainmenu),   // 本项子菜单中的条目数
		fm_mainmenu,                     // 本项子菜单地址
	},
};

//////////////////////////////////////////////////////////////////////////
//重置为默认值
#if 1//增加退出
static void FM_ParaReset(void)
{
	char  buf[16] = {0};
	__s32 i;

	for(i = 0; i < ID_MAX_CHANNEL_COUNT; i++)
	{
		if(0 == i)
		{
			eLIBs_memcpy(&fm_channel_list[0], &fm_channel_list_empty_item[0], sizeof(__sset_item_para_t));
		}
		else
		{
			eLIBs_memcpy(&fm_channel_list[i], &fm_channel_list_empty_item[1], sizeof(__sset_item_para_t));
		}

		app_sprintf(buf, "%02d\n", i);
		eLIBs_strcat(fm_channel_list[i].name, buf);
		fm_channel_list[i].item_msg.msg_value = i;
		__wrn(" msg_value = %d\n", i);
	}

	__wrn(" msg_value = %d\n", i);

	for(i = 0; i < BEETLES_TBL_SIZE(fm_audio_list); i++)
	{
		fm_audio_list[i].item_attr.opt = 0;
	}
}

void FM_InitSetting(void)
{
	//读注册表来相关初始化数据
	__s32 i;
	char exit_txt[128] = {0} ;
	reg_fm_para_t *para;
	get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
	__wrn(" cur_count = %d,   sel_audio = %d\n", fm_reg_get_channel_count(para), fm_reg_get_audio(para));
	//频道列表

	for(i = 0; i < BEETLES_TBL_SIZE(fm_channel_list); i++)	//因为增加退出一项
	{
		if(0 == i)
		{
			//eLIBs_strcpy(fm_channel_list[0].item_attr.text, "exit");
		}
		else
		{
			if((i - 1 < fm_reg_get_channel_count(para)) && (para->FM_channel[i - 1] != 0)) //reserverd[0]用于保存有效频道个数
			{
				MAKE_STR_CHANNEL_FREQ(fm_channel_list[i].item_attr.text, para->FM_channel[i - 1]);
				__wrn("fm_channel_list[%d] = %d, para->FM_channel[%d-1] = %d\n",
				      i, fm_channel_list[i].item_attr.text , i, para->FM_channel[i - 1]);
			}
			else
			{
				eLIBs_strcpy(fm_channel_list[i].item_attr.text, "empty");
			}
		}
	}

	__wrn("para->channel_id = %d \n", para->channel_id);

	if(para->channel_id < BEETLES_TBL_SIZE(fm_channel_list))
	{
		fm_channel_list[para->channel_id + 1].item_attr.opt = 1;
		__wrn("fm_channel_list[%d] = %d\n", para->channel_id + 1, para->channel_id + 1);
	}
	else
	{
		//fm_channel_list[para->channel_id = 0].item_attr.opt = 1;
		fm_channel_list[1].item_attr.opt = 1;
	}

	//声道列表
	dsk_radio_get_audio_mode(&para->reserverd[1]);

	//__wrn("initSETTING sel_audio = %d\n", para->reserverd[1]);

	if((para->reserverd[1] - 1) < BEETLES_TBL_SIZE(fm_audio_list))	//reserverd[1]用于保存声道
	{
		fm_audio_list[para->reserverd[1] - 1].item_attr.opt = 1;
	}
	else
	{
		fm_audio_list[para->reserverd[1] - 1].item_attr.opt = 1;
	}

	/*
		//声道列表
		if (fm_reg_get_audio(para) < BEETLES_TBL_SIZE(fm_audio_list))//reserverd[1]用于保存声道
		{

			fm_audio_list[fm_reg_get_audio(para)].item_attr.opt = 1;
		}
		else
		{
			fm_audio_list[fm_reg_set_audio(para, 0)].item_attr.opt = 1;
		}
	*/
}
#else
//重置为默认值
static void FM_ParaReset(void)
{
	char  buf[16] = {0};
	__s32 i;

	for(i = 0; i < ID_MAX_CHANNEL_COUNT; i++)
	{
		eLIBs_memcpy(&fm_channel_list[i], &fm_channel_list_empty_item, sizeof(__sset_item_para_t));
		app_sprintf(buf, "%02d", i);
		eLIBs_strcat(fm_channel_list[i].name, buf);
		fm_channel_list[i].item_msg.msg_value = i;
		__wrn("msg_value = %d\n", i);
	}

	__wrn("msg_value = %d\n", i);

	for(i = 0; i < BEETLES_TBL_SIZE(fm_audio_list); i++)
	{
		fm_audio_list[i].item_attr.opt = 0;
	}
}

void FM_InitSetting(void)
{
	//读注册表来相关初始化数据
	__s32 i;
	reg_fm_para_t *para;
	get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
	__wrn("count = %d, sel_audio = %d\n", fm_reg_get_channel_count(para), fm_reg_get_audio(para));

	//频道列表
	for(i = 0; i < BEETLES_TBL_SIZE(fm_channel_list); i++)
	{
		if((i < fm_reg_get_channel_count(para)) && (para->FM_channel[i] != 0)) //reserverd[0]用于保存有效频道个数
		{
			MAKE_STR_CHANNEL_FREQ(fm_channel_list[i].item_attr.text, para->FM_channel[i]);
			__wrn("fm_channel_list[%d] = %d, para->FM_channel[%d] = %d\n",
			      i, fm_channel_list[i].item_attr.text , i, para->FM_channel[i - 1]);
		}
		else
		{
			eLIBs_strcpy(fm_channel_list[i].item_attr.text, "empty");
		}
	}

	__wrn("para->channel_id = %d \n", para->channel_id);

	if(para->channel_id < BEETLES_TBL_SIZE(fm_channel_list))
	{
		fm_channel_list[para->channel_id].item_attr.opt = 1;
		__wrn("para->channel_id = %d \n", para->channel_id);
	}
	else
	{
		fm_channel_list[para->channel_id = 0].item_attr.opt = 1;
	}

	//声道列表
	dsk_radio_get_audio_mode(&para->reserverd[1]);
	__msg("initSETTING sel_audio = %d\n", para->reserverd[1]);

	if((para->reserverd[1] - 1) < BEETLES_TBL_SIZE(fm_audio_list))	//reserverd[1]用于保存声道
	{
		fm_audio_list[para->reserverd[1] - 1].item_attr.opt = 1;
	}
	else
	{
		fm_audio_list[para->reserverd[1] - 1].item_attr.opt = 1;
	}

	/*
	//声道列表
	if (fm_reg_get_audio(para) < BEETLES_TBL_SIZE(fm_audio_list))//reserverd[1]用于保存声道
	{

	fm_audio_list[fm_reg_get_audio(para)].item_attr.opt = 1;
	}
	else
	{
	fm_audio_list[fm_reg_set_audio(para, 0)].item_attr.opt = 1;
	}
	*/
}
#endif


//创建及初始化SSET创建参数实例
__sset_create_para_t *FM_SSET_Build_CreatePara(H_WIN h_win)
{
	__sset_create_para_t *create_para;
	BallocType(create_para, __sset_create_para_t);
	ZeroTypeMem(create_para, __sset_create_para_t);
	FM_ParaReset();
	create_para->name       = "fm_sset";
	create_para->sid	    = ID_FM_SCENE_SSET; // 1
	create_para->pwin       = h_win;
	create_para->attr       = fm_sset_root;
	create_para->bfont      = SWFFont;
	create_para->sfont      = SWFFont;
	create_para->icon_res   = NULL; // TODO:to remove ?
	create_para->icon_style = 0;    // TODO:to remove ?
	create_para->lang_res   = NULL; // TODO:to remove ?
	create_para->lang_style = NULL; // TODO:to remove ?
	create_para->draw_code  = get_language_charset();
	create_para->time_speed = SSET_DEFAULLT_TIME_SPEED;
	__inf("draw_code = %d\n", create_para->draw_code);
	FM_InitSetting();
	return create_para;
}


//end of file

