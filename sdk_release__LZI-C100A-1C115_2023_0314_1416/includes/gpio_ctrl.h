#ifndef _GPIO_CTRL_H_DEF_
#define _GPIO_CTRL_H_DEF_

#define PORTA			1
#define PORTB			2
#define PORTC			3
#define PORTD			4
#define PORTE				5
#define PORTF				6


// 输入:
#if SP_DEV_HW_PW_OFF
	#define IOCTRL__PWOFF_DET__IS_OFF()			(0)
	#define IOCTRL__PWOFF_DET__SET_OFF() 		
#else
	#define PORT__PWOFF_DET 								PORTA
	#define PORT_ID__PWOFF_DET							3
	#define IOCTRL__PWOFF_DET__GET()				( get_gpio_status(PORT__PWOFF_DET, PORT_ID__PWOFF_DET) )
	#define IOCTRL__PWOFF_DET__IS_OFF()			( IOCTRL__PWOFF_DET__GET() == 0 )
	#define IOCTRL__PWOFF_DET__SET_OFF() 		pull_down_gpio(PORT__PWOFF_DET, PORT_ID__PWOFF_DET)
#endif

#define PORT__MIC_DET										PORTE//麦克风
#define PORT_ID__MIC_DET									9
#define IOCTRL__MIC_DET__GET()						( get_gpio_status_pullup(PORT__MIC_DET, PORT_ID__MIC_DET) )
#define IOCTRL__MIC_DET__IS_PLUGIN()			( IOCTRL__MIC_DET__GET() == 0 )
#ifndef IOCTRL__MIC_DET__IS_PLUGIN
extern __bool IOCTRL__MIC_DET__IS_PLUGIN(void);
#endif

#if SP_AUX_DET
	#define STA__AUX__PLUGIN					0
	#define STA__AUX__PLUGOUT					1
	#define PORT__AUX_DET										PORTE	// 使用AD检测
	#define PORT_ID__AUX_DET								19
	#define IOCTRL__AUX_DET__IS_PLUGIN()			0	//( get_gpio_status(PORT__AUX_DET, PORT_ID__AUX_DET) == STA__AUX__PLUGIN )
	#ifndef IOCTRL__AUX_DET__IS_PLUGIN
	extern __s32 app_root_is_linein_insert(void);
	#define IOCTRL__AUX_DET__IS_PLUGIN			app_root_is_linein_insert
	#endif
#endif

#define PORT__DVD_STA										PORTE
#define PORT_ID__DVD_STA								15
#define IOCTRL__DVD_STA__GET()						( get_gpio_status(PORT__DVD_STA, PORT_ID__DVD_STA) )
#define IOCTRL__DVD_STA__IS_DISP_EN()		(0)//(1 == IOCTRL__DVD_STA__GET())
#define IOCTRL__DVD_STA__IS_PA_ON()			(0)//(0 == IOCTRL__DVD_STA__GET())

// 输出:
#define PORT__ENCRY											PORTE
#define PORT_ID__ENCRY									11
#define IOCTRL__ENCRY__SET_OK()					pull_up_gpio(PORT__ENCRY, PORT_ID__ENCRY);
#define IOCTRL__ENCRY__SET_FAIL() 				pull_down_gpio(PORT__ENCRY, PORT_ID__ENCRY);

#if SP_DEV_HW_PW_OFF
	#define IOCTRL__PWON__SET_ON()					
	#define IOCTRL__PWON__SET_OFF() 				
#else
	#define PORT__PWON											PORTC
	#define PORT_ID__PWON 									8
	#define IOCTRL__PWON__SET_ON()					pull_up_gpio(PORT__PWON, PORT_ID__PWON);		
	#define IOCTRL__PWON__SET_OFF() 				pull_down_gpio(PORT__PWON, PORT_ID__PWON);	
#endif

#define PORT__PA_SHDN										PORTA//功放
#define PORT_ID__PA_SHDN 								2
#define IOCTRL__PA_SHDN__SET_DIS()				pull_up_gpio(PORT__PA_SHDN, PORT_ID__PA_SHDN)			//打开功放 	
#define IOCTRL__PA_SHDN__SET_EN()				pull_down_gpio(PORT__PA_SHDN, PORT_ID__PA_SHDN)		//关闭功放
#define IOCTRL__PA_SHDN__XOR()					pull_xor_gpio(PORT__PA_SHDN, PORT_ID__PA_SHDN)
#define IOCTRL__PA_SHDN__IS_DIS()				(get_gpio_out_status(PORT__PA_SHDN, PORT_ID__PA_SHDN) != 0 )

