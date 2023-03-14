
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

	//list window, ����ļ�
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
		{299 - 20 + 140, 3, 40, 24},								//����
		{238, 5, 26, 16},	//{198,5,26,16},				//����ģʽ״̬ͼ��
		{271, 5, 50, 16},								//EQģʽ״̬ͼ��
		{271, 5, 26, 16},								//����ر�ʱ��ͼ��
		{328, 5, 26, 16},								//����ͼ��
		{350, 3, 35, 25},								//����text
		{386, 6, 31, 12},								//���ͼ��
		{390, 3, 85, 25},								//ʱ����ʾ����
#else
		{299 - 20 + 140, 3, 40, 24},								//����
		{248, 5, 26, 16},								//����ģʽ״̬ͼ��
		{286, 5, 50, 16},								//EQģʽ״̬ͼ��
		{271, 5, 26, 16},								//����ر�ʱ��ͼ��
		{348, 5, 26, 16},								//����ͼ��
		{379, 3, 18, 16},								//����text
		{411, 6, 31, 12},								//���ͼ��
		{0, 0, 0, 16},									//ʱ����ʾ����
#endif
	},
	//palyer ui
	{
		{250, 40, 480 - 250 - 5, 60}, //{210,40,400-210,120-40},					//���ư�ť��������()��Ƶ�׹���
		{270, 68, 200, 40}, //{230,120,150,16},							//������ʾ�ַ�����
		{294, 48, 26, 16},	//{294,68,26,16},							//����ģʽ��ť
		{328, 48, 26, 16},								//EQģʽ��ť
		{364, 48, 26, 16},								// ����ʱ����ư�ť

		{268, 80, 26, 16},							//��һ�װ�ť
		{301, 80, 26, 16},							//��һ�װ�ť
		{334, 80, 26, 16},							//������ͣ��ť
		{367, 80, 26, 16},							//���˰�ť
		{400, 80, 26, 16},							//�����ť

		{259, 154 - 10 + 40 - 5, 26, 20},							//����״̬ͼ��(��ͣ�򲥷�)

		{287 - 30 - 20, 129 + 40 - 10 - 5, 85, 20},	              		//��ǰ����ʱ��
		{385 , 129 + 40 - 10 - 5, 85, 20},						//��ʱ����ʾ����

		{284, 160 - 10 + 40 - 5, 5, 4},								//������head
		{284, 160 - 10 + 40 - 5, 146, 4},							//������body
		{284 + 5, 160 - 10 + 40 - 5, 5, 4},							//������point,��head֮����˼�5
		{284 + 146 - 5, 160 - 10 + 40 - 5, 5, 4},						//������tail

		{253,		183, 26, 16},						//�ݳ���ͼ��
		{253 + 26,	183, 70, 16},						//�ݳ�������
		{350,		183, 26, 16},						//�ļ���Сͼ��
		{350 + 26,	183, 70, 16},						//�ļ���С��ʾ

		{253,		213, 26, 16},						//ר������ͼ��
		{253 + 26,	213, 70, 16},						//ר������
		{350,		213, 26, 16},						//bitrateͼ��
		{350 + 26,	213, 70, 16},						//bitrate��С��ʾ
	},

	//volume bar ui
	{
		{0, 0, 0, 0},									//��������������ʾ����
		{0, 0, 0, 0},									//���speakerͼ����ʾ��������״̬
		{0, 0, 0, 0},									//�м�������ʾ������
		{0, 0, 0, 0},    								//�ұ�speakerͼ����ʾ
	},

	//spectrum    Ƶ��
	{
		{210, 40, 400 - 210, 60}
	},
	//icon table
	{
		//ID															res	handle,    	res buffer
		//���������
		{ ID_MUSIC_PALETTE_BMP,           			NULL,  				 NULL },    //
		{ NULL,           	NULL,  				 NULL },    //
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//�ļ�ͼ�꣬δ��ѡ��ʱ
		{ ID_MUSIC_FILE_FOCUSED_BMP,      			NULL,  				 NULL }, 	//�ļ�ͼ�꣬��ѡ��ʱ
		{ ID_MUSIC_LIST_ITEM_FOCUS_BMP,    			NULL,  				 NULL }, 	//ѡ���
		{ ID_MUSIC_LONG_STR_SCROLL_BMP,				NULL,  			 	 NULL },     //���ļ�������������
		//header bar
		//play mode
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 	//	����ȫ��,���ظ�
		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 	//����һ��
		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 	//�漴����
		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 	//�ظ�����
		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 	//�ظ�һ��

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

		//������
		//play mode button
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 		//����ȫ��, unfocused
		{ ID_MUSIC_PLAY_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//����ȫ��, focused
		{ ID_MUSIC_PLAY_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//����ȫ��, pressed

		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 		//����һ��, unfocused
		{ ID_MUSIC_PLAY_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//����һ��, focused
		{ ID_MUSIC_PLAY_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//����һ��, pressed

		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 		//�������, unfocused
		{ ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//�������, focused
		{ ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//�������, pressed

		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 		//�ظ�����ȫ��, unfocused
		{ ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//�ظ�����ȫ��, focused
		{ ID_MUSIC_REPEAT_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//�ظ�����ȫ��, pressed

		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 		//�ظ�����һ��, unfocused
		{ ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//�ظ�����һ��, focused
		{ ID_MUSIC_REPEAT_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//�ظ�����һ��, pressed

		//EQ mode button
		{ ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_EQ_MODE_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_EQ_MODE_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//BL time button
		{ ID_MUSIC_BL_TIME_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_BL_TIME_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_BL_TIME_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//play previous button
		{ ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//������һ��, unfocused
		{ ID_MUSIC_PLAY_PRE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PRE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//pressed
		//play next button
		{ ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//������һ��, unfocused
		{ ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_NEXT_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed
		//play pause button
		{ ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//������ͣ, unfocused
		{ ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed

		//play backward button
		{ ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//����, unfocused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play forward button
		{ ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//���, unfocused
		{ ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play status button,
		{ ID_MUSIC_PLAYING_STATUS_BMP,				NULL,				NULL }, 								//���ڲ���״̬
		{ ID_MUSIC_PAUSE_STATUS_BMP,					NULL,  				NULL }, 								//��ͣ״̬
		{ ID_MUSIC_PREVIOUS_STATUS_BMP,				NULL,  		 		NULL }, 								//��һ��״̬
		{ ID_MUSIC_NEXT_STATUS_BMP,					NULL,  				NULL }, 								//��һ��״̬

		//process bar
		{ ID_MUSIC_PROCESS_BAR_HEAD_BMP,				NULL,  			 	NULL }, 								//process bar ͷ��
		{ ID_MUSIC_PROCESS_BAR_BODY_BMP,				NULL,  				NULL }, 								//body
		{ ID_MUSIC_PROCESS_BAR_TAIL_BMP,				NULL,  		 		NULL }, 								//β��
		{ ID_MUSIC_PROCESS_BAR_POINT_BMP,			NULL,  				NULL }, 								//���ȵ�

		//������Ϣͼ��
		{ ID_MUSIC_PERFORMER_NAME_BMP,				NULL,  			 	NULL }, 								//����ͼ��
		{ ID_MUSIC_FILE_SIZE_BMP,					NULL,  				NULL }, 								//
		{ ID_MUSIC_ALBUM_NAME_BMP,					NULL,  		 		NULL }, 								//
		{ ID_MUSIC_BIT_RATE_BMP,						NULL,  				NULL }, 								//

		//Ƶ��ͼͼ��
		{ ID_MUSIC_PARTICLE_BMP,						NULL,  			 	NULL },
		{ ID_MUSIC_BOUY_BMP,							NULL,  			 	NULL },

		//�������������
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

	//list window, ����ļ�
	{
#if SP_FavoriteFiles
		{0, 0, 188 + 150, 448},						//listbar area�����б������
		{0, 0, 188 + 150, 32},						//item rectangle �������ֵ�����
		{5, 9, 188 + 150, 25},						//focus icon(select border)ѡ������ͼƬ����
		{11, 12, 15, 15},						//file icon�ļ�����ͼ��
		{36, 0, 148 + 150, 38},		//text rectangle��������
#else
		{70, 25, 386 , 544},						//listbar area�����б������
		{70, 25, 386 , 32},						//item rectangle �������ֵ�����
		{5, 9, 316, 22},						//focus icon(select border)ѡ������ͼƬ����
		{11, 12, 16, 16},						//file icon�ļ�����ͼ��
		{36, 0, 148 + 136, 38},		//text rectangle��������
#endif

	},

	//scroll bar ui
	{
#if SP_FavoriteFiles
		{219 + 123, 7,	7, 185 + 255},					//scroll bar background�����б����������λ��
		{221 + 123, 22, 5, 155 + 255},					//scroll�����б������λ��
#else
		{219 + 121 + 105, 7+25,	7, 535},					//scroll bar background�����б����������λ��
		{221 + 121 + 105, 22+25, 5, 505},					//scroll�����б������λ��
#endif
		8,										//head�����б�ͷ��������
		1,										//body�����б��ɫλ��
		8,										//tail�����б�ײ�������
	},

	//head bar ui
	{
		{250 + 200 + 80, 40, 270, 110}, //{210,40,400-210,150-40},					//headbar background������ͷ������
#if SP_APP_CALENDAR
		{299 - 20 + 140, 3, 16 * 8, 24},								//����
		{238, 5, 26, 16},	//{198,5,26,16},				//����ģʽ״̬ͼ��
		{271, 5, 50, 16},								//EQģʽ״̬ͼ��
		{271, 5, 26, 16},								//����ر�ʱ��ͼ��
		{328, 5, 26, 16},								//����ͼ��
		{350, 3, 35, 25},								//����text
		{386, 6, 31, 12},								//���ͼ��
		{390, 3, 85, 25},								//ʱ����ʾ����
#else
		{680 + 160, 3 + 25, 16 * 8, 24},								//����
		{500 + 160, 5 + 25, 26, 16},								//����ģʽ״̬ͼ��
		{550 + 170, 6 + 25, 50, 16},								//EQģʽ״̬ͼ��
		{271 + 200, 5, 26, 16},								//����ر�ʱ��ͼ��
		{348 + 200, 5, 26, 16},								//����ͼ��
		{379 + 200, 3, 18, 16},								//����text
		{411 + 200, 6, 31, 12},								//���ͼ��
		{0, 0, 0, 16},									//ʱ����ʾ����

#endif
	},
	//palyer ui
	{
		{400 + 110, 5 + 25, 100, 40}, //{210,40,400-210,120-40},					//���ư�ť��������()��Ƶ�׹���
		{400 + 110, 20 + 25, 20 * 8, 40}, //{230,120,150,16},						//������ʾ�ַ�����
		{400 + 110, 5 + 25, 26, 16},	//{294,68,26,16},							//����ģʽ��ť
		{450 + 110, 5 + 25, 26, 16},								//EQģʽ��ť
		{450 + 110, 5 + 25, 26, 16},								// ����ʱ����ư�ť

		{268 + 200 + 130, 80 + 70, 26, 16},							//��һ�װ�ť
		{301 + 200 + 130, 80 + 70, 26, 16},							//��һ�װ�ť
		{334 + 200 + 130, 80 + 70, 26, 16},							//������ͣ��ť
		{367 + 200 + 130, 80 + 70, 26, 16},							//���˰�ť
		{400 + 200 + 130, 80 + 70, 26, 16},							//�����ť

		{259 + 165 + 100, 179 + 70 + 60 + 1, 26, 20},						//����״̬ͼ��(��ͣ�򲥷�)
		{237 + 180 + 100, 154 + 70 + 60, 85, 20},	              		//��ǰ����ʱ��
		{385 + 299 + 200, 154 + 70 + 60, 85, 20},						//��ʱ����ʾ����

		{284 + 160 + 100, 185 + 70 + 60, 5, 5},								//������head
		{284 + 160 + 100, 185 + 70 + 60, 276 + 100, 5},							//������body  
		{289 + 160 + 100, 185 + 70 + 60, 5, 5},							//������point,��head֮����˼�5
		{425 + 160 + 100, 185 + 70 + 60, 5, 5},						//������tail

		{253 + 200 + 50 + 30 - 20,		183 + 70 + 80, 26, 16},						//�ݳ���ͼ��
		{253 + 26 + 200 + 50 + 30 -20,	183 + 70 + 80, 240, 16},						//�ݳ�������
		{350 + 200 + 50 + 220 -30,		183 + 70 + 80, 26, 16},						//�ļ���Сͼ��
		{350 + 26 + 200 + 50 + 220 -30,	183 + 70 + 80, 80, 16},						//�ļ���С��ʾ

		{253 + 200 + 50 + 30 -20,		213 + 70 + 80, 26, 16},						//ר������ͼ��
		{253 + 26 + 200 + 50 + 30 -20,	213 + 70 + 80, 240, 16},						//ר������
		{350 + 200 + 50 + 220 -30,		213 + 70 + 80, 26, 16},						//bitrateͼ��
		{350 + 26 + 200 + 50 + 220 -30,	213 + 70 + 80, 80, 16},						//bitrate��С��ʾ
	},

	//volume bar ui
	{
		{0, 0, 0, 0},									//��������������ʾ����
		{0, 0, 0, 0},									//���speakerͼ����ʾ��������״̬
		{0, 0, 0, 0},									//�м�������ʾ������
		{0, 0, 0, 0},    								//�ұ�speakerͼ����ʾ
	},

	//spectrum    Ƶ��
	{
		{0 , 0,  0,  0}
	},
	//icon table
	{
		//ID															res	handle,    	res buffer
		//���������
		{ ID_MUSIC_PALETTE_BMP,           			NULL,  				 NULL },    //
		{ NULL,           	NULL,  				 NULL },    //
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//
		{ ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//�ļ�ͼ�꣬δ��ѡ��ʱ
		{ ID_MUSIC_FILE_FOCUSED_BMP,      			NULL,  				 NULL }, 	//�ļ�ͼ�꣬��ѡ��ʱ
		{ ID_MUSIC_LIST_ITEM_FOCUS_BMP,    			NULL,  				 NULL }, 	//ѡ���
		{ ID_MUSIC_LONG_STR_SCROLL_BMP,				NULL,  			 	 NULL },     //���ļ�������������
		//header bar
		//play mode
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 	//	����ȫ��,���ظ�
		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 	//����һ��
		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 	//�漴����
		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 	//�ظ�����
		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 	//�ظ�һ��

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

		//������
		//play mode button
		{ ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 		//����ȫ��, unfocused
		{ ID_MUSIC_PLAY_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//����ȫ��, focused
		{ ID_MUSIC_PLAY_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//����ȫ��, pressed

		{ ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 		//����һ��, unfocused
		{ ID_MUSIC_PLAY_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//����һ��, focused
		{ ID_MUSIC_PLAY_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//����һ��, pressed

		{ ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 		//�������, unfocused
		{ ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//�������, focused
		{ ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//�������, pressed

		{ ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 		//�ظ�����ȫ��, unfocused
		{ ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//�ظ�����ȫ��, focused
		{ ID_MUSIC_REPEAT_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//�ظ�����ȫ��, pressed

		{ ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 		//�ظ�����һ��, unfocused
		{ ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//�ظ�����һ��, focused
		{ ID_MUSIC_REPEAT_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//�ظ�����һ��, pressed

		//EQ mode button
		{ ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_EQ_MODE_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_EQ_MODE_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//BL time button
		{ ID_MUSIC_BL_TIME_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
		{ ID_MUSIC_BL_TIME_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
		{ ID_MUSIC_BL_TIME_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

		//play previous button
		{ ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//������һ��, unfocused
		{ ID_MUSIC_PLAY_PRE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PRE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//pressed
		//play next button
		{ ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//������һ��, unfocused
		{ ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_NEXT_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed
		//play pause button
		{ ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//������ͣ, unfocused
		{ ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed

		//play backward button
		{ ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//����, unfocused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play forward button
		{ ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//���, unfocused
		{ ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
		{ ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

		//play status button,
		{ ID_MUSIC_PLAYING_STATUS_BMP,				NULL,				NULL }, 								//���ڲ���״̬
		{ ID_MUSIC_PAUSE_STATUS_BMP,					NULL,  				NULL }, 								//��ͣ״̬
		{ ID_MUSIC_PREVIOUS_STATUS_BMP,				NULL,  		 		NULL }, 								//��һ��״̬
		{ ID_MUSIC_NEXT_STATUS_BMP,					NULL,  				NULL }, 								//��һ��״̬

		//process bar
		{ ID_MUSIC_PROCESS_BAR_HEAD_BMP,				NULL,  			 	NULL }, 								//process bar ͷ��
		{ ID_MUSIC_PROCESS_BAR_BODY_BMP,				NULL,  				NULL }, 								//body
		{ ID_MUSIC_PROCESS_BAR_TAIL_BMP,				NULL,  		 		NULL }, 								//β��
		{ ID_MUSIC_PROCESS_BAR_POINT_BMP,			NULL,  				NULL }, 								//���ȵ�

		//������Ϣͼ��
		{ ID_MUSIC_PERFORMER_NAME_BMP,				NULL,  			 	NULL }, 								//����ͼ��
		{ ID_MUSIC_FILE_SIZE_BMP,					NULL,  				NULL }, 								//
		{ ID_MUSIC_ALBUM_NAME_BMP,					NULL,  		 		NULL }, 								//
		{ ID_MUSIC_BIT_RATE_BMP,						NULL,  				NULL }, 								//

		//Ƶ��ͼͼ��
		{ ID_MUSIC_PARTICLE_BMP,						NULL,  			 	NULL },
		{ ID_MUSIC_BOUY_BMP,							NULL,  			 	NULL },

		//�������������
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


//�������explorer AP��Ƶ�UI����
music_ui_t *music_get_ui_param(void)
{
	return &(music_ui_800_480);
}

//������������UI����
music_list_ui_t *music_get_list_ui_param(void)
{
	return &(music_ui_800_480.list_ctrl);
}

//��ö���HeadbarUI����
music_headbar_ui_t *music_get_headbar_ui_param(void)
{
	return 	&(music_ui_800_480.headbar);
}

//��ö���playerUI����
music_palyer_ui_t *music_get_player_ui_param(void)
{
	return 	&(music_ui_800_480.player);
}

//���volume barUI����
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

//���Explorer ����layer size
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

//����listbarÿҳ������ʾ����Ŀ����
__u32 music_count_listbar_pagenum(void)
{
	__u32 pagenum = 0;
	music_ui_t *ui_param;
	ui_param = music_get_ui_param();
	pagenum = ui_param->list_ctrl.listbar_rect.height / ui_param->list_ctrl.item_rect.height;
	return pagenum;
}













