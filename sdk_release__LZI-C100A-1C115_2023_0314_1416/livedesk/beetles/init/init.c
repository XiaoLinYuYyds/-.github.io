/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    		: init.c
* By      		: Andy.zhang
* Func		: init thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"
#include "ui\\init_server.h"
#include "ui\\tp_adjust_scene\\scene_adjust.h"
#if 1
#include "bmp_parser\\Parse_Picture.h"
#endif

#define XML_PATH	BEETLES_APP_ROOT"apps\\desktop"

/* DBMS module path */
#define	 DBMS_MOD_PATH	BEETLES_APP_ROOT"mod\\dbms.mod"

#define TIMEOUT_ID_TIMER_GET_DEBUG_INFO 	10

#if TIMEOUT_ID_TIMER_GET_DEBUG_INFO
	#define ID_TIMER_GET_DEBUG_INFO 			0x1000
#endif

#define TIMEOUT_ID_TIMER_GET_POWER				10
#define ID_TIMER_GET_POWER					0x1001

#define TIMEOUT_ID_TIMER_PAINT						50
#define ID_TIMER_PAINT						0x1002


extern GUI_FONT    *SWFFont;
extern __u8 g_b_lowpower;

static H_WIN init_mainwin;
static int 		ModDBMSID;
static __mp    *ModDBMS;
__bool lz_drv_fail_flag = 0;
__u32 g_logoDisable = 0;


__s32 show_charge_scene(void);
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
	__msg("tmp_data=0x%x\n", tmp_data);
	ret = (tmp_data & (1 << port_num)) >> port_num;
	return ret;
}

static __s32 get_gpio_out_status(__u32 port, __u32 port_num)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__s32               ret;
	__u32               tmp1;
	__u32               tmp2;
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

//设置gpio为输入
//pull: 0:高阻 1:上拉  2:下拉
static void set_input_gpio(__u32 port, __u32 port_num, __s32 pull)
{
	volatile  __u32    *tmp_addr;
	__u32               tmp_data;
	__u32               tmp1;
	__u32               tmp2;
	//设置为输入
	tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
	tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x07 << tmp1);
	//tmp_data |=  (0x01 << tmp1);
	*tmp_addr = tmp_data;
	//设置pull属性
	tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
	tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
	tmp_data = *tmp_addr;
	tmp_data &= ~(0x03 << tmp2);
	tmp_data |= (pull << tmp2);
	*tmp_addr = tmp_data;
}

//gpio引脚配置输出
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



static __s32 _install_dbms_mod(void)
{
	/* install dbms module */
	ModDBMSID = esMODS_MInstall(DBMS_MOD_PATH, 0);

	if(ModDBMSID == 0)
	{
		__wrn("Install dbms module failed");
		return EPDK_FAIL;
	}

	__inf("Install dbms module succeeded\n");
	/* open dbms module */
	ModDBMS = esMODS_MOpen(ModDBMSID, 0);

	if(ModDBMS == (__mp *)0)
	{
		__wrn("Open dbms module failed");
		return EPDK_FAIL;
	}

	__inf("Open dbms module succeeded\n");
	return EPDK_OK;
}


static void _uninstall_dbms_mod(void)
{
	/* uninstall dbms module    */
	if(ModDBMS)
	{
		esMODS_MClose(ModDBMS);
	}

	if(ModDBMSID)
	{
		esMODS_MUninstall(ModDBMSID);
	}

	ModDBMS   = NULL;
	ModDBMSID = 0;
}
static void _close_audio_dev(void)
{
	ES_FILE	  *p_dac;
	p_dac = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");

	if(!p_dac)
	{
		__err("open dac device fail!\n");
	}

	eLIBs_fioctrl(p_dac, AUDIO_DEV_CMD_CLOSE_DEV, AUDIO_DEV_PLAY, 0);
	eLIBs_fclose(p_dac);
}

static void _close_lcd(void)
{
	ES_FILE *disp;
	__u32 outputtype;
	__u32 	arg[3];
	disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

	if(!disp)
	{
		__wrn("open display fail\n");
		return ;
	}

	outputtype = eLIBs_fioctrl(disp, DISP_CMD_GET_OUTPUT_TYPE, 0, (void *)arg);

	if(outputtype == DISP_OUTPUT_TYPE_LCD)
	{
		arg[0] = 0;
		arg[1] = 0;
		arg[2] = 0;
		eLIBs_fioctrl(disp, DISP_CMD_LCD_OFF, 0, (void *)arg);
	}

	eLIBs_fclose(disp);
}

static void _framework_init(void)
{
	AppletInfoManager	*info_manager = NULL;
	ActivityManager 	*activity_manager = NULL;
	info_manager = applet_info_manager_create();
	applet_info_manager_load_dir(info_manager, XML_PATH);
	//applet_info_manager_printf(info_manager);		// 打印applet info 信息
	applet_info_manager_set_defualt(info_manager);
	activity_manager = activity_manager_create();
	activity_manager_set_default(activity_manager);
}

static void _framework_exit(void)
{
	AppletInfoManager 	*info_manager = applet_info_manager_get_default();
	ActivityManager 	*activity_manager = activity_manager_get_default();
	activity_manager_destroy(activity_manager);
	applet_info_manager_destroy(info_manager);
	applet_info_manager_set_defualt(NULL);
	activity_manager_set_default(NULL);
}


#if SP_DEV_KEY_NUMLOCK
#if (PRJ_KB_ID == KB_TAK_KEY718)
const __u32	gc_Shift__GUI_MSG_KEY__NUM0_9[] =
{
	GUI_MSG_KEY_LOCKSCREEN,
	GUI_MSG_KEY_AUDIO,
	GUI_MSG_KEY_FM,
	GUI_MSG_KEY_MENU,
	GUI_MSG_KEY_VADD,
	GUI_MSG_KEY_VDEC,
	GUI_MSG_KEY_VIDEO,
	GUI_MSG_KEY_RECORD,
	GUI_MSG_KEY_LEFT,
	GUI_MSG_KEY_RIGHT,
};
#elif (PRJ_KB_ID == KB_TAK_KEY758)
const __u32	gc_Shift__GUI_MSG_KEY__NUM0_9[] =
{
	GUI_MSG_KEY_MENU,
	GUI_MSG_KEY_VDEC,
	GUI_MSG_KEY_VADD,
	GUI_MSG_KEY_VIDEO,
	GUI_MSG_KEY_AUDIO,
	GUI_MSG_KEY_FM,
	GUI_MSG_KEY_RECORD,
	GUI_MSG_KEY_LEFT,
	GUI_MSG_KEY_RIGHT,
	GUI_MSG_KEY_SUBMENU,
};
#endif
#endif

