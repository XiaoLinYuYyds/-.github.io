#ifndef		__PROGRESSBAR_UI_PARA_H
#define		__PROGRESSBAR_UI_PARA_H


#include "progressbar_i.h"

typedef struct
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	H_THEME res_hdl;/*存放进度条背景图图片资源*/
	H_THEME res_hdl1;/*存放进度条图片资源*/
} progress_rect_t;/*矩形大小*/

typedef	struct tag_progress_uipara_t
{
	__s32             pipe;//图层通道选择
	progress_rect_t uipara_lyr;/*进度条图层*/
	progress_rect_t uipara_frm;/*framewin图层大小*/
	progress_rect_t uipara_icon;//进度条背景
	progress_rect_t uipara_progressbar_icon;//进度条图片
	progress_rect_t uipara_prog_cur_time[3];//当前时间
	progress_rect_t uipara_prog_cur_time_dot[2];//时间分隔点
	progress_rect_t uipara_prog_total_time[3];//总时间
	progress_rect_t uipara_prog_total_time_dot[2];//时间分隔点
}progress_uipara_t;/*进度条ui坐标和矩形大小参数*/

progress_uipara_t *progress_get_uipara(__s32 rotate);

#endif

