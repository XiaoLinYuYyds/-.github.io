#include	"new_fm.h"

static __s32 key_updown_flag;//上下按钮按下标志：1为按下一首，2为按上一首
static __s32 is_searching = 0;//搜索标志；1自动、2手动

#if 1 //方法1，通过注册表创建bmp图片资源
/*
************************************************************************************************************************
*                       				__new_fm_freq_scale_create_res
*
*Description: fm的framewin窗口界面申请使用图片资源注册表函数：相当于申请使用图片资源
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 __new_fm_freq_scale_create_res(NEW_FM_FREQ_SCALE_RES_T *res)
{
	create_bmp_res(ID_FM_SCALE_THUMB_N_BMP, res->new_bmp_arrow[0]);	//----创建bmp注册表
	create_bmp_res(ID_FM_SCALE_THUMB_F_BMP, res->new_bmp_arrow[1]);	//游标箭头指示

	create_bmp_res(ID_FM_87FREQSCALE_BMP, res->new_bmp_scale);		//刻度条
	create_bmp_res(ID_FM_87MHZ_BMP, res->new_bmp_87MHz);			//频率87.0MHz
	create_bmp_res(ID_FM_108MHZ_BMP, res->new_bmp_108MHz);			//频率108.0MHz

	create_bmp_res(ID_FM_CURSOR_SEL_BMP, res->new_bmp_cursor_sel);	//选择通道指示条
	create_bmp_res(ID_FM_POINT_BMP, res->new_bmp_freq_point);		//频率显示的小数点
	create_bmp_res(ID_FM_MHZ_BMP, res->new_bmp_freq_MHz);			//频率显示的MHz单位
	create_bmp_res(ID_FM_B0_BMP, res->new_bmp_number_b[0]);			//频率显示的数字：0~9
	create_bmp_res(ID_FM_B1_BMP, res->new_bmp_number_b[1]);
	create_bmp_res(ID_FM_B2_BMP, res->new_bmp_number_b[2]);
	create_bmp_res(ID_FM_B3_BMP, res->new_bmp_number_b[3]);
	create_bmp_res(ID_FM_B4_BMP, res->new_bmp_number_b[4]);
	create_bmp_res(ID_FM_B5_BMP, res->new_bmp_number_b[5]);
	create_bmp_res(ID_FM_B6_BMP, res->new_bmp_number_b[6]);
	create_bmp_res(ID_FM_B7_BMP, res->new_bmp_number_b[7]);
	create_bmp_res(ID_FM_B8_BMP, res->new_bmp_number_b[8]);
	create_bmp_res(ID_FM_B9_BMP, res->new_bmp_number_b[9]);

	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				__new_fm_freq_scale_destroy_res
*
*Description: fm的framewin窗口界面图片资源释放注册表函数：相当于释放使用图片资源
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 __new_fm_freq_scale_destroy_res(NEW_FM_FREQ_SCALE_RES_T *res)
{
	destroy_bmp_res(res->new_bmp_arrow[0]);		//----摧毁bmp注册表
	destroy_bmp_res(res->new_bmp_arrow[1]);		//释放游标箭头指示
	destroy_bmp_res(res->new_bmp_scale);		//释放刻度条
	destroy_bmp_res(res->new_bmp_87MHz);		//释放频率87.0MHz
	destroy_bmp_res(res->new_bmp_108MHz);		//释放频率108.0MHz
	destroy_bmp_res(res->new_bmp_cursor_sel);	//释放选择通道指示条
	destroy_bmp_res(res->new_bmp_freq_point);	//释放频率显示的小数点
	destroy_bmp_res(res->new_bmp_freq_MHz);		//释放频率显示的MHz单位
	destroy_bmp_res(res->new_bmp_number_b[0]);	//释放频率数字：0~9图片资源注册表
	destroy_bmp_res(res->new_bmp_number_b[1]);
	destroy_bmp_res(res->new_bmp_number_b[2]);
	destroy_bmp_res(res->new_bmp_number_b[3]);
	destroy_bmp_res(res->new_bmp_number_b[4]);
	destroy_bmp_res(res->new_bmp_number_b[5]);
	destroy_bmp_res(res->new_bmp_number_b[6]);
	destroy_bmp_res(res->new_bmp_number_b[7]);
	destroy_bmp_res(res->new_bmp_number_b[8]);
	destroy_bmp_res(res->new_bmp_number_b[9]);
	
	

	return EPDK_OK;
}

//源范围中的值_src_val映射到目标范围中
#define RANGE_MAPPING1(_dst_min, _dst_max, _src_min, _src_max, _src_val)  \
	(((_src_val) - (_src_min)) * ((_dst_max) - (_dst_min)) / ((_src_max) - (_src_min)) + (_dst_min))

/*
************************************************************************************************************************
*                       				_new_fm_draw_freq_scale
*
*Description: fm的framewin窗口界面绘制游标卡尺函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/

__s32 _new_fm_draw_freq_scale(H_WIN hparent, H_LYR lyr)
{
	char buf[8 * 2] = {0};
	static int x=0;
	int x1;
	GUI_RECT rt;
	NEW_FM_CTRLDATA_T *fm_ctrl;
	NEW_FM_FREQ_SCALE_RES_T *res;
	fm_ctrl_para_t *wnd_para;
	//new_fm_ctrl *wnd;
	__u8 a = 0, b = 0;
	__u8 c = 0, d = 0;
	__u32 cur_freq;
	__u32 channel_id;
	__u32 channel_count;
	__u32 audiomode;
	const __u16 c_cueHeight = 50;
	RECT scnwin;
#if 0	//存放伪内存变量
		GUI_MEMDEV_Handle draw_mem;
#endif

	GUI_LyrWinGetScnWindow(lyr, &scnwin);//1024,600
	__wrn("scnwin.height = %d, scnwin.width = %d\n",  scnwin.height, scnwin.width);
	NEW_FM_GetWndPara(wnd_para, fm_ctrl_para_t, hparent);//获取私有属性信息数据
	//wnd_para = (fm_ctrl_para_t *)GUI_WinGetAddData(hparent);
	res = &wnd_para->res;						//获取图片注册表数据结构体地址
	fm_ctrl = wnd_para->fm_frmcreate->fm_ctrl2;	//获取FM控制数据结构体地址
	__wrn("res = %d\n", res);
	__wrn("fm_ctrl->cur_freq = %d\n",fm_ctrl->cur_freq);
#if 0	//创建伪内存设备
		draw_mem = GUI_MEMDEV_Create(0, 0, 1024, 600);
		GUI_MEMDEV_Select(draw_mem);
#endif
	
	GUI_ClearRect(0, 0, scnwin.width-1, scnwin.height-c_cueHeight);//删除指定区域，相当于刷新时删除上一次的绘画内容
	__wrn("_new_fm_draw_freq_scale...\n");
	#if 1	//绘制刻度条直线
	{	
		__s32 i;
		GUI_SetColor(APP_COLOR_WHITE + 0x0f);
		for(i = 279; i<= 699; i+=10)
		{
			GUI_DrawLine(i, 302, i, 302 + 12);//画直线
		}
		i = 699;
		GUI_DrawLine(i, 302, i, 302 + 12);
	}
	#endif
	dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//获取当前频率
	#if 1	//绘制当前选择通道频率---指示条图片
		__wrn("res new_bmp_cursor_sel.data = %d\n", res->new_bmp_cursor_sel.data);
		x1 = RANGE_MAPPING1(279, 699, fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq);
		//GUI_BMP_RES_Draw(res->new_bmp_cursor_sel, 279 + x, 302);
		GUI_BMP_RES_Draw(res->new_bmp_cursor_sel, x1, 302);
		__wrn("x1 = %d\n",x1);
		//x += 1;

	#endif

	#if 1 //通过获取到的当前频率转换提取出百、十、个位，绘制频率图片
		//dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//获取当前频率
		//dsk_radio_rcv_search_matching_frep(__u32 freq);//获取搜索结果信息
		//dsk_radio_rcv_get_search_result_chaninfo(__s32 id, dsk_radio_chan_t * chan_info);//获取---搜索结果--频道信息
		__wrn("cur_freq = %d\n",fm_ctrl->cur_freq);
		a = (fm_ctrl->cur_freq) / 100000;			//百位，KHz转MHz
		b = (fm_ctrl->cur_freq) % 100000 / 10000;	//十位
		c = (fm_ctrl->cur_freq) % 10000 / 1000;		//个位
		d = (fm_ctrl->cur_freq) % 1000 / 100;		//小数点后的位
		__wrn("a = %d, b = %d, c = %d, d = %d\n", a, b, c, d);
	#endif
	
	#if 1	//绘制频率和箭头图片
	{
		//GUI_BMP_RES_Draw(res->new_bmp_arrow[0], (279-7) + x, 302-11);//绘制指示箭头
		GUI_BMP_RES_Draw(res->new_bmp_arrow[0], x1-7, 302-11);//绘制指示箭头
		if(a>0) {
			GUI_BMP_RES_Draw(res->new_bmp_number_b[a], 415, 208);	//绘制百位
		}
		GUI_BMP_RES_Draw(res->new_bmp_number_b[b], 435, 208);	//绘制十位
		GUI_BMP_RES_Draw(res->new_bmp_number_b[c], 455, 208);	//绘制个位
		GUI_BMP_RES_Draw(res->new_bmp_freq_point, 475, 228);	//绘制小数点
		GUI_BMP_RES_Draw(res->new_bmp_number_b[d], 480, 208);	//绘制小数点后面的位
		GUI_BMP_RES_Draw(res->new_bmp_freq_MHz, 502, 207);		//绘制MHz

		GUI_BMP_RES_Draw(res->new_bmp_87MHz, 263, 332);
		GUI_BMP_RES_Draw(res->new_bmp_108MHz, 658, 332);
	}
	#endif
	/*if(key_updown_flag == 1){	//按键下一首频率，x轴显示坐标 +2
		x += 2;
		if((279 + x) > 699)
		{
			x = 0;
		}
		key_updown_flag = 0;
	}
	else if(key_updown_flag == 1){	//按键上一首频率，x轴显示坐标 -2
		x -= 2;
		if((279 + x) < 279)
		{
			x = 699;
		}
		key_updown_flag = 0;
	}*/
