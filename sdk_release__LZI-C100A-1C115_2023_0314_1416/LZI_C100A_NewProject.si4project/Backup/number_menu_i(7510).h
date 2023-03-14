#ifndef __NUMBER_MENU_I_H_
#define  __NUMBER_MENU_I_H_

#if 1
#include "beetles_app.h"


#define ID_SWITCH_ITEM		0
#define ID_OP_UP			1
#define ID_OP_DOWN			2
#define ID_OP_ENTER			3
#define ID_OP_LEFT			4
#define ID_OP_RIGHT			5
#define ID_OP_EXIT			6
#define ID_ENTER_SMENU  	7
#define ID_EXIT_SMENU   	8
#define ID_SWITCH_TO_GENERAL 9
#define ID_SWITCH_TO_TIPS 10

#define TIPS_PRODUCT_INFO				0
#define TIPS_FACTORY_DEFAULT			1
typedef enum
{
  MSG_OP_UP = GUI_MSG_USER_DEF,
  MSG_OP_DOWN,
  MSG_OP_ENTER,
  MSG_OP_LEFT,
  MSG_OP_RIGHT,
  MSG_OP_EXIT,
  MSG_RESTORE_FACTORY,
} set_msg_t;

void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
#endif

#endif
