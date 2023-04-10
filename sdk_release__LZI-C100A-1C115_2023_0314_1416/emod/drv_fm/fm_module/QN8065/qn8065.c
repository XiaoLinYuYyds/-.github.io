 /*--------------------------------------------------------------------------*/
/**@file QN8065.C
   @brief FM模块QN8065驱动
   @details FM模块QN8065的驱动函数
   @author NC
   @date 2013-10-23
   @note 通过宏定义来选择驱动方式
*/
/*----------------------------------------------------------------------------*/
#include  "drv_fm_i.h"

#define FM_QN8065_EN					(FM_MODULE_TYPE & FM_MODULE_TYPE_QN8065)
#if FM_QN8065_EN
#define UINT16				__u16
#define UINT8					__u8
#include "qn8065.h"

//if using pilot as CCA algorithm,macro QN8065_PILOT_CCA will be set to 1
#define QN8065_PILOT_CCA 		           0			// 过滤假台

#if (FM_CLOCK == USE_IO_12MHZ_CLK_RC)
//if using scan noise floor as CCA algorithm,macro QN8065_SCAN_NOISE_FLOOR_CCA will be set to 1
#define QN8065_SCAN_NOISE_FLOOR_CCA			0
//if want improving some special frequency SNR, macro IMPROVING_SNR will be set to 1
#define IMPROVING_SNR                       0
#else
#define QN8065_SCAN_NOISE_FLOOR_CCA		    1
#define IMPROVING_SNR                       1
#endif
u8 QN8065_PreNoiseFloor = 40, QN8065_NoiseFloor = 40;
static __u16 max_freq = 0;
static __u16 min_freq = 0;

//#pragma constseg="QN8065_RX_TABLE"
#if 0
//32级音量
u8 volume_8065table[33] = {0, 2, 4, 6, 8, 10, 12, 13, 14, 15, 16, 17, 18, \
                           19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 36, 38, 41, 43, 45, 47
                          };
#else
//16级音量
u8 volume_8065table[17] = {0, 4, 8, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 34, 38, 43, 47};
#endif
//#pragma constseg=default

void QN8065_Delay(u16 us) // @"QN8065_RX"
{	// 单位: ms
	__s32 i;
	__s32 usTemp = us * 128;

	for(i = 0; i < usTemp; i++)
		;
}

UINT16 QN8065_FREQ2CHREG(UINT16 freq) // @"QN8065_RX"
{
	return	(freq - 6000) / 5;
}

/**********************************************************************
void QN8065_SetMute(u8 On)
**********************************************************************
Description: on or off mute chip
Parameters:
    On: 1:on mute chip;0:off mute chip
Return Value:
    None
**********************************************************************/
void QN8065_SetMute(u8 On) // @"QN8065_RX"
{
	//mute or unmute by holding DAC
	//and set low voltage regulator output as 0.9v
	QN8065_WriteReg(QN8065_REG_REF, On ? 0x14 : 0x10);
}

/**********************************************************************
void QN8065_SetCh(UINT16 start,UINT16 stop,u8 step)
**********************************************************************
Description: set channel frequency
Parameters:
    freq:  channel frequency to be set,frequency unit is 10KHZ
Return Value:
    None
**********************************************************************/
void QN8065_SetCh(UINT16 start, UINT16 stop, u8 step) // @"QN8065_RX"
{
	u8 temp;
	//TRACE("start:%d,stop:%d,step:%d \n",start,stop, step);
	start = QN8065_FREQ2CHREG(start);
	//fored the stop is bigger than start
	stop = QN8065_FREQ2CHREG(stop);
	//writing higher bits of channel frequency,CCA channel start,
	//stop and step index at first
	temp = (u8)((start >> 8) & QN8065_CH_CH);
	temp |= ((u8)(start >> 6) & QN8065_CH_CH_START);
	temp |= ((u8)(stop >> 4) & QN8065_CH_CH_STOP);
	temp |= (step << 6);
	QN8065_WriteReg(QN8065_CH_STEP, temp);
	//writing lower 8 bits of CCA channel start index
	QN8065_WriteReg(QN8065_CH_START, (u8)start);
	//writing lower 8 bits of CCA channel stop index
	QN8065_WriteReg(QN8065_CH_STOP, (u8)stop);
	//writing lower 8 bits of channel index
	QN8065_WriteReg(QN8065_CH, (u8)start);
}

