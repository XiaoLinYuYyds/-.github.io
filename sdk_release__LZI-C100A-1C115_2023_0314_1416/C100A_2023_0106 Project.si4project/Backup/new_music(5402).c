#include "new_music.h"

#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR
#define COLOUR_TO_8BPP_INDEX_0          0xF0


//播放当前音乐文件
static __s32 PlayCurMusicFile(new_music_ctrl_t *this, char *File)
{
	__s32 ret=0;
	__s32 index=-1;
	char Filename[FSYS_DIRNAME_MAX];
	char cur_playing_file[RAT_MAX_FULL_PATH_LEN];//存放当前播放文件
	eLIBs_memset(Filename, 0, sizeof(Filename));
	if(EPDK_FALSE == robin_is_open()){
		__wrn("robin_is_open is open false...\n");
	}
	//index = robin_npl_set_cur(0);		//设置当前播放的文件id
	rat_npl_set_cur(this->rat.rat_handle, 0);
	__wrn("robin_npl_set_cur index =%d\n", index);
	index = robin_npl_get_total_count();//获取当前播放列表的文件总数
	__wrn("total index =%d\n", index);
	index = robin_npl_get_cur();		//获取当前播放文件索引
	__wrn("cur index =%d\n", index);
	if(-1 == index)
	{
		index = 0;
		__wrn("robin_npl_get_cur fail...\n");
		//return EPDK_FAIL;
	}
	eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
	//ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
	if(-1 == ret)//索引成功
	{
		__wrn("robin_get_fsm_cur_file is failes\n");

	}
	__wrn("robin_get_fsm_cur_file cur_playing_file=%s\n", cur_playing_file);
	
	//ret = robin_npl_index2file(index, Filename);//通过索引获取文件名
	__wrn("ret =%d\n", ret);
	__wrn("robin_npl_index2file filename=%s\n", File);

	if(0 == ret)//索引成功
	{
		//robin_set_cmd_stop();//设置停止播放命令
		eLIBs_strcpy(Filename, File);
		robin_play_file(Filename,NULL);//直接播放文件
		__wrn("robin_play_file filename=%s\n", Filename);

		robin_set_cmd_play();//设置播放命令

	}
	else
	{
		__wrn("ANOLE_PlayCurFile fail\n");
	}

	return ret;
}
//播放音乐文件
static __s32 new_music_play_file(new_music_ctrl_t *this, char *filename, __u32 play_index)
{
	//__u32 play_index=4;
	__s32 amplifier_flag;
	__s32 ret;
	char  cur_playing_file[RAT_MAX_FULL_PATH_LEN];//存放当前播放文件
	
	if(this  == NULL)
	{
		return EPDK_FAIL;
	}
	eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
	ret = robin_npl_index2file(play_index, cur_playing_file);
	robin_set_cmd_stop();
	
	__wrn("robin_npl_index2file cur_playing_file=%s\n", cur_playing_file);
	
	robin_npl_set_cur(play_index);
	__wrn("play_index=%d\n", play_index);

	
	
	ret = robin_play_file(cur_playing_file, NULL);
	__wrn("ret=%d\n", ret);
	robin_set_cmd_play();//设置播放命令

	amplifier_flag = dsk_amplifier_is_on();//获取功放状态
		__wrn("amplifier_flag=%d\n", amplifier_flag);
	
	return EPDK_OK;
}
//开始播放音乐文件
static __s32 new_music_play_start(new_music_ctrl_t *this,__u32 play_index)
{
	char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名

	new_music_play_file(this, FileName, play_index);
}

