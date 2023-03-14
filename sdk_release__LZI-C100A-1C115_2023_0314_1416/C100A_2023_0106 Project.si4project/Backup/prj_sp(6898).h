// ========================================================================================
#ifndef _PRJ_SP_H_DEF_
#define _PRJ_SP_H_DEF_


// ~~~~ ??????: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DEV_MIC_Fixed										(1<<0)
#define DEV_MIC_HotPlug									(1<<1)
#define DEV_KNOB_Volume								(1<<2)
#define DEV_HW_ECHO										(1<<3)
#define DEV_KNOB_Echo									(1<<4)
#define DEV_HW_PW_OFF								(1<<5)

#define DEV_KEY_NUMLOCK								(1<<28)
#define DEV_TCON_EX_BIT1612M					(1<<29)
#define DEV_TCON_EX_BIT1628M					(2<<29)
#define DEV_TCON_EX_										(7<<29)
#define MASK_DEV_TCON_EX 							(7<<29)	

// --------------------------------------------------------------
#define MB_TAK906B							(DEV_MIC_HotPlug |DEV_HW_PW_OFF |DEV_KNOB_Volume)
#define MB_TAK907B							(DEV_MIC_HotPlug |DEV_HW_PW_OFF |DEV_KNOB_Volume |DEV_KNOB_Echo)

// ~~~~ ????????????: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define KB_AD_TYPE(X1, X2, Y1, Y2)			(((X1<<12) |(X2<<8) |(Y1<<4) |Y2) <<16) // ??0??????????????
	#define KB_AD_TYPE1						1 // ??R:7.5K, 7.5K, 10K, 12K, 15K, 15K, 22K, 27K, 51K
	#define KB_AD_TYPE2						2 // ??R:7.5K, 7.5K, 10K, 12K, 15K, 15K, 22K, 27K, 27K, 33K
	#define KB_AD_TYPE3						3 // ??R:5.6K, 6.2K, 7.5K, 8.2K, 9.1K, 11K, 12K, 15K, 18K, 22K, 33K
	#define KB_AD_TYPE4                     4 //??-30K????R:2.7K, 3.3K, 3.9K, 5.1K, 6.2K, 8.2K, 12K, 18K, 30K, 68K, 200K 
	#define KB_AD_TYPE_CUSTOM			0xF
	
#define KB_ID(_hex) 								(_hex)
#define MASK_KB_ID								KB_ID(0xFFFF)
#define KB_TYPE_0(_hex) 					(KB_ID(0x0000)|_hex)
#define KB_TYPE_1(_hex) 					(KB_ID(0x1000)|_hex)
#define KB_TYPE_2(_hex) 					(KB_ID(0x1100)|_hex)
#define KB_TYPE__(_hex) 					(KB_ID(0xFF00)|_hex)

// --------------------------------------------------------------
#define KB_TAK_DM901_2				(KB_TYPE_0(34) |KB_AD_TYPE(KB_AD_TYPE3, KB_AD_TYPE3, 0, 0))
#define KB_JNJ_133				    (KB_TYPE_0(35) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define KB_JNJ_1C109C					(KB_TYPE_0(36) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define 	KB_JNJ_1C115_KM708KEY		(KB_TYPE_0(37) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))


#define KB_JNJ_1C115_JP1006KEY			(KB_TYPE_0(38) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define KB_JNJ_1C115_1018KEY			(KB_TYPE_0(39) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define KB_JNJ_D1801KEY					(KB_TYPE_0(40) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define KB_JNJ_M603_KEY					(KB_TYPE_0(41) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define KB_JNJ_M605_KEY					(KB_TYPE_0(42) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))
#define KB_JNJ2SIDE_KEY					(KB_TYPE_0(43) |KB_AD_TYPE(KB_AD_TYPE4, KB_AD_TYPE4, 0, 0))






// ---------------------------------------------------------------------------------------------------
// ???????????????????:
// ---------------------------------------------------------------------------------------------------
#define PRJ_KB_ID			KB_JNJ_1C115_KM708KEY//KB_JNJ_M603_KEY//KB_JNJ_1C115_KM708KEY		// KB_JNJ_1C115_JP1006KEY

#if (PRJ_KB_ID == KB_JNJ_133 || PRJ_KB_ID == KB_JNJ_1C109C)
#define PRJ_MB_ID				MB_TAK907B//???
#else
#define PRJ_MB_ID				MB_TAK906B
#endif


// ---------------------------------------------------------------------------------------------------
// -----------------------------??????-----------------------------------
#define SP_KB_ID							(PRJ_KB_ID & MASK_KB_ID)

// -----------------------------AD????????---------------------------------
#define SP_KB_AD_TYPE				((PRJ_KB_ID >>16) &0xFFFF)
#define SP_KB_AD_X1_TYPE			((SP_KB_AD_TYPE >>12) &0x0F)
#define SP_KB_AD_X2_TYPE			((SP_KB_AD_TYPE >>8) &0x0F)
#define SP_KB_AD_Y1_TYPE			((SP_KB_AD_TYPE >>4) &0x0F)
#define SP_KB_AD_Y2_TYPE			((SP_KB_AD_TYPE >>0) &0x0F)

// --------------------------- ???MIC/AUX ??????AD --------------------
	#define OP_DET_PLUGIN_BY_AD_X1								1
	#define OP_DET_PLUGIN_BY_AD_X2								2
	#define OP_DET_PLUGIN_BY_AD_Y1								3
	#define OP_DET_PLUGIN_BY_AD_Y2								4
