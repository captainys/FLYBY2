#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>

/* include application header here. */

#define BIWORKSIZE 0x60000
char BiWork[BIWORKSIZE];

#define BICLICKBUFSIZE 0x60000
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

	BiSetWindowName("Application");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	/* Write Application Code Here */

	HtMessageBox("See You !","Zzzz...");

	BiCloseWindow();
	return 0;
}
