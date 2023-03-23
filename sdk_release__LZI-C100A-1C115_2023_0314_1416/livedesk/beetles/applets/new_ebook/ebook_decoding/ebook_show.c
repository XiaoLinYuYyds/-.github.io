#include "ebook_decod_i.h"



typedef struct tag_EBOOK_SHOW
{
	ES_FILE					*disp_hdle;	//显示驱动操作句柄
	__ebook_show_config_t	config;		//配置信息

}__ebook_show_t;



/*
**********************************************************************************************************************
*                                               __ebook_show_page_bgk
*
* Description: 写一页到软图层上（GBK编码格式）,显示一页电子书内容
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_show_page_bgk(__ebook_show_t *hdle, __hdle lyr, void *data)
{
	__u16			tmp;	//存放电子书字体
	__u32			tmp_w;	//存放列宽
	__u32			x,y;	//存放页的起始位置
	__u32			row;	//存放页的行数
	__u8			*p_data;
	__ebook_show_t			*p_show;
	__ebook_show_config_t	*p_config;
	p_data = data;
	p_show = hdle;
	p_config = &p_show->config;
	__wrn("__ebook_show_page_bgk is ...\n");
	__wrn("lyr = %d...\n",lyr);
	GUI_LyrWinSel(lyr);					//选择显示图层
	__wrn("1...\n");
	GUI_SetFont(p_config->char_font);	//设置文本部件
	__wrn("2...\n");
	GUI_SetBkColor(0);					//设置部件背景颜色
	__wrn("3...\n");
	GUI_SetColor(p_config->font_color);	//设置文本颜色
	__wrn("4...\n");
	GUI_Clear();
	__wrn("5...\n");
	x = p_config->show_start + EBOOK_SHOW_START;	//起始位置，x轴坐标
	y = 0 + EBOOK_SHOW_WIDTH;						//起始位置，y轴坐标
	row = 0;

	while(row < p_config->page_row)//一页显示的行数,页面还有剩余行数时
	{
		tmp = *p_data & 0xff;//按位与
		__wrn("tmp = 0x%x...\n",tmp);
		__wrn("6...\n");
		if((tmp == 0x0d) || (tmp == 0x0a))
		{
			row++;
			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;

			if((*(p_data + 1) & 0xff) == 0x0a){
				p_data = p_data + 2;
			} else {
				p_data = p_data + 1;
			}

			continue;//while循环里面的跳转语句
		}
		else if(tmp == 0xff)
		{
			if((*(p_data + 1) & 0xff) == 0xfe){
				break;
			} else {
				tmp_w = *(p_config->char_table + 0x3f);//列宽
				p_data = p_data + 1;
			}
		}
		else if(tmp > 0x7f)
		{
			tmp = (tmp << 8) + *(p_data + 1);
			{
				__u16 t;
				t = tmp;
				tmp = eLIBs_GBK_to_Unicode(t);//GBK格式

				if(0xffff == tmp)
				{
					__wrn("t = %x\n",t);
				}
			}
			tmp_w  = p_config->col_width;//列宽
			p_data = p_data + 2;
		}
		else
		{
			tmp_w = *(p_config->char_table + tmp);//列宽
			p_data = p_data + 1;
		}

		if((x + tmp_w) > p_config->show_width)//显示宽
		{
			row++;
			
			if(row >= p_config->page_row){
				break;
			}

			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;
			GUI_DispCharAt(tmp, x, y);//显示字符串
			x = x + tmp_w;
		} 
		else 
		{
			GUI_DispCharAt(tmp, x, y);//显示字符串
			x = x + tmp_w;
		}
		__wrn("tmp = %s...\n",tmp);
	}
}

/*
************************************************************************************************************************
*                       				MBOOK_Show_Init
*
*Description: 页面显示模块的初始化
*
*Arguments  :
*
*
*Return     : H_SHOW: 返回页面显示模块操作句柄
*
************************************************************************************************************************
*/
H_EBOOK_SHOW EBOOK_Show_Init(void)
{
	__ebook_show_t	*p_show;
	p_show = (__ebook_show_t *)My_Malloc(0, sizeof(__ebook_show_t));//申请内存空间

	if(p_show == 0){
		__wrn("p_show malloc error...\n");
		return NULL;
	}

	eLIBs_memset( p_show, 0, sizeof(__ebook_show_t));
	p_show->disp_hdle = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");//打开显示驱动文件，"r+":打开文本文件进行更新(读写)

	if(p_show->disp_hdle == NULL){//打开显示驱动文件失败
		__wrn("disp_hdle is null...\n");
		goto EBOOK_SHOW_ERROR_1;//跳转到下面的标志进行释放内存空间
	}

	return p_show;
SHOW_ERROR_2:
	eLIBs_fclose(p_show->disp_hdle);
	p_show->disp_hdle = NULL;

EBOOK_SHOW_ERROR_1:
	My_Mfree(0, p_show);//释放内存空间
	return NULL;
}

