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


extern   __s32  fm_init_rtc6207(void);
extern   __s32  fm_exit_rtc6207(void);
extern   __s32  fm_play_rtc6207(__s32 freq);
extern   __s32  fm_auto_search_rtc6207(__s32 freq, __u32 search_dir);
extern   __s32  fm_manual_search_rtc6207(__s32 freq, __u32 search_dir);
extern   __s32  fm_area_choose_rtc6207(__s32 area, void *pbuffer);
extern   __s32  fm_stereo_choose_rtc6207(__s32 audio_method);
extern   __s32  fm_mute_rtc6207(__s32 voice_onoff);
extern   __s32  fm_signal_level_rtc6207(__s32 signal_level);
extern   __s32  RTC6218_get_rssi(void);	//@"RTC6218_RX"

extern   __s32  fm_send_on_rtc6207(void);
extern   __s32  fm_send_rtc6207(__s32 freq);
extern   __s32  fm_send_off_rtc6207(void);
extern   __s32  fm_pa_gain_rtc6207(__u8 pagain);
extern   __s32  fm_get_status_rtc6207(void);


#endif /*_RDA_FM5807P_DRV_H_*/
