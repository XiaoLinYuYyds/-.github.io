#include "App_new_music.h"
#include "App_new_music_ui_para.h"

#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR
#define COLOUR_TO_8BPP_INDEX_0          0xF0

//创建图层
static	H_LYR app_new_music_8bpp_laye_creat(RECT *rect, __s32 pipe)	
{
		H_LYR layer = NULL;
		FB	fb =
		{
			{0, 0}, 										/* size 	 */
			{0, 0, 0},										/* buffer	 */
			{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},	/* fmt		 */
		};
		__disp_layer_para_t lstlyr =
		{
			DISP_LAYER_WORK_MODE_PALETTE,					 /* mode	  */
			0,												/* ck_mode	 */
			0,												/* alpha_en  */
			0,												/* alpha_val */
			1,												/* pipe 	 */
			0xff,											/* prio 	 */
			{0, 0, 0, 0},									/* screen	 */
			{0, 0, 0, 0},									/* source	 */
			DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
			NULL											/* fb		 */
		};
		__layerwincreate_para_t lyrcreate_info =
		{
			"new music layer",
			NULL,
			GUI_LYRWIN_STA_SUSPEND,
			GUI_LYRWIN_NORMAL
		};
		fb.size.width		= rect->width;
		fb.size.height		= rect->height;
		lstlyr.src_win.x		= 0;
		lstlyr.src_win.y		= 0;
		lstlyr.src_win.width	= rect->width;
		lstlyr.src_win.height	= rect->height;
		lstlyr.scn_win.x		= rect->x;
		lstlyr.scn_win.y		= rect->y;
		lstlyr.scn_win.width	= rect->width;
		lstlyr.scn_win.height	= rect->height;
		lstlyr.pipe = pipe;
		lstlyr.fb = &fb;
		lyrcreate_info.lyrpara = &lstlyr;
		layer = GUI_LyrWinCreate(&lyrcreate_info);
	
		if(!layer)
		{
			__err("GUI_LyrWinCreate fail... !\n");
		}
	
		return layer;
}

//创建图层32bpp
static	H_LYR app_new_music_32bpp_laye_creat(RECT *rect, __s32 pipe)	
{
	H_LYR layer = NULL;
	FB	fb =
	{
		{0, 0}, 										/* size 	 */
		{0, 0, 0},										/* buffer	 */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},	 /* fmt 	  */ /*与8bpp的不同处*/

	};
	__disp_layer_para_t lstlyr =
	{
		//DISP_LAYER_WORK_MODE_PALETTE, 				   /* mode		*/
		DISP_LAYER_WORK_MODE_NORMAL,					/*与8bpp的不同处*/
		0,												/* ck_mode	 */
		0,												/* alpha_en  */
		0,												/* alpha_val */
		0,												/* pipe 	 */ /*与8bpp的不同处*/
		0xff,											/* prio 	 */
		{0, 0, 0, 0},									/* screen	 */
		{0, 0, 0, 0},									/* source	 */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
		NULL											/* fb		 */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"APP_NEW_MUSIC",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;/*与8bpp的不同处*/
	lstlyr.src_win.x		= 0;
	lstlyr.src_win.y		= 0;
	lstlyr.src_win.width	= rect->width;
	lstlyr.src_win.height	= rect->height;
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y;
	lstlyr.scn_win.width	= rect->width;
	lstlyr.scn_win.height	= rect->height;
	lstlyr.pipe = pipe; 	//1
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("app new music layer create error !\n");
	}

	return layer;

}

//命令发送函数
__s32 app_new_music_cmd2para(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t mymsg;
	__wrn("\n*****************app_new_music_cmd2para********************\n");
	mymsg.h_deswin = GUI_WinGetParent(hwin);
	mymsg.h_srcwin = NULL;
	mymsg.id = GUI_MSG_COMMAND;
	mymsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	mymsg.dwAddData2 = data1;
	mymsg.dwReserved = data2;
	return (GUI_SendNotifyMessage(&mymsg));
}

