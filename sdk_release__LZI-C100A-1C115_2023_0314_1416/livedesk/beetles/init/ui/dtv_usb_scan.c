#include "dtv_usb_detect.h"

s32 usbportno = 0;
s32 peripheral_connect = 0;
static usb_board_info_t usbh_board_info_g;
static hcd_sys_cfg_t hcd_sys_cfg_g;

extern void shell_USBC_INT_DisableEp(__u32 port_no, __u32 ep_type, __u8 ep_index);
static void shell_disable_usb_irq(void);


static void shell_usb_wakeup_change_detect_disable(void)
{
    s32  cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
    shell_USBC_DisableWakeIrq(usbportno);
    shell_USBC_DisableVbusChange(usbportno);
    shell_USBC_DisableIdChange(usbportno);
    shell_USBC_DisableDpDmChange(usbportno);
    EXIT_CRITICAL(cpu_sr);
}
static void shell_usb_wakeup_change_detect_enable(void)
{
    s32  cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
	shell_USBC_EnableWakeIrq(usbportno);
	shell_USBC_EnableVbusChange(usbportno);
	shell_USBC_EnableIdChange(usbportno);
	shell_USBC_EnableDpDmChange(usbportno);
    EXIT_CRITICAL(cpu_sr);
}

void shell_clear_usb_wakeup_detect_flag(void)
{
    s32  cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
	shell_USBC_IsVbusChange(usbportno);
	shell_USBC_IsIdChange(usbportno);
	shell_USBC_IsDpDmChange(usbportno);
    EXIT_CRITICAL(cpu_sr);
}

static s32 shell_host_controller_init()
{
	//--<1>--设置bus的访问方式
	shell_USBC_SelectBus(usbportno, USBC_IO_TYPE_PIO, 0, 0);

	//--<2>--映射SRAM D给usb使用
	shell_USBC_ConfigFIFO_Base(usbportno, USBC_FIFO_MODE_8K);

	/* disable wakeup interruput */
	shell_usb_wakeup_change_detect_disable();
    shell_clear_usb_wakeup_detect_flag();

	/* force id/vbus */
	shell_USBC_ForceId(usbportno, USBC_ID_TYPE_HOST);
	return 0;
}

static s32 shell_host_start()
{
	/* disable all irq */
	shell_USBC_INT_DisableEpAll(usbportno, USBC_EP_TYPE_TX);
	shell_USBC_INT_DisableEpAll(usbportno, USBC_EP_TYPE_RX);
	shell_USBC_INT_DisableUsbMiscAll(usbportno);

	/* enable misc irq */
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_SUSPEND);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_RESUME);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_RESET);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_SOF);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_CONNECT);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_DISCONNECT);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_SESSION_REQ);
	shell_USBC_INT_EnableUsbMiscUint(usbportno, USBC_INTUSB_VBUS_ERROR);

    /* set usb power on */
	return 0;
}

static void shell_enable_usb_irq(void)
{
    esINT_EnableINT(INTC_IRQNO_USB0);
}

static void shell_disable_usb_irq(void)
{
    esINT_DisableINT(INTC_IRQNO_USB0);
}

static s32 shell_host_stop(void)
{
    /* disable irq */
	shell_USBC_INT_DisableEpAll(usbportno, USBC_EP_TYPE_TX);
	shell_USBC_INT_DisableEpAll(usbportno, USBC_EP_TYPE_RX);
	shell_USBC_INT_DisableUsbMiscAll(usbportno);
	return 0;
}


static __s32 shell_host_irq(void * arg)
{
	u32 int_pendding_misc   = 0;
	__cpu_sr cup_sr 	    = 0;

	ENTER_CRITICAL(cup_sr);
	int_pendding_misc = shell_USBC_INT_MiscPending(usbportno);
	EXIT_CRITICAL(cup_sr);

	__msg("[monitor irq]: misc irq = 0x%x\n", int_pendding_misc);

    if(int_pendding_misc & USBC_INTUSB_CONNECT){
        __msg("[monitor]: USB Connect\n");
        int_pendding_misc &= ~USBC_INTUSB_CONNECT;

        peripheral_connect = 1;
    }

    if(int_pendding_misc & USBC_INTUSB_DISCONNECT){
        __msg("[monitor]: USB Connect\n");
        int_pendding_misc &= ~USBC_INTUSB_DISCONNECT;

        peripheral_connect = 0;
    }

    if(int_pendding_misc & USBC_INTUSB_VBUS_ERROR){
        __msg("[monitor]: USB vbus error\n");
        int_pendding_misc &= ~USBC_INTUSB_VBUS_ERROR;
    }

    if(int_pendding_misc & USBC_INTUSB_SESSION_REQ){
        __msg("[monitor]: USB session request\n");
        int_pendding_misc &= ~USBC_INTUSB_SESSION_REQ;
    }

    if(int_pendding_misc & USBC_INTUSB_RESET){
        __msg("[monitor]: USB bable\n");
        int_pendding_misc &= ~USBC_INTUSB_RESET;
    }

    shell_USBC_INT_ClearMiscPendingAll(usbportno);
    shell_USBC_INT_ClearEpPendingAll(usbportno, USBC_EP_TYPE_TX);
    shell_USBC_INT_ClearEpPendingAll(usbportno, USBC_EP_TYPE_RX);

	return EPDK_TRUE;
}