#if 0	//删除伪内存设备
		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
#endif

	
	return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				fast_draw_freq_scale
*
*Description: fm的framewin窗口界面快速绘制游标卡尺函数
*			  添加了伪内存设备，所以有快速绘制图片功能
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
__s32 fast_draw_freq_scale(__gui_msg_t *msg)
{
	H_LYR layer = GUI_WinGetLyrWin(msg->h_deswin);
	if(GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_ON)
	{

	#if 1	//存放伪内存变量
		GUI_MEMDEV_Handle draw_mem;
	#endif

		GUI_LyrWinSel(layer);//选择图层

	#if 1	//创建伪内存设备
		draw_mem = GUI_MEMDEV_Create(0, 0, 1024, 600);
		GUI_MEMDEV_Select(draw_mem);
	#endif

		{//绘制
			GUI_SetBkColor(0);	//设置背景null
			_new_fm_draw_freq_scale(msg->h_deswin, layer);	//绘制游标卡尺函数
		}

	#if 1	//删除伪内存设备
		if(draw_mem)
		{
			GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
	#endif
	}
}
#endif

#if 1	//方法2 直接申请图片资源
//申请图片资源
int id[10] = {ID_FM_B0_BMP,ID_FM_B1_BMP,ID_FM_B2_BMP,ID_FM_B3_BMP,ID_FM_B4_BMP, 
			ID_FM_B5_BMP,ID_FM_B6_BMP,ID_FM_B7_BMP,ID_FM_B8_BMP,ID_FM_B9_BMP};//频率数字图片资源头句柄
static __s32 __new_fm_uipara_init(__gui_msg_t *msg)
{
	fm_ctrl_para_t *this;
	int i;
	__wrn("__new_fm_uipara_init \n");
	this = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);
	//this = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);
	if(!this->bmp_uipara.theme_arrow[0]){
		this->bmp_uipara.theme_arrow[0] = theme_open(ID_FM_SCALE_THUMB_N_BMP);
		this->bmp_uipara.theme_arrow[1] = theme_open(ID_FM_SCALE_THUMB_F_BMP);
	}
	if(!this->bmp_uipara.theme_scale){
		this->bmp_uipara.theme_scale = theme_open(ID_FM_87FREQSCALE_BMP);
		__wrn("theme_scale = %d \n", this->bmp_uipara.theme_scale);
	}
	if(!this->bmp_uipara.theme_87MHz){
		this->bmp_uipara.theme_87MHz = theme_open(ID_FM_87MHZ_BMP);
	}
	if(!this->bmp_uipara.theme_cursor_sel){
		this->bmp_uipara.theme_cursor_sel = theme_open(ID_FM_CURSOR_SEL_BMP);
	}
	if(!this->bmp_uipara.theme_freq_point){
		this->bmp_uipara.theme_freq_point = theme_open(ID_FM_POINT_BMP);
	}
	if(!this->bmp_uipara.theme_freq_MHz){
		this->bmp_uipara.theme_freq_MHz = theme_open(ID_FM_MHZ_BMP);
	}
	for(i = 0; i < 10; i++){
		if(!this->bmp_uipara.theme_number_b[i]){

			__wrn("id[%d] = %d \n", i, id[i]);
			this->bmp_uipara.theme_number_b[i] = theme_open(id[i]);
			__wrn("theme_number_b[%d] = %d \n", i, this->bmp_uipara.theme_number_b[i]);
		}
	}
}

