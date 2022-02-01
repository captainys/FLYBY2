#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void PitchYawMain(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("pitch");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	PitchYawMain();

	BiCloseWindow();
	return 0;
}

void PitchYawMain(void)
{
	BIPROJ prj;
	BIPOINT vec;
	BIPNTANG eye,obj;
	BISRFMODEL srf;
	int key;

	BiSetShadMode(BI_ON);

	if(BiLoadSrf(&srf,"t4blue.srf")==BI_OK)
	{
		BiSetPoint(&eye.p,0,0,0);
		BiSetAngle(&eye.a,0,0,0);

		BiSetPoint(&obj.p,0,0,20);
		BiSetAngleDeg(&obj.a,130,-30,30);

		while((key=BiInkey())!=BIKEY_X && BiFatal()==BI_FALSE)
		{
			BiUpdateDevice();

			switch(key)
			{
			case BIKEY_LEFT:
				BiPitchUp(&obj.a,&obj.a,0,512);
				break;
			case BIKEY_RIGHT:
				BiPitchUp(&obj.a,&obj.a,0,-512);
				break;
			case BIKEY_UP:
				BiPitchUp(&obj.a,&obj.a,-512,0);
				break;
			case BIKEY_DOWN:
				BiPitchUp(&obj.a,&obj.a,512,0);
				break;
			case BIKEY_F:
				BiSetPoint(&vec,0,0,3);
				BiRotLtoG(&vec,&vec,&obj.a);
				BiAddPoint(&obj.p,&obj.p,&vec);
				break;
			case BIKEY_B:
				BiSetPoint(&vec,0,0,-3);
				BiRotLtoG(&vec,&vec,&obj.a);
				BiAddPoint(&obj.p,&obj.p,&vec);
				break;
			}

			BiGetStdProjection(&prj);
			BiSetProjection(&prj);

			BiClearScreen();
			BiStartBuffer(&eye);
			BiInsSrfModel(&srf,&obj);
			BiFlushBuffer();
			BiSwapBuffers();
		}
	}
}
