#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void DrawCheckFloorMain(void);
void DrawCheckFloor(void);

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

	BiSetWindowName("check");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	DrawCheckFloorMain();

	BiCloseWindow();
	return 0;
}

void DrawCheckFloorMain(void)
{
	BIPROJ prj;
	BIPNTANG eye;

	BiSetPoint(&eye.p,0,10,0);
	BiSetAngle(&eye.a,0,0,0);

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();

		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();
		BiStartBuffer(&eye);
		DrawCheckFloor();
		BiFlushBuffer();
		BiSwapBuffers();

		eye.a.h+=256;
	}
}

void DrawCheckFloor(void)
{
	int c,x,z;
	BIPOINT plg[4];
	BICOLOR col[2];

	BiSetColorRGB(&col[0],255,0,0);
	BiSetColorRGB(&col[1],0,0,255);

	for(x=-10; x<=10; x++)
	{
		for(z=-10; z<=10; z++)
		{
			BiSetPoint(&plg[0],x*10   ,0,z*10   );
			BiSetPoint(&plg[1],x*10+10,0,z*10   );
			BiSetPoint(&plg[2],x*10+10,0,z*10+10);
			BiSetPoint(&plg[3],x*10   ,0,z*10+10);
			c=BiAbs(x-z)%2;
			BiOvwPolyg(4,plg,&col[c]);
		}
	}
}
