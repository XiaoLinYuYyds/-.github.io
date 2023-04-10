/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*
* File    	: record.c
* By      	: Kingvan
* Func		: record main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  Kingvan  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "app_record.h"
#include "app_fm_i.h"


#define CONTROL_TEST	0

#if CONTROL_TEST
#include "control.h"
H_WIN g_controlWin = NULL;
#endif

static void app_record_path(precord_ctrl_t precord_ctrl)
{
	char root_type[50];
	char disk_name[RAT_MAX_PARTITION][4];
	eLIBs_memset(root_type, 0, sizeof(root_type));
	eLIBs_memset(disk_name, 0, sizeof(disk_name));

	switch(precord_ctrl->root_type)
	{
		case RAT_LOCAL:
			eLIBs_strcpy(root_type, RAT_LOCAL_DISK);
			break;

		case RAT_TF:
			eLIBs_strcpy(root_type, RAT_SD_CARD);
			break;

		case RAT_USB:
			eLIBs_strcpy(root_type, RAT_USB_DISK);
			break;

		default:
			break;
	}

	if(rat_get_partition_name(root_type, disk_name) == EPDK_OK)
	{
		precord_ctrl->root_type_ch = disk_name[0][0];

		if(precord_ctrl->root_type_ch > 'a' && precord_ctrl->root_type_ch < 'z')
		{
			precord_ctrl->root_type_ch -= 32;
		}
	}
	else
	{
		precord_ctrl->root_type_ch = '\0';
	}
}

static void app_record_sendMsg(H_WIN hWin, __u32 id)
{
	__gui_msg_t sendMsg;
	sendMsg.h_deswin = hWin;
	sendMsg.h_srcwin = NULL;
	sendMsg.id = id;
	sendMsg.dwAddData1 = 0;
	sendMsg.dwAddData2 = 0;
	sendMsg.dwReserved = 0;
	GUI_PostMessage(&sendMsg);
}
void  app_record_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t msg;
	msg.h_deswin = GUI_WinGetParent(hwin);
	__msg("msg.h_deswin=%x\n", msg.h_deswin);
	msg.h_srcwin = NULL;
	msg.id = GUI_MSG_COMMAND;
	msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	__msg("msg.dwAddData1=%x\n", msg.dwAddData1);
	msg.dwAddData2 = data1;
	msg.dwReserved = data2;
	GUI_SendNotifyMessage(&msg);
}
static H_LYR record_8bpp_layer_create(void)
{
	H_LYR layer = NULL;
	RECT LayerRect = {0, 0, 1024, 600};
	FB  fb =
	{
		{0, 0},                                   					/* size      */
		{0, 0, 0},                                      			/* buffer    */
		{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},   /* fmt       */

	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_PALETTE,
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
		"app_record",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width			= LayerRect.width;
	fb.size.height			= LayerRect.height;
	lstlyr.src_win.x  		= LayerRect.x;
	lstlyr.src_win.y  		= LayerRect.y;
	lstlyr.src_win.width 	= LayerRect.width;
	lstlyr.src_win.height 	= LayerRect.height;
	lstlyr.scn_win.x		= LayerRect.x;
	lstlyr.scn_win.y		= LayerRect.y;
	lstlyr.scn_win.width  	= LayerRect.width;
	lstlyr.scn_win.height 	= LayerRect.height;
	lstlyr.pipe = 1;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app bar layer create error !\n");
	}

	return layer;
}

////////////////////
#define _ADDA_PHY_BASE  (0Xf1C23C00)
#define _ADDA_MEM_SZ    (1024)

#define _ADDA_R_OFST(n)     (0X00 + (n)*4U)
#if 1//offset of regs
#define ADDA_o_DAC_DIGI_CTL             _ADDA_R_OFST(0)
#define ADDA_o_DAC_FIFO_CTL             _ADDA_R_OFST(1)
#define ADDA_o_DAC_FIFO_STA             _ADDA_R_OFST(2)
#define ADDA_o_DAC_TX_DATA              _ADDA_R_OFST(3)
#define ADDA_o_DAC_ANA_CTL              _ADDA_R_OFST(4)
#define ADDA_o_ANA_TUNING               _ADDA_R_OFST(5)
#define ADDA_o_DAC_DEBUG                _ADDA_R_OFST(6)
#define ADDA_o_ADC_FIFO_CTL             _ADDA_R_OFST(7)
#define ADDA_o_ADC_FIFO_STA             _ADDA_R_OFST(8)
#define ADDA_o_ADC_RX_DATA              _ADDA_R_OFST(9)
#define ADDA_o_ADC_ANA_CTL              _ADDA_R_OFST(10)
#define ADDA_o_ADC_DEBUG                _ADDA_R_OFST(11)
#define ADDA_o_DAC_TX_CNT               _ADDA_R_OFST(12)
#define ADDA_o_ADC_RX_CNT               _ADDA_R_OFST(13)
#endif

