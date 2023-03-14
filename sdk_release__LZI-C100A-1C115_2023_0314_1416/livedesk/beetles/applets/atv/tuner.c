#include "beetles_app.h"

#if SP_APP_ATV


extern __s8		g_isAVInput;

__bool			g_isTconOccupyLcmi_flag = 2;

#if (SP_DEV_TCON_EX == DEV_TCON_EX_BIT1628M)
#define VALUE_IsWeakSignalGate		0x7242
void TCON_ProcVTRC(ES_FILE *a_pEsFile_drvAtv)
{
	ES_FILE 			*pEsFile_Atv = a_pEsFile_drvAtv;

	if(pEsFile_Atv == NULL)
	{
		return;
	}

	{
		__u32	t_data = 0;
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TUNER_READ_R, 0x0138, (void *)(&t_data));
		__wrn("5218(0x0138)=0x%04X\n", t_data);

		if(0)
		{
			t_data = 0;
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TUNER_READ_R, 0x0137, (void *)(&t_data));
			__wrn("5218(0x0137)=0x%04X\n", t_data);
			return;
		}

		if((t_data & 0x0F) == 2)
		{
			t_data = 0;
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TUNER_READ_R, 0x0137, (void *)(&t_data));
			__wrn("5218(0x0137)=0x%04X\n", t_data);

			if(t_data < VALUE_IsWeakSignalGate)
			{
				t_data = 0;
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
				__wrn("1628(0x00B6)=0x%04X\n", t_data);
				t_data |= 1 << 2;
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_WRITE_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
				return;
			}
		}

		{
			t_data = 0;
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg1 << 8) | 0xBC, (void *)(&t_data));
			__wrn("1628(0x01BC)=0x%04X\n", t_data);

			if(t_data & (1 << 3))
			{
				__u32	t_data1 = 0;
				t_data = 0;
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg1 << 8) | 0xBB, (void *)(&t_data1));
				__wrn("1628(0x01BB)=0x%04X\n", t_data1);
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg1 << 8) | 0xB9, (void *)(&t_data));
				__wrn("1628(0x01B9)=0x%04X\n", t_data);
				t_data |= (t_data1 << 4) & 0x0F00;

				if(t_data > 1150)
				{
					t_data = 0;
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
					__wrn("1628(0x00B6)=0x%04X\n", t_data);
					t_data |= 1 << 2;
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_WRITE_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
					esKRNL_TimeDly(2);
					__wrn("1628(0x00B6)=0x%04X\n", t_data);
					t_data &= ~(1 << 2);
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_WRITE_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
				}

				return;
			}
			else
			{
				t_data = 0;
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg1 << 8) | 0xBF, (void *)(&t_data));
				__wrn("1628(0x01BF)=0x%04X\n", t_data);

				if(!(t_data & (1 << 3)))
				{
					t_data = 0;
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
					__wrn("1628(0x00B6)=0x%04X\n", t_data);
					t_data |= 1 << 2;
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_WRITE_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
					__wrn("1628(0x00B6)=0x%04X\n", t_data);
					esKRNL_TimeDly(2);
					t_data &= ~(1 << 2);
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_WRITE_R, (1 << 16) | (IIC_CHIP_TconReg0 << 8) | 0xB6, (void *)(&t_data));
				}
			}
		}
	}
}
#endif

#if (SP_AUX_DET == OP_AUX_DET_FOR_AVIN_LINEIN)
__s32 TCON_CVBSDetect(__u8 openChannelIdx, PS_TUNER_INFO a_psTunerInfo)
{
#if SP_DEV_TCON_EX
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	__u32 			tconBrightness = 0x80;
	reg_system_para_t *para;

	if(pEsFile_Atv == NULL)
	{
		return 0;
	}

#ifdef TCON_SIGNAL_RGB
	dsk_display_lcd_off(OP_SCREENON_BUT_BKOFF);
	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

	if(para)
	{
		tconBrightness = para->tconBrightness;
	}

	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_INIT, 0 | (tconBrightness << 8), 0);
#endif
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_eTvSystem_avin, 0);

	if(openChannelIdx == TCON_INPUTCHANNEL_AVIN)
	{
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_BLKSCRSET, 0x8D, 0);
	}
	else
	{
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_BLKSCRSET, 0x00, 0);
	}

	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCHANNEL, openChannelIdx, 0);
	{
		__u8	i, cnt;

		for(i = 0, cnt = 0; i < 50; i++)
		{
			__u32 t_HLCK = 0;
			esKRNL_TimeDly(1);
#if (SP_DEV_TCON_EX == DEV_TCON_EX_BIT1612M)
			{
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg1 << 8) | 0x74, (void *)(&t_HLCK));
				__wrn("1612(0x0174)=0x%04X\n", t_HLCK);

				if(t_HLCK & (1 << 4))
				{
					cnt++;
				}
				else
				{
					cnt = 0;
				}
			}
