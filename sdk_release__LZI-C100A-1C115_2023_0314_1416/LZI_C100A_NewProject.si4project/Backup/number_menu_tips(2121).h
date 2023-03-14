#ifndef  __NUMBER_MENU_TIPS_H__
#define	 __NUMBER_MENU_TIPS_H__
//#include "app_setting_i.h"
#include "app_home_i.h"
#include "number_menu_i.h"


typedef struct number_menu_numbers_para
{
	GUI_FONT *msgbox_font;
	H_LYR	  layer1;

	H_WIN		layer;
	GUI_FONT 	*smenu_font;
	__s32			root_type;
	__s8	 		sub_menu_id;
	__s32           focus_item;
} number_menu_number_para_t;

H_WIN number_menu_numbers_win_create(H_WIN h_parent, number_menu_number_para_t *para);
H_LYR number_menu_numbers_layer_create(RECT *rect);




#endif

