/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: atv_menu.c
* By      	: Andy.zhang
* Func		: desk atv thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "app_atv_i.h"
#include "atv_uipara.h"
#include "atv_menu.h"
#include "atv_submenu_paint_init.h"
#include "atv_type_paint_init.h"

#if SP_APP_ATV


typedef struct tag_res_id_t
{
	__s32 lang_id;
	__s32 unfocus_bmp_id;
	__s32 focus_bmp_id;
}
mm_res_id_t;

#define W_ProcBarPoint						5
#define W_ProcBar							(W_ProcBarPoint *102)
#define H_ProcBar							20

#define W_TVSYS_BG_BMP					400
#define H_TVSYS_BG_BMP					240
#define W_TVSYS_FINGER_BMP			46
#define H_TVSYS_FINGER_BMP			20
#define W_TVSYS_ITEM						120
#define H_TVSYS_ITEM							24
#define X_TVSYS_ITEM_Space				(W_TVSYS_BG_BMP -(W_TVSYS_FINGER_BMP +W_TVSYS_ITEM)*2)
#define Y_TVSYS_ITEM_Space				(H_TVSYS_ITEM /3)
#define W_TVSYS_STR							94
#define H_TVSYS_STR							16


#ifndef ID_ATV_M0_0_FOCUS_BMP
#define ID_ATV_M0_0_FOCUS_BMP				ID_HOME_MAIN_TV_FOCUS_BMP
#endif
#ifndef ID_ATV_M0_0_LOSE_BMP
#define ID_ATV_M0_0_LOSE_BMP				ID_HOME_MAIN_TV_LOSE_BMP
#endif
#ifndef ID_ATV_M0_1_FOCUS_BMP
#define ID_ATV_M0_1_FOCUS_BMP				ID_HOME_MAIN_EXPLORER_FOCUS_BMP
#endif
#ifndef ID_ATV_M0_1_LOSE_BMP
#define ID_ATV_M0_1_LOSE_BMP				ID_HOME_MAIN_EXPLORER_LOSE_BMP
#endif
#ifndef ID_ATV_M0_2_FOCUS_BMP
#define ID_ATV_M0_2_FOCUS_BMP				ID_HOME_MAIN_GENERAL_SETTING_FOCUS_BMP
#endif
#ifndef ID_ATV_M0_2_LOSE_BMP
#define ID_ATV_M0_2_LOSE_BMP				ID_HOME_MAIN_GENERAL_SETTING_LOSE_BMP
#endif
#ifndef ID_ATV_M0_3_FOCUS_BMP
#define ID_ATV_M0_3_FOCUS_BMP				ID_HOME_MAIN_MOVIE_FOCUS_BMP
#endif
#ifndef ID_ATV_M0_3_LOSE_BMP
#define ID_ATV_M0_3_LOSE_BMP				ID_HOME_MAIN_MOVIE_LOSE_BMP
#endif

static mm_res_id_t uipara_default[] =
{
	{STRING_ATV_TV_PLAY, 			ID_ATV_M0_0_LOSE_BMP, ID_ATV_M0_0_FOCUS_BMP},
	{STRING_ATV_TV_SEARCH_AUTO, 	ID_ATV_M0_1_LOSE_BMP, ID_ATV_M0_1_FOCUS_BMP},
	{STRING_ATV_TV_AUDIO_SYSTEM, 	ID_ATV_M0_2_LOSE_BMP, ID_ATV_M0_2_FOCUS_BMP},
	{STRING_ATV_TV_AVIN,			ID_ATV_M0_3_LOSE_BMP, ID_ATV_M0_3_FOCUS_BMP},
};

#define TIMEOUT_ID_TIMER_ATV_SEARCH				10
#if TIMEOUT_ID_TIMER_ATV_SEARCH
#define ID_TIMER_ATV_SEARCH					5000
#endif

#define TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD		500
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD
#define ID_TIMER_ATV_AUTO_CLEAR_OSD			5100
#endif

static __u8 g_clearCueOsd_idx = 0;
#define IDX_ClearCueOsd_Invalid 			0
#define IDX_ClearCueOsd_PlayCue 		1
#define IDX_ClearCueOsd_ShortcutIdx 		2
#define IDX_ClearCueOsd_Volume			3

#define TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S	3000
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
#define ID_TIMER_ATV_AUTO_CLEAR_OSD_30S		5101
#endif

#if (SP_DEV_TCON_EX == DEV_TCON_EX_BIT1628M)
#define TIMEOUT_ID_TIMER_ATV_PROCVTRC			0	//200
#else
#define TIMEOUT_ID_TIMER_ATV_PROCVTRC			0
#endif
#if TIMEOUT_ID_TIMER_ATV_PROCVTRC
#define ID_TIMER_ATV_PROCVTRC					5102
#endif

#define TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN						5
#if TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN
	#define TIMEOUT_DVD_DISP_EN_ACK 								(15000 /10 /TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN)
	#define TIMEVALID_DVD_DISP_EN_ACK 							(500 /10 /TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN)
	#define ID_TIMER_ATV_DVD_DISP_EN								5103
#endif

#define GUI_MSG_VKEY						(0x5A<<24)	// 虚拟按键消息标志
#define MASK_GUI_MSG_VKEY				(0xFF<<24)
#define GUI_MSG_VKEY_ENTER			(GUI_MSG_VKEY |GUI_MSG_KEY_ENTER)


extern __s32 	g_is_mute;
extern __bool	g_isTconOccupyLcmi_flag;

__s8			g_isAVInput = 0;

__u8		g_atvSubMenuIdx = 0, g_tvSoundChannelChange = 0;
__u16		g_timerCnt_enDvdDisp = 0;
static __u8	gs_enIoctrlDetPaForDVD = 0;

__u32 atv_menu_lyr_del(H_LYR *hLyr_tab, __u32 maskLyr);


static mm_res_id_t *atv_get_item_para(__s32 rotate)
{
	/*
	__s32		screen_width;
	__s32		screen_height;

	// get lcd size//
	dsk_display_get_size(&screen_width, &screen_height);
	switch(rotate)
	{
	case GUI_SCNDIR_NORMAL:
	case GUI_SCNDIR_ROTATE180:
		{
			if((screen_width == 400 )&&( screen_height == 240))
				return uipara_400_240;
			else if((screen_width == 480 )&&( screen_height == 272))
				return uipara_480_272;
			else if((screen_width == 320 )&&( screen_height == 240))
				return uipara_320_240;
			else if((screen_width == 800 )&&( screen_height == 480))
				return uipara_800_480;

		}
	case GUI_SCNDIR_ROTATE90:
	case GUI_SCNDIR_ROTATE270:

		break;
	}
	*/
	return uipara_default;
}

/*
	申请资源
*/
static __s32 init_mmenu_res(mmenu_attr_t *mmenu_attr)
{
	__u32 	i;
	mm_res_id_t *atv_item_para;

	atv_item_para = atv_get_item_para(GUI_GetScnDir());

	if(!atv_item_para)
	{
		__err("atv_ui_para is null...\n");
		return EPDK_FAIL;
	}

	__wrn("init_mmenu_res:MAX_ITEM_NUM=%d\n", MAX_ITEM_NUM);

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(NULL == mmenu_attr->unfocus_bmp[i])
		{
			mmenu_attr->unfocus_bmp[i] = dsk_theme_open(atv_item_para[i].unfocus_bmp_id);
			__wrn("unfocus_bmp=%u, unfocus_bmp_id=%u\n", mmenu_attr->unfocus_bmp[i], atv_item_para[i].unfocus_bmp_id);
		}

		if(NULL == mmenu_attr->focus_bmp[i])
		{
			mmenu_attr->focus_bmp[i] = dsk_theme_open(atv_item_para[i].focus_bmp_id);
			__wrn("focus_bmp=%u, focus_bmp_id=%u\n", mmenu_attr->focus_bmp[i], atv_item_para[i].focus_bmp_id);
		}
	}

	if(mmenu_attr->hTheme_MHz == NULL)
	{
		mmenu_attr->hTheme_MHz = dsk_theme_open(ID_ATV_MHZ_BMP);
	}

	if(mmenu_attr->hTheme_div == NULL)
	{
		mmenu_attr->hTheme_div = dsk_theme_open(ID_ATV_DIV_BMP);
	}

	if(mmenu_attr->hTheme_point == NULL)
	{
		mmenu_attr->hTheme_point = dsk_theme_open(ID_ATV_POINT_BMP);
	}

	if(mmenu_attr->hTheme_percent == NULL)
	{
		mmenu_attr->hTheme_percent = dsk_theme_open(ID_ATV_PERCENT_BMP);
	}

	if(mmenu_attr->hTheme_procbarBody == NULL)
	{
		mmenu_attr->hTheme_procbarBody = dsk_theme_open(ID_ATV_PROCBAR_BODY_BMP);
	}

	if(mmenu_attr->hTheme_procbarPoint == NULL)
	{
		mmenu_attr->hTheme_procbarPoint = dsk_theme_open(ID_ATV_PROCBAR_POINT_BMP);
	}

	for(i = 0; i < 10; i++)
	{
		if(mmenu_attr->hTheme_num[i] == NULL)
		{
			mmenu_attr->hTheme_num[i] = dsk_theme_open(ID_ATV_0_BMP + i);
		}
	}

	for(i = 0; i < 10; i++)
	{
		if(mmenu_attr->hTheme_numEx[i] == NULL)
		{
			mmenu_attr->hTheme_numEx[i] = dsk_theme_open(ID_MOVIE_NUM0_BMP + i);
		}
	}

	for(i = 0; i < m_eTvSystem_max; i++)
	{
		if(mmenu_attr->hTheme_audioSys[i][0] == NULL)
		{
			mmenu_attr->hTheme_audioSys[i][0] = dsk_theme_open(ID_ATV_TVSYS_ITEM_0_FOCUS_PAL_DK_BMP + i * 2);
		}

		if(mmenu_attr->hTheme_audioSys[i][1] == NULL)
		{
			mmenu_attr->hTheme_audioSys[i][1] = dsk_theme_open(ID_ATV_TVSYS_ITEM_0_UNFOCUS_PAL_DK_BMP + i * 2);
		}
	}

#ifdef ID_ATV_M0_TVLOGO_BMP
	if(mmenu_attr->hTheme_TVLogo == NULL)
	{
		mmenu_attr->hTheme_TVLogo = dsk_theme_open(ID_ATV_M0_TVLOGO_BMP);
	}
#endif

	if(mmenu_attr->hTheme_tvSys_bg == NULL)
	{
		mmenu_attr->hTheme_tvSys_bg = dsk_theme_open(ID_ATV_TVSYS_BG_BMP);
	}

	if(mmenu_attr->hTheme_tvSys_finger == NULL)
	{
		mmenu_attr->hTheme_tvSys_finger = dsk_theme_open(ID_ATV_TVSYS_FINGER_BMP);
	}

	if(mmenu_attr->hTheme_tvSys_fingerClr == NULL)
	{
		mmenu_attr->hTheme_tvSys_fingerClr = dsk_theme_open(ID_ATV_TVSYS_FINGER_CLR_BMP);
	}

	return EPDK_OK;
}

/*
	释放资源
*/

static __s32 uninit_mmenu_res(mmenu_attr_t *mmenu_attr)
{
	__u32 	i;
	__wrn("uninit_mmenu_res:begin\n");

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(mmenu_attr->unfocus_bmp[i])
		{
			dsk_theme_close(mmenu_attr->unfocus_bmp[i]);
			mmenu_attr->unfocus_bmp[i] = NULL;
		}

		if(mmenu_attr->focus_bmp[i])
		{
			dsk_theme_close(mmenu_attr->focus_bmp[i]);
			mmenu_attr->focus_bmp[i]  = NULL;
		}
	}

	if(mmenu_attr->hTheme_MHz)
	{
		dsk_theme_close(mmenu_attr->hTheme_MHz);
		mmenu_attr->hTheme_MHz = NULL;
	}

	if(mmenu_attr->hTheme_div)
	{
		dsk_theme_close(mmenu_attr->hTheme_div);
		mmenu_attr->hTheme_div = NULL;
	}

	if(mmenu_attr->hTheme_point)
	{
		dsk_theme_close(mmenu_attr->hTheme_point);
		mmenu_attr->hTheme_point = NULL;
	}

	if(mmenu_attr->hTheme_percent)
	{
		dsk_theme_close(mmenu_attr->hTheme_percent);
		mmenu_attr->hTheme_percent = NULL;
	}

	if(mmenu_attr->hTheme_procbarBody)
	{
		dsk_theme_close(mmenu_attr->hTheme_procbarBody);
		mmenu_attr->hTheme_procbarBody = NULL;
	}

	if(mmenu_attr->hTheme_procbarPoint)
	{
		dsk_theme_close(mmenu_attr->hTheme_procbarPoint);
		mmenu_attr->hTheme_procbarPoint = NULL;
	}

	for(i = 0; i < 10; i++)
	{
		if(mmenu_attr->hTheme_num[i])
		{
			dsk_theme_close(mmenu_attr->hTheme_num[i]);
			mmenu_attr->hTheme_num[i] = NULL;
		}
	}

	for(i = 0; i < 10; i++)
	{
		if(mmenu_attr->hTheme_numEx[i])
		{
			dsk_theme_close(mmenu_attr->hTheme_numEx[i]);
			mmenu_attr->hTheme_numEx[i] = NULL;
		}
	}

	for(i = 0; i < m_eTvSystem_max; i++)
	{
		if(mmenu_attr->hTheme_audioSys[i][0])
		{
			dsk_theme_close(mmenu_attr->hTheme_audioSys[i][0]);
			mmenu_attr->hTheme_audioSys[i][0] = NULL;
		}

		if(mmenu_attr->hTheme_audioSys[i][1])
		{
			dsk_theme_close(mmenu_attr->hTheme_audioSys[i][1]);
			mmenu_attr->hTheme_audioSys[i][1] = NULL;
		}
	}

	if(mmenu_attr->hTheme_TVLogo)
	{
		dsk_theme_close(mmenu_attr->hTheme_TVLogo);
		mmenu_attr->hTheme_TVLogo = NULL;
	}

	if(mmenu_attr->hTheme_tvSys_bg)
	{
		dsk_theme_close(mmenu_attr->hTheme_tvSys_bg);
		mmenu_attr->hTheme_tvSys_bg = NULL;
	}

	if(mmenu_attr->hTheme_tvSys_finger)
	{
		dsk_theme_close(mmenu_attr->hTheme_tvSys_finger);
		mmenu_attr->hTheme_tvSys_finger = NULL;
	}

	if(mmenu_attr->hTheme_tvSys_fingerClr)
	{
		dsk_theme_close(mmenu_attr->hTheme_tvSys_fingerClr);
		mmenu_attr->hTheme_tvSys_fingerClr = NULL;
	}

	__wrn("uninit_mmenu_res:end\n");
	return EPDK_OK;
}

