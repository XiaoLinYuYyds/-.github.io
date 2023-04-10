#include "epdk.h"

#include "iic_read_write.h"
#include  "drv_fm_i.h"
#if SP_SimIIC
#include "IIC.h"
#endif

#if (FM_MODULE_TYPE & (FM_MODULE_TYPE_RDA5807M |FM_MODULE_TYPE_RTC6207M))

#if 0
__bool OperationRDAFM_2w(__u8 rw,  __u8 *data, __s32 n_byte)
{
	__iic_dev_para_t   piic;        //要写入TEA5767的数据

	//	return 1;
	if(rw == 1)
	{
		__u8 temp[2];
		piic.data = temp;
		piic.byte_addr = 0x0a;
		piic.byte_count = 2;
		piic.baddr_count = 0;		//
		eLIBs_fioctrl(fiic, IIC_READ_DATA_SPEC, 0x22 >> 1, &piic);
		data[0] = piic.data[0];
		data[1] = piic.data[1];
		//esKRNL_TimeDly(30);
		piic.data = temp;
		piic.byte_addr = 0x0b;
		piic.byte_count = 2;
		piic.baddr_count = 0;		//
		eLIBs_fioctrl(fiic, IIC_READ_DATA_SPEC, 0x22 >> 1, &piic);
		data[2] = piic.data[0];
		data[3] = piic.data[1];
	}
	else
	{
		piic.data = data;
		piic.byte_addr = 0xff;
		piic.byte_count = n_byte;
		piic.baddr_count = 0;		//
		eLIBs_fioctrl(fiic, IIC_WRITE_DATA_SPEC, 0x20 >> 1, &piic);
	}

	return 1;
}
#else
__bool OperationRDAFM_2w_old(__u8 rw,  __u8 *data, __s32 n_byte)
{
	__twi_dev_para_ex_t  piic;        //要写入TEA5767的数据
	__s32 ret;
	__u8 addr = 0xff;
	eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));

	//	return 1;
	if(rw == 1)
	{
#if 0
		__u8 temp[2];
		piic.data = temp;
		piic.byte_addr = 0x0a;
		piic.byte_count = 2;
		piic.baddr_count = 0;		//
		eLIBs_fioctrl(fiic, IIC_READ_DATA_SPEC, 0x21 >> 1, &piic);
		data[0] = piic.data[0];
		data[1] = piic.data[1];
		//esKRNL_TimeDly(30);
		piic.data = temp;
		piic.byte_addr = 0x0b;
		piic.byte_count = 2;
		piic.baddr_count = 0;		//
		eLIBs_fioctrl(fiic, IIC_READ_DATA_SPEC, 0x21 >> 1, &piic);
		data[2] = piic.data[0];
		data[3] = piic.data[1];
#else
		/*
		piic.data = data;
		piic.byte_addr = 0xff;
		piic.byte_count = n_byte;
		piic.baddr_count = 0;		//

		eLIBs_fioctrl(fiic, IIC_READ_DATA_SPEC, 0x21>> 1, &piic);
		*/
		piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
		piic.slave_addr 	 = (0x21 >> 1);
		piic.byte_addr_width = 0;
		piic.byte_addr		 = &addr;
		piic.data			 = data;
		piic.byte_count 	 = n_byte;
		ret = eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);

		if(ret != EPDK_OK)
		{
			__wrn("fm i2c read error~\n");
		}

		return ret;
#endif
	}
	else
	{
		/*
		piic.data = data;
		piic.byte_addr = 0xff;
		piic.byte_count = n_byte;
		piic.baddr_count = 0;		//
		eLIBs_fioctrl(fiic, IIC_WRITE_DATA_SPEC, 0x20>> 1, &piic);
		*/
		piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
		piic.slave_addr 	 = (0x20 >> 1);
		piic.byte_addr_width = 0;
		piic.byte_addr		 = &addr;
		piic.data			 = data;
		piic.byte_count 	 = n_byte;
		ret = eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, (void *)&piic);

		if(ret != EPDK_OK)
		{
			__wrn("fm i2c write error~\n");
		}

		return ret;
	}

	return 1;
}