//释放图片资源
static __s32 _new_fm_uipara_uninit(__gui_msg_t *msg)
{
	fm_ctrl_para_t *this;
	int i;
	__wrn("__new_fm_uipara_uninit \n");
	this = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);
	//this = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);
	if(this->bmp_uipara.theme_arrow[0]) {
		theme_close(this->bmp_uipara.theme_arrow[0]);
		this->bmp_uipara.theme_arrow[0] = NULL;
		theme_close(this->bmp_uipara.theme_arrow[1]);
		this->bmp_uipara.theme_arrow[1] = NULL;
	}
	if(this->bmp_uipara.theme_scale) {
		theme_close(this->bmp_uipara.theme_scale);
		this->bmp_uipara.theme_scale = NULL;
	}
	if(this->bmp_uipara.theme_87MHz) {
		theme_close(this->bmp_uipara.theme_87MHz);
		this->bmp_uipara.theme_87MHz = NULL;
	}
	if(this->bmp_uipara.theme_cursor_sel) {
		theme_close(this->bmp_uipara.theme_cursor_sel);
		this->bmp_uipara.theme_cursor_sel = NULL;
	}
	if(this->bmp_uipara.theme_freq_point) {
		theme_close(this->bmp_uipara.theme_freq_point);
		this->bmp_uipara.theme_freq_point = NULL;
	}
	if(this->bmp_uipara.theme_freq_MHz) {
		theme_close(this->bmp_uipara.theme_freq_MHz);
		this->bmp_uipara.theme_freq_MHz = NULL;
	}
	for(i = 0; i < 10; i++){
		if(this->bmp_uipara.theme_number_b[i]) {
			//__wrn("id[%d] = %d \n", i, id[i]);
			theme_close(this->bmp_uipara.theme_number_b[i]);
			this->bmp_uipara.theme_number_b[i] = NULL;
		}
	}

}

