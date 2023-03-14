/*
************************************************************************************************************************
*                                                          lcd
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_display_lcd.h
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
#ifndef  __DSK_DISPLAY_LCD_H__
#define  __DSK_DISPLAY_LCD_H__

#ifdef __cplusplus
extern "C" {
#endif


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
	extern __s32 dsk_display_lcd_on(void);

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
	extern __s32 dsk_display_lcd_off(__u8 a_screenCloseSel);
#define OP_SCREENOFF_AND_BKOFF			0
#define OP_SCREENOFF_BUT_BKON				1
#define OP_SCREENON_BUT_BKOFF				2


	extern __s32 dsk_display_set_lcd_pwm(__lion_bright_t level);
	extern __lion_bright_t dsk_display_get_lcd_pwm(void);
	extern __s32 dsk_display_set_lcd_pwm_ex(__lion_bright_t level);
	extern __lion_bright_t dsk_display_get_lcd_pwm_ex(void);

	extern __s32 dsk_set_display_global(__u32 val, __disp_cmd_t cmd);
	extern __s32 dsk_get_display_global(__disp_cmd_t cmd);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_LCD_H__

/* end of dsk_display_lcd.h */
