#ifndef		__APP_NEW_FM_I_H
#define		__APP_NEW_FM_I_H

#if 1  //必须加载的两个头文件
#include "apps.h"
#include "beetles_app.h"
#endif

#define ID_TIMER_FM_TestPlayFreq_1				0x01/*0x9876*/
#define ID_TIMER_FM_TestPlayFreq_Speed_1		250	// 250 *10ms


#define	NEW_FM_ID	(APP_NEWFM_ID+1)	//framewin窗口id

#define NEW_FM_GetManWnd()                   (g_FMManWnd)
#define NEW_FM_GetWndPara(_p, _t, _hwnd)     (_p = (_t *)GUI_WinGetAttr(_hwnd))
#define NEW_MAX_CHANNEL_FREQ                     FM_SEARCH_CHN_MAX_FREQ
#define NEW_MIN_CHANNEL_FREQ                     FM_SEARCH_CHN_MIN_FREQ
#define NEW_FM_UpdateUI(_hwnd)               do{ __msg("FM_UpdateUI\n"); GUI_WinUpdate(_hwnd, EPDK_TRUE);} while (0)	//窗口更新

typedef struct  //TODO: 精简某些量
{

	__s32 cur_max_freq; //存放当前最大频率：108000
	__s32 cur_min_freq; //存放当前最小频率：87000

	__s32 channel_id;     //存放当前通道id号
	__u32 cur_freq;       //存放当前频率
	__u32 channel_count;  //存放通道总数

	__s32 search_mode;    //搜索模式 : auto or manual

	__s32 is_record;

	__s32 thumb_show;     // thumb btn,0 -- 不显示, 1 -- 显示为非焦点, 2 -- 显示为焦点
	__s32 audio_mode;

	//数字选取===================================
	__s32  ndigit_fm_num;    //数字选取序号
	__u8   digit_timmer_id;  //数字选取timmer id号
	H_LYR  hdigit_layer;     //显示当前数字的图层
	//=============================================
	__bool m_dispCue;
}
NEW_FM_CTRLDATA_T;


typedef enum
{
  NEW_CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,	//自动搜索状态下搜索到一个频点
  NEW_CMD_AUTOSRH_FINDCHFAIL,					//自动搜索所有
  NEW_CMD_AUTOSRH_OVER,							//自动搜索完成
  NEW_FM_COMMAND,
} __NEW_fmplay_msg_t;

enum
{
  NEW_SRHMODE_IDLE,		//空状态
  NEW_SRHMODE_AUTO,		//自动搜索状态
  NEW_SRHMODE_MANUAL,	//手动搜索状态
};


#if 1//按键消息命令
typedef enum
{
  MSG_NEW_FM_OP_UP = GUI_MSG_USER_DEF,//0x3000
  MSG_NEW_FM_OP_DOWN,
  MSG_NEW_FM_OP_ENTER,
  MSG_NEW_FM_OP_LEFT,
  MSG_NEW_FM_OP_RIGHT,
  MSG_NEW_FM_OP_EXIT,
} new_setting_set_msg_t;//framewin窗口的回调处理函数的功能
#endif

































__s32  app_new_fm_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved);



#endif

