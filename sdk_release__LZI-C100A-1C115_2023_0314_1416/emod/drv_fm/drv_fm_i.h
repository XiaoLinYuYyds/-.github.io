/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          ir keyboard driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : drv_irkey_i.h
* By      : jerry
* Version : V1.00
*********************************************************************************************************
*/
#ifndef  _DRV_POWER_I_H_
#define  _DRV_POWER_I_H_

#include "epdk.h"

//define fm module type
#define FM_MODULE_TYPE_RDA5820      				(1 << 0)
#define FM_MODULE_TYPE_TEA5767      				(1 << 1)
#define FM_MODULE_TYPE_RDA5807P					(1 << 2)
#define FM_MODULE_TYPE_RDA5807SS    				(1 << 3)//兼容RDA5807SP
#define FM_MODULE_TYPE_RDA4702	    				(1 << 4)
#define FM_MODULE_TYPE_RDA5807	    				(1 << 5)//兼容RDA5807P、RDA5807SP、RDA5807SS、RDA5807H系列、RDA5807N系列
#define FM_MODULE_TYPE_RDA5807M	    			(1 << 6)//兼容RDA5807P、RDA5807SP、RDA5807SS、RDA5807H系列、RDA5807N系列
#define FM_MODULE_TYPE_RTC6207M	    			(1 << 7)
#define FM_MODULE_TYPE_QN8065	    				(1 << 8)
#define FM_MODULE_TYPE_RTC6222	    				(1 << 9)


//config fm module type
#define FM_MODULE_TYPE      (FM_MODULE_TYPE_RDA5807M |FM_MODULE_TYPE_RTC6207M |FM_MODULE_TYPE_QN8065| FM_MODULE_TYPE_RTC6222)

typedef enum
{
  __FM_TYPE_START = 0,
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)
  __FM_TYPE__RDA5807,
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)
  __FM_TYPE__RTC6207,
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)
  __FM_TYPE__QN8065,
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
	__FM_TYPE__RTC6222,
#endif

  __FM_TYPE_TOTAL
} __fm_type_t;

typedef struct STRUCT_POWER_DEV
{
	__u32           status;
	ES_FILE        *iic_file;

} __fm_dev_t;

typedef struct STRUCT_POWER_DRV
{
	__u32           mid;
	__u32           used;
	__hdle			hReg;
	__fm_dev_t      fm_dev;
	__fm_type_t		fm_type;
} __fm_drv_t;

extern  ES_FILE        *fiic;
extern  __dev_devop_t  fm_dev_ops;
extern __u32			g_flagFmIicFail;

__s32 DRV_FM_MInit(void);
__s32 DRV_FM_MExit(void);
__mp *DRV_FM_MOpen(__u32 mid, __u32 mode);
__s32 DRV_FM_MClose(__mp *mp);
__u32 DRV_FM_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DRV_FM_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 DRV_FM_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif /*_DRV_POWER_I_H_*/


