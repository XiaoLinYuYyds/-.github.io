#include "epdk.h"


#if (PRJ_KB_ID == KB_TAK_DM901_2)
const __u16	gc_adKeyPadX1_tab[] =
{
	KPAD_NUM0,
	KPAD_NUM9,
	KPAD_NUM8,
	KPAD_NUM7,
	KPAD_NUM6,
	KPAD_NUM5,
	KPAD_NUM4,
	KPAD_NUM3,
	KPAD_NUM2,
	KPAD_NUM1,
	KEY_RESERVED,
	KEY_RESERVED,
};
const __u16	gc_adKeyPadX2_tab[] =
{
	KPAD_SUBMENU,
	KPAD_MENU,
	KPAD_MOVIE,
	KPAD_MUSIC,
	KPAD_FM,
	KPAD_RECORD,
	KPAD_TV,
	KPAD_LEFT,
	KPAD_RIGHT,
	KPAD_ENTER,
	KEY_RESERVED,
	KEY_RESERVED,
};
#elif (PRJ_KB_ID == KB_JNJ_133)
const __u16	gc_adKeyPadX1_tab[] =
{
	KPAD_SUBMENU,
	KPAD_MENU,
	KPAD_MOVIE,
	KPAD_MUSIC,
	KPAD_FM,
	KPAD_RECORD,
	KPAD_PHOTO,
	KPAD_LEFT,
	KPAD_RIGHT,
	KPAD_ENTER,
	
};
const __u16	gc_adKeyPadX2_tab[] =
{
	KPAD_NUM1,
	KPAD_NUM2,
	KPAD_NUM3,
	KPAD_NUM4,
	KPAD_NUM5,
	KPAD_NUM6,
	KPAD_NUM7,
	KPAD_NUM8,
	KPAD_NUM9,
	KPAD_NUM0,
	
};


#elif (PRJ_KB_ID == KB_JNJ_1C109C)
const __u16	gc_adKeyPadX1_tab[] =
{
	KPAD_NUM6,
	KPAD_NUM7,
	KPAD_NUM8,
	KPAD_NUM9,
	KPAD_NUM0,
	KPAD_LEFT,
	KPAD_RIGHT,
	KPAD_SUBMENU,
	KPAD_ENTER,
	KPAD_MENU,
	
};
const __u16	gc_adKeyPadX2_tab[] =
{
	KPAD_NUM1,
	KPAD_NUM3,
	KPAD_NUM2,
	KPAD_NUM4,
	KPAD_NUM5,
	KPAD_MOVIE,
	KPAD_MUSIC,
	KPAD_PHOTO,
	KPAD_FM,
	KPAD_LOCK,
	
};

#elif (PRJ_KB_ID == KB_JNJ_1C115_KM708KEY)
const __u16	gc_adKeyPadX1_tab[] =
{
	KPAD_ENTER,
	KPAD_SUBMENU,
	KPAD_LOCK,
	KPAD_FM,
	KPAD_RIGHT,
	KPAD_LEFT,
	KPAD_MUSIC,
	KPAD_MOVIE,
	KPAD_MENU,


	//KPAD_MENU,
	
};
const __u16	gc_adKeyPadX2_tab[] =
{


			KPAD_NUM0,
			KPAD_NUM9,
			KPAD_NUM8,
			KPAD_NUM7,
			KPAD_NUM6,
			KPAD_NUM5,
			KPAD_NUM4,
			KPAD_NUM3,
			KPAD_NUM2,
			KPAD_NUM1,

	
};


#elif (PRJ_KB_ID == KB_JNJ_1C115_JP1006KEY	)
const __u16	gc_adKeyPadX1_tab[] =
{
	KPAD_PHOTO,
	KPAD_LOCK,
	KPAD_ENTER,
	KPAD_LEFT,
	KPAD_RIGHT,
	KPAD_NUM6,
	KPAD_NUM7,
	KPAD_NUM8,
	KPAD_NUM9,
	KPAD_NUM0,


	//KPAD_MENU,
	
};
const __u16	gc_adKeyPadX2_tab[] =
{


		KPAD_SUBMENU,
		KPAD_MENU,
		KPAD_FM,
		KPAD_MOVIE,
		KPAD_MUSIC,
		KPAD_NUM1,
		KPAD_NUM2,
		KPAD_NUM3,
		KPAD_NUM4,
		KPAD_NUM5,

	
};



