/*
************************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						          (c) Copyright 2007-2010, Jackie, China
*										All Rights Reserved
*
* File    		: init_server.c
* By      		: Jackie.chen
* Func		: init server
* Version	: v1.0
* ======================================================================================================================
* 2010-10-25  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/

//#include "apps.h"
#include "mod_init_i.h"
#include "init_server.h"
#include "live_init.h"
#include "dialog_scene\\dialog_scene.h"
#include "headbar\\headbar_com.h"
#include "assistant_scene\\assistant_scene.h"

#define OPEN_CURSOR				0
#define	AUTO_OFF_TIME_ID		1000
#define CLOSE_SCN_TIME_ID		1001
#define LOW_POWER_CHECK_TIME_ID	1002
#define CURSOR_TIME_ID			1003
#define LED_FLASH_TIME_ID		1004

#define ID_TIMER_LOAD_APP		1100
#define MAX_LowPowerCueCnt		5

#define TIMEOUT_ID_TIMER_DET_UDISK					600
#if TIMEOUT_ID_TIMER_DET_UDISK
	#define ID_TIMER_DET_UDISK						0x2001
#endif

#define INIT_FW_UPDATE_MAX_ICON_RES_NUM 2


extern GUI_FONT    *SWFFont;
extern __bool 	lz_drv_fail_flag;
extern __u32 	g_logoDisable;

__u8 				g_b_lowpower = 0;
__s8 				g_tconOccupyLcmi_flag = 0;
__u8				g_screenCloseSel = 0;

//static __s8 				g_led_on_flag = 0;	

static __bool 	g_b_enable_standby = 1;
static __u8 		g_b_off_check = 0;
static __u8 		g_b_off_later = 0;
static __u8 		g_b_mic_check = 0;


typedef struct
{
	__s32 x;
	__s32 y;
	__s32 w;
	__s32 h;
	__s32 res_id[INIT_FW_UPDATE_MAX_ICON_RES_NUM];
	HTHEME res_hdl[INIT_FW_UPDATE_MAX_ICON_RES_NUM];
}
init_fw_update_rect_t;

typedef enum
{
  init_fw_update_prog_bg = 0,
  init_fw_update_prog_left,
  init_fw_update_prog_mid,
  init_fw_update_prog_cursor,
  init_fw_update_prog_finish,
  init_fw_update_prog_icon_num
} init_fw_update_icon_t;

#define PIOC_MEM_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)               ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)              ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))


void init_lock(__gui_msg_t *msg)
{
	__u8 err;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	esKRNL_SemPend(init_ctr->state_sem, 0, &err);
	return;
}

void init_unlock(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	esKRNL_SemPost(init_ctr->state_sem);
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

static void pull_xor_gpio(__u32 port, __u32 port_num)
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
	tmp_data ^= (1 << port_num);
	*tmp_addr = tmp_data;
}

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

static void init_cursor_create(__init_ctl_t *m_ctl)
{
	HTHEME 	h_them_cursor;
	HTHEME 	h_them_pal;
	__pos_t	pos;
	int 	width, height;
	__err("headbar");
	h_them_cursor = dsk_theme_open(0);
	m_ctl->cursor_bmp = dsk_theme_hdl2buf(h_them_cursor);
	m_ctl->cursor_info.pat_mem = m_ctl->cursor_bmp + 54 + 1024;
	h_them_pal = dsk_theme_open(0);
	m_ctl->cursor_info.palette = dsk_theme_hdl2buf(h_them_pal);
	GUI_CursorCreate(&(m_ctl->cursor_info));
	dsk_display_get_size(&width, &height);
	pos.x = width / 2;
	pos.y = height / 2;
	GUI_CursorSetPos(&pos);
	GUI_CursorHide();
}

static void init_cursor_delete(__init_ctl_t *m_ctl)
{
	eLIBs_free(m_ctl->cursor_info.palette);
	eLIBs_free(m_ctl->cursor_bmp);
	GUI_CursorDelete();
}

static void system_message_handle(void *arg)
{
	init_scene_t *p_scene;
	H_WIN h_wnd = (H_WIN)arg;

	while(1)
	{
		if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
		{
			esKRNL_TDel(OS_PRIO_SELF);
			return;
		}

		//orchid update
		p_scene = (init_scene_t *)GUI_WinGetAddData(h_wnd);

		if(p_scene->orchid_update == EPDK_TRUE)
		{
			//dsk_orchid_update();
			//NOTIFY_MSG(DSK_MSG_ORCHID_UPDATE_END, NULL, h_wnd, 0, 0);
		}

		esKRNL_TimeDly(1);
	}
}

static __s32 sys_msg_handle_init(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	/*创建system_message_handle 线程 */
	init_ctr->sys_msg_handle_tid = esKRNL_TCreate(system_message_handle, (void *)msg->h_deswin, 0x800, KRNL_priolevel5);
	return EPDK_OK;
}

static __s32 sys_msg_handle_exit(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	/* 删除system_message_handle 线程 */
	while(esKRNL_TDelReq(init_ctr->sys_msg_handle_tid) != OS_TASK_NOT_EXIST)
	{
		esKRNL_TimeDly(1);
	}

	init_ctr->sys_msg_handle_tid = 0;
	return EPDK_OK;
}

static __s32 init_low_power_proc(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!init_ctr->usb_connect)
	{
		/* 退出应用程序*/
		//activity_return_home(NULL);
		//activity_kill_all_backgrd();
		__here__;
		activity_finish_all();
		gscene_bgd_set_state(BGD_STATUS_SHOW);
		gscene_hbar_set_state(HBAR_ST_HIDE);
		/* low power dialog */
		{
			init_scene_t *p_scene;
			p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			p_scene->low_power = EPDK_TRUE;
			scene_on_dialog(msg);
		}
	}

	return EPDK_OK;
}

static __s32 init_restart_close_scn(__gui_msg_t *msg)
{
	__bool exist;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(init_ctr->closescn_timeout == 0)
	{
		return EPDK_OK;
	}

	exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

	if(exist == EPDK_TRUE)
	{
		GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
	}
	else
	{
		GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
	}

	return EPDK_OK;
}

static __s32 init_power_off_proc(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(!init_ctr->usb_connect)
	{
		init_scene_t *p_scene;
		//activity_return_home(NULL);
		//activity_kill_all_backgrd();
		IOCTRL__ENCRY__SET_FAIL();
		activity_finish_all();
		gscene_bgd_set_state(BGD_STATUS_SHOW);
		gscene_hbar_set_state(HBAR_ST_HIDE);
		GUI_LyrWinCacheOff();
		/* poweroff dialog */
		p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
		p_scene->power_off = EPDK_TRUE;
		__here__;
		scene_on_dialog(msg);
		__here__;
		__msg_jjl("...............p_scene->usb_plug_in)=%x\n",p_scene->usb_plug_in);
		if(EPDK_FALSE == p_scene->usb_plug_in)//usb连接到来把关机对话框关闭后，程序不应该往下执行，只有usb未连接才能关机
		{

			
	//{
			//OCTRL__ENCRY__SET_FAIL();
				__msg_jjl("NOTIFY_MSG(GUI_MSG_CLOSE\n");
			#if SP_APP_ALARM
				/* 设置闹铃唤醒  */
				standby_startup_alarm();
			#endif
				//notify "init" to exit
				NOTIFY_MSG(GUI_MSG_CLOSE, 0, msg->h_deswin, 0, 0);
			}
		
	/*	init_close_screen(msg, 0);
		IOCTRL__PA_SHDN__SET_EN();
		
		while(1)
		{
			static __u32 i = 0;
			__msg_jjl("wait power off:%d\n", i++);


		}*/
		__here__;
	}

	return EPDK_OK;
}

static __s32 init_usb_device_plugin_proc(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	/* 通知除主界面外所有应用程序退出 */
	init_ctr->usb_connect = EPDK_TRUE;
	//activity_return_home(NULL);
	//activity_kill_all_backgrd();
	//activity_suspend_top();
	activity_finish_all();

	if(dsk_wkm_is_open())
	{
		dsk_wkm_close();
	}

	gscene_bgd_set_state(BGD_STATUS_SHOW);
	gscene_hbar_set_state(HBAR_ST_HIDE);
	/* usb dialog */
	{
		init_scene_t *p_scene;
		p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
		p_scene->usb_connect = EPDK_TRUE;
		scene_on_dialog(msg);
	}
	return EPDK_OK;
}

static __s32 init_usb_device_plugout_proc(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	init_ctr->usb_connect = EPDK_FALSE;
	{
		init_scene_t *p_scene;
		p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
		p_scene->usb_connect = EPDK_FALSE;
		scene_on_dialog(msg);
		p_scene->usb_plug_in = EPDK_FALSE;
	}
	return EPDK_OK;
}

/* 通知主应用程序 更新orchid开始 */
static __s32 init_orchid_update_start(__gui_msg_t *msg)
{
	{
		init_scene_t *p_scene;
		p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
		p_scene->orchid_update = EPDK_TRUE;
		scene_on_dialog(msg);
	}
	return EPDK_OK;
}

