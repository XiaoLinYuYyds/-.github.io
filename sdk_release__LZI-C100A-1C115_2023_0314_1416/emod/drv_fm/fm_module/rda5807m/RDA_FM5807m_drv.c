#include "epdk.h"
#include "RDA_FM5807m_drv.h"
#include  "drv_fm_i.h"
#include "iic_read_write.h"
#if SP_SimIIC
#include "IIC.h"
#endif

#if(FM_MODULE_TYPE & FM_MODULE_TYPE_RDA5807M)

#define             _SHARE_CRYSTAL_24MHz_   //����24MHz������򿪴˺�
//#define             _SHARE_CRYSTAL_12MHz_   //����12MHz������򿪴˺�
//#define             _SHARE_CRYSTAL_32KHz_   //����32KHz������򿪴˺�
//#define             _FM_STEP_50K_           //50K��������򿪴˺�


#define FM_ID__RDA5807PH											0x5801
#define FM_ID__RDA5807P 											0x5802
#define FM_ID__RDA5807P_											0x5803
#define FM_ID__RDA5807PE											0x5804
#define FM_ID__RDA5807M_RDA5807N						0x5808
static __u16 gChipID 	= 0;
static __u8 RDA5807P_REG[10];
/***************************************************
RDA5807P
****************************************************/
__u8 RDA5807P_initialization_reg[] =
{
#if defined(_SHARE_CRYSTAL_24MHz_)
	0xC0, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
	0xC0, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
	0xC0, 0x01, //02H:
#else
	0xC0, 0x01,
#endif

#if defined(_FM_STEP_50K_)
	0x00, 0x12,
#else
	0x00, 0x10,
#endif
	0x04, 0x00,
	0x88, 0xAF, //05H:
	0x00, 0x00,
	0x5E, 0xC6,
	0x50, 0x96,
	0x00, 0x00,
	0x40, 0x00, //0AH:
	0x00, 0x8F,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0xF0, 0x05, //10H:
	0x90, 0x00,
	0xF4, 0x84,
	0x70, 0x01,
	0x40, 0xF0,
	0x21, 0x80, //15H:
	0x7A, 0xD0,
	0x3E, 0x40,
	0x55, 0xA9,
	0xE8, 0x48,
	0x50, 0x80, //1AH:
	0x00, 0x00,
	0x37, 0xB6,
	0x40, 0x0C,
	0x07, 0x9B,
	0x4C, 0x1D,
	0x81, 0x11, //20H:
	0x45, 0xC0,
};

__u8 RDA5807PE_initialization_reg[] =
{
#if defined(_SHARE_CRYSTAL_24MHz_)
	0xc4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
	0xc4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
	0xc4, 0x01, //02H:
#else
	0xC0, 0x01,
#endif

#if defined(_FM_STEP_50K_)
	0x1b, 0x92,
	0x0C, 0x00,
#else       //Step 100K
	0x00, 0x10,
	0x04, 0x00,
#endif

	0x86, 0xad, //05H:
	0x80, 0x00,
	0x5F, 0x1A, //07H
	0x5e, 0xc6,
	0x00, 0x00,
	0x40, 0x6e, //0AH:
	0x2d, 0x80,
	0x58, 0x03,
	0x58, 0x04,
	0x58, 0x04,
	0x58, 0x04,
	0x00, 0x47, //10H:
	0x90, 0x00,
	0xF5, 0x87,
	0x7F, 0x0B, //13H:
	0x04, 0xF1,
	0x5E, 0xc0, //15H: 0x42, 0xc0
	0x41, 0xe0,
	0x50, 0x6f,
	0x55, 0x92,
	0x00, 0x7d,
	0x10, 0xC0,//1AH
	0x07, 0x80,
	0x41, 0x1d,//1CH,
	0x40, 0x06,
	0x1f, 0x9B,
	0x4c, 0x2b,//1FH.
	0x81, 0x10, //20H:
	0x45, 0xa0,// 21H

#if defined(_FM_STEP_50K_)
	0x55, 0x3F, //22H
#else
	0x19, 0x3F, //22H
#endif

	0xaf, 0x40,
	0x06, 0x81,
	0x1b, 0x2a, //25H
	0x0D, 0x04,
	0x80, 0x9F, //0x80, 0x2F,
	0x17, 0x8A,
	0xD3, 0x49,
	0x11, 0x42,
	0xA0, 0xC4, //2BH
	0x3E, 0xBB,
	0x00, 0x00,
	0x58, 0x04,
	0x58, 0x04, //2FH
	0x58, 0x04,
	0x00, 0x74,
	0x3D, 0x00,
	0x03, 0x0C,
	0x2F, 0x68,
	0x38, 0x77, //35H
};

