QN8065��̨������������(�ں���void QN8065_RXSetTH(void)����)
1.       QN8065_WriteReg(QN8065_CCA_CNT, 4);  ����������÷�ΧΪ1-8֮��,����
Խ��,̨Խ��

2.       QN8065_WriteReg(QN8065_CCA,
QN8065_ReadReg(QN8065_CCA)&~0x3f|rssiTH);,rssiTH���������Χ��10-25֮��,��ֵ
Խ��,̨Խ��

3.       QN8065_WriteReg(QN8065_CCA_SNR_TH,
QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f|snrTH),�����Χ��8-11֮��,��ֵԽ��,
̨Խ��.

4.       ����#define QN8065_PILOT_CCA   ,����궨����ͨ���ж��ź�������״̬
�����˼�̨��. 