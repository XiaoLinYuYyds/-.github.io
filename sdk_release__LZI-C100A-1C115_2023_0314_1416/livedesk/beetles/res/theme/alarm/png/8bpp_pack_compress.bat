::/*
::*********************************************************************************************************
::*                                                    MELIS
::*                                    the Easy Portable/Player Develop Kits
::*                                         8bpp, pack, compress config
::*
::*                                    (c) Copyright 2011-2014, Kingvan China
::*                                             All Rights Reserved
::*
::* File    : 8bpp_pack_compress.bat
::* By      : Kingvan
::* Version : v1.0
::* Date    : 2011-6-28
::* Descript: 8bpp, pack, compress
::* Update  : date                auther      ver     notes
::*           2011-6-28           Sunny       1.0     8bpp, pack, compress
::*********************************************************************************************************
::*/

@echo off

set EXEPATH=D:\8bpp_pack_compress
set PALNAME=wkm_8bpp.bmp
:START
    @echo ********************************************************
    @echo *   ѡ����Ҫ��ת��                                     *
    @echo ********************************************************
    @echo  0: ��pngת����32bpp
    @echo  1: ��32bppѹ��
    @echo  2: ��32bpp�������ѹ��
    @echo  3: ��32bppת����8bpp,Ȼ��8bppѹ��
    @echo  4: ��32bppת����8bpp,Ȼ��8bpp���
    @echo  5: ��32bppת����8bpp,Ȼ��8bpp���,��󽫴���ļ�ѹ��
    @echo  6: �˳�
    @echo ********************************************************
:SELECT
    set /p SEL=Please Select:
    if %SEL%==0     goto PNG_32bpp
    if %SEL%==1     goto 32bpp_COMPRESS
    if %SEL%==2     goto 32bpp_PACK_COMPRESS
    if %SEL%==3     goto 32bpp_8bpp_COMPRESS
    if %SEL%==4     goto 32bpp_8bpp_PACK
    if %SEL%==5     goto 32bpp_8bpp_PACK_COMPRESS
    if %SEL%==6     goto EXIT_CHANGE
    @echo �������,�밴��Ҫ��ѡ��!
    goto SELECT

:PNG_32bpp
    %EXEPATH%\png2bmp.exe
    goto START
    
:32bpp_COMPRESS
    for /f "delims=" %%i in ('dir /b /a-d /s "*.bmp"') do %EXEPATH%\az100_batch.exe e %%i tmp.o & copy tmp.o %%i
    del tmp.o 
    goto START

:32bpp_PACK_COMPRESS
    @echo ***********************************************************
    @echo	makePack.exe��.bmp�ļ������.pack,
    @echo   �û������Լ���ģ���Զ������   
    @echo ***********************************************************
    set /p SEL=Please Input Pack Name:
    %EXEPATH%\makePack.exe %SEL%
    for /f "delims=" %%i in ('dir /b /a-d /s "*.pack"') do %EXEPATH%\az100_batch.exe e %%i tmp.o & copy tmp.o %%i
    del tmp.o 
    goto START

:32bpp_8bpp_COMPRESS
    %EXEPATH%\bmp_list.exe
    %EXEPATH%\8bpp_interface.exe  bmp_list.txt        ^
    	                          custom_color.txt    ^
    	                          0xffffff            ^
    	                          0x000000            ^
    	                          %PALNAME%            
    %EXEPATH%\rename.exe
    
    for /f "delims=" %%i in ('dir /b /a-d /s "*.bmp"') do %EXEPATH%\az100_batch.exe e %%i tmp.o & copy tmp.o %%i
    del tmp.o 
    goto START

:32bpp_8bpp_PACK
    %EXEPATH%\bmp_list.exe
    %EXEPATH%\8bpp_interface.exe  bmp_list.txt        ^
    	                          custom_color.txt    ^
    	                          0xffffff            ^
    	                          0x000000            ^
    	                          %PALNAME%            
    %EXEPATH%\rename.exe

    @echo ***********************************************************
    @echo	makePack.exe��.bmp�ļ������.pack,
    @echo   �û������Լ���ģ���Զ������   
    @echo ***********************************************************
    set /p SEL=Please Input Pack Name:
    %EXEPATH%\makePack.exe %SEL%
    goto START
    
:32bpp_8bpp_PACK_COMPRESS
    %EXEPATH%\bmp_list.exe
    %EXEPATH%\8bpp_interface.exe  bmp_list.txt        ^
    	                          custom_color.txt    ^
    	                          0xffffff            ^
    	                          0x000000            ^
    	                          %PALNAME%            
    %EXEPATH%\rename.exe

    @echo ***********************************************************
    @echo	makePack.exe��.bmp�ļ������.pack,
    @echo   �û������Լ���ģ���Զ������   
    @echo ***********************************************************
    set /p SEL=Please Input Pack Name:
    %EXEPATH%\makePack.exe %SEL%

    for /f "delims=" %%i in ('dir /b /a-d /s "*.pack"') do %EXEPATH%\az100_batch.exe e %%i tmp.o & copy tmp.o %%i
    del tmp.o 
    goto START


:EXIT_CHANGE
    exit

:FINISH
    @echo *********************************************
    pause