void GUI_DrawString(char *pText, GUI_RECT *gui_rect, mmenu_attr_t *mmenu_attr)
{
	__s16	*x, xOft1, xOft2, len, dir;

	//__msg_jjl("GUI_DrawString(\"%s\")\n", pText);
	len = strlen(pText);
	if(!len)	return;
	
	GUI_ClearRectEx(gui_rect);
	
	if(mmenu_attr->paint_flag & MASK_PaintFlag_RTL)
	{
		mmenu_attr->paint_flag &= ~MASK_PaintFlag_RTL;
		x = &gui_rect->x1;
		xOft1 = -23;
		xOft2 = 0;
		pText += len -1;
		dir = -1;
	}
	else	// 默认LTR:Left To Right
	{
		x = &gui_rect->x0;
		xOft1 = 0;
		xOft2 = 23;
		dir = 1;
	}

	while(len--)
	{
		if(('0' <= *pText) && (*pText <= '9'))
		{
			__s8 num = *pText - '0';

			if(mmenu_attr->hTheme_num[num])
			{
				void *pbmp;
				pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_num[num]);
				*x += xOft1;
				GUI_BMP_Draw(pbmp, *x, gui_rect->y0);
				*x += xOft2;
			}
		}
		else if(*pText == '.')
		{
			if(mmenu_attr->hTheme_point)
			{
				void *pbmp;
				pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_point);
				*x += xOft1;
				GUI_BMP_Draw(pbmp, *x, gui_rect->y0);
				*x += xOft2;
			}
		}
		else if(*pText == '/')
		{
			if(mmenu_attr->hTheme_div)
			{
				void *pbmp;
				pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_div);
				*x += xOft1;
				GUI_BMP_Draw(pbmp, *x, gui_rect->y0);
				*x += xOft2;
			}
		}
		else
		{
			*x += xOft1;
			*x += xOft2;
		}

		pText += dir;
	}
}

void GUI_DrawStringEx(char *pText, GUI_RECT *gui_rect, mmenu_attr_t *mmenu_attr)
{
	__s16	*x, xOft1, xOft2, len, dir;

	//__msg_jjl("GUI_DrawString(\"%s\")\n", pText);
	len = strlen(pText);
	if(!len)	return;
	
	GUI_ClearRectEx(gui_rect);
	
	if(mmenu_attr->paint_flag & MASK_PaintFlag_RTL)
	{
		mmenu_attr->paint_flag &= ~MASK_PaintFlag_RTL;
		x = &gui_rect->x1;
		xOft1 = -W_Num;
		xOft2 = 0;
		pText += len -1;
		dir = -1;
	}
	else	// 默认LTR:Left To Right
	{
		x = &gui_rect->x0;
		xOft1 = 0;
		xOft2 = W_Num;
		dir = 1;
	}

	while(len--)
	{
		if(('0' <= *pText) && (*pText <= '9'))
		{
			__s8 num = *pText - '0';

			if(mmenu_attr->hTheme_numEx[num])
			{
				void *pbmp;
				pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_numEx[num]);
				*x += xOft1;
				GUI_BMP_Draw(pbmp, *x, gui_rect->y0);
				*x += xOft2;
			}
		}
		else
		{
			*x += xOft1;
			*x += xOft2;
		}

		pText += dir;
	}
}

void atv_get_sys_para(mmenu_attr_t *mmenu_attr)
{
	reg_atv_para_t	*regPara = NULL;
	get_reg_pointer(regPara, reg_atv_para_t, REG_APP_ATV);

#if 0
	mmenu_attr->sTunerInfo.m_pFreqTab = China_Catv_Frequency;
	mmenu_attr->sTunerInfo.m_pFreqTab_size = size_China_Catv_Frequency;
#else
	mmenu_attr->sTunerInfo.m_pFreqTab = NULL;
	mmenu_attr->sTunerInfo.m_pFreqTab_size = 0;
#endif

	if(regPara != NULL)
	{		
#if 0 // 创建假台
		{
			__u32 i;
			regPara->m_psTunerAtvList_cnt = 0;

			//for(i = 0; i < (SIZE_sTunerAtvList / 2); i++)
			for(i = 0; i < 10; i++)
			{
				regPara->m_sTunerAtvList[i].m_freq = RANGE_TVFreq_Min + STEP_TVFreq_RDA5218 * i;
				regPara->m_psTunerAtvList_cnt++;
			}
			regPara->m_psTunerAtvList_idx = 0;
		}
#endif

		mmenu_attr->sTunerInfo.m_psTunerAtvList_size = SIZE_sTunerAtvList;
		mmenu_attr->sTunerInfo.m_psTunerAtvList = regPara->m_sTunerAtvList;
		mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt = regPara->m_psTunerAtvList_cnt;			
		mmenu_attr->sTunerInfo.m_psTunerAtvList_idx = regPara->m_psTunerAtvList_idx;
		mmenu_attr->sTunerInfo.m_eTvSystem_tv = regPara->m_eTvSystem_tv;
		mmenu_attr->sTunerInfo.m_eTvSystem_avin = regPara->m_eTvSystem_avin;
#if SP_SaveTVShortcutIdx
		mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab = regPara->m_psTunerAtvList_shortcutIdx_tab;
#endif

		if(!mmenu_attr->pRegParamBak)
		{
			mmenu_attr->pRegParamBak = (reg_atv_para_t *)esMEMS_Balloc(sizeof(reg_atv_para_t));
			if(mmenu_attr->pRegParamBak)
			{
				memcpy(mmenu_attr->pRegParamBak, regPara, sizeof(reg_atv_para_t));
			}
		}
	}
	else
	{
		mmenu_attr->sTunerInfo.m_psTunerAtvList_idx = IDX_sTunerAtvList_invalid;
#if SP_SaveTVShortcutIdx
		mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab = NULL;
#endif
	}

	__msg("m_psTunerAtvList_cnt=%u\n", mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt);
	__msg("m_psTunerAtvList_idx=%d\n", mmenu_attr->sTunerInfo.m_psTunerAtvList_idx);
	__msg("m_eTvSystem_avin=%d\n", mmenu_attr->sTunerInfo.m_eTvSystem_avin);
	__msg("atv_get_sys_para:end\n");
}

void atv_save_sys_para(mmenu_attr_t *mmenu_attr)
{
	if(mmenu_attr->sTunerInfo.m_savePara_mask)
	{
		reg_atv_para_t	*regPara = NULL;
		get_reg_pointer(regPara, reg_atv_para_t, REG_APP_ATV);

		if(regPara == NULL)
		{
			return;
		}

		regPara->m_psTunerAtvList_cnt = mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt;
		regPara->m_psTunerAtvList_idx = mmenu_attr->sTunerInfo.m_psTunerAtvList_idx;
		regPara->m_eTvSystem_tv = mmenu_attr->sTunerInfo.m_eTvSystem_tv;
		regPara->m_eTvSystem_avin = mmenu_attr->sTunerInfo.m_eTvSystem_avin;

		if(mmenu_attr->pRegParamBak)
		{
			if(EPDK_YES == eLIBs_memcmp(mmenu_attr->pRegParamBak, regPara, sizeof(reg_atv_para_t)))
			{
				__msg_jjl("atv param unchanged.\n");
				return;				
			}
			memcpy(mmenu_attr->pRegParamBak, regPara, sizeof(reg_atv_para_t));
		}
		
		dsk_reg_flush();		
		mmenu_attr->sTunerInfo.m_savePara_mask = 0;
	}
}

__s32 atv_drv_open(__gui_msg_t *msg, __u32 inputMode)
{
	mmenu_attr_t *mmenu_attr;
	g_isAVInput = 0;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	if(mmenu_attr->sTunerInfo.m_pEsFile_drv != NULL)
	{
		return EPDK_OK;
	}

#if (!SP_SimIIC)
	esKRNL_TimeDly(10);
	esDEV_Plugin("\\drv\\twi.drv", 0, 0, 1);
	__wrn("after twi.drv plugin\n");
#endif
	esDEV_Plugin("\\drv\\atv.drv", 0, 0, 1);
	__wrn("after atv.drv plugin\n");
	mmenu_attr->sTunerInfo.m_pEsFile_drv = eLIBs_fopen("b:\\USER\\ATV", "r+");

	if(mmenu_attr->sTunerInfo.m_pEsFile_drv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
		return EPDK_FAIL;
	}

	//eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_INIT, 0, 0);
	if((!inputMode) || (inputMode == OP_RootFlags_CVBS_InputMode_TVFromHome))
	{
		__msg_jjl("inputMode=%d\n", inputMode);
		mmenu_attr->sTunerInfo.m_threadId = esKRNL_TCreate(TUNER_ScanTVFreq_Thread,
		                                    (void *)(&mmenu_attr->sTunerInfo),
		                                    0x400, KRNL_priolevel3);
	}
	return EPDK_OK;
}

__s32 atv_drv_close(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	__wrn("atv_drv_close:begin\n");
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	TUNER_ScanTVFreq_Stop(&mmenu_attr->sTunerInfo);

	if(mmenu_attr->sTunerInfo.m_threadId)
	{
		__wrn("atv_drv_close:del thread:begin\n");

		while(1)
		{
			if(esKRNL_TDelReq(mmenu_attr->sTunerInfo.m_threadId) == OS_TASK_NOT_EXIST)
			{
				break;
			}

			esKRNL_TimeDly(3);
			__wrn("atv_drv_close:del thread:wait\n");
		}

		mmenu_attr->sTunerInfo.m_threadId = 0;
		__wrn("atv_drv_close:del thread:over\n");
	}

	if(g_isAVInput)
	{
		g_isAVInput = 0;
		TCON_ReleaseLcmi(&mmenu_attr->sTunerInfo, msg);
	}

	if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY)
	{
		mmenu_attr->sTunerInfo.m_tunerMode = MODE_Tuner_TV;
	}

	TUNER_ExitTV(&mmenu_attr->sTunerInfo, msg);
	atv_menu_lyr_del(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_total);

	if(mmenu_attr->sTunerInfo.m_pEsFile_drv != NULL)
	{
		__wrn("atv_drv_close:close drv file\n");
		eLIBs_fclose(mmenu_attr->sTunerInfo.m_pEsFile_drv);
		mmenu_attr->sTunerInfo.m_pEsFile_drv = NULL;
	}

	esDEV_Plugout("\\drv\\atv.drv", 0);
	__wrn("after atv.drv plugout\n");
#if (!SP_SimIIC)
	esDEV_Plugout("\\drv\\twi.drv", 0);
	__wrn("after twi.drv plugout\n");
#endif
	return EPDK_OK;
}

void atv_enter_tv(mmenu_attr_t *mmenu_attr, __gui_msg_t *msg)
{
	__gui_msg_t t_msg = *msg;
	msg->dwAddData2 = mmenu_attr->hLyr_frame;
	TUNER_PlayTV(&mmenu_attr->sTunerInfo, msg);
	*msg = t_msg;
	paint_atv_tv_info(mmenu_attr);

	if(mmenu_attr->sTunerInfo.m_tunerMode == MODE_Tuner_TV)
	{
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

		if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
		{
			GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
		}
		else
		{
			GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
		}

#endif
		g_clearCueOsd_idx = IDX_ClearCueOsd_PlayCue;
		mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_TVFreq;
#if TIMEOUT_ID_TIMER_ATV_PROCVTRC

		if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_PROCVTRC))
		{
			GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_PROCVTRC, TIMEOUT_ID_TIMER_ATV_PROCVTRC, NULL);
		}
		else
		{
			GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_PROCVTRC, TIMEOUT_ID_TIMER_ATV_PROCVTRC, NULL);
		}

#endif
	}
}

void strdel_num0(char *pstr)
{
	__s16 len = strlen(pstr);
	while(len--)
	{
		switch(pstr[len])
		{
			case '0':	pstr[len] = 0;
				break;
			case '.':		pstr[len] = 0;
			default:	return;
		}
	}
}

__u32 atv_menu_lyr_del(H_LYR *hLyr_tab, __u32 maskLyr)
{
	__s16 i;
	__u32 delLog = 0, sleepLog = 0;

	if(maskLyr == m_eAtvMenuLyr_total)
	{
		maskLyr = ~((__u32)0);
		for(i = 0; i < m_eAtvMenuLyr_total; i++)
		{
			if(maskLyr & (1<<i))
			{
				if(hLyr_tab[i])
				{
					GUI_LyrWinDelete(hLyr_tab[i]);
					hLyr_tab[i] = NULL;
					delLog |= 1<<i;
					if(i == m_eAtvMenuLyr_menuList)
					{
						g_sComMenuInfo_atvSubMenu.maskFlags &= ~MASK_sComMenu_flag__AllPainted;
					}
				}
			}
		}
	}
	else
	{
		for(i = 0; i < m_eAtvMenuLyr_total; i++)
		{
			if(maskLyr & (1<<i))
			{
        if(GUI_LyrWinGetSta(hLyr_tab[i]) == GUI_LYRWIN_STA_ON)
        {
          GUI_LyrWinSetSta(hLyr_tab[i], GUI_LYRWIN_STA_SLEEP);
          sleepLog |= 1<<i;
        }
			}
		}
	}

	__msg_jjl("delLog=0x%X, sleepLog=0x%X\n", delLog, sleepLog);
	return delLog;
}

__s32 atv_menu_lyr_create(H_LYR *hLyr_tab, E_ATV_MENU_LYR a_eAtvMenuLyr)
{
	RECT rect;
	char lyrName[32] = "";
	
	if(a_eAtvMenuLyr >= m_eAtvMenuLyr_total)	return EPDK_FAIL;
	
	if(hLyr_tab[a_eAtvMenuLyr])
	{
    if(GUI_LyrWinGetSta(hLyr_tab[a_eAtvMenuLyr]) != GUI_LYRWIN_STA_ON)
    {
      GUI_LyrWinSetSta(hLyr_tab[a_eAtvMenuLyr], GUI_LYRWIN_STA_ON);
    }
		return EPDK_OK;
	}

	switch(a_eAtvMenuLyr)
	{
		case m_eAtvMenuLyr_menuList:
		{
			rect.width = g_sComMenuInfo_atvSubMenu.layer_sPaintPos.width;
			rect.height = g_sComMenuInfo_atvSubMenu.layer_sPaintPos.height;
			rect.x = g_sComMenuInfo_atvSubMenu.layer_sPaintPos.x;	
			rect.y = g_sComMenuInfo_atvSubMenu.layer_sPaintPos.y;
			strcpy(lyrName, "lyr_menuList");
			break;
		}
		case m_eAtvMenuLyr_num:
		{
			rect.width = W_NumLock_Bg;	
			rect.height = H_NumLock_Bg;
			rect.x = _W_SCR_ -rect.width;	
			rect.y = _H_SCR_  -rect.height;
			strcpy(lyrName, "lyr_num");
			break;
		}
		case m_eAtvMenuLyr_searchTv:
		{
			rect.width = W_ProcBar;	
			rect.height = 200;
			rect.x = (_W_SCR_ -W_ProcBar)/2;	
			rect.y = _H_SCR_ *8/10 -rect.height;
			strcpy(lyrName, "lyr_searchTv");
			break;
		}
		case m_eAtvMenuLyr_tvInfo:
		{
			rect.width = _W_SCR_ /3;	
			rect.height = _H_SCR_ /4;
			rect.x = 0;	
			rect.y = 0;
			strcpy(lyrName, "lyr_tvInfo");
			break;
		}
		case m_eAtvMenuLyr_tvSys:
		{
			rect.width = W_TVSYS_BG_BMP;	
			rect.height = H_TVSYS_BG_BMP;
			rect.x = (_W_SCR_ - W_TVSYS_BG_BMP) / 2;	
			rect.y = (_H_SCR_ - H_TVSYS_BG_BMP) / 2;
			strcpy(lyrName, "lyr_tvSys");
			break;
		}
		default:	return EPDK_FAIL;
	}

	if(!hLyr_tab[a_eAtvMenuLyr])
	{
		hLyr_tab[a_eAtvMenuLyr] = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SLEEP, lyrName);
		if(!hLyr_tab[a_eAtvMenuLyr])	return EPDK_FAIL;
	}

	__msg_jjl("atv_menu_lyr_create(%d):OK(%d,%d,%d,%d)\n", a_eAtvMenuLyr, rect.x, rect.y, rect.width, rect.height);
	return EPDK_OK;
}

