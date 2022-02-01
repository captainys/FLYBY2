#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void DrawTerrainMain(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("terrain");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	DrawTerrainMain();

	BiCloseWindow();
	return 0;
}

void DrawTerrainMain(void)
{
	BIPNTANG eye,obj;
	BITERRMESH ter;

	BiSetShadMode(BI_ON);

	if(BiLoadTerrMesh(&ter,"demo.ter")==BI_OK)
	{
		BiSetPoint(&eye.p,300,600,-600);
		BiSetAngleDeg(&eye.a,0,-30,0);

		BiSetPoint(&obj.p,0,0,0);
		BiSetAngle(&obj.a,0,0,0);

		while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
		{
			BiUpdateDevice();

			BiClearScreen();
			BiStartBuffer(&eye);
			BiInsTerrMesh(&ter,&obj);
			BiFlushBuffer();
			BiSwapBuffers();
		}
	}
}