/************************************************************************/
/* Register Detail                      */
/************************************************************************/

typedef struct _DacDigiCtl
{
u32 reserved0               :
	12;
u32 digiVolCtl              :
	6;
u32 HighPassFilterEnBit     :
	1;
u32 reserved1               :
	5;
u32 DWAEnBit                :
	1;
u32 reserved2               :
	6;
u32 dacDigiPartEnBit        :
	1;
}
DacDigiCtl_t;

typedef struct _DacFifoCtl
{
u32 fifoFlushBit        :
	1;
u32 fifoOvRunIrqEnbit   :
	1;
u32 fifoUnRunIrqEnbit   :
	1;
u32 fifoEmptyIrqEnbit   :
	1;//b3
u32 fifoEmptyDrqEnbit   :
	1;
u32 reserved0           :
	1;//b5
u32 txSampResBit        :
	1;//0/1--16/24 bits resolution
u32 monoEnBit           :
	1;//b7
u32 addaLoopEnBit       :
	1;//b8
u32 reserved1           :
	2;
u32 fifoEmpTrigLevel    :
	5;
	/*TX DATA Reg 32 bits width, Tx Fifo Item 24 bits width,
	 *Data in Tx fifo is always MSB, if inputModeBit==1, hw dac will fetch Low 16 bits to fill hgih 16 bits of fifo item*/
u32 fifoInputModeBit    :
	1;//b16, tx FIFO Input Mode, 0/1-- data of TX DATA Reg at MSB/LSB
u32 unRunSampSelBit     :
	1;//0/1---send zero/last audio sample when under run.(from Jerry: always send last sample)
u32 reserved2           :
	11;
	/*fs select map: 0 48Khz; 1 32Khz;    2 24Khz;    3 16Khz;
	                 4 12Khz; 5 8Khz;     6 reseved;  7 reserved;  */
u32 fsSelect            :
	3;//b29-31
}
DacFifoCtl_t;

typedef struct _dacFifoSta
{
u32 reserved0           :
	1;
u32 fifoOvRunPendBit    :
	1;
u32 fifoUnRunPendBit    :
	1;
u32 fifoEmptyPendBit    :
	1;
u32 reserved1           :
	4;//b4-b7
u32 fifoSpaceWordCnt    :
	5;//b8-b12
u32 fifoEmptyBit        :
	1;//at least one room for new sample
u32 reserved2           :
	18;
}
DacFifoSta_t;

typedef struct _dacAnaCtl
{
u32 paVolCtl                    :
	6;
u32 paNotMuteBit                :
	1;//b6 all src to pa not mute bit
u32 mixerToPaNotMuteBit         :
	1;
u32 dacToPaNotMuteBit           :
	1;//b8
u32 mic2ToRMixerNotMuteBit      :
	1;//mic2 to mixer right channel not mute
u32 mic2ToLMixerNotMuteBit      :
	1;//mic2 to mixer left  channel not mute
u32 mic1ToRMixerNotMuteBit      :
	1;//mic1 to mixer right channel not mute
u32 mic1ToLMixerNotMuteBit      :
	1;//mic1 to mixer left  channel not mute
u32 lDacToRMixerNotMuteBit      :
	1;//Left dac to mixer right channel not mute
u32 rDacToRMixerNotMuteBit      :
	1;//right dac to mixer right channel not mute
u32 lDacToLMixerNotMuteBit      :
	1;//left dac to mixer left  channel not mute
u32 rFm2RMixerNotMuteBit        :
	1;//right FM to mixer right channel not mute
u32 lFm2LMixerNotMuteBit        :
	1;//left FM to mixer left  channel not mute
u32 rLineInToRMixerNotMuteBit   :
	1;//right dac to mixer right channel not mute
u32 lLineInToLMixerNotMuteBit   :
	1;//left dac to mixer left  channel not mute
u32 mic1And2ToMixerGainCtl      :
	3;//20-22
u32 fmToMixerGainCtl            :
	3;//23-25
u32 LineInToMixerGainCtl        :
	3;//26-28
u32 anaOutMixerEnBit            :
	1;
u32 lDacEnBit                   :
	1;//b30
u32 rDacEnBit                   :
	1;
}
DacAnaCtl_t;

