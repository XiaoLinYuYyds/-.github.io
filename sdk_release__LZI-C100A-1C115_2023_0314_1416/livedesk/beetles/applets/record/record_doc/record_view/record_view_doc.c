#include "record_view_doc.h"
#include "record_comm.h"

static __bool	gs_isRecording = 0;
static __s32 	gs_bkVolume = 0;

static void record_set_quality(precord_doc_t precord_doc)
{
	switch(precord_doc->recQuality)
	{
		case QUALITY_VERY_GOOD:
			precord_doc->media_file.audio_inf.uAudioBps    		= 320000;
			precord_doc->media_file.audio_inf.uAudioSampRate 	= 44100;//48000
			break;

		case QUALITY_GOOD:
			precord_doc->media_file.audio_inf.uAudioBps			= 320000;//160000;
			precord_doc->media_file.audio_inf.uAudioSampRate	= 44100;//24000;
			break;

		case QUALITY_NORMAL:
			precord_doc->media_file.audio_inf.uAudioBps			= 320000;//80000;
			precord_doc->media_file.audio_inf.uAudioSampRate	= 44100;//12000;
			break;

		default:
			break;
	}
}

static void record_set_recSource(precord_doc_t precord_doc)
{
	if(precord_doc->recordType == RECORD_MIC)
	{
		precord_doc->media_file.audio_inf.uAudioSource  = GINKGO_AUDIO_SRC_MIC;
		SLIB_strcpy(precord_doc->recordSrc, "REC");
	}
	else
	{
		esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_SET_AUDIO_SOURCE, GINKGO_AUDIO_SRC_FM, 0);
		precord_doc->media_file.audio_inf.uAudioSource  = GINKGO_AUDIO_SRC_FM;
		SLIB_strcpy(precord_doc->recordSrc, "FM");
	}
}

static void record_set_path(precord_doc_t precord_doc)
{
	__date_t	date;
	char 		dataStr[16] = {0}, cntStr[10] = {0};
	__u32		fileNum = 0;
	// eg. F:\RECORD\REC200101010000.mp3
	fileNum = record_file_num(precord_doc->curSearchPath, RECORD_FILTER);
	cntStr[0] = ((fileNum / 1000 % 10) + '0');
	cntStr[1] = ((fileNum / 100 % 10) + '0');
	cntStr[2] = ((fileNum / 10 % 10) + '0');
	cntStr[3] = ((fileNum / 1 % 10) + '0');
	cntStr[4] = '\0';
	SLIB_strcpy(precord_doc->recordName, precord_doc->recordSrc);

	if(eLIBs_GetDate(&date) == EPDK_FAIL)
	{
		SLIB_strcat(precord_doc->recordName, "20010101");
	}
	else
	{
		record_date_to_char(dataStr, date, 0);
		SLIB_strcat(precord_doc->recordName, dataStr);
	}

	SLIB_strcat(precord_doc->recordName, cntStr);

	if(precord_doc->audioType == RECORD_MP3)
	{
		precord_doc->media_file.audio_inf.uAbsFmt      	= GINKGO_ABS_TYPE_MPEG12_LAYER3;
		precord_doc->media_file.audio_inf.uChn         	= GINKGO_AUDIO_CHN_MONO;
		precord_doc->media_file.uFileFmt  = GINKGO_MEDIA_FILE_FMT_MP3;
		SLIB_strcat(precord_doc->recordName, ".mp3");
	}
	/*else if( precord_doc->audioType == RECORD_WMA )
	{
		precord_doc->media_file.audio_inf.uAbsFmt      	= GINKGO_ABS_TYPE_WMA_STANDARD;
		precord_doc->media_file.audio_inf.uChn         	= GINKGO_AUDIO_CHN_MONO;
		precord_doc->media_file.uFileFmt  = GINKGO_MEDIA_FILE_FMT_WMA;
		SLIB_strcat(precord_doc->recordName, ".wma");
	}*/
	else
	{
		precord_doc->media_file.audio_inf.uAbsFmt      	= GINKGO_ABS_TYPE_ADPCM;
		precord_doc->media_file.audio_inf.uChn         	= GINKGO_AUDIO_CHN_MONO;
		precord_doc->media_file.uFileFmt  = GINKGO_MEDIA_FILE_FMT_WAV;
		SLIB_strcat(precord_doc->recordName, ".wav");
	}

	SLIB_strcpy(precord_doc->recordFilePath, precord_doc->curSearchPath);
	SLIB_strcat(precord_doc->recordFilePath, precord_doc->recordName);
	__inf("\n\n\n\n    record file: %s \n\n\n\n", precord_doc->recordFilePath);
	precord_doc->media_file.file_path = precord_doc->recordFilePath;
}

