#ifndef __MUSIC_LIST_H__
#define __MUSIC_LIST_H__

#include "beetles_app.h"
#if SP_FavoriteFiles
#define mstroe_up_max   100
#endif

typedef struct music_list_para_s
{
	H_WIN 	framewin;
	H_LYR 	layer;
	__bool	visiable;												//是否创建场景
	GUI_FONT *font;													//字体

	__s32   root_type;

	rat_media_type_t	media_type;									//媒体类型

	__s32  explorer_play_music;										//文件管理播放音乐
	char  *music_file_name;    										//文件管理播放音乐文件名
#if SP_FavoriteFiles
	char	is_stroe_up_flag[mstroe_up_max];
	__s32	stroe_up_flag;
#endif
} music_list_para_t;

typedef struct music_list_ctrl_s
{
	H_WIN 	framewin;
	H_LYR 	layer;
	__bool	visiable;												//是否创建场景

	__s32   root_type;
	__u32   last_start_id;
	__u32   last_focused_id;
	char 	*last_filename;											//最后一次浏览文件名
	//__s32   explorer_play_music;									//文件管理播放音乐
	//char*   music_file_name;    									//文件管理播放音乐文件名

	char 	search_path[RAT_MAX_FULL_PATH_LEN];				//搜索路径
	rat_media_type_t	media_type;									//媒体类型


	RECT 		area;												//整体显示区域
	GUI_FONT 	*font;

	H_LBAR 		listbar_handle;										//listbar 句柄
	HRAT   		rat_handle;											//搜索句柄
	__hdle      long_str_handle;									//长字符串滚动句柄
	__u32   	total;												//条目总数
	__u32    	play_index;											//当前播放文件索引号
#if SP_FavoriteFiles
	__s32	stroe_up_flag;
	char	is_stroe_up_flag[mstroe_up_max];
	__u32 	favoite_sign[mstroe_up_max];
#endif

	__listbar_scene_t	listbar_play_scene;							//当前播放列表的起始号和focusid号
	__s32(*OnDraw)(void *list_ctrl);
	__s32(*OnNextKey)(void *list_ctrl);
	__s32(*OnPreviousKey)(void *list_ctrl);
	__s32(*OnNextPageKey)(void *list_ctrl);
	__s32(*OnPreviousPageKey)(void *list_ctrl);
	__s32(*OnEnterKey)(void *list_ctrl);

	__s32(*OnTouchDown)(void *list_ctrl, __s32 x, __s32 y, __s32 direct, __s32 speed);
	__s32(*OnTouchMove)(void *list_ctrl, __s32 x, __s32 y, __s32 direct, __s32 speed);
	__s32(*OnTouchUp)(void *list_ctrl, __s32 x, __s32 y, __s32 direct, __s32 speed);
} music_list_ctrl_t;


//第一次搜索文件的个数
#define		C_SEARCH_FILE_CNT_AT_FIRST			0

extern void clear_music_list_area(void);
extern __s32 init_bg_music_list(music_list_ctrl_t *list_ctrl, music_list_para_t *list_para);
extern music_list_ctrl_t *new_music_list(music_list_para_t *list_para);
extern __s32 delete_music_list(music_list_ctrl_t *list_ctrl);
extern __s32 music_listbar_restore(music_list_ctrl_t *list_ctrl);

extern __s32 show_music_list(music_list_ctrl_t *list_ctrl);
extern __s32 music_list_set_focus_item(music_list_ctrl_t *list_ctrl, __u32 item_num);
#if SP_FavoriteFiles
extern __s32 MusicExtractFileName(char *FileName, char *FilePath);
#endif
extern __s32 MusicGetListItemFileFullPath(HRAT rat_handle, __s32 ItemIndex, char *FilePath);

//filename 为外部申请的内存或变量
extern __s32 get_music_list_focus_item_full_filename(music_list_ctrl_t *list_ctrl, char *filename);
extern __s32 music_list_long_string_stop_roll(music_list_ctrl_t *this);

extern __s32 music_list_long_string_stop(music_list_ctrl_t *this);


__s32 music_list_set_item_scene(music_list_ctrl_t *this, __listbar_scene_t *scene);
__s32 music_list_get_item_scene(music_list_ctrl_t *this, __listbar_scene_t *scene);
extern __s32 music_listbar_on_index(void *list_ctrl, __s32 index);


#endif