/**********************************************************************
void qn8065_init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.
Parameters:
    None
Return Value:
    None
**********************************************************************/
void qn8065_init(void)
{
	QN8065_WriteReg(QN8065_SYSTEM1, 0x81);
	esKRNL_TimeDly(1);
	/*********User sets chip working clock **********/
	//Following is where change the input clock wave type,as sine-wave or square-wave.
	QN8065_WriteReg(QN8065_CCA, QN8065_SINE_WAVE_CLOCK); //正弦波
	//Following is where change the input clock frequency.

	QN8065_WriteReg(QN8065_XTAL_DIV0, QN8065_XTAL_DIV0_VAL);
	QN8065_WriteReg(QN8065_XTAL_DIV1, QN8065_XTAL_DIV1_VAL);
	QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);
	esKRNL_TimeDly(1);

	/********User sets chip working clock end ********/
	QN8065_WriteReg(QN8065_CCA1, 0x96);
	QN8065_WriteReg(QN8065_CCOND1, 0xb6);
	///select SNR for SM,SNC,HCC MPX
	QN8065_WriteReg(QN8065_SMSTART, 0x80 | QN8065_SMSTART_VAL); //set SMSTART
	QN8065_WriteReg(QN8065_SNCSTART, 0x80 | QN8065_SNCSTART_VAL); //set SNCSTART
	QN8065_WriteReg(QN8065_HCCSTART, 0x80 | QN8065_HCCSTART_VAL); //set HCCSTART
	//enter receiver mode directly
	QN8065_WriteReg(QN8065_SYSTEM1, 0x11);
	QN8065_SetMute(1);
}

/**********************************************************************
void qn8065_tune2ch(u16 ch)
**********************************************************************
Description: Tune to the specific channel.
Parameters:
	ch:Set the frequency (10kHz) to be tuned,
eg: 101.30MHz will be set to 10130.
Return Value:
	None
**********************************************************************/
void qn8065_tune2ch(UINT16 ch)
{
	UINT8 sysClk32kFlag = 0;
	UINT8 stereoFlag = 0;
	UINT8 reg;
	QN8065_SetMute(1);

	if(QN8065_ReadReg(QN8065_XTAL_DIV0) == 0x01)
	{
		sysClk32kFlag = 1;
		stereoFlag = (QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX) ? 0 : 1;
	}

#if IMPROVING_SNR

	//improving some special stations snr when working clock is 32768Hz
	if(ch == 10030 || ch == 10090 || ch == 10100 || ch == 10110
	    || ch == 10150 || ch == 10160 || ch == 10220)
	{
		//frequency is added 50k only system clock is 32K.
		if(sysClk32kFlag)
		{
			QN8065_WriteReg(QN8065_XTAL_DIV2, 0x67);
			//add 50K
			ch += 5;
		}
	}
	//restore XTAL_DIV2 right value if the station is not a special station
	else
	{
		QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);
	}

#endif
	QN8065_WriteReg(QN8065_REG_FLL, 0x84); //enable 2k speed
	//enter idle mode and disable CCA, with user write into frequency
	QN8065_WriteReg(QN8065_SYSTEM1, 0x01);
	//step is 100K.
	QN8065_SetCh(ch, ch, 1);
	//enter rx mode and disable CCA, with user write into frequency
	QN8065_WriteReg(QN8065_SYSTEM1, 0x11);
	QN8065_Delay(QN8065_CH_SETUP_DELAY_TIME);
	QN8065_WriteReg(QN8065_REG_FLL, 0x04);//disable 2k speed

	if(sysClk32kFlag)
	{
		reg = QN8065_ReadReg(QN8065_CCA1);

		//disable bypass 32k spur rejection filter if is stereo
		if(stereoFlag)
		{
			reg &= ~0x20;
		}
		//enable bypass 32k spur rejection filter if is mono
		else
		{
			reg |= 0x20;
		}

		QN8065_WriteReg(QN8065_CCA1, reg);
	}

	QN8065_SetMute(0);
}