/* 通知主应用程序 更新orchid结束 */
static __s32 init_orchid_update_end(__gui_msg_t *msg)
{
	{
		init_scene_t *p_scene;
		p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
		p_scene->orchid_update = EPDK_FALSE;
	}
	return EPDK_OK;
}

static void init_screen_onoff(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	__disp_output_type_t output = dsk_display_get_output_type();

	if((output == DISP_OUTPUT_TYPE_NONE) || (output == DISP_OUTPUT_TYPE_LCD))
	{
		if(init_ctr->screen_close_done)
		{
			NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
		}
		else
		{
			SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0);
		}
	}
}

/**
 * 关屏
 */
static __s32 init_close_screen(__gui_msg_t *msg, __bool forceflag)
{
	__bool is_host_working;
	__disp_output_type_t output;
    __bool is_echo_on;
	//__gui_msg_t msgex;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	output = dsk_display_get_output_type();

	if((output == DISP_OUTPUT_TYPE_LCD && !init_ctr->usb_connect) || forceflag)//add by Kingvan
	{
		/* 置关屏状态位 */
		init_lock(msg);

		if(!init_ctr->screen_close_done)	/* 未关屏 */
		{
			init_ctr->screen_close_done = EPDK_TRUE;
			init_unlock(msg);
			/* 调用显示驱动, 关屏 */
			dsk_display_lcd_off(g_screenCloseSel);

			switch(g_screenCloseSel)
			{
				case OP_SCREENON_BUT_BKOFF :
				case OP_SCREENOFF_BUT_BKON :
					return EPDK_OK;

				case OP_SCREENOFF_AND_BKOFF :
					break;
				default :
					break;
			}

			if(!(EPDK_TRUE == activity_all_able_standby() && !dsk_wkm_is_playing() && !dsk_radio_is_open()
			     && g_is_enable_standby()))
			{
				//__init_decr_freq(msg);//112350
			}
			
		#if SP_APP_ALARM
			/* 设置闹铃唤醒  */
			standby_startup_alarm();
		#endif

			/* 进入 standby */
			__here__;
			is_host_working = dsk_usbh_is_working();
            __msg("is_host_working=%d\n", is_host_working);

            //混响打开的时候，不能进入standby，CPU要运行
            is_echo_on = dsk_audio_echo_is_open();
            __msg("is_echo_on=%d\n", is_echo_on);

            __msg("activity_all_able_standby()=%d\n", activity_all_able_standby());
            __msg("dsk_wkm_is_playing()=%d\n", dsk_wkm_is_playing());
            __msg("dsk_radio_is_open()=%d\n", dsk_radio_is_open());
            __msg("g_is_enable_standby()=%d\n", g_is_enable_standby());
            __msg("dsk_get_charge_state()=%d\n", dsk_get_charge_state());
          
			if(EPDK_FALSE == is_echo_on
				&&EPDK_FALSE == is_host_working
			    && EPDK_TRUE == activity_all_able_standby() && !dsk_wkm_is_playing() && !dsk_radio_is_open()
			    && g_is_enable_standby()
			    && EPDK_FALSE == dsk_get_charge_state())

			{
                ES_FILE            *hMonitor = NULL;
				ES_FILE *pfile = NULL;
				__here__;
            	__wrn("begin enter standby...\n");
				pfile = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");

				if(pfile)
				{
					//system will enter standby, set audio device to standby mode for power save
					//dsk_speaker_turnoff();
					eLIBs_fioctrl(pfile, AUDIO_DEV_CMD_ENTER_STANDBY, 0, 0);
				}
	         	
                if(1)
				{//先停止monitor                                        
                    hMonitor = eLIBs_fopen("b:\\HWSC\\monitor", "rb+");
                    if(hMonitor)
                    {
                        __inf("try to disable monitor\n");
                        eLIBs_fioctrl(hMonitor, DEV_IOC_USR_HWSC_DISABLE_MONITOR, 0, NULL);
                        __here__;
                        //eLIBs_fioctrl(hMonitor, DEV_IOC_USR_INIT_HOST_CONTROLER, 0, NULL);
                        __here__;
                    }
                }

                {//开启usb相关寄存器和时钟
                    USBHostDetectStart();
                }

                __here__;	
				esPWM_EnterStandby( init_ctr->autooff_timeout/100);		// 电流约20mA.
                
                {//关闭usb相关寄存器和时钟
                    USBHostDetectStop();
                }

                if(1)//再次打开monitor
                {
                    if(hMonitor)
                    {
                        __wrn("Try to start device monitor!\n");
                        //eLIBs_fioctrl(hMonitor, DEV_IOC_USR_DEINIT_HOST_CONTROLER, 0, NULL);
                        __here__;
                        eLIBs_fioctrl(hMonitor, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0, NULL);
                        __here__;
                        eLIBs_fclose(hMonitor);
                        hMonitor = NULL;
                    }   
                }                

				if(0)
				{
					//查看是否关机时间已经到了
					__sys_pwm_para_t standby_para;
					esPWM_GetStandbyPara(&standby_para);

					if(STANDBY_EVENT_POWEROFF & standby_para.EventFlag)
					{
						dsk_power_off();
					}
				}
				__here__;

				if(pfile)
				{
					//active audio device
					eLIBs_fioctrl(pfile, AUDIO_DEV_CMD_EXIT_STANDBY, 0, 0);
					esKRNL_TimeDly(10);
					//dsk_speaker_resume();
					eLIBs_fclose(pfile);
					pfile = NULL;
				}

				__here__;
				init_open_screen(msg);
			}
		}
		else									/* 已经关屏 */
		{
			init_unlock(msg);
		}
	}

	return EPDK_OK;
}

static __u32 cpu_clk = 0;
static __u32 sdram_clk = 0;

static __s32 __init_decr_freq(__gui_msg_t *msg)
{
	__s32 ret;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	init_ctr->bdec_freq = 1;
	//__here__;
	//cpu_clk = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL);
	__here__;
	sdram_clk = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL);
	__msg("sdram_clk=%d\n", sdram_clk);
	//__here__;
	//esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, 120*1000*1000);
	__here__;
	esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, 180 * 1000 * 1000);
	//ret = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL);
	//__msg("ret=%d\n", ret);
	//__here__;
	//esPWM_LockCpuFreq();
	//__here__;
	return EPDK_OK;
}

static __s32 __init_incr_freq(__gui_msg_t *msg)
{
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(1 == init_ctr->bdec_freq)
	{
		//__here__;
		//
		//esPWM_UnlockCpuFreq();
		__msg("sdram_clk=%d\n", sdram_clk);
		esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, sdram_clk);
		//__msg("cpu_clk=%d\n", cpu_clk);
		//esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, cpu_clk);
		//__here__;
		init_ctr->bdec_freq = 0;
	}

	return EPDK_OK;
}


/**
 * 开屏
 */
static __s32 init_open_screen(__gui_msg_t *msg)
{
	__bool exist;
	__disp_output_type_t output;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	init_scene_t *p_scene 	= (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);

	//__here__;
	if(g_tconOccupyLcmi_flag)
	{
		return EPDK_FAIL;
	}

	if(g_screenCloseSel == OP_SCREENOFF_BUT_BKON)
	{
		g_screenCloseSel = 0;
	}

	__wrn("Fun:init_open_screen\n");
	output = dsk_display_get_output_type();

	//__here__;
	if((output == DISP_OUTPUT_TYPE_NONE) || (output == DISP_OUTPUT_TYPE_LCD))
	{
		//   __here__;
		init_lock(msg);

		//  __here__;
		if(init_ctr->screen_close_done)	/* 屏幕已经关闭 */
		{
			//     __here__;
			init_unlock(msg);
			//      __here__;
			//__init_incr_freq(msg);//112350

			dsk_display_lcd_on();				/* 调用显示驱动, 开屏*/
			init_lock(msg);
			init_ctr->screen_close_done = EPDK_FALSE;/* 	清关屏状态位 */
			init_unlock(msg);
			//屏幕锁
			//p_scene->scnlock_win = create_screen_lock(msg->h_deswin);
			//GUI_WinSetFocusChild(p_scene->scnlock_win);
		}
		else									/* 屏幕还没关闭 */
		{
			init_unlock(msg);
		}

		exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

		if(exist == EPDK_TRUE)
		{
			GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);  /* 从新设置定时器*/
		}
	}

	return EPDK_OK;
}

/**
 * 设置自动关屏时间
 */
static __s32 init_set_close_scn(__gui_msg_t *msg)
{
	__bool exist;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(msg->dwAddData1 == 0)
	{
		init_ctr->closescn_timeout = 0;//add by Kingvan
		exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

		if(exist == EPDK_TRUE)
		{
			GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
		}
	}
	else if(msg->dwAddData1 > 0)
	{
		init_ctr->closescn_timeout = msg->dwAddData1 / 10;
		exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

		if(exist == EPDK_TRUE)
		{
			GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
		}
		else
		{
			GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
		}
	}
	else
	{
		__wrn(" close screen time < 0, set fail \n");
	}

	return EPDK_OK;
}

/**
 * 设置自动关机时间
 */