//装载自动播放定时器
static __s32 installed_auto_paly_timer(__gui_msg_t *msg)
{
	app_new_music_ctrl_t    *new_music_ctrl;
	new_music_ctrl = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!new_music_ctrl)
	{
		__err("timer installed invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("new_movie_ctrl->auto_play_timer_id = 0x%x\n",new_music_ctrl->play_timer_id);
	if(!GUI_IsTimerInstalled(msg->h_deswin, new_music_ctrl->play_timer_id))/*定时器加载*/
	{
		GUI_SetTimer(msg->h_deswin, new_music_ctrl->play_timer_id
		             , MUSIC_AUTO_FLUSH_FIRST_TIME, NULL);//0.5秒，设置定时器
		__wrn("GUI_SetTimer timer is installed...\n");
	}

	return EPDK_OK;
}
//卸载自动播放定时器
static __s32 uninstalled_auto_paly_timer(__gui_msg_t *msg)
{
	app_new_music_ctrl_t    *new_music_ctrl;
	new_music_ctrl = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!new_music_ctrl)
	{
		__err("timer uninstalled invalid para...\n");
		return EPDK_FAIL;
	}
	__wrn("new_movie_ctrl->auto_play_timer_id = 0x%x\n",new_music_ctrl->play_timer_id);
	if(GUI_IsTimerInstalled(msg->h_deswin, new_music_ctrl->play_timer_id))/*定时器存在*/
	{
		GUI_KillTimer(msg->h_deswin, new_music_ctrl->play_timer_id);//0.5秒，卸载定时器
		__wrn("GUI_KillTimer timer is uninstalled...\n");
	}

	return EPDK_OK;
}



static __s32 app_new_music_init(__gui_msg_t *msg)
{
	__s32			ret;
	app_new_music_ctrl_t *new_music_ctrl;
	new_music_ctrl = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

	//ret = robin_npl_get_cur();

	if(ret == -1)
	{
		__wrn("robin_init failed!\n");
		//return EPDK_FAIL;
	}
	/*else if(ret == 1)
	{
		__inf("robin_init failed! ret = 1\n");
		ShowErrorPic(new_music_ctrl->err_pic);
		return EPDK_FAIL;
	}*/

	new_music_ctrl->play_timer_id = 0x10;
	//PlayCurMusicFile();//播放音乐文件
	return EPDK_TRUE;
}

