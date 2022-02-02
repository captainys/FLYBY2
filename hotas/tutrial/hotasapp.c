#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>

/* include application header here. */

char *MenuString[]={"File",NULL};

#define BIWORKSIZE 0xf000
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();
	BiSetShadMode(BI_ON);

	HtInitialize();

	HtStartSetMenu(MenuString);
	HtSetMenuItem(1,"Quit",'Q',HtQuitProgram);
	HtEndSetMenu();

	HtStartSetShortCut();
	HtSetShortCut(BIKEY_Q,HtQuitProgram);
	HtEndSetShortCut();

	BiSetWindowName("Application");

	HtOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	/* Call Application Initializer Here */

	HtEventLoop();

	return 0;
}
