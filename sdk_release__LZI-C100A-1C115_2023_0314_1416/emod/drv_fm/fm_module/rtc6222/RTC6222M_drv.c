
#include  "drv_fm_i.h"

#define FM_RTC6222_EN					(FM_MODULE_TYPE & FM_MODULE_TYPE_RTC6222)
#if FM_RTC6222_EN
#define UINT16				__u16
#define UINT8					__u8
#include "RTC6222M_drv.h"
#define FM_SEEK_50K_STEP                1

#if FM_SEEK_50K_STEP
#define __RTCFM_STEP_50K__                      //打开宏使用50k步进，关闭使用100k步进
#define __RTCFM_STEP_50K_CHOOSE_BEST_FROM_TW0__
#endif

#define USE_IO_32KHZ_CLK				0
#define USE_SD_IO_32KHZ_CLK			1
#define SHARE_CRYSTAL_32KHz			2
#define USE_IO_12MHZ_CLK				3
#define USE_SD_IO_12MHZ_CLK			4
#define SHARE_CRYSTAL_12MHz			5
#define SHARE_CRYSTAL_24MHz			6

#define RTC6222_FM_CLOCK 						SHARE_CRYSTAL_24MHz



static __u16 max_freq = 0;
static __u16 min_freq = 0;






u8 rtc6222_ReadReg(__u8 addr, __u8 *data,__s32 n_byte) 
{
	if(g_flagFmIicFail & FM_MODULE_TYPE_RTC6222)
	{
		__msg("RTC6222_ReadReg fail.\n");
		return 0;
	}

	{
		__twi_dev_para_ex_t  piic;
		__s32 ret;
		__s32 		cpu_sr;
		ENTER_CRITICAL(cpu_sr);
		eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
		piic.slave_addr_flag	= TWI_SLAVE_ADDRESS_07BIT;
		piic.slave_addr 			= 0xc8 >> 1;
		piic.byte_addr_width	= 1;
		piic.byte_addr				= &addr;
		piic.byte_count 			= n_byte;
		piic.data 				= (__u8 *)data;
		ret = eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, &piic);
		
		if(ret != EPDK_OK)
		{
			data = 0;
			g_flagFmIicFail |= FM_MODULE_TYPE_RTC6222;
			__msg("....RTC6222_ReadReg FAIL.............\n");
		}

		EXIT_CRITICAL(cpu_sr);
		
		return ret;
	}
}

__s32 rtc6222_WriteReg(__u8 addr, __u8 *data,__s32 n_byte) // @"QN8065_RX"
{
	if(g_flagFmIicFail & FM_MODULE_TYPE_RTC6222)
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
		piic.slave_addr 				= 0xc8 >> 1;
		piic.byte_addr_width	= 1;
		piic.byte_addr					= &addr;
		piic.byte_count 				= n_byte;
		piic.data 							= (__u8 *)data;
		ret = eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, &piic);
		
		if(ret != EPDK_OK)
		{
			g_flagFmIicFail |= FM_MODULE_TYPE_RTC6222;
			__wrn("....QN8065_WriteReg FAIL.............\n");
		}
		
		EXIT_CRITICAL(cpu_sr);
	
		return ret;
	}
}

static __s32 fm_write_data_rtc6222(__u8 addr, __u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = rtc6222_WriteReg(addr,data,n_byte);
    if(ret != EPDK_OK)
    {
        __err("fm rtc6222 i2c write error~\n");
    }
    return ret;
}
static __s32 fm_read_data_rtc6222(__u8 addr, __u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = rtc6222_ReadReg(addr,data,n_byte);
    if(ret == EPDK_FAIL)
    {
        __err("fm rtc6222 i2c read error~\n");
    }
    return ret;
}

void OperationRTC6222_write(__u16 in_data, __u8 addr)
{
	__u8 data[2];

	data[0] = in_data>>8;
	data[1] = in_data;
	fm_write_data_rtc6222(addr,&data[0],2);
	//__wrn("write: addr = 0x%4x,in_data = 0x%4x\n",addr,in_data);
}

