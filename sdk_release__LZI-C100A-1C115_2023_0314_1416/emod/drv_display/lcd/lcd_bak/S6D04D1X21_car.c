/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Display Module
*
*                                    (c) Copyright 2011-2014, Vito China
*                                             All Rights Reserved
*
* File    : ILI9327.c
* By      : Vito
* Version : v1.0
* Date    : 2011-4-27
* Descript: .
* Update  : date                auther      ver     notes
*           2011-4-27 14:07:21  Vito        1.0     Create this file.
*********************************************************************************************************
*/

#include "lcd_panel_cfg.h"
void LCD_Reset(__u32 sel);

static void LCD_cfg_panel_info(__panel_para_t *info)
{
	memset(info, 0, sizeof(__panel_para_t));
	//���Ļ�����Ϣ
	info->lcd_x              = 400;//432;		//432
	info->lcd_y              = 240;
	info->lcd_dclk_freq      = 22;//14;  //14M////8MHz
	info->lcd_pwm_freq       = 1;  //KHz
	info->lcd_srgb           = 0x00202020;
	info->lcd_swap           = 0;
	__msg("==============================================================data clock is %d\n", info->lcd_dclk_freq);
	//__wait__;
	//���Ľӿ�������Ϣ
	info->lcd_if             = 0;//0:HV , 1:8080 I/F, 2:TTL I/F, 3:LVDS
	//����HVģ�������Ϣ
	info->lcd_hv_if          = 1;//lcd_hv_if = 1,rgb888, �����δ���, 0=24bit
	info->lcd_hv_hspw        = 5;//20;//1--ͬ���źſ��
	info->lcd_hv_lde_iovalue = 0;
	info->lcd_hv_lde_used    = 0;
	info->lcd_hv_smode       = 0;
	info->lcd_hv_syuv_if     = 0;
	info->lcd_hv_vspw        = 5;//20;
	info->lcd_hv_s888_if 	 = 0;//0~15
	//����HV������Ϣ
	info->lcd_hbp            = 10;//3;//3;					//���ֲ�����
	info->lcd_ht             = 1415;//info->lcd_hbp + 1 + (info->lcd_x + 1)*3+2 + 10;		//��cycle��//info->lcd_hbp + 1 + (info->lcd_x + 1)*3+2 + 100;		//��cycle��
	info->lcd_vbp            = 10;//2;//2;
	info->lcd_vt             = 600;	////(info->lcd_vbp + 1 + info->lcd_y + 1 + 2)*2+50;
	//����IO������Ϣ
	info->lcd_io_cfg0        = 0x00000000;
	info->lcd_io_cfg1        = 0x00000000;
	info->lcd_io_strength    = 0;
	//TTL��Ļ��������Ϣ
	info->lcd_ttl_ckvd       = 0;
	info->lcd_ttl_ckvh       = 0;
	info->lcd_ttl_ckvt       = 0;
	info->lcd_ttl_datainv_en = 0;
	info->lcd_ttl_datainv_sel = 0;
	info->lcd_ttl_datarate   = 0;
	info->lcd_ttl_oehd       = 0;
	info->lcd_ttl_oehh       = 0;
	info->lcd_ttl_oevd       = 0;
	info->lcd_ttl_oevh       = 0;
	info->lcd_ttl_oevt       = 0;
	info->lcd_ttl_revd       = 0;
	info->lcd_ttl_revsel     = 0;
	info->lcd_ttl_sthd       = 0;
	info->lcd_ttl_sthh       = 0;
	info->lcd_ttl_stvdl      = 0;
	info->lcd_ttl_stvdp      = 0;
	info->lcd_ttl_stvh       = 0;
	//cpu��Ļ��������Ϣ
	info->lcd_frm 			 = 1;	//0: disable; 1: enable rgb666 dither; 2:enable rgb656 dither
}

void  Lcd_cpuisr_proc(void);

static void LCD_init_isr(__u32 sel)
{
	LCD_cpu_register_irq(sel, Lcd_cpuisr_proc);
}


static void LCD_exit_isr(__u32 sel)
{
	LCD_cpu_register_irq(sel, 0);
}

static __s32 LCD_open_flow(__u32 sel)
{
	__msg("lcd init");
	LCD_OPEN_FUNC(sel, LCD_power_on	, 10); 	//��LCD���磬����ʱ10ms
	LCD_Reset(sel);
	LCD_OPEN_FUNC(sel, TCON_open	, 200); //��LCD������������ʱ200ms
	//LCD_OPEN_FUNC(sel, LCD_open_cmd	,650); 	//ͨ��IO����LCDģ��Ϊ����ģʽ������ʱ100ms
	LCD_OPEN_FUNC(sel, LCD_open_cmd	, 1000); 	//ͨ��IO����LCDģ��Ϊ����ģʽ������ʱ100ms
	LCD_OPEN_FUNC(sel, LCD_bl_open	, 0); 	//�򿪱��⣬����ʱ0ms
	LCD_OPEN_FUNC(sel, LCD_init_isr, 0); //��ʼ��CPU PANEL �жϷ�����򣬲���ʱ 0ms
	__msg("lcd reset....\n");
	//__wait__;
	return 0;
}

