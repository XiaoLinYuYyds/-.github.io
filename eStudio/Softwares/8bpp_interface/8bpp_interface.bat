REM  /* Arguments */
REM  /*
REM  	0 : exe file
REM  	1 : argb bmp list file 
REM  	2 : custom color list file
REM  	3 : text_color_a
REM  	4 : text_color_b
REM  	5 : palette file 
REM  */


8bpp_interface.exe  bmp_list.txt        ^
	custom_color.txt    ^
	0xffffff            ^
	0x000000             ^
	wkm.pal            

pause