/************************************************************************/
/* Analog to Digital Converter          */
/************************************************************************/
typedef struct _AdcFifoCtl
{
u32 fifoFlushBit            :
	1;//b0
u32 fifoOvRunIrqEnbit       :
	1;//b1
u32 reserved0               :
	1;//b2
u32 dataAvaIrqEnBit         :
	1;//b3
u32 dataAvaDrqEnBit         :
	1;//b4
u32 reserved1               :
	1;//b5
u32 rxSampResSelBit         :
	1;//b6
u32 monoEnBit               :
	1;//b7
u32 reserved2               :
	3;//b8-b10
u32 fifoFulTrigLevel        :
	5;//b11-15
u32 rxFifoOutModeBit        :
	1;//b16 0/1--expanding '0' at lsb of rx fifo Reg/expanding sign bit at MSB of rx fifo Reg
u32 reserved3               :
	11;//b17-27
u32 adcDigPartEnBit         :
	1;//b28
u32 fsSelect                :
	3;//b29-31
}
AdcFifoCtl_t;

typedef struct _AdcFifoSta
{
u32 reserved0           :
	1;//b0
u32 fifoOvRunPendBit    :
	1;//b1
u32 reserved1           :
	1;//b2
u32 dataAvaPendBit      :
	1;//b3
u32 reserved2           :
	3;//b4-6
u32 rcvSampCnt          :
	6;//b7-12
u32 fifoNotEmptyBit     :
	1;//b0
u32 reserved3           :
	18;//b14-31
}
AdcFifoSta_t;

typedef struct _AdcAnaCtl
{
u32 hpcomDebounceTime   :
	2;//HPCOM protect de-bounce time setting
u32 hpcomOutProtectEn   :
	1;//HPCOM output protection enable
u32 hpDirectDrvEn       :
	1;//Headphone direct-drive enable
u32 powerAmpEnBit       :
	1;//b4 Codec power amplifier enable bit
u32 reserved1           :
	1;//b5
u32 adcDitherClkSel     :
	2;//b6-7
u32 adcDitherEnBit      :
	1;//b8
u32 reserved2           :
	8;//b9-16
	/* src map
	 0 no input;    1 LFm + RFm;    2 mic1;
	 3 no input;    4 no input;     5 no input;
	 6 Left Mixer + Right Mixer;    7 no input
	*/
u32 adcInputSrcSel      :
	3;//b17-19. adc input source
u32 adcInputGainCtl     :
	3;//b20-22, adc input gain
u32 reserved3           :
	2;//b23-24
u32 micPreAmpGainCtl    :
	2;//b25-26
u32 reserved4           :
	2;//b27-28
u32 micPreAmpEnBit      :
	1;//b29
u32 reserved5           :
	1;//b30
u32 adcEnBit            :
	1;//b31
}
AdcAnaCtl_t;

//////
static __s32 app_record_on_create(__gui_msg_t *msg)
{
	precord_ctrl_t precord_para;
	precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
	g_disable_standby();
	init_record_res(&precord_para->rec_ui);
	precord_para->rec_view.pui 		= &precord_para->rec_ui;
	precord_para->rec_listView.pui 	= &precord_para->rec_ui;
	precord_para->layer = record_8bpp_layer_create();
	gscene_bgd_set_state(BGD_STATUS_SHOW);
	precord_para->curOpenFrmType = RECORD_NONE;
	precord_para->frm = rec_view_frm_create(msg->h_deswin, precord_para);

	if(precord_para->frm == NULL)
	{
		__inf("app_record_on_create() return to home!\n");
		GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
		app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
	}
	else
	{
		GUI_WinSetFocusChild(precord_para->frm);
	}

	return EPDK_OK;
}
static __s32 app_record_on_close(__gui_msg_t *msg)
{
	GUI_ManWinDelete(msg->h_deswin);
	return EPDK_OK;
}
static __s32 app_record_on_destory(__gui_msg_t *msg)
{
	precord_ctrl_t precord_para;
	precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
	dsk_radio_close();
	uninit_record_res(&precord_para->rec_ui);
	GUI_LyrWinDelete(precord_para->layer);
#if CONTROL_TEST
	g_controlWin = NULL;
#endif

	if(precord_para->h_dialoag_win)
	{
		app_dialog_destroy(precord_para->h_dialoag_win);
		precord_para->h_dialoag_win = NULL ;
	}

	precord_para->layer = 0;
	esMEMS_Bfree(precord_para, sizeof(record_ctrl_t));
	rat_set_modify_flag(RAT_MEDIA_TYPE_AUDIO, 1);
	g_enable_standby();
	{
		//恢复系统自动关屏时间
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			g_set_close_scn_time(para->backlightoff);
			__msg("para->backlightoff=%d\n", para->backlightoff);
		}
	}
	return EPDK_OK;
}
static __s32 app_record_on_restore(__gui_msg_t *msg)
{
	precord_ctrl_t precord_para;
	precord_para = (precord_ctrl_t)GUI_WinGetAddData(msg->h_deswin);
	gscene_bgd_set_state(BGD_STATUS_SHOW);
	GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_ON);
	GUI_WinSetFocusChild(precord_para->frm);
	return EPDK_OK;
}
static __s32 app_record_on_key(__gui_msg_t *msg)
{
#if CONTROL_TEST
	precord_ctrl_t precord_para;
	control_para para;
	precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

	if(KEY_UP_ACTION != msg->dwAddData2)
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_LEFT:
			{
				if(g_controlWin != NULL)
				{
					break;
				}

				para.controlType = CONTROL_VOLUME;
				para.controlID = APP_RECORD_ID + 3;
				para.parentWin = msg->h_deswin;
				g_controlWin = aux_control_frm_create(&para);

				if(g_controlWin == NULL)
				{
					__inf("control_frm_create() return to home!\n");
					GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
					app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
				}
				else
				{
					__inf("control_frm_create() OK!\n");
				}
			}
			break;
		}
	}

