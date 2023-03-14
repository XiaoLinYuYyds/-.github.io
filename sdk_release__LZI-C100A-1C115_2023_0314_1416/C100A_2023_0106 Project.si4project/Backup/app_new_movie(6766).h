#ifndef		_APP_NEW_MOVIE_H_
#define 	_APP_NEW_MOVIE_H_


#include 	"app_new_movie_i.h"




















typedef  struct tag_app_new_movie_ctrl
{
	H_WIN	new_movie_win;
	H_LYR	new_movie_lyr;

	root_para_t  		*root_para;		//存放root根目录结构体变量
}new_movie_strl_t;










H_WIN app_new_movie_manwin_create(root_para_t *para);



#endif 

