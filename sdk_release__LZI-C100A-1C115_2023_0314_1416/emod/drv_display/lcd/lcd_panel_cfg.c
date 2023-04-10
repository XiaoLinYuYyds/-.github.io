/*
*********************************************************************************************************
*                            sys_config.fex 配置示例
lcd_bl_en_used           = 0
lcd_bl_en                = 0

lcd_power_used           = 0
lcd_power                = 0

lcd_pwm_used             = 1
lcd_pwm                  = port:PA08<2><0><default><default>
*********************************************************************************************************
*/
#include "lcd_panel_cfg.h"


static void LCD_cfg_panel_info(__panel_para_t *info)
{
	memset(info, 0, sizeof(__panel_para_t));
	/*屏的基本信息*/
	info->lcd_x                   = 1024;
	info->lcd_y                   = 600;
	info->lcd_dclk_freq           = 44;  //33MHz 	// 不能低于22MHz
	info->lcd_pwm_freq            = 30;  //KHz
	info->lcd_srgb                = 0x00202020;
	info->lcd_swap                = 1;
	/*屏的接口配置信息*/
	info->lcd_if                  = 0;//0:HV , 1:8080 I/F, 2:TTL I/F, 3:LVDS
	/*屏的HV模块相关信息*/
	info->lcd_hv_if               = 0;
	info->lcd_hv_hspw             = 2;
	info->lcd_hv_lde_iovalue      = 0;
	info->lcd_hv_lde_used         = 0;
	info->lcd_hv_smode            = 0;
	info->lcd_hv_syuv_if          = 0;
	info->lcd_hv_vspw             = 2;

	/*屏的HV配置信息*/
	info->lcd_hbp           = 160;
	info->lcd_ht            = 1344;
	info->lcd_vbp           = 23;
	info->lcd_vt            = (2 * 635);

	/*屏的IO配置信息*/
	info->lcd_io_cfg0             = 0x00000000;
	info->lcd_io_cfg1             = 0x00000000;
	info->lcd_io_strength         = 0;

	/*TTL屏幕的配置信息*/
	info->lcd_ttl_ckvd            = 0;
	info->lcd_ttl_ckvh            = 0;
	info->lcd_ttl_ckvt            = 0;
	info->lcd_ttl_datainv_en      = 0;
	info->lcd_ttl_datainv_sel     = 0;
	info->lcd_ttl_datarate        = 0;
	info->lcd_ttl_oehd            = 0;
	info->lcd_ttl_oehh            = 0;
	info->lcd_ttl_oevd            = 0;
	info->lcd_ttl_oevh            = 0;
	info->lcd_ttl_oevt            = 0;
	info->lcd_ttl_revd            = 0;
	info->lcd_ttl_revsel          = 0;
	info->lcd_ttl_sthd            = 0;
	info->lcd_ttl_sthh            = 0;
	info->lcd_ttl_stvdl           = 0;
	info->lcd_ttl_stvdp           = 0;
	info->lcd_ttl_stvh            = 0;
	//cpu屏幕的配置信息
	info->lcd_frm 			 = 1;	//0: disable; 1: enable rgb666 dither; 2:enable rgb656 dither
}

static __s32 LCD_open_flow(__u32 sel)
{
	LCD_OPEN_FUNC(sel, LCD_power_on	, 10); 	//打开LCD供电，并延时10ms
	LCD_OPEN_FUNC(sel, LCD_io_init	, 20); 	//申请和初始化IO，并延时20ms
	LCD_OPEN_FUNC(sel, TCON_open	, 200); //打开LCD控制器，并延时200ms
	LCD_OPEN_FUNC(sel, LCD_open_cmd	, 10); 	//通过IO配置LCD模组为工作模式，并延时10ms
	LCD_OPEN_FUNC(sel, LCD_bl_open	, 0); 	//打开背光，并延时0ms
	return 0;
}

static __s32 LCD_close_flow(__u32 sel)
{
	LCD_CLOSE_FUNC(sel, LCD_bl_close    , 10); 	//关闭背光，并延时0ms
	LCD_CLOSE_FUNC(sel, LCD_close_cmd   , 20); 	//通过IO配置LCD模组为掉电/休眠模式，并延时0ms
	LCD_CLOSE_FUNC(sel, TCON_close	    , 20); 	//关闭LCD 控制器，并延时0ms
	LCD_CLOSE_FUNC(sel, LCD_io_exit	    , 20); 	//关屏时IO的状态并释放，并延时0ms
	LCD_CLOSE_FUNC(sel, LCD_power_off   , 100); //关闭LCD供电，并延时1700ms
	return 0;
}


