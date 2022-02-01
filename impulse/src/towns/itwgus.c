#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

extern int	BiWndId ;
extern int	MJ_BASEOBJ ;
extern int	BiWindowFunc() ;

#define ALIGN	4
#define OFFSET(type) (sizeof(MMIPACKET) + (sizeof(type)+ALIGN-1)/ALIGN*ALIGN)

/*	MMI_init 用データ	*/

/*	ヘッダ	*/

MMIINIT	initDataITWGUS = { "MmiInit",   1, 0 } ;

/* BiWndId */

static MMIPACKET d001 = {	&BiWndId,
							&MJ_BASEOBJ,
							&MJ_WINDOWL40,
							0,
							MS_NONE
						} ;
static WINDOWL40	d001d = {	MS_BTLEFTL40 | MS_EVMOSONL40 | MS_TITLEL40 | MS_RESIZEL40 | MS_AUTOERASEL40 | MS_SAVEL40,
							  40,  72, 443, 395, 8,15, 9,
							MS_FRAMEL40,
							NULL,   0,   0,
							   1,  17,   0, 240,
							 128,  96, 640, 480,
							BiWindowFunc,
							"BlueImpulseLibrary",
						} ;

