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
* File    : GUIAlloc.c
* Purpose : Dynamic memory management
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include <stddef.h>           /* needed for definition of NULL */
#include <string.h>           /* for memcpy, memset */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Internal memory management
*
**********************************************************************
*/

/*********************************************************************
*
*       GUI_ALLOC_AllocInit
*
* Purpose:
*   Alloc memory block of the given size and initialize from the given pointer.
*   If the pointer is a NULL pointer, zeroinit
*/
GUI_HMEM GUI_ALLOC_AllocInit(const void *pInitData, GUI_ALLOC_DATATYPE Size)
{
	GUI_HMEM hMem;
	GUI_LOCK();

	if(Size == 0)
	{
		return (GUI_HMEM)0;
	}

	hMem = GUI_ALLOC_AllocNoInit(Size);

	if(hMem)
	{
		void *pMem;
		pMem = GUI_ALLOC_h2p(hMem);

		if(pInitData)
		{
			memcpy(pMem, pInitData, Size);
		}
		else
		{
			GUI_MEMSET((U8 *)pMem, 0, Size);
		}
	}

	GUI_UNLOCK();
	return hMem;
}

/*************************** End of file ****************************/