static __s32  record_GetSearchPath(precord_doc_t precord_doc)
{
	__s32 ret = 0;
	char root_type[50];
	char disk_name[RAT_MAX_PARTITION][4];
	eLIBs_memset(root_type, 0, sizeof(root_type));
	eLIBs_memset(disk_name, 0, sizeof(disk_name));
	eLIBs_memset(precord_doc->curSearchPath, 0, sizeof(precord_doc->curSearchPath));
	eLIBs_memset(precord_doc->curSearchDisk, 0, sizeof(precord_doc->curSearchPath));

	switch(precord_doc->curRootType)
	{
		case RAT_LOCAL:
			eLIBs_strcpy(root_type, RAT_LOCAL_DISK);
			break;

		case RAT_TF:
			eLIBs_strcpy(root_type, RAT_SD_CARD);
			break;

		case RAT_USB:
			eLIBs_strcpy(root_type, RAT_USB_DISK);
			break;

		default:
			break;
	}

	ret = rat_get_partition_name(root_type, disk_name);
	eLIBs_strcpy(precord_doc->curSearchPath, disk_name[0]);
	eLIBs_strcpy(precord_doc->curSearchDisk, disk_name[0]);
	eLIBs_strcat(precord_doc->curSearchPath, RECORD_ROOT_PATH);
	__wrn("disk_name[0]=%s, root_type=%s\n", precord_doc->curSearchPath, root_type);
	return ret;
}