static __s32 init_set_auto_off(__gui_msg_t *msg)
{
	__bool exist;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	init_ctr->autooff_timeout = 0;

	if(msg->dwAddData1 == 0)
	{
		exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

		if(exist == EPDK_TRUE)
		{
			GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
		}
	}
	else if(msg->dwAddData1 > 0)
	{
		init_ctr->autooff_timeout = msg->dwAddData1 / 10;
		exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

		if(exist == EPDK_TRUE)
		{
			GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
		}
		else
		{
			GUI_SetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
		}
	}
	else
	{
		__wrn(" auto off time < 0, set fail \n");
	}

	return EPDK_OK;
}

static __s32 init_scene_create(__gui_msg_t *msg)
{
	SIZE	screen_size;
	//	init_scene_t *p_scene;
	//	p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
	init_screen_size();
	//dialog
	dialog_init(msg->h_deswin);

#if !BOOT_FROM_SDMMC
	//headbar
	gscene_hbar_create(msg->h_deswin, HBAR_FOARMAT_32BPP);
	//背景初始化放到init创建之前，充电图标结束后要用到。
	//backgrd
	//	screen_size.width	 = LCD_WIDTH;
	//	screen_size.height	 = LCD_HEIGHT;
	//	gscene_bgd_init(&screen_size, BGD_STATUS_HIDE, FB_TYPE_YUV);
#endif
	__here__
	return EPDK_OK;
}

static __s32 init_scene_destroy(__gui_msg_t *msg)
{
	init_scene_t *p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
	dialog_deinit(msg->h_deswin);
	__here__;
	gscene_hbar_delete();
	__here__;
	//gscene_bgd_deinit();
	return EPDK_OK;
}

static __s32 __init_prog_draw_progress(init_fw_update_rect_t *prog_bg, init_fw_update_rect_t *prog_left,
                                       init_fw_update_rect_t *prog_mid, init_fw_update_rect_t *prog_cursor,
                                       __s32 min, __s32 max, __s32 cur)
{
	//画进度条背景
	{
		void *pbmp;

		if(!prog_bg->res_hdl[0])
		{
			__msg("prog_bg->res_hdl[0] is null...\n");
			return EPDK_FAIL;
		}

		pbmp = dsk_theme_hdl2buf(prog_bg->res_hdl[0]);

		if(!pbmp)
		{
			__msg("pbmp is null...\n");
			return EPDK_FAIL;
		}

		GUI_BMP_Draw(pbmp, prog_bg->x, prog_bg->y);
	}
	//画进度条左边的图标
	{
		void *pbmp;
		__s32 focus;
		focus = 0;

		if(!prog_left->res_hdl[focus])
		{
			__msg("prog_left->res_hdl[focus] is null...\n");
			return EPDK_FAIL;
		}

		pbmp = dsk_theme_hdl2buf(prog_left->res_hdl[focus]);

		if(!pbmp)
		{
			__msg("pbmp is null...\n");
			return EPDK_FAIL;
		}

		GUI_BMP_Draw(pbmp, prog_left->x, prog_left->y);
	}

	//画进度条和cursor
	if(cur > max)//保护一下
	{
		cur = max;
	}

	if(cur < min)//保护一下
	{
		cur = min;
	}

	if(cur >= min
	    && cur <= max)
	{
		void *pbmp;

		if(!prog_mid->res_hdl[0])
		{
			__msg("prog_mid->res_hdl[0] is null...\n");
			return EPDK_FAIL;
		}

		pbmp = dsk_theme_hdl2buf(prog_mid->res_hdl[0]);

		if(!pbmp)
		{
			__msg("pbmp is null...\n");
			return EPDK_FAIL;
		}

		{
			__s32 i;
			__s32 n;
			__s32 mid_w;
			__s32 max_mid_w;
			__s32 cursor_w;
			__s32 cursor_pos;
			__s32 bg_pos;
			__s32 bg_w;

			if(cur == min)
			{
				n = 0;
			}
			else
			{
				n = (prog_bg->w - prog_cursor->w) * (cur - min)
				    / prog_mid->w / (max - min);
			}

			mid_w = prog_mid->w;
			max_mid_w = prog_bg->w - prog_cursor->w;
			cursor_w = prog_cursor->w;
			bg_pos = prog_bg->x;
			bg_w = prog_bg->w;

			//防止非整数倍时画不满
			if(max_mid_w - n * mid_w < mid_w)
			{
				n++;
				cursor_pos = bg_pos + bg_w - cursor_w;
			}
			else
			{
				cursor_pos = bg_pos + n * mid_w;
			}

			for(i = 1 ; i < n ; i++)
			{
				GUI_BMP_Draw(pbmp, prog_mid->x + i * prog_mid->w
				             , prog_mid->y);
			}

			//画cursor
			if(!prog_cursor->res_hdl[0])
			{
				__msg("prog_cursor->res_hdl[0] is null...\n");
				return EPDK_FAIL;
			}

			pbmp = dsk_theme_hdl2buf(prog_cursor->res_hdl[0]);

			if(!pbmp)
			{
				__msg("pbmp is null...\n");
				return EPDK_FAIL;
			}

			GUI_BMP_Draw(pbmp, cursor_pos, prog_cursor->y);
		}
	}

	return EPDK_OK;
}

#define H_UPDATE_CUE_FONT					20
#define H_UPDATE_PROG_BG					15
#define H_UPDATE_FINISH						32
static void fw_update(__gui_msg_t *msg, char *fw_path)
{
	__s32 len;
	__u8 	mid_update;
	__mp 	*mod_update;
	__s32 progress;
	RECT rect;
	char str_text[128];
	GUI_RECT gui_rect;
	init_scene_t *p_scene ;
	GUI_MEMDEV_Handle memdev_hdl = NULL;
	init_fw_update_rect_t prog_ui[init_fw_update_prog_icon_num];
	
	__msg_jjl("fw_path=%s\n", fw_path);
	p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
	GUI_LyrWinSetSta(p_scene->dialog_layer, GUI_LYRWIN_STA_ON);
	GUI_LyrWinSel(p_scene->dialog_layer);
	GUI_LyrWinSetTop(p_scene->dialog_layer);
	GUI_LyrWinGetScnWindow(p_scene->dialog_layer, &rect);
	dsk_langres_get_menu_text(STRING_FW_UPDATE, str_text, sizeof(str_text));
	prog_ui[init_fw_update_prog_bg].w = 134;
	prog_ui[init_fw_update_prog_bg].h = 15;
	gui_rect.x0 = 0;
	gui_rect.x1 = rect.width - 1;
	gui_rect.y0 = 0;
	gui_rect.y1 = gui_rect.y0 + H_UPDATE_CUE_FONT - 1;
	GUI_SetFont(SWFFont);
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	GUI_SetColor(GUI_WHITE);
	GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
	GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_TOP);
	prog_ui[init_fw_update_prog_finish].w = 120;
	prog_ui[init_fw_update_prog_finish].h = H_UPDATE_FINISH;
	prog_ui[init_fw_update_prog_finish].x = (rect.width - prog_ui[init_fw_update_prog_finish].w) / 2;
	prog_ui[init_fw_update_prog_finish].y = rect.height -prog_ui[init_fw_update_prog_finish].h -1;
	prog_ui[init_fw_update_prog_finish].res_id[0] = ID_INIT_UPDATE_FINISH_BMP;
	prog_ui[init_fw_update_prog_finish].res_id[1] = 0;
	prog_ui[init_fw_update_prog_bg].x = (rect.width - prog_ui[init_fw_update_prog_bg].w) / 2;
	prog_ui[init_fw_update_prog_bg].y = H_UPDATE_CUE_FONT +(rect.height -H_UPDATE_CUE_FONT -H_UPDATE_PROG_BG -H_UPDATE_FINISH) /2;
	prog_ui[init_fw_update_prog_bg].res_id[0] = ID_INIT_PROG_BG_BMP;
	prog_ui[init_fw_update_prog_bg].res_id[1] = 0;
	prog_ui[init_fw_update_prog_left].x = prog_ui[init_fw_update_prog_bg].x;
	prog_ui[init_fw_update_prog_left].y = prog_ui[init_fw_update_prog_bg].y;
	prog_ui[init_fw_update_prog_left].w = 7;
	prog_ui[init_fw_update_prog_left].h = 15;
	prog_ui[init_fw_update_prog_left].res_id[0] = ID_INIT_PROG_LEFT_BMP;
	prog_ui[init_fw_update_prog_left].res_id[1] = 0;
	prog_ui[init_fw_update_prog_mid].x = prog_ui[init_fw_update_prog_bg].x;
	prog_ui[init_fw_update_prog_mid].y = prog_ui[init_fw_update_prog_bg].y;
	prog_ui[init_fw_update_prog_mid].w = 7;
	prog_ui[init_fw_update_prog_mid].h = 15;
	prog_ui[init_fw_update_prog_mid].res_id[0] = ID_INIT_PROG_MID_BMP;
	prog_ui[init_fw_update_prog_mid].res_id[1] = 0;
	prog_ui[init_fw_update_prog_cursor].x = prog_ui[init_fw_update_prog_bg].x;
	prog_ui[init_fw_update_prog_cursor].y = prog_ui[init_fw_update_prog_bg].y;
	prog_ui[init_fw_update_prog_cursor].w = 20;
	prog_ui[init_fw_update_prog_cursor].h = 15;
	prog_ui[init_fw_update_prog_cursor].res_id[0] = ID_INIT_PROG_CURSOR_BMP;
	prog_ui[init_fw_update_prog_cursor].res_id[1] = 0;
	{
		int i;
		int j;

		for(i = 0 ; i < init_fw_update_prog_icon_num ; i++)
		{
			for(j = 0 ; j < INIT_FW_UPDATE_MAX_ICON_RES_NUM ; j++)
			{
				if(prog_ui[i].res_id[j])
				{
					prog_ui[i].res_hdl[j] = dsk_theme_open(prog_ui[i].res_id[j]);

					if(!prog_ui[i].res_hdl[j])
					{
						__wrn("dsk_theme_open fail...\n");
					}
					else//先读取nor数据占住到内存
					{
						dsk_theme_hdl2buf(prog_ui[i].res_hdl[j]);
					}
				}
				else
				{
					prog_ui[i].res_hdl[j] = NULL;
				}
			}
		}
	}
	__init_prog_draw_progress(&prog_ui[init_fw_update_prog_bg],
	                          &prog_ui[init_fw_update_prog_left],
	                          &prog_ui[init_fw_update_prog_mid],
	                          &prog_ui[init_fw_update_prog_cursor],
	                          0, 100, 0);
	mid_update = esMODS_MInstall(BEETLES_APP_ROOT"mod\\update.mod", 0);

	if(mid_update == 0)
	{
		__err("update mod install error \n");
		goto err1;
	}

	mod_update = esMODS_MOpen(mid_update, 0);

	if(mod_update == NULL)
	{
		__err("open update mode error \n");
		esMODS_MUninstall(mid_update);
		goto err1;
	}

	__msg("mod_update = %x,file=%s\n", mod_update, fw_path);
	esMODS_MIoctrl(mod_update, UPDATE_CMD_START, 0, (void *)fw_path);
	__msg_jjl(" ioctrl mod update UPDATE_CMD_START end\n");

	while(1)
	{
		esKRNL_TimeDly(50);
		progress = esMODS_MIoctrl(mod_update, UPDATE_CMD_CHECK_PROG, 0, 0);

		if(-1  == progress)
		{
			__err("error occur\n");
			break;
		}

		GUI_LyrWinSel(p_scene->dialog_layer);
		memdev_hdl = GUI_MEMDEV_Create(
		               prog_ui[init_fw_update_prog_bg].x,
		               prog_ui[init_fw_update_prog_bg].y,
		               prog_ui[init_fw_update_prog_bg].w,
		               prog_ui[init_fw_update_prog_bg].h);

		if(!memdev_hdl)
		{
			__msg("GUI_MEMDEV_Create fail...\n");
			continue ;
		}

		GUI_MEMDEV_Select(memdev_hdl);
		__init_prog_draw_progress(&prog_ui[init_fw_update_prog_bg],
		                          &prog_ui[init_fw_update_prog_left],
		                          &prog_ui[init_fw_update_prog_mid],
		                          &prog_ui[init_fw_update_prog_cursor],
		                          0, 100, progress);
		GUI_MEMDEV_CopyToLCD(memdev_hdl);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(memdev_hdl);
		__msg_jjl("current progress is %d%%\n", progress);

		if(100 == progress)
		{
			__msg_jjl("update complete\n");
			{
				esKRNL_TimeDly(200);
				GUI_BMP_Draw(
					dsk_theme_hdl2buf(prog_ui[init_fw_update_prog_finish].res_hdl[0]),
					prog_ui[init_fw_update_prog_finish].x,
					prog_ui[init_fw_update_prog_finish].y);
				#if !BOOT_FROM_SDMMC
					esKRNL_TimeDly(250);
					dsk_display_lcd_off(OP_SCREENOFF_AND_BKOFF);
					eLIBs_printf("system reset...");
					esKRNL_TimeDly(50);
					esKSRV_Reset();
				#endif
			}
			esMODS_MIoctrl(mod_update, UPDATE_CMD_STOP, 0, 0);
			__msg_jjl(" ioctrl mod update UPDATE_CMD_STOP end\n");
		}
	}

	esMODS_MClose(mod_update);
	esMODS_MUninstall(mid_update);
