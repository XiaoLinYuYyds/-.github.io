#include	"new_music_playlist.h"
#include	"App_new_music_ui_para.h"

#if 1 //rat模块操作+文件搜索
//rat模块初始化
static __s32 playlist_rat_init(playlist_ctrl_t	*this)
{
	if(this->rat.rat_handle == NULL){
		__wrn("playlist_rat_init state is  open...\n");
		this->rat.rat_handle = rat_open_ex(RAT_TF, RAT_MEDIA_TYPE_AUDIO, 0);
		if(this->rat.rat_handle == NULL){
			__wrn("this->rat.rat_handle is null...\n");
			return EPDK_FALSE;
		}
		else{
			__wrn("this->rat.rat_handle is open success...\n");
		}
	}
	__wrn("this->rat.rat_handle ...\n");
	this->rat.total = rat_get_cur_scan_cnt(this->rat.rat_handle);
	__wrn("this->rat.total = %d...\n",this->rat.total);
	return EPDK_OK;
}

//rat模块初始化删除
static __s32 playlist_rat_deinit(playlist_ctrl_t	*this)
{
	if(this->rat.rat_handle != NULL){
		__wrn("playlist_rat_init state is  close...\n");
		 rat_close(this->rat.rat_handle);
	}
	__wrn("this->rat.rat_handle ...\n");
	return EPDK_OK;
}

/***************************************************************************
//从全路径中获取条目文件的文件名
@return:
@parameters:char *FileName(字符型指针存放文件名), char *FilePath(存放路径)
***************************************************************************/
static __s32 playlist_ExtractFileName(char *FileName, char *FilePath)
{
	char *pFileName;
	__wrn("get extract file name is...\n");
	pFileName = eLIBs_strchrlast(FilePath, '\\');
	pFileName++;//地址++
	eLIBs_strcpy(FileName, pFileName);//提取好的汉字赋值FlieName
	return EPDK_OK;
}

//根据索引号获取条目文件的全路径
static __s32 get_playlist_FilePath(playlist_ctrl_t *this, __s32 ItemIndex, char *FilePath)
{
	__s32 ret;
	rat_entry_t ItemInfo;//
	eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
	if(!this->rat.rat_handle){
		__wrn("rat_handle is null...\n");
		return EPDK_FALSE;
	}
	ret = rat_get_item_info_by_index(this->rat.rat_handle, ItemIndex, &ItemInfo);//通过索引获取当前条目文件名的全路径
	if(ret == EPDK_FAIL){
		__wrn("get item file path is fail...\n");
		return EPDK_FAIL;
	}
	__wrn("get item file path is success...\n");
	eLIBs_strcpy(FilePath, ItemInfo.Path);//把路径复制给FilePath
	__wrn("iteminfo.path = %s\n",ItemInfo.Path);

	return EPDK_OK;
}

//根据索引号获取条目名（只有文件名，共显示）
static __s32 get_playlist_FileName(playlist_ctrl_t *this, __s32 FileIndex, char *FileName)
{
	__s32 ret;
	char  FilePath[RAT_MAX_FULL_PATH_LEN] = "";//定义一个字符型变量，用于存放文件路径
	__wrn("get list item file path is start...\n");
	ret = get_playlist_FilePath(this, FileIndex, FilePath);//获取文件全路径

	if(ret == EPDK_FAIL){
		__wrn("get list item file path is fail...\n");
		return EPDK_FAIL;
	}
	__msg("get list item file path is success...\n");

	__wrn(" forme file path get file name is start...\n");
	playlist_ExtractFileName(FileName, FilePath);//从文件全路径中提取出文件名
	__wrn(" forme file path get file name is success...\n");
}
#endif

//申请使用图片资源
static __s32 __playlist_uipara_init(__gui_msg_t *msg)
{
	playlist_ctrl_t	*this;
	this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	this->playlist_uipara = get_new_music_uipara(0);//获取UI控件的大小和位置参数
	if(!this->playlist_uipara){
		__wrn("this->playlist_uipara is null...\n");
	}
	
	if(!this->scene_para[0].focus_bmp){
		this->scene_para[0].focus_bmp = theme_open(ID_MUSIC_LIST_ITEM_FOCUS_BMP);//列表选中的显示图片
		__wrn("theme open is success...\n");
	}
}

//释放使用图片资源
static __s32 __playlist_uipara_deinit(__gui_msg_t *msg)
{
	playlist_ctrl_t	*this;
	this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

	if(this->scene_para[0].focus_bmp){
		theme_close(this->scene_para[0].focus_bmp);//列表选中的显示图片
		this->scene_para[0].focus_bmp = NULL;
		__wrn("theme close is success...\n");
	}
}

/*****************************************************************************
*Description	:	播放列表音乐文件的项目索引id转换成文件编号序号获取函数:
******************************************************************************/
static void playlist_get_item_number_size(__gui_msg_t *msg,__u32 num)
{
	playlist_ctrl_t *this;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str数组内存空间初始化
	//eLIBs_strcat(str, "number:");
	snum = num + 1;//文件搜索是从0开始，但是文件序号从1开始，所以要加1
	eLIBs_uint2str_dec(snum, numstr);//int型转char型
	 
	eLIBs_strcat(str, numstr);
	eLIBs_strcat(str, ".");
	eLIBs_memset(this->framewin_create_para->file_number, 0, 10);
	eLIBs_strcpy(this->framewin_create_para->file_number, (const char *)str);
}

