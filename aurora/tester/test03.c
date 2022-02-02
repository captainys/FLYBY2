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

	ArInitialize();

	Test01Main();

	BiCloseWindow();

	return 0;
}


static void MoveEyePosition(BIPNTANG *eye);
static void SetEyePosition(BIPNTANG *eye,real dist);
static void Motion(ARDYNAMDL *mdl,int stat);

void Test01Main(void)
{
	real dist;
	BIPROJ prj;
	ARDYNAMDL mdl;
	BIPNTANG pos,eye;
	int key,gear,stat;
	BIPOINTS s1,s2;
	long wid,hei;
	BICOLOR col;

	ArLoadDnm(&mdl,"t2blue.dnm");

	pos=BiOrgPA;
	eye=BiOrgPA;

	BiSetAngleDeg(&pos.a,180,0,0);

	BiSetColorRGB(&col,32,32,32);

	dist=30.0F;
	gear=256;
	stat=0;
	while((key=BiInkey())!=BIKEY_Q)
	{
		BiUpdateDevice();

		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();

		BiGetWindowSize(&wid,&hei);
		BiSetPointS(&s1,0,0);
		BiSetPointS(&s2,wid,hei);
		BiDrawRect2(&s1,&s2,&col);

		BiStartBuffer(&eye);

		MoveEyePosition(&eye);
		SetEyePosition(&eye,dist);

		switch(key)
		{
		case BIKEY_R:
			gear=0;
			break;
		case BIKEY_E:
			gear=256;
			break;
		case BIKEY_Z:
			dist-=2;
			break;
		case BIKEY_M:
			dist+=2;
			break;
		}

		if(gear<stat)
		{
			stat-=16;
		}
		else if(gear>stat)
		{
			stat+=16;
		}
		Motion(&mdl,stat);

		ArInsDynaMdlDivide(&mdl,&pos);
		BiFlushBuffer();

		BiSwapBuffers();
	}
}


void Motion(ARDYNAMDL *mdl,int stat)
{
	real t;

	t=(real)stat/256.0F;
	ArInBetweenDynaMdlStatus(mdl,0,0,1,t);
	ArInBetweenDynaMdlStatus(mdl,1,0,1,t);
	ArInBetweenDynaMdlStatus(mdl,2,0,1,t);
	ArInBetweenDynaMdlStatus(mdl,3,0,1,t);
}



static void MoveEyePosition(BIPNTANG *eye)
{
	int lbt,mbt,rbt;
	long mx,my,cx,cy,yaw,pit;

	BiMouse(&lbt,&mbt,&rbt,&mx,&my);
	if(lbt==BI_ON)
	{
		BiGetWindowSize(&cx,&cy);
		cx/=2;
		cy/=2;

		yaw=(mx-cx)*6;
		pit=(my-cy)*6;

		BiPitchUp(&eye->a,&eye->a, pit,yaw);
	}
}

static void SetEyePosition(BIPNTANG *eye,real dist)
{
	BIPOINT vec;
	BiSetPoint(&vec,0,0,dist);
	BiRotLtoG(&vec,&vec,&eye->a);
	BiSubPoint(&eye->p,&BiOrgP,&vec);
}
