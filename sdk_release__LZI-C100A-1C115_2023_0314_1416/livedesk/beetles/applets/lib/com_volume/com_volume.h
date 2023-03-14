/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: volume.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __COM_VOLUME_H__
#define  __COM_VOLUME_H__

#include "beetles_app.h"
#include "com_volume_ui.h"

//发往父窗口的消息
typedef enum
{
  com_volume_scene_msg_timeout = 0,
  com_volume_scene_msg_volumeset,
  com_volume_scene_msg_num
} com_volume_scene_msg_t;


typedef enum
{
  m_eWinVolume_default = 0,
  m_eWinVolume_echo
} E_WIN_VOLUME;

typedef struct
{
	H_WIN hparent;
	__s32 scene_id;
	__s32 min_val;
	__s32 max_val;
	__s32 cur_val;
	E_WIN_VOLUME	m_eWinVolume;
} com_volume_create_para_t;


void *com_volume_scene_create(com_volume_create_para_t *create_para);
__s32 com_volume_scene_delete(void *handle);
__s32 com_volume_scene_set_focus(void *handle);

#endif