#elif (SP_DEV_TCON_EX == DEV_TCON_EX_BIT1628M)
			{
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_READ_R, (1 << 16) | (IIC_CHIP_TconReg1 << 8) | 0xBF, (void *)(&t_HLCK));
				__wrn("1628(0x01BF)=0x%04X\n", t_HLCK);

				if(t_HLCK & (1 << 2))
				{
					cnt++;
				}
				else
				{
					cnt = 0;
				}
			}
#endif

			if(cnt >= 5)
			{
				return 1;
			}
		}
	}
#ifdef TCON_SIGNAL_RGB
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_INIT, TCON_SIGNAL_RGB | (tconBrightness << 8), 0);
	dsk_display_lcd_on();
#endif

#else
#endif

	return 0;
}
#endif

void TCON_SetColorSystem(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_eTvSystem_avin, 0);
	}
}

void TCON_OpenScreen(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		{
			if(g_isAVInput)
			{
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_eTvSystem_avin, 0);
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_BLKSCRSET, 0x8D, 0);
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCHANNEL, TCON_INPUTCHANNEL_AVIN, 0);
			}
			else
			{
				E_TV_SYSTEM eTvSystem;
			#if !SP_ATV_SYS_IndepSetEnable
				if(a_psTunerInfo->m_eTvSystem_tv != m_eTvSystem_Auto)
				{
					eTvSystem = a_psTunerInfo->m_eTvSystem_tv; // 实时伴音制式
				}
				else
			#endif
				{
					eTvSystem = a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_idx].m_eTvSystem;
				} 

			#if SP_ATV_SYS_IndepSetEnable
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_idx].m_eTvSystem, 0);
			#else
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, eTvSystem, 0);
			#endif
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_BLKSCRSET, 0x00, 0);
				eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCHANNEL, TCON_INPUTCHANNEL_TUNER, 0);
			}
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_ONOFF, 1, (void *)1);
		}
		esKRNL_TimeDly(10);
		{
			ES_FILE 			   *p_disp = NULL;
			/* open display driver */
			p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

			if(p_disp == NULL)
			{
				return;
			}

			eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_ON, 0, 0);
			/* close display driver */
			eLIBs_fclose(p_disp);
		}
	}
}

void TCON_CloseScreen(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		{
			ES_FILE 			   *p_disp = NULL;
			/* open display driver */
			p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

			if(p_disp == NULL)
			{
				return;
			}

			eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_OFF, (0 << 16), 0);
			/* close display driver */
			eLIBs_fclose(p_disp);
		}
		{
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_ONOFF, 0, (void *)1);
		}
	}
}

