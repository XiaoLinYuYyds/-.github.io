#include "epdk.h"
#include "RTC6207M_drv.h"
#include  "drv_fm_i.h"
#include "iic_read_write.h"

#if(FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6207M)

#define FM_ID__RTC6207											0x1688

#define FM_TUNER_GRID         10

const u8 RTC62XX_turnoff[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x16, 0xAA
};

const u8 RTC62XX_turnon[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x96, 0xAA
};


const u8  RTC62XX_power_up[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x05, 0xB4, 0x00, 0x00, 0x00,
	0x38, 0x48	// 值越大时，台越多
};  // last byte is spike threshold, the larger the more, recommand under 0x50, default 0x10

const u8  RTC62XX_Initial_value[] =
{
	0xC0, 0x0f, 0x14, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x80, 0x05, 0xB4, 0x00, 0xe0, 0x00
};	// RSSI_TH = 0x0c


const u8  RTC62XX_Initial_value1[] =
{
	0x00, 0x72, 0x00, 0xFF, 0x00, 0x1F, 0x03, 0xFF, 0x56, 0xD1, 0x13, 0xB7, 0x00, 0x00
};

const u8  RTC62XX_CKHopping[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x20, 0x00,
	0x05, 0x0F, 0x0E, 0x85, 0x5A, 0xA6, 0xDC, 0x53, 0x80, 0x00, 0x00, 0xA3, 0x00, 0xA3,
	0xD0, 0x08, 0x7F, 0x80, 0x3C, 0x08, 0xA6, 0xCF, 0x81, 0x00, 0x00, 0x00, 0x01, 0x40,
	0x47, 0x00, 0x00, 0x00
};

static __bool RTC6207M_read(u8 *dat, u8 n)
{
	return OperationRDAFM_2w(READ, IIC_CHIP__RTC6207_R, dat, n);
}

static __bool RTC6207M_write(u8 *dat, u8 n)
{
	return OperationRDAFM_2w(WRITE, IIC_CHIP__RTC6207_W, dat, n);
}


#define  FM_SEARCH_CHN_MIN_FREQ         87000
#define  FM_SEARCH_CHN_MAX_FREQ        108000

#define  FM_SEARCH_JAP_MIN_FREQ         76000
#define  FM_SEARCH_JAP_MAX_FREQ         91000
static __u16 max_freq = 0;
static __u16 min_freq = 0;
/**********************************************
* function:      fm_area_choose
*
* description:   地区选择，区别不同的起始和终止
*                频率
*
* notes:         输入正确地区返回成功，否则失败
*
**********************************************/
__s32 fm_area_choose_rtc6207(__s32 area, void *pbuffer)
{
	__drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

	if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
	{
		//tea5767_info.area_select = 1;
		max_freq = FM_SEARCH_JAP_MAX_FREQ / 100;
		min_freq = FM_SEARCH_JAP_MIN_FREQ / 100;
		freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
		freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
	}
	else if(area == DRV_FM_AREA_CHN_US_EUP)
	{
		// tea5767_info.area_select = 0;
		max_freq = FM_SEARCH_CHN_MAX_FREQ / 100;
		min_freq = FM_SEARCH_CHN_MIN_FREQ / 100;
		freq_range->fm_area_min_freq = FM_SEARCH_CHN_MIN_FREQ;
		freq_range->fm_area_max_freq = FM_SEARCH_CHN_MAX_FREQ;
	}
	else
	{
		return EPDK_FAIL;
	}

	return EPDK_OK;
}

