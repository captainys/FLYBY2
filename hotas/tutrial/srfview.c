#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>

void InitData(void);
void LoadSrf(void);
void ButtonFunc(int trig,HTMOUSE *mos);
void DragFunc(HTMOUSE *mos);
void RotateSrf(long mx,long my);
void RefreshScreen(void);

char *MenuString[]={"File",NULL};

#define BIWORKSIZE 0xf000
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();
	BiSetShadMode(BI_ON);

	HtInitialize();

	HtStartSetMenu(MenuString);
	HtSetMenuItem(1,"Quit",'Q',HtQuitProgram);
	HtSetMenuItem(1,"Load",'L',LoadSrf);
	HtEndSetMenu();

	HtStartSetShortCut();
	HtSetShortCut(BIKEY_Q,HtQuitProgram);
	HtEndSetShortCut();

	BiSetWindowName("srfview");

	HtOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	HtSetOnLeftButtonFunc(ButtonFunc);
	HtSetOnDragFunc(DragFunc);

	InitData();

	HtSetRepaintFunc(RefreshScreen);

	HtEventLoop();

	return 0;
}

int srfAlive;
BISRFMODEL srf;
BIPNTANG pos;
long prvMx,prvMy;

void InitData(void)
{
	srfAlive=HT_OFF;
	BiSetPoint(&pos.p,0,0,0);
	BiSetAngleDeg(&pos.a,180,0,0);
}

void LoadSrf(void)
{
	char fn[128];
	BISRFMODEL tmp;
	if(HtLoadFileDialog(fn,"Load File Name",NULL,"*.srf")==HT_YES)
	{
		if(BiLoadSrf(&tmp,fn)==BI_OK)
		{
			if(srfAlive==HT_ON)
			{
				BiFreeSrfModel(&srf);
			}
			srf=tmp;
			srfAlive=HT_ON;
		}
		else
		{
			HtMessageBox("Failed To Load File.",NULL);
			srfAlive=HT_OFF;
		}
		RefreshScreen();
	}
}

void ButtonFunc(int trig,HTMOUSE *mos)
{
	if(trig==HT_ON)
	{
		prvMx=mos->mx;
		prvMy=mos->my;
	}
}

void DragFunc(HTMOUSE *mos)
{
	long dx,dy;
	dx=mos->mx-prvMx;
	dy=mos->my-prvMy;
	RotateSrf(dx,dy);
	RefreshScreen();
	prvMx=mos->mx;
	prvMy=mos->my;
}

void RotateSrf(long dx,long dy)
{
	BIPOINT ev,uv;
	BIANGLE rot;

	BiSetAngle(&rot,dx*100,dy*100,0);

	BiAngleToVector(&ev,&uv,&pos.a);
	BiRotLtoG(&ev,&ev,&rot);
	BiRotLtoG(&uv,&uv,&rot);
	BiVectorToAngle(&pos.a,&ev,&uv);
}

void RefreshScreen(void)
{
	real dist;
	BIPNTANG eye;
	BIPROJ prj;

	BiSetPoint(&eye.p,0,0,0);
	BiSetAngle(&eye.a,0,0,0);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	BiClearScreen();
	if(srfAlive==HT_ON)
	{
		BiGetSrfRadius(&dist,&srf);
		BiSetPoint(&pos.p,0,0,dist*2);
		BiStartBuffer(&eye);
		BiInsSrfModel(&srf,&pos);
		BiFlushBuffer();
	}
	BiSwapBuffers();
}

