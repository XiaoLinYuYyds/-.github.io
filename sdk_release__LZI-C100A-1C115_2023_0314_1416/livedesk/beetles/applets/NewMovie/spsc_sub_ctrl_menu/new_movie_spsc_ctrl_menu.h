#ifndef		__NEW_MOVIE_SPSC_CTRL_MENU_H
#define		__NEW_MOVIE_SPSC_CTRL_MENU_H



#include	"new_movie_spsc_ctrl_menu_i.h"




typedef	struct
{
	/*接收输入参数*/
	H_LYR hlyr;//传入图层
	H_WIN hparent;//传入窗口句柄
	__s32 scene_id;//传入控制菜单中控件显示的id
	
	/*内部参数*/
	H_WIN hfrm;//存放内部的framewin窗口句柄
	
}new_movie_spsc_ctrl_menu_scene_t;/*控制菜单场景*/

typedef enum
{
  STR_NEW_MOVIE_AUTO = 12,          //auto
  STR_NEW_MOVIE_STRECTCH,           //拉伸
  STR_NEW_MOVIE_ORIGINAL,           //原始
  STR_NEW_MOVIE_ACTUAL,             //真实比例
  STR_NEW_MOVIE_SUB_SIZE,           //大小

} new_movie_screen_size_sta;/*存放屏幕比例大小切换变量*/

typedef struct
{
	// 输入参数
	H_WIN hparent;	//存放从manwin窗口创建framewin窗口的句柄
	__s32 scene_id;	//存放从manwin窗口创建framewin窗口的图层中菜单控件的id
	H_LYR hlyr;		//存放从manwin窗口创建framewin窗口的图层

}new_movie_spsc_ctrl_menu_create_para_t;/*用于存放manwin窗口中创建视频控制菜单framewin窗口参数*/

void *new_movie_spsc_ctrl_menu_scene_create(new_movie_spsc_ctrl_menu_create_para_t *create_para);
__s32 new_movie_spsc_ctrl_menu_scene_set_focus(void *handle);

#endif
