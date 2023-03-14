/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_volume.c
* By      	: Andy.zhang
* Func		: voice control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "..\\functions_i.h"

#define AUDIO_DRV_PATH "b:\\AUDIO\\PLAY"



static __bool dsk_volume_gate_on = EPDK_TRUE;
static __s32 dsk_volume = EPDK_TRUE;

static __s32 _set_volume(__s32 volume);
static __s32 _get_volume(void);


static __s32 g_amplifier_status = 0;

#define PIOC_MEM_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)               ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)              ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))

static __s32 get_gpio_status(__u32 port, __u32 port_num)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__s32               ret;
	__u32               tmp1;
	__u32               tmp2;
	//设置为输入
	tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
	tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x07 << tmp1);
	tmp_data |= (0x00 << tmp1);
	*tmp_addr = tmp_data;
	//设置为下拉
	tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
	tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x03 << tmp2);
	tmp_data |= (0x02 << tmp2);
	*tmp_addr = tmp_data;
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	__wrn("tmp_data=0x%x\n", tmp_data);
	ret = (tmp_data & (1 << port_num)) >> port_num;
	return ret;
}

static void pull_down_gpio(__u32 port, __u32 port_num)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	//设置为输出
	tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
	tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x07 << tmp1);
	tmp_data |= (0x01 << tmp1);
	*tmp_addr = tmp_data;
	//设置为下拉
	tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
	tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x03 << tmp2);
	tmp_data |= (0x02 << tmp2);
	*tmp_addr = tmp_data;
	//输出低电平
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	tmp_data &= ~(1 << port_num);
	//tmp_data |=  (1 << port_num);
	*tmp_addr = tmp_data;
}

static void pull_up_gpio(__u32 port, __u32 port_num)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	//设置为输出
	tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
	tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x07 << tmp1);
	tmp_data |= (0x01 << tmp1);
	*tmp_addr = tmp_data;
	//设置为上拉
	tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
	tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x03 << tmp2);
	tmp_data |= (0x01 << tmp2);
	*tmp_addr = tmp_data;
	//输出高电平
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	tmp_data &= ~(1 << port_num);
	tmp_data |= (1 << port_num);
	*tmp_addr = tmp_data;
}


/*
****************************************************************************************************
*Name        : dsk_volume_set_volume
*Prototype   : __s32 dsk_volume_set_volume(__s32 volume)
*Arguments   : volume     input. volume value, ranges AUDIO_DEVICE_VOLUME_MIN~~AUDIO_DEVICE_VOLUME_MAX.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : set the volume.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_set(__s32 volume)	// 设置当前音量
{
	__s32        ret;

	if(!dsk_volume_gate_on)
	{
		return EPDK_FAIL;
	}

	ret = _set_volume(volume);

	if(ret != EPDK_FAIL)
	{
		dsk_volume = volume;
		{
			reg_system_para_t *para;
			para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

			if(para)
			{
				para->volume = volume;
			}
		}
	}

	return ret;
}


/*
****************************************************************************************************
*Name        : dsk_volume_get_volume
*Prototype   : __s32 dsk_volume_get_volume(void)
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : get the curret volume value.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_get(void)		// 当前音量大小
{
	if(!dsk_volume_gate_on)
	{
		return 0;
	}

	return _get_volume();
}



/*
****************************************************************************************************
*Name        : dsk_volume_inc_volume
*Prototype   : __s32 dsk_volume_inc_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_inc(void)		// 增大音量
{
	__s32		volume;
	__s32       ret;

	if(!dsk_volume_gate_on)
	{
		return -1;
	}

	volume = _get_volume();

	if(volume == -1)
	{
		return -1;
	}

	if(volume < AUDIO_DEVICE_VOLUME_MAX)
	{
		volume++;
		ret = _set_volume(volume);

		if(ret != EPDK_FAIL)
		{
			dsk_volume = volume;
			{
				reg_system_para_t *para;
				para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

				if(para)
				{
					para->volume = volume;
				}
			}
			return volume;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return volume;
	}
}

/*
****************************************************************************************************
*Name        : dsk_volume_dec_volume
*Prototype   : __s32 dsk_volume_dec_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_dec(void)		// 减小音量
{
	__s32		volume;
	__s32       ret;

	if(!dsk_volume_gate_on)
	{
		return -1;
	}

	volume = dsk_volume_get();

	if(volume == -1)
	{
		return -1;
	}

	if(volume > AUDIO_DEVICE_VOLUME_MIN)
	{
		volume--;
		ret = _set_volume(volume);

		if(ret != EPDK_FAIL)
		{
			dsk_volume = volume;
			{
				reg_system_para_t *para;
				para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

				if(para)
				{
					para->volume = volume;
				}
			}
			return volume;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return volume;
	}
}



/*
****************************************************************************************************
*Name        : dsk_volume_on
*Prototype   : __s32 dsk_volume_on(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice on.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_on(void)
{
	if(dsk_volume_gate_on)
	{
		return EPDK_OK;
	}

	_set_volume(dsk_volume);
	dsk_volume_gate_on = EPDK_TRUE;
	return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_volume_off
*Prototype   : __s32 dsk_volume_off(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice off(mute).
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_off(void)	           // close voice
{
	if(!dsk_volume_gate_on)
	{
		return EPDK_OK;
	}

	_set_volume(0);                // mute
	dsk_volume_gate_on = EPDK_FALSE;
	return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_volume_is_on
*Prototype   : __bool dsk_volume_is_on( void )
*Arguments   : void
*Return      : the status of the voice, on or off(mute).
*Description : get the status of the voice, on or off(mute).
*Other       :
****************************************************************************************************
*/
__bool dsk_volume_is_on(void)
{
	return 	dsk_volume_gate_on;
}