__u8 RDA5807PH_initialization_reg[] =
{
#if defined(_SHARE_CRYSTAL_24MHz_)
	0xC4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
	0xC4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
	0xC4, 0x01, //02H:
#else
	0xC0, 0x01,
#endif
#if defined(_FM_STEP_50K_)
	0x00, 0x12,
#else
	0x00, 0x10,
#endif
	0x04, 0x00,
	0x86, 0xBF, //05H
	0x40, 0x00,
	0x56, 0xC6,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00, //0AH
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x06, //10H
	0x00, 0x19, //
	0x2A, 0x11,
	0x00, 0x2E,
	0x2A, 0x30,
	0xB8, 0x3C, //15H
	0x90, 0x00,
	0x2A, 0x91,
	0x84, 0x12,
	0x00, 0xA8,
	0xC4, 0x00, //1AH
	0xE0, 0x00,
	0x30, 0x1D, //0x24,0x9D,1cH
	0x81, 0x6A,
	0x46, 0x08,
	0x00, 0x86, //1FH
	0x06, 0x61, //20H
	0x00, 0x00,
	0x10, 0x9E,
	0x24, 0xC9, //   0x24,0x47,//0830//23H
	0x04, 0x08, //0830
	0x06, 0x08, //0x06,0x08,//0830  //0X25H
	0xE1, 0x05,
	0x3B, 0x6C,
	0x2B, 0xEC,
	0x09, 0x0F,
	0x34, 0x14, //2AH
	0x14, 0x50,
	0x09, 0x6D,
	0x2D, 0x96,
	0x01, 0xDA,
	0x2A, 0x7B,
	0x08, 0x21,  //30H
	0x13, 0xD5,
	0x48, 0x91,
	0x00, 0xbc,
	0x08, 0x96,  //34H
	0x15, 0x3C, //35H
	0x0B, 0x80,  //36H
	0x25, 0xC7,  //37H
	0x00, 0x00,  //38H
	0x3C, 0x58,
	0x2A, 0x50,
	0x26, 0x2C,
	0x06, 0x08,
	0x02, 0x00,
	0x00, 0x00,
};

__u8 RDA5807N_initialization_reg[] =
{
#if defined(_SHARE_CRYSTAL_24MHz_)
	0xC0, 0x51, //02H: // ȥ��2Mŷ���ص���ĵ�·��ȡֵ0xC0
#elif defined(_SHARE_CRYSTAL_12MHz_)
	0xC4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
	0xC4, 0x01, //02H:
#else
	0xC0, 0x01,
#endif
	0x00, 0x10,
	0x04, 0x00,
	0xC5, 0xaf, //05h	//0xc3
	0x60, 0x00,
	0x42, 0x1a,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,  //0x0ah
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,  //0x10h
	0x00, 0x19,
	0x2a, 0x11,
	0xB0, 0x42,
	0x2A, 0x11,
	0xb8, 0x31,  //0x15h
	0xc0, 0x00,
	0x2a, 0x91,
	0x94, 0x00,
	0x00, 0xa8,
	0xc4, 0x00,  //0x1ah
	0xF7, 0xcF,
	0x2A, 0xDC,  //0x1ch
	0x80, 0x6F,
	0x46, 0x08,
	0x00, 0x86,
	0x06, 0x61, //0x20H
	0x00, 0x00,
	0x10, 0x9E,
	0x23, 0xC8,
	0x04, 0x06,
	0x0E, 0x1C, //0x25H
};

