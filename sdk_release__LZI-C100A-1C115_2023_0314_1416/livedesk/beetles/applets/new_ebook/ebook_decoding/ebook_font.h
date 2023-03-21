#ifndef		__EBOOK_FONT_H
#define		__EBOOK_FONT_H




typedef     void   *H_EBOOK_FONT;



typedef struct tag_EBOOK_FONT
{
	__s32       font_size;	//文本大小
	__s32      *font_table;	//字体表
	GUI_FONT   *char_font;	//字体句柄
}
__ebook_font_t;

H_EBOOK_FONT  EBOOK_Font_Init(GUI_FONT *font, __u32 size);//文本初始化

__s32   EBOOK_Font_Uninit(H_EBOOK_FONT hdle);///文本释放，不初始化

__s32  *EBOOK_Font_GetTable(H_EBOOK_FONT hdle);//获取文本字宽表

GUI_FONT   *EBOOK_Font_GetFont(H_EBOOK_FONT hdle);//获取文本操作句柄




#endif