static s32 host_controller_exit()
{
	/* enable wakeup interruput */
    shell_usb_wakeup_change_detect_enable();

	/* force id/vbus */
	shell_USBC_ForceId(usbportno, USBC_ID_TYPE_DISABLE);
	return 0;
}

typedef struct tag_shell_usb_hw_scan_sys{
    __hdle  clock_ahb;         //usb时钟管理
    __hdle  clock_phy;

    __hdle  usb_id_pin_hdle;   //pin脚  ID
    __hdle  usb_vbus_pin_hdle;
}usb_shell_hw_scan_sys_t;

usb_shell_hw_scan_sys_t g_hw_scan_sys;
s32 open_usb_clock(void)
{
    s32 ret = 0;
    usb_shell_hw_scan_sys_t *hw_scan_sys = &g_hw_scan_sys;

    //--<1>--开usb sie时钟
    hw_scan_sys->clock_ahb = esCLK_OpenMclk(CSP_CCM_MOD_CLK_AHB_USB);
	if(hw_scan_sys->clock_ahb == NULL){
		//__wrn("usb_monitor: open usb AHB clock failed\n");
		return EPDK_FAIL;
	}

	ret = esCLK_MclkOnOff(hw_scan_sys->clock_ahb, CLK_ON);
	if(ret != EPDK_OK){
		__wrn("usb_monitor: on usb AHB clock failed\n");
		return EPDK_FAIL;
	}

	//--<2>--开usb phy时钟
	hw_scan_sys->clock_phy = esCLK_OpenMclk(CSP_CCM_MOD_CLK_USB_PHY);
	if(hw_scan_sys->clock_phy == NULL){
		__wrn("ERR: close_usb_clock_mode: open usb PHY clock failed\n");
		return EPDK_FAIL;
	}

    ret = esCLK_MclkOnOff(hw_scan_sys->clock_phy, CLK_ON);
	if(ret != EPDK_OK){
		__wrn("ERR: close_usb_clock_mode: on usb PHY clock failed\n");
		return EPDK_FAIL;
	}
	
	esCLK_MclkReset(hw_scan_sys->clock_phy, 0);
    __msg("USB_MONITOR: open_usb_clock1\n");

	//延时30ms
	esKRNL_TimeDly(3);

    __msg("USB_MONITOR: open_usb_clock2\n");

    return EPDK_OK;
}

s32  close_usb_clock_mode(void)
{
    s32  ret = 0;
    usb_shell_hw_scan_sys_t *hw_scan_sys = &g_hw_scan_sys;

	ret = esCLK_MclkOnOff(hw_scan_sys->clock_ahb, CLK_OFF);
	if(ret != EPDK_OK){
		__wrn("usbd_drv: off usbd AHB clock failed\n");
		return -1;
	}

    //--<1>--关usb sie时钟,只是释放句柄
	ret = esCLK_CloseMclk(hw_scan_sys->clock_ahb);
	if(ret != EPDK_OK){
		__wrn("ERR: close_usb_clock_mode: close usbd AHB clock failed\n");
		return EPDK_FAIL;
	}

	ret = esCLK_MclkOnOff(hw_scan_sys->clock_phy, CLK_OFF);
	if(ret != EPDK_OK){
		__wrn("usbd_drv: off usbd PHY clock failed\n");
		return -1;
	}

	//--<2>--关usb phy时钟,只是释放句柄
	ret = esCLK_CloseMclk(hw_scan_sys->clock_phy);
	if(ret != EPDK_OK){
		__wrn("ERR: close_usb_clock_mode: close usbd PHY clock failed\n");
		return EPDK_FAIL;
	}

 	esCLK_MclkReset(hw_scan_sys->clock_phy, 1);
    return EPDK_OK;
}

s32 hcd_hardware_init()
{
    usb_board_info_t *board_info = &usbh_board_info_g;
	__s32 ret = 0;

    eLIBs_memset(board_info, 0, sizeof(usb_board_info_t));
	shell_get_usb_board_info(board_info);

    //--<1>--disable wakeup interruput
	shell_usb_wakeup_change_detect_disable();
    shell_clear_usb_wakeup_detect_flag();

	//--<2>--for id to low, as host
	shell_USBC_ForceId(usbportno, USBC_ID_TYPE_HOST);

	//--<3>--hcd sys config initialize
	ret = shell_init_hcd_sys_cfg(usbportno, board_info, &hcd_sys_cfg_g);
	if(ret != 0){
		__msg("ERR: init_hcd_sys_cfg failed\n");
		return -1;
	}

    return 0;
}

