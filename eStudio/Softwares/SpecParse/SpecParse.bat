@echo off
title Spec�Զ�ת������

:START
echo ********************************
echo 	  ��ѡ��ת����ʽ
echo ********************************
echo 0:ʹ��Ĭ���ļ���
echo 1:�ֶ������ļ���
echo ********************************
set /p SEL=Please Select:
if %SEL%==0 goto DEFAULT
if %SEL%==1 goto IN
echo Error:Please Input(0 or 1)!!!!
goto START

rem INPUT����Ϊ��ת�����ļ��� 
rem OUTPUT����Ϊת������ļ���

:IN
echo �������ת�����ĵ���������Spec_Reg.doc
set INPUT=:
set /p INPUT=
if "%INPUT%"==":" goto IN
goto OUT

:OUT
echo ������ת������ļ��У�����reg20
set OUTPUT=:
set /p OUTPUT=
if "%Output%"==":" goto OUT

rem �������޸�INPUT��OUTPUT��Ĭ��ֵ
:DEFAULT
set INPUT=Spec_Reg.doc
set OUTPUT=reg20
goto MAIN

:MAIN
@echo on
WordToTxt .\%INPUT% .\ >log.txt
SpecParse -off	.\txt_off\file_off.txt    .\%OUTPUT% >>log.txt
SpecParse -base	.\txt_base\file_base.txt  .\%OUTPUT% >>log.txt

pause