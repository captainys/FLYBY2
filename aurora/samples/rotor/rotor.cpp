#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>
#include <aurora.h>


ARDNM dnm;


////////////////////////////////////////////////////////////

char caption[256]={0};
BICOLOR background={64,0,128};

// Viewing Control

static BIATTITUDE viewAngle={24634,-5234,-2818};
static BIPOINT viewTarget={0.0F,0.0F,0.0F};
static real viewDistance=20.0F;

void GetOrbitViewer(BIPOSATT *pos)
{
	BIPOSATT vp2G;
	BIVECTOR vec;

	BiSetPoint(&vec,0,0,viewDistance);
	BiRotLtoG(&vec,&vec,&viewAngle);

	BiSubPoint(&vp2G.p,&viewTarget,&vec);
	vp2G.a=viewAngle;

	*pos=vp2G;
}

void OrbitViewer(void)
{
	long wid,hei,cx,cy;
	int l,m,r;
	long x,y;

	BiMouse(&l,&m,&r,&x,&y);
	BiGetWindowSize(&wid,&hei);

	cx=wid/2;
	cy=hei/2;

	x-=cx;
	y-=cy;

	if(r==BI_ON)
	{
		BiPitchUp(&viewAngle,&viewAngle,y*6,x*6);
	}
	if(l==BI_ON)
	{
		real m,t;
		m=1.0F+(real)y/5000.0F;
		t=viewDistance*m;
		if(1.0F<=t && t<1000.0F)
		{
			viewDistance=t;
		}
	}
}

////////////////////////////////////////////////////////////

void Visualize(void)
{
	BIPOSATT eye;
	int ky;

	BIATTITUDE att={0,0,0};

	ky=BIKEY_NULL;
	while(ky!=BIKEY_ESC)
	{
		BiUpdateDevice();
		BIPROJ prj;
		long wid,hei;

		BiGetWindowSize(&wid,&hei);

		BiGetStdProjection(&prj);
		prj.nearz=0.1;
		BiSetProjection(&prj);

		BiSetOrthoDist(viewDistance);

		OrbitViewer();

		ky=BiInkey();

		if(ky==BIKEY_RET)
		{
			static n=1;
			char buf[256];
			sprintf(buf,"snap%02d.ps",n++);
			BiEpsOpen(buf,400,300);
		}

		BiClearScreen();
		BIPOINTS s1,s2;
		BiSetPointS(&s1,0,0);
		BiSetPointS(&s2,wid,hei);
		BiDrawRect2(&s1,&s2,&background);

		GetOrbitViewer(&eye);
		BiStartBuffer(&eye);

		//ArInsDnm(&dnm,&BiOrgPA);
		ArInsDnmDivide(&dnm,&BiOrgPA);

		int nObj,classId;
		ArGetDnmNumObj(&nObj,&dnm);
		for(int i=0; i<nObj; i++)
		{
			ArGetDnmClassId(&classId,&dnm,i);
			if(classId==3)
			{
				ArSetDnmRelativeAttitude(&dnm,i,&att);
			}
		}
		att.h+=4096;

		BiFlushBuffer();

		if(ky==BIKEY_SPACE)
		{
			printf("%d %d %d\n",viewAngle.h,viewAngle.p,viewAngle.b);
		}

		if(ky==BIKEY_F1)
		{
			BiSetAngleDeg(&viewAngle,0,0,0);
		}
		else if(ky==BIKEY_F2)
		{
			BiSetAngleDeg(&viewAngle,90,0,0);
		}
		else if(ky==BIKEY_F3)
		{
			BiSetAngleDeg(&viewAngle,0,90,270);
		}
		else if(ky==BIKEY_F4)
		{
			viewAngle.b=0;
		}

		BIPOINTS s={32,32};
		BiDrawString2(&s,caption,&BiBlack);

		BiSwapBuffers();

		if(ky==BIKEY_RET)
		{
			BiEpsClose();
		}
	}
}


////////////////////////////////////////////////////////////

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("VIEWER");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetProjectionMode(BIPRJ_PERS);
	BiSetShadMode(BI_ON);


	if(ac==1)
	{
		char fn[256];
		if(HtLoadFileDialog(fn,"Select DNM","*.*","*.dnm")==BI_OK &&
		   ArLoadDnm(&dnm,fn)==BI_OK)
		{
			Visualize();
		}
	}
	else
	{
		if(ArLoadDnm(&dnm,av[1])==BI_OK)
		{
			Visualize();
		}
	}

	BiCloseWindow();
	return 0;
}

////////////////////////////////////////////////////////////
