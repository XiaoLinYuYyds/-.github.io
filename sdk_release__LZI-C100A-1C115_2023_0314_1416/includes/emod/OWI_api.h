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
描述:
	反馈OWI版本信息.用于校验OWI_api.h与库文件版本是否一致.
返回值:
	ascii字符串.格式如:"V1.00".
----------------------------------------------------------------------------------------------------*/
extern char *OWI_MVersion(void);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MOpen
描述:
	打开且初始化指定类型的OWI设备,相同的端口不允许重复OWI_MOpen().
参数:
	a_eOwiType;	// OWI数据格式类型
	pOwiParam;		// 传递a_eOwiType需要的参数
返回:
	NULL (打开失败), 
	下层回传的句柄(打开成功).
----------------------------------------------------------------------------------------------------*/
extern __hdle OWI_MOpen(E_OWI_TYPE a_eOwiType, void *pOwiParam);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MClose
描述:
	关闭OWI_MOpen打开的OWI.
	OWI_MOpen成功的OWI,可用OWI_MClose()释放某一OWI设备占用的资源.
参数:
	hdleThis;	// OWI_MOpen打开的OWI句柄.
返回:
	EPDK_OK 或EPDK_FAIL.
----------------------------------------------------------------------------------------------------*/
extern __s32 OWI_MClose(__hdle hdleThis);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MExit
描述:
	退出OWI模块(OWI_MClose()所有OWI设备).
返回值:
	OWI_MClose()的OWI设备数量.
----------------------------------------------------------------------------------------------------*/
extern __u32 OWI_MExit(void);

/*----------------------------------------------------------------------------------------------------
Fun:
	OWI_MIoctrl
描述:
	底层函数接口.
参数:
	hdleThis;			// 属性值.
	cmd;					// 根据E_OWI_CMD、以及功能取值.
	aux, pbuffer;	// 根据cmd的需求取值.
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
	__u32			freq;										// 单位:Hz
	__u32			dutyCycle;								// 占空比.
	__u32			dutyCycle_levelTotal;			// 占空比调节等级数量.
} S_SET_PWM_PARAM, *PS_SET_PWM_PARAM;
	#define PWM_FREQ_UPPER_LIMIT					100000

#endif


/*----------------------------------- E_OWI_CMD 描述 ------------------------------------------------
cmd:
	m_eOwiCmd_getSta
描述:
	获取OWI状态(实为E_OWI_TYPE值,表示当前运行的OWI类型).
返回:
	同"描述".
------------------------------------------------------------------------------------------------------
cmd:
	m_eOwiCmd_stop
描述:
	停止发送.
返回:
	EPDK_OK或EPDK_FAIL.
------------------------------------------------------------------------------------------------------
cmd:
	m_eOwiCmd_startTx
描述:
	(重新)开始发送.
参数:
	(__u32)pbuffer; 	// 数据码原码;第1字节.
								// 用户码(地址码)原码;第3字节.
返回:
	EPDK_OK或EPDK_FAIL.
------------------------------------------------------------------------------------------------------
cmd:
	m_eOwiCmd_setPwmDutyCycle
描述:
	重新设置(已经开始输出的)PWM的占空比.
参数:
	aux;	// 占空比.
返回:
	EPDK_OK或EPDK_FAIL.
----------------------------------------------------------------------------------------------------*/

