#include "IIC.h"
#include  "Esh_lz_drv.h"

#if 1


//#define __DebugPrintf		eLIBs_printf
#ifndef __DebugPrintf
#define __DebugPrintf(...)
#endif

ES_FILE *mcuI2cHdl = NULL;

#if 1
#define MCU_COPYRIGHT_W_SADDR		0x92
#define MCU_COPYRIGHT_R_SADDR			0x93
static __s32 mcu_Init(void)
{
	//esDEV_Plugout("\\drv\\twi.drv", 0);
	I2C_Init();
	return EPDK_OK;
}

static __s32 mcu_Exit(void)
{
	I2C_End();
	//esDEV_Plugin("\\drv\\twi.drv", 0, 0, 1);
}

static __s32 mcu_iic_write(__u8 *data)
{
	__s32 j;
	__bool ack  = 0;
	__s32 	cpu_sr;

	//ENTER_CRITICAL(cpu_sr);
	I2C_Start();

	if(I2C_I2Ctransbyte(MCU_COPYRIGHT_W_SADDR, EPDK_TRUE))
	{
		ack = 1;

		for(j = 0; j < 4; j++)
		{
			if(!I2C_I2Ctransbyte(data[j], EPDK_TRUE))
			{
				ack = 0;
				break;
			}
		}
	}
	else
	{
		ack = 0;
	}

	I2C_Stop();
	//EXIT_CRITICAL(cpu_sr);

	return EPDK_OK;
}

static __s32 mcu_iic_read(__u8 *data)
{
	__s32 j;
	__bool ack  = 0;
	__s32 	cpu_sr;
	
	//ENTER_CRITICAL(cpu_sr);
	I2C_Start();

	if(I2C_I2Ctransbyte(MCU_COPYRIGHT_W_SADDR, EPDK_TRUE))
	{
		ack = 1;

		if(!I2C_I2Ctransbyte(0x69, EPDK_TRUE))
		{
			ack = 0;
		}
	}
	else
	{
		ack = 0;
	}

	I2C_usDelay(30);
	I2C_Start();

	if(I2C_I2Ctransbyte(MCU_COPYRIGHT_R_SADDR, EPDK_TRUE))
	{
		for(j = 0; j < 4; j++)
		{
			if(j == 3)
			{
				data[j] = I2C_I2Crecebyte(EPDK_FALSE);
			}
			else
			{
				data[j] = I2C_I2Crecebyte(EPDK_TRUE);
			}
		}

		ack = 1;
	}
	else
	{
		ack = 0;
	}

	I2C_Stop();
	//EXIT_CRITICAL(cpu_sr);

	return EPDK_OK;
}

#else

#define MCU_COPYRIGHT_SADDR		(0x92 >> 1 )

__s32 mcu_Init(void)
{
	__s32 ret;
	//use exteral rtc
	mcuI2cHdl = eLIBs_fopen("b:\\BUS\\twi0", "r+");

	if(!mcuI2cHdl)
	{
		__wrn("mcu driver try to open bus\\twi0 failed!\n");
		return EPDK_FAIL;
	}

	ret = eLIBs_fioctrl(mcuI2cHdl, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_100KHZ);

	if(ret == EPDK_OK)
	{
		__msg("set iic clock as 100Khz successfully\n");
	}

	return EPDK_OK;
}

__s32 mcu_Exit(void)
{
	return eLIBs_fclose(mcuI2cHdl);
}

static __s32 mcu_iic_write(__u8 *data)
{
	__twi_dev_para_ex_t  piic;
	eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
	piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
	piic.slave_addr      = MCU_COPYRIGHT_SADDR;
	piic.byte_addr_width = 0;
	piic.byte_addr       = NULL;
	piic.data            = data;
	piic.byte_count      = 4;
	return  eLIBs_fioctrl(mcuI2cHdl, TWI_WRITE_SPEC_RS, 0, &piic);
}

static __s32 mcu_iic_read(__u8 *data)
{
	__twi_dev_para_ex_t  piic;
	__u8 buf = 0x69;
	eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
	piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
	piic.slave_addr      = MCU_COPYRIGHT_SADDR;
	piic.byte_addr_width = 1;
	piic.byte_addr       = &buf;
	piic.data            = data;
	piic.byte_count      = 4;
	return  eLIBs_fioctrl(mcuI2cHdl, TWI_READ_SPEC_RS, 0, (void *)&piic);
}

#endif

#ifndef __SIZEOF
#define __SIZEOF(array)		(sizeof(array)/sizeof(array[0]))
#endif
static __s32 __shell_twi_copyright(void)
{
	__u8   idx;
	__s32 result;
	__u32 random_num;
	__u32 mcuReturnNum;
	const pfnGetCheckForLZDRV	c_pfnGetCheckForLZDRV[] =
	{
		shell_get_copyright,
	};
	__u32 copyCheckNum[__SIZEOF(c_pfnGetCheckForLZDRV)];

	do
	{
		random_num = eLIBs_ramdom(0xffffffff);
	}
	while((random_num & 0x000000ff) == 0x69);

	//random_num = 0xd2c3b4a5;
	__DebugPrintf("\n--------random:%x--", random_num);
	esKRNL_TimeDly(16);
	result = mcu_iic_write((__u8 *)(&random_num));
	/*
	if(result != EPDK_OK)
	{
		return EPDK_FAIL;
	}
	*/

	for(idx = 0; idx < __SIZEOF(c_pfnGetCheckForLZDRV); idx++)
	{
		copyCheckNum[idx] = c_pfnGetCheckForLZDRV[idx]((__u8 *)(&random_num));
		__DebugPrintf("\n--------copyCheckNum[%d]:%x--", idx, copyCheckNum[idx]);
	}

	esKRNL_TimeDly(3);	//esKRNL_TimeDly(2);
	result = mcu_iic_read((__u8 *)(&mcuReturnNum));
	__DebugPrintf("\n--------Return:%x--\n", mcuReturnNum);

	if(result != EPDK_OK)
	{
		return EPDK_FAIL;
	}

	for(idx = 0; idx < __SIZEOF(c_pfnGetCheckForLZDRV); idx++)
	{
		if(mcuReturnNum == copyCheckNum[idx])
		{
			return idx;
		}
	}

	return EPDK_FAIL;
}

__s32 LzDrvChk(void)
{
	__u32 cpu_clk = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL);
	__s32 checkCnt, checkResult;

	esPWM_LockCpuFreq();
	esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, 96*1000*1000);
	
	mcu_Init();
	__shell_twi_copyright();

	for(checkCnt = 0; checkCnt < 20; checkCnt++)
	{
		checkResult = __shell_twi_copyright();

		if(checkResult != EPDK_FAIL)
		{
			break;
		}
	}

	mcu_Exit();

	esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, cpu_clk);
	esPWM_UnlockCpuFreq();

	{
		char *pstrChkResult[] = {"523"};
		eLIBs_printf("**** LzDrvChk(%d, %s%s) ****\n", 
			checkCnt, 
			(checkResult == EPDK_FAIL)?"FAIL":"OK:", 
			(checkResult == EPDK_FAIL)?"":pstrChkResult[checkResult]);
	}
	
	if(checkResult == EPDK_FAIL)
	{
		return 0;
	}

	return 1;
}


#endif
