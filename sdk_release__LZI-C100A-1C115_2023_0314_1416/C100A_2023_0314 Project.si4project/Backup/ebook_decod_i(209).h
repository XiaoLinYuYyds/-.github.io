#ifndef		__EBOOK_DECOD_H
#define		__EBOOK_DECOD_H

#include "App_new_ebook_i.h"
#define     DECODE_EBOOK_NUM     3//预解码页数
#define		MAX_FILE_NAME_SIZE	1024//文件名最大字节



typedef     void   *H_ANALYSIS_HDLE;
typedef     void   *H_DECODE_NEW;
typedef     void   *H_SHOW_NEW;
typedef     void   *H_FONT_NEW;





typedef	enum tag__NEWDECODE_MODE
{
  DECODE_NULL = 0,								/* 不解码任何页			*/
  DECODE_CURR,									/* 解码当前页			*/
  DECODE_PREV,									/* 解码当前页的前面页	*/
  DECODE_NEXT,									/* 解码所有页的后面页	*/
} __newdecode_mode_e;

typedef struct tag_EBOOK_CACHE               // 页缓冲
{
	__s32       page_sta;                   // 页面解码成功标示：1 成功、 0 失败
	__s32       page_no;                    // 缓存里的页面号
	H_WIN       page_lyr;                   // 缓存页对应的软图层
}
__newpage_cache_t;



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


typedef struct tag_EBOOK_DECODE
{
	char				    file_path[MAX_FILE_NAME_SIZE];  // 文件路径
	ES_FILE                *file_hdle;                      // 文件句柄
	H_ANALYSIS_HDLE         analysis_hdle;                  // 文件分析句柄
	H_SHOW_NEW                  show_hdle;                      // 页面显示句柄

	H_FONT_NEW                  font_hdle;                      // 字体操作句柄
	/*文件文本参数*/
	__s64                   file_size;                      // 文件大小
	__epdk_charset_enm_e    default_charset;                // 默认编码格式
	GUI_FONT               *char_font;                      // 字库句柄
	__s32                  *char_table;                     // 字符字宽表
	/*页面参数*/
	H_WIN                   show_lyr;                       // 显示图层
	__newpage_cache_t       page[DECODE_EBOOK_NUM];         // 页面缓冲器
	__u32                   data_len;                       // 一页数据的大小
	__u8                   *page_data;                      // 一页数据存放地址

	__s32                   total_page;                     // 文件总页数
	__s32                   current_page;                   // 当前显示页号

	__s32                   move_state;                     // 页面移动控制标示
	__s32                   move_height;                    // 页面移动高度
	/*预解码参数*/
	__u8				    decode_tsk;                     // 预解线程
	__s32				    decode_cnt;                     // 预解页循环计数
	__krnl_event_t		    *decode_sem;                    // 预解控制信号量
	__newdecode_mode_e		decode_sta;                     // 预解模式

	__newdecode_config_t       config;                         // 配置信息
	__epdk_charset_enm_e    charset;                        // 文件编码
}
__ebook_decode_t;

























#endif