#define  FM_SEARCH_CHN_MIN_FREQ         87000
#define  FM_SEARCH_CHN_MAX_FREQ        108000

#define  FM_SEARCH_JAP_MIN_FREQ         76000
#define  FM_SEARCH_JAP_MAX_FREQ         91000
static __u16 max_freq = 0;
static __u16 min_freq = 0;
/**********************************************
* function:      fm_area_choose
*
* description:   ����ѡ������ͬ����ʼ����ֹ
*                Ƶ��
*
* notes:         ������ȷ�������سɹ�������ʧ��
*
**********************************************/
__s32 fm_area_choose_rda5807(__s32 area, void *pbuffer)
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
* description:   ����ѡ������������ͨ����
*
* notes:
*
**********************************************/
__s32 fm_stereo_choose_rda5807(__s32 audio_method)
{
	__u8 RDA5807P_channel_start_tune[] = {0xc0, 0x01, 0x00, 0x10}; 	//87.0MHz
	__u16 curChan;

	if(audio_method == 0)
	{
		RDA5807P_channel_start_tune[0] &= 0xdf;
	}
	else
	{
		RDA5807P_channel_start_tune[0] |= 0x20;
	}

	RDA5807P_channel_start_tune[0] = RDA5807N_initialization_reg[0]; //just for 5807ph(5807ss)
	RDA5807P_channel_start_tune[1] = RDA5807N_initialization_reg[1];
	RDA5807P_channel_start_tune[2] = curChan >> 2;
	RDA5807P_channel_start_tune[3] = (((curChan & 0x0003) << 6) | 0x10) | (RDA5807P_initialization_reg[3] & 0x0f);	//set tune bit
	OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_channel_start_tune[0]), 2);
	return EPDK_OK;
}
/**********************************************
* function:      fm_mute
*
* description:   ��������
*
* notes:
*
**********************************************/
__s32 fm_mute_rda5807(__s32 voice_onoff)
{
	//__u8 RDA5807P_Muteon[] ={0x80,0x01};
	__u8 RDA5807P_Muteon[] = {0x80, 0x01}; //for 5807pe(5807ss)
	//__u8 RDA5807P_Muteclose[] ={0xc0,0x01};
	//__u8 RDA5807P_Muteclose[] ={0xc0,0x01}; //for 5807pe(5807ss)

	//RDA5807N_initialization_reg

	if(voice_onoff == DRV_FM_VOICE_OFF)
	{
		RDA5807P_Muteon[0] = RDA5807N_initialization_reg[0] & 0xbf;
		RDA5807P_Muteon[1] = RDA5807N_initialization_reg[1];
		__msg("%d\n", voice_onoff);
		OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_Muteon[0]), 2);
	}
	else if(voice_onoff == DRV_FM_VOICE_ON)
	{
		RDA5807P_Muteon[0] = RDA5807N_initialization_reg[0] | 0x40;
		RDA5807P_Muteon[1] = RDA5807N_initialization_reg[1];
		__msg("%d\n", voice_onoff);
		OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_Muteon[0]), 2);
	}

	return EPDK_OK;
}


/**
 * @brief Cover the frequency to channel value
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param __u16 frequency:covered frequency
 * @return __u16: channel value
 * @retval
 */
__u16 RDA5807P_FreqToChan(__u16 n_frequency)
{
	__u8 channelSpacing = 0;
	__u16 bottomOfBand = 0;
	__u16 channel = 0;

	if((RDA5807P_initialization_reg[3] & 0x0c) == 0x00)
	{
		bottomOfBand = 870;
	}
	else if((RDA5807P_initialization_reg[3] & 0x0c) == 0x04)
	{
		bottomOfBand = 760;
	}
	else if((RDA5807P_initialization_reg[3] & 0x0c) == 0x08)
	{
		bottomOfBand = 760;
	}

	if((RDA5807P_initialization_reg[3] & 0x03) == 0x00)
	{
		channelSpacing = 1;
	}
	else if((RDA5807P_initialization_reg[3] & 0x03) == 0x01)
	{
		channelSpacing = 2;
	}

	channel = (n_frequency - bottomOfBand) / channelSpacing;
	return (channel);
}