__s32 atv_menu_lyr_create_all(H_LYR *hLyr_tab)
{
	__s16 i;
	
	for(i = 0; i < m_eAtvMenuLyr_total; i++)
	{
		atv_menu_lyr_create(hLyr_tab, i);
	}
}

void paint_atv_type_info(__gui_msg_t *msg, E_ATV_TYPE_IDX eAtvTypeIdx)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return;
	}

	atv_menu_lyr_del(mmenu_attr->hLyr_sub, ~(1 << m_eAtvMenuLyr_tvInfo));
	atv_menu_lyr_create(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_tvInfo);
	
	g_sComMenuInfo_atvType.itemFocus = eAtvTypeIdx;
	g_sComMenuInfo_atvType.maskItemDisable = ~(1<<eAtvTypeIdx);
	com_menu_paint_all_item(&g_sComMenuInfo_atvType);

#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

	if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
	{
		GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
	}
	else
	{
		GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
	}

#endif
	g_clearCueOsd_idx = IDX_ClearCueOsd_PlayCue;
}

__s32 paint_atv_tv_info(mmenu_attr_t *mmenu_attr)
{
//1/23
//48.25MHz
//PAL-I
	__s32 tvIdx;
	__u32 tvTotal;
	__u32 freq;
	E_TV_SYSTEM eTvSys;

	if(mmenu_attr->sTunerInfo.m_psTunerAtvList_idx <= IDX_sTunerAtvList_invalid)	return EPDK_FAIL;

	tvIdx = mmenu_attr->sTunerInfo.m_psTunerAtvList_idx;
	tvTotal = mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt;
	if(mmenu_attr->sTunerInfo.tmpFreq)
	{
		freq = mmenu_attr->sTunerInfo.tmpFreq;
	}
	else
	{
		freq = mmenu_attr->sTunerInfo.m_psTunerAtvList[tvIdx].m_freq;
	}	

#if !SP_ATV_SYS_IndepSetEnable
	if(mmenu_attr->sTunerInfo.m_eTvSystem_tv != m_eTvSystem_Auto)
	{
		eTvSys = mmenu_attr->sTunerInfo.m_eTvSystem_tv;	// 实时伴音制式
	}
	else
#endif
	{
		eTvSys = mmenu_attr->sTunerInfo.m_psTunerAtvList[tvIdx].m_eTvSystem;
	} 

	{
		GUI_MEMDEV_Handle draw_mem = NULL;
		FB fb;

		atv_menu_lyr_del(mmenu_attr->hLyr_sub,
			(1 << m_eAtvMenuLyr_num)|(1 << m_eAtvMenuLyr_searchTv));
		if(EPDK_FAIL == atv_menu_lyr_create(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_tvInfo))	return EPDK_FAIL;
		
		GUI_LyrWinGetFB(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvInfo], &fb);
		GUI_LyrWinSel(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvInfo]);
		GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
		draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
		__msg("draw_mem=0x%x\n", draw_mem);
		
		if(draw_mem)
		{
			GUI_MEMDEV_Select(draw_mem);
		}

		{
			GUI_RECT gui_rect;
			char strTmp[32];

			sprintf(strTmp, "%d/%d", tvIdx+1, tvTotal);					// 512/512
			__msg_jjl("strTmp=%s\n", strTmp);			
			gui_rect.x0 = 32;
			gui_rect.y0 = 16;
			gui_rect.x1 = gui_rect.x0 + 23 * 7;
			gui_rect.y1 = gui_rect.y0 + 32 - 1;
			GUI_DrawString(strTmp, &gui_rect, mmenu_attr);

			freq /=10000;
			sprintf(strTmp, "%d.%d", freq /100, freq %100); 				// 863.25
			strdel_num0(strTmp);
			__msg_jjl("strTmp=%s\n", strTmp);			
			gui_rect.x0 = 32;
			gui_rect.y0 = gui_rect.y1 +4;
			gui_rect.x1 = gui_rect.x0 + 23 * 6 + 53 + 4 - 1;
			gui_rect.y1 = gui_rect.y0 + 32 - 1;
			GUI_DrawString(strTmp, &gui_rect, mmenu_attr);
			if(mmenu_attr->hTheme_MHz)
			{
				void *pbmp;
				pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_MHz);
				GUI_BMP_Draw(pbmp, gui_rect.x0 + 4, gui_rect.y0 + 8);
			}

			{
				HTHEME		t_hTheme_itemBase = mmenu_attr->hTheme_audioSys[eTvSys][0];
				if(t_hTheme_itemBase)
				{
					void *pbmp;
					pbmp = dsk_theme_hdl2buf(t_hTheme_itemBase);
					GUI_BMP_Draw(pbmp, 32, gui_rect.y1 +4);
					t_hTheme_itemBase = NULL;
				}
			}
			
		}

		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
			__msg("over draw : draw_mem\n");
		}

		GUI_LyrWinSetTop(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvInfo]);
	}

	return EPDK_OK;
}

__bool paint_atv_volume(mmenu_attr_t *mmenu_attr, __bool mute)
{
	// OSD显示音量
	__u8 writeDispRam_tcon[7];
	__s32 volume;

	if(mute)
	{
		volume = 0;
	}
	else
	{
		volume = dsk_volume_get();

		if(volume < 0)
		{
			return 0;
		}
	}

	if(volume == 0)
	{
		memset(writeDispRam_tcon, 0x20, sizeof(writeDispRam_tcon));
		writeDispRam_tcon[0] = 0x7B;
	}
	else
	{
		memset(writeDispRam_tcon, 0x00, sizeof(writeDispRam_tcon));
		writeDispRam_tcon[0] = 0x7C;
		{
			__u8 tmp[] = "30", *pCharIdx;
			pCharIdx = IntegerToAscii(volume, 0, tmp, sizeof(tmp));
			eLIBs_strcat(writeDispRam_tcon, pCharIdx);

			for(volume = 7; volume--;)
			{
				if(writeDispRam_tcon[volume])
				{
					break;
				}

				writeDispRam_tcon[volume] = 0x20;
			}
		}
	}

	__wrn("writeDispRam_tcon=\"%s\"$\n", writeDispRam_tcon);
	{
		__u32 aux;
		aux = ((__u32)(sizeof(writeDispRam_tcon) - 1) << 16) | (0x0009 << 8) | m_eTconPaletteColorMap_red;
		eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDWRITE, aux, writeDispRam_tcon);
		aux = ((__u32)((1 << 0) | (1 << 1))) << 8 | (1 << 1);
		eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDONOFF, aux, 0);
	}
	return 1;
}

void paint_atv_shortcutidx(mmenu_attr_t *mmenu_attr)
{
#if 1
{
//48250
	__s32 tvIdx;
	tvIdx = mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx;

	{
		GUI_MEMDEV_Handle draw_mem = NULL;
		FB fb;

		atv_menu_lyr_del(mmenu_attr->hLyr_sub,
			(1 << m_eAtvMenuLyr_menuList)|(1 << m_eAtvMenuLyr_searchTv)|(1 << m_eAtvMenuLyr_tvSys));
		if(EPDK_FAIL == atv_menu_lyr_create(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_num))	return;
		
		GUI_LyrWinGetFB(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_num], &fb);
		GUI_LyrWinSel(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_num]);
		GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
		draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
		__msg("draw_mem=0x%x\n", draw_mem);
		
		if(draw_mem)
		{
			GUI_MEMDEV_Select(draw_mem);
		}

		{
			GUI_RECT gui_rect;
			char strTmp[32];

			sprintf(strTmp, "%d", tvIdx);
			__msg_jjl("strTmp=%s\n", strTmp);			
			gui_rect.x1 = fb.size.width -1;
			gui_rect.x0 = gui_rect.x1 - W_Num* 5;
			gui_rect.y1 =fb.size.height -(H_NumLock_Num - H_Num) / 2;
			gui_rect.y0 = gui_rect.y1 - H_Num;
			mmenu_attr->paint_flag |= MASK_PaintFlag_RTL;
			GUI_DrawStringEx(strTmp, &gui_rect, mmenu_attr);
		}

		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
			__msg("over draw : draw_mem\n");
		}

		GUI_LyrWinSetTop(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_num]);
	}
}
#else
	// OSD显示频率及搜索到的节目数量
	__u8 writeDispRam_tcon[7], i;
	memset(writeDispRam_tcon, 0x00, sizeof(writeDispRam_tcon));
	{
		// 显示输入的台号
		__u8 tmp[] = "512", *pCharIdx;
		__s32 tvIdx;
		tvIdx = mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx;

		for(i = 0; i < (3 - mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_digits); i++)
		{
			writeDispRam_tcon[i] = '-';
		}

		pCharIdx = IntegerToAscii(tvIdx, 0, tmp, sizeof(tmp));
		eLIBs_strcat(writeDispRam_tcon, pCharIdx);

		for(i = 7; i--;)
		{
			if(writeDispRam_tcon[i])
			{
				break;
			}

			writeDispRam_tcon[i] = 0x20;
		}
	}
	__wrn("writeDispRam_tcon=\"%s\"$\n", writeDispRam_tcon);
	{
		__u32 aux;
		aux = ((__u32)(sizeof(writeDispRam_tcon) - 1) << 16) | (0x0009 << 8) | m_eTconPaletteColorMap_red;
		eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDWRITE, aux, writeDispRam_tcon);
		aux = ((__u32)((1 << 0) | (1 << 1))) << 8 | (1 << 1);
		eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDONOFF, aux, 0);
	}
#endif
}

const __u8 gc_tvMenu_manualSearch[] = "Manual Search";
const __u8 gc_tvMenu_sys[] = "SYS";

const __u8 *gc_tvMenu_system[] =
{
	":PAL-DK",
	":PAL-I",
	":PAL-BG",
	":NTSC-M",
	":SECAM-DK",
	":PAL-M",
	":PAL-N",
	":PAL-60",
	":SECAM-BG",
};

const __u8 gc_tvMenu_manualSearch_sc[] = {(0x80 | (16 + 0)), (0x80 | (16 + 1)), (0x80 | (16 + 2)), (0x80 | (16 + 3)), 0x00};
const __u8 gc_tvMenu_sys_sc[] = {(0x80 | (16 + 4)), (0x80 | (16 + 5)), 0x00};

void paint_atv_tv_menu(mmenu_attr_t *mmenu_attr, E_TV_MENU_IDX a_eTvMenuIdx_focus, __u32 a_eTvMenuIdx_mask)
{
#if 1	
	atv_menu_lyr_del(mmenu_attr->hLyr_sub,
		(1 << m_eAtvMenuLyr_num)|(1 << m_eAtvMenuLyr_searchTv)|(1 << m_eAtvMenuLyr_tvSys));
	atv_menu_lyr_create(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_menuList);

	g_sComMenuInfo_atvSubMenu.itemFocus = a_eTvMenuIdx_focus;
	g_sComMenuInfo_atvSubMenu.maskItemDisable = ~a_eTvMenuIdx_mask;
	com_menu_paint_all_item(&g_sComMenuInfo_atvSubMenu);
#else
	// OSD显示"Manual"
	__u8 	writeDispRam[15], idx, voidItemCnt = 0;
	__u16 	seat = 0x000F;
	__u32	aux;
	__s32	language = dsk_langres_get_type();

	for(idx = 0; idx < m_eTvMenuIdx_max; idx++)
	{
		E_TCON_PALETTE_COLOR_MAP color;

		if(a_eTvMenuIdx_mask & (1 << idx))
		{
			memset(writeDispRam, 0x00, sizeof(writeDispRam));

			if(idx == a_eTvMenuIdx_focus)
			{
				writeDispRam[0] = 0x1D;
				color = m_eTconPaletteColorMap_red;
			}
			else
			{
				writeDispRam[0] = ' ';
				color = m_eTconPaletteColorMap_white;
			}

			switch(idx)
			{
				case m_eTvMenuIdx_manualSearch :
					switch(language)
					{
						case EPDK_LANGUAGE_ENM_CHINESES:
							eLIBs_strcat(writeDispRam, gc_tvMenu_manualSearch_sc);
							break;

						default :
							eLIBs_strcat(writeDispRam, gc_tvMenu_manualSearch);
							break;
					}

					break;

				case m_eTvMenuIdx_system :
					switch(language)
					{
						case EPDK_LANGUAGE_ENM_CHINESES:
							eLIBs_strcat(writeDispRam, gc_tvMenu_sys_sc);
							break;

						default :
							eLIBs_strcat(writeDispRam, gc_tvMenu_sys);
							break;
					}

				{
					E_TV_SYSTEM 	eTvSys;

					if(g_isAVInput)
					{
						eTvSys = mmenu_attr->sTunerInfo.m_eTvSystem_avin;
					}
					else
					{
					#if !SP_ATV_SYS_IndepSetEnable
						if(mmenu_attr->sTunerInfo.m_eTvSystem_tv != m_eTvSystem_Auto)
						{
							eTvSys = mmenu_attr->sTunerInfo.m_eTvSystem_tv; // 实时伴音制式
						}
						else
					#endif
						{
							eTvSys = mmenu_attr->sTunerInfo.m_psTunerAtvList[tvIdx].m_eTvSystem;
						} 
					}

					eLIBs_strcat(writeDispRam, gc_tvMenu_system[eTvSys]);
				}
				
					break;

				default :
					return;
			}

			{
				__u16 i = eLIBs_strlen(writeDispRam);

				for(; i < 14; i++)
				{
					writeDispRam[i] = 0x20;
				}
			}

			aux = ((__u32)(14) << 16) | (seat << 8) | color;
			eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDWRITE, aux, writeDispRam);
			seat += 14;
		}
		else
		{
			// 不显示的菜单项计数
			voidItemCnt++;
		}
	}

	if(voidItemCnt)
	{
		memset(writeDispRam, ' ', sizeof(writeDispRam));

		for(idx = 0; idx < voidItemCnt; idx++)
		{
			// 不显示的菜单项
			aux = ((__u32)(14) << 16) | (seat << 8) | m_eTconPaletteColorMap_white;
			eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDWRITE, aux, writeDispRam);
			seat += 14;
		}
	}

	aux = ((__u32)0x07) << 8 | (1 << 2);
	eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDONOFF, aux, 0);
#endif
}

__bool paint_atv_manualsearch_info(mmenu_attr_t *mmenu_attr)
{
	// OSD显示频率及搜索到的节目数量
	__u32 t_curSearchFreq, t_psTunerAtvList_cnt;
	__bool t_searchOver = 0;

	if(mmenu_attr->sTunerInfo.m_searchFlags_mask & MASK_SearchFlags_Over)
	{
		t_searchOver = 1;
	}

	t_curSearchFreq = mmenu_attr->sTunerInfo.m_curSearchFreq;
	t_psTunerAtvList_cnt = mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt;
	__wrn("paint_atv_manualsearch_info:begin\n");
	{
		__u8 writeDispRam_tcon[16], i;
		memset(writeDispRam_tcon, 0x00, sizeof(writeDispRam_tcon));
		__wrn("m_curSearchFreq=%u, m_psTunerAtvList_cnt=%u\n", t_curSearchFreq, t_psTunerAtvList_cnt);
		{
			// 显示频率
			__u8 tmp[] = "863.25", *pCharIdx;
			pCharIdx = IntegerToAscii(t_curSearchFreq, 6, tmp, sizeof(tmp));
			eLIBs_strcpy(writeDispRam_tcon, pCharIdx);
			eLIBs_strcat(writeDispRam_tcon, "MHz");

			for(i = 9; i--;)
			{
				if(writeDispRam_tcon[i])
				{
					break;
				}

				writeDispRam_tcon[i] = 0x20;
			}
		}
		{
			// 显示台号
			__u8 tmp[] = "512", *pCharIdx;
			__s32 tvIdx;
			tvIdx = t_psTunerAtvList_cnt;
			//if(tvIdx <=0) 	tvIdx = 1;
			pCharIdx = IntegerToAscii(tvIdx, 0, tmp, sizeof(tmp));
			eLIBs_strcat(writeDispRam_tcon, "CH ");
			eLIBs_strcat(writeDispRam_tcon, pCharIdx);

			for(i = 0x0F; i--;)
			{
				if(writeDispRam_tcon[i])
				{
					break;
				}

				writeDispRam_tcon[i] = 0x20;
			}
		}
		__wrn("writeDispRam_tcon=\"%s\"$\n", writeDispRam_tcon);
		{
			__u32 aux;
			aux = ((__u32)(sizeof(writeDispRam_tcon) - 1) << 16) | 0 | m_eTconPaletteColorMap_red;
			eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDWRITE, aux, writeDispRam_tcon);
			aux = ((__u32)((1 << 0) | (1 << 1))) << 8 | 0x03;
			eLIBs_fioctrl(mmenu_attr->sTunerInfo.m_pEsFile_drv, DRV_ATV_CMD_TCON_OSDONOFF, aux, 0);
		}
	}
	return t_searchOver;
}

