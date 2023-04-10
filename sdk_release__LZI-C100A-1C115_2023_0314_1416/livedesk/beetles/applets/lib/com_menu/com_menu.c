#include "com_menu.h"

#if SP_APP_ATV


// ------------ 参数模拟:begin  ---------------
#if 0 // 资源ID为0时，表示不显示该项
#define ID_BMP__BG								1000
#define ID_BMP__FOCUS_BG				1000
#define ID_BMP__UNFOCUS_BG			1000

#define ID_BMP__ITEM0_F					1000
#define ID_BMP__ITEM0_UF					1000
#define ID_BMP__ITEM1_F					1000
#define ID_BMP__ITEM1_UF					1000
#define ID_BMP__ITEM2_F					1000
#define ID_BMP__ITEM2_UF					1000

#define ID_STR__ITEM0						1000
#define ID_STR__ITEM1						1000
#define ID_STR__ITEM2						1000

const S_PAINT_ICON	gc_sPaintIcon =
{
	{ID_BMP__BG, {0, 0, 400, 240}},
	{ID_BMP__FOCUS_BG, ID_BMP__UNFOCUS_BG}
};

const S_PAINT_ITEM	gc_sPaintItem_tab[] =
{
	{
		{
			{ID_BMP__ITEM0_F, {0, 0, 130, 130}},
			{ID_BMP__ITEM0_UF, {0, 0, 90, 90}},
		},
		{ID_STR__ITEM0, {0, 0, 90, 90}}
	},
	{
		{
			{ID_BMP__ITEM1_F, {0, 0, 130, 130}},
			{ID_BMP__ITEM1_UF, {0, 0, 90, 90}},
		},
		{ID_STR__ITEM1, {0, 0, 90, 90}}
	},
	{
		{
			{ID_BMP__ITEM2_F, {0, 0, 130, 130}},
			{ID_BMP__ITEM2_UF, {0, 0, 90, 90}},
		},
		{ID_STR__ITEM2, {0, 0, 90, 90}}
	}
};

S_PAINT_ITEM_BUF 				g_sPaintItemBuf_tab[__SIZEOF(gc_sPaintItem_tab)];
S_PAINT_BUF						g_sPaintBuf;
S_COM_MENU_INFO			g_sComMenuInfo;

void com_menu_paint_init_param(void)
{
	//g_sComMenuInfo.phLayer = ;
	g_sComMenuInfo.layer_sPaintPos.x = 0;
	g_sComMenuInfo.layer_sPaintPos.y = 0;
	g_sComMenuInfo.layer_sPaintPos.width = 800;
	g_sComMenuInfo.layer_sPaintPos.height = 480;
	//g_sComMenuInfo.pFont = ;

	g_sComMenuInfo.psPaintIcon = (PS_PAINT_ICON)&gc_sPaintIcon;
	g_sComMenuInfo.psPaintItem = (PS_PAINT_ITEM)gc_sPaintItem_tab;
	g_sComMenuInfo.psPaintBuf = &g_sPaintBuf;
	memset(g_sPaintBuf.sPaintIconExBuf, 0, sizeof(g_sPaintBuf.sPaintIconExBuf));
	g_sPaintBuf.psPaintItemBuf = g_sPaintItemBuf_tab;
	memset(g_sPaintItemBuf_tab, 0, sizeof(g_sPaintItemBuf_tab));

	g_sComMenuInfo.itemTotal = __SIZEOF(gc_sPaintItem_tab);
	g_sComMenuInfo.itemFocus = 0;
	g_sComMenuInfo.itemOldFocus = 0;
	g_sComMenuInfo.maskItemDisable = 0;
	g_sComMenuInfo.txtColorFocus = GUI_YELLOW;
	g_sComMenuInfo.txtColorUnfocus = GUI_WHITE;

	g_sComMenuInfo.maskFlags = 0;
}
#endif
// ------------ 参数模拟:end  ---------------


void com_paint_res_icon(__s32 bmpId, HTHEME *phTheme, __s32 bmp_x, __s32 bmp_y)
{
	void *pbmp;

	if(*phTheme == NULL)
	{
		*phTheme = dsk_theme_open(bmpId);
	}
	
	pbmp = dsk_theme_hdl2buf(*phTheme);
	
	if(pbmp)
	{
		__msg("GUI_BMP_Draw(%d, %d,%d)\n", bmpId, bmp_x, bmp_y);
		GUI_BMP_Draw(pbmp, bmp_x, bmp_y);
	} 	
}

