#ifndef		__APP_NEW_FM_H
#define		__APP_NEW_FM_H

#include	"app_new_fm_i.h"




typedef struct app_new_fm_ctrl
{
	H_WIN	 fm_framewin;	//存放fm的framewin窗口
	H_LYR	 fm_lyr;		//存放fm的framewin窗口图层

	NEW_FM_CTRLDATA_T	fm_ctrl;
		
	GUI_FONT *font;		//存放文本
	__s32 root_type;	//存放传入进来的root参数
}new_fm_ctrl;	


extern new_fm_ctrl;


#endif
