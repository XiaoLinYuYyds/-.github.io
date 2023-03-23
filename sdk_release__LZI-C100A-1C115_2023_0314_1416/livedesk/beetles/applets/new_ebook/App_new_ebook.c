#include "App_new_ebook.h"



/************************************************************
*创建8bpp图层
************************************************************/
static	H_LYR app_new_ebook_8bpp_laye_creat(RECT *rect, __s32 pipe)	
{
		H_LYR layer = NULL;
		FB	fb =
		{
			{0, 0}, 										/* size 	 */
			{0, 0, 0},										/* buffer	 */
			{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},	/* fmt		 */
		};
		__disp_layer_para_t lstlyr =
		{
			DISP_LAYER_WORK_MODE_PALETTE,					 /* mode	  */
			0,												/* ck_mode	 */
			0,												/* alpha_en  */
			0,												/* alpha_val */
			1,												/* pipe 	 */
			0xff,											/* prio 	 */
			{0, 0, 0, 0},									/* screen	 */
			{0, 0, 0, 0},									/* source	 */
			DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
			NULL											/* fb		 */
		};
		__layerwincreate_para_t lyrcreate_info =
		{
			"new music layer",
			NULL,
			GUI_LYRWIN_STA_SUSPEND,
			GUI_LYRWIN_NORMAL
		};
		fb.size.width		= rect->width;
		fb.size.height		= rect->height;
		lstlyr.src_win.x		= 0;
		lstlyr.src_win.y		= 0;
		lstlyr.src_win.width	= rect->width;
		lstlyr.src_win.height	= rect->height;
		lstlyr.scn_win.x		= rect->x;
		lstlyr.scn_win.y		= rect->y;
		lstlyr.scn_win.width	= rect->width;
		lstlyr.scn_win.height	= rect->height;
		lstlyr.pipe = pipe;
		lstlyr.fb = &fb;
		lyrcreate_info.lyrpara = &lstlyr;
		layer = GUI_LyrWinCreate(&lyrcreate_info);
	
		if(!layer)
		{
			__err("GUI_LyrWinCreate fail... !\n");
		}
	
		return layer;
}

/************************************************************************************************************************
*Description	:		命令发送函数
************************************************************************************************************************/
__s32 app_new_ebook_cmd2para(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t mymsg;
	__wrn("\n*****************app_new_ebook_cmd2para********************\n");
	mymsg.h_deswin = GUI_WinGetParent(hwin);
	mymsg.h_srcwin = NULL;
	mymsg.id = GUI_MSG_COMMAND;
	mymsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
	mymsg.dwAddData2 = data1;
	mymsg.dwReserved = data2;
	return (GUI_SendNotifyMessage(&mymsg));
}


