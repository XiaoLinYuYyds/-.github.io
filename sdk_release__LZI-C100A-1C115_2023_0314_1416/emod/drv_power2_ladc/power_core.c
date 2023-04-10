/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Power Driver
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : power_core.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-30
* Descript: no pmu power dirver.
* Update  : date                auther      ver     notes
*           2011-4-30 10:07:53  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "drv_power_i.h"
typedef struct
{
	__u32	*pValList, size_pValList;
	__u32	sampleCnt, timerCnt, timerCnt_top;
} S_ContainerAdcSample;

//以下定义为AD采样服务
#define POWER_ADC_SPMPLE_NUM 		6//6次AD采样取平均
static __u32 	g_adc_val[POWER_ADC_SPMPLE_NUM] = {0},
    g_adc_val_fast[3] = {0};
__s32 g_adc_exp = 1080;

static __u32 g_adc_val_tp[POWER_ADC_SPMPLE_NUM] = {0};
static __s32 g_cur_adc_sample_num_tp = 0;
__s32 g_adc_exp_tp = 961;

static S_ContainerAdcSample g_sContainerAdcSample[m_eDrvPowerFuelguageIdx_total] =
{
	{
		g_adc_val_fast, sizeof(g_adc_val_fast) / sizeof(g_adc_val_fast[0]),
		0, 0, 1
	},
	{
		g_adc_val, sizeof(g_adc_val) / sizeof(g_adc_val[0]),
		0, 0, 7
	}
};


static __u32 CalcVoltage(__u32 ADCVal)
{
	return ADCVal * 2000 / 63 * g_adc_exp / 1000.0 * (BAT_HIGHT_RESISITOR + BAT_LOW_RESISITOR) / BAT_LOW_RESISITOR;
}

static __u32 CalcVoltageTP(__u32 ADCVal)
{
	return ADCVal * g_adc_exp_tp / 1000.0 * (BAT_HIGHT_RESISITOR + BAT_LOW_RESISITOR) / BAT_LOW_RESISITOR;
}

static __u32 PowerGetADSampleVal(void)
{
	__s32 cnt;
	volatile __u32          *tmpAddr;
	volatile __u32           tmpVal;
	//enable LADC
	tmpAddr = LRADC_CRTL_ADDR;
	tmpVal = *tmpAddr;
	tmpVal = tmpVal | 0x01;
	*tmpAddr = tmpVal;
	//wait data ready
	tmpVal = LRADC_DATA_VALUE;
	tmpAddr = LRADC_INTSTAT_ADDR;
	*tmpAddr |= 0x1;
	cnt = 0;

	while(1)
	{
		if((*tmpAddr & 0x1) == 1)
		{
			//data ready
			break;
		}

		if(cnt++ > 2)
		{
			break;
		}

		esKRNL_TimeDly(10);
	}

	if((*tmpAddr & 0x1) == 1)
	{
		//获取ADC采样值
		tmpVal = LRADC_DATA_VALUE;
	}
	else
	{
		tmpVal = 0x3f;
	}

	//__msg("********tmpVal=%d********\n", tmpVal);
	tmpVal = tmpVal & 0x3f;
	return tmpVal;
}

static __s32 PowerGetADSampleValTP(void)
{
	__u32 ret;
	__s32 data[32];
	__s32 average;
	__s32 i;
	__s32 cnt;
	//RK_SelectChannel(RK_CHANNEL_Y1);
	//RK_ClearFifo();
	// 读取数据
	cnt = 0;

	while(1)
	{
		ret = RK_QueryIntPending();

		if(ret & RK_DATA_PENDING)
		{
			RK_GetData(data, sizeof(data) / sizeof(data[0]));
			RK_ClearIntPending(ret);
			RK_ClearFifo();
			break;
		}

		if(cnt++ > 2)
		{
			RK_ClearIntPending(ret);
			RK_ClearFifo();
			break;
		}

		esKRNL_TimeDly(10);
	}

	if(!(ret & RK_DATA_PENDING))
	{
		for(i = sizeof(data) / sizeof(data[0]) - 1 ; i >= 0 ; i--)
		{
			data[i] = 0x7ff;
		}
	}

	average = 0;

	for(i = sizeof(data) / sizeof(data[0]) - 1 ; i >= 0 ; i--)
	{
		average += data[i];
		//        __msg("data[i]=%d \n", data[i]);
	}

	average /= sizeof(data) / sizeof(data[0]);
	return average;
}

