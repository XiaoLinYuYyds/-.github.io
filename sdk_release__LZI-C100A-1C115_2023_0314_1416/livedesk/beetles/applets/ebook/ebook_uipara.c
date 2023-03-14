/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    :  ebook_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "ebook_uipara.h"
#define SET_TIPS_W  191
#define SET_TIPS_H  101

static ebook_uipara_t ebook_uipara;

ebook_uipara_t *get_ebook_uipara(void)
{
	ebook_uipara.tips_lyr.x = 145 + 260;	//�ϵ��Ķ����ڵ�λ��
	ebook_uipara.tips_lyr.y = 85 + 135;
	ebook_uipara.tips_lyr.w = SET_TIPS_W;
	ebook_uipara.tips_lyr.h = SET_TIPS_H;
	//ebook_uipara.loading_lyr.x = (480-100)/2+50;
	//ebook_uipara.loading_lyr.y = (272-100)/2+50;
	ebook_uipara.loading_lyr.x = 448;	//���ص������"���������Ժ�"
	ebook_uipara.loading_lyr.y = 238;
	ebook_uipara.loading_lyr.w = 100;
	ebook_uipara.loading_lyr.h = 100;
	ebook_uipara.volume_bar_lyr.x  = 0;	//������ʾͼ��λ��
	ebook_uipara.volume_bar_lyr.y  = 0;
	ebook_uipara.volume_bar_lyr.w  = 1024;
	ebook_uipara.volume_bar_lyr.h  = 15;
	ebook_uipara.page_lyr.x  = 0;    //�տ�ʼ����ʾ�е��������Ĳ˵�ͼ��
	ebook_uipara.page_lyr.y  = 576;
	ebook_uipara.page_lyr.w  = 1024;
	ebook_uipara.page_lyr.h  = 24;
	ebook_uipara.mset_lyr.x  = 0;	//����ĳ�������Ĳ˵�ͼ��
	ebook_uipara.mset_lyr.y  = 541;
	ebook_uipara.mset_lyr.w  = 1024;
	ebook_uipara.mset_lyr.h  = 59;
	ebook_uipara.sset_bgmusic_lyr.x  = 75;//���ñ�������
	ebook_uipara.sset_bgmusic_lyr.y  = 367 + 120;
	ebook_uipara.sset_bgmusic_lyr.w  = 118;
	ebook_uipara.sset_bgmusic_lyr.h  = 54;//24*2+3+3=54
	ebook_uipara.sset_play_lyr.x  = 326 - 6;//�����Զ�����
	ebook_uipara.sset_play_lyr.y  = 319 + 120;
	ebook_uipara.sset_play_lyr.w  = 118;
	ebook_uipara.sset_play_lyr.h  = 102;//24*4+3+3=102
	ebook_uipara.sset_txtcolor_lyr.x  = 577 - 7;//����������ɫ
	ebook_uipara.sset_txtcolor_lyr.y  = 343 + 120;
	ebook_uipara.sset_txtcolor_lyr.w  = 118;
	ebook_uipara.sset_txtcolor_lyr.h  = 78;//24*3+3+3=78
	ebook_uipara.sset_goto_lyr.x  = 828 - 8;//����ҳ����ת
	ebook_uipara.sset_goto_lyr.y  = 391 + 120;
	ebook_uipara.sset_goto_lyr.w  = 118;
	ebook_uipara.sset_goto_lyr.h  = 30;//24+3+3=30
	ebook_uipara.bmp_mset_bg.x = 0;
	ebook_uipara.bmp_mset_bg.y = 456  + 120;
	ebook_uipara.bmp_mset_bg.w = 1024;
	ebook_uipara.bmp_mset_bg.h = 59;
	ebook_uipara.bmp_mset_item.x = 0;
	ebook_uipara.bmp_mset_item.y = 0;
	ebook_uipara.bmp_mset_item.w = 50;//ÿ��item�Ŀ����50
	ebook_uipara.bmp_mset_item.h = 42;
	ebook_uipara.bmp_mset_line.x = 0;
	ebook_uipara.bmp_mset_line.y = 0;
	ebook_uipara.bmp_mset_line.w = 1;
	ebook_uipara.bmp_mset_line.h = 34;
	ebook_uipara.bmp_sset_top.x = 0;
	ebook_uipara.bmp_sset_top.y = 0;
	ebook_uipara.bmp_sset_top.w = 118;
	ebook_uipara.bmp_sset_top.h = 2;
	ebook_uipara.bmp_sset_btm.x = 0;
	ebook_uipara.bmp_sset_btm.y = 0;
	ebook_uipara.bmp_sset_btm.w = 118;
	ebook_uipara.bmp_sset_btm.h = 2;
	ebook_uipara.bmp_sset_bar.x = 0;
	ebook_uipara.bmp_sset_bar.y = 0;
	ebook_uipara.bmp_sset_bar.w = 118;
	ebook_uipara.bmp_sset_bar.h = 24;
	ebook_uipara.bmp_play.x = 10;//������ͣͼ���λ��
	ebook_uipara.bmp_play.y = 5;
	ebook_uipara.bmp_play.w = 15;
	ebook_uipara.bmp_play.h = 15;
	ebook_uipara.page_name_pos.x = 215;	//������ʾλ��
	ebook_uipara.page_name_pos.y = 0;
	ebook_uipara.page_name_pos.w = 250;
	ebook_uipara.page_name_pos.h = 24;
	ebook_uipara.page_no_pos.x = 595;//ҳ��λ��
	ebook_uipara.page_no_pos.y = 0;
	ebook_uipara.page_no_pos.w = 8 * (1 + 10 * 2);	// ������ʾ10λҳ��
	ebook_uipara.page_no_pos.h = 24;
	ebook_uipara.bmp_battery.x = 929;	//�����ʾλ��
	ebook_uipara.bmp_battery.y = 6;
	ebook_uipara.bmp_battery.w = 30;
	ebook_uipara.bmp_battery.h = 12;
	ebook_uipara.bmp_volume_r.x	= 974;//����ͼ��λ��
	ebook_uipara.bmp_volume_r.y = 1;
	ebook_uipara.bmp_volume_r.w = 15;
	ebook_uipara.bmp_volume_r.h = 13;
	ebook_uipara.page_volume_value_pos.x = 994;//����������λ��
	ebook_uipara.page_volume_value_pos.y = 0;
	ebook_uipara.page_volume_value_pos.w = 20;
	ebook_uipara.page_volume_value_pos.h = 24;
	ebook_uipara.bmp_slide_block_l.x = 72;
	ebook_uipara.bmp_slide_block_l.y = 5;
	ebook_uipara.bmp_slide_block_l.w = 5;
	ebook_uipara.bmp_slide_block_l.h = 4;
	ebook_uipara.bmp_slide_bar.x = 72;
	ebook_uipara.bmp_slide_bar.y = 5;
	ebook_uipara.bmp_slide_bar.w = 255;
	ebook_uipara.bmp_slide_bar.h = 4;
	ebook_uipara.bmp_slide_block_r.x = 322;
	ebook_uipara.bmp_slide_block_r.y = 5;
	ebook_uipara.bmp_slide_block_r.w = 5;
	ebook_uipara.bmp_slide_block_r.h = 4;
	ebook_uipara.bmp_slide_block.x = 0;
	ebook_uipara.bmp_slide_block.y = 0;
	ebook_uipara.bmp_slide_block.w = 5;
	ebook_uipara.bmp_slide_block.h = 4;
	ebook_uipara.bmp_volume_l.x	= 54;
	ebook_uipara.bmp_volume_l.y = 1;
	ebook_uipara.bmp_volume_l.w = 15;
	ebook_uipara.bmp_volume_l.h = 13;
	ebook_uipara.tips_txt_pos.x = 0;	//�ϵ���ʾ������ʾλ��
	ebook_uipara.tips_txt_pos.y = 30;
	ebook_uipara.tips_txt_pos.w = 180;
	ebook_uipara.tips_txt_pos.h = 15;
	ebook_uipara.tip_yes_bmp_pos.x = 30;//�ϵ��Ķ�''��''
	ebook_uipara.tip_yes_bmp_pos.y = 59;
	ebook_uipara.tip_yes_bmp_pos.w = 37;
	ebook_uipara.tip_yes_bmp_pos.h = 18;
	ebook_uipara.tip_no_bmp_pos.x = 118;//�ϵ��Ķ�''��''
	ebook_uipara.tip_no_bmp_pos.y = 59;
	ebook_uipara.tip_no_bmp_pos.w = 37;
	ebook_uipara.tip_no_bmp_pos.h = 18;
	ebook_uipara.loading_bmp_pos.x = 35;//���ڼ��ص�ͼƬλ��
	ebook_uipara.loading_bmp_pos.y = 0;
	ebook_uipara.loading_bmp_pos.w = 30;
	ebook_uipara.loading_bmp_pos.h = 30;
	ebook_uipara.loading_txt_pos.x = 0;//���ڼ��ص�����λ��
	ebook_uipara.loading_txt_pos.y = 30;
	ebook_uipara.loading_txt_pos.w = 100;
	ebook_uipara.loading_txt_pos.h = 60;
	return &ebook_uipara;
}

