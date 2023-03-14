/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           ir key driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : DEV_KEY.c
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/

#include "drv_fm_i.h"
//#include "rda5820.h"
//#include "tea5767.h"
//#include "rda5807p.h"
//#include "RDA_FM5807M_drv.h"

extern    __krnl_event_t  *fm_lock;
extern    __fm_drv_t       fm_drv;
ES_FILE   *fiic;
__u32			g_flagFmIicFail = 0;

/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void FM_OpLock(void)
{
	__u8  err;
	esKRNL_SemPend(fm_lock, 0, &err);
}
/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void FM_OpUnlock(void)
{
	esKRNL_SemPost(fm_lock);
}
/*
****************************************************************************************************
*
*             DEV_FM_Open
*
*  Description:
*       DEV_FM_Open
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__hdle DEV_FM_Open(void *open_arg, __u32 mode)
{
#if (!SP_SimIIC)

	if(!fm_drv.fm_dev.iic_file)
	{
		return (__hdle)0;
	}
	else
	{
		fiic = fm_drv.fm_dev.iic_file;
#endif
		//eLIBs_fioctrl(fm_drv.fm_dev.power_file, DRV_POWER_CMD_VSTS_SET, POWER_VOL_LDO5, (void *)POWER_VOL_ON);
		//esKRNL_TimeDly(5);
		//fm_mute(DRV_FM_VOICE_OFF);
		return (__hdle) & (fm_drv.fm_dev);
#if (!SP_SimIIC)
	}

#endif
}
/*
****************************************************************************************************
*
*             DEV_FM_Close
*
*  Description:
*       DEV_FM_Close
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DEV_FM_Close(__hdle hfm)
{
	//eLIBs_fioctrl(fm_drv.fm_dev.power_file, DRV_POWER_CMD_VSTS_SET, POWER_VOL_LDO5, (void *)POWER_VOL_OFF);
	//fm_mute(DRV_FM_VOICE_OFF);
	fiic = NULL;
	return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DEV_FM_Read
*
*  Description:
*       DEV_FM_Read
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DEV_FM_Read(void *pdata, __u32 size, __u32 n, __hdle hPower)
{
	return 0;
}
/*
****************************************************************************************************
*
*             DEV_FM_Write
*
*  Description:
*       DEV_FM_Write
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DEV_FM_Write(const void *pdata, __u32 size, __u32 n, __hdle hPower)
{
	return size * n;
}
/*
****************************************************************************************************
*
*             DEV_FM_Ioctrl
*
*  Description:
*       DEV_FM_Ioctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DEV_FM_Ioctrl(__hdle hPower, __u32 cmd, __s32 aux, void *pbuffer)
{
	__s32                ret;
	ret = EPDK_FAIL;
#if (!SP_SimIIC)

	if(!fiic)
	{
		return ret;
	}

#endif
	FM_OpLock();

	switch(cmd)
	{
		case DRV_FM_CMD_INIT:
		{
			ES_FILE *paudio;
			__u16	fm_id = 0;
			__s32   ret = 0;

			g_flagFmIicFail = 0;
			for(fm_drv.fm_type = __FM_TYPE_START + 1; fm_drv.fm_type < __FM_TYPE_TOTAL; fm_drv.fm_type++)
			{
				switch(fm_drv.fm_type)
				{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

					case __FM_TYPE__RDA5807:
						ret = fm_init_rda5807();
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

					case __FM_TYPE__RTC6207:
						ret = fm_init_rtc6207();
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

					case __FM_TYPE__QN8065:
						ret = qn8065_online();
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
					
					case __FM_TYPE__RTC6222:
						ret = rtc6222_online();
						break;
#endif


					default:
						__wrn("\nfm type invalid\n");
						break;
				}

				if(ret)
				{
					break;
				}
			}

			eLIBs_printf("fm_type=%d, fm_id=0x%X\n", fm_drv.fm_type, ret);

			if(fm_drv.fm_type >= __FM_TYPE_TOTAL)
			{
				__wrn("Not support FM type!\n");
				ret = EPDK_FAIL;
				break;
			}

			fm_drv.fm_dev.status = FM_DRV_STATUS_RECEIVE;
			paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");

			if(!paudio)
			{
				__wrn("Open Audio Dev File Failed\n");
				ret = EPDK_FAIL;
				break;
			}

			eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_START, 0, 0);
			eLIBs_fclose(paudio);
			ret = fm_drv.fm_dev.status;
			__msg("DRV_FM_CMD_INIT:end\n");
			break;
		}

		case DRV_FM_CMD_EXIT:
		{
			ES_FILE  *paudio;
			paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");

			if(paudio)
			{
				eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_STOP, 0, 0);
				eLIBs_fclose(paudio);
			}

			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					fm_exit_rda5807();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					fm_exit_rtc6207();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					qn8065_off();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					rtc6222_off();
					break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_EXIT:end\n");
			ret = EPDK_OK;
			break;
		}

		case DRV_FM_CMD_RECVE_INIT:
		{
			break;
		}

		case DRV_FM_CMD_RECVE_EXIT:
		{
			break;
		}

		case DRV_FM_CMD_RECVE_PLAY:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					fm_play_rda5807((__s32)pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					fm_play_rtc6207((__s32)pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					qn8065_tune2ch(((__s32)pbuffer) / 10);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					fm_play_rtc6222((__s32)pbuffer);
					break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_RECVE_PLAY:end\n");
			ret = EPDK_OK;
			break;
		}

		case DRV_FM_CMD_RECVE_SEARCH:
		{
			if((aux & 0xff) == DRV_FM_AUTO_SEARCH)
			{
				switch(fm_drv.fm_type)
				{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

					case __FM_TYPE__RDA5807:
						ret = fm_auto_search_rda5807((__s32)pbuffer, aux & 0xff00);
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

					case __FM_TYPE__RTC6207:
						ret = fm_auto_search_rtc6207((__s32)pbuffer, aux & 0xff00);
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

					case __FM_TYPE__QN8065:
						ret = fm_auto_search_qn8065((__s32)pbuffer, aux & 0xff00);
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
					
					case __FM_TYPE__RTC6222:
						ret = fm_auto_search_rtc6222((__s32)pbuffer, aux & 0xff00);
						break;
#endif


					default:
						break;
				}
			}
			else if((aux & 0xff) == DRV_FM_MANUAL_SEARCH)
			{
				switch(fm_drv.fm_type)
				{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

					case __FM_TYPE__RDA5807:
						ret = fm_manual_search_rda5807((__s32)pbuffer, aux & 0xff00);
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

					case __FM_TYPE__RTC6207:
						ret = fm_manual_search_rtc6207((__s32)pbuffer, aux & 0xff00);
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

					case __FM_TYPE__QN8065:
						ret = fm_auto_search_qn8065((__s32)pbuffer, aux & 0xff00);
						break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
					
					case __FM_TYPE__RTC6222:
						ret = fm_auto_search_rtc6222((__s32)pbuffer, aux & 0xff00);
						break;
#endif


					default:
						break;
				}
			}

			__wrn(" ret = %d,  aux & 0xff00 = 0x%x\n", ret, aux & 0xff00);
			__msg("DRV_FM_CMD_RECVE_SEARCH:end\n");
			break;
		}

		case DRV_FM_CMD_AREA_SET:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_area_choose_rda5807(aux, pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_area_choose_rtc6207(aux, pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = fm_area_choose_qn8065(aux, pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					ret = fm_area_choose_rtc6222(aux, pbuffer);
					break;
#endif


				default:
					ret = fm_area_choose_rda5807(aux, pbuffer);
					break;
			}

			__msg("DRV_FM_CMD_AREA_SET:end\n");
			break;
		}

		case DRV_FM_CMD_STEREO_SET:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_stereo_choose_rda5807(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_stereo_choose_rtc6207(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = EPDK_OK;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
					case __FM_TYPE__RTC6222:
						ret = EPDK_OK;
						break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_STEREO_SET:end\n");
			break;
		}

		case DRV_FM_CMD_MUTE:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_mute_rda5807(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_mute_rtc6207(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = fm_mute_qn8065(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
					case __FM_TYPE__RTC6222:
						ret = fm_mute_rtc6222(aux);
						break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_MUTE:end\n");
			break;
		}

		case DRV_FM_CMD_SLEVEL_SET:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_signal_level_rda5807(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_signal_level_rtc6207(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = EPDK_OK;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					ret = EPDK_OK;
					break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_SLEVEL_SET:end\n");
			break;
		}

		case DRV_FM_CMD_SEND_INIT:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_send_on_rda5807();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_send_on_rtc6207();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = EPDK_FAIL;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					ret = EPDK_FAIL;
					break;
#endif


				default:
					break;
			}

			if(ret == EPDK_OK)
			{
				fm_drv.fm_dev.status = FM_DRV_STATUS_SEND;
			}

			__msg("DRV_FM_CMD_SEND_INIT:end\n");
			break;
		}

		case DRV_FM_CMD_SEND_EXIT:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_send_off_rda5807();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_send_off_rtc6207();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = EPDK_FAIL;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					ret = EPDK_FAIL;
					break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_SEND_EXIT:end\n");
			break;
		}

		case DRV_FM_CMD_SEND_START:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_send_rda5807((__s32)pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_send_rtc6207((__s32)pbuffer);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = EPDK_FAIL;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)

				case __FM_TYPE__RTC6222:
					ret = EPDK_FAIL;
					break;
#endif

				default:
					break;
			}

			__msg("DRV_FM_CMD_SEND_START:end\n");
			break;
		}

		case DRV_FM_CMD_SEND_PAGAIN_SET:
		{
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					ret = fm_pa_gain_rda5807(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					ret = fm_pa_gain_rtc6207(aux);
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					ret = EPDK_FAIL;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
				
				case __FM_TYPE__RTC6222:
					ret = EPDK_FAIL;
					break;
#endif


				default:
					break;
			}

			__msg("DRV_FM_CMD_SEND_PAGAIN_SET:end\n");
			break;
		}

		case DRV_FM_CMD_QUERY_STATUS:
		{
			//ret = fm_drv.fm_dev.status;
			switch(fm_drv.fm_type)
			{
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

				case __FM_TYPE__RDA5807:
					fm_drv.fm_dev.status = fm_get_status_rda5807();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

				case __FM_TYPE__RTC6207:
					fm_drv.fm_dev.status = fm_get_status_rtc6207();
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)

				case __FM_TYPE__QN8065:
					fm_drv.fm_dev.status = FM_DRV_STATUS_RECEIVE;
					break;
#endif
#if (FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)

				case __FM_TYPE__RTC6222:
					fm_drv.fm_dev.status = FM_DRV_STATUS_RECEIVE;
					break;
#endif

				default:
					break;
			}

			ret = fm_drv.fm_dev.status;
			__msg("DRV_FM_CMD_QUERY_STATUS:end\n");
			break;
		}

		/*case DRV_FM_CMD_SLEVEL_GET:
			{
				if (fm_drv.fm_type == __FM_TYPE__RTC6207)
					ret = RTC6218_get_rssi();

				break;
			}*/
		default:
		{
			__wrn("Unkonwn Command...\n");
			break;
		}
	}

	FM_OpUnlock();
	return ret;
}

__dev_devop_t fm_dev_ops =
{
	DEV_FM_Open,
	DEV_FM_Close,
	DEV_FM_Read,
	DEV_FM_Write,
	DEV_FM_Ioctrl
};
