::  �˹��߽�ttfʸ���ֿ�ת����256���ҶȻ���mono��ʽ��sft�����ֿ⣬�˵����ֿ����ԭʸ���ֿ�������ַ���ͬʱ���԰�����
::  ���ֺţ��ɲ�������������������£�

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
