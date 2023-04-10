/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : matrix_key.c
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#include "drv_matrix_key_i.h"

static __u32 g_pw_off_support = 1;//为1则代表使用PA3口检测关机，和jtag口复用

static __u32 input_row1 = 0;
static __u32 input_row2 = 0;
static __u32 input_row3 = 0;
static __u32 output_col = 0;
static __u32 output_col1 = 0;
static __u32 output_col2 = 0;

static __u32 pw_off = 0;


/* 扫描周期 */
static __s32 period;
/* 关机时间 */
static __s32 pw_off_time;
/* 按键按下延迟的时间，设为扫描周期的倍数，提高用户体验 */
static __s32 key_down_delay;

static __s32 volatile is_power_off = EPDK_FALSE;
static __krnl_stmr_t *timer = NULL;

static __u8 matrix_key_tid = 0;

//以下定义为了解决在组合按键检测情况下可能出现的按键down、up消息不配对的情况
static __u32 g_key_down_tbl[KEY_CNT] = {0};
__u32 power_count = 0;
typedef enum
{
  SW9 = 0,
  SW11,
  SW12,
  SW13,
  SW2,
  SW3,
  SW5,
  SW6,
  SW7,
  SW8,
  SW1,
  SW4,
  SW10,
  SW14,
  SW15,
  SW16,
  SW17,
  SW18,
  SW19,
  SW20,
  SW21,
  KPAD_KEY_CNT
} key_pad_enum_e;

static char *g_key_pad_str[] =
{
	"SW9",
	"SW11",
	"SW12",
	"SW13",
	"SW2",
	"SW3",
	"SW5",
	"SW6",
	"SW7",
	"SW8",
	"SW1",
	"SW4",
	"SW10",
	"SW14",
	"SW15",
	"SW16",
	"SW17",
	"SW18",
	"SW19",
	"SW20",
	"SW21",
};

typedef struct
{
	char *name;
	__u32 val;
}
key_pad_name2val_t;



static key_pad_name2val_t g_key_pad_name2val[] =
{
	{"MENU"     , KPAD_MENU},
	{"ZOOM"     , KPAD_ZOOM},
	{"ENTER"    , KPAD_ENTER},
	{"RETURN"   , KPAD_RETURN},
	{"LEFT"     , KPAD_LEFT},
	{"RIGHT"    , KPAD_RIGHT},
	{"UP"       , KPAD_UP},
	{"DOWN"     , KPAD_DOWN},
	{"VOICEUP"  , KPAD_VOICEUP},
	{"VOICEDOWN", KPAD_VOICEDOWN},
	{"POWER"    , KPAD_POWER},
	{"POWEROFF" , KPAD_POWEROFF},
	{"AUDIO"    , KPAD_MUSIC},
	{"VIDEO"    , KPAD_MOVIE},
	{"MODE"     , KPAD_MODE},
	{"TVOUT"    , KPAD_FMTUNER},
	{"SETTING"  , KPAD_SETTING},
	{"FM"    	, KPAD_FM},
	{"PHOTO"    , KPAD_PHOTO},
	{"EBOOK"    , KPAD_EBOOK},
	{"RECORD"   , KPAD_RECORD},
	{"EQ"    , KPAD_EQ},
	{"REPEAT"    , KPAD_REPEAT},
	{"NUM0"    , KPAD_NUM0},
	{"NUM1"    , KPAD_NUM1},
	{"NUM2"    , KPAD_NUM2},
	{"NUM3"    , KPAD_NUM3},
	{"NUM4"    , KPAD_NUM4},
	{"NUM5"    , KPAD_NUM5},
	{"NUM6"    , KPAD_NUM6},
	{"NUM7"    , KPAD_NUM7},
	{"NUM8"    , KPAD_NUM8},
	{"NUM9"    , KPAD_NUM9}
};

static __u32 g_key_pad_tbl[KPAD_KEY_CNT] = {0};

static __u32 key_pad_name2val(char *name)
{
	__s32 i;
	__s32 size;
	size = sizeof(g_key_pad_name2val) / sizeof(g_key_pad_name2val[0]);

	for(i = 0 ; i < size ; i++)
	{
		if(0 == eLIBs_stricmp(name, g_key_pad_name2val[i].name))
		{
			return g_key_pad_name2val[i].val;
		}
	}

	return 0;
}

