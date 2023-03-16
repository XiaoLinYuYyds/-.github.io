::  此工具将ttf矢量字库转换成256级灰度或者mono格式的sft点阵字库，此点阵字库所包含的字符由unicode列表参数决定，同时可以包含多
::  个字号，由参数决定。具体参数如下：


::      input arguments
::   0: exe file
::   1: source font file
::   2: destination font file
::   3: reverse flag.          set 1 only when the source font file is aw font
::   4: pixel mode.            1 : mono mode, 1-bit bitmap; 2 : gray mode, 8-bit bitmap.
::   5: unicode list file
::   6: pixel size 1
::   ...pixel size 2
::   ...pixel size 3
::   ... ...


    
   create_sft_spec_char_v2_0.exe     ^
   font.ttf                        ^
   font16.sft                ^
   0                                 ^
   1                                 ^
   unicode(eng_chs).txt                       ^
   16                                ^
   > log.txt
