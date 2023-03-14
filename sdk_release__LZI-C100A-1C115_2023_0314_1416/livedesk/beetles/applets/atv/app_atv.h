/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:app_atv.h
* By      	: Andy.zhang
* Func		: desk atv thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_ATV_H__
#define  __APP_ATV_H__

H_WIN app_atv_create(root_para_t *para);

extern  __bool app_atv_isAVInput(void);

extern __bool app_atv_enIoctrlDetPaForDVD(void);

#endif// __APP_HOME_H_
