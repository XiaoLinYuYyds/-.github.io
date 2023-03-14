#ifndef __RECORD_VIEW_DOC_H__
#define __RECORD_VIEW_DOC_H__


#include "record_ui.h"

#define RECORD_FILTER					".mp3|.wav|.wma"

typedef enum RECORD_STATE
{
  RECORD_START,
  RECORD_PAUSE,
  RECORD_STOP

} RECORD_STATE;

typedef enum RECORD_AUDIO_TYPE
{
  RECORD_MP3 = 0,
  RECORD_WAV,
  //RECORD_WMA,
  RECORD_AUDIO_TYPE_MAX

} RECORD_AUDIO_TYPE;

typedef enum RECORD_TYPE
{
  RECORD_MIC,
  RECORD_FM
} RECORD_TYPE;

#define RECORD_SPCTRM_LEVEL	32

#define NO_FILES_TIMES		10

typedef struct tag_record_doc
{
	__s32			curRootType;
	char			curSearchPath[50];
	char			curSearchDisk[10];

	__u32			fileSize;				//单位KB
	__u32			diskFreeTime;			//单位秒
	__u64			diskFreeSize;

	RECORD_STATE	recState;				//录音状态
	__u32			recordMaxTimes;			//最大可录音时间
	__u32			recordCurTimes;			//当前录音时间

	char	recordSrc[10];
	char	recordName[25];					//录音文件名
	char	recordFilePath[50];				//录音文件路径

	__u32   				mid_ginkgo;		//模块
	__mp    				*mod_ginkgo;	//模块
	__krnl_event_t 			*msg_queue;		// 返馈消息队列
	__u16					oldSpctrmLevel[RECORD_SPCTRM_LEVEL];//录音频谱
	__u16					spctrmLevel[RECORD_SPCTRM_LEVEL];	//录音频谱
	__ginkgo_media_file_inf	media_file;		// 录制文件信息
	RECORD_AUDIO_TYPE		audioType;		//.mp3 or wav
	RECORD_AUDIO_TYPE		audioTypeTemp;	//临时保存.mp3 or wav
	RECORD_TYPE				recordType;		//mic or FM
	QUALITY_STATE			recQuality;		//录音质量
	QUALITY_STATE			recQualityTemp;	//临时保存录音质量

	__u32			timeCnt;

} record_doc_t, *precord_doc_t;

extern __s32 record_init(precord_doc_t precord_doc);
extern __s32 record_unInit(precord_doc_t precord_doc);
extern __u32 record_get_files(precord_doc_t precord_doc);
extern __s32 record_start(precord_doc_t precord_doc);
extern __s32 record_pause(precord_doc_t precord_doc);
extern __s32 record_continue(precord_doc_t precord_doc);
extern __s32 record_stop(precord_doc_t precord_doc);
extern __s32 record_updateCurTime(precord_doc_t precord_doc);

extern void record_updateFreeTime(precord_doc_t precord_doc);

extern __bool app_record_is_recording(void);
extern void record_mic_param_set(__u8 enter_exit, __u8 recordType);

#endif//__RECORD_VIEW_DOC_H__

