/*
************************************************************************************************************************
*                                                          lcd
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_display_lcd.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.02
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.09.02        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_DISPLAY_LCD_C__
#define  __DSK_DISPLAY_LCD_C__


#include "dsk_display_i.h"

#define  ACTIVY_CYCLE_VAL_MIN			0
#define  ACTIVY_CYCLE_VAL_MAX			63

/*
****************************************************************************************************
*Name        : dsk_display_lcd_on
*Prototype   : __s32 dsk_display_lcd_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : open the LCD display.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_lcd_on(void)
{
	__disp_output_type_t  	output;
	ES_FILE      		   *p_disp = NULL;
	/* open display driver */
	p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");

	if(p_disp == NULL)
	{
		__err("Error in opening display driver.\n");
		goto error;
	}
	
	/* get output type and format */
	output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

	if(output == DISP_OUTPUT_TYPE_LCD)
	{
		esKRNL_TimeDly(10);
		eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_ON, 0, 0);
	}
	else if(output == DISP_OUTPUT_TYPE_NONE)
	{
		eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 0, 0);
		{
			// 重置屏参
			dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_BRIGHT), DISP_CMD_SET_BRIGHT);
			dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_CONTRAST), DISP_CMD_SET_CONTRAST);
			dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_HUE), DISP_CMD_SET_HUE);
			dsk_set_display_global(dsk_get_display_global(DISP_CMD_GET_SATURATION), DISP_CMD_SET_SATURATION);
		}
		esKRNL_TimeDly(10);
		eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_ON, 0, 0);
	}

	/* close display driver */
	eLIBs_fclose(p_disp);
	p_disp = NULL;
	return EPDK_OK;
error:

	if(p_disp != NULL)
	{
		eLIBs_fclose(p_disp);
		p_disp = NULL;
	}

	return EPDK_FAIL;
}

/*
****************************************************************************************************
*Name        : dsk_display_lcd_off
*Prototype   : __s32 dsk_display_lcd_off(__u8 a_screenCloseSel)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : close the LCD display.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_lcd_off(__u8 a_screenCloseSel)
{
	__disp_output_type_t  	output;
	ES_FILE      		   *p_disp = NULL;
	/* open display driver */
	p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");

	if(p_disp == NULL)
	{
		__err("Error in opening display driver.\n");
		goto error;
	}

	/* get output type and format */
	output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

	if(output == DISP_OUTPUT_TYPE_LCD)
	{
		switch(a_screenCloseSel)
		{
			case OP_SCREENOFF_AND_BKOFF :
				eLIBs_fioctrl(p_disp, DISP_CMD_LCD_OFF, 0, 0);	// note: 这句有延时
				break;

			case OP_SCREENOFF_BUT_BKON :	// 关屏时，背光要打开
				eLIBs_fioctrl(p_disp, DISP_CMD_LCD_OFF, 0, 0);	// note: 这句有延时
				eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_ON, 0, 0);
				break;

			case OP_SCREENON_BUT_BKOFF :	// 关屏时，只关背光
				eLIBs_fioctrl(p_disp, DISP_CMD_LCD_BK_OFF, 0, 0);
				break;

			default :
				break;
		}
	}

	/* close display driver */
	eLIBs_fclose(p_disp);
	p_disp = NULL;
	return EPDK_OK;
error:

	if(p_disp != NULL)
	{
		eLIBs_fclose(p_disp);
		p_disp = NULL;
	}

	return EPDK_FAIL;
}

