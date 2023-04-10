#ifndef  _IIC_READ_WRITE_H_
#define  _IIC_READ_WRITE_H_


#define READ	1
#define WRITE	0

#define IIC_CHIP__RDA5807_W 	0x20
#define IIC_CHIP__RDA5807_R 	0x21

#define IIC_CHIP__RTC6207_W 	0xC8
#define IIC_CHIP__RTC6207_R 	0xC9

__bool OperationRDAFM_2w(__u8 rw, __u16 dev_addr, __u8 *data, __s32 n_byte);

#endif