//申请图片资源
static __s32 new_music_uipara_init(new_music_ctrl_t *ctrl_para)
{
	ctrl_para->new_music_uipara  =	get_new_music_uipara(0);
	if(!ctrl_para->new_music_uipara){
		__wrn("new_music_uipara is 0...\n");
	}
	//eLIBs_memset(ctrl_para->scene_para->progressbar_bmp, 0, sizeof(ctrl_para->scene_para->progressbar_bmp))
	__wrn("new_music_uipara_init is 1...\n");
	if(!ctrl_para->scene_para[0].progressbar_bmp){//进度条背景图片
		ctrl_para->scene_para[0].progressbar_bmp = theme_open(ID_MUSIC_PROCESS_BAR_BODY_BMP);
		__wrn("new_music_uipara_init is 2...\n");
	}
	if(!ctrl_para->scene_para[1].progressbar_bmp){//进度条移动部分图片
		ctrl_para->scene_para[1].progressbar_bmp = dsk_theme_open(ID_MUSIC_PROCESS_BAR_POINT_BMP);
		__wrn("new_music_uipara_init is 3...\n");
	}

	/*if(!ctrl_para->bmp[0]){
		ctrl_para->bmp[0] = dsk_theme_open(ID_MUSIC_PROCESS_BAR_BODY_BMP);
		__wrn("new_music_uipara_init is 4...\n");
	}

	if(!ctrl_para->bmp_music){
		ctrl_para->bmp_music = theme_open(ID_MUSIC_PROCESS_BAR_HEAD_BMP);
		__wrn("new_music_uipara_init is 5...\n");
	}*/
	
	if(!ctrl_para->scene_uipara.progress_bmp[0]){//频谱背景图片
		ctrl_para->scene_uipara.progress_bmp[0] = theme_open(ID_MUSIC_PARTICLE_BMP);
		__wrn("new_music_uipara_init is 6...\n");
	}

	return EPDK_OK;
}
//释放图片资源
static __s32 new_music_uipara_deinit(new_music_ctrl_t *ctrl_para)
{
	ctrl_para->new_music_uipara  =	get_new_music_uipara(0);
	
	if(ctrl_para->scene_para[0].progressbar_bmp){//进度条背景图片
		theme_close(ctrl_para->scene_para[0].progressbar_bmp);
		ctrl_para->scene_para[0].progressbar_bmp = NULL;
	}
	if(ctrl_para->scene_para[1].progressbar_bmp){//进度条移动部分图片
		dsk_theme_close(ctrl_para->scene_para[1].progressbar_bmp);
		ctrl_para->scene_para[1].progressbar_bmp = NULL;
	}
	/*if(ctrl_para->bmp[0]){//进度条移动部分图片
		dsk_theme_close(ctrl_para->bmp[0]);
		ctrl_para->bmp[0] = NULL;
	}
	if(ctrl_para->bmp_music){//进度条移动部分图片
		theme_close(ctrl_para->bmp_music);
		ctrl_para->bmp_music = NULL;
	}*/
	if(ctrl_para->scene_uipara.progress_bmp[0]){//频谱图片
		theme_close(ctrl_para->scene_uipara.progress_bmp[0]);
		ctrl_para->scene_uipara.progress_bmp[0] = NULL;
	}
	return EPDK_FALSE;
}
//绘制图片
static __s32 new_music_paint_one_item(__gui_msg_t *msg)
{
	new_music_ctrl_t	*new_music_ctrl;
	GUI_RECT	gui_rect_bj,gui_rect;
	GUI_RECT 	bmp_rect,music_rect;
	void	*theme_bmp[5];
	__s32 i;
	new_music_ctrl	=	(new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)//检测图层是否开启状态
	{
		__wrn("new_music_paint_one_item the layer state is not open...\n");
		return EPDK_FAIL;
	}
	__wrn("new_music_paint_one_item is 1...\n");
	GUI_LyrWinSel(new_music_ctrl->framewin_create_para->new_music_hlyr);//选择该窗口的图层
	#if 1//绘制进度条图片
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
		//GUI_SetBkColor(GUI_LIGHTBLUE);
		gui_rect_bj.x0	=	new_music_ctrl->new_music_uipara->progressbar[0].x;
		gui_rect_bj.y0	=	new_music_ctrl->new_music_uipara->progressbar[0].y;
		gui_rect_bj.x1	=	gui_rect_bj.x0 + new_music_ctrl->new_music_uipara->progressbar[0].width;
		gui_rect_bj.y1	=	gui_rect_bj.y0 + new_music_ctrl->new_music_uipara->progressbar[0].heght;
		{
			__wrn(" theme_bmp[0]...\n");
			theme_bmp[0] = dsk_theme_hdl2buf(new_music_ctrl->scene_para[0].progressbar_bmp);
			GUI_BMP_Draw(theme_bmp[0], gui_rect_bj.x0, gui_rect_bj.y0);
			__wrn("gui_rect_bj.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect_bj.x0,gui_rect_bj.y0,gui_rect_bj.x1,gui_rect_bj.y1);
			
		}
		gui_rect_bj.x0	=	new_music_ctrl->new_music_uipara->progressbar[1].x;
		gui_rect_bj.y0	=	new_music_ctrl->new_music_uipara->progressbar[1].y;
		gui_rect_bj.x1	=	gui_rect_bj.x0 + new_music_ctrl->new_music_uipara->progressbar[1].width;
		gui_rect_bj.y1	=	gui_rect_bj.y0 + new_music_ctrl->new_music_uipara->progressbar[1].heght;
		{
			__wrn(" theme_bmp[0]...\n");
			theme_bmp[1] = dsk_theme_hdl2buf(new_music_ctrl->scene_para[1].progressbar_bmp);
			GUI_BMP_Draw(theme_bmp[1], gui_rect_bj.x0, gui_rect_bj.y0);
			__wrn("gui_rect_bj.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect_bj.x0,gui_rect_bj.y0,gui_rect_bj.x1,gui_rect_bj.y1);
			
		}
		
		for(i=0;i<12;i++){
		//GUI_SetBkColor(0);
		gui_rect.x0	=	(i*33)+new_music_ctrl->new_music_uipara->spectrogram.x;//162+i*20
		gui_rect.y0	=	new_music_ctrl->new_music_uipara->spectrogram.y;//320
		gui_rect.x1	=	gui_rect.x0 + new_music_ctrl->new_music_uipara->spectrogram.width;//15+162+i*20
		gui_rect.y1	=   new_music_ctrl->new_music_uipara->spectrogram.heght;//150
		{
			__wrn(" theme_bmp[1]...\n");
			theme_bmp[4] = dsk_theme_hdl2buf(new_music_ctrl->scene_uipara.progress_bmp[0]);
			GUI_BMP_Draw(theme_bmp[4], gui_rect.x0, gui_rect.y0);
			__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			
		}
		}
		//GUI_SetBkColor(0xF0);//透明
		GUI_SetBkColor(GUI_LIGHTBLUE);
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形框控件的背景颜色为黑色
		
	#endif
	return EPDK_OK;
}
//rat模块初始化，文件搜索打开
static	__s32 new_music_rat_init(new_music_ctrl_t *this)
{
	if(this->rat.rat_handle == NULL){//文件搜索打开
		this->rat.rat_handle = rat_open_ex(RAT_TF, RAT_MEDIA_TYPE_AUDIO, 0);//打开TF卡

		if(this->rat.rat_handle == NULL){
			__wrn("rat.rat_handle is null!...\n");
			return EPDK_FALSE;
		}
		else{
			__wrn("open rat File Search successed!...\n");
		}
	}
	

	this->rat.total = rat_get_cur_scan_cnt(this->rat.rat_handle);
	__wrn("this->rat.total = %d...\n",this->rat.total);

	return EPDK_OK;
}

/************************************************************************************************************************
*Description	:		从全路径中获取文件名
************************************************************************************************************************/
static __s32 new_music_ExtractFileName(char *FileName, char *FilePath)
{
	char *pFileName;
	__wrn("get Extract File Name is...!!\n");
	pFileName = eLIBs_strchrlast(FilePath, '\\');//汉字识别
	pFileName++;
	eLIBs_strcpy(FileName, pFileName);
	return EPDK_OK;
}
/************************************************************************************************************************
*Description	:	Get List Item File Full Path	
*					根据索引号获得条目文件名全路径
************************************************************************************************************************/
static __s32 new_music_GetListItemFileFullPath(new_music_ctrl_t *list_para, __s32 ItemIndex, char *FilePath)
{
	int ret;
	rat_entry_t ItemInfo;
	eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
	if(!list_para->rat.rat_handle){
		__wrn("rat_handle is null!!\n");
	}
	ret = rat_get_item_info_by_index(list_para->rat.rat_handle, ItemIndex, &ItemInfo);//获取当前项目信息索引
	__wrn("get List Item File Full Path is...!!\n");
	if(ret == EPDK_FAIL)
	{
		__wrn("get file information form rat fail!!\n");
		return EPDK_FAIL;
	}

	eLIBs_strcpy(FilePath, ItemInfo.Path);			//不去操作rat 内存
	return EPDK_OK;
}

/************************************************************************************************************************
*Description	:		根据索引号获得条目文件名(只有文件名，供显示)
************************************************************************************************************************/
static __s32 new_music_GetListItemFileName(new_music_ctrl_t *list_para, __s32 ItemIndex, char *FileName)
{
	int ret;
	char FilePath[RAT_MAX_FULL_PATH_LEN] = "";
	eLIBs_memset(FilePath, 0, sizeof(FilePath));//数组初始化数据清0
	ret = new_music_GetListItemFileFullPath(list_para, ItemIndex, FilePath);//通过索引获取当前条目文件名的全路径,返回EPDK_OK
	__wrn("get List Item File Name is...!!\n");
	if(ret == EPDK_FAIL)
	{
		return EPDK_FAIL;
	}

	new_music_ExtractFileName(FileName, FilePath);//从全路径获取文件名
	__wrn("get List Item Extract File Name is ok ok...!!\n");
	return EPDK_OK;
}


//初始化音频设备
static __s32 new_music_player_device_init(new_music_ctrl_t *this)
{
	__s32 ret,total=-1,total_count;	
	HRATNPL hRatNpl;
	
	__wrn("_init begining!\n");
	
	//this->framewin_create_para->auto_play_timmer_id = 0x01;//定时器初始化
	//this->play_mode	=	RAT_PLAY_MODE_ROTATE_ALL;//播放模式
	//app_print_memory_usage("robin create", NULL);
	/*open display*/
	//ADE_Open();
	ret = robin_open(ROBIN_MODE_AUDIO_MAX, NULL);//打开robin模块接口
	if(EPDK_OK != ret)
	{
		__wrn("robin_open fail...\n");
		return EPDK_FAIL;
	}
#if 1//单独打开音乐rat_npl_open列表
	 hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);//音频文件
	 __wrn("hRatNpl = %d\n",hRatNpl);
	 total_count =  rat_get_cur_scan_cnt(hRatNpl);
	 __wrn("total_count 1= %d\n",total_count);

	 total_count = robin_npl_get_total_count();
	__wrn("total_count 2=%d\n", total_count);
	
	 rat_npl_set_cur(hRatNpl, 0);
	 total = rat_npl_get_cur(hRatNpl);
	 __wrn("total = %d\n",total);
#endif
	//robin_enable_spectrum();//使能打开robin模块的频谱接口
	//robin_set_eq_mode(this->EQ_mode);//设置均衡器接口
	//music_player_set_robin_EQ_mode(this);
	//robin_set_ff_rr_speed(MUSIC_FF_SPEED);//快进或后退速度
	//this->play_mode = RAT_PLAY_MODE_SEQUENCE;
	robin_set_play_mode(this->play_mode);//设置播放模式
	//GUI_SetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
	//app_print_memory_usage("robin create finished", NULL);
	return EPDK_OK;
}

//音乐播放初始化
static __s32 new_music_init(__gui_msg_t *msg)
{
	__s32			ret=0,total=-1;
	HRATNPL		H;

	new_music_ctrl_t *new_music_ctrl;
	robin_open_arg_t arg;
	char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名，
	new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	new_music_ctrl->framewin_create_para->auto_play_timmer_id = 0x01;//定时器初始化
	new_music_ctrl->play_mode	=	RAT_PLAY_MODE_ROTATE_ALL;//播放模式
	/*H = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);
	__wrn("rat_npl_open H=%d...\n",H);
	total = rat_npl_get_total_count(H);
	__wrn("rat_npl_get_total_count total=%d...\n",total);
	ret = rat_npl_get_cur(H);
	__wrn("rat_npl_get_cur ret=%d...\n",ret);*/
	//rat_npl_file2index(0, );
	//arg.cedar_stop_mode = CEDAR_STOP_MODE_UNINSTALL_PLUGINS;	/*停止播放时,卸载所有插件*/
	//arg.switch_vplay_mode = CEDAR_FILE_SWITCH_VPLY_MODE_GAP;	/*2个文件切换*/
	//arg.reserve_mem_size = 1024 * 1024;	//512*1024;				/*预留1M字节*/
	//ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);				/*打开robin模块音频接口*/
	//ret = robin_open(ROBIN_MODE_AUDIO_MAX, NULL);				/*打开robin模块接口*/
	//new_music_player_set_play_mode(new_music_ctrl);
		


	if(EPDK_OK != ret)
	{
		__wrn("robin_open fail...\n");
		return EPDK_FAIL;
	}

	if(ret == -1)
	{
		__wrn("robin_init failed!\n");
		return EPDK_FAIL;
	}
	/*eLIBs_memset(FileName, 0, sizeof(FileName));//数组数据初始化，清0
	new_music_GetListItemFileName(new_music_ctrl, 0, FileName);
	__wrn("\FileName = %s\n",FileName);*/

	//PlayCurMusicFile(new_music_ctrl);//播放音乐文件
	return EPDK_TRUE;
}

//保存注册表信息直接操作结构体
__s32 new_music_player_save_last_para(new_music_ctrl_t *This)
{
	__s32 ret = 0;
	reg_music_para_t *music_last_para;
	reg_system_para_t *system_last_para = NULL;
	system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
	music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);
	//music_last_para->EQ_mode = This->EQ_mode;
	music_last_para->play_mode = This->play_mode;

	return EPDK_OK;
}