__u16 OperationRTC6222_read(__u8 addr)
{
	__u8 data[2]={0};
	__u16 read_data;

	fm_read_data_rtc6222(addr,&data[0],2);
	read_data = data[0];
	read_data = read_data<< 8;
	read_data += data[1];	
	//eLIBs_printf("addr = 0x%x  read: read_data = 0x%4x,data[0] = 0x%x,data[1] = 0x%x\n",addr,read_data,data[0],data[1]);	
	return read_data;
}
void delay_5ms(__u8 ms)
{
	esKRNL_TimeDly((ms+1)/2);
}
void RTC6222_SetMute(u8 On) // @"QN8065_RX"
{
	__u16 write_byte;

	write_byte = OperationRTC6222_read(0x02);
	if (On)
		write_byte &= ~0x4000;
	else
		write_byte |= 0x4000;
	OperationRTC6222_write(write_byte,0x02);
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
void rtc6222_init(void)
{
	OperationRTC6222_write(0x16AA,0x00); //Power off
	delay_5ms(2);

	OperationRTC6222_write(0x96AA,0x00);//Power on
	delay_5ms(2);
	//print_all_reg();

	OperationRTC6222_write(0x4008,0x02);//vol = 0a ,disable mute
	
#ifndef __INTERNAL_SEEK__
#ifdef __RTCFM_STEP_50K__
	OperationRTC6222_write(0x1100,0x04);//enable rclk osc
#else
	OperationRTC6222_write(0x1900,0x04);//enable rclk osc
#endif

	OperationRTC6222_write(0x650f,0x05);//0x650f//set seek noise th and spike th ,0x01h=0010bin,00=80(noise_th),01=144(spike_th),and rssi th=0x0c.

#if ((RTC6222_FM_CLOCK == USE_IO_32KHZ_CLK) || (RTC6222_FM_CLOCK == USE_SD_IO_32KHZ_CLK) || (RTC6222_FM_CLOCK == SHARE_CRYSTAL_32KHz))
		OperationRTC6222_write(0xB000,0x06);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
#elif ((RTC6222_FM_CLOCK == USE_IO_12MHZ_CLK) || (RTC6222_FM_CLOCK == USE_SD_IO_12MHZ_CLK) || (RTC6222_FM_CLOCK == SHARE_CRYSTAL_12MHz))
	   OperationRTC6222_write(0xB001,0x06);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from	32k~38.4M
#elif (RTC6222_FM_CLOCK == SHARE_CRYSTAL_24MHz)
		OperationRTC6222_write(0xB005,0x06);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
		OperationRTC6222_write(0xB400,0x07);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
#endif


	OperationRTC6222_write(0x043C,0x08);//set seek band of freq  start_freq0x043C=1084. x100khz
	OperationRTC6222_write(0x035C,0x09);//set seek start_freq 0x035C=860, x100khz

	OperationRTC6222_write(0x0B10,0x11);//rclk force 0x0Bh= bin1011


	OperationRTC6222_write(0x1008,0x20);//1008 hopping open is better than 1018 nohopping
#else //__INTERNAL_SEEK__
	OperationRTC6222_write(0x6000,0x05);// Use internal Clock For Seek UP 20151221
	OperationRTC6222_write(0x0700,0x43);// unkown reson  but just do it.
	OperationRTC6222_write(0x0348,0x09);//set seek top bottom of freq  start_freq0x0348=840. x100khz
	OperationRTC6222_write(0x0465,0x08);//set seek top band of freq  start_freq0x0465=1125. x100khz
#endif //end of __INTERNAL_SEEK__

	delay_5ms(1);
	OperationRTC6222_write((OperationRTC6222_read(0x06)|0x8000),0x06);		// Write Power On
	delay_5ms(1); // For print request delay time because wait power on finish time
}




u8 RTC6222_set_freq(__u16 channel_freq) // @"rtc6222_RX"
{
	__u16 write_byte;
	__u8 timeout = 0;
	__u16 Seek_Status = 0;
	__u16 Seek_Status2 = 0;//no used
	__u16 read_buf; //no used

#ifndef __INTERNAL_SEEK__

#ifndef __RTCFM_STEP_50K__
	channel_freq *= 10;
#endif

	OperationRTC6222_write(0x8000|channel_freq,0x03);

	//New Algorithm part 1=on 0=off by Danny 20150724
	do{
		delay_5ms(8);				// 8 = 40ms(Default) 2 = 10ms(Speed limit) by Danny 20150724
		timeout++;
		Seek_Status = ((0xFC00 & OperationRTC6222_read(0x0B)) >> 10);	//add by Danny 20150702   
	   //Print_Reg(1);			 //if open this trace log will make more than 30 fake channel and assert restart. so  just for test.
	}while ((Seek_Status == 0) && ((0x4000 & OperationRTC6222_read(0x0A))== 0) && (timeout < 200));

	
	write_byte = channel_freq & (~0x8000);
	OperationRTC6222_write(write_byte,0x03);

	//while(0x4000 & OperationRTC6222_read(0x0A)){};

			Seek_Status2 = ((0xFC00 & OperationRTC6222_read(0x0B)) >> 10);	 //add by Danny 20150702
		if(Seek_Status == 0)			//When Get Seek_Satus BK0_B[15:10] all 6b' = 0, Return 1. add by Danny 20150703
		{
			return 1;
		}
		else
		{
			return 0;
		}

#else //__INTERNAL_SEEK__
#ifndef __RTCFM_STEP_50K__
		channel_freq *= 10; // For ax2073b_cola platform version need in RTC6222 20151222
#endif
	//__wrn("\nChannel_freq= %d\r",channel_freq);

	write_byte = OperationRTC6222_read(0x05);
	write_byte &= (~0x8000);
	OperationRTC6222_write(write_byte,0x05);
	
	write_byte &= (~0x8000);
	OperationRTC6222_write(write_byte,0x03);
	
	OperationRTC6222_write(0x8000|channel_freq,0x03);


	do
	{
		delay_5ms(6);
		read_buf = OperationRTC6222_read(0x0A);
		timeout++;
	} while ((!(0x4000 & read_buf)) && (timeout < 200));

	write_byte = channel_freq & (~0x8000);
	OperationRTC6222_write(write_byte,0x03);
	
	while((0x4000 & OperationRTC6222_read(0x0A)) && (timeout < 200));

	return 0;

#endif //__INTERNAL_SEEK__
}

__s32 fm_play_rtc6222(__s32 curFreq)
{
	RTC6222_set_freq(curFreq/10);
	return EPDK_OK;
}

void rtc6222_set_vol(u8 volume) // @"rtc6222_RX"
{
	__u16 write_byte = 0x4000|((volume)&0x0f);//from uplever FM_Volumn:1~16 so need -1
	OperationRTC6222_write(write_byte,0x02);
}


void rtc6222_off(void) // @"rtc6222_RX"
{
	OperationRTC6222_write(0x16AA,0x00);
}


u8 rtc6222_seek(__u16 channel_freq) // @"rtc6222_RX"
{
	//u16 read_buf;
	__u8 RSSIValue= 0;
	__u8 Status0 = 0;
	__u16 Reg_B = 0x0000;
	__u16 Reg_C = 0x0000;

#ifndef __RTCFM_STEP_50K__
	channel_freq *= 10;
#endif

#ifdef __RTCFM_STEP_50K_CHOOSE_BEST_FROM_TW0__
	__u16 IF_Shift_1 = 0, IF_Shift_2 = 0;
	__u8 Status1 =0;
	__u8 shiftindex=0; //请看函数头的返回值说明，0表示当前上层传给底层的頻点，1表示上层传给底层的頻点再+50k。在搜到台时来告诉上层存台和放音选择正确的頻点。
#endif
	
	Status0 = RTC6222_set_freq(channel_freq);  // Check Seek_States is 0 ornot by Danny 20150702 

	if(Status0) //Get one ture Channnel
	{
    #ifdef __RTCFM_STEP_50K_CHOOSE_BEST_FROM_TW0__
		if((0x0040 & OperationRTC6222_read(0x0E)) != 0)
		{
			IF_Shift_1 = (~(0x003F & OperationRTC6222_read(0x0E))+1) &0x003F; //2's complement
		}
		else
		{
			IF_Shift_1 = ((0x007F & OperationRTC6222_read(0x0E))); //2's complement
		}		 
		Status1 = RTC6222_set_freq(channel_freq+5); 	  

		if(Status1)
		{
			if((0x0040 & OperationRTC6222_read(0x0E)) != 0)
			{
				IF_Shift_2 = (~(0x003F & OperationRTC6222_read(0x0E))+1) &0x003F; //2's complement
			}
			else
			{
				IF_Shift_2 = ((0x007F & OperationRTC6222_read(0x0E))); //2's complement
			}			 
			if(IF_Shift_1 < IF_Shift_2)
			{
				RTC6222_set_freq(channel_freq);
				shiftindex = 0;
			}
			else
			{
				RTC6222_set_freq(channel_freq+5);
				shiftindex = 1;
			}			 
		}
		else
		{
			RTC6222_set_freq(channel_freq); //Back Channel 50KHz for Return 1
		}
		RSSIValue = (0x00FF & (OperationRTC6222_read(0x0B)));
		Reg_C = (OperationRTC6222_read(0x0C));
		return 1+shiftindex;
		
    #else
		
		return 1;
		
    #endif    
	}
	else
	{
		return 0;
	}

		Reg_B = (OperationRTC6222_read(0x0B));
		Reg_C = (OperationRTC6222_read(0x0C));
		//Reg_D = (OperationRTC6222_read(0x0D));
		//Reg_E = (OperationRTC6222_read(0x0E));
		RSSIValue = (0x00FF & Reg_B);		
		//Seek_Status = ((0xFC00 & OperationRTC6222_read(0x0B)) >> 10); //add by Danny 20150702
	   
}


__s32 fm_manual_search_rtc6222(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 10);

	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ / 10) || (temp < FM_SEARCH_CHN_MIN_FREQ / 10))
		{
			return EPDK_FAIL;
		}

		returnValue = rtc6222_seek(temp);

		if(returnValue)
		{
			return temp * 10;
		}
		else
		{
			return (temp * 10) | 0xff000000;
		}

		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			temp--;
		}
		else//DRV_FM_SEARCH_UP
		{
			temp++;
		}
	}

	return EPDK_FAIL;
}


