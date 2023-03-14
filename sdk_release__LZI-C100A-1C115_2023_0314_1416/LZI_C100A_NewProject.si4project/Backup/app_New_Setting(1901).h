#ifndef __APP_NEW_SETTING_H_
#define  __APP_NEW_SETTING_H_

#include "New_Setting_para.h"
#include "number_menu_i.h"

typedef enum
{
  APP_NEW_SETTING_GENERAL = 0,
  SETTING_POWER_MANAGE,
  SETTING_TIPS,
  SETTING_ALARM,
} app_new_setting_root_type_t;
typedef struct tag_app_new_setting_para
{
	GUI_FONT *setting_font;

	H_LYR layer;
	app_new_setting_root_type_t root_type;
	__s32 focus_id;
	__s32 old_focus;

} app_new_setting_para_t;

typedef struct tag_app_new_setting_ctrl
{
	H_WIN h_frm_general;		//通用设置窗口句柄
	H_WIN h_frm_power;		// 电源管理窗口句柄
	H_WIN h_frm_tips;		//产品信息提示框句柄
	H_WIN h_dialoag_win ;		//恢复出厂设置对话框句柄
	H_WIN lyr_setting;

	app_new_setting_root_type_t root_type;
	GUI_FONT *setting_font;

} app_new_setting_ctrl_t;



H_WIN app_New_Setting_manwin_create(root_para_t *para);
__s32 app_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin);


#endif