static __u32 PowerGetAverage(__u32 *val, __s32 num)
{
	__u32 total;
	__s32 i;

	if(num <= 0)
	{
		return 0;
	}

	total = 0;

	for(i = 0 ; i < num ; i++)
	{
		total += val[i];
	}

	return total / num;
}

__s8 get_battery_level(__u32 Voltage, __u32 *level)
{
	if(Voltage < 3250)	// 会出现白屏
	{
		*level = NO_PMU_BATTERY_LEVEL0;
		return -2;
	}
	else if(Voltage < 3350)//低电关机电压
	{
		*level = NO_PMU_BATTERY_LEVEL0;
		return -1;
	}
	else if(Voltage <= 3580)
	{
		*level = NO_PMU_BATTERY_LEVEL0;
	}
	else if(Voltage <= 3650)
	{
		//电池电量为20%
		*level = NO_PMU_BATTERY_LEVEL1;
	}
	else if(Voltage <= 3750)
	{
		//电池电量为40%
		*level = NO_PMU_BATTERY_LEVEL2;
	}
	else if(Voltage <= 3850)
	{
		//电池电量为60%
		*level = NO_PMU_BATTERY_LEVEL3;
	}
	else if(Voltage <= 3950)
	{
		//电池电量为80%
		*level = NO_PMU_BATTERY_LEVEL4;
	}
	else if(Voltage <= 4150)
	{
		//电池电量为100%
		*level = NO_PMU_BATTERY_LEVEL5;
	}
	else
	{
		//电池电量为100%
		*level = NO_PMU_BATTERY_LEVEL5;
		return 1;
	}

	return 0;
}

__s8 get_battery_level_charging(__u32 Voltage, __u32 *level)
{
	if(Voltage < 3250)	// 会出现白屏
	{
		*level = NO_PMU_BATTERY_LEVEL0;
		return -2;
	}
	else if(Voltage < 3350) //有充电器时，低电关机电压
	{
		*level = NO_PMU_BATTERY_LEVEL0;
		return -1;
	}
	else if(Voltage <= 3580)
	{
		*level = NO_PMU_BATTERY_LEVEL0;
	}
	else if(Voltage <= 3650)
	{
		//电池电量为20%
		*level = NO_PMU_BATTERY_LEVEL0;
	}
	else if(Voltage <= 3750)
	{
		//电池电量为40%
		*level = NO_PMU_BATTERY_LEVEL1;
	}
	else if(Voltage <= 3850)
	{
		//电池电量为60%
		*level = NO_PMU_BATTERY_LEVEL2;
	}
	else if(Voltage <= 3950)
	{
		//电池电量为80%
		*level = NO_PMU_BATTERY_LEVEL3;
	}
	else if(Voltage <= 4150)
	{
		//电池电量为100%
		*level = NO_PMU_BATTERY_LEVEL4;
	}
	else
	{
		//电池电量为100%
		*level = NO_PMU_BATTERY_LEVEL5;
		return 1;
	}

	return 0;
}

__u32 GetSampleVoltage(S_ContainerAdcSample *sContainerAdcSample, __u32 cur_adc_val)
{
	if(sContainerAdcSample->pValList == NULL)
	{
		return ~0;
	}

	if(sContainerAdcSample->sampleCnt < sContainerAdcSample->size_pValList)
	{
		sContainerAdcSample->pValList[sContainerAdcSample->sampleCnt++] = cur_adc_val;
	}
	else
	{
		__s32 i;

		for(i = 0 ; i < sContainerAdcSample->size_pValList - 1 ; i++)
		{
			sContainerAdcSample->pValList[i] = sContainerAdcSample->pValList[i + 1];
		}

		sContainerAdcSample->pValList[sContainerAdcSample->size_pValList - 1] = cur_adc_val;
	}

	{
		__u32 tmpVal, VoltageAD;
		tmpVal = PowerGetAverage(sContainerAdcSample->pValList, sContainerAdcSample->sampleCnt);
		__msg("********tmpValAD=%d********\n", tmpVal);
		//计算实际电池电压(mV)
		VoltageAD = CalcVoltage(tmpVal);
//		__wrn("********VoltageAD=%d********\n", VoltageAD);
		return VoltageAD;
	}
}

