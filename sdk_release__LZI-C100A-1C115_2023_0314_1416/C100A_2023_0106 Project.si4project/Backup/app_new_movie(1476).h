#ifndef		_APP_NEW_MOVIE_H_
#define 	_APP_NEW_MOVIE_H_


#include 	"app_new_movie_i.h"




















typedef  struct tag_app_new_movie_ctrl
{
	H_WIN	new_movie_win;			/*存放manwin窗口的句柄*/
	H_LYR	new_movie_lyr;			/*存放manwin窗口图层的句柄*/

	ES_FILE 	*hdisp;				/*显示驱动句柄*/
	__hdle	 hvedio_lyr;			/*视频图层*/
	
	__u8	 auto_play_timmer_id;	/*存放自动播放定时器ID*/
	__u32 	 root_type;				/*存放SD卡模式或USB模式*/
	root_para_t  *root_para;		/*存放root根目录结构体变量*/
}new_movie_ctrl_t;










H_WIN app_new_movie_manwin_create(root_para_t *para);



#endif 