//设置播放模式
static __s32 new_music_player_set_play_mode(new_music_ctrl_t *this)
{
	switch(this->play_mode)//播放模式
	{
		case RAT_PLAY_MODE_SEQUENCE:
			this->play_mode = RAT_PLAY_MODE_ROTATE_ONE;
			break;

		case RAT_PLAY_MODE_ROTATE_ONE:
			this->play_mode = RAT_PLAY_MODE_ROTATE_ALL;
			break;

		case RAT_PLAY_MODE_ROTATE_ALL:
			this->play_mode = RAT_PLAY_MODE_ONLY_ONCE;
			break;

		case RAT_PLAY_MODE_ONLY_ONCE:
			this->play_mode = RAT_PLAY_MODE_RANDOM;
			//this->play_mode = RAT_PLAY_MODE_SEQUENCE;
			break;

		case RAT_PLAY_MODE_RANDOM:
			this->play_mode = RAT_PLAY_MODE_SEQUENCE;
			break;

		default:
			break;
	}

#if 0
	DrawMusicPlayModeButton(this->play_mode, PRESSED);
#endif
	//DrawMusicPlayModeStatus(this->play_mode);
	robin_set_play_mode(this->play_mode);//设置播放模式
	return EPDK_OK;
}

//自动播放定时器
static __s32 auto_palymusic_tiemer(new_music_ctrl_t *new_music_ctrl,__gui_msg_t *msg)
{
	if(!new_music_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	
	__wrn("new_movie_ctrl->auto_play_timmer_id = 0x%x\n",new_music_ctrl->framewin_create_para->auto_play_timmer_id);
	#if 1
	if(!GUI_IsTimerInstalled(new_music_ctrl->framewin_create_para->new_music_hfrm, new_music_ctrl->framewin_create_para->auto_play_timmer_id))/*定时器加载*/
	{
		GUI_SetTimer(new_music_ctrl->framewin_create_para->new_music_hfrm, new_music_ctrl->framewin_create_para->auto_play_timmer_id
		             , 200, NULL);//0.5秒，设置定时器
		__wrn("GUI_SetTimer timmer is 1...\n");
	}
	#else
	if(!GUI_IsTimerInstalled(msg->h_deswin, new_music_ctrl->framewin_create_para->auto_play_timmer_id))/*定时器加载*/
	{
		GUI_SetTimer(msg->h_deswin, new_music_ctrl->framewin_create_para->auto_play_timmer_id
		             , 200, NULL);//0.5秒，设置定时器
		__wrn("GUI_SetTimer timmer is 2...\n");
	}
	#endif
	
	return EPDK_OK;
}

//回调处理函数
static __s32 __new_music_proc(__gui_msg_t *msg)
{
	__wrn("msg->id = %d\n",msg->id);
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			new_music_ctrl_t	*new_music_ctrl;
			new_music_create_para_t	*new_music_create_para;
			char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名，
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_HIDE);
			
			new_music_create_para	=	(new_music_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
			new_music_ctrl	=	(new_music_ctrl_t *)esMEMS_Balloc(sizeof(new_music_ctrl_t));
			//gscene_bgd_set_state(BGD_STATUS_HIDE);//背景场景隐藏
			if(!new_music_ctrl){
				__wrn("new_music_ctrl is balloc is failes...\n");
			}
			eLIBs_memset(new_music_ctrl, 0, sizeof(new_music_ctrl_t));//结构体初始化，数据清0
			new_music_ctrl->framewin_create_para = new_music_create_para;//framewin窗口创建参数
			new_music_create_para->new_music_hfrm = msg->h_deswin;	//窗口赋值
			GUI_WinSetAddData(msg->h_deswin, (__u32)new_music_ctrl);

			new_music_uipara_init(new_music_ctrl);//申请图片资源
			new_music_player_save_last_para(new_music_ctrl);//注册表初始化
			__wrn("new_music_uipara_init is ok...\n");
			//new_music_paint_one_item(msg);
			__wrn("new_music_paint_one_item is ok...\n");
			new_music_rat_init(new_music_ctrl);//rat模块文件搜索打开初始化

			#if 1	//获取媒体类型文件的文件名
				eLIBs_memset(FileName, 0, sizeof(FileName));//数组数据初始化，清0
				new_music_GetListItemFileName(new_music_ctrl, 0, FileName);
				__wrn("\FileName = %s\n",FileName);
			#endif
			rat_set_file_for_play(new_music_ctrl->rat.rat_handle, FileName);
			new_music_player_device_init(new_music_ctrl);//音乐播放设备初始化

			#if 1	//直接播放音乐文件
			//PlayCurMusicFile(new_music_ctrl,FileName);			 //播放音乐文件
			new_music_play_file(new_music_ctrl, FileName,0);//播放音乐文件
			#else	//开始播放音乐文件
			new_music_play_start(new_music_ctrl,1);//开始播放
			#endif
			new_music_init(msg);						 //音乐播放初始化
			//new_music_player_set_play_mode(new_music_ctrl);//设置播放模式
			new_music_ctrl->framewin_create_para->auto_play_timmer_id = 0x01;//定时器初始化
			GUI_LyrWinSetSta(new_music_ctrl->framewin_create_para->new_music_hlyr, GUI_LYRWIN_STA_ON);//窗口图层设置开启状态
			GUI_LyrWinSetTop(new_music_ctrl->framewin_create_para->new_music_hlyr);	
			new_music_paint_one_item(msg);
			auto_palymusic_tiemer(new_music_ctrl,msg);//开启播放定时器
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1，销毁*/
			new_music_ctrl_t	*new_music_ctrl;
			new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			new_music_uipara_deinit(new_music_ctrl);//释放图片
			/*if(new_music_ctrl->framewin_create_para->new_music_hlyr)//释放图层
			{
				eLIBs_memset(new_music_ctrl->framewin_create_para->new_music_hlyr, 0, sizeof(new_music_ctrl->framewin_create_para->new_music_hlyr));
				new_music_ctrl->framewin_create_para->new_music_hlyr = NULL;
			}*/
			if(new_music_ctrl->framewin_create_para){
				esMEMS_Bfree(new_music_ctrl->framewin_create_para,sizeof(new_music_create_para_t));
				new_music_ctrl->framewin_create_para = NULL;
			}
			if(new_music_ctrl){
				esMEMS_Bfree(new_music_ctrl,sizeof(new_music_ctrl_t));
				new_music_ctrl = NULL;
			}
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2，绘制*/
			//new_music_paint_one_item(msg);
			__wrn("new_music_paint_one_item is ok...\n");

		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{/*3，关闭*/

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{/*4，退出*/
			__wrn("send GUI_MSG_QUIT cmd is start...\n");
			app_new_music_cmd2para(msg->h_deswin, GUI_MSG_QUIT, 0, 0);
		}
		return EPDK_OK;

		case GUI_MSG_KEY:{/*5，按键*/

			switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{
					__gui_msg_t mmsg;
					mmsg.id = GUI_MSG_QUIT;
					mmsg.h_deswin=msg->h_deswin;
					mmsg.dwAddData1=0;
					GUI_SendNotifyMessage(&mmsg);
				}
			}
		}
		break;

		case GUI_MSG_COMMAND:{/*7，命令*/

		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{
			
			new_music_ctrl_t *new_music_ctrl;
			__s32 ret,amplifier_onoff,fsm_sta;
			__u32 play_index;
			__u32 cur_time=0,total_time=0;
			ret = -1;
			new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__wrn("send GUI_MSG_TIMER cmd is start...\n");
			if(new_music_ctrl)
			{
				if(LOWORD(msg->dwAddData1) == new_music_ctrl->framewin_create_para->auto_play_timmer_id)
				{

					{
						cur_time = robin_get_cur_time();
						__wrn("cur_time = %d...\n",cur_time);
						total_time = robin_get_total_time();
						__wrn("total_time = %d...\n",total_time);
						play_index = robin_npl_get_cur();//获取当前播放列表的文件索引id
						
						amplifier_onoff = dsk_amplifier_is_on();//获取功放状态
						__wrn("amplifier_onoff 1 = %d...\n",amplifier_onoff);
						
						if(total_time == cur_time){//判断播放一首完成
							play_index += 1;
							if(play_index > 25){
								play_index = 0;
							}
							__wrn("play_index = %d...\n",play_index);
							new_music_play_start(new_music_ctrl, play_index);//通过索引id播放音乐文件
						}

						fsm_sta = robin_get_fsm_status();
						if(fsm_sta == CEDAR_STAT_PLAY){//播放状态
							dsk_amplifier_onoff(1);//打开功放，声音打开
						}
						else if((fsm_sta == CEDAR_STAT_PAUSE)||
								(fsm_sta == CEDAR_STAT_STOP)){//暂停状态
							dsk_amplifier_onoff(0);//关闭功放，声音关闭
							
						}
						amplifier_onoff = dsk_amplifier_is_on();//获取功放状态
						__wrn("amplifier_onoff 2 = %d...\n",amplifier_onoff);
					}
				}
			}
			
		}
		break;
		
		default:
			break;

	}


	return GUI_ManWinDefaultProc(msg);
}

//创建manwin窗口
H_WIN	new_music_frmwin_create(H_WIN h_parent, new_music_create_para_t *para)
{
	__gui_framewincreate_para_t	framewin_para;
	new_music_create_para_t *new_music_create_para;
	FB	fb;
	#if 0 //检测内存是否泄漏
		esKSRV_SysInfo();
	#endif
	GUI_LyrWinGetFB(para->new_music_hlyr, &fb);
	new_music_create_para = (new_music_create_para_t *)esMEMS_Balloc(sizeof(new_music_create_para_t));

	eLIBs_memset(new_music_create_para, 0, sizeof(new_music_create_para_t));
	new_music_create_para->font = para->font;//文本
	new_music_create_para->root_type = para->root_type;//TF卡模式
	new_music_create_para->new_music_hlyr = para->new_music_hlyr;//传入图层
	
	
	__wrn("h_parent = %x \n",h_parent);
	/*开始操作配置framewin窗口参数*/
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));//结构体初始化，数据清0
	framewin_para.name	=	"new_music_framewin";
	framewin_para.dwExStyle	=	WS_EX_NONE;
	framewin_para.dwStyle	=	WS_NONE | WS_VISIBLE;
	framewin_para.spCaption	=	NULL;
	framewin_para.hOwner	=	NULL;
	framewin_para.hHosting	=	h_parent;//framewin窗口句柄
	framewin_para.id		=	NEW_MUSIC_ID;
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__new_music_proc);
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width  = fb.size.width;
	framewin_para.rect.height = fb.size.height;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red   = 0;
	framewin_para.BkColor.green	= 0;
	framewin_para.BkColor.blue	= 0;
	framewin_para.attr	=	(void *)new_music_create_para;
	framewin_para.hLayer	=	para->new_music_hlyr;

	__wrn("hlayer = %x \n",framewin_para.hLayer);
	if(!framewin_para.hLayer){
		__wrn("hlayer is null...\n");
		return EPDK_FALSE;
	}
	
	return (GUI_FrmWinCreate(&framewin_para));//返回一个创建好的manwin窗口
	
}




