#ifndef _OWI_API_H_DEF_
#define _OWI_API_H_DEF_


#define STR__OWI_VER			"V1.03"

#include "epdk.h"

typedef enum
{
	m_eOwiType_invalid = 0,
	m_eOwiType_custom,
	m_eOwiType_txNec,
	m_eOwiType_pwm,
	m_eOwiType_total
} E_OWI_TYPE;

	typedef user_gpio_set_t S_GPIO_SET, *PS_GPIO_SET;

typedef struct
{
	S_GPIO_SET					sGpioSet;
}	S_OWI_PARAM_TYPE_TX_NEC, *PS_OWI_PARAM_TYPE_TX_NEC,
	S_TX_NEC_PARAM, *PS_TX_NEC_PARAM;

typedef struct
{
	S_GPIO_SET					sGpioSet;
} 	S_OWI_PARAM_TYPE_TX_PWM, *PS_OWI_PARAM_TYPE_TX_PWM,
	S_TX_PWM_PARAM, *PS_TX_PWM_PARAM;

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MGetVer
����:
	����OWI�汾��Ϣ.����У��OWI_api.h����ļ��汾�Ƿ�һ��.
����ֵ:
	ascii�ַ���.��ʽ��:"V1.00".
----------------------------------------------------------------------------------------------------*/
extern char *OWI_MVersion(void);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MOpen
����:
	���ҳ�ʼ��ָ�����͵�OWI�豸,��ͬ�Ķ˿ڲ������ظ�OWI_MOpen().
����:
	a_eOwiType;	// OWI���ݸ�ʽ����
	pOwiParam;		// ����a_eOwiType��Ҫ�Ĳ���
����:
	NULL (��ʧ��), 
	�²�ش��ľ��(�򿪳ɹ�).
----------------------------------------------------------------------------------------------------*/
extern __hdle OWI_MOpen(E_OWI_TYPE a_eOwiType, void *pOwiParam);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MClose
����:
	�ر�OWI_MOpen�򿪵�OWI.
	OWI_MOpen�ɹ���OWI,����OWI_MClose()�ͷ�ĳһOWI�豸ռ�õ���Դ.
����:
	hdleThis;	// OWI_MOpen�򿪵�OWI���.
����:
	EPDK_OK ��EPDK_FAIL.
----------------------------------------------------------------------------------------------------*/
extern __s32 OWI_MClose(__hdle hdleThis);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MExit
����:
	�˳�OWIģ��(OWI_MClose()����OWI�豸).
����ֵ:
	OWI_MClose()��OWI�豸����.
----------------------------------------------------------------------------------------------------*/
extern __u32 OWI_MExit(void);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MIoctrl
����:
	�ײ㺯���ӿ�.
����:
	hdleThis;			// ����ֵ.
	cmd;					// ����E_OWI_CMD���Լ�����ȡֵ.
	aux, pbuffer;	// ����cmd������ȡֵ.
----------------------------------------------------------------------------------------------------*/
extern __s32 OWI_MIoctrl(__hdle hdleThis, __u32 cmd, __s32 aux, void *pbuffer);
 
typedef enum
{
	m_eOwiCmd_forDrv = 0x100,		// m_eOwiCmdPriv_max
 	m_eOwiCmd_getSta = m_eOwiCmd_forDrv,

	m_eOwiCmd_forDev = 0x120,
	m_eOwiCmd_stop = m_eOwiCmd_forDev,
 	m_eOwiCmd_startTx,
	m_eOwiCmd_startRx,
	
	m_eOwiCmd_setPwmDutyCycle = 0x200,

	m_eOwiCmd_max
} E_OWI_CMD;

// for aux with NEC_CMD.m_eOwiCmd_startTx :
typedef enum
{
	m_eNecFmt_default = 0,
	m_eNecFmt_16bitsAddr_16bitsData,
	m_eNecFmt_custom,
	m_eNecFmt_total
} E_NEC_FMT;

// for E_NEC_FMT.m_eNecFmt_custom:
typedef struct
{
	__u8 			*datBuf;
	__u32			datBuf_bitWide;
} S_TX_NEC_CUSTOM, *PS_TX_NEC_CUSTOM;

// for pbuffer with PWM_CMD. m_eOwiCmd_startTx:
typedef struct
{
	__u32			freq;										// ��λ:Hz
	__u32			dutyCycle;								// ռ�ձ�.
	__u32			dutyCycle_levelTotal;			// ռ�ձȵ��ڵȼ�����.
} S_SET_PWM_PARAM, *PS_SET_PWM_PARAM;
	#define PWM_FREQ_UPPER_LIMIT					100000

#endif


/*----------------------------------- E_OWI_CMD ���� ------------------------------------------------
cmd:
	m_eOwiCmd_getSta
����:
	��ȡOWI״̬(ʵΪE_OWI_TYPEֵ,��ʾ��ǰ���е�OWI����).
����:
	ͬ"����".
------------------------------------------------------------------------------------------------------
cmd:
	m_eOwiCmd_stop
����:
	ֹͣ����.
����:
	EPDK_OK��EPDK_FAIL.
------------------------------------------------------------------------------------------------------
cmd:
	m_eOwiCmd_startTx
����:
	(����)��ʼ����.
����:
	(__u32)pbuffer; 	// ������ԭ��;��1�ֽ�.
								// �û���(��ַ��)ԭ��;��3�ֽ�.
����:
	EPDK_OK��EPDK_FAIL.
------------------------------------------------------------------------------------------------------
cmd:
	m_eOwiCmd_setPwmDutyCycle
����:
	��������(�Ѿ���ʼ�����)PWM��ռ�ձ�.
����:
	aux;	// ռ�ձ�.
����:
	EPDK_OK��EPDK_FAIL.
----------------------------------------------------------------------------------------------------*/