void TCON_OccupyLcmi(__u8 cvbsSrc, PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		__u8 openChannel;
		__s32 en3D;
		__s32 ret;
		__drv_TVD_system colorSys;
		E_TV_SYSTEM *peTvSys = NULL;
		//dsk_display_lcd_off(OP_SCREENON_BUT_BKOFF);
		//GUI_LyrWinSetSta((H_LYR)(msg->dwAddData1), GUI_LYRWIN_STA_SLEEP);	// subMenuLayer
		GUI_LyrWinSetSta((H_LYR)(msg->dwAddData2), GUI_LYRWIN_STA_SLEEP);		// layer

		com_set_backcolor(CONST_BackColor_Black);
		gscene_bgd_set_state(BGD_STATUS_HIDE);
		
		g_isAVInput = cvbsSrc;

		switch(cvbsSrc)
		{
			case CVBS_SRC__TV:
			{
			#if !SP_ATV_SYS_IndepSetEnable
				if(a_psTunerInfo->m_eTvSystem_tv != m_eTvSystem_Auto)
				{
					peTvSys = &a_psTunerInfo->m_eTvSystem_tv; // 实时伴音制式
				}
				else
			#endif
				{
					peTvSys = &a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_idx].m_eTvSystem;
				}
				openChannel = TCON_INPUTCHANNEL_TUNER;
				en3D = 0;
				break;
			}
			case CVBS_SRC__AVIN:
			{
				peTvSys = &a_psTunerInfo->m_eTvSystem_avin;
				openChannel = TCON_INPUTCHANNEL_AVIN;
				en3D = 1;
				break;
			}
			case CVBS_SRC__DVD:
			{
				colorSys = TVD_SOURCE_PAL;
				openChannel = TCON_INPUTCHANNEL_DVD;
				en3D = 1;
				break;
			}
		}
		
		if(CVBS_SRC__DVD != cvbsSrc)
		{
			switch(*peTvSys)
			{
				case m_eTvSystem_NTSC_M:
					colorSys = TVD_SOURCE_NTSC;
					break;
				case m_eTvSystem_PAL_M:
					colorSys = TVD_SOURCE_PAL_M;
					break;
				case m_eTvSystem_PAL_N:
					colorSys = TVD_SOURCE_PAL_N;
					break;
				case m_eTvSystem_SECAM:
				case m_eTvSystem_SECAM_BG:
					colorSys = TVD_SOURCE_SECAM;
					break;					
				default :
					colorSys = TVD_SOURCE_PAL;
					break;
			}
		}
		ret = com_video_in_open(1, openChannel, colorSys, en3D);
		__msg("com_video_in_open():ret=%d\n", ret);

		switch(cvbsSrc)
		{
			case CVBS_SRC__DVD:
				com_video_set_blue_flag(1);
				break;
			case CVBS_SRC__TV:
				com_video_set_blue_flag(2);
				break;
			case CVBS_SRC__AVIN:
			default:
				com_video_set_blue_flag(2);
				break;
		}
		
		g_disable_close_scn();

		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_AUX_ONOFF, 1, 0);

	#if 0
	#if !SP_ATV_SYS_IndepSetEnable
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_eTvSystem_tv, 0);
	#endif

		if(cvbsSrc == TCON_INPUTCHANNEL_AVIN)
		{
		#if SP_ATV_SYS_IndepSetEnable
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_eTvSystem_avin, 0);
		#endif
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_BLKSCRSET, 0x8D, 0);
		}
		else
		{
		#if SP_ATV_SYS_IndepSetEnable
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCOLOR, a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_idx].m_eTvSystem, 0);
		#endif
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_BLKSCRSET, 0x00, 0);
		}

		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_SELCHANNEL, cvbsSrc, 0);
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_ONOFF, 1, 0);
		esKRNL_TimeDly(10);
		SEND_MSG(DSK_MSG_TCON_OCCUPY_LCMI, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
		g_isTconOccupyLcmi_flag = 1;
	#endif
	}
}

void TCON_ReleaseLcmi(PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		if(dsk_audio_echo_is_open())
		{
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_AUX_ONOFF, 1, 0);
		}
		else
		{
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_AUX_ONOFF, 0, 0);
		}

		com_video_in_close();

		gscene_bgd_set_state(BGD_STATUS_SHOW);
		com_set_backcolor(CONST_BackColor);
		g_enable_close_scn();

	#if 0
		{
			ES_FILE 			   *p_disp = NULL;
			/* open display driver */
			p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

			if(p_disp == NULL)
			{
				return;
			}

			eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_OFF, 0, 0);
			eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 0, 0);
			{
				// 重置屏参
				dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_BRIGHT), DISP_CMD_SET_BRIGHT);
				dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_CONTRAST), DISP_CMD_SET_CONTRAST);
				dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_HUE), DISP_CMD_SET_HUE);
				dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_SATURATION), DISP_CMD_SET_SATURATION);
			}
			/* close display driver */
			eLIBs_fclose(p_disp);
		}
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_ONOFF, 0, 0);
		esKRNL_TimeDly(10);
		SEND_MSG(DSK_MSG_TCON_RELEASE_LCMI, msg->h_deswin, GUI_WinGetHandFromName("init"), 0, 0, 0);
		g_isTconOccupyLcmi_flag = 0;
	#endif
	}

	g_isAVInput = 0;
}

extern __u8 g_tvSoundChannelChange;
void TCON_OSDFrameOff(PS_TUNER_INFO a_psTunerInfo, __u8 osdFrame_mask)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		__u32 aux;
		aux = ((__u32)osdFrame_mask) << 8 | 0x00;
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_OSDONOFF, aux, 0);
	}

	if(osdFrame_mask & (1 << 2))
	{
		g_tvSoundChannelChange = 0;
	}
}


const __u32 China_Catv_Frequency[] =  		// 单位: Hz
{
	49750000L,      57750000L,      65750000L,      77250000L,
	85250000L,      112250000L,     120250000L,     128250000L,
	136250000L,     144250000L,     152250000L,     160250000L,
	168250000L,     176250000L,     184250000L,     192250000L,
	200250000L,     208250000L,     216250000L,     224250000L,
	232250000L,     240250000L,     248250000L,     256250000L,
	264250000L,     272250000L,     280250000L,     288250000L,
	296250000L,     304250000L,     312250000L,     320250000L,
	328250000L,     336250000L,     344250000L,     352250000L,
	360250000L,     368250000L,     376250000L,     384250000L,
	392250000L,     400250000L,     408250000L,     416250000L,
	424250000L,     432250000L,     440250000L,     448250000L,
	456250000L,     464250000L,     471250000L,     479250000L,
	487250000L,     495250000L,     503250000L,     511250000L,
	519250000L,     527250000L,     535250000L,     543250000L,
	551250000L,     559250000L,     567250000L,     575250000L,
	583250000L,     591250000L,     599250000L,     607250000L,
	615250000L,     623250000L,     631250000L,     639250000L,
	647250000L,     655250000L,     663250000L,     671250000L,
	679250000L,     687250000L,     695250000L,     703250000L,
	711250000L,     719250000L,     727250000L,     735250000L,
	743250000L,     751250000L,     759250000L,     767250000L,
	775250000L,     783250000L,     791250000L,     799250000L,
	807250000L,     815250000L,     823250000L,     831250000L,
	839250000L,     847250000L,     855250000L,     863250000L
};

