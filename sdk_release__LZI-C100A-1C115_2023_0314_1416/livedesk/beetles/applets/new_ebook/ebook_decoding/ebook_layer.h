#ifndef		__EBOOK_LAYER_H
#define		__EBOOK_LAYER_H












H_WIN	EBOOK_8bpp_layer_creat(__u32 W, __u32 H, __s32 pipe);
__s32   EBOOK_Layer_OpenShow(H_WIN h_lyr);
void    EBOOK_Layer_Rotate(H_WIN h_lyr, __u32 w, __u32 h, __u32 rotate);

#endif



