#ifndef __APP_NEW_SETTING_H_
#define  __APP_NEW_SETTING_H_

#include "New_Setting_para.h"
//#include "number_menu_i.h"
#include "New_Setting_i.h"

typedef enum
{
  APP_NEW_SETTING_GENERAL = 0,
  SETTING_POWER_MANAGE,
  SETTING_TIPS,
  SETTING_ALARM,
} app_new_setting_root_type_t;//用于存放控制manwin窗口下的framewin窗口切换标志，默认为0
typedef struct tag_app_new_setting_para
{
	GUI_FONT *setting_font;

	H_LYR layer;
	app_new_setting_root_type_t root_type;
	__s32 focus_id;
	__s32 old_focus;

} app_new_setting_para_t;//存放manwin窗口的图层和文本和功能选择标志等，暂时没有使用

typedef struct tag_app_new_setting_ctrl
{
	H_WIN h_frm_general_setting;			//存放新添加的app应用程序manwin窗口句柄

	H_LYR lry_new_frm_setting;				//存放新添加的app应用程序manwin窗口图层句柄

	app_new_setting_root_type_t root_type;
	GUI_FONT *setting_font;					//存放manwin窗口的消息文本

} app_new_setting_ctrl_t;//用于整个manwin窗口需要使用的结构体控制



H_WIN app_New_Setting_manwin_create(root_para_t *para);
__s32 app_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin);


#endif
