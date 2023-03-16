@echo off
title Spec自动转换工具

:START
echo ********************************
echo 	  请选择转换方式
echo ********************************
echo 0:使用默认文件名
echo 1:手动输入文件名
echo ********************************
set /p SEL=Please Select:
if %SEL%==0 goto DEFAULT
if %SEL%==1 goto IN
echo Error:Please Input(0 or 1)!!!!
goto START

rem INPUT设置为待转换的文件名 
rem OUTPUT设置为转换后的文件夹

:IN
echo 请输入待转换的文档名，例如Spec_Reg.doc
set INPUT=:
set /p INPUT=
if "%INPUT%"==":" goto IN
goto OUT

:OUT
echo 请输入转换后的文件夹，例如reg20
set OUTPUT=:
set /p OUTPUT=
if "%Output%"==":" goto OUT

rem 在这里修改INPUT和OUTPUT的默认值
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