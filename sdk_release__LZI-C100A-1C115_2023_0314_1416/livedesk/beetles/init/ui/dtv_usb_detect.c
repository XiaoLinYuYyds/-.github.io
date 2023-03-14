#include "dtv_usb_detect.h"

#define USB0_REG_BASE   (0xf1c13000)
#define USB1_REG_BASE   (0xf1c14000)
#define USB2_REG_BASE   (0xf1c1e000)

static __hdle shell_usb_port_base[USBC_MAX_CTL_NUM] = {USB0_REG_BASE/*, USB1_REG_BASE, USB2_REG_BASE*/};
__u32 shell_USBC_IsDpDmChange(__u32 port_no);
void shell_USBC_SelectBus(__u32 port_no, __u32 io_type, __u32 ep_type, __u32 ep_index);
void shell_USBC_ConfigFIFO_Base(__u32 port_no, __u32 fifo_mode);
void shell_USBC_ForceId(__u32 port_no, __u32 id_type);
void shell_USBC_DisableWakeIrq(__u32 port_no);
void shell_USBC_INT_ClearEpPendingAll(__u32 port_no, __u32 ep_type);
void shell_USBC_DisableWakeIrq(__u32 port_no);
void shell_USBC_DisableVbusChange(__u32 port_no);
void shell_USBC_DisableIdChange(__u32 port_no);
void shell_USBC_DisableDpDmChange(__u32 port_no);
void shell_USBC_DisableWakeIrq(__u32 port_no);

/* 禁用 vbus 中断 */
void shell_USBC_DisableVbusChange(__u32 port_no);

/* 禁用 id 中断 */
void shell_USBC_DisableIdChange(__u32 port_no);

/* 禁用 dpdm 中断 */
void shell_USBC_DisableDpDmChange(__u32 port_no);

/* 使能 wake 中断 */
void shell_USBC_EnableWakeIrq(__u32 port_no);

/* 使能 vbus 变化中断 */
void shell_USBC_EnableVbusChange(__u32 port_no);

/* 使能id变化中断 */
void shell_USBC_EnableIdChange(__u32 port_no);

/* 使能dmdp变化中断 */
void shell_USBC_EnableDpDmChange(__u32 port_no);



/* vbus, id, dpdm变化位是写1清零, 因此我们在操作其他bit的时候清除这些位 */
static __u32 __shell_WakeUpClearChangeDetect(__u32 reg_val)
{
    __u32 temp = reg_val;

	temp &= ~(1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT);
	temp &= ~(1 << USBC_BP_ISCR_ID_CHANGE_DETECT);
	temp &= ~(1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT);
	temp &= ~(1 << USBC_BP_ISCR_DPDM_PULLUP_EN);

	return temp;
}
/* 查询是否产生 vbus 中断 */
__u32 shell_USBC_IsVbusChange(__u32 port_no)
{
	__u32 reg_val = 0;
	__u32 temp = 0;

    //读取变化位的同时, 写1清除该位
    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
	
	temp = reg_val & (1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT);
	
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
    reg_val |= 1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT;
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));

	return temp;
}

/* 查询是否产生 id 中断 */
__u32 shell_USBC_IsIdChange(__u32 port_no)
{
	__u32 reg_val = 0;
	__u32 temp = 0;

    //读取变化位的同时, 写1清除该位
    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
	
	temp = reg_val & (1 << USBC_BP_ISCR_ID_CHANGE_DETECT);
	
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
    reg_val |= 1 << USBC_BP_ISCR_ID_CHANGE_DETECT;
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));

	return temp;
}
/* 查询是否产生 dpdm 中断 */
__u32 shell_USBC_IsDpDmChange(__u32 port_no)
{
	__u32 reg_val = 0;
	__u32 temp = 0;

    //读取变化位的同时, 写1清除该位
    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));

	temp = reg_val & (1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
    reg_val |= 1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT;

	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
	return temp;
}