#if QN8065_SCAN_NOISE_FLOOR_CCA
/***********************************************************************
Description: scan a noise floor from 87.5M to 108M by step 200K
Parameters:
Return Value:
1: scan a noise floor successfully.
0: chip would not normally work.
**********************************************************************/
u8 QN8065_ScanNoiseFloor(u16 start, u16 stop) // @"QN8065_RX"
{
	u8 regValue;
	u8 timeOut = 255; //time out is 2.55S
	QN8065_WriteReg(QN8065_CCA_TIME, 0x00);
	//config CCS from start frequency to  stop frequency by step 200KHZ
	QN8065_SetCh(start, stop, 2);
	//enable CCS
	QN8065_WriteReg(QN8065_CCA2, 0x95);
	//enter CCA mode,channel index is decided by internal CCA
	QN8065_WriteReg(QN8065_SYSTEM1, 0x12);

	while(1)
	{
		regValue = QN8065_ReadReg(QN8065_SYSTEM1);

		//if it seeks a potential channel, the loop will be quited
		if((regValue & QN8065_CHSC) == 0)
		{
			break;
		}

		esKRNL_TimeDly(1);   //delay 10ms

		//if it was time out,chip would not normally work.
		if((timeOut--) == 0)
		{
			//disable CCS while exit
			QN8065_WriteReg(QN8065_CCA2, 0x85);
			return 0;
		}
	}

	//disable CCS while exit
	QN8065_WriteReg(QN8065_CCA2, 0x85);
	QN8065_NoiseFloor = QN8065_ReadReg(QN8065_CCS_NOISE) & 0x7f;

	if(((QN8065_PreNoiseFloor - QN8065_NoiseFloor) > 2) || ((QN8065_NoiseFloor - QN8065_PreNoiseFloor) > 2))
	{
		QN8065_PreNoiseFloor = QN8065_NoiseFloor;
	}

	//TRACE("%d-%d noiseFloor:%d,timeElapsed:%dms\n",start,stop,QN8065_NoiseFloor,(255-timeOut)*10);
	return 1;
}
#endif

/***********************************************************************
void QN8065_RXSetTH(void)
***********************************************************************
Description: Setting the threshold value of automatic scan channel

Return Value:
  	None
***********************************************************************/
void QN8065_RXSetTH(void) // @"QN8065_RX"
{
    UINT8 rssiTH;
    rssiTH = (QN8065_PreNoiseFloor-28)+3;
#if IMPROVING_SNR
	//restore XTAL_DIV2 right value if the station is not a special station
	QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);
#endif
	//set IF counter error range value is 256 = 4*64
	QN8065_WriteReg(QN8065_CCA_CNT, 8);//4		// 值小台少
	//set CCA time
	QN8065_WriteReg(QN8065_CCA_TIME, QN8065_CCA_NAGC_40MS | QN8065_CCA_NSNR_20MS | QN8065_CCA_NRSSI_10MS);
	//set RSSI threshold for CCA
	QN8065_WriteReg(QN8065_CCA, QN8065_ReadReg(QN8065_CCA)&~0x3f | 10);
	//set QN8065_SNR threshold for CCA
#if QN8065_PILOT_CCA//8-11
	QN8065_WriteReg(QN8065_CCA_SNR_TH, QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f | 8); //pilot CCA the QN8065_SNR thershold is 8
#else
	QN8065_WriteReg(QN8065_CCA_SNR_TH, QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f | 8); 	//normall CCA the QN8065_SNR thereshold is 9
#endif
	//TRACE("rssiTH=%d, snrTH=%d\n", rssiTH,snrTH);
}

