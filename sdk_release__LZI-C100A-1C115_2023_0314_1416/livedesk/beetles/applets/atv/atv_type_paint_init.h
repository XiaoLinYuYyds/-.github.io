#ifndef _ATV_TYPE_H_DEF_
#define _ATV_TYPE_H_DEF_

#include "beetles_app.h"
#include "atv_menu.h"

extern S_COM_MENU_INFO			g_sComMenuInfo_atvType;

typedef enum
{
	m_eAtvTypeIdx_avin = 0,
	m_eAtvTypeIdx_dvd,
	m_eAtvTypeIdx_total,
	m_eAtvTypeIdx_tv,
} E_ATV_TYPE_IDX;

extern void atv_type_paint_init_param(mmenu_attr_t *mmenu_attr);

#endif
