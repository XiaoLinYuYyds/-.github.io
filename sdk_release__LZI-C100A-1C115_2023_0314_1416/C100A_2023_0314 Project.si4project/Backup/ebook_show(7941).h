#ifndef		__EBOOK_SHOW_H
#define		__EBOOK_SHOW_H


#define EBOOK_SHOW_START 5
#define EBOOK_SHOW_WIDTH 10



typedef     void   *H_EBOOK_SHOW;

typedef struct tag_EBOOKSHOW_CONFIG
{
	__u32       show_start;             // 显示的起始位置
	__u32       show_width;             // 可显示的宽度
	__u32	    scroll_width;			// 滚动条宽
	__u32       page_row;               // 一页显示的行数
	__u32       row_height;             // 行与行间高度
	__u32       col_width;              // 列与列的宽度
	__u32       font_color;             // 字体的颜色
	__u32       back_color;             // 背景的颜色
	GUI_FONT   *char_font;              // 字库句柄
	__s32      *char_table;             // 字符字宽表

	__epdk_charset_enm_e    charset;    // 字符编码
} __ebook_show_config_t;	//电子书显示控制结构体



typedef struct tag_EBOOK_SHOW
{
	ES_FILE					*disp_hdle;	//显示驱动操作句柄
	__ebook_show_config_t	config;		//配置信息

}__ebook_show_t;


H_EBOOK_SHOW EBOOK_Show_Init(void);//显示初始化
__s32 EBOOK_Show_Config(H_EBOOK_SHOW hdle, __ebook_show_config_t *config);//显示配置
__s32 EBOOK_Show_Page(H_EBOOK_SHOW hdle, __hdle lyr, void *data);//显示一页
__s32 EBOOK_Show_Uninit(H_EBOOK_SHOW hdle);//显示释放，不初始化



#endif



