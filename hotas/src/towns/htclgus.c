#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtColorPanelId ;
extern int	HtInputColorIconId ;
extern int	HtColNumId[6] ;
extern int	HtColNumberFunc() ;
extern int	HtColBarId[6] ;
extern int	HtColBarFunc() ;
extern int	HtColGrbYesBtnId ;
extern int	HtColExitFunc() ;
extern int	HtColGrbNoBtnId ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTCLGUS = { "MmiInit",  25, 0 } ;

/* HtColorPanelId */

static MMIPACKET d001 = {	&HtColorPanelId,
							NULL,
							&MJ_ALERTL40,
							OFFSET(ALERTL40),
							MS_NONE
						} ;
static ALERTL40	d001d = {	MS_BTLEFTL40 | MS_EVMOVEL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 131, 121, 510, 360, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							NULL,   0,   0,
							   0,   0,   0,   0
						} ;

/* NULL */

static MMIPACKET d002 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d002d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 191, 136, 350, 159, 0,15,15,
							MS_NONEL40,
							"カラー指定",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtInputColorIconId */

static MMIPACKET d003 = {	&HtInputColorIconId,
							&HtColorPanelId,
							&MJ_ICONL40,
							OFFSET(ICONL40),
							MS_NONE
						} ;
static ICONL40	d003d = 	{	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40 | MS_LEFTL40,
							 329, 133, 392, 164, 8,15, 8,
							MS_NONEL40,
							0,
							0x0000,   0
						} ;

/* NULL */

static MMIPACKET d004 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d004d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 153, 164, 184, 187, 0,15,15,
							MS_NONEL40,
							"Hue",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d005 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d005d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 153, 186, 184, 209, 0,15,15,
							MS_NONEL40,
							"Satur",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d006 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d006d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 153, 208, 184, 231, 0,15,15,
							MS_NONEL40,
							"VAUE",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d007 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d007d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 153, 243, 184, 266, 0,15,15,
							MS_NONEL40,
							"Green",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d008 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d008d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 151, 266, 182, 289, 0,15,15,
							MS_NONEL40,
							"Red",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d009 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d009d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 151, 289, 182, 312, 0,15,15,
							MS_NONEL40,
							"Blue",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* HtColNumId[0] */

static MMIPACKET d010 = {	&HtColNumId[0],
							&HtColorPanelId,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d010d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 190, 170, 249, 185, 8,15, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColNumberFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0,1535,   1,   0,
							 263, 264
						} ;

/* HtColNumId[1] */

static MMIPACKET d011 = {	&HtColNumId[1],
							&HtColorPanelId,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d011d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 190, 192, 249, 207, 8,15, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColNumberFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 263, 264
						} ;

/* HtColNumId[2] */

static MMIPACKET d012 = {	&HtColNumId[2],
							&HtColorPanelId,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d012d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 190, 214, 249, 229, 8,15, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColNumberFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 263, 264
						} ;

/* HtColNumId[3] */

static MMIPACKET d013 = {	&HtColNumId[3],
							&HtColorPanelId,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d013d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 190, 249, 249, 264, 8,15, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColNumberFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 263, 264
						} ;

/* HtColNumId[4] */

static MMIPACKET d014 = {	&HtColNumId[4],
							&HtColorPanelId,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d014d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 190, 271, 249, 286, 8,15, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColNumberFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 263, 264
						} ;

/* HtColNumId[5] */

static MMIPACKET d015 = {	&HtColNumId[5],
							&HtColorPanelId,
							&MJ_NUMBOXL40,
							OFFSET(NUMBOXL40),
							MS_NONE
						} ;
static NUMBOXL40	d015d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_EVKEYONL40,
							 190, 293, 249, 308, 8,15, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColNumberFunc,
							   1,  12,  12,
							MS_NONEL40,
							   0,   0,
							   0,   0, 255,   1,   0,
							 263, 264
						} ;

/* HtColBarId[0] */

static MMIPACKET d016 = {	&HtColBarId[0],
							&HtColorPanelId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d016d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_HORIL40,
							 251, 170, 490, 185, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtColBarFunc,
							  16,  16,1551,  16,  32,
							 265, 266
						} ;

/* HtColBarId[1] */

static MMIPACKET d017 = {	&HtColBarId[1],
							&HtColorPanelId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d017d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_HORIL40,
							 251, 192, 430, 207, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtColBarFunc,
							  16,  16, 271,  16,  16,
							 265, 266
						} ;

/* HtColBarId[2] */

static MMIPACKET d018 = {	&HtColBarId[2],
							&HtColorPanelId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d018d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_HORIL40,
							 251, 214, 430, 229, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtColBarFunc,
							  16,  16, 271,  16,  16,
							 265, 266
						} ;

/* HtColBarId[3] */

static MMIPACKET d019 = {	&HtColBarId[3],
							&HtColorPanelId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d019d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_HORIL40,
							 252, 248, 431, 263, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtColBarFunc,
							  16,  16, 271,  16,  16,
							 265, 266
						} ;

/* HtColBarId[4] */

static MMIPACKET d020 = {	&HtColBarId[4],
							&HtColorPanelId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d020d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_HORIL40,
							 252, 272, 431, 287, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtColBarFunc,
							  16,  16, 271,  16,  16,
							 265, 266
						} ;

/* HtColBarId[5] */

static MMIPACKET d021 = {	&HtColBarId[5],
							&HtColorPanelId,
							&MJ_SCRLL40,
							OFFSET(SCRLL40),
							MS_NONE
						} ;
static SCRLL40	d021d = 	{	MS_BTLEFTL40 | MS_REPEATL40 | MS_EVMOSONL40 | MS_HORIL40,
							 252, 293, 431, 308, 8,15, 7,
							MS_PANELL40 | MS_FRAMEL40,
							HtColBarFunc,
							  16,  16, 271,  16,  16,
							 265, 266
						} ;

/* HtColGrbYesBtnId */

static MMIPACKET d022 = {	&HtColGrbYesBtnId,
							&HtColorPanelId,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d022d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 332, 319, 395, 342, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColExitFunc,
							0x8012
						} ;

/* HtColGrbNoBtnId */

static MMIPACKET d023 = {	&HtColGrbNoBtnId,
							&HtColorPanelId,
							&MJ_DBUTTONL40,
							OFFSET(DBUTTONL40),
							MS_NONE
						} ;
static DBUTTONL40	d023d = {	MS_BTLEFTL40 | MS_EVMOSOFFL40 | MS_EVKEYONL40,
							 413, 319, 476, 342, 8, 7, 8,
							MS_PANELL40 | MS_FRAMEL40,
							HtColExitFunc,
							0x8011
						} ;

/* NULL */

static MMIPACKET d024 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							OFFSET(MSGL40),
							MS_NONE
						} ;
static MSGL40	d024d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 347, 319, 378, 342, 0,15,15,
							MS_NONEL40,
							"確定",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

/* NULL */

static MMIPACKET d025 = {	NULL,
							&HtColorPanelId,
							&MJ_MSGL40,
							0,
							MS_NONE
						} ;
static MSGL40	d025d = 	{	MS_CENTERL40 | MS_DSPONLYL40,
							 431, 319, 462, 342, 8,15,15,
							MS_NONEL40,
							"取消",
							   1,  12,  12,
							MS_NONEL40,
							   0,   0
						} ;

