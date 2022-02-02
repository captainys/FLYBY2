#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>

void InitializeDraw(void);
void ButtonCallBack(int trig,HTMOUSE *mos);
void DragCallBack(HTMOUSE *mos);
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
	HtEndSetMenu();

	HtStartSetShortCut();
	HtSetShortCut(BIKEY_Q,HtQuitProgram);
	HtEndSetShortCut();

	BiSetWindowName("drag");

	HtOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	HtSetOnLeftButtonFunc(ButtonCallBack);
	HtSetOnDragFunc(DragCallBack);

	InitializeDraw();

	HtSetRepaintFunc(RefreshScreen);

	HtEventLoop();

	return 0;
}

#define MAX_N_VTX 256
static int nVtx;
static BIPOINTS vtx[MAX_N_VTX];
static BICOLOR col;

void InitializeDraw(void)
{
	nVtx=0;
}

void ButtonCallBack(int trig,HTMOUSE *mos)
{
	switch(trig)
	{
	case HT_ON:
		BiSetColorRGB(&col,0,255,0);
		nVtx=0;
		RefreshScreen();
		break;
	case HT_OFF:
		BiSetColorRGB(&col,255,255,255);
		RefreshScreen();
		break;
	}
}

void DragCallBack(HTMOUSE *mos)
{
	if(nVtx<MAX_N_VTX)
	{
		BiSetPointS(&vtx[nVtx],mos->mx,mos->my);
		nVtx++;
		RefreshScreen();
	}
}

void RefreshScreen(void)
{
	BiClearScreen();
	BiDrawPolyline2(nVtx,vtx,&col);
	BiSwapBuffers();
}
