#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtMbDlg ;
extern int	HtMbMsg[3] ;
extern int	HtMbYesBtn ;
extern int	HtMbYesFunc() ;
extern int	HtMbNoBtn ;
extern int	HtMbNoFunc() ;
extern int	HtMbYesMsg ;
extern int	HtMbNoMsg ;
extern int	HtMbBtn ;
extern int	HtMbBtnMsg ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTMBGUS = { "MmiInit",  10, 0 } ;

/* HtMbDlg */

static MMIPACKET d001 = {	&HtMbDlg,
							NULL,
							&MJ_ALERTL40,
							OFFSET(ALERTL40),
							MS_NONE
						} ;
static ALERTL40	d001d = {	MS_BTLEFTL40 | MS_EVMOVEL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_SAVEL40,
							 168, 140, 487, 251, 8, 8, 8,
							MS_PANELL40 | MS_FRAMEL40,
							NULL,   0,   0,
							   0,   0,   0,   0
						} ;

/* HtMbMsg[0] */

static MMIPACKET d002 = {	&HtMbMsg[0],
							&HtMbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d002d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 168, 152, 487, 171, 8,15,15,
							MS_NONEL40,
							"メッセージ",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtMbMsg[1] */

static MMIPACKET d003 = {	&HtMbMsg[1],
							&HtMbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d003d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 168, 172, 487, 191, 8,15,15,
							MS_NONEL40,
							"メッセージ",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtMbMsg[2] */

static MMIPACKET d004 = {	&HtMbMsg[2],
							&HtMbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d004d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 168, 192, 487, 211, 8,15,15,
							MS_NONEL40,
							"メッセージ",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtMbYesBtn */

static MMIPACKET d005 = {	&HtMbYesBtn,
							&HtMbDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d005d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 308, 216, 387, 239, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtMbYesFunc,
							0x8012
						} ;

/* HtMbNoBtn */

static MMIPACKET d006 = {	&HtMbNoBtn,
							&HtMbDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d006d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 396, 216, 475, 239, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtMbNoFunc,
							0x8011
						} ;

/* HtMbYesMsg */

static MMIPACKET d007 = {	&HtMbYesMsg,
							&HtMbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d007d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 316, 216, 379, 239, 8,15,15,
							MS_NONEL40,
							"ＯＫ",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtMbNoMsg */

static MMIPACKET d008 = {	&HtMbNoMsg,
							&HtMbDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d008d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 404, 216, 467, 239, 8,15,15,
							MS_NONEL40,
							"CANCEL",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtMbBtn */

static MMIPACKET d009 = {	&HtMbBtn,
							&HtMbDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d009d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 352, 216, 431, 239, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtMbYesFunc,
							0x8012
						} ;

/* HtMbBtnMsg */

static MMIPACKET d010 = {	&HtMbBtnMsg,
							&HtMbDlg,
							&MJ_MSGL40,
							0,
							MS_NONE
						} ;
static MSGL40	d010d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 360, 216, 423, 239, 8,15,15,
							MS_NONEL40,
							"ＯＫ",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