#elif (PRJ_KB_ID == KB_JNJ_1C115_1018KEY	)
const __u16	gc_adKeyPadX1_tab[] =
{
	
		KPAD_SUBMENU,
		KPAD_MENU,
		KPAD_MOVIE,
		KPAD_MUSIC,
		KPAD_FM,
		KPAD_RECORD,
		KPAD_LOCK,
		KPAD_LEFT,
		KPAD_RIGHT,
		KPAD_ENTER,

	
};
const __u16	gc_adKeyPadX2_tab[] =
{

		KPAD_NUM1,
		KPAD_NUM2,
		KPAD_NUM3,
		KPAD_NUM4,
		KPAD_NUM5,
		KPAD_NUM6,
		KPAD_NUM7,
		KPAD_NUM8,
		KPAD_NUM9,
		KPAD_NUM0,
};

#elif (PRJ_KB_ID == KB_JNJ_D1801KEY)
const __u16	gc_adKeyPadX1_tab[] =
{
	
		KPAD_ENTER,
		KPAD_MENU,
		KPAD_RECORD,
		KPAD_FM,
		KPAD_NUM0,
		KPAD_NUM9,
		KPAD_NUM8,
		KPAD_NUM7,
		KPAD_NUM6,
		KEY_RESERVED,	
};
const __u16	gc_adKeyPadX2_tab[] =
{

		KPAD_NUM5,
		KPAD_NUM4,
		KPAD_NUM3,
		KPAD_NUM2,
		KPAD_NUM1,
		KPAD_RIGHT,
		KPAD_LEFT,
		KPAD_MUSIC,
		KPAD_MOVIE,
		KPAD_SUBMENU,
};
#elif (PRJ_KB_ID == KB_JNJ_M603_KEY)
const __u16	gc_adKeyPadX1_tab[] =
{
	
		KPAD_RIGHT,
		KPAD_ENTER,
		KPAD_LEFT,
		KPAD_LOCK,
		KPAD_PHOTO,
		KPAD_FM,
		KPAD_MUSIC,
		KPAD_MOVIE,
		KPAD_MENU,
		KPAD_SUBMENU,	
};
const __u16	gc_adKeyPadX2_tab[] =
{

		KPAD_NUM0,
		KPAD_NUM9,
		KPAD_NUM8,
		KPAD_NUM7,
		KPAD_NUM6,
		KPAD_NUM5,
		KPAD_NUM4,
		KPAD_NUM3,
		KPAD_NUM2,
		KPAD_NUM1,
};
#elif (PRJ_KB_ID == KB_JNJ_M605_KEY)
const __u16	gc_adKeyPadX1_tab[] =
{
	KPAD_SUBMENU,
	KPAD_MENU,
	KPAD_MOVIE,
	KPAD_MUSIC,
	KPAD_FM,
	KPAD_PHOTO,
	KPAD_RECORD,
	KPAD_EBOOK,
	KPAD_LEFT,
	KPAD_RIGHT,	
	KPAD_ENTER,
};
const __u16	gc_adKeyPadX2_tab[] =
{

	KPAD_NUM1,
	KPAD_NUM2,
	KPAD_NUM3,
	KPAD_NUM4,
	KPAD_NUM5,
	KPAD_NUM6,
	KPAD_NUM7,
	KPAD_NUM8,
	KPAD_NUM9,
	KPAD_NUM0,
	KPAD_LOCK,		
};

#elif (PRJ_KB_ID == KB_JNJ2SIDE_KEY)
const __u16 gc_adKeyPadX1_tab[] =
{	
	KPAD_SUBMENU,
	KPAD_MENU,
	KPAD_MOVIE,
	KPAD_MUSIC,
	KPAD_FM,
	KPAD_VOICEDOWN,
	KPAD_VOICEUP,
	KPAD_LEFT,
	KPAD_RIGHT,
	KPAD_ENTER,


};
const __u16 gc_adKeyPadX2_tab[] =
{
	
	KPAD_NUM1,
	KPAD_NUM2,
	KPAD_NUM3,
	KPAD_NUM4,
	KPAD_NUM5,
	KPAD_NUM6,
	KPAD_NUM7,
	KPAD_NUM8,
	KPAD_NUM9,
	KPAD_NUM0,

};

#endif

const __u16	gc_adKeyPadX1_tabSize = sizeof(gc_adKeyPadX1_tab) / sizeof(gc_adKeyPadX1_tab[0]);
const __u16	gc_adKeyPadX2_tabSize = sizeof(gc_adKeyPadX2_tab) / sizeof(gc_adKeyPadX2_tab[0]);
// ---------------------------------------------------------------------------------------------------