#endif
	return EPDK_OK;
}
static __s32 app_record_on_command(__gui_msg_t *msg)
{
	return EPDK_OK;
}

static __s32 _record_manager_win_proc(__gui_msg_t *msg)
{
	__msg("_record_manager_win_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
	      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

	switch(msg->id)
	{
		case DSK_MSG_FS_PART_PLUGOUT:
		{
			precord_ctrl_t precord_para;
			precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
			__inf("data2:%c, root_type_ch:%c\n", msg->dwAddData2, precord_para->root_type_ch);

			if(msg->dwAddData2 == precord_para->root_type_ch)
			{
				if(precord_para->bIsFMRecord)
				{
					precord_para->curOpenFrmType = TO_FM;
				}
				else
				{
					precord_para->curOpenFrmType = MAIN_HOME;
				}

				app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
			}
		}

		return EPDK_OK;

		case GUI_MSG_CREATE:
			return app_record_on_create(msg);

		case GUI_MSG_CLOSE:
			__msg("----record manager window GUI_MSG_CLOSE begin----\n");
			return app_record_on_close(msg);

		case GUI_MSG_DESTROY:
			__msg("----record manager window GUI_MSG_DESTROY begin----\n");
			return app_record_on_destory(msg);

		case GUI_MSG_KEY:
			app_record_on_key(msg);
			break;//注意!!!

		case DSK_APP_RESTORE:
			return app_record_on_restore(msg);

		case GUI_MSG_TOUCH:
			break;

		case GUI_MSG_COMMAND:
			return app_record_on_command(msg);

		case GUI_MSG_NOTIFY_CHILD_DELETED:
		{
			precord_ctrl_t precord_para;
			precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

			if(precord_para->curOpenFrmType == RECORD_FRM)
			{
				precord_para->curOpenFrmType = RECORD_NONE;

				if(precord_para->bIsFMRecord)
				{
					dsk_radio_unmute();
				}

				precord_para->frm = rec_view_frm_create(msg->h_deswin, precord_para);

				if(precord_para->frm == NULL)
				{
					GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
					app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
				}
				else
				{
					GUI_WinSetFocusChild(precord_para->frm);
				}
			}
			else if(precord_para->curOpenFrmType == RECORD_LIST_FRM)
			{
				precord_para->curOpenFrmType = RECORD_NONE;

				if(precord_para->bIsFMRecord)
				{
					dsk_radio_mute();
				}

				precord_para->frm = rec_list_view_frm_create(msg->h_deswin, precord_para);

				if(precord_para->frm == NULL)
				{
					GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
					app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
				}
				else
				{
					GUI_WinSetFocusChild(precord_para->frm);
				}
			}
			else if(precord_para->curOpenFrmType == MAIN_HOME)
			{
				precord_para->curOpenFrmType = RECORD_NONE;
				GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
				app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
			}
			else if(precord_para->curOpenFrmType == TO_FM)
			{
				precord_para->curOpenFrmType = RECORD_NONE;

				if(precord_para->bIsFMRecord)
				{
					dsk_radio_unmute();
				}

				GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
				app_record_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, MAKELONG(RECORD_SW_TO_FM, precord_para->root_type), 0);
			}
		}
		break;

		case RECORD_TO_LIST_VIEW_FRAME:
		{
			precord_ctrl_t precord_para;
			precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
			precord_para->curOpenFrmType = RECORD_LIST_FRM;
			app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
		}

		return EPDK_OK;

		case RECORD_TO_VIEW_FRAME:
		{
			precord_ctrl_t precord_para;
			precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
			precord_para->curOpenFrmType = RECORD_FRM;
			app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
		}

		return EPDK_OK;

		case RECORD_TO_HOME:
		{
			precord_ctrl_t precord_para;
			precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

			if(precord_para->bIsFMRecord)
			{
				precord_para->curOpenFrmType = TO_FM;
			}
			else
			{
				precord_para->curOpenFrmType = MAIN_HOME;
			}

			app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
		}

		return EPDK_OK;
	}

	return GUI_ManWinDefaultProc(msg);
}