/*
****************************************************************************************************
*Name        : dsk_display_set_lcd_pwm
*Prototype   : __s32 dsk_display_set_lcd_pwm(__LION_BRIGHT_t level)
*Arguments   : level       input. bright level.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_set_lcd_pwm(__lion_bright_t level)
{
	__disp_output_type_t  	output;
	ES_FILE      		   *p_disp = NULL;
	__disp_lcd_bright_t  	lcd_bright;
	__u32 arg[3];

	/* open display driver */
	p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");

	if(p_disp == NULL)
	{
		__err("Error in opening display driver.\n");
		goto error;
	}

	/* get output type and format */
	output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

	if(output == DISP_OUTPUT_TYPE_LCD)
	{
		switch(level)
		{
			case LION_BRIGHT_LEVEL1:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL0;
				break;

			case LION_BRIGHT_LEVEL2:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL1;
				break;

			case LION_BRIGHT_LEVEL3:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL2;
				break;

			case LION_BRIGHT_LEVEL4:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL3;
				break;

			case LION_BRIGHT_LEVEL5:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL4;
				break;

			case LION_BRIGHT_LEVEL6:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL5;
				break;

			case LION_BRIGHT_LEVEL7:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL6;
				break;

			case LION_BRIGHT_LEVEL8:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL7;
				break;

			case LION_BRIGHT_LEVEL9:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL8;
				break;

			case LION_BRIGHT_LEVEL10:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL9;
				break;

			case LION_BRIGHT_LEVEL11:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL10;
				break;

			case LION_BRIGHT_LEVEL12:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
				break;

			case LION_BRIGHT_LEVEL13:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL12;
				break;

			case LION_BRIGHT_LEVEL14:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL13;
				break;

			case LION_BRIGHT_LEVEL15:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL14;
				break;

			case LION_BRIGHT_LEVEL16:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL14;
				break;

			default:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
				break;
		}

		arg[0] = lcd_bright;
		//arg[0] = level;
		arg[1] = 0;
		arg[2] = 0;
		eLIBs_fioctrl(p_disp, DISP_CMD_LCD_SET_BRIGHTNESS, 0, (void *)arg);

		__msg("dsk_display_set_lcd_pwm(%d):%d\n", level, lcd_bright);
		//__msg_jjl("dsk_display_set_lcd_pwm(%d):%d\n", level, lcd_bright);
	}

	/* close display driver */
	eLIBs_fclose(p_disp);
	p_disp = NULL;
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			para->pwm = level;
		}
	}
	return EPDK_OK;
error:

	if(p_disp != NULL)
	{
		eLIBs_fclose(p_disp);
		p_disp = NULL;
	}

	return EPDK_FAIL;
}

/*
****************************************************************************************************
*Name        : dsk_display_get_lcd_pwm
*Prototype   : __LION_BRIGHT_t dsk_display_get_lcd_pwm( void )
*Arguments   : void
*Return      : the bright level of the LCD.
*Description : get the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
__lion_bright_t dsk_display_get_lcd_pwm(void)
{
	__disp_output_type_t  	output;
	ES_FILE      		   *p_disp = NULL;
	__lion_bright_t			level;
	__disp_lcd_bright_t  	lcd_bright;
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			return para->pwm;
		}
	}
	/* open display driver */
	p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");

	if(p_disp == NULL)
	{
		__err("Error in opening display driver.\n");
		goto error;
	}

	/* get output type and format */
	level = LION_BRIGHT_LEVEL1;
	output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

	if(output == DISP_OUTPUT_TYPE_LCD)
	{
		lcd_bright = (__disp_lcd_bright_t)eLIBs_fioctrl(p_disp, DISP_CMD_LCD_GET_BRIGHTNESS, 0, 0);

		switch(lcd_bright)
		{
			case DISP_LCD_BRIGHT_LEVEL0:
				level = LION_BRIGHT_LEVEL1;
				break;

			case DISP_LCD_BRIGHT_LEVEL1:
				level = LION_BRIGHT_LEVEL2;
				break;

			case DISP_LCD_BRIGHT_LEVEL2:
				level = LION_BRIGHT_LEVEL3;
				break;

			case DISP_LCD_BRIGHT_LEVEL3:
				level = LION_BRIGHT_LEVEL4;
				break;

			case DISP_LCD_BRIGHT_LEVEL4:
				level = LION_BRIGHT_LEVEL5;
				break;

			case DISP_LCD_BRIGHT_LEVEL5:
				level = LION_BRIGHT_LEVEL6;
				break;

			case DISP_LCD_BRIGHT_LEVEL6:
				level = LION_BRIGHT_LEVEL7;
				break;

			case DISP_LCD_BRIGHT_LEVEL7:
				level = LION_BRIGHT_LEVEL8;
				break;

			case DISP_LCD_BRIGHT_LEVEL8:
				level = LION_BRIGHT_LEVEL9;
				break;

			case DISP_LCD_BRIGHT_LEVEL9:
				level = LION_BRIGHT_LEVEL10;
				break;

			case DISP_LCD_BRIGHT_LEVEL10:
				level = LION_BRIGHT_LEVEL11;
				break;

			case DISP_LCD_BRIGHT_LEVEL11:
				level = LION_BRIGHT_LEVEL12;
				break;

			case DISP_LCD_BRIGHT_LEVEL12:
				level = LION_BRIGHT_LEVEL13;
				break;

			case DISP_LCD_BRIGHT_LEVEL13:
				level = LION_BRIGHT_LEVEL14;
				break;

			case DISP_LCD_BRIGHT_LEVEL14:
				level = LION_BRIGHT_LEVEL15;
				break;

			case DISP_LCD_BRIGHT_LEVEL15:
				level = LION_BRIGHT_LEVEL16;
				break;

			default:
				level = LION_BRIGHT_;
				break;
		}
	}

	/* close display driver */
	eLIBs_fclose(p_disp);
	p_disp = NULL;
	__msg("dsk_display_get_lcd_pwm()=%d\n", level);
	//__msg_jjl("dsk_display_get_lcd_pwm()=%d\n", level);
	return level;
