#ifndef		__APP_NEW_FM_H
#define		__APP_NEW_FM_H

#include	"app_new_fm_i.h"


typedef struct  //TODO: ����ĳЩ��
{

	__s32 cur_max_freq; //��ǰ���Ƶ��
	__s32 cur_min_freq; //��ǰ��СƵ��

	__s32 channel_id;     //��ǰͨ��id��
	__u32 cur_freq;       //��ǰƵ��
	__u32 channel_count;  //ͨ������

	__s32 search_mode;    //����ģʽ : auto or manual

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
}
NEW_FM_CTRLDATA_T;

typedef struct app_new_fm_ctrl
{
	H_WIN	 fm_framewin;	//���fm��framewin����
	H_LYR	 fm_lyr;		//���fm��framewin����ͼ��

	NEW_FM_CTRLDATA_T	fm_ctrl;
		
	GUI_FONT *font;		//����ı�
	__s32 root_type;	//��Ŵ��������root����
}new_fm_ctrl;	





#endif
