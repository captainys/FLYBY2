#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>

/* include application header here. */

char *MenuString[]={"File",NULL};

#define BIWORKSIZE 0x40000
char BiWork[BIWORKSIZE];

#define BICLICKBUFSIZE 0x60000L
char BiClick[BICLICKBUFSIZE];

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiSetShadMode(BI_ON);
	BiRenderInitialize();

	BiSetClickableBuffer(BiClick,BICLICKBUFSIZE);

	HtInitialize();

	HtStartSetMenu(MenuString);
	HtSetMenuItem(1,"Quit",'Q',HtQuitProgram);
	HtEndSetMenu();

	HtStartSetShortCut();
	HtSetShortCut(BIKEY_Q,HtQuitProgram);
	HtEndSetShortCut();

	BiSetWindowName("ApplicationWindow");

	HtOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	/* Call Application Initializer Here */

	HtEventLoop();

	return 0;
}
