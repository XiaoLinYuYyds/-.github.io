#ifndef		__APP_NEW_FM_I_H
#define		__APP_NEW_FM_I_H

#if 1  //必须加载的两个头文件
#include "apps.h"
#include "beetles_app.h"
#endif

#define ID_TIMER_FM_TestPlayFreq_1				0x9876
#define ID_TIMER_FM_TestPlayFreq_Speed_1			250	// 250 *10ms


#define	NEW_FM_ID	(APP_NEWFM_ID+1)

#define NEW_FM_GetManWnd()                   (g_FMManWnd)
#define NEW_FM_GetWndPara(_p, _t, _hwnd)     (_p = (_t *)GUI_WinGetAttr(_hwnd))
#define NEW_MAX_CHANNEL_FREQ                     FM_SEARCH_CHN_MAX_FREQ
#define NEW_MIN_CHANNEL_FREQ                     FM_SEARCH_CHN_MIN_FREQ
#define NEW_FM_UpdateUI(_hwnd)               do{ __msg("FM_UpdateUI\n"); GUI_WinUpdate(_hwnd, EPDK_TRUE);} while (0)

typedef enum
{
  CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,
  CMD_AUTOSRH_FINDCHFAIL,
  CMD_AUTOSRH_OVER,
  FM_COMMAND,
} __fmplay_msg_t;

typedef enum
{
  NEW_CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,
  NEW_CMD_AUTOSRH_FINDCHFAIL,
  NEW_CMD_AUTOSRH_OVER,
  NEW_FM_COMMAND,
} __NEW_fmplay_msg_t;

enum
{
  NEW_SRHMODE_IDLE,
  NEW_SRHMODE_AUTO,
  NEW_SRHMODE_MANUAL,
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

