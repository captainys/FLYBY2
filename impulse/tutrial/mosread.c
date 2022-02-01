#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void MouseMain(void);
void DrawCursor(int lbt,long x,long y);

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

	BiSetWindowName("mosread");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	MouseMain();

	BiCloseWindow();
	return 0;
}

void MouseMain(void)
{
	int lbt,rbt,mbt;
	long mx,my;

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();

		BiMouse(&lbt,&mbt,&rbt,&mx,&my);

		BiClearScreen();
		DrawCursor(lbt,mx,my);
		BiSwapBuffers();
	}
}

void DrawCursor(int lbt,long x,long y)
{
	BIPOINTS s;
	BICOLOR col;

	BiSetColorRGB(&col,255,255,255);
	BiSetPointS(&s,x,y);
	switch(lbt)
	{
	case BI_ON:
		BiDrawMarker2(&s,BIMK_RECT,&col);
		break;
	case BI_OFF:
		BiDrawMarker2(&s,BIMK_CROSS,&col);
		break;
	}
}