/**********************************************
* function:      fm_play
*
* description:   ���������Ƶ�ʣ����Ŵ�Ƶ�ʵ�
*                ��̨��Ŀ����ʹû�н�Ŀֻ��
*                ��������������
*
* notes:         ֻ���سɹ�
*
**********************************************/
__s32 fm_play_rda5807(__s32 curFreq)
{
	__u8 RDA5807P_channel_start_tune[] = {0xc0, 0x01, 0x00, 0x10}; 	//87.0MHz
	__u16 temp = (__u16)(curFreq / 100);
	__u16 curChan;
	__msg("temp = %d\n", temp);
	curChan = RDA5807P_FreqToChan(temp);
	__msg("temp = %d\n", temp);
	RDA5807P_channel_start_tune[0] = RDA5807N_initialization_reg[0]; //for 5807pe(5807ss)
	RDA5807P_channel_start_tune[1] = RDA5807N_initialization_reg[1];
	RDA5807P_channel_start_tune[2] = curChan >> 2;
	RDA5807P_channel_start_tune[3] = (((curChan & 0x0003) << 6) | 0x10) | (RDA5807P_initialization_reg[3] & 0x0f);	//set tune bit
	OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_channel_start_tune[0]), 4);
	//	esKRNL_TimeDly(5);     //Delay five ms
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
__bool RDA5807P_ValidStop(__u16 freq)
{
#if 0
	__u8 RDA5807P_reg_data[4] = {0};
	//__u8 RDA5807P_channel_seek[] = {0xc0,0x01};
	//__u8 RDA5807P_channel_seek[] = {0xc0,0x01};
	__u8 falseStation = 0;
	__u16 curChan;
	curChan = RDA5807P_FreqToChan(freq);
	RDA5807P_reg_data[0] = RDA5807N_initialization_reg[0];
	RDA5807P_reg_data[1] = RDA5807N_initialization_reg[1];
	RDA5807P_reg_data[2] = curChan >> 2;
	RDA5807P_reg_data[3] = (((curChan & 0x0003) << 6) | 0x10) | (RDA5807P_initialization_reg[3] & 0x0f);
	OperationRDAFM_2w(WRITE, &(RDA5807P_reg_data[0]), 4);
	esKRNL_TimeDly(10);
	//read REG0A&0B
	OperationRDAFM_2w(READ, &(RDA5807P_reg_data[0]), 4);

	//check whether STC=1
	if((RDA5807P_reg_data[0] & 0x40) == 0)
	{
		falseStation = 1;
	}

	//check FM_TURE
	if((RDA5807P_reg_data[2] & 0x01) == 0)
	{
		falseStation = 1;
	}

	if(falseStation == 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}

#else
	__u8 RDA5807P_reg_data[4] =
	{
		0
	};
	__u8 falseStation = 0;
	__u8 i = 0;
	__u16 curChan;
	curChan = RDA5807P_FreqToChan(freq);
	//SetNoMute
	RDA5807P_REG[0] |=	1 << 6;
	RDA5807P_reg_data[0] = RDA5807N_initialization_reg[0]; //RDA5807P_REG[0];
	RDA5807P_reg_data[1] = RDA5807N_initialization_reg[1]; //RDA5807P_REG[1];
	RDA5807P_reg_data[2] = curChan >> 2;
	RDA5807P_reg_data[3] = (((curChan & 0x0003) << 6) | 0x10) | (RDA5807P_REG[3] & 0x0f);
	OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_reg_data[0]), 4);
	esKRNL_TimeDly(7);    //Dealy 25 ms

	if(0x5808 == gChipID)
	{
		OperationRDAFM_2w(READ, IIC_CHIP__RDA5807_R, &(RDA5807P_reg_data[0]), 4);
		//ReadRDAFM(0x0A,RDA5807P_reg_data,4);
	}
	else
	{
		do
		{
			i++;

			if(i > 5)
			{
				return 0;
			}

			esKRNL_TimeDly(1);
			//read REG0A&0B
			OperationRDAFM_2w(READ, IIC_CHIP__RDA5807_R, &(RDA5807P_reg_data[0]), 4);
			//ReadRDAFM(0x0A,RDA5807P_reg_data,4);
		}
		while((RDA5807P_reg_data[0] & 0x40) == 0);
	}

	//check FM_TURE
	if((RDA5807P_reg_data[2] & 0x01) == 0)
	{
		falseStation = 1;
	}

	if(freq == 960)
	{
		falseStation = 1;
	}

	if(freq == 900)
	{
		falseStation = 1;
	}

	if(falseStation == 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}

