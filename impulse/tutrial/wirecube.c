#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void DrawCubeLoop(void);
void DrawCube(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("wirecube");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	DrawCubeLoop();

	BiCloseWindow();
	return 0;
}

void DrawCubeLoop(void)
{
	BIPNTANG eye;

	BiSetPoint(&eye.p,0,0,-20);
	BiSetAngle(&eye.a,0,0,0);

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();
		BiClearScreen();
		BiStartBuffer(&eye);
		DrawCube();
		BiFlushBuffer();
		BiSwapBuffers();
	}
}

static BIPOINT vtx[]=
{
	{ 5, 5, 5},{-5, 5, 5},{-5,-5, 5},{ 5,-5, 5},
	{ 5, 5,-5},{-5, 5,-5},{-5,-5,-5},{ 5,-5,-5}
};

void DrawCube(void)
{
	BICOLOR col;
	BiSetColorRGB(&col,255,255,255);

	BiInsLine(&vtx[0],&vtx[1],&col);
	BiInsLine(&vtx[1],&vtx[2],&col);
	BiInsLine(&vtx[2],&vtx[3],&col);
	BiInsLine(&vtx[3],&vtx[0],&col);

	BiInsLine(&vtx[4],&vtx[5],&col);
	BiInsLine(&vtx[5],&vtx[6],&col);
	BiInsLine(&vtx[6],&vtx[7],&col);
	BiInsLine(&vtx[7],&vtx[4],&col);

	BiInsLine(&vtx[0],&vtx[4],&col);
	BiInsLine(&vtx[1],&vtx[5],&col);
	BiInsLine(&vtx[2],&vtx[6],&col);
	BiInsLine(&vtx[3],&vtx[7],&col);
}
