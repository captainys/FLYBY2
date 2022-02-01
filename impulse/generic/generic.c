#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>
#include <aurora.h>
#include <impulse2g.h>


#define BIWORKSIZE 0x60000L
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
	BiRenderInitialize();

	BiSetClickableBuffer(BiClick,BICLICKBUFSIZE);

	BiSetWindowName("APPLICATION");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BIPOINTS s;

		BiUpdateDevice();

		BiClearScreen();
		BiClearClickable();

		BiSetPointS(&s,24,24);
		BiDrawString2(&s,"Example of Minimum Application",&BiWhite);

		BiFlushBuffer();
		BiSwapBuffers();
	}

	BiCloseWindow();
	return 0;
}
