#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0x20000L
char BiWork[BIWORKSIZE];
char BiCbuf[BIWORKSIZE];


void DrawBallLoop(void);
void DrawBall(BIPNTANG *pos);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("Clickable Polygon");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	/* Init Clickable Buffer *************************/
	BiSetClickableBuffer(BiCbuf,BIWORKSIZE);
	/*************************************************/

	DrawBallLoop();

	BiCloseWindow();
	return 0;
}

int hit,i1,i2;

void DrawBallLoop(void)
{
	BIPROJ prj;
	BIPNTANG eye,cub;
	long lb,mb,rb;
	BIPOINTS m;

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
		DrawBall(&cub);
		BiFlushBuffer();

		BiMouse(&lb,&mb,&rb,&m.x,&m.y);

		BiDrawMarker2(&m,BIMK_CROSS,&BiWhite);

		/* Detect Clickable Object ***********************/
		hit=BiCheckClicked(&i1,&i2,NULL,NULL,&m,1);
		/*************************************************/
		if(hit==BI_OK)
		{
			char str[256];
			BIPOINTS sc={32,32};
			sprintf(str,"HIT %d %d",i1,i2);
			BiDrawString2(&sc,str,&BiWhite);
		}

		cub.a.h+=16;
		cub.a.p-=16;

		BiSwapBuffers();
	}
}

void DrawBall(BIPNTANG *pos)
{
	int h,p,ctr;
	BIANGLE att;
	BIPOINT sq[4],nom,vecZ;
	BICOLOR *col;

	BiSetLocalMatrix(pos);

	BiSetPoint(&vecZ,0,0,5);

	for(p=-16384; p<16384; p+=2048)
	{
		for(h=0; h<65536; h+=2048)
		{
			BiSetAngle(&att,h     ,p     ,0);
			BiRotLtoG(&sq[0],&vecZ,&att);
			BiSetAngle(&att,h+2048,p     ,0);
			BiRotLtoG(&sq[1],&vecZ,&att);
			BiSetAngle(&att,h+2048,p+2048,0);
			BiRotLtoG(&sq[2],&vecZ,&att);
			BiSetAngle(&att,h     ,p+2048,0);
			BiRotLtoG(&sq[3],&vecZ,&att);

			BiSetAngle(&att,h+1024,p+1024,0);
			BiRotLtoG(&nom,&BiVecZ,&att);

			if(hit==BI_OK && i1==h && i2==p)
			{
				col=&BiRed;
			}
			else
			{
				ctr=(p/2048)+(h/2048);
				col=((ctr&1) ? &BiGreen : &BiCyan);
			}

			BiInsPolygFast(4,sq,NULL,&nom,col);

			/* Register Clickable Object *********************/
			BiInsClickablePolyg(4,sq,&nom,h,p,0,NULL);
			/*************************************************/
		}
	}

	BiClearLocalMatrix();
}
