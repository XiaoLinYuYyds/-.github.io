
///C500�ṩfm gain��mic gain��mic pregain��linein gain�Լ�micֱͨ�ӿڡ������½ӿ��б仯������dsk_audio_echo�ӿڸ�C100һ����

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//#include "Gain.h"
#include "..\\functions_i.h"

#if SP_Software_Echo
__s8		g_echoLevel = -1;
#endif

typedef struct  {
	
  __u32 micPreAmpGainCtl    : 3;//B2-B0
    
  __u32 micPreAmpEnBit      : 1;//B3
	__u32	FMtoLRMirerGain		: 3;//B6-B4 
	__u32 paspreedselect 		: 1;//b7
	__u32 adcInputSrcSel      : 6;//b13-b8. adc input source
	__u32 CosSlopTimeForAntipop:2;//b15-b14
	__u32 adcInputGainCtl     : 3;//b18-16, adc input gain
  __u32 reserved4           : 2;//b20-19
  __u32	LineintoLRMirerGain	: 3;//b23-21
  __u32 MictoLRMirerGain	: 3; //b26-24
  __u32 reserved5           : 4;//b30-b27
  __u32 adcEnBit            : 1;//b31
}AdcAnaCtl;

static ES_FILE *g_fp_rec = NULL;

//type:AUDIO_REC_USR_MIC/AUDIO_REC_USR_FM....
//auto_clean_buf:0�������Զ����buffer(¼��Ӧ��)��Ϊ1�����Զ����buffer(��¼����Ӧ��)
//��ϸ���÷����μ�SDK��app_root_scene.c��record_view_doc.c
__s32 dsk_audio_echo_open(__audio_rec_user_t type, __s32 auto_clean_buf)
{
	__s32 aux;
	__audio_dev_para_t adev_para;

	if(g_fp_rec)
	{
		__msg("rec hdl alreadly open...\n");
		return EPDK_FAIL;
	}

	g_fp_rec = eLIBs_fopen("b:\\AUDIO\\REC", "r+");

	if(NULL == g_fp_rec)
	{
		__wrn("open rec hdl fail...\n");
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_REG_USERMODE, type, 0);
	adev_para.chn = 2;
	adev_para.bps = 16;
	adev_para.fs = 44100;
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_PARA, 0, (void *)&adev_para);
	//eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_REG_USERMODE, type, 0);
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_SAMPCNT, 0, 0);//��adc buffer������;
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO_OUTPUT, 1, 0);
	aux = 1;
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO, aux, 0);
	aux = auto_clean_buf;
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_BUF_MODE, aux, 0);
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_START, 0, 0);
	//dsk_audio_echo_set_mic_pre_gain(1);
	//dsk_audio_echo_set_mic_gain(0);
	return EPDK_OK;
}

//��ϸ���÷����μ�SDK��app_root_scene.c��record_view_doc.c
__s32 dsk_audio_echo_close(void)
{
	__s32 aux;

	if(NULL == g_fp_rec)
	{
		__msg("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	aux = 0;
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO, aux, 0);
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO_OUTPUT, 0, 0);
	aux = 0;
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_BUF_MODE, aux, 0);
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_STOP, 0, 0);

	if(g_fp_rec)
	{
		eLIBs_fclose(g_fp_rec);
		g_fp_rec = NULL;
	}

#if SP_Software_Echo
	g_echoLevel = -1;
#endif

	return EPDK_OK;
}

 //when value ==1,gain== 24db  3db/step
 //when value =0 gain=0;
 //value  rang is 0 -7
__s32 dsk_audio_echo_set_mic_pre_gain(__s32 value)
{
	volatile AdcAnaCtl* pAdcAnaCtl = (volatile AdcAnaCtl*)(0x24 + 0Xf1C23C00);
	pAdcAnaCtl->micPreAmpEnBit=1;
	if(value>7||value<0)
		return -1;
	pAdcAnaCtl->micPreAmpGainCtl=value;

	return 0;
}