__s32 app_record_create(root_para_t  *para)
{
	H_WIN hManwin;
	__gui_manwincreate_para_t 	create_info;
	precord_ctrl_t				precord_ctrl = NULL;
	__inf("****************************************************************************************\n");
	__inf("********  enter record application  root_type %d**************\n", para->root_type);
	__inf("****************************************************************************************\n");
	precord_ctrl = (precord_ctrl_t)esMEMS_Balloc(sizeof(record_ctrl_t));

	if(precord_ctrl == NULL)
	{
		__msg("esMEMS_Balloc fail\n");
		return NULL;
	}

	eLIBs_memset(precord_ctrl, 0, sizeof(record_ctrl_t));
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	precord_ctrl->pfont = para->font;
	precord_ctrl->bIsFMRecord = (para->record_opt == 1);
#if TIME_FMAutoCloseScreen

	if(precord_ctrl->bIsFMRecord)
	{
		g_set_close_scn_time(TIME_FMAutoCloseScreen);
	}

#endif
	precord_ctrl->root_type = para->root_type;
	app_record_path(precord_ctrl);
	create_info.name            = APP_RECORD;
	create_info.hParent         = para->h_parent;
	create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_record_manager_win_proc);
	create_info.attr            = (void *)precord_ctrl;
	create_info.id				= APP_RECORD_ID;
	create_info.hHosting        = NULL;
	hManwin = (GUI_ManWinCreate(&create_info));

	if(0)
	{
		//112350
		volatile DacAnaCtl_t *pDacAnaCtl = (volatile DacAnaCtl_t *)(ADDA_o_DAC_ANA_CTL + _ADDA_PHY_BASE);
		pDacAnaCtl->paNotMuteBit = 0;
		pDacAnaCtl->dacToPaNotMuteBit = 0;
		pDacAnaCtl->mic1ToLMixerNotMuteBit = 0;
		pDacAnaCtl->mic1ToRMixerNotMuteBit = 0;
		pDacAnaCtl->mic2ToLMixerNotMuteBit = 0;
		pDacAnaCtl->mic2ToRMixerNotMuteBit = 0;
		pDacAnaCtl->lDacToLMixerNotMuteBit = 0;
		pDacAnaCtl->rDacToRMixerNotMuteBit = 0;
		pDacAnaCtl->lDacToRMixerNotMuteBit = 0;
		pDacAnaCtl->lDacEnBit = 0;
		pDacAnaCtl->rDacEnBit = 0;
	}

	return hManwin;
}

__s32 app_record_notify_delete_sub_scene(H_WIN hmanwin)
{
	precord_ctrl_t precord_para;

	if(NULL == hmanwin)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	precord_para = (precord_ctrl_t)GUI_WinGetAttr(hmanwin);

	if(precord_para->frm)
	{
		char name[256] = {0};
		GUI_WinGetName(precord_para->frm, name);
		__wrn("name=%s\n", name);

		if(0 == eLIBs_strcmp("record_list_view_frm", name))
		{
			prec_list_view_frame_t plistviewFrm_ctl;
			plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(precord_para->frm);

			if(plistviewFrm_ctl == NULL)
			{
				return EPDK_FAIL;
			}

			if(plistviewFrm_ctl->delDlg)
			{
				app_dialog_destroy(plistviewFrm_ctl->delDlg);
				plistviewFrm_ctl->delDlg = NULL;
			}
		}
	}

	return EPDK_OK;
}

