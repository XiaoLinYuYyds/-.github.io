/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : Shell
* File    : startx.c
*
* By      : Steven
* Version : vx.xx.xx
* Date    : (please press f7 here)
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

static __u8 mid_mod_lemon;
static __mp *mod_lemon;

static __u8 mid_mod_desktop;
static __mp *mod_desktop;

static void __exec_startx(void)
{
	__u32 bid, sid, pid, chip;
	__u32 arg[3];
	//__getc();
	__msg("after close logo\n");
	Esh_printf("1\n");
	mid_mod_lemon = esMODS_MInstall(BEETLES_APP_ROOT"mod\\orange.mod", 0);
	Esh_printf("2\n");

	if(!mid_mod_lemon)
	{
		Esh_Error("orange.mod install fail!\n");
	}

	Esh_printf("3\n");
	mod_lemon = esMODS_MOpen(mid_mod_lemon, 0);

	if(mod_lemon == 0)
	{
		Esh_Error("orange.mod start fail!\n");
	}

	Esh_printf(".orange Installed and Openned!........................................................................................\n");
	mid_mod_desktop = esMODS_MInstall(BEETLES_APP_ROOT"mod\\desktop.mod", 0);

	if(!mid_mod_desktop)
	{
		Esh_Error("desktop.mod install fail!\n");
	}

	Esh_printf("4\n");

	mod_desktop = esMODS_MOpen(mid_mod_desktop, 0);
	if(mod_desktop == 0)
	{
		Esh_Error("desktop.mod start fail!\n");
	}

	Esh_printf("5\n");
	Esh_printf(".Desktop Installed and Openned!........................................................................................\n");
}
/*
*****************************************************************************
*                            dostartx
*
*  Description: start lemon.mod
*
*
*  Parameters:
*
*  Return value:
*****************************************************************************
*/
int dostartx(struct op *t , const char **args)
{
	/* do startx core: write by Steven */
	__exec_startx();
	/* no any error */
	return DOSUCCEED;
}