__s32 record_init(precord_doc_t precord_doc)
{
	reg_record_para_t	*recPara;
	ES_DIR		*tmp_dir;

	//得到当前路径
	if(record_GetSearchPath(precord_doc) == EPDK_FAIL)
	{
		__wrn("\n\ncan't fined this disk %d!\n\n", precord_doc->curRootType);
		return EPDK_FAIL;
	}

	//获得剩余可录时间
	record_set_quality(precord_doc);
	precord_doc->diskFreeSize	= eLIBs_GetVolFSpace(precord_doc->curSearchDisk);//1024*1024*1024;

	if(precord_doc->diskFreeSize == 0)
	{
		__wrn("\n\ndisk is full!\n\n");
		return EPDK_FAIL;
	}
	else
	{
		__u32 s = 0;
		precord_doc->diskFreeTime = precord_doc->diskFreeSize / (precord_doc->media_file.audio_inf.uAudioBps / 8);
		s = (precord_doc->diskFreeSize >> 20);
		__inf("disk free size:%d MB\n", s);
		__inf("disk free time:%d hour\n", precord_doc->diskFreeTime / 3600);
	}

	//install ginkgo module
	precord_doc->mid_ginkgo = esMODS_MInstall(BEETLES_APP_ROOT"mod\\ginkgo.mod", 0);

	if(!precord_doc->mid_ginkgo)
	{
		__wrn("\n\nInstall cedar module failed!\n\n");
		return EPDK_FAIL;
	}

	//open ginkgo module
	precord_doc->mod_ginkgo = esMODS_MOpen(precord_doc->mid_ginkgo, 0);

	if(!precord_doc->mod_ginkgo)
	{
		__wrn("\n\nOpen cedar module failed!\n\n");
		esMODS_MUninstall(precord_doc->mid_ginkgo);
		precord_doc->mid_ginkgo = 0;
		return EPDK_FAIL;
	}

	//check if F:\\RECORD is exist, if not, create it
	tmp_dir = eLIBs_opendir(precord_doc->curSearchPath);

	if(!tmp_dir)
	{
		if(eLIBs_mkdir(precord_doc->curSearchPath))
		{
			__wrn("\n\nCreate record dir failed!\n\n");
			esMODS_MClose(precord_doc->mod_ginkgo);
			precord_doc->mod_ginkgo = 0;
			esMODS_MUninstall(precord_doc->mid_ginkgo);
			precord_doc->mid_ginkgo = 0;
			return EPDK_FAIL;
		}
	}
	else
	{
		eLIBs_closedir(tmp_dir);
		tmp_dir = 0;
	}

	//set media file info
	precord_doc->media_file.audio_inf.uAbsFmt      	= GINKGO_ABS_TYPE_MPEG12_LAYER3;
	precord_doc->media_file.audio_inf.uChn         	= GINKGO_AUDIO_CHN_MONO;
	precord_doc->media_file.audio_inf.uSampWidth   	= GINKGO_AUDIO_SAMP_WIDTH_16;
	precord_doc->fileSize		= 0;
	eLIBs_memset(precord_doc->spctrmLevel, 0, sizeof(precord_doc->spctrmLevel));
	eLIBs_memset(precord_doc->oldSpctrmLevel, 0, sizeof(precord_doc->spctrmLevel));
	precord_doc->recordCurTimes = 0;
	precord_doc->recState	    = RECORD_STOP;
	gs_isRecording = 0;
	recPara = (reg_record_para_t *)dsk_reg_get_para_by_app(REG_APP_RECORD);
	precord_doc->recQuality		= recPara->reserverd[0];//Quality
	precord_doc->audioType 		= recPara->reserverd[1];//mp3, wav
	return EPDK_OK;
}

__s32 record_unInit(precord_doc_t precord_doc)
{
	reg_record_para_t	*recPara;
	record_stop(precord_doc);

	//close and uninstall cedar module
	if(precord_doc->mod_ginkgo)
	{
		esMODS_MClose(precord_doc->mod_ginkgo);
		precord_doc->mod_ginkgo = 0;
	}

	if(precord_doc->mid_ginkgo)
	{
		esMODS_MUninstall(precord_doc->mid_ginkgo);
		precord_doc->mid_ginkgo = 0;
	}

	recPara = (reg_record_para_t *)dsk_reg_get_para_by_app(REG_APP_RECORD);
	recPara->reserverd[0] = precord_doc->recQuality; //Quality
	recPara->reserverd[1] = precord_doc->audioType; //mp3, wav
	return EPDK_OK;
}

__s32 record_clean(precord_doc_t precord_doc)
{
	precord_doc->recState	    	= RECORD_STOP;
	precord_doc->recordCurTimes 	= 0;
	precord_doc->recordName[0]  	= '\0';
	precord_doc->recordFilePath[0]  = '\0';
	eLIBs_memset(precord_doc->spctrmLevel, 0, sizeof(precord_doc->spctrmLevel));
	eLIBs_memset(precord_doc->oldSpctrmLevel, 0, sizeof(precord_doc->spctrmLevel));
	gs_isRecording = 0;
	return EPDK_OK;
}

__u32 record_get_files(precord_doc_t precord_doc)
{
	return record_file_num(precord_doc->curSearchPath, RECORD_FILTER);
}

#define VOL_RecordThreshold				22
void record_volume_reset(__u8 enter_exit)
{
#if SP_Software_Echo
	if(enter_exit)
	{
		gs_bkVolume = dsk_volume_get();
		if(gs_bkVolume > VOL_RecordThreshold)
		{
			dsk_volume_set(VOL_RecordThreshold);
		}
	}
	else
	{
		if(gs_bkVolume > VOL_RecordThreshold)
		{
			dsk_volume_set(gs_bkVolume);
		}
	}
#endif
}

