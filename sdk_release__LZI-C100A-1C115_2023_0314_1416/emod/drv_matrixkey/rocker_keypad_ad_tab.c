#include "epdk.h"


#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE1) || (SP_KB_AD_X2_TYPE == KB_AD_TYPE1)	\
	|| (SP_KB_AD_Y1_TYPE == KB_AD_TYPE1) || (SP_KB_AD_Y2_TYPE == KB_AD_TYPE1)
	const S_KEYPAD_AD gc_sKeypadAD_type1[] =
	{
		{0, 		65},
		{221, 	351},
		{469, 	599},
		{754, 	884},
		{1041, 	1171},
		{1336, 	1466},
		{1578, 	1708},
		{1863, 	1993},
		{2134, 	2264},
		{2496, 	2626},
	};
	const __u16 gc_sKeypadAD_type1__size = sizeof(gc_sKeypadAD_type1) /sizeof(gc_sKeypadAD_type1[0]);

	#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE1)
		const PS_KEYPAD_AD gc_psKeypadAD_x1 = (PS_KEYPAD_AD)gc_sKeypadAD_type1;
		const __u16 gc_psKeypadAD_x1__size = gc_sKeypadAD_type1__size;
	#endif
	#if (SP_KB_AD_X2_TYPE == KB_AD_TYPE1)
		const PS_KEYPAD_AD gc_psKeypadAD_x2 = (PS_KEYPAD_AD)gc_sKeypadAD_type1;
		const __u16 gc_psKeypadAD_x2__size = gc_sKeypadAD_type1__size;
	#endif
	#if (SP_KB_AD_Y1_TYPE == KB_AD_TYPE1)
		const PS_KEYPAD_AD gc_psKeypadAD_y1 = (PS_KEYPAD_AD)gc_sKeypadAD_type1;
		const __u16 gc_psKeypadAD_y1__size = gc_sKeypadAD_type1__size;
	#endif
	#if (SP_KB_AD_Y2_TYPE == KB_AD_TYPE1)
		const PS_KEYPAD_AD gc_psKeypadAD_y2 = (PS_KEYPAD_AD)gc_sKeypadAD_type1;
		const __u16 gc_psKeypadAD_y2__size = gc_sKeypadAD_type1__size;
	#endif
#endif

#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE2) || (SP_KB_AD_X2_TYPE == KB_AD_TYPE2)	\
	|| (SP_KB_AD_Y1_TYPE == KB_AD_TYPE2) || (SP_KB_AD_Y2_TYPE == KB_AD_TYPE2)
	const S_KEYPAD_AD gc_sKeypadAD_type2[] =
	{
		{0, 		65},
		{221, 	351},
		{469, 	599},
		{754, 	884},
		{1041,	1171},
		{1336,	1466},
		{1578,	1708},
		{1863,	1993},
		{2134,	2264},
		{2345,	2475},
		{2546,	2676},
	};
	const __u16 gc_sKeypadAD_type2__size = sizeof(gc_sKeypadAD_type2) /sizeof(gc_sKeypadAD_type2[0]);
	
	#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE2)
		const PS_KEYPAD_AD gc_psKeypadAD_x1 = (PS_KEYPAD_AD)gc_sKeypadAD_type2;
		const __u16 gc_psKeypadAD_x1__size = gc_sKeypadAD_type2__size;
	#endif
	#if (SP_KB_AD_X2_TYPE == KB_AD_TYPE2)
		const PS_KEYPAD_AD gc_psKeypadAD_x2 = (PS_KEYPAD_AD)gc_sKeypadAD_type2;
		const __u16 gc_psKeypadAD_x2__size = gc_sKeypadAD_type2__size;
	#endif
	#if (SP_KB_AD_Y1_TYPE == KB_AD_TYPE2)
		const PS_KEYPAD_AD gc_psKeypadAD_y1 = (PS_KEYPAD_AD)gc_sKeypadAD_type2;
		const __u16 gc_psKeypadAD_y1__size = gc_sKeypadAD_type2__size;
	#endif
	#if (SP_KB_AD_Y2_TYPE == KB_AD_TYPE2)
		const PS_KEYPAD_AD gc_psKeypadAD_y2 = (PS_KEYPAD_AD)gc_sKeypadAD_type2;
		const __u16 gc_psKeypadAD_y2__size = gc_sKeypadAD_type2__size;
	#endif