#define PORT__PA_MODE										PORTE//功放类型
#define PORT_ID__PA_MODE								8
#define IOCTRL__PA_MODE__SET_AB()				pull_down_gpio(PORT__PA_MODE, PORT_ID__PA_MODE)		//AB类(模拟,功耗大)
#define IOCTRL__PA_MODE__SET_D()				pull_up_gpio(PORT__PA_MODE, PORT_ID__PA_MODE)			//D类(数字,辐射大)

#if SP_APP_ATV
#if 0
	#define PORT__TCON_RST									PORTD
	#define PORT_ID__TCON_RST								17
	#define IOCTRL__TCON_RST__SET_EN()			pull_down_gpio(PORT__TCON_RST, PORT_ID__TCON_RST);
	#define IOCTRL__TCON_RST__SET_DIS() 			pull_up_gpio(PORT__TCON_RST, PORT_ID__TCON_RST);
	#define IOCTRL__TCON_RST()								\
		do{	\
			IOCTRL__TCON_RST__SET_EN();	esKRNL_TimeDly(15); IOCTRL__TCON_RST__SET_DIS();	\
		}while(0)
#else
	#define IOCTRL__TCON_RST__SET_EN()
	#define IOCTRL__TCON_RST__SET_DIS()
	#define IOCTRL__TCON_RST()
#endif
	
	#define PORT__ATV_PW										PORTE
	#define PORT_ID__ATV_PW									10
	#define IOCTRL__ATV_PW__SET_ON()			//	pull_up_gpio(PORT__ATV_PW, PORT_ID__ATV_PW);		//打开ATV电源
	#define IOCTRL__ATV_PW__SET_OFF() 				\
		do{	\
			pull_down_gpio(PORT__ATV_PW, PORT_ID__ATV_PW);	\
			IOCTRL__TCON_RST__SET_EN();	\
		}while(0)	//关闭ATV电源
														
#else
	#define IOCTRL__TCON_RST__SET_EN()
	#define IOCTRL__TCON_RST__SET_DIS()
	#define IOCTRL__TCON_RST()
	
	#define IOCTRL__ATV_PW__SET_ON()				
	#define IOCTRL__ATV_PW__SET_OFF() 			
#endif

#define PORT__IR_TX											PORTA//TX
#define PORT_ID__IR_TX									3
#define STA__IR_TX__INIT								1

#define PORT__USB_PW										PORTA//usb电源
#define PORT_ID__USB_PW									2
#define IOCTRL__USB_PW__SET_ON()				//pull_up_gpio(PORT__USB_PW, PORT_ID__USB_PW)
#define IOCTRL__USB_PW__SET_OFF()				//pull_down_gpio(PORT__USB_PW, PORT_ID__USB_PW)

#define PORT__DVD_PW										PORTE
#define PORT_ID__DVD_PW									16
#define IOCTRL__DVD_PW__SET_ON()				//pull_up_gpio(PORT__DVD_PW, PORT_ID__DVD_PW)
#define IOCTRL__DVD_PW__SET_OFF()				//pull_down_gpio(PORT__DVD_PW, PORT_ID__DVD_PW)

#define PORT__BT_PW											PORTA
#define PORT_ID__BT_PW									3
#define IOCTRL__BT_PW__SET_ON()					//pull_up_gpio(PORT__BT_PW, PORT_ID__BT_PW)
#define IOCTRL__BT_PW__SET_OFF()				//pull_down_gpio(PORT__BT_PW, PORT_ID__BT_PW)


#if (SP_KB_TYPE == DEV_LED_EFFECT1)
#define PORT__LEDKEY											PORTE
#define PORT_ID__LEDKEY									5 
#define IOCTRL__LEDKEY__SET_ON()				pull_up_gpio(PORT__LEDKEY, PORT_ID__LEDKEY);
#define IOCTRL__LEDKEY__SET_OFF()				pull_down_gpio(PORT__LEDKEY, PORT_ID__LEDKEY);
#endif


#endif
