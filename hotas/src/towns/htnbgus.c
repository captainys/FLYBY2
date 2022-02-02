#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtNbDlg ;
extern int	HtNbMsg[3] ;
extern int	HtNbNb[3] ;
extern int	HtNbYesBtn ;
extern int	HtNbYesFunc() ;
extern int	HtNbNoBtn ;
extern int	HtNbNoFunc() ;
extern int	HtNbYesMsg ;
extern int	HtNbNoMsg ;
extern int	HtNbTitle ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTNBGUS = { "MmiInit",  12, 0 } ;

/* HtNbDlg */

static MMIPACKET d001 = {	&HtNbDlg,
							NULL,
							&MJ_ALERTL40,
							OFFSET(ALERTL40),
							MS_NONE
						} ;
static ALERTL40	d001d = {	MS_BTLEFTL40 | MS_EVMOVEL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_SAVEL40,
							 104,  96, 307, 211, 7, 8, 7,
							MS_PANELL40 | MS_FRAMEL40,
							NULL,   0,   0,
							   0,   0,   0,   0
						} ;

/* HtNbMsg[0] */

static MMIPACKET d002 = {	&HtNbMsg[0],
							&HtNbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d002d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 116, 132, 203, 155, 8, 8,15,
							MS_NONEL40,
							"数値1",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtNbMsg[1] */

static MMIPACKET d003 = {	&HtNbMsg[1],
							&HtNbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d003d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 116, 156, 203, 179, 8, 8,15,
							MS_NONEL40,
							"数値2",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtNbMsg[2] */

static MMIPACKET d004 = {	&HtNbMsg[2],
							&HtNbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d004d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 116, 180, 203, 203, 8, 8,15,
							MS_NONEL40,
							"数値3",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtNbNb[0] */

static MMIPACKET d005 = {	&HtNbNb[0],
							&HtNbDlg,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d005d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_RIGHTL40,
							 204, 132, 299, 155, 7,15, 8,
							MS_FRAMEL40,
							0,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 281, 282
						} ;

/* HtNbNb[1] */

static MMIPACKET d006 = {	&HtNbNb[1],
							&HtNbDlg,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d006d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_RIGHTL40,
							 204, 156, 299, 179, 7,15, 8,
							MS_FRAMEL40,
							0,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 281, 282
						} ;

/* HtNbNb[2] */

static MMIPACKET d007 = {	&HtNbNb[2],
							&HtNbDlg,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d007d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_RIGHTL40,
							 204, 180, 299, 203, 7,15, 8,
							MS_FRAMEL40,
							0,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 281, 282
						} ;

/* HtNbYesBtn */

static MMIPACKET d008 = {	&HtNbYesBtn,
							&HtNbDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d008d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 236, 104, 267, 127, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtNbYesFunc,
							0x8012
						} ;

/* HtNbNoBtn */

static MMIPACKET d009 = {	&HtNbNoBtn,
							&HtNbDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d009d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 268, 104, 299, 127, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtNbNoFunc,
							0x8011
						} ;

/* HtNbYesMsg */

static MMIPACKET d010 = {	&HtNbYesMsg,
							&HtNbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d010d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 240, 104, 263, 127, 8, 8,15,
							MS_NONEL40,
							"確定",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtNbNoMsg */

static MMIPACKET d011 = {	&HtNbNoMsg,
							&HtNbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d011d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 272, 104, 295, 127, 8, 8,15,
							MS_NONEL40,
							"取消",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtNbTitle */

static MMIPACKET d012 = {	&HtNbTitle,
							&HtNbDlg,
							&MJ_MSGL40,
							0,
							MS_NONE
						} ;
static MSGL40	d012d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 112, 104, 235, 127, 8, 8,15,
							MS_NONEL40,
							"数値入力",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

