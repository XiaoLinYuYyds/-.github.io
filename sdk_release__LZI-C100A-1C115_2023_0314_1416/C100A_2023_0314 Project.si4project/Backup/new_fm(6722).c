#include	"new_fm.h"

























/*
************************************************************************************************************************
*                       				__new_fm_frmwin_proc
*
*Description: fm的framewin窗口创建回调处理函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_frmwin_proc(__gui_msg_t *msg)
{
	switch(msg->id)
	{
		case GUI_MSG_CREATE: {	//0,

		}
		return EPDK_OK;

		case GUI_MSG_DESTROY: { //1,
			app_new_fm_framewin_delete(msg);
		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{
			__wrn("quit new fm framewin..\n");
			app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_EXIT, 0, 0);
		}
		return EPDK_OK;
		
		case GUI_MSG_KEY: { //5,

			switch(msg->dwAddData1){
				case GUI_MSG_KEY_MENU:{
					__gui_msg_t mymsg;
					mymsg.id = GUI_MSG_QUIT;
					mymsg.h_deswin = msg->h_deswin;
					GUI_SendMessage(&mymsg);
				}
				break;
			}
		}
		break;
	}


}

/*
************************************************************************************************************************
*                       				new_fm_frmwin_create
*
*Description: fm的framewin窗口创建
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
H_WIN	new_fm_frmwin_create(H_WIN hparent, fm_create_para_t *para)
{
	__gui_framewincreate_para_t		framewin_info;
	fm_ctrl_para_t	*frm_create;
	FB fb;
	GUI_LyrWinGetFB(para->fm_frmlyr, &fb);
	frm_create = (fm_ctrl_para_t *)My_Balloc(sizeof(fm_ctrl_para_t));
	eLIBs_memset(frm_create, 0, sizeof(fm_ctrl_para_t));

	frm_create->fm_frmcreate->hfrm = para->fm_frmwin;
	frm_create->fm_frmcreate->hlyr = para->fm_frmlyr;
	/////////////开始操作framewin窗口配置///////////////
	eLIBs_memset(&framewin_info, 0, sizeof(__gui_framewincreate_para_t));
	framewin_info.name		= "new_fm_frmwin";
	framewin_info.dwExStyle	= WS_EX_NONE;
	framewin_info.dwStyle	= WS_NONE | WS_VISIBLE;
	framewin_info.spCaption	= NULL;
	framewin_info.hOwner	= NULL;
	framewin_info.hHosting	= hparent;
	framewin_info.id		= NEW_FM_ID;
	framewin_info.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__new_fm_frmwin_proc);
	framewin_info.rect.x	= 0;
	framewin_info.rect.y	= 0;
	framewin_info.rect.width	= fb.size.width;
	framewin_info.rect.height	= fb.size.height;
	framewin_info.BkColor.red	= 0;
	framewin_info.BkColor.green	= 0;
	framewin_info.BkColor.blue	= 0;
	framewin_info.BkColor.alpha	= 0;
	framewin_info.attr		= (void *)frm_create;
	framewin_info.hLayer	= para->fm_frmlyr;

	
	return (GUI_FrmWinCreate(&framewin_info));
}

/*
************************************************************************************************************************
*                       				app_new_fm_framewin_delete
*
*Description: fm的framewin窗口删除
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 app_new_fm_framewin_delete(__gui_msg_t *msg)
{
	fm_ctrl_para_t	*this;

	this = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);

	/*if(this->fm_framewin){
		GUI_FrmWinDelete(this->fm_framewin);
		__wrn("fm_framewin is delete success...\n");
		this->fm_framewin = 0;
	}

	if(this->fm_lyr){
		GUI_LyrWinDelete(this->fm_lyr);
		this->fm_lyr = 0;
		__wrn("fm_lyr is delete success...\n");
	}*/

	if(this){
		eLIBs_memset(this, 0, sizeof(fm_ctrl_para_t));//先初始化，清0
		My_Bfree(this, sizeof(fm_ctrl_para_t));//释放内存空间
	}

}