#endif

#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE3) || (SP_KB_AD_X2_TYPE == KB_AD_TYPE3)	\
	|| (SP_KB_AD_Y1_TYPE == KB_AD_TYPE3) || (SP_KB_AD_Y2_TYPE == KB_AD_TYPE3)
	const S_KEYPAD_AD gc_sKeypadAD_type3[] =
	{
		{0, 		65},
		{152, 	282},
		{367, 	497},
		{597, 	727},
		{818, 	948},
		{1032, 	1162},
		{1256, 	1386},
		{1464, 	1594},
		{1685, 	1815},
		{1904, 	2034},
		{2122,	2252},
		{2376,	2506},
	};
	const __u16 gc_sKeypadAD_type3__size = sizeof(gc_sKeypadAD_type3) /sizeof(gc_sKeypadAD_type3[0]);
	
	#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE3)
		const PS_KEYPAD_AD gc_psKeypadAD_x1 = (PS_KEYPAD_AD)gc_sKeypadAD_type3;
		const __u16 gc_psKeypadAD_x1__size = gc_sKeypadAD_type3__size;
	#endif
	#if (SP_KB_AD_X2_TYPE == KB_AD_TYPE3)
		const PS_KEYPAD_AD gc_psKeypadAD_x2 = (PS_KEYPAD_AD)gc_sKeypadAD_type3;
		const __u16 gc_psKeypadAD_x2__size = gc_sKeypadAD_type3__size;
	#endif
	#if (SP_KB_AD_Y1_TYPE == KB_AD_TYPE3)
		const PS_KEYPAD_AD gc_psKeypadAD_y1 = (PS_KEYPAD_AD)gc_sKeypadAD_type3;
		const __u16 gc_psKeypadAD_y1__size = gc_sKeypadAD_type3__size;
	#endif
	#if (SP_KB_AD_Y2_TYPE == KB_AD_TYPE3)
		const PS_KEYPAD_AD gc_psKeypadAD_y2 = (PS_KEYPAD_AD)gc_sKeypadAD_type3;
		const __u16 gc_psKeypadAD_y2__size = gc_sKeypadAD_type3__size;
	#endif
#endif

#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE4) || (SP_KB_AD_X2_TYPE == KB_AD_TYPE4)	\
	|| (SP_KB_AD_Y1_TYPE == KB_AD_TYPE4) || (SP_KB_AD_Y2_TYPE == KB_AD_TYPE4)
