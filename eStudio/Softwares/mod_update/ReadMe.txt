����˵��

    1. script.exe        �����ű��ļ���Ϊ�����ļ�����׼��ʽ
       �÷���

       script.exe        ����1     ����2   ......   ����n, ......

       ���ΰ������ļ�װ����������ʽ������ο����ýű��ĵ�

       ����
       script.exe        sys_config1.fex       sys_config2.fex   sys_config3.fex  ......
       ת�����
                         sys_config1.bin       sys_config2.bin   sys_config3.bin  ......

    2. script_old.exe    �����ű��ļ���Ϊ�����ļ����ɸ�ʽ���ַ���ǰ��û��ǰ׺"string"
       �÷���

       script_old.exe    ����1     ����2   ......   ����n, ......

       ���ΰ������ļ�װ����������ʽ������ο����ýű��ĵ�

       script_old.exe    sys_config1.fex       sys_config2.fex   sys_config3.fex  ......
       ת�����
                         sys_config1.bin       sys_config2.bin   sys_config3.bin  ......

    3. update.exe        �޸�boot�ļ��������ṩ��F20���Ժ�汾ʹ��
       �÷�Ψһ��

       update.exe        ����1(script.exe����script_old.exeת�����Ľű������ļ�)
                         ����2(boot0·��)
                         ����3(boot1·��)

    4. update_boot_15.exe    �޸�boot�ļ��������ṩ��F15�汾ʹ��
    	�÷�Ψһ��

       update_boot_15.exe    ����1(script.exe����script_old.exeת�����Ľű������ļ�)
                         	 ����2(boot0·��)
                         	 ����3(boot1·��)

    5. update_mbr.exe    ����MBR�ļ����ṩ����
    	�÷���

    	update_mbr.exe       ����1(script.exe����script_old.exeת�����Ľű������ļ�)
    						 ����2(mbr·��)

    						 �����÷��趨mbr������4��ռ�ÿռ�1M

    	update_mbr.exe       ����1(script.exe����script_old.exeת�����Ľű������ļ�)
    						 ����2(mbr·��)
    						 ����3(MBR����)
    						 ����4(MBRռ�ÿռ��С���ֽڵ�λ)


