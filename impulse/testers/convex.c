#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int ac,char *av[])
{
	int key;
	int n;
	BIPOINT p[256],r[256];
	BIPOINT2 p2[256];
	BIANGLE ang;
	BIPROJ prj;

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

	/* Main Procedure Here */

	n=0;
	BiSetAngleDeg(&ang,20,30,40);
	while((key=BiInkey())!=BIKEY_ESC)
	{
		int i,cvx3,cvxr,cvx2,qcvx3,qcvxr,qcvx2;
		int lb,mb,rb;
		long mx,my,cx,cy;
		BIPROJ prj;
		BIPOSATT eye;
		BIPOLYGON plg;
		BIPOLYGON2 plg2;
		BIPOINTS s;

		BiUpdateDevice();
		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiMouse(&lb,&mb,&rb,&mx,&my);
		BiGetWindowSize(&cx,&cy);
		cx/=2;
		cy/=2;

		if(key==BIKEY_SPACE)
		{
			BiSetPoint(&p[n],mx-cx,cy-my,0);
			BiSetPoint2(&p2[n],mx-cx,cy-my);
			n++;
		}
		if(key==BIKEY_C)
		{
			n=0;
		}

		BiClearScreen();

		BiSetPoint(&eye.p,0,0,-prj.magx);
		BiSetAngle(&eye.a,0,0,0);
		BiStartBuffer(&eye);

		for(i=0; i<n; i++)
		{
			BiRotLtoG(&r[i],&p[i],&ang);
		}

		BiInsPolyg(n,p,&BiRed);

		BiFlushBuffer();

		if(n>0)
		{
			BiOvwLine(&p[0],&p[n-1],&BiWhite);
			BiOvwLine(&r[0],&r[n-1],&BiRed);
			for(i=0; i<n-1; i++)
			{
				BiOvwLine(&p[i],&p[i+1],&BiWhite);
				BiOvwLine(&r[i],&r[i+1],&BiRed);
			}
		}

		plg.np=n;
		plg.p=p;
		cvx3=BiCheckConvex(&plg);
		qcvx3=BiQuickCheckConvex(&plg);

		plg.np=n;
		plg.p=r;
		cvxr=BiCheckConvex(&plg);
		qcvxr=BiQuickCheckConvex(&plg);

		plg2.np=n;
		plg2.p=p2;
		cvx2=BiCheckConvex2(&plg2);
		qcvx2=BiQuickCheckConvex2(&plg2);

		BiSetPointS(&s,24,24);
		BiDrawString2(&s,(cvx3==BI_TRUE ? "3:CONVEX" : "3:CONCAVE"),&BiBlue);
		s.y+=24;
		BiDrawString2(&s,(cvxr==BI_TRUE ? "R:CONVEX" : "R:CONCAVE"),&BiBlue);
		s.y+=24;
		BiDrawString2(&s,(cvx2==BI_TRUE ? "2:CONVEX" : "2:CONCAVE"),&BiBlue);
		s.y+=24;

		BiDrawString2(&s,(qcvx3==BI_TRUE ? "Q3:CONVEX" : "Q3:CONCAVE"),&BiBlue);
		s.y+=24;
		BiDrawString2(&s,(qcvxr==BI_TRUE ? "QR:CONVEX" : "QR:CONCAVE"),&BiBlue);
		s.y+=24;
		BiDrawString2(&s,(qcvx2==BI_TRUE ? "Q2:CONVEX" : "Q2:CONCAVE"),&BiBlue);
		s.y+=24;

		BiSwapBuffers();
	}

	BiCloseWindow();
	return 0;
}
