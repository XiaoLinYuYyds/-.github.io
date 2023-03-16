REM  0 : 工具本身的路径
REM  1 : 待生成的目标文件的路径
REM  2 : 字号文件，文件内部是字号列表，字号以像素为单位
REM  3 : 源字库文件1
REM  4 : unicode列表文件1，文件内部是希望从源字库文件1提取出的字符所对应的unicode列表
REM  5 : 源字库文件2
REM  6 : unicode列表文件2，文件内部是希望从源字库文件2提取出的字符所对应的unicode列表
REM      … …


	merge_sft.exe      ^
	dst.sft            ^
    pixel_size.txt     ^
    times.sft          ^
    times.txt          ^
    simkai.sft         ^
    simkai.txt         ^
    venus.sft          ^
    venus.txt          ^
    > log.txt