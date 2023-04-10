/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin_spectrum.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.24
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
* Gary.Wang      2009.09.24       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_spectrum_c
#define  __robin_spectrum_c


#include "..\\robin_i.h"


/***************************************************************************************************
*Name        : robin_enable_spectrum
*Prototype   : void  robin_enable_spectrum( void )
*Arguments   : void
*Return      : void
*Description : enable parsing spectrum
*Other       :
***************************************************************************************************/
void  robin_enable_spectrum(void)
{
	if(EPDK_FALSE == robin_is_open())
	{
		return ;
	}

	esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_SPECTRUM, 1, NULL);
}



/***************************************************************************************************
*Name        : robin_disable_spectrum
*Prototype   : void  robin_disable_spectrum( void )
*Arguments   : void
*Return      : void
*Description : disable parsing spectrum
*Other       :
***************************************************************************************************/
void  robin_disable_spectrum(void)
{
	if(EPDK_FALSE == robin_is_open())
	{
		return ;
	}

	esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_SPECTRUM, 0, NULL);
}



/***************************************************************************************************
*Name        : robin_get_spectrum_info
*Prototype   : __s32  robin_get_spectrum_info( __u16 *value_list )
*Arguments   : value_list     output. a buffer to store instant spectrum info, whose item's count is
*                             SPECT_GROUP_WIDTH.
*Return      : ==0   succeed.
*              !=0   there is no spectrum info at this moment.
*Description : get instant spectrum info
*Other       :
***************************************************************************************************/
__s32 robin_get_spectrum_info(__u16 *value_list)
{
	__s32  ret;

	if(EPDK_FALSE == robin_is_open())
	{
		__wrn("robin_get_spectrum_info is 1 false...\n");
		return EPDK_FAIL;
	}

	if(value_list == NULL)
	{
		__wrn("robin_get_spectrum_info is 2 false...\n");
		return -1;
	}
	__msg("robin_hced=%d...\n",robin_hced);
	ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_SPECTRUM, 0, value_list);

	if(ret == EPDK_OK)
	{
		return 0;
	}
	else                                // no instant spectrum info
	{
		__wrn("robin_get_spectrum_info is 3 false...\n");
		return -1;
	}
}



#endif     //  ifndef __robin_spectrum_c

/* end of robin_spectrum.c */
