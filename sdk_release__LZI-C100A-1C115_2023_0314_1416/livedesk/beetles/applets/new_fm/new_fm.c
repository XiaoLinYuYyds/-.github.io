#include	"new_fm.h"

static __s32 key_updown_flag;//���°�ť���±�־��1Ϊ����һ�ף�2Ϊ����һ��
static __s32 is_searching = 0;//������־��1�Զ���2�ֶ�

#if 1 //����1��ͨ��ע�����bmpͼƬ��Դ
/*
************************************************************************************************************************
*                       				__new_fm_freq_scale_create_res
*
*Description: fm��framewin���ڽ�������ʹ��ͼƬ��Դע��������൱������ʹ��ͼƬ��Դ
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
	create_bmp_res(ID_FM_SCALE_THUMB_N_BMP, res->new_bmp_arrow[0]);	//----����bmpע���
	create_bmp_res(ID_FM_SCALE_THUMB_F_BMP, res->new_bmp_arrow[1]);	//�α��ͷָʾ

	create_bmp_res(ID_FM_87FREQSCALE_BMP, res->new_bmp_scale);		//�̶���
	create_bmp_res(ID_FM_87MHZ_BMP, res->new_bmp_87MHz);			//Ƶ��87.0MHz
	create_bmp_res(ID_FM_108MHZ_BMP, res->new_bmp_108MHz);			//Ƶ��108.0MHz

	create_bmp_res(ID_FM_CURSOR_SEL_BMP, res->new_bmp_cursor_sel);	//ѡ��ͨ��ָʾ��
	create_bmp_res(ID_FM_POINT_BMP, res->new_bmp_freq_point);		//Ƶ����ʾ��С����
	create_bmp_res(ID_FM_MHZ_BMP, res->new_bmp_freq_MHz);			//Ƶ����ʾ��MHz��λ
	create_bmp_res(ID_FM_B0_BMP, res->new_bmp_number_b[0]);			//Ƶ����ʾ�����֣�0~9
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
*Description: fm��framewin���ڽ���ͼƬ��Դ�ͷ�ע��������൱���ͷ�ʹ��ͼƬ��Դ
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
	destroy_bmp_res(res->new_bmp_arrow[0]);		//----�ݻ�bmpע���
	destroy_bmp_res(res->new_bmp_arrow[1]);		//�ͷ��α��ͷָʾ
	destroy_bmp_res(res->new_bmp_scale);		//�ͷſ̶���
	destroy_bmp_res(res->new_bmp_87MHz);		//�ͷ�Ƶ��87.0MHz
	destroy_bmp_res(res->new_bmp_108MHz);		//�ͷ�Ƶ��108.0MHz
	destroy_bmp_res(res->new_bmp_cursor_sel);	//�ͷ�ѡ��ͨ��ָʾ��
	destroy_bmp_res(res->new_bmp_freq_point);	//�ͷ�Ƶ����ʾ��С����
	destroy_bmp_res(res->new_bmp_freq_MHz);		//�ͷ�Ƶ����ʾ��MHz��λ
	destroy_bmp_res(res->new_bmp_number_b[0]);	//�ͷ�Ƶ�����֣�0~9ͼƬ��Դע���
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

//Դ��Χ�е�ֵ_src_valӳ�䵽Ŀ�귶Χ��
#define RANGE_MAPPING1(_dst_min, _dst_max, _src_min, _src_max, _src_val)  \
	(((_src_val) - (_src_min)) * ((_dst_max) - (_dst_min)) / ((_src_max) - (_src_min)) + (_dst_min))

/*
************************************************************************************************************************
*                       				_new_fm_draw_freq_scale
*
*Description: fm��framewin���ڽ�������α꿨�ߺ���
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
#if 0	//���α�ڴ����
		GUI_MEMDEV_Handle draw_mem;
#endif

	GUI_LyrWinGetScnWindow(lyr, &scnwin);//1024,600
	__wrn("scnwin.height = %d, scnwin.width = %d\n",  scnwin.height, scnwin.width);
	NEW_FM_GetWndPara(wnd_para, fm_ctrl_para_t, hparent);//��ȡ˽��������Ϣ����
	//wnd_para = (fm_ctrl_para_t *)GUI_WinGetAddData(hparent);
	res = &wnd_para->res;						//��ȡͼƬע������ݽṹ���ַ
	fm_ctrl = wnd_para->fm_frmcreate->fm_ctrl2;	//��ȡFM�������ݽṹ���ַ
	__wrn("res = %d\n", res);
	__wrn("fm_ctrl->cur_freq = %d\n",fm_ctrl->cur_freq);
#if 0	//����α�ڴ��豸
		draw_mem = GUI_MEMDEV_Create(0, 0, 1024, 600);
		GUI_MEMDEV_Select(draw_mem);
#endif
	
	GUI_ClearRect(0, 0, scnwin.width-1, scnwin.height-c_cueHeight);//ɾ��ָ�������൱��ˢ��ʱɾ����һ�εĻ滭����
	__wrn("_new_fm_draw_freq_scale...\n");
	#if 1	//���ƿ̶���ֱ��
	{	
		__s32 i;
		GUI_SetColor(APP_COLOR_WHITE + 0x0f);
		for(i = 279; i<= 699; i+=10)
		{
			GUI_DrawLine(i, 302, i, 302 + 12);//��ֱ��
		}
		i = 699;
		GUI_DrawLine(i, 302, i, 302 + 12);
	}
	#endif
	dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//��ȡ��ǰƵ��
	#if 1	//���Ƶ�ǰѡ��ͨ��Ƶ��---ָʾ��ͼƬ
		__wrn("res new_bmp_cursor_sel.data = %d\n", res->new_bmp_cursor_sel.data);
		x1 = RANGE_MAPPING1(279, 699, fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq);
		//GUI_BMP_RES_Draw(res->new_bmp_cursor_sel, 279 + x, 302);
		GUI_BMP_RES_Draw(res->new_bmp_cursor_sel, x1, 302);
		__wrn("x1 = %d\n",x1);
		//x += 1;

	#endif

	#if 1 //ͨ����ȡ���ĵ�ǰƵ��ת����ȡ���١�ʮ����λ������Ƶ��ͼƬ
		//dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//��ȡ��ǰƵ��
		//dsk_radio_rcv_search_matching_frep(__u32 freq);//��ȡ���������Ϣ
		//dsk_radio_rcv_get_search_result_chaninfo(__s32 id, dsk_radio_chan_t * chan_info);//��ȡ---�������--Ƶ����Ϣ
		__wrn("cur_freq = %d\n",fm_ctrl->cur_freq);
		a = (fm_ctrl->cur_freq) / 100000;			//��λ��KHzתMHz
		b = (fm_ctrl->cur_freq) % 100000 / 10000;	//ʮλ
		c = (fm_ctrl->cur_freq) % 10000 / 1000;		//��λ
		d = (fm_ctrl->cur_freq) % 1000 / 100;		//С������λ
		__wrn("a = %d, b = %d, c = %d, d = %d\n", a, b, c, d);
	#endif
	
	#if 1	//����Ƶ�ʺͼ�ͷͼƬ
	{
		//GUI_BMP_RES_Draw(res->new_bmp_arrow[0], (279-7) + x, 302-11);//����ָʾ��ͷ
		GUI_BMP_RES_Draw(res->new_bmp_arrow[0], x1-7, 302-11);//����ָʾ��ͷ
		if(a>0) {
			GUI_BMP_RES_Draw(res->new_bmp_number_b[a], 415, 208);	//���ư�λ
		}
		GUI_BMP_RES_Draw(res->new_bmp_number_b[b], 435, 208);	//����ʮλ
		GUI_BMP_RES_Draw(res->new_bmp_number_b[c], 455, 208);	//���Ƹ�λ
		GUI_BMP_RES_Draw(res->new_bmp_freq_point, 475, 228);	//����С����
		GUI_BMP_RES_Draw(res->new_bmp_number_b[d], 480, 208);	//����С��������λ
		GUI_BMP_RES_Draw(res->new_bmp_freq_MHz, 502, 207);		//����MHz

		GUI_BMP_RES_Draw(res->new_bmp_87MHz, 263, 332);
		GUI_BMP_RES_Draw(res->new_bmp_108MHz, 658, 332);
	}
	#endif
	/*if(key_updown_flag == 1){	//������һ��Ƶ�ʣ�x����ʾ���� +2
		x += 2;
		if((279 + x) > 699)
		{
			x = 0;
		}
		key_updown_flag = 0;
	}
	else if(key_updown_flag == 1){	//������һ��Ƶ�ʣ�x����ʾ���� -2
		x -= 2;
		if((279 + x) < 279)
		{
			x = 699;
		}
		key_updown_flag = 0;
	}*/
