/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : spectrum.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _NEW_SPCTRM_H_
#define _NEW_SPCTRM_H_

/* define message para   */
typedef enum        /* user define msg id */
{
  NEW_SPCTRM_ID_UPDATE = GUI_MSG_,
  NEW_SPCTRM_ID_SET_NUM
} __new_spctrm_cmdid_t;

H_WIN NEW_SPCTRM_SceneCreate(H_WIN h_parent, H_WIN lyrwin);
void  NEW_SPCTRM_Update(H_WIN hwin);
void  NEW_SPCTRM_Clear(__u32 value);					//add by libaiao,2011.5.10

#endif
