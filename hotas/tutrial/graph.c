#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>

void DrawLattice(void);
void ClearGraphic(void);

char *MenuString[]={"File","Draw",NULL};

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
	HtSetMenuItem(2,"Lattice",0,DrawLattice);
	HtEndSetMenu();

	HtStartSetShortCut();
	HtSetShortCut(BIKEY_C,ClearGraphic);
	HtEndSetShortCut();

	BiSetWindowName("graph");

	HtOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	HtEventLoop();

	return 0;
}

void DrawLattice(void)
{
	long x,y,wx,wy;
	BIPOINTS s1,s2;
	BICOLOR col;
	BiGetWindowSize(&wx,&wy);
	BiClearScreen();
	BiSetColorRGB(&col,0,0,255);
	for(x=0; x<wx; x+=50)
	{
		BiSetPointS(&s1,x,0);
		BiSetPointS(&s2,x,wy);
		BiDrawLine2(&s1,&s2,&col);
	}
	for(y=0; y<wy; y+=50)
	{
		BiSetPointS(&s1,0,y);
		BiSetPointS(&s2,wx,y);
		BiDrawLine2(&s1,&s2,&col);
	}
	BiSwapBuffers();
}

void ClearGraphic(void)
{
	BiClearScreen();
	BiSwapBuffers();
}
