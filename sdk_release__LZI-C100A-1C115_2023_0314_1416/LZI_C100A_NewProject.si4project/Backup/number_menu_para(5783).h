#ifndef  __NUMBER_MENU_TIPS_H__
#define	 __NUMBER_MENU_TIPS_H__
#include "app_setting_i.h"


typedef struct number_menu_tips_para
{
	GUI_FONT *msgbox_font;
	H_LYR	  layer;
} number_menu_tip_para_t;

H_WIN number_menu_tips_win_create(H_WIN h_parent, number_menu_tip_para_t *para);
H_LYR number_menu_tips_layer_create(RECT *rect);




#endif