#if 0	//ɾ��α�ڴ��豸
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
*Description: fm��framewin���ڽ�����ٻ����α꿨�ߺ���
*			  �����α�ڴ��豸�������п��ٻ���ͼƬ����
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

	#if 1	//���α�ڴ����
		GUI_MEMDEV_Handle draw_mem;
	#endif

		GUI_LyrWinSel(layer);//ѡ��ͼ��

	#if 1	//����α�ڴ��豸
		draw_mem = GUI_MEMDEV_Create(0, 0, 1024, 600);
		GUI_MEMDEV_Select(draw_mem);
	#endif

		{//����
			GUI_SetBkColor(0);	//���ñ���null
			_new_fm_draw_freq_scale(msg->h_deswin, layer);	//�����α꿨�ߺ���
		}

	#if 1	//ɾ��α�ڴ��豸
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

#if 1	//����2 ֱ������ͼƬ��Դ
//����ͼƬ��Դ
int id[10] = {ID_FM_B0_BMP,ID_FM_B1_BMP,ID_FM_B2_BMP,ID_FM_B3_BMP,ID_FM_B4_BMP, 
			ID_FM_B5_BMP,ID_FM_B6_BMP,ID_FM_B7_BMP,ID_FM_B8_BMP,ID_FM_B9_BMP};//Ƶ������ͼƬ��Դͷ���
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

