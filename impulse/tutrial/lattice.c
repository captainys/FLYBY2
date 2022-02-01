#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void DrawLattice(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("lattice");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();
		DrawLattice();
	}

	BiCloseWindow();
	return 0;
}

void DrawLattice(void)
{
	long x,y,wx,wy;
	BIPOINTS p1,p2;
	BICOLOR col;

	BiClearScreen();

	BiSetColorRGB(&col,0,0,255);

	BiGetWindowSize(&wx,&wy);
	for(y=0; y<wy; y+=20)
	{
		BiSetPointS(&p1,0 ,y);
		BiSetPointS(&p2,wx,y);
		BiDrawLine2(&p1,&p2,&col);
	}
	for(x=0; x<wx; x+=20)
	{
		BiSetPointS(&p1,x,0 );
		BiSetPointS(&p2,x,wy);
		BiDrawLine2(&p1,&p2,&col);
	}

	BiSwapBuffers();
}
