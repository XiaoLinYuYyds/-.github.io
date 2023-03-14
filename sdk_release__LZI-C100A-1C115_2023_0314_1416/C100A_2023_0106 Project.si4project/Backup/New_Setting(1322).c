#include "New_Setting.h"
#include "New_Setting_para.h"
#include "string.h"
#include "typedef.h"



/***********************************************************************************************************
	����ͼ��
************************************************************************************************************/
H_LYR New_Setting_8bpp_layer_create(RECT *rect,__s32 pipe)
{
	H_LYR layer = NULL;
	FB  fb =
	{
		{0, 0},                                   		/* size      */
		{0, 0, 0},                                      /* buffer    */
		{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
		0,                                              /* ck_mode   */
		0,                                              /* alpha_en  */
		0,                                              /* alpha_val */
		1,                                              /* pipe      */
		0xff,                                           /* prio      */
		{0, 0, 0, 0},                           		/* screen    */
		{0, 0, 0, 0},                               	/* source    */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
		NULL                                            /* fb        */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"New_Setting layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	lstlyr.src_win.x  		= 0;
	lstlyr.src_win.y  		= 0;
	lstlyr.src_win.width 	= rect->width;
	lstlyr.src_win.height 	= rect->height;
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y; //240Ϊ��Ļ�߶ȣ�60Ϊ�׶˸߶�
	lstlyr.scn_win.width  	= rect->width;
	lstlyr.scn_win.height 	= rect->height;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);

	if(!layer)
	{
		__err("New_Setting layer create error !\n");
	}
	__wrn("New_Setting_8bpp_layer_create CREATE\n");
	return layer;
}
/****************************************************
*�س�����������ȡ����:number:00
****************************************************/
void New_Setting_get_volume_size(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str�����ڴ�ռ��ʼ��
	eLIBs_strcat(str, "number:");
	snum = num;
	eLIBs_uint2str_dec(snum, numstr);//int��תchar��
	 
	if(snum >= 10)
	{
		eLIBs_strcat(str, numstr);
	}
	else
	{
		eLIBs_strcat(str, "0");
		eLIBs_strcat(str, numstr);
	}

	eLIBs_strcpy(New_Setting_ctrl->new_number_size, (const char *)str);


}
/****************************************************
*ʱ���ʱ������ȡ������time
****************************************************/
void New_Setting_get_Time_size(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	char		str[768];
	char		numstr[32];
	__u32		 snum;
	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	eLIBs_memset(str, 0, 768);	//str�����ڴ�ռ��ʼ��
	eLIBs_strcat(str, "time:");
	snum = num;
	eLIBs_uint2str_dec(snum, numstr);//int��תchar��
	 
	if(snum >= 10)
	{
		eLIBs_strcat(str, numstr);
	}
	else if(snum >59)//1���Ӽ�ʱ����
	{
		eLIBs_strcat(str, "0");//�ַ����浽str����
		eLIBs_strcat(str, "0");
	}
	else
	{
		eLIBs_strcat(str, "0");
		eLIBs_strcat(str, numstr);
	}

	eLIBs_strcpy(New_Setting_ctrl->time_size, (const char *)str);//��str�������ݿ������ṹ���time_size�ַ�������


}
/***********************************************************
*�������ã�ʱ��ת�ַ���
��	   ����char *buff������һ��char�����飬
		   __time_t time��esTIME_GetTime()������ȡ����ʱ��
��	   �����ѻ�ȡ����ʱ���ŵ�����
***********************************************************/
static void new_time_to_char(char *buff, __time_t time)
{
	__wrn("time: %d:%d:%d \n",time.hour,time.minute,time.second);
	buff[0] = time.hour / 10 + '0';//ʮλ����ʾ��+'0'�ǰ�ʮ����ת����ascii��
	buff[1] = time.hour % 10 + '0';//��λ����ʾ
	buff[2] = ':';
	buff[3] = time.minute / 10 + '0';//ʮλ����ʾ
	buff[4] = time.minute % 10 + '0';//��λ����ʾ
	//buff[5] = '\0';
	buff[5] = ':';

	buff[6] = time.second/10 + '0';//ʮλ����ʾ
	buff[7] = time.second%10 + '0';//��λ����ʾ

	buff[8] = '\0';//��б��0Ϊ����
}
/***********************************************************
*����ת�ַ���
***********************************************************/
static void new_date_to_char(char *buff, __date_t date)
{
	__wrn("date: %d/%d/%d \n",date.year,date.month,date.day);
		buff[0] = date.year/1000 + '0';
		buff[1] = (date.year%1000)/100 + '0';
		buff[2] = ((date.year%1000)%100)/10 + '0';
		buff[3] = (date.year%1000)%100%10 + '0';
	
		buff[4] = '-';
		buff[5] = date.month/10 + '0';
		buff[6] = date.month%10 + '0';
	
		buff[7] = '-';
		buff[8] = date.day/10 + '0';
		buff[9] = date.day%10 + '0';
	//buff[0] = date.month / 10 + '0';
	//buff[1] = date.month % 10 + '0';
	//buff[2] = '-';
	//buff[3] = date.day / 10 + '0';
	//buff[4] = date.day % 10 + '0';
	//buff[5] = '\0';
}

/******************************************************************
*��ʼ��������ʹ��ͼƬ��Դ
*******************************************************************/
static void New_Setting_init(New_Setting_ctrl_t *New_Setting_ctrl)
{
	SIZE screen_size;
	dsk_display_get_size(&screen_size.width, &screen_size.height);
	New_Setting_ctrl->New_Settings_uipara = New_Setting_get_numbers_para(screen_size);//��ȡ�ؼ��Ĵ�С����ʾλ��
	__wrn("New_Setting_init is ok\n");
	if(!New_Setting_ctrl->New_Setting_bmp)
	{
		New_Setting_ctrl->New_Setting_bmp = theme_open(ID_SETTING_SET_INFO_BMP);//���򱳾�ͼ
	}
	#if 1
	if(!New_Setting_ctrl->New_Setting_Bottom_bmp)
	{
		New_Setting_ctrl->New_Setting_Bottom_bmp = theme_open(ID_SETTING_SET_BOTTOM_BMP);//��ť���ο򱳾�ͼ
	}
	#endif

	if(!New_Setting_ctrl->New_Setting_f_bmp)
	{
		New_Setting_ctrl->New_Setting_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);//ȷ�ϱ���ͼ
	}
	get_menu_text(STRING_SET_SATURDAY, New_Setting_ctrl->Saturday_txt, 128);//���������ֻ�ȡ
	__wrn("New_Setting_init CREATE\n");
}
/**********************************************************************************
*�ͷ������ʹ��ͼƬ��Դ
**********************************************************************************/
static void New_Setting_uninit(New_Setting_ctrl_t *New_Setting_ctrl)
{
	__u32 	i;

	{
		__wrn("New_Setting_uninit is 1 ok\n");
		if(New_Setting_ctrl->New_Setting_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_bmp);//���򱳾�ͼ
			New_Setting_ctrl->New_Setting_bmp = NULL;
		}
		__wrn("New_Setting_uninit is 2 ok\n");
		if(!New_Setting_ctrl->New_Setting_Bottom_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_Bottom_bmp);//��ť���ο򱳾�ͼ
			New_Setting_ctrl->New_Setting_Bottom_bmp = NULL;
		}
		__wrn("New_Setting_uninit is 3 ok\n");
		if(New_Setting_ctrl->New_Setting_f_bmp)
		{
			dsk_theme_close(New_Setting_ctrl->New_Setting_f_bmp);//ȷ�ϱ���ͼ
			New_Setting_ctrl->New_Setting_f_bmp  = NULL;
		}
		__wrn("New_Setting_uninit is 4 ok\n");
	}

	return;
}
/****************************************************************
*���Ƶ���Ӧ��
****************************************************************/
 __s32 New_Setting_paint_one_item(__gui_msg_t *msg,__u32 num)
{
	New_Setting_ctrl_t *New_Setting_ctrl;
	new_setting_time_para_t *time_para;//���ʱ������ڽṹ��
	new_setting_time_data_t set_time_para;//���ʱ������ڽṹ��
	GUI_RECT gui_rect;		//���GUI���οؼ�����
	GUI_RECT gui_bottom_rect;		//���GUI��ť���οؼ�����
	char	 *txt_character;//������ֵ��ַ�����
	void  	*number_theme;	//��Ż�ȡ��ͼƬʹ����Դ����
	void	*New_Setting_Bottom_theme;//��Ż�ȡ�İ���ͼƬʹ����Դ����
	__s32  sys_time = 10;		  //��Ż�ȡ��ϵͳʱ��
	static __s32  minute_count = 0;		  //ʱ��ֵļ���
	static __s32  hour_count = 0;		  	  //ʱ��ʱ�ļ���

	GUI_MEMDEV_Handle draw_mem = NULL;//���α�ڴ�	
	__s32 screen_width, screen_height;//��ȡlcd��ʾ���Ŀ�͸�
	if(GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
	{
		return EPDK_FAIL;
	}

	New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ���һ�����ݵ�msg�ṹ��
	GUI_LyrWinSel(New_Setting_ctrl->New_Settings_para->layer);//ѡ������ӵ�appӦ��framewin�Ӵ���ͼ��
	
	#if 0		//ֱ����ͼ��������ʾ�����ı��ַ���
				GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);	//���öԻ�������
				GUI_UC_SetEncodeUTF8(); 								//���ñ����ʽ
				__wrn("number_menu_numbers_paint_all 3 CREATE\n");
				GUI_SetColor(GUI_BLACK);								//�����ı���ɫ����ɫ
				New_Setting_get_volume_size(msg,num);			//��ȡ�س������������ �ַ���
				txt_character = New_Setting_ctrl->new_number_size;		//��Ҫ��ʾ�������ַ�����ȡ
				__wrn("number_menu_numbers_paint_all 4 CREATE\n");
				GUI_DispStringAt(txt_character,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x,New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				{
					__wrn("txt_character = %s\n",txt_character);		//ֱ����ʾ�����ַ������ı����Ƴɹ�
				}
				
				//number_theme = theme_hdl2buf(numbers_ctrl->number_bmp);
				//GUI_BMP_Draw(number_theme, numbers_ctrl->numbers_uipara->numbers_bmp_pos.x, numbers_ctrl->numbers_uipara->numbers_bmp_pos.y);//�����ı��ؼ�number:00��ʾ����
				__wrn("New_Setting_paint_one_item CREATE\n");
				__wrn("New_Setting_ctrl->new_number_size = %s\n",New_Setting_ctrl->new_number_size);
				__wrn("number_tsize_pos.x = %d , number_tsize_pos.y = %d\n",New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.y);
				//GUI_DispStringAt(numbers_ctrl->number_total_size, numbers_ctrl->numbers_uipara->number_tsize_pos.x, numbers_ctrl->numbers_uipara->number_tsize_pos.y);			//���ı��ؼ�����ʾ�ַ���
				//GUI_DispStringAt("01",32,28);//�ַ�����ʾ������
				__wrn("txt paint is CREATE\n");//�ı����Ƴɹ�
	#endif
	#if 0//����α�ڴ棬ʹ�û��Ƹ���

		/* get lcd size*/
		dsk_display_get_size(&screen_width, &screen_height);
		draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height);
		__wrn("draw_mem=0x%x\n", draw_mem);

		
		if(draw_mem)//ѡ��α�ڴ�
		{
			GUI_MEMDEV_Select(draw_mem);
		}
		if(draw_mem)//α�ڴ�ɾ��
		{
			GUI_MEMDEV_CopyToLCD(draw_mem);
			GUI_MEMDEV_Select(NULL);
			GUI_MEMDEV_Delete(draw_mem);
			draw_mem = NULL;
		}
	#endif
	#if 1		//���ƾ��� Ȼ���ھ��ο���ʾ���������ַ���---> ��ʾ "number:00"
		GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);		//���öԻ�������
		GUI_UC_SetEncodeUTF8();										//���ñ����ʽ
		
		GUI_SetColor(APP_COLOR_YELLOW);									//GUI_WHITE�����ı���ɫ����ɫ
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//���û�ͼģʽΪ����
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//���þ��ο�ؼ��ı�����ɫΪ��ɫ
		New_Setting_get_volume_size(msg,num);						//��ȡ�س������������ �ַ���
		txt_character = New_Setting_ctrl->new_number_size;			//��Ҫ��ʾ�������ַ�����ȡ
		#if 0//�ֶ�ֱ���޸ľ��δ�С
		gui_rect.x0 = 0;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;		//����x 100 /*416*/
		gui_rect.y0 = 0;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;		//����y 80    /*290*/
		gui_rect.x1 = 800;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//���ο�191
		gui_rect.y1 = 480;//New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height;  //���θ�20
		#else//��ȡNew_Setting_para.c�ļ����õľ��δ�С��λ��
		gui_rect.x0 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.x;		//����x 0 /*416*/
		gui_rect.y0 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.y;		//����y 0	/*290*/
		gui_rect.x1 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.width;	//���ο�191
		gui_rect.y1 = New_Setting_ctrl->New_Settings_uipara->numbers_tiltle_pos.height; //���θ�101
		#endif
		{
			number_theme = theme_hdl2buf(New_Setting_ctrl->New_Setting_bmp);			//��ȡͼƬʹ����Դ����
			GUI_BMP_Draw(number_theme, New_Setting_ctrl->New_Settings_uipara->numbers_bmp_pos.x, New_Setting_ctrl->New_Settings_uipara->numbers_bmp_pos.y);//�����ı��ؼ�number:00��ʾ�������α���ͼ����ʾ���꣬��ֹnumber���ַ�������������
			//GUI_BMP_Draw(number_theme, 0, 0);//�����ı��ؼ�number:00��ʾ��������ֹnumber���ַ�������������
	
			__wrn("gui_rect.x0 = %d , gui_rect.y0 = %d,gui_rect.width = %d , gui_rect.heigh = %d\n",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
			GUI_DispStringInRect(txt_character , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);//�������ַ�����ʾ�����οؼ����棬��ˮƽ����
			
			//GUI_DispStringAt(txt_character, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x, New_Setting_ctrl->New_Settings_uipara->number_tsize_pos.x + 20);
			__wrn("txt_character = %s \n",txt_character);//��ӡ�ı���ʾ��Ϣ
			__wrn("New_Setting_paint_one_item ok ok \n");
		}
	#endif
	//while(sys_time--)
	{
	#if 1		//���ư������� Ȼ���ڰ������ο���ʾ���������ַ��� ---> ��ʾ���� "������ 2023/1/17 16:09"
		GUI_SetFont(New_Setting_ctrl->New_Settings_para->font);		//���öԻ�������
		GUI_UC_SetEncodeUTF8();										//���ñ����ʽ
		
		GUI_SetColor(APP_COLOR_YELLOW);								//GUI_WHITE�����ı���ɫ����ɫ
		GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);						//���û�ͼģʽΪ����
		//GUI_SetBkColor(BEETLES_BLACK_COLOR);						//���þ��ο�ؼ��ı�����ɫΪ��ɫ
		#if 1
			gui_bottom_rect.x0 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.x;		//��ť����x 272 
			gui_bottom_rect.y0 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.y;		//��ť����y 120	
			gui_bottom_rect.x1 = New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.width;	//��ť���ο�480
			gui_bottom_rect.y1 = 480;//New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_pos.height; //��ť���θ�37
		#endif
		{
			__wrn("gui_bottom_rect.x0 = %d , gui_bottom_rect.y0 = %d,gui_bottom_rect.width = %d , gui_bottom_rect.heigh = %d\n",
						gui_bottom_rect.x0,gui_bottom_rect.y0,gui_bottom_rect.x1,gui_bottom_rect.y1);
			New_Setting_Bottom_theme = theme_hdl2buf(New_Setting_ctrl->New_Setting_Bottom_bmp);  		//��ȡ��ťͼƬʹ����Դ����
			GUI_BMP_Draw(New_Setting_Bottom_theme ,
				New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.x , 
				New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.y);	 				//��ťͼƬ����
				{
					__wrn("New_Settings_tiltle_bmp_pos.x = %d , New_Settings_tiltle_bmp_pos.y = %d\n",
						New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.x,New_Setting_ctrl->New_Settings_uipara->New_Settings_tiltle_bmp_pos.y);
					__wrn("Saturday_txt 0 is... = %s \n",New_Setting_ctrl->Saturday_txt);						//��ӡ�ı���ʾ��Ϣ
					GUI_DispStringInRect(New_Setting_ctrl->Saturday_txt, &gui_bottom_rect , GUI_TA_LEFT | GUI_TA_TOP);//��ʾ������
					
					//GUI_DispStringInRect(txt_character, &gui_bottom_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);//��ʾ������
					__wrn("txt_character 1 is... = %s \n",txt_character);
					__wrn("Saturday_txt 1 is... = %s \n",New_Setting_ctrl->Saturday_txt);
					//GUI_DispStringAt(New_Setting_ctrl->Saturday_txt,278,125);//������ʾ
					//GUI_DispStringAt("2023/1/10 10:09",332,125);//������ʾ
					GUI_SetBkColor(BEETLES_BLACK_COLOR);						//���þ��ο�ؼ��ı�����ɫΪ��ɫ
					#if 1	//��ȡϵͳʱ��
						__wrn("sys_time 1 = %d \n",sys_time);
						#if 0//�ṹ��1 set_time_para �����ṹ����� ����ʱ�������

							set_time_para.time.hour = 9;
							set_time_para.time.minute = 59;
							set_time_para.time.second = 24;
							esTIME_SetTime(&(set_time_para.time));						// ��������ʱ��
							__wrn("0 set_time_para.time = %d \n",set_time_para.time);
							set_time_para.date.year = 2023;
							set_time_para.date.month = 01;
							set_time_para.date.day = 10;
							esTIME_SetDate(&(set_time_para.date));						// ������������
							__wrn("0 set_time_para.date = %d \n",set_time_para.date);
						#endif
						#if 0//�ṹ��2 *time_para �ṹ��ָ����� ����ʱ�������

							time_para->data.time.hour = 10;
							time_para->data.time.minute = 52;
							time_para->data.time.second = 24;
							esTIME_SetTime(&(time_para->data.time));						// ��������ʱ��
							__wrn("0 time_para->data.time = %d \n",time_para->data.time);
							time_para->data.date.year = 2023;
							time_para->data.date.month = 01;
							time_para->data.date.day = 10;
							esTIME_SetDate(&(set_time_para.date));						// ������������
							__wrn("0 time_para->data.date = %d \n",time_para->data.date);
						#endif
						#if 0//�ṹ��3 New_Setting_ctrl->set_time_para �ṹ��ָ����ýṹ��ָ�����д�� ����ʱ������� ֻ��Ҫ����һ��
							New_Setting_ctrl->set_time_para->data.time.hour = 9;
							New_Setting_ctrl->set_time_para->data.time.minute = 58;
							New_Setting_ctrl->set_time_para->data.time.second = 24;
							esTIME_SetTime(&(New_Setting_ctrl->set_time_para->data.time));						// ��������ʱ��
							__wrn("New_Setting_ctrl->set_time_para.data.time is... = %d \n",New_Setting_ctrl->set_time_para->data.time);
							__wrn("set time OK set_time_para->data.time.hour = %d:%d:%d \n",New_Setting_ctrl->set_time_para->data.time.hour,New_Setting_ctrl->set_time_para->data.time.minute,New_Setting_ctrl->set_time_para->data.time.second);
							New_Setting_ctrl->set_time_para->data.date.year = 2023;
							New_Setting_ctrl->set_time_para->data.date.month = 01;
							New_Setting_ctrl->set_time_para->data.date.day = 10;
							esTIME_SetDate(&(New_Setting_ctrl->set_time_para->data.date));						// ������������
							__wrn("New_Setting_ctrl->set_time_para->data.date is... = %d \n",New_Setting_ctrl->set_time_para->data.date);
							__wrn("set date OK set_time_para->data.date.year = %d-%d-%d \n",New_Setting_ctrl->set_time_para->data.date.year,New_Setting_ctrl->set_time_para->data.date.month,New_Setting_ctrl->set_time_para->data.date.day);
						#endif
						//while(sys_time--)
						{
						//sys_time = esTIME_GetDate(&(time_para->data.date));
						__wrn("sys_time 2 = %d \n",sys_time);
						#if 0//ʱ�����м���
							if(sys_time == 0)//s����0��ʱ��
							{
								
								minute_count += 1;
								//if(minute_count == 9)
								{
									hour_count += 1;
									if(New_Setting_ctrl->set_time_para->data.time.minute >= 59)//60�֣�������0
									{
										New_Setting_ctrl->set_time_para->data.time.hour += 1;//Сʱ��1
										New_Setting_ctrl->set_time_para->data.time.minute = 0;//����0
										New_Setting_ctrl->set_time_para->data.time.second = sys_time;//����0
									}
									else if(New_Setting_ctrl->set_time_para->data.time.hour > 23)
									{
										New_Setting_ctrl->set_time_para->data.time.hour = 0;
										New_Setting_ctrl->set_time_para->data.time.minute = 0;
										New_Setting_ctrl->set_time_para->data.time.second = sys_time;//����0
									}
									else
									{
										New_Setting_ctrl->set_time_para->data.time.minute += 1;
										New_Setting_ctrl->set_time_para->data.time.second = sys_time;

									}
									if(hour_count > 10)
									{
										hour_count = 0;
									}
									minute_count = 0;
								}
															
							}
							else
							{
								//New_Setting_ctrl->set_time_para->data.time.hour += hour_count;//ʱ
								//New_Setting_ctrl->set_time_para->data.time.minute += minute_count;//��
								New_Setting_ctrl->set_time_para->data.time.second += 1;//��
							}
							esTIME_SetTime(&(New_Setting_ctrl->set_time_para->data.time));//��������ʱ��
						#endif
						#if 0//�ṹ��2 *time_para�ṹ��ָ����� ��ȡʱ�������
							__wrn("get time start time_para->data.time.hour = %d:%d:%d \n",time_para->data.time.hour,time_para->data.time.minute,time_para->data.time.second);
							__wrn("get date start time_para->data.date.year = %d:%d:%d \n",time_para->data.date.year,time_para->data.date.month,time_para->data.date.day);
							esTIME_GetTime(&(time_para->data.time));						// ʱ��
							__wrn("time_para->data.time = %d \n",time_para->data.time);
							eLIBs_memset(time_para->time_buf, 0, 32);
							new_time_to_char(time_para->time_buf, time_para->data.time);		//ʱ�� ��ֵ�� time_para->time_buf����
							__wrn("time_para->time_buf = %s \n",time_para->time_buf);
							GUI_DispStringAt(time_para->time_buf,480,125);//������ʾʱ��
							
							esTIME_GetDate(&(time_para->data.date));						// ����
							__wrn("time_para->data.date = %d \n",time_para->data.date);
							eLIBs_memset(time_para->date_buf, 0, 32);
							new_date_to_char(time_para->date_buf, time_para->data.date);		//���� ��ֵ�� time_para->data_buf����
							__wrn("time_para->date_buf = %s \n",time_para->date_buf);
							GUI_DispStringAt(time_para->date_buf,680,125);//������ʾ����
						#endif
						#if 0//�ṹ��1 set_time_para�����ṹ����ò��� ��ȡʱ�������
							__wrn("get time start set_time_para.time.hour = %d:%d:%d \n",set_time_para.time.hour,set_time_para.time.minute,set_time_para.time.second);
							__wrn("get date start set_time_para.date.year = %d:%d:%d \n",set_time_para.date.year,set_time_para.date.month,set_time_para.date.day);
							esTIME_GetTime(&(set_time_para.time));						// ʱ��
							__wrn("set_time_para.time = %d \n",set_time_para.time);
							eLIBs_memset(time_para->time_buf, 0, 32);
							new_time_to_char(time_para->time_buf, set_time_para.time);		//ʱ�� ��ֵ�� time_para->time_buf����
							__wrn("time_para->time_buf = %s \n",time_para->time_buf);
							GUI_DispStringAt(time_para->time_buf,480,125);//������ʾʱ��
							
							esTIME_GetDate(&(set_time_para.date));						// ����
							__wrn("set_time_para.date = %d \n",set_time_para.date);
							eLIBs_memset(time_para->date_buf, 0, 32);
							new_date_to_char(time_para->date_buf, set_time_para.date);		//���� ��ֵ�� time_para->data_buf����
							__wrn("time_para->date_buf = %s \n",time_para->date_buf);
							GUI_DispStringAt(time_para->date_buf,680,125);//������ʾ����
						#endif
						#if 1//�ṹ��3 New_Setting_ctrl->set_time_para �ṹ��ָ����ýṹ��ָ�����д�� ��ȡʱ�������
							__wrn("get time start New_Setting_ctrl->set_time_para->data.time.hour = %d:%d:%d \n",
								New_Setting_ctrl->set_time_para->data.time.hour,New_Setting_ctrl->set_time_para->data.time.minute,New_Setting_ctrl->set_time_para->data.time.second);
							__wrn("get date start New_Setting_ctrl->set_time_para->data.date.year = %d:%d:%d \n",
								New_Setting_ctrl->set_time_para->data.date.year,New_Setting_ctrl->set_time_para->data.date.month,New_Setting_ctrl->set_time_para->data.date.day);
							esTIME_GetTime(&(New_Setting_ctrl->set_time_para->data.time));						// ʱ��
							__wrn("New_Setting_ctrl->set_time_para->data.time = %d \n",New_Setting_ctrl->set_time_para->data.time);
							eLIBs_memset(time_para->time_buf, 0, 32);
							new_time_to_char(time_para->time_buf, New_Setting_ctrl->set_time_para->data.time);		//ʱ�� ��ֵ�� time_para->time_buf����
							__wrn("time_para->time_buf = %s \n",time_para->time_buf);
							//GUI_DispStringAt(time_para->time_buf,480,125);//������ʾʱ��
							GUI_DispStringInRect(time_para->time_buf , &gui_bottom_rect , GUI_TA_RIGHT | GUI_TA_TOP);
							
							esTIME_GetDate(&(New_Setting_ctrl->set_time_para->data.date));						// ����
							__wrn("set_time_para->data.date = %d \n",New_Setting_ctrl->set_time_para->data.date);
							eLIBs_memset(time_para->date_buf, 0, 32);
							new_date_to_char(time_para->date_buf, New_Setting_ctrl->set_time_para->data.date);		//���� ��ֵ�� time_para->data_buf����
							__wrn("time_para->date_buf = %s \n",time_para->date_buf);
							//GUI_DispStringAt(time_para->date_buf,680,125);//������ʾ����
							GUI_DispStringInRect(time_para->date_buf , &gui_bottom_rect , GUI_TA_CENTER | GUI_TA_TOP);	//GUI_TA_VCENTER
							
						#endif
						esKRNL_TimeDly(20);		// 10 * 5ms = 50ms
						}
					#endif
				}
		}
		
	#endif
	}
	return EPDK_OK;
}
//��������ͼƬ
static __s32 New_Setting_paint_all(__gui_msg_t *msg)
{

	return EPDK_OK;
}
/****************************************************************
*�������ܣ�appӦ��framewin���ڵİ���������
****************************************************************/
static __s32  New_Setting_key_proc(__gui_msg_t *msg)
{
	#if 1
	static __s32 last_key = -1;
	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		__wrn("New_Setting_key key2 msg->dwAddData2 = %d\n",msg->dwAddData2);
		__wrn("New_Setting_key key2 last_key = %d\n",last_key);
		if(GUI_MSG_KEY_ENTER == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_ENTER, 0, 0);//�򸸴���app_New_Setting.c�ļ���manwin���ڷ���"case GUI_MSG_COMMAND:	//��������"���뵽case NEW_SETTING_ID:����ִ�й���
			//app_new_setting_cmd2parent(msg->h_deswin, NEW_SETTING_ID, ID_OP_NEW_SETTING_ENTER, 0);
		}
		if(GUI_MSG_KEY_RIGHT == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
		}
		if(GUI_MSG_KEY_LEFT == last_key)
		{
			app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
		}
	}
	else
	{
		switch(msg->dwAddData1)
		{
		__wrn("New_Setting_key key1 msg->dwAddData1 = %d\n",msg->dwAddData1);
			case GUI_MSG_KEY_LEFT:		//Ϊup
			case GUI_MSG_KEY_LONGLEFT:
			{
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);//�������
			}
			break;

			case GUI_MSG_KEY_RIGHT: 	//Ϊdown
			case GUI_MSG_KEY_LONGRIGHT:
			{
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
			}
			break;

			case GUI_MSG_KEY_UP:
			case GUI_MSG_KEY_LONGUP:
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_UP, 0, 0);
				break;

			case GUI_MSG_KEY_DOWN:
			case GUI_MSG_KEY_LONGDOWN:
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_DOWN, 0, 0);
				break;

			case GUI_MSG_KEY_ESCAPE:
			case GUI_MSG_KEY_MENU:
				app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_EXIT, 0, 0);
				break;

			default:
				break;
		}
	}

	if(KEY_UP_ACTION == msg->dwAddData2)
	{
		last_key = -1;
	}
	else
	{
		last_key = msg->dwAddData1;
	}

	return EPDK_OK;
	#endif
}

