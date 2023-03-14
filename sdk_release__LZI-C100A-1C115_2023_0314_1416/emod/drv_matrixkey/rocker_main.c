/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           ir key driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : rocker_main.c
* By      : Jerry
* Version : V1.00
*********************************************************************************************************
*/
#include "drv_matrix_key_i.h"
#include "drv_rocker_i.h"
#include "epdk.h"

__s32 g_getEchoLevel = 0;

#if SP_DEV_DET_PLUGIN_BY_AD
S_GET_STA		g_sGetSta[m_eGetStaIdx_total];
#endif

__s32 data_filter(__s32 *input, __s32  *output , int arr_len)
{
	__s32 tmp, i, j, half, rec, k = 0;
	__s32 *data;
	data = input;
	i = arr_len;

	for(i = 0; i < arr_len - 1; i++)
		for(j = 0; j < arr_len - i - 1; j++)
		{
			if(data[j] > data[j + 1])
			{
				// 从小到大排序
				tmp = data[j];
				data[j] = data[j + 1];
				data[j + 1] = tmp;
			}
		}

	half = arr_len / 2;

	for(i = 0; i < arr_len; i++)
	{
		if(data[half] > data[i])
		{
			rec = data[half] - data[i];
		}
		else
		{
			rec = data[i] - data[half];
		}

		if(rec < 30)	// 相差值
		{
			output[k] = data[i];
			k++;
		}
	}

	return k;
}


#if SP_KB_AD_X1_TYPE
__s32 get_x1_channe1_keypad(__s32 ad_data)
{
	__s16 idx, valueKeyIdx = -1;
	for(idx =0; idx <gc_psKeypadAD_x1__size; idx++)
	{
		if(gc_psKeypadAD_x1[idx].min <= ad_data && ad_data <= gc_psKeypadAD_x1[idx].max)
		{
			valueKeyIdx = idx;
			break;
		}
	}	

	if((0 <= valueKeyIdx) && (valueKeyIdx < gc_adKeyPadX1_tabSize))
	{
		return gc_adKeyPadX1_tab[valueKeyIdx];	// 注: 如果此表中有按键空缺，请将表中相应KPAD值填0x00。
	}

	return 0;
}
#endif

#if SP_KB_AD_X2_TYPE
__s32 get_x2_channe1_keypad(__s32 ad_data)
{
	__s16 idx, valueKeyIdx = -1;
	for(idx =0; idx <gc_psKeypadAD_x2__size; idx++)
	{
		if(gc_psKeypadAD_x2[idx].min <= ad_data && ad_data <= gc_psKeypadAD_x2[idx].max)
		{
			valueKeyIdx = idx;
			break;
		}
	}	

	if((0 <= valueKeyIdx) && (valueKeyIdx < gc_adKeyPadX2_tabSize))
	{
		return gc_adKeyPadX2_tab[valueKeyIdx];	// 注: 如果此表中有按键空缺，请将表中相应KPAD值填0x00。
	}

	return 0;
}
#endif