#endif
}

/**
 * @brief Get the signal level(RSSI) of the current frequency
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param __u16 curf:frequency value
 * @return __u8: the signal level(RSSI)
 * @retval
 */
__u8 RDA5807P_GetSigLvl(__u16 curf)
{
	__u8 RDA5807P_reg_data[4] = {0};
	OperationRDAFM_2w(READ, IIC_CHIP__RDA5807_R, &(RDA5807P_reg_data[0]), 4);
	//ReadRDAFM(0x0A,RDA5807P_reg_data,4);
	return (RDA5807P_reg_data[2] >> 1); /*����rssi*/
}

/**********************************************
* function:      fm_manual_search
*
* description:   �ֶ�������ʹ��fm_play
*                ����ֵ����һ������Ƶ�ʵ�
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
__s32 fm_manual_search_rda5807(__s32 freq, __u32 search_dir)
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

		returnValue = RDA5807P_ValidStop(temp);
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
__s32  fm_auto_search_rda5807(__s32 freq, __u32 search_dir)
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

		returnValue = RDA5807P_ValidStop(temp);
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
void RDA5807P_SetVolumeLevel(__u8 level)
{
	RDA5807P_initialization_reg[7] = (RDA5807P_initialization_reg[7] & 0xf0) | level;
	OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_initialization_reg[0]), 8);
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

__s32  fm_init_rda5807(void)
{
	__u8 error_ind 				= 0;
	__u8 RDA5807P_REGR[10]		= {0x0};
	__u8 i 						= 0;
	__s8 flagAgainCnt;
#if SP_SimIIC
	I2C_Init();
	esKRNL_TimeDly(10);
#endif
	{
		__u8 thread_hold;
		thread_hold = 4;
		{
			//init key val by app_config.fex
			char *path = "d:\\app_config.bin";
			H_PARSER parser;
			__s32 ret;
			char keyname[64];
			int val;

			do
			{
				parser = NULL;
				parser = esCFG_Init_Ex(path);

				if(NULL == parser)
				{
					__wrn("esCFG_Init_Ex fail...\n");
					break;
				}

				__wrn("before esCFG_GetKeyValue_Ex\n");
				eLIBs_strcpy(keyname, "system");
				eLIBs_strcat(keyname, "/");
				eLIBs_strcat(keyname, "fm_th");
				ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)&val, 1);
				__wrn("after esCFG_GetKeyValue_Ex, ret=%d\n", ret);

				if(EPDK_OK == ret)
				{
					thread_hold = val;
					__wrn("fm_th=%d\n", val);
				}
			}
			while(0);

			if(parser)
			{
				esCFG_Exit_Ex(parser);
				parser = NULL;
			}
		}
		thread_hold &= 0x3f;
		__wrn("thread_hold=%d\n", thread_hold);
		//RDA5807N_initialization_reg[11] bit2:7���������ȣ�ԭ����110�� �޸ĺ�Ϊ100
		RDA5807N_initialization_reg[11] &= ~(0x3f << 2);
		RDA5807N_initialization_reg[11] |= (thread_hold << 2);
	}
	RDA5807P_REG[0] = 0x00;
	RDA5807P_REG[1] = 0x02;
	error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807P_REG[0], 2);
	esKRNL_TimeDly(5);
	error_ind = OperationRDAFM_2w(READ, IIC_CHIP__RDA5807_R, (__u8 *)&RDA5807P_REGR[0], 10);
	//ReadRDAFM(0x0A,RDA5807P_REGR,10);
	esKRNL_TimeDly(5);
	//for(i = 0; i < 10; i++)
	//{
	//	__msg("###################RDA5807P_REGR[%d] = %x \n", i, RDA5807P_REGR[i]);
	//}
	gChipID = RDA5807P_REGR[8];
	gChipID = ((gChipID << 8) | RDA5807P_REGR[9]);
	flagAgainCnt = 0;

	do
	{
		switch(gChipID)
		{
			case FM_ID__RDA5807PH:
			{
				for(i = 0; i < 8; i++)
				{
					RDA5807P_REG[i] = RDA5807PH_initialization_reg[i];
				}

				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807PH_initialization_reg[0], 2);
				esKRNL_TimeDly(60);
				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807PH_initialization_reg[0], sizeof(RDA5807PH_initialization_reg));
				break;
			}

			case FM_ID__RDA5807PE:
			{
				for(i = 0; i < 8; i++)
				{
					RDA5807P_REG[i] = RDA5807PE_initialization_reg[i];
				}

				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807PE_initialization_reg[0], 2);
				esKRNL_TimeDly(60);
				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807PE_initialization_reg[0], sizeof(RDA5807PE_initialization_reg));
				break;
			}

			case FM_ID__RDA5807M_RDA5807N:
			{
				for(i = 0; i < 8; i++)
				{
					RDA5807P_REG[i] = RDA5807N_initialization_reg[i];
				}

				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807N_initialization_reg[0], 2);
				esKRNL_TimeDly(60);
				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807N_initialization_reg[0], sizeof(RDA5807N_initialization_reg));
				break;
			}

			case FM_ID__RDA5807P:
			case FM_ID__RDA5807P_:
			{
				gChipID = FM_ID__RDA5807P;

				for(i = 0; i < 8; i++)
				{
					RDA5807P_REG[i] = RDA5807P_initialization_reg[i];
				}

				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807P_initialization_reg[0], 2);
				esKRNL_TimeDly(60);
				error_ind = OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, (__u8 *)&RDA5807P_initialization_reg[0], sizeof(RDA5807P_initialization_reg));
				break;
			}

			default:
			{
				flagAgainCnt++;
				__msg("flagAgainCnt=%d\n", flagAgainCnt);

				switch(flagAgainCnt)
				{
					case 1:
						gChipID = RDA5807P_REGR[4];
						gChipID = ((gChipID << 8) | RDA5807P_REGR[5]);
						break;

					default:
						flagAgainCnt = -1;
						break;
				}
			}
		}
	}
	while(flagAgainCnt > 0);

	__wrn("error_ind:%x\n", error_ind);
	esKRNL_TimeDly(5);//Dealy 50 ms

	if(error_ind || flagAgainCnt < 0)
	{
		__here__
		return 0;
	}
	else
	{
		__here__
		__msg("\nXXXXXXXXXXXXX  gChipID = 0x%X   XXXXXXXXXXXXXXX\n",	gChipID);
		return gChipID;
	}
}

__s32  fm_exit_rda5807(void)
{
	__u8 RDA5807P_poweroff[] = {0x00, 0x00};
	OperationRDAFM_2w(WRITE, IIC_CHIP__RDA5807_W, &(RDA5807P_poweroff[0]), 2);
	return EPDK_OK;
}
__s32  fm_send_on_rda5807(void)
{
	return EPDK_FAIL;
}

__s32  fm_send_rda5807(__s32 freq)
{
	return EPDK_FAIL;
}

__s32  fm_send_off_rda5807(void)
{
	return EPDK_FAIL;
}

__s32  fm_pa_gain_rda5807(__u8 pagain)
{
	return EPDK_FAIL;
}

__s32  fm_get_status_rda5807(void)
{
	return FM_DRV_STATUS_RECEIVE;
}
/**********************************************
* function:      fm_signal_level
*
* description:   �ź�ǿ��ѡ��Ҫ���ź�ǿ��Խ�ߣ��յ��ĵ�̨Խ��
*                   Ҫ���ź�ǿ��Խ�ߣ��յ��ĵ�̨Խ�࣬����Ч��̨Ҳ��
*
* notes:
*
**********************************************/
__s32 fm_signal_level_rda5807(__s32 signal_level)
{
	return EPDK_OK;
}

#endif