/**********************************************
* function:      fm_stereo_choose
*
* description:   音质选择，立体声和普通声音
*
* notes:
*
**********************************************/
__s32 fm_stereo_choose_rtc6207(__s32 audio_method)
{
	return EPDK_OK;
}
/**********************************************
* function:      fm_mute
*
* description:   静音功能
*
* notes:
*
**********************************************/
__s32 fm_mute_rtc6207(__s32 voice_onoff)
{
	u8  RTC62XX_set_property[] = {0xC0, 0x0f};
	u8	RTC62XX_reg_data[18];
	u8	volume_temp = 0;
	RTC6207M_read(&(RTC62XX_reg_data[0]), 18);
	volume_temp = RTC62XX_reg_data[17] & 0x0f;
	RTC62XX_set_property[1] = (RTC62XX_set_property[1] & 0xf0) | volume_temp;

	if(voice_onoff == DRV_FM_VOICE_OFF)
	{
		RTC62XX_set_property[0] = 0x00;
		RTC6207M_write(&(RTC62XX_set_property[0]), 2);
	}
	else if(voice_onoff == DRV_FM_VOICE_ON)
	{
		RTC62XX_set_property[0] = 0xC0;
		RTC6207M_write(&(RTC62XX_set_property[0]), 2);
	}

	return EPDK_OK;
}



/**
 * @brief Station judge for auto search
 * @In auto search mode,uses this function to judge the frequency if has a station
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param int16 freq:frequency value
 * @return bool: if return true,the frequency has a true station;otherwise doesn't have a station
 * @retval
 */
static __bool RTC6207M_FM_SW_Seek(u32 channel_freq, u8 channel_space)
{
	u16  freq_reg_data, loop_counter;
	u8  RTC6207M_reg_data[24];
	u8  RTC6207M_channel_start_tune[] = {0xC0, 0x0f, 0x94, 0xCA};	//107.7MHz
	u8  RTC6207M_channel_stop_tune[] = {0xC0, 0x0f, 0x14, 0xCA};
	u8  RSSIValue = 0;

	if(channel_freq < 87000)
	{
		return 0;
	}

	__wrn("##########channel_freq=%d\n", channel_freq);
	freq_reg_data = (channel_freq - 76000) / channel_space;
	RTC6207M_channel_start_tune[3] = freq_reg_data & 0xff;
	RTC6207M_channel_start_tune[2] = (RTC6207M_channel_start_tune[2] & 0xfc) | (freq_reg_data >> 8);
	RTC6207M_channel_stop_tune[3] = RTC6207M_channel_start_tune[3] ;
	RTC6207M_channel_start_tune[2] &= 0x7f;
	RTC6207M_write(&(RTC6207M_channel_start_tune[0]), 4);
	RTC6207M_channel_start_tune[2] |= 0x80;
	RTC6207M_write(&(RTC6207M_channel_start_tune[0]), 4);

	//wait STC=1
	esKRNL_TimeDly(5);
	do
	{
		RTC6207M_read(&(RTC6207M_reg_data[0]), 2);
		loop_counter++;
	}
	while(((RTC6207M_reg_data[0] & 0x40) == 0) && (loop_counter < 0xff));		//for loop_counter, when tune, the loop time must > 60ms

	if(loop_counter == 0xff)
	{
		return 0;
	}

	loop_counter = 0;
	//clear tune bit
	RTC6207M_write(&(RTC6207M_channel_stop_tune[0]), 4);

	//wait STC=0
	esKRNL_TimeDly(1);
	do
	{
		RTC6207M_read(&(RTC6207M_reg_data[0]), 2);
		loop_counter++;
	}
	while(((RTC6207M_reg_data[0] & 0x40) != 0) && (loop_counter < 0xff));

	RTC6207M_write((u8 *) & (RTC62XX_Initial_value[0]), 14);
	RTC6207M_read(&(RTC6207M_reg_data[0]), 24);
	RTC6207M_write((u8 *)&RTC62XX_Initial_value1[0], 14);
	// delay_n10ms(1);

	if((RTC6207M_reg_data[22] & 0x80) != 0)
	{
		return 0;
	}

	RTC6207M_read(&(RTC6207M_reg_data[0]), 4);
	RSSIValue = RTC6207M_reg_data[3];
	return 1;
	/*
	    if(RSSIValue > RTC62XX_Initial_value[7])
	        return 1;
	    else
	        return 0;*/
}

/**********************************************
* function:      fm_play
*
* description:   根据输入的频率，播放此频率的
*                电台节目，即使没有节目只有
*                噪音，照样播出
*
* notes:         只返回成功
*
**********************************************/
__s32 fm_play_rtc6207(__s32 curFreq)
{
	RTC6207M_FM_SW_Seek(curFreq, 100);
	return EPDK_OK;
}