__s32 Preprocess_GUI_MSG_KEY(__gui_msg_t *msg)
{
	reg_system_para_t *para;
	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

	if(!para)
	{
		__wrn("invalid para...\n");
		return EPDK_FAIL;
	}

	if(msg->dwAddData1 & MASK_GUI_MSG_KEY_TYPE_IR)
	{
		// 获取IR按键标志，还原按键值。
		msg->dwAddData1 &= ~MASK_GUI_MSG_KEY_TYPE_IR;
		para->m_maskFlags |= MASK_SysFlags_IsIrKey;
		return EPDK_OK;
	}
	
	para->m_maskFlags &= ~MASK_SysFlags_IsIrKey;
	
#if SP_DEV_KEY_REUSE
#if MASK_SysFlags_ShiftNum
	if(para->m_maskFlags & MASK_SysFlags_ShiftNum)
	{
		// 将数字键转换为功能键
		if((GUI_MSG_KEY_NUM0 <= msg->dwAddData1) && (msg->dwAddData1 <= GUI_MSG_KEY_NUM9))
		{
			msg->dwAddData1 = gc_Shift__GUI_MSG_KEY__NUM0_9[msg->dwAddData1 - GUI_MSG_KEY_NUM0];
		}
		else if((GUI_MSG_KEY_LONGNUM0 <= msg->dwAddData1) && (msg->dwAddData1 <= GUI_MSG_KEY_LONGNUM9))
		{
			msg->dwAddData1 = gc_Shift__GUI_MSG_KEY__NUM0_9[msg->dwAddData1 - GUI_MSG_KEY_LONGNUM0]
			                  + (GUI_MSG_KEY_LONGNUM0 - GUI_MSG_KEY_NUM0);
		}
	}

#endif
#endif
	return EPDK_OK;
}

extern __s8 g_tconOccupyLcmi_flag;
__s32 key_hook_cb(__gui_msg_t *msg)
{
	if(EPDK_FAIL == Preprocess_GUI_MSG_KEY(msg))
	{
		msg->dwAddData1 &= ~MASK_GUI_MSG_KEY_TYPE_IR;
	}

	if(init_mainwin)
	{
		__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);
		init_lock(msg);

		if(!init_ctr->key_msg_enble)				// 禁止按键
		{
			init_unlock(msg);
			return -1;
		}

#if TIMEOUT_ID_TIMER_KEYLED_OFF
		{
			IOCTRL__LEDKEY__SET_ON();

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_KEYLED_OFF))
			{
				GUI_ResetTimer(msg->h_deswin, ID_TIMER_KEYLED_OFF, TIMEOUT_ID_TIMER_KEYLED_OFF, NULL);
			}
			else
			{
				GUI_SetTimer(msg->h_deswin, ID_TIMER_KEYLED_OFF, TIMEOUT_ID_TIMER_KEYLED_OFF, NULL);
			}
		}
#endif
#if 1

		if(
		  (msg->dwAddData1 != GUI_MSG_KEY_LOCKSCREEN) &&
		  (msg->dwAddData1 != GUI_MSG_KEY_LONGLOCKSCREEN)
		)
		{
			if(g_tconOccupyLcmi_flag == -1)
			{
				init_unlock(msg);

				if(/*GUI_MSG_KEY_ENTER == msg->dwAddData1 && */KEY_UP_ACTION == msg->dwAddData2)
				{
					__here__;
					g_tconOccupyLcmi_flag = 1;
					NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
				}

				__here__;
				return 0;
			}
			else if(g_tconOccupyLcmi_flag == 0)
			{
				if(init_ctr->screen_close_done) // 已经关屏
				{
					init_unlock(msg);

					if(/*GUI_MSG_KEY_ENTER == msg->dwAddData1 && */KEY_UP_ACTION == msg->dwAddData2)
					{
						__here__;
						NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
					}

					__here__;
					return 0;
				}

				if(init_ctr->screen_close_done)
				{
					init_unlock(msg);
					return -1;
				}
			}
		}

#endif
		init_unlock(msg);
		esPWM_UsrEventNotify();
		__msg("msg->h_deswin=%x msg->id=%d msg->dwAddData1=%d msg->dwAddData2=%d\n"
		      , msg->h_deswin, msg->id, msg->dwAddData1, msg->dwAddData2);
	}

	__msg("msg->dwReserved=%d\n", msg->dwReserved);
	GUI_PostMessage(msg);
	{
		//按键enter键down和up键同时到达，导致消息丢失，之间加一个延时
		if(GUI_MSG_KEY_ENTER == msg->dwAddData1
		    && KEY_DOWN_ACTION == msg->dwAddData2)
		{
			esKRNL_TimeDly(10);
		}
	}
	return 0;
}

/* tp msg hook */
__s32 tp_hook_cb(__gui_msg_t *msg)
{
	if(init_mainwin)
	{
		__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);
		init_lock(msg);

		if(!init_ctr->tp_msg_enble)			// 禁止 touch
		{
			init_unlock(msg);
			return -1;
		}

		if(init_ctr->screen_close_done)
		{
			init_unlock(msg);
			return -1;
		}

		init_unlock(msg);
		esPWM_UsrEventNotify();
		//__msg("msg->h_deswin = %x, msg->id = %d\n", msg->h_deswin, msg->id);
	}

	__here__;
	GUI_PostMessage(msg);
	return 0;
}

/* desktop msg hook */
__s32 init_hook_cb(__gui_msg_t *msg)
{
	if(init_mainwin)
	{
		__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);

		/* 请求开屏 */
		if(init_ctr->screen_close_done)
		{
			NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
		}
	}

	__here__;
	GUI_PostMessage(msg);
	//GUI_SendNotifyMessage(msg);
	__here__;
	return 0;
}

static void _process_init(void)
{
	H_WIN			scene_adjust;
	/* orchid */
	//	dsk_orchid_createDisk();
	//	dsk_orchid_check();
	/* framework init */
	_framework_init();

#if !BOOT_FROM_SDMMC
	/* load main*/
	activity_load_app("application://app_root");
#endif
}

