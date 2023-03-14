
#ifndef  __new_spectrum_h
#define  __new_spectrum_h


/* control class name */
#define CTRL_SPECTRUM                   "CTRL_SPECTRUM"


/* spectrum style */


/* spectrum notify message */
typedef enum
{
  STN_KEY = 0,
  STN_
} stn_e;


/* attr for a spectrum */
typedef struct tag_new_spectrum_para_t
{
	__s32  min; //0
	__s32  max; //16
	void  *particle_bmp_buf;
	__u32  particle_nr;
	void *buoy_bmp_buf;
} new_spectrum_para_t;//频谱参数


typedef struct _NEW_HTRUM_t
{
	H_LYR   h_lyr;			//控件窗口图层
	RECT    disp_rect;		//显示矩形
	__s32   min;
	__s32   max;
	__u32   bk_color;		//背景颜色
	__s32   particle_nr;
	__s32   particle_width;	//频谱背景图宽
	__s32   particle_height;
	__u32  *particle_matrix;//频谱背景图矩阵
	__s32  *particle_x_array;
	__s32   buoy_width;		//频谱黄色图片宽
	__s32   buoy_height;
	__u32  *buoy_matrix;
	__s32  *buoy_x_array;	//频谱移动x阵列
	__s32  *value_array;	//阵列值
	__s32  *length_array;
	__s32  *buoy_y_array;
	__s32  *fall_times;
	__bool  is_internal_cmd;
	__bool  is_waking;		 //新的唤醒状态
	__bool  is_showing;		 //新的显示
	__s32  *old_length_array;//旧的矩形/数组长度
	__bool  old_is_waking;	 //旧的唤醒状态
	__bool  old_is_showing;	 //旧的显示
}
NEW_HTRUM_t, *NEW_HTRUM;


static __bool _is_valid_handle(NEW_HTRUM htrum);//检测是否有句柄
static __s32  _ondraw(NEW_HTRUM htrum, H_WIN hwin);//绘制频谱
static __s32  _spectrumCtrlProc(__gui_msg_t *msg);
static void   _save_locale(NEW_HTRUM htrum);
static void   _notify_parent(H_WIN hwin, __u32 notify_code, __u32 dwAddData);
static __bool _my_GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd);
static __s32  _reset_environment(NEW_HTRUM htrum, H_WIN hwin);
static void   _reset_status(NEW_HTRUM htrum);
static void   _release_res(NEW_HTRUM htrum);
static __s32  _init_htrum(NEW_HTRUM htrum, H_WIN hwin);
static __s32  _init_bk(NEW_HTRUM htrum);
static __s32  _init_particle(NEW_HTRUM htrum, const void *particle_bmp_buf);
static __s32  _init_buoy(NEW_HTRUM htrum, const void *buoy_bmp_buf);
static __s32  _init_coordinate(NEW_HTRUM htrum);
static void   _draw_buoy(NEW_HTRUM htrum, __s32 i);
static void   _clear_buoy(NEW_HTRUM htrum, __s32 i);
static void   _redraw_particle(NEW_HTRUM htrum, __s32 i);
static void   _redraw_buoy(NEW_HTRUM htrum, __s32 i);
static __u32  _calculate_length(__s32 min, __s32 max, __s32 value, __u32 total_length);
static void   _draw_particle(NEW_HTRUM htrum, __s32 i);
static __s32  _requeset_other_res(NEW_HTRUM htrum);
static void   _draw_spectrum(NEW_HTRUM htrum);
static void   _redraw_spectrum(NEW_HTRUM htrum);


extern __bool NEW_RegisterSpectrumControl(void);
extern __bool NEW_UnRegisterSpectrumControl(void);
extern __s32  NEW_SPECTRUM_SetValue(H_WIN hwin, __s32 *value_array);


#endif     //  ifndef __spectrum_h

/* end of spectrum.h */
