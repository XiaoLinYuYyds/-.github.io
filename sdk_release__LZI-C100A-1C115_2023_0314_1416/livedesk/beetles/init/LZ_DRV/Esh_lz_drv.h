/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     Some backup function for shell
*
* File    : Esh_lz_drv.h
* By      : Tarring
* Version : V1.00                                                                 2009.3.2
*********************************************************************************************************
*/

#ifndef   _ESH_LZ_DRV_H_
#define   _ESH_LZ_DRV_H_

__u32 shell_get_copyright_109(__u8 *data);
__u32 shell_get_copyright_112(__u8 *data);

//#define shell_get_copyright	shell_get_copyright_109
#ifndef shell_get_copyright
__u32 shell_get_copyright(__u8 *data);
#endif

typedef __u32(*pfnGetCheckForLZDRV)(__u8 *data);

#endif	/* _ESH_LZ_DRV_H_ */