/***********************************************************************
INT8 QN8065_RXValidCH(UINT16 freq);
***********************************************************************
Description: to validate a ch (frequency)(if it's a valid channel)
Freq: specific channel frequency, unit: 10Khz
  eg: 108.00MHz will be set to 10800.
Return Value:
  0: it's not a valid channel
  1: it's a valid channel
 -1: chip does not normally work.
***********************************************************************/
u8 QN8065_RXValidCH(u16 freq) // @"QN8065_RX"
{
	u8 regValue;
	u8 timeOut;
	u8 isValidChannelFlag;
#if QN8065_PILOT_CCA
	u8 snr, readCnt, stereoCount = 0;
#endif
	__msg("QN8065_RXValidCH(%d)\n", freq);
	//return 1;
#if QN8065_SCAN_NOISE_FLOOR_CCA

	switch(freq)
	{
		case 8750:
			QN8065_ScanNoiseFloor(8750, 8800);
			QN8065_RXSetTH();
			break;

		case 8810:
			QN8065_ScanNoiseFloor(8810, 9000);
			QN8065_RXSetTH();
			break;

		case 9010:
			QN8065_ScanNoiseFloor(9010, 9200);
			QN8065_RXSetTH();
			break;

		case 9210:
			QN8065_ScanNoiseFloor(9210, 9400);
			QN8065_RXSetTH();
			break;

		case 9410:
			QN8065_ScanNoiseFloor(9410, 9600);
			QN8065_RXSetTH();
			break;

		case 9610:
			QN8065_ScanNoiseFloor(9610, 9800);
			QN8065_RXSetTH();
			break;

		case 9810:
			QN8065_ScanNoiseFloor(9810, 10000);
			QN8065_RXSetTH();
			break;

		case 10010:
			QN8065_ScanNoiseFloor(10010, 10200);
			QN8065_RXSetTH();
			break;

		case 10210:
			QN8065_ScanNoiseFloor(10210, 10400);
			QN8065_RXSetTH();
			break;

		case 10410:
			QN8065_ScanNoiseFloor(10410, 10600);
			QN8065_RXSetTH();
			break;

		case 10610:
			QN8065_ScanNoiseFloor(10610, 10800);
			QN8065_RXSetTH();
			break;

		default:
			//delay_5ms(100); //this delay can improve user display effect
			break;
	}

	//enter idle mode and disable CCA, with user write into frequency
	QN8065_WriteReg(QN8065_SYSTEM1, 0x01);
#endif	
	QN8065_SetCh(freq, freq + 10, 2);
	//entering into RX mode and CCA mode,channels index decide by CCA.
	QN8065_WriteReg(QN8065_SYSTEM1, 0x12);
	timeOut = 60;  // time out is 300ms

	while(1)
	{
		regValue = QN8065_ReadReg(QN8065_SYSTEM1);

		//if it seeks a potential channel, the loop will be quited
		if((regValue & QN8065_CHSC) == 0)
		{
			break;
		}

		esKRNL_TimeDly(5);   //delay 5ms
		//__msg("timeOut=%d\n", timeOut);

		//if it was time out,chip would not normally work.
		if((timeOut--) == 0)
		{
			return 0;	// -1 搜台异常
		}
	}

	//reading out the rxcca_fail flag of RXCCA status
	isValidChannelFlag = (QN8065_ReadReg(QN8065_STATUS1) & QN8065_RXCCA_FAIL ? 0 : 1);

	if(isValidChannelFlag)
	{
		QN8065_SetMute(0);					//搜到真台声音打开
#if QN8065_PILOT_CCA
		//set pilot threshold as 1/256
		QN8065_WriteReg(QN8065_CCOND2, 0x91);
		esKRNL_TimeDly(5);
		snr = QN8065_ReadReg(QN8065_SNR);

		if(snr < 25) //25
		{
			isValidChannelFlag = 0;

			for(readCnt = 10; readCnt > 0; readCnt--)
			{
				esKRNL_TimeDly(1);
				stereoCount += ((QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX) ? 0 : 1);

				if(stereoCount >= 3)
				{
					isValidChannelFlag = 1;
					break;
				}
			}
		}

		//restore pilot threshold as 2/256
		QN8065_WriteReg(QN8065_CCOND2, 0x92);
		return (isValidChannelFlag);
#else
		return 1;
#endif
	}

	return 0;
}

/************************************************************************
void QN8065_RXConfigAudio(u8 volume )
*************************************************************************
Description: config audio
Parameters:
  volume:control the volume gain,range : 0~47(-47db~0db)
Return Value:
    None
**********************************************************************/
void QN8065_RXConfigAudio(u8 option) // @"QN8065_RX"
{
	u8 regVal;
	option = option > 47 ? 47 : option;
	regVal = QN8065_ReadReg(QN8065_VOL_CTL);
	regVal = (regVal & 0xc0) | (option / 6) | ((5 - option % 6) << 3);
	QN8065_WriteReg(QN8065_VOL_CTL, regVal);
}

void qn8065_set_vol(u8 volume) // @"QN8065_RX"
{
	if(volume == 0)
	{
		QN8065_SetMute(1);
	}
	else
	{
		QN8065_SetMute(0);
	}

	QN8065_RXConfigAudio(volume_8065table[volume]);
}

void qn8065_off(void) // @"QN8065_RX"
{
	QN8065_WriteReg(QN8065_SYSTEM1, 0x20);
	esKRNL_TimeDly(5);
}

