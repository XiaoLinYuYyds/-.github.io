/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.29
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
* Gary.Wang      2009.09.29       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_c
#define  __robin_c


#include "robin_i.h"


#define ROBIN_MONITOR_PRIO              KRNL_priolevel3


static robin_mode_e  cur_robin_mode = ROBIN_MODE_UNKNOWN;

static __bool   robin_open_state  = EPDK_FALSE;


static void  _release_res(void);


/***************************************************************************************************
*Name        : robin_open
*Prototype   : __s32  robin_open( robin_mode_e robin_mode, void *arg_p )
*Arguments   : robin_mode    input. the work mode of the robin module.
*              arg_p       input. arguments, should be NULL at present.
*Return      : ==0    succeed
*              !=0    fail
*Description : open robin.
*Other       :
***************************************************************************************************/
__s32  robin_open(robin_mode_e robin_mode, robin_open_arg_t *arg_p)
{
	if(EPDK_TRUE == robin_is_open())
	{
		return EPDK_OK;
	}

	if(arg_p)
	{
		if(0 == arg_p->reserve_mem_size)
		{
			arg_p->reserve_mem_size = 512 * 1024;
		}
	}

	switch(robin_mode)
	{
		case ROBIN_MODE_AUDIO_MIN :
			npl_delete();

			if(npl_create(RAT_MEDIA_TYPE_AUDIO))
			{
				goto error;
			}

			if(cedar_open(arg_p))
			{
				goto error;
			}

			if(get_feedback_msgQ())
			{
				goto error;
			}

			if(cmdQ_create())
			{
				goto error;
			}

			if(robin_feedbackQ_create())
			{
				goto error;
			}

			if(robin_monitor_create(ROBIN_MONITOR_PRIO))
			{
				goto error;
			}

			if(robin_cedar_mutex_create())
			{
				goto error;
			}

			if(robin_cmd_mutex_create())
			{
				goto error;
			}

			break;

		case ROBIN_MODE_AUDIO_MAX :
			__wrn("ROBIN_MODE_AUDIO_MAX ...\n");
			npl_delete();
			__wrn("1 ...\n");
			if(-1==npl_create(RAT_MEDIA_TYPE_AUDIO))
			{
				__wrn("2 is...\n");
				goto error;
			}
			__wrn("2 ...\n");
			if(cedar_open(arg_p))
			{
				goto error;
			}
			__wrn("3 ...\n");
			if(willow_open())
			{
				goto error;
			}
			__wrn("4 ...\n");
			if(get_feedback_msgQ())
			{
				goto error;
			}
			__wrn("5 ...\n");
			if(disp_open())
			{
				goto error;
			}
			__wrn("6 ...\n");
			if(cmdQ_create())
			{
				goto error;
			}
			__wrn("7 ...\n");
			if(robin_feedbackQ_create())
			{
				goto error;
			}
			__wrn("8 ...\n");
			if(robin_monitor_create(ROBIN_MONITOR_PRIO))
			{
				goto error;
			}
			__wrn("9 ...\n");
			if(robin_cedar_mutex_create())
			{
				goto error;
			}
			__wrn("10 ...\n");
			if(robin_cmd_mutex_create())
			{
				goto error;
			}
			__wrn("11 ...\n");
			break;

		case ROBIN_MODE_VIDEO_MIN :
		case ROBIN_MODE_VIDEO_MAX :
			__wrn("ROBIN_MODE_VIDEO_MAX ...\n");
			if(orchid_open())
			{
				__wrn("1 ...\n");
				goto error;
			}

			if(npl_create(RAT_MEDIA_TYPE_VIDEO))
			{
				__wrn("npl_create is RAT_MEDIA_TYPE_VIDEO \n");
				goto error;
			}

			if(cedar_open(arg_p))
			{
				__wrn("2 ...\n");
				goto error;
			}

			if(get_feedback_msgQ())
			{
				__wrn("3 ...\n");
				goto error;
			}

			if(disp_open())
			{
				__wrn("4 ...\n");
				goto error;
			}

			if(cmdQ_create())
			{
				__wrn("5 ...\n");
				goto error;
			}

			if(robin_feedbackQ_create())
			{
				__wrn("6 ...\n");
				goto error;
			}

			if(robin_monitor_create(ROBIN_MONITOR_PRIO))
			{
				__wrn("7 ...\n");
				goto error;
			}

			if(robin_cedar_mutex_create())
			{
				__wrn("8 ...\n");
				goto error;
			}

			if(robin_cmd_mutex_create())
			{
				__wrn("9 ...\n");
				goto error;
			}

			break;

		default :
			{
				__wrn("10 ...\n");
			goto error;
			}
	}

	cur_robin_mode = robin_mode;
	to_quit_robin  = EPDK_FALSE;
	robin_open_state  = EPDK_TRUE;
	return 0;
error:
	_release_res();
	cur_robin_mode = ROBIN_MODE_UNKNOWN;
	to_quit_robin  = EPDK_FALSE;
	return -1;
}



