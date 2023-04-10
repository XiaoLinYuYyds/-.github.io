/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_power.h
* By      	: Andy.zhang
* Func		: power interface
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "..\\functions_i.h"

#define 	BATTERY_AVER_CNT 	20

/*
 * 剩余电量level, 判断低点
 */
static __u32 g_battery[BATTERY_AVER_CNT];
static __u32 g_battery_cnt = 0;

static __u32 speaker_open_status;

//电压级别 DRV_POWER_CMD_BATADC_GET
__s32 dsk_power_get_battery_level(__u8 *p_level)
{
	ES_FILE 						*power;
	power_level_e					vol_level;
	__drv_power_fuelguage_t 		info;
	int 							rest_vol;
	power = eLIBs_fopen("b:\\HID\\POWER", "w");

	if(!power)
	{
		__msg(" Power Driver cannot be open !\n");
		*p_level = DSK_POWER_LEVEL_0;
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_FUELGUAGE_FAST, 0, &info);
	eLIBs_fclose(power);
	rest_vol = info.rest_vol;
	__msg("info.rest_vol=%d\n", info.rest_vol);

	if(rest_vol >= 100)
	{
		vol_level = DSK_POWER_LEVEL_5;
	}
	else if(rest_vol >= 80)
	{
		vol_level = DSK_POWER_LEVEL_4;
	}
	else if(rest_vol >= 60)
	{
		vol_level = DSK_POWER_LEVEL_3;
	}
	else if(rest_vol >= 40)
	{
		vol_level = DSK_POWER_LEVEL_2;
	}
	else if(rest_vol >= 20)
	{
		vol_level = DSK_POWER_LEVEL_1;
	}
	else if(rest_vol >= 0)
	{
		vol_level = DSK_POWER_LEVEL_0;
	}
	else
	{
		vol_level = DSK_POWER_LEVEL_0;
	}

	*p_level = vol_level | (info.is_low_power << 6);
	return info.bat_power;
}

/*
 * 工作电压level,(工具栏显示电池电量级别)
 */
//电量级别 DRV_POWER_CMD_GET_FUELGUAGE
__s32 dsk_power_get_voltage_level(power_level_e *p_level)
{
	ES_FILE  						*power;
	power_level_e 					vol_level;
	__drv_power_fuelguage_t  		info;
	int 							rest_vol;
	power = eLIBs_fopen("b:\\HID\\POWER", "w");

	if(!power)
	{
		__msg(" Power Driver cannot be open !\n");
		*p_level = DSK_POWER_LEVEL_0;
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_FUELGUAGE, 0, &info);
	eLIBs_fclose(power);
	rest_vol = info.rest_vol;
	__msg("info.rest_vol=%d\n", info.rest_vol);

	if(rest_vol >= 100)
	{
		vol_level = DSK_POWER_LEVEL_5;
	}
	else if(rest_vol >= 80)
	{
		vol_level = DSK_POWER_LEVEL_4;
	}
	else if(rest_vol >= 60)
	{
		vol_level = DSK_POWER_LEVEL_3;
	}
	else if(rest_vol >= 40)
	{
		vol_level = DSK_POWER_LEVEL_2;
	}
	else if(rest_vol >= 20)
	{
		vol_level = DSK_POWER_LEVEL_1;
	}
	else if(rest_vol >= 0)
	{
		vol_level = DSK_POWER_LEVEL_0;
	}
	else
	{
		vol_level = DSK_POWER_LEVEL_0;
	}

	*p_level = vol_level;
	return EPDK_OK;
}

__bool dsk_power_is_low(void)
{
	ES_FILE *power;
	__s32	 ret;
	power_level_e level;
	__drv_power_battery_status_t3 bat_sta;
	__drv_power_fuelguage_t  		info;
	static __s32 low_power_cnt = 0;
	power = eLIBs_fopen("b:\\HID\\POWER", "w");

	if(!power)
	{
		__msg(" Power Driver cannot be open !\n");
		return EPDK_FALSE;
	}

	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_FUELGUAGE, 0, &info);
	eLIBs_memset(&bat_sta, 0, sizeof(__drv_power_battery_status_t3));
	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_DCIN, 0, &bat_sta);
	eLIBs_fclose(power);
	__msg("***********bat_sta.charge_status=%d***********\n", bat_sta.charge_status);
	__msg("***********info.is_low_power=%d, ***********\n", info.is_low_power);
	__msg("***********info.is_full_charge=%d***********\n", info.is_full_charge);
	__msg("***********info.bat_power=%d***********\n", info.bat_power);
	__msg("low_power_cnt=%d\n", low_power_cnt);
#if 1

	if(bat_sta.charge_status)
	{
		low_power_cnt = 0;
	}
	else
#endif
	{
		switch(info.is_low_power)
		{
			case LEVEL_LowBattery_Limit :
				if(++low_power_cnt >= 2)
				{
					return EPDK_TRUE;
				}

				break;

			case LEVEL_LowBattery_Normal :
				if(++low_power_cnt >= 10)
				{
					return EPDK_TRUE;
				}

				break;

			default :
				low_power_cnt = 0;
				return EPDK_FALSE;
		}

		__msg_jjl("LowCnt=%d, Bat=%d\n", low_power_cnt, info.bat_power);
	}

	return EPDK_FALSE;
}

