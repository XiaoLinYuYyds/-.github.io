/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          ir keyboard driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : drv_rocker_i.h
* By      : jerry
* Version : V1.00
*********************************************************************************************************
*/
#ifndef	_DRV_ROCKER_I_H_
#define	_DRV_ROCKER_I_H_

#include "epdk.h"
#include "bitops.h"
#include "bsp_rk.h"


#define __ADC_BITWIDE		12
#define __AD_MAX			((__u32)(~((~((__u32)0)) <<__ADC_BITWIDE)))
#define __ADC(vol, Vref)		((__u32)((__u32)(vol) *__AD_MAX /Vref))	// 将电平转化为AD值

#define __ADC_Vref			3300.0	// :mV
#define __ADC_sim(vol)		(__ADC(vol, __ADC_Vref))

#define Y2_AD_MIN			(__ADC_sim(0))
#define Y2_AD_MAX			(__ADC_sim(1875))
#define Y2_AD_LEVEL(n)		(((Y2_AD_MAX -Y2_AD_MIN) *n)/8)
#define Y2_AD_LEVEL_OFT	(__ADC_sim(42))



extern  __s32  rocker_init(void);
extern  __s32  rocker_exit(void);
extern  __u32  rocker_scan_tp(void);

typedef struct
{
	__u8				unlock;
	__s8					sta;
	__u8				pluginCnt, plugoutCnt;
} S_GET_STA, *PS_GET_STA;
	// for unlock:
	#define FLAG__PlugStaChangeDetEn				0x5a
	// for sta:
	#define STA__PlugStaChangeDet_Invalid		0
	#define STA__PlugStaChangeDet_Plugin			1
	#define STA__PlugStaChangeDet_Plugout		-1		

typedef enum
{
	m_eGetStaIdx_begin = 0,
	m_eGetStaIdx_mic = m_eGetStaIdx_begin,
	m_eGetStaIdx_aux,
	m_eGetStaIdx_total
} E_GET_STA_IDX;

#if SP_DEV_DET_PLUGIN_BY_AD
extern S_GET_STA		g_sGetSta[];
#endif

#endif	/* _DRV_ROCKER_I_H_ */
