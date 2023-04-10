#ifndef _IIC_H_DEF_
#define _IIC_H_DEF_


#include "epdk.h"

extern void I2C_usDelay(__s32 us);
extern void I2C_Init(void);
extern void I2C_End(void);
extern void I2C_Start(void);
extern void I2C_Stop(void);
extern __bool I2C_I2Ctransbyte(__u8 shu, __bool ACK);
extern __u8 I2C_I2Crecebyte(__bool SendACK);


#endif

