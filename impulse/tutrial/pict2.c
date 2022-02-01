#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void DrawPict2Main(void);
void DrawPict2(BIPICT2 *pc2);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("pict2");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	DrawPict2Main();

	BiCloseWindow();
	return 0;
}

void DrawPict2Main(void)
{
	BIPICT2 pc2;

	if(BiLoadPict2(&pc2,"title.pc2")==BI_OK)
	{
		while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
		{
			BiUpdateDevice();

			BiClearScreen();
			DrawPict2(&pc2);
			BiSwapBuffers();
		}
	}
}

void DrawPict2(BIPICT2 *pc2)
{
	BIPOINTS s1,s2;

	BiSetPointS(&s1,0,0);
	BiGetWindowSize(&s2.x,&s2.y);
	BiDrawPict2Zoom(pc2,&s1,&s2);
}
