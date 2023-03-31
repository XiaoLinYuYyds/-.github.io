#include "ebook_decod.h"
#include "ebook_decod_i.h"


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

typedef struct tag_EBOOK_DECODE
{
	char				    file_path[MAX_FILE_NAME_SIZE];  // 文件路径
	ES_FILE                *file_hdle;                      // 文件句柄
	H_EBOOK_ANALYSIS         analysis_hdle;                  // 文件分析句柄
	H_EBOOK_SHOW                  show_hdle;                      // 页面显示句柄

	H_EBOOK_FONT                  font_hdle;                      // 字体操作句柄
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

static void __ebook_decode_read_page(__ebook_decode_t *hdle);
static void __ebook_decode_brush_page(__ebook_decode_t *hdle, __newdecode_mode_e mode);
#if 1
/*
**********************************************************************************************************************
*                                               __decode_task
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_decode_task(void *p_arg)
{
	__u8 			    err;
	__ebook_decode_t	*p_decode;
	p_decode = (__ebook_decode_t *)p_arg;

	while(1)
	{
		if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
		{
			goto EXIT_DEC_TASK;
		}
		//__wrn("decode_sta is start...\n");
		esKRNL_TimeDly(5);//5*10ms
		esKRNL_SemPend(p_decode->decode_sem, 0, &err);

		if(p_decode->decode_sta != DECODE_NULL)
		{
			if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
			    && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) >= 1)
			    && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) <= p_decode->total_page))
			{
				__wrn("decode_sta ...\n");
				__ebook_decode_read_page(p_decode);//读取一页数据到缓存BUF里。
			}

			p_decode->decode_cnt++;

			if(p_decode->decode_cnt == DECODE_EBOOK_NUM)
			{
				__msg("decode_cnt = %d ...\n",p_decode->decode_cnt);
				p_decode->decode_cnt = 0;
			}
		}

		esKRNL_SemPost(p_decode->decode_sem);
	}

EXIT_DEC_TASK:
	esKRNL_TDel(EXEC_prioself);
	return;
}

/*
**********************************************************************************************************************
*                                               __decode_read_page
*
* Description: 读取一页数据到缓存BUF里。
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_decode_read_page(__ebook_decode_t *hdle)
{
	__u32               size;
	__ebook_one_page_t        tmp_page;
	__ebook_decode_t	*p_decode;
	p_decode = hdle;
	// 从文件中读取数据
	EBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2, &tmp_page);
	__wrn("start = %d, end = %d, page = %d \n", tmp_page.page_start, tmp_page.page_end, tmp_page.page_no);
	size = tmp_page.page_end - tmp_page.page_start;
	eLIBs_fseek(p_decode->file_hdle, tmp_page.page_start, ELIBS_SEEK_SET);	//函数用来移动文件位置指针到指定的位置上，作用是读取文本时一个一个读取文本内容的位置
	eLIBs_fread(p_decode->page_data, 1, size, p_decode->file_hdle);			//从文件读二进制数据
	*(p_decode->page_data + size) = 0xff;				//读取到1个字节 赋值为0xff，
	*(p_decode->page_data + size + 1) = 0xfe;			//读取到的第二个字节，赋值为0xfe;则两个结合相当于是设置为UTF16-LE编码格式
	__wrn("page_data = %x ...\n",p_decode->page_data);
	__wrn("size= %d ...\n",size);
	__wrn("show_rotote = %d ...\n",p_decode->config.show_rotate);
	// 将读取的数据写到软图层上
	if(p_decode->config.show_rotate == 0)//横屏方向
	{
		EBOOK_Layer_Rotate(p_decode->page[p_decode->decode_cnt].page_lyr, p_decode->config.show_width,
		                   p_decode->config.show_height - p_decode->config.bottom_width, 0);
	}
	else//竖屏显示
	{
		EBOOK_Layer_Rotate(p_decode->page[p_decode->decode_cnt].page_lyr,
		                   p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1);
	}
	__wrn("page = %d, page_lyr = %d ...\n", p_decode->decode_cnt, p_decode->page[p_decode->decode_cnt].page_lyr);
	
	p_decode->page[p_decode->decode_cnt].page_sta = 1;	//解码成功，页面状态置1
	p_decode->page[p_decode->decode_cnt].page_no  = p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2;//页面号
	__wrn("page_lyr show page is start ...\n");
	EBOOK_Show_Page(p_decode->show_hdle, p_decode->page[p_decode->decode_cnt].page_lyr, p_decode->page_data);//显示当前页数据到页面图层
	__wrn("page_lyr show page is stop ...\n");
}

/*
**********************************************************************************************************************
*                                               __decode_get_charset
*
* Description: 获取当前文件的编码格式
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void	__ebook_decode_get_charset(__ebook_decode_t *hdle)
{
	char		buf[3];
	__ebook_decode_t	*p_decode;
	p_decode = hdle;
	eLIBs_fseek(p_decode->file_hdle, 0, SEEK_SET);			//文件位置移动
	eLIBs_fread(buf, 3, sizeof(char), p_decode->file_hdle);	//读取文件二进制数据
	__wrn("__ebook_decode_get_charset buf[0] = %x, buf[1] = %x, buf[2] = %x...\n", buf[0], buf[1], buf[2]);
	if((buf[0] == 0xff) && (buf[1] == 0xfe)) {			//获取文件流前两个字节，分别为0xff、0xfe表示UTF16LE编码格式文件
		p_decode->charset = EPDK_CHARSET_ENM_UTF16LE;	//重新设置解码的编码格式
	}
	else if((buf[0] == 0xfe) && (buf[1] == 0xff)) {		//读取到文件流前两个字节，分别为0xfe、0xff表示UTF16BE编码格式文件
		p_decode->charset = EPDK_CHARSET_ENM_UTF16BE;
	}
	else if((buf[0] == 0xe7) && (buf[1] == 0xac)) {		//BIG5编码格式
		p_decode->charset = EPDK_CHARSET_ENM_BIG5;
	}
	else if((buf[0] == 0xef) && (buf[1] == 0xbb) && (buf[2] == 0xbf)) {//读取到文件流前三个字节，表示UTF8有BOM编码格式
		p_decode->charset = EPDK_CHARSET_ENM_UTF8;
		__wrn("get UTF8 have bom charset...\n");
	}
	/*else if((buf[0] == 0x30) && (buf[2] == 0xe3)) {		//读取到文件流前三个字节，使用第1和第三个字节，表示UTF8无BOM编码格式
		p_decode->charset = EPDK_CHARSET_ENM_UTF8;
		__wrn("get UTF8 not bom charset...\n");
	}*/
	else {//其他编码格式
		p_decode->charset = p_decode->default_charset;//默认GBK编码格式
	}

}

