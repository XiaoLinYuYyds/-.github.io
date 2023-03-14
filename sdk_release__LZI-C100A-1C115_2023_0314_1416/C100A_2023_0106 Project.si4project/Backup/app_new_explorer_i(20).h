#ifndef  __APP_NEW_EXPLORER_I_H_
#define  __APP_NEW_EXPLORER_I_H_

#if 1  //必须加载的两个头文件
#include "apps.h"
#include "beetles_app.h"
#endif



#if 1//存放按键ID宏定义
#define ID_NEW_EXPLORER_SWITCH_ITEM		0
#define ID_OP_NEW_EXPLORER_UP			1
#define ID_OP_NEW_EXPLORER_DOWN			2
#define ID_OP_NEW_EXPLORER_ENTER			3
#define ID_OP_NEW_EXPLORER_LEFT			4
#define ID_OP_NEW_EXPLORER_RIGHT			5
#define ID_OP_NEW_EXPLORER_EXIT			6
#define ID_NEW_EXPLORER_ENTER_SMENU  	7
#define ID_NEW_EXPLORER_EXIT_SMENU   	8
#define ID_NEW_EXPLORER_SWITCH_TO_SETTING 	9
#define ID_NEW_EXPLORER_SWITCH_TO_HOME 		10

#endif
#if 1//按键消息命令
typedef enum
{
  MSG_NEW_EXPLORER_OP_UP = GUI_MSG_USER_DEF,//0x3000
  MSG_NEW_EXPLORER_OP_DOWN,
  MSG_NEW_EXPLORER_OP_ENTER,
  MSG_NEW_EXPLORER_OP_LEFT,
  MSG_NEW_EXPLORER_OP_RIGHT,
  MSG_NEW_EXPLORER_OP_EXIT,
} new_setting_set_msg_t;//framewin窗口的回调处理函数的功能
#endif

typedef enum new_explorer_view_mode_s
{
  NEW_EXPLR_LIST_MODE = 10000,
  NEW_EXPLR_SQUARE_MODE = 10001,
} new_explorer_view_mode_e;

#define NEW_NEWEXPLORER_LIST_ID		(APP_NEWEXPLORER_ID+100)	

void app_new_explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);














































#endif 

