#ifndef _RTC6222_H
#define _RTC6222_H
#include  "drv_fm_i.h"






extern void rtc6222_init(void);
extern u8   rtc6222_seek(u16 freq);

extern void rtc6222_set_vol(u8 volume);

extern u8 rtc6222_online(void);
extern void rtc6222_off(void);

extern u8 rtc6222_ReadReg(__u8 addr, __u8 *data,__s32 n_byte);
extern __s32 rtc6222_WriteReg(__u8 addr, __u8 *data,__s32 n_byte);
extern __s32 fm_play_rtc6222(__s32 curFreq);


#endif
