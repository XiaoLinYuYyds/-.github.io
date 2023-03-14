
#ifndef  __APP_LINEIN_H_
#define  __APP_LINEIN_H_

#include "beetles_app.h"

typedef	struct tag_linein_para
{
	H_LYR			layer;
	GUI_FONT 	*font;
} linein_para_t;

H_WIN app_linein_create(root_para_t *para);

//mode 0:background 1:forground
__s32 app_linein_set_mode(H_WIN hwin, linein_mode_e mode);

//mode 0:background 1:forground
linein_mode_e app_linein_get_mode(H_WIN hwin);

extern __bool app_linein_has_bt(void);

#endif

