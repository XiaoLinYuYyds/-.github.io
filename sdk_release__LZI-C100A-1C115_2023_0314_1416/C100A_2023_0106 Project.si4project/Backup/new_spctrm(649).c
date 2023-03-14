/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : spectrum.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#include "beetles_app.h"

#include "new_spectrum.h"

#include "new_spctrm.h"
#include "new_spctrm_uipara.h"

#include "new_spctrm_i.h"


static H_WIN hwin_spectrum = NULL;
static H_LYR hlyr_spectrum = NULL;


static new_spectrum_para_t *spc_para = 0;
//更新频谱图
static void _new_update_spectrum(void)
{
	__u16  temp_level_array[SPECT_GROUP_WIDTH];//一组频谱内的频段数
	__s32  real_level_array[NUM_SPCTRM];//12
	__s32  ret;
	ret = robin_get_spectrum_info(temp_level_array);
	__wrn("_new_update_spectrum ret=%d...\n",ret);
	//ret = dsk_wkm_get_spectrum_info(temp_level_array);
	if(ret)//获取频谱即时信息失败
	{
		__wrn("No spectrum.\n");
		NEW_SPECTRUM_SetValue(hwin_spectrum, NULL);
	}
	else
	{//获取频谱即时信息成功
		__u32  i;
		__wrn("_new_update_spectrum real_level_array...\n");
		for(i = 0;  i < NUM_SPCTRM;  i++)
		{
			real_level_array[i] = temp_level_array[i];
			__msg("real_level_array[%d]=%d...\n",i,real_level_array[i]);
			__msg("temp_level_array[%d]=%d...\n",i,temp_level_array[i]);
		}

		NEW_SPECTRUM_SetValue(hwin_spectrum, real_level_array);
	}
}

//回调处理函数
static __s32 _new_FrameProc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE:
		{
			/*create spctrum*/
			__gui_ctlwincreate_para_t ctlwin_create_info;//创建控制窗口的
			__new_spctrm_uipara_t  *ui_para;
			__new_spctrm_item *spctrm_item;
			__s32 			screen_width;
			__s32 			screen_height;
			dsk_display_get_size(&screen_width, &screen_height);
			ui_para = new_spctrm_get_uipara();//获取频谱图控件的大小
			__here__;
			spctrm_item = esMEMS_Malloc(0, sizeof(__new_spctrm_item));
			__wrn("GUI_MSG_CREATE start...\n");
			if(0 == spctrm_item)
			{
				__wrn("malloc fail...\n");
				return EPDK_FAIL;
			}

			eLIBs_memset(spctrm_item, 0, sizeof(__new_spctrm_item));
			spc_para = esMEMS_Malloc(0, sizeof(new_spectrum_para_t));//分配内存空间

			if(0 == spc_para)
			{
				__wrn("malloc fail...\n");

				if(spctrm_item)
				{
					esMEMS_Mfree(0, spctrm_item);
					spctrm_item = 0;
				}

				return EPDK_FAIL;
			}

			eLIBs_memset(spc_para, 0, sizeof(new_spectrum_para_t));
			spc_para->min = 0;
			spc_para->max = 16;
			spc_para->particle_nr = NUM_SPCTRM;
			/****************************申请频谱图片使用资源*********************************/
			__here__;
			spctrm_item->h_theme_particle_bmp = theme_open(ui_para->partical_bmp_id);
			spctrm_item->h_theme_buoy_bmp = theme_open(ui_para->ball_bmp_id);
			__here__;
			spc_para->particle_bmp_buf = theme_hdl2buf(spctrm_item->h_theme_particle_bmp);//频谱图背景图片
			spc_para->buoy_bmp_buf = theme_hdl2buf(spctrm_item->h_theme_buoy_bmp);//频谱图移动黄色部分图片
			/****************************创建窗口控件配置操作*********************************/
			__here__;
			ctlwin_create_info.name = "new spctrum";		//控件窗口名
			ctlwin_create_info.hParent = msg->h_deswin;		//控件窗口的父窗口句柄
			ctlwin_create_info.spClassName = CTRL_SPECTRUM;	//控件窗口的类名称
			ctlwin_create_info.spCaption = NULL;			//控件窗口的标题
			ctlwin_create_info.dwStyle = WS_VISIBLE;		//控件窗口的风格
			ctlwin_create_info.dwExStyle = WS_NONE;			//控件窗口的扩展风格
			ctlwin_create_info.id = ID_SPCTRUM;				//控件窗口id
			ctlwin_create_info.attr = spc_para;				//控件窗口的私有属性信息
			ctlwin_create_info.ctl_rect.x			= ui_para->para_specctr.x;//控件窗口在父窗口的位置x轴
			;
			ctlwin_create_info.ctl_rect.y			= ui_para->para_specctr.y;//y
			;
			ctlwin_create_info.ctl_rect.width		= ui_para->para_specctr.width;	//宽
			ctlwin_create_info.ctl_rect.height		= ui_para->para_specctr.height; //控件的高
			__wrn("ctl_rect: x=%d, y=%d, w=%d, h=%d...\n",ctlwin_create_info.ctl_rect.x,
				  ctlwin_create_info.ctl_rect.y,ctlwin_create_info.ctl_rect.width,ctlwin_create_info.ctl_rect.height);
			__here__;
			hwin_spectrum = GUI_CtrlWinCreate(&ctlwin_create_info);//控件窗口创建
			if(hwin_spectrum){//判断窗口控件创建成功
				__wrn("hwin_spectrum is success...\n");
			}
			__here__;
			GUI_WinSetAttr(msg->h_deswin, (void *)spctrm_item);		//设置数据
			__here__;
			{
				hlyr_spectrum = GUI_WinGetLyrWin(msg->h_deswin);	//获取控件窗口的图层
				if(hlyr_spectrum){//判断频谱控件图层获取成功
					__wrn("hlyr_spectrum is = %x...\n",hlyr_spectrum);
					GUI_LyrWinSetSta(hlyr_spectrum, GUI_LYRWIN_STA_ON);//窗口图层设置开启状态
					GUI_LyrWinSetTop(hlyr_spectrum);	
		
				}
			}
			//app_print_memory_usage("spectrum create window finished", NULL);
			break;
		}

		case GUI_MSG_COMMAND:
		{
			break;
		}

		case GUI_MSG_PAINT:
		{
			break;
		}

		case GUI_MSG_DESTROY:
		{
			__new_spctrm_item *spctrm_item;
			hwin_spectrum = NULL;
			hlyr_spectrum = NULL;
			//app_print_memory_usage("spectrum start to destroy", NULL);
			spctrm_item = GUI_WinGetAttr(msg->h_deswin);
			__wrn("_new_FrameProc GUI_MSG_DESTROY...\n");
			
			if(spctrm_item)//释放频谱申请使用的图片资源
			{
				theme_close(spctrm_item->h_theme_particle_bmp);
				theme_close(spctrm_item->h_theme_buoy_bmp);
				esMEMS_Mfree(0, spctrm_item);
				spctrm_item = 0;
			}

			if(spc_para)
			{
				esMEMS_Mfree(0, spc_para);
				spc_para = 0;
			}

			//app_print_memory_usage("spectrum window destroy finished", NULL);
			break;
		}

		case GUI_MSG_CLOSE:
		{
			GUI_FrmWinDelete(msg->h_deswin);
			return 0;
		}

		case NEW_SPCTRM_ID_UPDATE:
		{
			__wrn("NEW_SPCTRM_ID_UPDATE...\n");
			_new_update_spectrum();//更新频谱
			break;
		}

		default:
			GUI_FrmWinDefaultProc(msg);
			break;
	}

	return 0;
}

