/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: gscene_headbar.h
* By      	: Andy.zhang
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2010-9-25 19:19:34 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __GSCENE_HEADBAR_H__
#define __GSCENE_HEADBAR_H__

typedef enum _HBarState
{
  HBAR_ST_HIDE,
  HBAR_ST_SHOW,
  HBAR_ST_SHOW_WIDTH_VOLUME,//显示的同时显示音量条
} HBarState;

typedef enum tag_HBAR_FORMAT
{
  HBAR_FOARMAT_8BPP,
  HBAR_FOARMAT_32BPP
} __hbar_format_t;
typedef enum
{
  BG_NONE = 0,
  BG_MUSIC,
  BG_AUX,

} __bg_audio_t;

typedef struct
{
	char 			*name;		// 场景名称
	__u16 			sid;		// 场景id
	H_WIN 			parent;		// 父窗口
	H_WIN 			layer;		// 图层
	GUI_FONT    	*font;		// 字体句柄
	__hbar_format_t format;		//format
} HBarCreatePara;

__s32 gscene_hbar_create(H_WIN parent, __hbar_format_t format);
__s32 gscene_hbar_set_state(HBarState state);
__s32 gscene_hbar_get_state(HBarState *p_state);
__s32 gscene_hbar_set_title(char *title, __u32 len);	/* title 为utf8编码格式字符串，len<=32 ,设置hbar title 区域字符串*/
//__s32 gscene_hbar_set_volume(void);
__s32 gscene_hbar_set_format(__hbar_format_t format);
__s32 gscene_hbar_get_screen_rect(RECT *rect);
__s32 gscene_hbar_delete(void);
__s32 gscene_hbar_set_bt(__bool staOn);


#endif /* __GSCENE_HEADBAR_H__ */


