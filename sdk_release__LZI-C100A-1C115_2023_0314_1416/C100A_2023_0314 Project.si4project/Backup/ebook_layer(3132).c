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
H_WIN	EBOOK_8bpp_layer_creat(__u32 W, __u32 H, __s32 pipe)
{
	H_WIN layer = NULL;
	FB	fb =
	{
		{0, 0}, 										/* size 	 */
		{0, 0, 0},										/* buffer	 */
		{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},	/* fmt		 */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_PALETTE,					 /* mode	  */
		0,												/* ck_mode	 */
		0,												/* alpha_en  */
		0,												/* alpha_val */
		1,												/* pipe 	 */
		0xff,											/* prio 	 */
		{0, 0, 0, 0},									/* screen	 */
		{0, 0, 0, 0},									/* source	 */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
		NULL											/* fb		 */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"ebook layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= W;
	fb.size.height		= H;
	lstlyr.src_win.x		= 0;
	lstlyr.src_win.y		= 0;
	lstlyr.src_win.width	= W;
	lstlyr.src_win.height	= H;
	lstlyr.scn_win.x		= 0;
	lstlyr.scn_win.y		= 0;
	lstlyr.scn_win.width	= W;
	lstlyr.scn_win.height	= H;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);
	
	if(!layer)
	{
		__err("GUI_LyrWinCreate fail... !\n");
	}
	GUI_LyrWinSetTop(layer);//
	return layer;
}






