static __s32 _set_volume(__s32 volume)
{
	ES_FILE		*p_dac;
	__s32        ret;
	__wrn("_set_volume: volume=%d\n", volume);

	if(volume < AUDIO_DEVICE_VOLUME_MIN
	    || volume > AUDIO_DEVICE_VOLUME_MAX)
	{
		return EPDK_FAIL;
	}

	p_dac = eLIBs_fopen(AUDIO_DRV_PATH, "r+");

	if(!p_dac)
	{
		__err("open dac device fail!\n");
		return EPDK_FAIL;
	}

	if(AUDIO_DEVICE_VOLUME_MIN == volume)
	{
		IOCTRL__PA_SHDN__SET_EN();
	}
	else
	{
		if(g_amplifier_status == 0)
		{
			IOCTRL__PA_SHDN__SET_EN();
		}
		else
		{
			IOCTRL__PA_SHDN__SET_DIS();
		}
	}

	if(1)
	{
		__u32 realVolume_max;
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			realVolume_max = para->realVolume_max;
		}
		else
		{
			realVolume_max = 28;
		}

		volume = (__s32)((volume - AUDIO_DEVICE_VOLUME_MIN)
		                 * (realVolume_max - AUDIO_DEVICE_VOLUME_MIN) / (AUDIO_DEVICE_VOLUME_MAX - AUDIO_DEVICE_VOLUME_MIN));
	}

	if(1)
	{
		//音量限制为最高28
		volume = (__s32)((volume - AUDIO_DEVICE_VOLUME_MIN)
		                 * (28 - AUDIO_DEVICE_VOLUME_MIN)
		                 / (AUDIO_DEVICE_VOLUME_MAX - AUDIO_DEVICE_VOLUME_MIN));
	}

	__wrn("_set_volume: really volume=%d\n", volume);
	ret = eLIBs_fioctrl(p_dac, AUDIO_DEV_CMD_SET_VOLUME, volume, 0);
	eLIBs_fclose(p_dac);
	return ret;
}

static __s32 _get_volume(void)
{
	ES_FILE		*p_dac;
	__s32        volume;

	if(1)//音量限制为28最高后，直接注册表读取
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			//            __wrn("_get_volume: volume=%d\n", para->volume);
			return para->volume;
		}
	}

	p_dac = eLIBs_fopen(AUDIO_DRV_PATH, "r+");

	if(!p_dac)
	{
		__err("open dac device fail!\n");
		return -1;
	}

	volume = eLIBs_fioctrl(p_dac, AUDIO_DEV_CMD_GET_VOLUME, 0, 0);
	eLIBs_fclose(p_dac);
	return volume;
}

__s32 dsk_amplifier_onoff(__s32 flag)//设置功放开关
{
	g_amplifier_status = flag;

	if(0 == flag)
	{
		//pull_down_gpio(3, 11);
		IOCTRL__PA_SHDN__SET_EN();
	}
	else
	{
		/*
		if( flag == 2 )
		{
			pull_up_gpio(3, 11);
		}
		*/
		{
			__s32 sta;
			reg_system_para_t *para;
			para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

			if(para && AUDIO_DEVICE_VOLUME_MIN == para->volume) //音量最小
			{
				IOCTRL__PA_SHDN__SET_EN();
			}
			else
			{
				IOCTRL__PA_SHDN__SET_DIS();
			}
		}
		/*
		if( flag == 1 )
		{
			pull_down_gpio(3, 11);
		}
		*/
	}

	//__msg_jjl("dsk_amplifier_onoff(%d)\n", flag);
	return EPDK_OK;
}

__bool dsk_amplifier_is_on(void)//查询功放开关
{
	if(1 == g_amplifier_status)
	{
		return EPDK_TRUE;
	}
	else
	{
		return EPDK_FALSE;
	}
}

