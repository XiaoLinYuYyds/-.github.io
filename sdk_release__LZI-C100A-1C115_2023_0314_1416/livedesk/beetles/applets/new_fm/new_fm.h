#ifndef		__NEW_FM_H
#define		__NEW_FM_H



#include	"app_new_fm_i.h"




























typedef	struct	fm_create_para
{
	H_WIN	fm_frmwin;	//�����framewin���ھ��
	H_LYR	fm_frmlyr;	//�����ͼ����


}fm_create_para_t;//����Ĳ���

typedef	struct  fm_scene_para
{
	//�ڲ�����
	H_WIN	hfrm;
	H_LYR	hlyr;
	
}fm_scene_t;//�ڲ�����


typedef	struct	fm_ctrl_para
{
	fm_scene_t	*fm_frmcreate;
	
}fm_ctrl_para_t;//���framewin���ڵ������ؼ���Ϣ


H_WIN	new_fm_frmwin_create(H_WIN hparent, fm_create_para_t *para);


#endif

