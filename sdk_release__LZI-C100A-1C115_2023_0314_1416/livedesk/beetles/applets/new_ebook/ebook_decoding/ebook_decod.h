#ifndef		__EBOOK_DECOD_H
#define		__EBOOK_DECOD_H

//#include "App_new_ebook_i.h"
#include "ebook_decod_i.h"
#if 1
#include "ebook_decod.h"
#include "ebook_show.h"
#include "ebook_analysis.h"
#include "ebook_layer.h"
#include "ebook_font.h"
#endif

#define     DECODE_EBOOK_NUM     3//预解码页数
#define		MAX_FILE_NAME_SIZE	1024//文件名最大字节



typedef     void   *H_DECODE_NEW;









typedef struct tag_NEWDECODE_CONFIG
{
	GUI_FONT  *char_font;
	__u32       font_size;              // 字体大小
	__u32       border_width;           // 两边留白宽度
	__u32       bottom_width;           // 底边留白宽度
	__u32	      scroll_width;			//滑动条的宽度
	__u32       show_width;             // 显示的宽度
	__u32       show_height;            // 显示的高度
	__u32       row_space;              // 行间距
	__u32       col_space;              // 列间距
	__u32       font_color;             // 字体的颜色
	__u32       back_color;             // 背景的颜色
	__u32       show_rotate;            // 横竖屏显示
} __newdecode_config_t;



















H_DECODE_NEW   EBOOK_Decode_Init(char *filename, __u8 *err);
__s32 EBOOK_Decode_Config(H_DECODE_NEW hdle, __newdecode_config_t *config);
__s32 EBOOK_Decode_SetDefaultCharset(H_DECODE_NEW hdle, __epdk_charset_enm_e charset);
__s32   EBOOK_Decode_ShowPage(H_DECODE_NEW hdle, __u32 offset);
__s32   EBOOK_Analysis_GetTotalPage(H_EBOOK_ANALYSIS hdle);












#endif

