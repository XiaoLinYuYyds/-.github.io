/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doxcopy.c
* By      : sunny
* Version : V2.00                                                                         2009.6.30
*********************************************************************************************************
*/
#include    "Esh_shell_private.h"

int doregrd(struct op *t, const char **args)
{
    int reg_addr  ;
	const char  *err = NULL;
    /* check input arguments */
	if ( NULL == args[1] ) {
	    Esh_Error("invalid argument!\n");
	    return -1;
	}
	//eLIBs_printf("arg[1]=%s\n",args[1]);

	reg_addr = SLIB_strtol(args[1],&err,0);

	//eLIBs_printf("reg_addr=%x \n",reg_addr);
	if(reg_addr<0xf0000000)
	{
		reg_addr |= 0xf0000000 ;
	}
	eLIBs_printf("reg_addr[%x]=%x \n",reg_addr,*((volatile __u32*)(reg_addr)));
    return 0;
}
#include    "Esh_shell_private.h"