/********************************************************************************************************
* Function:    matrix_key_read
* Description: read routine
* Input：
* Output：
* Other：
*********************************************************************************************************/

static __u32 matrix_key_read()
{
	__s32 data = 0;
	__s32 lever = 0;
#if 0

	if(1 == g_pw_off_support)	/* 电源键按下时优先返回 */
	{
		if(pw_off)
		{
			data = esPINS_ReadPinData(pw_off, 0);

			if(data)
			{
				__msg("return KPAD_POWEROFF\n");
				return KPAD_POWEROFF;
			}
		}
	}

	esPINS_SetPinIO(input_row1, EPDK_FALSE, NULL);
	esPINS_SetPinIO(input_row2, EPDK_FALSE, NULL);
	esPINS_SetPinIO(input_row3, EPDK_FALSE, NULL);
	esPINS_SetPinIO(output_col, EPDK_FALSE, NULL);
	esPINS_SetPinIO(output_col1, EPDK_FALSE, NULL);
	esPINS_SetPinIO(output_col2, EPDK_FALSE, NULL);
	esPINS_SetPinPull(input_row1, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(input_row2, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(input_row3, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(output_col, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(output_col1, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(output_col2, PIN_PULL_DOWN, NULL);
	data = esPINS_ReadPinData(output_col, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  9\n");
		return g_key_pad_tbl[SW9];
	}

	data = esPINS_ReadPinData(input_row1, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  2\n");
		return g_key_pad_tbl[SW2];
	}

	data = esPINS_ReadPinData(input_row2, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  6\n");
		return g_key_pad_tbl[SW6];
	}

	data = esPINS_ReadPinData(input_row3, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  8\n");
		return g_key_pad_tbl[SW8];
	}

	data = esPINS_ReadPinData(output_col1, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  1\n");
		return g_key_pad_tbl[SW1];
	}

	data = esPINS_ReadPinData(output_col2, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  4\n");
		return g_key_pad_tbl[SW4];
	}

	esPINS_SetPinIO(output_col2, EPDK_TRUE, NULL);
	esPINS_WritePinData(output_col2, 1, NULL);
	data = esPINS_ReadPinData(output_col, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  10\n");
		return g_key_pad_tbl[SW10];
	}

	data = esPINS_ReadPinData(input_row1, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  15\n");
		return g_key_pad_tbl[SW15];
	}

	data = esPINS_ReadPinData(input_row2, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  17\n");
		return g_key_pad_tbl[SW17];
	}

	data = esPINS_ReadPinData(input_row3, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  19\n");
		return g_key_pad_tbl[SW19];
	}

	data = esPINS_ReadPinData(output_col1, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  21\n");
		return g_key_pad_tbl[SW21];
	}

	esPINS_SetPinIO(output_col1, EPDK_TRUE, NULL);
	esPINS_WritePinData(output_col1, 1, NULL);
	data = esPINS_ReadPinData(output_col, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  14\n");
		return g_key_pad_tbl[SW14];
	}

	data = esPINS_ReadPinData(input_row1, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  16\n");
		return g_key_pad_tbl[SW16];
	}

	data = esPINS_ReadPinData(input_row2, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  18\n");
		return g_key_pad_tbl[SW18];
	}

	data = esPINS_ReadPinData(input_row3, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  20\n");
		return g_key_pad_tbl[SW20];
	}

	esPINS_SetPinIO(input_row3, EPDK_TRUE, NULL);
	esPINS_WritePinData(input_row3, 1, NULL);
	data = esPINS_ReadPinData(output_col, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  11\n");
		return g_key_pad_tbl[SW11];
	}

	data = esPINS_ReadPinData(input_row1, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  3\n");
		return g_key_pad_tbl[SW3];
	}

	data = esPINS_ReadPinData(input_row2, 0);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  7\n");
		return g_key_pad_tbl[SW7];
	}

	esPINS_SetPinIO(input_row2, EPDK_TRUE, NULL);
	esPINS_WritePinData(input_row2, 1, NULL);
	data = esPINS_ReadPinData(output_col, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  12\n");
		return g_key_pad_tbl[SW12];
	}

	data = esPINS_ReadPinData(input_row1, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  5\n");
		return g_key_pad_tbl[SW5];
	}

	esPINS_SetPinIO(input_row1, EPDK_TRUE, NULL);
	esPINS_WritePinData(input_row1, 1, NULL);
	data = esPINS_ReadPinData(output_col, NULL);

	if(data)
	{
		eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  13\n");
		return g_key_pad_tbl[SW13];
	}

	/*
	    data = esPINS_ReadPinData(input_row2, NULL);
		if (data)
		{
	        if(0 == g_pw_off_support)//如果powoff无效，则采用左键替代enter键
	        {
			    return KPAD_ENTER;
	        }
	        else
	        {
	            return g_key_pad_tbl[SW7];
	        }
		}
	*/
#endif
#if 0
	esPINS_SetPinIO(input_row1, EPDK_FALSE, NULL);
	esPINS_SetPinIO(input_row2, EPDK_FALSE, NULL);
	esPINS_SetPinIO(input_row3, EPDK_FALSE, NULL);
	esPINS_SetPinPull(input_row1, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(input_row2, PIN_PULL_DOWN, NULL);
	esPINS_SetPinPull(input_row3, PIN_PULL_UP, NULL);
	data = esPINS_ReadPinData(input_row1, NULL);

	if(data)
	{
		//eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  KPAD_LEFT=%d\n", data);
		return KPAD_LEFT;
	}

	data = esPINS_ReadPinData(input_row2, NULL);

	if(data)
	{
		//eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  KPAD_RIGHT=%d\n", data);
		return KPAD_RIGHT;
	}

#endif
	// -----------------------------------AD 按键扫描---------------------------------
	return rocker_scan_tp();
}

/********************************************************************************************************
* Function:    matrix_key_down
* Description: matrix key down
* Input：
* Output：
* Other：
*********************************************************************************************************/
static void matrix_key_down(__u32 key_value)
{
	__key_msg_t msg;
	msg.key_value = key_value;
	msg.flag = 1;
	dev_matrix_key_ioctrl(0, DRV_KEY_CMD_PUTKEY, 0, &msg);
	//__wrn("matrix_key_down, key_value=0x%x\n", key_value);
	g_key_down_tbl[key_value]++;
}

/********************************************************************************************************
* Function:    matrix_key_up
* Description: matrix key up
* Input：
* Output：
* Other：
*********************************************************************************************************/
static void matrix_key_up(__u32 key_value)
{
	__key_msg_t msg;

	if(g_key_down_tbl[key_value] > 0)
	{
		g_key_down_tbl[key_value]--;
	}
	else
	{
		return ;
	}

	msg.key_value = key_value;
	msg.flag = 0;
	dev_matrix_key_ioctrl(0, DRV_KEY_CMD_PUTKEY, 0, &msg);
	//__wrn("matrix_key_up, key_value=0x%x\n", key_value);
}

//全部按键松开后，用于补充发送未发送的key up消息
static void matrix_match_key_down(void)
{
	__s32 i;
	static __s32 key_cnt = sizeof(g_key_down_tbl) / sizeof(g_key_down_tbl[0]);

	for(i = 0 ; i < key_cnt ; i++)
	{
		while(g_key_down_tbl[i] > 0)
		{
			matrix_key_up(i);
			esKRNL_TimeDly(1);
		}
	}
}

/********************************************************************************************************
* Function:    power_off_timer
* Description:
* Input：
* Output：
* Other：
*********************************************************************************************************/
static void power_off_timer(void *arg)
{
	matrix_key_down(KPAD_POWEROFF);
	matrix_key_up(KPAD_POWEROFF);
	is_power_off = EPDK_TRUE;
}

/********************************************************************************************************
* Function:    matrix_key_thread
* Description: matrix key running thread
* Input：
* Output：
* Other：
*********************************************************************************************************/
static void matrix_key_thread(void *p_arg)
{
	__u32 old_key, new_key, key = 0;
	__s32 delay;
	__s32 count = 0;
	//以下定义用于自动测试
	static int auto_test_enable;
	{
		//如果是debug模式，则自动测试，随机向系统发按键消息
		if(EPDK_OK == esCFG_GetKeyValue("auto_test_para", "auto_test_enable", &auto_test_enable, 1))
		{
			__msg("***********auto_test_enable********=%d\n", auto_test_enable);
		}
		else
		{
			auto_test_enable = 0;
		}

		__msg("***********auto_test_enable********=%d\n", auto_test_enable);
	}

	while(1)
	{
		if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
		{
			break;
		}

		__here__;
		old_key = matrix_key_read();
		__msg("old_key = %x\n", old_key);
		//Tarring
		/*
		if( old_key == KPAD_ENTER)
		{
			__here__;
			count++;
		}
		else
		*/
		{
			//__here__;
			count = 0;
		}
		__msg("power_count = %d\n", power_count);

		if(power_count == 1 && count == 50)
		{
			__here__;
			esKSRV_Reset();
			__here__;
		}

		//enter键被优先检测，因此如果不是enter键，
		//说明enter键没被按下或者已经抬起，需重置定时器,防止组合键的时候关机处理不正确
		if(KPAD_POWEROFF != old_key && old_key)
		{
			__msg("stop pw-off timmer...\n");

			if(esKRNL_TmrStateGet(timer) == OS_TMR_STATE_RUNNING)
			{
				esKRNL_TmrStop(timer, OS_TMR_OPT_NONE, NULL);

				if(is_power_off)
				{
					is_power_off = EPDK_FALSE;
				}
			}
		}

		delay = 0;
		__here__;

		while(1)
		{
			esKRNL_TimeDly(period);
			__here__;
			new_key = matrix_key_read();
			__msg("new_key = %x\n", new_key);
			//Tarring
			/*
			if( new_key == KPAD_ENTER)
			{
				__here__;
				count++;
			}
			else
			*/
			{
				__here__;
				count = 0;
			}
			__msg("power_count = %d\n", power_count);

			if(power_count == 1 && count == 50)
			{
				__here__;
				// set_watchDog(1);
				esKSRV_Reset();
				__here__;
			}

			//enter键被优先检测，因此如果不是enter键，
			//说明enter键没被按下或者已经抬起，需重置定时器,防止组合键的时候关机处理不正确
			//Tarring
			//if(KPAD_ENTER != new_key && new_key)
			if(new_key)
			{
				__msg("stop pw-off timmer...\n");

				if(esKRNL_TmrStateGet(timer) == OS_TMR_STATE_RUNNING)
				{
					esKRNL_TmrStop(timer, OS_TMR_OPT_NONE, NULL);

					if(is_power_off)
					{
						is_power_off = EPDK_FALSE;
					}
				}
			}

			if(new_key)  /* 读到一个有效值 */
			{
				__here__;

				/* 前后两次读到的值不同，并且前面读到的值有效 */
				if((new_key ^ old_key) && old_key)
				{
					__msg("new_key=%d\n", new_key);
					__msg("old_key=%d\n", old_key);
					__here__;
					break;
				}

				__here__;

				if(KPAD_VOICEUP == new_key) //如按下了menu键，开始自动测试
				{
					__here__;

					if(1 == auto_test_enable)
					{
						__here__;

						while(1)
						{
							__here__;
							matrix_key_down(KPAD_RIGHT);
							esKRNL_TimeDly(2);
							matrix_key_up(KPAD_RIGHT);
							esKRNL_TimeDly(10 + eLIBs_ramdom(20));
							continue;
						}
					}
				}

				delay += period;

				//Tarring
				/*
				if (new_key == KPAD_ENTER)
				{
					__msg("start pw-off timmer...\n");
					//  start timer if not start, otherwise do nothing //
					if (esKRNL_TmrStateGet(timer) == OS_TMR_STATE_STOPPED)
					{
						key = new_key;
						__here__;
						esKRNL_TmrStart(timer);
					}
				}
				else */
				if(!(delay ^ key_down_delay))
				{
					__here__;
					/* 发送消息前检查是否延迟是否达到事先设定的阈值 */
					key = new_key;
					matrix_key_down(key);
				}

				__here__;
			}
			else if(key)  /* 按键弹起 */
			{
				__here__;
				//Tarring
				/*
				if (key == KPAD_ENTER)
				{
					__msg("stop pw-off timmer...\n");
					if (esKRNL_TmrStateGet(timer) == OS_TMR_STATE_RUNNING)
					{
						esKRNL_TmrStop(timer, OS_TMR_OPT_NONE, NULL);
						__here__;
						if (is_power_off)
						{
							__here__;
							is_power_off = EPDK_FALSE;
						}
						else
						{
							//  发送消息前检查是否延迟是否达到事先设定的阈值 //
							if (delay >= key_down_delay)
							{
								__here__;
								matrix_key_down(KPAD_ENTER);
								esKRNL_TimeDly(10);
								matrix_key_up(KPAD_ENTER);
							}
						}
					}
				}
				else
				*/
				{
					__here__;
					matrix_key_up(key);
				}
				key = 0;
				break; /* 退出内循环 */
			}
			else /* 读到一个无效值 */
			{
				//__here__;
				delay = 0;
				matrix_match_key_down();
			}
		}
	}
}

/********************************************************************************************************
* Function:    matrix_key_init
* Description: initial matrix key
* Input：
* Output：
* Other：
*********************************************************************************************************/
__s32 matrix_key_init(void)
{
	__s32            ret;
	user_gpio_set_t  gpio_set[1];

#if 0
	{
		//init key val by app_config.fex
		char *path = "d:\\app_config.bin";
		H_PARSER parser;
		__s32 ret;
		__s32 i;
		char keyname[64];
		int val;
		char str_val[32];

		do
		{
			parser = NULL;
			parser = esCFG_Init_Ex(path);

			if(NULL == parser)
			{
				__wrn("esCFG_Init_Ex fail...\n");
				break;
			}

			if(KPAD_KEY_CNT != sizeof(g_key_pad_str) / sizeof(g_key_pad_str[0]))
			{
				__wrn("element num of g_key_pad_str incorrect...\n");
				break;
			}

			for(i = 0 ; i < KPAD_KEY_CNT ; i++)
			{
				eLIBs_memset(keyname, 0, sizeof(keyname));
				eLIBs_strcpy(keyname, "key_para");
				eLIBs_strcat(keyname, "/");
				eLIBs_strcat(keyname, g_key_pad_str[i]);
				eLIBs_memset(str_val, 0, sizeof(str_val));
				ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)str_val, sizeof(str_val) / sizeof(int));

				if(EPDK_OK != ret)
				{
					__wrn("esCFG_GetKeyValue_Ex fail...\n");
					break;
				}

				g_key_pad_tbl[i] = key_pad_name2val(str_val);
				__wrn("i=%d, str_val=%s, g_key_pad_tbl[i]=0x%x\n", i, str_val, g_key_pad_tbl[i]);
			}
		}
		while(0);

		if(parser)
		{
			esCFG_Exit_Ex(parser);
			parser = NULL;
		}
	}
	
	{
		//如果是debug模式，则把pw-off口给jtag用
		__target_para_t target;
		esKSRV_GetTargetPara(&target);
		__wrn("target.debugenable=%d\n", target.debugenable);

		if(target.debugenable == EPDK_TRUE)
		{
			g_pw_off_support = 0;
		}
		else
		{
			g_pw_off_support = 1;
		}
	}

	/* 申请input_row1 */
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("matrixkey_para", "input_row1", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		input_row1 = esPINS_PinGrpReq(gpio_set, 1);

		if(!input_row1)
		{
			__wrn("request input_row1 pin failed\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__wrn("fetch para from script failed\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(input_row1, PIN_PULL_DOWN, NULL);

	if(ret)
	{
		__wrn("pull input_row1 failed\n");
		return EPDK_FAIL;
	}

	/* 申请input_row2 */
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("matrixkey_para", "input_row2", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		input_row2 = esPINS_PinGrpReq(gpio_set, 1);

		if(!input_row2)
		{
			__wrn("request input_row2 pin failed\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__wrn("fetch para from script failed\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(input_row2, PIN_PULL_DOWN, NULL);

	if(ret)
	{
		__wrn("pull input_row2 failed\n");
		return EPDK_FAIL;
	}

	/* 申请input_row3 */
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("matrixkey_para", "input_row3", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		input_row3 = esPINS_PinGrpReq(gpio_set, 1);

		if(!input_row3)
		{
			__wrn("request input_row3 pin failed\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__wrn("fetch para from script failed\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(input_row3, PIN_PULL_DOWN, NULL);

	if(ret)
	{
		__wrn("pull input_row2 failed\n");
		return EPDK_FAIL;
	}

	/* 申请output_col */
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("matrixkey_para", "output_col", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		output_col = esPINS_PinGrpReq(gpio_set, 1);

		if(!output_col)
		{
			__wrn("request output_col pin failed\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__wrn("fetch para from script failed\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(output_col, PIN_PULL_DOWN, NULL);

	if(ret)
	{
		__wrn("pull output_col failed\n");
		return EPDK_FAIL;
	}

	// 申请output_col /
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("matrixkey_para", "output_col1", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		output_col1 = esPINS_PinGrpReq(gpio_set, 1);

		if(!output_col1)
		{
			__wrn("request output_col1 pin failed\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__wrn("fetch para from script failed\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(output_col1, PIN_PULL_DOWN, NULL);

	if(ret)
	{
		__wrn("pull output_col1 failed\n");
		return EPDK_FAIL;
	}

	// 申请output_col /
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("matrixkey_para", "output_col2", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

	if(!ret)
	{
		output_col2 = esPINS_PinGrpReq(gpio_set, 1);

		if(!output_col2)
		{
			__wrn("request output_col2 pin failed\n");
			return EPDK_FAIL;
		}
	}
	else
	{
		__wrn("fetch para from script failed\n");
		return EPDK_FAIL;
	}

	ret = esPINS_SetPinPull(output_col2, PIN_PULL_DOWN, NULL);

	if(ret)
	{
		__wrn("pull output_col2 failed\n");
		return EPDK_FAIL;
	}

	if(1 == g_pw_off_support)
	{
		/* 申请Power Off */
		eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
		ret = esCFG_GetKeyValue("matrixkey_para", "pw_off", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

		if(!ret)
		{
			pw_off = esPINS_PinGrpReq(gpio_set, 1);

			if(!pw_off)
			{
				__wrn("request Power Off pin failed\n");
				return EPDK_FAIL;
			}
		}
		else
		{
			__wrn("fetch para from script failed\n");
			//return EPDK_FAIL;
		}
	}
#endif

	/* 读取扫描周期 */
	ret = esCFG_GetKeyValue("matrixkey_para", "period", &period, 1);

	if(ret)
	{
		__wrn("fetch para: period from script failed\n");
		period = 2;
	}

	__wrn("matrix key update period: %d\n", period);
	/* 读取关机时间 */
	ret = esCFG_GetKeyValue("matrixkey_para", "pw_off_time", &pw_off_time, 1);

	if(ret)
	{
		__wrn("fetch para: pw_off_time from script failed\n");
		pw_off_time = 2000;
	}

	pw_off_time = 20000;
	__wrn("power off time: %d\n", pw_off_time);
	/* 读取按键按下延迟的时间 */
	ret = esCFG_GetKeyValue("matrixkey_para", "key_down_delay", &key_down_delay, 1);

	if(ret)
	{
		__wrn("fetch para: key_down_delay from script failed\n");
		key_down_delay = 3 * period;
	}

	__wrn("key down delay: %d\n", key_down_delay);
	/* 创建timer */
	timer = esKRNL_TmrCreate(
	          pw_off_time,
	          OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC,
	          (OS_TMR_CALLBACK)power_off_timer,
	          NULL);

	if(!timer)
	{
		__wrn("create timer failed\n");
		esKRNL_TmrDel(timer);
		timer = NULL;
	}

	matrix_key_tid = esKRNL_TCreate(matrix_key_thread, 0, 0x800, KRNL_priolevel1);
	__wrn("matrix key init finish\n");
	return EPDK_OK;
}

/********************************************************************************************************
* Function:    matrix_key_exit
* Description: exit matrix key
* Input：
* Output：
* Other：
*********************************************************************************************************/
__s32 matrix_key_exit(void)
{
	if(!timer)
	{
		esKRNL_TmrDel(timer);
		__here__;
		timer = NULL;
	}

	// 删除线程
	while(1)
	{
		if(esKRNL_TDelReq(matrix_key_tid) != OS_TASK_DEL_REQ)
		{
			matrix_key_tid = 0;
			__here__;
			esKRNL_TDel(matrix_key_tid);
			__here__;
			return ;
		}
	}

	esPINS_PinGrpRel(input_row1, 0);
	esPINS_PinGrpRel(input_row2, 0);
	esPINS_PinGrpRel(input_row3, 0);
	esPINS_PinGrpRel(output_col, 0);
	esPINS_PinGrpRel(output_col1, 0);
	esPINS_PinGrpRel(output_col2, 0);

	if(1 == g_pw_off_support)
	{
		__here__;
		esPINS_PinGrpRel(pw_off, 0);
	}

	/*esKRNL_TDel(matrix_key_tid);

	if (!timer)
	{
	esKRNL_TmrDel(timer);
	timer = NULL;
	}   */
	__msg("matrix key exit finish\n");
	return EPDK_OK;
}

