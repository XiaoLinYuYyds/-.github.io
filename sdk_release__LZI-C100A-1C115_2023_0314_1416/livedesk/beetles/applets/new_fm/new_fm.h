#ifndef		__NEW_FM_H
#define		__NEW_FM_H



#include	"app_new_fm_i.h"




























typedef	struct	fm_create_para
{
	H_WIN	fm_frmwin;	//传入的framewin窗口句柄
	H_LYR	fm_frmlyr;	//传入的图层句柄


}fm_create_para_t;//传入的参数

typedef	struct  fm_scene_para
{
	//内部参数
	H_WIN	hfrm;
	H_LYR	hlyr;
	
}fm_scene_t;//内部参数


typedef	struct	fm_ctrl_para
{
	fm_scene_t	*fm_frmcreate;
	
}fm_ctrl_para_t;//存放framewin窗口的其他控件信息


H_WIN	new_fm_frmwin_create(H_WIN hparent, fm_create_para_t *para);


#endif

