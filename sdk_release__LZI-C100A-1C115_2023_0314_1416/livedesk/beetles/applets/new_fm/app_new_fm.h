#ifndef		__APP_NEW_FM_H
#define		__APP_NEW_FM_H

#include	"app_new_fm_i.h"




typedef struct app_new_fm_ctrl
{
	H_WIN	 fm_framewin;	//���fm��framewin����
	H_LYR	 fm_lyr;		//���fm��framewin����ͼ��

	NEW_FM_CTRLDATA_T	fm_ctrl;
		
	GUI_FONT *font;		//����ı�
	__s32 root_type;	//��Ŵ��������root����
}new_fm_ctrl;	


extern new_fm_ctrl;


#endif
