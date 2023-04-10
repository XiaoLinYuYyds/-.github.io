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
} progressbar_cuckoo_time_t;//存放时间数据的共用体


typedef struct
{
	//接收输入参数
	H_WIN hparent;//接收到输入参数结构体的数据framewin窗口句柄
	__s32 scene_id;//接收到进度条的窗口id


	//内部参数
	H_LYR hlyr;//存放进度条framewin窗口图层
	H_WIN hfrm;//
	
	__s32 cur_time_index;//当画当前时间时亮选的位置（时、分、秒）
	progressbar_cuckoo_time_t cur_time;//输入的设置时间

	__u8 prog_timmer_id;		//进度条定时器id
	__u8 prog_hide_timmer_id;
}
progressbar_scene_t;

//作用于manwin窗口上面再创建进度条framewin窗口的输入参数
typedef struct
{
	H_WIN hparent;		/*存放进度条framewin窗口句柄*/
	__s32 scene_id;		/*存放进度条framewin窗口的id*/
} progressbar_create_para_t;//用于进度条framewin窗口创建的输入参数结构体

void *progressbar_frmwin_create(progressbar_create_para_t *create_para);
__s32 progressbar_ui_scene_delete(void *handle);


#endif

