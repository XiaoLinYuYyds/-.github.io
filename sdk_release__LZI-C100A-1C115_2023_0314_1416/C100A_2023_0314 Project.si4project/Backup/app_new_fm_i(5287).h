#ifndef		__APP_NEW_FM_I_H
#define		__APP_NEW_FM_I_H

#if 1  //������ص�����ͷ�ļ�
#include "apps.h"
#include "beetles_app.h"
#endif



#define	NEW_FM_ID	(APP_NEWFM_ID+1)





#if 1//������Ϣ����
typedef enum
{
  MSG_NEW_FM_OP_UP = GUI_MSG_USER_DEF,//0x3000
  MSG_NEW_FM_OP_DOWN,
  MSG_NEW_FM_OP_ENTER,
  MSG_NEW_FM_OP_LEFT,
  MSG_NEW_FM_OP_RIGHT,
  MSG_NEW_FM_OP_EXIT,
} new_setting_set_msg_t;//framewin���ڵĻص��������Ĺ���
#endif

































__s32  app_new_fm_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved);



#endif