////////////////////////////////////////   POWER   ////////////////////////////////////////////////////////////////////
static __u32 LCD_Request_GPIO(char *ioname)
{
	/* 申请input_row1 */
	__u32			 GPIO;
	__s32            ret;
	user_gpio_set_t  gpio_set[1];
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("lcd0_para", (char *)ioname, (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		GPIO = esPINS_PinGrpReq(gpio_set, 1);

		if(!GPIO)
		{
			__wrn("request %s pin failed\n", ioname);
			return 0;
		}
	}
	else
	{
		__wrn("%s fetch para from script failed\n", ioname);
		return 0;
	}

	{
		//ret = esPINS_SetPinPull(GPIO, PIN_PULL_DISABLE, NULL);
		ret = esPINS_SetPinIO(GPIO, 1, NULL);
		ret = esPINS_WritePinData(GPIO, 0, NULL);

		if(ret)
		{
			__wrn("pull %s failed\n", ioname);
			return 0;
		}

		esPINS_PinGrpRel(GPIO, 2);
	}

	return GPIO;
}

#if 0
static __u32 LCD_lvdsvccen_set(__u32 sel)
{
	/* 申请lvdsvccen */
	static __hdle	s_hdleGPIO = 0;
	
	__s32    ret;
	user_gpio_set_t  gpio_set[1];
	
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));

	if(!s_hdleGPIO)
	{
		ret = esCFG_GetKeyValue("lcd0_para", "lvdsvccen", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
		if(ret == EPDK_OK)
		{
			s_hdleGPIO = esPINS_PinGrpReq(gpio_set, 1);
			if(!s_hdleGPIO) return 0;
		}
	}

	if(s_hdleGPIO)
	{
		//ret = esPINS_SetPinPull(s_hdleGPIO, PIN_PULL_DISABLE, NULL);		
		ret = esPINS_SetPinIO(s_hdleGPIO, 1, NULL);
		ret = esPINS_WritePinData(s_hdleGPIO,sel, NULL);

		if(ret != EPDK_OK)
		{
			__wrn("pull lvdsvccen failed\n");
			return 0;
		}
	}
	
	return s_hdleGPIO;
}
#endif

static void LCD_io_hight_res(void)
{
	//LCD_bl_open(0);
	//LCD_power_on(0);
	LCD_Request_GPIO("lcdd2");
	LCD_Request_GPIO("lcdd3");
	LCD_Request_GPIO("lcdd4");
	LCD_Request_GPIO("lcdd5");
	LCD_Request_GPIO("lcdd6");
	LCD_Request_GPIO("lcdd7");
	LCD_Request_GPIO("lcdd10");
	LCD_Request_GPIO("lcdd11");
	LCD_Request_GPIO("lcdd12");
	LCD_Request_GPIO("lcdd13");
	LCD_Request_GPIO("lcdd14");
	LCD_Request_GPIO("lcdd15");
	LCD_Request_GPIO("lcdclk");
	LCD_Request_GPIO("lcdde");
	LCD_Request_GPIO("lcdhsync");
	LCD_Request_GPIO("lcdvsync");
	LCD_Request_GPIO("lcdd18");
	LCD_Request_GPIO("lcdd19");
	LCD_Request_GPIO("lcdd20");
	LCD_Request_GPIO("lcdd21");
	LCD_Request_GPIO("lcdd22");
	LCD_Request_GPIO("lcdd23");
}

static void LCD_PWM1_EN(__u32 open_or_colse)
{
	ES_FILE* pfile_pwm ;

	pfile_pwm =  eLIBs_fopen("b:\\USER\\PWM","rb+"); //打开模块
	if (!pfile_pwm)
	{
		eLIBs_printf("opne failed\n");
		return ;
	}
	eLIBs_printf("pfile_pwm=%x\n",pfile_pwm);
	if (open_or_colse)
	{
		eLIBs_fioctrl(pfile_pwm, PWM_CMD_SET_PWM_FRE, 30,(void *)0);
		eLIBs_fioctrl(pfile_pwm,PWM_CMD_START,0,(void *)1); //打开PWM0   最后一个参数0:pwm0 1:pwm1 
	}
	else
	{
		eLIBs_fioctrl(pfile_pwm,PWM_CMD_STOP,0,(void *)1); //关闭PWM0   最后一个参数0:pwm0 1:pwm1 
	}
	esKRNL_TimeDly(2);
	eLIBs_fclose(pfile_pwm);
	
	return ;

}


#define PIOC_REG_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)    ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i) ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)   ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	 ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + 0x10))

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
	tmp_data |= (1 << port_num);
	*tmp_addr = tmp_data;
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

static void LCD_power_on(__u32 sel)//0 active
{
	LCD_POWER_EN(sel, 1);
	
	esKRNL_TimeDly(10);	
//	LCD_POWER_EN(sel, 1);
	LCD_PWM_EN(sel, 1);
	
	//LCD_lvdsvccen_set(1);
}

static void LCD_power_off(__u32 sel)//0 active
{
//LCD_lvdsvccen_set(0);
	LCD_POWER_EN(sel, 0);
	LCD_PWM_EN(sel, 0);
	LCD_io_hight_res();


//	LCD_io_hight_res();
//	LCD_PWM_EN(sel, 0);
}

/*
*********************************************************************************************************
*                                           BACK LIGHT ON
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
void LCD_bl_open(__u32 sel)
{
	//esKRNL_TimeDly(100);
	//LCD_power_on(1);
	//esKRNL_TimeDly(10);	
	
	
	
	LCD_BL_EN(sel, 1);

	esKRNL_TimeDly(20); 

	LCD_PWM1_EN(1);
	

}

void LCD_bl_close(__u32 sel)
{
	__u32 t_sel = sel;

	sel &= 0xFFFF;
	if(t_sel & (1 << 16))
	{
		// 需要关闭LCD电源
		LCD_power_off(sel);
	}
	//LCD_POWER_EN(sel, 0);
	LCD_BL_EN(sel, 0);
//	esKRNL_TimeDly(20);
//	LCD_power_off(0);
	LCD_PWM1_EN(0);
}


//申请IO,设置IO状态为输出HIGHT
static void  LCD_io_init(__u32 sel)
{}

//设置IO状态为输入,高阻态
static void LCD_io_exit(__u32 sel)
{
}

static void  LCD_open_cmd(__u32 sel)
{}

static void  LCD_close_cmd(__u32 sel)
{}

void LCD_get_panel_funs_0(__lcd_panel_fun_t *fun)
{
	fun->cfg_panel_info = LCD_cfg_panel_info;
	fun->cfg_open_flow = LCD_open_flow;
	fun->cfg_close_flow = LCD_close_flow;
}