//进度条+频谱图frameiwn窗口创建
static __s32 app_new_music_framewin_create(__gui_msg_t *msg)
{
	app_new_music_ctrl_t 	*app_new_music_ctrl_para;//manwin窗口的结构体参数
	new_music_create_para_t create_para;//framewin窗口内部的结构体参数
	new_music_uipara_t		*new_music_uipara;//获取framewin窗口大小的UI参数
	RECT	rect;
	SIZE	screen_size;//存放屏幕大小
	app_new_music_ctrl_para = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	/*{
		#if 1 //获取进度条显示的图层UI参数定义窗口大小
		//new_music_uipara = get_new_music_uipara(0);//获取UI坐标和大小等参数
		rect.x = 0;
		rect.y = 0;
		rect.width = 600;
		rect.height = 480;
		#else //获取进度条显示的图层UI参数定义窗口大小
		new_music_uipara = get_new_music_uipara(0);//获取UI坐标和大小等参数
		rect.x = new_music_uipara->uipara_hrm.x;
		rect.y = new_music_uipara->uipara_hrm.y;
		rect.width = new_music_uipara->uipara_hrm.width;
		rect.height = new_music_uipara->uipara_hrm.heght;
		#endif
		__wrn("rect: x=%d, y=%d, w=%d, h=%d\n",rect.x, rect.y, rect.width, rect.height);
		app_new_music_ctrl_para->h_spectrum_lyr = app_new_music_32bpp_laye_creat(&rect, 1);//创建32bpp图层
	
		#if 1 //创建频谱图窗口控件
		{
			__wrn("!!!!!!!! spectrum  framewin start create!!!\n");
			if(!NEW_RegisterSpectrumControl())
			{
				__wrn("!!!!!!!!register spectrum ocx error!!!\n");
			}
			//app_new_music_ctrl_para->h_spectrum_lyr = app_new_music_ctrl_para->new_music_frm_lyr;
			app_new_music_ctrl_para->h_spectrum_win = 	NEW_SPCTRM_SceneCreate((H_WIN)(msg->h_deswin), (H_WIN)(app_new_music_ctrl_para->h_spectrum_lyr));

			if(app_new_music_ctrl_para->h_spectrum_win == NULL)
			{
				__wrn("creat spectrum frame win error!\n");
			}
		}
		#endif
		//GUI_LyrWinSetSta(app_new_music_ctrl_para->h_spectrum_lyr, GUI_LYRWIN_STA_ON);
	}*/
	#if 0 //framewin窗口大小
	dsk_display_get_size(&screen_size.width, &screen_size.height);//获取屏幕大小1024*600
		rect.x = (1024-600)/2;
		rect.y = (600-480)/2;
		rect.width = 600;//screen_size.width;
		rect.height = 480;//screen_size.height;
	#else //获取进度条显示的图层UI参数定义窗口大小
		new_music_uipara = get_new_music_uipara(0);//获取UI坐标和大小等参数
		rect.x = new_music_uipara->uipara_hrm.x;
		rect.y = new_music_uipara->uipara_hrm.y;
		rect.width = new_music_uipara->uipara_hrm.width;
		rect.height = new_music_uipara->uipara_hrm.heght;
	#endif
	__wrn("rect: x=%d, y=%d, w=%d, h=%d\n",rect.x, rect.y, rect.width, rect.height);
	app_new_music_ctrl_para->new_music_frm_lyr = app_new_music_32bpp_laye_creat(&rect, 1);//创建32bpp图层
	{
		__wrn("new_music_frm_lyr = %x...\n",app_new_music_ctrl_para->new_music_frm_lyr);
		if(!app_new_music_ctrl_para->new_music_frm_lyr){
			__wrn("app_new_music_ctrl_para->new_music_frm_lyr is null...\n");
		}
		
		create_para.new_music_hlyr = app_new_music_ctrl_para->new_music_frm_lyr;//创建好的图层，赋值到framewin窗口内部图层存放变量
		create_para.font	=	app_new_music_ctrl_para->font;
		app_new_music_ctrl_para->new_music_frm_win = new_music_frmwin_create(msg->h_deswin, &create_para);
		GUI_WinSetFocusChild(app_new_music_ctrl_para->new_music_frm_win);//按键信息发送
		__wrn("new_music_frm_win = %x...\n",app_new_music_ctrl_para->new_music_frm_win);
	}
	
	return EPDK_OK;
}

//单独绘制频谱图
static __s32 spectrum_framewin_create(__gui_msg_t *msg)
{
	app_new_music_ctrl_t 	*app_new_music_ctrl_para;//manwin窗口的结构体参数
	//new_music_create_para_t create_para;//framewin窗口内部的结构体参数
	//new_music_uipara_t		*new_music_uipara;//获取framewin窗口大小的UI参数
	RECT	rect;
	SIZE	screen_size;//存放屏幕大小
	app_new_music_ctrl_para = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	
	#if 0 //framewin窗口大小
	dsk_display_get_size(&screen_size.width, &screen_size.height);//获取屏幕大小1024*600
		rect.x = (1024-600)/2;
		rect.y = (600-480)/2;
		rect.width = 600;//screen_size.width;
		rect.height = 480;//screen_size.height;
	#else //获取进度条显示的图层UI参数定义窗口大小
		//new_music_uipara = get_new_music_uipara(0);//获取UI坐标和大小等参数
		rect.x = 0;
		rect.y = 0;
		rect.width = 1024;
		rect.height = 600;
	#endif
	__wrn("rect: x=%d, y=%d, w=%d, h=%d\n",rect.x, rect.y, rect.width, rect.height);
	app_new_music_ctrl_para->h_spectrum_lyr = app_new_music_32bpp_laye_creat(&rect, 1);//创建8bpp图层
	
	#if 1 //创建频谱图窗口控件
	{
		__wrn("!!!!!!!! spectrum  framewin start create!!!\n");
		if(!NEW_RegisterSpectrumControl())
		{
			__wrn("!!!!!!!!register spectrum ocx error!!!\n");
		}
		//app_new_music_ctrl_para->h_spectrum_lyr = app_new_music_ctrl_para->new_music_frm_lyr;
		app_new_music_ctrl_para->h_spectrum_win = 	NEW_SPCTRM_SceneCreate((H_WIN)(msg->h_deswin), (H_WIN)(app_new_music_ctrl_para->h_spectrum_lyr));

		if(app_new_music_ctrl_para->h_spectrum_win == NULL)
		{
			__wrn("creat spectrum frame win error!");
		}
	}
	#endif
	GUI_LyrWinSetSta(app_new_music_ctrl_para->h_spectrum_lyr, GUI_LYRWIN_STA_ON);//打开这个频谱图层
}