//绘制图片
static __s32 __new_fm_ui_paint_all(__gui_msg_t *msg)
{
	fm_ctrl_para_t	*this;
	GUI_RECT	rect_scale;//存放游标卡尺、刻度表
	void *scale_bmp;
	void *sel_bmp;
	#if 1	//存放伪内存变量
		GUI_MEMDEV_Handle draw_mem;
	#endif
	
	this = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);
	//this = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);

	GUI_LyrWinSel(this->fm_frmcreate->hlyr);//选择图层
	#if 1	//创建伪内存设备
		draw_mem = GUI_MEMDEV_Create(0, 0, 1024, 600);
		GUI_MEMDEV_Select(draw_mem);
	#endif

		#if 1	//绘制游标卡尺、刻度尺图片
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//设置绘制模式
			{
				rect_scale.x0 = 279;
				rect_scale.y0 = 302;
				rect_scale.x1 = 699;
				rect_scale.y1 = 302+12;
				scale_bmp = dsk_theme_hdl2buf(this->bmp_uipara.theme_scale);	//获取图片资源数据
				sel_bmp = dsk_theme_hdl2buf(this->bmp_uipara.theme_cursor_sel);
				__wrn("scale_bmp = %d \n", scale_bmp);
				{
					GUI_BMP_Draw(scale_bmp, 279, 302);//绘制图片
					GUI_BMP_Draw(sel_bmp, 279, 302);
				}
			}
		#endif
		
	#if 1	//删除伪内存设备
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
#endif

