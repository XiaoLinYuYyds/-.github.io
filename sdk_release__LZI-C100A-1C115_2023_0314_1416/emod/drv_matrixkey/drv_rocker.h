/*
*********************************************************************************************************
*											        eMOD
*						                    the Easy Module System
*                                          drv_touchpanel sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : drv_rocker.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _DRV_ROCKER_H_
#define _DRV_ROCKER_H_

#if 0
#define DebugPrintf(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
                                 eLIBs_printf(__VA_ARGS__)		)
#define __msg(...)		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
                       eLIBs_printf(__VA_ARGS__)		)
#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#else
#define DebugPrintf(...)
#define __msg(...)
#define __here__
#endif

#endif  /*_DRV_ROCKER_H_*/
