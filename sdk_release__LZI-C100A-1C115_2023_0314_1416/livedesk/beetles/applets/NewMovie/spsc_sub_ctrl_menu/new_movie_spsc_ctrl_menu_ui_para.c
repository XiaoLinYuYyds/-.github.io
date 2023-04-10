
#include	"new_movie_spsc_ctrl_menu_ui_para.h"



new_movie_spsc_ctrl_menu_uipara_t uipara_1024_600 =
{
	{//控制菜单图层大小
		(1024-364)/2, 465, 364, 94
	},
	{//窗口图层大小
		0, 0, 364, 94
	},
	{//图层背景大小
		0, 0, 364, 94
	},
	{//图标的位置和大小
		{
			7, 1, 24, 18
		},
		{
			43, 1, 24, 18
		},
		{
			79, 1, 24, 18
		},
		{
			115, 1, 24, 18
		},
		{
			151, 1, 24, 18
		},
		{
			7, 1, 24, 18
		},
		{
			38, 1, 24, 18
		},
		{
			69, 1, 24, 18,
		},
		{
			100, 1, 24, 18
		},
		{
			131, 1, 24, 18,
		},
		{
			162, 1, 24, 18
		},
	},
	/*{//图标的图片源
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_NEXT_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},
		{
			ID_MOVIE_SPSC_PREV_UF_BMP
		},

	},*/




};





new_movie_spsc_ctrl_menu_uipara_t *new_movie_spsc_ctrl_menu_get_uipara(__s32 rotate)
{
	SIZE	screen_size;

	dsk_display_get_size(&screen_size.width, &screen_size.height);//获取LCD屏幕大小

	switch(rotate){
		case 0:{
			if((screen_size.width == 1024) && (screen_size.height == 600)){
				return &uipara_1024_600;
			}
		}
		break;

		case 1:
			break;
	}

	return &uipara_1024_600;
}