error:

	if(p_disp != NULL)
	{
		eLIBs_fclose(p_disp);
		p_disp = NULL;
	}

	return LION_BRIGHT_;
}

__s32 dsk_display_set_lcd_pwm_ex(__lion_bright_t level)
{
	__disp_output_type_t  	output;
	ES_FILE      		   *p_disp = NULL;
	ES_FILE				   *pfile_pwm = NULL;
	__disp_lcd_bright_t  	lcd_bright;

	/* open display driver */
	p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");

	if(p_disp == NULL)
	{
		__err("Error in opening display driver.\n");
		goto error1;
	}

	pfile_pwm = eLIBs_fopen("b:\\USER\\PWM","rb+");
	if (pfile_pwm == NULL)
	{
		__err("Error in opening pwm driver.\n");
		goto error2;
	}
	/* get output type and format */
	output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

	if(output == DISP_OUTPUT_TYPE_LCD)
	{
		switch(level)
		{
			case LION_BRIGHT_LEVEL1:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL0;
				break;

			case LION_BRIGHT_LEVEL2:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL1;
				break;

			case LION_BRIGHT_LEVEL3:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL2;
				break;

			case LION_BRIGHT_LEVEL4:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL3;
				break;

			case LION_BRIGHT_LEVEL5:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL4;
				break;

			case LION_BRIGHT_LEVEL6:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL5;
				break;

			case LION_BRIGHT_LEVEL7:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL6;
				break;

			case LION_BRIGHT_LEVEL8:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL7;
				break;

			case LION_BRIGHT_LEVEL9:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL8;
				break;

			case LION_BRIGHT_LEVEL10:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL9;
				break;

			case LION_BRIGHT_LEVEL11:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL10;
				break;

			case LION_BRIGHT_LEVEL12:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
				break;

			case LION_BRIGHT_LEVEL13:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL12;
				break;

			case LION_BRIGHT_LEVEL14:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL13;
				break;

			case LION_BRIGHT_LEVEL15:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL14;
				break;

			case LION_BRIGHT_LEVEL16:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL14;
				break;

			default:
				lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
				break;
		}

		lcd_bright = ((ACTIVY_CYCLE_VAL_MAX - ACTIVY_CYCLE_VAL_MIN)*(lcd_bright+1) / (DISP_LCD_BRIGHT_LEVEL15 - DISP_LCD_BRIGHT_LEVEL0));

		eLIBs_fioctrl(pfile_pwm, PWM_CMD_SET_PWM_FRE, 30, (void *)0); //频率为30 KHZ
		eLIBs_fioctrl(pfile_pwm, PWM_CMD_SET_ACTIVY_CYCLE, lcd_bright, (void *)0);//0~64
		eLIBs_fioctrl(pfile_pwm, PWM_CMD_START, 0, (void *)1); //打开PWM0   最后一个参数0:pwm0 1:pwm1 
		esKRNL_TimeDly(2);
		
		eLIBs_fclose(pfile_pwm);
		pfile_pwm = NULL;
	}

	/* close display driver */
	eLIBs_fclose(p_disp);
	p_disp = NULL;
	{
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

		if(para)
		{
			para->pwm1 = level;
		}
	}
	return EPDK_OK;
error2:
	if(pfile_pwm != NULL)
	{
		eLIBs_fclose(pfile_pwm);
		pfile_pwm = NULL;
	}
error1:
	if(p_disp != NULL)
	{
		eLIBs_fclose(p_disp);
		p_disp = NULL;
	}

	return EPDK_FAIL;
}