#define MASK_PaintAtvSearchInfo_CurFreq			(1<<0)
#define MASK_PaintAtvSearchInfo_ListCnt			(1<<1)
#define MASK_PaintAtvSearchInfo_ProcBar			(1<<2)

__bool paint_atv_autosearch_info(mmenu_attr_t *mmenu_attr, __u8 status)
{
	__u8	drawItem_flag = 0;
	static __s32	s_curSearchFreq_bak = -1, s_psTunerAtvList_cnt_bak = -1;
	static __u8	s_numerator_bk = 100;
	__u8	numerator;
	__bool t_searchOver = 0;

	if(mmenu_attr->sTunerInfo.m_searchFlags_mask & MASK_SearchFlags_Over)
	{
		t_searchOver = 1;
	}

	//	__wrn("paint_atv_autosearch_info\n");

	if(!status)
	{
		drawItem_flag = MASK_PaintAtvSearchInfo_ListCnt | MASK_PaintAtvSearchInfo_ProcBar;
		s_curSearchFreq_bak = RANGE_TVFreq_Min;
		s_psTunerAtvList_cnt_bak = 0;
		s_numerator_bk = 0;
		numerator = 0;
	}
	else if(!mmenu_attr->sTunerInfo.m_searchValueValid_flag)
	{
		return 0;
	}
	else
	{
		if(s_curSearchFreq_bak != mmenu_attr->sTunerInfo.m_curSearchFreq)
		{
			s_curSearchFreq_bak = mmenu_attr->sTunerInfo.m_curSearchFreq;
			drawItem_flag |= MASK_PaintAtvSearchInfo_CurFreq;
		}

		if(s_psTunerAtvList_cnt_bak != mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt)
		{
			s_psTunerAtvList_cnt_bak = mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt;
			drawItem_flag |= MASK_PaintAtvSearchInfo_ListCnt;
		}

		{
			__msg("m_pFreqTab_size=%u\n", mmenu_attr->sTunerInfo.m_pFreqTab_size);

			if(!mmenu_attr->sTunerInfo.m_pFreqTab_size)
			{
				numerator = 0;
			}
			else
			{
				numerator =  mmenu_attr->sTunerInfo.m_searchFreqCnt * 100 / mmenu_attr->sTunerInfo.m_pFreqTab_size;
			}

			if(s_numerator_bk != numerator)
			{
				drawItem_flag |= MASK_PaintAtvSearchInfo_ProcBar;
			}
		}
	}

	if(drawItem_flag)
	{
		GUI_MEMDEV_Handle	draw_mem = NULL;
		FB fb;
		__s32	lyrW, lyrH;

    if(GUI_LyrWinGetSta(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_searchTv]) != GUI_LYRWIN_STA_ON)
    {
      atv_menu_lyr_del(mmenu_attr->hLyr_sub,
        (1 << m_eAtvMenuLyr_num)|(1 << m_eAtvMenuLyr_tvInfo)|(1 << m_eAtvMenuLyr_tvSys)|(1 << m_eAtvMenuLyr_menuList));
      if(EPDK_FAIL == atv_menu_lyr_create(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_searchTv))  return 0;
    }
		
		GUI_LyrWinGetFB(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_searchTv], &fb);
		GUI_LyrWinSel(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_searchTv]);
		lyrW = fb.size.width;	lyrH = fb.size.height;
		GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
		draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
		__msg("draw_mem=0x%x\n", draw_mem);

		if(draw_mem)
		{
			GUI_MEMDEV_Select(draw_mem);
		}

		{
			char text[16] = "";
			char *pText = NULL;
			char prtString[32];
			__msg("judge draw : freq\n");

			if(drawItem_flag & MASK_PaintAtvSearchInfo_CurFreq)
			{
				pText = IntegerToAscii(s_curSearchFreq_bak / 1000, 3, text, sizeof(text));
				{
					GUI_RECT gui_rect;
					gui_rect.x0 = (lyrW - W_ProcBar) / 2;
					gui_rect.x1 = gui_rect.x0 + 23 * 6 + 53 + 4 - 1;
					gui_rect.y1 = 	lyrH -H_ProcBar-16 -1;
					gui_rect.y0 = gui_rect.y1 - 32;
					GUI_DrawString(pText, &gui_rect, mmenu_attr);

					if(mmenu_attr->hTheme_MHz)
					{
						void *pbmp;
						pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_MHz);
						GUI_BMP_Draw(pbmp, gui_rect.x0 + 4, gui_rect.y0 + 8);
					}
				}
				__msg("over draw : freq=%u\n", s_curSearchFreq_bak);
			}

			__msg("judge draw : cnt\n");

			if(drawItem_flag & MASK_PaintAtvSearchInfo_ListCnt)
			{
				pText = IntegerToAscii(s_psTunerAtvList_cnt_bak, 0, text, sizeof(text));
				{
					GUI_RECT gui_rect;
					gui_rect.x0 = lyrW - (lyrW - W_ProcBar) / 2 - 23 * 3;
					gui_rect.x1 = gui_rect.x0 + 23 * 3 - 1;
					gui_rect.y1 = 	lyrH -H_ProcBar-16 -1;
					gui_rect.y0 = gui_rect.y1 - 32;
					mmenu_attr->paint_flag |= MASK_PaintFlag_RTL;
					GUI_DrawString(pText, &gui_rect, mmenu_attr);
				}
				__msg("over draw : cnt\n");
			}

			__msg("judge draw : procbar\n");

			if(drawItem_flag & MASK_PaintAtvSearchInfo_ProcBar)
			{
				__wrn("numerator=%u, numerator_bk=%u\n", numerator, s_numerator_bk);
				{
					GUI_RECT gui_rect;
					gui_rect.x0 = (lyrW - W_ProcBar) / 2;	// 水平居中
					gui_rect.y0 = lyrH -H_ProcBar;		// 垂直居底的1/5处
					gui_rect.x1 = gui_rect.x0 + W_ProcBar - 1;
					gui_rect.y1 = gui_rect.y0 + H_ProcBar - 1;

					if(!status)
					{
						__wrn("paint_atv_autosearch_info:status == 0\n");

						if(mmenu_attr->hTheme_procbarBody)
						{
							void *pbmp;
							pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_procbarBody);
							GUI_BMP_Draw(pbmp, gui_rect.x0, gui_rect.y0);
							__wrn("over draw : procbody, x0=%d, y0=%d\n", gui_rect.x0, gui_rect.y0);
						}
					}

					while(s_numerator_bk < numerator)
					{
						// s_numerator_bk : [0, 99]
						s_numerator_bk++;

						if(mmenu_attr->hTheme_procbarPoint)
						{
							void *pbmp;
							pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_procbarPoint);
							GUI_BMP_Draw(pbmp, gui_rect.x0 + W_ProcBarPoint * (s_numerator_bk), gui_rect.y0);
						}
					}

					__msg("over draw : procpoint\n");
				}
				pText = IntegerToAscii(s_numerator_bk, 0, text, sizeof(text));
				{
					GUI_RECT gui_rect;
					gui_rect.x0 = (lyrW - 23 * 4) / 2;
					gui_rect.y0 = 0;
					gui_rect.x1 = gui_rect.x0 + 23 * 4 - 1;
					gui_rect.y1 = gui_rect.y0 + 32 - 1;
					GUI_DrawString(pText, &gui_rect, mmenu_attr);

					if(mmenu_attr->hTheme_percent)
					{
						void *pbmp;
						pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_percent);
						GUI_BMP_Draw(pbmp, gui_rect.x0, gui_rect.y0);
						gui_rect.x0 += 23;
					}
				}
				__msg("over draw : percent\n");
			}
		}

		__msg("judge draw : draw_mem\n");

		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
			__msg("over draw : draw_mem\n");
		}
	}

	GUI_LyrWinSetTop(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_searchTv]);
	return t_searchOver;
}

void paint_atv_audiosys_info(mmenu_attr_t *mmenu_attr, __s8 status)
{
	GUI_MEMDEV_Handle	draw_mem;
	FB fb;

	atv_menu_lyr_del(mmenu_attr->hLyr_sub,
		(1 << m_eAtvMenuLyr_num)|(1 << m_eAtvMenuLyr_searchTv)|(1 << m_eAtvMenuLyr_menuList));
	if(EPDK_FAIL == atv_menu_lyr_create(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_tvSys))	return;
	
	GUI_LyrWinGetFB(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvSys], &fb);
	GUI_LyrWinSel(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvSys]);	
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
	__wrn("draw_mem=0x%x\n", draw_mem);

	if(draw_mem)
	{
		GUI_MEMDEV_Select(draw_mem);
	}

	{
		__s16 tvSysBg_x = 0, tvSysBg_y = 0;
		tvSysBg_x = 0;
		tvSysBg_y = 0;

		if(status == 0)
		{
			if(mmenu_attr->hTheme_tvSys_bg)
			{
				void *pbmp;
				pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_tvSys_bg);
				GUI_BMP_Draw(pbmp, tvSysBg_x, tvSysBg_y);
			}
		}

		{
			__s16	tvSysItem_yEnd;
			__u8	idx, row;
			GUI_RECT gui_rect;
			E_TV_SYSTEM 	eTvSys;
			
			if(g_isAVInput)
			{
				eTvSys = mmenu_attr->sTunerInfo.m_eTvSystem_avin;
			}
			else
			{
			#if !SP_ATV_SYS_IndepSetEnable
				if(mmenu_attr->sTunerInfo.m_eTvSystem_tv != m_eTvSystem_Auto)
				{
					eTvSys = mmenu_attr->sTunerInfo.m_eTvSystem_tv; // 实时伴音制式
				}
				else
			#endif
				{
					eTvSys = mmenu_attr->sTunerInfo.m_psTunerAtvList[mmenu_attr->sTunerInfo.m_psTunerAtvList_idx].m_eTvSystem;
				} 
			}			

			for(row = 0, idx = 0; idx < m_eTvSystem_max; row++)
			{
				gui_rect.x0 = tvSysBg_x + W_TVSYS_FINGER_BMP + (W_TVSYS_ITEM + X_TVSYS_ITEM_Space) * row;
				gui_rect.y0 = tvSysBg_y + H_TVSYS_ITEM + Y_TVSYS_ITEM_Space;
				tvSysItem_yEnd = gui_rect.y0 + H_TVSYS_ITEM + Y_TVSYS_ITEM_Space;

				for(; ((idx < m_eTvSystem_max) && (tvSysItem_yEnd < (tvSysBg_y + H_TVSYS_BG_BMP))); idx++)
				{
					HTHEME		t_hTheme_itemBase = NULL;

					if(idx == eTvSys)
					{
						if(status < 0)
						{
							t_hTheme_itemBase = mmenu_attr->hTheme_audioSys[idx][1];
							{
								// finger clear
								if(mmenu_attr->hTheme_tvSys_fingerClr)
								{
									void *pbmp;
									pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_tvSys_fingerClr);
									GUI_BMP_Draw(pbmp,
									             gui_rect.x0 + W_TVSYS_ITEM,
									             gui_rect.y0 + (H_TVSYS_ITEM - H_TVSYS_FINGER_BMP) / 2);
								}
							}
						}
						else
						{
							t_hTheme_itemBase = mmenu_attr->hTheme_audioSys[idx][0];
							{
								// finger
								if(mmenu_attr->hTheme_tvSys_finger)
								{
									void *pbmp;
									pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_tvSys_finger);
									GUI_BMP_Draw(pbmp,
									             gui_rect.x0 + W_TVSYS_ITEM,
									             gui_rect.y0 + (H_TVSYS_ITEM - H_TVSYS_FINGER_BMP) / 2);
								}
							}
						}
					}
					else if(status == 0)
					{
						t_hTheme_itemBase = mmenu_attr->hTheme_audioSys[idx][1];
					}

					if(t_hTheme_itemBase)
					{
						void *pbmp;
						pbmp = dsk_theme_hdl2buf(t_hTheme_itemBase);
						GUI_BMP_Draw(pbmp, gui_rect.x0, gui_rect.y0);
						t_hTheme_itemBase = NULL;
					}

					gui_rect.y0 += H_TVSYS_ITEM + Y_TVSYS_ITEM_Space;
					tvSysItem_yEnd += H_TVSYS_ITEM + Y_TVSYS_ITEM_Space;
				}
			}
		}
	}

	if(draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(draw_mem);
		draw_mem = NULL;
	}

	GUI_LyrWinSetTop(mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvSys]);
}