const __u32 size_China_Catv_Frequency = sizeof(China_Catv_Frequency) / sizeof(China_Catv_Frequency[0]);

E_TV_SYSTEM TUNER_TvSys_Mapping(E_STD_T tvStd)
{
	switch(tvStd)
	{
		case CDTMTV_VSTD_I :
			return m_eTvSystem_PAL_I;

		case CDTMTV_VSTD_BG :
			return m_eTvSystem_PAL_BG;

		case CDTMTV_VSTD_N :
			return m_eTvSystem_NTSC_M;

		case CDTMTV_VSTD_DK_X :
			return m_eTvSystem_SECAM;

		case CDTMTV_VSTD_M :
			return m_eTvSystem_PAL_M;

		case CDTMTV_VSTD_N_X :
			return m_eTvSystem_PAL_N;

		case CDTMTV_VSTD_DK :
		default :
			return m_eTvSystem_PAL_DK;
	}
}

void TUNER_PlayTV(PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	__wrn("TUNER_PlayTV:begin\n");

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else
	{
		a_psTunerInfo->m_tunerMode = MODE_Tuner_TV;
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVPOWERON, 0, 0);
		__wrn("DRV_ATV_CMD_TVPOWERON:end\n");
		{
			S_TUNER_ATV 	sTunerAtv;

		#if 1
			if(a_psTunerInfo->m_psTunerAtvList_idx <= IDX_sTunerAtvList_invalid)
			{ // 添加一个默认频道
				a_psTunerInfo->m_psTunerAtvList_idx = 0;
				a_psTunerInfo->m_psTunerAtvList_cnt = 1;
				a_psTunerInfo->m_psTunerAtvList[0].m_freq = RANGE_TVFreq_Min;
				a_psTunerInfo->m_psTunerAtvList[0].m_eTvSystem = a_psTunerInfo->m_eTvSystem_tv;	// 使用系统制式
			}
		#endif

			if(a_psTunerInfo->tmpFreq)
			{
				sTunerAtv.m_freq = a_psTunerInfo->tmpFreq;
			}
			else
			{
				sTunerAtv.m_freq = a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_idx].m_freq;
			}

		#if !SP_ATV_SYS_IndepSetEnable
			if(a_psTunerInfo->m_eTvSystem_tv != m_eTvSystem_Auto)
			{
				sTunerAtv.m_eTvSystem = a_psTunerInfo->m_eTvSystem_tv;	// 实时伴音制式
			}
			else
		#endif
			{
				sTunerAtv.m_eTvSystem = a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_idx].m_eTvSystem;
			}	

			__msg_jjl("TUNER_PlayTV:ch total=%u, cur ch=%d, freq=%u, audiosys=%u\n", 
				a_psTunerInfo->m_psTunerAtvList_cnt, a_psTunerInfo->m_psTunerAtvList_idx,
				sTunerAtv.m_freq, sTunerAtv.m_eTvSystem);
				
			eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVSENDTUNER,
			              sTunerAtv.m_freq, (void *)(sTunerAtv.m_eTvSystem));
			esKRNL_TimeDly(1);
			TCON_OccupyLcmi(CVBS_SRC__TV, a_psTunerInfo, msg);

			#if(0)
			{
				__u8 writeDispRam_tcon[16], i;
				memset(writeDispRam_tcon, 0x00, sizeof(writeDispRam_tcon));
				{
					// 显示频率
					__u8 tmp[] = "863.25", *pCharIdx;
					pCharIdx = IntegerToAscii(sTunerAtv.m_freq, 6, tmp, sizeof(tmp));
					eLIBs_strcpy(writeDispRam_tcon, pCharIdx);
					eLIBs_strcat(writeDispRam_tcon, "MHz");

					for(i = 9; i--;)
					{
						if(writeDispRam_tcon[i])
						{
							break;
						}

						writeDispRam_tcon[i] = 0x20;
					}
				}
				{
					// 显示台号
					__u8 tmp[] = "512", *pCharIdx;
					__s32 tvIdx;
					tvIdx = a_psTunerInfo->m_psTunerAtvList_idx + 1;

					if(tvIdx <= 0)
					{
						tvIdx = 1;
					}

					pCharIdx = IntegerToAscii(tvIdx, 0, tmp, sizeof(tmp));
					eLIBs_strcat(writeDispRam_tcon, "CH ");
					eLIBs_strcat(writeDispRam_tcon, pCharIdx);

					for(i = 0x0F; i--;)
					{
						if(writeDispRam_tcon[i])
						{
							break;
						}

						writeDispRam_tcon[i] = 0x20;
					}
				}
				__wrn("writeDispRam_tcon=\"%s\"$\n", writeDispRam_tcon);
				{
					__u32 aux;
					aux = ((__u32)(sizeof(writeDispRam_tcon) - 1) << 16) | 0 | m_eTconPaletteColorMap_white;
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_OSDWRITE, aux, writeDispRam_tcon);
					aux = ((__u32)0x03) << 8 | 0x03;
					eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TCON_OSDONOFF, aux, 0);
				}
			}
			#endif

			__wrn("DRV_ATV_CMD_TVSENDTUNER:end\n");
		}
	}

	__wrn("TUNER_PlayTV:end\n");
}

