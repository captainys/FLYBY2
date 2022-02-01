#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

void DrawObject(void);

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int ac,char *av[])
{
	BIPROJ prj;
	BIPOSATT eye;


	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("ApplicationWindow");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);


	BiSetPoint(&eye.p,0,0,-20);
	eye.a=BiOrgA;

	BiClearScreen();
	BiVrmlOpen("test.wrl");
	BiStartBuffer(&eye);
	DrawObject();
	BiFlushBuffer();
	BiVrmlClose();
	BiSwapBuffers();

	BiClearScreen();
	BiSrfOpen("test.srf");
	BiStartBuffer(&eye);
	DrawObject();
	BiFlushBuffer();
	BiSrfClose();
	BiSwapBuffers();


	BiCloseWindow();
	return 0;
}

BIPOINT src[8][3]=
{
	{{ 0.0F, 0.0F,-5.0F},{ 5.0F, 0.0F, 0.0F},{ 0.0F, 5.0F, 0.0F}},
	{{ 5.0F, 0.0F, 0.0F},{ 0.0F, 0.0F, 5.0F},{ 0.0F, 5.0F, 0.0F}},
	{{ 0.0F, 0.0F, 5.0F},{-5.0F, 0.0F, 0.0F},{ 0.0F, 5.0F, 0.0F}},
	{{-5.0F, 0.0F, 0.0F},{ 0.0F, 0.0F,-5.0F},{ 0.0F, 5.0F, 0.0F}},
	{{ 5.0F, 0.0F, 0.0F},{ 0.0F, 0.0F,-5.0F},{ 0.0F,-5.0F, 0.0F}},
	{{ 0.0F, 0.0F, 5.0F},{ 5.0F, 0.0F, 0.0F},{ 0.0F,-5.0F, 0.0F}},
	{{-5.0F, 0.0F, 0.0F},{ 0.0F, 0.0F, 5.0F},{ 0.0F,-5.0F, 0.0F}},
	{{ 0.0F, 0.0F,-5.0F},{-5.0F, 0.0F, 0.0F},{ 0.0F,-5.0F, 0.0F}}
};

void DrawObject(void)
{
	int i;
	for(i=0; i<8; i++)
	{
		BiInsPolyg(3,src[i],&BiBlue);
	}
}