__s32  fm_auto_search_rtc6222(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 10);
	DebugPrintf(" #############freq = %d,  returnValue = %d,  temp = %d\n", freq, returnValue, temp);
	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ / 10) || (temp < FM_SEARCH_CHN_MIN_FREQ / 10))
		{
			__wrn(" temp = %d\n", temp);
			return EPDK_FAIL;
		}
	
		returnValue = rtc6222_seek(temp);
		__wrn(" freq = %d\n,returnValue = %d\n", temp, returnValue);
		if(returnValue)
		{
			__wrn(" temp = %d\n", temp);
			return temp * 10;
		}
		else
		{
			__wrn(" (temp * 100)|0xff000000 = 0x%x\n", (temp * 10) | 0xff000000);
			return (temp * 10) | 0xff000000;
		}

		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			temp--;
			__wrn(" temp = %d\n", temp);
		}
		else
		{
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
__s32 fm_area_choose_rtc6222(__s32 area, void *pbuffer)
{
	__drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

	if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
	{
		//tea5767_info.area_select = 1;
		max_freq = FM_SEARCH_JAP_MAX_FREQ / 10;
		min_freq = FM_SEARCH_JAP_MIN_FREQ / 10;
		freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
		freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
	}
	else if(area == DRV_FM_AREA_CHN_US_EUP)
	{
		// tea5767_info.area_select = 0;
		max_freq = FM_SEARCH_CHN_MAX_FREQ / 10;
		min_freq = FM_SEARCH_CHN_MIN_FREQ / 10;
		freq_range->fm_area_min_freq = FM_SEARCH_CHN_MIN_FREQ;
		freq_range->fm_area_max_freq = FM_SEARCH_CHN_MAX_FREQ;
	}
	else
	{
		return EPDK_FAIL;
	}
	return EPDK_OK;
}


__s32 fm_mute_rtc6222(__bool is_mute)
{
	return EPDK_OK;
}


////////////////////////////////////////////

u8 rtc6222_online(void) // @"rtc6222_RX"
{
	__u16 fm_id;
	__u16 chip_id;
	
	fm_id = OperationRTC6222_read(0x00);
	chip_id = OperationRTC6222_read(0x01);
	DebugPrintf("#########666666666666666################fm_id==0X%x,chip_id==0X%x####\n",fm_id,chip_id);
	if(0x1688 == fm_id && 0x18c0 == chip_id)
	{
		rtc6222_init();
		return 1;
	}
	else
	{
		return 0;
	}
}




#endif
