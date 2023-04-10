#ifndef		__NEW_MOVIE_SPSC_CTRL_MENU_UI_PARA_H
#define		__NEW_MOVIE_SPSC_CTRL_MENU_UI_PARA_H



#include	"new_movie_spsc_ctrl_menu_i.h"

#define	movie_spsc_ctrl_menu_icon_num	11

typedef	struct
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	
}new_movie_spsc_ctrl_menu_rect_t;/*控制菜单控件的UI位置和大小参数*/
typedef	struct
{
	HTHEME	res_theme;/*存放控件图标*/
}new_movie_spsc_ctrl_menu_icon_t;/*控制菜单控件的图标*/


typedef	struct
{
	new_movie_spsc_ctrl_menu_rect_t uipara_lyr;/*控制菜单图层大小*/
	new_movie_spsc_ctrl_menu_rect_t uipara_frm;/*控制菜单图层大小*/
	new_movie_spsc_ctrl_menu_rect_t uipara_bg;/*控制菜单图层背景*/
	new_movie_spsc_ctrl_menu_rect_t uipara_icon_retc[movie_spsc_ctrl_menu_icon_num];//11个图标位置
	new_movie_spsc_ctrl_menu_icon_t uipara_icon[movie_spsc_ctrl_menu_icon_num];//11个图标
	new_movie_spsc_ctrl_menu_icon_t uipara_bg_theme;/*控制菜单图层背景*/
}new_movie_spsc_ctrl_menu_uipara_t;/*控制菜单UI显示控件*/



new_movie_spsc_ctrl_menu_uipara_t *new_movie_spsc_ctrl_menu_get_uipara(__s32 rotate);


#endif


