#include "app_new_movie.h"



















































static	__s32	new_movie_scene_create(__gui_msg_t *msg)
{
	SIZE screen_size;
	RECT rect;
	dsk_display_get_size(screen_size.width, screen_size.height);
	#if 1
		rect.x = 0;
		rect.y = 0;
		rect.width	=	screen_size.width;
		rect.height	=	screen_size.height;
		__wrn("rect: x=%d, y=%d, width=%d, height=%d\n",rect.x, rect.y, rect.width, rect.height);
	#endif
	


}


static __s32	_new_movie_manager_win_proc(__gui_msg_t *msg)
{
	switch(msg->id){
	case GUI_MSG_CREATE:/*0*/
		gscene_hbar_set_state(HBAR_ST_HIDE);
		gscene_bgd_set_state(BGD_STATUS_SHOW);
		new_movie_scene_create(msg);
		return EPDK_OK;

	case GUI_MSG_PAINT:	/*1*/

		return EPDK_OK;
	case GUI_MSG_KEY:	/*5*/

		break;

	
	default:/*否则直接退出switch语句*/
		break;
	}

}




H_WIN app_new_movie_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	new_movie_strl_t 	*new_movie_ctrl;

	new_movie_ctrl = (new_movie_strl_t *)My_Balloc(sizeof(new_movie_strl_t));
	if(!new_movie_ctrl){
		__wrn("new_movie_ctrl memory balloc fail.........");
		return EPDK_FAIL;
	}
	eLIBs_memset(new_movie_ctrl, 0, sizeof(new_movie_strl_t));
	
	new_movie_ctrl->root_para	=	para;
	
	/*************开始操作manwin窗口配置**********************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t))
	create_info.name	 		=	"app_new_movie"
	create_info.hParent	 		=	para->h_parent;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_movie_manager_win_proc);
	create_info.id				=	APP_NEWMOVIE_ID;//0x9800地址
	create_info.attr			=	(void *)new_movie_ctrl;
	create_info.hHosting 		=	NULL;
	
	return(GUI_ManWinCreate(&create_info));/*返回一个创建好的manwin窗口句柄*/
}





