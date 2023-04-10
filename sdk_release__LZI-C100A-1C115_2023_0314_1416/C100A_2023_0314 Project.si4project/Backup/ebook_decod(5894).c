#include "ebook_decod_i.h"
#include "ebook_decod.h"



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
static void __decode_task(void *p_arg)
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

		esKRNL_TimeDly(5);
		esKRNL_SemPend(p_decode->decode_sem, 0, &err);

		if(p_decode->decode_sta != DECODE_NULL)
		{
			if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
			    && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) >= 1)
			    && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_EBOOK_NUM / 2) <= p_decode->total_page))
			{
				//__decode_read_page(p_decode);
			}

			p_decode->decode_cnt++;

			if(p_decode->decode_cnt == DECODE_EBOOK_NUM)
			{
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
H_DECODE_NEW   EBOOK_Decode_Init(char *filename, __u8 *err)
{
	__ebook_decode_t	*p_decode;

	p_decode = (__ebook_decode_t *)My_Balloc(sizeof(__ebook_decode_t));
	eLIBs_memset(p_decode, 0, sizeof(__ebook_decode_t));
	if(filename == NULL){ //如果文件名为空
		__s32   npl_index;//当前播放列表的文件索引id
		HRATNPL	h_rat_npl;//rat模块的当前播放列表句柄
		h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);	//打开相关的媒体类型文件,电子书媒体类型文件
		npl_index = rat_npl_get_cur(h_rat_npl);			//获取当前播放列表的播放文件索引id
		rat_npl_close(h_rat_npl);						//关闭rat相关的媒体类型文件
	} else {
		eLIBs_memcpy(p_decode->file_path, filename, MAX_FILE_NAME_SIZE);
	}

	p_decode->file_hdle = eLIBs_fopen(p_decode->file_path, "rb");		//"rb"打开二进制文件进行读取;文件句柄获取

	if(p_decode->file_hdle == NULL){//文件句柄为空,文件不存在
		*err = 1;
		goto INIT_ERROR_FLAG_1;
	}

	p_decode->file_size = eLIBs_GetFileSize(p_decode->file_path);		//获取文件的大小
	//p_decode->analysis_hdle = MBOOK_Analysis_Init(p_decode->file_hdle);	//文件解码分析

	if(p_decode->analysis_hdle == NULL){//文件解码分析句柄为空
		*err = 2;
		goto INIT_ERROR_FLAG_2;
	}
	
	//p_decode->show_hdle = MBOOK_Show_Init();//显示初始化

	if(p_decode->show_hdle == NULL){//显示句柄为空
		*err = 2;
		goto INIT_ERROR_FLAG_3;
	}

	p_decode->data_len = 15;
	p_decode->page_data = (void *)My_Palloc(p_decode->data_len, 0);//申请内存

	if(p_decode->page_data == 0){//内存申请失败
		*err = 2;
		goto INIT_ERROR_FLAG_4;
	}

	/*************************************页面参数配置初始化****************************************/
	p_decode->total_page 			= 0;
	p_decode->current_page 			= 0;
	p_decode->move_height			= 0;
	p_decode->move_state			= 0;
	p_decode->charset				= EPDK_CHARSET_ENM_GBK;	//文件编码格式
	p_decode->default_charset		= EPDK_CHARSET_ENM_GBK;	//默认编码格式
	p_decode->decode_cnt			= DECODE_EBOOK_NUM;		//预解码页数
	p_decode->decode_sta			= DECODE_NULL;
	p_decode->decode_sem			= esKRNL_SemCreate(1);
	p_decode->decode_tsk			= esKRNL_TCreate(__decode_task, (void *)p_decode, 0x2000, KRNL_priolevel5);		// 加载预解线程
	return p_decode;

INIT_ERROR_FLAG_4:

	if(p_decode->show_hdle)
	{
		MBOOK_Show_Uninit(p_decode->show_hdle);			//不初始化解码显示
		p_decode->show_hdle = NULL;
	}

INIT_ERROR_FLAG_3:

	if(p_decode->analysis_hdle)
	{
		MBOOK_Analysis_Uninit(p_decode->analysis_hdle);	//不初始化文件分析模块
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
		__WRN("hdle is null...\n");
		return EPDK_FAIL;
	}

	p_decode =(__ebook_decode_t *)hdle;
	//获取文件编码格式
	__decode_get_charset(p_decode);
	//获取字符字库信息
	eLIBs_memset(&(p_decode->config), config, sizeof(__newdecode_config_t));
	p_decode->file_hdle 	= EBOOK_Font_Init(config->char_font, config->font_size);//电子书文本初始化 
	p_decode->char_font		= EBOOK_Font_GetFont(p_decode->file_hdle);//获取文本操作句柄
	p_decode->char_table	= EBOOK_Font_GetTable(p_decode->file_hdle);//获取文本字宽表

	if(config->show_rotate == 0)
	{
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
		analysis_cfg.show_width = config->show_height - 2 * config->border_width + config->row_space;
		analysis_cfg.page_row   = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
		analysis_cfg.col_width  = config->font_size + config->col_space;
		analysis_cfg.char_table = p_decode->char_table;
		analysis_cfg.charset    = p_decode->charset;
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
		EBOOK_Show_Config(p_decode->show_hdle, &show_config);//电子书显示模块配置
		//启动电子书文件分析模块
		EBOOK_Analysis_Work(p_decode->analysis_hdle);
	}
	else
	{
		for(i = 0; i < DECODE_EBOOK_NUM; i++)
		{
			p_decode->page[i].page_sta  = 0;
			p_decode->page[i].page_no   = 0;
			p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(config->show_width - config->bottom_width, config->show_height, 1);	// 申请软图层

			if(p_decode->page[i].page_lyr == NULL)
			{
				goto EBOOK_INIT_ERROR_1;
			}
		}

		p_decode->show_lyr  = MBOOK_Layer_CreatLayer(config->show_width - config->bottom_width, config->show_height, 1);	// 申请显示图层

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
		EBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
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
		EBOOK_Show_Config(p_decode->show_hdle, &show_config);
		// 启动文件分析
		EBOOK_Analysis_Work(p_decode->analysis_hdle);
	}

	//获取总页数
	p_decode->current_page = 1;
	//p_decode->total_page   = ;//总页数
	return EPDK_OK;

EBOOK_INIT_ERROR_2:
EBOOK_INIT_ERROR_1:
	for(i = 0; i < DECODE_EBOOK_NUM; i++)
	{
		if(p_decode->page[i].page_lyr){
			//删除图层
			
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

	if(p_decode->default_charset == charset)
	{
		return EPDK_OK;
	}

	p_decode->default_charset = charset;

	if((p_decode->charset == EPDK_CHARSET_ENM_UTF16LE)
	    || (p_decode->charset == EPDK_CHARSET_ENM_UTF16BE)
	    || (p_decode->charset == EPDK_CHARSET_ENM_UTF8))
	{
		// 非未知编码
		return EPDK_OK;
	}

	if(p_decode->decode_sta == DECODE_NULL)
	{
		// 未开始解码
		return EPDK_OK;
	}

	// 挂起预解线程
	esKRNL_SemPend(p_decode->decode_sem, 0, &err);
	// 获取文件编码格式
	__decode_get_charset(p_decode);
	// 配置解码
	__decode_analysis_config(p_decode);
	// 配置显示
	__decode_brush_page(p_decode, DECODE_CURR);
	__decode_show_config(p_decode);
	// 恢复预解线程
	esKRNL_SemPost(p_decode->decode_sem);
	// 获取当前页
	__decode_get_page(p_decode, DECODE_CURR);

	if(p_decode->move_height)
	{
		__decode_get_next_page(p_decode);
	}

	__decode_show_page(p_decode);
	return EPDK_OK;	


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
__s32   MBOOK_Decode_ShowPage(H_DECODE_NEW hdle, __u32 offset)
{
	__ebook_decode_t       *p_decode;

	if(hdle == NULL)
	{
		return EPDK_FAIL;
	}

	p_decode = (__ebook_decode_t *)hdle;
	p_decode->current_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, offset);
	__decode_get_page(p_decode, DECODE_CURR);
	__decode_show_page(p_decode);
	MBOOK_Layer_OpenShow(p_decode->show_lyr);
	GUI_LyrWinSetTop(p_decode->show_lyr);
	return p_decode->current_page;
}
