/*
************************************************************************************************************************
*                       				EBOOK_Show_Config
*
*Description: 页面显示模块的参数配置
*
*Arguments  : hdle: 操作句柄
*             config: 配置信息参数指针
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32 EBOOK_Show_Config(H_EBOOK_SHOW hdle, __ebook_show_config_t *config)
{
	__ebook_show_t	*p_show;

	if(hdle == NULL){
		return EPDK_FAIL;
	}
	__wrn("EBOOK_Show_Config is entrance...\n");
	p_show = (__ebook_show_t *)hdle;
	eLIBs_memcpy(&(p_show->config), config, sizeof(__ebook_show_t));
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				EBOOK_Show_Page
*
*Description: 写一页数据到指定的软图层上，根据文件编码格式显示电子书文件内容
*
*Arguments  : hdle: 操作句柄
*             lyr: 图层句柄
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32 EBOOK_Show_Page(H_EBOOK_SHOW hdle, __hdle lyr, void *data)
{
	__ebook_show_t	*p_show;
	__s32 charset;
	__wrn("EBOOK_Show_Page...\n");
	if(hdle == NULL){
		__wrn("hdle is null...\n");
		return EPDK_FAIL;
	}

	p_show = (__ebook_show_t *)hdle;
	__wrn("p_show = %d...\n",p_show);
	__wrn("EBOOK_Show_Page 1...\n");
	__wrn("config.charset = %d...\n",p_show->config.charset);
	charset = p_show->config.charset;
	//switch(p_show->config.charset){//文件编码格式
	/*switch(charset){//文件编码格式
		__wrn("config.charset = %d...\n",charset);
		case EPDK_CHARSET_ENM_GBK:{
			__ebook_show_page_bgk(hdle, lyr, data);//显示BGK编码格式
			__wrn("ebook file format is GBK...\n");
		}
		break;

		default:
			__wrn("EBOOK_Show_Page charset is null...\n");
			break;
	}*/

	switch(p_show->config.charset)
	{
		case EPDK_CHARSET_ENM_UTF8 :
		{
			//__show_page_utf8(p_show, lyr, data);
		}
		break;

		case EPDK_CHARSET_ENM_UTF16BE:
		{
			//__show_page_utf16_be(p_show, lyr, data);
		}
		break;

		case EPDK_CHARSET_ENM_UTF16LE:
		{
			//__show_page_utf16_le(p_show, lyr, data);
		}
		break;

		case EPDK_CHARSET_ENM_GBK:
		{
			__wrn("EPDK_CHARSET_ENM_GBK...\n");
			__ebook_show_page_bgk(hdle, lyr, data);//显示BGK编码格式
			//__show_page_gbk(p_show, lyr, data);
		}
		break;

		case EPDK_CHARSET_ENM_BIG5:
		{
			//__show_page_big5(p_show, lyr, data);
		}
		break;

		default:
			break;
	}

	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				EBOOK_Show_Uninit
*
*Description: 删除页面显示模块
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Show_Uninit(H_EBOOK_SHOW hdle)
{
	__ebook_show_t          *p_show;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_show = (__ebook_show_t *)hdle;

	if(p_show->disp_hdle)//显示驱动文件句柄存在
	{
		eLIBs_fclose(p_show->disp_hdle);//关闭显示驱动文件句柄
		p_show->disp_hdle = NULL;
	}

	My_Mfree(0, p_show);//释放结构体内存空间
	return EPDK_OK;
}

































