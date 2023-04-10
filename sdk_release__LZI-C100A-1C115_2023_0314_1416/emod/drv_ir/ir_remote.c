/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                             audio driver Module
*
*                                    (c) Copyright 2009-2010, kevin China
*                                             All Rights Reserved
*
* File    : ir_remote.c
* By      : victor
* Version : V1.0
* Date    : 2010-12-31
*********************************************************************************************************
*/

#include "drv_ir_remote_i.h"
#include "ir_remote.h"
#include "ir_remote_keymap.h"


__u32 *ir_solution_keymapping = NULL;

__ir_key_setting_t *pIrkey = NULL;
__u32 g_getIrKeyValue = 0;


/*
****************************************************************************************************
*
*  FunctionName:           lradckey_keydown
*
*  Description:
*              按键按下，调用ioctrl发送按键按下消息。回调函数。
*
*  Parameters:
*       keyvalue :  按键值
*
*  Return value:
*            无。
*  Notes:
*
****************************************************************************************************
*/
static  void Irkey_keydown(__u32 keyvalue)
{
	__ir_key_msg_t msg;
	g_getIrKeyValue = keyvalue | (1 << 15);
	msg.key_value = ir_solution_keymapping[keyvalue];
	msg.flag = 1;
	__wrn("\n\nIrkey_keydown_keyvalue=========%d\n\n", keyvalue);
	DEV_IR_KEY_Ioctrl(0, DRV_IRKEY_CMD_PUTKEY, 0, &msg);
	return;
}

/*
****************************************************************************************************
*
*  FunctionName:           lradckey_keyup
*
*  Description:
*              按键抬起，调用ioctrl发送按键抬起消息。回调函数。
*
*  Parameters:
*       keyvalue :  按键值
*
*  Return value:
*            无。
*  Notes:
*
****************************************************************************************************
*/
static  void Irkey_keyup(__u32 keyvalue)
{
	__ir_key_msg_t msg;
	g_getIrKeyValue = keyvalue;
	msg.key_value = ir_solution_keymapping[keyvalue];
	msg.flag = 0;
	__wrn("\n\nIrkey_keyup_keyvalue=========%d\n\n", keyvalue);
	DEV_IR_KEY_Ioctrl(0, DRV_IRKEY_CMD_PUTKEY, 0, &msg);
	return;
}

/*
****************************************************************************************************
*
*  FunctionName:           Lradc_key_Init
*
*  Description:
*             lradc 按键初始化。
*  Parameters:
*       无。
*
*  Return value:
*             成功返回EPDK_OK,失败返回EPDK_FAIL.
*  Notes:
*
****************************************************************************************************
*/
__s32 Ir_key_Init(void)
{
	if(pIrkey == NULL)
	{
		pIrkey = (__ir_key_setting_t *)eLIBs_malloc(sizeof(KEYC_SETTING));

		if(pIrkey == NULL)
		{
			__err("ir key driver memory malloc error!\n");
			return EPDK_FAIL;
		}

		eLIBs_memset(pIrkey, 0, sizeof(__ir_key_setting_t));
	}
	else
	{
		__wrn("ir key has been initial already!\n");
		return EPDK_FAIL;
	}

	//initial ir key
	pIrkey->irq_flags    = 0;
	pIrkey->irq_no       = IR_IRQ_NUM;
	pIrkey->irq_prio     = 0;
	pIrkey->remote_controller = NEC_REMOTE_CONTROLLER;
	pIrkey->user_code         = IR_NEC_ADDR_CODE;/* 如果为0则认为不检查用户码 */
	pIrkey->holdkey           = 0;
	pIrkey->rp_keytime = 130; /* repeat key interval times: 95ms */
	pIrkey->keyup_times = 3; /* detect key up times: 连续按285ms抬起,单键按190ms抬起 ms*/
	pIrkey->key_down = Irkey_keydown;
	pIrkey->key_up   = Irkey_keyup;

	// 不用的遥控码对应的功能要置为'KEY_RESERVED'
	switch(pIrkey->user_code)
	{
		case IR_NEC_ADDR_CODE:
		{
			ir_keymap[0x45]  = KPAD_MUTE;
			ir_keymap[0x46] = KPAD_TVOUT;
			ir_keymap[0x47] = KPAD_LR;
			ir_keymap[0x44] = KPAD_SUBMENU;
			ir_keymap[0x40] = KPAD_VOICEUP;
			ir_keymap[0x43] = KPAD_MENU;
			ir_keymap[0x07] = KPAD_LEFT;
			ir_keymap[0x15] = KPAD_ENTER;
			ir_keymap[0x09] = KPAD_RIGHT;
			ir_keymap[0x16] = KPAD_NUM0;
			ir_keymap[0x19] = KPAD_VOICEDOWN;
			ir_keymap[0x0d] = KPAD_MODE;
			ir_keymap[0x0c] = KPAD_NUM1;
			ir_keymap[0x18] = KPAD_NUM2;
			ir_keymap[0x5e] = KPAD_NUM3;
			ir_keymap[0x08] = KPAD_NUM4;
			ir_keymap[0x1c] = KPAD_NUM5;
			ir_keymap[0x5a] = KPAD_NUM6;
			ir_keymap[0x42] = KPAD_NUM7;
			ir_keymap[0x52] = KPAD_NUM8;
			ir_keymap[0x4a] = KPAD_NUM9;
			break;
		}
		default :
			break;
	}

	ir_solution_keymapping = ir_keymap;
	return BSP_IR_key_init(pIrkey);
}

/*
****************************************************************************************************
*
*  FunctionName:           Lradc_key_Exit
*
*  Description:
*              释放资源，调用bsp退出函数。
*
*  Parameters:
*       无。
*
*  Return value:
*            成功返回EPDK_OK,失败返回EPDK_FAIL
*  Notes:
*
****************************************************************************************************
*/
__s32 Ir_key_Exit(void)
{
	__inf("lradc key drv exit!\n");

	if(pIrkey)
	{
		BSP_IR_key_exit(pIrkey);
		eLIBs_free(pIrkey);
		pIrkey = NULL;
		return EPDK_OK;
	}

	return  EPDK_FAIL;
}