void record_mic_param_set(__u8 enter_exit, __u8 recordType)
{
	if(enter_exit)
	{
		#if SP_Software_Echo
			if((recordType == RECORD_MIC) && IOCTRL__MIC_DET__IS_PLUGIN())
			{
				// 话筒录音时，有话筒插入，则调整混响参数
				dsk_audio_echo_close();
				dsk_audio_echo_open(AUDIO_REC_USR_MIC, 0);
				dsk_echo_level_set(0);
			}
		#endif
			
		dsk_audio_echo_set_mic_pre_gain(1);
		dsk_audio_echo_set_mic_gain(5);
	}
	else
	{
		#if SP_Software_Echo	
			if((recordType == RECORD_MIC) && IOCTRL__MIC_DET__IS_PLUGIN())
			{
				// 话筒录音停止时，有话筒插入，则恢复混响参数
				dsk_audio_echo_close();
				dsk_audio_echo_open(AUDIO_REC_USR_MIC, 1);
				#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByKnob)
				{
					__u32 level;
					level = dsk_echo_level_get_setting();
	
					if(level == EPDK_FAIL)
					{
						level = 0;
					}
	
					dsk_echo_level_set(level);
				}
				#elif (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
				{
					__s32 level;
					level = dsk_echo_level_get_setting();
	
					if(level == EPDK_FAIL)
					{
						level = 3;
					}
	
					dsk_echo_level_set(level);
				}
				#endif
			}	
	#endif
	}
}

__s32 record_start(precord_doc_t precord_doc)
{
	__s32				ret;
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_RECORD_START);
	precord_doc->diskFreeSize	= eLIBs_GetVolFSpace(precord_doc->curSearchDisk);//1024*1024*1024;

	if(precord_doc->diskFreeSize == 0)
	{
		__wrn("\n\ndisk is full!\n\n");
		precord_doc->diskFreeSize = 1;
		precord_doc->diskFreeTime = 0;
		return EPDK_FAIL;
	}

	if(precord_doc->recordType == RECORD_MIC)
	{
		record_volume_reset(1);	
	}
	// 设置录音文件
	record_set_quality(precord_doc);
	record_set_recSource(precord_doc);
	record_set_path(precord_doc);
	ret = esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_SET_MEDIAFILE, 0, &precord_doc->media_file);

	if(ret < 0)
	{
		__wrn("\n\nInput media file to ginkgo failed!\n\n");
		record_clean(precord_doc);
		return EPDK_FAIL;
	}
	else
	{
		__msg("\n\nInput media file to ginkgo OK!\n\n");
	}

	record_mic_param_set(1, precord_doc->recordType);

	// 启动录制
	ret = esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_START, 0, 0);

	if(ret < 0)
	{
		__wrn("\n\nSend start command to ginkgo failed!!!\n\n");
		record_clean(precord_doc);
		return EPDK_FAIL;
	}
	else
	{
		__msg("\n\nSend start command to ginkgo OK!!!\n\n");
	}

	// 开启频谱
	ret = esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_SET_SPECTRUM, 1, 0);

	if(ret < 0)
	{
		__wrn("\n\nSend SPECTRUM command to ginkgo failed!!!\n\n");
		record_clean(precord_doc);
		return EPDK_FAIL;
	}
	else
	{
		__msg("\n\nSend SPECTRUM command to ginkgo OK!!!\n\n");
	}

	/*get msg channel*/
	precord_doc->msg_queue = (__krnl_event_t *)esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_GET_MESSAGE_CHN, 0, 0);
	precord_doc->fileSize		= 0;
	precord_doc->recState	    = RECORD_START;
	gs_isRecording = 1;
	return EPDK_OK;
}