void TUNER_ExitTV(PS_TUNER_INFO a_psTunerInfo, __gui_msg_t *msg)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	__wrn("TUNER_ExitTV:begin, mode=%d\n", a_psTunerInfo->m_tunerMode);

	if(pEsFile_Atv == NULL)
	{
		__wrn(" Error open atv driver failed \n ");
	}
	else if(a_psTunerInfo->m_tunerMode == MODE_Tuner_TV)
	{
		TCON_ReleaseLcmi(a_psTunerInfo, msg);
		a_psTunerInfo->m_tunerMode = MODE_Tuner_Idle;
		eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVPOWEROFF, 0, 0);
		//TCON_OSDFrameOff(a_psTunerInfo, 0x07);
	}

	__wrn("TUNER_ExitTV:end\n");
}

__s32 TUNER_ScanTVFreq_Start(PS_TUNER_INFO a_psTunerInfo, __bool a_manual_flag)
{
	if(a_psTunerInfo == NULL)
	{
		return -1;
	}

	if(a_psTunerInfo->m_searchStatus == STATE_TunerSearch_Idle)
	{
		a_psTunerInfo->m_searchFlags_mask &= ~(MASK_SearchFlags_Backward
		                                       | MASK_SearchFlags_Over
		                                       | MASK_SearchFlags_ShiftDir
		                                       | MASK_SearchFlags_Manual);

		if(a_manual_flag)
		{
			__wrn("a_psTunerInfo->m_searchFlags_mask |= MASK_SearchFlags_Manual\n");
			a_psTunerInfo->m_searchFlags_mask |= MASK_SearchFlags_Manual;
		}

		a_psTunerInfo->m_tunerMode = MODE_Tuner_Searching;
		a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Init;
		return 0;
	}

	return 1;
}

void TUNER_ScanTVFreq_Stop(PS_TUNER_INFO a_psTunerInfo)
{
	if(a_psTunerInfo == NULL)
	{
		return;
	}

	switch(a_psTunerInfo->m_searchStatus)
	{
		case STATE_TunerSearch_Idle:
			break;

		case STATE_TunerSearch_Suspend :
			a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Idle;
			break;

		default :
			a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Over;

			while(a_psTunerInfo->m_tunerMode != MODE_Tuner_Idle)
			{
				// 等待搜索结束
				__wrn("TUNER_ScanTVFreq_Stop:wait...\n");
				esKRNL_TimeDly(1);
			}

			break;
	}
}

__s32 TUNER_ScanTVFreq_Resume(PS_TUNER_INFO a_psTunerInfo, __bool a_setBackward)
{
	if(a_psTunerInfo == NULL)
	{
		return -1;
	}

	if(a_setBackward)
	{
		TUNER_ScanTVFreq_SetBackward(a_psTunerInfo);
	}
	else
	{
		TUNER_ScanTVFreq_SetForward(a_psTunerInfo);
	}

	if(a_psTunerInfo->m_searchStatus == STATE_TunerSearch_Suspend)
	{
		a_psTunerInfo->m_tunerMode = MODE_Tuner_Searching;
		a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Resume;
		return 0;
	}

	return 1;
}

void TUNER_ScanTVFreq_SetForward(PS_TUNER_INFO a_psTunerInfo)
{
	//正在搜索的时候也可调用
	if(a_psTunerInfo == NULL)
	{
		return;
	}

	a_psTunerInfo->m_searchFlags_mask &= ~MASK_SearchFlags_ShiftDir;
	a_psTunerInfo->m_searchFlags_mask |= CASE_SearchFlags_WaitForward;
}