/* 禁用 wake 中断 */
void shell_USBC_DisableWakeIrq(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val &= ~(1 << USBC_BP_ISCR_IRQ_ENABLE);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 禁用 vbus 中断 */
void shell_USBC_DisableVbusChange(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val &= ~(1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT_EN);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 禁用 id 中断 */
void shell_USBC_DisableIdChange(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val &= ~(1 << USBC_BP_ISCR_ID_CHANGE_DETECT_EN);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 禁用 dpdm 中断 */
void shell_USBC_DisableDpDmChange(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val &= ~(1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT_EN);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 使能 wake 中断 */
void shell_USBC_EnableWakeIrq(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val |= 1 << USBC_BP_ISCR_IRQ_ENABLE;
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 使能 vbus 变化中断 */
void shell_USBC_EnableVbusChange(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val |= 1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT_EN;
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 使能id变化中断 */
void shell_USBC_EnableIdChange(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val |= 1 << USBC_BP_ISCR_ID_CHANGE_DETECT_EN;
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

/* 使能dmdp变化中断 */
void shell_USBC_EnableDpDmChange(__u32 port_no)
{
	__u32 reg_val = 0;

    reg_val = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    reg_val |= 1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT_EN;
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(shell_usb_port_base[port_no]));
}

__u32 shell_usb_phy_new_tp_write(__u32 port_no,__u32 addr, __u32 data, __u32 len)
{
    __u32 temp, dtmp;
  	__u32 j=0;

  	dtmp = data;

  	for(j=0; j<len; j++)
  	{
  		USBC_Writeb(addr+j,USBC_REG_PHYCTL((shell_usb_port_base[port_no]))+1);
  		temp = USBC_Readb(USBC_REG_PHYCTL(shell_usb_port_base[port_no]));
  		temp &= ~(0x1<<7);
  		temp |= (dtmp&0x1)<<7;
  		temp &= ~(0x1);
  		USBC_Writeb(temp,USBC_REG_PHYCTL((shell_usb_port_base[port_no])));
  		temp |= (0x1);
  		USBC_Writeb(temp,USBC_REG_PHYCTL((shell_usb_port_base[port_no])));
  		temp &= ~(0x1);
  		USBC_Writeb(temp,USBC_REG_PHYCTL((shell_usb_port_base[port_no])));
  		dtmp >>= 1;
	}
	return data;
}

static void shell__USBC_ForceIdDisable(__u32 usbc_base_addr)
{
	__u32 reg_val = 0;

	//vbus, id, dpdm变化位是写1清零, 因此我们在操作其他bit的时候清除这些位
	reg_val = USBC_Readl(USBC_REG_ISCR(usbc_base_addr));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(usbc_base_addr));
}

static void shell__USBC_ForceIdToLow(__u32 usbc_base_addr)
{
	__u32 reg_val = 0;

	//先写00，后写10
	reg_val = USBC_Readl(USBC_REG_ISCR(usbc_base_addr));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val |= (0x02 << USBC_BP_ISCR_FORCE_ID);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(usbc_base_addr));
}

static void shell__USBC_ForceIdToHigh(__u32 usbc_base_addr)
{
	__u32 reg_val = 0;

	//先写00，后写10
	reg_val = USBC_Readl(USBC_REG_ISCR(usbc_base_addr));
	//reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val |= (0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(usbc_base_addr));
}

/* force id to (id_type) */
void shell_USBC_ForceId(__u32 port_no, __u32 id_type)
{
    
    switch(id_type){
		case USBC_ID_TYPE_HOST:
			shell__USBC_ForceIdToLow(shell_usb_port_base[port_no]);
		break;
		
		case USBC_ID_TYPE_DEVICE:
			shell__USBC_ForceIdToHigh(shell_usb_port_base[port_no]);
		break;
		
		default:
			shell__USBC_ForceIdDisable(shell_usb_port_base[port_no]);
		break;
	}
}

/* 获得tx ep中断标志位 */
static __u32 shell__USBC_INT_TxPending(__u32 usbc_base_addr)
{
    return (USBC_Readw(USBC_REG_INTTx(usbc_base_addr)));
}

/* 清除tx ep中断标志位 */
static void shell__USBC_INT_ClearTxPending(__u32 usbc_base_addr, __u8 ep_index)
{
    USBC_Writew((1 << ep_index), USBC_REG_INTTx(usbc_base_addr));
}

/* 清除所有tx ep中断标志位 */
static void shell__USBC_INT_ClearTxPendingAll(__u32 usbc_base_addr)
{
    USBC_Writew(0xffff, USBC_REG_INTTx(usbc_base_addr));
}

/* 获得rx ep中断标志位 */
static __u32 shell__USBC_INT_RxPending(__u32 usbc_base_addr)
{
    return (USBC_Readw(USBC_REG_INTRx(usbc_base_addr)));
}

/* 清除rx ep中断标志位 */
void shell__USBC_INT_ClearRxPending(__u32 usbc_base_addr, __u8 ep_index)
{
    USBC_Writew((1 << ep_index), USBC_REG_INTRx(usbc_base_addr));
}

/* 清除rx ep中断标志位 */
static void shell__USBC_INT_ClearRxPendingAll(__u32 usbc_base_addr)
{
    USBC_Writew(0xffff, USBC_REG_INTRx(usbc_base_addr));
}

/* 开某一个tx ep的中断 */
static void shell__USBC_INT_EnableTxEp(__u32 usbc_base_addr, __u8 ep_index)
{
    USBC_REG_set_bit_w(ep_index, USBC_REG_INTTxE(usbc_base_addr));
}

/* 开某一个rx ep的中断 */
static void shell__USBC_INT_EnableRxEp(__u32 usbc_base_addr, __u8 ep_index)
{
    USBC_REG_set_bit_w(ep_index, USBC_REG_INTRxE(usbc_base_addr));
}

/* 关某一个tx ep的中断 */
static void shell__USBC_INT_DisableTxEp(__u32 usbc_base_addr, __u8 ep_index)
{
    USBC_REG_clear_bit_w(ep_index, USBC_REG_INTTxE(usbc_base_addr));
}

/* 关某一个rx ep的中断 */
static void shell__USBC_INT_DisableRxEp(__u32 usbc_base_addr, __u8 ep_index)
{
    USBC_REG_clear_bit_w(ep_index, USBC_REG_INTRxE(usbc_base_addr));
}

/* 关所有的tx ep中断 */
static void shell__USBC_INT_DisableTxAll(__u32 usbc_base_addr)
{
    USBC_Writew(0, USBC_REG_INTTxE(usbc_base_addr));
}

/* 关所有的rx ep中断 */
static void shell__USBC_INT_DisableRxAll(__u32 usbc_base_addr)
{
    USBC_Writew(0, USBC_REG_INTRxE(usbc_base_addr));
}

/* 获得ep中断标志位 */
__u32 shell_USBC_INT_EpPending(__u32 port_no, __u32 ep_type)
{
	switch(ep_type){
		case USBC_EP_TYPE_EP0:
		case USBC_EP_TYPE_TX:
		    return shell__USBC_INT_TxPending(shell_usb_port_base[port_no]);
			
		case USBC_EP_TYPE_RX:
		    return shell__USBC_INT_RxPending(shell_usb_port_base[port_no]);

		default:
			return 0;
	}
}

/* 清除ep中断标志位 */
void shell_USBC_INT_ClearEpPendingAll(__u32 port_no, __u32 ep_type)
{
	switch(ep_type){
		case USBC_EP_TYPE_EP0:
		case USBC_EP_TYPE_TX:
		    shell__USBC_INT_ClearTxPendingAll(shell_usb_port_base[port_no]);
		break;

		case USBC_EP_TYPE_RX:
		    shell__USBC_INT_ClearRxPendingAll(shell_usb_port_base[port_no]);
		break;

		default:
			break;
	}

	return ;
}

/* 开某一个ep中断 */
void shell_USBC_INT_EnableEp(__u32 port_no, __u32 ep_type, __u8 ep_index)
{
	switch(ep_type){
		case USBC_EP_TYPE_TX:
		    shell__USBC_INT_EnableTxEp(shell_usb_port_base[port_no], ep_index);
		break;
		
		case USBC_EP_TYPE_RX:
		    shell__USBC_INT_EnableRxEp(shell_usb_port_base[port_no], ep_index);
		break;

		default:
        break;
	}
	
	return ;
}

/* 关某tx ep的中断 */
void shell_USBC_INT_DisableEp(__u32 port_no, __u32 ep_type, __u8 ep_index)
{
	switch(ep_type){
		case USBC_EP_TYPE_TX:
		    shell__USBC_INT_DisableTxEp(shell_usb_port_base[port_no], ep_index);
		break;
			
		case USBC_EP_TYPE_RX:
		    shell__USBC_INT_DisableRxEp(shell_usb_port_base[port_no], ep_index);
		break;

		default:
        break;
	}
	
	return;
}

/* 关某一个usb misc中断 */
void shell_USBC_INT_DisableUsbMiscUint(__u32 port_no, __u32 mask)
{
	__u32 reg_val = 0;
	reg_val = USBC_Readb(USBC_REG_INTUSBE(shell_usb_port_base[port_no]));
	reg_val &= ~mask;
	USBC_Writeb(reg_val, USBC_REG_INTUSBE(shell_usb_port_base[port_no]));
}

/* 关所有的ep中断 */
void shell_USBC_INT_DisableEpAll(__u32 port_no, __u32 ep_type)
{

	switch(ep_type){
		case USBC_EP_TYPE_TX:
		    shell__USBC_INT_DisableTxAll(shell_usb_port_base[port_no]);
		break;
		
		case USBC_EP_TYPE_RX:
		    shell__USBC_INT_DisableRxAll(shell_usb_port_base[port_no]);
		break;

		default:
        break;
	}
	
	return;
}

void printf_test(__u32 port_no)
{
//	printf_usbreg(shell_usb_port_base[port_no],0);
}

/* 关所有的usb misc中断 */
void shell_USBC_INT_DisableUsbMiscAll(__u32 port_no)
{
    USBC_Writeb(0, USBC_REG_INTUSBE(shell_usb_port_base[port_no]));
}

/* 获得usb misc中断标志位 */
__u32 shell_USBC_INT_MiscPending(__u32 port_no)
{
    return (USBC_Readb(USBC_REG_INTUSB(shell_usb_port_base[port_no])));
}

/* 清除所有usb misc中断标志位 */
void shell_USBC_INT_ClearMiscPendingAll(__u32 port_no)
{
    USBC_Writeb(0xff, USBC_REG_INTUSB(shell_usb_port_base[port_no]));
}

/* 开某一个usb misc中断 */
void shell_USBC_INT_EnableUsbMiscUint(__u32 port_no, __u32 mask)
{
	__u32 reg_val = 0;
	
	reg_val = USBC_Readb(USBC_REG_INTUSBE(shell_usb_port_base[port_no]));
	reg_val |= mask;
	USBC_Writeb(reg_val, USBC_REG_INTUSBE(shell_usb_port_base[port_no]));
}

void shell_USBC_SelectBus(__u32 port_no, __u32 io_type, __u32 ep_type, __u32 ep_index)
{
	__u32 reg_val = 0;
    reg_val = USBC_Readb(USBC_REG_VEND0(shell_usb_port_base[port_no]));
    if(io_type == USBC_IO_TYPE_DMA){
		if(ep_type == USBC_EP_TYPE_TX){
			reg_val |= ((ep_index - 0x01) << 1) << USBC_BP_VEND0_DRQ_SEL;  //drq_sel
			reg_val |= 0x1<<USBC_BP_VEND0_BUS_SEL;   //io_dma
		}else{
		    reg_val |= ((ep_index << 1) - 0x01) << USBC_BP_VEND0_DRQ_SEL;
			reg_val |= 0x1<<USBC_BP_VEND0_BUS_SEL;
		}
	}else{
	    //reg_val &= ~(0x1 << USBC_BP_VEND0_DRQ_SEL);  //清除drq_sel, 选择pio
	    reg_val &= 0x00;  //清除drq_sel, 选择pio
	}

	USBC_Writeb(reg_val, USBC_REG_VEND0(shell_usb_port_base[port_no]));

	
}

void shell_USBC_ConfigFIFO_Base(__u32 port_no, __u32 fifo_mode)
{
	__u32 ccu_base  = 0xf1c00000;
	__u32 reg_value = 0;
	reg_value = USBC_Readl(ccu_base + 0x04);
	reg_value &= ~(0x03 << 16);
	reg_value |= (1 << 16);
	USBC_Writel(reg_value, (ccu_base + 0x04));
}

__s32 shell_init_hcd_sys_cfg(__u32 port, usb_board_info_t *board_info, hcd_sys_cfg_t *hcd_sys_cfg)
{
    board_usb_port_t *usb_port = &(board_info->usb_port.port[port]);
    __u32 Pin_Dir_in = 1;    /* pin 的方向: 输出 */

    switch(usb_port->drv_vbus.group_type){
		case GROUP_TYPE_PIO:
			__msg("shell_init_hcd_sys_cfg GROUP_TYPE_PIO\n");
			hcd_sys_cfg->drv_vbus_pin_hdle = (void *)esPINS_PinGrpReq(&usb_port->drv_vbus.gpio_set, 1);
			if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL){
				__msg("ERR: init_hcd_sys_cfg: esPINS_PIOReq failed\n");
				return -1;
			}

		    esPINS_SetPinIO((__hdle)hcd_sys_cfg->drv_vbus_pin_hdle, Pin_Dir_in, 0);
			esPINS_SetPinPull((__hdle)hcd_sys_cfg->drv_vbus_pin_hdle, 0, 0);
			break;

		case GROUP_TYPE_P1:
			__msg("shell_init_hcd_sys_cfg GROUP_TYPE_P1\n");
			hcd_sys_cfg->drv_vbus_pin_hdle = (void *)eLIBs_fopen("b:\\HID\\POWER", "r+");
			if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL){
				__msg("ERR: init_hcd_sys_cfg: eLIBs_fopen p1 failed\n");
				return -1;
			}
			break;

		default:
			__msg("ERR: unkown group_type(%x)\n", usb_port->drv_vbus.group_type);
			return -1;
	}

	return 0;
}

/*
*******************************************************************************
*                     exit_hcd_sys_cfg
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__s32 shell_exit_hcd_sys_cfg(__u32 port, usb_board_info_t *board_info, hcd_sys_cfg_t *hcd_sys_cfg)
{
    board_usb_port_t *usb_port = &(board_info->usb_port.port[port]);
    __s32 ret = 0;

	if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL){
		__msg("ERR: drv_vbus_pin_hdle is NULL\n");
		return -1;
	}

	if(hcd_sys_cfg->drv_vbus_pin_hdle){
	    switch(usb_port->drv_vbus.group_type){
			case GROUP_TYPE_PIO:
			{
				__hdle pin_hdle = (__hdle)hcd_sys_cfg->drv_vbus_pin_hdle;

				ret = esPINS_PinGrpRel(pin_hdle , 0);
				if(ret != EPDK_OK){
		    		__wrn("ERR : free id hdle failed\n");
		    		return -1;
				}
			}
			break;

			case GROUP_TYPE_P1:
			{
				ES_FILE *pfile = (ES_FILE *)hcd_sys_cfg->drv_vbus_pin_hdle;

				ret = eLIBs_fclose(pfile);
			    if(ret == EPDK_FAIL){
			        __msg("ERR: hcd_close_p1_device: eLIBs_fclose failed\n");
			        return -1;
			    }
			}
			break;

			default:
				__msg("ERR: unkown group_type(%x)\n", usb_port->drv_vbus.group_type);
				return -1;
		}
	}

    return 0;
}

__s32 shell_usb_script_parse(usb_board_info_t *board_info)
{
	s32 ret = 0;
	u32 i = 0;
	char *set_usbc = NULL;

	/* usb_global_enable */
	ret = esCFG_GetKeyValue(SET_USB_PARA, KEY_USB_GLOBAL_ENABLE, (int *)&board_info->usb_global_enable, 1);
	if(ret != 0){
		__wrn("ERR: get usb_global_enable failed\n");
//		return -1;
	}

	/* usbc_num */
	ret = esCFG_GetKeyValue(SET_USB_PARA, KEY_USBC_NUM, (int *)&board_info->usbc_num, 1);
	if(ret != 0){
		__wrn("ERR: get usbc_num failed\n");
//		return -1;
	}

	for(i = 0; i < board_info->usbc_num; i++){
		if(i == 0){
			set_usbc = SET_USB0;
		}else if(i == 1){
			set_usbc = SET_USB1;
		}else{
			set_usbc = SET_USB2;
		}

		/* usbc enable */
		ret = esCFG_GetKeyValue(set_usbc, KEY_USB_ENABLE, (int *)&board_info->usb_port.port[i].valid, 1);
		if(ret != 0){
			__wrn("ERR: get usbc(%d) enable failed\n", i);
//			return -1;
		}

		/* usbc port type */
		ret = esCFG_GetKeyValue(set_usbc, KEY_USB_PORT_TYPE, (int *)&board_info->usb_port.port[i].port_type, 1);
		if(ret != 0){
			__wrn("ERR: get usbc(%d) port type failed\n", i);
//			return -1;
		}

		/* usbc detect type */
		ret = esCFG_GetKeyValue(set_usbc, KEY_USB_DETECT_TYPE, (int *)&board_info->usb_port.port[i].detect_type, 1);
		if(ret != 0){
			__wrn("ERR: get usbc(%d) detect type failed\n", i);
//			return -1;
		}

		/* usbc id */
		ret = esCFG_GetKeyValue(set_usbc, KEY_USB_ID_GPIO, (int *)&board_info->usb_port.port[i].id.gpio_set, 64);
		if(ret != 0){
			__wrn("ERR: get usbc(%d) id failed\n", i);
//			return -1;
		}

		/* usbc det_vbus */
		ret = esCFG_GetKeyValue(set_usbc, KEY_USB_DETVBUS_GPIO, (int *)&board_info->usb_port.port[i].det_vbus.gpio_set, 64);
		if(ret != 0){
			__wrn("ERR: get usbc(%d) id failed\n", i);
//			return -1;
		}

		/* usbc drv_vbus */
		ret = esCFG_GetKeyValue(set_usbc, KEY_USB_DRVVBUS_GPIO, (int *)&board_info->usb_port.port[i].drv_vbus.gpio_set, 64);
		if(ret != 0){
			__wrn("ERR: get usbc(%d) id failed\n", i);
//			return -1;
		}

	}

	return 0;
}

void shell_modify_usb_borad_info(usb_board_info_t *board_info)
{
    u32 i = 0;

	for(i = 0; i < board_info->usbc_num; i++){
		if(board_info->usb_port.port[i].id.gpio_set.port){
			board_info->usb_port.port[i].id.valid = 1;

			if(board_info->usb_port.port[i].id.gpio_set.port == 0xffff){
				board_info->usb_port.port[i].id.group_type = GROUP_TYPE_P1;
				board_info->usb_port.port[i].id.ex_pin_no =
					board_info->usb_port.port[i].id.gpio_set.port_num + POWER_EPT_1;
			}else{
				board_info->usb_port.port[i].id.group_type = GROUP_TYPE_PIO;
			}
		}

		if(board_info->usb_port.port[i].det_vbus.gpio_set.port){
			board_info->usb_port.port[i].det_vbus.valid = 1;

			if(board_info->usb_port.port[i].det_vbus.gpio_set.port == 0xffff){
				board_info->usb_port.port[i].det_vbus.group_type = GROUP_TYPE_P1;
				board_info->usb_port.port[i].det_vbus.ex_pin_no =
					board_info->usb_port.port[i].det_vbus.gpio_set.port_num + POWER_EPT_1;
			}else{
				board_info->usb_port.port[i].det_vbus.group_type = GROUP_TYPE_PIO;
			}
		}

		if(board_info->usb_port.port[i].drv_vbus.gpio_set.port){
			board_info->usb_port.port[i].drv_vbus.valid = 1;

			if(board_info->usb_port.port[i].drv_vbus.gpio_set.port == 0xffff){
				board_info->usb_port.port[i].drv_vbus.group_type = GROUP_TYPE_P1;
				board_info->usb_port.port[i].drv_vbus.ex_pin_no =
					board_info->usb_port.port[i].drv_vbus.gpio_set.port_num + POWER_EPT_1;
			}else{
				board_info->usb_port.port[i].drv_vbus.group_type = GROUP_TYPE_PIO;
			}
		}
	}

	return;
}
__s32 exit_hcd_sys_cfg(__u32 port, usb_board_info_t *board_info, hcd_sys_cfg_t *hcd_sys_cfg)
{
    board_usb_port_t *usb_port = &(board_info->usb_port.port[port]);
    __s32 ret = 0;

	if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL){
		__wrn("ERR: drv_vbus_pin_hdle is NULL\n");
		return -1;
	}

	if(hcd_sys_cfg->drv_vbus_pin_hdle){
	    switch(usb_port->drv_vbus.group_type){
			case GROUP_TYPE_PIO:
			{
				__hdle pin_hdle = (__hdle)hcd_sys_cfg->drv_vbus_pin_hdle;

				ret = esPINS_PinGrpRel(pin_hdle , 0);
				if(ret != EPDK_OK){
		    		__wrn("ERR : free id hdle failed\n");
		    		return -1;
				}
			}
			break;

			case GROUP_TYPE_P1:
			{
				ES_FILE *pfile = (ES_FILE *)hcd_sys_cfg->drv_vbus_pin_hdle;

				ret = eLIBs_fclose(pfile);
			    if(ret == EPDK_FAIL){
			        __wrn("ERR: hcd_close_p1_device: eLIBs_fclose failed\n");
			        return -1;
			    }
			}
			break;

			default:
				__wrn("ERR: unkown group_type(%x)\n", usb_port->drv_vbus.group_type);
				return -1;
		}
	}

    return 0;
}