__s32 record_get_spectrum(precord_doc_t precord_doc)
{
	__s32			ret;
	eLIBs_memcpy(precord_doc->oldSpctrmLevel, precord_doc->spctrmLevel, sizeof(precord_doc->spctrmLevel));
	ret = esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_GET_SPECTRUM, 0, precord_doc->spctrmLevel);

	if(ret < 0)
	{
		__inf("\n\nGet SPECTRUM from ginkgo failed!!!\n\n");
		eLIBs_memset(precord_doc->spctrmLevel, 0, sizeof(precord_doc->spctrmLevel));
		return EPDK_FAIL;
	}

	return ret;
}

__s32 record_pause(precord_doc_t precord_doc)
{
	esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_PAUSE, 0, 0);
	precord_doc->recState	    = RECORD_PAUSE;
	gs_isRecording = 0;
	record_mic_param_set(0, precord_doc->recordType);
	if(precord_doc->recordType == RECORD_MIC)
	{
		record_volume_reset(0);
	}
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_RECORD_PAUSE);
	return EPDK_OK;
}

__s32 record_continue(precord_doc_t precord_doc)
{
	app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_RECORD_START);
	
	if(precord_doc->recordType == RECORD_MIC)
	{
		record_volume_reset(1);	
	}
	record_mic_param_set(1, precord_doc->recordType);

	esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_START, 0, 0);
	precord_doc->recState	    = RECORD_START;
	gs_isRecording = 1;
	return EPDK_OK;
}

__s32 record_stop(precord_doc_t precord_doc)
{
	if(precord_doc->recState != RECORD_STOP)
	{
		esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_STOP, 0, 0);
		app_root_play_app_sounds(SHORT_KEY_WAVE_FILE_RECORD_STOP);

		record_mic_param_set(0, precord_doc->recordType);
	}

	record_clean(precord_doc);
	if(precord_doc->recordType == RECORD_MIC)
	{
		record_volume_reset(0);
	}
	return EPDK_OK;
}

void *record_get_msgqueue(precord_doc_t precord_doc)
{
	__u8 err;
	void *msg;
	msg = (void *)esKRNL_QAccept(precord_doc->msg_queue, &err);
	return msg;
}

void record_updateFreeTime(precord_doc_t precord_doc)
{
	record_set_quality(precord_doc);
	precord_doc->diskFreeTime = precord_doc->diskFreeSize / (precord_doc->media_file.audio_inf.uAudioBps / 8);

	if(precord_doc->diskFreeTime < 60)
	{
		record_stop(precord_doc);
	}
}

__s32 record_updateCurTime(precord_doc_t precord_doc)
{
	void *msg;

	if(precord_doc->recState != RECORD_STOP)
	{
		precord_doc->recordCurTimes = esMODS_MIoctrl(precord_doc->mod_ginkgo, GINKGO_CMD_GET_CUR_TIME, 0, 0);
		precord_doc->fileSize = (precord_doc->recordCurTimes / 1000) * (precord_doc->media_file.audio_inf.uAudioBps / 8000) + (precord_doc->recordCurTimes % 10);
		record_get_spectrum(precord_doc);
		msg = record_get_msgqueue(precord_doc);

		if(msg != NULL)
		{
			if((__s32)(msg) == GINKGO_ERROR_WRITE_FILE_FAIL)
			{
				record_stop(precord_doc);
			}

			__inf("\n\n      %d       \n\n", (__s32)(msg));
		}

		precord_doc->diskFreeTime = precord_doc->diskFreeSize / (precord_doc->media_file.audio_inf.uAudioBps / 8);
		precord_doc->diskFreeTime -= (precord_doc->recordCurTimes / 1000);

		if(precord_doc->diskFreeTime < 60)
		{
			record_stop(precord_doc);
		}
	}

	return precord_doc->recordCurTimes;
}

__bool app_record_is_recording(void)
{
	if(gs_isRecording)
	{
		return 1;
	}

	return 0;
}

