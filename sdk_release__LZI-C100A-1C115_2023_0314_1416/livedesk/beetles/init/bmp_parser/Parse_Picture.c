/*
*************************************************************************************
*                         			eGon
*					        Application Of eGon2.0
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: Parse_Picture.c
*
* Author 		: javen
*
* Description 	: 图片解析
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   2010-09-10          1.0            create this file
*
*************************************************************************************
*/

#include "epdk.h"
#include  "Parse_Picture.h"
#include  "bmp.h"
//#include "mod_init_i.h"

#if 1


/*
*******************************************************************************
*                     Parse_Pic_BMP_ByBuffer
*
* Description:
*    解析存放在内存中的图片
*
* Parameters:
*    Pic_Buffer     :  input.  存放图片的入口
*    Pic_BufferSize :  input.  缓冲区大小
*    PictureInfo    :  output. 图片解析后的信息
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
/*
__s32 Parse_Pic_BMP_ByBuffer(void *Pic_Buffer, __u32 Pic_BufferSize, Picture_t *PictureInfo)
{
	bmp_info_head_t *info_head_p = NULL;
	info_head_p = (bmp_info_head_t *)((__u32)Pic_Buffer + sizeof(bmp_file_head_t));
	PictureInfo->BitCount = info_head_p->biBitCount;
	PictureInfo->Width    = info_head_p->biWidth;
	PictureInfo->Height	  = abs(info_head_p->biHeight);
	PictureInfo->RowSize  = spcl_size_align(info_head_p->biWidth * (info_head_p->biBitCount >> 3), 4);
	PictureInfo->Buffer     = (void *)((__u32)Pic_Buffer + sizeof(bmp_info_head_t) + sizeof(bmp_file_head_t));
	PictureInfo->BufferSize = Pic_BufferSize - (sizeof(bmp_info_head_t) + sizeof(bmp_file_head_t));
	return 0;
}*/

/*
*******************************************************************************
*                     Parse_Pic_BMP
*
* Description:
*    以路径名来解析图片, 并且把解析处理的图片拷贝到指定的地址，
* 如果指定的地址为NULL, 则可以存放在任何地址。
*
* Parameters:
*    Path           :  input.  图片路径
*    PictureInfo    :  output. 图片解析后的信息
*    Addr			:  input.  存放解析后的图片,
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__s32 Parse_Pic_BMP_ByPath(const char *Path, Picture_t *PictureInfo, __u32 Addr)
{
	HBMP_i_t hbmp_buf;
	HBMP  hbmp = NULL;
	eLIBs_memset(&hbmp_buf, 0, sizeof(HBMP_i_t));
	hbmp = open_bmp(Path, &hbmp_buf);

	if(hbmp == NULL)
	{
		__wrn("ERR: open_bmp failed\n");
		return -1;
	}

	PictureInfo->BitCount = get_bitcount(hbmp);
	PictureInfo->Width    = get_width(hbmp);
	PictureInfo->Height	  = get_height(hbmp);
	PictureInfo->RowSize  = get_rowsize(hbmp);
	PictureInfo->BufferSize = PictureInfo->RowSize * PictureInfo->Height;

	if(Addr)
	{
		PictureInfo->Buffer = (void *)Addr;
	}
	else
	{
		PictureInfo->Buffer = (void *)esMEMS_Balloc(PictureInfo->BufferSize);
	}

	if(PictureInfo->Buffer == NULL)
	{
		__wrn("ERR: wboot_malloc failed\n");
		goto error;
	}

	eLIBs_memset(PictureInfo->Buffer, 0, PictureInfo->BufferSize);
	get_matrix(hbmp, PictureInfo->Buffer);
	close_bmp(hbmp);
	hbmp = NULL;
	return 0;
error:
	close_bmp(hbmp);
	return -1;
}


static Picture_t 	*gs_pLogo = NULL;
static __hdle 		gs_hLogoLayer = NULL;

__s32 LogoOpen(const char *a_pcLogoPath)
{
	ES_FILE       *p_disp;
	__s32		width, height;
	__disp_layer_info_t layer_para;
	__u32 arg[3];
	__s32 ret;

	if(gs_pLogo == NULL)
	{
		gs_pLogo = (Picture_t *)eLIBs_malloc(sizeof(Picture_t));

		if(gs_pLogo == NULL)
		{
			__wrn("malloc logo bmp buffer failed!\n");
			return EPDK_FAIL;
		}
	}

	eLIBs_memset(gs_pLogo, 0, sizeof(Picture_t));
	ret = Parse_Pic_BMP_ByPath(a_pcLogoPath, gs_pLogo, NULL);

	if(-1 == ret)
	{
		return -1;
	}

	//	__msg("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	//	__msg("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	/*open display driver handle*/
	p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
	//dsk_display_lcd_on();
	if(!p_disp)
	{
		__wrn("open display device fail!\n");
		return EPDK_FAIL;
	}

	width  = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_WIDTH, 0, 0);
	height = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_HEIGHT, 0, 0);
	__msg("width = %d, height = %d\n", width, height);

	if(gs_hLogoLayer == NULL)
	{
	#if LOGO_USE_SCLAER_MODE
		arg[0] = DISP_LAYER_WORK_MODE_SCALER;
	#else
		arg[0] = DISP_LAYER_WORK_MODE_NORMAL;
	#endif
		arg[1] = 0;
		arg[2] = 0;
		gs_hLogoLayer = eLIBs_fioctrl(p_disp, DISP_CMD_LAYER_REQUEST, 0, (void *)arg);
	}

