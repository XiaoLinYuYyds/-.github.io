#include "new_music.h"

#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR
#define COLOUR_TO_8BPP_INDEX_0          0xF0

static __s32 key_up_action_flag=0;//按键松开标志

#if 1 //音乐播放操作
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
	//robin_set_cmd_stop();
	
	__wrn("robin_npl_index2file cur_playing_file=%s\n", cur_playing_file);
	
	robin_npl_set_cur(play_index);
	__wrn("play_index=%d\n", play_index);

	
	
	ret = robin_play_file(cur_playing_file, NULL);
	__wrn("ret=%d\n", ret);
	//robin_set_cmd_play();//设置播放命令

	amplifier_flag = dsk_amplifier_is_on();//获取功放状态
		__wrn("amplifier_flag=%d\n", amplifier_flag);
	
	return EPDK_OK;
}
//开始播放音乐文件
static __s32 new_music_play_start(new_music_ctrl_t *this,__u32 play_index)
{
	char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名

	new_music_play_file(this, FileName, play_index);//播放音乐文件
}
#endif

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
	
	if(!ctrl_para->scene_uipara.progress_bmp[1]){//频谱移动图片
		ctrl_para->scene_uipara.progress_bmp[1] = theme_open(ID_MUSIC_BOUY_BMP);
		__wrn("new_music_uipara_init is 6...\n");
	}

	return EPDK_OK;
}
//释放图片资源
static __s32 new_music_uipara_deinit(new_music_ctrl_t *ctrl_para)
{
	ctrl_para->new_music_uipara  =	get_new_music_uipara(0);
	__wrn("new_music_uipara_deinit is 1...\n");
	if(ctrl_para->scene_para[0].progressbar_bmp){//进度条背景图片
		theme_close(ctrl_para->scene_para[0].progressbar_bmp);
		ctrl_para->scene_para[0].progressbar_bmp = NULL;
	__wrn("new_music_uipara_deinit is 2...\n");
	}
	if(ctrl_para->scene_para[1].progressbar_bmp){//进度条移动部分图片
		dsk_theme_close(ctrl_para->scene_para[1].progressbar_bmp);
		ctrl_para->scene_para[1].progressbar_bmp = NULL;
	__wrn("new_music_uipara_deinit is 3...\n");
	}
	/*if(ctrl_para->bmp[0]){//进度条移动部分图片
		dsk_theme_close(ctrl_para->bmp[0]);
		ctrl_para->bmp[0] = NULL;
	}
	if(ctrl_para->bmp_music){//进度条移动部分图片
		theme_close(ctrl_para->bmp_music);
		ctrl_para->bmp_music = NULL;
	}*/
	if(ctrl_para->scene_uipara.progress_bmp[0]){//频谱背景图片
		theme_close(ctrl_para->scene_uipara.progress_bmp[0]);
		ctrl_para->scene_uipara.progress_bmp[0] = NULL;
	__wrn("new_music_uipara_deinit is 4...\n");
	}
	if(ctrl_para->scene_uipara.progress_bmp[1]){//频谱移动图片
		theme_close(ctrl_para->scene_uipara.progress_bmp[1]);
		ctrl_para->scene_uipara.progress_bmp[1] = NULL;
	__wrn("new_music_uipara_deinit is 5...\n");
	}
	return EPDK_FALSE;
}

static __s32 CleanMusicPlayingTime(void)
{
	GUI_RECT text_rect;
	new_music_uipara_t *player_ui;
	player_ui = get_new_music_uipara(0);
	text_rect.x0 = player_ui->play_time.x;
	text_rect.y0 = player_ui->play_time.y;
	text_rect.x1 = player_ui->play_time.x + player_ui->play_time.width;
	text_rect.y1 = player_ui->play_time.y + player_ui->play_time.height;
	__msg("Clean********************text_rect.x0=%d\n", text_rect.x0);
	__msg("Clean********************text_rect.y0=%d\n", text_rect.y0);
	__msg("Clean********************text_rect.x1=%d\n", text_rect.x1);
	__msg("Clean********************text_rect.y1=%d\n\n", text_rect.y1);
	//GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	//GUI_SetBkColor(GUI_WHITE);
	NewMusicSetTransColor();
	GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
	return EPDK_OK;
}

//显示当前播放时间
static __s32 DrawMusicPlayingTime(__u32 time)
{
	GUI_RECT text_rect;
	char time_text[20];
	new_music_uipara_t *player_ui;
	player_ui = get_new_music_uipara(0);//获取当前播放时间的ui显示区域位置和大小
	text_rect.x0 = player_ui->play_time.x;
	text_rect.y0 = player_ui->play_time.y;
	text_rect.x1 = player_ui->play_time.x + player_ui->play_time.width;
	text_rect.y1 = player_ui->play_time.y + player_ui->play_time.height;
	//GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	NewMusicSetTransColor();
	GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
	eLIBs_memset(time_text, 0 , sizeof(time_text));
	//time2str(time, time_text);
	time2str_by_format(time, time_text, TIME_AUTO_HMS);//时间毫秒转换成字符串
	GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
	return EPDK_OK;
}