static void paint_matv_one_item(mmenu_attr_t *mmenu_attr, __s32 index, __s32 bfocus)
{
	GUI_RECT gui_rect;
	__u32 	i;
	void *pbmp;
	__s32 bmp_width;
	__s32 bmp_height;
	HBMP hbmp;
	__s32 bmp_x;
	__s32 bmp_y;
	atv_uipara_t *atv_ui_para;
	mm_res_id_t *atv_item_para;
	char text[256];
	GUI_MEMDEV_Handle draw_mem = NULL;
	FB fb;
	__s32 width,height;
	__u8 paint_flag = mmenu_attr->paint_flag;
	
	atv_ui_para = atv_get_ui_para(GUI_GetScnDir());
	mmenu_attr->paint_flag &= ~MASK_PaintFlag_MEMDEV_Nonuse;

	if(!atv_ui_para)
	{
		__err("atv_ui_para is null..\n");
		return ;
	}

	atv_item_para = atv_get_item_para(GUI_GetScnDir());

	if(!atv_ui_para)
	{
		__err("atv_ui_para is null...\n");
		return ;
	}

	if(index < 0 || index >= MAX_ITEM_NUM)
	{
		__err("invalid para...\n");
		return ;
	}

	GUI_LyrWinGetFB(mmenu_attr->hLyr_frame, &fb);
	width = fb.size.width;
	height = fb.size.height;
	if(!(paint_flag & MASK_PaintFlag_MEMDEV_Nonuse))
	{
		GUI_LyrWinSel(mmenu_attr->hLyr_frame);
		GUI_SetFont(mmenu_attr->font);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(0);
		GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
		draw_mem = GUI_MEMDEV_Create(0, 0, width, height);
		__wrn("draw_mem=0x%x\n", draw_mem);
	}

	if(draw_mem)
	{
		GUI_MEMDEV_Select(draw_mem);
	}

	if(1)
	{
		if(mmenu_attr->drawTvLogo == 0)
		{
			mmenu_attr->drawTvLogo = 1;
			pbmp = dsk_theme_hdl2buf(mmenu_attr->hTheme_TVLogo);
			GUI_BMP_Draw(pbmp, (width - 300) / 2, height / 8);
		}
	}

	{
		GUI_RECT gui_rect;
		__u16 xOft, yOft;
		xOft = (W_ITEM_FOCUS - ITEM_W) / 2;
		yOft = (H_ITEM_FOCUS - ITEM_H);
		bmp_x = atv_ui_para->item_pos[index].x;
		//bmp_y = atv_ui_para->item_pos[index].y;
		bmp_y = SCN_H - 20 - ITEM_H - atv_ui_para->item_pos[0].x * 618 / 1000;
		gui_rect.x0 = bmp_x - 16;
		gui_rect.y0 = bmp_y + ITEM_H + 4;
		gui_rect.x1 = gui_rect.x0 + ITEM_W - 1 + 32;
		gui_rect.y1 = gui_rect.y0 + 24 - 1;

		if(bfocus)//选中图标
		{
			GUI_SetColor(mmenu_attr->focus_txt_color);
			pbmp = dsk_theme_hdl2buf(mmenu_attr->focus_bmp[index]);
			bmp_x -= xOft;
			bmp_y -= yOft;
		}
		else//未选中图标
		{
			GUI_SetColor(mmenu_attr->unfocus_txt_color);
			pbmp = dsk_theme_hdl2buf(mmenu_attr->unfocus_bmp[index]);
			GUI_ClearRect(bmp_x - yOft, bmp_y - yOft, bmp_x + W_ITEM_FOCUS - 1, bmp_y + H_ITEM_FOCUS - 1);
		}

		GUI_BMP_Draw(pbmp, bmp_x, bmp_y);
		{
			dsk_langres_get_menu_text(atv_item_para[index].lang_id, text, sizeof(text));
			GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_TOP);
		}
	}

	if(draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(draw_mem);
		draw_mem = NULL;
	}

	GUI_LyrWinSetTop(mmenu_attr->hLyr_frame);
}

/*
	显示全部ITEM
*/

static void paint_matv_item_all(mmenu_attr_t *mmenu_attr)
{
	__s32 i;
	GUI_MEMDEV_Handle draw_mem;
	FB fb;

	GUI_LyrWinSel(mmenu_attr->hLyr_frame);
	GUI_SetFont(mmenu_attr->font);
	GUI_UC_SetEncodeUTF8();
	GUI_SetBkColor(0);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	
	GUI_LyrWinGetFB(mmenu_attr->hLyr_frame, &fb);
	draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
	__wrn("draw_mem=0x%x\n", draw_mem);

	if(draw_mem)
	{
		GUI_MEMDEV_Select(draw_mem);
	}

	for(i = 0; i < MAX_ITEM_NUM; i++)
	{
		if(i == mmenu_attr->focus_item)
		{
			mmenu_attr->paint_flag |= MASK_PaintFlag_MEMDEV_Nonuse;
			paint_matv_one_item(mmenu_attr, i, 1);
		}
		else
		{
			mmenu_attr->paint_flag |= MASK_PaintFlag_MEMDEV_Nonuse;
			paint_matv_one_item(mmenu_attr, i, 0);
		}
	}

	if(draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(draw_mem);
		draw_mem = NULL;
	}
}

void atv_submenu_on(mmenu_attr_t *mmenu_attr, __gui_msg_t *msg)
{
	if(!mmenu_attr)
	{
		return;
	}

	switch(mmenu_attr->focus_item)
	{
		case IDX_ATV_MENU_TV_AVIN :
		{
			GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);

			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_2 :
				{
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

					if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
					{
						GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
					}

#endif
					g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
					paint_atv_tv_menu(mmenu_attr, m_eTvMenuIdx_system, MASK_PaintTVMenu_AVIN);
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S

					if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
					{
						GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
					}
					else
					{
						GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
					}

#endif
				}
				break;

				case m_eMenuDepth_3 :
				{
					switch(g_atvSubMenuIdx)
					{
						case m_eTvMenuIdx_system:
						{
		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD
		
							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
							}
		
		#endif
							//mmenu_attr->sTunerInfo.m_tunerMode = MODE_Tuner_Setting;
							paint_atv_audiosys_info(mmenu_attr, 0);

		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
		
							if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
							{
								GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
							}
							else
							{
								GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
							}
		
		#endif
							break;
						}
						
						default:	break;
					}
					break;
				}
				
				default :
					break;
			}
		}
		break;

		case IDX_ATV_MENU_TV_PLAY :
		{
			GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);

			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_2 :
				{
				#if 0
					if(g_tvSoundChannelChange)
					{
						paint_atv_tv_menu(mmenu_attr, m_eTvMenuIdx_system, (1 << m_eTvMenuIdx_system));
					}
					else
				#endif
					{
						paint_atv_tv_menu(mmenu_attr, g_atvSubMenuIdx, MASK_PaintTVMenu_TV);
					}

#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S

					if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
					{
						GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
					}
					else
					{
						GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
					}

#endif
				}
				break;

				case m_eMenuDepth_3 :
				{
					switch(g_atvSubMenuIdx)
					{
						case m_eTvMenuIdx_system:
						{
							//mmenu_attr->sTunerInfo.m_tunerMode = MODE_Tuner_Setting;
							paint_atv_audiosys_info(mmenu_attr, 0);

		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
		
							if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
							{
								GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
							}
							else
							{
								GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S, NULL);
							}
		
		#endif
							break;
						}

						case m_eTvMenuIdx_autoSearch:
						{
							__u32 i;
		#if TIMEOUT_ID_TIMER_ATV_SEARCH

							if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
							{
								GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH, TIMEOUT_ID_TIMER_ATV_SEARCH, NULL);
							}

		#endif
							paint_atv_autosearch_info(mmenu_attr, 0);
		#if SP_SaveTVShortcutIdx

							for(i = 0; i < SIZE_sTunerAtvList_shortcutIdx; i++)
							{
								mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab[i] = IDX_sTunerAtvList_invalid;
							}

							mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_ShortcutIdx;
		#endif
							g_disable_close_scn();
							break;
						}
						
						case m_eTvMenuIdx_manualSearch:
						{
							__u32 i;
		#if TIMEOUT_ID_TIMER_ATV_SEARCH
		
							if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
							{
								GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH, TIMEOUT_ID_TIMER_ATV_SEARCH, NULL);
							}
		
		#endif
		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD
		
							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
							}
		
		#endif
		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
		
							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S);
							}
		
		#endif
							g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
							paint_atv_manualsearch_info(mmenu_attr);
							break;
						}
						
						default:	break;
					}
					break;
				}

				default :
					break;
			}
		}
		break;

		case IDX_ATV_MENU_TV_SEARCH:
		{
			GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);

			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_1 :
				{
					__u32 i;
#if TIMEOUT_ID_TIMER_ATV_SEARCH

					if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
					{
						GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH, TIMEOUT_ID_TIMER_ATV_SEARCH, NULL);
					}

#endif
					paint_atv_autosearch_info(mmenu_attr, 0);
#if SP_SaveTVShortcutIdx

					for(i = 0; i < SIZE_sTunerAtvList_shortcutIdx; i++)
					{
						mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab[i] = IDX_sTunerAtvList_invalid;
					}

					mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_ShortcutIdx;
#endif
					g_disable_close_scn();
				}
				break;

				default :
					break;
			}
		}
		break;

		case IDX_ATV_MENU_TV_SETTING:
		{
			GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);

			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_1 :
				{
					mmenu_attr->sTunerInfo.m_tunerMode = MODE_Tuner_Setting;
					paint_atv_audiosys_info(mmenu_attr, 0);
				}
				break;

				default :
					break;
			}
		}
		break;

		default :
			break;
	}
}

void atv_submenu_sleep(mmenu_attr_t *mmenu_attr, __gui_msg_t *msg)
{
	if(!mmenu_attr)
	{
		return;
	}

	//GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_ON);

	switch(mmenu_attr->focus_item)
	{
		case IDX_ATV_MENU_TV_AVIN :
		{
			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_2 :
				{
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S

					if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
					{
						GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S);
					}

#endif
					//TCON_OSDFrameOff(&mmenu_attr->sTunerInfo, 1 << 2);
					atv_menu_lyr_del(mmenu_attr->hLyr_sub,
						1<<m_eAtvMenuLyr_menuList);
					//atv_save_sys_para(mmenu_attr);
					break;
				}
				
				case m_eMenuDepth_3 :
				{
					switch(g_atvSubMenuIdx)
					{
						case m_eTvMenuIdx_system:
						{
		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
		
							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S);
							}
		
		#endif
							//atv_save_sys_para(mmenu_attr);
							break;
						}
					}
					break;
				}

				default :
					break;
			}
		}
		break;

		case IDX_ATV_MENU_TV_PLAY :
		{
			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_2 :
				{
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S

					if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
					{
						GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S);
					}

#endif
					atv_menu_lyr_del(mmenu_attr->hLyr_sub,
						1<<m_eAtvMenuLyr_menuList);
					break;
				}

				case m_eMenuDepth_3 :
				{
					switch(g_atvSubMenuIdx)
					{
						case m_eTvMenuIdx_system:
						{
		#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
		
							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S);
							}
		
		#endif
							//atv_save_sys_para(mmenu_attr);
							break;
						}
						case m_eTvMenuIdx_autoSearch:
						case m_eTvMenuIdx_manualSearch:
						{
		#if TIMEOUT_ID_TIMER_ATV_SEARCH
		
							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH);
							}
		
		#endif
							TUNER_ScanTVFreq_Stop(&mmenu_attr->sTunerInfo);
							//TCON_OSDFrameOff(&mmenu_attr->sTunerInfo, (1 << 2) | (1 << 1) | (1 << 0));
							atv_menu_lyr_del(mmenu_attr->hLyr_sub,
								(1<<m_eAtvMenuLyr_searchTv)|(1<<m_eAtvMenuLyr_tvSys)|(1<<m_eAtvMenuLyr_num));
							//atv_save_sys_para(mmenu_attr);
							mmenu_attr->sTunerInfo.m_searchValueValid_flag = 0;
							break;
						}
						default:	break;
					}
				}
				break;

				default :
					break;
			}
		}
		break;

		case IDX_ATV_MENU_TV_SEARCH:
		{
			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_1 :
				{
#if TIMEOUT_ID_TIMER_ATV_SEARCH

					if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
					{
						GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH);
					}

#endif
					TUNER_ScanTVFreq_Stop(&mmenu_attr->sTunerInfo);
					//atv_save_sys_para(mmenu_attr);
					mmenu_attr->sTunerInfo.m_searchValueValid_flag = 0;
					g_enable_close_scn();
				}
				break;

				default :
					break;
			}
		}
		break;

		case IDX_ATV_MENU_TV_SETTING:
		{
			switch(mmenu_attr->menuDepth)
			{
				case m_eMenuDepth_1 :
				{
					//atv_save_sys_para(mmenu_attr);
					mmenu_attr->sTunerInfo.m_tunerMode = MODE_Tuner_Idle;
				}
				break;

				default :
					break;
			}
		}
		break;

		default :
			break;
	}

	if(mmenu_attr->menuDepth == m_eMenuDepth_1)
	{
		//GUI_LyrWinSetTop(mmenu_attr->hLyr_frame);
	}
}

void atv_submenu_on_volume(mmenu_attr_t *mmenu_attr, __bool mute, __gui_msg_t *msg)
{
	if(paint_atv_volume(mmenu_attr, mute))
	{
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

		if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
		{
			GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
		}
		else
		{
			GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
		}

#endif
		g_clearCueOsd_idx = IDX_ClearCueOsd_Volume;
	}
}

void atv_submenu_off_volume(mmenu_attr_t *mmenu_attr, __gui_msg_t *msg)
{
	if(g_clearCueOsd_idx != IDX_ClearCueOsd_Volume)
	{
		return;
	}

#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

	if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
	{
		GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
	}

#endif
	g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
	TCON_OSDFrameOff(&mmenu_attr->sTunerInfo, (1 << 0) | (1 << 1));
}

void atv_clear_cue_osd(mmenu_attr_t *mmenu_attr, __gui_msg_t *msg)
{
	if(g_clearCueOsd_idx == IDX_ClearCueOsd_Invalid)
	{
		return;
	}

#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

	if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
	{
		GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
	}

#endif

	switch(g_clearCueOsd_idx)
	{
		case IDX_ClearCueOsd_PlayCue :
		case IDX_ClearCueOsd_Volume:
		{
			//TCON_OSDFrameOff(&mmenu_attr->sTunerInfo, (1 << 0) | (1 << 1));
			atv_menu_lyr_del(mmenu_attr->hLyr_sub,
				(1<<m_eAtvMenuLyr_tvInfo));
			break;
		}

		case IDX_ClearCueOsd_ShortcutIdx:
		{
			//TCON_OSDFrameOff(&mmenu_attr->sTunerInfo, (1 << 0) | (1 << 1));
			atv_menu_lyr_del(mmenu_attr->hLyr_sub,
				(1<<m_eAtvMenuLyr_searchTv)|(1<<m_eAtvMenuLyr_tvSys)|(1<<m_eAtvMenuLyr_num)|(1<<m_eAtvMenuLyr_menuList));

			if(
			  (mmenu_attr->sTunerInfo.m_psTunerAtvList_idx != IDX_sTunerAtvList_invalid) &&
			  (mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx <= mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt) &&
			  (mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx > 0)
			)
			{
				// 播放电台
				mmenu_attr->sTunerInfo.m_psTunerAtvList_idx = mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx - 1;
				atv_enter_tv(mmenu_attr, msg);
				return;
			}
			else if(((RANGE_TVFreq_Min/10000) <= mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx)
				&& (mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx <= (RANGE_TVFreq_Max/10000)))
			{
				mmenu_attr->sTunerInfo.tmpFreq = mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx *10000;
				atv_enter_tv(mmenu_attr, msg);
				mmenu_attr->sTunerInfo.tmpFreq = 0;	// 清掉临时频道
				return;
			}
			break;
		}

		default :
			break;
	}

	g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
}

