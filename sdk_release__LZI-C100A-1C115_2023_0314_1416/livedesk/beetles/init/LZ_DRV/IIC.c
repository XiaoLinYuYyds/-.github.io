#include "IIC.h"

#if 1


#define PIOC_REG_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)    ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i) ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)   ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	 ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + 0x10))


#define PORT__IIC_SCLK						PORTE
#define PORT_ID__IIC_SCLK 				0
#define PORT__IIC_SDAT						PORTE
#define PORT_ID__IIC_SDAT				1

__bool I2C_Ack;

static void SetSDA_HardIIC_IO(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	__u32 port = PORT__IIC_SDAT;
	__u32 port_num = PORT_ID__IIC_SDAT;
	//设置为输出
	tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
	tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x07 << tmp1);
	tmp_data |= (0x04 << tmp1);
	*tmp_addr = tmp_data;
}

static void SetCLK_HardIIC_IO(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	__u32 port = PORT__IIC_SCLK;
	__u32 port_num = PORT_ID__IIC_SCLK;
	//设置为输出
	tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
	tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x07 << tmp1);
	tmp_data |= (0x04 << tmp1);
	*tmp_addr = tmp_data;
}

static void SetSDA_1(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32 port = PORT__IIC_SDAT;
	__u32 port_num = PORT_ID__IIC_SDAT;
	//输出高电平
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	//tmp_data &= ~(1 << port_num);
	tmp_data |= (1 << port_num);
	*tmp_addr = tmp_data;
}
static void SetSDA_0(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32 port = PORT__IIC_SDAT;
	__u32 port_num = PORT_ID__IIC_SDAT;
	//输出低电平
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	tmp_data &= ~(1 << port_num);
	//tmp_data |=  (1 << port_num);
	*tmp_addr = tmp_data;
}
static void SetCLK_1(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32 port = PORT__IIC_SCLK;
	__u32 port_num = PORT_ID__IIC_SCLK;
	//输出高电平
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	//tmp_data &= ~(1 << port_num);
	tmp_data |= (1 << port_num);
	*tmp_addr = tmp_data;
}
static void SetCLK_0(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32 port = PORT__IIC_SCLK;
	__u32 port_num = PORT_ID__IIC_SCLK;
	//输出低电平
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	tmp_data &= ~(1 << port_num);
	//tmp_data |=  (1 << port_num);
	*tmp_addr = tmp_data;
}


static void SetSDAOut(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	__u32 port = PORT__IIC_SDAT;
	__u32 port_num = PORT_ID__IIC_SDAT;
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
}
static void SetSDAIn(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	__u32 port = PORT__IIC_SDAT;
	__u32 port_num = PORT_ID__IIC_SDAT;
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
}
static void SetCLKOut(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	__u32 port = PORT__IIC_SCLK;
	__u32 port_num = PORT_ID__IIC_SCLK;
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
}

static __s32 GetSDAIn(void)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__s32               ret;
	__u32 port = PORT__IIC_SDAT;
	__u32 port_num = PORT_ID__IIC_SDAT;
	tmp_addr = PIO_REG_DATA(port);
	tmp_data = *tmp_addr;
	//__wrn("tmp_data=0x%x\n", tmp_data);
	ret = (tmp_data & (1 << port_num)) >> port_num;
	return ret;
}


void I2C_usDelay(__s32 us)
{
	__s32 i;
	__s32 usTemp = us * 128 * 8;

	for(i = 0; i < usTemp; i++)
		;

	//CLEAR_WATCHDOG();
}

void I2C_Init(void)
{
	SetSDAOut();
	SetCLKOut();
	SetSDA_1();
	SetCLK_1();
}

void I2C_End(void)
{
	SetSDA_HardIIC_IO();
	SetCLK_HardIIC_IO();
}

void I2C_Start(void)
{
	I2C_Ack = EPDK_FALSE;
	I2C_usDelay(50);
	SetSDA_1();
	I2C_usDelay(1);
	SetCLK_1();
	I2C_usDelay(10);
	SetSDA_0();
	I2C_usDelay(1);
	SetCLK_0();
	I2C_usDelay(2);
}

void I2C_Stop(void)
{
	SetSDA_0();
	SetCLK_0();
	I2C_usDelay(2);
	SetCLK_1();
	I2C_usDelay(2);
	SetSDA_1();
}

void I2C_SetAck(__bool ack)
{
	I2C_Ack = ack;
}

__bool I2C_GetAck(void)
{
	return I2C_Ack;
}

__bool I2C_I2Ctransbyte(__u8 shu, __bool ACK)
{
	__u8 i, j, datatemp;

	for(i = 0; i < 8; i++)
	{
		if((shu & 0x80) > 0)
		{
			SetSDA_1();
		}
		else
		{
			SetSDA_0();
		}

		I2C_usDelay(1);
		SetCLK_1();
		I2C_usDelay(2);
		SetCLK_0();
		//SetSDA_0();
		I2C_usDelay(1);
		shu = shu << 1;
	}

	if(ACK)
	{
		SetSDA_1();
		SetSDAIn();
		I2C_usDelay(1);
		SetCLK_1();
		I2C_usDelay(2);
		I2C_Ack = GetSDAIn();
		SetCLK_0();
		SetSDAOut();
		SetSDA_0();
		//__wrn("I2C_Ack=%d\n", I2C_Ack);
	}
	else
	{
		I2C_Ack = EPDK_TRUE;
	}

	I2C_usDelay(2);
	return EPDK_TRUE;
}

__u8 I2C_I2Crecebyte(__bool SendACK)
{
	__u8 nn = 0x00, mm = 0x01, uu = 0xfE;
	__u8 j;
	SetSDAIn();
	I2C_usDelay(1);

	for(j = 0; j < 8; j++)
	{
		nn = nn << 1;
		SetCLK_1();
		I2C_usDelay(2);

		if(GetSDAIn())
		{
			nn = (nn | mm);
		}
		else
		{
			nn = (nn & uu);
		}

		SetCLK_0();
		I2C_usDelay(1);
	}

	SetSDAOut();

	if(SendACK)
	{
		SetSDA_0();
	}
	else
	{
		SetSDA_1();
	}

	//if (SendACK)
	{
		I2C_usDelay(2);
		SetCLK_1();
		I2C_usDelay(1);
		SetCLK_0();
	}
	I2C_usDelay(2);
	return(nn);
}


#endif
