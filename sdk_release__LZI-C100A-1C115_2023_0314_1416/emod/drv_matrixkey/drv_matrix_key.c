/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : drv_matrix_key.c
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#include "drv_matrix_key_i.h"

#define SP_KeyMappingAll		1

#define MATRIX_KEY_DEV_NAME "MATRIX_KEY"

__matrix_key_drv_t matrix_key_drv;
__matrix_key_dev_t matrix_key_dev;

#if (!SP_KeyMappingAll)
static int matrix_keymapping[] =
{
	KPAD_MENU,
	KPAD_ZOOM,
	KPAD_ENTER,
	KPAD_RETURN,
	KPAD_LEFT,
	KPAD_RIGHT,
	KPAD_UP,
	KPAD_DOWN,
	KPAD_VOICEUP,
	KPAD_VOICEDOWN,
	KPAD_POWER,
	KPAD_POWEROFF,
	KPAD_MUSIC,
	KPAD_MOVIE,
	KPAD_MODE,
	KPAD_FMTUNER,
	KPAD_SETTING,
	KPAD_FM,
	KPAD_PHOTO,
	KPAD_EBOOK,
	KPAD_RECORD,
	KPAD_EQ,
	KPAD_REPEAT,
	KPAD_NUM0,
	KPAD_NUM1,
	KPAD_NUM2,
	KPAD_NUM3,
	KPAD_NUM4,
	KPAD_NUM5,
	KPAD_NUM6,
	KPAD_NUM7,
	KPAD_NUM8,
	KPAD_NUM9,
	KPAD_LOCK,
	KPAD_SUBMENU,
	KPAD_ATV,
	KPAD_ECHODOWN,
	KPAD_ECHOUP,
	KPAD_TVOUT,
	KPAD_PSUB,
	KPAD_PADD,
	KPAD_NUMLOCK,
};
#endif

/********************************************************************************************************
* Function:    drv_matrix_key_init
* Description: initial matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__s32 drv_matrix_key_init(void)
{
	//__hdle hReg;
	rocker_init();
	matrix_key_drv.used = 0;
	__here__;
	/*hReg = esDEV_DevReg("USER", "KEY", &matrix_key_dev_ops, 0);
	  if(!hReg)
	  {
	      __wrn("esDEV_DevReg() fail!\n");
	  	return EPDK_FAIL;
	  }
	  __here__;*/
	return matrix_key_init();
	;
}