static __s32 CleanMusicTotalTime(void)
{
	GUI_RECT text_rect;
	new_music_uipara_t *player_ui;
	player_ui = get_new_music_uipara(0);
	text_rect.x0 = player_ui->total_time.x;
	text_rect.y0 = player_ui->total_time.y;
	text_rect.x1 = player_ui->total_time.x + player_ui->total_time.width;
	text_rect.y1 = player_ui->total_time.y + player_ui->total_time.height;
	NewMusicSetTransColor();
	GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
	//eLIBs_memset(time_text, 0 ,sizeof(time_text));
	//time2str(time, time_text);
	//GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
	return EPDK_OK;
}
//显示音乐播放总时间
static __s32 DrawMusicTotalTime(__u32 time)
{
	GUI_RECT text_rect;
	char time_text[20];
	new_music_uipara_t *player_ui;
	player_ui = get_new_music_uipara(0);
	//GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	//GUI_SetBkColor(0x99000000);
	text_rect.x0 = player_ui->total_time.x;
	text_rect.y0 = player_ui->total_time.y;
	text_rect.x1 = player_ui->total_time.x + player_ui->total_time.width;
	text_rect.y1 = player_ui->total_time.y + player_ui->total_time.height;
	NewMusicSetTransColor();
	GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
	eLIBs_memset(time_text, 0 , sizeof(time_text));
	//time2str(time, time_text);
	time2str_by_format(time, time_text, TIME_AUTO_HMS);
	GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
	return EPDK_OK;
}
//绘制进度条移动的黄色图片部分
static __s32 new_music_progressbar_move_paint(__gui_msg_t *msg)
{
	new_music_ctrl_t *new_music_ctrl;
	__u32 cur_time=0,total_time=0;
	//new_music_uipara_t *ui_para;
	//ui_para = get_new_music_uipara(0);//获取进度条矩形大小和坐标
	
	new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	GUI_LyrWinSel(new_music_ctrl->framewin_create_para->new_music_hlyr);//选择该窗口的图层
	#if 1//进度条滑动部分
		//画进度条背景
		{
			void *pbmp;

			if(!new_music_ctrl->scene_para[0].progressbar_bmp){
				__wrn("ui_para->uipara_icon.prog_bg_bmp is null...\n");
				//com_memdev_delete();//删除伪内存设置
				//return EPDK_FAIL;
			}
			pbmp = dsk_theme_hdl2buf(new_music_ctrl->scene_para[0].progressbar_bmp);//获取图片数据

			if(!pbmp){
				__wrn("pbmp is null...\n");
				//com_memdev_delete();//删除伪内存设置
				//return EPDK_FAIL;
			}


			GUI_BMP_Draw(pbmp, new_music_ctrl->new_music_uipara->progressbar[0].x, new_music_ctrl->new_music_uipara->progressbar[0].y);//绘制进度条背景图片
		}
		{
			void *pbmp;
			GUI_RECT	gui_rect_bj;
			cur_time = robin_get_cur_time();
			total_time = robin_get_total_time();
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			//GUI_SetBkColor(GUI_LIGHTBLUE);
			gui_rect_bj.x0	=	new_music_ctrl->new_music_uipara->progressbar[1].x;
			gui_rect_bj.y0	=	new_music_ctrl->new_music_uipara->progressbar[1].y;
			gui_rect_bj.x1	=	new_music_ctrl->new_music_uipara->progressbar[1].width;
			gui_rect_bj.y1	=	new_music_ctrl->new_music_uipara->progressbar[1].heght;
			{
				__wrn(" pbmp...\n");
				pbmp = dsk_theme_hdl2buf(new_music_ctrl->scene_para[1].progressbar_bmp);
				//GUI_BMP_Draw(theme_bmp[1], gui_rect_bj.x0, gui_rect_bj.y0);
				__wrn("gui_rect_bj.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect_bj.x0,gui_rect_bj.y0,gui_rect_bj.x1,gui_rect_bj.y1);
				
			}

			{//绘制进度条移动

				__s32 i=0;
				__s32 n;
				__wrn("cur_time=%d total_time=%d\n", cur_time, total_time);
				if(cur_time <= 0 || total_time <= 0){//当前播放时间或总时间小于等于0时
					n = 0;
				}
				else{
					__here__;
					n = ((new_music_ctrl->new_music_uipara->progressbar[0].width * cur_time)/*进度条背景图片的宽(276)*当前时间*/
						/ (gui_rect_bj.x1 * total_time));/*进度条移动图片的宽(5)*总时间*/
					__wrn("ui_para->uipara_icon.w * total_time = %d \n",gui_rect_bj.x1 * total_time);
					__wrn("ui_para->progressbar[0].width * cur_time = %d \n",new_music_ctrl->new_music_uipara->progressbar[0].width * cur_time);
				}

				__wrn("n= %d \n", n);
				for(i = 0 ; i < n ; i++){
					__here__;
					GUI_BMP_Draw(pbmp, gui_rect_bj.x0 + i * gui_rect_bj.x1, gui_rect_bj.y0);//绘制进度条移动图片
					__wrn(" ui_para->uipara_progressbar_icon.x + i * ui_para->uipara_progressbar_icon.w= %d \n",  
							gui_rect_bj.x0 + i * gui_rect_bj.x1);
				}
			}
		}
	#endif	

}

//绘制图片
static __s32 new_music_paint_one_item(__gui_msg_t *msg)
{
	new_music_ctrl_t	*new_music_ctrl;
	GUI_RECT	gui_rect_bj,gui_rect;
	GUI_RECT 	bmp_rect,music_rect;
	void	*theme_bmp[5];
	void 	*spectrogram_bmp;
	__s32 i;
	GUI_MEMDEV_Handle	draw_mem;
	
	new_music_ctrl	=	(new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)//检测图层是否开启状态
	{
		__wrn("new_music_paint_one_item the layer state is not open...\n");
		return EPDK_FAIL;
	}
	__wrn("new_music_paint_one_item is 1...\n");
	GUI_LyrWinSel(new_music_ctrl->framewin_create_para->new_music_hlyr);//选择该窗口的图层
	#if 1	//创建伪内存设备
		draw_mem = GUI_MEMDEV_Create( 0, 0, new_music_ctrl->new_music_uipara->uipara_hlyr.width, 
											new_music_ctrl->new_music_uipara->uipara_hlyr.heght);
		GUI_MEMDEV_Select(draw_mem);
		__wrn("draw_mem = 0x%x...\n",draw_mem);
	#endif
	
	#if 0//绘制进度条图片
		#if 0//进度条背景图
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
		#endif

		#if 0//进度条滑动部分
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
		//GUI_SetBkColor(GUI_LIGHTBLUE);
		gui_rect_bj.x0	=	new_music_ctrl->new_music_uipara->progressbar[1].x;
		gui_rect_bj.y0	=	new_music_ctrl->new_music_uipara->progressbar[1].y;
		gui_rect_bj.x1	=	gui_rect_bj.x0 + new_music_ctrl->new_music_uipara->progressbar[1].width;
		gui_rect_bj.y1	=	gui_rect_bj.y0 + new_music_ctrl->new_music_uipara->progressbar[1].heght;
		{
			__wrn(" theme_bmp[1]...\n");
			theme_bmp[1] = dsk_theme_hdl2buf(new_music_ctrl->scene_para[1].progressbar_bmp);
			GUI_BMP_Draw(theme_bmp[1], gui_rect_bj.x0, gui_rect_bj.y0);
			__wrn("gui_rect_bj.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect_bj.x0,gui_rect_bj.y0,gui_rect_bj.x1,gui_rect_bj.y1);
			
		}
		#endif

		#if 1	//绘制频谱图背景
		for(i=1;i<10;i++){//绘制频谱图
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			//GUI_SetBkColor(GUI_LIGHTBLUE);
			
			//GUI_SetBkColor(0);
			gui_rect.x0	=	((i-1)*33) + new_music_ctrl->new_music_uipara->spectrogram.x;//162+i*20
			gui_rect.y0	=	new_music_ctrl->new_music_uipara->spectrogram.y ;//+ i*new_music_ctrl->new_music_uipara->spectrogram_move.height;//320+ i*5
			gui_rect.x1	=	gui_rect.x0 + new_music_ctrl->new_music_uipara->spectrogram.width;//15+162+i*20
			gui_rect.y1	=   new_music_ctrl->new_music_uipara->spectrogram.heght;//150
			{
				__wrn(" theme_bmp[4]...\n");
				spectrogram_bmp = dsk_theme_hdl2buf(new_music_ctrl->scene_uipara.progress_bmp[0]);
				GUI_BMP_Draw(spectrogram_bmp, gui_rect.x0, gui_rect.y0);
				__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
				
			}

			{//绘制频谱图移动部分
				GUI_RECT	gui_rect;
				void *spectrogram_move_bmp;
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				gui_rect.x0 = ((i-1)*33) + new_music_ctrl->new_music_uipara->spectrogram.x;
				gui_rect.y0 = new_music_ctrl->new_music_uipara->spectrogram.y;//((i-1)*5) + 
				gui_rect.x1 = new_music_ctrl->new_music_uipara->spectrogram.width;
				gui_rect.y1 = new_music_ctrl->new_music_uipara->spectrogram.heght;
				{
					__wrn(" spectrogram_move_bmp...\n");
					spectrogram_move_bmp = dsk_theme_hdl2buf(new_music_ctrl->scene_uipara.progress_bmp[1]);
					GUI_BMP_Draw(spectrogram_move_bmp, gui_rect.x0, gui_rect.y0);
					__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
					
				}
			}
			
		}
		#endif
		
		#if 0  //绘制频谱图移动部分
		/*for(i=0;i<1;i++){//绘制频谱图移动部分
			GUI_RECT	gui_rect;
			void *spectrogram_move_bmp;
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
			gui_rect.x0 = 0;//new_music_ctrl->new_music_uipara->spectrogram.x;
			gui_rect.y0 = 0;//new_music_ctrl->new_music_uipara->spectrogram.y;
			gui_rect.x1 = 15;//new_music_ctrl->new_music_uipara->spectrogram.width;
			gui_rect.y1 = 10;//new_music_ctrl->new_music_uipara->spectrogram.heght;
			{
				__wrn(" spectrogram_move_bmp...\n");
				spectrogram_move_bmp = dsk_theme_hdl2buf(new_music_ctrl->scene_uipara.progress_bmp[0]);
				GUI_BMP_Draw(spectrogram_bmp, 0, 0);
				__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
				
			}
		}*/
		#endif
		
		//GUI_SetBkColor(0xF0);//透明
		//GUI_SetBkColor(GUI_LIGHTBLUE);
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//设置矩形框控件的背景颜色为黑色
		
	#endif


	#if 0//绘制当前播放时间和总时间
	__wrn(" paint DrawMusicPlayingTime time 0...\n");
	DrawMusicPlayingTime(0);
	__wrn(" paint DrawMusicPlayingTime success ...\n");
	DrawMusicTotalTime(0);
	__wrn(" paint DrawMusicTotalTime time 0...\n");
	#endif

	#if 1 //删除伪内存设备
		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
	#endif
	
	return EPDK_OK;
}

#if 1 //rat模块初始化+文件搜索
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
#endif

//初始化音频设备,robin模块初始化
static __s32 new_music_player_device_init(new_music_ctrl_t *this)
{
	__s32 ret,total=-1,total_count;	
	HRATNPL hRatNpl;
	
	__wrn("_init begining!\n");
	
	//app_print_memory_usage("robin create", NULL);
	/*open display*/
	//ADE_Open();
	ret = robin_open(ROBIN_MODE_AUDIO_MAX, NULL);//打开robin模块接口
	if(EPDK_OK != ret)
	{
		__wrn("robin_open fail...\n");
		return EPDK_FAIL;
	}
#if 0//单独打开音乐rat_npl_open列表
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
	robin_enable_spectrum();//使能打开robin模块的频谱接口
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
	//robin_open_arg_t arg;
	//char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名，
	new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	new_music_ctrl->framewin_create_para->auto_play_timmer_id = 0x01;//定时器初始化
	new_music_ctrl->play_mode	=	RAT_PLAY_MODE_ROTATE_ALL;//播放模式

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
//音乐播放按键控制处理
static __s32  new_music_play_ctrl_key_proc(__gui_msg_t *msg)
{
	static __s32 last_key=-1;
	static __u32 last_index=0;//保存上一次的文件索引id
	if(KEY_UP_ACTION == msg->dwAddData2){//按键松开动作获取，第一次进入的按键执行功能
			__wrn("last_key=%d\n", last_key);
			switch(last_key){
				case GUI_MSG_KEY_ENTER:{
					__cedar_status_t fsm_sta;
					fsm_sta = robin_get_fsm_status();	//获取fsm状态机的状态

					if(CEDAR_STAT_PLAY == fsm_sta)		//播放状态
					{
						__wrn("before send play music pause\n");
						robin_set_cmd_pause();			//设置暂停播放
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PAUSE);//播放声音暂停
						__wrn("after robin_set_cmd_pause\n");
						//new_movie_playsta_scene_create(new_movie_ctrl, new_movie_playsta_pause);
						//bok = EPDK_OK;
					}
					else if(CEDAR_STAT_PAUSE == fsm_sta)//暂停状态
					{
						__wrn("before send play music\n");
						robin_set_cmd_play();//设置开始播放
						app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_PLAY);//播放声音打开
						__wrn("after robin_set_cmd_play\n");
						//bok = EPDK_OK;
					}
				}
				break;

				case GUI_MSG_KEY_RIGHT:{/*下一首*/
					//__u32 curplay_index;
					__wrn("key press is send playlist prev files ...\n");
				/*#if 0
					curplay_index = robin_npl_get_cur();//获取当前播放的文件索引id
					__wrn("GUI_MSG_KEY_RIGHT is curplay_index = %d ...\n",curplay_index);
					//curplay_index += last_index;
					//last_index+=1;
					__wrn("key_up_action_flag = %d ...\n",key_up_action_flag);
					key_up_action_flag = 1;//松开标志打开
					app_new_music_cmd2para(msg->h_deswin, MSG_PLAY_OP_DOWN, curplay_index, 0);//向父窗口发送命令下一首命令,和当前播放文件索引
				#else
					curplay_index = last_index;
					curplay_index += 1;//获取当前播放的文件索引id
					last_index = curplay_index;
					__wrn("GUI_MSG_KEY_RIGHT is curplay_index = %d ...\n",curplay_index);
					if(curplay_index > 16){
						curplay_index = 0;
						last_index = curplay_index;
					}
					__wrn("key_up_action_flag = %d ...\n",key_up_action_flag);
					key_up_action_flag = 1;//松开标志打开
					app_new_music_cmd2para(msg->h_deswin, MSG_PLAY_OP_DOWN, curplay_index, 0);//向父窗口发送命令下一首命令,和当前播放文件索引
				
				#endif*/
				}
				break;

				case GUI_MSG_KEY_MENU:{
					app_new_music_cmd2para(msg->h_deswin, new_music_play_key_exit, 0, 0);//退出
				}
				break;

				default:{
					break;
				}
			}

	}
	else{
		switch(msg->dwAddData1){
			new_music_ctrl_t *this;
			
			this = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:{/*下一首*/
				__u32 curplay_index=0;
				__wrn("key press is send playlist prev files ...\n");

				#if 0//从当前播放列表获取到的文件id开始
				curplay_index = robin_npl_get_cur();//获取当前播放的文件索引id
				__wrn("GUI_MSG_KEY_RIGHT is curplay_index = %d ...\n",curplay_index);
				//curplay_index += last_index;
				//last_index+=1;
				__wrn("key_up_action_flag = %d ...\n",key_up_action_flag);
				key_up_action_flag = 1;//松开标志打开
				app_new_music_cmd2para(msg->h_deswin, MSG_PLAY_OP_DOWN, curplay_index, 0);//向父窗口发送命令下一首命令,和当前播放文件索引
				{//更新频谱图
					NEW_SPCTRM_Clear(0);
					NEW_SPCTRM_Update((H_WIN)(this->framewin_create_para->new_music_hfrm));
					//NEW_SPCTRM_Update((H_WIN)(msg->h_deswin));
				}
				#else//自动从0开始
					curplay_index = last_index;
					
					__wrn("GUI_MSG_KEY_RIGHT is curplay_index = %d ...\n",curplay_index);
					curplay_index += 1;//获取当前播放的文件索引id
					if(curplay_index > 16){
						curplay_index = 0;
						last_index = 0;
						last_index = curplay_index;
					}
					__wrn("key_up_action_flag = %d ...\n",key_up_action_flag);
					key_up_action_flag = 1;//松开标志打开
					
					app_new_music_cmd2para(msg->h_deswin, MSG_PLAY_OP_DOWN, curplay_index, 0);//向父窗口发送命令下一首命令,和当前播放文件索引
					
					last_index = curplay_index;
				#endif
			}
			break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:{/*上一首*/
				__u32 curplay_index=0;
				__wrn("key press is send playlist next files ...\n");
				#if 0//通过获取到的文件索引id
				curplay_index = robin_npl_get_cur();//获取当前播放的文件索引id
				__wrn("last_index =%d...\n",last_index);
				key_up_action_flag = 1;//松开标志打开
				__wrn("key_up_action_flag = %d ...\n",key_up_action_flag);
				app_new_music_cmd2para(msg->h_deswin, MSG_PLAY_OP_UP, curplay_index, 0);//向父窗口发送命令上一首命令
				{//更新频谱图
					NEW_SPCTRM_Clear(0);
					NEW_SPCTRM_Update((H_WIN)(this->framewin_create_para->new_music_hfrm));
					//NEW_SPCTRM_Update((H_WIN)(msg->h_deswin));
				}
				#else//自动从0开始
					curplay_index = last_index;
					
					__wrn("curplay_index =%d...\n",curplay_index);
					__wrn("last_index =%d...\n",last_index);
					curplay_index -= 1;//获取当前播放的文件索引id
					if(curplay_index < 0){
						__wrn("curplay_index last_index =%d...\n",last_index);
						curplay_index = 16;
						last_index = 0;
						last_index = 16;
					}
					key_up_action_flag = 1;//松开标志打开
					__wrn("key_up_action_flag = %d ...\n",key_up_action_flag);
					
					app_new_music_cmd2para(msg->h_deswin, MSG_PLAY_OP_UP, curplay_index, 0);//向父窗口发送命令上一首命令
					
					last_index = curplay_index;
				#endif
			}
			break;

			case GUI_MSG_KEY_NUM0:{
				__wrn("key press is set ff or rr the speed...\n");
				robin_set_ff_rr_speed(CEDAR_SLOWEST_FRSPEED);//最小倍速播放
				break;
			}
			break;

			case GUI_MSG_KEY_NUM1:{//快进
				__wrn("key press is send ff ...\n");
				robin_set_cmd_ff();
			}
			break;

			case GUI_MSG_KEY_NUM2:{//后退
				__wrn("key press is send rr ...\n");
				robin_set_cmd_rr();
			}
			break;

			default:{
				break;
			}
		}

	}

	if(KEY_UP_ACTION == msg->dwAddData2){//按键抬起动作
		__wrn("key release the button...\n");
		//delay_5ms(__u8 ms);
		//esKRNL_TimeDly(1000);//延时时间1s
		key_up_action_flag = 1;//松开标志打开
		__wrn("key_up_action_flag =%d...\n",key_up_action_flag);
		last_key = -1;
	}
	else{
		__wrn("key press the button...\n");
		key_up_action_flag = 0;//松开标志关闭
		__wrn("key_up_action_flag =%d...\n",key_up_action_flag);
		last_key = msg->dwAddData1;
	}


}

//装载自动播放定时器
static __s32 install_auto_palymusic_timer(new_music_ctrl_t *new_music_ctrl,__gui_msg_t *msg)
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
		             , 20, NULL);//0.5秒，设置定时器
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
/*卸载自动播放定时器*/
static __s32 uninstall_auto_palymusic_timer(new_music_ctrl_t *new_music_ctrl)
{
	if(!new_music_ctrl)
	{
		__err("invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("uninstall_auto_palymusic_timer is start uninstall...\n");
	if(GUI_IsTimerInstalled(new_music_ctrl->framewin_create_para->new_music_hfrm, new_music_ctrl->framewin_create_para->auto_play_timmer_id))
	{
		__wrn("start uninstall...\n");
		GUI_KillTimer(new_music_ctrl->framewin_create_para->new_music_hfrm, new_music_ctrl->framewin_create_para->auto_play_timmer_id);//删除定时器
		__wrn("start uninstall success...\n");
	}
	return EPDK_OK;
}


//回调处理函数
static __s32 __new_music_proc(__gui_msg_t *msg)
{
	static __s32 key_num=0;
	__wrn("msg->id = %d\n",msg->id);
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			new_music_ctrl_t	*new_music_ctrl;
			new_music_create_para_t	*new_music_create_para;
			char FileName[RAT_MAX_FULL_PATH_LEN];	//存放文件名，
			//gscene_hbar_set_state(HBAR_ST_HIDE);
			//gscene_bgd_set_state(BGD_STATUS_SHOW);
			
			new_music_create_para	=	(new_music_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
			new_music_ctrl	=	(new_music_ctrl_t *)My_Balloc(sizeof(new_music_ctrl_t));
			//gscene_bgd_set_state(BGD_STATUS_HIDE);//背景场景隐藏
			if(!new_music_ctrl){
				__wrn("new_music_ctrl is balloc is failes...\n");
			}
			eLIBs_memset(new_music_ctrl, 0, sizeof(new_music_ctrl_t));//结构体初始化，数据清0
			new_music_ctrl->framewin_create_para = new_music_create_para;//framewin窗口创建参数
			new_music_create_para->new_music_hfrm = msg->h_deswin;	//窗口赋值
			__wrn("new_music_create_para->new_music_hfrm =%x...\n",new_music_create_para->new_music_hfrm);
			GUI_WinSetAddData(msg->h_deswin, (__u32)new_music_ctrl);//窗口设置添加结构体数据

			new_music_uipara_init(new_music_ctrl);			//申请图片资源
			//new_music_player_save_last_para(new_music_ctrl);//注册表初始化
			__wrn("new_music_uipara_init is ok...\n");
			//new_music_paint_one_item(msg);	//绘制ui图片
			__wrn("new_music_paint_one_item is ok...\n");
			new_music_rat_init(new_music_ctrl);				//rat模块文件搜索打开初始化
			
			#if 1	//获取媒体类型文件的文件名
				eLIBs_memset(FileName, 0, sizeof(FileName));				//数组数据初始化，清0
				new_music_GetListItemFileName(new_music_ctrl, 0, FileName);
				__wrn("\FileName = %s\n",FileName);
				rat_set_file_for_play(new_music_ctrl->rat.rat_handle, FileName);//设置当前文件播放
						
			#endif
						

			new_music_init(msg);						 					//音乐播放初始化
			//new_music_player_set_play_mode(new_music_ctrl);				//设置播放模式
			//new_music_ctrl->framewin_create_para->auto_play_timmer_id = 0x01;//定时器初始化
			GUI_LyrWinSetSta(new_music_ctrl->framewin_create_para->new_music_hlyr, GUI_LYRWIN_STA_ON);//窗口图层设置开启状态
			GUI_LyrWinSetTop(new_music_ctrl->framewin_create_para->new_music_hlyr);	
			new_music_paint_one_item(msg);								//绘制ui图片场景
			new_music_progressbar_move_paint(msg);//绘制进度条
			
			new_music_player_device_init(new_music_ctrl);					//音乐播放设备初始化
			#if 0	//直接播放音乐文件
			//PlayCurMusicFile(new_music_ctrl,FileName);			 	//播放音乐文件
			new_music_play_file(new_music_ctrl, FileName,0);			//播放音乐文件
			#else	//开始播放音乐文件
			new_music_play_start(new_music_ctrl,0);//开始播放
			#endif
			
			install_auto_palymusic_timer(new_music_ctrl,msg);			//开启播放定时器
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1，销毁*/
			new_music_ctrl_t	*new_music_ctrl;
			new_music_create_para_t *create_para;
			create_para = GUI_WinGetAttr(msg->h_deswin);
			new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			if(!create_para){
				__wrn("create_para is null...\n");
			}
			__wrn("1...\n");
			uninstall_auto_palymusic_timer(new_music_ctrl);//先释放定时器，再释放图片
			__wrn("2...\n");
			robin_set_cmd_stop();					//先停止播放
			__wrn("3...\n");
			
			__wrn("4...\n");
			new_music_uipara_deinit(new_music_ctrl);//释放图片
			__wrn("5...\n");

			if(new_music_ctrl->framewin_create_para->new_music_hlyr)//释放图层
			{
				__wrn("10...\n");
				//GUI_LyrWinDelete(new_music_ctrl->framewin_create_para->new_music_hlyr);
				__wrn("11...\n");
				//eLIBs_memset(new_music_ctrl->framewin_create_para->new_music_hlyr, 0, sizeof(new_music_ctrl->framewin_create_para->new_music_hlyr));
				new_music_ctrl->framewin_create_para->new_music_hlyr = 0;
			}
			__wrn("6...\n");
			if(new_music_ctrl->framewin_create_para){
				__wrn("7...\n");
				eLIBs_memset(new_music_ctrl->framewin_create_para, 0, sizeof(new_music_create_para_t));
				My_Bfree(new_music_ctrl->framewin_create_para,sizeof(new_music_create_para_t));
				__wrn("8...\n");
				//new_music_ctrl->framewin_create_para = NULL;
				__wrn("new_music_ctrl->framewin_create_para is null...\n");
			}
			

			if(new_music_ctrl->rat.rat_handle){//关闭rat模块
				__wrn("rat_handle free start\n");
				rat_close(new_music_ctrl->rat.rat_handle);
				__wrn("rat_handle free size = %d\n",new_music_ctrl->rat.rat_handle);
				if(new_music_ctrl->rat.rat_handle==NULL){
					__wrn("rat_handle free success\n");
				}
			}

			
			if(new_music_ctrl){
				__wrn("12...\n");
				eLIBs_memset(new_music_ctrl, 0, sizeof(new_music_ctrl_t));
				__wrn("13...\n");
				My_Bfree(new_music_ctrl,sizeof(new_music_ctrl_t));
				__wrn("14...\n");
				//new_music_ctrl = NULL;
				__wrn("new_music_ctrl is null...\n");
			}
			/*if(create_para){
				__wrn("15...\n");
				eLIBs_memset(create_para, 0, sizeof(new_music_create_para_t));
				__wrn("16...\n");
				//My_Bfree(create_para,sizeof(new_music_create_para_t));
				//create_para = NULL;
				__wrn("17...\n");
			}*/

			__wrn("new_music_ctrl->framewin_create_para layer free size = %d\n",new_music_ctrl->framewin_create_para->new_music_hlyr);
			__wrn("new_music_ctrl->framewin_create_para free size = %d\n",new_music_ctrl->framewin_create_para);
			__wrn("new_music_ctrl free size = %d\n",new_music_ctrl);
			__wrn("create_para free size = %d\n",create_para);
			//__wrn("new_music_create_para->new_music_hlyr  free size = %d\n",new_music_create_para);//打印结构体是否释放内存

			robin_close();							//robin模块关闭
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2，绘制*/
			new_music_create_para_t	*new_music_create_para;
			new_music_create_para	=	(new_music_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
			//new_music_paint_one_item(msg);
			__wrn("GUI_MSG_PAINT new_music_paint_one_item is ok...\n");
			if(GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(new_music_create_para->new_music_hlyr))//检测图层是否打开状态
			{
				__wrn("\n...do not paint...\n");
				return EPDK_OK;
			}
			
			//new_music_paint_one_item(msg);								//绘制ui图片场景
			//new_music_progressbar_move_paint(msg);//绘制进度条

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
			new_music_play_ctrl_key_proc(msg);
			__wrn("__new_music_proc GUI_MSG_KEY is press...\n");
			
		}
		break;

		case MSG_PLAY_OP_PLAY_BY_INDEX:{/*通过播放列表的文件索引进行播放音乐文件*/
			new_music_ctrl_t	*this;
			__u32 play_index;//存放播放列表传入的文件索引id
			__s32 time=100;
			__wrn("PLAYLIST_PLAY_BY_INDEX is input...\n");
			this = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			play_index = msg->dwAddData2;
			__wrn("play_index = %d\n",play_index);
			__wrn("key_up_action_flag = %d\n",key_up_action_flag);
			
			#if 1 //新添加功能，如果按键松开1s后才播放
				if(key_up_action_flag)//如果按键没松开
				{
					__wrn("key_up_action_flag is action...\n");
					while(time--){
						__wrn("time = %d\n",time);
						esKRNL_TimeDly(1);//延时1s,1*5ms
					}
					//new_music_play_start(this,play_index);//开始播放音乐文件
					return EPDK_FALSE;
				}else{
					__wrn("key_up_action_flag is press...\n");
					esKRNL_TimeDly(0);//延时1s，200*5ms
					//return EPDK_FALSE;
				}
			#endif
			
			new_music_play_start(this,play_index);//开始播放音乐文件
		}
		return EPDK_OK;

		case GUI_MSG_COMMAND:{/*7，命令*/

		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{/*19，定时器*/
			
			new_music_ctrl_t *new_music_ctrl;
			__s32 ret,amplifier_onoff,fsm_sta;
			__u32 play_index;
			__u32 cur_time=0,total_time=0;
			ret = -1;
			new_music_ctrl = (new_music_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__msg("send GUI_MSG_TIMER cmd is start...\n");
			if(new_music_ctrl)
			{
				if(LOWORD(msg->dwAddData1) == new_music_ctrl->framewin_create_para->auto_play_timmer_id)
				{
					static __u32 cnt = 0;

					{
						cur_time = robin_get_cur_time();
						__msg("cur_time = %d...\n",cur_time);
						total_time = robin_get_total_time();
						__msg("total_time = %d...\n",total_time);
						play_index = robin_npl_get_cur();//获取当前播放列表的文件索引id
						fsm_sta = robin_get_fsm_status();//获取robin状态机的状态
						amplifier_onoff = dsk_amplifier_is_on();//获取功放状态
						__msg("amplifier_onoff 1 = %d...\n",amplifier_onoff);

						if(fsm_sta == CEDAR_STAT_PLAY){//播放状态下才更新
						#if 1//绘制当前播放时间
							GUI_LyrWinSel(new_music_ctrl->framewin_create_para->new_music_hlyr);//选择该窗口的图层
							DrawMusicPlayingTime(cur_time);//显示当前播放时间
							//eLIBs_printf("\n\n==DrawMusicPlayingTime==\n\n");
							CleanMusicTotalTime();//清除显示当前播放时间
							//eLIBs_printf("\n\n==CleanMusicTotalTime==\n\n");
							DrawMusicTotalTime(total_time);//显示音乐播放总时间
						#endif

						#if 1 //绘制进度条移动部分显示
							if((cnt % 50) == 0)//定时器10s画一次
							{
								__wrn("before new_music_progressbar_move_paint\n");
								new_music_progressbar_move_paint(msg);		//绘制进度条
								__wrn("after new_music_progressbar_move_paint\n");
							}
							__msg("cnt = %d...\n",cnt);
							cnt++;
						#endif
						}
						
						if(total_time == cur_time){//判断播放一首完成
							play_index += 1;
							if(play_index > 25){
								play_index = 0;
							}

							new_music_progressbar_move_paint(msg);		//绘制进度条
							
							#if 1//播放完一首绘制当前播放时间总时间清0
								DrawMusicPlayingTime(0);//显示当前播放时间
								//eLIBs_printf("\n\n==DrawMusicPlayingTime==\n\n");
								CleanMusicTotalTime();//清除显示当前播放时间
								//eLIBs_printf("\n\n==CleanMusicTotalTime==\n\n");
								DrawMusicTotalTime(0);//显示音乐播放总时间
							#endif

							#if 1//播放完一首之后清除总时间显示区域
								CleanMusicTotalTime();
							#endif
							
							__msg("play_index = %d...\n",play_index);
								new_music_play_start(new_music_ctrl, play_index);//通过索引id播放音乐文件
						}
						
						
						if(fsm_sta == CEDAR_STAT_PLAY){//播放状态
							dsk_amplifier_onoff(1);//打开功放，声音打开
							{//更新频谱图
								__msg("NEW_SPCTRM_Update ...\n");
								//NEW_SPCTRM_Update((H_WIN)(msg->h_deswin));
								
								NEW_SPCTRM_Update((H_WIN)(new_music_ctrl->framewin_create_para->new_music_hfrm));
								__msg("NEW_SPCTRM_Update success...\n");
							}
						}
						else if((fsm_sta == CEDAR_STAT_PAUSE)||
								(fsm_sta == CEDAR_STAT_STOP)){//暂停状态
							dsk_amplifier_onoff(0);//关闭功放，声音关闭
							/*{//更新频谱图
								NEW_SPCTRM_Clear(0);
								//NEW_SPCTRM_Update((H_WIN)(msg->h_deswin));
								//NEW_SPCTRM_Update((H_WIN)(new_music_ctrl->framewin_create_para->new_music_hfrm));
							}*/
							
						}
						amplifier_onoff = dsk_amplifier_is_on();//获取功放状态
						__msg("amplifier_onoff 2 = %d...\n",amplifier_onoff);
					}
				}
			}
			
		}
		break;
		
		default:
			break;

	}


	return GUI_FrmWinDefaultProc(msg);
}

//创建音乐播放器manwin窗口
H_WIN	new_music_frmwin_create(H_WIN h_parent, new_music_create_para_t *para)
{
	__gui_framewincreate_para_t	framewin_para;
	new_music_create_para_t *new_music_create_para;
	FB	fb;
	#if 0 //检测内存是否泄漏
		esKSRV_SysInfo();
	#endif
	GUI_LyrWinGetFB(para->new_music_hlyr, &fb);
	new_music_create_para = (new_music_create_para_t *)My_Balloc(sizeof(new_music_create_para_t));

	eLIBs_memset(new_music_create_para, 0, sizeof(new_music_create_para_t));
	new_music_create_para->font = para->font;//文本
	new_music_create_para->root_type = para->root_type;//TF卡模式
	new_music_create_para->new_music_hlyr = para->new_music_hlyr;//传入图层
	
	__wrn("fb.size.width=%d, fb.size.height=%d \n",fb.size.width, fb.size.height);
	__wrn("h_parent = %x \n",h_parent);
	__wrn("new_music_hlyr = %x \n",new_music_create_para->new_music_hlyr);
	/*开始操作配置播放器framewin窗口参数*/
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


//进度条图层删除
__s32 new_music_ui_scene_delete(void *handle)
{
	new_music_create_para_t *scene_para;
	scene_para = (new_music_create_para_t *)handle;//manwin父窗口中用于创建进度条的framewin句柄指针

	if(NULL == handle)//判断是否创建有进度条的framewin窗口句柄
	{
		__wrn("invalid para..\n");
		return EPDK_FAIL;
	}

	if(!scene_para->new_music_hfrm)//进度条内部的famewin窗口句柄是否创建
	{
		__wrn("invalid para..\n");
		return EPDK_FAIL;
	}
	__wrn("scene_para->new_music_hfrm = %x ok...\n",scene_para->new_music_hfrm);
	GUI_FrmWinDelete(scene_para->new_music_hfrm);//删除本进度条framewin窗口句柄
	__wrn("scene_para->new_music_hlyr = %x ok...\n",scene_para->new_music_hlyr);
	if(!scene_para->new_music_hlyr)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	GUI_LyrWinDelete(scene_para->new_music_hlyr);//删除当前的进度条framewin窗口图层
	eLIBs_memset(scene_para, 0, sizeof(new_music_create_para_t));//清空结构体数据
	esMEMS_Bfree(scene_para, sizeof(new_music_create_para_t));//释放该结构体的空间内存
	return EPDK_OK;
}

//删除本窗口内部的图层等数据
__s32 __new_music_internal_data_delet(void)
{


}

