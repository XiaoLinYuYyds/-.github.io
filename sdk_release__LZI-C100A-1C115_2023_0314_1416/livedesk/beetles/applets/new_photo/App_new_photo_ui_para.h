#ifndef		__APP_NEW_PHOTO_UI_PARA_H
#define 	__APP_NEW_PHOTO_UI_PARA_H

#include	"App_new_photo_i.h"

typedef	struct 
{
	__s32 x;
	__s32 y;
	__s32 width;
	__s32 heght;
}new_photo_rect_t;

typedef	struct tag_new_photo_uipara_t
{
	/*这里面添加存放UI控件的变量*/
	new_photo_rect_t	uipara_hrm;//framewin窗口大小
	new_photo_rect_t	uipara_hlyr;//framewin图层大小
	
}new_photo_uipara_t;/*界面内容的ui坐标和矩形大小参数*/
		
new_photo_uipara_t	*get_new_photo_uipara(__s32 rotate);

#endif
