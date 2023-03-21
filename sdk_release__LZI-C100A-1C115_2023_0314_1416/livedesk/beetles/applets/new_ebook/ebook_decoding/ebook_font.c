#include "ebook_decod_i.h"


/*
************************************************************************************************************************
*                       				MBOOK_Font_Init
*
*Description: 创建字体操作句柄
*
*Arguments  : path: 字库存放路径
*             size：字体的大小
*
*
*Return     : 成功：返回操作句柄
*			  失败：NULL
*
************************************************************************************************************************
*/
H_EBOOK_FONT EBOOK_Font_Init(GUI_FONT *ebook_font, __u32 size)
{
	__u32			i;
	__ebook_font_t	*p_font;
	p_font = (__ebook_font_t *)My_Malloc(0, sizeof(__ebook_font_t));//申请内存空间

	if(p_font == NULL){
		__wrn("p_font malloc is null...\n");
		return NULL;
	}

	p_font->char_font = ebook_font;
	//申请字符字宽存放内存
	p_font->font_table = (__s32 *)My_Malloc(0, 1024);//申请内存空间

	if(p_font->font_table == NULL){//内存空间申请失败
		__wrn("font_table malloc is null...\n");
		goto EBOOK_FONT_ERROR_2;
	}

	
	GUI_SetFont(p_font->char_font);//设置文本

	for(i = 0; i < 256; i++)
	{
		*(p_font->font_table + i) = GUI_GetCharDistX(i);//获取字符间的距离
	}

	return p_font;
EBOOK_FONT_ERROR_2:
	GUI_TTF_Done(p_font->char_font);//字体
	p_font->char_font = NULL;
	return NULL;
}

/*
************************************************************************************************************************
*                       				EBOOK_Font_GetTable
*
*Description: 获取字符的字宽表
*
*Arguments  : hdle：字体操作句柄
*
*
*Return     : 成功：字符字宽表
*			  失败：NULL
*
************************************************************************************************************************
*/
__s32  *EBOOK_Font_GetTable(H_EBOOK_FONT hdle)
{
	__ebook_font_t     *p_font;

	if(hdle == NULL)
	{
		return NULL;
	}

	p_font = (__ebook_font_t *)hdle;
	return p_font->font_table;
}

/*
************************************************************************************************************************
*                       				EBOOK_Font_GetFont
*
*Description: 获取字库句柄
*
*Arguments  : hdle：字体操作句柄
*
*
*Return     : 成功：字库句柄
*			  失败：NULL
*
************************************************************************************************************************
*/
GUI_FONT   *EBOOK_Font_GetFont(H_EBOOK_FONT hdle)
{
	__ebook_font_t     *p_font;

	if(hdle == NULL)
	{
		return NULL;
	}

	p_font = (__ebook_font_t *)hdle;
	return p_font->char_font;
}


/*
************************************************************************************************************************
*                       				EBOOK_Font_Uninit
*
*Description: 释放字体操作句柄
*
*Arguments  : hdle：字体操作句柄
*
*
*Return     : 成功：EPDK_OK
*			  失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   EBOOK_Font_Uninit(H_EBOOK_FONT hdle)
{
	__ebook_font_t     *p_font;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_font = (__ebook_font_t *)hdle;

	if(p_font->font_table)
	{
		My_Mfree(0, p_font->font_table);//释放字体表内存空间
		p_font->font_table = NULL;
	}

	My_Mfree(0, p_font);//释放句柄内存空间
	return EPDK_OK;
}






