/*
************************************************************************************************************************
*                       				__new_fm_frmwin_proc
*
*Description: fm的framewin窗口定时器处理函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32 __new_fm_install_ui_timer()
{

}

/*
************************************************************************************************************************
*                       				__new_fm_frmwin_proc
*
*Description: fm的framewin窗口按键处理函数
*
*Arguments  : 
*            
*
*Return     : 
*
************************************************************************************************************************
*/
static __s32  __new_fm_key_proc(__gui_msg_t *msg)
{
	static __s32 key_last = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		switch(key_last)
		{
			case GUI_MSG_KEY_ENTER:{//自动搜索---开始
				__wrn("enter new fm framewin..\n");
				is_searching = 2;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_ENTER, is_searching, 0);
			}
			break;
			
			case GUI_MSG_KEY_LONGENTER:{//自动搜索---开始
				__wrn("enter new fm framewin..\n");
				is_searching = 1;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_ENTER, is_searching, 0);
			}
			break;

			case GUI_MSG_KEY_MENU:{//退出应用---
				__wrn("quit new fm framewin..\n");
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_EXIT, 0, 0);
			}
			break;
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
			case GUI_MSG_KEY_ENTER:{

			}
			break;

			case GUI_MSG_KEY_RIGHT:
			case GUI_MSG_KEY_LONGRIGHT: {//下一首频率，播放
				H_LYR layer;
				__wrn("right new fm framewin..\n");
				key_updown_flag = 1;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_RIGHT, 0, 0);
				layer = GUI_WinGetLyrWin(msg->h_deswin);		//获取窗口的图层
				//_new_fm_draw_freq_scale(msg->h_deswin, layer);//绘制bmp图片
				//fast_draw_freq_scale(msg->h_deswin,  layer);	//快速绘制bmp图片
				fast_draw_freq_scale(msg);	//快速绘制bmp图片
			}
			break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT: {//上一首频率，播放
				H_LYR layer;
				__wrn("right new fm framewin..\n");
				key_updown_flag = 2;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_LEFT, 0, 0);
				layer = GUI_WinGetLyrWin(msg->h_deswin);		//获取窗口的图层
				//_new_fm_draw_freq_scale(msg->h_deswin, layer);//绘制bmp图片
				//fast_draw_freq_scale(msg->h_deswin,  layer);	//快速绘制bmp图片
				fast_draw_freq_scale(msg);	//快速绘制bmp图片
			}
			break;

			case GUI_MSG_KEY_MENU:{

			}
			break;
		}
	}

	if(KEY_UP_ACTION == msg->dwAddData2) {
		key_last = -1;
	}
	else {
		key_last = msg->dwAddData1;
	}

}




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
			fm_ctrl_para_t *fm_ctrl;
			fm_scene_t *fm_create;
			H_LYR layer;
			fm_create = (fm_scene_t *)GUI_WinGetAttr(msg->h_deswin);
			fm_ctrl = (fm_ctrl_para_t *)My_Balloc(sizeof(fm_ctrl_para_t));
			eLIBs_memset(fm_ctrl, 0, sizeof(fm_ctrl_para_t));
			
			fm_ctrl->fm_frmcreate = fm_create;					//结构体数据赋值
			GUI_WinSetAttr(msg->h_deswin, (void *) fm_ctrl);	//设置一个私有属性信息
			//GUI_WinSetAddData(msg->h_deswin, (__u32) fm_ctrl);	//设置添加一个数据
		#if 0	//作用一样，打开图层、图层置顶
			GUI_LyrWinSetSta(fm_ctrl->fm_frmcreate->hlyr, GUI_LYRWIN_STA_ON);	//图层状态打开
			GUI_LyrWinSetTop(fm_ctrl->fm_frmcreate->hlyr);	//图层置顶
		#else
			_app_show_layer(fm_ctrl->fm_frmcreate->hlyr);
		#endif
		
		 #if 1//方法1 通过注册表的方式申请图片资源
		 {
			 __new_fm_freq_scale_create_res(&fm_ctrl->res);	//bmp图片注册表创建
			 layer = GUI_WinGetLyrWin(msg->h_deswin);		//获取窗口的图层
			 __wrn("layer = %d\n",layer);
			 __wrn("hlyr = %d\n",fm_ctrl->fm_frmcreate->hlyr);
			 //GUI_SetBkColor(0);//设置背景
			 //_new_fm_draw_freq_scale(msg->h_deswin, layer);//绘制bmp图片
			 //fast_draw_freq_scale(msg->h_deswin,  layer);//快速绘制bmp图片
			 fast_draw_freq_scale(msg);	//快速绘制bmp图片
			 //_new_fm_draw_freq_scale(fm_ctrl->fm_frmcreate->hfrm, fm_ctrl->fm_frmcreate->hlyr, &fm_ctrl->res);
		 }
		 #endif

		 #if 0 //方法2 通过直接调用图片申请API接口方式
		 {
			__new_fm_uipara_init(msg);//申请图片资源
			__wrn("__new_fm_ui_paint_all\n");
			__new_fm_ui_paint_all(msg);//绘制图片
		 }
		 #endif
			 
			
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY: { //1,
			fm_ctrl_para_t *wnd;
			__wrn("DESTROY new fm framewin..\n");
			wnd = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);
			//wnd = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);
			//_new_fm_uipara_uninit(msg);				//方法2、释放图片资源
			__new_fm_freq_scale_destroy_res(&wnd->res);	//方法1、释放bmp注册表
			app_new_fm_framewin_delete(msg);			//删除本窗口结构体数据
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{
			fast_draw_freq_scale(msg);	//快速绘制bmp图片
		}
		return EPDK_OK;

		case GUI_MSG_QUIT:{
			__wrn("quit new fm framewin..\n");
			app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_EXIT, 0, 0);
		}
		return EPDK_OK;
		
		case GUI_MSG_KEY: { //5,

			__new_fm_key_proc(msg);
		}
		break;

		case NEW_CMD_AUTOSRH_FINDCH: { 		//自动搜索状态下搜索到一个频点
			NEW_FM_UpdateUI(msg->h_deswin);	//更新UI数据
		}
		return EPDK_OK;

		case NEW_CMD_AUTOSRH_FINDCHFAIL: { 	//自动搜索所有
			NEW_FM_UpdateUI(msg->h_deswin);	//更新UI数据,使用这个API接口会更新到GUI_MSG_PAINT：绘制命令
		}
		return EPDK_OK;

		case NEW_CMD_AUTOSRH_OVER: {		//自动搜索完成
			NEW_FM_UpdateUI(msg->h_deswin);	//更新UI数据
		}
		return EPDK_OK;

		
	}

	return GUI_FrmWinDefaultProc(msg);
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
	fm_scene_t	*frm_create;
	FB fb;
	GUI_LyrWinGetFB(para->fm_frmlyr, &fb);
	frm_create = (fm_scene_t *)My_Balloc(sizeof(fm_scene_t));
	eLIBs_memset(frm_create, 0, sizeof(fm_scene_t));

	frm_create->hfrm = para->fm_frmwin;
	frm_create->hlyr = para->fm_frmlyr;
	frm_create->fm_ctrl2 = para->fm_ctrl1;
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
	//this = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);
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
		if(this->fm_frmcreate){//
			eLIBs_memset(this->fm_frmcreate, 0, sizeof(fm_scene_t));
			My_Bfree(this->fm_frmcreate, sizeof(fm_scene_t));
		}
		
		eLIBs_memset(this, 0, sizeof(fm_ctrl_para_t));//先初始化，清0
		My_Bfree(this, sizeof(fm_ctrl_para_t));//释放内存空间
	}

}