static __bool RTC6207M_ValidStop(__u16 freq)
{
	return RTC6207M_FM_SW_Seek(freq * 100, 100);
}

/**********************************************
* function:      fm_manual_search
*
* description:   手动搜索，使用fm_play
*                返回值是下一个搜索频率点
*
* notes:
*
**********************************************/
/*
__s32 fm_manual_search(__s32 freq, __u32 search_dir)
{
	__u16 temp = (__u16)(freq / 100);
	__wrn("freq = %d\n", temp);
    return fm_play(temp);
}
*/
__s32 fm_manual_search_rtc6207(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 100);
	__wrn(" freq = %d,  returnValue = %d  , temp = %d\n", freq, returnValue, temp);

	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ / 100) || (temp < FM_SEARCH_CHN_MIN_FREQ / 100))
		{
			__wrn(" temp = %d\n", temp);
			return EPDK_FAIL;
		}

		returnValue = RTC6207M_ValidStop(temp);
		__wrn(" freq = %d,   returnValue = %d\n", temp, returnValue);

		if(returnValue)
		{
			__wrn(" temp = %d\n", temp);
			return temp * 100;
		}
		else
		{
			__wrn("  (temp * 100)|0xff000000 = 0x%x\n", (temp * 100) | 0xff000000);
			return (temp * 100) | 0xff000000;
		}

		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			temp--;
			__wrn(" temp = %d\n", temp);
		}
		else//DRV_FM_SEARCH_UP
		{
			temp++;
			__wrn(" temp = %d\n", temp);
		}
	}

	return EPDK_FAIL;
}
__s32  fm_auto_search_rtc6207(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 100);
	__wrn(" freq = %d,  returnValue = %d,  temp = %d\n", freq, returnValue, temp);

	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ / 100) || (temp < FM_SEARCH_CHN_MIN_FREQ / 100))
		{
			__wrn(" temp = %d\n", temp);
			return EPDK_FAIL;
		}

		returnValue = RTC6207M_ValidStop(temp);
		__wrn(" freq = %d\n,returnValue = %d\n", temp, returnValue);

		if(returnValue)
		{
			__wrn(" temp = %d\n", temp);
			return temp * 100;
		}
		else
		{
			__wrn(" (temp * 100)|0xff000000 = 0x%x\n", (temp * 100) | 0xff000000);
			return (temp * 100) | 0xff000000;
		}

		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			//__wrn(" temp = %d\n", temp);
			temp--;
			__wrn(" temp = %d\n", temp);
		}
		else
		{
			//__wrn(" temp = %d\n", temp);
			temp++;
			__wrn(" temp = %d\n", temp);
		}
	}

	return EPDK_FAIL;
}

/**
 * @brief Set FM volume
 * @It has better use the system volume operation to replace this function
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param __u8 level: volume value
 * @return void
 * @retval
 */
static void RTC6207M_Set_Property_FM_Volume(unsigned char FM_Volumn)
{
	//unsigned char RTC62XX_reg_data[32];
	unsigned char RTC62XX_set_property[] = {0xC0, 0x0F};

	if(FM_Volumn > 15)
	{
		FM_Volumn = 15;
	}

	RTC62XX_set_property[1] = (RTC62XX_set_property[1] & 0xf0) | FM_Volumn;
	RTC6207M_write(RTC62XX_set_property, 2);
}

/**
 * @brief Initialize RDA5807P
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param void
 * @return bool:if true,the operation is successful;otherwise is failed
 * @retval
 */
/*
__s32  fm_init(void)
{

__u8 RDA5807P_REG1[]={0x00,0x02};
__bool error_ind = 0;

  	__inf("RDA5807P_Intialization1\n");
error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_REG1[0], 2);
__inf("RDA5807P_Intialization1\n");
esKRNL_TimeDly(2);
error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_initialization_reg[0], sizeof(RDA5807P_initialization_reg));
__inf("RDA5807P_Intialization3\n");
esKRNL_TimeDly(30);       //Dealy 500 ms
   return EPDK_OK;
}*/

