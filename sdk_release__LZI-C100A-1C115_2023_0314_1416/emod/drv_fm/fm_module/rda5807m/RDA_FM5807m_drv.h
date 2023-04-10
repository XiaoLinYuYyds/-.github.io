/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           hello world sample
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : tea5767.h
* By      : xueli
* Version : V1.00
*********************************************************************************************************
*/
#ifndef  _RDA_FM5807M_DRV_H_
#define  _RDA_FM5807M_DRV_H_

#include  "drv_fm_i.h"


extern   __s32  fm_init_rda5807(void);
extern   __s32  fm_exit_rda5807(void);
extern   __s32  fm_play_rda5807(__s32 freq);
extern   __s32  fm_auto_search_rda5807(__s32 freq, __u32 search_dir);
extern   __s32  fm_manual_search_rda5807(__s32 freq, __u32 search_dir);
extern   __s32  fm_area_choose_rda5807(__s32 area, void *pbuffer);
extern   __s32  fm_stereo_choose_rda5807(__s32 audio_method);
extern   __s32  fm_mute_rda5807(__s32 voice_onoff);
extern   __s32  fm_signal_level_rda5807(__s32 signal_level);

extern   __s32  fm_send_on_rda5807(void);
extern   __s32  fm_send_rda5807(__s32 freq);
extern   __s32  fm_send_off_rda5807(void);
extern   __s32  fm_pa_gain_rda5807(__u8 pagain);
extern   __s32  fm_get_status_rda5807(void);


#endif /*_RDA_FM5807P_DRV_H_*/
