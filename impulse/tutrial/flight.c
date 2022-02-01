#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

BIPICT2 map;
BITERRMESH ter;

void FlightMain(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("flight");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	if(BiLoadPict2(&map,"demo.pc2")==BI_OK &&
	   BiLoadTerrMesh(&ter,"demo.ter")==BI_OK)
	{
		FlightMain();
	}

	BiCloseWindow();
	return 0;
}

void FlightMain(void)
{
	long vh,vp,mx,my,wx,wy;
	int lbt,rbt,mbt;
	BIPNTANG eye,obj;
	BICOLOR grd,sky;
	BIPOINT vec;
	real elv;

	BiSetPoint(&eye.p,0,300,-1000);
	BiSetAngle(&eye.a,0,0,0);

	BiSetPoint(&obj.p,0,0,0);
	BiSetAngle(&obj.a,0,0,0);

	BiSetColorRGB(&grd,0,128,0);
	BiSetColorRGB(&sky,32,180,180);

	BiSetShadMode(BI_ON);

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();

		BiMouse(&lbt,&mbt,&rbt,&mx,&my);

		BiGetWindowSize(&wx,&wy);
		vh=-(mx-wx/2)*3;
		vp= (my-wy/2)*3;

		eye.a.h+=vh;
		eye.a.p+=vp;
		if(eye.a.p>8192)
		{
			eye.a.p=8192;
		}
		if(eye.a.p<-8192)
		{
			eye.a.p=-8192;
		}

		BiSetPoint(&vec,0,0,10);
		BiRotLtoG(&vec,&vec,&eye.a);
		BiAddPoint(&eye.p,&eye.p,&vec);

		if(BiTerrMeshHeight(&elv,eye.p.x,eye.p.z,&ter,&obj)==BI_IN)
		{
			if(eye.p.y < elv+10)
			{
				eye.p.y=elv+10;
			}
		}
		else
		{
			if(eye.p.y < 10)
			{
				eye.p.y=10.0F;
			}
		}

		BiClearScreen();
		BiStartBuffer(&eye);
		BiDrawGroundSky(&grd,&sky);
		obj.a.p=-16384;
		BiOvwPict2(&map,&obj);
		obj.a.p=0;
		BiInsTerrMesh(&ter,&obj);
		BiFlushBuffer();
		BiSwapBuffers();
	}
}