/********************************************************************************************************
* Function:    drv_matrix_key_exit
* Description: exit matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__s32 drv_matrix_key_exit(void)
{
	rocker_exit();
	matrix_key_drv.used = 0;
	return matrix_key_exit();
	;
}

/********************************************************************************************************
* Function:    drv_matrix_key_open
* Description: open matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__mp *drv_matrix_key_open(__u32 mid, __u32 mode)
{
	__s32 cpu_sr;
	ENTER_CRITICAL(cpu_sr);

	if(matrix_key_drv.used)
	{
		__wrn("driver already openned by someone else\n");
		return (__mp *)0;
	}

	matrix_key_drv.used = 1;
	EXIT_CRITICAL(cpu_sr);
	matrix_key_drv.mid = mid;
	return (__mp *)&matrix_key_drv;
}

/********************************************************************************************************
* Function:    drv_matrix_key_close
* Description: close matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__s32 drv_matrix_key_close(__mp *mp)
{
	__matrix_key_drv_t *drv = (__matrix_key_drv_t *)mp;

	if(drv->used == 0)
	{
		return EPDK_FAIL;
	}

	drv->used = 0;
	return EPDK_OK;
}

/********************************************************************************************************
* Function:    drv_matrix_key_read
* Description: read routine of matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__u32 drv_matrix_key_read(void *pdata, __u32 size, __u32 n, __mp *mp)
{
	return 0;
}

/********************************************************************************************************
* Function:    drv_matrix_key_write
* Description: write routine of matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__u32 drv_matrix_key_write(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
	return 0;
}

/********************************************************************************************************
* Function:    drv_matrix_key_ioctrl
* Description: io ctrl routine of matrix key driver
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__s32 drv_matrix_key_ioctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
	__s32 cpu_sr = 0;

	switch(cmd)
	{
		case DRV_CMD_PLUGIN:
		{
			__u32 keyCodeNum = 0;
			__u32 i = 0;
			__input_dev_t *pKeycDev = NULL;
			ENTER_CRITICAL(cpu_sr);

			if(matrix_key_drv.status)
			{
				__wrn("Driver cannot support more than one matrix key\n");
			}

			matrix_key_drv.status = 1;
			EXIT_CRITICAL(cpu_sr);
			pKeycDev = (__input_dev_t *)eLIBs_malloc(sizeof(__input_dev_t));

			if(!pKeycDev)
			{
				__wrn("allocate memory for input device hanle fail\n");
				return EPDK_FAIL;
			}

			eLIBs_memset(pKeycDev, 0, sizeof(__input_dev_t));
			matrix_key_dev.used = 0; /* for device manager */
			/* general managment system information */
			pKeycDev->classname = INPUT_CLASS_NAME;
			pKeycDev->name      = MATRIX_KEY_DEV_NAME;
			pKeycDev->ops       = &matrix_key_dev_ops;
			pKeycDev->parg      = &matrix_key_dev;
			pKeycDev->id.product = 0x0001;
			pKeycDev->id.version = 0x0001;
			/* KEYBOARD DEVICE */
			pKeycDev->device_class = INPUT_KEYBOARD_CLASS;
			/* set keyboard support event types */
			pKeycDev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
			/* set keyboard support event codes */
#if (SP_KeyMappingAll)

			for(i = 0; i < (KEY_MAX - KEY_MIN + 1); i++)
			{
				__set_bit(KEY_MIN + i, pKeycDev->keybit);	//”≥…‰À˘”–matrix∞¥º¸¬Î
			}

#else
			keyCodeNum = sizeof(matrix_keymapping) / sizeof(matrix_keymapping[0]);

			for(i = 0; i < keyCodeNum; i++)
			{
				__set_bit(matrix_keymapping[i], pKeycDev->keybit);
			}

#endif
			/* set key event handler, handle the feedback events */
			pKeycDev->event = drv_keyc_event;

			/* register keyboard device to INPUT managemant system */
			if(esINPUT_RegDev(pKeycDev) != EPDK_OK)
			{
				__wrn("register matrix key device to input system failed\n");
				eLIBs_free(pKeycDev);
				return EPDK_FAIL;
			}

			/* attach pKeycDev to keydrv */
			matrix_key_drv.pKeyc_dev = pKeycDev;
			return EPDK_OK;
		}

		case DRV_CMD_PLUGOUT:
		{
			/* unregister keyboard deivce from INPUT manage system,
			 * free keyboard device handle.
			 */
			if(matrix_key_dev.used)
			{
				__wrn("matrix key is using now\n");
				return EPDK_FAIL;
			}

			esINPUT_UnregDev(matrix_key_drv.pKeyc_dev);
			eLIBs_free(matrix_key_drv.pKeyc_dev);
			return EPDK_OK;
		}

		case DRV_CMD_GET_STATUS:
		{
			if(matrix_key_drv.status)
			{
				return DRV_STA_BUSY;
			}
			else
			{
				return DRV_STA_FREE;
			}
		}

		default:
			break;
	}

	return EPDK_OK;
}

/********************************************************************************************************
* Function:    drv_keyc_event
* Description: deal with matrix key feedback event
* Input£∫
* Output£∫
* Other£∫
*********************************************************************************************************/
__s32 drv_keyc_event(__input_dev_t *dev, __u32 type, __u32 code, __s32 value)
{
	__wrn("matrix key feedback event [type:%d, code:%d, value:%d]\n", type, code, value);
	return EPDK_OK;
}
