#include "atv_type_paint_init.h"

#if SP_APP_ATV // ��ԴIDΪ0ʱ����ʾ����ʾ����


#define ID_BMP__BG								0
#define ID_BMP__FOCUS_BG				0
#define ID_BMP__UNFOCUS_BG			0

#define ID_BMP__ITEM0_F					ID_ATV_SUBMENU_ITEM_BG_UF_BMP
#define ID_BMP__ITEM0_UF					0
#define ID_BMP__ITEM1_F					ID_ATV_SUBMENU_ITEM_BG_UF_BMP
#define ID_BMP__ITEM1_UF					0
#define ID_BMP__ITEM2_F					0
#define ID_BMP__ITEM2_UF					0

#define ID_STR__ITEM0						STRING_ATV_AVIN
#define ID_STR__ITEM1						STRING_ATV_DVD
#define ID_STR__ITEM2						0

#define W_ITEM0_F								130
#define H_ITEM0_F								48
#define W_ITEM0_UF								130
#define H_ITEM0_UF								48

#define W_ITEM1_F								130
#define H_ITEM1_F								48
#define W_ITEM1_UF								130
#define H_ITEM1_UF								48

#define X_OFT											16
#define Y_OFT										16

static const S_PAINT_ITEM	gc_sPaintItem_tab[] =
{
	{
		{
			{
				ID_BMP__ITEM0_F, 
				{X_OFT, Y_OFT, 
				W_ITEM0_F, H_ITEM0_F}
			},
			{
				ID_BMP__ITEM0_UF, 
				{X_OFT, Y_OFT, 
				W_ITEM0_UF, H_ITEM0_UF}
			},
		},
		{ID_STR__ITEM0, {X_OFT, Y_OFT, W_ITEM0_F, H_ITEM0_F}}
	},
	{
		{
			{
				ID_BMP__ITEM1_F, 
				{X_OFT, Y_OFT, 
				W_ITEM1_F, H_ITEM1_F}
			},
			{
				ID_BMP__ITEM1_UF, 
				{X_OFT, Y_OFT, 
				W_ITEM1_UF, H_ITEM1_UF}
			},
		},
		{ID_STR__ITEM1, {X_OFT, Y_OFT, W_ITEM1_F, H_ITEM1_F}}
	},
};

static S_PAINT_ITEM_BUF 				g_sPaintItemBuf_tab[__SIZEOF(gc_sPaintItem_tab)];
static S_PAINT_BUF							g_sPaintBuf;
S_COM_MENU_INFO						g_sComMenuInfo_atvType;

void atv_type_paint_init_param(mmenu_attr_t *mmenu_attr)
{
	memset(&g_sComMenuInfo_atvType, 0, sizeof(g_sComMenuInfo_atvType));
	memset(g_sPaintBuf.sPaintIconExBuf, 0, sizeof(g_sPaintBuf.sPaintIconExBuf));
	memset(g_sPaintItemBuf_tab, 0, sizeof(g_sPaintItemBuf_tab));
	
	g_sComMenuInfo_atvType.phLayer = &mmenu_attr->hLyr_sub[m_eAtvMenuLyr_tvInfo];
	g_sComMenuInfo_atvType.pFont = mmenu_attr->font;

	g_sComMenuInfo_atvType.psPaintItem = (PS_PAINT_ITEM)gc_sPaintItem_tab;
	g_sComMenuInfo_atvType.psPaintBuf = &g_sPaintBuf;
	g_sPaintBuf.psPaintItemBuf = g_sPaintItemBuf_tab;

	g_sComMenuInfo_atvType.itemTotal = m_eAtvTypeIdx_total;	//__SIZEOF(gc_sPaintItem_tab);
	g_sComMenuInfo_atvType.txtColorFocus = GUI_YELLOW;
	g_sComMenuInfo_atvType.txtColorUnfocus = GUI_WHITE;
}


#endif