//�ýӿ���ʱ���ã�CPU�ܲ���������mic������-32��32�����Ե���mic����
__s32 dsk_audio_echo_gain_incr(void)
{
	__s32 aux;
	__wrn("dsk_audio_echo_gain_incr...\n");

	if(NULL == g_fp_rec)
	{
		__wrn("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	aux = eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_GET_ECHO_REC_GAIN, 0, 0);

	if(aux < 32)
	{
		aux++;
	}

	__wrn("dsk_audio_echo_gain_incr, aux=%d\n", aux);
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_REC_GAIN, aux, 0);
	return EPDK_OK;
}

//�ýӿ���ʱ���ã�CPU�ܲ�������Сmic���� -32��32�����Ե���mic����
__s32 dsk_audio_echo_gain_decr(void)
{
	__s32 aux;

	if(NULL == g_fp_rec)
	{
		__wrn("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	aux = eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_GET_ECHO_REC_GAIN, 0, 0);

	if(aux > -32)
	{
		aux--;
	}

	__wrn("dsk_audio_echo_gain_decr, aux=%d\n", aux);
	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_REC_GAIN, aux, 0);
	return EPDK_OK;
}

 //value  rang is 0-7
 //	  -4.5 db - 6db 1.6 db /step
__s32 dsk_audio_echo_set_mic_gain(__s32 value)
{
	volatile AdcAnaCtl* pAdcAnaCtl = (volatile AdcAnaCtl*)(0x24 + 0Xf1C23C00);
	if(value>7||value<0)
			return -1;
	pAdcAnaCtl->MictoLRMirerGain=value;
	return 0;
}

__s32 dsk_audio_echo_pt_regs(__u32 start, __u32 end)
{
	__u32 i;
	eLIBs_printf("*****dsk_audio_echo_pt_regs begin******\n");

	for(i = start ; i <= end ; i += 4)
	{
		eLIBs_printf("0x%8x:0x%8x\n", i, *(volatile unsigned long *)i);
	}

	eLIBs_printf("*****dsk_audio_echo_pt_regs end******\n");
	return EPDK_OK;
}

/*aux: 0< aux <= 10 ���û�����ʱ������Ĭ��Ϊ3*/
__s32 dsk_audio_echo_set_delay_num(__s32 aux)
{
	//__wrn("dsk_audio_echo_set_delay_num...\n");
	if(NULL == g_fp_rec)
	{
		__wrn("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_DELAY_NUM, aux, 0);
	return EPDK_OK;
}

/*aux:10<= aux <= 3000,Ĭ��Ϊ100���Ժ���Ϊ��λ*/
__s32 dsk_audio_echo_set_delay_time(__s32 aux)
{
	//__wrn("dsk_audio_echo_set_delay_num...\n");
	if(NULL == g_fp_rec)
	{
		__wrn("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_DELAY_TIME, aux, 0);
	return EPDK_OK;
}

__s32 dsk_audio_echo_is_open(void)
{
	if(g_fp_rec)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*�Ƿ�����MICХ�У�aux: 0:disable ec, 1: enable ec*/
__s32 dsk_audio_echo_enable_ec(__s32 aux)
{
	__msg("dsk_audio_echo_enable_ec, aux=%d...\n", aux);

	if(NULL == g_fp_rec)
	{
		__wrn("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO_CANCEL, aux, 0);
	return EPDK_OK;
}

/*�ýӿ���ʱ�����ã�CPU�ܲ��������ñ��ģʽ:
mode: 0xff:�û�ģʽ��1:��Ů�� 2:������ 3:��ͯ��
level:���mode=0xff,��ôlevelΪ����ȼ�-8<=level<=8, ���mode!=0xff����ôlevel��Ч*/
__s32 dsk_audio_echo_set_pitch_mode(__s32 mode, __s32 level)
{
	__wrn("dsk_audio_echo_set_pitch_mode, mode=%d, level=%d...\n", mode, level);

	if(NULL == g_fp_rec)
	{
		__wrn("rec hdl not open...\n");
		return EPDK_FAIL;
	}

	eLIBs_fioctrl(g_fp_rec, AUDIO_DEV_CMD_ECHO_SET_PITCH_MODE, mode, (void *)level);
	return EPDK_OK;
}

#if SP_Software_Echo
__s32 dsk_echo_level_get_setting(void)
{
	__s32 ret = EPDK_FAIL;
#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByKnob)
	{
		ES_FILE *pKeyFile;
		pKeyFile = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "rb");

		if(!pKeyFile)
		{
			__wrn("key connot be openned!\n");
		}
		else
		{
			ret = eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_GET_ECHO_LEVEL, 0, 0);
			eLIBs_fclose(pKeyFile);
		}
	}
#elif (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			ret = para->m_echoLevel;
		}
	}
#endif
	return ret;
}

typedef struct
{
	__s32	delayTime, delayNum, micPreGain, micGain;
} S_ECHO_PARA;

const S_ECHO_PARA gc_sEchoPara[8] =
{
	{60,		1, 	1 ,	1},
	{140, 	2, 	1 ,	1},
	{160, 	3, 	1 ,	2},
	{180, 	4, 	1 ,	2},
	{190, 	5, 	2,	2},
	{200, 	6, 	2,	3},
	{210, 	7, 	3,	3},
	{220, 	8, 	3,	4},
};

void dsk_echo_level_set(__s32 level)	// level��ȡֵ��Χ[0, 7]
{
	//__msg_jjl("dsk_echo_level_set(%d)\n", level);
	if(g_echoLevel == level)	return;	
	g_echoLevel = level;
	
	dsk_audio_echo_set_delay_time(gc_sEchoPara[level].delayTime);
	dsk_audio_echo_set_delay_num(gc_sEchoPara[level].delayNum);
	//dsk_audio_echo_set_mic_pre_gain(gc_sEchoPara[level].micPreGain);
	//dsk_audio_echo_set_mic_gain(gc_sEchoPara[level].micGain);
	dsk_audio_echo_set_mic_pre_gain(0); // ����������������ʵ���������ԭ����.
	dsk_audio_echo_set_mic_gain(7);
	dsk_audio_echo_enable_ec(1);
	__wrn("echoLevel=%d, delayTime=%d, delayNum=%d",
	      level, gc_sEchoPara[level].delayTime, gc_sEchoPara[level].delayNum);
}

void dsk_echo_level_init(void)
{
	if(g_echoLevel != -1)
	{
		return;
	}

	//__msg_jjl("dsk_echo_level_init()\n");

	dsk_audio_echo_open(AUDIO_REC_USR_MIC, 1);

#if (SP_Software_Echo == OP_SP_Software_Echo_AdjustByMenu)
	{
		__s32 level;
		level = dsk_echo_level_get_setting();

		if(level == EPDK_FAIL)
		{
			level = 3;
		}

		dsk_echo_level_set(level);
	}
#elif (SP_Software_Echo == OP_SP_Software_Echo_AdjustByKnob)
	{
		dsk_audio_echo_enable_ec(1);
		g_echoLevel = -2;
	}
#endif
}

__s8 dsk_echo_level_get(void)
{
	return g_echoLevel;
}
#endif



 //value rang is 0-7
 //0db -14db 2 db/step
__s32 dsk_audio_set_fm_gain(__s32 value)
{
	volatile AdcAnaCtl* pAdcAnaCtl = (volatile AdcAnaCtl*)(0x24 + 0Xf1C23C00);
	if(value>7||value<0)
		return -1;
	pAdcAnaCtl->FMtoLRMirerGain=value;
	return 0;

}

  //value rang is 0-7
 //0db -14db 2 db/step
__s32 dsk_audio_set_linein_gain(__s32 value)
 {
	volatile AdcAnaCtl* pAdcAnaCtl = (volatile AdcAnaCtl*)(0x24 + 0Xf1C23C00);
	if(value>7||value<0)
		return -1;
	pAdcAnaCtl->LineintoLRMirerGain	=value;
	return 0;

 }


__s32 dsk_audio_direct_output_on(void)
{    
    (*(volatile __u32 *)(0xf1c23c00+0x20))|=(1<<28)|(1<<29);//enable analog output
    (*(volatile __u32 *)(0xf1c23c00+0x20))|=(1<<20)|(1<<12);// mute the l and r channle mic

    return 0;
}

__s32 dsk_audio_direct_output_off(void)
{
    (*(volatile __u32 *)(0xf1c23c00+0x20))&=(~(1<<28)|~(1<<29));//enable analog output
    (*(volatile __u32 *)(0xf1c23c00+0x20))&=(~(1<<20)|~(1<<12));// mute the l and r channle mic
        
    return 0;
}




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>