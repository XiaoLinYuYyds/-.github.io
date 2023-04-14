#ifndef		__NEW_FM_H
#define		__NEW_FM_H



#include	"app_new_fm_i.h"
#include	"app_new_fm.h"













typedef	struct
{
	load_bmp_res_t new_bmp_arrow[2];	//游标箭头指示
	load_bmp_res_t new_bmp_scale;		//游标刻度、标尺，刻度条
	load_bmp_res_t new_bmp_cursor_sel;	//当前选择通道频率---指示条
	load_bmp_res_t new_bmp_freq_point;	//频率显示的小数点图片
	load_bmp_res_t new_bmp_freq_MHz;	//频率显示的后面MHz图片
	load_bmp_res_t new_bmp_number_b[10];//存放显示的MHz频率数字：0~9
	load_bmp_res_t new_bmp_87MHz;		//存放87.0MHz图片
	load_bmp_res_t new_bmp_108MHz;		//存放108.0MHz图片




}NEW_FM_FREQ_SCALE_RES_T;//用于存放UI界面图片数据结构体


typedef struct	bmp_uipara
{
	H_THEME theme_arrow[2]; 	//游标箭头指示
	H_THEME theme_scale;		//游标刻度、标尺，刻度条
	H_THEME theme_cursor_sel;	//当前选择通道频率---指示条
	H_THEME theme_freq_point;	//频率显示的小数点图片
	H_THEME theme_freq_MHz; 	//频率显示的后面MHz图片
	H_THEME theme_number_b[10];	//存放显示的MHz频率数字：0~9
	H_THEME theme_87MHz;		//存放87.0MHz图片

}bmp_uipara_t;//存放图片资源



typedef	struct	fm_create_para
{
	H_WIN	fm_frmwin;	//传入的framewin窗口句柄
	H_LYR	fm_frmlyr;	//传入的图层句柄
	NEW_FM_CTRLDATA_T	*fm_ctrl1;//从app_new_fm.c传入的FM控制结构体数据

}fm_create_para_t;//传入的参数

typedef	struct  fm_scene_para
{
	//内部参数
	H_WIN	hfrm;
	H_LYR	hlyr;
	NEW_FM_CTRLDATA_T	*fm_ctrl2;//内部需要使用的FM控制结构体数据
}fm_scene_t;//内部参数


typedef	struct	fm_ctrl_para
{
	fm_scene_t	*fm_frmcreate;
	NEW_FM_FREQ_SCALE_RES_T res;
	bmp_uipara_t bmp_uipara;
	//NEW_FM_CTRLDATA_T	*fm_ctrl;
}fm_ctrl_para_t;//存放framewin窗口的其他控件信息


H_WIN	new_fm_frmwin_create(H_WIN hparent, fm_create_para_t *para);
//__s32 fast_draw_freq_scale(H_WIN hparent, H_LYR lyr);


#endif

