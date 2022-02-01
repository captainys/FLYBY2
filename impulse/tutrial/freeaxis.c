#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void FreeAxisMain(void);
void FreeAxisRotation(BIANGLE *obj);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("freeaxis");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	FreeAxisMain();

	BiCloseWindow();
	return 0;
}

void FreeAxisMain(void)
{
	BIPROJ prj;
	BIPNTANG eye,obj;
	BISRFMODEL srf;
	int key;

	BiSetShadMode(BI_ON);

	if(BiLoadSrf(&srf,"t4blue.srf")==BI_OK)
	{
		BiSetPoint(&eye.p,0,0,0);
		BiSetAngle(&eye.a,0,0,0);

		BiSetPoint(&obj.p,0,0,20);
		BiSetAngle(&obj.a,32768,0,0);

		while((key=BiInkey())==BIKEY_NULL && BiFatal()==BI_FALSE)
		{
			BiUpdateDevice();

			FreeAxisRotation(&obj.a);

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

void FreeAxisRotation(BIANGLE *obj)
{
	int lbt,rbt,mbt;
	long mx,my,wx,wy;
	BIPOINT ev,uv;
	BIANGLE rot;

	BiMouse(&lbt,&mbt,&rbt,&mx,&my);
	if(lbt==BI_ON)
	{
		BiGetWindowSize(&wx,&wy);

		BiSetAngle(&rot,(mx-wx/2)*10,(my-wy/2)*10,0);

		BiAngleToVector(&ev,&uv,obj);
		BiRotLtoG(&ev,&ev,&rot);
		BiRotLtoG(&uv,&uv,&rot);
		BiVectorToAngle(obj,&ev,&uv);
	}
}