void TUNER_ScanTVFreq_SetBackward(PS_TUNER_INFO a_psTunerInfo)
{
	if(a_psTunerInfo == NULL)
	{
		return;
	}

	a_psTunerInfo->m_searchFlags_mask &= ~MASK_SearchFlags_ShiftDir;
	a_psTunerInfo->m_searchFlags_mask |= CASE_SearchFlags_WaitBackward;
}

void TUNER_ScanTVFreq_GetSearchDir(PS_TUNER_INFO a_psTunerInfo)
{
	switch(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_ShiftDir)
	{
		case CASE_SearchFlags_WaitForward :
			a_psTunerInfo->m_searchFlags_mask &= ~MASK_SearchFlags_Backward;
			__wrn("TUNER_ScanTVFreq_GetSearchDir:Forward\n");
			break;

		case CASE_SearchFlags_WaitBackward:
			__wrn("TUNER_ScanTVFreq_GetSearchDir:Backward\n");
			a_psTunerInfo->m_searchFlags_mask |= MASK_SearchFlags_Backward;
			break;

		default :
			break;
	}

	a_psTunerInfo->m_searchFlags_mask &= ~MASK_SearchFlags_ShiftDir;
}

void TUNER_ScanTVFreq_FFStep(PS_TUNER_INFO  a_psTunerInfo, __u32 ffStepFreq)
{
	// 快速步进扫描
	__u32	stepCnt = ffStepFreq / STEP_TVFreq_RDA5218;

	while(stepCnt--)
	{
		__wrn("stepCnt=%u\n", stepCnt);

		if(a_psTunerInfo->m_curSearchFreq < RANGE_TVFreq_Max)
		{
			a_psTunerInfo->m_curSearchFreq += STEP_TVFreq_RDA5218;

			if((a_psTunerInfo->m_curSearchFreq == RANGE_TVFreq_Max) || stepCnt)
			{
				a_psTunerInfo->m_searchFreqCnt++;
				__wrn("TUNER_ScanTVFreq_Thread:cnt=%u, freq=%u\n",
				      a_psTunerInfo->m_searchFreqCnt, a_psTunerInfo->m_curSearchFreq);
			}
		}

		if(a_psTunerInfo->m_curSearchFreq >= RANGE_TVFreq_Max)
		{
			a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Over;
			break;
		}
	}
}

void TUNER_ScanTVFreq_FBStep(PS_TUNER_INFO  a_psTunerInfo, __u32 fbStepFreq)
{
	// 快速步进扫描
	__u32	stepCnt = fbStepFreq / STEP_TVFreq_RDA5218;

	while(stepCnt--)
	{
		__wrn("stepCnt=%u\n", stepCnt);

		if(a_psTunerInfo->m_curSearchFreq > RANGE_TVFreq_Min)
		{
			a_psTunerInfo->m_curSearchFreq -= STEP_TVFreq_RDA5218;

			if((a_psTunerInfo->m_curSearchFreq == RANGE_TVFreq_Min) || stepCnt)
			{
				a_psTunerInfo->m_searchFreqCnt--;
				__wrn("TUNER_ScanTVFreq_Thread:cnt=%u, freq=%u\n",
				      a_psTunerInfo->m_searchFreqCnt, a_psTunerInfo->m_curSearchFreq);
			}
		}

		if(a_psTunerInfo->m_curSearchFreq <= RANGE_TVFreq_Min)
		{
			a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Over;
			break;
		}
	}
}

void TUNER_ScanTVFreq_SearchStatus_Init(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	a_psTunerInfo->m_savePara_mask |= MASK_SaveTunerInfo_TVList;

	if((a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Manual) == 0)
	{
		a_psTunerInfo->m_psTunerAtvList_cnt = 0;
		a_psTunerInfo->m_psTunerAtvList_idx = IDX_sTunerAtvList_invalid;
	}
	else
	{
		// 不删除已搜到的频道；新的频道追加到频道表后面
	}

	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVPOWERON, 0, 0);
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVSCANINIT, 0, 0);

	if((a_psTunerInfo->m_pFreqTab == NULL) || (a_psTunerInfo->m_pFreqTab_size <= 0))
	{
		a_psTunerInfo->m_searchMode = MODE_TunerSearch_All;
		a_psTunerInfo->m_curSearchFreq = RANGE_TVFreq_Min;
		a_psTunerInfo->m_pFreqTab_size = (RANGE_TVFreq_Max - RANGE_TVFreq_Min) / STEP_TVFreq_RDA5218 + 1;
	}
	else
	{
		a_psTunerInfo->m_searchMode = MODE_TunerSearch_ByTVSys;
		a_psTunerInfo->m_curSearchFreq = a_psTunerInfo->m_pFreqTab[0];
	}

	a_psTunerInfo->m_searchFreqCnt = 0; // 注: 默认(或起始)搜索方向: 向前
	a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Ing;
	a_psTunerInfo->m_searchValueValid_flag = 1;
	__wrn("STATE_TunerSearch_Init\n");
	__wrn("TUNER_ScanTVFreq_Thread:AtvList_size=%u\n", a_psTunerInfo->m_psTunerAtvList_size);
	__wrn("TUNER_ScanTVFreq_Thread:FreqTab_size=%u\n", a_psTunerInfo->m_pFreqTab_size);
}

