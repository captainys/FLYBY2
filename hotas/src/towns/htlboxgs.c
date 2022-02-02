#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtLBoxId ;
extern int	HtLBoxUpFunc() ;
extern int	HtLBoxDownFunc() ;
extern int	HtLBoxMesId ;
extern int	HtLBoxMenuId ;
extern int	HtLBoxMenuFunc() ;
extern int	HtLBoxScrId ;
extern int	HtLBoxScrolLFunc() ;
extern int	HtLBoxYesBtnId ;
extern int	HtLBoxExitFunc() ;
extern int	HtLBoxYesMesId ;
extern int	HtLBoxNoBtnId ;
extern int	HtLBoxNoMesId ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTLBOXGS = { "MmiInit",  10, 0 } ;

/* HtLBoxId */

static MMIPACKET d001 = {	&HtLBoxId,
							NULL,
							&MJ_ALERTL40,
							OFFSET(ALERTL40),
							MS_NONE
						} ;
static ALERTL40	d001d = {	MS_BTLEFTL40 | MS_EVMOVEL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_SAVEL40,
							 200, 140, 439, 339, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							NULL,   0,   0,
							   0,   0,   0,   0
						} ;

/* NULL */

static MMIPACKET d002 = {	NULL,
							&HtLBoxId,
							&MJ_ICONL40,
							OFFSET(ICONL40),
							MS_NONE
						} ;
static ICONL40	d002d = 	{	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40 | MS_LEFTL40,
							 304, 200, 319, 215, 0,15, 0,
							MS_NONEL40,
							HtLBoxUpFunc,
							0x001e, 297
						} ;

/* NULL */

static MMIPACKET d003 = {	NULL,
							&HtLBoxId,
							&MJ_ICONL40,
							OFFSET(ICONL40),
							MS_NONE
						} ;
static ICONL40	d003d = 	{	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40 | MS_LEFTL40,
							 304, 216, 319, 231, 0,15, 0,
							MS_NONEL40,
							HtLBoxDownFunc,
							0x001f, 298
						} ;

/* HtLBoxMesId */

static MMIPACKET d004 = {	&HtLBoxMesId,
							&HtLBoxId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d004d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 208, 148, 367, 171, 0,15,15,
							MS_NONEL40,
							"項目を選んでください.",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtLBoxMenuId */

static MMIPACKET d005 = {	&HtLBoxMenuId,
							&HtLBoxId,
							&MJ_LMENUL40,
							OFFSET(LMENUL40),
							MS_NONE
						} ;
static LMENUL40	d005d = {	MS_BTLEFTL40 | MS_DOUBLEL40 | MS_EVMOSONL40 | MS_LEFTL40,
							 224, 176, 407, 303, 8,15, 8,
							MS_FRAMEL40,
							HtLBoxMenuFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							0,0,0,0,0,0
						} ;

/* HtLBoxScrId */

static MMIPACKET d006 = {	&HtLBoxScrId,
							&HtLBoxId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d006d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40,
							 408, 176, 423, 303, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtLBoxScrolLFunc,
							   0,   0, 255,  16,  16,
							 263, 264
						} ;

/* HtLBoxYesBtnId */

static MMIPACKET d007 = {	&HtLBoxYesBtnId,
							&HtLBoxId,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d007d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 224, 308, 307, 331, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtLBoxExitFunc,
							0x8012
						} ;

/* HtLBoxYesMesId */

static MMIPACKET d008 = {	&HtLBoxYesMesId,
							&HtLBoxId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d008d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 248, 308, 283, 331, 0,15,15,
							MS_NONEL40,
							"確定",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtLBoxNoBtnId */

static MMIPACKET d009 = {	&HtLBoxNoBtnId,
							&HtLBoxId,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d009d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 344, 308, 423, 331, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtLBoxExitFunc,
							0x8011
						} ;

/* HtLBoxNoMesId */

static MMIPACKET d010 = {	&HtLBoxNoMesId,
							&HtLBoxId,
							&MJ_MSGL40,
							0,
							MS_NONE
						} ;
static MSGL40	d010d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 364, 308, 399, 331, 0,15,15,
							MS_NONEL40,
							"取消",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

