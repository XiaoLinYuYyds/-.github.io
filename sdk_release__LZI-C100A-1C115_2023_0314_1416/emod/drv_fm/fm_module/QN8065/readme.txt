QN8065搜台门限设置如下(在函数void QN8065_RXSetTH(void)里面)
1.       QN8065_WriteReg(QN8065_CCA_CNT, 4);  这个参数设置范围为1-8之间,数字
越大,台越多

2.       QN8065_WriteReg(QN8065_CCA,
QN8065_ReadReg(QN8065_CCA)&~0x3f|rssiTH);,rssiTH这个函数范围在10-25之间,数值
越大,台越少

3.       QN8065_WriteReg(QN8065_CCA_SNR_TH,
QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f|snrTH),这个范围在8-11之间,数值越大,
台越少.

4.       另外#define QN8065_PILOT_CCA   ,这个宏定义是通过判断信号立体声状态
来过滤假台的. 