err1:
	;
	return ;
}




__s32 DbgMode__AdjustDispParam(__gui_msg_t *msg)
{
	reg_system_para_t *para;
	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
	if(para->m_maskFlags & MASK_SysFlags_DebugMode)
	{
		if(msg->dwAddData2 != KEY_UP_ACTION)
		{
			__s32 val;
			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_NUM1:
				case GUI_MSG_KEY_LONGNUM1:
					val = dsk_get_display_global(DISP_CMD_GET_BRIGHT) +1;
					if(val > CONST_DispBright_Max)	val = CONST_DispBright_Min;
					dsk_set_display_global(val, DISP_CMD_SET_BRIGHT);
					break;
				case GUI_MSG_KEY_NUM2:
				case GUI_MSG_KEY_LONGNUM2:
					val = dsk_get_display_global(DISP_CMD_GET_BRIGHT) -1;
					if(val < CONST_DispBright_Min)	val = CONST_DispBright_Max;
					dsk_set_display_global(val, DISP_CMD_SET_BRIGHT);
					break;
				case GUI_MSG_KEY_NUM3:
				case GUI_MSG_KEY_LONGNUM3:
					val = dsk_get_display_global(DISP_CMD_GET_CONTRAST) +1;
					if(val > CONST_DispContrast_Max)	val = CONST_DispContrast_Min;
					dsk_set_display_global(val, DISP_CMD_SET_CONTRAST);
					break;
				case GUI_MSG_KEY_NUM4:
				case GUI_MSG_KEY_LONGNUM4:
					val = dsk_get_display_global(DISP_CMD_GET_CONTRAST) -1;
					if(val < CONST_DispContrast_Min)	val = CONST_DispContrast_Max;
					dsk_set_display_global(val, DISP_CMD_SET_CONTRAST);
					break;
				case GUI_MSG_KEY_NUM5:
				case GUI_MSG_KEY_LONGNUM5:
					val = dsk_get_display_global(DISP_CMD_GET_HUE) +1;
					if(val > CONST_DispHue_Max)	val = CONST_DispHue_Min;
					dsk_set_display_global(val, DISP_CMD_SET_HUE);
					break;
				case GUI_MSG_KEY_NUM6:
				case GUI_MSG_KEY_LONGNUM6:
					val = dsk_get_display_global(DISP_CMD_GET_HUE) -1;
					if(val < CONST_DispHue_Min)	val = CONST_DispHue_Max;
					dsk_set_display_global(val, DISP_CMD_SET_HUE);
					break;
				case GUI_MSG_KEY_NUM7:
				case GUI_MSG_KEY_LONGNUM7:
					val = dsk_get_display_global(DISP_CMD_GET_SATURATION) +1;
					if(val > CONST_DispSaturability_Max)	val = CONST_DispSaturability_Min;
					dsk_set_display_global(val, DISP_CMD_SET_SATURATION);
					break;
				case GUI_MSG_KEY_NUM8:
				case GUI_MSG_KEY_LONGNUM8:
					val = dsk_get_display_global(DISP_CMD_GET_SATURATION) -1;
					if(val < CONST_DispSaturability_Min)	val = CONST_DispSaturability_Max;
					dsk_set_display_global(val, DISP_CMD_SET_SATURATION);
					break;
				case GUI_MSG_KEY_NUM9:
				case GUI_MSG_KEY_LONGNUM9:
					val = para->pwm +1;
					if(val >= LION_BRIGHT_TOTAL)	val = LION_BRIGHT_LEVEL1;
					dsk_display_set_lcd_pwm(val);
					break;
				case GUI_MSG_KEY_NUM0:
				case GUI_MSG_KEY_LONGNUM0:
					if(para->pwm <= LION_BRIGHT_LEVEL1)	val = LION_BRIGHT_TOTAL-1;
					else	val = para->pwm -1;
					dsk_display_set_lcd_pwm(val);
					break;
				default : break;
			}		
		}

		if(((msg->dwAddData1 >= GUI_MSG_KEY_NUM0) && (msg->dwAddData1 <= GUI_MSG_KEY_NUM9))
			|| ((msg->dwAddData1 >= GUI_MSG_KEY_LONGNUM0) && (msg->dwAddData1 <= GUI_MSG_KEY_LONGNUM9)))
		{
			//__msg_jjl("DbgMode__AdjustDispParam:dbg mode mask KEY_NUM(0x%X)\n", msg->dwAddData1-GUI_MSG_KEY_NUM0);
			return EPDK_OK;
		}
	}

	return EPDK_FAIL;
}

