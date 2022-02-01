#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0x20000L
char BiWork[BIWORKSIZE];
char BiCbuf[BIWORKSIZE];

void DrawCubeLoop(void);
void DrawCube(BIPNTANG *pos);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("Clickable");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	/* Init Clickable Buffer *************************/
	BiSetClickableBuffer(BiCbuf,BIWORKSIZE);
	/*************************************************/
	DrawCubeLoop();

	BiCloseWindow();
	return 0;
}

void DrawCubeLoop(void)
{
	BIPROJ prj;
	BIPNTANG eye,cub;
	long lb,mb,rb;
	BIPOINTS m;
	int hit,i1,i2,i3;

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

		/* Clear Clickable Buffer ************************/
		BiClearClickable();
		/*************************************************/

		BiStartBuffer(&eye);
		DrawCube(&cub);
		BiFlushBuffer();

		BiMouse(&lb,&mb,&rb,&m.x,&m.y);

		BiDrawMarker2(&m,BIMK_CROSS,&BiWhite);

		/* Detect Clickable Object ***********************/
		hit=BiCheckClicked(&i1,&i2,&i3,NULL,&m,1);
		/*************************************************/
		if(hit==BI_OK)
		{
			char str[256];
			BIPOINTS sc={32,32};
			sprintf(str,"HIT %d %d %d",i1,i2,i3);
			BiDrawString2(&sc,str,&BiWhite);
		}
		else
		{
			cub.a.h+=16;
		}

		BiSwapBuffers();
	}
}

BIPOINT marker[8]=
{
	{ 5.0F, 5.0F, 5.0F},
	{-5.0F, 5.0F, 5.0F},
	{-5.0F,-5.0F, 5.0F},
	{ 5.0F,-5.0F, 5.0F},
	{ 5.0F, 5.0F,-5.0F},
	{-5.0F, 5.0F,-5.0F},
	{-5.0F,-5.0F,-5.0F},
	{ 5.0F,-5.0F,-5.0F}
};

BICOLOR col[6]=
{
	{  0,  0,255},{  0,255,  0},{  0,255,255},{255,  0,  0},
	{255,  0,255},{255,255,  0}
};

void DrawCube(BIPNTANG *pos)
{
	int i;
	BiSetLocalMatrix(pos);

	BiInsLine(&marker[0],&marker[1],&BiWhite);
	BiInsLine(&marker[1],&marker[2],&BiWhite);
	BiInsLine(&marker[2],&marker[3],&BiWhite);
	BiInsLine(&marker[3],&marker[0],&BiWhite);
	BiInsLine(&marker[4],&marker[5],&BiWhite);
	BiInsLine(&marker[5],&marker[6],&BiWhite);
	BiInsLine(&marker[6],&marker[7],&BiWhite);
	BiInsLine(&marker[7],&marker[4],&BiWhite);
	BiInsLine(&marker[0],&marker[4],&BiWhite);
	BiInsLine(&marker[1],&marker[5],&BiWhite);
	BiInsLine(&marker[2],&marker[6],&BiWhite);
	BiInsLine(&marker[3],&marker[7],&BiWhite);

	for(i=0; i<8; i++)
	{
		char str[256];
		BiInsMarker(&marker[i],&BiRed,BIMK_RECT);
		sprintf(str,"%d",i);
		BiInsString(&marker[i],str,&BiRed,BIMG_CENTER,BIMG_CENTER);

		/* Register Clickable Object *********************/
		BiInsClickable(&marker[i],0.0F,5,i,i*10,i*100,NULL);
		/*************************************************/
	}

	BiClearLocalMatrix();
}
