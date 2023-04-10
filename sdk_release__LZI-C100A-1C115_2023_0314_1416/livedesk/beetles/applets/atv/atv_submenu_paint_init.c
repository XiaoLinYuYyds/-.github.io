#include "atv_submenu_paint_init.h"

#if SP_APP_ATV // 资源ID为0时，表示不显示该项


#define ID_BMP__BG								0
#define ID_BMP__FOCUS_BG				0
#define ID_BMP__UNFOCUS_BG			0

#define ID_BMP__ITEM0_F					ID_ATV_SUBMENU_ITEM_BG_F_BMP
#define ID_BMP__ITEM0_UF					ID_ATV_SUBMENU_ITEM_BG_UF_BMP
#define ID_BMP__ITEM1_F					ID_ATV_SUBMENU_ITEM_BG_F_BMP
#define ID_BMP__ITEM1_UF					ID_ATV_SUBMENU_ITEM_BG_UF_BMP
#define ID_BMP__ITEM2_F					0
#define ID_BMP__ITEM2_UF					0

#define ID_STR__ITEM0						STRING_ATV_TV_AUDIO_SYSTEM
#define ID_STR__ITEM1						STRING_ATV_TV_SEARCH_AUTO
#define ID_STR__ITEM2						0

#define W_ITEM0_F								130
#define H_ITEM0_F								48
#define W_ITEM0_UF								130
#define H_ITEM0_UF								48

#define W_ITEM1_F								130
#define H_ITEM1_F								48
#define W_ITEM1_UF								130
#define H_ITEM1_UF								48

#define X_OFT											0
#define Y_OFT										0

static const S_PAINT_ICON	gc_sPaintIcon =
{
	{
		ID_BMP__BG, 
		{0, 0, 400, 240}
	},
	{
		ID_BMP__FOCUS_BG,
		ID_BMP__UNFOCUS_BG
	}
};

static const S_PAINT_ITEM	gc_sPaintItem_tab[] =
{
	{
		{
			{
				ID_BMP__ITEM0_F, 
				{X_OFT, Y_OFT+H_ITEM1_F, 
				W_ITEM0_F, H_ITEM0_F}
			},
			{
				ID_BMP__ITEM0_UF, 
				{X_OFT, Y_OFT+H_ITEM1_UF, 
				W_ITEM0_UF, H_ITEM0_UF}
			},
		},
		{ID_STR__ITEM0, {X_OFT, Y_OFT+H_ITEM1_UF, W_ITEM0_F, H_ITEM0_F}}
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
S_COM_MENU_INFO						g_sComMenuInfo_atvSubMenu;

void atv_submenu_paint_init_param(mmenu_attr_t *mmenu_attr)
{
	g_sComMenuInfo_atvSubMenu.phLayer = &mmenu_attr->hLyr_sub[m_eAtvMenuLyr_menuList];
	g_sComMenuInfo_atvSubMenu.layer_sPaintPos.x = 32;
	g_sComMenuInfo_atvSubMenu.layer_sPaintPos.y = (_H_SCR_ -H_ITEM0_F*2-32);
	g_sComMenuInfo_atvSubMenu.layer_sPaintPos.width = W_ITEM0_F;
	g_sComMenuInfo_atvSubMenu.layer_sPaintPos.height = H_ITEM0_F*2;
	g_sComMenuInfo_atvSubMenu.pFont = mmenu_attr->font;

	g_sComMenuInfo_atvSubMenu.psPaintIcon = (PS_PAINT_ICON)&gc_sPaintIcon;
	g_sComMenuInfo_atvSubMenu.psPaintItem = (PS_PAINT_ITEM)gc_sPaintItem_tab;
	g_sComMenuInfo_atvSubMenu.psPaintBuf = &g_sPaintBuf;
	memset(g_sPaintBuf.sPaintIconExBuf, 0, sizeof(g_sPaintBuf.sPaintIconExBuf));
	g_sPaintBuf.psPaintItemBuf = g_sPaintItemBuf_tab;
	memset(g_sPaintItemBuf_tab, 0, sizeof(g_sPaintItemBuf_tab));

	g_sComMenuInfo_atvSubMenu.itemTotal = m_eTvMenuIdx_max;	//__SIZEOF(gc_sPaintItem_tab);
	g_sComMenuInfo_atvSubMenu.itemFocus = 0;
	g_sComMenuInfo_atvSubMenu.itemOldFocus = 0;
	g_sComMenuInfo_atvSubMenu.maskItemDisable = 0;
	g_sComMenuInfo_atvSubMenu.txtColorFocus = GUI_YELLOW;
	g_sComMenuInfo_atvSubMenu.txtColorUnfocus = GUI_WHITE;

	g_sComMenuInfo_atvSubMenu.maskFlags = 0;
}


#endif