/*====================================================================================
*�������ã�ͼ�㴰�ڴ����ص�����
*�βΣ�__gui_msg_t *msg��GUI�̶�����Ľṹ��ָ�����
====================================================================================*/
static __s32 _New_Setting_proc(__gui_msg_t *msg)
{
	static __s32  key_number=0;
	new_setting_time_para_t *set_time_para;//���ʱ������ڽṹ��
	__wrn("_New_Setting_proc msg->id=%d is...\n",msg->id);
	switch(msg->id)
	{
		case GUI_MSG_CREATE:	//��������
		{
			New_Setting_ctrl_t *New_Setting_ctrl;
			new_setting_para_t *New_Settings_para;
			New_Settings_para = (new_setting_para_t *)GUI_WinGetAttr(msg->h_deswin);		//��ȡ���ڵ�����
			New_Setting_ctrl = (New_Setting_ctrl_t *)My_Balloc(sizeof(New_Setting_ctrl_t)); //�ڴ����
			if(!New_Setting_ctrl)
			{
				__err("memory balloc fail........\n");
				return EPDK_FAIL;
			}
			__wrn("New_Setting GUI_MSG_CREATE 1 is...\n");
			eLIBs_memset(New_Setting_ctrl, 0, sizeof(New_Setting_ctrl_t));			//���ṹ��number_menu_numbers_ctrl_t�ڴ��ʼ��
			New_Setting_ctrl->focus_txt_color = APP_COLOR_YELLOW;//GUI_YELLOW;//				//ѡ�е��ı���ɫ
			New_Setting_ctrl->unfocus_txt_color = APP_COLOR_WHITE;//GUI_WHITE;//				//δѡ�е��ı���ɫ
			New_Setting_ctrl->New_Settings_para = New_Settings_para;
			GUI_WinSetAddData(msg->h_deswin, (__u32)New_Setting_ctrl);						//���һ������
			__wrn("New_Setting GUI_MSG_CREATE 2 is...\n");
			#if 1//New_Setting_ctrl->set_time_para �ṹ��ָ����ýṹ��ָ�����д�� ����ʱ������� ֻ��Ҫ����һ��
				New_Setting_ctrl->set_time_para->data.time.hour = 9;
				New_Setting_ctrl->set_time_para->data.time.minute = 58;
				New_Setting_ctrl->set_time_para->data.time.second = 24;
				esTIME_SetTime(&(New_Setting_ctrl->set_time_para->data.time));						// ��������ʱ��
				__wrn("New_Setting_ctrl->set_time_para.data.time is... = %d \n",New_Setting_ctrl->set_time_para->data.time);
				__wrn("set time OK set_time_para->data.time.hour = %d:%d:%d \n",New_Setting_ctrl->set_time_para->data.time.hour,New_Setting_ctrl->set_time_para->data.time.minute,New_Setting_ctrl->set_time_para->data.time.second);
				New_Setting_ctrl->set_time_para->data.date.year = 2023;
				New_Setting_ctrl->set_time_para->data.date.month = 01;
				New_Setting_ctrl->set_time_para->data.date.day = 10;
				esTIME_SetDate(&(New_Setting_ctrl->set_time_para->data.date));						// ������������
				__wrn("New_Setting_ctrl->set_time_para->data.date is... = %d \n",New_Setting_ctrl->set_time_para->data.date);
				__wrn("set date OK set_time_para->data.date.year = %d-%d-%d \n",New_Setting_ctrl->set_time_para->data.date.year,New_Setting_ctrl->set_time_para->data.date.month,New_Setting_ctrl->set_time_para->data.date.day);
			#endif
			New_Setting_init(New_Setting_ctrl);												//number:00��ʾ��ʾͼƬ��Դ����ʹ�����ʾ��С
			GUI_LyrWinSetSta(New_Setting_ctrl->New_Settings_para->layer, GUI_LYRWIN_STA_ON);//����ͼ�����ÿ���״̬
			GUI_LyrWinSetTop(New_Setting_ctrl->New_Settings_para->layer);					//����ͼ������ֹͣ״̬
			//number_menu_numbers_paint_all(msg,0);											//��������ͼ��
			__wrn("New_Setting GUI_MSG_CREATE 3 is...\n");
			//New_Setting_paint_one_item(msg,0);				//����ͼ��												//���Ƶ���ͼ��
			__wrn("New_Setting GUI_MSG_CREATE 4 is ok\n");

			
			
		}

		return EPDK_OK;

		case GUI_MSG_CLOSE:	//�رմ��ڣ�Ȼ�󷵻���һ�������ڽ���
		{
			#if 0
			New_Setting_ctrl_t *New_Setting_ctrl;
			New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

			if(!New_Setting_ctrl)
			{
				__msg("New_Setting_ctrl_t is null \n");
				return EPDK_FAIL ;
			}
			__wrn("New_Setting_ctrl_t GUI_MSG_CLOSE is...  \n");
			
			//app_new_setting_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_SWITCH_TO_GENERAL, 0, 0);//������һ��
			//app_new_setting_cmd2parent(msg->h_deswin, ID_NUMBER_MENU_OP_EXIT, 0, 0);//ֱ���˳�
			//app_new_setting_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, ID_HOME_NEW_SETTING_NUMBER, NULL);//ֱ�ӽ���ID_HOME_NEW_SETTING_NUMBER,Ȼ����뵽����ӵ�ͨ�����ý���
			#endif
			#if 0//ֱ���ͷ�ͼƬ��Դ
			New_Setting_ctrl_t *New_Setting_ctrl;
			New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ����ṹ�������
			New_Setting_uninit(New_Setting_ctrl);//�ͷ�ͼƬ��Դ
			if(New_Setting_ctrl)//�ͷŽṹ���ڴ�
			{
				__msg("New_Setting_ctrl_t win free New_Setting_ctrl_t.....\n");
				My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
				New_Setting_ctrl = NULL ;
			}
			#endif
			#if 1//��app_New_Setting.c������manwin���ڵ�case NEW_SETTING_ID:�����˳��ź�
				__wrn("New_Setting_ctrl_t APP_NEWSETTING_ID is... ok\n");
				//app_new_setting_cmd2parent(msg->h_deswin, ID_OP_NEW_SETTING_EXIT, 0, 0);//��������ӵ�appӦ���˳�ID��Ϣ
				app_new_setting_cmd2parent(msg->h_deswin, ID_NEW_SETTING_SWITCH_TO_SETTING, 0, 0);//��������ӵ�appӦ����ת��ͨ������appӦ�ó�����Ϣ;����appӦ�ó���manwin����֮���л���ת
				//app_new_setting_cmd2parent(msg->h_deswin, APP_NEWSETTING_ID, GUI_MSG_CLOSE, 0);//��app_New_Setting.c������manwin���͹ر��ź�
			#endif
			__wrn("New_Setting_ctrl_t GUI_MSG_CLOSE is... ok\n");
		}

		return EPDK_OK;

		case GUI_MSG_DESTROY: //���ٴ���
		{
			#if 0	//����1���ͷ�ͼƬʹ����Դ
				New_Setting_ctrl_t *New_Setting_ctrl;
				New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ����ṹ�������
				New_Setting_uninit(New_Setting_ctrl);//�ͷ�ͼƬ��Դ
				if(New_Setting_ctrl->New_Settings_para)//�ͷ�ͼ��
				{
					__wrn("New_Setting_ctrl win free New_Settings_para.....\n");
					My_Bfree(New_Setting_ctrl->New_Settings_para, sizeof(new_setting_para_t));
					New_Setting_ctrl->New_Settings_para = NULL ;
				}

				if(New_Setting_ctrl)
				{
					__wrn("New_Setting_ctrl win free New_Setting_ctrl.....\n");
					My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
					New_Setting_ctrl = NULL ;
				}
			#endif
			#if 1	//����2���ͷ�ͼƬ��Դ
				New_Setting_ctrl_t *New_Setting_ctrl;
				New_Setting_ctrl = (New_Setting_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);//��ȡ����ṹ�������

				if(!New_Setting_ctrl)//�ͷŽṹ���ڴ�
				{
					__err("New_Setting_ctrl is null........\n");
					return EPDK_FAIL;
				}
				

				__wrn("New_Setting_ctrl win destroy...\n");
				theme_close(New_Setting_ctrl->New_Setting_bmp);	//�ͷž��η���ͼƬ��Դ
				__wrn("New_Setting_ctrl win close theme...\n");
				theme_close(New_Setting_ctrl->New_Setting_Bottom_bmp);	//�ͷŰ�ťͼƬ��Դ
				__wrn("New_Setting_ctrl win close theme...\n");
				theme_close(New_Setting_ctrl->New_Setting_f_bmp);//�ͷ�ȷ��ͼƬ��Դ
				__wrn("New_Setting_ctrl win close theme ok\n");

				if(New_Setting_ctrl->New_Settings_para)//�ͷ�ͼ��
				{
					__wrn("New_Setting_ctrl win free New_Settings_para.....\n");
					My_Bfree(New_Setting_ctrl->New_Settings_para, sizeof(new_setting_para_t));
					New_Setting_ctrl->New_Settings_para = NULL ;
				}

				if(New_Setting_ctrl)
				{
					__wrn("New_Setting_ctrl win free New_Setting_ctrl.....\n");
					My_Bfree(New_Setting_ctrl, sizeof(New_Setting_ctrl_t));
					New_Setting_ctrl = NULL ;
				}
			#endif
		}

		return EPDK_OK;

		case GUI_MSG_PAINT:	//������ʾ����ͼƬ����
			__wrn("one  paint new Setting \n");
			New_Setting_paint_one_item(msg,0);
		return EPDK_OK;

		case GUI_MSG_KEY:	//����������
		__wrn("GUI_MSG_KEY new Setting \n");
			New_Setting_key_proc(msg);
		return EPDK_OK;

		case MSG_NEW_SETTING_OP_ENTER:	//�س�ȷ�ϰ���
		{
			__gui_msg_t mymsg;
			key_number += 1;
			__wrn("new setkey_number start =%d  \n",key_number);
			#if 1	
			if(key_number == 5)//ɾ��appӦ�ã�Ȼ�󷵻���һ��app_New_Setting.c��manwin������ʾȫ��appӦ��
			{
				key_number = 0;
				#if 1
				mymsg.id = GUI_MSG_CLOSE;		//framewin���ڵĹرմ���id
				mymsg.h_deswin = msg->h_deswin;	//�ڵ�ǰͼ�㷢�Ͱ�����Ϣ
				mymsg.h_srcwin = msg->h_srcwin; //�ڵ�ǰͼ�㷢�Ͱ�����Ϣ
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#else
				mymsg.id = GUI_MSG_DESTROY;//�������ٴ���
				mymsg.h_deswin = msg->h_deswin;
				mymsg.h_srcwin = msg->h_srcwin;
				mymsg.dwAddData1 = mymsg.dwAddData1;
				mymsg.dwAddData2 = mymsg.dwAddData2;
				GUI_SendMessage(&mymsg);
				#endif
			}
			else
			{	
				__wrn("new setkey_number stop =%d  \n",key_number);
				New_Setting_paint_one_item(msg,key_number);//ˢ��number������ʾ����
			}
			#endif
		}
		return EPDK_OK;
			
		default:
			break;
	}

	return GUI_FrmWinDefaultProc(msg);
}