void shell_get_usb_board_info(usb_board_info_t *board_info)
{
    __s32 ret = 0;

	ret = shell_usb_script_parse(board_info);
	if(ret != 0){
		__err("err: _cfg_parser failed\n");
	}
	shell_modify_usb_borad_info(board_info);
	return ;
}


__u32 shell_USBC_GetStatus_DpDm(__u32 port_no)
{
	__u32 temp = 0;
	__u32 dp = 0;
	__u32 dm = 0;
	temp = USBC_Readl(USBC_REG_ISCR(shell_usb_port_base[port_no]));
    temp |= 1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT;
	USBC_Writel(temp & ~USBC_BP_ISCR_DPDM_CHANGE_DETECT,USBC_REG_ISCR(shell_usb_port_base[port_no]));
	
	//eLIBs_printf("USBC_REG_ISCR reg_val = %08x\n",temp);
	
	dp = (temp >> USBC_BP_ISCR_EXT_DP_STATUS) & 0x01;
	dm = (temp >> USBC_BP_ISCR_EXT_DM_STATUS) & 0x01;
	if(dp && dm)
	{
		return (~temp >> USBC_BP_ISCR_DPDM_CHANGE_DETECT) & 0x01;
	}
	return (temp >> USBC_BP_ISCR_DPDM_CHANGE_DETECT) & 0x01;

}

