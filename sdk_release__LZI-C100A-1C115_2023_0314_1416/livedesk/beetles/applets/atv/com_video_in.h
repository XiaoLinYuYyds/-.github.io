#ifndef __COM_VIDEO_IN_H__
#define __COM_VIDEO_IN_H__


//ev_index:0 for csi, 1 for tv in
//tvd_sel: tv0/tv1
__s32 com_video_in_open(__s32 dev_index, __s32 tvd_sel, __drv_TVD_system cvbsSys, __s32 enable_3d);

__s32 com_video_in_close(void);

//tvd_sel: tv0/tv1
__s32 com_video_in_set_sel(__s32 tvd_sel);

///flag: 0: 取消蓝屏，检测无信号也照样不蓝屏  1:直接输出蓝屏，不检测信号， 2:有信号则正常显示，无信号显示蓝屏
__s32 com_video_set_blue_flag(__s32 flag);

///抓一批数据到U盘
__s32 com_video_grap_data(void);


//0<=val<=255 默认128
__s32 com_video_set_contrast(__s32 val);

//0<=val<=255 默认32
__s32 com_video_set_bright(__s32 val);

//0<=val<=255 默认0
__s32 com_video_set_saturation(__s32 val);

//0<=val<=255 默认128
__s32 com_video_set_hue(__s32 val);

void com_video_set_smooth(__disp_video_smooth_t mode);

//返回1代表有信号，返回0代表无信号
__s32 com_video_get_status(void);

//返回1代表P制，返回0代表N制
__s32 com_video_get_format(void);

__s32 com_video_cvbs_analyze(void);

__s32 com_video_set_gain(__s32 val);

#endif//__COM_VIDEO_IN_H__