__s32 mmenu_proc_key_enter(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__wrn("mmenu_key_proc:GUI_MSG_KEY_ENTER, subMenuDepth=%d, focus_item=%d\n",
		      mmenu_attr->menuDepth, mmenu_attr->focus_item);

		switch(mmenu_attr->focus_item)
		{
			case IDX_ATV_MENU_TV_PLAY :
			{
				switch(mmenu_attr->menuDepth)
				{
					case m_eMenuDepth_0 :
					{
						atv_enter_tv(mmenu_attr, msg);

						if(mmenu_attr->sTunerInfo.m_tunerMode == MODE_Tuner_TV)
						{
							mmenu_attr->menuDepth++;
						}
					}
					break;

					case m_eMenuDepth_1 :
					{
						if(g_clearCueOsd_idx == IDX_ClearCueOsd_ShortcutIdx)
						{
							atv_clear_cue_osd(mmenu_attr, msg);
						}
					}
					break;

					case m_eMenuDepth_2 :
					{
						switch(g_atvSubMenuIdx)
						{
							case m_eTvMenuIdx_system :
							{
								atv_submenu_sleep(mmenu_attr, msg);
								mmenu_attr->menuDepth++;
								atv_submenu_on(mmenu_attr, msg);	//显示菜单
								break;
							}

							case m_eTvMenuIdx_autoSearch:
							{
								if(TUNER_ScanTVFreq_Start(&mmenu_attr->sTunerInfo, 0) != -1)
								{
									atv_submenu_sleep(mmenu_attr, msg);
									mmenu_attr->menuDepth++;
									atv_submenu_on(mmenu_attr, msg);
								}
								break;
							}

							case m_eTvMenuIdx_manualSearch :
							{
								// 手动搜台，搜到一个台追加在节目表后面，返回播放最后搜到的电视台.
								__s32 result;
								result = TUNER_ScanTVFreq_Start(&mmenu_attr->sTunerInfo, 1);

								if(result == 1)
								{
									result = TUNER_ScanTVFreq_Resume(&mmenu_attr->sTunerInfo, 0);
								}

								if(result == 0)
								{
									mmenu_attr->menuDepth++;
									atv_submenu_on(mmenu_attr, msg);
								}
								break;
							}

							default :
								break;
						}
						break;
					}

					case m_eMenuDepth_3:
					{
						switch(g_atvSubMenuIdx)
						{
							case m_eTvMenuIdx_system:
							{
								atv_submenu_sleep(mmenu_attr, msg);
								mmenu_attr->menuDepth--;
								atv_submenu_on(mmenu_attr, msg);
								break;
							}
							default:	break;
						}
						break;
					}

					default :
						break;
				}

				break;
			}

			case IDX_ATV_MENU_TV_SEARCH :
			{
				switch(mmenu_attr->menuDepth)
				{
					case m_eMenuDepth_0 :
						if(TUNER_ScanTVFreq_Start(&mmenu_attr->sTunerInfo, 0) != -1)
						{
							mmenu_attr->menuDepth++;
							atv_submenu_on(mmenu_attr, msg);
						}

						break;

					case m_eMenuDepth_1 :
						if(TUNER_ScanTVFreq_Start(&mmenu_attr->sTunerInfo, 0) == 0)
						{
							atv_submenu_on(mmenu_attr, msg);
						}

						break;

					default :
						break;
				}

				break;
			}

			case IDX_ATV_MENU_TV_SETTING :
			{
				switch(mmenu_attr->menuDepth)
				{
					case m_eMenuDepth_0 :
						mmenu_attr->menuDepth++;
						atv_submenu_on(mmenu_attr, msg);
						break;

					case m_eMenuDepth_1 :
						atv_submenu_sleep(mmenu_attr, msg);
						mmenu_attr->menuDepth--;
						break;

					default :
						break;
				}

				break;
			}

			case IDX_ATV_MENU_TV_AVIN :
			{
				switch(mmenu_attr->menuDepth)
				{
					case m_eMenuDepth_0 :
					{
						mmenu_para_t *mmenu_para;
						mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);

					#if SP_AUX_DET
						if((!IOCTRL__AUX_DET__IS_PLUGIN())
							&& ((mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) != OP_RootFlags_CVBS_InputMode_DVDFromHome)
						#if (SP_AUX_DET == OP_AUX_DET_FOR_AVIN_LINEIN) || (SP_AUX_DET == OP_AUX_DET_FOR_AVIN)
							&& ((mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) != OP_RootFlags_CVBS_InputMode_AVINFromDetect)
						#endif
							)
						{
							H_WIN dlg;
							__s32 lang_id[] = {STRING_ROOT_CUE, STRING_ATV_INSERT_AVIN};
							default_dialog(dlg, mmenu_attr->h_parent, APP_ATV_DLG_ID, ADLG_OK, lang_id);
							esKRNL_TimeDly(120);
							app_dialog_destroy(dlg);
							gscene_bgd_set_bottom();
							__msg_jjl("aux isn't plugin\n");
						}
					#endif

					#if (SP_AUX_DET == OP_AUX_DET_FOR_AVIN_LINEIN)
						{
							if((mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode) == OP_RootFlags_CVBS_InputMode_AVINFromDetect)
							{
								__s32 ret = 0;
								
								if(IOCTRL__AUX_DET__IS_PLUGIN())
								{
									ret = TCON_CVBSDetect(TCON_INPUTCHANNEL_AVIN, &mmenu_attr->sTunerInfo);
									__msg_jjl("TCON_CVBSDetect:%d\n", ret);
								}
								
								if(!ret)
								{
									// 没有CVBS信号
									root_para_t *root_para;
									__gui_msg_t parent_msg;
									root_para = (root_para_t *)GUI_WinGetAttr(mmenu_para->h_parent);
									parent_msg.id = GUI_MSG_COMMAND;
									parent_msg.h_srcwin = msg->h_deswin;
									parent_msg.h_deswin = root_para->h_parent;
									parent_msg.dwAddData1 = MAKELONG(APP_ATV_ID, SWITCH_TO_LINE_IN);
									parent_msg.dwAddData2 = 0;
									GUI_SendMessage(&parent_msg);
									break;
								}
							}
						}
					#endif

						{
							__gui_msg_t t_msg = *msg;
							mmenu_attr->menuDepth++;
							
							switch(mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
							{
								case OP_RootFlags_CVBS_InputMode_DVDFromHome:
								{
									msg->dwAddData2 = mmenu_attr->hLyr_frame;
									SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
									TCON_OccupyLcmi(CVBS_SRC__DVD, &mmenu_attr->sTunerInfo, msg);
									*msg = t_msg;
									//paint_atv_type_info(msg, m_eAtvTypeIdx_dvd);
									break;
								}
								case OP_RootFlags_CVBS_InputMode_AVINFromHome:
								case OP_RootFlags_CVBS_InputMode_AVINFromDetect:
								default:
								{
									msg->dwAddData2 = mmenu_attr->hLyr_frame;
									TCON_OccupyLcmi(CVBS_SRC__AVIN, &mmenu_attr->sTunerInfo, msg);
									*msg = t_msg;
									paint_atv_type_info(msg, m_eAtvTypeIdx_avin);
									break;
								}
							}							
						}
					}
					break;

					case m_eMenuDepth_2 :
					{
						switch(g_atvSubMenuIdx)
						{
							case m_eTvMenuIdx_system :
							{
								atv_submenu_sleep(mmenu_attr, msg);
								mmenu_attr->menuDepth++;
								atv_submenu_on(mmenu_attr, msg);
								break;
							}

							default :
								break;
						}
					
						break;
					}

					case m_eMenuDepth_3:
					{
						switch(g_atvSubMenuIdx)
						{
							case m_eTvMenuIdx_system:
							{
								atv_submenu_sleep(mmenu_attr, msg);
								mmenu_attr->menuDepth--;
								atv_submenu_on(mmenu_attr, msg);
								break;
							}
							default:	break;
						}
						break;
					}

					default :
						break;
				}

				break;
			}

			default :
				break;
		}
	}

	return EPDK_OK;
}

__s32 mmenu_proc_key_return(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	{
		__s32 ret = EPDK_FAIL;
		switch(mmenu_attr->menuDepth)
		{
			case m_eMenuDepth_0 :
			{
				__gui_msg_t parent_msg;
				parent_msg.id = GUI_MSG_CLOSE;
				parent_msg.h_srcwin = msg->h_deswin;
				parent_msg.h_deswin = msg->h_srcwin;
				parent_msg.dwAddData1 = 0;
				parent_msg.dwAddData2 = 0;
				GUI_SendNotifyMessage(&parent_msg);
				ret = EPDK_OK;
				break;
			}

			case m_eMenuDepth_1:
			{
				switch(mmenu_attr->focus_item)
				{
					case IDX_ATV_MENU_TV_PLAY :
					{
						mmenu_para_t *mmenu_para;
						mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
	#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

						if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
						{
							GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
						}

	#endif
	#if TIMEOUT_ID_TIMER_ATV_PROCVTRC

						if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_PROCVTRC))
						{
							GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_PROCVTRC);
						}

	#endif
						g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
						ret = EPDK_OK;
						
						atv_menu_lyr_del(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_total);
						if(mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
						{
							GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);
							TUNER_ExitTV(&mmenu_attr->sTunerInfo, msg);
							{
								__gui_msg_t parent_msg;
								parent_msg.id = GUI_MSG_CLOSE;
								parent_msg.h_srcwin = msg->h_deswin;
								parent_msg.h_deswin = msg->h_srcwin;
								parent_msg.dwAddData1 = 0;
								parent_msg.dwAddData2 = 0;
								GUI_SendMessage(&parent_msg);
							}
							break;
						}

						//paint_matv_item_all(mmenu_attr);
						TUNER_ExitTV(&mmenu_attr->sTunerInfo, msg);
						atv_submenu_sleep(mmenu_attr, msg); 	// 因为可能是从ATV的其它任何界面进入TV播放
						atv_save_sys_para(mmenu_attr);
						mmenu_attr->menuDepth--;
						mmenu_attr->m_closeScreenDone_tcon = 0;
						break;
					}

					case IDX_ATV_MENU_TV_SEARCH :
					{
						atv_submenu_sleep(mmenu_attr, msg);
						mmenu_attr->menuDepth--;
						ret = EPDK_OK;
						break;
					}

					case IDX_ATV_MENU_TV_SETTING :
					{
						atv_submenu_sleep(mmenu_attr, msg);
						mmenu_attr->menuDepth--;
						ret = EPDK_OK;
						break;
					}

					case IDX_ATV_MENU_TV_AVIN :
					{
						mmenu_para_t *mmenu_para;
						mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);

						ret = EPDK_OK;
						if(mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
						{
							atv_menu_lyr_del(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_total);
							GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);
							TCON_ReleaseLcmi(&mmenu_attr->sTunerInfo, msg);
							g_isAVInput = 0;
							{
								__gui_msg_t parent_msg;
								parent_msg.id = GUI_MSG_CLOSE;
								parent_msg.h_srcwin = msg->h_deswin;
								parent_msg.h_deswin = msg->h_srcwin;
								parent_msg.dwAddData1 = 0;
								parent_msg.dwAddData2 = 0;
								GUI_SendMessage(&parent_msg);
							}
							break;
						}
						
						atv_submenu_sleep(mmenu_attr, msg);
						TCON_ReleaseLcmi(&mmenu_attr->sTunerInfo, msg);
						g_isAVInput = 0;
						mmenu_attr->menuDepth--;
						mmenu_attr->m_closeScreenDone_tcon = 0;
						break;
					}

					default :
						break;
				}
				break;
			}

			case m_eMenuDepth_2 :
			{
				switch(mmenu_attr->focus_item)
				{
					case IDX_ATV_MENU_TV_PLAY :
					{
						atv_submenu_sleep(mmenu_attr, msg);

						if(mmenu_attr->sTunerInfo.m_tunerMode != MODE_Tuner_TV)
						{
							atv_enter_tv(mmenu_attr, msg);
						}

						mmenu_attr->menuDepth--;
						ret = EPDK_OK;
						break;
					}

					case IDX_ATV_MENU_TV_AVIN :
					{
						atv_submenu_sleep(mmenu_attr, msg);
						mmenu_attr->menuDepth--;
						ret = EPDK_OK;
						break;
					}

					default :
						break;
				}
				break;
			}

			case m_eMenuDepth_3 :
			{
				switch(mmenu_attr->focus_item)
				{
					case IDX_ATV_MENU_TV_PLAY:
					case IDX_ATV_MENU_TV_AVIN:
					{
						atv_submenu_sleep(mmenu_attr, msg);
						mmenu_attr->menuDepth--;
						
						switch(g_atvSubMenuIdx)
						{
							case m_eTvMenuIdx_system:
							case m_eTvMenuIdx_autoSearch:
							{
								atv_submenu_on(mmenu_attr, msg);
								if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_AVIN)
								{
									break;
								}
							}
							case m_eTvMenuIdx_manualSearch:
							{
								if(mmenu_attr->sTunerInfo.m_tunerMode != MODE_Tuner_TV)
								{
									atv_enter_tv(mmenu_attr, msg);
								}
								break;
							}
							default:	break;
						}

						ret = EPDK_OK;
						break;
					}

					default :
						break;
				}
				break;
			}

			default :
				break;
		}

		if((ret == EPDK_FAIL) && (mmenu_attr->menuDepth > m_eMenuDepth_0))
		{
			mmenu_attr->menuDepth--;
		}
	}

	return EPDK_OK;
}

__s32 mmenu_proc_key_num(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	{
	#if SP_SaveTVShortcutIdx
		//if((mmenu_attr->menuDepth == m_eMenuDepth_1)&&(mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY))
		{
			if(
				(mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab == NULL)
				|| (mmenu_attr->sTunerInfo.m_psTunerAtvList_idx == IDX_sTunerAtvList_invalid)
				|| (mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab[msg->dwAddData1 - GUI_MSG_KEY_NUM0] == IDX_sTunerAtvList_invalid)
			)
			{
				break;
			}

			mmenu_attr->sTunerInfo.m_psTunerAtvList_idx =
			mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab[msg->dwAddData1 - GUI_MSG_KEY_NUM0];
			atv_enter_tv(mmenu_attr, msg);

			if(mmenu_attr->sTunerInfo.m_tunerMode == MODE_Tuner_TV)
			{
				mmenu_attr->focus_item = IDX_ATV_MENU_TV_PLAY;
				mmenu_attr->menuDepth = m_eMenuDepth_1;
			}
		}
	#else
	#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

		if((mmenu_attr->menuDepth == m_eMenuDepth_1) && (mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY))
		{
			__bool	refresh = 0;

			if(g_clearCueOsd_idx != IDX_ClearCueOsd_ShortcutIdx)
			{
				atv_clear_cue_osd(mmenu_attr, msg);
				g_clearCueOsd_idx = IDX_ClearCueOsd_ShortcutIdx;
				// 进入输入台号模式
				if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
				{
					GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
				}
				else
				{
					GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);
				}

				mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx = msg->dwAddData1 - GUI_MSG_KEY_NUM0;
				mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_digits = 1;
				refresh = 1;
			}
			else
			{
				if(mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_digits < 5)
				{
					GUI_ResetTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD, TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD, NULL);

					if(mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx > 0)
					{
						mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_digits++;
						mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx *= 10;
					}

					mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx += msg->dwAddData1 - GUI_MSG_KEY_NUM0;
					refresh = 1;
				}
			}

			__wrn("shortcutIdx=%d\n", mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx);

			if(refresh)
			{
				paint_atv_shortcutidx(mmenu_attr);	// 显示输入的台号
			}
		}

	#endif
	#endif
	}

	return EPDK_OK;
}