static __s32 init_mainwin_cb(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			__init_ctl_t *init_ctr;
			init_scene_t *p_scene;
			/////////////////////////////////////////////////////////////////
			init_ctr = (__init_ctl_t *)eLIBs_malloc(sizeof(__init_ctl_t));

			if(!init_ctr)
			{
				__err("init_ctr malloc error \n");
				return EPDK_FAIL;
			}

			eLIBs_memset(init_ctr, 0, sizeof(__init_ctl_t));
			GUI_WinSetAttr(msg->h_deswin, (void *)init_ctr);
			/////////////////////////////////////////////////////////////////
			p_scene = (init_scene_t *)eLIBs_malloc(sizeof(init_scene_t));

			if(!p_scene)
			{
				__err("p_scene malloc error \n");
				return EPDK_FAIL;
			}

			eLIBs_memset(p_scene, 0, sizeof(init_scene_t));
			GUI_WinSetAddData(msg->h_deswin, (__u32)p_scene);
			p_scene->init_win = msg->h_deswin;
			//p_scene->hbar_format = HBAR_FOARMAT_32BPP;
			/////////////////////////////////////////////////////////////////
			init_ctr->low_power_check_time_id	= LOW_POWER_CHECK_TIME_ID;
			init_ctr->cursor_time_id		   	= CURSOR_TIME_ID;
			init_ctr->close_scn_time_id	   		= CLOSE_SCN_TIME_ID;
			init_ctr->auto_off_time_id			= AUTO_OFF_TIME_ID;
			init_ctr->led_flash_time_id			= LED_FLASH_TIME_ID;
			init_ctr->led_flash_timeout = 25;
			/* 低电查询定时器 */
			init_ctr->low_power_check_timeout	= 100;							// 1s
			/* cursor 隐藏定时器 */
			init_ctr->cursor_timeout   			= 500;							// 5s
			/* 创建 timer */
		#if TIMEOUT_ID_TIMER_DET_UDISK
			GUI_SetTimer(msg->h_deswin, ID_TIMER_DET_UDISK, TIMEOUT_ID_TIMER_DET_UDISK, NULL);
		#endif
		#if !BOOT_FROM_SDMMC
			if(g_logoDisable)
			{
				GUI_SetTimer(msg->h_deswin, ID_TIMER_LOAD_APP, 1, NULL);
			}
			else
			{
				GUI_SetTimer(msg->h_deswin, ID_TIMER_LOAD_APP, 100, NULL);
			}
		#endif
			GUI_SetTimer(msg->h_deswin, init_ctr->led_flash_time_id, init_ctr->led_flash_timeout, NULL);
			GUI_SetTimer(msg->h_deswin, init_ctr->low_power_check_time_id, init_ctr->low_power_check_timeout, NULL);
#if OPEN_CURSOR
			GUI_SetTimer(msg->h_deswin, init_ctr->cursor_time_id, init_ctr->cursor_timeout, NULL);
#endif
#if TIMEOUT_ID_TIMER_KEYLED_OFF
			GUI_SetTimer(msg->h_deswin, ID_TIMER_KEYLED_OFF, 400, NULL);
#endif

			if(init_ctr->closescn_timeout != 0)
			{
				GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
			}

			/* 创建信号量 */
			init_ctr->state_sem = esKRNL_SemCreate(1);
			init_ctr->closescn_gate_on 	= EPDK_TRUE;
			init_ctr->usb_connect 	   	= EPDK_FALSE;
			init_ctr->screen_close_done = EPDK_FALSE;
			init_ctr->tp_msg_enble	   	= EPDK_TRUE;
			init_ctr->key_msg_enble	   	= EPDK_TRUE;
			//initialize res
			//live_init();
			init_server_init();
			//create cursor
#if OPEN_CURSOR
			init_cursor_create(init_ctr);
#endif
			//create hbar,bgd,dialog
			init_scene_create(msg);
			//后台线程处理耗时操作
			sys_msg_handle_init(msg);

			break;
		}

		case GUI_MSG_DESTROY:
		{
			__u8 err;
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			init_scene_t *p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);

			OWI_MExit();
			
#if ID_TIMER_LOAD_APP

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_LOAD_APP))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_LOAD_APP);
			}

#endif

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->low_power_check_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->cursor_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->cursor_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->led_flash_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->led_flash_time_id);
			}

#if TIMEOUT_ID_TIMER_KEYLED_OFF

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_KEYLED_OFF))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_KEYLED_OFF);
			}

#endif

		#if TIMEOUT_ID_TIMER_DET_UDISK
			if( GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_DET_UDISK))
				GUI_KillTimer(msg->h_deswin, ID_TIMER_DET_UDISK);
		#endif
			esKRNL_SemDel(init_ctr->state_sem, 0, &err);
			//删除后台线程
			sys_msg_handle_exit(msg);
			//delete hbar,bgd,dialog
			init_scene_destroy(msg);
			//delete cursor
#if OPEN_CURSOR
			init_cursor_delete(init_ctr);
#endif
			//deinit res
			live_exit();
			eLIBs_free(init_ctr);
			eLIBs_free(p_scene);
			__msg("init_mainwin destroyed...\n");
			break;
		}

		case GUI_MSG_CLOSE:
			__here__
			GUI_ManWinDelete(msg->h_deswin);
			break;

		case GUI_MSG_TIMER:
		{
			init_scene_t *p_scene;
			__init_ctl_t *init_ctr;
			init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);

			if(msg->dwAddData1 == init_ctr->cursor_time_id)  //光标存在时间
			{
				GUI_CursorHide();
			}
		#if TIMEOUT_ID_TIMER_DET_UDISK
			else if( msg->dwAddData1 == ID_TIMER_DET_UDISK)
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_DET_UDISK);
				//__msg_jjl("GUI_MSG_TIMER: Kill ID_TIMER_DET_UDISK...\n");
			}
		#endif
			else if(msg->dwAddData1 == init_ctr->low_power_check_time_id)  //低电检查
			{
				{
					static __u8 s_1sCnt = 0;

					if(++s_1sCnt >= 10)
					{
						s_1sCnt = 0;
						LogoBufRelease();
					}
				}
#if SP_PowerOffWithLowPower

				if(g_b_lowpower)
				{
					GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
					SEND_MSG(DSK_MSG_POWER_OFF, NULL, msg->h_deswin, 0, 0);
					__wrn("SEND_MSG(DSK_MSG_POWER_OFF)\n");
					break;
				}

#endif

				if(dsk_power_is_low() == EPDK_TRUE)  //低电
				{
					__msg("****************low power...****************\n");
					//Tarring
					//GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
					{
						// 记忆低电标志
						reg_system_para_t	 *system_para;
						system_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
						system_para->m_ePowerOffType = m_ePowerOffType_lowPower;
					}
					SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
					SEND_MSG(DSK_MSG_LOW_POWER, NULL, msg->h_deswin, 0, 0);
					{
						if(g_b_lowpower >= MAX_LowPowerCueCnt)
						{
							dsk_reg_flush();
							esKSRV_Reset();
						}
					}
				}
				else
				{
					//会出现大功率放电导致低电，而后在短时间内电压回升，进入此条件内.
					__msg("**************not low power...*********\n");

					if(g_b_lowpower)
					{
						g_b_lowpower = 0;
						//SEND_MSG(DSK_MSG_HOME, NULL, msg->h_deswin, 0, 0);
						//activity_load_app("application://app_root");
						//init_open_screen(msg);
						dsk_reg_flush();
						esKSRV_Reset();
						break;
					}
				}

			#if 1
				if(lz_drv_fail_flag)
				{
					static __u8 s_timerLzDrvFail = 0;
					s_timerLzDrvFail++;
					s_timerLzDrvFail %= 30;

					eLIBs_printf("&&&& s_timerLzDrvFail=%d &&&&\n", s_timerLzDrvFail);
					if(!s_timerLzDrvFail)
					{
						//IOCTRL__PA_SHDN__XOR();
						//SEND_MSG(DSK_MSG_SCREEN_FORCE_CLOSE, NULL, msg->h_deswin, 0, 0);
						SEND_MSG(DSK_MSG_LOW_POWER, NULL, msg->h_deswin, 0, 0);
					}
				}
			#endif
			}
			else if(msg->dwAddData1 == LED_FLASH_TIME_ID)
			{
				#if (!SP_DEV_HW_PW_OFF)
				{
					IOCTRL__PWOFF_DET__SET_OFF();
					{
						__u32 i = 100;

						while(i--);
					}

					if(IOCTRL__PWOFF_DET__IS_OFF())
					{
						if(g_b_off_check)
						{
#if ID_TIMER_LOAD_APP

							if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_LOAD_APP))
							{
								GUI_KillTimer(msg->h_deswin, ID_TIMER_LOAD_APP);
							}

#endif
							//IOCTRL__ENCRY__SET_FAIL();
							SEND_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
							SEND_MSG(DSK_MSG_POWER_OFF, NULL, msg->h_deswin, 0, 0);
						}
						else
						{
							g_b_off_check = 1;
						}
					}
					else
					{
						g_b_off_check = 0;
					}
				}
				#endif
				
				IOCTRL__PWON__SET_ON();//开机时打开电源控制
			}
			else if(msg->dwAddData1 == LOW_POWER_DIALOG_TIME_ID)  //低电对话框存在时间
			{
				p_scene->low_power = EPDK_FALSE;
				GUI_KillTimer(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID);
				scene_on_dialog(msg);
			}
			else if(msg->dwAddData1 == POWER_OFF_DIALOG_TIME_ID)  //关机对话框存在时间
			{
				__here__;
				p_scene->power_off = EPDK_FALSE;
				GUI_KillTimer(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID);
				scene_on_dialog(msg);
				__here__;
			}
			else if(msg->dwAddData1 == ORCHID_UPDATE_DIALOG_TIME_ID)  //媒体库升级对话框存在时间
			{
				if(p_scene->orchid_update == EPDK_FALSE)
				{
					GUI_KillTimer(msg->h_deswin, ORCHID_UPDATE_DIALOG_TIME_ID);
					scene_on_dialog(msg);
				}
			}
			else if(msg->dwAddData1 == init_ctr->close_scn_time_id)  //自动关屏时间
			{
				__here__;
				SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, msg->h_deswin, 0, 0);
				__here__;
			}
			else if(msg->dwAddData1 == init_ctr->auto_off_time_id) //自动关机时间
			{
				__here__;
				SEND_MSG(DSK_MSG_POWER_OFF, NULL, msg->h_deswin, 0, 0);
				__here__;
			}

