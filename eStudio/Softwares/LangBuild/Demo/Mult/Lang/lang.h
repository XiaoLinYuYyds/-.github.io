/*
**********************************************************************************************************************
														 ePDK
										 the Easy Portable/Player Develop Kits
*												 RES Compiler Sub-System
*
* 												(c) Copyright 2007-2010 
* 												 All Rights Reserved
*
* Moudle	: language compiler
* File		: lang.h
*
* By		: 
* Version	: v1.00
* Date		: 2010-10-25
**********************************************************************************************************************
*/


#ifndef __LANG__H__
#define __LANG__H__

// LangId 
#define	LANG_CHINESES_TYPE		0x410
#define	LANG_CHINESET_TYPE		0x420
#define	LANG_ENGLISH_TYPE		0x400

// StringID 
#define 	 STRING_BIT_RATE		0x01
#define 	 STRING_CHARGE_TIME		0x02
#define 	 STRING_CHARGED		0x03
#define 	 STRING_CLASSIC		0x04
#define 	 STRING_COLOR_BLACK		0x05
#define 	 STRING_COLOR_BLUE		0x06
#define 	 STRING_COLOR_CYAN		0x07
#define 	 STRING_COLOR_GRAY		0x08
#define 	 STRING_COLOR_GREEN		0x09
#define 	 STRING_COLOR_ORANGE		0x0a
#define 	 STRING_COLOR_PURPLE		0x0b
#define 	 STRING_COLOR_RED		0x0c
#define 	 STRING_COLOR_TRANS		0x0d
#define 	 STRING_COLOR_WHITE		0x0e
#define 	 STRING_COLOR_YELLOW		0x0f
#define 	 STRING_DBB		0x10
#define 	 STRING_ENCODE_FORMAT		0x11
#define 	 STRING_EQ		0x12
#define 	 STRING_FAST		0x13
#define 	 STRING_FOCUS_COLOR		0x14
#define 	 STRING_FILE_FORMAT		0x15
#define 	 STRING_FILE_INFO		0x16
#define 	 STRING_FILE_NAME		0x17
#define 	 STRING_FILE_SIZE		0x18
#define 	 STRING_HOUR		0x19
#define 	 STRING_JAZZ		0x1a
#define 	 STRING_KARA_OKE		0x1b
#define 	 STRING_LCD_BRIGHT		0x1c
#define 	 STRING_LCD_BRIGHT1		0x1d
#define 	 STRING_LCD_BRIGHT2		0x1e
#define 	 STRING_LCD_BRIGHT3		0x1f
#define 	 STRING_LCD_BRIGHT4		0x20
#define 	 STRING_LCD_BRIGHT5		0x21
#define 	 STRING_LYRIC_PROMPT		0x22
#define 	 STRING_MINUTE		0x23
#define 	 STRING_NATURE		0x24
#define 	 STRING_NO_BATTERY		0x25
#define 	 STRING_NORMAL		0x26
#define 	 STRING_OK_TEXT		0x27
#define 	 STRING_ONLY_ONCE		0x28
#define 	 STRING_PLAY_LAST_PROMPT		0x29
#define 	 STRING_PLAY_MODE		0x2a
#define 	 STRING_POP		0x2b
#define 	 STRING_POWER		0x2c
#define 	 STRING_RANDOM		0x2d
#define 	 STRING_REMANENT_POWER		0x2e
#define 	 STRING_REMANENT_TIME		0x2f
#define 	 STRING_ROCK		0x30
#define 	 STRING_ROTATE_ALL		0x31
#define 	 STRING_ROTATE_ONE		0x32
#define 	 STRING_SAMPLE_RATE		0x33
#define 	 STRING_SEQUENCE		0x34
#define 	 STRING_SHOW_LYRIC		0x35
#define 	 STRING_SHOW_MODE		0x36
#define 	 STRING_SLOW		0x37
#define 	 STRING_SOFT		0x38
#define 	 STRING_SPEED		0x39
#define 	 STRING_STAR_SET		0x3a
#define 	 STRING_UNFOCUS_COLOR		0x3b
#define 	 STRING_UNKNOWN		0x3c
#define 	 STRING_VERY_FAST		0x3d
#define 	 STRING_VERY_SLOW		0x3e
#define 	 STRING_WHOLE_LINE		0x3f
#define 	 STRING_STAR0		0x40
#define 	 STRING_STAR1		0x41
#define 	 STRING_STAR2		0x42
#define 	 STRING_STAR3		0x43
#define 	 STRING_STAR4		0x44
#define 	 STRING_STAR5		0x45
#define 	 STRING_ALBUMPIC		0x46
#define 	 STRING_SPECTRUM		0x47
#define 	 STRING_LYRIC		0x48
#define 	 STRING_MUSIC		0x49
#define 	 STRING_DANCE		0x4a
#define 	 STRING_VOCAL		0x4b
#define 	 STRING_BLUERED		0x4c
#define 	 STRING_REDBLUE		0x4d
#define 	 STRING_YELLOWGREEN		0x4e
#define 	 STRING_GREENYELLOW		0x4f
#define 	 STRING_ORANGEBLUE		0x50
#define 	 STRING_REDYELLOW		0x51
#define 	 STRING_BLUEGREEN		0x52
#define 	 STRING_BLACK		0x65
#define 	 STRING_BLUE		0x66
#define 	 STRING_GREEN		0x67
#define 	 STRING_RED		0x68
#define 	 STRING_WHITE		0x69
#define 	 STRING_BIG_FONT		0x6a
#define 	 STRING_SMALL_FONT		0x6b
#define 	 STRING_TTF_24		0x6c
#define 	 STRING_TTF_28		0x6d
#define 	 STRING_TTF_32		0x6e
#define 	 STRING_BACKGROUND1		0x6f
#define 	 STRING_BACKGROUND2		0x70
#define 	 STRING_BACKGROUND3		0x71
#define 	 STRING_BACKGROUND4		0x72
#define 	 STRING_ROTATE0		0x73
#define 	 STRING_ROTATE1		0x74
#define 	 STRING_ROTATE2		0x75
#define 	 STRING_ROTATE3		0x76
#define 	 STRING_BOOKMARK1		0x77
#define 	 STRING_BOOKMARK2		0x78
#define 	 STRING_BOOKMARK3		0x79
#define 	 STRING_BOOKMARK4		0x7a
#define 	 STRING_BOOKMARK5		0x7b
#define 	 STRING_BOOKMARK6		0x7c
#define 	 STRING_BOOKMARK7		0x7d
#define 	 STRING_BOOKMARK8		0x7e
#define 	 STRING_SWITCH_NO		0x7f
#define 	 STRING_SWITCH_GLIDE		0x80
#define 	 STRING_BRIGHT1		0x81
#define 	 STRING_BRIGHT2		0x82
#define 	 STRING_BRIGHT3		0x83
#define 	 STRING_BRIGHT4		0x84
#define 	 STRING_BRIGHT5		0x85
#define 	 STRING_EBOOK		0x86
#define 	 STRING_TIME1		0x87
#define 	 STRING_TIME2		0x88
#define 	 STRING_TIME3		0x89
#define 	 STRING_TIME4		0x8a
#define 	 STRING_TIME5		0x8b
#define 	 STRING_SPACE1		0x8c
#define 	 STRING_SPACE2		0x8d
#define 	 STRING_SPACE3		0x8e
#define 	 STRING_SPACE4		0x8f
#define 	 STRING_SPACE5		0x90
#define 	 STRING_MBOX1		0x91
#define 	 STRING_YES		0x92
#define 	 STRING_NO		0x93
#define 	 STRING_OUTPUT		0xc9
#define 	 STRING_DEFAULT		0xca
#define 	 STRING_BRIGHT		0xcb
#define 	 STRING_AUTOOFF		0xcc
#define 	 STRING_CLOSESCN		0xcd
#define 	 STRING_FMSEND		0xce
#define 	 STRING_SYSINFO		0xcf
#define 	 STRING_KEYTONE		0xd0
#define 	 STRING_POWER		0xd1
#define 	 STRING_LANGUAGE		0xd2
#define 	 STRING_1MINIUTE		0xd3
#define 	 STRING_5MINIUTE		0xd4
#define 	 STRING_10MINIUTE		0xd5
#define 	 STRING_15MINIUTE		0xd6
#define 	 STRING_20MINIUTE		0xd7
#define 	 STRING_30MINIUTE		0xd8
#define 	 STRING_1HOUR		0xd9
#define 	 STRING_2HOURS		0xda
#define 	 STRING_5SECOND		0xdb
#define 	 STRING_10SECOND		0xdc
#define 	 STRING_15SECOND		0xdd
#define 	 STRING_30SECOND		0xde
#define 	 STRING_TV		0xdf
#define 	 STRING_LCD		0xe0
#define 	 STRING_TVFORMAT		0xe1
#define 	 STRING_NTSC		0xe2
#define 	 STRING_PAL		0xe3
#define 	 STRING_480I		0xe4
#define 	 STRING_480P		0xe5
#define 	 STRING_576I		0xe6
#define 	 STRING_576P		0xe7
#define 	 STRING_720P_50HZ		0xe8
#define 	 STRING_720P_60HZ		0xe9
#define 	 STRING_1080P_50HZ		0xea
#define 	 STRING_1080P_60HZ		0xeb
#define 	 STRING_HDMI_720P_50HZ		0xec
#define 	 STRING_HDMI_720P_60HZ		0xed
#define 	 STRING_HDMI_1080P_50HZ		0xee
#define 	 STRING_HDMI_1080P_60HZ		0xef
#define 	 STRING_OFF		0xf0
#define 	 STRING_ON		0xf1
#define 	 STRING_CHINESES		0xf2
#define 	 STRING_CHINESET		0xf3
#define 	 STRING_ENGLISH		0xf4
#define 	 STRING_ALWAYS		0xf5
#define 	 STRING_LEVEL1		0xf6
#define 	 STRING_LEVEL2		0xf7
#define 	 STRING_LEVEL3		0xf8
#define 	 STRING_LEVEL4		0xf9
#define 	 STRING_LEVEL5		0xfa
#define 	 STRING_LEVEL6		0xfb
#define 	 STRING_LEVEL7		0xfc
#define 	 STRING_LEVEL8		0xfd
#define 	 STRING_LEVEL9		0xfe
#define 	 STRING_LEVEL10		0xff
#define 	 STRING_LEVEL11		0x100
#define 	 STRING_LEVEL12		0x101
#define 	 STRING_LEVEL13		0x102
#define 	 STRING_LEVEL14		0x103
#define 	 STRING_LEVEL15		0x104
#define 	 STRING_ADJUST		0x105
#define 	 STRING_SETTING		0x106
#define 	 STRING_SENDONOFF		0x107
#define 	 STRING_SENDREQ		0x108
#define 	 STRING_MSG		0x109
#define 	 STRING_YES		0x10a
#define 	 STRING_NO		0x10b
#define 	 STRING_TSIZE		0x10c
#define 	 STRING_FSIZE		0x10d
#define 	 STRING_STYPE		0x10e
#define 	 STRING_ADJUSTOK		0x10f
#define 	 STRING_CHARGE		0x110
#define 	 STRING_DISCHARGE		0x111
#define 	 STRING_CHARGE_TIME		0x112
#define 	 STRING_DISCHARGE_TIME		0x113
#define 	 STRING_EXIT		0x114
#define 	 STRING_CHARGEFULL		0x115
#define 	 STRING_CHANGESTA		0x116
#define 	 STRING_RVALUE		0x117
#define 	 STRING_GVALUE		0x118
#define 	 STRING_BVALUE		0x119
#define 	 STRING_GAMMA		0x11a


#endif //__LANG__H__ 


