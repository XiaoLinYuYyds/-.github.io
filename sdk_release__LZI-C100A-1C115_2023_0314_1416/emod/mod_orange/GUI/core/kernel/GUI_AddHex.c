/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_AddHex.c
* Purpose : Converts hex value 2 string
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static const char acHex[16] =  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_AddHex
*/
void GUI_AddHex(U32 v, U8 Len, char **ps)
{
	char *s = *ps;

	if(Len > 8)
	{
		return;
	}

	(*ps) += Len;
	**ps   = '\0';    /* Make sure string is 0-terminated */

	while(Len--)
	{
		*(s + Len) = acHex[v & 15];
		v >>= 4;
	}
}

/*************************** End of file ****************************/