static void _process_exit(void)
{
	bookengine_close();
	_uninstall_dbms_mod();
	//卸载USB
	//	App_RmmodUsbHostDriver(1);
	//	App_RmmodUsbHostDriver(2);
	/* deinit message server */
	msg_emit_deinit();
	/* framework deinit */
	_framework_exit();
	/* close device */
	_close_audio_dev();
	_close_lcd();
	{
		ES_FILE 			 *p_key = NULL;
		/* open display driver */
		//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
		p_key = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");
		__here__;

		//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
		if(p_key == NULL)
		{
			__wrn("Error in opening key driver.\n");
		}
		else
		{
			//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
			eLIBs_fioctrl(p_key, DRV_KEY_CMD_SET_POWER_FLAGS, 0, 0);
			//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
			eLIBs_fclose(p_key);
		}
	}
	{
		ES_FILE 			 *ir_key = NULL;
		/* open display driver */
		//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
		ir_key = eLIBs_fopen("b:\\INPUT\\IR_KEY", "rb");
		__here__;

		//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
		if(ir_key == NULL)
		{
			__wrn("Error in opening key driver.\n");
		}
		else
		{
			//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
			eLIBs_fioctrl(ir_key, DRV_IRKEY_CMD_SET_POWER_FLAGS, 0, 0);
			//eLIBs_printf("++++++++  %s, %d\n  +++++\n", __FILE__, __LINE__);
			eLIBs_fclose(ir_key);
		}
	}
	/* plug out driver  */
	//	esDEV_Plugout("\\drv\\mp.drv", 0);
	//	esDEV_Plugout("\\drv\\touchpanel.drv", 0);
	//	esDEV_Plugout("\\drv\\orchid.drv",0);
	esDEV_Plugout("\\drv\\audio.drv", 0);
	//    esDEV_Plugout("\\drv\\display.drv", 0 );
	//	esDEV_Plugout("\\drv\\ir.drv", 0 );
	//	App_WaitForUsbHostReomveComplete(1);
	//	App_WaitForUsbHostReomveComplete(2);
}


#define CHARGE_BMP_NUM 4
typedef struct
{
	H_LYR hlayer;
	__s32 bmp_id[CHARGE_BMP_NUM];
	HTHEME bmp_hdl[CHARGE_BMP_NUM];
	__s8		m_lowPower, m_dispOff;
}
chagre_attr_t;

void init_check_power_off(void)
{
#if (!SP_DEV_HW_PW_OFF)
	IOCTRL__PWOFF_DET__SET_OFF();
	{
		__u32 i = 100;

		while(i--);
	}

	if(IOCTRL__PWOFF_DET__IS_OFF())
	{
		reg_system_para_t	 *system_para;
		system_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(system_para->m_ePowerOffType == m_ePowerOffType_lowPower)
		{
			system_para->m_ePowerOffType = m_ePowerOffType_normal;
			dsk_reg_flush();
		}

		while(1)
		{
			dsk_display_lcd_off(OP_SCREENOFF_AND_BKOFF);
			esKRNL_TimeDly(10);
			IOCTRL__PWON__SET_OFF();
		}
	}
#endif
}

__s8 init_power_is_low(void)
{
	if(dsk_get_charge_state() == EPDK_FALSE)
	{
		__u8 powerLevel = 0x80, lowPower;
		dsk_power_get_battery_level(&powerLevel);
		__wrn("powerLevel=0x%02X\n", powerLevel);
		lowPower = powerLevel >> 6;

		if(lowPower == 2)
		{
			return 2;
		}

		powerLevel &= 0x0F;

		if(1)
		{
			reg_system_para_t	 *system_para;
			system_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
			if(system_para)
			{
				if(system_para->m_ePowerOffType == m_ePowerOffType_lowPower)
				{
					if(powerLevel >= DSK_POWER_LEVEL_1) // 3.58V
					{
						system_para->m_ePowerOffType = m_ePowerOffType_normal;
						return 0;
					}
				
					return 1;
				}
			}
		}

		if(1)
		{
			if(lowPower)
			{
				return 1;
			}
		}
	}

	return 0;
}

static H_LYR charge_scene_32bpp_layer_create(RECT *LayerRect)
{
	H_LYR layer = NULL;
	FB  fb =
	{
		{0, 0},                                   						/* size      */
		{0, 0, 0},                                      				/* buffer    */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    	/* fmt       */

	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_NORMAL,
		0,                                              /* ck_mode   */
		0,                                              /* alpha_en  */
		0,                                              /* alpha_val */
		1,                                              /* pipe      */
		0xff,                                           /* prio      */
		{0, 0, 0, 0},                           		/* screen    */
		{0, 0, 0, 0},                               	/* source    */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
		NULL                                            /* fb        */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"charge scene",
		NULL,
		GUI_LYRWIN_STA_SLEEP,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width			= LayerRect->width;
	fb.size.height			= LayerRect->height;
	fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= LayerRect->width;
	lstlyr.src_win.height   = LayerRect->height;
	lstlyr.scn_win.x		= LayerRect->x;
	lstlyr.scn_win.y		= LayerRect->y;
	lstlyr.scn_win.width  	= LayerRect->width;
	lstlyr.scn_win.height 	= LayerRect->height;
	lstlyr.pipe 			= 1;
	lstlyr.fb 				= &fb;
	lyrcreate_info.lyrpara 	= &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__wrn("desktop scene layer create error !\n");
	}

	return layer;
}