static __s32 LCD_close_flow(__u32 sel)
{
	LCD_CLOSE_FUNC(sel, LCD_exit_isr, 0); //�ر�CPU PANEL�жϷ������,����ʱ0ms
	LCD_CLOSE_FUNC(sel, LCD_bl_close    , 0); 	//�رձ��⣬����ʱ0ms
	LCD_CLOSE_FUNC(sel, LCD_close_cmd   , 0); 	//ͨ��IO����LCDģ��Ϊ����/����ģʽ������ʱ0ms
	LCD_CLOSE_FUNC(sel, TCON_close	    , 0); 	//�ر�LCD ������������ʱ0ms
	LCD_CLOSE_FUNC(sel, LCD_power_off   , 1700); //�ر�LCD���磬����ʱ1700ms
	return 0;
}


void LCD_Reset(__u32 sel)
{
	//piod24
	__hdle           hdl;
	user_gpio_set_t  gpio_set[1];
	//esKRNL_TimeDly(20);
	gpio_set->port = 4;
	gpio_set->port_num = 24;
	gpio_set->mul_sel = 1;							//output
	gpio_set->pull = 1;									//pull down
	gpio_set->drv_level = 1;
	gpio_set->data = 1;									//
	hdl = OSAL_GPIO_Request(gpio_set, 1);
	OSAL_GPIO_Release(hdl, 2);
	__msg("LCD Reset high....\n");
	esKRNL_TimeDly(1);
	gpio_set->port = 4;
	gpio_set->port_num = 24;
	gpio_set->mul_sel = 1;							//output
	gpio_set->pull = 0;									//pull down
	gpio_set->drv_level = 1;
	gpio_set->data = 0;									//
	hdl = OSAL_GPIO_Request(gpio_set, 1);
	OSAL_GPIO_Release(hdl, 2);
	__msg("select panel is %d\n", sel);
	__msg("LCD Reset....\n");
	//__wait__;
	esKRNL_TimeDly(8);
	gpio_set->port = 4;
	gpio_set->port_num = 24;
	gpio_set->mul_sel = 1;							//output
	gpio_set->pull = 1;									//pull down
	gpio_set->drv_level = 1;
	gpio_set->data = 1;									//
	hdl = OSAL_GPIO_Request(gpio_set, 1);
	OSAL_GPIO_Release(hdl, 2);
	//sys_put_wvalue(0xf1c20848, (sys_get_wvalue(0xf1c20848)&0xfff0ffff)|(0x1<<16));
	//sys_put_wvalue(0xf1c20858, (sys_get_wvalue(0xf1c20858)&0xffffffef));
}

/*
*********************************************************************************************************
*                                           POWER ON
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
static void LCD_power_on(__u32 sel)
{
	//pioc4
	__hdle           hdl;
	user_gpio_set_t  gpio_set[1];
	//LCD_Reset(sel);
	gpio_set->port = 3;
	gpio_set->port_num = 4;
	gpio_set->mul_sel = 1;
	gpio_set->pull = 1;
	gpio_set->drv_level = 1;
	gpio_set->data = 0;
	hdl = OSAL_GPIO_Request(gpio_set, 1);
	OSAL_GPIO_Release(hdl, 2);
	//sys_put_wvalue(0xf1c20848, (sys_get_wvalue(0xf1c20848)&0xfff0ffff)|(0x1<<16));
	//sys_put_wvalue(0xf1c20858, (sys_get_wvalue(0xf1c20858)&0xffffffef));
}

static void LCD_power_off(__u32 sel)
{
	//pioc4
	__hdle hdl;
	user_gpio_set_t  gpio_set[1];
	gpio_set->port = 3;
	gpio_set->port_num = 4;
	gpio_set->mul_sel = 1;
	gpio_set->pull = 1;
	gpio_set->drv_level = 1;
	gpio_set->data = 1;
	hdl = OSAL_GPIO_Request(gpio_set, 1);
	OSAL_GPIO_Release(hdl, 2);
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
static void LCD_bl_open(__u32 sel)
{
	__u32 tmp;
	__lcd_panel_init_para_t para;
	LCD_get_init_para(&para);
	tmp = sys_get_wvalue(para.base_timer + 0xa0);
	tmp |= (1 << 4);
	sys_put_wvalue(para.base_timer + 0xa0, tmp);
}

static void LCD_bl_close(__u32 sel)
{
	__u32 tmp;
	__lcd_panel_init_para_t para;
	LCD_get_init_para(&para);
	tmp = sys_get_wvalue(para.base_timer + 0xa0);
	tmp &= (~(1 << 4));
	sys_put_wvalue(para.base_timer + 0xa0, tmp);
}

void  lcd_cpu_cmdset()
{}

static void  LCD_open_cmd(__u32 sel)
{}

static void  LCD_close_cmd(__u32 sel)
{}

void  Lcd_cpuisr_proc(void)
{}

void LCD_get_panel_funs_0(__lcd_panel_fun_t *fun)
{
	fun->cfg_panel_info = LCD_cfg_panel_info;
	fun->cfg_open_flow = LCD_open_flow;
	fun->cfg_close_flow = LCD_close_flow;
}

