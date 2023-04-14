#ifndef		__NEW_FM_H
#define		__NEW_FM_H



#include	"app_new_fm_i.h"
#include	"app_new_fm.h"













typedef	struct
{
	load_bmp_res_t new_bmp_arrow[2];	//�α��ͷָʾ
	load_bmp_res_t new_bmp_scale;		//�α�̶ȡ���ߣ��̶���
	load_bmp_res_t new_bmp_cursor_sel;	//��ǰѡ��ͨ��Ƶ��---ָʾ��
	load_bmp_res_t new_bmp_freq_point;	//Ƶ����ʾ��С����ͼƬ
	load_bmp_res_t new_bmp_freq_MHz;	//Ƶ����ʾ�ĺ���MHzͼƬ
	load_bmp_res_t new_bmp_number_b[10];//�����ʾ��MHzƵ�����֣�0~9
	load_bmp_res_t new_bmp_87MHz;		//���87.0MHzͼƬ
	load_bmp_res_t new_bmp_108MHz;		//���108.0MHzͼƬ




}NEW_FM_FREQ_SCALE_RES_T;//���ڴ��UI����ͼƬ���ݽṹ��


typedef struct	bmp_uipara
{
	H_THEME theme_arrow[2]; 	//�α��ͷָʾ
	H_THEME theme_scale;		//�α�̶ȡ���ߣ��̶���
	H_THEME theme_cursor_sel;	//��ǰѡ��ͨ��Ƶ��---ָʾ��
	H_THEME theme_freq_point;	//Ƶ����ʾ��С����ͼƬ
	H_THEME theme_freq_MHz; 	//Ƶ����ʾ�ĺ���MHzͼƬ
	H_THEME theme_number_b[10];	//�����ʾ��MHzƵ�����֣�0~9
	H_THEME theme_87MHz;		//���87.0MHzͼƬ

}bmp_uipara_t;//���ͼƬ��Դ



typedef	struct	fm_create_para
{
	H_WIN	fm_frmwin;	//�����framewin���ھ��
	H_LYR	fm_frmlyr;	//�����ͼ����
	NEW_FM_CTRLDATA_T	*fm_ctrl1;//��app_new_fm.c�����FM���ƽṹ������

}fm_create_para_t;//����Ĳ���

typedef	struct  fm_scene_para
{
	//�ڲ�����
	H_WIN	hfrm;
	H_LYR	hlyr;
	NEW_FM_CTRLDATA_T	*fm_ctrl2;//�ڲ���Ҫʹ�õ�FM���ƽṹ������
}fm_scene_t;//�ڲ�����


typedef	struct	fm_ctrl_para
{
	fm_scene_t	*fm_frmcreate;
	NEW_FM_FREQ_SCALE_RES_T res;
	bmp_uipara_t bmp_uipara;
	//NEW_FM_CTRLDATA_T	*fm_ctrl;
}fm_ctrl_para_t;//���framewin���ڵ������ؼ���Ϣ


H_WIN	new_fm_frmwin_create(H_WIN hparent, fm_create_para_t *para);
//__s32 fast_draw_freq_scale(H_WIN hparent, H_LYR lyr);


#endif