#if TIMEOUT_ID_TIMER_GET_DEBUG_INFO
void __GetDebugInfo(__s8 option)
{
	char str[32] = "";
	int x = 0, y = 0, w = 16 * 4, h = 16 * 1, yBk = h*2;

	if(option == 1)
	{		
		x = 0;	y = 0;
		w = 8 * 11; // 0x10010000
		GUI_ClearRect(x, y, x + w - 1, y + h - 1);
		sprintf(str, "0x%08lX", PRJ_MB_ID);
		GUI_DispStringAt(str, x, y);
		
		x += w;
		GUI_ClearRect(x, y, x + w - 1, y + h - 1);
		sprintf(str, "0x%08lX", PRJ_KB_ID);
		GUI_DispStringAt(str, x, y);
		x += w * 2;
		eLIBs_strcpy(str, "<-- Prj ID");
		GUI_DispStringAt(str, x, y);
		w = 16 * 4;
	}
	
	{
		__s32 ret = EPDK_FAIL;
		S_ADSampling	sAdSampling;
		{
			ES_FILE *pKeyFile;
			pKeyFile = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");

			if(!pKeyFile)
			{
				__wrn("key connot be openned!\n");
			}
			else
			{
				x = 0;
				y = yBk;
				ret = eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_GET_DEBUG_INFO, OP_GetDebugInfo_X1, (void *)(&sAdSampling));
				if(ret == EPDK_OK)
				{
					if(option == 1)
					{
						int	i;
						GUI_DispStringAt("X1 Key AD Ref :", x, y);						
						
						y += h;
						yBk = y;
						GUI_DispStringAt("Min", x, y);

						for(i = 0; i < sAdSampling.psKeypadAD_size; i++)
						{
							y += h;
							sprintf(str, "%d", sAdSampling.psKeypadAD[i].min);
							GUI_DispStringAt(str, x, y);
						}

						y = yBk;
						x += w;
						GUI_DispStringAt("Max", x, y);

						for(i = 0; i < sAdSampling.psKeypadAD_size; i++)
						{
							y += h;
							sprintf(str, "%d", sAdSampling.psKeypadAD[i].max);
							GUI_DispStringAt(str, x, y);
						}
					}

					{
						x = 0;
						y = h *16;
						if(sAdSampling.m_cur >= 0)
						{
							GUI_ClearRect(x, y, x + w - 1, y + h - 1);
							sprintf(str, "%d", sAdSampling.m_cur);
							GUI_DispStringAt(str, x, y);
						}
						
						x += w;						
						if(sAdSampling.m_min >= 0)
						{
							GUI_ClearRect(x, y, x + w - 1, y + h - 1);
							sprintf(str, "%d", sAdSampling.m_min);
							GUI_DispStringAt(str, x, y);
						}
						
						x += w;						
						if(sAdSampling.m_max >= 0)
						{
							GUI_ClearRect(x, y, x + w - 1, y + h - 1);
							sprintf(str, "%d", sAdSampling.m_max);
							GUI_DispStringAt(str, x, y);
						}
						
						x += w;
						eLIBs_strcpy(str, "<-- X1");
						GUI_DispStringAt(str, x, y);
					}					
				}

				ret = eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_GET_DEBUG_INFO, OP_GetDebugInfo_X2, (void *)(&sAdSampling));

				if(ret == EPDK_OK)
				{
					if(option == 1)
					{
						int	i;
						x = w *3;	y = 2*h;
						GUI_DispStringAt("X2 Key AD Ref :", x, y);						
						
						y += h;
						yBk = y;
						GUI_DispStringAt("Min", x, y);

						for(i = 0; i < sAdSampling.psKeypadAD_size; i++)
						{
							y += h;
							sprintf(str, "%d", sAdSampling.psKeypadAD[i].min);
							GUI_DispStringAt(str, x, y);
						}

						y = yBk;
						x += w;
						GUI_DispStringAt("Max", x, y);

						for(i = 0; i < sAdSampling.psKeypadAD_size; i++)
						{
							y += h;
							sprintf(str, "%d", sAdSampling.psKeypadAD[i].max);
							GUI_DispStringAt(str, x, y);
						}
					}

					{
						x = 0;
						y = h *17;
						if(sAdSampling.m_cur >= 0)
						{
							GUI_ClearRect(x, y, x + w - 1, y + h - 1);
							sprintf(str, "%d", sAdSampling.m_cur);
							GUI_DispStringAt(str, x, y);
						}
						
						x += w;						
						if(sAdSampling.m_min >= 0)
						{
							GUI_ClearRect(x, y, x + w - 1, y + h - 1);
							sprintf(str, "%d", sAdSampling.m_min);
							GUI_DispStringAt(str, x, y);
						}
						
						x += w;						
						if(sAdSampling.m_max >= 0)
						{
							GUI_ClearRect(x, y, x + w - 1, y + h - 1);
							sprintf(str, "%d", sAdSampling.m_max);
							GUI_DispStringAt(str, x, y);
						}
						
						x += w;
						eLIBs_strcpy(str, "<-- X2");
						GUI_DispStringAt(str, x, y);
					}					
				}

				eLIBs_fclose(pKeyFile);
			}
		}
	}

	{
		__u8 	powerLevel = 0x80;
		__s32	voltage;
		voltage = dsk_power_get_battery_level(&powerLevel);

		if(voltage >= 0)
		{
			x = 0;
			y = h * 18;			
			GUI_ClearRect(x, y, x + w - 1, y + h - 1);
			sprintf(str, "%d", voltage);
			GUI_DispStringAt(str, x, y);
			x += w;
			GUI_ClearRect(x, y, x + w - 1, y + h - 1);
			sprintf(str, "%d", powerLevel & 0x0F);
			GUI_DispStringAt(str, x, y);
			x += w * 2;
			eLIBs_strcpy(str, "<-- Vbat");
			GUI_DispStringAt(str, x, y);
		}
	}
}

void __PaintDebugInfo(__gui_msg_t *msg, __s8 option)
{
	chagre_attr_t *pattr;
	pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

	if(NULL == pattr)
	{
		return;
	}

	GUI_LyrWinSetSta(pattr->hlayer, GUI_LYRWIN_STA_ON);
	GUI_LyrWinSel(pattr->hlayer);

	if(SWFFont)
	{
		GUI_SetFont(SWFFont);
	}

	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_CharSetToEncode(EPDK_CHARSET_ENM_GBK);
	GUI_SetColor(GUI_RED);
	__GetDebugInfo(option);
}
#endif

#if TIMEOUT_ID_TIMER_GET_DEBUG_INFO
void __SetTimer_GetDebugInfo(__gui_msg_t *msg)
{
	__s32 ret = EPDK_FAIL;
	S_ADSampling	sAdSampling;
	
	{
		ES_FILE *pKeyFile;
		pKeyFile = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");

		if(!pKeyFile)
		{
			__wrn("key connot be openned!\n");
		}
		else
		{
			ret = eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_GET_DEBUG_INFO, OP_GetDebugInfo_X1, (void *)(&sAdSampling));
			eLIBs_fclose(pKeyFile);
		}
	}

	if(ret == EPDK_OK)
	{
		if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_DEBUG_INFO))
		{
			GUI_SetTimer(msg->h_deswin, ID_TIMER_GET_DEBUG_INFO, TIMEOUT_ID_TIMER_GET_DEBUG_INFO, NULL);
		}
		else
		{
			GUI_ResetTimer(msg->h_deswin, ID_TIMER_GET_DEBUG_INFO, TIMEOUT_ID_TIMER_GET_DEBUG_INFO, NULL);
		}

		__PaintDebugInfo(msg, 1);
	}
}
#endif

