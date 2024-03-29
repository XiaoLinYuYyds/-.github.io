/*
************************************************************************************************************************
*                                                      eNand
*                                     Nand flash driver logic control module define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : bsp_nand.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.25
*
* Description : This file define the function interface and some data structure export
*               for the nand bsp.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Kevin.z         2008.03.25      0.1          build the file
*
************************************************************************************************************************
*/
#ifndef __BSP_NAND_H__
#define __BSP_NAND_H__

//---------------------------------------------------------------
//  nand driver 版本号
//---------------------------------------------------------------
#define  NAND_VERSION_0                 0x02
#define  NAND_VERSION_1                 0x06

//---------------------------------------------------------------
//  结构体 定义
//---------------------------------------------------------------
typedef struct
{
	__u8        ChipCnt;                            //the count of the total nand flash chips are currently connecting on the CE pin
	__u16       ChipConnectInfo;                    //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	__u8		RbCnt;
	__u8		RbConnectInfo;						//the connect  information of the all rb  chips are connected
	__u8        RbConnectMode;						//the rb connect  mode
	__u8        BankCntPerChip;                     //the count of the banks in one nand chip, multiple banks can support Inter-Leave
	__u8        DieCntPerChip;                      //the count of the dies in one nand chip, block management is based on Die
	__u8        PlaneCntPerDie;                     //the count of planes in one die, multiple planes can support multi-plane operation
	__u8        SectorCntPerPage;                   //the count of sectors in one single physic page, one sector is 0.5k
	__u16       PageCntPerPhyBlk;                   //the count of physic pages in one physic block
	__u16       BlkCntPerDie;                       //the count of the physic blocks in one die, include valid block and invalid block
	__u16       OperationOpt;                       //the mask of the operation types which current nand flash can support support
	__u8        FrequencePar;                       //the parameter of the hardware access clock, based on 'MHz'
	__u8        EccMode;                            //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
	__u8        NandChipId[5];                      //the nand chip id of current connecting nand chip
	__u16       ValidBlkRatio;                      //the ratio of the valid physical blocks, based on 1024
	__u32 		good_block_ratio;					//good block ratio get from hwscan
	__u32		ReadRetryType;						//the read retry type
	__u32		Reserved[32];
	__u32       DDRType;


} boot_nand_para_t;

typedef struct boot_flash_info
{
	__u32 chip_cnt;
	__u32 blk_cnt_per_chip;
	__u32 blocksize;
	__u32 pagesize;
	__u32 pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
} boot_flash_info_t;


//for simple
struct boot_physical_param
{
	__u8   chip; //chip no
	__u16  block; // block no within chip
	__u16  page; // apge no within block
	__u16  sectorbitmap; //done't care
	void   *mainbuf; //data buf
	void   *oobbuf; //oob buf
};

//---------------------------------------------------------------
//  函数 定义
//---------------------------------------------------------------

//for logic
extern __s32 LML_Init(void);
extern __s32 LML_Exit(void);
extern __s32 LML_Read(__u32 nLba, __u32 nLength, void *pBuf);
extern __s32 LML_Write(__u32 nLba, __u32 nLength, void *pBuf);
extern __s32 LML_FlushPageCache(void);
extern __s32 NAND_CacheFlush(void);
extern __s32 NAND_CacheRead(__u32 blk, __u32 nblk, void *buf);
extern __s32 NAND_CacheWrite(__u32 blk, __u32 nblk, void *buf);
extern __s32 NAND_CacheOpen(void);
extern __s32 NAND_CacheClose(void);

//for format
extern __s32 FMT_Init(void);
extern __s32 FMT_Exit(void);
extern __s32 FMT_FormatNand(void);
extern void  ClearNandStruct(void);

//for scan
__s32  SCN_AnalyzeNandSystem(void);

//for physical
extern __s32 PHY_Init(void);
extern __s32 PHY_Exit(void);
extern __s32 PHY_ChangeMode(__u8 serial_mode);

//for simplie(boot0)
extern __s32 PHY_SimpleErase(struct boot_physical_param *eraseop);
extern __s32 PHY_SimpleRead(struct boot_physical_param *readop);
extern __s32 PHY_SimpleWrite(struct boot_physical_param *writeop);
extern __s32 PHY_SimpleWrite_1K(struct boot_physical_param *writeop);
extern __s32 PHY_SimpleWrite_Seq(struct boot_physical_param *writeop);
extern __s32 PHY_SimpleRead_Seq(struct boot_physical_param *readop);
extern __s32 PHY_SimpleRead_1K(struct boot_physical_param *readop);
extern __s32 BOOT_AnalyzeNandSystem(void);

//for param get&set
extern __u32 NAND_GetValidBlkRatio(void);
extern __s32 NAND_SetValidBlkRatio(__u32 ValidBlkRatio);
extern __u32 NAND_GetFrequencePar(void);
extern __s32 NAND_SetFrequencePar(__u32 FrequencePar);
extern __u32 NAND_GetNandVersion(void);
extern __s32 NAND_GetParam(boot_nand_para_t *nand_param);
extern __s32 NAND_GetFlashInfo(boot_flash_info_t *info);
extern __u32 NAND_GetDiskSize(void);
extern void  NAND_SetSrcClkName(__u32 pll_name);

/*
*   Description:
*   1. if u wanna set dma callback hanlder(sleep during dma transfer) to free cpu for other tasks,
*      one must call the interface before nand flash initialization.
      this option is also protected by dma poll method,wakup(succeed or timeout) then check
      dma transfer complete or still running.
*   2. if u use dma poll method,no need to call the interface.
*
*   3. default is unuse dma callback hanlder,that is,dma poll method.
*   4. input para  : 0:dma poll method;  1:dma callback isr,free cpu for other tasks.
*   5. return value: 0:set succeed; -1:set failed.
*/
extern __s32 NAND_SetDrqCbMethod(__u32 used);


#endif  //ifndef __NAND_LOGIC_H__