/*
**********************************************************************************************************************
*                                               __decode_get_page
*
* Description: 获得一个已解好的页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_decode_get_page(__ebook_decode_t *hdle, __newdecode_mode_e mode)
{
	__u8 			    err;
	__ebook_decode_t	*p_decode;
	p_decode = hdle;
	// 等待、悬挂解码信号量decode_sem。
	esKRNL_SemPend(p_decode->decode_sem, 0, &err);
	__wrn("__ebook_decode_get_page is start mode = %d...\n",mode);
	// 刷新页面缓冲池
	if(mode == DECODE_CURR)
	{
		__ebook_decode_brush_page(p_decode, DECODE_CURR);//刷新页面数据
	}
	else if(mode == DECODE_PREV)
	{
		p_decode->current_page--;
		__ebook_decode_brush_page(p_decode, DECODE_PREV);//刷新上一页
	}
	else if(mode == DECODE_NEXT)
	{
		p_decode->current_page++;
		__ebook_decode_brush_page(p_decode, DECODE_NEXT);//刷新下一页
	}

	// 判断当前页解码是否完成，未完成则立刻解当前页。
	p_decode->decode_cnt = DECODE_EBOOK_NUM / 2;

	if((p_decode->page[p_decode->decode_cnt].page_sta == 0)//如果解码失败
	    && ((p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) >= 1)
	    && ((p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) <= p_decode->total_page))
	{
		__wrn("__ebook_decode_get_page ...\n");
		__ebook_decode_read_page(p_decode);//读取解码页数据
	}

	p_decode->decode_sta = mode;
	// 释放解码信号量decode_sem。
	esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_get_next_page
*
* Description: 获得当前页的下一页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_decode_get_next_page(__ebook_decode_t *hdle)
{
	__u8 			    err;
	__ebook_decode_t	*p_decode;
	p_decode = hdle;
	esKRNL_SemPend(p_decode->decode_sem, 0, &err);//挂起线程信号量
	p_decode->decode_cnt = DECODE_EBOOK_NUM / 2 + 1;

	if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
	    && ((p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) > 0))
	{
		__ebook_decode_read_page(p_decode);//读取解码页数据
	}

	esKRNL_SemPost(p_decode->decode_sem);//恢复线程信号量
}

/*
**********************************************************************************************************************
*                                               __decode_show_page
*
* Description: 读取一页数据到缓存BUF里。显示一页数据
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_decode_show_page(__ebook_decode_t *hdle)
{
	__s32               i, tmp;
	__u8               *pdes;
	__u8               *psrc1;
	__u8               *psrc2;
	FB                  fb_des;//显示图层，大小
	FB                  fb_src1;//页面1图层，大小
	FB                  fb_src2;//页面2图层，大小
	__ebook_decode_t	*p_decode;
	p_decode = hdle;
	__wrn("__decode_show_page...\n");
	__wrn("config.show_rotate = %d\n",p_decode->config.show_rotate);

	if(p_decode->config.show_rotate == 0)
	{
		GUI_LyrWinGetFB(p_decode->page[DECODE_EBOOK_NUM / 2    ].page_lyr, &fb_src1);
		GUI_LyrWinGetFB(p_decode->page[DECODE_EBOOK_NUM / 2 + 1].page_lyr, &fb_src2);
		GUI_LyrWinGetFB(p_decode->show_lyr, &fb_des);
		eLIBs_memcpy(fb_des.addr[0], (void *)((__u32)fb_src1.addr[0] + p_decode->move_height * fb_des.size.width),
		             fb_des.size.width * (fb_des.size.height - p_decode->move_height));
		eLIBs_memcpy((void *)((__u32)fb_des.addr[0] + (fb_des.size.height - p_decode->move_height) * fb_des.size.width), fb_src2.addr[0],
		             fb_des.size.width * p_decode->move_height);
	}
	else
	{
		GUI_LyrWinGetFB(p_decode->page[DECODE_EBOOK_NUM / 2    ].page_lyr, &fb_src1);
		GUI_LyrWinGetFB(p_decode->page[DECODE_EBOOK_NUM / 2 + 1].page_lyr, &fb_src2);
		GUI_LyrWinGetFB(p_decode->show_lyr, &fb_des);
		tmp     = fb_des.size.width - p_decode->move_height;
		pdes    = fb_des.addr[0];
		psrc1   = fb_src1.addr[0];
		psrc2   = fb_src2.addr[0];
		psrc1   = psrc1 + p_decode->move_height;

		for(i = 0; i < fb_des.size.height; i++)
		{
			eLIBs_memcpy(pdes, psrc1, tmp);
			pdes    = pdes + tmp;
			psrc1   = psrc1 + fb_des.size.width;
			eLIBs_memcpy(pdes, psrc2, p_decode->move_height);
			pdes    = pdes + p_decode->move_height;
			psrc2   = psrc2 + fb_des.size.width;
		}
	}
	__wrn("tmp = %d, fb_src1.h = %d...\n", tmp, fb_src1.size.height);
	__wrn("fb_src1.w = %d, fb_src1.h = %d...\n", fb_src1.size.width, fb_src1.size.height);
	__wrn("fb_src2.w = %d, fb_src2.h = %d...\n", fb_src2.size.width, fb_src2.size.height);
	__wrn("fb_des.w = %d, fb_des.h = %d...\n", fb_des.size.width, fb_des.size.height);
		
}

/*
**********************************************************************************************************************
*                                               __decode_brush_page
*
* Description: 刷新页面缓存里的信息
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __ebook_decode_brush_page(__ebook_decode_t *hdle, __newdecode_mode_e mode)
{
	__u32               i;
	__newpage_cache_t      tmp_page;
	__ebook_decode_t	*p_decode;
	p_decode = hdle;
	__wrn("mode = %d\n",mode);

	if(mode == DECODE_PREV)//解码上一页
	{
		tmp_page.page_no	= p_decode->page[DECODE_EBOOK_NUM - 1].page_no;
		tmp_page.page_lyr	= p_decode->page[DECODE_EBOOK_NUM - 1].page_lyr;

		for(i = DECODE_EBOOK_NUM - 1; i > 0; i--)
		{
			p_decode->page[i].page_sta	= p_decode->page[i - 1].page_sta;
			p_decode->page[i].page_no	= p_decode->page[i - 1].page_no;
			p_decode->page[i].page_lyr	= p_decode->page[i - 1].page_lyr;
		}

		p_decode->page[0].page_sta	= 0;
		p_decode->page[0].page_no	= tmp_page.page_no;
		p_decode->page[0].page_lyr	= tmp_page.page_lyr;
	}
	else if(mode == DECODE_NEXT)//解码下一页
	{
		tmp_page.page_no	= p_decode->page[0].page_no;
		tmp_page.page_lyr	= p_decode->page[0].page_lyr;

		for(i = 0; i < DECODE_EBOOK_NUM - 1; i++)
		{
			p_decode->page[i].page_sta	= p_decode->page[i + 1].page_sta;
			p_decode->page[i].page_no	= p_decode->page[i + 1].page_no;
			p_decode->page[i].page_lyr	= p_decode->page[i + 1].page_lyr;
		}

		p_decode->page[DECODE_EBOOK_NUM - 1].page_sta	= 0;
		p_decode->page[DECODE_EBOOK_NUM - 1].page_no	= tmp_page.page_no;
		p_decode->page[DECODE_EBOOK_NUM - 1].page_lyr	= tmp_page.page_lyr;
	}
	else if(mode == DECODE_CURR)//解码当前页
	{
		for(i = 0; i < DECODE_EBOOK_NUM; i++)
		{
			p_decode->page[i].page_sta = 0;//解码成功
		}
	}
}

/*
************************************************************************************************************************
*                       				__decode_analysis_config
*
*Description: 配置解码模块相关参数
*
*Arguments  : hdle： 操作句柄
*
*
*Return     : NULL
*
************************************************************************************************************************
*/
static void __ebook_decode_analysis_config(__ebook_decode_t *hdle)
{
	__ebook_one_page_t            tmp_page;
	__ebook_analysis_config_t     analysis_cfg;
	__ebook_decode_t       *p_decode;
	__newdecode_config_t      *p_config;
	p_decode = hdle;
	p_config = &p_decode->config;
	// 读取当前页的信息
	EBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page, &tmp_page);

	// 配置文件解析模块
	if(p_config->show_rotate == 0)
	{
		analysis_cfg.show_width = p_config->show_width - 2 * p_config->border_width + p_config->row_space;
		analysis_cfg.page_row   = (p_config->show_height - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
		analysis_cfg.col_width  = p_config->font_size + p_config->col_space;
		analysis_cfg.char_table = p_decode->char_table;
		analysis_cfg.charset    = p_decode->charset;
	}
	else
	{
		analysis_cfg.show_width = p_config->show_height - 2 * p_config->border_width + p_config->row_space;
		analysis_cfg.page_row   = (p_config->show_width - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
		analysis_cfg.col_width  = p_config->font_size + p_config->col_space;
		analysis_cfg.char_table = p_decode->char_table;
		analysis_cfg.charset    = p_decode->charset;
	}

	EBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
	// 启动文件分析
	EBOOK_Analysis_Work(p_decode->analysis_hdle);
	// 获取总页数
	p_decode->total_page = EBOOK_Analysis_GetTotalPage(p_decode->analysis_hdle);
	// 定位当前页
	p_decode->current_page = EBOOK_Analysis_GetPage(p_decode->analysis_hdle, tmp_page.page_start);

}

/*
************************************************************************************************************************
*                       				__decode_show_config
*
*Description: 配置绘制模块相关参数
*
*Arguments  : hdle： 操作句柄
*
*
*Return     : NULL
*
************************************************************************************************************************
*/
static void __ebook_decode_show_config(__ebook_decode_t *hdle)
{
	__ebook_show_config_t         show_cfg;
	__ebook_decode_t       *p_decode;
	__newdecode_config_t      *p_config;
	p_decode = hdle;
	p_config = &p_decode->config;
	__wrn("__decode_show_config....\n");
	__wrn("show_rotate = %d\n",p_config->show_rotate);

	// 配置数据读写模块
	if(p_config->show_rotate == 0)//横屏
	{
		show_cfg.show_start = p_config->border_width;
		show_cfg.show_width = p_config->show_width - 2 * p_config->border_width + p_config->row_space - p_config->scroll_width;
		show_cfg.page_row   = (p_config->show_height - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
		show_cfg.row_height = p_config->font_size + p_config->row_space;
		show_cfg.col_width  = p_config->font_size + p_config->col_space;
		show_cfg.font_color = p_config->font_color;
		show_cfg.back_color = p_config->back_color;
		show_cfg.char_font  = p_decode->char_font;
		show_cfg.char_table = p_decode->char_table;
		show_cfg.charset    = p_decode->charset;
	}
	else//竖屏
	{
		show_cfg.show_start = p_config->border_width;
		show_cfg.show_width = p_config->show_height - 2 * p_config->border_width + p_config->row_space - p_config->scroll_width;
		show_cfg.page_row   = (p_config->show_width - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
		show_cfg.row_height = p_config->font_size + p_config->row_space;
		show_cfg.col_width  = p_config->font_size + p_config->col_space;
		show_cfg.font_color = p_config->font_color;
		show_cfg.back_color = p_config->back_color;
		show_cfg.char_font  = p_decode->char_font;
		show_cfg.char_table = p_decode->char_table;
		show_cfg.charset    = p_decode->charset;
	}

	EBOOK_Show_Config(p_decode->show_hdle, &show_cfg);//页面显示模块的参数配置
}
#endif

/*
************************************************************************************************************************
*                       				EBOOK_Decode_Init
*
*Description: 初始化页面解码模块
*
*Arguments  : filename：文件全路径名
*
*
*Return     : H_DECODE: 返回解码操作句柄 err :0 代表无错误，1代表文件不存在 2代表其它错误
*
************************************************************************************************************************
*/
H_DECODE_NEW   EBOOK_Decode_Init(char *filename, __u8 *err, __u32 index)
{
	__ebook_decode_t	*p_decode;
	*err = 0;
	__wrn("EBOOK_Decode_Init is start...\n");
	p_decode = (__ebook_decode_t *)My_Malloc(0, sizeof(__ebook_decode_t));//申请内存空间
	__wrn("EBOOK_Decode_Init 1...\n");
	if(p_decode == 0){
		*err = 2;
		__wrn("p_decode is null...\n");
		return NULL;
	}
	eLIBs_memset(p_decode, 0, sizeof(__ebook_decode_t));
	
	if(filename == NULL){ //如果文件名为空
		__s32   npl_index;//当前播放列表的文件索引id
		HRATNPL	h_rat_npl;//rat模块的当前播放列表句柄
		__wrn("filename is null...\n");
		h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);	//打开相关的媒体类型文件,电子书媒体类型文件
		if(h_rat_npl == NULL){
			__wrn("h_rat_npl = %d is null...\n",h_rat_npl);
		}
		if(index != -1){
			rat_npl_set_cur(h_rat_npl, index);			//设置当前要播放文件的索引id
		}
		__wrn("index = %d...\n", index);
		npl_index = rat_npl_get_cur(h_rat_npl);			//获取当前播放列表的播放文件索引id
		__wrn("npl_index = %d...\n",npl_index);
		rat_npl_index2file(h_rat_npl, npl_index, p_decode->file_path);//通过文件索引id获取文件名
		__wrn("file_path = %s...\n",p_decode->file_path);
		rat_npl_close(h_rat_npl);						//关闭rat相关的媒体类型文件
	} else {
		eLIBs_memcpy(p_decode->file_path, filename, MAX_FILE_NAME_SIZE);
	}

	p_decode->file_hdle = eLIBs_fopen(p_decode->file_path, "rb");		//"rb"打开二进制文件进行读取;文件句柄获取，先获取文件路径/文件名才能执行这个文件打开API接口函数
	__wrn("EBOOK_Decode_Init 2...\n");
	if(p_decode->file_hdle == NULL){//文件句柄为空,文件不存在
		*err = 1;
		__wrn("file_hdle is null...\n");
		goto INIT_ERROR_FLAG_1;
	}

	p_decode->file_size = eLIBs_GetFileSize(p_decode->file_path);		//获取文件的大小
	p_decode->analysis_hdle = EBOOK_Analysis_Init(p_decode->file_hdle);	//文件解码分析

	if(p_decode->analysis_hdle == NULL){//文件解码分析句柄为空
		*err = 2;
		__wrn("analysis_hdle is null...\n");
		goto INIT_ERROR_FLAG_2;
	}
	
	p_decode->show_hdle = EBOOK_Show_Init();//显示初始化

	if(p_decode->show_hdle == NULL){//显示句柄为空
		*err = 2;
		__wrn("show_hdle is null...\n");
		goto INIT_ERROR_FLAG_3;
	}

	p_decode->data_len = 15;
	p_decode->page_data = (void *)My_Palloc(p_decode->data_len, 0);//申请内存

	if(p_decode->page_data == 0){//内存申请失败
		*err = 2;
		__wrn("page_data is null...\n");
		goto INIT_ERROR_FLAG_4;
	}

	/*************************************页面参数配置初始化****************************************/
	p_decode->total_page 			= 0;
	p_decode->current_page 			= 0;
	p_decode->move_height			= 0;
	p_decode->move_state			= 0;
	p_decode->charset				= EPDK_CHARSET_ENM_GBK;	//文件编码格式
	p_decode->default_charset		= EPDK_CHARSET_ENM_GBK;	//默认编码格式
	p_decode->decode_cnt			= DECODE_EBOOK_NUM / 2;	//预解码页数
	p_decode->decode_sta			= DECODE_NULL;			//解码状态位
	p_decode->decode_sem			= esKRNL_SemCreate(1);	//解码控制信号量
	p_decode->decode_tsk			= esKRNL_TCreate(__ebook_decode_task, (void *)p_decode, 0x2000, KRNL_priolevel5);	// 加载预解线程
	return p_decode;

INIT_ERROR_FLAG_4:

	if(p_decode->show_hdle)
	{
		EBOOK_Show_Uninit(p_decode->show_hdle);			//不初始化解码显示
		p_decode->show_hdle = NULL;
	}

INIT_ERROR_FLAG_3:

	if(p_decode->analysis_hdle)
	{
		EBOOK_Analysis_Uninit(p_decode->analysis_hdle);	//不初始化文件分析模块
		p_decode->analysis_hdle = NULL;
	}

INIT_ERROR_FLAG_2:

	if(p_decode->file_hdle)
	{
		eLIBs_fclose(p_decode->file_hdle);	//关闭文件
		p_decode->file_hdle = NULL;
	}

INIT_ERROR_FLAG_1:

	if(p_decode)
	{
		My_Mfree(0, p_decode);				//释放内存空间
		p_decode = NULL;
	}

	return NULL;
}

/*
************************************************************************************************************************
*                       				EBOOK_Decode_Config
*
*Description: 配置页面解码模块
*
*Arguments  : hdle: 操作句柄
*             config: 参数配置信息指针
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32 EBOOK_Decode_Config(H_DECODE_NEW hdle, __newdecode_config_t *config)
{
	__u32 			i;
	__ebook_show_config_t		show_config;
	__ebook_analysis_config_t	analysis_cfg;
	__ebook_decode_t			*p_decode;

	if(hdle == NULL){
		__wrn("hdle is null...\n");
		return EPDK_FAIL;
	}

	p_decode =(__ebook_decode_t *)hdle;
	//获取文件编码格式
	__ebook_decode_get_charset(p_decode);
	//获取字符字库信息
	eLIBs_memcpy(&(p_decode->config), config, sizeof(__newdecode_config_t));		//字符串拷贝
	p_decode->font_hdle 	= EBOOK_Font_Init(config->char_font, config->font_size);//电子书文本初始化，设置文本 
	p_decode->char_font		= EBOOK_Font_GetFont(p_decode->font_hdle);				//获取文本操作句柄
	p_decode->char_table	= EBOOK_Font_GetTable(p_decode->font_hdle);				//获取文本字宽表

	if(config->show_rotate == 0)	//横屏
	{
		__wrn("EBOOK_Decode_Config 0 show_rotate = 0\n");
		for(i = 0; i < DECODE_EBOOK_NUM; i++)//页数，一页一图层
		{
			p_decode->page[i].page_sta = 0;
			p_decode->page[i].page_no  = 0;
			p_decode->page[i].page_lyr = EBOOK_8bpp_layer_creat(config->show_width, config->show_height - config->bottom_width, 1);//创建页面缓冲区的图层8bpp

			if(p_decode->page[i].page_lyr == NULL){
				__wrn("page[i].page_lyr is create fails...\n");
				goto EBOOK_INIT_ERROR_1;
			}
		}

		//创建电子书显示图层
		p_decode->show_lyr	=	EBOOK_8bpp_layer_creat(config->show_width, config->show_height - config->bottom_width, 1);//创建显示的图层
		if(p_decode->show_lyr == NULL){
			__wrn("show_lyr is create fails...\n");
			goto EBOOK_INIT_ERROR_2;
		}

		//配置文件解析模块
		analysis_cfg.show_width = config->show_width - 2 * config->border_width + config->row_space;
		analysis_cfg.page_row   = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
		analysis_cfg.col_width  = config->font_size + config->col_space;
		analysis_cfg.char_table = p_decode->char_table;
		analysis_cfg.charset    = p_decode->charset;
		#if 1
			__wrn("show_width = %d\n",analysis_cfg.show_width);
			__wrn("page_row = %d\n",analysis_cfg.page_row);
			__wrn("col_width = %d\n",analysis_cfg.col_width);
			__wrn("char_table = %d \n",analysis_cfg.char_table);
			__wrn("charset = %d\n",analysis_cfg.charset);
		#endif
		EBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
		//配置数据读写模块
		show_config.show_start	= config->border_width;
		show_config.show_width  = config->show_width - 2 * config->border_width + config->row_space - config->scroll_width;
		show_config.page_row    = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
		show_config.row_height  = config->font_size + config->row_space;
		show_config.col_width   = config->font_size + config->col_space;
		show_config.font_color  = config->font_color;
		show_config.back_color  = config->back_color;
		show_config.char_font   = p_decode->char_font;
		show_config.char_table  = p_decode->char_table;
		show_config.charset     = p_decode->charset;
		#if 1
			__wrn("show_start = %d\n",show_config.show_width);
			__wrn("show_width = %d\n",show_config.show_width);
			__wrn("page_row = %d\n",show_config.page_row);
			__wrn("row_height = %d\n",show_config.row_height);
			__wrn("col_width = %d\n",show_config.col_width);
			__wrn("font_color = %d\n",show_config.font_color);
			__wrn("back_color = %d\n",show_config.back_color);
			__wrn("char_font = %d\n",show_config.char_font);
			__wrn("char_table = %d \n",show_config.char_table);
			__wrn("charset = %d\n",show_config.charset);
		#endif
		EBOOK_Show_Config(p_decode->show_hdle, &show_config);//电子书显示模块配置
		//启动电子书文件分析模块
		EBOOK_Analysis_Work(p_decode->analysis_hdle);
	}
	else
	{
		/*__wrn("EBOOK_Decode_Config 1 show_rotate = %d\n", config->show_rotate);
		for(i = 0; i < DECODE_EBOOK_NUM; i++)
		{
			p_decode->page[i].page_sta  = 0;
			p_decode->page[i].page_no   = 0;
			p_decode->page[i].page_lyr  = EBOOK_8bpp_layer_creat(config->show_width - config->bottom_width, config->show_height, 1);	// 申请软图层

			if(p_decode->page[i].page_lyr == NULL)
			{
				goto EBOOK_INIT_ERROR_1;
			}
		}

		p_decode->show_lyr  = EBOOK_8bpp_layer_creat(config->show_width - config->bottom_width, config->show_height, 1);	// 申请显示图层

		if(p_decode->show_lyr == NULL)
		{
			goto EBOOK_INIT_ERROR_2;
		}

		// 配置文件解析模块
		analysis_cfg.show_width = config->show_height - 2 * config->border_width + config->row_space - config->scroll_width;
		analysis_cfg.page_row   = (config->show_width - config->bottom_width) / (config->font_size + config->row_space);
		analysis_cfg.col_width  = config->font_size + config->col_space;
		analysis_cfg.char_table = p_decode->char_table;
		analysis_cfg.charset    = p_decode->charset;
		EBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);//电子书文件解析模块配置
		// 配置数据读写模块
		show_config.show_start = config->border_width;
		show_config.show_width = config->show_height - 2 * config->border_width + config->row_space;
		show_config.page_row   = (config->show_width - config->bottom_width) / (config->font_size + config->row_space);
		show_config.row_height = config->font_size + config->row_space;
		show_config.col_width  = config->font_size + config->col_space;
		show_config.font_color = config->font_color;
		show_config.back_color = config->back_color;
		show_config.char_font  = p_decode->char_font;
		show_config.char_table = p_decode->char_table;
		show_config.charset    = p_decode->charset;
		EBOOK_Show_Config(p_decode->show_hdle, &show_config);//电子书显示模块配置
		// 启动文件分析
		EBOOK_Analysis_Work(p_decode->analysis_hdle);*/
	}

	//获取总页数
	p_decode->current_page = 1;
	p_decode->total_page   = EBOOK_Analysis_GetTotalPage(p_decode->analysis_hdle);//总页数
	__wrn("total_page = %d\n",p_decode->total_page);
	return EPDK_OK;

EBOOK_INIT_ERROR_2:
EBOOK_INIT_ERROR_1:
	for(i = 0; i < DECODE_EBOOK_NUM; i++)
	{
		if(p_decode->page[i].page_lyr){
			//删除图层
			//MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
			GUI_LyrWinDelete(p_decode->page[i].page_lyr);
			p_decode->page[i].page_lyr = NULL;
		}
	}

	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				EBOOK_Decode_SetDefaultCharset
*
*Description: 设置默认编码格式
*
*Arguments  : hdle: 操作句柄
*             charset: 默认编码
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32 EBOOK_Decode_SetDefaultCharset(H_DECODE_NEW hdle, __epdk_charset_enm_e charset)
{
	__u8                    err;
	__ebook_decode_t       *p_decode;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_decode = (__ebook_decode_t *)hdle;
	__wrn("EBOOK_Decode_SetDefaultCharset...\n");

	if(p_decode->default_charset == charset)//如果默认编码格式 等于 检测到的编码格式
	{
		__wrn("default_charset is GBK...\n");
		return EPDK_OK;
	}

	/*p_decode->default_charset = charset;//重新把编码格式赋值给默认编码格式变量

	if((p_decode->charset == EPDK_CHARSET_ENM_UTF16LE)
	    || (p_decode->charset == EPDK_CHARSET_ENM_UTF16BE)
	    || (p_decode->charset == EPDK_CHARSET_ENM_UTF8))
	{
		// 非未知编码
		__wrn("UTF8 is STOP...\n");
		return EPDK_OK;
	}

	if(p_decode->decode_sta == DECODE_NULL)
	{
		// 未开始解码
		__wrn("decode_sta is NULL...\n");
		return EPDK_OK;
	}

	// 挂起预解线程
	esKRNL_SemPend(p_decode->decode_sem, 0, &err);
	// 获取文件编码格式
	__wrn("get charset is start...\n");
	__ebook_decode_get_charset(p_decode);
	// 配置解码
	__wrn("analysis config is start...\n");
	__ebook_decode_analysis_config(p_decode);
	// 配置显示
	__wrn("brush page is start...\n");
	__ebook_decode_brush_page(p_decode, DECODE_CURR);//刷新当前页界面数据，刷新页面缓存里的信息
	__wrn("show config is start...\n");
	__ebook_decode_show_config(p_decode);//显示配置
	__wrn("get charset is stop...\n");
	// 恢复预解线程
	esKRNL_SemPost(p_decode->decode_sem);
	// 获取当前页
	__wrn("get page is start...\n");
	__ebook_decode_get_page(p_decode, DECODE_CURR);
	__wrn("show page is start...\n");
	if(p_decode->move_height)
	{
		__wrn("move_height = %d\n",p_decode->move_height);
		__ebook_decode_get_next_page(p_decode);//下一页
	}
	__wrn("show page is start...\n");
	__ebook_decode_show_page(p_decode);//显示到页面界面图层
	__wrn("show page is stop...\n");
	return EPDK_OK;	*/


}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_ShowPage
*
*Description: 显示当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   EBOOK_Decode_ShowPage(H_DECODE_NEW hdle, __u32 offset)
{
	__ebook_decode_t       *p_decode;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}
	__wrn("EBOOK_Decode_ShowPage...\n");

	p_decode = (__ebook_decode_t *)hdle;
	p_decode->current_page = EBOOK_Analysis_GetPage(p_decode->analysis_hdle, offset);//获取当前分析页的页号
	__ebook_decode_get_page(p_decode, DECODE_CURR);	//获取当前页数据
	__ebook_decode_show_page(p_decode);			//显示一页数据
	EBOOK_Layer_OpenShow(p_decode->show_lyr);	//打开图层状态
	GUI_LyrWinSetTop(p_decode->show_lyr);		//设置图层置顶
	return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_Uninit
*
*Description: 删除页码解码模块
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32 EBOOK_Decode_Uninit(H_DECODE_NEW *hdle)
{
	__ebook_decode_t	*p_decode;
	__u8	err;
	__s32 	i;
	if(hdle == NULL){
		__wrn("H_DECODE_NEW hdle is null...\n");
		return EPDK_FALSE;
	}

	p_decode = (__ebook_decode_t *)hdle;	//获取地址

	if(p_decode->decode_tsk)//预解线程
	{
		while(1)
		{
			if(esKRNL_TDelReq(p_decode->decode_tsk) == OS_TASK_NOT_EXIST){//释放删除预解线程
				break;
			}

			esKRNL_TimeDly(1);//延时10ms
		}
		p_decode->decode_tsk = 0;
	}

	if(p_decode->decode_sem){//预解线程控制----信号量
		esKRNL_SemDel(p_decode->decode_sem, OS_DEL_ALWAYS, &err);//信号量删除
		p_decode->decode_sem = NULL;
	}

	if(p_decode->page_data){//页数据存放内存地址
		My_Pfree(p_decode->page_data, p_decode->data_len);
		p_decode->page_data = NULL;
	}

	for(i = 0; i<DECODE_EBOOK_NUM; i++){//删除页面软图层
		if(p_decode->page[i].page_lyr){
			//删除页面图层
			EBOOK_Layer_DelLay(p_decode->page[i].page_lyr);
			p_decode->page[i].page_lyr = NULL;
		}
	}

	if(p_decode->show_lyr){//删除显示图层
		//删除显示的图层
		EBOOK_Layer_DelLay(p_decode->show_lyr);
		p_decode->show_lyr = NULL;
	}
	
	if(p_decode->analysis_hdle){//释放文件分析句柄
		EBOOK_Analysis_Uninit(p_decode->analysis_hdle);
		p_decode->analysis_hdle = NULL;
	}

	if(p_decode->font_hdle){//释放文本模块句柄
		EBOOK_Font_Uninit(p_decode->show_hdle);
		p_decode->show_hdle = NULL;
	}

	if(p_decode->show_hdle){//释放显示模块句柄
		EBOOK_Show_Uninit(p_decode->show_hdle);
		p_decode->show_hdle = NULL;
	}

	if(p_decode->file_hdle){//关闭文件
		eLIBs_fclose(p_decode->file_hdle);
		p_decode->file_hdle =  NULL;
	}

	if(p_decode){
		My_Mfree(0, p_decode);
		p_decode = NULL;
	}

	return EPDK_OK;
}

























