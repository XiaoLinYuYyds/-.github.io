#ifndef _TUNER_H_DEF_
#define _TUNER_H_DEF_

typedef enum
{
  m_eTvSystem_PAL_DK = 0,
  m_eTvSystem_PAL_I,
  m_eTvSystem_PAL_BG,
  m_eTvSystem_NTSC_M,
  m_eTvSystem_SECAM,
  m_eTvSystem_PAL_M,
  m_eTvSystem_PAL_N,
  m_eTvSystem_PAL_60,
  m_eTvSystem_SECAM_BG,
  m_eTvSystem_max,
  m_eTvSystem_Auto,
} E_TV_SYSTEM, *PE_TV_SYSTEM;	// 电视制式

typedef enum
{
  CDTMTV_VSTD_M,
  CDTMTV_VSTD_M_X,
  CDTMTV_VSTD_N,
  CDTMTV_VSTD_N_X,
  CDTMTV_VSTD_BG,
  CDTMTV_VSTD_BG_X,
  CDTMTV_VSTD_I,
  CDTMTV_VSTD_I_X,
  CDTMTV_VSTD_DK,
  CDTMTV_VSTD_DK_X,

  CDTMTV_VSTD_FM,
  CDTMTV_VSTD_NONE		= 255	// No Video Standard Specified
} E_STD_T;

typedef struct
{
	__u32				m_freq;
	E_TV_SYSTEM		m_eTvSystem;		// 电视制式
} S_TUNER_ATV, *PS_TUNER_ATV;

typedef struct
{
	__u32				*m_pFreqTab;				// 频率表
	__u32				m_pFreqTab_size;			// 频率表数量
	PS_TUNER_ATV		m_psTunerAtvList;			// 存储搜索到的频道列表
	__u32				m_psTunerAtvList_size;		// 频道列表最大数量

	__u32				m_psTunerAtvList_cnt;		// 频道列表的频道数量
	__s32				m_psTunerAtvList_idx;		// 播放频道在频道列表中的位置索引
	__u32				tmpFreq;								// 临时频道

	ES_FILE			*m_pEsFile_drv;
	__u8				m_threadId;
	__u8				m_searchStatus;
	__u8				m_searchMode;
	__u32				m_savePara_mask;
	__u8				m_tunerMode;
	__u32				m_curSearchFreq;
	__u32				m_searchFreqCnt;
	E_TV_SYSTEM 	m_eTvSystem_tv, m_eTvSystem_avin;
	__bool				m_searchValueValid_flag;
#if SP_SaveTVShortcutIdx
	__s32				*m_psTunerAtvList_shortcutIdx_tab;
#else
	__s32				m_psTunerAtvList_shortcutIdx;
#endif
	__u8				m_psTunerAtvList_shortcutIdx_digits;
	__u8				m_searchFlags_mask;
} S_TUNER_INFO, *PS_TUNER_INFO;
// m_searchStatus :
#define STATE_TunerSearch_Idle				0
#define STATE_TunerSearch_Init				1
#define STATE_TunerSearch_Ing				2
#define STATE_TunerSearch_Over			3
#define STATE_TunerSearch_Resume		4
#define STATE_TunerSearch_Suspend		5
// m_searchMode :
#define MODE_TunerSearch_All				0
#define MODE_TunerSearch_ByTVSys		1
// m_tunerMode :
#define MODE_Tuner_Idle							0
#define MODE_Tuner_Searching				1
#define MODE_Tuner_TV							2
#define MODE_Tuner_Setting					3
// m_savePara_mask :
#define MASK_SaveTunerInfo_TVFreq			(1<<0)
#define MASK_SaveTunerInfo_TVList			(1<<1)
#define MASK_SaveTunerInfo_TvSys 			(1<<2)
#define MASK_SaveTunerInfo_AvinSys		(1<<3)
#define MASK_SaveTunerInfo_ShortcutIdx	(1<<4)
// m_searchFlags_mask :
#define MASK_SearchFlags_Manual				(1<<0)	// 此位为0时，表示AUTO;为1时，表示MANUAL
#define MASK_SearchFlags_Backward			(1<<1)	// 此位为0时，表示forward;为1时，表示back
#define MASK_SearchFlags_ShiftDir				(3<<2)
#define CASE_SearchFlags_WaitForward		(1<<2)	// 等待改变搜索方向: 向前搜索
#define CASE_SearchFlags_WaitBackward	(2<<2)	// 等待改变搜索方向: 向前搜索
#define MASK_SearchFlags_Over					(1<<4)	// 搜索结束

