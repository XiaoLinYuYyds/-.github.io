/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                             audio driver Module
*
*                                    (c) Copyright 2009-2010, kevin China
*                                             All Rights Reserved
*
* File    : ir_remote.h
* By      : victor
* Version : V1.0
* Date    : 2010-12-31
*********************************************************************************************************
*/
#ifndef  _IR_REMOTE_H_
#define  _IR_REMOTE_H_


#define IR_IRQ_NUM              (6)//ir模块中断号
#define IR_KEY_CODE_TABLE_SIZE   256

/* NEC remoter user code ,defined by user */
#define IR_NEC_ADDR_CODE__0x7F80		(0x7f80) /* 银色学习型的红外遥控  */
#define IR_NEC_ADDR_CODE__0xFF00		(0xff00) /* 银色学习型的红外遥控  */
#define IR_NEC_ADDR_CODE__0xFE01		(0xfe01) /* 黑色pvp方案的红外遥控 */

#if ((PRJ_IR_ID & MASK_IR_NEC) == IR_NEC_0xFF00)
	#define IR_NEC_ADDR_CODE						IR_NEC_ADDR_CODE__0xFF00
#elif ((PRJ_IR_ID & MASK_IR_NEC) == IR_NEC_0x7F80)
	#define IR_NEC_ADDR_CODE						IR_NEC_ADDR_CODE__0x7F80
#elif ((PRJ_IR_ID & MASK_IR_NEC) == IR_NEC_0xFE01)
	#define IR_NEC_ADDR_CODE						IR_NEC_ADDR_CODE__0xFE01
#endif	


extern  __s32  Ir_key_Init(void);
extern  __s32  Ir_key_Exit(void);

#endif  /*_IR_REMOTE_H_*/

