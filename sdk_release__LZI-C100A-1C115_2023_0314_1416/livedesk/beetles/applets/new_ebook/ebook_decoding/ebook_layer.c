#include "ebook_decod_i.h"



/*
**********************************************************************************************************************
*                                               MBOOK_Layer_CreatLayer
*
* Description:	创建图层8bpp
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
H_WIN	EBOOK_8bpp_layer_creat(__u32 w, __u32 h, __s32 pipe)
{
	H_WIN   hlyr;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, 0}},    		/* fmt       */
	};
	__disp_layer_para_t para =
	{
		DISP_LAYER_WORK_MODE_PALETTE,                   /* mode      */
		0,                                              /* ck_mode   */
		0,                                              /* alpha_en  */
		0,                                              /* alpha_val */
		1,                                              /* pipe      */
		0xff,                                           /* prio      */
		{0, 0, 0, 0},                           		/* screen    */
		{0, 0, 0, 0},                               	/* source    */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
		NULL                                            /* fb        */
	};
	__layerwincreate_para_t create_info =
	{
		"layer",
		NULL,
		GUI_LYRWIN_STA_SLEEP,
		0
	};
	fb.size.width  = w;
	fb.size.height = h;
	para.pipe			= pipe;
	para.scn_win.x 		= 0;
	para.scn_win.y 		= 0;
	para.scn_win.width 	= w;
	para.scn_win.height = h;
	para.src_win.x 		= 0;
	para.src_win.y 		= 0;
	para.src_win.width 	= w;
	para.src_win.height = h;
	para.fb = &fb;
	create_info.lyrpara = &para;
	hlyr = GUI_LyrWinCreate(&create_info);
	GUI_LyrWinSetTop(hlyr);
	return hlyr;

}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_OpenShow
*
* Description:打开图层状态
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   EBOOK_Layer_OpenShow(H_WIN h_lyr)
{
	if(h_lyr == NULL)
	{
		return EPDK_FAIL;
	}

	GUI_LyrWinSetSta(h_lyr, GUI_LYRWIN_STA_ON);
	return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               EBOOK_Layer_Rotate
*
* Description:图层旋转方向，图层的竖屏显示或横屏显示
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
void    EBOOK_Layer_Rotate(H_WIN h_lyr, __u32 w, __u32 h, __u32 rotate)
{
	GUI_RECT rect;
	GUI_LyrWinSel(h_lyr);

	if(rotate == 0)
	{
		GUI_SetLCDSize(w, h);
		GUI_SetLCDDirection(rotate);//设置LCD显示屏的方向：横屏
		rect.x0 = 0;
		rect.y0 = 0;
		rect.x1 = w;
		rect.y1 = h;
		GUI_SetClipRect(&rect);		//设置用于剪切的矩形
	}
	else
	{
		GUI_SetLCDSize(h, w);		//设置LCD显示屏尺寸大小
		GUI_SetLCDDirection(rotate);//设置LCD显示屏的方向：竖屏
		rect.x0 = 0;
		rect.y0 = 0;
		rect.x1 = h;
		rect.y1 = w;
		GUI_SetClipRect(&rect);		//设置用于剪切的矩形
	}
}






























