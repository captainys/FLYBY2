/* test01.c  Testing Dynamic Model Animation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <impulse.h>
#include <aurora.h>



#define BIWORKSIZE 0xf000
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];



void Test01Main(void);

BIAPPENTRY
{
	BIAPPINIT;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();
	BiSetShadMode(BI_ON);

	BiOpenWindow(640,480);

	Test01Main();

	BiCloseWindow();

	return 0;
}


void MakeChainModel(ARDYNAMDL *mdl);
void Motion(ARDYNAMDL *mdl);

void Test01Main(void)
{
	BIPROJ prj;
	ARDYNAMDL mdl;
	BIPNTANG pos;

	MakeChainModel(&mdl);

	BiSetPoint(&pos.p,0,0,14);
	pos.a=BiOrgA;

	while(BiInkey()!=BIKEY_Q)
	{
		BiUpdateDevice();

		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();
		BiStartBuffer(&BiOrgPA);

		pos.a.h+=256;

		Motion(&mdl);

		ArInsDynaMdl(&mdl,&pos);
		BiFlushBuffer();

		BiSwapBuffers();
	}
}


BIPOLYGON bdy,arm,hed;
BIPOINT bdyVtx[3],armVtx[3],hedVtx[3];

void MakeChainModel(ARDYNAMDL *mdl)
{
	BIPNTANG pos;

	bdy.np=3;
	bdy.p=bdyVtx;
	BiSetPoint(&bdyVtx[0],-3, 0,0);
	BiSetPoint(&bdyVtx[1], 3, 0,0);
	BiSetPoint(&bdyVtx[2], 0,-8,0);

	hed.np=3;
	hed.p=hedVtx;
	BiSetPoint(&hedVtx[0],-1.5,2.4,0);
	BiSetPoint(&hedVtx[1], 1.5,2.4,0);
	BiSetPoint(&hedVtx[2],   0,  0,0);

	arm.np=3;
	arm.p=armVtx;
	BiSetPoint(&armVtx[0],   0,0,0);
	BiSetPoint(&armVtx[1],-0.5,3,0);
	BiSetPoint(&armVtx[2], 0.5,3,0);

	ArInitDynaMdl(mdl);
	ArAddDynaMdlPolyg(NULL,mdl,&bdy,&BiWhite,0,&BiOrgPA,&BiOrgP);
	ArAddDynaMdlPolyg(NULL,mdl,&hed,&BiWhite,0,&BiOrgPA,&BiOrgP);
	BiSetPoint(&pos.p,-3,0,0);
	BiSetAngle(&pos.a,0,0,0);
	ArAddDynaMdlPolyg(NULL,mdl,&arm,&BiWhite,0,&pos,&BiOrgP);
	BiSetPoint(&pos.p, 3,0,0);
	BiSetAngle(&pos.a,0,0,0);
	ArAddDynaMdlPolyg(NULL,mdl,&arm,&BiWhite,0,&pos,&BiOrgP);

	BiSetPoint(&pos.p,0,3,0);
	BiSetAngle(&pos.a,0,0,0);
	ArAddDynaMdlPolyg(NULL,mdl,&arm,&BiWhite,2,&pos,&BiOrgP);
	BiSetPoint(&pos.p,0,3,0);
	BiSetAngle(&pos.a,0,0,0);
	ArAddDynaMdlPolyg(NULL,mdl,&arm,&BiWhite,3,&pos,&BiOrgP);
}

void Motion(ARDYNAMDL *mdl)
{
	int i;
	BIPNTANG pos;
	for(i=0; i<6; i++)
	{
		ArGetDynaMdlRelativePosition(&pos,mdl,i);
		pos.a.h+=256;
		pos.a.p+=256;
		pos.a.b+=256;
		ArSetDynaMdlRelativePosition(mdl,i,&pos);
	}
}
