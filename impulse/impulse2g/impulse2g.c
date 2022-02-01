#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "impulse2g.h"


#define BIWORKSIZE 0x80000L
static char BiWork[BIWORKSIZE];


int BiOpenWindow2G(int wid,int hei)
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("Blue Impulse 3D Graphics Development Kit for IPL2G");
	BiOpenWindow(wid,hei);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	return BI_OK;
}

int BiUpdateDevice2G(void)
{
	BIPROJ prj;
	BiUpdateDevice();
	BiGetStdProjection(&prj);
	prj.nearz=0.05F;
	BiSetProjection(&prj);

	if(BiKey(BIKEY_ESC)==BI_ON)
	{
		while(BiInkey()!=BIKEY_NULL);
	}

	return BI_OK;
}

int BiKey2G(void)
{
	int k;
	while(BiInkey()!=BIKEY_NULL)
	{
		BiUpdateDevice2G();
	}
	while((k=BiInkey())==BIKEY_NULL)
	{
		BiUpdateDevice2G();
	}
	return k;
}

void BiMouse2G(int *lb,int *mb,int *rb,int *mx,int *my,int wait)
{
	int l,m,r;
	long x,y;
	if(wait==BI_OFF)
	{
		BiMouse(&l,&m,&r,&x,&y);
	}
	else
	{
		l=BI_ON;
		m=BI_ON;
		r=BI_ON;
		while(l==BI_ON || r==BI_ON || m==BI_ON)
		{
			BiMouse(&l,&m,&r,&x,&y);
		}
		while(l!=BI_ON && r!=BI_ON && m!=BI_ON)
		{
			BiMouse(&l,&m,&r,&x,&y);
		}
	}
	*lb=l;
	*mb=m;
	*rb=r;
	*mx=x;
	*my=y;
}


static BIATTITUDE va2G={0,0,0};
static BIPOINT target2G={0.0F,0.0F,0.0F};
static real vd2G=10.0F;

#ifdef SATA
static int writing;
static int number=0;
#endif

void BiStartBuffer2G(BIPOSATT *vp)
{
	BiClearScreen();

#ifdef SATA
	writing=-1;
	if(BiKey(BIKEY_Y)==BI_ON && BiKey(BIKEY_S)==BI_ON)
	{
		char fn[256];
		sprintf(fn,"bi%04d",number);

		if(BiKey(BIKEY_T)==BI_ON)
		{
			strcat(fn,".tif");
			BiRenderOpen(fn,BIRENDER_TIFF,320,240);
			number++;
			writing=0;
		}
		else if(BiKey(BIKEY_B)==BI_ON)
		{
			strcat(fn,".bmp");
			BiRenderOpen(fn,BIRENDER_BMP,320,240);
			number++;
			writing=1;
		}
		else if(BiKey(BIKEY_P)==BI_ON)
		{
			strcat(fn,".ppm");
			BiRenderOpen(fn,BIRENDER_PPM,320,240);
			number++;
			writing=2;
		}
		else if(BiKey(BIKEY_W)==BI_ON)
		{
			strcat(fn,".wrl");
			BiVrml2Open(fn);
			number++;
			writing=3;
		}
		else if(BiKey(BIKEY_E)==BI_ON)
		{
			strcat(fn,".ps");
			BiEpsOpen(fn,320,240);
			number++;
			writing=4;
		}
	}
#endif

	if(vp!=NULL)
	{
		BiStartBuffer(vp);
		BiSetLightPosition(&vp->p);
	}
	else
	{
		BIPOSATT vp2G;
		BiGetOrbitViewer2G(&vp2G);
		BiStartBuffer(&vp2G);
		BiSetLightPosition(&vp2G.p);
	}
}

void BiGetOrbitViewer2G(BIPOSATT *pos)
{
	BIPOSATT vp2G;
	BIVECTOR vec;

	BiSetPoint(&vec,0,0,vd2G);
	BiRotLtoG(&vec,&vec,&va2G);

	BiSubPoint(&vp2G.p,&target2G,&vec);
	vp2G.a=va2G;

	*pos=vp2G;
}

void BiSwapBuffers2G(void)
{
	BiFlushBuffer();
	BiSwapBuffers();

#ifdef SATA
	switch(writing)
	{
	case 0:
		BiRenderClose();
		break;
	case 1:
		BiRenderClose();
		break;
	case 2:
		BiRenderClose();
		break;
	case 3:
		BiVrml2Close();
		break;
	case 4:
		BiEpsClose();
		break;
	}
#endif
}

void BiZoom(void)
{
	if(vd2G>=1.0F)
	{
		vd2G/=1.0F;
	}
}

void BiMooz(void)
{
	if(vd2G<100.0F)
	{
		vd2G*=1.0F;
	}
}

