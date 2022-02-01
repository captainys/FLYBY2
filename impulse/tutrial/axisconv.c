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
void DrawCube(BIPNTANG *pos);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("axisconv");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	DrawCubeLoop();

	BiCloseWindow();
	return 0;
}

void DrawCubeLoop(void)
{
	BIPROJ prj;
	BIPNTANG eye,cub;

	BiSetPoint(&eye.p,0,0,0);
	BiSetAngle(&eye.a,0,0,0);

	BiSetPoint(&cub.p,0,0,20);
	BiSetAngle(&cub.a,0,0,0);

	while(BiInkey()==BIKEY_NULL && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();

		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();
		BiStartBuffer(&eye);
		DrawCube(&cub);
		BiFlushBuffer();
		BiSwapBuffers();

		cub.a.h+=512;
		cub.a.p+=512;
	}
}

BIPOINT vtx[6][4]=
{
	{{ 5, 5, 5},{-5, 5, 5},{-5,-5, 5},{ 5,-5, 5}},
	{{ 5, 5,-5},{-5, 5,-5},{-5,-5,-5},{ 5,-5,-5}},
	{{ 5, 5, 5},{ 5,-5, 5},{ 5,-5,-5},{ 5, 5,-5}},
	{{-5, 5, 5},{-5,-5, 5},{-5,-5,-5},{-5, 5,-5}},
	{{ 5, 5, 5},{-5, 5, 5},{-5, 5,-5},{ 5, 5,-5}},
	{{ 5,-5, 5},{-5,-5, 5},{-5,-5,-5},{ 5,-5,-5}}
};

BICOLOR col[6]=
{
	{  0,  0,255},{  0,255,  0},{  0,255,255},{255,  0,  0},
	{255,  0,255},{255,255,  0}
};

void DrawCube(BIPNTANG *pos)
{
	int i,j;
	BIPOINT tmp[4];
	BIAXIS axs;

	BiPntAngToAxis(&axs,pos);
	for(i=0; i<6; i++)
	{
		for(j=0; j<4; j++)
		{
			BiConvLtoG(&tmp[j],&vtx[i][j],&axs);
		}
		BiInsPolyg(4,tmp,&col[i]);
	}
}
