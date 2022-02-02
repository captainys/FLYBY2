#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtDlg ;
extern int	MJ_BASEOBJ ;
extern int	HtPd ;
extern int	HtPdTw ;
extern int	HtPdTwLst ;
extern int	HtFmtMenuSrc ;
extern int	HtFmtMenuLstSrc ;
extern int	HtFmtMenuItemSrc ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTFMTDLG = { "MmiInit",   7, 0 } ;

/* HtDlg */

static MMIPACKET d001 = {	&HtDlg,
							&MJ_BASEOBJ,
							&MJ_DIALOGL40,
							OFFSET(DIALOGL40),
							MS_NONE
						} ;
static DIALOGL40	d001d = {	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_FIXEDL40,
							   0,   0, 511,  19, 8, 9, 8,
							MS_FRAMEL40,
							NULL,   0,   0
						} ;

/* HtPd */

static MMIPACKET d002 = {	&HtPd,
							&HtDlg,
							&MJ_MENUL40,
							OFFSET(MENUL40),
							MS_NONE
						} ;
static MENUL40	d002d = 	{	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_SKEYL40 | MS_CHECKL40 | MS_HORIL40,
							   0,   0, 411,  19, 8, 7, 8,
							MS_NONEL40,
							NULL,   0
						} ;

/* HtPdTw */

static MMIPACKET d003 = {	&HtPdTw,
							&HtPd,
							&MJ_MITEML40,
							OFFSET(MITEML40),
							MS_NONE
						} ;
static MITEML40	d003d = {	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_LEFTL40,
							   0,   0, 111,  19, 8, 1,15,
							MS_PANELL40,
							0,
							0x0000, 295,
							NULL,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0
						} ;

/* HtPdTwLst */

static MMIPACKET d004 = {	&HtPdTwLst,
							&HtPdTw,
							&MJ_MENUL40,
							OFFSET(MENUL40),
							MS_NONE
						} ;
static MENUL40	d004d = 	{	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_SKEYL40 | MS_CHECKL40 | MS_SAVEL40,
							   0,  20, 127,  43, 8,15, 8,
							MS_NONEL40,
							NULL,   0
						} ;

/* HtFmtMenuSrc */

static MMIPACKET d005 = {	&HtFmtMenuSrc,
							NULL,
							&MJ_MITEML40,
							OFFSET(MITEML40),
							MS_NONE
						} ;
static MITEML40	d005d = {	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_LEFTL40,
							 116, 136, 227, 155, 8, 9,15,
							MS_PANELL40,
							0,
							0x0000,  -1,
							"Menu",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0
						} ;

/* HtFmtMenuLstSrc */

static MMIPACKET d006 = {	&HtFmtMenuLstSrc,
							NULL,
							&MJ_MENUL40,
							OFFSET(MENUL40),
							MS_NONE
						} ;
static MENUL40	d006d = 	{	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_SKEYL40 | MS_CHECKL40 | MS_SAVEL40,
							 116, 160, 243, 175, 8,15, 8,
							MS_NONEL40,
							NULL,   0
						} ;

/* HtFmtMenuItemSrc */

static MMIPACKET d007 = {	&HtFmtMenuItemSrc,
							NULL,
							&MJ_MITEML40,
							0,
							MS_NONE
						} ;
static MITEML40	d007d = {	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_LEFTL40,
							 116, 180, 227, 199, 8,15, 8,
							MS_NONEL40,
							0,
							0x0000,  -1,
							"MenuItem",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0
						} ;