//频谱图控件framewin窗口
H_WIN NEW_SPCTRM_SceneCreate(H_WIN h_parent, H_WIN lyrwin)
{
	__gui_framewincreate_para_t framewin_para;
	__new_spctrm_uipara_t  *ui_para;
	//app_print_memory_usage("spectrum win on create message", NULL);
	ui_para = new_spctrm_get_uipara();
	__wrn("ui_para = %x...\n",ui_para);
	/**************************开始操作framewin窗口配置*******************************/
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	framewin_para.name          = "new spectrum frame win",
	              framewin_para.dwExStyle     = WS_EX_NONE;
	framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
	framewin_para.spCaption     =  NULL;
	framewin_para.hOwner	    = NULL;
	framewin_para.hHosting      = h_parent;
	framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_new_FrameProc);
	framewin_para.rect.x        = ui_para->para_frmwin.x;
	framewin_para.rect.y        = ui_para->para_frmwin.y;
	framewin_para.rect.width    = ui_para->para_frmwin.width;
	framewin_para.rect.height   = ui_para->para_frmwin.height;
	framewin_para.BkColor.alpha = 0;
	framewin_para.BkColor.red   = 0;
	framewin_para.BkColor.green = 255;
	framewin_para.BkColor.blue  = 0;
	framewin_para.attr          =  NULL;
	framewin_para.hLayer        = lyrwin;
	
	__wrn("framewin_para rect: rect.x=%d,rect.y=%d,rect.w=%d,rect.h=%d\n",framewin_para.rect.x,
			framewin_para.rect.y,framewin_para.rect.width,framewin_para.rect.height);
	//GUI_SetBkColor(GUI_BLACK);
	return (GUI_FrmWinCreate(&framewin_para));
}

//频谱图绘制更新
void NEW_SPCTRM_Update(H_WIN hwin)
{
	if(NULL == hlyr_spectrum)
	{
		__wrn("*********NULL == hlyr_spectrum********\n");
		return ;
	}

	if(GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(hlyr_spectrum))
	{
		__wrn("*********GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(hlyr_spectrum)************\n");
		return ;
	}
	__wrn("NEW_SPCTRM_Update 1...\n");
	_new_update_spectrum();
	return;
}

//删除频谱绘制数据
void NEW_SPCTRM_Clear(__u32 value)
{
	__s32  real_level_array[NUM_SPCTRM];

	if(NULL == hwin_spectrum)
	{
		__wrn("********NULL == hwin_spectrum************\n");
		return ;
	}

	eLIBs_memset(real_level_array, value, sizeof(real_level_array));
	NEW_SPECTRUM_SetValue(hwin_spectrum, real_level_array);//设置频谱的值
	return;
}