typedef enum
{
  CDTMTV_RT_SUCCESS = 0,
  CDTMTV_RT_IIC_ERROR,
  CDTMTV_RT_SCAN_FAIL,
  CDTMTV_RT_SCAN_DONE,
  CDTMTV_RT_SCAN_TIMEOUT,
  CDTMTV_RT_SCAN_IN_AFCWIN,
  CDTMTV_RT_SCAN_GOT_YSIGN,
  CDTMTV_RT_SCAN_EMPTY_CH,
#ifdef DEBUG_SCAN
  CDTMTV_RT_SCAN_YSNR_FAIL,
  CDTMTV_RT_SCAN_FMC_FAIL,
  CDTMTV_RT_SCAN_HV_FAIL,
#endif
  CDTMTV_RT_ERROR
} TV_RES;

typedef enum
{
  m_eTconPaletteColorMap_white = 0,
  m_eTconPaletteColorMap_black,
  m_eTconPaletteColorMap_red,
  m_eTconPaletteColorMap_green,
  m_eTconPaletteColorMap_max
} E_TCON_PALETTE_COLOR_MAP;

#define RANGE_TVFreq_Min					48250000L	// 单位: Hz
#define RANGE_TVFreq_Max					863250000L	// 单位: Hz
#define STEP_TVFreq_RDA5218 			500000L 		// 单位: Hz
#define FFSTEP_TVFreq_RDA5218 		1500000L 	// 单位: Hz

#define CVBS_SRC__TV				0	// 固定不要变
#define CVBS_SRC__AVIN			1
#define CVBS_SRC__DVD 			2

	#define TCON_INPUTCHANNEL_TUNER 		0
	#define TCON_INPUTCHANNEL_AVIN			1
	#define TCON_INPUTCHANNEL_DVD			1
	
extern const __u32 China_Catv_Frequency[];
extern const __u32 size_China_Catv_Frequency;

extern void TCON_ProcVTRC(ES_FILE *a_pEsFile_drvAtv);
extern __s32 TCON_CVBSDetect(__u8 cvbsSrc, PS_TUNER_INFO a_psTunerInfo);

extern void TCON_SetColorSystem(PS_TUNER_INFO a_psTunerInfo);
extern void TCON_OpenScreen(PS_TUNER_INFO a_psTunerInfo);
extern void TCON_CloseScreen(PS_TUNER_INFO a_psTunerInfo);
extern void TCON_OccupyLcmi(__u8 openChannelIdx, PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg);
extern void TCON_ReleaseLcmi(PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg);
extern void TCON_OSDFrameOff(PS_TUNER_INFO a_psTunerInfo, __u8 osdFrame_mask);

extern void TUNER_PlayTV(PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg);
extern void TUNER_ExitTV(PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg);
extern __s32 TUNER_ScanTVFreq_Start(PS_TUNER_INFO a_psTunerInfo, __bool a_manual_flag);
extern void TUNER_ScanTVFreq_Stop(PS_TUNER_INFO a_psTunerInfo);
extern __s32 TUNER_ScanTVFreq_Resume(PS_TUNER_INFO a_psTunerInfo, __bool a_setBackward);
extern void TUNER_ScanTVFreq_SetForward(PS_TUNER_INFO a_psTunerInfo);
extern void TUNER_ScanTVFreq_SetBackward(PS_TUNER_INFO a_psTunerInfo);
extern void TUNER_ScanTVFreq_Thread(void *p_arg);

#endif

