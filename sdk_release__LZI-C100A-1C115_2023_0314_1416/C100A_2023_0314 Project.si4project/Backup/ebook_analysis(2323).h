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


typedef struct tag_ebook_analysis
{
	ES_FILE			*file_hdle;		//*文件句柄
	__u32			block_size;		//一个block的大小 4 byte
	__u32			block_len;		//一块的长度

	__u8			*block_buf;		//block的内存地址
	__ebook_one_page_t	*page_info;	//page info的内存地址
	__ebook_one_page_t	*p_operate;	//指向当前操作的info

	__u32			total_page;		//总页码
	__u32			last_row;		//上次退出时，结尾所在的行号
	__u32			last_start;		//上次退出时，结尾所在行偏移位置
	__u32			last_end;		//上次退出时，文件偏移位置

	__ebook_analysis_config_t config;		//info的配置信息
}
__ebook_analysis_t;






#endif


