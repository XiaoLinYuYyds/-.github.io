/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : Esh_shell.c
* By      : sunny
* Version : V2.0                                                            2009.3.2
*********************************************************************************************************
*/
#include    "Esh_shell_private.h"

__bool pass_flag = EPDK_FALSE;

static void __Esh_Begin_Info(void)
{
	__inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	__inf("&                                                                                                                   &\n");
	__inf("&                                                      FFBBEE                                                       &\n");
	__inf("&                          I BBBBBBBBBBBB            I BBBB                            MMI         BBLL             &\n");
	__inf("&                    BBBBB@STEVEN@BMMBBBBBB          BBBBI                           BBBBFF      BBBBFF             &\n");
	__inf("&                  BBBBBBBBMMBBBBBBMMEEBBLL        BBBBBB                          BBBBBB::    BBBBBB..             &\n");
	__inf("&                BBBBBBBBBB        LLBB.         BBBBBB                          BBBB  BB    EEBB  BB               &\n");
	__inf("&                BBBB..          BB            I BBBB.                           BBI FFLL    BB..FFFF               &\n");
	__inf("&              I BBFF                          BBBBLL                          BBBB  BB    BBBB  BB                 &\n");
	__inf("&              ::BBBBBB                      BBBBBB    BB          BBBBBBBB    BBLLBB::    BBLLBB::                 &\n");
	__inf("&                LLBBBBBBBBBB              FFBBBB  . BBBBBB    . BBBB  BBFF  BBBBBBEE    BBBBBBBB                   &\n");
	__inf("&                      BBBBBBEEBB          BBBB  LLBBBBBB      BBBB  EEBB    BBBBBB      BBBBBB                     &\n");
	__inf("&                              BBBB::    BBBB. EEBBI BB      BBBB  BBEE    . BBBB      . BBBB                       &\n");
	__inf("&                                BBBB    MMBBLLBB  BBLL    ..BBBBBB        I BB::      BBBB.       ..               &\n");
	__inf("&                              BBBBBB    BBEEBB::LLBB    BBFFBBEE      LLBBEEBB::    BBBBBB..    BB                 &\n");
	__inf("&          BB::          BBBBBBBBBBFF    BBBBFF  BBBBBBBB  I BBBB    BBBB  I BBBB::BB::FFBBBB..BB..                 &\n");
	__inf("&        BBBBBBBBMMBBBBBBBBMMMMBBBB      BBBB    BBBBBB      BBBBBBBBBB      BBBBBBI     BBBBBBI                    &\n");
	__inf("&        BBBB@V1.0@BBMMBBBBBBBB                                BBBB            BB          BB                       &\n");
	__inf("&            ::BBBBMMBB                                                                                             &\n");
	__inf("&                                                                                                                   &\n");
	__inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	__inf("-----Please input cmd after \"work_dir>:\"-----------------------------------------------------------------------------\n");
	__inf("-----Input \"help\" command for more information!----------------------------------------------------------------------\n");
	__inf("-----Give your requirement or sugguestion to sunny for improvement---------------------------------------------------\n");
	__inf("---------------------------------------------------------------------------------------------------------------------\n");
	__inf("\n");
}