/***************************************************************************************************
*Name        : robin_close
*Prototype   : void  robin_close( void )
*Arguments   : void
*Return      : void
*Description : close robin.
*Other       :
***************************************************************************************************/
void  robin_close(void)
{
	if(EPDK_FALSE == robin_is_open())
	{
		return ;
	}

	to_quit_robin  = EPDK_TRUE;
	robin_open_state  = EPDK_FALSE;
	__here__;
	_release_res();
	__here__
	cur_robin_mode = ROBIN_MODE_UNKNOWN;
}



/***************************************************************************************************
*Name        : robin_set_mode
*Prototype   : __s32  robin_set_mode( robin_mode_e robin_mode )
*Arguments   : robin_mode    input. the work mode of the robin module.
*Return      : ==0    succeed
*              !=0    fail
*Description : set the work mode of the robin module.
*Other       :
***************************************************************************************************/
__s32  robin_set_mode(robin_mode_e robin_mode, robin_open_arg_t *arg_p)
{
	if(arg_p)
	{
		if(0 == arg_p->reserve_mem_size)
		{
			arg_p->reserve_mem_size = 512 * 1024;
		}
	}

	switch(robin_mode)
	{
		case ROBIN_MODE_AUDIO_MIN :
			disp_close();
			willow_close();
			npl_delete();

			if(orchid_open())
			{
				goto error;
			}

			if(npl_create(RAT_MEDIA_TYPE_AUDIO))
			{
				goto error;
			}

			if(cedar_open(arg_p))
			{
				goto error;
			}

			if(get_feedback_msgQ())
			{
				goto error;
			}

			if(cmdQ_create())
			{
				goto error;
			}

			if(robin_feedbackQ_create())
			{
				goto error;
			}

			if(robin_monitor_create(ROBIN_MONITOR_PRIO))
			{
				goto error;
			}

			if(robin_cedar_mutex_create())
			{
				goto error;
			}

			if(robin_cmd_mutex_create())
			{
				goto error;
			}

			break;

		case ROBIN_MODE_AUDIO_MAX :
			npl_delete();

			if(orchid_open())
			{
				goto error;
			}

			if(npl_create(RAT_MEDIA_TYPE_AUDIO))
			{
				goto error;
			}

			if(cedar_open(arg_p))
			{
				goto error;
			}

			if(willow_open())
			{
				goto error;
			}

			if(get_feedback_msgQ())
			{
				goto error;
			}

			if(disp_open())
			{
				goto error;
			}

			if(cmdQ_create())
			{
				goto error;
			}

			if(robin_feedbackQ_create())
			{
				goto error;
			}

			if(robin_monitor_create(ROBIN_MONITOR_PRIO))
			{
				goto error;
			}

			if(robin_cedar_mutex_create())
			{
				goto error;
			}

			if(robin_cmd_mutex_create())
			{
				goto error;
			}

			break;

		case ROBIN_MODE_VIDEO_MIN :
		case ROBIN_MODE_VIDEO_MAX :
			willow_close();

			if(orchid_open())
			{
				goto error;
			}

			if(npl_create(RAT_MEDIA_TYPE_VIDEO))
			{
				goto error;
			}

			if(cedar_open(arg_p))
			{
				goto error;
			}

			if(get_feedback_msgQ())
			{
				goto error;
			}

			if(disp_open())
			{
				goto error;
			}

			if(cmdQ_create())
			{
				goto error;
			}

			if(robin_feedbackQ_create())
			{
				goto error;
			}

			if(robin_monitor_create(ROBIN_MONITOR_PRIO))
			{
				goto error;
			}

			if(robin_cedar_mutex_create())
			{
				goto error;
			}

			if(robin_cmd_mutex_create())
			{
				goto error;
			}

			break;

		default :
			goto error;
	}

	cur_robin_mode = robin_mode;
	return 0;
error:
	_release_res();
	cur_robin_mode = ROBIN_MODE_UNKNOWN;
	return -1;
}



/***************************************************************************************************
*Name        : robin_get_mode
*Prototype   : robin_mode_e  robin_get_mode( void )
*Arguments   : void
*Return      : the work mode of the robin module.
*Description : get the work mode of the robin module.
*Other       :
***************************************************************************************************/
robin_mode_e  robin_get_mode(void)
{
	return cur_robin_mode;
}

__bool robin_is_open(void)
{
	return robin_open_state;
}

static void  _release_res(void)
{
	__here__;
	robin_monitor_delete();
	__here__;
	cmdQ_delete();
	__here__;
	robin_feedbackQ_delete();
	__here__;
	disp_close();
	__here__;
	willow_close();
	__here__;
	cedar_close();
	__here__;
	npl_delete();
	__here__;
	orchid_close();
	__here__;
	robin_cmd_mutex_delete();
	__here__;
	robin_cedar_mutex_delete();
	__here__;
}

#endif     //  ifndef __robin_c

/* end of robin.c */