//播放列表framewin窗口创建
static __s32 playlist_framewin_create(__gui_msg_t *msg)
{
	app_new_music_ctrl_t 	*app_new_music_ctrl_para;//manwin窗口的结构体参数
	playlist_create_para_t create_para;//framewin窗口内部的结构体参数
	new_music_uipara_t		*new_music_uipara;//获取framewin窗口大小的UI参数
	RECT	rect;
	SIZE	screen_size;//存放屏幕大小
	app_new_music_ctrl_para = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	
	#if 0 //framewin窗口大小
		dsk_display_get_size(&screen_size.width, &screen_size.height);//获取屏幕大小1024*600
		rect.x = (1024-600)/2;
		rect.y = (600-480)/2;
		rect.width = 600;//screen_size.width;
		rect.height = 480;//screen_size.height;
	#else //获取播放列表显示的图层UI参数定义窗口大小
		new_music_uipara = get_new_music_uipara(0);//获取UI坐标和大小等参数
		rect.x = new_music_uipara->playlist_uipara.playlist_area_hlyr.x;
		rect.y = new_music_uipara->playlist_uipara.playlist_area_hlyr.y;
		rect.width = new_music_uipara->playlist_uipara.playlist_area_hlyr.width;
		rect.height = new_music_uipara->playlist_uipara.playlist_area_hlyr.height;
	#endif
	__wrn("playlist laye rect: x=%d, y=%d, w=%d, h=%d\n",rect.x, rect.y, rect.width, rect.height);
	app_new_music_ctrl_para->playlist_frm_lyr = app_new_music_32bpp_laye_creat(&rect, 1);//创建8bpp图层
	{
		if(!app_new_music_ctrl_para->playlist_frm_lyr){
			__wrn("app_new_music_ctrl_para->playlist_frm_lyr is null...\n");
		}
		create_para.playlist_hlyr = app_new_music_ctrl_para->playlist_frm_lyr;//创建好的图层，赋值到framewin窗口内部图层存放变量
		create_para.font	=	app_new_music_ctrl_para->playlist_font;//文本
		app_new_music_ctrl_para->playlist_frm_win = playlist_frmwin_create(msg->h_deswin, &create_para);
		GUI_WinSetFocusChild(app_new_music_ctrl_para->playlist_frm_win);//按键信息发送
	}

	
	return EPDK_OK;
}


