
//libaiao

#include "beetles_app.h"
#include "music_ui.h"
#if 0
static music_ui_t  music_ui_400_240 =
{
	//layer window
	{
		PIXEL_COLOR_ARGB8888,       //fmt
		{ 480, 272 },  //{ 400, 240 },             //fb size
		{ 0, 0, 480, 272 },  // { 0, 0, 400, 240 },       //src rect
		{ 0, 0, 480, 272 },  //{ 0, 0, 400, 240 },       //scn rect
		{ 0 },                      //reserved
	},

	//list window, 浏览文件
	{
		{0, 2, 188, 192},						//listbar area
		{0, 0, 188, 32},						//item rectangle
		{5, 9, 181, 22},						//focus icon(select border)
		{11, 12, 15, 15 },						//file icon
		{36, 0, 148, 36},		//text rectangle
	},

	//scroll bar ui
	{
		{219,	7,	7,	185},					//scroll bar background
		{221,	22,	3,	155},					//scroll
		6,										//head
		1,										//body
		6,										//tail
	},

	//head bar ui
	{
		{250, 40, 480 - 210, 150 - 40}, //{210,40,400-210,150-40},					//headbar background
#if SP_APP_CALENDAR
		{299 - 20 + 140, 3, 40, 24},								//声道
		{238, 5, 26, 16},	//{198,5,26,16},				//播放模式状态图标
		{271, 5, 50, 16},								//EQ模式状态图标
		{271, 5, 26, 16},								//背光关闭时间图标
		{328, 5, 26, 16},								//音量图标
		{350, 3, 35, 25},								//音量text
		{386, 6, 31, 12},								//电池图标
		{390, 3, 85, 25},								//时间显示区域
#else
		{299 - 20 + 140, 3, 40, 24},								//声道
		{248, 5, 26, 16},								//播放模式状态图标
		{286, 5, 50, 16},								//EQ模式状态图标
		{271, 5, 26, 16},								//背光关闭时间图标
		{348, 5, 26, 16},								//音量图标
		{379, 3, 18, 16},								//音量text
		{411, 6, 31, 12},								//电池图标
		{0, 0, 0, 16},									//时间显示区域
#endif
	},
	//palyer ui
	{
		{250, 40, 480 - 250 - 5, 60}, //{210,40,400-210,120-40},					//控制按钮背景区域()与频谱共用
		{270, 68, 200, 40}, //{230,120,150,16},							//按键提示字符区域
		{294, 48, 26, 16},	//{294,68,26,16},							//播放模式按钮
		{328, 48, 26, 16},								//EQ模式按钮
		{364, 48, 26, 16},								// 背光时间控制按钮

		{268, 80, 26, 16},							//上一首按钮
		{301, 80, 26, 16},							//下一首按钮
		{334, 80, 26, 16},							//播放暂停按钮
		{367, 80, 26, 16},							//快退按钮
		{400, 80, 26, 16},							//快进按钮

		{259, 154 - 10 + 40 - 5, 26, 20},							//播放状态图标(暂停或播放)

		{287 - 30 - 20, 129 + 40 - 10 - 5, 85, 20},	              		//当前播放时间
		{385 , 129 + 40 - 10 - 5, 85, 20},						//总时间显示区域

		{284, 160 - 10 + 40 - 5, 5, 4},								//进度条head
		{284, 160 - 10 + 40 - 5, 146, 4},							//进度条body
		{284 + 5, 160 - 10 + 40 - 5, 5, 4},							//进度条point,从head之后，因此加5
		{284 + 146 - 5, 160 - 10 + 40 - 5, 5, 4},						//进度条tail

		{253,		183, 26, 16},						//演唱者图标
		{253 + 26,	183, 70, 16},						//演唱者名称
		{350,		183, 26, 16},						//文件大小图标
		{350 + 26,	183, 70, 16},						//文件大小显示

		{253,		213, 26, 16},						//专辑名称图标
		{253 + 26,	213, 70, 16},						//专辑名称
		{350,		213, 26, 16},						//bitrate图标
		{350 + 26,	213, 70, 16},						//bitrate大小显示
	},

	//volume bar ui
	{
		{0, 0, 0, 0},									//整个音量控制显示区域
		{0, 0, 0, 0},									//左边speaker图标显示，有两种状态
		{0, 0, 0, 0},									//中间音量显示进度条
		{0, 0, 0, 0},    								//右边speaker图标显示
	},

	//spectrum    频谱
	{
		{210, 40, 400 - 210, 60}
	},
	//icon table
	{
		//ID															res	handle,    	res buffer
		//音乐浏览器
		{ ID_MUSIC_PALETTE_BMP,           			NULL,  				 NULL },    //
		{ NULL,           	NULL,  				 NULL },    //
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//文件图标，未被选中时
		{ ID_MUSIC_FILE_FOCUSED_BMP,      			NULL,  				 NULL }, 	//文件图标，被选中时
		{ ID_MUSIC_LIST_ITEM_FOCUS_BMP,    			NULL,  				 NULL }, 	//选择框
		{ ID_MUSIC_LONG_STR_SCROLL_BMP,				NULL,  			 	 NULL },     //长文件名滚动背景条
		//header bar
		//play mode
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 	//	播放全部,不重复
		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 	//播放一首
		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 	//随即播放
		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 	//重复所有
		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 	//重复一首

		//EQ mode
		{ ID_MUSIC_EQ_NORMAL_BMP,    				NULL,  				 NULL }, 	//EQ normal
		{ ID_MUSIC_EQ_CLASSIC_BMP,    				NULL,  				 NULL }, 	//EQ classic
		{ ID_MUSIC_EQ_ROCK_BMP,    					NULL,  				 NULL }, 	//	EQ Rock
		{ ID_MUSIC_EQ_JAZZ_BMP,    					NULL,  				 NULL }, 	//	EQ JAZZ
		{ ID_MUSIC_EQ_POP_BMP,    					NULL,  				 NULL }, 	//	EQ POP

		//BL Time
		{ ID_MUSIC_BL_ON_BMP,    					NULL,  				 NULL }, 	//	Backlight never off
		{ ID_MUSIC_BL_10S_BMP,    					NULL,  				 NULL }, 	//last 10 second
		{ ID_MUSIC_BL_30S_BMP,    					NULL,  				 NULL }, 	//
		{ ID_MUSIC_BL_60S_BMP,    					NULL,  				 NULL }, 	//
		{ ID_MUSIC_BL_90S_BMP,    					NULL,  				 NULL }, 	//

		//volume
		{ ID_MUSIC_VOLUME_STATUS_BMP,    			NULL,  				 NULL }, 	//

		//bettery
		{ ID_MUSIC_BETTERY0_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY1_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY2_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY3_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY4_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY5_BMP,    				NULL,  				 NULL }, 	//

		{ ID_MUSIC_CHARGE_BETTERY0_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY1_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY2_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY3_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY4_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY5_BMP,    				NULL,  				 NULL }, 	//

		//播放器
		//play mode button
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 		//播放全部, unfocused
		{ ID_MUSIC_PLAY_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//播放全部, focused
		{ ID_MUSIC_PLAY_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//播放全部, pressed

		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 		//播放一首, unfocused
		{ ID_MUSIC_PLAY_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//播放一首, focused
		{ ID_MUSIC_PLAY_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//播放一首, pressed

		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 		//随机播放, unfocused
		{ ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//随机播放, focused
		{ ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//随机播放, pressed

		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 		//重复播放全部, unfocused
		{ ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//重复播放全部, focused
		{ ID_MUSIC_REPEAT_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//重复播放全部, pressed

		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 		//重复播放一首, unfocused
		{ ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//重复播放一首, focused
		{ ID_MUSIC_REPEAT_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//重复播放一首, pressed

		//EQ mode button
		{ ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_EQ_MODE_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_EQ_MODE_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//BL time button
		{ ID_MUSIC_BL_TIME_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_BL_TIME_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_BL_TIME_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//play previous button
		{ ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//播放上一曲, unfocused
		{ ID_MUSIC_PLAY_PRE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PRE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//pressed
		//play next button
		{ ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//播放下一曲, unfocused
		{ ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_NEXT_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed
		//play pause button
		{ ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//播放暂停, unfocused
		{ ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed

		//play backward button
		{ ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//快退, unfocused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play forward button
		{ ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//快进, unfocused
		{ ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play status button,
		{ ID_MUSIC_PLAYING_STATUS_BMP,				NULL,				NULL }, 								//正在播放状态
		{ ID_MUSIC_PAUSE_STATUS_BMP,					NULL,  				NULL }, 								//暂停状态
		{ ID_MUSIC_PREVIOUS_STATUS_BMP,				NULL,  		 		NULL }, 								//上一曲状态
		{ ID_MUSIC_NEXT_STATUS_BMP,					NULL,  				NULL }, 								//下一曲状态

		//process bar
		{ ID_MUSIC_PROCESS_BAR_HEAD_BMP,				NULL,  			 	NULL }, 								//process bar 头部
		{ ID_MUSIC_PROCESS_BAR_BODY_BMP,				NULL,  				NULL }, 								//body
		{ ID_MUSIC_PROCESS_BAR_TAIL_BMP,				NULL,  		 		NULL }, 								//尾部
		{ ID_MUSIC_PROCESS_BAR_POINT_BMP,			NULL,  				NULL }, 								//进度点

		//歌曲信息图标
		{ ID_MUSIC_PERFORMER_NAME_BMP,				NULL,  			 	NULL }, 								//作者图标
		{ ID_MUSIC_FILE_SIZE_BMP,					NULL,  				NULL }, 								//
		{ ID_MUSIC_ALBUM_NAME_BMP,					NULL,  		 		NULL }, 								//
		{ ID_MUSIC_BIT_RATE_BMP,						NULL,  				NULL }, 								//

		//频谱图图标
		{ ID_MUSIC_PARTICLE_BMP,						NULL,  			 	NULL },
		{ ID_MUSIC_BOUY_BMP,							NULL,  			 	NULL },

		//浏览器，滚动条
		{ ID_MUSIC_LIST_SCROLL_BG_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_LIST_SCROLL_HEAD_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_LIST_SCROLL_BODY_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_LIST_SCROLL_TAIL_BMP,				NULL,  			 	NULL },


		{ ID_MUSIC_NUM0_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM1_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM2_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM3_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM4_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM5_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM6_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM7_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM8_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_NUM9_BMP,				NULL,  			 	NULL },

		{ 0,                                    		NULL,  				NULL }      	//browser_icon_max
	},
};
#endif
static music_ui_t  music_ui_800_480 =
{
	//layer window
	{
		PIXEL_COLOR_ARGB8888,       //fmt
		{ 1024, 600 },  //{ 800, 480},             //fb size
		{ 0, 0, 1024, 600 },  // { 0, 0, 800, 480 },       //src rect
		{ 0, 0, 1024, 600 },  //{ 0, 0, 800, 480 },       //scn rect
		{ 0 },                      //reserved
	},

	//list window, 浏览文件
	{
#if SP_FavoriteFiles
		{0, 0, 188 + 150, 448},						//listbar area音乐列表的区域
		{0, 0, 188 + 150, 32},						//item rectangle 单首音乐的区域
		{5, 9, 188 + 150, 25},						//focus icon(select border)选中音乐图片区域
		{11, 12, 15, 15},						//file icon文件类型图标
		{36, 0, 148 + 150, 38},		//text rectangle文字区域
#else
		{70, 25, 386 , 544},						//listbar area音乐列表的区域
		{70, 25, 386 , 32},						//item rectangle 单首音乐的区域
		{5, 9, 316, 22},						//focus icon(select border)选中音乐图片区域
		{11, 12, 16, 16},						//file icon文件类型图标
		{36, 0, 148 + 136, 38},		//text rectangle文字区域
#endif

	},

	//scroll bar ui
	{
#if SP_FavoriteFiles
		{219 + 123, 7,	7, 185 + 255},					//scroll bar background音乐列表滚动条背景位置
		{221 + 123, 22, 5, 155 + 255},					//scroll音乐列表滚动条位置
#else
		{219 + 121 + 105, 7+25,	7, 535},					//scroll bar background音乐列表滚动条背景位置
		{221 + 121 + 105, 22+25, 5, 505},					//scroll音乐列表滚动条位置
#endif
		8,										//head音乐列表头部三角形
		1,										//body音乐列表黄色位置
		8,										//tail音乐列表底部三角形
	},

	//head bar ui
	{
		{250 + 200 + 80, 40, 270, 110}, //{210,40,400-210,150-40},					//headbar background播放器头部背景
#if SP_APP_CALENDAR
		{299 - 20 + 140, 3, 16 * 8, 24},								//声道
		{238, 5, 26, 16},	//{198,5,26,16},				//播放模式状态图标
		{271, 5, 50, 16},								//EQ模式状态图标
		{271, 5, 26, 16},								//背光关闭时间图标
		{328, 5, 26, 16},								//音量图标
		{350, 3, 35, 25},								//音量text
		{386, 6, 31, 12},								//电池图标
		{390, 3, 85, 25},								//时间显示区域
#else
		{680 + 160, 3 + 25, 16 * 8, 24},								//声道
		{500 + 160, 5 + 25, 26, 16},								//播放模式状态图标
		{550 + 170, 6 + 25, 50, 16},								//EQ模式状态图标
		{271 + 200, 5, 26, 16},								//背光关闭时间图标
		{348 + 200, 5, 26, 16},								//音量图标
		{379 + 200, 3, 18, 16},								//音量text
		{411 + 200, 6, 31, 12},								//电池图标
		{0, 0, 0, 16},									//时间显示区域

#endif
	},
	//palyer ui
	{
		{400 + 110, 5 + 25, 100, 40}, //{210,40,400-210,120-40},					//控制按钮背景区域()与频谱共用
		{400 + 110, 20 + 25, 20 * 8, 40}, //{230,120,150,16},						//按键提示字符区域
		{400 + 110, 5 + 25, 26, 16},	//{294,68,26,16},							//播放模式按钮
		{450 + 110, 5 + 25, 26, 16},								//EQ模式按钮
		{450 + 110, 5 + 25, 26, 16},								// 背光时间控制按钮

		{268 + 200 + 130, 80 + 70, 26, 16},							//上一首按钮
		{301 + 200 + 130, 80 + 70, 26, 16},							//下一首按钮
		{334 + 200 + 130, 80 + 70, 26, 16},							//播放暂停按钮
		{367 + 200 + 130, 80 + 70, 26, 16},							//快退按钮
		{400 + 200 + 130, 80 + 70, 26, 16},							//快进按钮

		{259 + 165 + 100, 179 + 70 + 60 + 1, 26, 20},						//播放状态图标(暂停或播放)
		{237 + 180 + 100, 154 + 70 + 60, 85, 20},	              		//当前播放时间
		{385 + 299 + 200, 154 + 70 + 60, 85, 20},						//总时间显示区域

		{284 + 160 + 100, 185 + 70 + 60, 5, 5},								//进度条head
		{284 + 160 + 100, 185 + 70 + 60, 276 + 100, 5},							//进度条body  
		{289 + 160 + 100, 185 + 70 + 60, 5, 5},							//进度条point,从head之后，因此加5
		{425 + 160 + 100, 185 + 70 + 60, 5, 5},						//进度条tail

		{253 + 200 + 50 + 30 - 20,		183 + 70 + 80, 26, 16},						//演唱者图标
		{253 + 26 + 200 + 50 + 30 -20,	183 + 70 + 80, 240, 16},						//演唱者名称
		{350 + 200 + 50 + 220 -30,		183 + 70 + 80, 26, 16},						//文件大小图标
		{350 + 26 + 200 + 50 + 220 -30,	183 + 70 + 80, 80, 16},						//文件大小显示

		{253 + 200 + 50 + 30 -20,		213 + 70 + 80, 26, 16},						//专辑名称图标
		{253 + 26 + 200 + 50 + 30 -20,	213 + 70 + 80, 240, 16},						//专辑名称
		{350 + 200 + 50 + 220 -30,		213 + 70 + 80, 26, 16},						//bitrate图标
		{350 + 26 + 200 + 50 + 220 -30,	213 + 70 + 80, 80, 16},						//bitrate大小显示
	},

	//volume bar ui
	{
		{0, 0, 0, 0},									//整个音量控制显示区域
		{0, 0, 0, 0},									//左边speaker图标显示，有两种状态
		{0, 0, 0, 0},									//中间音量显示进度条
		{0, 0, 0, 0},    								//右边speaker图标显示
	},

	//spectrum    频谱
	{
		{0 , 0,  0,  0}
	},
	//icon table
	{
		//ID															res	handle,    	res buffer
		//音乐浏览器
		{ ID_MUSIC_PALETTE_BMP,           			NULL,  				 NULL },    //
		{ NULL,           	NULL,  				 NULL },    //
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//文件图标，未被选中时
		{ ID_MUSIC_FILE_FOCUSED_BMP,      			NULL,  				 NULL }, 	//文件图标，被选中时
		{ ID_MUSIC_LIST_ITEM_FOCUS_BMP,    			NULL,  				 NULL }, 	//选择框
		{ ID_MUSIC_LONG_STR_SCROLL_BMP,				NULL,  			 	 NULL },     //长文件名滚动背景条
		//header bar
		//play mode
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 	//	播放全部,不重复
		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 	//播放一首
		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 	//随即播放
		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 	//重复所有
		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 	//重复一首

		//EQ mode
		{ ID_MUSIC_EQ_NORMAL_BMP,    				NULL,  				 NULL }, 	//EQ normal
		{ ID_MUSIC_EQ_CLASSIC_BMP,    				NULL,  				 NULL }, 	//EQ classic
		{ ID_MUSIC_EQ_ROCK_BMP,    					NULL,  				 NULL }, 	//	EQ Rock
		{ ID_MUSIC_EQ_JAZZ_BMP,    					NULL,  				 NULL }, 	//	EQ JAZZ
		{ ID_MUSIC_EQ_POP_BMP,    					NULL,  				 NULL }, 	//	EQ POP

		//BL Time
		{ ID_MUSIC_BL_ON_BMP,    					NULL,  				 NULL }, 	//	Backlight never off
		{ ID_MUSIC_BL_10S_BMP,    					NULL,  				 NULL }, 	//last 10 second
		{ ID_MUSIC_BL_30S_BMP,    					NULL,  				 NULL }, 	//
		{ ID_MUSIC_BL_60S_BMP,    					NULL,  				 NULL }, 	//
		{ ID_MUSIC_BL_90S_BMP,    					NULL,  				 NULL }, 	//

		//volume
		{ ID_MUSIC_VOLUME_STATUS_BMP,    			NULL,  				 NULL }, 	//

		//bettery
		{ ID_MUSIC_BETTERY0_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY1_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY2_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY3_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY4_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_BETTERY5_BMP,    				NULL,  				 NULL }, 	//

		{ ID_MUSIC_CHARGE_BETTERY0_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY1_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY2_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY3_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY4_BMP,    				NULL,  				 NULL }, 	//
		{ ID_MUSIC_CHARGE_BETTERY5_BMP,    				NULL,  				 NULL }, 	//

		//播放器
		//play mode button
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 		//播放全部, unfocused
		{ ID_MUSIC_PLAY_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//播放全部, focused
		{ ID_MUSIC_PLAY_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//播放全部, pressed

		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 		//播放一首, unfocused
		{ ID_MUSIC_PLAY_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//播放一首, focused
		{ ID_MUSIC_PLAY_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//播放一首, pressed

		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 		//随机播放, unfocused
		{ ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//随机播放, focused
		{ ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//随机播放, pressed

		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 		//重复播放全部, unfocused
		{ ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//重复播放全部, focused
		{ ID_MUSIC_REPEAT_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//重复播放全部, pressed

		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 		//重复播放一首, unfocused
		{ ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//重复播放一首, focused
		{ ID_MUSIC_REPEAT_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//重复播放一首, pressed

		//EQ mode button
		{ ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_EQ_MODE_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_EQ_MODE_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//BL time button
		{ ID_MUSIC_BL_TIME_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_BL_TIME_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_BL_TIME_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//play previous button
		{ ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//播放上一曲, unfocused
		{ ID_MUSIC_PLAY_PRE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PRE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//pressed
		//play next button
		{ ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//播放下一曲, unfocused
		{ ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_NEXT_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed
		//play pause button
		{ ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//播放暂停, unfocused
		{ ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed

		//play backward button
		{ ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//快退, unfocused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play forward button
		{ ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//快进, unfocused
		{ ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play status button,
		{ ID_MUSIC_PLAYING_STATUS_BMP,				NULL,				NULL }, 								//正在播放状态
		{ ID_MUSIC_PAUSE_STATUS_BMP,					NULL,  				NULL }, 								//暂停状态
		{ ID_MUSIC_PREVIOUS_STATUS_BMP,				NULL,  		 		NULL }, 								//上一曲状态
		{ ID_MUSIC_NEXT_STATUS_BMP,					NULL,  				NULL }, 								//下一曲状态

		//process bar
		{ ID_MUSIC_PROCESS_BAR_HEAD_BMP,				NULL,  			 	NULL }, 								//process bar 头部
		{ ID_MUSIC_PROCESS_BAR_BODY_BMP,				NULL,  				NULL }, 								//body
		{ ID_MUSIC_PROCESS_BAR_TAIL_BMP,				NULL,  		 		NULL }, 								//尾部
		{ ID_MUSIC_PROCESS_BAR_POINT_BMP,			NULL,  				NULL }, 								//进度点

		//歌曲信息图标
		{ ID_MUSIC_PERFORMER_NAME_BMP,				NULL,  			 	NULL }, 								//作者图标
		{ ID_MUSIC_FILE_SIZE_BMP,					NULL,  				NULL }, 								//
		{ ID_MUSIC_ALBUM_NAME_BMP,					NULL,  		 		NULL }, 								//
		{ ID_MUSIC_BIT_RATE_BMP,						NULL,  				NULL }, 								//

		//频谱图图标
		{ ID_MUSIC_PARTICLE_BMP,						NULL,  			 	NULL },
		{ ID_MUSIC_BOUY_BMP,							NULL,  			 	NULL },

		//浏览器，滚动条
		{ ID_MUSIC_LIST_SCROLL_BG_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_LIST_SCROLL_HEAD_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_LIST_SCROLL_BODY_BMP,				NULL,  			 	NULL },
		{ ID_MUSIC_LIST_SCROLL_TAIL_BMP,				NULL,  			 	NULL },
#if SP_FavoriteFiles
		{ ID_MUSIC_FAVOLITE_UNFOCUSED_BMP,			NULL,				NULL },
		{ ID_MUSIC_FAVOLITE_FOCUSED_BMP,				NULL,				NULL },
#endif

		{ ID_MOVIE_NUM0_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM1_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM2_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM3_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM4_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM5_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM6_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM7_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM8_BMP,				NULL,  			 	NULL },
		{ ID_MOVIE_NUM9_BMP,				NULL,  			 	NULL },
#ifdef ID_MOVIE_NUMLOCK_BG_BMP
		{ ID_MOVIE_NUMLOCK_BG_BMP,			NULL,				NULL },
#endif

		{ 0,                                NULL,  				NULL }      	//browser_icon_max
	},
	
};


//获得整个explorer AP设计的UI参数
music_ui_t *music_get_ui_param(void)
{
	return &(music_ui_800_480);
}

//获得音乐浏览器UI参数
music_list_ui_t *music_get_list_ui_param(void)
{
	return &(music_ui_800_480.list_ctrl);
}

//获得顶部HeadbarUI参数
music_headbar_ui_t *music_get_headbar_ui_param(void)
{
	return 	&(music_ui_800_480.headbar);
}

//获得顶部playerUI参数
music_palyer_ui_t *music_get_player_ui_param(void)
{
	return 	&(music_ui_800_480.player);
}

//获得volume barUI参数
music_volume_bar_ui_t *music_get_volume_bar_ui_param(void)
{
	return 	&(music_ui_800_480.volume_bar);
}
/*
************************************************************************************************************************
*Function	:         void* explorer_get_listview_icon_res(__u32 index)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
void *music_get_icon_res(__u32 index)
{
	music_icon_data_t *icon_table = NULL;
	icon_table = music_ui_800_480.icon_table;

	if(index > MUSIC_ICON_MAX)
	{
		return NULL;
	}

	//__msg("----icon_id: %d, res_id: %d\n", index, icon_table[index].res_id);
	if(icon_table == NULL)
	{
		return NULL;
	}
	else if(icon_table[index].res_id == 0)
	{
		return NULL;
	}
	else if(icon_table[index].res_handle == NULL)
	{
#if 1
		icon_table[index].res_handle = theme_open(icon_table[index].res_id);
		icon_table[index].res_buffer = theme_hdl2buf(icon_table[index].res_handle);
		return icon_table[index].res_buffer;
#else
		{
			int i = 0;
			__msg("------------open all of the picture at first\n");
			app_print_memory_usage("new memory for picture resource", NULL);
			app_print_memory_usage("new memory for picture resource", NULL);

			for(i = 0; i < MUSIC_ICON_MAX; i++)
			{
				if(icon_table[i].res_handle == NULL)
				{
					icon_table[i].res_handle = theme_open(icon_table[i].res_id);
					icon_table[i].res_buffer = theme_hdl2buf(icon_table[i].res_handle);
				}
			}

			app_print_memory_usage("new memory for picture resource finished", NULL);
			return icon_table[index].res_buffer;
		}
#endif
	}
	else
	{
		return icon_table[index].res_buffer;
	}
}

/*
************************************************************************************************************************
*Function	:         __s32 explorer_free_listview_icon_res(void)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 music_free_icon_res(void)
{
	__s32 i = 0;
	music_icon_data_t *icon_table = NULL;
	icon_table = music_ui_800_480.icon_table;

	if(icon_table == NULL)
	{
		return EPDK_FAIL;
	}
	else
	{
		//app_print_memory_usage("release memory for picture resource", NULL);
		//app_print_memory_usage("release memory for picture resource", NULL);
		for(i = 0; i < MUSIC_ICON_MAX; i++)
		{
			if(icon_table[i].res_handle != NULL)
			{
				theme_close(icon_table[i].res_handle);
				icon_table[i].res_handle = NULL;
				icon_table[i].res_buffer = NULL;
			}
		}

		//app_print_memory_usage("release memory for picture resource finished", NULL);
		return EPDK_OK;
	}
}

//获得Explorer 窗口layer size
__s32 music_get_screen_rect(RECT *screen_rect)
{
	music_ui_t *ui_param;
	ui_param = music_get_ui_param();
	screen_rect->x = ui_param->layer.scn_rect.x;
	screen_rect->y = ui_param->layer.scn_rect.y;
	screen_rect->width = ui_param->layer.scn_rect.width;
	screen_rect->height = ui_param->layer.scn_rect.height;
	return EPDK_OK;
}

//计算listbar每页可以显示的条目数量
__u32 music_count_listbar_pagenum(void)
{
	__u32 pagenum = 0;
	music_ui_t *ui_param;
	ui_param = music_get_ui_param();
	pagenum = ui_param->list_ctrl.listbar_rect.height / ui_param->list_ctrl.item_rect.height;
	return pagenum;
}













