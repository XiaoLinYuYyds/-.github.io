::  �˹��߽�ttfʸ���ֿ�ת����256���ҶȻ���mono��ʽ��sft�����ֿ⣬�˵����ֿ����������ַ���unicode�б����������ͬʱ���԰�����
::  ���ֺţ��ɲ�������������������£�


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