//回调处理函数
static __s32 __app_new_music_proc(__gui_msg_t *msg)
{
	__wrn("msg->id = %d\n",msg->id);
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			
			//gscene_bgd_set_state(BGD_STATUS_HIDE);//背景场景隐藏
			//g_disable_close_scn();//关闭荧光屏
			//com_set_backcolor(CONST_BackColor_Black);//设置背景颜色为黑色
			gscene_hbar_set_state(HBAR_ST_HIDE);
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			/*com_set_palette_by_id(ID_MUSIC_PALETTE_BMP);//调色板id
			{
				//背景色，文字编辑底色
				__u32 color[] = {COLOUR_BG_8BPP, COLOUR_EDIT_BG_8BPP};
				com_set_palette_color_by_index(COLOUR_TO_8BPP_INDEX_0, color, BEETLES_TBL_SIZE(color));
			}*/
			//app_new_music_init(msg);
			spectrum_framewin_create(msg);//单独绘制频谱图
			playlist_framewin_create(msg);//播放列表framewin创建
			app_new_music_framewin_create(msg);//播放器framewin创建
			
			//installed_auto_paly_timer(msg);//定时器打开
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1，销毁*/
			app_new_music_ctrl_t	*app_new_music_ctrl_para;
			app_new_music_ctrl_para = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
			if(app_new_music_ctrl_para){
				__wrn("app_new_music_ctrl_para GUI_MSG_DESTROY  is start...\n");
				//uninstalled_auto_paly_timer(msg);//卸载定时器

				
				#if 1	//删除播放器的窗口和图层
					if(app_new_music_ctrl_para->new_music_frm_win){//删除播放器窗口
						__wrn("new_music_frm_win delete is start...\n");
						GUI_FrmWinDelete(app_new_music_ctrl_para->new_music_frm_win);
						__wrn("new_music_frm_win delete is 1...\n");
						//app_new_music_ctrl_para->new_music_frm_win = NULL;
						__wrn("new_music_frm_win delete is 2...\n");
						if(app_new_music_ctrl_para->new_music_frm_win == NULL){
							__wrn("new_music_frm_win delete is success...\n");
						}
					}
					if(app_new_music_ctrl_para->new_music_frm_lyr){//删除播放器图层
						__wrn("new_music_frm_lyr delete is start...\n");
						GUI_LyrWinDelete(app_new_music_ctrl_para->new_music_frm_lyr);
						__wrn("new_music_frm_lyr delete is 1...\n");
						//app_new_music_ctrl_para->new_music_frm_lyr = NULL;
						__wrn("new_music_frm_lyr delete is 2...\n");
						if(app_new_music_ctrl_para->new_music_frm_lyr == NULL){
							__wrn("new_music_frm_lyr delete is success...\n");
						}
					}
					//new_music_ui_scene_delete((void *)app_new_music_ctrl_para->new_music_frm_win);
				#endif
				
				#if 1	//删除播放列表窗口图层
					if(app_new_music_ctrl_para->playlist_frm_lyr){//删除播放列表图层
						__wrn("playlist_frm_lyr delete is start...\n");
						GUI_LyrWinDelete(app_new_music_ctrl_para->playlist_frm_lyr);
						app_new_music_ctrl_para->playlist_frm_lyr = NULL;
						if(app_new_music_ctrl_para->playlist_frm_lyr == NULL){
							__wrn("playlist_frm_lyr delete is success...\n");
						}
					}
				#endif

				#if 1	//删除频谱控件窗口
					if(app_new_music_ctrl_para->h_spectrum_win!=NULL){//删除频谱framewin窗口
						__wrn("h_spectrum_win delete is start...\n");
						GUI_LyrWinDelete(app_new_music_ctrl_para->h_spectrum_win);
						app_new_music_ctrl_para->h_spectrum_win = NULL;
						if(app_new_music_ctrl_para->h_spectrum_win == NULL){
							__wrn("h_spectrum_win delete is success...\n");
						}
					}
					#if 1	//删除频谱framewin窗口图层
					if(app_new_music_ctrl_para->h_spectrum_lyr){//删除频谱framewin窗口图层
						__wrn("h_spectrum_lyr delete is start...\n");
						GUI_LyrWinDelete(app_new_music_ctrl_para->h_spectrum_lyr);
						app_new_music_ctrl_para->h_spectrum_lyr = NULL;
						if(app_new_music_ctrl_para->h_spectrum_lyr == NULL){
							__wrn("h_spectrum_lyr delete is success...\n");
						}
					}
					#endif
					NEW_UnRegisterSpectrumControl();//删除framewin窗口下的控件窗口
					
				#endif
				
				eLIBs_memset(app_new_music_ctrl_para, 0, sizeof(app_new_music_ctrl_t));//先初始化结构体，数据清0
				esMEMS_Bfree(app_new_music_ctrl_para, sizeof(app_new_music_ctrl_t));//释放内存空间
				__wrn("app_new_music_ctrl_para GUI_MSG_DESTROY  is success...\n");
			}
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2，绘制*/


		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{/*3，关闭*/

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{/*4，退出*/
			__wrn("__app_new_music_proc GUI_MSG_QUIT  is ok\n");
			app_new_music_cmd2para(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);//向app_root_scene.c根目录发送manwin窗口切换切换命令
		}
		return EPDK_OK;

		case GUI_MSG_KEY:{/*5，按键*/
			
			switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{/*退出窗口*/
					__gui_msg_t mmsg;
					mmsg.id = GUI_MSG_QUIT;
					mmsg.h_deswin = msg->h_deswin;
					GUI_SendMessage(&mmsg);
				}
				break;
			}
		}
		break;

		case GUI_MSG_COMMAND:{/*7，命令*/
			app_new_music_ctrl_t 	*app_new_music_ctrl_para;//manwin窗口的结构体参数
			app_new_music_ctrl_para = (app_new_music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
							
			switch(LOWORD(msg->dwAddData1)){
				#if 1//接收到来自new_music.c的framewin窗口命令
				__wrn("NEW_MUSIC_ID  is....\n");
				case NEW_MUSIC_ID:{/*播放器*/
				
					__wrn("NEW_MUSIC_ID  is ok\n");
					__wrn("HIWORD(msg->dwAddData1) = %d is....\n",HIWORD(msg->dwAddData1));
					switch(HIWORD(msg->dwAddData1)){
						case new_music_play_key_exit:{		//发送退出播放器窗口命令
							__gui_msg_t mymsg;
							mymsg.id = GUI_MSG_QUIT;
							mymsg.h_deswin = msg->h_deswin;	//本父窗口窗口的窗口句柄
							GUI_SendMessage(&mymsg);		//向本父窗口窗口发送信息
						}
						return EPDK_OK;

						case MSG_PLAY_OP_UP:{				//接收到播放上一首命令，然后向播放列表窗口发送上一首命令
							__gui_msg_t playlistmsg;
							playlistmsg.id = MSG_PLAYLIST_OP_UP;
							playlistmsg.h_deswin = app_new_music_ctrl_para->playlist_frm_win;//播放列表窗口的窗口句柄
							playlistmsg.dwAddData2 = msg->dwAddData2;	//当前播放的文件索引id
							__wrn("playlistmsg.dwAddData2 = %d\n",playlistmsg.dwAddData2);
							GUI_SendMessage(&playlistmsg);				//向播放列表窗口发送信息
						}
						return EPDK_OK;

						case MSG_PLAY_OP_DOWN:{//接收到播下一首命令，然后向播放列表窗口发送下一首命令
							__gui_msg_t playlistmsg;
							playlistmsg.id = MSG_PLAYLIST_OP_DOWN;
							playlistmsg.h_deswin = app_new_music_ctrl_para->playlist_frm_win;//播放列表窗口的窗口句柄
							playlistmsg.dwAddData2 = msg->dwAddData2;//当前播放的文件索引id
							__wrn("playlistmsg.dwAddData2 = %d\n",playlistmsg.dwAddData2);
							GUI_SendMessage(&playlistmsg);//向播放列表窗口发送信息
						}
						return EPDK_OK;

						default:{
							break;
						}
					}
				}
				return EPDK_OK;
				#endif

				case NEW_MUSIC_PLAYLIST_ID:{/*播放列表*/
					switch(HIWORD(msg->dwAddData1)){
						case send_cmd_play_prev_to_PlayDev:{//接收来自播放列表framewin窗口接收到的播放器发送过来的命令
							#if 0 //向播放列表本窗口发送上一个命令
							{	
								__gui_msg_t playlistmsg;
								playlistmsg.id = MSG_PLAYLIST_OP_UP;
								playlistmsg.h_deswin = app_new_music_ctrl_para->playlist_frm_win;//本窗口的窗口句柄
	
								GUI_SendMessage(&playlistmsg);//向本窗口发送信息
							}
							#endif
							#if 1 //向播放器发送播放命令
							{	
								__gui_msg_t playmsg;
								playmsg.id = MSG_PLAY_OP_PLAY_BY_INDEX;//播放命令id
								playmsg.h_deswin = app_new_music_ctrl_para->new_music_frm_win;//播放器窗口的窗口句柄
								playmsg.dwAddData2 = msg->dwAddData2;//文件索引id
								playmsg.dwReserved = msg->dwReserved;//播放命令预留位暂时没用上，0为暂停，1为播放
								__wrn("playmsg.dwAddData2 = %d\n",playmsg.dwAddData2);
								GUI_SendMessage(&playmsg);//向播放器窗口发送信息
							}
							#endif
						}
						return EPDK_OK;

						case playlist_send_cmd_prev_play:{//接收来自播放列表framewin窗口按键操作的命令
							#if 1 //向播放列表本窗口发送上一个命令
							{	
								__gui_msg_t playlistmsg;
								playlistmsg.id = MSG_PLAYLIST_OP_UP;
								playlistmsg.h_deswin = app_new_music_ctrl_para->playlist_frm_win;//本播放列表窗口的窗口句柄
								playlistmsg.dwAddData2 = msg->dwAddData2;
								GUI_SendMessage(&playlistmsg);//向本窗口发送信息
							}
							#endif

						}
						return EPDK_OK;

						case playlist_send_cmd_next_play:{//接收来自播放列表framewin窗口按键操作的命令
							#if 1 //向播放列表本窗口发送下一个命令
							{	
								__gui_msg_t playlistmsg;
								playlistmsg.id = MSG_PLAYLIST_OP_DOWN;
								playlistmsg.h_deswin = app_new_music_ctrl_para->playlist_frm_win;//本播放列表窗口的窗口句柄
								playlistmsg.dwAddData2 = msg->dwAddData2;
								GUI_SendMessage(&playlistmsg);//向本窗口发送信息
							}
							#endif

						}
						return EPDK_OK;
						
						case send_cmd_playlist_exit:{		//发送退出播放列表窗口命令
							__gui_msg_t mymsg;
							mymsg.id = GUI_MSG_QUIT;
							mymsg.h_deswin = msg->h_deswin;	//本父窗口窗口的窗口句柄
							GUI_SendMessage(&mymsg);		//向本父窗口窗口发送信息
						}
						return EPDK_OK;

						default:{
							break;
						}
					}
					
				}
				return EPDK_OK;
				
				
			}

		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{/*19，定时器*/

		}
		break;
		
		default:
			break;

	}


	return GUI_ManWinDefaultProc(msg);
}

//创建音乐应用的manwin窗口
H_WIN	app_new_music_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	app_new_music_ctrl_t *new_music_ctrl;
	H_WIN	hManWin;//用于判断manwin窗口创建是否成功

	new_music_ctrl = (app_new_music_ctrl_t *)esMEMS_Balloc(sizeof(app_new_music_ctrl_t));
	eLIBs_memset(new_music_ctrl, 0, sizeof(app_new_music_ctrl_t));
	new_music_ctrl->font = para->font;
	new_music_ctrl->root_type = para->root_type;
	/*开始操作配置manwin窗口参数*/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));//结构体初始化，数据清0
	create_info.name	=	APP_NEWMUSIC;//APP_NEWPHOTO;
	create_info.hParent	=	para->h_parent;
	create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_music_proc);
	create_info.attr	=	(void *)new_music_ctrl;//para;
	create_info.id		=	APP_NEWMUSIC_ID;
	create_info.hHosting = NULL;

	hManWin = GUI_ManWinCreate(&create_info);//返回一个创建好的manwin窗口
	if(!hManWin){
		__wrn("manwin create is false...");
		return EPDK_FALSE;
	}

	
	return hManWin;
}