__s32 mmenu_proc_key_left(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	switch(mmenu_attr->menuDepth)
	{
		case m_eMenuDepth_0 :
		{
			paint_matv_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
			mmenu_attr->focus_item--;
	
			if(mmenu_attr->focus_item < 0)
			{
				mmenu_attr->focus_item = MAX_ITEM_NUM - 1;
			}
	
			paint_matv_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
			break;
		}
	
		case m_eMenuDepth_1 :
		{
			switch(mmenu_attr->focus_item)
			{
				case IDX_ATV_MENU_TV_PLAY :
					if(mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt > 1)
					{
						mmenu_attr->sTunerInfo.m_psTunerAtvList_idx--;
	
						if(mmenu_attr->sTunerInfo.m_psTunerAtvList_idx < 0)
						{
							mmenu_attr->sTunerInfo.m_psTunerAtvList_idx = mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt - 1;
						}
	
						atv_enter_tv(mmenu_attr, msg);
						break;
					}
	
				case IDX_ATV_MENU_TV_SEARCH:
					break;
	
				case IDX_ATV_MENU_TV_SETTING:
				{
					paint_atv_audiosys_info(mmenu_attr, -1);
				
					if(mmenu_attr->sTunerInfo.m_eTvSystem_tv == 0)
					{
						mmenu_attr->sTunerInfo.m_eTvSystem_tv = m_eTvSystem_max - 1;
					}
					else
					{
						mmenu_attr->sTunerInfo.m_eTvSystem_tv--;
					}
					mmenu_attr->sTunerInfo.m_eTvSystem_avin = mmenu_attr->sTunerInfo.m_eTvSystem_tv;	
					mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_TvSys | MASK_SaveTunerInfo_AvinSys;
				
					paint_atv_audiosys_info(mmenu_attr, 1);
					break;
				}
	
				default :
					break;
			}
			break;
		}
	
		case m_eMenuDepth_2 :
		{
			switch(mmenu_attr->focus_item)
			{
				case IDX_ATV_MENU_TV_PLAY :
				{
					if(g_atvSubMenuIdx == 0)
					{
						g_atvSubMenuIdx = m_eTvMenuIdx_max - 1;
					}
					else
					{
						g_atvSubMenuIdx--;
					}
	
					atv_submenu_on(mmenu_attr, msg);
				}
				break;
	
				default :
					break;
			}
			break;
		}
	
		case m_eMenuDepth_3:
		{
			switch(mmenu_attr->focus_item)
			{
				case IDX_ATV_MENU_TV_PLAY :
				case IDX_ATV_MENU_TV_AVIN:
				{
					switch(g_atvSubMenuIdx)
					{
						case m_eTvMenuIdx_system:
						{
							E_TV_SYSTEM *peTvSys = NULL;
							//paint_atv_audiosys_info(mmenu_attr, -1);

							if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY)
							{
							#if !SP_ATV_SYS_IndepSetEnable
								if(mmenu_attr->sTunerInfo.m_eTvSystem_tv != m_eTvSystem_Auto)
								{
									peTvSys = &mmenu_attr->sTunerInfo.m_eTvSystem_tv; // 实时伴音制式
								}
								else
							#endif
								{
									peTvSys = &mmenu_attr->sTunerInfo.m_psTunerAtvList[mmenu_attr->sTunerInfo.m_psTunerAtvList_idx].m_eTvSystem;
								} 
								mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_TvSys;
							}
							else if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_AVIN)
							{
								peTvSys = &mmenu_attr->sTunerInfo.m_eTvSystem_avin;								
								mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_AvinSys;
							}

							if(peTvSys)
							{
								if(*peTvSys == 0)
								{
									*peTvSys = m_eTvSystem_max - 1;
								}
								else
								{
									(*peTvSys)--;
								}
							}						
						
							//paint_atv_audiosys_info(mmenu_attr, 1);

							com_video_in_close();
							switch(mmenu_attr->focus_item)
							{
								case IDX_ATV_MENU_TV_PLAY:
								{
									atv_enter_tv(mmenu_attr, msg);
									atv_submenu_on(mmenu_attr, msg);
									break;
								}
								case IDX_ATV_MENU_TV_AVIN:
								{
									__gui_msg_t t_msg = *msg;
									msg->dwAddData2 = mmenu_attr->hLyr_frame;
									TCON_OccupyLcmi(CVBS_SRC__AVIN, &mmenu_attr->sTunerInfo, msg);
									*msg = t_msg;
									atv_submenu_on(mmenu_attr, msg);
									break;
								}
								default:	break;
							}							
							break;
						}
						default:	break;
					}
					break;
				}
				default:	break;
			}
			break;
		}
	
		default :
			break;
	}	

	return EPDK_OK;
}

__s32 mmenu_proc_key_right(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	switch(mmenu_attr->menuDepth)
	{
		case m_eMenuDepth_0 :
		{
			paint_matv_one_item(mmenu_attr, mmenu_attr->focus_item, 0);
			mmenu_attr->focus_item++;
	
			if(mmenu_attr->focus_item >= MAX_ITEM_NUM)
			{
				mmenu_attr->focus_item = 0;
			}
	
			paint_matv_one_item(mmenu_attr, mmenu_attr->focus_item, 1);
		}
		break;
	
		case m_eMenuDepth_1 :
		{
			switch(mmenu_attr->focus_item)
			{
				case IDX_ATV_MENU_TV_PLAY :
					if(mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt > 1)
					{
						mmenu_attr->sTunerInfo.m_psTunerAtvList_idx++;
	
						if(mmenu_attr->sTunerInfo.m_psTunerAtvList_idx >= mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt)
						{
							mmenu_attr->sTunerInfo.m_psTunerAtvList_idx = 0;
						}
	
						atv_enter_tv(mmenu_attr, msg);
					}
	
					break;
	
				case IDX_ATV_MENU_TV_SEARCH:
					break;
	
				case IDX_ATV_MENU_TV_SETTING:
				{
					paint_atv_audiosys_info(mmenu_attr, -1);
				
					if(mmenu_attr->sTunerInfo.m_eTvSystem_tv == 0)
					{
						mmenu_attr->sTunerInfo.m_eTvSystem_tv = m_eTvSystem_max - 1;
					}
					else
					{
						mmenu_attr->sTunerInfo.m_eTvSystem_tv--;
					}
					mmenu_attr->sTunerInfo.m_eTvSystem_avin = mmenu_attr->sTunerInfo.m_eTvSystem_tv;	
					mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_TvSys | MASK_SaveTunerInfo_AvinSys;
				
					paint_atv_audiosys_info(mmenu_attr, 1);
					break;
				}
	
				default :
					break;
			}
		}
		break;
	
		case m_eMenuDepth_2 :
		{
			switch(mmenu_attr->focus_item)
			{
				case IDX_ATV_MENU_TV_PLAY :
				{
					if(++g_atvSubMenuIdx >= m_eTvMenuIdx_max)
					{
						g_atvSubMenuIdx = 0;
					}

					atv_submenu_on(mmenu_attr, msg);
				}
				break;

				default :
					break;
			}
		}
		break;
	
		case m_eMenuDepth_3:
		{
			switch(mmenu_attr->focus_item)
			{
				case IDX_ATV_MENU_TV_PLAY :
				case IDX_ATV_MENU_TV_AVIN:
				{
					switch(g_atvSubMenuIdx)
					{
						case m_eTvMenuIdx_system:
						{
							E_TV_SYSTEM *peTvSys = NULL;
							//paint_atv_audiosys_info(mmenu_attr, -1);
							
							if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY)
							{
							#if !SP_ATV_SYS_IndepSetEnable
								if(mmenu_attr->sTunerInfo.m_eTvSystem_tv != m_eTvSystem_Auto)
								{
									peTvSys = &mmenu_attr->sTunerInfo.m_eTvSystem_tv; // 实时伴音制式
								}
								else
							#endif
								{
									peTvSys = &mmenu_attr->sTunerInfo.m_psTunerAtvList[mmenu_attr->sTunerInfo.m_psTunerAtvList_idx].m_eTvSystem;
								} 
								mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_TvSys;
							}
							else if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_AVIN)
							{
								peTvSys = &mmenu_attr->sTunerInfo.m_eTvSystem_avin; 							
								mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_AvinSys;
							}
							
							if(peTvSys)
							{
								if(++(*peTvSys) >= m_eTvSystem_max)
								{
									*peTvSys = 0;
								}
							} 					

							//paint_atv_audiosys_info(mmenu_attr, 1);
							
							com_video_in_close();
							switch(mmenu_attr->focus_item)
							{
								case IDX_ATV_MENU_TV_PLAY:
								{
									atv_enter_tv(mmenu_attr, msg);
									atv_submenu_on(mmenu_attr, msg);
									break;
								}
								case IDX_ATV_MENU_TV_AVIN:
								{
									__gui_msg_t t_msg = *msg;
									msg->dwAddData2 = mmenu_attr->hLyr_frame;
									TCON_OccupyLcmi(CVBS_SRC__AVIN, &mmenu_attr->sTunerInfo, msg);
									*msg = t_msg;
									atv_submenu_on(mmenu_attr, msg);
									break;
								}
								default:	break;
							}							
							break;
						}
						default:	break;
					}
					break;
				}
				default:	break;
			}
			break;
		}
		
		default :
			break;
	}	

	return EPDK_OK;
}

__s32 mmenu_proc_key_search(__gui_msg_t *msg)
{
	mmenu_attr_t *mmenu_attr;
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	if(mmenu_attr->menuDepth >= m_eMenuDepth_1)
	{
		// 从各子模块退出
		switch(mmenu_attr->focus_item)
		{
			case IDX_ATV_MENU_TV_PLAY :
			{
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_PROCVTRC

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_PROCVTRC))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_PROCVTRC);
				}

#endif
				g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
				//paint_matv_item_all(mmenu_attr);
				TUNER_ExitTV(&mmenu_attr->sTunerInfo, msg);
				atv_submenu_sleep(mmenu_attr, msg); 	// 因为可能是从ATV的其它任何界面进入TV播放
				//atv_save_sys_para(mmenu_attr);
				mmenu_attr->menuDepth--;
				mmenu_attr->m_closeScreenDone_tcon = 0;
			}
			break;

			case IDX_ATV_MENU_TV_SEARCH :
				atv_submenu_sleep(mmenu_attr, msg);
				mmenu_attr->menuDepth--;
				break;

			case IDX_ATV_MENU_TV_SETTING :
				atv_submenu_sleep(mmenu_attr, msg);
				mmenu_attr->menuDepth--;
				break;

			case IDX_ATV_MENU_TV_AVIN :
				TCON_ReleaseLcmi(&mmenu_attr->sTunerInfo, msg);
				atv_submenu_sleep(mmenu_attr, msg);
				mmenu_attr->menuDepth--;
				g_isAVInput = 0;
				mmenu_attr->m_closeScreenDone_tcon = 0;
				break;

			default :
				break;
		}
	}

	if(TUNER_ScanTVFreq_Start(&mmenu_attr->sTunerInfo, 0) == 0)
	{
		mmenu_attr->focus_item = IDX_ATV_MENU_TV_SEARCH;
		mmenu_attr->menuDepth = m_eMenuDepth_1;
		atv_submenu_on(mmenu_attr, msg);
	}

	return EPDK_OK;
}

void atv_pt(mmenu_attr_t *mmenu_attr)
{
#if 1
	__u32 i;
	__msg_jjl("ATV menu info:focus=%d, depth=%d\n", mmenu_attr->focus_item, mmenu_attr->menuDepth);
	//__msg_jjl("eTvSystem_tv=%d, eTvSystem_avin=%d\n", mmenu_attr->sTunerInfo.m_eTvSystem_tv, mmenu_attr->sTunerInfo.m_eTvSystem_avin);
	for(i = 0; i < mmenu_attr->sTunerInfo.m_psTunerAtvList_cnt; i++)
	{
		//__msg_jjl("ch[%d]=(%d, %d)\n", i, mmenu_attr->sTunerInfo.m_psTunerAtvList[i].m_freq, mmenu_attr->sTunerInfo.m_psTunerAtvList[i].m_eTvSystem);
	}
#endif
}

static __s32  mmenu_key_proc(__gui_msg_t *msg)
{
	static __s32 last_key = -1;
	mmenu_attr_t *mmenu_attr;
	__msg("......mmenu_key_proc.........\n");
	mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

	if(!mmenu_attr)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}

	atv_pt(mmenu_attr);

	if((msg->dwAddData1 & MASK_GUI_MSG_VKEY) == GUI_MSG_VKEY)
	{
		last_key = msg->dwAddData1 & (~MASK_GUI_MSG_VKEY);
	}

#if SP_DEV_TCON_EX
	if(
	  (GUI_MSG_KEY_VADD != msg->dwAddData1)
	  && (GUI_MSG_KEY_LONGVADD != msg->dwAddData1)
	  && (GUI_MSG_KEY_VDEC != msg->dwAddData1)
	  && (GUI_MSG_KEY_LONGVDEC != msg->dwAddData1)
	  && (GUI_MSG_KEY_MUTE != msg->dwAddData1)
	  && (GUI_MSG_KEY_LONGMUTE != msg->dwAddData1)
	)
	{
		atv_submenu_off_volume(mmenu_attr, msg);
	}

	if(KEY_DOWN_ACTION == msg->dwAddData2)
	{
		if(GUI_MSG_KEY_MUTE == msg->dwAddData1)
		{
			atv_submenu_on_volume(mmenu_attr, g_is_mute, msg);
		}
	}