static void RTC6207M_Power_Up(void)
{
	RTC6207M_write((u8 *) & (RTC62XX_turnoff[0]), 30);
	esKRNL_TimeDly(5);
	RTC6207M_write((u8 *) & (RTC62XX_turnon[0]), 30);
	esKRNL_TimeDly(1);
	RTC6207M_write((u8 *) & (RTC62XX_CKHopping[0]), 46);
	esKRNL_TimeDly(1);
	RTC6207M_write((u8 *) & (RTC62XX_power_up[0]), 16);
	esKRNL_TimeDly(10);
}

__s32  fm_init_rtc6207(void)
{
	u16 fm_id;
	u8 RTC62XX_reg_data[14];
	__s32 error_ind;
	__s8 flagAgainCnt;
	RTC6207M_Power_Up();
	error_ind = RTC6207M_read(&(RTC62XX_reg_data[0]), 14);
	fm_id = ((RTC62XX_reg_data[12] << 8) | RTC62XX_reg_data[13]);
	__wrn("fm ID read ~~~~~Now ID is %x-------------\n", fm_id);
	flagAgainCnt = 0;

	do
	{
		switch(fm_id)
		{
			case FM_ID__RTC6207:
				break;

			default:
				flagAgainCnt = -1;
				break;
		}
	}
	while(flagAgainCnt > 0);

	if(error_ind || flagAgainCnt < 0)
	{
		__here__
		return 0;
	}
	else
	{
		__here__
		__msg("\nXXXXXXXXXXXXX  fm_id = 0x%X   XXXXXXXXXXXXXXX\n",	fm_id);
		return fm_id;
	}
}

__s32  fm_exit_rtc6207(void)
{
	RTC6207M_write((u8 *) & (RTC62XX_turnoff[0]), 30);
	return EPDK_OK;
}
__s32  fm_send_on_rtc6207(void)
{
	return EPDK_FAIL;
}

__s32  fm_send_rtc6207(__s32 freq)
{
	return EPDK_FAIL;
}

__s32  fm_send_off_rtc6207(void)
{
	return EPDK_FAIL;
}

__s32  fm_pa_gain_rtc6207(__u8 pagain)
{
	return EPDK_FAIL;
}

__s32  fm_get_status_rtc6207(void)
{
	return FM_DRV_STATUS_RECEIVE;
}
/**********************************************
* function:      fm_signal_level
*
* description:   信号强度选择，要求信号强大越高，收到的电台越少
*                   要求信号强大越高，收到的电台越多，但无效电台也多
*
* notes:
*
**********************************************/
__s32 fm_signal_level_rtc6207(__s32 signal_level)
{
	return EPDK_OK;
}

// 信号强度
__s32 RTC6218_get_rssi(void)
{
	u8 RTC6218_reg_data[4] = {0};
	RTC6207M_read(RTC6218_reg_data, 4);
	return RTC6218_reg_data[3];
}

#endif

#if 0
void f1()
{
	// read Spike and DC
	RTC6218_set_freq(channel_freq);
	OperationRTC6218_write(RTC6218_Initial_value, 14);
	RTC6218_Delay(10);
	OperationRTC6218_read(RTC6218_reg_data, 24);	 // Readout (BK7_5[15])
	Spike_Count_1 = RTC6218_reg_data[20];

	if(RTC6218_reg_data[22] & 0x2 != 0)
	{
		DC_Count_1 = (~(((RTC6218_reg_data[22] & 0x1) << 8) | RTC6218_reg_data[23]) + 1) & 0x1ff; //2's complement
	}
	else
	{
		DC_Count_1 = (((RTC6218_reg_data[22] & 0x1) << 8) | RTC6218_reg_data[23]); //2's complement
	}

	OperationRTC6218_write(RTC6218_Initial_value1, 14);
	RTC6218_Delay(10);
}

u8 RTC6218_get_rssi(void)	//@"RTC6218_RX"
{
	// 信号强度
	u8 RTC6218_reg_data[4] = {0};
	RTC6207M_read(RTC6218_reg_data, 4);
	return RTC6218_reg_data[3];
}
#endif



