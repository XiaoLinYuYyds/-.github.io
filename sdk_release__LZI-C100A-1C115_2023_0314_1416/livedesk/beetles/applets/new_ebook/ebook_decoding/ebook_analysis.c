#include "ebook_decod_i.h"

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

#if 1 //分析编码格式
/*
************************************************************************************************************************
*                       				__analysis_page_utf8 有BOM格式
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __ebook_analysis_page_utf8(__ebook_analysis_t *hdle, __u32 len)
{
	__u32                   tmp;
	__u32                   tmp_ofs;
	__u32                   tmp_row;
	__u32                   tmp_x;
	__u32                   tmp_w;
	__u8                   *p_data;
	__ebook_one_page_t           *p_one;
	__ebook_analysis_config_t    *p_config;
	tmp_ofs = 0;
	tmp_row = hdle->last_row;
	tmp_x   = hdle->last_start;
	p_data  = hdle->block_buf;
	p_one   = hdle->p_operate;
	p_config = &hdle->config;
	__wrn("utf8 hdle->total_page = %d\n",hdle->total_page);
	while(1)
	{
		tmp = *(p_data + tmp_ofs) & 0xff;

		if((tmp == 0x0a) || (tmp == 0x0d))		//换行或回车符号
		{
			tmp_row++;      // 行数加1
			tmp_x = 0;

			if(*(p_data + tmp_ofs + 1) == 0x0a)	//回车换行
			{
				tmp_ofs = tmp_ofs + 2;
			}
			else
			{
				tmp_ofs = tmp_ofs + 1;
			}

			if(tmp_row >= p_config->page_row)
			{
				hdle->total_page++;
				p_one->page_end = hdle->last_end + tmp_ofs;
				p_one->page_no = hdle->total_page;
				(p_one + 1)->page_start = p_one->page_end;
				p_one++;
				tmp_row = 0;
			}
		}
		else if((tmp & 0xf0) == 0xe0)
		{
			tmp_w = p_config->col_width;               // 行偏移相对起始位置1个汉字偏移量
			tmp_ofs = tmp_ofs + 3;
		}
		else if((tmp & 0xe0) == 0xc0)
		{
			tmp_w = p_config->col_width;               // 行偏移相对起始位置1个汉字偏移量
			tmp_ofs = tmp_ofs + 2;
		}
		else
		{
			tmp_w = *(p_config->char_table + tmp);      // 行偏移相对起始位置1个字符偏移量
			tmp_ofs = tmp_ofs + 1;
		}

		if((tmp_x + tmp_w) > p_config->show_width)
		{
			tmp_row++;
			tmp_x = tmp_w;

			if(tmp_row >= p_config->page_row)
			{
				hdle->total_page++;
				p_one->page_end = hdle->last_end + tmp_ofs;
				p_one->page_no  = hdle->total_page;
				(p_one + 1)->page_start = p_one->page_end;
				p_one++;
				tmp_row = 0;
			}
		}
		else
		{
			tmp_x = tmp_x + tmp_w;
		}

		if((tmp_ofs + 3) >= len)
		{
			break;
		}
	}

	hdle->last_end  = hdle->last_end + tmp_ofs;
	hdle->p_operate = p_one;
	hdle->last_row   = tmp_row;
	hdle->last_start = tmp_x;
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__ebook_analysis_page_utf16_le
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __ebook_analysis_page_utf16_le(__ebook_analysis_t *hdle, __u32 len)
{
	__u32                   tmp;			//存放文本内容的十六进制数据
	__u32                   tmp_ofs;		//文件光标移动位置
	__u32                   tmp_row;		//行数
	__u32                   tmp_x;			//文件所在行的偏移位置
	__u32                   tmp_w;			//列与列之间的宽度，列宽
	__u8                   *p_data;			//存放传入的文本内容数据
	__ebook_one_page_t           *p_one;	//文件偏移量信息
	__ebook_analysis_config_t    *p_config;	//页面的配置信息参数
	tmp_ofs = 0;
	tmp_row = hdle->last_row;
	tmp_x   = hdle->last_start;
	p_data  = hdle->block_buf;
	p_one   = hdle->p_operate;
	p_config = &hdle->config;


	while(1)
	{
		tmp = (*(p_data + tmp_ofs) & 0xff) + ((*(p_data + tmp_ofs + 1) & 0xff) << 8);

		if((tmp == 0x0d)||(tmp == 0x0a))	//回车或者换行符号
		{
			tmp_row++;//行数自增1
			tmp_x = 0;//所在行的文件偏移量置0，从头开始

			if((*(p_data + tmp_ofs + 2) && 0xff) == 0x0a)	//换行符号'\n'十六进制为0x0a，地址+2的原因：接收到回车换行符号十六进制分别为"0D、00、0A、00"；所以为0D回车符号时+2就等于0A换行符号
			{
				tmp_ofs = tmp_ofs + 4;						//回车换行符号共占4个字节，所以光标移动位置+4
			}
			else
			{
				tmp_ofs = tmp_ofs + 2;						//单独回车或者单独换行符号时只占2个字节
			}
		
			if(tmp_row >= p_config->page_row)				//行数大于等于页面总行数
			{
				hdle->total_page++;							//页数自增1
				p_one->page_end = hdle->last_end + tmp_ofs;	//结束的文件偏移量位置 = 2 + 光标移动的位置
				p_one->page_no  = hdle->total_page;			//页号
				(p_one + 1)->page_start = p_one->page_end;	//结构体指针地址+1，下一页开始的文件偏移量位置
				p_one++;									//存放下一页的文件偏移信息结构体
				tmp_row = 0;								//行数置0
			}
		}
		else if(tmp < 0x80)							//小于0x80为ASCII值码，相当于十六进制0x7F以下
		{
			tmp = *(p_config->char_table + tmp);
			tmp_ofs = tmp_ofs + 2;
		}
		else										//汉字编码
		{
			tmp = p_config->col_width;
			tmp_ofs = tmp_ofs + 2;
		}

		if((tmp_x + tmp_w) > p_config->show_width)
		{
			tmp_row++;
			tmp_x = tmp_w;

			if(tmp_row >= p_config->page_row)
			{
				hdle->total_page++;
				p_one->page_end = hdle->last_end + tmp_ofs;
				p_one->page_no  = hdle->total_page;
				(p_one + 1)->page_start = p_one->page_end;
				p_one++;
				tmp_row = 0;
			}
		}
		else
		{
			tmp_x = tmp_x + tmp_w;
		}

		if((tmp_ofs + 4) >= len)
		{
			break;
		}
	}

	hdle->last_end  = hdle->last_end + tmp_ofs;
	hdle->p_operate = p_one;
	hdle->last_row = tmp_row;
	hdle->last_start = tmp_x;
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__ebook_analysis_page_utf16_be
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
*Arguments  : hdle: 页结构的私有数据.
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static  __s32 __ebook_analysis_page_utf16_be(__ebook_analysis_t *hdle, __u32 len)
{
	__u32                   tmp;
	__u32                   tmp_ofs;
	__u32                   tmp_row;
	__u32                   tmp_x;
	__u32                   tmp_w;
	__u8                   *p_data;
	__ebook_one_page_t           *p_one;
	__ebook_analysis_config_t    *p_config;
	tmp_ofs = 0;
	tmp_row = hdle->last_row;
	tmp_x   = hdle->last_start;
	p_data  = hdle->block_buf;
	p_one   = hdle->p_operate;
	p_config = &hdle->config;

	while(1)
	{
		tmp = ((*(p_data + tmp_ofs) & 0xff) << 8) + (*(p_data + tmp_ofs + 1) & 0xff);

		if((tmp == 0x0d) || (tmp == 0x0a))					//回车或换行符号
		{
			tmp_row++;
			tmp_x = 0;

			if((*(p_data + tmp_ofs + 3) & 0xff) == 0x0a)	//回车符号时占用2个字节，到第三个是换行符，光标移动3个字节
			{
				tmp_ofs = tmp_ofs + 4;						//回车换行符号共占用4个字节
			}
			else
			{
				tmp_ofs = tmp_ofs + 2;						//只有换行符时，占2个字节，光标移动2字节
			}

			if(tmp_row >= p_config->page_row)
			{
				hdle->total_page++;
				p_one->page_end = hdle->last_end + tmp_ofs;
				p_one->page_no = hdle->total_page;
				(p_one + 1)->page_start = p_one->page_end;
				p_one++;
				tmp_row = 0;
			}
		}
		else if(tmp < 0x80)								//ASCII码值符号，小于0x80的
		{
			tmp_w   = *(p_config->char_table + tmp);    // 行偏移相对起始位置1个字符偏移量
			tmp_ofs = tmp_ofs + 2;						//光标移动2字节
		}
		else											//汉字，大于0x7F的
		{
			tmp_w   = p_config->col_width;              // 行偏移相对起始位置1个汉字偏移量
			tmp_ofs = tmp_ofs + 2;						//光标移动2字节
		}

		if((tmp_x + tmp_w) > p_config->show_width)
		{
			tmp_row++;
			tmp_x = tmp_w;

			if(tmp_row >= p_config->page_row)
			{
				hdle->total_page++;
				p_one->page_end = hdle->last_end + tmp_ofs;
				p_one->page_no  = hdle->total_page;
				(p_one + 1)->page_start = p_one->page_end;
				p_one++;
				tmp_row = 0;
			}
		}
		else
		{
			tmp_x = tmp_x + tmp_w;
		}

		if((tmp_ofs + 4) >= len)
		{
			break;
		}
	}

	hdle->last_end  = hdle->last_end + tmp_ofs;
	hdle->p_operate = p_one;
	hdle->last_row = tmp_row;
	hdle->last_start = tmp_x;
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__ebook_analysis_page_gbk
*
*Description: 对页里的数据，进行分析，得到单个页面信息。分析文件的GBK编码格式数据内容
*			  GBK编码格式的页面显示坐标位置；
*Arguments  : hdle: 页结构的私有数据. len:长度
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __ebook_analysis_page_gbk(__ebook_analysis_t *hdle, __u32 len)
{
#if 1
	__u32                   tmp;			//存放文本内容的十六进制数据
	__u32                   tmp_ofs;		//文件光标移动位置
	__u32                   tmp_row;		//行数
	__u32                   tmp_x;			//文件所在行的偏移位置
	__u32                   tmp_w;			//列与列之间的宽度，列宽
	__u8                   *p_data;			//存放传入的文本内容数据
	__ebook_one_page_t           *p_one;	//文件偏移量信息
	__ebook_analysis_config_t    *p_config;	//页面的配置信息参数
	tmp_ofs = 0;
	tmp_row = hdle->last_row;
	tmp_x   = hdle->last_start;
	p_data  = hdle->block_buf;
	p_one   = hdle->p_operate;
	p_config = &hdle->config;
/*	__wrn("gbk len = %d\n",len);
	__wrn("gbk tmp_row = %d\n",tmp_row);
	__wrn("gbk tmp_x = %d\n",tmp_x);
	__wrn("\n gbk p_data = %s \n",p_data);
	__wrn("gbk p_one = 0x%x\n",p_one);
	__wrn("gbk p_config = 0x%x\n",p_config);
	__wrn("gbk tmp_ofs = %d\n",tmp_ofs);
	__wrn("gbk page_row = %d\n",p_config->page_row);
*/

	while(1)
	{
		tmp = *(p_data + tmp_ofs) & 0xff;//获取文本内容数据，按位与：全1出1。
		//__wrn("gbk tmp = 0x%.2x\n",tmp);
		//__wrn("gbk 0 total_page = %d\n",hdle->total_page);
		
		//相当于文本内容里面的回车按下换行标志；换行符：0x0A意思是换行至下一行行首起始位置;
		//回车换行符：0x0D 0x0A;其中回车符为：0x0D意思是把光标移动到该行的起始位置
		if((tmp == 0x0d) || (tmp == 0x0a))
		{
			tmp_row++;										//行数自增1
			tmp_x = 0;										//光标的起始位置置0
			__wrn("gbk 11 tmp_row = %d\n",tmp_row);
			if((*(p_data + tmp_ofs + 1) & 0xff) == 0x0a)	//如果下一个文本数据内容等于换行符号的时候，就是说当出现回车换行符号时有效
			{
				tmp_ofs = tmp_ofs + 2;						//光标移动两个字节
				__wrn("tmp_ofs +2...\n");
			}
			else											//否则下一个不是换行符号的时候
			{
				tmp_ofs = tmp_ofs + 1;						//光标移动一个字节
				__wrn("tmp_ofs +1...\n");
			}

			if(tmp_row >= p_config->page_row)				//大于或等于页的总行数21行
			{
				hdle->total_page++;					 		//页数自增1
				__wrn("gbk total_page = %d\n",hdle->total_page);
				p_one->page_end = hdle->last_end + tmp_ofs;	//页面结束位置 文件偏移量
				p_one->page_no = hdle->total_page;			//页号
				(p_one + 1)->page_start = p_one->page_end;	//下一页的起始位置 文件偏移量
				p_one++;									//页面文件偏移量信息自增1
				tmp_row = 0;								//行数清0，从0行开始显示
			}
		}
		else if(tmp > 0x7f)									//标准的ASCII值码都小于0x7F,大于则认为是汉字
		{
			//__wrn("tmp > 0x7f, col_width = %d...\n",p_config->col_width);
			tmp_w   = p_config->col_width;                 // 行偏移相对起始位置1个汉字偏移量
			__wrn("0 tmp_w = %d, tmp_ofs=%d\n", tmp_w, tmp_ofs);
			tmp_ofs = tmp_ofs + 2;							//中文占两个字节，所以光标需要移动2个字节
		}
		else
		{
			//__wrn("tmp else...\n");
			tmp_w   = *(p_config->char_table + tmp);        // 行偏移相对起始位置1个字符偏移量
			//__wrn("p_config->char_table = 0x%x\n", p_config->char_table);
			//__wrn("tmp = %d\n", tmp);
			__wrn("1 tmp_w = %d, tmp_ofs=%d\n", tmp_w, tmp_ofs);
			tmp_ofs = tmp_ofs + 1;							//属于ASCII码里面的数值比如0~9，A~Z,a~z等，光标只需要移动1个字节
		}

		if((tmp_x + tmp_w) > p_config->show_width)			//一行的x轴如果 大于 文本显示的宽度1024
		{
			tmp_row++;										//行数自增1
			tmp_x = tmp_w;									
			__wrn("show_width tmp_row = %d\n",tmp_row);
			if(tmp_row >= p_config->page_row)				//行数大于等于一页的最大行数
			{
				hdle->total_page++;								//页数+1
				__wrn(" total_page = %d\n",hdle->total_page);
				p_one->page_end = hdle->last_end + tmp_ofs;		//结束位置 文件偏移量
				p_one->page_no  = hdle->total_page;				//页号；总页数
				(p_one + 1)->page_start = p_one->page_end;		//下一页起始位置 文件偏移量
				__wrn("(p_one + 1)->page_start = = %d...\n",(p_one + 1)->page_start);
				p_one++;									//页面文件偏移信息自增1
				tmp_row = 0;								//行数清0
			}
		}
		else												//未超过一行的最大宽度时
		{
			tmp_x = tmp_x + tmp_w;							//行的偏移位置 = 上一个偏移位置 + 列宽
			__wrn(" tmp_x = %d\n",tmp_x);
		}

		if((tmp_ofs + 4) >= len)							//光标偏移位置 + 4 大于等于总长度
		{
			__wrn(" tmp_ofs = %d\n",tmp_ofs);
			break;
		}
	}
	__wrn("gbk hdle->total_page = %d\n",hdle->total_page);
	//hdle->total_page = 4;

	hdle->last_end  = hdle->last_end + tmp_ofs;				//保存当前结束的文件偏移位置
	hdle->p_operate = p_one;								//文件偏移信息
	hdle->last_row = tmp_row;								//保存当前行数
	hdle->last_start = tmp_x;								//保存当前文件最后所在行的文件偏移位置
	return EPDK_OK;
