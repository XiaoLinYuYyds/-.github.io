/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, JOHN, China
*											All	Rights Reserved
*
* File    : spctrm_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#include "beetles_app.h"

#include "new_spctrm_i.h"

#include "new_spctrm_uipara.h"

static __new_spctrm_uipara_t spectrum_uipara_400_240 =
{
	{
		270, 40, 160, 60			//frame win270,40,160,80
	},
	{
		//0, 0, (16+16/3)*NUM_SPCTRM, 60
		0, 0, 160, 60
	},
	ID_MUSIC_PARTICLE_BMP,	//height of  particle is 60, ID_MUSIC_PARTICLE_UF_800X480_BMP
	ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_800X480_BMP
};
static __new_spctrm_uipara_t spectrum_uipara_800_480 =
{
	{
		270 + 150 + 100, 40 + 70, 300 + 100, 150		//频谱的位置
	},
	{
		0, 0, 300 + 100, 150		//频谱控件大小
	},

	ID_MUSIC_PARTICLE_BMP,	//height of  particle is 60, ID_MUSIC_PARTICLE_UF_1024X600_BMP
	ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_1024X600_BMP
};

__new_spctrm_uipara_t *new_spctrm_get_uipara(void)
{
	__s32 			screen_width;
	__s32 			screen_height;
	dsk_display_get_size(&screen_width, &screen_height);

	if((screen_width == 480) && (screen_height == 272))
	{
		return &spectrum_uipara_400_240;
	}
	else if((screen_width == 800) && (screen_height == 480))
	{
		return &spectrum_uipara_800_480;
	}

	__msg("NULL ui para...\n");
	return &spectrum_uipara_800_480;
}


//
__s32 new_spctrm_get_screen_rect(RECT *screen_rect)
{
	__new_spctrm_uipara_t *ui_param;
	ui_param = new_spctrm_get_uipara();
	screen_rect->x = ui_param->para_frmwin.x;
	screen_rect->y = ui_param->para_frmwin.y;
	screen_rect->width = ui_param->para_frmwin.width;
	screen_rect->height = ui_param->para_frmwin.height;
	return EPDK_OK;
}


