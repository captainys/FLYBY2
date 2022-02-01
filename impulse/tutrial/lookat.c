#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void LookAtMain(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("lookat");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	LookAtMain();

	BiCloseWindow();
	return 0;
}

void LookAtMain(void)
{
	BIPROJ prj;
	BIPOINT vec;
	BIPNTANG eye,obj;
	BISRFMODEL srf;
	BIPOINTS s1,s2;
	BICOLOR bkg;

	BiSetShadMode(BI_ON);

	BiSetColorRGB(&bkg,0,255,255);

	if(BiLoadSrf(&srf,"t2blue.srf")==BI_OK)
	{
		BiSetPoint(&eye.p,0,0,0);
		BiSetAngle(&eye.a,0,0,0);

		BiSetPoint(&obj.p,-100,15,15);
		BiSetAngleDeg(&obj.a,-90,0,0);
		while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
		{
			BiUpdateDevice();

			BiSetPointS(&s1,0,0);
			BiGetWindowSize(&s2.x,&s2.y);

			BiGetStdProjection(&prj);
			BiSetProjection(&prj);

			BiSubPoint(&vec,&obj.p,&eye.p);
			BiVectorToHeadPitch(&eye.a,&vec);

			BiClearScreen();
			BiDrawRect2(&s1,&s2,&bkg);
			BiStartBuffer(&eye);
			BiInsSrfModel(&srf,&obj);
			BiFlushBuffer();
			BiSwapBuffers();

			obj.p.x+=5;
			if(obj.p.x>=100)
			{
				obj.p.x=-100.0F;
			}
		}
	}
}
