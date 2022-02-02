/* test02.c  Testing Dynamic Model Animation */
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


#define DIST 60
static void MoveEyePosition(BIPNTANG *eye);
static void SetEyePosition(BIPNTANG *eye);


void MakeChainModel(ARDYNAMDL *mdl);
void Motion(ARDYNAMDL *mdl);
void ExecMotion(ARDYNAMDL *mdl,int id);
int Launch(ARDYNAMDL *mdl,int lau);

void Test01Main(void)
{
	BIPROJ prj;
	ARDYNAMDL mdl;
	BIPNTANG pos,eye;
	int key,launch;

	MakeChainModel(&mdl);

	eye=BiOrgPA;
	pos=BiOrgPA;

	BiSetAngleDeg(&pos.a,180,0,0);

	launch=0;
	while((key=BiInkey())!=BIKEY_Q)
	{
		BiUpdateDevice();

		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();
		BiStartBuffer(&eye);

		MoveEyePosition(&eye);
		SetEyePosition(&eye);

		if(key==BIKEY_SPACE)
		{
			launch=BI_ON;
		}

		launch=Launch(&mdl,launch);
		Motion(&mdl);

		ArInsDynaMdl(&mdl,&pos);
		BiFlushBuffer();

		BiSwapBuffers();
	}
}


BISRFMODEL bdy,hed,arm;

void MakeChainModel(ARDYNAMDL *mdl)
{
	BIPNTANG pos;
	BIPOINT ofs;

	BiLoadSrf(&bdy,"body.srf");
	BiLoadSrf(&hed,"head.srf");
	BiLoadSrf(&arm,"arm.srf");

	ArInitDynaMdl(mdl);
	ArAddDynaMdlSrf(NULL,mdl,&bdy,0,&BiOrgPA,&BiOrgP);  /* ID 0 */

	BiSetPoint(&pos.p,0,5,0);
	BiSetAngleDeg(&pos.a,0,90,0);
	BiSetPoint(&ofs,0,0,-4);
	ArAddDynaMdlSrf(NULL,mdl,&hed,0,&pos,&ofs);  /* ID 1 */

	BiSetPoint(&ofs,0,0,-5);

	BiSetPoint(&pos.p,-3,3,0);
	BiSetAngleDeg(&pos.a,90,-80,-90);
	ArAddDynaMdlSrf(NULL,mdl,&arm,0,&pos,&ofs);  /* ID 2 */
	BiSetPoint(&pos.p,0,0,5);
	BiSetAngleDeg(&pos.a,0,0,0);
	ArAddDynaMdlSrf(NULL,mdl,&arm,2,&pos,&ofs);  /* ID 3 */

	BiSetPoint(&pos.p, 3,3,0);
	BiSetAngleDeg(&pos.a,-90,-80,90);
	ArAddDynaMdlSrf(NULL,mdl,&arm,0,&pos,&ofs);  /* ID 4 */
	BiSetPoint(&pos.p,0,0,5);
	BiSetAngleDeg(&pos.a,0,0,0);
	ArAddDynaMdlSrf(NULL,mdl,&arm,4,&pos,&ofs);  /* ID 5 */

	BiSetPoint(&pos.p, -2,-6,0);
	BiSetAngleDeg(&pos.a,90,-80,-90);
	ArAddDynaMdlSrf(NULL,mdl,&arm,0,&pos,&ofs);  /* ID 6 */
	BiSetPoint(&pos.p,0,0,5);
	BiSetAngle(&pos.a,0,0,0);
	ArAddDynaMdlSrf(NULL,mdl,&arm,6,&pos,&ofs);  /* ID 7 */

	BiSetPoint(&pos.p, 2,-6,0);
	BiSetAngleDeg(&pos.a,-90,-80,+90);
	ArAddDynaMdlSrf(NULL,mdl,&arm,0,&pos,&ofs);  /* ID 8 */
	BiSetPoint(&pos.p,0,0,5);
	BiSetAngle(&pos.a,0,0,0);
	ArAddDynaMdlSrf(NULL,mdl,&arm,8,&pos,&ofs);  /* ID 9 */
}

typedef struct {
	int bikey,id;
} KEYTAB;

KEYTAB tab[]={
	{BIKEY_ONE,  1},
	{BIKEY_TWO,  2},
	{BIKEY_THREE,3},
	{BIKEY_FOUR, 4},
	{BIKEY_FIVE, 5},
	{BIKEY_SIX,  6},
	{BIKEY_SEVEN,7},
	{BIKEY_EIGHT,8},
	{BIKEY_NINE, 9}
};

void Motion(ARDYNAMDL *mdl)
{
	int i;
	for(i=0; i<sizeof(tab)/sizeof(KEYTAB); i++)
	{
		if(BiKey(tab[i].bikey)==BI_ON)
		{
			ExecMotion(mdl,tab[i].id);
		}
	}
}

void ExecMotion(ARDYNAMDL *mdl,int id)
{
	BIPNTANG pos;
	long mx,my;
	int lbt,rbt,mbt;
	long vp,vh,cenX,cenY;

	BiGetWindowSize(&cenX,&cenY);
	cenX/=2;
	cenY/=2;

	BiMouse(&lbt,&mbt,&rbt,&mx,&my);

	ArGetDynaMdlRelativePosition(&pos,mdl,id);
	vp=(my-cenY)*512/cenY;
	vh=(cenX-mx)*1024/cenX;
	BiPitchUp(&pos.a,&pos.a,vp,vh);
	ArSetDynaMdlRelativePosition(mdl,id,&pos);
}

int Launch(ARDYNAMDL *mdl,int lau)
{
	BIPNTANG pos;
	if(lau==BI_ON)
	{
		ArGetDynaMdlRelativePosition(&pos,mdl,1);
		pos.p.z+=2;
		if(pos.p.z>30)
		{
			pos.p.z=0.0F;
			lau=BI_OFF;
		}
		ArSetDynaMdlRelativePosition(mdl,1,&pos);
		return lau;
	}
	return BI_OFF;
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

static void SetEyePosition(BIPNTANG *eye)
{
	BIPOINT vec;
	BiSetPoint(&vec,0,0,DIST);
	BiRotLtoG(&vec,&vec,&eye->a);
	BiSubPoint(&eye->p,&BiOrgP,&vec);
}