void TUNER_ScanTVFreq_SearchStatus_Resume(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	a_psTunerInfo->m_savePara_mask |= MASK_SaveTunerInfo_TVList;
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVPOWERON, 0, 0);
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVSCANINIT, 0, 0);
	TUNER_ScanTVFreq_GetSearchDir(a_psTunerInfo);

	if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Manual)
	{
		if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Backward)
		{
			a_psTunerInfo->m_curSearchFreq -= STEP_TVFreq_RDA5218;
		}
		else
		{
			a_psTunerInfo->m_curSearchFreq += STEP_TVFreq_RDA5218;
		}
	}

	a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Ing;
	a_psTunerInfo->m_searchValueValid_flag = 1;
	__wrn("STATE_TunerSearch_Resume\n");
}

void TUNER_ScanTVFreq_SearchStatus_Suspend(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVPOWEROFF, 0, 0);
	a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Suspend;
	a_psTunerInfo->m_tunerMode = MODE_Tuner_Idle;
	__wrn("STATE_TunerSearch_Suspend\n");
	__wrn("TUNER_ScanTVFreq_Thread:search_cnt=%u, m_psTunerAtvList_cnt=%u\n",
	      a_psTunerInfo->m_searchFreqCnt, a_psTunerInfo->m_psTunerAtvList_cnt);
}

void TUNER_ScanTVFreq_SearchStatus_Over(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVPOWEROFF, 0, 0);

	if(a_psTunerInfo->m_psTunerAtvList_cnt > 0)
	{
		a_psTunerInfo->m_psTunerAtvList_idx = 0;
	}

#if 1
	if(a_psTunerInfo->m_psTunerAtvList_idx <= IDX_sTunerAtvList_invalid)
	{ // 添加一个默认频道
		a_psTunerInfo->m_psTunerAtvList_idx = 0;
		a_psTunerInfo->m_psTunerAtvList_cnt = 1;
		a_psTunerInfo->m_psTunerAtvList[0].m_freq = RANGE_TVFreq_Min;
		a_psTunerInfo->m_psTunerAtvList[0].m_eTvSystem = a_psTunerInfo->m_eTvSystem_tv; // 使用系统制式
	}
#endif

	a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Idle;
	a_psTunerInfo->m_tunerMode = MODE_Tuner_Idle;
	a_psTunerInfo->m_searchFlags_mask |= MASK_SearchFlags_Over;
	__wrn("STATE_TunerSearch_Over\n");
	__wrn("TUNER_ScanTVFreq_Thread:search_cnt=%u, m_psTunerAtvList_cnt=%u\n",
	      a_psTunerInfo->m_searchFreqCnt, a_psTunerInfo->m_psTunerAtvList_cnt);
}

