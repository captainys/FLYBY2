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
	BIPOSATT viw;
	BIVECTOR ev,uv,proc;
	BIPOINT light;
	BITER ter;
	BIPROJ prj;
	int ky;

	BiSetShadMode(BI_ON);

	BiSetPoint(&viw.p,20.0,0.0,20.0);
	BiSetAngle(&viw.a,0.0,0.0,0.0);

	if(BiLoadTerrMesh(&ter,"demo2.ter")==BI_OK)
	{
		while((ky=BiInkey())!=BIKEY_ESC && BiFatal()==BI_FALSE)
		{
			if(BiTerEyeVecUpVec
			    (&ev,&uv,viw.p.x,viw.p.z,viw.a.h,&ter,&BiOrgPA)==BI_IN)
			{
				BiTerHeight(&viw.p.y,viw.p.x,viw.p.z,&ter,&BiOrgPA);
				viw.p.y+=1.0;
				BiVectorToAngle(&viw.a,&ev,&uv);
			}
			else
			{
				viw.p.y=1.0;
				viw.a.p=0;
				viw.a.b=0;
			}

			if(BiKey(BIKEY_UP)==BI_ON)
			{
				BiSetPoint(&proc,0.0,0.0,0.25);
				BiRotLtoG(&proc,&proc,&viw.a);
				BiAddPoint(&viw.p,&viw.p,&proc);
			}
			if(BiKey(BIKEY_DOWN)==BI_ON)
			{
				BiSetPoint(&proc,0.0,0.0,0.25);
				BiRotLtoG(&proc,&proc,&viw.a);
				BiSubPoint(&viw.p,&viw.p,&proc);
			}
			if(BiKey(BIKEY_LEFT)==BI_ON)
			{
				viw.a.h+=1024;
			}
			if(BiKey(BIKEY_RIGHT)==BI_ON)
			{
				viw.a.h-=1024;
			}


			BiUpdateDevice();
			BiGetStdProjection(&prj);
			prj.nearz=0.2;
			BiSetProjection(&prj);

			light=viw.p;
			light.y+=2.0;
			BiSetLightPosition(&light);

			BiClearScreen();
			BiStartBuffer(&viw);
			BiInsTerrMesh(&ter,&BiOrgPA);
			BiFlushBuffer();
			BiSwapBuffers();
		}
	}
}