/*====================================================================================
*�������ã��µ�appӦ��framewin���ڴ�������
*�βΣ�H_WIN h_parent�����ڶ��� ��__gui_msg_t *msg��GUI�̶�����Ľṹ��ָ�����
====================================================================================*/
H_WIN New_Setting_app_win_create(H_WIN h_parent, new_setting_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;		//����һ�� framewin���ڵĴ��� �ṹ�����
	new_setting_para_t *New_Setting_para;
	FB fb;
	__wrn("esKSRV_SysInfo   is...\n");				//��ӡ�ڴ��Ƿ�й¶
	esKSRV_SysInfo();								//�鿴ϵͳ�ڴ���Ϣ�Ƿ�й¶�����
	//esMEMS_FreeMemSize();							//ʣ���ڴ��С��KBΪ��λ
	//esMEMS_TotalMemSize();						//�ܵ��ڴ��С
	__wrn("esKSRV_SysInfo | esMEMS_FreeMemSize = %d \n",esMEMS_FreeMemSize());//��ӡʣ���ڴ�
	__wrn("esKSRV_SysInfo | esMEMS_TotalMemSize = %d \n",esMEMS_TotalMemSize());//��ӡ���ڴ�
	GUI_LyrWinGetFB(para->layer, &fb);				//ͼ�㴰�ڻ�ȡFB
	New_Setting_para = (new_setting_para_t *)My_Balloc(sizeof(new_setting_para_t));	//���ṹ��ָ����������ڴ�ռ�

	if(!New_Setting_para)	//�жϽṹ���Ƿ�����ڴ�ɹ�
	{
		__err("memory balloc fail........\n");
		return EPDK_FAIL;
	}

	New_Setting_para->font = para->font;									//��ȡ��Ϣ�Ի����ı�
	New_Setting_para->layer = para->layer;									//ͼ��
	eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));	//��ʼ���ṹ�壬��0
	framewin_para.name =	"New_Setting_win";								//��������
	framewin_para.dwExStyle = WS_EX_NONE;									//������չ���Ϊ��
	framewin_para.dwStyle = WS_NONE | WS_VISIBLE;							//framewin�Ӵ��ڵ���ʽΪ�ɼ�����
	framewin_para.spCaption =  NULL;										//framewin�Ӵ��ڵĿؼ�����
	framewin_para.hOwner	= NULL;											//framewin�Ӵ��ڵ�ӵ���߾��
	framewin_para.hHosting = h_parent;										//framewin�������ھ��
	framewin_para.id         = NEW_SETTING_ID;								//framewin�Ĵ���ID��
	framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_New_Setting_proc);	//framewin����Ϣ������̣��ص���������
	framewin_para.rect.x = 0;												//framewin��ͼ�㴰���е�λ��x
	framewin_para.rect.y = 0;												//framewin��ͼ�㴰���е�λ��y
	framewin_para.rect.width =  fb.size.width;								//framewin��ͼ�㴰���е�λ�ÿ�1024
	framewin_para.rect.height =  fb.size.height;							//framewin��ͼ�㴰���е�λ�ø�600
	framewin_para.BkColor.alpha =  0;										//framewin�ı���ɫ 32-bit (ARGB) color  
	framewin_para.BkColor.red = 0;
	framewin_para.BkColor.green = 0;
	framewin_para.BkColor.blue = 0;
	framewin_para.attr = (void *)New_Setting_para;							//framewin�Ĵ���˽��������Ϣ
	framewin_para.hLayer = para->layer;										//framewin��ͼ�㴰��
	return (GUI_FrmWinCreate(&framewin_para));								//����һ��framewin��������
}
/*====================================================================================
*�������ã�����ɾ��
====================================================================================*/
void  new_setting_app_win_delete(H_WIN New_Setting_win)
{
	GUI_FrmWinDelete(New_Setting_win);
}