#if TIMEOUT_ID_TIMER_PAINT
static __s32 charge_paint(__gui_msg_t *msg)
{
	static __s32 index = 1;
	chagre_attr_t *pattr;
	void *pbmp;
	HBMP  hbmp;
	__s32 width;
	__s32 height;
	__s32 screen_width;
	__s32 screen_height;
	GUI_MEMDEV_Handle hmem_dev;
	//esMEMS_Info();
	pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

	if(NULL == pattr)
	{
		return EPDK_OK;
	}

	if(pattr->m_lowPower == 1)
	{
		index = 0;
	}

	__here__;
	dsk_display_get_size(&screen_width, &screen_height);
	__wrn("index=%d, pattr->bmp_hdl[index]=%x\n", index, pattr->bmp_hdl[index]);
	pbmp = dsk_theme_hdl2buf(pattr->bmp_hdl[index]);

	if(NULL == pbmp)
	{
		__wrn("pbmp is null...\n");
		return EPDK_OK;
	}

	__here__;
	hbmp = bmp_open(pbmp);

	if(NULL == hbmp)
	{
		return EPDK_OK;
	}

	__here__;
	width = bmp_get_width(hbmp);
	height = bmp_get_height(hbmp);
	__wrn("width=%d, height=%d\n", width, height);

	if(hbmp)
	{
		bmp_close(hbmp);
		hbmp = NULL;
	}

	__here__;
	GUI_LyrWinSetSta(pattr->hlayer, GUI_LYRWIN_STA_ON);
	GUI_LyrWinSel(pattr->hlayer);

	if(SWFFont)
	{
		GUI_SetFont(SWFFont);
	}

	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
	GUI_SetColor(GUI_RED);
	hmem_dev = GUI_MEMDEV_Create((screen_width - width) / 2, (screen_height - height) / 2, width, height);

	if(NULL == hmem_dev)
	{
		GUI_MEMDEV_Select(NULL);
	}
	else
	{
		GUI_MEMDEV_Select(hmem_dev);
	}

	GUI_BMP_Draw(pbmp, (screen_width - width) / 2, (screen_height - height) / 2);

	if(hmem_dev)
	{
		GUI_MEMDEV_CopyToLCD(hmem_dev);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(hmem_dev);
		hmem_dev = NULL;
	}

	{
		GUI_RECT gui_rect;
		char str[64];
		__s32	strIdx;

		if(pattr->m_lowPower)
		{
			strIdx = STRING_LOW_POWER;
		}
		else
		{
			strIdx = STRING_INIT_ENTER_HOME;
		}

		gui_rect.x0 = 0;
		gui_rect.y0 = screen_height - 32;
		gui_rect.x1 = gui_rect.x0 + screen_width - 1;
		gui_rect.y1 = gui_rect.y0 + screen_height - 1;
		GUI_ClearRectEx(&gui_rect);
		dsk_langres_get_menu_text(strIdx, str, sizeof(str));
		GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_TOP);
	}

	if(pattr->m_lowPower == 0)
	{
		if(++index >= CHARGE_BMP_NUM)//循环切换
		{
			index = 1;
		}

		if(EPDK_TRUE == dsk_power_is_full_charge())//如果充满了，就停留在最后一张
		{
			index = CHARGE_BMP_NUM - 1;
			//pull_down_gpio(3, 8);
			//pull_up_gpio(4, 2);
		}
		else
		{
			//pull_up_gpio(3, 8);
			//pull_down_gpio(4, 2);
		}
	}

	return EPDK_OK;
}
#endif

#if TIMEOUT_ID_TIMER_GET_POWER
void show_charge_timer_get_power(__gui_msg_t *msg)
{
	static __u8 	s_powerNormalCnt = 0;
	static __s8 	s_lowPower_bk = -1;
	static __u32 	s_timerCnt = 0;
	__s8			powerState;
	chagre_attr_t *pattr;
	pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

	if(NULL == pattr)
	{
		return;
	}

	init_check_power_off();
	powerState = init_power_is_low();
	__msg("powerState = %d, s_powerNormalCnt = %d\n", powerState, s_powerNormalCnt);

	switch(powerState)
	{
			// 检测低电
		case 2 :
			pattr->m_lowPower = 2;
			s_powerNormalCnt = 0;
			break;

		case 1 :
			pattr->m_lowPower = 1;
			s_powerNormalCnt = 0;
			break;

		case 0 :
			if(EPDK_FALSE == dsk_get_charge_state())
			{
				//有电且没充电了， 准备退出开机充电界面
				pattr->m_lowPower = -1;
				dsk_display_lcd_on();
				pattr->m_dispOff = 0;	// 亮屏，但是不更新电量图标
				s_timerCnt = 0;

				if(++s_powerNormalCnt >= 10)
				{
					__gui_msg_t msgex;
					eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
					msgex.id = GUI_MSG_CLOSE;
					msgex.h_deswin = msg->h_deswin;
					GUI_SendNotifyMessage(&msgex);
				}
			}
			else
			{
				pattr->m_lowPower = 0;
			}

			break;

		default :
			break;
	}

	if(s_lowPower_bk != pattr->m_lowPower)
	{
		// 低电状态发生改变时，亮屏
		s_lowPower_bk = pattr->m_lowPower;

		if(s_lowPower_bk != 2)
		{
			if(0)	//(pattr->m_dispOff)
			{
				pattr->m_dispOff = 0;
				dsk_display_lcd_on();
			}
		}
	}

	if((!pattr->m_dispOff) && (s_lowPower_bk >= 0)) // 电量状态不确定时，显示电量图标
	{
		// 5秒自动关屏
		if((s_lowPower_bk == 2) || (++s_timerCnt >= 5 * 10))
		{
			s_timerCnt = 0;

		#ifdef ID_TIMER_GET_DEBUG_INFO
			if(!GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_DEBUG_INFO))
		#endif
			{
				dsk_display_lcd_off(OP_SCREENOFF_AND_BKOFF);
				pattr->m_dispOff = 1;
#if SP_PowerOffWithLowPower

				if(s_lowPower_bk > 0)
				{
					while(1)
					{
						// 给MCU发送关机信号
						IOCTRL__ENCRY__SET_FAIL();
						init_check_power_off();
						esKRNL_TimeDly(10);
					}
				}

#endif
			}
		}

		/*	else if(s_lowPower_bk >=0) // 电量状态不确定时，不更新电量图标
			{ // 显示画面
				charge_paint(msg);
			}*/
	}
}
#endif

