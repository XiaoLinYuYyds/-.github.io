
#include "linein_uipara.h"

#if SP_APP_AUX


static linein_uipara_t linein_uipara_800_480 =
    {	{0,0,800,480},
      {(800-100)/2,(480-80)/2, 160, 32},
      {(800-100)/2-48,(480-80)/2+9, 32, 28},
      {ID_MUSIC_BACK0_BMP, ID_MUSIC_BACK1_BMP},
    };

linein_uipara_t *app_linein_get_uipara()
{
	return &linein_uipara_800_480;
}


#endif
