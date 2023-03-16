程序说明

    1. script.exe        解析脚本文件成为数据文件，标准格式
       用法：

       script.exe        参数1     参数2   ......   参数n, ......

       依次把所有文件装换成数据形式，具体参考配置脚本文档

       举例
       script.exe        sys_config1.fex       sys_config2.fex   sys_config3.fex  ......
       转换结果
                         sys_config1.bin       sys_config2.bin   sys_config3.bin  ......

    2. script_old.exe    解析脚本文件成为数据文件，旧格式，字符串前面没有前缀"string"
       用法：

       script_old.exe    参数1     参数2   ......   参数n, ......

       依次把所有文件装换成数据形式，具体参考配置脚本文档

       script_old.exe    sys_config1.fex       sys_config2.fex   sys_config3.fex  ......
       转换结果
                         sys_config1.bin       sys_config2.bin   sys_config3.bin  ......

    3. update.exe        修改boot文件参数，提供给F20与以后版本使用
       用法唯一：

       update.exe        参数1(script.exe或者script_old.exe转换出的脚本数据文件)
                         参数2(boot0路径)
                         参数3(boot1路径)

    4. update_boot_15.exe    修改boot文件参数，提供给F15版本使用
    	用法唯一：

       update_boot_15.exe    参数1(script.exe或者script_old.exe转换出的脚本数据文件)
                         	 参数2(boot0路径)
                         	 参数3(boot1路径)

    5. update_mbr.exe    生成MBR文件，提供参数
    	用法：

    	update_mbr.exe       参数1(script.exe或者script_old.exe转换出的脚本数据文件)
    						 参数2(mbr路径)

    						 这种用法设定mbr份数是4，占用空间1M

    	update_mbr.exe       参数1(script.exe或者script_old.exe转换出的脚本数据文件)
    						 参数2(mbr路径)
    						 参数3(MBR份数)
    						 参数4(MBR占用空间大小，字节单位)