static __s32 charge_mainwin_cb(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			__s32 i;
			chagre_attr_t *pattr;
			RECT rect;
			__s32 screen_width;
			__s32 screen_height;
			__wrn("dsk_get_charge_state()=%d\n", dsk_get_charge_state());
			pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

			if(NULL == pattr)
			{
				return NULL;
			}

			for(i = 0 ; i < CHARGE_BMP_NUM ; i++)
			{
				__wrn("pattr->bmp_id[i]=%d\n", pattr->bmp_id[i]);

				if(pattr->bmp_id[i])
				{
					pattr->bmp_hdl[i] = dsk_theme_open(pattr->bmp_id[i]);
					__wrn("i=%d, pattr->bmp_hdl[i]=%x\n", i, pattr->bmp_hdl[i]);
				}
				else
				{
					__wrn("dsk_theme_open fail,bmp_id=%d\n", pattr->bmp_id[i]);
					pattr->bmp_hdl[i] = NULL;
				}
			}

			__here__;
			__wrn("dsk_get_charge_state()=%d\n", dsk_get_charge_state());
			dsk_display_get_size(&screen_width, &screen_height);
			rect.x = 0;
			rect.y = 0;
			rect.width = screen_width;
			rect.height = screen_height;
			__wrn("screen_width=%d, screen_height=%d\n", screen_width, screen_height);
			pattr->hlayer = charge_scene_32bpp_layer_create(&rect);

			if(NULL == pattr->hlayer)
			{
				return EPDK_OK;
			}
			
			esKRNL_TimeDly(100);

			__wrn("dsk_get_charge_state()=%d\n", dsk_get_charge_state());
			__here__;
#if TIMEOUT_ID_TIMER_GET_POWER
			GUI_SetTimer(msg->h_deswin, ID_TIMER_GET_POWER, TIMEOUT_ID_TIMER_GET_POWER, NULL);
#endif
#if TIMEOUT_ID_TIMER_PAINT
			GUI_SetTimer(msg->h_deswin, ID_TIMER_PAINT, TIMEOUT_ID_TIMER_PAINT, NULL);
#endif
			pattr->m_lowPower = init_power_is_low();
			GUI_LyrWinSel(pattr->hlayer);
			GUI_SetBkColor(0x0);
			GUI_Clear();
			GUI_LyrWinCacheOff();
			dsk_display_lcd_on();
#if TIMEOUT_ID_TIMER_PAINT
			charge_paint(msg);
#endif
#if TIMEOUT_ID_TIMER_GET_DEBUG_INFO
			__SetTimer_GetDebugInfo(msg);
#endif
			return EPDK_OK;
		}

		case GUI_MSG_DESTROY:
		{
			__s32 i;
			chagre_attr_t *pattr;
			__wrn("charge_mainwin_cb, GUI_MSG_DESTROY begin...\n");
			pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

			if(NULL == pattr)
			{
				__wrn("show charge scene alreadly been destroy...\n");
				return NULL;
			}

#if TIMEOUT_ID_TIMER_GET_DEBUG_INFO

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_DEBUG_INFO))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_GET_DEBUG_INFO);
			}

#endif
#if TIMEOUT_ID_TIMER_GET_POWER

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_GET_POWER))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_GET_POWER);
			}

#endif
#if TIMEOUT_ID_TIMER_PAINT

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_PAINT))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_PAINT);
			}

#endif
			gscene_bgd_set_state(BGD_STATUS_SHOW);
			__here__;

			if(pattr->hlayer)
			{
				GUI_LyrWinDelete(pattr->hlayer);
				pattr->hlayer = NULL;
			}

			__here__;

			for(i = 0 ; i < CHARGE_BMP_NUM ; i++)
			{
				__wrn("dsk_theme_close, pattr->bmp_hdl[i]=0x%x\n", pattr->bmp_hdl[i]);

				if(pattr->bmp_hdl[i])
				{
					dsk_theme_close(pattr->bmp_hdl[i]);
					pattr->bmp_hdl[i] = NULL;
				}
			}

			__here__;
			eLIBs_memset(pattr, 0, sizeof(chagre_attr_t));
			esMEMS_Bfree(pattr, sizeof(chagre_attr_t));
			GUI_WinSetAttr(msg->h_deswin, NULL);
			__wrn("charge_mainwin_cb, GUI_MSG_DESTROY end...\n");
			return EPDK_OK;
		}

		case GUI_MSG_CLOSE:
			__here__
			GUI_ManWinDelete(msg->h_deswin);
			return EPDK_OK;

		case GUI_MSG_TIMER:
		{
			chagre_attr_t *pattr;
			pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

			if(NULL == pattr)
			{
				return EPDK_OK;
			}

			switch(msg->dwAddData1)
			{
#if TIMEOUT_ID_TIMER_PAINT

				case ID_TIMER_PAINT :
				{
					if((!pattr->m_dispOff) && (pattr->m_lowPower >= 0))
					{
						charge_paint(msg);
					}

					break;
				}

#endif
#if TIMEOUT_ID_TIMER_GET_POWER

				case ID_TIMER_GET_POWER :
				{
					show_charge_timer_get_power(msg);
					break;
				}

#endif
#if TIMEOUT_ID_TIMER_GET_DEBUG_INFO

				case ID_TIMER_GET_DEBUG_INFO :
				{
					__PaintDebugInfo(msg, 0);
					break;
				}

#endif

				default :
					break;
			}

			return EPDK_OK;
		}

		case GUI_MSG_KEY:
		{
			chagre_attr_t *pattr;
			pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

			if(NULL == pattr)
			{
				return EPDK_OK;
			}

			if((pattr->m_lowPower != 2) && (pattr->m_dispOff))
			{
				pattr->m_dispOff = 0;
				dsk_display_lcd_on();
			}

			if(pattr->m_lowPower)
			{
				return EPDK_OK;
			}

			if(KEY_UP_ACTION == msg->dwAddData2)
			{
				if(GUI_MSG_KEY_ENTER == msg->dwAddData1 || GUI_MSG_KEY_LONGENTER == msg->dwAddData1)
				{
					__gui_msg_t msgex;
					eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
					msgex.id = GUI_MSG_CLOSE;
					msgex.h_deswin = msg->h_deswin;
					GUI_SendNotifyMessage(&msgex);
				}
			}

			return EPDK_OK;
		}

		case GUI_MSG_TOUCH:
		{
			return EPDK_OK;
		}

		default:
		{
			break;
		}
	}

	return GUI_ManWinDefaultProc(msg);
}

static H_WIN show_charege_mainwin_create(void)
{
	H_WIN  						hManWin;
	__gui_manwincreate_para_t  	create_info;
	chagre_attr_t               *pattr;
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	pattr = (chagre_attr_t *)esMEMS_Balloc(sizeof(chagre_attr_t));

	if(NULL == pattr)
	{
		return NULL;
	}

	eLIBs_memset(pattr, 0, sizeof(chagre_attr_t));
	pattr->bmp_id[0] = ID_INIT_CHARGE_NULL_BMP;
	pattr->bmp_id[1] = ID_INIT_CHARGE_BMP0_BMP;
	pattr->bmp_id[2] = ID_INIT_CHARGE_BMP1_BMP;
	pattr->bmp_id[3] = ID_INIT_CHARGE_BMP2_BMP;
	create_info.attr            = pattr;
	create_info.hParent			= NULL;
	create_info.hHosting 		= NULL;
	create_info.ManWindowProc   = charge_mainwin_cb;
	create_info.name            = "charge";
	hManWin 					= GUI_ManWinCreate(&create_info);

	if(hManWin == NULL)
	{
		__wrn("show_charege_mainwin_create failed!\n");
		return NULL;
	}

	return hManWin;
}