//绘制播放列表UI
static __s32 __playlist_ui_paint_all_item(playlist_ctrl_t *this, __gui_msg_t *msg)
{
	GUI_RECT FocusItem_Rect;//选中的音乐文件条目矩形
	GUI_RECT unFocusItem_Rect;//未选中的音乐文件条目矩形
	playlist_create_para_t *playlist_create_para;
	char FileName[RAT_MAX_FULL_PATH_LEN];
	void *focus_bmp;
	__s32 index=0;
	char file_index;
	#if MEMDEV_ONOFF//存放伪内存变量
		GUI_MEMDEV_Handle	draw_mem;
	#endif
	
	playlist_create_para = (playlist_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(this->framewin_create_para->playlist_hfrm)) == GUI_LYRWIN_STA_SUSPEND)//检测图层是否开启状态
	{
		__wrn("__playlist_ui_paint_all_item the layer state is not open...\n");
		return EPDK_FAIL;
	}
	__wrn("__playlist_ui_paint_all_item is 1...\n");
	GUI_LyrWinSel(this->framewin_create_para->playlist_hlyr);//选择该窗口的图层
	playlist_create_para->font = NULL;
	#if MEMDEV_ONOFF//创建伪内存设备
		draw_mem = GUI_MEMDEV_Create( 0, 0, this->playlist_uipara->playlist_uipara.playlist_area_hlyr.width,
											this->playlist_uipara->playlist_uipara.playlist_area_hlyr.height);
		GUI_MEMDEV_Select(draw_mem);
		__wrn("draw_mem = 0x%x...\n",draw_mem);
	#endif
	
	#if 1//获取文件名
		eLIBs_memset(FileName, 0, sizeof(FileName));//字符型数组初始化，数据清0
		//get_playlist_FileName(this, index, FileName);//获取文件名，然后存放到FileName数组
		//__wrn("FileName = %s...\n",FileName);
	#endif
	for(index=0; index<(this->rat.total-8);index++)
	{
		#if 1//通过索引id获取文件名
			get_playlist_FileName(this, index, FileName);//获取文件名，然后存放到FileName数组
			__wrn("FileName = %s...\n",FileName);
		#endif
		
		#if 1//通过音乐文件索引id转换成文件序号
			playlist_get_item_number_size(msg, index);
			__wrn("file_number = %s...\n",this->framewin_create_para->file_number);//打印文件序号
		#endif

		
		//else
		//for(index=0; index<(this->rat.total-8);index++)//绘制多个音乐文件空白条目
		//{
		#if 1//绘制未选中的音乐文件空白条目
			//GUI_SetFont(this->framewin_create_para->font);//设置文本
			GUI_SetFont(playlist_create_para->font);	//设置文本
			GUI_UC_SetEncodeUTF8();						//设置文本编码格式
			GUI_SetColor(GUI_WHITE);					//设置文本颜色，白色
			//GUI_SetBkColor(GUI_BLACK);					//设置控件背景颜色
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);		//设置绘制模式
			/*{//未选中的矩形显示区域
				unFocusItem_Rect.x0 = 10;//this->playlist_uipara->playlist_uipara.list_unfocus.x;
				unFocusItem_Rect.y0 = index*32 + 0;//index*10 + this->playlist_uipara->playlist_uipara.list_unfocus.y;
				unFocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.list_focus.width;
				unFocusItem_Rect.y1 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.height;
			}*/
			{//未选中的矩形显示区域
				unFocusItem_Rect.x0 = 5;
				unFocusItem_Rect.y0 = index*32 + 9;
				unFocusItem_Rect.x1 = 340;
				unFocusItem_Rect.y1 = index*32 + 32;
			}
			__wrn("unFocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",unFocusItem_Rect.x0,
								unFocusItem_Rect.y0, unFocusItem_Rect.x1, unFocusItem_Rect.y1);
			//focus_bmp = dsk_theme_hdl2buf(this->scene_para[0].focus_bmp);
			{
				//GUI_BMP_Draw(focus_bmp, FocusItem_Rect.x0, FocusItem_Rect.y0);//绘制选中的音乐文件条目图片
				//GUI_DispStringAt(FileName, FocusItem_Rect.x0, FocusItem_Rect.y0);
				//GUI_DispStringInRect(FileName, &FocusItem_Rect, GUI_TA_VCENTER|GUI_TA_HCENTER);
			//{
				__wrn("FileName = %s...\n",FileName);
				//GUI_DispStringInRect(this->framewin_create_para->file_number, &unFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER);
				GUI_DispStringInRect(FileName, &unFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER|GUI_TA_HCENTER);
				GUI_DispStringAt(this->framewin_create_para->file_number, 10, index*32 + 8);//直接显示字符串
			}
		#endif
		//}
		if(index == 0){//第一个绘制选中背景
			#if 1//绘制选中的音乐文件背景条目
				//GUI_SetFont(this->framewin_create_para->font);//设置文本
				GUI_SetFont(playlist_create_para->font);//设置文本
				GUI_UC_SetEncodeUTF8();//设置文本编码格式
				GUI_SetColor(GUI_YELLOW);//设置文本颜色
				//GUI_SetBkColor(GUI_BLACK);//设置控件背景颜色
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
				/*{//选中的矩形显示区域
					FocusItem_Rect.x0 = this->playlist_uipara->playlist_uipara.list_focus.x;
					FocusItem_Rect.y0 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.y;
					FocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.list_focus.width;
					FocusItem_Rect.y1 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.height;
				}*/
				{//选中的矩形显示区域
					FocusItem_Rect.x0 = 5;
					FocusItem_Rect.y0 = index*32 + 9;
					FocusItem_Rect.x1 = 340;
					FocusItem_Rect.y1 = index*32 + 32;
				}
				__wrn("FocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",FocusItem_Rect.x0,
									FocusItem_Rect.y0, FocusItem_Rect.x1, FocusItem_Rect.y1);
				focus_bmp = dsk_theme_hdl2buf(this->scene_para[0].focus_bmp);
				{
					GUI_BMP_Draw(focus_bmp, FocusItem_Rect.x0, FocusItem_Rect.y0);//绘制选中的音乐文件条目图片
					//GUI_DispStringAt(FileName, FocusItem_Rect.x0, FocusItem_Rect.y0);
					GUI_DispStringInRect(FileName, &FocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER|GUI_TA_HCENTER);
					__wrn("FileName = %s...\n",FileName);
					GUI_DispStringAt(this->framewin_create_para->file_number, 10, index*32 + 8);//直接显示字符串
					//GUI_DispStringInRect(this->framewin_create_para->file_number, &FocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER);//文件序号
					this->framewin_create_para->CurFocus_id = 0;
					eLIBs_strcpy(this->framewin_create_para->last_filename, FileName);//把第一个选中的文件名赋值给last_filename
				}
			#endif
		}
	}

	#if 0//绘制音乐文本显示区域
		GUI_SetFont(playlist_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();					//设置文本编码格式
		GUI_SetColor(GUI_WHITE);				//设置文本颜色，白色
		GUI_SetBkColor(GUI_BLACK);				//设置控件背景颜色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//设置绘制模式
		{//未选中的矩形显示区域
			unFocusItem_Rect.x0 = this->playlist_uipara->playlist_uipara.txt_rect.x;
			unFocusItem_Rect.y0 = index*32 + this->playlist_uipara->playlist_uipara.txt_rect.y;
			unFocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.txt_rect.width;
			unFocusItem_Rect.y1 = index*32 + this->playlist_uipara->playlist_uipara.txt_rect.height;
		}
		__wrn("unFocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",unFocusItem_Rect.x0,
							unFocusItem_Rect.y0, unFocusItem_Rect.x1, unFocusItem_Rect.y1);
		{
			__wrn("FileName = %s...\n",FileName);
			GUI_DispStringInRect(FileName, &unFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER);//绘制音乐文件名到矩形
			//GUI_DispStringAt(FileName, 70, 28);
		}
	#endif
	//}

	#if MEMDEV_ONOFF//删除伪内存设备
		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
	#endif
	
	return EPDK_OK;
}

