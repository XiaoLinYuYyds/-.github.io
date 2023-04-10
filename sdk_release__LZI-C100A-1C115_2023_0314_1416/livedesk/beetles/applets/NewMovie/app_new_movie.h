/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:app_movie.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05   Bayden.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_NEW_MOVIE_H__
#define  __APP_NEW_MOVIE_H__


typedef struct app_new_movie_ctrl
{
	
	void *progressbar_scene;	/*存放进度条的frmwin窗口句柄*/
	void *spsc_ctrl_scene;
	H_WIN new_movie_manager;	/*movie的manwin窗口句柄*/
	H_WIN new_spsc_ctrl_menu_manager;/*视频控制菜单framewin窗口*/

	ES_FILE *hdisp;				/*显示驱动句柄*/
	__hdle	 hvedio_lyr;		/*视频图层*/

	__u8	 auto_play_timmer_id;/*自动播放定时器id*/

	__bool   exit_movie;		/*是否退出movie*/
	
	__s32    rr_flag;			/*是否进行了快退操作，用于快退到尽头时，做判断用*/

	__s32 stop_cnt ;			/*连续stop_cnt次检测到stop，才播下一首，防止还没播起来就跳到下一首*/
	__s32 cur_time;				/*记录当前播放时间，用于制造一个视频断点信息*/
	char  cur_playing_file[RAT_MAX_FULL_PATH_LEN + 1];/*存放当前播放的文件名*/
	__bool quit_by_fs_part_plug_out;/*如果退出movie了，是否拔卡造成的退出*/
	
	__s32 btv_out;				/*显示输出类型*/
	GUI_FONT *time_font;

}
new_movie_ctrl_t;


H_WIN app_new_movie_manwin_create(root_para_t *para);
__s32 app_new_movie_is_tv_out(H_WIN hmanwin);

#endif
