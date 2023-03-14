#ifndef _ROCKER_KEYPAD_H_DEF_
#define _ROCKER_KEYPAD_H_DEF_

#if SP_GetDebugInfo_KeyAdSampling
extern S_ADSampling		g_sAdSampling_x1, g_sAdSampling_x2;
#endif


extern const PS_KEYPAD_AD gc_psKeypadAD_x1;
extern const __u16 gc_psKeypadAD_x1__size;

extern const PS_KEYPAD_AD gc_psKeypadAD_x2;
extern const __u16 gc_psKeypadAD_x2__size;

extern const PS_KEYPAD_AD gc_psKeypadAD_y1;
extern const __u16 gc_psKeypadAD_y1__size;

extern const PS_KEYPAD_AD gc_psKeypadAD_y2;
extern const __u16 gc_psKeypadAD_y2__size;


extern const __u16	gc_adKeyPadX1_tab[], gc_adKeyPadX1_tabSize;
extern const __u16	gc_adKeyPadX2_tab[], gc_adKeyPadX2_tabSize;


#endif