s32 hcd_hardware_exit()
{
    usb_board_info_t *board_info = &usbh_board_info_g;

	//--<1>--hcd sys config initialize
	shell_exit_hcd_sys_cfg(usbportno, board_info, &hcd_sys_cfg_g);

	//--<2>--enable wakeup interruput
	shell_usb_wakeup_change_detect_enable();

	//--<3>--for id to disable
	shell_USBC_ForceId(usbportno, USBC_ID_TYPE_DISABLE);

    return 0;
}

void __port_power(u32 is_on)
{
    __cpu_sr cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
	if(is_on){
		shell_USBC_Host_StartSession(usbportno);
		shell_USBC_ForceVbusValid(usbportno, USBC_VBUS_TYPE_HIGH);
		__msg("INFO : USB VBus power ON\n");
	}else{
		shell_USBC_Host_EndSession(usbportno);
		shell_USBC_ForceVbusValid(usbportno, USBC_VBUS_TYPE_DISABLE);
		__msg("INFO : USB VBus power OFF\n");
	}
    EXIT_CRITICAL(cpu_sr);

	return;
}

/*
********************************************************************************
*                     hcd_ops_start
* Description:
*     usb host start to work
* Arguments:
*     hcd  : input.
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
static s32 __hcd_ops_start(void)
{

	//往vbus供电
	__port_power(1);

	return 0;
}

/*
********************************************************************************
*                     hcd_ops_stop
* Description:
*     usb host stop working
* Arguments:
*     hcd  : input.
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
static void __hcd_ops_stop(void)
{

	//vbus停电
	__port_power(0);

	return ;
}

s32 IsUSBHostDetect(void)
{
	s32 ret;
	__msg("IsUSBHostDetect\n");
	open_usb_clock();
	shell_disable_usb_irq();

	hcd_hardware_init();
	
	shell_host_controller_init();
	__msg("esINT_InsISR INTC_IRQNO_USB0\n");
	ret = esINT_InsISR(INTC_IRQNO_USB0, (__pISR_t)shell_host_irq, NULL);
	if(ret != EPDK_OK){
		__wrn("ERR: reg isr fail\n");
    	goto err2;
	}
	
	__msg("shell_host_start\n");
	shell_host_start();
	__msg("shell_enable_usb_irq\n");
//	printf_test(0);
	__hcd_ops_start();
	
	shell_enable_usb_irq();

	__msg("delay 1s\n");

    {
        __s32 i;

        for(i = 0 ; i < 1 ; i++)
        if(1){
                __s32 j = 0;        
                __s32 cnt = 2400000;//50ms
                while(j ++ < cnt)
                {
                    __asm 
                    {
                        nop;
                    }
                }
              }
    }

    /* disable hcd */
 	__msg("shell_disable_usb_irq\n");
    shell_disable_usb_irq(); 
	__hcd_ops_stop();
 	__msg("shell_host_stop\n");
	shell_host_stop();

 	__msg("esINT_UniISR INTC_IRQNO_USB0\n");
	esINT_UniISR(INTC_IRQNO_USB0, (__pISR_t)shell_host_irq);

	hcd_hardware_exit();
	if(peripheral_connect)
	{
 		__msg("peripheral_connect == 1\n");
		host_controller_exit();
		close_usb_clock_mode();
		return 1;
	}
err2:
 	__msg("peripheral_connect == 0\n");
	host_controller_exit();
	close_usb_clock_mode();
	return 0;
}

s32 USBHostDetectStart(void)
{
	s32 ret;

    eLIBs_memset(&g_hw_scan_sys, 0, sizeof(usb_shell_hw_scan_sys_t));
	__msg("IsUSBHostDetect\n");
	open_usb_clock();
	shell_disable_usb_irq();

	hcd_hardware_init();
	
	shell_host_controller_init();


    
	
	__msg("shell_host_start\n");
	shell_host_start();
	__msg("shell_enable_usb_irq\n");
//	printf_test(0);
	__hcd_ops_start();
	
	//shell_enable_usb_irq();


    return 0;
}

static __s32 init_enable_device(void)
{
    *(volatile unsigned long *)(0xf1c13400) |= 1<<USBC_BP_ISCR_DPDM_PULLUP_EN;
    
    return 0;
}


s32 USBHostDetectStop(void)
{
     /* disable hcd */
 	__msg("shell_disable_usb_irq\n");
    //shell_disable_usb_irq(); 
	__hcd_ops_stop();
 	__msg("shell_host_stop\n");
	shell_host_stop();


    
 	
	hcd_hardware_exit();
	
    __msg("peripheral_connect == 1\n");
    host_controller_exit();
    close_usb_clock_mode();

    init_enable_device();
    
    return 0;
}