#define KB_AD_STEP   130
	const S_KEYPAD_AD gc_sKeypadAD_type4[] =
	{
		{0, 	KB_AD_STEP},
		{338-KB_AD_STEP,	338+KB_AD_STEP},
		{683-KB_AD_STEP,	683+KB_AD_STEP},
		{1016-KB_AD_STEP,	1016+KB_AD_STEP},
		{1365-KB_AD_STEP,	1365+KB_AD_STEP},
		{1696-KB_AD_STEP,	1696+KB_AD_STEP},
		{2027-KB_AD_STEP,	2027+KB_AD_STEP},
		{2374-KB_AD_STEP,	2374+KB_AD_STEP},
		{2721-KB_AD_STEP,	2721+KB_AD_STEP},
		{3066-KB_AD_STEP,	3066+KB_AD_STEP},	
		{3395-KB_AD_STEP,	3395+KB_AD_STEP},	
		{3749-KB_AD_STEP,	3749+KB_AD_STEP},		
	};
	const __u16 gc_sKeypadAD_type4__size = sizeof(gc_sKeypadAD_type4) /sizeof(gc_sKeypadAD_type4[0]);
	#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE4)
		const PS_KEYPAD_AD gc_psKeypadAD_x1 = (PS_KEYPAD_AD)gc_sKeypadAD_type4;
		const __u16 gc_psKeypadAD_x1__size = gc_sKeypadAD_type4__size;
	#endif
	#if (SP_KB_AD_X2_TYPE == KB_AD_TYPE4)
		const PS_KEYPAD_AD gc_psKeypadAD_x2 = (PS_KEYPAD_AD)gc_sKeypadAD_type4;
		const __u16 gc_psKeypadAD_x2__size = gc_sKeypadAD_type4__size;
	#endif
	#if (SP_KB_AD_Y1_TYPE == KB_AD_TYPE4)
		const PS_KEYPAD_AD gc_psKeypadAD_y1 = (PS_KEYPAD_AD)gc_sKeypadAD_type4;
		const __u16 gc_psKeypadAD_y1__size = gc_sKeypadAD_type4__size;
	#endif
	#if (SP_KB_AD_Y2_TYPE == KB_AD_TYPE4)
		const PS_KEYPAD_AD gc_psKeypadAD_y2 = (PS_KEYPAD_AD)gc_sKeypadAD_type4;
		const __u16 gc_psKeypadAD_y2__size = gc_sKeypadAD_type4__size;
	#endif
#endif
#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE_CUSTOM) || (SP_KB_AD_X2_TYPE == KB_AD_TYPE_CUSTOM)	\
	|| (SP_KB_AD_Y1_TYPE == KB_AD_TYPE_CUSTOM) || (SP_KB_AD_Y2_TYPE == KB_AD_TYPE_CUSTOM)
	#if 0 	// Ê¾Àý:
		const S_KEYPAD_AD gc_sKeypadAD_custom_x1[] =
		{
			{0, 	70},
			{144, 	284},
			{331, 	471},
			{544, 	684},
			{759, 	899},
			{980, 	1120},
			{1162, 	1302},
			{1376, 	1516},
			{1579, 	1719},
			{1851, 	1991},
		};
		const __u16 gc_sKeypadAD_custom_x1__size = sizeof(gc_sKeypadAD_custom_x1) /sizeof(gc_sKeypadAD_custom_x1[0]);
	#endif
	
	#if (SP_KB_AD_X1_TYPE == KB_AD_TYPE_CUSTOM)
		const PS_KEYPAD_AD gc_psKeypadAD_x1 = (PS_KEYPAD_AD)gc_sKeypadAD_custom_x1;
		const __u16 gc_psKeypadAD_x1__size = gc_sKeypadAD_custom_x1__size;
	#endif
	#if (SP_KB_AD_X2_TYPE == KB_AD_TYPE_CUSTOM)
		const PS_KEYPAD_AD gc_psKeypadAD_x2 = (PS_KEYPAD_AD)gc_sKeypadAD_custom_x2;
		const __u16 gc_psKeypadAD_x2__size = gc_sKeypadAD_custom_x2__size;
	#endif
	#if (SP_KB_AD_Y1_TYPE == KB_AD_TYPE_CUSTOM)
		const PS_KEYPAD_AD gc_psKeypadAD_y1 = (PS_KEYPAD_AD)gc_sKeypadAD_custom_y1;
		const __u16 gc_psKeypadAD_y1__size = gc_sKeypadAD_custom_y1__size;
	#endif
	#if (SP_KB_AD_Y2_TYPE == KB_AD_TYPE_CUSTOM)
		const PS_KEYPAD_AD gc_psKeypadAD_y2 = (PS_KEYPAD_AD)gc_sKeypadAD_custom_y2;
		const __u16 gc_psKeypadAD_y2__size = gc_sKeypadAD_custom_y2__size;
	#endif
#endif


#if SP_GetDebugInfo_KeyAdSampling
S_ADSampling		g_sAdSampling_x1 = {NULL,  -1, -1, -1},
							g_sAdSampling_x2 = {NULL,  -1, -1, -1};
#endif