#define Y1_AD_LEVEL_OFT			__ADC_sim(50)
__s32 get_y1_channe1_keypad(__s32 ad_data)
{
	if(((__ADC_sim(595) -Y1_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (__ADC_sim(595) +Y1_AD_LEVEL_OFT)))
	{
		return 0;
	}
	else if(((__ADC_sim(926) -Y1_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (__ADC_sim(926) +Y1_AD_LEVEL_OFT)))
	{
		return 1;
	}
	else if(((__ADC_sim(407) -Y1_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (__ADC_sim(407) +Y1_AD_LEVEL_OFT)))
	{
		return 2;
	}

	return -1;
}

__s32 get_y2_channe1_keypad(__s32 ad_data)
{
	if((0 <= ad_data) && (ad_data <= (Y2_AD_LEVEL(1) - Y2_AD_LEVEL_OFT)))
	{
		return 0;
	}
	else if(((Y2_AD_LEVEL(1) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (Y2_AD_LEVEL(2) - Y2_AD_LEVEL_OFT)))
	{
		return 1;
	}
	else if(((Y2_AD_LEVEL(2) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (Y2_AD_LEVEL(3) - Y2_AD_LEVEL_OFT)))
	{
		return 2;
	}
	else if(((Y2_AD_LEVEL(3) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (Y2_AD_LEVEL(4) - Y2_AD_LEVEL_OFT)))
	{
		return 3;
	}
	else if(((Y2_AD_LEVEL(4) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (Y2_AD_LEVEL(5) - Y2_AD_LEVEL_OFT)))
	{
		return 4;
	}
	else if(((Y2_AD_LEVEL(5) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (Y2_AD_LEVEL(6) - Y2_AD_LEVEL_OFT)))
	{
		return 5;
	}
	else if(((Y2_AD_LEVEL(6) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (Y2_AD_LEVEL(7) - Y2_AD_LEVEL_OFT)))
	{
		return 6;
	}
	else if(((Y2_AD_LEVEL(7) + Y2_AD_LEVEL_OFT) <= ad_data) && (ad_data <= (__AD_MAX)))
	{
		return 7;
	}

	return -1;
}

#if SP_GetDebugInfo_KeyAdSampling
void GetDebugInfo_KeyAdSampling(PS_ADSampling psADSamping, __s32 ad_data)
{
	psADSamping->m_cur = ad_data;

	if(ad_data < 4000)
	{
		// 有效按键发生时，取最小、大值
		if(psADSamping->m_min < 0)
		{
			psADSamping->m_min = ad_data;
		}
		else if(psADSamping->m_min > ad_data)
		{
			psADSamping->m_min = ad_data;
		}

		if(psADSamping->m_max < 0)
		{
			psADSamping->m_max = ad_data;
		}
		else if(psADSamping->m_max < ad_data)
		{
			psADSamping->m_max = ad_data;
		}
	}
	else
	{
		// 无按键发生时，上次的最小、最大值无效
		psADSamping->m_min = -1;
		psADSamping->m_max = -1;
	}
}
#endif

#if SP_DEV_DET_PLUGIN_BY_AD
static __s8 PlugStaChangeDet(__s8 flagPlugSta, PS_GET_STA psGetSta)
{
	__s8 ret = STA__PlugStaChangeDet_Invalid;
	
	if(psGetSta->unlock == FLAG__PlugStaChangeDetEn)
	{
		if(flagPlugSta == STA__PlugStaChangeDet_Plugin)
		{
			psGetSta->plugoutCnt = 0;
			psGetSta->pluginCnt++;
			if(psGetSta->pluginCnt >= 10)
			{
				if(flagPlugSta != psGetSta->sta)
				{
					psGetSta->sta = STA__PlugStaChangeDet_Plugin;
					ret = STA__PlugStaChangeDet_Plugin;
				}
			}
		}
		else
		{
			psGetSta->pluginCnt = 0;
			psGetSta->plugoutCnt++;
			if(psGetSta->plugoutCnt >= 10)
			{
				if(flagPlugSta != psGetSta->sta)
				{
					psGetSta->sta = STA__PlugStaChangeDet_Plugout;
					ret = STA__PlugStaChangeDet_Plugout;
				}
			}
		}
	}

	//__msg_jjl("PlugStaChangeDet():%d,%d,%d\n", flagPlugSta, ret, psGetSta->sta);
	return ret;
}

void rocker_scan_tp_y1(__s32 ad_data)
{
	__s32 retKpad, plugSta;
	retKpad = get_y1_channe1_keypad(ad_data);
	//__msg_jjl("Y1:%d, %d\n", ad_data, retKpad);

	// det mic:
	switch(retKpad)
	{
		case 0:
		case 2:
			plugSta = STA__PlugStaChangeDet_Plugin;
			break;
		default:
			plugSta = STA__PlugStaChangeDet_Plugout;
			break;
	}
	
	switch(PlugStaChangeDet(plugSta, &g_sGetSta[m_eGetStaIdx_mic]))
	{
		case STA__PlugStaChangeDet_Plugin:
		{
			esKSRV_SendMsg(KMSG_USR_SYSTEM_MIC_PLUGIN, KMSG_TGT_PROC_LOW);
			break;
		}
		case STA__PlugStaChangeDet_Plugout:
		{
			esKSRV_SendMsg(KMSG_USR_SYSTEM_MIC_PLUGOUT, KMSG_TGT_PROC_LOW);
			break;
		}
		default:	break;
	}

	// det aux:
	switch(retKpad)
	{
		case 1:
		case 2:
			plugSta = STA__PlugStaChangeDet_Plugin;
			break;
		default:
			plugSta = STA__PlugStaChangeDet_Plugout;
			break;
	}
	
	switch(PlugStaChangeDet(plugSta, &g_sGetSta[m_eGetStaIdx_aux]))
	{
		case STA__PlugStaChangeDet_Plugin:
		{
			esKSRV_SendMsg(KMSG_USR_SYSTEM_AUX_PLUGIN, KMSG_TGT_PROC_LOW);
			break;
		}
		case STA__PlugStaChangeDet_Plugout:
		{
			esKSRV_SendMsg(KMSG_USR_SYSTEM_AUX_PLUGOUT, KMSG_TGT_PROC_LOW);
			break;
		}
		default:	break;
	}
}
#endif

static __u32		 s_adChannel = RK_CHANNEL_X1;
__u32  rocker_scan_tp(void)
{
	__s32		   ret, ad_data, vaild_num, sum, num;
	__s32		   getAdBuf[2][8];

	ret = RK_QueryIntPending();

	if(ret & RK_DATA_PENDING)
	{
		eLIBs_memset(getAdBuf[0], 0, sizeof(getAdBuf));
		RK_GetData(getAdBuf[0], 8); // 获取AD值
		RK_ClearIntPending(ret);
		RK_ClearFifo();
		vaild_num = data_filter(getAdBuf[0], getAdBuf[1], 8);
		sum = 0;

		for(num = 0; num < vaild_num; num++)
		{
			sum += getAdBuf[1][num];
		}

		ad_data = sum  / vaild_num;
		
		switch(s_adChannel)
		{
			case RK_CHANNEL_X1 :
			{
		//		__msg_jjl("X1:ad_data = %d\n", ad_data);
#if SP_GetDebugInfo_KeyAdSampling//是否开按键打印
				GetDebugInfo_KeyAdSampling(&g_sAdSampling_x1, ad_data);
#endif
				ret = get_x1_channe1_keypad(ad_data);

				if(ret != 0)
				{
					return ret;
				}
				else
				{
					s_adChannel = RK_CHANNEL_X2;
				}
				
				break;
			}

			case RK_CHANNEL_X2 :
			{
			//	__msg_jjl("X2:ad_data = %d\n", ad_data);
#if SP_GetDebugInfo_KeyAdSampling
				GetDebugInfo_KeyAdSampling(&g_sAdSampling_x2, ad_data);	
#endif
				ret = get_x2_channe1_keypad(ad_data);

				if(ret != 0)
				{
					return ret;
				}
				else
				{
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByKnob)
					s_adChannel = RK_CHANNEL_Y2;	// 需要获取混响等级值
#else
				#if SP_DEV_DET_PLUGIN_BY_AD
					s_adChannel = RK_CHANNEL_Y1;
				#else
					s_adChannel = RK_CHANNEL_X1;
				#endif
#endif
				}
				
				break;
			}

		#if SP_DEV_DET_PLUGIN_BY_AD
			case RK_CHANNEL_Y1:
			{
				//__msg_jjl("Y1:ad_data = %d\n", ad_data);
				rocker_scan_tp_y1(ad_data);
				
				s_adChannel = RK_CHANNEL_X1;
				break;
			}
		#endif

			case RK_CHANNEL_Y2 :
			{
				__s32 level;
				static __s32 s_level = 0, s_level_cnt = 0;
				level = get_y2_channe1_keypad(ad_data);
				//__msg("Y2 : AD_DATA = %d, level = %d\n", ad_data, level);
				//__msg_jjl("Y2 : AD_DATA = %d, level = %d\n", ad_data, level);

				if(level >= 0)
				{
					if(s_level != level)
					{
						s_level = level;
						s_level_cnt = 0;
					}
					else
					{
						s_level_cnt++;
					}
				}

				if(s_level_cnt >= 2)		// 变化的值已经稳定
				{
					if(g_getEchoLevel != s_level)
					{
						g_getEchoLevel = s_level;
					}
				}

				s_adChannel = RK_CHANNEL_X1;
				break;
			}

			default :
				s_adChannel = RK_CHANNEL_X1;
				break;
		}
		
		RK_SelectChannel(s_adChannel);
		RK_ClearIntPending(ret);
		RK_ClearFifo();
	}

	return 0;
}

void InitVar(void)
{
#if SP_DEV_DET_PLUGIN_BY_AD
	E_GET_STA_IDX idx;

	memset(g_sGetSta, 0, sizeof(S_GET_STA));
	for(idx = m_eGetStaIdx_begin; idx < m_eGetStaIdx_total; idx++)
	{
		g_sGetSta[idx].sta = STA__PlugStaChangeDet_Plugout;
	}
#endif
	
#if SP_GetDebugInfo_KeyAdSampling
	#if SP_KB_AD_X1_TYPE
	{
		__s16 idx;
		__msg_jjl("\ngc_psKeypadAD_x1_size=%d\n", gc_psKeypadAD_x1__size);
		for(idx =0; idx <gc_psKeypadAD_x1__size; idx++)
		{
			__msg_jjl("X1[%d]:[%d, %d]\n", idx, gc_psKeypadAD_x1[idx].min, gc_psKeypadAD_x1[idx].max);
		} 
		g_sAdSampling_x1.psKeypadAD = gc_psKeypadAD_x1;
		g_sAdSampling_x1.psKeypadAD_size = gc_psKeypadAD_x1__size;
	}
	#endif
	#if SP_KB_AD_X2_TYPE
	{
		__s16 idx;
		__msg_jjl("\ngc_psKeypadAD_x2_size=%d\n", gc_psKeypadAD_x2__size);
		for(idx =0; idx <gc_psKeypadAD_x2__size; idx++)
		{
			__msg_jjl("X2[%d]:[%d, %d]\n", idx, gc_psKeypadAD_x2[idx].min, gc_psKeypadAD_x2[idx].max);
		} 
		g_sAdSampling_x2.psKeypadAD = gc_psKeypadAD_x2;
		g_sAdSampling_x2.psKeypadAD_size = gc_psKeypadAD_x2__size;
	}
	#endif
#endif
}

__s32 rocker_init(void)
{
	RK_Init();
	RK_SetFifoDepth(RK_FIFO_DEPTH_08);
	RK_SetSampleRate(RK_SAMPLE_RATE_16K);
	RK_EnableInt(RK_DATA_IRQ);
	RK_SelectChannel(s_adChannel);
	RK_ClearIntPending(RK_QueryIntPending());
	RK_ClearFifo();

	InitVar();
	__msg("enter init--------------------------------------------------------------\n");
	return EPDK_OK;
}
/*
****************************************************************************************************
*
*             rocker_exit
*
*  Description:
*       rocker_exit
*
*  Parameters:
*
*  Return value:
*       none
*
****************************************************************************************************
*/
__s32 rocker_exit(void)
{
#if 0
	__u8  err;

	if(rk_private.data_tid)
	{
		esKRNL_TDel(rk_private.data_tid);
		esKRNL_TimeDly(4);
	}

	esKRNL_SemDel(rk_private.data_sem, OS_DEL_ALWAYS, &err);
#endif
	RK_Exit();
	//   esPINS_PinGrpRel(pw_pin, 0);
	return EPDK_OK;
}
