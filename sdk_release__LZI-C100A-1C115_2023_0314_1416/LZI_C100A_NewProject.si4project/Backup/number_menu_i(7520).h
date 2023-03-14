#ifndef __NUMBER_MENU_I_H_
#define  __NUMBER_MENU_I_H_

#if 1
#include "apps.h"
#include "beetles_app.h"

#if 1	//按键ID
#define ID_NUMBER_MENU_SWITCH_ITEM		0
#define ID_OP_NUMBER_MENU_UP			1
#define ID_OP_NUMBER_MENU_DOWN			2
#define ID_OP_NUMBER_MENU_ENTER			3
#define ID_OP_NUMBER_MENU_LEFT			4
#define ID_OP_NUMBER_MENU_RIGHT			5
#define ID_OP_NUMBER_MENU_EXIT			6
#define ID_NUMBER_MENU_ENTER_SMENU  	7
#define ID_NUMBER_MENU_EXIT_SMENU   	8
#define ID_NUMBER_MENU_SWITCH_TO_GENERAL 9
#define ID_NUMBER_MENU_SWITCH_TO_TIPS 10
#endif
#define APP_MAX_NUM		8	//app应用数量
#define NEW_SETTING_ID					(APP_SETTING_ID+300)//new setting 的id

#define NUMBERS_PRODUCT_INFO				0
#define NUMBERS_FACTORY_DEFAULT			1
typedef enum
{
  MSG_NUMBER_MENU_OP_UP = GUI_MSG_USER_DEF,
  MSG_NUMBER_MENU_OP_DOWN,
  MSG_NUMBER_MENU_OP_ENTER,
  MSG_NUMBER_MENU_OP_LEFT,
  MSG_NUMBER_MENU_OP_RIGHT,
  MSG_NUMBER_MENU_OP_EXIT,
  MSG_NUMBER_MENU_RESTORE_FACTORY,
} set_msg_t;

void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
#endif

#endif