#if TIMEOUT_ID_TIMER_KEYLED_OFF
			else if(msg->dwAddData1 == ID_TIMER_KEYLED_OFF)
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_KEYLED_OFF);
				IOCTRL__LEDKEY__SET_OFF();
			}

#endif
#if ID_TIMER_LOAD_APP
			else if(msg->dwAddData1 == ID_TIMER_LOAD_APP)
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_LOAD_APP);
				activity_load_app("application://app_root");
			}

#endif
			else
			{
				__wrn(" unknown msg timer id = %d \n", msg->dwAddData1);
			}

			break;
		}

		case GUI_MSG_KEY:
		{
			__bool exist;
			__init_ctl_t *init_ctr;
			init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			__msg("msg->dwReserved=%d\n", msg->dwReserved);
			__msg("key value = %d, key action = %d\n", msg->dwAddData1, msg->dwAddData2);
			{
				//重新设置关屏定时器
				exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

				if(exist == EPDK_TRUE)
				{
					GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
				}

				//重新设置关机定时器
				exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

				if(exist == EPDK_TRUE)
				{
					GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
				}
			}

		//	if(EPDK_OK == DbgMode__AdjustDispParam(msg))	return EPDK_OK;

			#if 0
			if (EPDK_FAIL ==__AdjustDispParam_ForDVD(msg))	return EPDK_OK;
			#endif
			
			if(init_ctr->hdleOwi)
			{
				if(EPDK_OK == shift_key_to_ir_tx(msg, init_ctr->hdleOwi))	return EPDK_OK;
			
			}

			switch(msg->dwAddData1)
			{
				case GUI_MSG_KEY_LONGVADD:
				case GUI_MSG_KEY_VADD:
				{
					if(msg->dwAddData2 != KEY_UP_ACTION)
					{
						//dsk_volume_inc();
					}

					return GUI_ManWinDefaultProc(msg);
				}

				case GUI_MSG_KEY_LONGVDEC:
				case GUI_MSG_KEY_VDEC:
				{
					if(msg->dwAddData2 != KEY_UP_ACTION)
					{
						//dsk_volume_dec();
					}

					return GUI_ManWinDefaultProc(msg);
				}

				case GUI_MSG_KEY_LOCKSCREEN:
				case GUI_MSG_KEY_LONGLOCKSCREEN:
				{
					if(g_tconOccupyLcmi_flag)
					{
						return GUI_ManWinDefaultProc(msg);
					}
					else
					{
						if(KEY_UP_ACTION == msg->dwAddData2)
						{
							init_screen_onoff(msg);
						}

						return EPDK_OK;
					}
				}
				
				case GUI_MSG_KEY_ENTER:
				case GUI_MSG_KEY_LONGENTER:
				case GUI_MSG_KEY_MENU:
				case GUI_MSG_KEY_LONGMENU:
				{
					if(msg->dwAddData2 != KEY_UP_ACTION)
					{
						reg_system_para_t* para;
						para = (reg_system_para_t*)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
						if(para)
						{
							para->m_knlTime = esKRNL_Time();
						}
					}
					return GUI_ManWinDefaultProc(msg);
				}
					
				default:
					return GUI_ManWinDefaultProc(msg);
			}
		}

		case GUI_MSG_TOUCH:
		{
			__bool  exist;
			__pos_t pos;
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			//重新设置关屏定时器
			exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

			if(exist == EPDK_TRUE)
			{
				GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
			}

			//重新设置关机定时器
			exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

			if(exist == EPDK_TRUE)
			{
				GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
			}

			pos.x = LOSWORD(msg->dwAddData2);
			pos.y = HISWORD(msg->dwAddData2);
			__msg(" pos.x =%d, pos.y =%d \n", pos.x, pos.y);
#if OPEN_CURSOR
			GUI_CursorSetPos(&pos);
			GUI_CursorShow();
#endif
			exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->cursor_time_id);

			if(exist == EPDK_TRUE)
			{
				GUI_ResetTimer(msg->h_deswin, init_ctr->cursor_time_id, init_ctr->cursor_timeout, NULL);
			}

			return GUI_ManWinDefaultProc(msg);
		}

		case HEADBAR_CMD_SET_FORMAT:
		{
			//init_scene_t * p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			//p_scene->hbar_format   = (__hbar_format_t)msg->dwAddData1;
			//gscene_hbar_delete();
			//gscene_hbar_create(msg->h_deswin, (__hbar_format_t)msg->dwAddData1);
			break;
		}

		/*case HEADBAR_CMD_GET_SCN_POS:
		{
			RECT hbar_rect;
			init_scene_t * p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			GUI_LyrWinGetScnWindow(p_scene->hbar_layer, &hbar_rect);

			return ((hbar_rect.y<<16) + hbar_rect.x);
		}

		case HEADBAR_CMD_GET_SIZE:
		{
			RECT hbar_rect;
			init_scene_t * p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			GUI_LyrWinGetScnWindow(p_scene->hbar_layer, &hbar_rect);

			return ((hbar_rect.height<<16) + hbar_rect.width);
		}*/
		case DSK_MSG_USBD_PLUG_IN:				/* usb device plug in */
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			init_scene_t *p_scene;
			__bool        other_dlg_exist;//连接usb时，是否存在其它模式框
			p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			//提前设置usb连接标志，防止usb连接时，低电或者关机对话框被强制关闭后程序还往下跑，导致应用被结束
			p_scene->usb_plug_in = EPDK_TRUE;
			__here__;
			other_dlg_exist = EPDK_FALSE;
			//连接usb的时候禁止自动关屏和关机
			init_ctr->autooff_timer_exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

			if(EPDK_TRUE == init_ctr->autooff_timer_exist)
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
			}

			init_ctr->closescn_timer_exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

			if(EPDK_TRUE == init_ctr->closescn_timer_exist)
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
			}

			// 禁止消息响应
			init_ctr->tp_msg_enble = EPDK_FALSE;
			__here__;
			//USB连接到来的时候可能此时提示正在关机或者低电对话框，先结束掉他们
			//并且设置标记不让它往下跑，从而不关机。
			__msg("p_scene->power_off=%d\n", p_scene->power_off);

			if(p_scene->power_off == EPDK_TRUE)
			{
				other_dlg_exist = EPDK_TRUE;
				__here__;
				p_scene->power_off = EPDK_FALSE;

				if(GUI_IsTimerInstalled(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID))
				{
					__here__;
					GUI_KillTimer(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID);
					scene_on_dialog(msg);
					__here__;
				}
			}

			__here__;

			if(p_scene->low_power == EPDK_TRUE)
			{
				other_dlg_exist = EPDK_TRUE;
				__here__;
				p_scene->low_power = EPDK_FALSE;

				if(GUI_IsTimerInstalled(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID))
				{
					__here__;
					GUI_KillTimer(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID);
					scene_on_dialog(msg);
				}
			}

			//如果连接usb时，存在其它模式框，则必须投递消息的方式通知usb连接，
			//否则关闭其它对话框后，对话框消息循环未结束，会造成死锁
			if(EPDK_TRUE == other_dlg_exist)
			{
				__gui_msg_t set_msg;
				eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
				GUI_SendNotifyMessage(&set_msg);
			}
			else
			{
				__here__;
				init_usb_device_plugin_proc(msg);
				__here__;
				NOTIFY_MSG(DSK_MSG_ORCHID_UPDATE_START, NULL, msg->h_deswin, 0, 0);
				__here__;
			}

			break;
		}

		case DSK_MSG_USBD_PLUG_OUT:				/* usb devcie plug out */
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			__here__;
			init_usb_device_plugout_proc(msg);
			init_ctr->tp_msg_enble = EPDK_TRUE;
			__here__;
			//usb连接拔出后，需重置低电查询定时器，因为可能低电
			GUI_ResetTimer(msg->h_deswin, init_ctr->low_power_check_time_id, init_ctr->low_power_check_timeout, NULL);
			GUI_ResetTimer(msg->h_deswin, init_ctr->led_flash_time_id, init_ctr->led_flash_timeout, NULL);
			//usb连接拔出后，重置自动关机和自动关屏定时器
			__wrn("init_ctr->autooff_timer_exist=%d\n", init_ctr->autooff_timer_exist);
			__wrn("init_ctr->autooff_timeout=%d\n", init_ctr->autooff_timeout);

			if(EPDK_TRUE == init_ctr->autooff_timer_exist)
			{
				if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id))
				{
					GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
				}
				else
				{
					GUI_SetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
				}
			}

			__wrn("init_ctr->closescn_timer_exist=%d\n", init_ctr->closescn_timer_exist);
			__wrn("init_ctr->closescn_timeout=%d\n", init_ctr->closescn_timeout);

			if(EPDK_TRUE == init_ctr->closescn_timer_exist)
			{
				if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id))
				{
					GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
				}
				else
				{
					GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
				}
			}

			//低电提示框弹出后插入usb，重新拔出后
			//该定时器已被删除，需重置一下
			if(!GUI_IsTimerInstalled(msg->h_deswin, init_ctr->low_power_check_time_id))
			{
				GUI_SetTimer(msg->h_deswin, init_ctr->low_power_check_time_id, init_ctr->low_power_check_timeout, NULL);
			}

			if(!GUI_IsTimerInstalled(msg->h_deswin, init_ctr->led_flash_time_id))
			{
				GUI_SetTimer(msg->h_deswin, init_ctr->led_flash_time_id, init_ctr->led_flash_timeout, NULL);
			}

			break;
		}

		case DSK_MSG_ORCHID_UPDATE_START:
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			__here__
			//init_orchid_update_start(msg);
			__here__
		#if !BOOT_FROM_SDMMC
			activity_load_app("application://app_root");
		#endif
			__here__
			break;
		}

		case DSK_MSG_ORCHID_UPDATE_END:
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			__here__
			init_orchid_update_end(msg);
			__here__

			if(dsk_display_get_output_type() == DISP_OUTPUT_TYPE_LCD)
			{
				init_ctr->tp_msg_enble = EPDK_TRUE;
			}

			break;
		}

		case DSK_MSG_FS_PART_PLUGIN:
		{
			char diskname[4];
			__gui_msg_t msgex;
			__wrn("msg->dwAddData2=0x%X\n", msg->dwAddData2);
			diskname[0] = (__u8)(msg->dwAddData2);
			diskname[1] = '\0';
			__msg("disk %s plug in\n", diskname);

			if((diskname[0] == 'D') || (diskname[0] == 'E') || (diskname[0] == 'Z'))
			{
				return 0;
			}

			#if TIMEOUT_ID_TIMER_DET_UDISK
				if( GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_DET_UDISK))
				{
					eLIBs_printf("ID_TIMER_DET_UDISK is installed.\n");
					return 0;
				}
			#endif
			
			//notify top
			msgex.id 			= DSK_MSG_FS_PART_PLUGIN;
			msgex.h_srcwin 		= 0;
			msgex.h_deswin 		= 0;
			msgex.dwAddData1 	= msg->dwAddData1;
			msgex.dwAddData2 	= msg->dwAddData2;
			activity_notify_top(&msgex);
			break;
		}

		case DSK_MSG_FS_PART_PLUGOUT:
		{
			char diskname[4];
			__gui_msg_t msgex;
			diskname[0] = msg->dwAddData2;
			diskname[1] = '\0';
			__msg("disk %s plug out\n", diskname);

			if((diskname[0] == 'D') || (diskname[0] == 'E') || (diskname[0] == 'Z'))
			{
				return 0;
			}

			//notify top
			msgex.id 			= DSK_MSG_FS_PART_PLUGOUT;
			msgex.h_srcwin 		= 0;
			msgex.h_deswin 		= 0;
			msgex.dwAddData1 	= msg->dwAddData1;
			msgex.dwAddData2 	= msg->dwAddData2;
			activity_notify_top(&msgex);

			if(dsk_wkm_is_open())
			{
				char filename[512];
				dsk_wkm_get_now_playing_file(filename);
				__msg("filename = %s\n", filename);

				if(0 == eLIBs_strnicmp(filename, diskname, 1))
				{
					__here__
					dsk_wkm_close();
				}
			}

			break;
		}

		case DSK_MSG_TCON_RELEASE_LCMI :
			__wrn("DSK_MSG_TCON_RELEASE_LCMI\n");
			g_tconOccupyLcmi_flag = 0;

		case DSK_MSG_SCREEN_OPEN:				/* 开屏 */
			init_open_screen(msg);
			{
				__gui_msg_t msgex;
				msgex.id				= DSK_MSG_SCREEN_OPEN;
				msgex.h_srcwin		= 0;
				msgex.h_deswin		= 0;
				msgex.dwAddData1	= msg->dwAddData1;
				msgex.dwAddData2	= msg->dwAddData2;
				activity_notify_top(&msgex);
			}
			break;

		case DSK_MSG_LOW_POWER:				/* low power */
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

			//Tarring
			//break;//112350

			if(init_ctr->power_off)
			{
				break;
			}

			init_ctr->power_off = 1;
			init_restart_close_scn(msg);
			init_low_power_proc(msg);

		//	init_power_off_proc(msg);
	
			init_ctr->power_off = 0;
			g_b_lowpower++;
			IOCTRL__ATV_PW__SET_OFF();
			//pull_down_gpio(3, 8 );
			break;
		}
	

		case DSK_MSG_ENABLE_STANDBY:
			init_lock(msg);
			g_b_enable_standby = 1;
			init_unlock(msg);
			break;

		case DSK_MSG_DISABLE_STANDBY:
			init_lock(msg);
			g_b_enable_standby = 0;
			init_unlock(msg);
			break;

		case DSK_MSG_GET_STANDBY_FLAG:
		{
			__bool flag;
			init_lock(msg);
			flag = g_b_enable_standby;
			init_unlock(msg);
			return flag;
		}
		
  
     
		case DSK_MSG_POWER_OFF: 			/* power off */
		{


			init_scene_t *p_scene;
			__init_ctl_t *init_ctr;
#if ID_TIMER_LOAD_APP

			if(GUI_IsTimerInstalled(msg->h_deswin, ID_TIMER_LOAD_APP))
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_LOAD_APP);
			}

