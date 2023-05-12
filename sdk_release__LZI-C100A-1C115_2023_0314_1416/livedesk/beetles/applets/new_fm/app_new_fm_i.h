#ifndef		__APP_NEW_FM_I_H
#define		__APP_NEW_FM_I_H

#if 1  //������ص�����ͷ�ļ�
#include "apps.h"
#include "beetles_app.h"
#endif

#define ID_TIMER_FM_TestPlayFreq_1				0x01/*0x9876*/
#define ID_TIMER_FM_TestPlayFreq_Speed_1		250	// 250 *10ms


#define	NEW_FM_ID	(APP_NEWFM_ID+1)	//framewin����id

#define NEW_FM_GetManWnd()                   (g_FMManWnd)
#define NEW_FM_GetWndPara(_p, _t, _hwnd)     (_p = (_t *)GUI_WinGetAttr(_hwnd))
#define NEW_MAX_CHANNEL_FREQ                     FM_SEARCH_CHN_MAX_FREQ
#define NEW_MIN_CHANNEL_FREQ                     FM_SEARCH_CHN_MIN_FREQ
#define NEW_FM_UpdateUI(_hwnd)               do{ __msg("FM_UpdateUI\n"); GUI_WinUpdate(_hwnd, EPDK_TRUE);} while (0)	//���ڸ���

typedef struct  //TODO: ����ĳЩ��
{

	__s32 cur_max_freq; //��ŵ�ǰ���Ƶ�ʣ�108000
	__s32 cur_min_freq; //��ŵ�ǰ��СƵ�ʣ�87000

	__s32 channel_id;     //��ŵ�ǰͨ��id��
	__u32 cur_freq;       //��ŵ�ǰƵ��
	__u32 channel_count;  //���ͨ������

	__s32 search_mode;    //����ģʽ : auto or manual

	__s32 is_record;

	__s32 thumb_show;     // thumb btn,0 -- ����ʾ, 1 -- ��ʾΪ�ǽ���, 2 -- ��ʾΪ����
	__s32 audio_mode;

	//����ѡȡ===================================
	__s32  ndigit_fm_num;    //����ѡȡ���
	__u8   digit_timmer_id;  //����ѡȡtimmer id��
	H_LYR  hdigit_layer;     //��ʾ��ǰ���ֵ�ͼ��
	//=============================================
	__bool m_dispCue;
}
NEW_FM_CTRLDATA_T;


typedef enum
{
  NEW_CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,	//�Զ�����״̬��������һ��Ƶ��
  NEW_CMD_AUTOSRH_FINDCHFAIL,					//�Զ���������
  NEW_CMD_AUTOSRH_OVER,							//�Զ��������
  NEW_FM_COMMAND,
} __NEW_fmplay_msg_t;

enum
{
  NEW_SRHMODE_IDLE,		//��״̬
  NEW_SRHMODE_AUTO,		//�Զ�����״̬
  NEW_SRHMODE_MANUAL,	//�ֶ�����״̬
};


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

