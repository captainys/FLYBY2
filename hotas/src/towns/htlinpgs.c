#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtLinpDlg ;
extern int	HtLinpTxt ;
extern int	HtLinpTxtFunc() ;
extern int	HtLinpMsg ;
extern int	HtLinpHiddenButton ;
extern int	HtLinpYesFunc() ;
extern int	HtLinpYesBtn ;
extern int	HtLinpNoBtn ;
extern int	HtLinpNoFunc() ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTLINPGS = { "MmiInit",   8, 0 } ;

/* HtLinpDlg */

static MMIPACKET d001 = {	&HtLinpDlg,
							NULL,
							&MJ_ALERTL40,
							OFFSET(ALERTL40),
							MS_NONE
						} ;
static ALERTL40	d001d = {	MS_BTLEFTL40 | MS_EVMOVEL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_SAVEL40,
							 168, 140, 423, 195, 7, 8, 7,
							MS_PANELL40 | MS_FRAMEL40,
							NULL,   0,   0,
							   0,   0,   0,   0
						} ;

/* HtLinpTxt */

static MMIPACKET d002 = {	&HtLinpTxt,
							&HtLinpDlg,
							&MJ_TEXTL40,
							OFFSET(TEXTL40),
							MS_NONE
						} ;
static TEXTL40	d002d = 	{	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_HORIL40,
							 176, 172, 415, 187, 8,15, 8,
							MS_FRAMEL40,
							HtLinpTxtFunc,
							   0,   0,   0,   0,
							   0,   0,   0,   0,   0,
							      -1,   0,   0,
							   0,   0,   0,   0,
							   0,
							   0,   0,   0,   0,
							   1,  12,  12,   0,   0,
							   0,   0,   0,   0,   0,   0,   0,
							   0,   0,   0,   0,
							  24
						} ;

/* HtLinpMsg */

static MMIPACKET d003 = {	&HtLinpMsg,
							&HtLinpDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d003d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 176, 148, 303, 167, 8, 8,15,
							MS_NONEL40,
							"1行入力",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtLinpHiddenButton */

static MMIPACKET d004 = {	&HtLinpHiddenButton,
							&HtLinpDlg,
							&MJ_BUTTONL40,
							OFFSET(BUTTONL40),
							MS_NONE
						} ;
static BUTTONL40	d004d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 352, 148, 415, 167, 0, 0, 0,
							MS_NONEL40,
							HtLinpYesFunc,
							0x000d
						} ;

/* HtLinpYesBtn */

static MMIPACKET d005 = {	&HtLinpYesBtn,
							&HtLinpDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d005d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 320, 148, 367, 167, 7, 8, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtLinpYesFunc,
							0x8012
						} ;

/* HtLinpNoBtn */

static MMIPACKET d006 = {	&HtLinpNoBtn,
							&HtLinpDlg,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d006d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 368, 148, 415, 167, 7, 8, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtLinpNoFunc,
							0x8011
						} ;

/* NULL */

static MMIPACKET d007 = {	NULL,
							&HtLinpDlg,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d007d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 332, 148, 355, 167, 8, 8,15,
							MS_NONEL40,
							"確定",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d008 = {	NULL,
							&HtLinpDlg,
							&MJ_MSGL40,
							0,
							MS_NONE
						} ;
static MSGL40	d008d = 	{	MS_LEFTL40 | MS_DSPONLYL40,
							 380, 148, 403, 167, 8, 8,15,
							MS_NONEL40,
							"取消",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

