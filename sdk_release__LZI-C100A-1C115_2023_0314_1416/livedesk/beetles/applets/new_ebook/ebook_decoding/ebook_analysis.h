#ifndef		__EBOOK_ANALYSIS_H
#define		__EBOOK_ANALYSIS_H

#define  EBOOK_MAX_SHOW_PAGE	100000
typedef  void   *H_EBOOK_ANALYSIS;

typedef struct tag_one_page     // 页：12 byte,文件偏移信息
{
	__u32       page_no;        //页号
	__u32       page_start;     //起始位置 文件偏移量
	__u32       page_end;       //结束位置 文件偏移量
} __ebook_one_page_t;

typedef struct tag_info_config //页：配置信息
{
	__u32       show_width;     // 文本显示的宽度
	__u32       page_row;       // 一页可显示的行数
	__u32       col_width;      // 列与列间的宽度
	__s32      *char_table;     // 字符字宽表

	__epdk_charset_enm_e    charset;//字符集，文本编码格式
} __ebook_analysis_config_t;







H_EBOOK_ANALYSIS  EBOOK_Analysis_Init(ES_FILE *fd);//文件编码格式分析初始化
__s32   EBOOK_Analysis_Config(H_EBOOK_ANALYSIS hdle, __ebook_analysis_config_t *config);//文件分析配置
__s32   EBOOK_Analysis_Work(H_EBOOK_ANALYSIS hdle);//分析当前文件的页面信息
__s32   EBOOK_Analysis_GetInfo(H_EBOOK_ANALYSIS hdle, __u32 page_no, __ebook_one_page_t *page_info);//获取页面信息
__s32   EBOOK_Analysis_GetPage(H_EBOOK_ANALYSIS hdle, __u32 offset);//获取页面号
__s32	EBOOK_Analysis_Uninit(H_EBOOK_ANALYSIS hdle);//释放文件分析模块





#endif


