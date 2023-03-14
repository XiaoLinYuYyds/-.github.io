#ifndef _COM_MENU_H_DEF_
#define _COM_MENU_H_DEF_

#include "apps.h"

#define __SIZEOF(array)		(sizeof(array)/sizeof(array[0]))


typedef enum
{
	m_ePaintIconSta_focus = 0,
	m_ePaintIconSta_unfocus,
	m_ePaintIconSta_max
} E_PAINT_ICON_STA;

typedef enum
{
	m_eIdxPaintIconEx_bg = m_ePaintIconSta_max,
	m_eIdxPaintIconEx_max
} E_IDX_PAINT_ICON_EX;

typedef RECT S_PAINT_POS, *PS_PAINT_POS;

typedef struct
{
	__s32					id;
	S_PAINT_POS		sPaintPos;
} S_PAINT_RES, *PS_PAINT_RES;

typedef struct
{
	S_PAINT_RES		sPaintRes_bg;
	__s32					id_iconBg[m_ePaintIconSta_max];
} S_PAINT_ICON, *PS_PAINT_ICON;

typedef struct
{
	S_PAINT_RES 		sPaintRes_icon[m_ePaintIconSta_max];
	S_PAINT_RES 		sPaintRes_txt;
} S_PAINT_ITEM, *PS_PAINT_ITEM;

typedef struct
{
	HTHEME				hTheme_icon;
	//void 						*pBmpBuf;
} S_PAINT_ICON_BUF, *PS_PAINT_ICON_BUF;

	#define SIZE__PAINT_ITEM_BUF_TXT					(2*32)
typedef struct
{
	S_PAINT_ICON_BUF					sPaintIconBuf[m_ePaintIconSta_max];
	char											txt[SIZE__PAINT_ITEM_BUF_TXT];
} S_PAINT_ITEM_BUF, *PS_PAINT_ITEM_BUF;

typedef struct
{
	S_PAINT_ICON_BUF					sPaintIconExBuf[m_eIdxPaintIconEx_max];
	PS_PAINT_ITEM_BUF				psPaintItemBuf;													// sPaintItemBuf[itemTotal]
} S_PAINT_BUF, *PS_PAINT_BUF;

typedef struct
{
	H_LYR										*phLayer;
	S_PAINT_POS 							layer_sPaintPos;
	GUI_FONT 								*pFont;

	PS_PAINT_ICON						psPaintIcon;
	PS_PAINT_ITEM 						psPaintItem;
	PS_PAINT_BUF							psPaintBuf;
	
	__u16 										itemTotal;
	__u16										itemFocus, itemOldFocus;
	__u32										maskItemDisable;
	GUI_COLOR								txtColorFocus, txtColorUnfocus;

	__u16										maskFlags;
} S_COM_MENU_INFO, *PS_COM_MENU_INFO;
	// for maskFlags:
	#define MASK_sComMenu_flag__AllPainted											(1<<1)
	#define MASK_sComMenu_flag__MemDev_Nonuse								(1<<0)
	

extern void com_paint_res_icon(__s32 bmpId, HTHEME *phTheme, __s32 bmp_x, __s32 bmp_y);
extern void com_paint_res_txt(__s32 strId, char *pstr, PS_PAINT_POS psPaintPos);
extern __s8 com_menu_paint_one_item(PS_COM_MENU_INFO psMenuInfo, __u16 itemIdx);
extern void com_menu_paint_all_item(PS_COM_MENU_INFO psMenuInfo);

#endif
