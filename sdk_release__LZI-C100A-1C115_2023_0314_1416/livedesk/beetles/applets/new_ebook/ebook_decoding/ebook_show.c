#include "ebook_decod_i.h"



typedef struct tag_EBOOK_SHOW
{
	ES_FILE					*disp_hdle;	//显示驱动操作句柄
	__ebook_show_config_t	config;		//配置信息

}__ebook_show_t;


#if 1 //将文本文字数据内容显示到页面图层
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
	#if 1
	__u16                   tmp;
	__u32                   tmp_w;
	__u32                   x, y;
	__u32                   row;
	__u8                   *p_data;
	__ebook_show_t         *p_show;
	__ebook_show_config_t        *p_config;
	p_data = data;
	p_show = hdle;
	p_config = &p_show->config;
	GUI_LyrWinSel(lyr);
	GUI_SetFont(p_config->char_font);
	GUI_SetBkColor(0);
	GUI_SetColor(p_config->font_color);
	GUI_Clear();
	x = p_config->show_start + EBOOK_SHOW_START;
	y = 0 + EBOOK_SHOW_WIDTH;
	row = 0;
	__wrn("p_config->page_row = %d...\n",p_config->page_row);
	while(row < p_config->page_row)
	{
		tmp = *p_data & 0xff;

		if((tmp == 0x0d) || (tmp == 0x0a))//相当于文本内容里面的回车按下换行标志；换行符：0x0a;回车换行符：0x0d 0x0a；
		{
			row++;
			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;

			if((*(p_data + 1) & 0xff) == 0x0a)//回车换行符
			{
				p_data = p_data + 2;//文件指针往后移动两个字节
			}
			else//只有换行符
			{
				p_data = p_data + 1;//文本指针往后移动1个字节
			}

			continue;
		}
		else if(tmp == 0xff)//文件的结束标识符:0xff
		{
			if((*(p_data + 1) & 0xff) == 0xfe)
			{
				break;
			}
			else
			{
				tmp_w = *(p_config->char_table + 0x3f);     // ?
				p_data = p_data + 1;
			}
		}
		else if(tmp > 0x7f)
		{
			tmp = (tmp << 8) + *(p_data + 1);
			{
				__u16 t;
				t = tmp;
				tmp = eLIBs_GBK_to_Unicode(t);//GBK统一编码格式
				__wrn("eLIBs_GBK_to_Unicode t = %x\n", t);
				if(0xffff == tmp)
				{
					__wrn("eLIBs_GBK_to_Unicode fail, t = %x\n", t);
				}
			}
			tmp_w = p_config->col_width;
			p_data = p_data + 2;
		}
		else
		{
			tmp_w = *(p_config->char_table + tmp);
			p_data = p_data + 1;
		}
		//__wrn("p_config->show_width = %x\n",p_config->show_width);

		if((x + tmp_w) > p_config->show_width)
		{
			row++;

			if(row >= p_config->page_row)
			{
				__wrn("row = %d\n", row);
				break;
			}
			__wrn("y=%d, x=%d\n", y, x);

			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
		else
		{
			__wrn("x = %d\n", x);
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
	}
	#endif
}

/*
**********************************************************************************************************************
*                                               __ebook_show_page_utf8 有BOM格式
*
* Description: 写一页到软图层上（UTF8编码格式）
*
* Arguments  : hdle:显示句柄；lyr:指定的软图层句柄；data:需要写入并显示文本内容数据
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_show_page_utf8(__ebook_show_t *hdle, __hdle lyr, void *data)
{
	__u16                   tmp;
	__u32                   tmp_w;
	__u32                   x, y;
	__u32                   row;
	__u8                   *p_data;
	__ebook_show_t         *p_show;
	__ebook_show_config_t        *p_config;
	p_data = data;
	p_show = hdle;
	p_config = &p_show->config;
	GUI_LyrWinSel(lyr);
	//	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(p_config->char_font);
	//	GUI_SetFontMode(GUI_FONTMODE_8BPP256);
	GUI_SetBkColor(0x0);
	GUI_SetColor(p_config->font_color);
	GUI_Clear();
	x = p_config->show_start + EBOOK_SHOW_START;
	y = 0 + EBOOK_SHOW_WIDTH;
	row = 0;

	while(row < p_config->page_row)
	{
		if((*p_data == 0x0a) || (*p_data == 0x0d))//相当于文本内容里面的回车按下换行标志；换行符：0x0a;回车换行符：0x0d 0x0a；
		{
			//换行符检查
			row++;
			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;

			if(*(p_data + 1) == 0x0a)
			{
				p_data = p_data + 2;
			}
			else
			{
				p_data = p_data + 1;
			}

			continue;
		}
		else if((tmp == 0xff) && ((*(p_data + 1) & 0xff) == 0xfe))
		{
			break;
		}
		else if((*p_data & 0xf0) == 0xe0)//首字节1110开头，表示该序列3个字节，主要用于检查值的前两位；除首字节之外后续字节都是以二进制10开头
		{
			// 三字节流数据
			tmp = ((*p_data & 0x0f) << 12) | ((*(p_data + 1) & 0x3f) << 6) | (*(p_data + 2) & 0x3f);
			tmp_w = p_config->col_width;
			p_data = p_data + 3;//文本数据指针往后移动3字节，+3
		}
		else if((*p_data & 0xe0) == 0xc0)//首字节1100开头，表示该序列2个字节，后面跟着的数据也是1100开头；除首字节之外后续字节都是以二进制10开头
		{
			// 双字节流数据
			tmp = ((*p_data & 0x3f) << 6) | (*(p_data + 1) & 0x3f);
			tmp_w = p_config->col_width;
			p_data = p_data + 2;//文件指针往后移动两个字节，+2
		}
		else//首字节0开头，表示序列1给字节。
		{
			// 单字节流数据
			tmp = *p_data & 0xff;
			tmp_w = *(p_config->char_table + tmp);
			p_data = p_data + 1;//文件指针往后移动1个字节，+1
		}

		if((x + tmp_w) > p_config->show_width)
		{
			row++;

			if(row >= p_config->page_row)
			{
				break;
			}

			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
		else
		{
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
	}
}

/*
**********************************************************************************************************************
*                                               __ebook_show_page_utf16_le
*
* Description: 写一页到软图层上（UTF16_LE编码格式）
*
* Arguments  : hdle:显示句柄；lyr:指定的软图层句柄；data:需要写入并显示文本内容数据
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_show_page_utf16_le(__ebook_show_t *hdle, __hdle lyr, void *data)
{
	__u16                   tmp;
	__u32                   tmp_w;
	__u32                   x, y;
	__u32                   row;
	__u8                   *p_data;
	__ebook_show_t         *p_show;
	__ebook_show_config_t        *p_config;
	p_data = data;
	p_show = hdle;
	p_config = &p_show->config;
	GUI_LyrWinSel(lyr);
	//	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(p_config->char_font);
	//	GUI_SetFontMode(GUI_FONTMODE_8BPP256);
	GUI_SetBkColor(0x0);
	GUI_SetColor(p_config->font_color);
	GUI_Clear();
	x = p_config->show_start + EBOOK_SHOW_START;
	y = 0 + EBOOK_SHOW_WIDTH;
	row = 0;

	while(row < p_config->page_row)
	{
		tmp = (*p_data & 0xff) + ((*(p_data + 1) & 0xff) << 8);

		if((tmp == 0x0d) || (tmp == 0x0a))
		{
			row++;
			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;

			if((*(p_data + 2) & 0xff) == 0x0a)
			{
				p_data = p_data + 4;
			}
			else
			{
				p_data = p_data + 2;
			}

			continue;
		}
		else if(tmp == 0xfeff)//文件开头包含字节数据，为UTF16-BE时，直接退出循环
		{
			break;
		}
		else if(tmp < 0x80)//ASCII码值字符
		{
			tmp_w = *(p_config->char_table + tmp);
			p_data = p_data + 2;//文本数据字节+2
		}
		else//汉字
		{
			tmp_w = p_config->col_width;
			p_data = p_data + 2;//文本数据字节+2
		}

		if((x + tmp_w) > p_config->show_width)
		{
			row++;

			if(row >= p_config->page_row)
			{
				break;
			}

			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
		else
		{
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
	}
}

/*
**********************************************************************************************************************
*                                               __ebook_show_page_utf16_be
*
* Description: 写一页到软图层上（UTF16_BE编码格式）
*
* Arguments  : hdle:显示句柄；lyr:指定的软图层句柄；data:需要写入并显示文本内容数据
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_show_page_utf16_be(__ebook_show_t *hdle, __hdle lyr, void *data)
{
	__u16                   tmp;
	__u32                   tmp_w;
	__u32                   x, y;
	__u32                   row;
	__u8                   *p_data;
	__ebook_show_t         *p_show;
	__ebook_show_config_t        *p_config;
	p_data = data;
	p_show = hdle;
	p_config = &p_show->config;
	GUI_LyrWinSel(lyr);
	//	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(p_config->char_font);
	//	GUI_SetFontMode(GUI_FONTMODE_8BPP256);
	GUI_SetBkColor(0x0);
	GUI_SetColor(p_config->font_color);
	GUI_Clear();
	x = p_config->show_start + EBOOK_SHOW_START;
	y = 0 + EBOOK_SHOW_WIDTH;
	row = 0;

	while(row < p_config->page_row)
	{
		tmp = ((*p_data & 0xff) << 8) + (*(p_data + 1) & 0xff);

		if((tmp == 0x0d) || (tmp == 0x0a))
		{
			row++;
			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;

			if((*(p_data + 3) & 0xff) == 0x0a)//回车换行符号
			{
				p_data = p_data + 4;//文本数据+4字节
			}
			else//只存在换行符号
			{
				p_data = p_data + 2;//文本数据+2字节
			}

			continue;
		}
		else if(tmp == 0xfffe)//文件开头包含字节数据，为UTF16-BE时，直接退出循环
		{
			break;
		}
		else if(tmp < 0x80)//文本数据为ASCII码值字符
		{
			tmp_w = *(p_config->char_table + tmp);
			p_data = p_data + 2;//数据+2字节
		}
		else//文本数据为汉字
		{
			tmp_w = p_config->col_width;
			p_data = p_data + 2;//数据+2字节
		}

		if((x + tmp_w) > p_config->show_width)
		{
			row++;

			if(row >= p_config->page_row)
			{
				break;
			}

			x = p_config->show_start + EBOOK_SHOW_START;
			y = y + p_config->row_height;
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
		else
		{
			GUI_DispCharAt(tmp, x, y);
			x = x + tmp_w;
		}
	}
}

#endif

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
*Arguments  : hdle: 显示操作句柄
*             lyr: 	指定的图层句柄
*			  data：传入解码成功的文件数据内容
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

	switch(p_show->config.charset)
	{
		case EPDK_CHARSET_ENM_UTF8 :	//检测到文件为UTF8编码格式
		{
			__wrn("EPDK_CHARSET_ENM_UTF8 have BOM...\n");
			__ebook_show_page_utf8(p_show, lyr, data);		//显示UTF8编码格式
		}
		break;

		case EPDK_CHARSET_ENM_UTF16BE:	//检测到文件为UTF16LE编码格式
		{
			__wrn("EPDK_CHARSET_ENM_UTF16BE...\n");
			__ebook_show_page_utf16_be(p_show, lyr, data);	//显示UTF16BE编码格式
		}
		break;

		case EPDK_CHARSET_ENM_UTF16LE:	//检测到文件为UTF16BE编码格式
		{
			__wrn("EPDK_CHARSET_ENM_UTF16LE...\n");
			__ebook_show_page_utf16_le(p_show, lyr, data);	//显示UTF16LE编码格式
		}
		break;

		case EPDK_CHARSET_ENM_GBK:		//检测到文件为GBK编码格式
		{
			__wrn("EPDK_CHARSET_ENM_GBK...\n");
			__ebook_show_page_bgk(hdle, lyr, data);			//显示BGK编码格式
		}
		break;

		case EPDK_CHARSET_ENM_BIG5:		//检测到文件为BIG5编码格式
		{

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

