#endif
}


#if 1 //整合信息
/*
************************************************************************************************************************
*                       				__ebook_analysis_page_info_charset
*
*Description: 对页里的数据，进行分析，得到单个页面数据内容信息。获取文件的数据内容：电子书文字等
*
*Arguments  : hdle: 页结构的私有数据.
*			  charset：传入的解析文件编码格式：UTF8、UTF16LE、UTF16BE、GBK等
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
static __s32 __ebook_analysis_page_info_charset(__ebook_analysis_t *hdle, __epdk_charset_enm_e  charset)
{
	#if 1
	__u32                   len;
	__ebook_analysis_t     *p_analysis;
	p_analysis = hdle;

	while(1)
	{
		// 定位文件指针
		eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, ELIBS_SEEK_SET);
		// 读一段数据
		len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);
		switch(charset)
		{
			case EPDK_CHARSET_ENM_GBK:
				__ebook_analysis_page_gbk(hdle, len);
			break;

			case EPDK_CHARSET_ENM_UTF8:
				__ebook_analysis_page_utf8(hdle, len);
			break;

			case EPDK_CHARSET_ENM_UTF16LE:
				__ebook_analysis_page_utf16_le(hdle, len);
			break;

			case EPDK_CHARSET_ENM_UTF16BE:
				__ebook_analysis_page_utf16_be(hdle, len);
			break;

		}

		if(len < p_analysis->block_len)
		{
			__wrn("len = %d\n",len);
			__wrn("block_len = %d\n",p_analysis->block_len);
			p_analysis->total_page++;
			p_analysis->p_operate->page_no = p_analysis->total_page;
			p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);
			break;
		}
	}

	p_analysis->p_operate = p_analysis->page_info;
	return EPDK_OK;
	#endif
}
	#endif

#endif

/*
************************************************************************************************************************
*                       				EBOOK_Analysis_Init
*
*Description: 初始化页面分析模块
*
*Arguments  : fd: 文件操作句柄.
*             len: 文件总长度
*
*Return     : H_EBOOK_ANALYSIS: 返回PAGE操作句柄
*
************************************************************************************************************************
*/
H_EBOOK_ANALYSIS  EBOOK_Analysis_Init(ES_FILE *fd)
{
	__ebook_analysis_t		*p_analysis;
	p_analysis = (__ebook_analysis_t *)My_Malloc(0, sizeof(__ebook_analysis_t));//申请内存

	if(p_analysis == 0)
	{
		__wrn("p_analysis malloc error...\n");
		return NULL;
	}

	eLIBs_memset(p_analysis, 0,  sizeof(__ebook_analysis_t));//结构体初始化，数据清0
	p_analysis->file_hdle  = fd;		//文件句柄
	p_analysis->block_size = 1 * 1024;	//单位：KB；块的字节大小
	p_analysis->block_len  = 1 * 1024 *1024;//块的长度
	p_analysis->block_buf  = (void *)My_Palloc(p_analysis->block_size, 0);//给块 申请内存

	if(p_analysis->block_buf == 0){
		__wrn("block_buf palloc erro...\n");
		goto PALLOC_ERROR_1;
	}

	p_analysis->page_info = (void *)My_Palloc((sizeof(__ebook_one_page_t) * EBOOK_MAX_SHOW_PAGE + 1023) / 1024, 0);	//申请2M内存

	if(p_analysis->page_info == 0){
		__wrn("page_info palloc erro...\n");
		goto PALLOC_ERROR_2;
	}

	p_analysis->p_operate = p_analysis->page_info;//页的偏移信息
	p_analysis->p_operate->page_no = 0;
	p_analysis->p_operate->page_start = 0;
	p_analysis->last_end = 0;
	return p_analysis;

PALLOC_ERROR_2:
	if(p_analysis->block_buf){//释放内存空间
		My_Pfree(p_analysis->block_buf, p_analysis->block_size);
		p_analysis->block_buf = NULL;
	}
PALLOC_ERROR_1:
	if(p_analysis){//释放内存空间
		My_Mfree(0, p_analysis);
		p_analysis = NULL;
	}

	return NULL;
}

