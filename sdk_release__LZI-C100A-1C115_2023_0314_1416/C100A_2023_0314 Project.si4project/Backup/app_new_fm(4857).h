#ifndef		__APP_NEW_FM_H
#define		__APP_NEW_FM_H

#include	"app_new_fm_i.h"


typedef struct  //TODO: 精简某些量
{

	__s32 cur_max_freq; //当前最大频率
	__s32 cur_min_freq; //当前最小频率

	__s32 channel_id;     //当前通道id号
	__u32 cur_freq;       //当前频率
	__u32 channel_count;  //通道总数

	__s32 search_mode;    //搜索模式 : auto or manual

	__s32 is_record;
	//	dsk_radio_band_mode_t band_mode;

	__s32 thumb_show;     // thumb btn,0 -- 不显示, 1 -- 显示为非焦点, 2 -- 显示为焦点
	__s32 audio_mode;

	//数字选取===================================
	__s32  ndigit_fm_num;    //数字选取序号
	__u8   digit_timmer_id;  //数字选取timmer id号
	H_LYR  hdigit_layer;     //显示当前数字的图层
	//=============================================
	__bool m_dispCue;
}
NEW_FM_CTRLDATA_T;

typedef struct app_new_fm_ctrl
{
	H_WIN	 fm_framewin;	//存放fm的framewin窗口
	H_LYR	 fm_lyr;		//存放fm的framewin窗口图层

	NEW_FM_CTRLDATA_T	fm_ctrl;
		
	GUI_FONT *font;		//存放文本
	__s32 root_type;	//存放传入进来的root参数
}new_fm_ctrl;	





#endif
