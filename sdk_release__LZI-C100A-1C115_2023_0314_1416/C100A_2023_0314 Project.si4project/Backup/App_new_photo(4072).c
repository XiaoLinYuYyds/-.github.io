#include "App_new_photo.h"

#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR
#define COLOUR_TO_8BPP_INDEX_0          0xF0

//创建图层
#define		app_new_photo_8bpp_laye_creat(_lyr, _prt)	app_com_layer_create_default(_lyr, _prt, "new photo  layer")


//命令发送函数
__s32 app_new_photo_cmd2para(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
	__gui_msg_t mymsg;
	__wrn("\n*****************app_new_photo_cmd2para********************\n");
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
static	__s32 new_photo_rat_init(__gui_msg_t *msg)
{
	new_photo_ctrl_t *this;
	this = (new_photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	if(this->rat.rat_handle == NULL){//文件搜索句柄为空打开
		this->rat.rat_handle = rat_open_ex(RAT_TF, RAT_MEDIA_TYPE_PIC, 0);//打开TF卡

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
static __s32 new_photo_ExtractFileName(char *FileName, char *FilePath)
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
static __s32 new_photo_GetListItemFileFullPath(new_photo_ctrl_t *list_para, __s32 ItemIndex, char *FilePath)
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
static __s32 new_photo_GetListItemFileName(__gui_msg_t *msg, __s32 ItemIndex, char *FileName)
{
	int ret;
	char FilePath[RAT_MAX_FULL_PATH_LEN] = "";
	new_photo_ctrl_t *this;
	this = (new_photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	eLIBs_memset(FilePath, 0, sizeof(FilePath));			//数组初始化数据清0
	ret = new_photo_GetListItemFileFullPath(this, ItemIndex, FilePath);//通过索引获取当前条目文件名的全路径,返回EPDK_OK
	__wrn("get List Item File Name is...!!\n");
	if(ret == EPDK_FAIL)
	{
		return EPDK_FAIL;
	}

	new_photo_ExtractFileName(FileName, FilePath);			//从全路径获取文件名
	__wrn("get List Item Extract File Name is ok ok...!!\n");
	
	rat_set_file_for_play(this->rat.rat_handle, FileName);	//需要设置当前文件播放
	return EPDK_OK;
}
#endif

#if 1	//anole模块初始化+图片文件播放
//anole模块初始化
static __s32 ANOLE_Init(__anole_para_t *para)
{
	__s32 ret;
	ret = anole_init();//anole模块初始化
	__wrn("STATIC ANOLE_Init\n");
	if(ret == 0)//anole模块初始化成功
	{
		//init ok
		anole_set_switch_mode_array_for_play(para->switchmodearray, para->switchnum);//设置切换阵列模式播放
		anole_set_switch_mode(para->SwitchMode);//设置切换模式
		anole_set_play_order(para->PlayOrder);	//设置图片播放顺序

		if(para->interval > 15) //for debug
		{
			para->interval = 15;
		}

		anole_set_play_interval(para->interval);//设置播放时间间隔
		//anole_set_show_mode(0);//temp
		PlayCurPhotoFile();						//播放当前图片文件
	}

	return ret;
}
//anole模块释放退出
static void ANOLE_Exit(void)
{
	//DEBUG_CallFuncHere(anole_exit);//调式
	anole_exit();//anole模块退出，关闭图片浏览
}

//设置相册图片显示模式
static __bool ANOLE_SetShowMode(__s32 index)
{
	__bool ret;
	//DEBUG_CallFuncHere(ANOLE_SetShowMode);
	__wrn("ANOLE_SetShowMode(%d)\n", index);
	ret = anole_set_show_mode(index);//设置显示模式
	//ret = 0;
	__here__;
	return ret;
}

//获取相册当前图片文件索引id
static __u32 ANOLE_GetCurPicIndex(void)
{
	__u32 ret = anole_npl_get_cur();//获取当前播放列表的当前播放文件的索引id
	__wrn(" Call %s() = %d\n", Symbol2Str(ANOLE_GetCurPicIndex), ret);//标识符转字符串
	return ret;
}

//获取相册图片总数数目
static __u32 ANOLE_GetPicNum(void)
{
	__u32 ret = anole_npl_get_total_count();//获取当前播放列表的文件总数
	__wrn(" Call %s() = %d\n", Symbol2Str(ANOLE_GetPicNum), ret);//标识符转字符串
	return ret;
}

//获取anole状态
static __anole_sta_t ANOLE_GetSta(void)
{
	//	DEBUG_CallFuncHere(ANOLE_GetSta);
	return anole_get_sta();//获取anole的状态
}

//获取相册图片比例
static __u32 _get_new_photo_ratio(void)
{
	reg_photo_para_t *para;
	get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);//获取注册表
	__wrn("_get_photo_ratio \n");
	if(para)
	{
		return para->ratio;
	}
	else
	{
		__err("dsk_reg_get_para_by_app fail...\n");
		return 0;
	}
}

//播放当前文件
__s32 PlayCurPhotoFile(void)
{
	__s32 ret;
	char Buf[FSYS_DIRNAME_MAX];				//存放文件名
	eLIBs_memset(Buf, 0, sizeof(Buf));
	//ret = anole_npl_playing_seek(index);	//在当前播放列表中通过索引搜索文件
	//__wrn("ret index=%d\n",ret);
	ret = anole_npl_get_cur();				//获取当前播放文件
	__wrn("ret index=%d\n",ret);
	__wrn("PlayCurPhotoFile...\n");
	if(-1 == ret)
	{
		__wrn("anole_npl_get_cur fail...\n");
		return EPDK_FAIL;
	}

	ret = anole_npl_index2file(ret, Buf);	//在当前播放列表中获取与其索引相对应的文件
	__wrn("ret =%d\n", ret);

	if(/*Buf[0] != 0 && */0 == ret)
	{
		anole_play_file(Buf);				//通过文件名直接播放文件
		__wrn("anole_play_file file=%s\n", Buf);
		//ANOLE_ResetRotate();//旋转角度
		//rotate = 0;
	}
	else
	{
		__wrn("ANOLE_PlayCurPhotoFile fail\n");
	}

	return ret;
}
#endif

//下一个图片回调处理函数
__s32 __new_cb_show_next_pic(void *arg_p)
{
	H_WIN hwin;
	new_photo_ctrl_t *photo_ctrl;
	__gui_msg_t  	 setmsg = {0};
	static __s32     OldIndex = -1, OldNum = -1;
	hwin = (H_WIN)arg_p;
	photo_ctrl = (new_photo_ctrl_t *)GUI_WinGetAttr(hwin);
	//ZeroTypeMem(&setmsg, __gui_msg_t);
	__here__;

	/*if(photo_ctrl->lyr_photo_spsc && photo_ctrl->h_photo_spsc && ANOLE_GetSta() == ANOLE_MODE_PLAY)
	{
		setmsg.h_deswin   = photo_ctrl->h_photo_spsc;
		setmsg.id         = MSG_PHOTO_SPSC_SET_NO;
		__here__;
		setmsg.dwAddData1 = ANOLE_GetCurPicIndex();//获取当前播放列表的图片索引id
		__here__;
		setmsg.dwAddData2 = ANOLE_GetPicNum();//获取当前播放列表的图片名
		__here__;

		if(OldIndex != setmsg.dwAddData1 || OldNum != setmsg.dwAddData2)
		{
			__here__;
			photo_spsc_set_no(&setmsg);//GUI_SendNotifyMessage(&setmsg);
			__here__;
			OldIndex = setmsg.dwAddData1;
			OldNum   = setmsg.dwAddData2;
		}
	}*/

	__here__;
	//DEBUG_photo("cur_id = %d, total = %d, sta = %d\n", ANOLE_GetCurPicIndex(), ANOLE_GetPicNum(), ANOLE_GetSta());
	return 0;
}

//显示图片失败回调处理函数
__s32 __new_cb_show_pic_fail(void *arg_p)
{
	new_photo_ctrl_t   *photo_ctrl;

	if(arg_p)
	{
		photo_ctrl = (new_photo_ctrl_t *)GUI_WinGetAttr((H_WIN)arg_p);

		if(photo_ctrl)
		{
			__here__;
			//ShowErrorPic(photo_ctrl->err_pic);
			__here__;
		}
	}

	//	__u8 *buf = (__u8 *)arg_p;
	__wrn("__cb_show_pic_fail...\n");
	return 0;
}

//相册不初始化
static __s32 app_new_photo_uninit(new_photo_ctrl_t *photo_ctrl)
{
	__here__;
	//设置cb“回调函数指针”---图片显示事件：图片显示失败事件、显示下一张图片；回调函数为空
	anole_set_cb(ANOLE_EVENT_SHOW_PIC_FAIL, NULL, (void *)NULL);
	__here__;
	anole_set_cb(ANOLE_EVENT_SHOW_PIC_NEXT, NULL, (void *)NULL);
	__here__;
	ANOLE_Exit();	//anole模块退出，关闭图片浏览
	__inf("photo uninit success!\n");
	//destroy_bmp_res(photo_ctrl->err_pic);//摧毁图片插件，释放图片使用资源
	return EPDK_TRUE;
}
//相册初始化
static __s32 app_new_photo_init(__gui_msg_t *msg)
{
	__anole_para_t 	anole_para;
	__s32			ret;
	new_photo_ctrl_t *new_photo_ctrl;
	new_photo_ctrl = (new_photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
	//	notify_bg_photo(msg->h_deswin, msg->h_deswin, get_photo_bgphoto());
	//PHOTO_LoadData(&anole_para);//加载图片数据
	//create_bmp_res(ID_PHOTO_ERROR_BMP, new_photo_ctrl->err_pic);//创建图片插件，用于申请图片使用资源
	//设置cb“回调函数指针”---图片显示事件：图片显示失败事件、显示下一张图片
	//anole_set_cb(ANOLE_EVENT_SHOW_PIC_FAIL, esKRNL_GetCallBack(__new_cb_show_pic_fail), (void *)(msg->h_deswin));
	//anole_set_cb(ANOLE_EVENT_SHOW_PIC_NEXT, esKRNL_GetCallBack(__new_cb_show_next_pic), (void *)(msg->h_deswin));
	/*initial anole core*/
	ret = ANOLE_Init(&anole_para);				//anole模块初始化

	if(ret == -1)
	{
		__wrn("anole_init failed!\n");
		return EPDK_FAIL;
	}
	/*else if(ret == 1)
	{
		__inf("anole_init failed! ret = 1\n");
		ShowErrorPic(new_photo_ctrl->err_pic);
		return EPDK_FAIL;
	}*/
	new_photo_play_for_index_or_default(msg);		//通过传入文件索引播放图片文件
	ANOLE_SetShowMode(_get_new_photo_ratio());		//设置显示模式
	anole_set_show_mode(WILLOW_IMAGE_SHOW_ORISIZE);	//设置显示模式为以图片原始大小显示
	PlayCurPhotoFile();//播放图片文件
	return EPDK_TRUE;
}

/*通过文件索引id播放文件或者默认从第一个文件开始播放*/
static __s32 new_photo_play_for_index_or_default(__gui_msg_t *msg)
{
	new_photo_ctrl_t *para;
	static __s32 index = 0;
	para = (new_photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);//获取从root中创建manwin窗口时输入的参数数据
					
	#if 1//通过 索引或者直接 播放视频文件
		index = anole_npl_get_cur();					//获取当前播放列表要播放的当前播放文件索引
		__wrn("save breakpoint the index = %d\n",index);
	
		if(para->photo_index != -1)						//不等于-1的时候是从new_explorer列表中跳转到new_movie播放的
		{
			__wrn("para->photo_index = %d\n",para->photo_index);	//播放的文件索引号
			index = para->photo_index;					//获取需要播放的文件索引
			__wrn(" index = %d\n",index);
			//robin_npl_set_cur(index);					//设置当前播放文件
			anole_npl_playing_seek(index);
			para->photo_index = -1;						//清除跳转app标志位
			
		}
		else											//等于-1的时候是直接从home界面进入new_movie播放的
		{
			if(index != 0)								/*默认从头开始播放*/
			{
				index = 0;
				__wrn(" index 2= %d\n",index);
				/*重新设置从头开始播放文件*/
				//robin_npl_set_cur(index);				/*设置当前播放列表要播放的当前播放文件索引id*/
				anole_npl_playing_seek(index);
			}

		}
	#endif
}

//相册回调处理函数
static __s32 __app_new_photo_proc(__gui_msg_t *msg)
{
	__wrn("msg->id = %d\n",msg->id);
	switch(msg->id){
		case GUI_MSG_CREATE:{/*0，创建*/
			char FileName[RAT_MAX_FULL_PATH_LEN];			//存放搜索到的图片文件名
			gscene_bgd_set_state(BGD_STATUS_HIDE);  		//背景场景隐藏
			g_disable_close_scn();							//关闭荧光屏
			com_set_backcolor(CONST_BackColor_Black);		//设置背景颜色为黑色
			com_set_palette_by_id(ID_PHOTO_COLOURTABLE_BMP);//调色板id
			{
				//背景色，文字编辑底色
				__u32 color[] = {COLOUR_BG_8BPP, COLOUR_EDIT_BG_8BPP};
				com_set_palette_color_by_index(COLOUR_TO_8BPP_INDEX_0, color, BEETLES_TBL_SIZE(color));
			}
			#if 1//打开RAT搜索文件模块，并且搜索相册图片类型文件
				eLIBs_memset(FileName, 0, sizeof(FileName));		//数组数据初始化，清0
				new_photo_rat_init(msg);							//rat模块初始化
				new_photo_GetListItemFileName(msg, 25, FileName);	//获取列表项目的图片文件名
				__wrn("\FileName = %s\n",FileName);
				
			#endif
			app_new_photo_init(msg);								//相册初始化
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY:{/*1，销毁*/
			new_photo_ctrl_t *new_photo_ctrl;
			new_photo_ctrl = (new_photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

			g_enable_close_scn();					//使能打开荧光屏
			app_new_photo_uninit(new_photo_ctrl);	//相册不初始化
			eLIBs_memset(new_photo_ctrl, 0, sizeof(new_photo_ctrl_t));	//结构体初始化，数据清0
			esMEMS_Bfree(new_photo_ctrl, sizeof(new_photo_ctrl_t));		//释放结构体内存空间
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{/*2，绘制*/


		}
		return EPDK_OK;

		case GUI_MSG_CLOSE:{/*3，关闭*/
			app_new_photo_cmd2para(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_MOVIE_SW_TO_NEW_EXPLORER, 0);//跳转到new explorer列表应用
			__wrn("GUI_MSG_CLOSE is press frees...\n");
		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{/*4，退出*/
			app_new_photo_cmd2para(msg->h_deswin, NEW_SWITCH_TO_OTHER_APP, NEW_SETTING_SW_TO_MAIN, 0);//跳转到home应用界面
			__wrn("GUI_MSG_QUIT is press frees...\n");
		}
		return EPDK_OK;

		case GUI_MSG_KEY:{/*5，按键*/
			__wrn("__app_new_photo_proc to GUI_MSG_KEY is press...\n");
			switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{//返回键按下
					__gui_msg_t mymsg;
					mymsg.id = GUI_MSG_QUIT;		//发送退出命令
					mymsg.h_deswin = msg->h_deswin;	//选中当前窗口句柄
					GUI_SendMessage(&mymsg);		//发送信息到当前窗口界面
				}
				break;
			}
		}
		break;

		case GUI_MSG_COMMAND:{/*7，命令*/

		}
		return EPDK_OK;
		
		default:
			break;

	}


	return GUI_ManWinDefaultProc(msg);//返回一个manwin回调函数
}

//创建相册应用manwin窗口
H_WIN	app_new_photo_manwin_create(root_para_t *para)
{
	__gui_manwincreate_para_t	create_info;
	new_photo_ctrl_t *new_photo_ctrl;
	H_WIN	hManWin;
	#if 1 //检测内存是否泄漏
		esKSRV_SysInfo();
	#endif
	new_photo_ctrl = (new_photo_ctrl_t *)esMEMS_Balloc(sizeof(new_photo_ctrl_t));
	eLIBs_memset(new_photo_ctrl, 0, sizeof(new_photo_ctrl_t));
	new_photo_ctrl->font = para->font;			//文本
	new_photo_ctrl->root_type = para->root_type;//RAT_TF
	new_photo_ctrl->photo_index = para->data;	//通过列表资源管理器传入的图片文件索引id
	/****************************开始操作配置manwin窗口参数**************************************/
	eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));//结构体初始化，数据清0
	create_info.name	=	APP_NEWPHOTO;
	create_info.hParent	=	para->h_parent;
	create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_new_photo_proc);
	create_info.attr	=	(void *)new_photo_ctrl;
	create_info.id		=	APP_NEWPHOTO_ID;
	create_info.hHosting = NULL;

	hManWin = GUI_ManWinCreate(&create_info);//返回一个创建好的manwin窗口
	if(!hManWin){
		__wrn("manwin create is false...");
		return EPDK_FALSE;
	}

	
	return hManWin;
}