#if LOGO_USE_SCLAER_MODE
	layer_para.mode = DISP_LAYER_WORK_MODE_SCALER;
#else
	layer_para.mode = DISP_LAYER_WORK_MODE_NORMAL;
#endif
	layer_para.fb.addr[0]       = (__u32)gs_pLogo->Buffer;
	layer_para.fb.size.width    =  gs_pLogo->Width;
	layer_para.fb.size.height    = gs_pLogo->Height;
	layer_para.fb.mode          = DISP_MOD_INTERLEAVED;
	layer_para.fb.format        = DISP_FORMAT_ARGB8888;
	layer_para.fb.br_swap       = 0;
	layer_para.fb.seq           = DISP_SEQ_ARGB;
	layer_para.ck_enable        = 0;
	layer_para.alpha_en         = 1;
	layer_para.alpha_val        = 0xff;
	layer_para.pipe             = 0;
	layer_para.src_win.x        = 0;
	layer_para.src_win.y        = 0;
	layer_para.src_win.width    = gs_pLogo->Width;
	layer_para.src_win.height   = gs_pLogo->Height;
#if LOGO_USE_SCLAER_MODE
	layer_para.scn_win.x				= 0;
	layer_para.scn_win.y				= 0;
	layer_para.scn_win.width		= width;
	layer_para.scn_win.height 	= height;
#else
	layer_para.scn_win.x        = (width - gs_pLogo->Width) >> 1;
	layer_para.scn_win.y        = (height - gs_pLogo->Height) >> 1;
	layer_para.scn_win.width    = gs_pLogo->Width;
	layer_para.scn_win.height   = gs_pLogo->Height;
#endif
	__msg("scn_win.x = %d, scn_win.y = %d\n", layer_para.scn_win.x, layer_para.scn_win.y);
	__msg("gs_pLogo->Width = %d, gs_pLogo->Height = %d\n", gs_pLogo->Width, gs_pLogo->Height);
	arg[0] = gs_hLogoLayer;
	arg[1] = (__u32)&layer_para;
	arg[2] = 0;
	eLIBs_fioctrl(p_disp, DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
	arg[0] = gs_hLogoLayer;
	arg[1] = 0;
	arg[2] = 0;
	eLIBs_fioctrl(p_disp, DISP_CMD_LAYER_OPEN, 0, (void *)arg);
	eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 0, 0);
	eLIBs_fclose(p_disp);
	return EPDK_OK;
}

__s32 LogoClose(void)
{
	ES_FILE       *p_disp;
	__u32 arg[3];

	if(!gs_hLogoLayer)
	{
		return -1;
	}

	/*open display driver handle*/
	p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

	if(!p_disp)
	{
		__wrn("open display device fail!\n");
		return EPDK_FAIL;
	}

	//close logo
	arg[0] = gs_hLogoLayer;
	arg[1] = 0;
	arg[2] = 0;
	eLIBs_fioctrl(p_disp, DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
	eLIBs_fioctrl(p_disp, DISP_CMD_LAYER_RELEASE, 0, (void *)arg);
	eLIBs_fclose(p_disp);
	gs_hLogoLayer = NULL;
	return EPDK_OK;
}

__s32 LogoBufRelease(void)
{
	if(gs_pLogo)
	{
		if(gs_pLogo->Buffer)
		{
			esMEMS_Bfree(gs_pLogo->Buffer, gs_pLogo->BufferSize);
			gs_pLogo->Buffer = NULL;
		}

		eLIBs_free(gs_pLogo);
		gs_pLogo = NULL;
	}

	return EPDK_OK;
}


#endif