void GetSampleVoltageLevel(__drv_power_fuelguage_t *pFuelguage, __u32 voltage)
{	
	__msg("^^^^^%d^^^^^^^\n",voltage);
	if((!pFuelguage) || (voltage == (~0)))
	{
		return;
	}
	__msg("^^^^^%d^^^^^^^\n",voltage);
	__msg("^^^^^%d^^^^^^^\n",voltage);
	pFuelguage->bat_power = voltage;
	pFuelguage->is_low_power = 0;
	pFuelguage->is_full_charge = 0;
	//voltage >>= 1;
	{
		__u8 charge_status = 0;
		__s8 ret;

		if(hChargeStatus)
		{
			charge_status = esPINS_ReadPinData(hChargeStatus, NULL);
		}

		if(charge_status)
		{
			ret = get_battery_level_charging(voltage, &pFuelguage->rest_vol);
		}
		else
		{
			ret = get_battery_level(voltage, &pFuelguage->rest_vol);
		}

		switch(ret)
		{
			case -2 :
				pFuelguage->is_low_power = LEVEL_LowBattery_Limit;
				break;

			case -1 :
				pFuelguage->is_low_power = LEVEL_LowBattery_Normal;
				break;

			case 1 :
				pFuelguage->is_full_charge = 1;
				break;

			default :
				break;
		}
	}
	//剩余时间暂时不支持
	pFuelguage->rest_time = 0;
	__msg("PowerTask, rest vol = %d\n", pFuelguage->rest_vol);
	__msg("^^^^^%d^^^^^^^\n",voltage);
}

void PowerTask(void *parg)
{
	__drv_power_fuelguage_t 			*pFuelguage;
	__u32                    					cur_adc_val;
	__u32                    					voltageAD;
	E_DRV_POWRE_FUELGUAGE_IDX		eDrvPowerFuelguageIdx;
	__bool							flagGetSampleEnable;
	static __bool						s_flagGetSampleFirst = 1;

	//return ;
	if(parg == NULL)
	{
		__wrn("invalid argument for power timer task\n");
		return ;
	}

	__msg("********before PowerGetADSampleVal*************\n");
	cur_adc_val = PowerGetADSampleVal();
	__msg("********after PowerGetADSampleVal*************\n");

	for(pFuelguage = (__drv_power_fuelguage_t *)parg,
	    eDrvPowerFuelguageIdx = m_eDrvPowerFuelguageIdx_fast;
	    eDrvPowerFuelguageIdx < m_eDrvPowerFuelguageIdx_total;
	    eDrvPowerFuelguageIdx++, pFuelguage++)
	{
		if(++g_sContainerAdcSample[eDrvPowerFuelguageIdx].timerCnt >= g_sContainerAdcSample[eDrvPowerFuelguageIdx].timerCnt_top)
		{
			g_sContainerAdcSample[eDrvPowerFuelguageIdx].timerCnt = 0;
			flagGetSampleEnable = 1;
		}

		__msg("------------ eDrvPowerFuelguageIdx=%d : -------------\n", eDrvPowerFuelguageIdx);
		__msg("timerCnt_top=%d,timerCnt=%d\n",
		      g_sContainerAdcSample[eDrvPowerFuelguageIdx].timerCnt_top, g_sContainerAdcSample[eDrvPowerFuelguageIdx].timerCnt);

		if(flagGetSampleEnable || s_flagGetSampleFirst)
		{
			flagGetSampleEnable = 0;
			voltageAD = GetSampleVoltage(&g_sContainerAdcSample[eDrvPowerFuelguageIdx], cur_adc_val);
			GetSampleVoltageLevel(pFuelguage, voltageAD);
			__msg("sampleCnt[%d]=%d,voltageAD=%d,bat_power=%d\n\n\n",
			      eDrvPowerFuelguageIdx, g_sContainerAdcSample[eDrvPowerFuelguageIdx].sampleCnt, voltageAD, pFuelguage->bat_power);
			__msg_jjl("sampleCnt[%d]=%d,voltageAD=%d,bat_power=%d\n\n\n",
      	eDrvPowerFuelguageIdx, g_sContainerAdcSample[eDrvPowerFuelguageIdx].sampleCnt, voltageAD, pFuelguage->bat_power);
		}
	}

	s_flagGetSampleFirst = 0;
}

