/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:app_movie.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05   Bayden.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_MOVIE_H__
#define  __APP_MOVIE_H__

H_WIN app_movie_create(root_para_t *para);
__s32 app_movie_notify_delete_sub_scene(H_WIN hmanwin);
__s32 app_movie_notify_delete_sub_dlg_scene(H_WIN hmanwin);
__s32 app_movie_is_tv_out(H_WIN hmanwin);

#endif
