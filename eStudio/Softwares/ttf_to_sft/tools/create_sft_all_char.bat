REM  此工具将ttf矢量字库转换成256级灰度的sft点阵字库，此点阵字库包含原矢量字库的所有字符，同时可以包含多
REM  个字号，由参数决定。具体参数如下：

REM  input arguments
REM  0: exe file
REM  1: source ttf font file
REM  2: destination sft font file
REM  3: reverse flag.          set 1 only when the source font file is sw.ttf font
REM  4: pixel size 1
REM  ...pixel size 2
REM  ...pixel size 3
REM  ... ...

    
	create_sft_all_char.exe      ^
	simkai.ttf                   ^
	simkai_20_32.sft             ^
	0                            ^
	20                           ^
	32                           ^
	> log.txt

