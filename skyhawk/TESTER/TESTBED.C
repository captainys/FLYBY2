#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>

/* include application header here. */

#define BIWORKSIZE 0xf000
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

static void TesterMain(void);

BIAPPENTRY
{
	BIPROJ prj;

	BIAPPINIT;
	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();
	BiSetShadMode(BI_ON);

	HtInitialize();

	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	TesterMain();

	BiCloseWindow();
	return 0;
}

static void TesterMain(void)
{
	int ac,n;
	char str[128],*av[16];

	while(HtLineInput(str,"","")==HT_YES)
	{
		SkArguments(&ac,av,15,str);
		av[ac]=NULL;
		HtListBox(&n,av,"",NULL,NULL);
	}
}