void TUNER_ScanTVFreq_SearchStatus_Ing(PS_TUNER_INFO a_psTunerInfo)
{
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;

	do
	{
		__s32	result;

		if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Backward)
		{
			a_psTunerInfo->m_searchFreqCnt--;
		}
		else
		{
			a_psTunerInfo->m_searchFreqCnt++;
		}

		__wrn("TUNER_ScanTVFreq_Thread:cnt=%u, freq=%u\n",
		      a_psTunerInfo->m_searchFreqCnt, a_psTunerInfo->m_curSearchFreq);
		result = eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVSCAN, a_psTunerInfo->m_curSearchFreq, 0);

		if(result == CDTMTV_RT_SCAN_DONE)
		{
			if(a_psTunerInfo->m_psTunerAtvList_cnt < a_psTunerInfo->m_psTunerAtvList_size)
			{
				if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Manual)
				{
					a_psTunerInfo->m_psTunerAtvList_idx = a_psTunerInfo->m_psTunerAtvList_cnt;
				}
				else
				{
					a_psTunerInfo->m_psTunerAtvList_idx = 0;
				}

				a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_cnt].m_freq
				  = a_psTunerInfo->m_curSearchFreq;

				if(a_psTunerInfo->m_eTvSystem_tv == m_eTvSystem_Auto)
				{ // 搜台时，自动获取伴音制式
					E_STD_T tvStd = eLIBs_fioctrl(pEsFile_Atv, DRV_ATV_CMD_TVGETSYSMODE, 0, 0);
					a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_cnt].m_eTvSystem
					  = TUNER_TvSys_Mapping(tvStd);
				}
				else
				{ // 搜台时，恢复为系统设置的制式
					a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_cnt].m_eTvSystem
					  = a_psTunerInfo->m_eTvSystem_tv;
				}

				__wrn("m_eTvSystem=%u\n", a_psTunerInfo->m_psTunerAtvList[a_psTunerInfo->m_psTunerAtvList_cnt].m_eTvSystem);
				a_psTunerInfo->m_psTunerAtvList_cnt++;
				__wrn("m_psTunerAtvList_cnt=%u\n", a_psTunerInfo->m_psTunerAtvList_cnt);
			}
		}

		TUNER_ScanTVFreq_GetSearchDir(a_psTunerInfo);

		switch(a_psTunerInfo->m_searchMode)
		{
			case MODE_TunerSearch_All:
				if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Backward)
				{
					if(a_psTunerInfo->m_curSearchFreq > RANGE_TVFreq_Min)
					{
						switch(result)
						{
							case CDTMTV_RT_SCAN_DONE :
								if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Manual)
								{
									TUNER_ScanTVFreq_SearchStatus_Suspend(a_psTunerInfo);
									break;
								}

							case CDTMTV_RT_SCAN_EMPTY_CH :
								TUNER_ScanTVFreq_FBStep(a_psTunerInfo, STEP_TVFreq_RDA5218);
								break;	// 如果使用快退，搜索会出现漏台

							default :
								a_psTunerInfo->m_curSearchFreq -= STEP_TVFreq_RDA5218;
								break;
						}
					}
					else
					{
						a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Over;
					}
				}
				else
				{
					if(a_psTunerInfo->m_curSearchFreq < RANGE_TVFreq_Max)
					{
						switch(result)
						{
							case CDTMTV_RT_SCAN_DONE :
								if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Manual)
								{
									TUNER_ScanTVFreq_SearchStatus_Suspend(a_psTunerInfo);
									break;
								}

							case CDTMTV_RT_SCAN_EMPTY_CH :
								TUNER_ScanTVFreq_FFStep(a_psTunerInfo, FFSTEP_TVFreq_RDA5218);
								break;

							default :
								a_psTunerInfo->m_curSearchFreq += STEP_TVFreq_RDA5218;
								break;
						}
					}
					else
					{
						a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Over;
					}
				}

				break;

			case MODE_TunerSearch_ByTVSys :
				if(a_psTunerInfo->m_searchFlags_mask & MASK_SearchFlags_Backward)
				{
				}
				else
				{
					if(a_psTunerInfo->m_searchFreqCnt < a_psTunerInfo->m_pFreqTab_size)
					{
						a_psTunerInfo->m_curSearchFreq = a_psTunerInfo->m_pFreqTab[a_psTunerInfo->m_searchFreqCnt];
					}
					else
					{
						a_psTunerInfo->m_searchStatus = STATE_TunerSearch_Over;
					}
				}

				break;

			default :
				break;
		}
	}
	while(0);
}

void TUNER_ScanTVFreq_Thread(void *p_arg)
{
	PS_TUNER_INFO 	a_psTunerInfo = (PS_TUNER_INFO)p_arg;
	ES_FILE 			*pEsFile_Atv = a_psTunerInfo->m_pEsFile_drv;
	__wrn("TUNER_ScanTVFreq_Thread:begin\n");

	while(1)
	{
		if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
		{
			esKRNL_TDel(EXEC_prioself);
			break;
		}

		if((pEsFile_Atv != NULL) && a_psTunerInfo->m_psTunerAtvList_size)
		{
			switch(a_psTunerInfo->m_searchStatus)
			{
				case STATE_TunerSearch_Init :
					TUNER_ScanTVFreq_SearchStatus_Init(a_psTunerInfo);
					break;

				case STATE_TunerSearch_Resume :
					TUNER_ScanTVFreq_SearchStatus_Resume(a_psTunerInfo);
					break;

				case STATE_TunerSearch_Ing :
					TUNER_ScanTVFreq_SearchStatus_Ing(a_psTunerInfo);
					break;

				case STATE_TunerSearch_Over :
					TUNER_ScanTVFreq_SearchStatus_Over(a_psTunerInfo);
					break;

				default :
					break;
			}
		}

		esKRNL_TimeDly(1);
	}

	__wrn("TUNER_ScanTVFreq_Thread:end\n");
}


#endif
