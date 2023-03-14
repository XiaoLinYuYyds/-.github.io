
#ifndef __APP_LINEIN_PARA_H_
#define  __APP_LINEIN_PARA_H_
#include "app_linein_i.h"

typedef struct linein_uipara
{
	RECT linein_scene_uipara;
	RECT linein_tips_rect;
	RECT linein_bmp_rect;
	__s32 linein_btn_bmp[2];
} linein_uipara_t;

linein_uipara_t *app_linein_get_uipara(void);

#endif