#if 1 //rat模块初始化+文件搜索
/************************************************************************************************************************
*Description	:		rat模块初始化，文件搜索打开
************************************************************************************************************************/
static	__s32 new_ebook_rat_init(__gui_msg_t *msg)
{
	app_new_ebook_ctrl_t *this;
	this = (app_new_ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	if(this->rat.rat_handle == NULL){//文件搜索句柄为空打开
		this->rat.rat_handle = rat_open_ex(RAT_TF, RAT_MEDIA_TYPE_EBOOK, 0);//打开TF卡

		if(this->rat.rat_handle == NULL){
			__wrn("rat.rat_handle is null!...\n");
			return EPDK_FALSE;
		}
		else{
			__wrn("open rat File Search successed!...\n");
		}
	}
	

	this->rat.total = rat_get_cur_scan_cnt(this->rat.rat_handle);//获取当前媒体类型文件扫描到的文件总数
	__wrn("this->rat.total = %d...\n",this->rat.total);

	return EPDK_OK;
}

/************************************************************************************************************************
*Description	:		从全路径中获取文件名
************************************************************************************************************************/
static __s32 new_ebook_ExtractFileName(char *FileName, char *FilePath)
{
	char *pFileName;
	__wrn("get Extract File Name is...!!\n");
	pFileName = eLIBs_strchrlast(FilePath, '\\');//汉字识别
	pFileName++;
	eLIBs_strcpy(FileName, pFileName);
	return EPDK_OK;
}
/************************************************************************************************************************
*Description	:	Get List Item File Full Path	
*					根据索引号获得条目文件名全路径
************************************************************************************************************************/
static __s32 new_ebook_GetListItemFileFullPath(app_new_ebook_ctrl_t *list_para, __s32 ItemIndex, char *FilePath)
{
	int ret;
	rat_entry_t ItemInfo;
	eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
	if(!list_para->rat.rat_handle){
		__wrn("rat_handle is null!!\n");
	}
	ret = rat_get_item_info_by_index(list_para->rat.rat_handle, ItemIndex, &ItemInfo);//获取当前项目信息索引
	__wrn("get List Item File Full Path is...!!\n");
	if(ret == EPDK_FAIL)
	{
		__wrn("get file information form rat fail!!\n");
		return EPDK_FAIL;
	}

	eLIBs_strcpy(FilePath, ItemInfo.Path);			//不去操作rat 内存
	return EPDK_OK;
}

/************************************************************************************************************************
*Description	:		根据索引号获得条目文件名(只有文件名，供显示)
************************************************************************************************************************/
static __s32 new_ebook_GetListItemFileName(__gui_msg_t *msg, __s32 ItemIndex, char *FileName)
{
	int ret;
	char FilePath[RAT_MAX_FULL_PATH_LEN] = "";
	app_new_ebook_ctrl_t *this;
	this = (app_new_ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	eLIBs_memset(FilePath, 0, sizeof(FilePath));			//数组初始化数据清0
	ret = new_ebook_GetListItemFileFullPath(this, ItemIndex, FilePath);//通过索引获取当前条目文件名的全路径,返回EPDK_OK
	__wrn("get List Item File Name is...!!\n");
	if(ret == EPDK_FAIL)
	{
		return EPDK_FAIL;
	}

	new_ebook_ExtractFileName(FileName, FilePath);			//从全路径获取文件名
	__wrn("get List Item Extract File Name is ok ok...!!\n");
	
	rat_set_file_for_play(this->rat.rat_handle, FileName);	//需要设置当前文件播放
	return EPDK_OK;
}
#endif

/************************************************************************************
*Description	:		播放文件
************************************************************************************/
static __s32 PlayEbookFiles(__gui_msg_t *msg, __s32 index, char *FileName)
{
	app_new_ebook_ctrl_t *this;
	HRATNPL h_rat;
	this = (app_new_ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	//h_rat = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
	rat_npl_set_cur(this->rat.rat_handle, index);//设置当前要播放的文件索引id
	__wrn("\nindex = %d\n",index);
}

/************************************************************************************
*Description	:	创建电子书文件显示和解码
************************************************************************************/
static __s32 app_new_ebook_decode_create(__gui_msg_t *msg)
{
	__s32               tmp, time;
	__s32               tmp_point;
	char                path[1024];
	app_new_ebook_ctrl_t   *new_ebook_ctrl;
	RECT 	rect;
	__s32 	ret;
	__u8    err;
	new_ebook_ctrl = (app_new_ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	//ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
	__wrn(" app_new_ebook_decode_create is start... \n");
	if(new_ebook_ctrl == NULL)
	{
		__wrn(" CShowScene malloc error \n");
		return EPDK_FAIL;
	}

	/* 创建 mbook core 句柄 */
	err = 0;
	new_ebook_ctrl->mbook = EBOOK_Decode_Init(NULL, &err);//电子书解码初始化

	if(new_ebook_ctrl->mbook == NULL)
	{
		__wrn(" init mbook error \n");
		
	}
	// 获取路径
	//MBOOK_Decode_GetFilePath(new_ebook_ctrl->mbook, path);
	// 设置默认编码
	EBOOK_Decode_SetDefaultCharset(new_ebook_ctrl->mbook, EPDK_CHARSET_ENM_GBK);//默认为GBK编码格式
	{
		new_ebook_ctrl->config.font_color    = APP_COLOR_WHITE;//文本颜色白色
	}
	/*********************************电子书文本显示控制**************************/
	new_ebook_ctrl->config.show_rotate   = 0;									//横竖屏显示，0为竖屏
	new_ebook_ctrl->config.font_size     = 16;									//文本字节大小
	new_ebook_ctrl->config.char_font     = new_ebook_ctrl->font;				//
	new_ebook_ctrl->config.row_space     = 10;									//行间距
	new_ebook_ctrl->config.col_space     = 2;									//列间距
	new_ebook_ctrl->config.border_width  = 10;									//边框宽
	new_ebook_ctrl->config.scroll_width  =  0;									//
	new_ebook_ctrl->config.bottom_width  = 0;									//底宽
	new_ebook_ctrl->config.show_width    = 1024;								//显示宽
	new_ebook_ctrl->config.show_height   = 558;									//显示高
	EBOOK_Decode_Config(new_ebook_ctrl->mbook, &new_ebook_ctrl->config);		//解码控制
	/*****************************************************************************/
	// 打开页面显示
	//new_ebook_ctrl->total_page = MBOOK_Decode_GetTotalPage(new_ebook_ctrl->mbook);//获取解码的总页数
	__wrn("total_page=%d...\n",new_ebook_ctrl->total_page);
	//if(new_ebook_ctrl->book_bkpoint.page_no == 0)//从头开始阅读
	{
		new_ebook_ctrl->cur_page = EBOOK_Decode_ShowPage(new_ebook_ctrl->mbook, 0); //不存在断点信息或者存在断点信息但从头开始阅读
	}
	//else
	{
		//new_ebook_ctrl->cur_page = MBOOK_Show_LastPage(new_ebook_ctrl->mbook, new_ebook_ctrl->book_bkpoint.page_no);//从上一次阅读的地方，接着往下阅读
	}
	__wrn("cur_page=%d...\n",new_ebook_ctrl->cur_page);

	
	/*page_para.volume = dsk_volume_get();
	page_para.cur_page = new_ebook_ctrl->cur_page;
	page_para.total_page = new_ebook_ctrl->total_page;
	eLIBs_strcpy(page_para.name, (void *)(eLIBs_strchrlast(path, '\\') + 1));	//获取当前阅读的电子书的书名
	*/


	return EPDK_OK;

}

/************************************************************************************
*创建电子书的manwin父窗口
************************************************************************************/
static __s32 __app_new_ebook_proc(__gui_msg_t *msg)
{
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0*/
			char FileName[RAT_MAX_FULL_PATH_LEN];
			gscene_bgd_set_state(BGD_STATUS_HIDE);  		//背景场景隐藏
			//g_disable_close_scn();							//关闭荧光屏
			//com_set_backcolor(CONST_BackColor_Black);		//设置背景颜色为黑色
			com_set_palette_by_id(ID_EBOOK_WKM_BMP);		//调色板id
			{
				//背景色，文字编辑框底色
				__u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR};
				com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
			}
		
			#if 1//打开RAT搜索文件模块，并且搜索相册图片类型文件
				eLIBs_memset(FileName, 0, sizeof(FileName));		//数组数据初始化，清0
				new_ebook_rat_init(msg);							//rat模块初始化
				new_ebook_GetListItemFileName(msg, 24, FileName);	//获取列表项目的图片文件名
				__wrn("\nFileName = %s\n",FileName);
				//PlayEbookFiles(msg, 24, FileName);//播放文件
			#endif

			app_new_ebook_decode_create(msg);//创建电子书解码
			
			
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1*/
			app_new_ebook_ctrl_t *this;
			this = (app_new_ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			rat_close(this->rat.rat_handle);//关闭rat模块
			
			if(this->mbook)
			{
				EBOOK_Decode_Uninit(this->mbook);//删除解码模块
				this->mbook = NULL;
			}
			eLIBs_memset(this, 0, sizeof(app_new_ebook_ctrl_t));
			esMEMS_Bfree(this,sizeof(app_new_ebook_ctrl_t));
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2*/

		}
		return EPDK_OK;
		
		case GUI_MSG_CLOSE:{/*3*/
			__wrn("close new ebook app is...\n");
			app_new_ebook_cmd2para(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, SWITCH_TO_EXPLORER, 0);//退出到资源管理器列表
			
		}
		return EPDK_OK;
		
		case GUI_MSG_QUIT:{/*4*/
			__wrn("quit new ebook app is...\n");
			app_new_ebook_cmd2para(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);//退出到home应用
		}
		return EPDK_OK;
		
		case GUI_MSG_KEY:{/*5*/

			switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{
					__gui_msg_t *mymsg;
					mymsg->id = GUI_MSG_QUIT;
					mymsg->h_deswin = msg->h_deswin;
					GUI_SendMessage(mymsg);
				}
				break;

				case GUI_MSG_KEY_NUM0:{
					__gui_msg_t *mymsg;
					mymsg->id = GUI_MSG_CLOSE;
					mymsg->h_deswin = msg->h_deswin;
					GUI_SendMessage(mymsg);
				}
				break;

				default:
					break;
			}
		}
		break;

		case GUI_MSG_COMMAND:{/*7*/

		}
		return EPDK_OK;

		default:
			break;
	}

	return GUI_ManWinDefaultProc(msg);
}

/************************************************************************************
*创建电子书的manwin父窗口
************************************************************************************/
H_WIN app_new_ebook_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	app_new_ebook_ctrl_t	*new_ebook_ctrl;
	H_WIN	hManWin;//用于判断manwin窗口创建是否成功
	__wrn("app_new_ebook_manwin_create is start...\n");
	new_ebook_ctrl = (app_new_ebook_ctrl_t *)esMEMS_Balloc(sizeof(app_new_ebook_ctrl_t));
	eLIBs_memset(new_ebook_ctrl, 0, sizeof(app_new_ebook_ctrl_t));
	new_ebook_ctrl->font      = para->font;//文本
	new_ebook_ctrl->root_type = para->root_type;//TF卡或USB类型
	new_ebook_ctrl->index	  = para->data;//传入的文件索引id
	__wrn("root_type=%d\n",new_ebook_ctrl->root_type);
	/*****************************开始操作配置manwin窗口*********************************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
	create_info.name		=	APP_NEWEBOOK;
	create_info.hParent		=	para->h_parent;
	create_info.ManWindowProc	=	(__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_ebook_proc);//回调处理函数
	create_info.attr		=	(void *)new_ebook_ctrl;
	create_info.id			=	APP_NEWEBOOK_ID;
	create_info.hHosting	=	NULL;

	hManWin = GUI_ManWinCreate(&create_info);//返回一个创建好的manwin窗口句柄

	if(!hManWin){
		__wrn("manwin creat is false...");
		return EPDK_FALSE;
	}

	return hManWin;
}


















