void com_paint_res_txt(__s32 strId, char *pstr, PS_PAINT_POS psPaintPos)
{
	if((!strId) || (!pstr) || (!psPaintPos))	return;

	if(!*pstr)
	{
		dsk_langres_get_menu_text(strId, pstr, SIZE__PAINT_ITEM_BUF_TXT);
	}
	
	if(pstr)
	{
		GUI_RECT gui_rect;
		gui_rect.x0 = psPaintPos->x;
		gui_rect.y0 = psPaintPos->y;
		gui_rect.x1 = gui_rect.x0 +psPaintPos->width -1;
		gui_rect.y1 = gui_rect.y0 +psPaintPos->height -1;
		GUI_DispStringInRect(pstr, &gui_rect, GUI_TA_HCENTER |GUI_TA_VCENTER);
		__msg("GUI_DispStringInRect(\"%s\", %d,%d,%d,%d)\n", pstr, gui_rect.x0, gui_rect.x1, gui_rect.y0, gui_rect.y1);
	}
}

void com_menu_paint_one_item_base(PS_COM_MENU_INFO psMenuInfo, __u16 itemIdx, E_PAINT_ICON_STA ePaintIconSta)
{
	__s32 		bmpId, strId;
	HTHEME	*phTheme;
	__s32 		bmp_x, bmp_y;
	char			*pstr = NULL;

	__msg("com_menu_paint_one_item_base(%d)\n", itemIdx);
	
	if((ePaintIconSta >= m_ePaintIconSta_max)
		|| (!psMenuInfo->psPaintItem)
		|| (!psMenuInfo->psPaintBuf)
		|| (!psMenuInfo->psPaintBuf->psPaintItemBuf)
	)
	{
		return;
	}

	bmpId = psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].id;
	if(bmpId)
	{
		bmp_x = psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.x;
		bmp_y = psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.y;
		phTheme = &psMenuInfo->psPaintBuf->psPaintItemBuf[itemIdx].sPaintIconBuf[ePaintIconSta].hTheme_icon;
		com_paint_res_icon(bmpId, phTheme, bmp_x, bmp_y);
	}

#if 1
	strId = psMenuInfo->psPaintItem[itemIdx].sPaintRes_txt.id;
	if(strId)
	{
		if(ePaintIconSta == m_ePaintIconSta_focus)
		{
			GUI_SetColor(psMenuInfo->txtColorFocus);
		}
		else
		{
			GUI_SetColor(psMenuInfo->txtColorUnfocus);
		} 
		pstr = psMenuInfo->psPaintBuf->psPaintItemBuf[itemIdx].txt;
		com_paint_res_txt(strId, pstr, &psMenuInfo->psPaintItem[itemIdx].sPaintRes_txt.sPaintPos);
	}
#endif
}

void com_menu_clr_one_item_base(PS_COM_MENU_INFO psMenuInfo, __u16 itemIdx, E_PAINT_ICON_STA ePaintIconSta)
{
	__s32 		bmpId = 0;
	HTHEME	*phTheme;
	__s32 		bmp_x, bmp_y;

	__msg("com_menu_clr_one_item_base():begin\n");
	if((ePaintIconSta >= m_ePaintIconSta_max)
		|| (!psMenuInfo->psPaintItem)
		|| (!psMenuInfo->psPaintBuf)
	)
	{
		return;
	}

	if(psMenuInfo->psPaintIcon)
	{
		bmpId = psMenuInfo->psPaintIcon->id_iconBg[ePaintIconSta];
	}
	
	if(bmpId)
	{
		bmp_x = psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.x;
		bmp_y = psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.y;
		phTheme = &psMenuInfo->psPaintBuf->sPaintIconExBuf[ePaintIconSta].hTheme_icon;
		com_paint_res_icon(bmpId, phTheme, bmp_x, bmp_y);
	}
	else
	{
		E_PAINT_ICON_STA t_ePaintIconSta;
		switch(ePaintIconSta)
		{
			case m_ePaintIconSta_focus:
				t_ePaintIconSta = m_ePaintIconSta_unfocus;
				break;
			case m_ePaintIconSta_unfocus:
				t_ePaintIconSta = m_ePaintIconSta_focus;
				break;
			default:	return;
		}

		if((psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.width
			<= psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[t_ePaintIconSta].sPaintPos.width)
			|| (psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.height
			<= psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[t_ePaintIconSta].sPaintPos.height))
		{
			return;
		}
		
		GUI_ClearRect(psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.x,
			psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.y,
			psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.x
			+psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.width -1,
			psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.y
			+psMenuInfo->psPaintItem[itemIdx].sPaintRes_icon[ePaintIconSta].sPaintPos.height -1);
	}

#if 0
		// 清除 (有需要时再添加相关代码)
	#if 0
		// 显示另一段字串 (有需要时再添加相关代码)
	#endif
#endif
	__msg("com_menu_clr_one_item_base():end\n");
}

