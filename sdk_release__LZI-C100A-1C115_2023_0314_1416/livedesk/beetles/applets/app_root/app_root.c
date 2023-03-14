/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    		: app_root_win.c
* By      		: Bayden
* Func		: app_root applets plugin interface define
* Version	: v1.0
* ============================================================================================================
* 2010-9-28 17:11:20  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "app_root.h"
#include "app_root_init.h"
#include "app_root_scene.h"
/**********************************************************************************************************************/


/**********************************************************************************************************************
����ӿ�ע��
**********************************************************************************************************************/
/* ���� */
static __s32 app_root_start(Activity  *thiz);
static __s32 app_root_suspend(Activity *thiz);
static __s32 app_root_resume(Activity *thiz);
static __s32 app_root_background(Activity *thiz);
static __s32 app_root_stop(Activity *thiz);

/* ע�����ӿ� */
AppletPlugin APPLET_PLUGIN_INTERFACE =
{
	app_root_start,
	app_root_suspend,
	app_root_resume,
	app_root_background,
	app_root_stop,
};

Activity *root_activity = NULL; //Ӧ�ó����ܲ��ָ��
H_WIN happ_root_manwin = NULL;//Ӧ�ó�����manwin���

/**********************************************************************************************************************
����ӿ�ʵ��
**********************************************************************************************************************/
static __s32 app_root_start(Activity *activity)
{
	__msg("**************app_root plugin start!**************** \n");
	__msg("****************************\n");
	//	__msg("**********TEST test_rat_partition**********\n");
	//	__msg("****************************\n");
	//	test_rat_partition(RAT_LOCAL_DISK);
	//	test_rat_partition(RAT_USB_DISK);
	//	test_rat_partition(RAT_SD_CARD);
	//   {
	//       __hdle fp;
	//       fp = esFSYS_fopen(BEETLES_APP_ROOT"apps\\lang.bin", "r");
	//        __msg("fp=%x file=%s\n", fp, BEETLES_APP_ROOT"apps\\lang.bin");
	//   esFSYS_fclose(fp);
	//    }
	//__wait__;
	app_root_init_res();
	root_activity = activity;
	happ_root_manwin = app_root_wincreate(activity);

	if(NULL == happ_root_manwin)
	{
		__msg("	 app_root_wincreate fail! \n");
		return EPDK_FAIL;
	}
	else
	{
		GUI_WinSetFocusChild(happ_root_manwin);
		__msg("	 app_root_wincreate success! \n");
		return EPDK_OK;
	}
}

static __s32 app_root_suspend(Activity *activity)
{
	__msg("	 app_root plugin suspend! \n");

	if(happ_root_manwin)
	{
		GUI_ManWinDelete(happ_root_manwin);
		happ_root_manwin = NULL;
		app_root_deinit_res();
	}

	return EPDK_OK;
}

static __s32 app_root_resume(Activity *activity)
{
	__msg("	 app_root plugin reseum! \n");
	__msg("**************app_root plugin start!**************** \n");
	app_root_init_res();
	root_activity = activity;
	happ_root_manwin = app_root_wincreate(activity);

	if(NULL == happ_root_manwin)
	{
		__msg("	 app_root_wincreate fail! \n");
		return EPDK_FAIL;
	}
	else
	{
		__msg("	 app_root_wincreate success! \n");
		return EPDK_OK;
	}
}

static __s32 app_root_background(Activity *activity)
{
	__msg("	 app_root plugin background! \n");

	if(happ_root_manwin)
	{
		GUI_ManWinDelete(happ_root_manwin);
		happ_root_manwin = NULL;
		app_root_deinit_res();
	}

	return EPDK_OK;
}

static __s32 app_root_stop(Activity *activity)
{
	__msg("	********************app_root plugin stop!**************** \n");

	if(happ_root_manwin)
	{
		GUI_ManWinDelete(happ_root_manwin);
		happ_root_manwin = NULL;
		app_root_deinit_res();
	}

	return EPDK_OK;
}
