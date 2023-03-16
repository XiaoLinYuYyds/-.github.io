::  此工具将ttf矢量字库转换成256级灰度或者mono格式的sft点阵字库，此点阵字库包含原矢量字库的所有字符，同时可以包含多
::  个字号，由参数决定。具体参数如下：

::      input arguments
::   0: exe file
::   1: source font file
::   2: destination font file
::   3: reverse flag.          set 1 only when the source font file is aw font
::   4: pixel mode.            1 : mono mode, 1-bit bitmap; 2 : gray mode, 8-bit bitmap.
::   5: pixel size 1
::   ...pixel size 2
::   ...pixel size 3
::   ... ...

    
	create_sft_all_char_v2_0.exe      ^
	simkai.ttf                        ^
	simkai_momo_20_32.sft             ^
	0                                 ^
	1                                 ^
	20                                ^
	32                                ^
	> log.txt