#endif

			p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			//Tarring
			//break;//112350
			__wrn("DSK_MSG_POWER_OFF\n");

			if(p_scene->usb_plug_in)//如果usb连接当中，那么不关机
			{
				break;
			}

			__msg_jjl("init_ctr->power_off=%d\n", init_ctr->power_off);

			if(init_ctr->power_off)
			{
				break;
			}

			__wrn("");

			if(msg->dwAddData2 == KEY_UP_ACTION/*  && dsk_get_charge_state()==EPDK_FALSE*/)//add by Kingvan
			{
				init_ctr->power_off = EPDK_TRUE;
				//关屏计时开始
				init_restart_close_scn(msg);
				__wrn("");
				init_power_off_proc(msg);
				__wrn("");
				//pull_down_gpio(3, 8 );
				IOCTRL__ATV_PW__SET_OFF();
			}

			break;
		}

		/*case DSK_MSG_HOLD:
		{
			__init_ctl_t * init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			if(init_ctr->closescn_gate_on == EPDK_TRUE)
			{
				init_close_screen(msg);
			}
			break;
		}*/
		case DSK_MSG_STANDBY_WAKE_UP:
			__here__
			init_open_screen(msg);
			break;

		case DSK_MSG_TVDAC_PLUGIN:
			__msg("************DSK_MSG_TVDAC_PLUGIN*****************\n");
			break;

		case DSK_MSG_TVDAC_PLUGOUT:
		case DSK_MSG_HDMI_PLUGOUT:
		{
			__msg("************DSK_MSG_TVDAC_PLUGOUT/DSK_MSG_HDMI_PLUGOUT*****************\n");

			//切屏
			if(dsk_display_get_output_type() != DISP_OUTPUT_TYPE_LCD)
			{
				__gui_msg_t msgex;
				__here__;
				//notify top
				msgex.id 			= msg->id;
				msgex.h_srcwin 		= 0;
				msgex.h_deswin 		= 0;
				msgex.dwAddData1 	= msg->dwAddData1;
				msgex.dwAddData2 	= msg->dwAddData2;
				activity_notify_top(&msgex);
			}

			__here__;
			break;
		}

		case DSK_MSG_SCREEN_SWITCH:
		{
			__init_ctl_t *init_ctr;
			init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

			if(msg->dwAddData1 == LION_DISP_LCD)		// 切换到LCD，打开触摸屏消息
			{
				init_lock(msg);
				init_ctr->tp_msg_enble = EPDK_TRUE;
				init_unlock(msg);
			}
			else											// 切换到tv , 关闭触摸屏消息
			{
				init_lock(msg);
				init_ctr->tp_msg_enble = EPDK_FALSE;
				init_unlock(msg);
			}

			//重新创建场景
			init_scene_destroy(msg);
			init_scene_create(msg);
			break;
		}

		case DSK_MSG_ASSISTANT_SCENE_CREATE:
		{
			/*init_scene_t *p_scene;
			ES_FILE 	 *p_disp = NULL;
			p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			__here__
			if(!p_scene->assistant_win)
			{
				//创建辅屏场景
				GUI_SetScnNo(1);
				p_scene->assistant_win = assistant_scene_create(msg->h_deswin);

				// open display driver
				p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
				if( p_disp == NULL )
				{
				    __err("Error in opening display driver.\n");
				    return EPDK_FAIL;
				}
				eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 1, 0);
				eLIBs_fclose(p_disp);
				p_disp = NULL;

				GUI_SetScnNo(0);
			}*/
			break;
		}

		case DSK_MSG_ASSISTANT_SCENE_DELETE:
		{
			/*init_scene_t *p_scene;
			ES_FILE 	 *p_disp = NULL;
			p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
			__here__
			if(p_scene->assistant_win)
			{
				__here__
				GUI_SetScnNo(1);
				assistant_scene_delete(p_scene->assistant_win);
				p_scene->assistant_win = NULL;

				p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
				if( p_disp == NULL )
				{
				    __err("Error in opening display driver.\n");
				    return EPDK_FAIL;
				}
				eLIBs_fioctrl(p_disp, DISP_CMD_LCD_OFF, 1, 0);
				eLIBs_fclose(p_disp);
				p_disp = NULL;

				GUI_SetScnNo(0);
			}*/
			break;
		}

		case DSK_MSG_SET_SCN_CLOSE:
			init_set_close_scn(msg);
			break;

		case DSK_MSG_SCREEN_CLOSE:
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			__msg("Tarring-----close---------\n");

			if(init_ctr->closescn_gate_on == EPDK_TRUE)
			{
				init_close_screen(msg, 0);
				{
					__gui_msg_t msgex;
					msgex.id				= DSK_MSG_SCREEN_CLOSE;
					msgex.h_srcwin		= 0;
					msgex.h_deswin		= 0;
					msgex.dwAddData1	= msg->dwAddData1;
					msgex.dwAddData2	= msg->dwAddData2;
					activity_notify_top(&msgex);
				}
			}

			break;
		}

		case DSK_MSG_RELEASE_LOGO_BUF :
			LogoBufRelease();
			break;

		case DSK_MSG_SCREENOFF_BKOFF :
			g_screenCloseSel = OP_SCREENOFF_AND_BKOFF;
			return EPDK_OK;

		case DSK_MSG_SCREENON_BUT_BKOFF :
			g_screenCloseSel = OP_SCREENON_BUT_BKOFF;		// 关屏时，只关背光
			return EPDK_OK;

		case DSK_MSG_TCON_CLOSE_SCREEN :
			g_tconOccupyLcmi_flag = -1;
			return EPDK_OK;

		case DSK_MSG_TCON_OCCUPY_LCMI :
			__wrn("DSK_MSG_TCON_OCCUPY_LCMI\n");
			g_tconOccupyLcmi_flag = 1;
			g_screenCloseSel = OP_SCREENOFF_BUT_BKON;		// 关屏时，背光要打开

		case DSK_MSG_SCREEN_FORCE_CLOSE:
		{
			__wrn("Tarring-----Force close---------\n");
			init_close_screen(msg, 1);
			{
				__gui_msg_t msgex;
				msgex.id				= DSK_MSG_SCREEN_FORCE_CLOSE;
				msgex.h_srcwin		= 0;
				msgex.h_deswin		= 0;
				msgex.dwAddData1	= msg->dwAddData1;
				msgex.dwAddData2	= msg->dwAddData2;
				activity_notify_top(&msgex);
			}
			break;
		}

		case DSK_MSG_CLOSE_SCN_GATE_OFF:
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			init_ctr->closescn_gate_on = EPDK_FALSE;
			break;
		}

		case DSK_MSG_CLOSE_SCN_GATE_ON:
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			init_ctr->closescn_gate_on = EPDK_TRUE;
			break;
		}

		case DSK_MSG_HOME:
		{
			__gui_msg_t msgex;
			__here__;
			//notify top
			eLIBs_memcpy(&msgex, msg, sizeof(__gui_msg_t));
			activity_notify_top(&msgex);
			break;
		}

		case DSK_MSG_ALARM:
		{
			__gui_msg_t msgex;
			init_open_screen(msg);
			//notify top
			msgex.id 			= msg->id;
			msgex.h_srcwin 		= 0;
			msgex.h_deswin 		= 0;
			msgex.dwAddData1 	= msg->dwAddData1;
			msgex.dwAddData2 	= msg->dwAddData2;
			activity_notify_top(&msgex);
			break;
		}

		case DSK_MSG_SET_AUTO_OFF:
		{
			init_set_auto_off(msg);
			break;
		}

		case DSK_MSG_FW_UPDATE:
		{
			__here__;
			/* delay */
			esKRNL_TimeDly(200);
			__msg("********dsk_fw_update*******\n");
			/* 执行升级过程 */
			dsk_fw_update();
			__here__;
			esKRNL_TimeDly(200);
			__here__;
			break;
		}

		case DSK_MSG_FW_START_UPDATE: //升级固件
		{
			__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
			__msg("msg->dwAddData1=%s\n", msg->dwAddData1);

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->low_power_check_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->cursor_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->cursor_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
			}

			if(GUI_IsTimerInstalled(msg->h_deswin, init_ctr->led_flash_time_id))
			{
				GUI_KillTimer(msg->h_deswin, init_ctr->led_flash_time_id);
			}

			__here__;
			activity_finish_all();

		#if !BOOT_FROM_SDMMC
			__here__;
			gscene_bgd_set_flag(EPDK_TRUE);
			__here__;
			gscene_bgd_refresh();
			__here__;
			gscene_hbar_set_state(HBAR_ST_HIDE);
		#endif
		
			__here__;
			fw_update(msg, (char *)(msg->dwAddData1));
			__here__;
			break;
		}

		/*case GUI_MSG_COMMAND:
		{
			if(ID_SCREEN_LOCK == LOSWORD(msg->dwAddData1) && SCREEN_LOCK_EXIT == HISWORD(msg->dwAddData1))
			{
				__gui_msg_t msgex;
				init_scene_t * p_scene 	= (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
				delete_screen_lock(p_scene->scnlock_win);

				msgex.id 			= DSK_MSG_SCREEN_OPEN;
				msgex.h_srcwin 		= 0;
				msgex.h_deswin 		= 0;
				msgex.dwAddData1 	= 0;
				msgex.dwAddData2 	= 0;
				activity_notify_top(&msgex);
			}
			break;
		}*/
		
		case DSK_MSG_RELEASE_TIMER_DET_UDISK:	
		#if TIMEOUT_ID_TIMER_DET_UDISK
			{
				GUI_KillTimer(msg->h_deswin, ID_TIMER_DET_UDISK);
				//__msg_jjl("DSK_MSG_RELEASE_TIMER_DET_UDISK: Kill ID_TIMER_DET_UDISK...\n");
			}
		#endif
			break;

	#if SP_DEV_OWI
		case DSK_MSG_APP_MODE_ENTER:
		{
			char *pstr = (char *)(msg->dwAddData1);
			if(!strcmp(pstr, "DVD"))
			{
				__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
				if(!init_ctr->hdleOwi)
				{
					S_TX_NEC_PARAM sTxNecParam;
					__s32 ret;
					ret = esCFG_GetKeyValue("ir_para", "IR_TX", (int *)(&sTxNecParam.sGpioSet), sizeof(user_gpio_set_t) / sizeof(int));					
					if(EPDK_OK != ret)
					{
						__wrn("read ir tx para fail.\n");
					}
					else
					{
						__msg_jjl("OWI_MOpen():begin, (%d,%d,%d)\n", 
							sTxNecParam.sGpioSet.port, sTxNecParam.sGpioSet.port_num, sTxNecParam.sGpioSet.data);
					init_ctr->hdleOwi = OWI_MOpen(m_eOwiType_txNec, &sTxNecParam);
					__msg_jjl("init_ctr->hdleOwi=0x%X\n", init_ctr->hdleOwi);
					}
				}	
			}
			break;
		}
		
		case DSK_MSG_APP_MODE_EXIT:
		{
			char *pstr = (char *)(msg->dwAddData1);
			if(!strcmp(pstr, "DVD"))
			{
				__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
				if(init_ctr->hdleOwi)
				{
					OWI_MClose(init_ctr->hdleOwi);
					init_ctr->hdleOwi = 0;
				}
			}
			break;
		}
	#endif

		case DSK_MSG_MIC_PLUGIN:
		case DSK_MSG_MIC_PLUGOUT:
		case DSK_MSG_AUX_PLUGIN:
		case DSK_MSG_AUX_PLUGOUT:
		{
			__gui_msg_t msgex;
			msgex.id 			= msg->id;
			msgex.h_srcwin 		= 0;
			msgex.h_deswin 		= 0;
			msgex.dwAddData1 	= msg->dwAddData1;
			msgex.dwAddData2 	= msg->dwAddData2;
			activity_notify_top(&msgex);
		}
		
		default:
			GUI_ManWinDefaultProc(msg);
			break;
	}

	return EPDK_OK;
}

H_WIN init_mainwin_create(void)
{
	H_WIN  						hManWin;
	__gui_manwincreate_para_t  	create_info;
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.attr            = NULL;
	create_info.hParent			= NULL;
	create_info.hHosting 		= NULL;
	create_info.ManWindowProc   = init_mainwin_cb;
	create_info.name            = "init";
	hManWin 					= GUI_ManWinCreate(&create_info);

	if(hManWin == NULL)
	{
		__wrn("init: create main windows failed!\n");
		return NULL;
	}

	return hManWin;
}