//删除上一次选中的音乐文件UI显示区域功能,并绘制为未选择音乐条目
static __s32 delete_last_focus_area(__gui_msg_t *msg, playlist_ctrl_t	*this, __s32 last_index)
{
	GUI_RECT LastFocusItem_Rect;//上一次选中的音乐文件条目矩形
	
	#if 1//通过音乐文件索引id转换成文件序号
		playlist_get_item_number_size(msg, last_index);
		__wrn("noe item last_index the file_number = %s...\n",this->framewin_create_para->file_number);//打印文件序号
	#endif
		
	#if 1//删除上一个选中的音乐文件矩形区域
		GUI_SetFont(this->framewin_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		//GUI_SetColor(GUI_WHITE);//设置文本颜色，白色
		//GUI_SetBkColor(GUI_INVALID_COLOR);//设置控件背景颜色,蓝色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
		{//选中的矩形显示区域5,9,316,22
			LastFocusItem_Rect.x0 = 5;
			LastFocusItem_Rect.y0 = last_index*32 + 9;
			LastFocusItem_Rect.x1 = 345;
			LastFocusItem_Rect.y1 = last_index*32 + 32;
		}
		GUI_ClearRectEx(&LastFocusItem_Rect);//先删除前面的矩形，然后在下面重新画
	#endif
	
	#if 1//绘制前一个为未选中的音乐文件空白条目
		GUI_SetFont(this->framewin_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		GUI_SetColor(GUI_WHITE);//设置文本颜色，白色
		//GUI_SetBkColor(GUI_INVALID_COLOR);//设置控件背景颜色,透明
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
		/*{//未选中的矩形显示区域0,0,386,32
			LastFocusItem_Rect.x0 = 0;//this->playlist_uipara->playlist_uipara.list_unfocus.x;
			LastFocusItem_Rect.y0 = last_index*32 + 0;//index*10 + this->playlist_uipara->playlist_uipara.list_unfocus.y;
			LastFocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.list_focus.width;
			LastFocusItem_Rect.y1 = last_index*32 + this->playlist_uipara->playlist_uipara.list_focus.height;
		}*/
		{//选中的矩形显示区域
			LastFocusItem_Rect.x0 = 5;
			LastFocusItem_Rect.y0 = last_index*32 + 9;
			LastFocusItem_Rect.x1 = 345;
			LastFocusItem_Rect.y1 = last_index*32 + 32;
		}
		__wrn("LastFocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",LastFocusItem_Rect.x0,
			LastFocusItem_Rect.y0, LastFocusItem_Rect.x1, LastFocusItem_Rect.y1);
		{
			__wrn("last_filename = %s...\n",this->framewin_create_para->last_filename);
			GUI_DispStringInRect(this->framewin_create_para->last_filename, &LastFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER|GUI_TA_HCENTER);
			//GUI_DispStringAt(FileName, 70, 28);
			GUI_DispStringAt(this->framewin_create_para->file_number, 10, last_index*32 + 8);//直接显示字符串
					
		}
	#endif

}

//通过按键上下选择绘制音乐文件显示条目的功能
static __s32 key_updown_focus_ui_paint(__gui_msg_t *msg, playlist_ctrl_t	*this)
{
	GUI_RECT CurFocusItem_Rect;	//当前选中的音乐文件条目矩形
	char cur_filename[RAT_MAX_FULL_PATH_LEN];//存放当前选中的音乐文件名
	void *CurFocus_bmp;			//存放当前选中的音乐文件背景图片资源
	__s32 index;				//存放当前选中的文件索引id
	static __s32 last_index;	//存放上一次的选中文件索引id

	eLIBs_memset(cur_filename, 0, sizeof(cur_filename));	 //字符串数组初始化，数据清0
	GUI_LyrWinSel(this->framewin_create_para->playlist_hlyr);//选择该窗口的图层
	this->framewin_create_para->font = NULL;				 //文本变量空
	index = this->framewin_create_para->CurFocus_id;		 //获取当前选中的音乐文件id
	__wrn("last_filename = %s...\n",this->framewin_create_para->last_filename);//上一次选中的音乐文件名
	
	#if 1//通过索引获取当前选中音乐文件的文件名
		get_playlist_FileName(this, index, cur_filename);//获取文件名，然后存放到FileName数组
		__wrn("cur_filename = %s...\n",cur_filename);
	#endif

	#if 1 //删除上一次选中的音乐文件条目显示矩形
		delete_last_focus_area(msg, this, last_index);
	#endif
	
	#if 1 //通过音乐文件索引id转换成文件序号
			playlist_get_item_number_size(msg, index);
			__wrn("index the file_number = %s...\n",this->framewin_create_para->file_number);//打印文件序号
	#endif
	
	#if 1 //绘制按键上下选中的音乐文件背景条目
		GUI_SetFont(this->framewin_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		GUI_SetColor(GUI_YELLOW);//设置文本颜色,黄色
		//GUI_SetBkColor(GUI_BLACK);//设置控件背景颜色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
		/*{//选中的矩形显示区域5,9,316,22
			CurFocusItem_Rect.x0 = this->playlist_uipara->playlist_uipara.list_focus.x;
			CurFocusItem_Rect.y0 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.y;
			CurFocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.list_focus.width;
			CurFocusItem_Rect.y1 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.height;
		}*/
		{//选中的矩形显示区域
			CurFocusItem_Rect.x0 = 5;
			CurFocusItem_Rect.y0 = index*32 + 9;
			CurFocusItem_Rect.x1 = 340;
			CurFocusItem_Rect.y1 = index*32 + 32;
		}
		__wrn("CurFocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",CurFocusItem_Rect.x0,
			CurFocusItem_Rect.y0, CurFocusItem_Rect.x1, CurFocusItem_Rect.y1);
		CurFocus_bmp = dsk_theme_hdl2buf(this->scene_para[0].focus_bmp);
		{
			GUI_BMP_Draw(CurFocus_bmp, CurFocusItem_Rect.x0, CurFocusItem_Rect.y0);//绘制选中的音乐文件条目图片
			//GUI_DispStringAt(FileName, FocusItem_Rect.x0, FocusItem_Rect.y0);
			GUI_DispStringInRect(cur_filename, &CurFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER|GUI_TA_HCENTER);
			__wrn("cur_filename = %s...\n",cur_filename);
		}
		GUI_DispStringAt(this->framewin_create_para->file_number, 10, index*32 + 8);//直接显示字符串,显示文件序号
	#endif

	eLIBs_strcpy(this->framewin_create_para->last_filename, cur_filename);//把第一个选中的文件名赋值给last_filename
	last_index = index;//把当前的音乐文件索引id赋值给last_index，保存为上一次的选中文件索引id
	__wrn("last_index = %d...\n",last_index);
	
}

//绘制播放列表选中的单个音乐文件项目
static __s32 __playlist_ui_paint_one_item(__gui_msg_t *msg)
{
	playlist_ctrl_t	*this;
	GUI_RECT CurFocusItem_Rect;	//当前选中的音乐文件条目矩形
	GUI_RECT LastFocusItem_Rect;//上一次选中的音乐文件条目矩形
	char cur_filename[RAT_MAX_FULL_PATH_LEN];//存放当前选中的音乐文件名
	void *CurFocus_bmp;			//存放当前选中的音乐文件背景图片资源
	__s32 index;				//存放当前选中的文件索引id
	static __s32 last_index;	//存放上一次的选中文件索引id
	
	#if MEMDEV_ONOFF	//存放伪内存变量
		GUI_MEMDEV_Handle	draw_mem;
	#endif

	this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	__wrn("__playlist_ui_paint_one_item is 1...\n");
	#if 0 //
	eLIBs_memset(cur_filename, 0, sizeof(cur_filename));	 //字符串数组初始化，数据清0
	GUI_LyrWinSel(this->framewin_create_para->playlist_hlyr);//选择该窗口的图层
	this->framewin_create_para->font = NULL;				 //文本变量空
	index = this->framewin_create_para->CurFocus_id;		 //获取当前选中的音乐文件id
	__wrn("last_filename = %s...\n",this->framewin_create_para->last_filename);//上一次选中的音乐文件名
	#endif
	
	#if MEMDEV_ONOFF//创建伪内存设备，作用是加快图片的绘制
		draw_mem = GUI_MEMDEV_Create( 0, 0, this->playlist_uipara->playlist_uipara.playlist_area_hlyr.width, this->playlist_uipara->playlist_uipara.playlist_area_hlyr.height);
		GUI_MEMDEV_Select(draw_mem);
		__wrn("draw_mem = 0x%x...\n",draw_mem);
	#endif
	
	#if 0 //通过索引获取当前选中音乐文件的文件名
		get_playlist_FileName(this, index, cur_filename);//获取文件名，然后存放到FileName数组
		__wrn("cur_filename = %s...\n",cur_filename);
	#endif
	
	#if 0 //通过音乐文件索引id转换成文件序号
		playlist_get_item_number_size(msg, last_index);
		__wrn(" time last_index the file_number = %s...\n",this->framewin_create_para->file_number);//打印文件序号
	#endif
		
	#if 0 //删除上一个选中的音乐文件矩形区域
		GUI_SetFont(this->framewin_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		//GUI_SetColor(GUI_WHITE);//设置文本颜色，白色
		//GUI_SetBkColor(GUI_INVALID_COLOR);//设置控件背景颜色,蓝色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
		{//选中的矩形显示区域5,9,316,22
			LastFocusItem_Rect.x0 = 5;
			LastFocusItem_Rect.y0 = last_index*32 + 9;
			LastFocusItem_Rect.x1 = 345;
			LastFocusItem_Rect.y1 = last_index*32 + 32;
		}
		GUI_ClearRectEx(&LastFocusItem_Rect);//先删除前面的矩形，然后在下面重新画
	#endif
	
	#if 0 //绘制前一个为未选中的音乐文件空白条目
		GUI_SetFont(this->framewin_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		GUI_SetColor(GUI_WHITE);//设置文本颜色，白色
		//GUI_SetBkColor(GUI_INVALID_COLOR);//设置控件背景颜色,透明
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
		/*{//未选中的矩形显示区域0,0,386,32
			LastFocusItem_Rect.x0 = 0;//this->playlist_uipara->playlist_uipara.list_unfocus.x;
			LastFocusItem_Rect.y0 = last_index*32 + 0;//index*10 + this->playlist_uipara->playlist_uipara.list_unfocus.y;
			LastFocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.list_focus.width;
			LastFocusItem_Rect.y1 = last_index*32 + this->playlist_uipara->playlist_uipara.list_focus.height;
		}*/
		{//选中的矩形显示区域
			LastFocusItem_Rect.x0 = 5;
			LastFocusItem_Rect.y0 = last_index*32 + 9;
			LastFocusItem_Rect.x1 = 345;
			LastFocusItem_Rect.y1 = last_index*32 + 32;
		}
		__wrn("LastFocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",LastFocusItem_Rect.x0,
			LastFocusItem_Rect.y0, LastFocusItem_Rect.x1, LastFocusItem_Rect.y1);
		{
			__wrn("last_filename = %s...\n",this->framewin_create_para->last_filename);
			GUI_DispStringInRect(this->framewin_create_para->last_filename, &LastFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER|GUI_TA_HCENTER);
			//GUI_DispStringAt(FileName, 70, 28);
			GUI_DispStringAt(this->framewin_create_para->file_number, 10, last_index*32 + 8);//直接显示字符串
					
		}
	#endif

	#if 0 //通过音乐文件索引id转换成文件序号
			playlist_get_item_number_size(msg, index);
			__wrn(" time index the file_number = %s...\n",this->framewin_create_para->file_number);//打印文件序号
	#endif
	
	#if 0 //绘制按键上下选中的音乐文件背景条目
		GUI_SetFont(this->framewin_create_para->font);//设置文本
		GUI_UC_SetEncodeUTF8();//设置文本编码格式
		GUI_SetColor(GUI_YELLOW);//设置文本颜色,黄色
		//GUI_SetBkColor(GUI_BLACK);//设置控件背景颜色
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);//设置绘制模式
		/*{//选中的矩形显示区域5,9,316,22
			CurFocusItem_Rect.x0 = this->playlist_uipara->playlist_uipara.list_focus.x;
			CurFocusItem_Rect.y0 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.y;
			CurFocusItem_Rect.x1 = this->playlist_uipara->playlist_uipara.list_focus.width;
			CurFocusItem_Rect.y1 = index*32 + this->playlist_uipara->playlist_uipara.list_focus.height;
		}*/
		{//选中的矩形显示区域
			CurFocusItem_Rect.x0 = 5;
			CurFocusItem_Rect.y0 = index*32 + 9;
			CurFocusItem_Rect.x1 = 340;
			CurFocusItem_Rect.y1 = index*32 + 32;
		}
		__wrn("CurFocusItem_Rect: x0=%d, y0=%d, x1=%d, y1=%d, ...\n",CurFocusItem_Rect.x0,
			CurFocusItem_Rect.y0, CurFocusItem_Rect.x1, CurFocusItem_Rect.y1);
		CurFocus_bmp = dsk_theme_hdl2buf(this->scene_para[0].focus_bmp);
		{
			GUI_BMP_Draw(CurFocus_bmp, CurFocusItem_Rect.x0, CurFocusItem_Rect.y0);//绘制选中的音乐文件条目图片
			//GUI_DispStringAt(FileName, FocusItem_Rect.x0, FocusItem_Rect.y0);
			GUI_DispStringInRect(cur_filename, &CurFocusItem_Rect, GUI_TA_LEFT|GUI_TA_VCENTER|GUI_TA_HCENTER);
			__wrn("cur_filename = %s...\n",cur_filename);
		}
		GUI_DispStringAt(this->framewin_create_para->file_number, 10, index*32 + 8);//直接显示字符串,显示文件序号
	#endif

	#if 0 //文件名和文件索引id保存
		eLIBs_strcpy(this->framewin_create_para->last_filename, cur_filename);//把第一个选中的文件名赋值给last_filename
		last_index = index;//把当前的音乐文件索引id赋值给last_index，保存为上一次的选中文件索引id
		__wrn("last_index = %d...\n",last_index);
	#endif

	#if 1 //通过封装函数绘制上下选择音乐文件条目矩形
		key_updown_focus_ui_paint(msg, this);//绘制上下按键选中音乐文件条目
	#endif
	
	#if MEMDEV_ONOFF//删除伪内存设备
		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
	#endif
	
	return EPDK_OK;
}

//按键处理函数
static __s32 __playlist_key_proc(__gui_msg_t *msg)
{
	static __s32 last_key = -1;
	playlist_ctrl_t	*this;
	this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	__wrn("last_key = %d\n",last_key);
	if(KEY_UP_ACTION == msg->dwAddData2){//第一次进入
		switch(last_key){
			case GUI_MSG_KEY_ENTER:{
				
			}
			break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:{


			}
			break;

			default:{
				break;
			}
		}
	}
	else{
		switch(msg->dwAddData1){
			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT:{
				__wrn("CurFocus_id prev = %d\n",this->framewin_create_para->CurFocus_id);
				//this->framewin_create_para->CurFocus_id -= 1;
				__wrn("CurFocus_id = %d\n",this->framewin_create_para->CurFocus_id);
				/*if(this->framewin_create_para->CurFocus_id < 0){
					this->framewin_create_para->CurFocus_id = 16;
				}*/
				//__playlist_ui_paint_one_item(msg);
				app_new_music_cmd2para(msg->h_deswin, playlist_send_cmd_prev_play, this->framewin_create_para->CurFocus_id, 0);
			}
			break;

			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT:{
				__wrn("CurFocus_id prev = %d\n",this->framewin_create_para->CurFocus_id);
				//this->framewin_create_para->CurFocus_id -= 1;
				__wrn("CurFocus_id = %d\n",this->framewin_create_para->CurFocus_id);
				/*if(this->framewin_create_para->CurFocus_id > 16){
					this->framewin_create_para->CurFocus_id = 0;
				}*/
				//__playlist_ui_paint_one_item(msg);
				app_new_music_cmd2para(msg->h_deswin, playlist_send_cmd_next_play, this->framewin_create_para->CurFocus_id, 0);
			}
			break;

			case GUI_MSG_KEY_ENTER:{

			}
			break;

			case GUI_MSG_KEY_MENU:{/*退出窗口*/
				__gui_msg_t mmsg;
				mmsg.id = GUI_MSG_QUIT;
				mmsg.h_deswin = msg->h_deswin;
				GUI_SendMessage(&mmsg);//向本窗口发送命令
			}
			break;

			default:{
				break;
			}

		}
	}

	if(KEY_UP_ACTION == msg->dwAddData2){
		last_key = -1;
	}
	else{
		last_key = msg->dwAddData1;
	}
	
	return EPDK_OK;
}

//播放列表场景UI创建
static __s32 __playlist_scene_create(__gui_msg_t *msg)
{
	playlist_ctrl_t	*playlist_ctrl;
	playlist_create_para_t	*playlist_create_para;

	playlist_create_para = (playlist_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
	playlist_ctrl = (playlist_ctrl_t *)My_Balloc(sizeof(playlist_ctrl_t));//分配内存空间
	eLIBs_memset(playlist_ctrl, 0, sizeof(playlist_ctrl_t));
	if(!playlist_ctrl){
				__wrn("playlist_ctrl is balloc is failes...\n");
	}
	playlist_ctrl->framewin_create_para = playlist_create_para;//将结构体地址赋值给
	playlist_create_para->playlist_hfrm = msg->h_deswin;//窗口赋值
		
	GUI_WinSetAddData(msg->h_deswin, (__u32)playlist_ctrl);//添加一个数据

	GUI_LyrWinSetSta(playlist_ctrl->framewin_create_para->playlist_hlyr, GUI_LYRWIN_STA_ON);//窗口图层设置开启状态
	GUI_LyrWinSetTop(playlist_ctrl->framewin_create_para->playlist_hlyr);	
			
	#if 1//申请图片资源
	__playlist_uipara_init(msg);
	__wrn("__playlist_ui_paint the layer state is open ok...\n");
	#endif
	#if 1//rat模块初始化
	playlist_rat_init(playlist_ctrl);
	__wrn("playlist_rat_init the layer state is open ok...\n");
	#endif
	#if 1//绘制所有的播放列表
	__playlist_ui_paint_all_item(playlist_ctrl, msg);
	#endif
	return EPDK_OK;
}

//回调函数
static __s32 __playlist_proc(__gui_msg_t *msg)
{
	switch(msg->id){
		__wrn("__playlist_proc ...\n");
		case GUI_MSG_CREATE:{	/*0，创建*/
			__wrn("GUI_MSG_CREATE ...\n");
			__playlist_scene_create(msg);
			//__playlist_ui_paint_one_item(msg);
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{	/*1，销毁*/
			playlist_ctrl_t	*playlist_ctrl;
			playlist_ctrl = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			__wrn("GUI_MSG_DESTROY ...\n");
			playlist_rat_deinit(playlist_ctrl);//rat模块初始化删除
			__playlist_uipara_deinit(msg);//释放图片资源

			if(playlist_ctrl->framewin_create_para){//释放窗口创建的结构体
				__wrn("framewin_create_para is not null...\n");
				eLIBs_memset(playlist_ctrl->framewin_create_para, 0, sizeof(playlist_create_para_t));
				My_Bfree(playlist_ctrl->framewin_create_para, sizeof(playlist_create_para_t));
			}
			if(playlist_ctrl){//释放播放列表总控制结构体
				__wrn("playlist_ctrl is not null...\n");
				eLIBs_memset(playlist_ctrl, 0, sizeof(playlist_ctrl_t));
				My_Bfree(playlist_ctrl, sizeof(playlist_ctrl_t));
			}
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{	/*2，绘制*/
			
		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{	/*3，关闭*/

		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{		/*4，退出*/
			__wrn("__app_new_music_proc GUI_MSG_QUIT  is ok\n");
			app_new_music_cmd2para(msg->h_deswin, send_cmd_playlist_exit, 0, 0);//向
		}
		return EPDK_OK;

		case GUI_MSG_KEY:{		/*5，按键*/
			
			__wrn("key is press...\n");
			__playlist_key_proc(msg);
		
			/*switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{//退出窗口
					__gui_msg_t mmsg;
					mmsg.id = GUI_MSG_QUIT;
					mmsg.h_deswin = msg->h_deswin;
					GUI_SendMessage(&mmsg);//向本窗口发送命令
				}
				break;
			}*/
			
		}
		break;

		case MSG_PLAYLIST_OP_UP:{//选中上一个文件，可从播放器new_music.c文件接收到这命令
			playlist_ctrl_t	*this;
			__u32 playlist_autodisp;//播放列表自动显示，0为直接发送播放命令，1为播放完一首后不发送播放命令
			this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			this->framewin_create_para->CurFocus_id = msg->dwAddData2;//从播放器获取到的当前播放文件索引id
			playlist_autodisp = msg->dwReserved;//等于0更新播放列表并且播放文件，等于1只更新播放列表不发送播放文件命令
			__wrn("playlist_autodisp next  = %d\n",playlist_autodisp);

			__wrn("MSG_PLAYLIST_OP_UP...\n");
			__wrn("CurFocus_id prev = %d\n",this->framewin_create_para->CurFocus_id);
			
			//this->framewin_create_para->CurFocus_id -= 1;
			if(this->framewin_create_para->CurFocus_id < 0){
				this->framewin_create_para->CurFocus_id = 16;
			}
			__wrn("CurFocus_id = %d\n",this->framewin_create_para->CurFocus_id);
			__wrn("playlist_key_prev ...\n");
			__playlist_ui_paint_one_item(msg);
			if(playlist_autodisp == 0){
			app_new_music_cmd2para(msg->h_deswin, send_cmd_play_prev_to_PlayDev, this->framewin_create_para->CurFocus_id, 1);
			}
		}
		return EPDK_OK;

		case MSG_PLAYLIST_OP_DOWN:{//选中下一个文件，可从播放器new_music.c文件接收到这命令
			playlist_ctrl_t	*this;
			__u32 playlist_autodisp;//播放列表自动显示，0为直接发送播放命令，1为播放完一首后不发送播放命令
			this = (playlist_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			this->framewin_create_para->CurFocus_id = msg->dwAddData2;//从播放器获取到的当前播放文件索引id
			playlist_autodisp = msg->dwReserved;//等于0更新播放列表并且播放文件，等于1只更新播放列表不发送播放文件命令
			__wrn("playlist_autodisp next  = %d\n",playlist_autodisp);

			__wrn("MSG_PLAYLIST_OP_DOWN...\n");
			__wrn("CurFocus_id next  = %d\n",this->framewin_create_para->CurFocus_id);
			//this->framewin_create_para->CurFocus_id += 1;
			if(this->framewin_create_para->CurFocus_id > 16){
				this->framewin_create_para->CurFocus_id = 0;
			}
			__wrn("CurFocus_id = %d\n",this->framewin_create_para->CurFocus_id);
			__wrn("playlist_key_next ...\n");
			__playlist_ui_paint_one_item(msg);
			if(playlist_autodisp == 0){//为0，发送播放命令
			app_new_music_cmd2para(msg->h_deswin, send_cmd_play_prev_to_PlayDev, this->framewin_create_para->CurFocus_id, 1);
			}
		}
		return EPDK_OK;
			
		case GUI_MSG_COMMAND:{	/*7，命令*/

		}
		return EPDK_OK;

		case GUI_MSG_TIMER:{/*19,定时器*/
			
		}
		break;

	}



	return GUI_FrmWinDefaultProc(msg);
}


//创建manwin窗口
H_WIN	playlist_frmwin_create(H_WIN h_parent, playlist_create_para_t *para)
{
	__gui_framewincreate_para_t	framewin_para;
	playlist_create_para_t *playlist_create_para;
	FB	fb;
	#if 0 //检测内存是否泄漏
		esKSRV_SysInfo();
	#endif
	GUI_LyrWinGetFB(para->playlist_hlyr, &fb);
	playlist_create_para = (playlist_create_para_t *)My_Balloc(sizeof(playlist_create_para_t));

	eLIBs_memset(playlist_create_para, 0, sizeof(playlist_create_para_t));//结构体初始化，数据清0
	playlist_create_para->font = para->font;//文本
	playlist_create_para->root_type = para->root_type;//TF卡模式
	playlist_create_para->playlist_hlyr = para->playlist_hlyr;//传入图层
	
	__wrn("fb.size.width=%d, fb.size.height=%d \n",fb.size.width, fb.size.height);
	__wrn("h_parent = %x \n",h_parent);
	/*开始操作配置framewin窗口参数*/
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));//结构体初始化，数据清0
	framewin_para.name	=	"playlist_framewin";
	framewin_para.dwExStyle	=	WS_EX_NONE;
	framewin_para.dwStyle	=	WS_NONE | WS_VISIBLE;
	framewin_para.spCaption	=	NULL;
	framewin_para.hOwner	=	NULL;
	framewin_para.hHosting	=	h_parent;//framewin窗口句柄
	framewin_para.id		=	NEW_MUSIC_PLAYLIST_ID;
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__playlist_proc);
	framewin_para.rect.x = 0;
	framewin_para.rect.y = 0;
	framewin_para.rect.width  = fb.size.width;
	framewin_para.rect.height = fb.size.height;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red   = 0;
	framewin_para.BkColor.green	= 0;
	framewin_para.BkColor.blue	= 0;
	framewin_para.attr	=	(void *)playlist_create_para;
	framewin_para.hLayer	=	para->playlist_hlyr;

	__wrn("hlayer = %x \n",framewin_para.hLayer);
	if(!framewin_para.hLayer){
		__wrn("hlayer is null...\n");
		return EPDK_FALSE;
	}
	
	return (GUI_FrmWinCreate(&framewin_para));//返回一个创建好的manwin窗口
	
}

//进度条图层删除
__s32 playlist_ui_scene_delete(void *handle)
{
/*	progressbar_scene_t *scene_para;
	scene_para = (progressbar_scene_t *)handle;//manwin父窗口中用于创建进度条的framewin句柄指针

	if(NULL == handle)//判断是否创建有进度条的framewin窗口句柄
	{
		__wrn("invalid para..\n");
		return EPDK_FAIL;
	}

	if(!scene_para->hfrm)//进度条内部的famewin窗口句柄是否创建
	{
		__wrn("invalid para..\n");
		return EPDK_FAIL;
	}
	__wrn("scene_para->hfrm = %x ok...\n",scene_para->hfrm);
	GUI_FrmWinDelete(scene_para->hfrm);//删除本进度条framewin窗口句柄
	__wrn("scene_para->hlyr = %x ok...\n",scene_para->hlyr);
	if(!scene_para->hlyr)
	{
		__err("invalid para..\n");
		return EPDK_FAIL;
	}

	GUI_LyrWinDelete(scene_para->hlyr);//删除当前的进度条framewin窗口图层
	eLIBs_memset(scene_para, 0, sizeof(progressbar_scene_t));//清空结构体数据
	esMEMS_Bfree(scene_para, sizeof(progressbar_scene_t));//释放该结构体的空间内存*/
	return EPDK_OK;
}