#endif	

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		switch(last_key)
		{
			case GUI_MSG_KEY_ENTER:
			{
				mmenu_proc_key_enter(msg);
				break;
			}
			

			case GUI_MSG_KEY_MENU :
			{
				mmenu_proc_key_return(msg);				
				break;
			}

			case GUI_MSG_KEY_NUM0 :
			case GUI_MSG_KEY_NUM1 :
			case GUI_MSG_KEY_NUM2 :
			case GUI_MSG_KEY_NUM3 :
			case GUI_MSG_KEY_NUM4 :
			case GUI_MSG_KEY_NUM5 :
			case GUI_MSG_KEY_NUM6 :
			case GUI_MSG_KEY_NUM7 :
			case GUI_MSG_KEY_NUM8 :
			case GUI_MSG_KEY_NUM9 :
			{
				mmenu_proc_key_num(msg);
				break;
			}

			case GUI_MSG_KEY_LOCKSCREEN :
			case GUI_MSG_KEY_LONGLOCKSCREEN :
			{
				if(g_isTconOccupyLcmi_flag)
				{
					if(mmenu_attr->m_closeScreenDone_tcon)
					{
						// 开屏
						mmenu_attr->m_closeScreenDone_tcon = 0;
						TCON_OpenScreen(&mmenu_attr->sTunerInfo);
					}
					else
					{
						// 关屏
						mmenu_attr->m_closeScreenDone_tcon = 1;
						TCON_CloseScreen(&mmenu_attr->sTunerInfo);
						SEND_MSG(DSK_MSG_TCON_CLOSE_SCREEN, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
					}
				}
			}
			break;

			default:
				break;
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_SEARCHFREQ :	// 自动搜台
			{
				//mmenu_proc_key_search(msg);
				break;
			}

			case GUI_MSG_KEY_LR :
			#if 0
				if(g_tvSoundChannelChange)
				{
					__gui_msg_t mymsg;
					mymsg.id = GUI_MSG_KEY;
					mymsg.h_srcwin = msg->h_deswin;
					mymsg.h_deswin = msg->h_deswin;
					mymsg.dwAddData1 = GUI_MSG_KEY_ENTER;
					mymsg.dwAddData2 = KEY_UP_ACTION;
					GUI_SendNotifyMessage(&mymsg);
					last_key = GUI_MSG_KEY_ENTER;
					return EPDK_OK;
				}
			#endif

			case GUI_MSG_KEY_SUBMENU :
			case GUI_MSG_KEY_LONGMENU :
				if(
					(mmenu_attr->menuDepth == m_eMenuDepth_1) &&
					(
						(mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY) ||
						((mmenu_attr->focus_item == IDX_ATV_MENU_TV_AVIN) && (g_isAVInput == CVBS_SRC__AVIN))
					)
				)
				{
					// 弹出子菜单:手动搜台和制式选择
					if(msg->dwAddData1 == GUI_MSG_KEY_LR)
					{
						g_tvSoundChannelChange = 1;
					}
					
					mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx = 0;
					atv_clear_cue_osd(mmenu_attr, msg);
					mmenu_attr->menuDepth++;
					g_atvSubMenuIdx = m_eTvMenuIdx_system;
					atv_submenu_on(mmenu_attr, msg);
				}

				break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:
			case GUI_MSG_KEY_PSUB:
			case GUI_MSG_KEY_LONGPSUB:
			{
				mmenu_proc_key_left(msg);
				break;
			}

			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:
			case GUI_MSG_KEY_PADD:
			case GUI_MSG_KEY_LONGPADD:
			{
				mmenu_proc_key_right(msg);
				break;
			}

		#if SP_DEV_TCON_EX
			case GUI_MSG_KEY_VADD :
			case GUI_MSG_KEY_LONGVADD :
			case GUI_MSG_KEY_VDEC :
			case GUI_MSG_KEY_LONGVDEC :
			{
				switch(mmenu_attr->focus_item)
				{
					case IDX_ATV_MENU_TV_PLAY :
					case IDX_ATV_MENU_TV_AVIN :
						atv_submenu_on_volume(mmenu_attr, 0, msg);
						break;

					default :
						break;
				}
				break;
			}
		#endif
		
#if SP_SaveTVShortcutIdx

			case GUI_MSG_KEY_LONGNUM0 :
			case GUI_MSG_KEY_LONGNUM1 :
			case GUI_MSG_KEY_LONGNUM2 :
			case GUI_MSG_KEY_LONGNUM3 :
			case GUI_MSG_KEY_LONGNUM4 :
			case GUI_MSG_KEY_LONGNUM5 :
			case GUI_MSG_KEY_LONGNUM6 :
			case GUI_MSG_KEY_LONGNUM7 :
			case GUI_MSG_KEY_LONGNUM8 :
			case GUI_MSG_KEY_LONGNUM9 :
				if((mmenu_attr->menuDepth == m_eMenuDepth_1) && (mmenu_attr->focus_item == IDX_ATV_MENU_TV_PLAY))
				{
					if(
					  (mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab == NULL)
					  || (mmenu_attr->sTunerInfo.m_psTunerAtvList_idx == IDX_sTunerAtvList_invalid)
					)
					{
						break;
					}

					mmenu_attr->sTunerInfo.m_psTunerAtvList_shortcutIdx_tab[msg->dwAddData1 - GUI_MSG_KEY_LONGNUM0]
					  = mmenu_attr->sTunerInfo.m_psTunerAtvList_idx;
					mmenu_attr->sTunerInfo.m_savePara_mask |= MASK_SaveTunerInfo_ShortcutIdx;
				}

				break;
#endif

			default :
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

	atv_pt(mmenu_attr);
	return EPDK_OK;
}

static __s32 _atv_menu_proc(__gui_msg_t *msg)
{
	//__msg("_atv_menu_Proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	//    ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			E_IDX_ATV_MENU eIdxAtvMenu = IDX_ATV_MENU_MAX;
			mmenu_attr_t *mmenu_attr;
			mmenu_para_t *mmenu_para;
			mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
			mmenu_attr = (mmenu_attr_t *)esMEMS_Balloc(sizeof(mmenu_attr_t));

			if(!mmenu_attr)
			{
				return EPDK_FAIL;
			}

			eLIBs_memset((void *)mmenu_attr, 0, sizeof(mmenu_attr_t));
			mmenu_attr->font = mmenu_para->mmenu_font;
			mmenu_attr->hLyr_frame = mmenu_para->hLyr_frame;
			mmenu_attr->focus_item = 0;
			mmenu_attr->focus_txt_color = GUI_YELLOW;//APP_COLOR_YELLOW;
			mmenu_attr->unfocus_txt_color = GUI_WHITE;//APP_COLOR_WHITE;
			mmenu_attr->menuDepth = 0;
			mmenu_attr->h_parent = mmenu_para->h_parent;
			GUI_WinSetAddData(msg->h_deswin, (__s32)mmenu_attr);
			
			atv_get_sys_para(mmenu_attr);
			init_mmenu_res(mmenu_attr);
			atv_submenu_paint_init_param(mmenu_attr);
			atv_type_paint_init_param(mmenu_attr);
			atv_menu_lyr_create_all(mmenu_attr->hLyr_sub);
			atv_drv_open(msg, mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode);

			switch(mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
			{
				case OP_RootFlags_CVBS_InputMode_TVFromHome:
				{					
					eIdxAtvMenu = IDX_ATV_MENU_TV_PLAY;
					SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
					eLIBs_printf(
						"************************************\n"
						"**            Enter TV            **\n"
						"************************************\n");
					break;
				}
				case OP_RootFlags_CVBS_InputMode_DVDFromHome:
				{
					dsk_amplifier_onoff(0);
					esKRNL_TimeDly(10);
					SEND_MSG(DSK_MSG_APP_MODE_ENTER, msg->h_deswin, GUI_WinGetHandFromName("init"), "DVD", 0, 0);
					SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);	
					IOCTRL__DVD_PW__SET_ON();
				#if TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN
					gs_enIoctrlDetPaForDVD = 0;
					GUI_SetTimer(msg->h_deswin, ID_TIMER_ATV_DVD_DISP_EN, TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN, 0);
					g_timerCnt_enDvdDisp = 0;
				#endif
					eIdxAtvMenu = IDX_ATV_MENU_TV_AVIN;
					eLIBs_printf(
						"************************************\n"
						"**            Enter DVD           **\n"
						"************************************\n");
					break;
				}
				case OP_RootFlags_CVBS_InputMode_AVINFromHome:
				case OP_RootFlags_CVBS_InputMode_AVINFromDetect:
				{
					eIdxAtvMenu = IDX_ATV_MENU_TV_AVIN;
					SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
					eLIBs_printf(
						"************************************\n"
						"**            Enter AVIN          **\n"
						"************************************\n");
					break;
				}
				default:	break;
			}
			
			if(eIdxAtvMenu < IDX_ATV_MENU_MAX)
			{
				//SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
				mmenu_attr->focus_item = eIdxAtvMenu;
				mmenu_attr->menuDepth = m_eMenuDepth_0;
				{
					__gui_msg_t mymsg;
					mymsg.id = GUI_MSG_KEY;
					mymsg.h_deswin = msg->h_deswin;
					mymsg.h_srcwin = msg->h_deswin;
					mymsg.dwAddData1 = GUI_MSG_VKEY_ENTER;
					mymsg.dwAddData2 = KEY_UP_ACTION;
					GUI_SendNotifyMessage(&mymsg);
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
			__wrn("GUI_MSG_DESTROY\n");
			{
				mmenu_para_t *mmenu_para;
				mmenu_attr_t *mmenu_attr;
				mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
				mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
#if TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_DVD_DISP_EN))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_DVD_DISP_EN);
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_SEARCH

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH);
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD);
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_AUTO_CLEAR_OSD_30S);
				}

#endif
				g_clearCueOsd_idx = IDX_ClearCueOsd_Invalid;
#if TIMEOUT_ID_TIMER_ATV_PROCVTRC

				if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_PROCVTRC))
				{
					GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_PROCVTRC);
				}

#endif
				if((mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
					== OP_RootFlags_CVBS_InputMode_DVDFromHome)
				{
					IOCTRL__DVD_PW__SET_OFF();
					SEND_MSG(DSK_MSG_APP_MODE_EXIT, msg->h_deswin, GUI_WinGetHandFromName("init"), "DVD", 0, 0);
				}
				g_enable_close_scn();
				atv_menu_lyr_del(mmenu_attr->hLyr_sub, m_eAtvMenuLyr_total);
				GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);
				atv_drv_close(msg);
				atv_save_sys_para(mmenu_attr);
				uninit_mmenu_res(mmenu_attr);
				if(mmenu_attr->pRegParamBak)
				{
					esMEMS_Bfree(mmenu_attr->pRegParamBak, sizeof(mmenu_attr_t));
				}
				if(mmenu_attr)
				{
					esMEMS_Bfree(mmenu_attr, sizeof(mmenu_attr_t));
				}
				if(mmenu_para)
				{
					esMEMS_Bfree(mmenu_para, sizeof(mmenu_para_t));
				}
			}
			return EPDK_OK;

		case GUI_MSG_PAINT:
		{
			mmenu_attr_t *mmenu_attr;
			mmenu_para_t *mmenu_para;
			mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);

			if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)))
			{
				__msg("layer not on, not need paint...\n");
				return EPDK_OK;
			}

			mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!mmenu_attr)
			{
				__err("invalid para...\n");
				return EPDK_FAIL;
			}

			if(mmenu_para->m_maskFlags & MASK_RootFlags_CVBS_InputMode)
			{
				GUI_LyrWinSetSta(mmenu_attr->hLyr_frame, GUI_LYRWIN_STA_SLEEP);
				return EPDK_OK;
			}

			//init_mmenu_res(mmenu_attr);
			paint_matv_item_all(mmenu_attr);
			return EPDK_OK;
		}

		case GUI_MSG_KEY:
		{
			__here__;
			mmenu_key_proc(msg);
			return EPDK_OK;
		}

		case GUI_MSG_TOUCH:
		{
			//mmenu_touch_proc(msg);
			return EPDK_OK;
		}

		case GUI_MSG_TIMER:
			__msg("GUI_MSG_TIMER: ID=%d\n", msg->dwAddData1);
			{
				mmenu_attr_t *mmenu_attr;
				mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
#if TIMEOUT_ID_TIMER_ATV_SEARCH

				if(msg->dwAddData1 == ID_TIMER_ATV_SEARCH)
				{
					__bool result;

					if(mmenu_attr->sTunerInfo.m_searchFlags_mask & MASK_SearchFlags_Manual)
					{
						result = paint_atv_manualsearch_info(mmenu_attr);
					}
					else
					{
						result = paint_atv_autosearch_info(mmenu_attr, 1);
					}

					if(mmenu_attr->sTunerInfo.m_tunerMode == MODE_Tuner_Idle)
					{
						__wrn("ID_TIMER_ATV_SEARCH: MODE_Tuner_Idle\n");

						if(mmenu_attr->sTunerInfo.m_searchStatus == STATE_TunerSearch_Idle)
						{
							__wrn("ID_TIMER_ATV_SEARCH:STATE_TunerSearch_Idle\n");

							if(!result)
							{
								__wrn("paint search info isn't over.\n");
								return EPDK_OK;	// 搜台结束，但是进度提示没有完成
							}
						}

						if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_ATV_SEARCH))
						{
							GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_SEARCH);
						}

						//atv_save_sys_para(mmenu_attr);
						mmenu_attr->sTunerInfo.m_searchValueValid_flag = 0;
						g_enable_close_scn();

						atv_submenu_sleep(mmenu_attr, msg);
						if(mmenu_attr->focus_item == IDX_ATV_MENU_TV_SEARCH)
						{
							mmenu_attr->focus_item = IDX_ATV_MENU_TV_PLAY;
							mmenu_attr->menuDepth = m_eMenuDepth_1;
						}
						else
						{
							mmenu_attr->menuDepth--;
							atv_submenu_on(mmenu_attr, msg);
						}

						SEND_MSG(DSK_MSG_SCREEN_OPEN, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
						atv_enter_tv(mmenu_attr, msg);	// 搜索完成后，自动播放TV
						__wrn("ID_TIMER_ATV_SEARCH:menuDepth=%d\n", mmenu_attr->menuDepth);
					}
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD
				else if(msg->dwAddData1 == ID_TIMER_ATV_AUTO_CLEAR_OSD)
				{
					atv_clear_cue_osd(mmenu_attr, msg);
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_AUTO_CLEAR_OSD_30S
				else if(msg->dwAddData1 == ID_TIMER_ATV_AUTO_CLEAR_OSD_30S)
				{
					__wrn("ID_TIMER_ATV_AUTO_CLEAR_OSD_30S:menuDepth=%d,focus_item=%d\n",
					      mmenu_attr->menuDepth, mmenu_attr->focus_item);
					atv_submenu_sleep(mmenu_attr, msg);
					mmenu_attr->menuDepth--;
					if(mmenu_attr->menuDepth > m_eMenuDepth_0)
					{
						atv_submenu_on(mmenu_attr, msg);
					}
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_PROCVTRC
				else if(msg->dwAddData1 == ID_TIMER_ATV_PROCVTRC)
				{
					__wrn("\nMSG:ID_TIMER_ATV_PROCVTRC\n");
					TCON_ProcVTRC(mmenu_attr->sTunerInfo.m_pEsFile_drv);
					atv_clear_cue_osd(mmenu_attr, msg);
				}

#endif
#if TIMEOUT_ID_TIMER_ATV_DVD_DISP_EN
				else if(msg->dwAddData1 == ID_TIMER_ATV_DVD_DISP_EN)
				{
					static __u8 s_cnt = 0;
					__s8 setFlag = -1;
					
					if(++g_timerCnt_enDvdDisp >= TIMEOUT_DVD_DISP_EN_ACK)
					{
						__msg_jjl("g_timerCnt_enDvdDisp = %d timeout.\n", g_timerCnt_enDvdDisp);
						g_timerCnt_enDvdDisp = 0;
						setFlag = 2;
					}
					else
					{
						if(IOCTRL__DVD_STA__IS_DISP_EN())
						{
							if(++s_cnt >= TIMEVALID_DVD_DISP_EN_ACK)
							{
								setFlag = 2;
							}
						}
						else
						{
							s_cnt = 0;
						}
					}

					if(setFlag != -1)
					{
						s_cnt = 0;
						GUI_KillTimer(msg->h_deswin, ID_TIMER_ATV_DVD_DISP_EN);
						com_video_set_blue_flag(setFlag);
						gs_enIoctrlDetPaForDVD = 1;
						//esKRNL_TimeDly(100);
						SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, GUI_WinGetHandFromName("init"), 0, 0, 0);
						paint_atv_type_info(msg, m_eAtvTypeIdx_dvd);
					}
				}

#endif
				return EPDK_OK;
			}

		case GUI_MSG_COMMAND:
			return EPDK_OK;

		case GUI_MSG_WIN_WAKEUP:
		{
			GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
			return EPDK_OK;
		}

		case DSK_MSG_SCREEN_OPEN :
			if(g_isTconOccupyLcmi_flag)
			{
				mmenu_attr_t *mmenu_attr;
				mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

				if(mmenu_attr->m_closeScreenDone_tcon)
				{
					// 开屏
					mmenu_attr->m_closeScreenDone_tcon = 0;
					TCON_OpenScreen(&mmenu_attr->sTunerInfo);
					return EPDK_OK;
				}
			}

		default :
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

__s32 atv_menu_win_create(H_WIN h_parent, mmenu_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	mmenu_para_t *mmenu_para;
	FB fb;
	GUI_LyrWinGetFB(para->hLyr_frame, &fb);
	mmenu_para = (mmenu_para_t *)esMEMS_Balloc(sizeof(mmenu_para_t));

	if(!mmenu_para)
	{
		return EPDK_FAIL;
	}

	eLIBs_memset((void *)mmenu_para, 0, sizeof(mmenu_para_t));
	mmenu_para->mmenu_font = para->mmenu_font;
	mmenu_para->hLyr_frame = para->hLyr_frame;
	mmenu_para->m_maskFlags = para->m_maskFlags;
	mmenu_para->h_parent = h_parent;
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name = "atv_menu_win";
	framewin_para.dwExStyle = WS_EX_NONE;
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption =  NULL;
	framewin_para.hOwner	= NULL;
	framewin_para.id         = MAIN_MENU_ID;
	framewin_para.hHosting = h_parent;
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_atv_menu_proc);
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width = fb.size.width;
	framewin_para.rect.height = fb.size.height;
	framewin_para.BkColor.alpha =  0;
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)mmenu_para;
	framewin_para.hLayer = para->hLyr_frame;
	return (GUI_FrmWinCreate(&framewin_para));
}

__bool app_atv_isAVInput(void)
{
	if(g_isAVInput == CVBS_SRC__AVIN)	return 1;
	
	return 0;
}

__bool app_atv_enIoctrlDetPaForDVD(void)
{
	return gs_enIoctrlDetPaForDVD;
}


#endif