void BiOrbitViewer2G(void)
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
		BiPitchUp(&va2G,&va2G,y*6,x*6);
	}
	if(l==BI_ON)
	{
		real m,t;
		m=1.0F+(real)y/5000.0F;
		t=vd2G*m;
		if(1.0F<=t && t<100.0F)
		{
			vd2G=t;
		}
	}
}

void BiCube2G(float x,float y,float z,float lx,float ly,float lz,BICOLOR *c)
{
	int i,j;
	BIPOSATT mat;
	BIPOINT buf[4];
	BIPOINT coord[24]=
	{
		{-.5F,-.5F,+.5F},{-.5F,+.5F,+.5F},{+.5F,+.5F,+.5F},{+.5F,-.5F,+.5F},
		{+.5F,-.5F,-.5F},{+.5F,+.5F,-.5F},{-.5F,+.5F,-.5F},{-.5F,-.5F,-.5F},
		{-.5F,+.5F,+.5F},{-.5F,+.5F,-.5F},{+.5F,+.5F,-.5F},{+.5F,+.5F,+.5F},
		{+.5F,-.5F,+.5F},{+.5F,-.5F,-.5F},{-.5F,-.5F,-.5F},{-.5F,-.5F,+.5F},
		{-.5F,-.5F,+.5F},{-.5F,-.5F,-.5F},{-.5F,+.5F,-.5F},{-.5F,+.5F,+.5F},
		{+.5F,+.5F,+.5F},{+.5F,+.5F,-.5F},{+.5F,-.5F,-.5F},{+.5F,-.5F,+.5F}
	};
	BIPOINT normal[6]=
	{
		{ 0.0F, 0.0F, 1.0F},
		{ 0.0F, 0.0F,-1.0F},
		{ 0.0F, 1.0F, 0.0F},
		{ 0.0F,-1.0F, 0.0F},
		{-1.0F, 0.0F, 0.0F},
		{ 1.0F, 0.0F, 0.0F}
	};

	BiSetPoint(&mat.p,x,y,z);
	mat.a=BiOrgA;

	BiPushMatrix(&mat);
	for(i=0; i<6; i++)
	{
		for(j=0; j<4; j++)
		{
			buf[j]=coord[i*4+j];
			buf[j].x*=lx;
			buf[j].y*=ly;
			buf[j].z*=lz;
		}
		BiInsPolygFast(4,buf,NULL,&normal[i],c);
	}
	BiPopMatrix();
}

void BiLine2G(float x1,float y1,float z1,float x2,float y2,float z2,BICOLOR *c)
{
	BIPOINT p1,p2;
	BiSetPoint(&p1,x1,y1,z1);
	BiSetPoint(&p2,x2,y2,z2);
	BiInsLine(&p1,&p2,c);
}

void BiPset2G(float x,float y,float z,BICOLOR *c)
{
	BIPOINT p;
	BiSetPoint(&p,x,y,z);
	BiInsPset(&p,c);
}

static char *Solidplane[]={
	"SURF",
	"VER 0.000 1.500 0.000 R",
	"VER 3.600 -0.000 -0.000 R",
	"VER 0.001 0.000 5.000 R",
	"VER -0.000 -0.500 0.000 R",
	"VER -3.600 0.000 0.001 R",
	"VER -0.000 0.000 -1.800 R",
	"FAC",
	"COL 31",
	"NOR 1.200 0.500 1.667 0.845 0.439 0.306",
	"VER 0 1 2",
	"END",
	"FAC",
	"COL 31",
	"NOR -1.200 -0.166 1.667 -0.468 -0.852 0.237",
	"VER 2 3 4",
	"END",
	"FAC",
	"COL 31",
	"NOR 1.200 -0.167 1.667 0.468 -0.852 0.237",
	"VER 2 1 3",
	"END",
	"FAC",
	"COL 31",
	"NOR -1.200 0.500 -0.600 -0.447 0.236 -0.863",
	"VER 0 4 5",
	"END",
	"FAC",
	"COL 31",
	"NOR -1.200 -0.167 -0.600 -0.366 -0.662 -0.654",
	"VER 4 3 5",
	"END",
	"FAC",
	"COL 31",
	"NOR 1.200 0.500 -0.600 0.446 0.236 -0.863",
	"VER 0 5 1",
	"END",
	"FAC",
	"COL 31",
	"NOR 1.200 -0.167 -0.600 0.365 -0.662 -0.654",
	"VER 5 3 1",
	"END",
	"FAC",
	"COL 31",
	"NOR -1.200 0.500 1.667 -0.845 0.440 0.306",
	"VER 0 2 4",
	"END",
	"END",
	NULL
};

void BiPlane2G(BIPOSATT *pos)
{
	static int first=BI_ON;
	static BISRF srf;

	if(first==BI_ON)
	{
		BiLoadSrfFromString(&srf,Solidplane);
		BiScaleSrf(&srf,0.05F);
		first=BI_OFF;
	}

	BiInsSrf(&srf,pos);
}

