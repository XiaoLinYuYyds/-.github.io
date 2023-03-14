
#ifndef __APP_FM_I_H__
#define  __APP_FM_I_H__

#include "beetles_app.h"

#include "app_fm.h"
#include "fm_uipara.h"

#define APP_FM_MOD   			"APP FM MOD"


#define FM_MAINWND_NAME         "FM_MAINWND_WND"
#define FM_MAINWND_ID            (APP_FM_ID + 1)

#define TIME_FMAutoCloseScreen		3		// ��λ:��//Ϊ0ʱ����ʾ��ʹ�ô˲���

#define ID_FM_MAINWND_NOTIFY_TOUCHUP 0x100
#define ID_FM_MAINWND_CLICKUP       0X101
#define ID_FM_MAINWND_CLICKDOWN       0X102
#define ID_FM_MAINWND_CLICKREPEAT       0X103

#define ID_FM_IS_AUTO_SEARCH_DLG (FM_MAINWND_ID + 1)
#define ID_FM_SAVE_FREQ_DLG      (FM_MAINWND_ID + 2)

#if SP_FM_WhenAutoSearch_PlayFreq
#define ID_TIMER_FM_TestPlayFreq				0x9876
#define ID_TIMER_FM_TestPlayFreq_Speed			250	// 250 *10ms
#endif

typedef enum
{
  CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,
  CMD_AUTOSRH_FINDCHFAIL,
  CMD_AUTOSRH_OVER,
  FM_COMMAND,
} __fmplay_msg_t;

enum
{
  SRHMODE_IDLE,
  SRHMODE_AUTO,
  SRHMODE_MANUAL,
};

enum
{
  ID_FM_CMD_SHOW_SSET = 0,
  ID_FM_CMD_HIDE_SSET,
  ID_FM_CMD_CREATE_SSET,
  ID_FM_CMD_DESTROY_SSET,
  ID_FM_CMD_SHOW_ASEARCH_DLG,
};

#define FM_HIDE_THUMB          0
#define FM_SHOW_THUMB_N        1
#define FM_SHOW_THUMB_F        2
#define IsFmShowThumbBtn(_v)   ((_v))
#define IsFmFocusThumbBtn(_v)  ((_v) == FM_SHOW_THUMB_F)


typedef struct  //TODO: ����ĳЩ��
{

	__s32 cur_max_freq; //��ǰ���Ƶ��
	__s32 cur_min_freq; //��ǰ��СƵ��

	__s32 channel_id;     //indicate current channel number
	__u32 cur_freq;       //indicate current frequence
	__u32 channel_count;  //indicate total channel

	__s32 search_mode;    //indicate search mode : auto or manual

	__s32 is_record;
	//	dsk_radio_band_mode_t band_mode;

	__s32 thumb_show;     // thumb btn,0 -- ����ʾ, 1 -- ��ʾΪ�ǽ���, 2 -- ��ʾΪ����
	__s32 audio_mode;

	//����ѡȡ===================================
	__s32  ndigit_fm_num;    //����ѡȡ���
	__u8   digit_timmer_id;  //����ѡȡtimmer id��
	H_LYR  hdigit_layer;     //��ʾ��ǰ���ֵ�ͼ��
	//=============================================
	__bool m_dispCue;
} FM_CTRLDATA_T;

typedef struct fm_icon_data_s
{
	__u32 res_id;
	HTHEME res_handle;
	void *res_buffer;
} fm_icon_data_t;									//icon data struct

typedef struct tag_fm_wnd_t
{
	//__u32 search_freq;                          //����Ƶ��
	H_WIN     h_main_wnd;
	H_LYR     h_main_lyr;

	__sset_create_para_t       *sset_create_para;
	H_WIN                       h_sset;

	H_WIN     h_dlg;

	FM_CTRLDATA_T fm_ctrl;

	GUI_FONT *font;
	H_WIN     root_type;
	H_WIN h_frm_alarm_coming ;	//���Ӷ�ʱʱ�䵽��ʱ�򵯳��ĶԻ���Ĵ��ھ��
} FM_WND_T;

//////////////////////////////////////////////////////////////////////////
extern H_WIN g_FMManWnd;

#define FM_GetManWnd()                   (g_FMManWnd)
#define FM_GetWndPara(_p, _t, _hwnd)     (_p = (_t *)GUI_WinGetAttr(_hwnd))

#define FM_UpdateUI(_hwnd)               do{ __msg("FM_UpdateUI\n"); GUI_WinUpdate(_hwnd, EPDK_TRUE);} while (0)
#define UI_UpdateFMNow(_hwnd)            SEND_MSG(GUI_MSG_PAINT, _hwnd, _hwnd, 0, 0, 0)

//////////////////////////////////////////////////////////////////////////
extern __s32 notify_to_close_fm(H_WIN hwnd, __s32 id, __s32 para);
//////////////////////////////////////////////////////////////////////////
#endif

