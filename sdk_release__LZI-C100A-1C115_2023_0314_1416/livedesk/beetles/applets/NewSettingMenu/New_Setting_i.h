#ifndef __NEW_SETTING_I_H_
#define  __NEW_SETTING_I_H_

#if 1
#include "apps.h"
#include "beetles_app.h"

#if 1	//按键功能ID
#define ID_NEW_SETTING_SWITCH_ITEM		0
#define ID_OP_NEW_SETTING_UP			1
#define ID_OP_NEW_SETTING_DOWN			2
#define ID_OP_NEW_SETTING_ENTER			3
#define ID_OP_NEW_SETTING_LEFT			4
#define ID_OP_NEW_SETTING_RIGHT			5
#define ID_OP_NEW_SETTING_EXIT			6
#define ID_NEW_SETTING_ENTER_SMENU  	7
#define ID_NEW_SETTING_EXIT_SMENU   	8
#define ID_NEW_SETTING_SWITCH_TO_SETTING 9
#define ID_NEW_SETTING_SWITCH_TO_TIPS 10
#endif
//#define APP_MAX_NUM		8	//app应用数量
#define NEW_SETTING_ID		(APP_NEWSETTING_ID+100)	//(APP_SETTING_ID +100)//使用原来通用设置的ID //(APP_NEWSETTING_ID+100)	//(APP_HOME_ID +5)		////new setting 的id

#define NUMBERS_PRODUCT_INFO				0
#define NUMBERS_FACTORY_DEFAULT			1
typedef enum
{
  MSG_NEW_SETTING_OP_UP = GUI_MSG_USER_DEF,
  MSG_NEW_SETTING_OP_DOWN,
  MSG_NEW_SETTING_OP_ENTER,
  MSG_NEW_SETTING_OP_LEFT,
  MSG_NEW_SETTING_OP_RIGHT,
  MSG_NEW_SETTING_OP_EXIT,
  MSG_NEW_SETTING_RESTORE_FACTORY,
} new_setting_set_msg_t;//framewin窗口的回调处理函数的功能

//void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
void  app_new_setting_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);//GUI界面之间的消息命令信号发送函数
#endif

#endif
