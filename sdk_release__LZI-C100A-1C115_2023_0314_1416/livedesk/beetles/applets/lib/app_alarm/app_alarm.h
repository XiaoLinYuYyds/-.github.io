#ifndef __APP_ALARM_H__
#define __APP_ALARM_H__



#include "beetles_app.h"

#define ALARM_TIMER_TIMES				10								//0.5s

#define ALARM_MSG_CLOSE			GUI_MSG_USER_DEF+1000	//���͹ر���Ϣ��������

typedef enum app_alarm_bmp_e
{
  ALARM_ALARM_BG_BMP,		//����

  ALARM_ALARM_ANI1_BMP,	//����1
  ALARM_ALARM_ANI2_BMP,	//����2
  ALARM_ALARM_ANI3_BMP,	//����3

  ALARM_BUTTON_UNF_BMP,	//�޽���
  ALARM_BUTTON_F_BMP,		//����

  ALARM_0_BMP,			//����
  ALARM_1_BMP,
  ALARM_2_BMP,
  ALARM_3_BMP,
  ALARM_4_BMP,
  ALARM_5_BMP,
  ALARM_6_BMP,
  ALARM_7_BMP,
  ALARM_8_BMP,
  ALARM_9_BMP,
  ALARM_10_BMP,

  ALARM_BMP_MAX
} app_alarm_bmp;

typedef enum app_alarm_button_e
{
  ALARM_DELAY_BUTTON,		//̰˯����
  ALARM_EXIT_BUTTON		//�˳�����

} app_alarm_button;

typedef struct tag_app_alarm_attr
{
	H_LYR				layer;
	reg_alarm_para_t	*reg_alarm;

	__u16				alarmTimeID;
	__u32				alarmTimeStart;

	__bool				isDelay;

	__u32				ani_fp;

	app_alarm_button	buttonF;

	__bool				extRobinOpen;
	__s32				sysVolume;

	HTHEME 				h_bmp_alarm[ALARM_BMP_MAX];

} app_alarm_attr, *papp_alarm_attr;






extern H_WIN app_alarm_frm_create(H_WIN parentWin);


extern __s32 app_alarm_frm_destory(H_WIN alarmWin);














#endif//__APP_ALARM_H__