__lion_bright_t dsk_display_get_lcd_pwm_ex(void)
{
	reg_system_para_t *para;

	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

	if(para)
	{
		return para->pwm1;
	}
	else
	{
		return LION_BRIGHT_LEVEL12;
	}
	
}

__s32 dsk_set_display_global(__u32 val, __disp_cmd_t cmd)
{
	__u32 *pVal = NULL;
	reg_system_para_t *para;
	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

	switch(cmd)
	{
		case DISP_CMD_SET_BRIGHT :
			if(para)
			{
				pVal = &para->bright;
			}

			break;

		case DISP_CMD_SET_CONTRAST :
			if(para)
			{
				pVal = &para->contrast;
			}

			break;

		case DISP_CMD_SET_HUE :
			if(para)
			{
				pVal = &para->hue;
			}

			break;

		case DISP_CMD_SET_SATURATION :
			if(para)
			{
				pVal = &para->saturability;
			}

			break;

		default :
			return EPDK_FAIL;
	}

	__msg("dsk_set_display_global(%d, %d)\n", val, cmd);
	{
		ES_FILE 	*p_disp;
		__u32	arg[3];
		p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

		if(NULL != p_disp)
		{
			if(pVal)
			{
				*pVal = val;
			}

			arg[0] = val;
			arg[1] = 0;
			arg[2] = 0;
			eLIBs_fioctrl(p_disp, DISP_CMD_ENHANCE_ON, 0, (void *)0);
			eLIBs_fioctrl(p_disp, cmd, 0, (void *)arg);
			eLIBs_fclose(p_disp);
			return EPDK_OK;
		}
	}
	return EPDK_FAIL;
}

__s32 dsk_get_display_global(__disp_cmd_t cmd)
{
	__s32	val = EPDK_FAIL;
	reg_system_para_t *para;
	para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

	switch(cmd)
	{
		case DISP_CMD_GET_BRIGHT :
			if(para)
			{
				val = para->bright;
			}

			break;

		case DISP_CMD_GET_CONTRAST :
			if(para)
			{
				val = para->contrast;
			}

			break;

		case DISP_CMD_GET_HUE :
			if(para)
			{
				val = para->hue;
			}

			break;

		case DISP_CMD_GET_SATURATION :
			if(para)
			{
				val = para->saturability;
			}

			break;

		default :
			return EPDK_FAIL;
	}

	if(val <= 0)
	{
		ES_FILE 	*p_disp;
		p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

		if(NULL != p_disp)
		{
			eLIBs_fioctrl(p_disp, DISP_CMD_ENHANCE_ON, 0, (void *)0);
			val = eLIBs_fioctrl(p_disp, cmd, 0, (void *)0);
			eLIBs_fclose(p_disp);
		}
	}

	__msg("dsk_get_display_global(%d)=%d\n", cmd, val);
	return val;
}


#endif     //  ifndef __DSK_DISPLAY_LCD_C__

/* end of dsk_display_lcd.c */