u8 qn8065_seek(u16 freq) // @"QN8065_RX"
{
	QN8065_SetMute(1);
	__msg("qn8065_seek(%d)\n", freq);
	
#if (FM_CLOCK == USE_IO_12MHZ_CLK_RC)
	return QN8065_RXValidCH(freq);
#else
	return QN8065_RXValidCH(freq * 10); //qn8035 step frequency unit is 10KHZ
#endif
}

__s32  fm_auto_search_qn8065(__s32 freq, __u32 search_dir)
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

		returnValue = qn8065_seek(temp);
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

/**********************************************
* function:      fm_area_choose
*
* description:   地区选择，区别不同的起始和终止
*                频率
*
* notes:         输入正确地区返回成功，否则失败
*
**********************************************/
__s32 fm_area_choose_qn8065(__s32 area, void *pbuffer)
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

	__msg("fm_area[%d,%d]\n", freq_range->fm_area_min_freq, freq_range->fm_area_max_freq);
	return EPDK_OK;
}

__s32 fm_mute_qn8065(__s32 flag)
{
	if(flag == DRV_FM_VOICE_OFF)
	{
		QN8065_SetMute(1);
	}
	else
	{
		QN8065_SetMute(0);
	}

	return EPDK_OK;
}

////////////////////////////////////////////
u8 QN8065_ReadReg(u8 adr) // @"QN8065_RX"
{
	if(g_flagFmIicFail & FM_MODULE_TYPE_QN8065)
	{
		__msg("QN8065_ReadReg fail.\n");
		return 0;
	}

	{
		__twi_dev_para_ex_t  piic;
		__s32 ret;
		__s32 		cpu_sr;
		__u8	data;
		ENTER_CRITICAL(cpu_sr);
		eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
		piic.slave_addr_flag	= TWI_SLAVE_ADDRESS_07BIT;
		piic.slave_addr 			= 0x21 >> 1;
		piic.byte_addr_width	= 1;
		piic.byte_addr				= &adr;
		piic.byte_count 			= 1;
		piic.data 				= &data;
		ret = eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, &piic);

		if(ret != EPDK_OK)
		{
			data = 0;
			g_flagFmIicFail |= FM_MODULE_TYPE_QN8065;
			__wrn("....QN8065_ReadReg FAIL.............\n");
		}

		EXIT_CRITICAL(cpu_sr);
		//QN8065_Delay(60);
		return data;
	}
}

__s32 QN8065_WriteReg(u8 adr, u8 value) // @"QN8065_RX"
{
	if(g_flagFmIicFail & FM_MODULE_TYPE_QN8065)
	{
		__msg("QN8065_WriteReg fail.\n");
		return 0;
	}

	{
		__twi_dev_para_ex_t  piic;
		__s32 ret;
		__s32 					cpu_sr;
		ENTER_CRITICAL(cpu_sr);
		eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
		piic.slave_addr_flag	= TWI_SLAVE_ADDRESS_07BIT;
		piic.slave_addr 				= 0x20 >> 1;
		piic.byte_addr_width	= 1;
		piic.byte_addr					= &adr;
		piic.byte_count 				= 1;
		piic.data 							= &value;
		ret = eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, &piic);
		
		if(ret != EPDK_OK)
		{
			g_flagFmIicFail |= FM_MODULE_TYPE_QN8065;
			__wrn("....QN8065_WriteReg FAIL.............\n");
		}
		
		EXIT_CRITICAL(cpu_sr);
		//QN8065_Delay(60);
		return ret;
	}
}

u8 qn8065_online(void) // @"QN8065_RX"
{
	u8 temp, i;

	for(i = 0; i < 3; i++)
	{
		temp = QN8065_ReadReg(0x06) & 0xFC;
		__msg("QN_id=%02x\n", temp);

		if(temp == CHIPID_QN8065)
		{
			qn8065_init();
			return temp;
		}
	}

	return 0;
}

u16 GetIFCount(void) // @"QN8065_RX"
{
	u8 r_temp;
	u8 IF_CNT1;
	UINT16 IF_CNT;
	IF_CNT1 = QN8065_ReadReg(0x1D);
	IF_CNT = QN8065_ReadReg(0x1E);
	IF_CNT = IF_CNT << 8;
	IF_CNT = IF_CNT + IF_CNT1;
	r_temp = QN8065_ReadReg(0x11);
	r_temp = r_temp & 0xfd;
	QN8065_WriteReg(0x11, r_temp);
	r_temp = r_temp | 0x02;
	QN8065_WriteReg(0x11, r_temp);
	return IF_CNT;
}

#endif