__bool OperationRDAFM_2w(__u8 rw, __u16 dev_addr, __u8 *data, __s32 n_byte)
{
#if SP_SimIIC
	{
		if(rw == 1)
		{
			// 读
			I2C_Start();
			I2C_I2Ctransbyte(dev_addr, EPDK_TRUE);
			n_byte--;

			while(n_byte--)
			{
				*data++ = I2C_I2Crecebyte(EPDK_TRUE);
			}

			*data = I2C_I2Crecebyte(EPDK_FALSE);
			I2C_Stop();
		}
		else
		{
			// 写
			I2C_Start();
			I2C_I2Ctransbyte(dev_addr , EPDK_TRUE);

			while(n_byte--)
			{
				I2C_I2Ctransbyte(*data++ , EPDK_TRUE);
			}

			I2C_Stop();
		}

		return EPDK_OK;
	}
#else
	switch(dev_addr)
	{
		case IIC_CHIP__RDA5807_R:
		case IIC_CHIP__RDA5807_W:
			if(g_flagFmIicFail & FM_MODULE_TYPE_RDA5807M)
			{
				__msg("OperationRDAFM_2w():rda5807m fail.\n");
				return EPDK_FAIL;
			}
			break;
		case IIC_CHIP__RTC6207_R:
		case IIC_CHIP__RTC6207_W:
			if(g_flagFmIicFail & FM_MODULE_TYPE_RTC6207M)
			{
				__msg("OperationRDAFM_2w():rtc6207m fail.\n");
				return EPDK_FAIL;
			}
			break;
		default : return EPDK_FAIL;
	}

	{
		__twi_dev_para_ex_t  piic;        //要写入TEA5767的数据
		__s32 ret;

		eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));

		if(rw == 1)
		{
			__u8 addr = 0xFF;
			piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
			piic.slave_addr 	 = (dev_addr >> 1);
			piic.byte_addr_width = 0;
			piic.byte_addr		 = &addr;
			piic.data			 = data;
			piic.byte_count 	 = n_byte;

			if(dev_addr == IIC_CHIP__RTC6207_R)
			{
				ret = eLIBs_fioctrl(fiic, TWI_READ_EX_STP_RS, 0, (void *)&piic);
			}
			else
			{
				ret = eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);
			}

			if(ret != EPDK_OK)
			{
				eLIBs_printf("fm i2c read error~\n");
			}
		}
		else
		{
			__u8 addr = 0xFF;

			piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
			piic.slave_addr 	 = (dev_addr >> 1);
			piic.byte_addr_width = 0;
			piic.byte_addr		 = &addr;
			piic.data			 = data;
			piic.byte_count 	 = n_byte;

			ret = eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, (void *)&piic);

			if(ret != EPDK_OK)
			{
				eLIBs_printf("fm i2c write error~\n");
			}
		}

		if(ret != EPDK_OK)
		{
			switch(dev_addr)
			{
				case IIC_CHIP__RDA5807_R:
				case IIC_CHIP__RDA5807_W:
					g_flagFmIicFail |= FM_MODULE_TYPE_RDA5807M;
					break;
				case IIC_CHIP__RTC6207_R:
				case IIC_CHIP__RTC6207_W:
					g_flagFmIicFail |= FM_MODULE_TYPE_RTC6207M;
					break;
				default : break;
			}
		}
		return ret;
	}
#endif
}
#endif

#if (!SP_SimIIC)
__bool ReadRDAFM(__u8 addr, __u8 *data, __s32 n_byte)
{
	__twi_dev_para_ex_t  piic;        //要写入TEA5767的数据
	__s32 ret;
	__u8 addr1 = addr;
	eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
	{
		//addr = 0x0a;
		//n_byte = 4;
		piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
		piic.slave_addr 	 = (0x22 >> 1);	//(0x22>>1);//(0x20>>1);
		piic.byte_addr_width = 1;
		piic.byte_addr		 = &addr1;
		piic.data			 = data;
		piic.byte_count 	 = n_byte;
		ret = eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);

		if(ret == EPDK_FAIL)
		{
			__wrn("fm i2c read error~\n");
		}

		{
			int i = 0;
			__u8 *pdata = data;
			//for(i = 0; i < n_byte; i++)
			{
				//__msg("read data is : %x\n", *pdata++);
			}
		}

		__msg("**\n");
		__msg("**\n");
	}
	return ret;
}
#endif

#endif