void shell_USBC_Host_StartSession(__u32 port_no)
{
	USBC_REG_set_bit_b(USBC_BP_DEVCTL_SESSION, USBC_REG_DEVCTL(shell_usb_port_base[port_no]));
}

void shell_USBC_Host_EndSession(__u32 port_no)
{
	USBC_REG_clear_bit_b(USBC_BP_DEVCTL_SESSION, USBC_REG_DEVCTL(shell_usb_port_base[port_no]));
}

static void shell__USBC_ForceVbusValidDisable(__u32 usbc_base_addr)
{
	__u32 reg_val = 0;

	//先写00，后写10
	reg_val = USBC_Readl(USBC_REG_ISCR(usbc_base_addr));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(usbc_base_addr));
}

static void shell__USBC_ForceVbusValidToLow(__u32 usbc_base_addr)
{
	__u32 reg_val = 0;

	//先写00，后写10
	reg_val = USBC_Readl(USBC_REG_ISCR(usbc_base_addr));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val |= (0x02 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(usbc_base_addr));
}

static void shell__USBC_ForceVbusValidToHigh(__u32 usbc_base_addr)
{
	__u32 reg_val = 0;

	//先写00，后写11
	reg_val = USBC_Readl(USBC_REG_ISCR(usbc_base_addr));
	//reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val |= (0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val = __shell_WakeUpClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(usbc_base_addr));
}

/* force vbus valid to (id_type) */
void shell_USBC_ForceVbusValid(__u32 port_no, __u32 vbus_type)
{
    switch(vbus_type){
		case USBC_VBUS_TYPE_LOW:
			shell__USBC_ForceVbusValidToLow(shell_usb_port_base[port_no]);
		break;

		case USBC_VBUS_TYPE_HIGH:
			shell__USBC_ForceVbusValidToHigh(shell_usb_port_base[port_no]);
		break;

		default:
			shell__USBC_ForceVbusValidDisable(shell_usb_port_base[port_no]);
	}
}

