#ifndef		__PROGRESSBAR_H
#define		__PROGRESSBAR_H

#include 	"progressbar_i.h"


typedef union
{
	struct
	{
		__s32    hour;
		__s32    minute;
		__s32    second;
	}
	data1;

	__s32 data2[3];
} progressbar_cuckoo_time_t;


typedef struct
{
	//输入参数
	H_WIN hparent;
	__s32 scene_id;


	//内部参数
	H_LYR hlyr;
	H_WIN hfrm;
	
	__s32 cur_time_index;//当画当前时间时亮选的位置（时、分、秒）
	progressbar_cuckoo_time_t cur_time;//输入的设置时间

	__u8 prog_timmer_id;
	__u8 prog_hide_timmer_id;
}
progressbar_scene_t;


typedef struct
{
	H_WIN hparent;
	__s32 scene_id;
} progressbar_create_para_t;



#endif