__s32 dsk_power_set_off_time(__u32 time)
{
	//	__gui_msg_t msg;
	//	__s32 ret;
	//
	//	g_memset(&msg, 0, sizeof(__gui_msg_t));
	//	msg.id 			= DSK_MSG_SET_AUTO_OFF;
	//	msg.h_srcwin 	= NULL;
	//	msg.h_deswin	= GUI_WinGetHandFromName(APP_DESKTOP_MWIN);
	//	msg.dwAddData1	= (__u32)time;
	//	msg.dwAddData2	= (__u32)0;
	//
	//	ret = GUI_SendNotifyMessage(&msg);
	//
	//	return ret;
	return EPDK_OK;
}

__s32 dsk_power_get_battary_state(power_state_e *state)
{
	/*
	ES_FILE *power;
	__drv_power_fuelguage_t  	info;

	power = eLIBs_fopen("b:\\HID\\POWER", "w");
	if( !power )
	{
	__msg(" Power Driver cannot be open !\n");
	return EPDK_FALSE;
	}

	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_FUELGUAGE, 0, &info);

	if(info.charge_status & 0x2)
	{
	if(info.charge_status & 0x4)
	{
	if(info.charge_status & 0x1)
	{
	*state = DSK_POWER_BATTARY_CHARGE;
	}
	else
	{
	*state = DSK_POWER_BATTARY_FULL;
	}
	}
	else
	{
	*state = DSK_POWER_BATTARY_FULL;
	}
	}
	else
	{
	*state = DSK_POWER_BATTARY_FULL;
	}

	eLIBs_fclose( power );
	*/
	return EPDK_OK;
}

//__s32 dsk_power_off(void);
//{
//	__gui_msg_t msg;
//
//	msg.id 			= DSK_MSG_POWER_OFF;
//	msg.h_srcwin 	= NULL;
//	msg.h_deswin	= GUI_WinGetHandFromName(APP_DESKTOP_MWIN);
//	msg.dwAddData1	= 0;
//	msg.dwAddData2  = 0;
//	msg.p_arg		= 0;
//
//	return GUI_SendNotifyMessage(&msg);
//}



void dsk_speaker_turnoff(void)
{
	ES_FILE *pfile;
	__inf("speaker control off.....\n");
	pfile = eLIBs_fopen("b:\\HID\\POWER", "r+");

	if(pfile == 0)
	{
		__msg("b:\\HID\\POWER cannot open \n");
		return;
	}

	//Get the previous status
	speaker_open_status = eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_GET, POWER_EPT_3, 0);
	//Pull Low
	eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_3, (void *)((POWER_EPT_MOS_N) | (POWER_EPT_GATE_ON)));
	eLIBs_fclose(pfile);
}

void dsk_speaker_turnon(void)
{
	ES_FILE *pfile;
	__inf("speaker control On.....\n");
	pfile = eLIBs_fopen("b:\\HID\\POWER", "r+");

	if(pfile == 0)
	{
		__err("b:\\HID\\POWER cannot open \n");
		return;
	}

	// Get the previous status
	speaker_open_status = eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_GET, POWER_EPT_3, 0);
	//Pull High
	eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_3, (void *)((POWER_EPT_MOS_P) | (POWER_EPT_GATE_ON)));
	eLIBs_fclose(pfile);
}

void dsk_speaker_resume(void)
{
	ES_FILE *pfile;
	__inf("speaker control resume.....\n");
	pfile = eLIBs_fopen("b:\\HID\\POWER", "r+");

	if(pfile == 0)
	{
		__msg("b:\\HID\\POWER cannot open \n");
		return;
	}

	eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_3, (void *)speaker_open_status);     //return the previous status
	eLIBs_fclose(pfile);
}

__bool dsk_get_charge_state(void)
{
	ES_FILE *power;
	__drv_power_battery_status_t3 bat_sta;
	//return EPDK_TRUE;//112350
	power = eLIBs_fopen("b:\\HID\\POWER", "w");

	if(!power)
	{
		__msg(" Power Driver cannot be open !\n");
		return EPDK_FALSE;
	}

	eLIBs_memset(&bat_sta, 0, sizeof(__drv_power_battery_status_t3));
	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_DCIN, 0, &bat_sta);
	eLIBs_fclose(power);

	if(1 == bat_sta.charge_status)	// 在充电
	{
		__here__;
		return EPDK_TRUE;
	}
	else//充电
	{
		__here__;
		return EPDK_FALSE;
	}
}

__bool dsk_power_is_full_charge(void)
{
	ES_FILE  						*power;
	__drv_power_fuelguage_t  		info;
	power = eLIBs_fopen("b:\\HID\\POWER", "w");

	if(!power)
	{
		__msg(" Power Driver cannot be open !\n");
		return EPDK_FALSE;
	}

	eLIBs_fioctrl(power, DRV_POWER_CMD_GET_FUELGUAGE, 0, &info);
	eLIBs_fclose(power);

	if(info.is_full_charge)
	{
		return EPDK_TRUE;
	}
	else
	{
		return EPDK_FALSE;
	}
}