static __s32 __key_graber(void *packet)
{
	__u32 i;
	__input_event_packet_t *pPacket;
	__input_event_t        *pEventFrame;

	if(packet == NULL)
	{
		__msg("invalid argument for call back\n");
		return EPDK_FAIL;
	}

	//dump packet events
	pPacket = (__input_event_packet_t *)packet;

	if(pPacket->event_cnt > INPUT_MAX_EVENT_NR)
	{
		__msg("invalid event count number\n");
		return EPDK_FAIL;
	}

	__msg("------------------------------------------------\n");

	for(i = 0; i < pPacket->event_cnt; i++)
	{
		pEventFrame = &(pPacket->events[i]);
		__msg("Event %d: type = %d, code = %d, value = %d\n", i + 1,
		      pEventFrame->type,
		      pEventFrame->code,
		      pEventFrame->value
		     );

		if(pEventFrame->type == EV_KEY)
		{
			pass_flag = EPDK_TRUE;
		}
	}

	__msg("------------------------------------------------\n");
	return EPDK_OK;
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

__s32 shellmain(void *p_arg)
{
	__target_para_t target;
	
	IOCTRL__ENCRY__SET_OK();
	IOCTRL__PA_SHDN__SET_EN();//开机时关闭功放
	IOCTRL__PWON__SET_ON();//开机时打开电源控制
//	IOCTRL__ATV_PW__SET_OFF();
#if (SP_KB_TYPE == DEV_LED_EFFECT1)
	IOCTRL__LEDKEY__SET_ON();
#endif
#if SP_DEV_DVD
	IOCTRL__DVD_PW__SET_OFF();
#endif
	pull_down_gpio(5, 2);

	__Esh_Begin_Info();

	esKSRV_GetTargetPara(&target);
	if(target.debugenable == EPDK_TRUE)
	{
		__here__;
		esDEV_Plugin("drv\\sdmmc.drv", 0, 0, 1);  //112350 如果是调试，则加载sdmmc
	}

	esDEV_Plugin("\\drv\\pwm.drv", 0, 0, 1);  //挂载驱动
	
	esDEV_Plugin("\\drv\\display.drv", 0, 0, 1);

	__msg(".Display Device Installed!...........................................................................................\n");
	//esMEMS_Info();

	//__wait__;
	/* print shell begin information */

	esDEV_Plugin("\\drv\\key.drv", 0, 0, 1);
#if 1
	esKSRV_GetTargetPara(&target);
	__msg("target.debugenable == %d\n", target.debugenable);
	if(target.debugenable == EPDK_TRUE)
	{
		__hdle  		h_keyGraber;

		//esDEV_Plugin("\\drv\\ir.drv", 0, 0, 1);
		//enable device monitor
		if(0)
		{
			ES_FILE     *pHwsc;
			pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");

			if(pHwsc)
			{
				eLIBs_fioctrl(pHwsc, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0, NULL);
				eLIBs_fclose(pHwsc);
				__inf(".start device monitor .......................................\n");
			}
			else
			{
				__err("try to open b:\\HWSC\\hwsc failed!\n");
			}
		}

		{
			h_keyGraber = esINPUT_LdevGrab(INPUT_LKEYBOARD_DEV_NAME, (__pCBK_t)__key_graber, 0, 0);

			if(h_keyGraber == NULL)
			{
				eLIBs_printf("shell grab logical keyboard failed\n");
			}
		}

		Esh_printf(".....................................................................................................................\n");
		Esh_printf(".Please press enter key(keypad or irkey) to continue!\n..............................................................\n");
		Esh_printf(".....................................................................................................................\n");
		while(1)
		{
			if(pass_flag)
			{
				if(h_keyGraber)
				{
					esINPUT_LdevRelease(h_keyGraber);
				}

				break;
			}

			esKRNL_TimeDly(10);
		}
	}

#else
	//enable device monitor
	{
		ES_FILE     *pHwsc;
		pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");

		if(pHwsc)
		{
			eLIBs_fioctrl(pHwsc, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0, NULL);
			eLIBs_fclose(pHwsc);
			__inf(".start device monitor .......................................\n");
		}
		else
		{
			__err("try to open b:\\HWSC\\hwsc failed!\n");
		}
	}
#endif

	/* Eshell initialize  */
	if(ESH_FAILED == Esh_init())
	{
		Esh_Error("Eshell initialize failed\n");
		return ESH_FAILED;
	}

	/* execute setup first */
	Esh_StartUp();

	if(target.debugenable == EPDK_TRUE)
	{
		/* loop read commands, execute commands */
		Esh_ReaderLoop();
	}

	/* Eshell exit */
	Esh_exit();
	/* kill shell self */
	esEXEC_PDel(EXEC_pidself);
	/* del shell main thread */
	Esh_Msg("shell main thread: Bye Bye!\n");
	esKRNL_TDel(OS_PRIO_SELF);
	esKRNL_TimeDly(1000);
	return ESH_SUCCEED;
}