/*
************************************************************************************************************************
*                       				EBOOK_Analysis_Config
*
*Description: 配置页面分析模块相关参数
*
*Arguments  : hdle: 操作句柄
*             config: 参数配置信息指针
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Analysis_Config(H_EBOOK_ANALYSIS hdle, __ebook_analysis_config_t *config)
{
	__ebook_analysis_t     *p_analysis;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_analysis = (__ebook_analysis_t *)hdle;
	eLIBs_memcpy(&(p_analysis->config), config, sizeof(__ebook_analysis_config_t));
	p_analysis->last_row    = 0;
	p_analysis->last_start  = 2 * p_analysis->config.col_width;//列与列间的宽度
	__wrn("config.charset = %d...\n",p_analysis->config.charset);
	switch(p_analysis->config.charset)//文件编码格式
	{
		case EPDK_CHARSET_ENM_UTF8 :
		{
			p_analysis->p_operate->page_start = 3;
			p_analysis->last_end    = 3;//从第三个十六进制字符开始，因为前面三个字符用来表示文件的编码格式为确认UTF8
		}
		break;

		case EPDK_CHARSET_ENM_UTF16BE:
		case EPDK_CHARSET_ENM_UTF16LE:
		{
			p_analysis->p_operate->page_start = 2;
			p_analysis->last_end    = 2;//从第二个十六进制字符开始，因为前面两个个字符用来表示文件的编码格式为确认UTF8
		}
		break;

		default:
		{
			p_analysis->p_operate->page_start = 0;
			p_analysis->last_end    = 0;
		}
		break;
	}

	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				EBOOK_Analysis_Work
*
*Description: 分析当前文件的页面信息
*
*Arguments  : hdle: 操作句柄
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Analysis_Work(H_EBOOK_ANALYSIS hdle)
{
	__ebook_analysis_t     *p_analysis;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_analysis = (__ebook_analysis_t *)hdle;
	p_analysis->total_page  = 0;
	p_analysis->p_operate   = p_analysis->page_info;
	p_analysis->p_operate->page_no = 0;
	p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle);//函数用以获得文件当前位置指针的位置

	switch(p_analysis->config.charset)//文件编码格式
	{
		case EPDK_CHARSET_ENM_GBK://gbk编码格式
		{
			__wrn("EPDK_CHARSET_ENM_GBK...\n");
			//__ebook_analysis_page_info_gbk(p_analysis);//分析当前页面信息GBK编码格式
			__ebook_analysis_page_info_charset(hdle, EPDK_CHARSET_ENM_GBK);
		}
		break;

		case EPDK_CHARSET_ENM_UTF8://utf8编码格式
		{
			__wrn("analysis file UTF8\n");
			//__ebook_analysis_page_info_utf8(p_analysis);//分析当前页面utf8编码格式
			__ebook_analysis_page_info_charset(hdle, EPDK_CHARSET_ENM_UTF8);
		}
		break;

		case EPDK_CHARSET_ENM_UTF16LE:
		{
			__wrn("analysis file UTF16-LE...\n");
			//__ebook_analysis_page_info_utf16_le(p_analysis);//分析当前页面utf16-LE编码格式
			__ebook_analysis_page_info_charset(hdle, EPDK_CHARSET_ENM_UTF16LE);
		}
		break;

		case EPDK_CHARSET_ENM_UTF16BE:
		{
			__wrn("analysis file UTF16-BE...\n");
			//__ebook_analysis_page_info_utf16_be(p_analysis);//分析当前页面utf16-BE编码格式
			__ebook_analysis_page_info_charset(hdle, EPDK_CHARSET_ENM_UTF16BE);
		}
		break;

		default:
			__wrn("default charset null...\n");
			break;
	}

	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Analysis_GetInfo
*
*Description: 获取一页的页面信息。
*
*Arguments  : hdle: 操作句柄.
*             page_no: 要获取的页面号
*             page_info: 页面信息存放的地址
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Analysis_GetInfo(H_EBOOK_ANALYSIS hdle, __u32 page_no, __ebook_one_page_t *page_info)
{
	__ebook_one_page_t           *p_one;
	__ebook_analysis_t     *p_analysis;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_analysis = (__ebook_analysis_t *)hdle;

	if((page_no == 0) || (page_no > p_analysis->total_page))
	{
		return EPDK_FAIL;
	}

	p_one = p_analysis->page_info + page_no - 1;
	eLIBs_memcpy(page_info, p_one, sizeof(__ebook_one_page_t));
	__wrn("EBOOK_Analysis_GetInfo is stop...\n");
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Analysis_GetPage
*
*Description: 获取指定偏移量对应的页面号。
*
*Arguments  : hdle: 操作句柄.
*             offset: 文件偏移量
*
*Return     : total_page: 成功，返回当前文件页码
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Analysis_GetPage(H_EBOOK_ANALYSIS hdle, __u32 offset)
{
	__u32                   i;
	__ebook_one_page_t           *p_one = NULL;
	__ebook_analysis_t     *p_analysis;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_analysis = (__ebook_analysis_t *)hdle;

	if(offset < 10)
	{
		__wrn("offset = %d\n",offset);
		return 1;
	}

	if(offset >= ((p_analysis->page_info + p_analysis->total_page - 1)->page_end))
	{
		return 1;
	}

	for(i = 0; i < p_analysis->total_page; i++)
	{
		p_one = p_analysis->page_info + i;

		if((p_one->page_start <= offset) && (offset < p_one->page_end))
		{
			break;
		}
	}
	__wrn("page_no = %d\n",p_one->page_no);

	return p_one->page_no;
}

/*
************************************************************************************************************************
*                       				MBOOK_Analysis_GetTotalPage
*
*Description: 分析模块获取文件总页数
*
*Arguments  : hdle: 操作句柄.
*
*Return     : total_page: 成功，返回当前文件总页码
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Analysis_GetTotalPage(H_EBOOK_ANALYSIS hdle)
{
	__ebook_analysis_t     *p_analysis;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_analysis = (__ebook_analysis_t *)hdle;
	return p_analysis->total_page;
}


/*
************************************************************************************************************************
*                       				MBOOK_Analysis_GetTotalPage
*
*Description: 删除页面分析模块
*
*Arguments  : hdle: 操作句柄.
*
*Return     : total_page: 成功，返回当前文件总页码
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32	EBOOK_Analysis_Uninit(H_EBOOK_ANALYSIS hdle)
{
	__ebook_analysis_t		*p_analysis;

	if(hdle == NULL)
	{
		__wrn("hdle is null...\n");
		return EPDK_FALSE;
	}

	p_analysis = (__ebook_analysis_t *)hdle;

	if(p_analysis->block_buf)
	{
		My_Pfree(p_analysis->block_buf, p_analysis->block_size);//释放块的内存空间
		p_analysis->block_buf = NULL;
	}

	if(p_analysis->page_info)
	{
		My_Pfree(p_analysis->page_info, (sizeof(__ebook_one_page_t) * EBOOK_MAX_SHOW_PAGE + 1023) / 1024);
		p_analysis->page_info = NULL;
		p_analysis->p_operate = NULL;
	}

	My_Mfree(0, p_analysis);
	return EPDK_OK;
}






