// eg: com_menu_paint_one_item(psMenuInfo, psMenuInfo->itemFocus);	// 切换焦点项
__s8 com_menu_paint_one_item(PS_COM_MENU_INFO psMenuInfo, __u16 itemIdx)
{
	GUI_MEMDEV_Handle draw_mem = NULL;

	__msg("com_menu_paint_one_item():begin\n");
	
	if((itemIdx >= psMenuInfo->itemTotal)
		|| (!psMenuInfo->phLayer)
		|| (!*psMenuInfo->phLayer)
		|| (psMenuInfo->maskItemDisable & (1<<itemIdx))
	)
	{
		return 0;
	}

	if(!(psMenuInfo->maskFlags & MASK_sComMenu_flag__MemDev_Nonuse))
	{
		FB fb;		
		GUI_LyrWinGetFB(*psMenuInfo->phLayer, &fb);
		GUI_LyrWinSel(*psMenuInfo->phLayer);
		GUI_SetFont(psMenuInfo->pFont);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(0);
		GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
		draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
		__wrn("draw_mem=0x%x\n", draw_mem);
	}

	if(draw_mem)
	{
		GUI_MEMDEV_Select(draw_mem);
	}

	{
		if(itemIdx == psMenuInfo->itemFocus)
		{
			if(psMenuInfo->itemOldFocus != psMenuInfo->itemFocus)
			{
				// 清除old focus
				com_menu_clr_one_item_base(psMenuInfo, psMenuInfo->itemOldFocus, m_ePaintIconSta_focus);
				// 显示unfocus : old focus
				com_menu_paint_one_item_base(psMenuInfo, psMenuInfo->itemOldFocus, m_ePaintIconSta_unfocus);
				psMenuInfo->itemOldFocus = psMenuInfo->itemFocus;
			}	

			// 清除old unfocus
			com_menu_clr_one_item_base(psMenuInfo, itemIdx, m_ePaintIconSta_unfocus);
			// 显示focus
			com_menu_paint_one_item_base(psMenuInfo, itemIdx, m_ePaintIconSta_focus);
		}
		else
		{
			// 显示unfocus
			com_menu_paint_one_item_base(psMenuInfo, itemIdx, m_ePaintIconSta_unfocus);
		}
	}

	if(draw_mem)
	{
		GUI_MEMDEV_CopyToLCD(draw_mem);
		GUI_MEMDEV_Select(NULL);
		GUI_MEMDEV_Delete(draw_mem);
		draw_mem = NULL;
	}

	GUI_LyrWinSetTop(*psMenuInfo->phLayer);
	__msg("com_menu_paint_one_item():end\n");
	return 1;
}

void com_menu_paint_all_item(PS_COM_MENU_INFO psMenuInfo)
{
	GUI_MEMDEV_Handle draw_mem;

	__msg("com_menu_paint_all_item():begin\n");
	if(!psMenuInfo)	return;

	if(psMenuInfo->maskFlags & MASK_sComMenu_flag__AllPainted)
	{
		com_menu_paint_one_item(psMenuInfo, psMenuInfo->itemFocus);
	}
	else
	{
		FB fb;		
		GUI_LyrWinGetFB(*psMenuInfo->phLayer, &fb);
		GUI_LyrWinSel(*psMenuInfo->phLayer);
		GUI_SetFont(psMenuInfo->pFont);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(0);
		GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
		
		draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
		__wrn("draw_mem=0x%x\n", draw_mem);
		
		if(draw_mem)
		{
			GUI_MEMDEV_Select(draw_mem);
		}
		
		{
			__s32 i;
			__msg("maskItemDisable=0x%X\n", psMenuInfo->maskItemDisable);

			psMenuInfo->itemOldFocus = psMenuInfo->itemFocus;
			psMenuInfo->maskFlags |= MASK_sComMenu_flag__MemDev_Nonuse;
			for(i = 0; i < psMenuInfo->itemTotal; i++)
			{
				com_menu_paint_one_item(psMenuInfo, i);
			}
			psMenuInfo->maskFlags &= ~MASK_sComMenu_flag__MemDev_Nonuse;
			psMenuInfo->maskFlags |= MASK_sComMenu_flag__AllPainted;
		}
		
		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
	}
	__msg("com_menu_paint_all_item():end\n");
}


#endif