//�ͷ�ͼƬ��Դ
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

//����ͼƬ
static __s32 __new_fm_ui_paint_all(__gui_msg_t *msg)
{
	fm_ctrl_para_t	*this;
	GUI_RECT	rect_scale;//����α꿨�ߡ��̶ȱ�
	void *scale_bmp;
	void *sel_bmp;
	#if 1	//���α�ڴ����
		GUI_MEMDEV_Handle draw_mem;
	#endif
	
	this = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);
	//this = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);

	GUI_LyrWinSel(this->fm_frmcreate->hlyr);//ѡ��ͼ��
	#if 1	//����α�ڴ��豸
		draw_mem = GUI_MEMDEV_Create(0, 0, 1024, 600);
		GUI_MEMDEV_Select(draw_mem);
	#endif

		#if 1	//�����α꿨�ߡ��̶ȳ�ͼƬ
			GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);	//���û���ģʽ
			{
				rect_scale.x0 = 279;
				rect_scale.y0 = 302;
				rect_scale.x1 = 699;
				rect_scale.y1 = 302+12;
				scale_bmp = dsk_theme_hdl2buf(this->bmp_uipara.theme_scale);	//��ȡͼƬ��Դ����
				sel_bmp = dsk_theme_hdl2buf(this->bmp_uipara.theme_cursor_sel);
				__wrn("scale_bmp = %d \n", scale_bmp);
				{
					GUI_BMP_Draw(scale_bmp, 279, 302);//����ͼƬ
					GUI_BMP_Draw(sel_bmp, 279, 302);
				}
			}
		#endif
		
	#if 1	//ɾ��α�ڴ��豸
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
*Description: fm��framewin���ڶ�ʱ��������
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
*Description: fm��framewin���ڰ���������
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
			case GUI_MSG_KEY_ENTER:{//�Զ�����---��ʼ
				__wrn("enter new fm framewin..\n");
				is_searching = 2;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_ENTER, is_searching, 0);
			}
			break;
			
			case GUI_MSG_KEY_LONGENTER:{//�Զ�����---��ʼ
				__wrn("enter new fm framewin..\n");
				is_searching = 1;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_ENTER, is_searching, 0);
			}
			break;

			case GUI_MSG_KEY_MENU:{//�˳�Ӧ��---
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
			case GUI_MSG_KEY_LONGRIGHT: {//��һ��Ƶ�ʣ�����
				H_LYR layer;
				__wrn("right new fm framewin..\n");
				key_updown_flag = 1;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_RIGHT, 0, 0);
				layer = GUI_WinGetLyrWin(msg->h_deswin);		//��ȡ���ڵ�ͼ��
				//_new_fm_draw_freq_scale(msg->h_deswin, layer);//����bmpͼƬ
				//fast_draw_freq_scale(msg->h_deswin,  layer);	//���ٻ���bmpͼƬ
				fast_draw_freq_scale(msg);	//���ٻ���bmpͼƬ
			}
			break;

			case GUI_MSG_KEY_LEFT:
			case GUI_MSG_KEY_LONGLEFT: {//��һ��Ƶ�ʣ�����
				H_LYR layer;
				__wrn("right new fm framewin..\n");
				key_updown_flag = 2;
				app_new_fm_cmd2parent(msg->h_deswin, MSG_NEW_FM_OP_LEFT, 0, 0);
				layer = GUI_WinGetLyrWin(msg->h_deswin);		//��ȡ���ڵ�ͼ��
				//_new_fm_draw_freq_scale(msg->h_deswin, layer);//����bmpͼƬ
				//fast_draw_freq_scale(msg->h_deswin,  layer);	//���ٻ���bmpͼƬ
				fast_draw_freq_scale(msg);	//���ٻ���bmpͼƬ
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
*Description: fm��framewin���ڴ����ص�������
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
			
			fm_ctrl->fm_frmcreate = fm_create;					//�ṹ�����ݸ�ֵ
			GUI_WinSetAttr(msg->h_deswin, (void *) fm_ctrl);	//����һ��˽��������Ϣ
			//GUI_WinSetAddData(msg->h_deswin, (__u32) fm_ctrl);	//�������һ������
		#if 0	//����һ������ͼ�㡢ͼ���ö�
			GUI_LyrWinSetSta(fm_ctrl->fm_frmcreate->hlyr, GUI_LYRWIN_STA_ON);	//ͼ��״̬��
			GUI_LyrWinSetTop(fm_ctrl->fm_frmcreate->hlyr);	//ͼ���ö�
		#else
			_app_show_layer(fm_ctrl->fm_frmcreate->hlyr);
		#endif
		
		 #if 1//����1 ͨ��ע���ķ�ʽ����ͼƬ��Դ
		 {
			 __new_fm_freq_scale_create_res(&fm_ctrl->res);	//bmpͼƬע�����
			 layer = GUI_WinGetLyrWin(msg->h_deswin);		//��ȡ���ڵ�ͼ��
			 __wrn("layer = %d\n",layer);
			 __wrn("hlyr = %d\n",fm_ctrl->fm_frmcreate->hlyr);
			 //GUI_SetBkColor(0);//���ñ���
			 //_new_fm_draw_freq_scale(msg->h_deswin, layer);//����bmpͼƬ
			 //fast_draw_freq_scale(msg->h_deswin,  layer);//���ٻ���bmpͼƬ
			 fast_draw_freq_scale(msg);	//���ٻ���bmpͼƬ
			 //_new_fm_draw_freq_scale(fm_ctrl->fm_frmcreate->hfrm, fm_ctrl->fm_frmcreate->hlyr, &fm_ctrl->res);
		 }
		 #endif

		 #if 0 //����2 ͨ��ֱ�ӵ���ͼƬ����API�ӿڷ�ʽ
		 {
			__new_fm_uipara_init(msg);//����ͼƬ��Դ
			__wrn("__new_fm_ui_paint_all\n");
			__new_fm_ui_paint_all(msg);//����ͼƬ
		 }
		 #endif
			 
			
		}
		return EPDK_OK;

		case GUI_MSG_DESTROY: { //1,
			fm_ctrl_para_t *wnd;
			__wrn("DESTROY new fm framewin..\n");
			wnd = (fm_ctrl_para_t *)GUI_WinGetAttr(msg->h_deswin);
			//wnd = (fm_ctrl_para_t *)GUI_WinGetAddData(msg->h_deswin);
			//_new_fm_uipara_uninit(msg);				//����2���ͷ�ͼƬ��Դ
			__new_fm_freq_scale_destroy_res(&wnd->res);	//����1���ͷ�bmpע���
			app_new_fm_framewin_delete(msg);			//ɾ�������ڽṹ������
		}
		return EPDK_OK;

		case GUI_MSG_PAINT:{
			fast_draw_freq_scale(msg);	//���ٻ���bmpͼƬ
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

		case NEW_CMD_AUTOSRH_FINDCH: { 		//�Զ�����״̬��������һ��Ƶ��
			NEW_FM_UpdateUI(msg->h_deswin);	//����UI����
		}
		return EPDK_OK;

		case NEW_CMD_AUTOSRH_FINDCHFAIL: { 	//�Զ���������
			NEW_FM_UpdateUI(msg->h_deswin);	//����UI����,ʹ�����API�ӿڻ���µ�GUI_MSG_PAINT����������
		}
		return EPDK_OK;

		case NEW_CMD_AUTOSRH_OVER: {		//�Զ��������
			NEW_FM_UpdateUI(msg->h_deswin);	//����UI����
		}
		return EPDK_OK;

		
	}

	return GUI_FrmWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*                       				new_fm_frmwin_create
*
*Description: fm��framewin���ڴ���
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
	/////////////��ʼ����framewin��������///////////////
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
*Description: fm��framewin����ɾ��
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
		
		eLIBs_memset(this, 0, sizeof(fm_ctrl_para_t));//�ȳ�ʼ������0
		My_Bfree(this, sizeof(fm_ctrl_para_t));//�ͷ��ڴ�ռ�
	}

}











