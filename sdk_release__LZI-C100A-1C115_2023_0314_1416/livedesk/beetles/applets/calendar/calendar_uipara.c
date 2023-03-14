/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_uipara.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/13 14:35  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "calendar_uipara.h"

#if SP_APP_CALENDAR


//////////////////////////////////////////////////////////////////////////
//400x240 LCD
#define CLD_FONT_H                   16 //�����
static calendar_uipara_t calendar_ui_400x240 =
{
	// 	{//rt_lcd
	// 		0, 0, 400, 240
	// 	},

	//CLD_UI_TIME_PARA_T
	{
		{0, 0, 320, 240},                        //ui��������(ͼ������)
		{
			//CLD_UI_SIZE_TIME_T
			{42, 81},                            //����ͼƬ��С
			{25, 81},                            //ð��ͼƬ��С
			{42, 81},                            //am / pm ͼƬ��С
		},
		{1, 78},                                //����ʱ�ӵ�һλ���ֵ�����
		{1 + 42 * 6 + 30 * 2, 78},              //am, pmͼƬ������
	},

	//CLD_UI_DATE_PARA_T
	{
		{0, 0, 320, 240},                        //ui��������(ͼ������)
		{
			//CLD_UI_COLOUR_DATE_T
			APP_COLOR_WHITE,                     //һ��������ɫ
			APP_COLOR_YELLOW,                    //date focus colour
			//			APP_COLOR_RED,                       //holiday colour
			// 			APP_COLOR_WHITE,                     //������ɫ
			// 			APP_COLOR_WHITE,                     //date normal colour
			// 			APP_COLOR_WHITE,                     //������Ϣ��ɫ
		},
		{
			//CLD_UI_SIZE_DATE_T
			{46, 0},                             //���ڼ�ļ��
			{46, CLD_FONT_H * 2 + 2},            //���ڼ�ļ��
			{0, CLD_FONT_H},                     //���� / ũ�� ��ļ��
			{CLD_FONT_H * 2, CLD_FONT_H * 2},    //���ڿ�(������ / ũ��)�Ĵ�С
		},
		{5, 9},                                 //���ڿ�ʼ������
		{2, 26},                                //���ڿ�ʼ������
		{2, 202},                               //������Ϣ������
	},

	//CLD_UI_SETTING_PARA_T
	{
		{55 + 100, 8, 192 + 100, 94},                   //ui��������(ͼ������)
		{
			//CLD_UI_COLOUR_SETTING_T
			APP_COLOR_BLACK,                     //һ��������ɫ
			APP_COLOR_YELLOW,                    //focus line text colour
		},
		{
			//CLD_UI_SIZE_SETTING_T
			{172, 20},                           //һ�����ֿ��
		},
		{2, 8},                                 //"����"������
		{2, 8 + (CLD_FONT_H + 15)},             //"ģʽ"������
		{2, 8 + (CLD_FONT_H + 15) * 2},         //"ʱ��"������
	},
};

calendar_uipara_t *get_calendar_uipara(void)
{
	return &calendar_ui_400x240;
}



#endif