//#define SP_DEV_DET_PLUGIN_BY_AD							OP_DET_PLUGIN_BY_AD_Y1
#define SP_DEV_DET_PLUGIN_BY_AD								0

// -----------------------------งน????????-----------------------------
#define DEV_LED_EFFECT1				1// ??????8?????????8??.

//#if ((KB_TYPE_0(0) <= SP_KB_ID) && (SP_KB_ID < KB_TYPE_1(0)))
//	#define SP_KB_TYPE 						0
//#elif (SP_KB_ID < KB_TYPE_2(0))
#if (PRJ_KB_ID == KB_JNJ_1C115_JP1006KEY)
	#define SP_KB_TYPE						DEV_LED_EFFECT1
#else	
	#define SP_KB_TYPE 						0
#endif

//#else
//#error "KB_ID not support!"
//#endif

// -----------------------------?????????MIC-----------------------------
#define SP_FixedMic				(PRJ_MB_ID & DEV_MIC_Fixed)

// -----------------------------?????????MIC-----------------------------
#define SP_HotplugMic			(PRJ_MB_ID & DEV_MIC_HotPlug)

// -----------------------------?????????????--------------------
#define SP_KnobVolume			(PRJ_MB_ID & DEV_KNOB_Volume)

// -----------------------------????งÝ????????--------------------
#define SP_KnobEcho				(PRJ_MB_ID & DEV_KNOB_Echo)

// -----------------------------??TCON ------------------------------------
#define SP_DEV_TCON_EX			(PRJ_MB_ID & MASK_DEV_TCON_EX)

// --------------------------- ????????e???-------------------------
#define SP_DEV_KEY_NUMLOCK	(PRJ_MB_ID & DEV_KEY_NUMLOCK)

#define SP_DEV_KEY_REUSE		(0 |SP_DEV_KEY_NUMLOCK)

// ---------------------------?????????----------------------------
#define SP_DEV_TP						0

// ---------------------------??????????----------------------------
#define SP_DEV_HW_PW_OFF		(PRJ_MB_ID & DEV_HW_PW_OFF)

// ---------------------------?????????????-------------------------
#define SP_DEV_HW_ECHO		(PRJ_MB_ID & DEV_HW_ECHO)

// -----------------------------???????-----------------------------
#define OP_SP_Software_Echo_AdjustByMenu		1	// ??????????????
#define OP_SP_Software_Echo_AdjustByKnob		2	// ???????????????

#if SP_DEV_HW_ECHO
	#define SP_Software_Echo							0
#else
	#if (!SP_HotplugMic)
		#define SP_Software_Echo							0
	#else
		#if SP_KnobEcho
			#define SP_Software_Echo						OP_SP_Software_Echo_AdjustByKnob
		#else
			#define SP_Software_Echo						OP_SP_Software_Echo_AdjustByMenu
		#endif
	#endif
#endif

// ---------------------------FM???????งเ?--------------------------
#define SP_FM_IC_CDT5218						1

//#define SP_FM_IC								SP_FM_IC_CDT5218
#ifndef SP_FM_IC
#define SP_FM_IC							0
#endif

// --------------------------- LCD ??? --------------------------------------
#define _W_SCR_			1024
#define _H_SCR_			600

// ---------------------------??????????-------------------------------

// ======================== ????APPLETS ============================
#define SP_APP_ALARM										0 // ???????res
#define SP_APP_CALENDAR								0 // ???????res
#define SP_APP_ATV											0 // ???????res
#define SP_APP_AUX											0 // ???????res

// ===============================================
// for system:
#define SP_PowerOffWithLowPower				1
#define SP_SimIIC												0
#define SP_AutoUpdateFirmware						1	// ????????
	#define PATH__LZ_C100_IMG_FOLDER			"x:\\update"

#define LOGO_USE_SCLAER_MODE				1

#define SP_LPressUpForFFRR				0
#define SP_FavoriteFiles						0 // ???????res

// for drv_matrixkey:
#define SP_GetDebugInfo_KeyAdSampling		0    //???????


// for app_fm:
#define SP_FM_WhenAutoSearch_PlayFreq		1

// for app_line:
	#define OP_AUX_DET_FOR_LINEIN 						1
	#define OP_AUX_DET_FOR_AVIN_LINEIN				2
	#define OP_AUX_DET_FOR_AVIN 							3
#if SP_APP_AUX
	#define SP_AUX_DET 				OP_AUX_DET_FOR_LINEIN
#else
	#define SP_AUX_DET					0
#endif

// for app_atv:
#define SP_SaveTVShortcutIdx							0	// ????????????????????
#if SP_APP_ATV
	//#undef SP_AUX_DET
	//#define SP_AUX_DET 				OP_AUX_DET_FOR_AVIN_LINEIN
#endif
#define SP_ATV_SYS_IndepSetEnable				0

// for app_movie:
#define SP_VideoPauseWhenScreenClose		0	// ??????

// for app_setting:
#define SP_APP_SETTING_ADJUST_CONTENT_BY_KEYENTER			0

// for app_photo:
#if (_W_SCR_ > 800) && (_H_SCR_ > 480)
	#define SP_APP_PHOTO_BG						0
#else
	#define SP_APP_PHOTO_BG 					1
#endif


#endif
// ========================================================================================