void InitDispParam(void)
{
	reg_system_para_t *para;
	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

	if(para)
	{
		if(g_logoDisable)
		{
			ES_FILE 			   *p_disp = NULL;
			p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

			if(p_disp == NULL)
			{
				__err("Error in opening display driver.\n");
				return;
			}

			eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 0, 0);
			eLIBs_fclose(p_disp);
		}

		dsk_display_set_lcd_pwm(para->pwm);
		dsk_display_set_lcd_pwm_ex(para->pwm1);
	}
	else
	{
		dsk_display_set_lcd_pwm(LION_BRIGHT_LEVEL12);
		dsk_display_set_lcd_pwm_ex(LION_BRIGHT_LEVEL12);
	}

	{
		// 重置屏参
		dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_BRIGHT), DISP_CMD_SET_BRIGHT);
		dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_CONTRAST), DISP_CMD_SET_CONTRAST);
		dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_HUE), DISP_CMD_SET_HUE);
		dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_SATURATION), DISP_CMD_SET_SATURATION);
	}

	if(!g_logoDisable)
	{
		dsk_display_lcd_on();
	}
}

__s32 show_charge_scene(void)
{
	__s32 		ret;
	H_WIN 		hManWin;
	__gui_msg_t 	msg;
	__s8			powerState;
	powerState = init_power_is_low();
	__wrn("powerState = %d\n", powerState);

	if(powerState != 2)
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			g_logoDisable = para->logoDisable;
		}

		if(!g_logoDisable)
		{
			if(-1 == LogoOpen(PATH__LOGO_DEFAULT))
			{
				g_logoDisable = 1;
			}
		}

		InitDispParam();

		if(!g_logoDisable)
		{
			GUI_LyrWinCacheOn();
			esKRNL_TimeDly(10);
			LogoClose();
		}
	}

#if SP_PowerOffWithLowPower
	else
	{
		while(1)
		{
			// 给MCU发送关机信号
			IOCTRL__ENCRY__SET_FAIL();
			init_check_power_off();
			esKRNL_TimeDly(10);
		}
	}

#endif

#if 0
	{
		lz_drv_fail_flag = 0;

		if(!LzDrvChk())
		{
			lz_drv_fail_flag = 1;
		}
	}
#endif

	if(EPDK_FALSE == dsk_get_charge_state())
	{
		if(init_power_is_low() == EPDK_FALSE)
		{
			__wrn("*********not in charge state*************...\n");
			return EPDK_OK;
		}
	}

	__wrn("dsk_get_charge_state()=%d\n", dsk_get_charge_state());
	__wrn("************in charge state***********...\n");
	hManWin = show_charege_mainwin_create();
	__wrn("dsk_get_charge_state()=%d\n", dsk_get_charge_state());

	if(NULL == hManWin)
	{
		__wrn("show_charege_mainwin_create fail...\n");
		return EPDK_OK;
	}

	GUI_SetActiveManWin(hManWin);
	__wrn("hManWin = %x\n", hManWin);
	/* set message loop win */
	message_loop_win_set_default(hManWin);

	while(GUI_GetMessageEx(&msg, hManWin))	// 消息循环
	{
#ifdef __MSG

		if(msg.id != GUI_MSG_TIMER)
		{
			__msg("msg.h_deswin=%x, msg.id=%d, msg.dwAddData1=%d,msg.dwAddData2=%d\n"
			      , msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);
		}

#endif
		ret = GUI_DispatchMessage(&msg);		// 分发消息到回调

		if(msg.p_arg)							// 同步消息回应
		{
			GUI_SetSyncMsgRetVal(&msg, ret);
			GUI_PostSyncSem(&msg);
		}

		// __here__;
	}

	GUI_WinThreadCleanup(hManWin);
	message_loop_win_set_default(NULL);
	GUI_SetActiveManWin(NULL);//置空，防止消息队列无对应窗口
	return EPDK_OK;
}

void init_reg_param_load(void)
{
	//init reg
	{
		__s32 ret;
		ret = dsk_reg_init_para();
		__wrn("******dsk_reg_init_para******\n");

		if(EPDK_FAIL == ret)
		{
			__msg("dsk_reg_init_para fail...\n");
		}
	}

	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(!para)
		{
			__wrn("init get reg para fail............\n");
		}
		else
		{		
			esKRNL_TimeSet(para->m_knlTime);
			
			#if (SP_APP_ALARM |SP_APP_CALENDAR)
			{
				__s32 ret;
				__date_t	date;
				ret = esTIME_GetDate(&date);
				
				if(EPDK_OK == ret)
				{
					if(date.year < 2010 || date.year > 2020)
					{
						__wrn("date.year < 2010 || date.year > 2020, reset system time...\n");
						para->time_set = 1;
					}
				}
				
				__wrn("para->time_set=%d\n", para->time_set);
				
				if(1 == para->time_set)
				{
					__wrn("para->time.hour=%d, para->time.minute=%d, para->time.second=%d",
								para->time.hour, para->time.minute, para->time.second);
					__wrn("para->date.day=%d, para->date.month=%d, para->date.year=%d",
								para->date.day, para->date.month, para->date.year);
					esTIME_SetTime(&para->time);
					esTIME_SetDate(&para->date);
					para->time_set = 0;
				}
			}
			#endif

		#if SP_DEV_TP
			{
				__bool bexist;
				ES_FILE *fp;
				fp = eLIBs_fopen("E:\\tpcfg.ini", "rb");

				if(fp)
				{
					bexist = 1;
					eLIBs_fclose(fp);
				}
				else
				{
					bexist = 0;
				}

				if(0 == bexist)
				{
					__wrn("E:\\tpcfg.ini not exist, set tp_adjust_finish_flag to 0...\n");
					para->tp_adjust_finish_flag = 0;
				}
			}

			__wrn("para->tp_adjust_finish_flag=%d\n", para->tp_adjust_finish_flag);

			if(para->tp_adjust_finish_flag == 0)
			{
				if(1)
				{
					ES_FILE       *p_disp;
					/*open display driver handle*/
					p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

					if(p_disp)
					{
						/*start hardware cache layer*/
						eLIBs_fioctrl(p_disp, DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, SEL_SCREEN, 0);
						__wrn("******DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE************\n");
						eLIBs_fclose(p_disp);
					}
					else
					{
						__wrn("open display drv fail...\n");
					}
				}

				{
					H_WIN   scene_adjust;
					__here__;
					scene_adjust = scene_adjust_create(NULL);
					scene_adjust_destroy(scene_adjust);
					para->tp_adjust_finish_flag = 1;
					dsk_reg_flush();
				}
			}
		#endif
		}
	}
}

