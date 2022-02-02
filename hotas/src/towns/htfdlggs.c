#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	HtFdlgDlg ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataHTFDLGGS = { "MmiInit",   1, 0 } ;

/* HtFdlgDlg */

static MMIPACKET d001 = {	&HtFdlgDlg,
							NULL,
							&MJ_ALERTL40,
							0,
							MS_NONE
						} ;
static ALERTL40	d001d = {	MS_BTLEFTL40 | MS_EVMOVEL40 | MS_EVMOSONL40 | MS_EVKEYONL40 | MS_FIXEDL40 | MS_INVISIBLEL40,
							 168, 140, 327, 299, 8,15, 8,
							MS_NONEL40,
							NULL,   0,   0,
							   0,   0,   0,   0
						} ;

