#include "ebook_decod_i.h"


/*
************************************************************************************************************************
*                       				__ebook_analysis_page_gbk
*
*Description: 对页里的数据，进行分析，得到单个页面信息。
*
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
	__u32			tmp;
	__u32			tmp_ofs;
	__u32			tmp_row;//一页可以显示的行数
	__u32			tmp_x;
	__u32			tmp_w;
	__u8			*p_data;//存放block块的内存地址
	__ebook_one_page_t			*p_one;		//存放指向当前操作页面的页信息info
	__ebook_analysis_config_t	*p_config;	//一页的配置信息

	tmp_ofs = 0;
	tmp_row = hdle->last_row;
	tmp_x   = hdle->last_start;
	p_data  = hdle->block_buf;
	p_one   = hdle->p_operate;
	p_config = &hdle->config;

	while(1)
	{
		tmp = *(p_data + tmp_ofs) & 0xff;

		if((tmp == 0x0d) || (tmp == 0x0a))
		{
			tmp_row++;//行数自增
			tmp_x = 0;//x轴坐标0开始

			if((*(p_data + tmp_ofs +1) & 0xff) == 0x0a){
				tmp_ofs = tmp_ofs + 2;
			}else{
				tmp_ofs = tmp_ofs + 1;
			}

			if(tmp_row >= p_config->page_row)				//一页显示不完，如果还有剩余行数
			{
				hdle->total_page++;							//总页码自增
				p_one->page_end = hdle->last_end + tmp_ofs;	//结束位置 文件偏移量
				p_one->page_no  = hdle->total_page;			//页号
				(p_one + 1)->page_start = p_one->page_end;	//起始位置 文件偏移量
				p_one++;
				tmp_row = 0;
			}
		}
		else if(tmp >0x7f)
		{
			tmp_w   = p_config->col_width;			//列与列之间的宽；行偏移相对起始位置1个汉字偏移量
			tmp_ofs = tmp_ofs + 2;
		}
		else
		{
			tmp_w   = *(p_config->char_table + tmp);//行偏移相对起始位置1个字符偏移量
			tmp_ofs = tmp_ofs + 1;
		}

		if((tmp_x + tmp_w) > p_config->show_width)	//大于文本显示的宽
		{
			tmp_row++;
			tmp_x = tmp_w;

			if(tmp_row >= p_config->page_row)				//页的显示行数
			{
				hdle->total_page++;							//总页码自增
				p_one->page_end = hdle->last_end + tmp_ofs;	//结束位置 文件偏移量
				p_one->page_no  = hdle->total_page;			//页号
				(p_one + 1)->page_start = p_one->page_end;	//起始位置 文件偏移量
				p_one++;
				tmp_row = 0;
			}
		} else {
			tmp_x = tmp_x + tmp_w;
		}

		if((tmp_ofs + 4) >= len){
			break;
		}
	}

	hdle->last_end   = hdle->last_end + tmp_ofs;
	hdle->p_operate  = p_one;
	hdle->last_start = tmp_row;
	hdle->last_row   = tmp_x;
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__ebook_analysis_page_info_gbk
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
static __s32 __ebook_analysis_page_info_gbk(__ebook_analysis_t *hdle)
{
	__u32				len;
	__ebook_analysis_t	*p_analysis;

	while(1)
	{
		//定位文件指针
		eLIBs_fseek(p_analysis->file_hdle, p_analysis->last_end, ELIBS_SEEK_SET);					//文件位置的偏移量；函数用来移动文件位置指针到指定的位置上
		//读一段数据
		len = eLIBs_fread(p_analysis->block_buf, 1, p_analysis->block_len, p_analysis->file_hdle);	//读取文件句柄的数据，然后存放到块block_buf储存区域
		__ebook_analysis_page_gbk(hdle, len);

		if(len < p_analysis->block_len)
		{
			p_analysis->total_page++;												//页码自增
			p_analysis->p_operate->page_no  = p_analysis->total_page;				//页号
			p_analysis->p_operate->page_end = eLIBs_ftell(p_analysis->file_hdle); 	//结束位置 文件偏移量
			break;
		}

	}

	p_analysis->p_operate = p_analysis->page_info;
	return EPDK_OK;
}

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
	p_analysis->block_size = 1 * 1024;	//单位：KB
	p_analysis->block_len  = 1 * 1024 *1024;
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

	switch(p_analysis->config.charset)
	{
		case EPDK_CHARSET_ENM_UTF8 :
		{
			p_analysis->p_operate->page_start = 3;
			p_analysis->last_end    = 3;
		}
		break;

		case EPDK_CHARSET_ENM_UTF16BE:
		case EPDK_CHARSET_ENM_UTF16LE:
		{
			p_analysis->p_operate->page_start = 2;
			p_analysis->last_end    = 2;
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

	switch(p_analysis->config.charset)
	{
		case EPDK_CHARSET_ENM_GBK:
		{
			__ebook_analysis_page_info_gbk(p_analysis);//分析当前页面信息
		}
		break;

		default:
			break;
	}

	return EPDK_OK;
}












