void application_init_process(void *arg)
{
	__gui_msg_t 	msg;
	__s32			ret = EPDK_FAIL;
	__memit_hook_t 	emit_hook;
	__wrn("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	__wrn("&           application_init_process enter!          	 &\n");
	__wrn("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");

#if BOOT_FROM_SDMMC
	eLIBs_printf("----------------------------------------------------\n"
							 "------------!!!!!! BOOT FROM SDMMC !!!!!!-----------\n"
							 "----------------------------------------------------\n");
#endif

	dsk_amplifier_onoff(0);
	
	/* drvier plugin */
	//	App_InsmodUsbHostDriver(1);
	//	App_InsmodUsbHostDriver(2);
	esDEV_Plugin("\\drv\\ir.drv", 0, 0, 1);

#if !BOOT_FROM_SDMMC	
	__msg("before audio plugin\n");	
	esDEV_Plugin("\\drv\\audio.drv", 0, 0, 1);
	__msg("after audio plugin\n");
	if(1)
	{
		//112358
		ES_FILE *hAudio;
		hAudio = eLIBs_fopen("b:\\AUDIO\\PLAY", "r+");

		if(NULL != hAudio)
		{
			eLIBs_fioctrl(hAudio, AUDIO_DEV_CMD_RESIZE_BUF, 1024 * 100, 0);
			eLIBs_fclose(hAudio);
			hAudio = NULL;
		}
	}

	//	esDEV_Plugin("\\drv\\orchid.drv", 0, 0, 1);
	//	esDEV_Plugin("\\drv\\touchpanel.drv", 0, 0, 1);
	//	esDEV_Plugin("\\drv\\mp.drv", 0, 0, 1);
	//	bookengine_open();
	//	_install_dbms_mod();
	
	init_reg_param_load();
#endif

	/* messge server init*/
	msg_emit_init();
	emit_hook.key_hook 			= key_hook_cb;
	emit_hook.tp_hook 	   		= tp_hook_cb;
	emit_hook.init_hook 		= init_hook_cb;
	msg_emit_register_hook(&emit_hook);
	
	live_init();
	//backgrd
	{
		SIZE screen_size;
		__s32 w, h;
		dsk_display_get_size(&w, &h);
		screen_size.width	 = w;
		screen_size.height	 = h;
		gscene_bgd_init(&screen_size, BGD_STATUS_HIDE, FB_TYPE_YUV);
	}
	
	//esKSRV_SysInfo();
	//Tarring
	show_charge_scene();
	//esKSRV_SysInfo();

#if !BOOT_FROM_SDMMC
	IOCTRL__USB_PW__SET_ON();
	/*enable device monitor */
	{
		ES_FILE     *pHwsc;
		pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");

		if(pHwsc)
		{
			eLIBs_fioctrl(pHwsc, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0, NULL);
			eLIBs_fclose(pHwsc);
			__wrn(".start device monitor .......................................\n");
		}
		else
		{
			__wrn("try to open b:\\HWSC\\hwsc failed!\n");
		}
	}
	//App_InsmodUsbHostDriver(0);
#endif
	
	//	esKRNL_TimeDly(100); //不需要开机自动播放，无需延时
	__wrn("*****init_mainwin_create******\n");
	/* create main win*/
	init_mainwin = init_mainwin_create();
	GUI_SetActiveManWin(init_mainwin);
	__wrn("init_mainwin = %x\n", init_mainwin);
	/* set message loop win */
	message_loop_win_set_default(init_mainwin);
	/* drv or mod plugin */
	//_process_init();
	_framework_init();

	/* load main*/
	//activity_load_app("application://app_root");

	/*add usb mode */
	#if(0)
	{
		ES_FILE     *pHwsc;
		pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");

		if(pHwsc)
		{
			eLIBs_fioctrl(pHwsc, DEV_IOC_USR_ADD_USBD_MODE, USBD_UDISK, NULL);
			eLIBs_fclose(pHwsc);
			__wrn(".start device monitor .......................................\n");
		}
		else
		{
			__wrn("try to open b:\\HWSC\\hwsc failed!\n");
		}
	}
	#endif

#if BOOT_FROM_SDMMC 	 //卡量产
	eLIBs_printf("start to run firmware update .........\n");
	{
		__s32 ret;
		__u32 card_flag;
		esDEV_Plugin("\\drv\\spi.drv", 0, 0, 1);

		esKRNL_TimeDly(2);
		GUI_LyrWinCacheOn();
		GUI_LyrWinCacheOff();		
		gscene_bgd_set_state(BGD_STATUS_SHOW);
		LogoBufRelease();
		card_flag = 1; 
		ret = esDEV_Plugin("\\drv\\spinor.drv", 0, (void *)card_flag, 1);
		__msg("ret = %d --------------------------\n",ret);
	
		dsk_start_fw_update("d:\\ePDKv100.img");
	}
	__here__;
#endif

	/* message loop*/
	while(GUI_GetMessageEx(&msg, init_mainwin))	// 消息循环
	{
#ifdef __MSG

		if(msg.id != GUI_MSG_TIMER)
		{
			__msg("msg.h_deswin=%x, msg.id=%d, msg.dwAddData1=%d,msg.dwAddData2=%d\n"
			      , msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);
		}

#endif
		ret = GUI_DispatchMessage(&msg);		// 分发消息到回调

		if(msg.p_arg)							// 同步消息回应
		{
			GUI_SetSyncMsgRetVal(&msg, ret);
			GUI_PostSyncSem(&msg);
		}

		// __here__;
	}

	GUI_WinThreadCleanup(init_mainwin);
	message_loop_win_set_default(NULL);
	/* unstall mod and driver */
	_process_exit();
	dsk_amplifier_onoff(0);

#if (!SP_DEV_HW_PW_OFF)
	//Tarring
	if(!g_b_lowpower)
	{
		__u16 i;

		for(i = 0; i < 50; i++)
		{
			esKRNL_TimeDly(1);
			__wrn("wait reset:%d\n", i);

			if(!IOCTRL__PWOFF_DET__IS_OFF())
			{
				__wrn("esKSRV_Reset()\n");
				esKSRV_Reset();
			}
		}
	}

	IOCTRL__PWON__SET_OFF();//提前关机掉电

	while(1)
	{
		static __u32 i = 0;
		esKRNL_TimeDly(1);
		__wrn("wait power off:%d\n", i++);
	}
#endif

	__inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	__inf("&           application_init_process exit!                          &\n");
	__inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	/* 关闭电源 */
	esKSRV_PowerOff();
	__msg("end of application_init_process\n");
}


