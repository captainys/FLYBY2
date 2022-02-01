#include <stdio.h>
#include <stddef.h>
#include <math.h>

#include "impulse.h"
#include "irender.h"


static int BiRenderMalloc(void);

static void RnDummyDrawFunc(int objNo,int x,int y,int r,int g,int b,real z);
static int RnDummyAbortFunc(void);
static int RnPolygonBackFaceRemove(BIPOINT *v1,BIPOINT *v2);
static void RnTransformPoints(BIPOINT *dst,int np,BIPOINT *src);
static void RnMakeTable(int np,BIPOINT *p,BIPOINT *n,real *dif,real *spe);
static int RnFrontFace(BIPOINT *nv,BIPOINT *ev,BIPOINT *lv);
static real RnDiffuseReflectionLight(BIPOINT *nv,BIPOINT *lv);
static real RnSpecularReflectionLight(BIPOINT *nv,BIPOINT *ev,BIPOINT *lv);
static void RnProjectToScreen(BIPOINT2 *dst,int np,BIPOINT *p);
static int RnPolygonPaint(int np,BIPOINT2 *p,int r,int g,int b);
static void RnPolygPaintMinMax(int np,long *p,long *xmin,long *xmax,long *ymin,long *ymax);
static void RnPolygPaintPset(int x,int y,int r,int g,int b);
static int RnRoundPolygon3Paint(int np,BIPOINT2 *p,int r,int g,int b);
static void RnRoundPolyg3PaintPset(int x,int y,int r,int g,int b);
static real OuterLength(BIPOINT *p1,BIPOINT *p2,BIPOINT *p3);



typedef struct {
	BIPOINT pnt;
	real amb,dif,spe;
	real spePow;
} LIGHT;

typedef struct {
	int x1,y1,x2,y2,cx,cy;
} SCREEN;

typedef struct {
	BIPOSATT eyePos;
	BITRIGON eyeTrg;
	real mag,cpz;
	LIGHT lit;
	SCREEN scr;
} DRAWINFO;


/* (*AppAbortFunc)() ‚ª0ˆÈŠO‚ð•Ô‚·‚Æ’†’f */
static void (*AppDrawFunc)(int objNo,int x,int y,int r,int g,int b,real z);
static void (*App2DDrawFunc)(int objNo,int x,int y,int r,int g,int b);
static int (*AppAbortFunc)(void);
static int objNo;
static DRAWINFO info;
static int shadMode;


static void RnDummyDrawFunc(int objNo,int x,int y,int r,int g,int b,real z);
static void RnDummy2DDrawFunc(int objNo,int x,int y,int r,int g,int b);
static int RnDummyAbortFunc(void);

int BiRenderInitialize(void)
{
	AppDrawFunc=RnDummyDrawFunc;
	App2DDrawFunc=RnDummy2DDrawFunc;
	AppAbortFunc=RnDummyAbortFunc;

	BiSetPoint(&info.lit.pnt,0,10000,0);
	info.lit.amb=(real)0.3;
	info.lit.dif=(real)0.8;
	info.lit.spe=(real)0.8;
	info.lit.spePow=60.0F;

	info.scr.x1=0;
	info.scr.y1=0;
	info.scr.x2=639;
	info.scr.y2=479;
	info.scr.cx=320;
	info.scr.cy=240;

	BiSetPoint(&info.eyePos.p,0,0,0);
	BiSetAngle(&info.eyePos.a,0,0,0);
	BiMakeTrigonomy(&info.eyeTrg,&info.eyePos.a);
	info.mag=64.0F;
	info.cpz=5.0F;

	shadMode=BI_ON;

	return BiRenderMalloc();
}

static void RnDummyDrawFunc(int objNo,int x,int y,int r,int g,int b,real z)
{
}

static void RnDummy2DDrawFunc(int objNo,int x,int y,int r,int g,int b)
{
}

static int RnDummyAbortFunc(void)
{
	return 0;
}



int BiRenderGetReflection(real *amb,real *dif,real *spe,real *spePow)
{
	if(amb!=NULL){*amb=info.lit.amb;}
	if(dif!=NULL){*dif=info.lit.dif;}
	if(spe!=NULL){*spe=info.lit.spe;}
	if(spePow!=NULL){*spePow=info.lit.spePow;}
	return BI_TRUE;
}

int BiRenderGetScreen(int *x1,int *y1,int *x2,int *y2,int *cx,int *cy)
{
	if(x1!=NULL){*x1=info.scr.x1;}
	if(y1!=NULL){*y1=info.scr.y1;}
	if(x2!=NULL){*x2=info.scr.x2;}
	if(y2!=NULL){*y2=info.scr.y2;}
	if(cx!=NULL){*cx=info.scr.cx;}
	if(cy!=NULL){*cy=info.scr.cy;}
	return BI_TRUE;
}

int BiRenderGetProjection(real *mag,real *cpz)
{
	if(mag!=NULL){*mag=info.mag;}
	if(cpz!=NULL){*cpz=info.cpz;}
	return BI_TRUE;
}

int BiRenderSetObjectNumber(int num)
{
	objNo=num;
	return BI_TRUE;
}


int BiRenderSetLightPosition(BIPOINT *pnt)
{
	info.lit.pnt=*pnt;
	return BI_TRUE;
}


int BiRenderSetReflection(real amb,real dif,real spe,real spePow)
{
	info.lit.amb=amb;
	info.lit.dif=dif;
	info.lit.spe=spe;
	info.lit.spePow=spePow;
	return BI_TRUE;
}


int BiRenderSetScreen(int x1,int y1,int x2,int y2,int cx,int cy)
{
	info.scr.x1=x1;
	info.scr.y1=y1;
	info.scr.x2=x2;
	info.scr.y2=y2;
	info.scr.cx=cx;
	info.scr.cy=cy;
	return BI_TRUE;
}


int BiRenderSetProjection(real mag,real cpz)
{
	info.mag=mag;
	info.cpz=cpz;
	return BI_TRUE;
}


int BiRenderSetShadMode(int sw)
{
	if(sw==BI_ON || sw==BI_OFF)
	{
		shadMode=sw;
	}
	return BI_TRUE;
}

int BiRenderGetShadMode(int *sw)
{
	*sw=shadMode;
	return BI_TRUE;
}

int BiRenderSetEyePosition(BIPOSATT *eyePos)
{
	info.eyePos=*eyePos;
	BiMakeTrigonomy(&info.eyeTrg,&eyePos->a);
	return BI_TRUE;
}


int BiRenderSetDrawFunc(void (*drawFunc)(int,int,int,int,int,int,real))
{
	AppDrawFunc=drawFunc;
	return BI_TRUE;
}

int BiRenderSet2DDrawFunc(void (*drawFunc)(int,int,int,int,int,int))
{
	App2DDrawFunc=drawFunc;
	return BI_TRUE;
}

int BiRenderSetAbortFunc(int (*abortFunc)(void))
{
	AppAbortFunc=abortFunc;
	return BI_TRUE;
}


/* ------------------------------------------------------------------------- */
static int nLocPnt;
static BIPOINT *locPnt,*locNom;

static int nCutPnt;
static BIPOINT *cutPnt;

static BIPOINT locLight,nomVec,locCen;
static BIPLANE locPln;

static BIPOINT2 *scrPnt;
static real *dif,*spe;

static int BiRenderMalloc(void)
{
	locPnt=(BIPOINT *)BiMalloc(BI_RENDER_MAX_VTX*sizeof(BIPOINT));
	locNom=(BIPOINT *)BiMalloc(BI_RENDER_MAX_VTX*sizeof(BIPOINT));
	cutPnt=(BIPOINT *)BiMalloc(BI_RENDER_MAX_VTX*2*sizeof(BIPOINT));
	scrPnt=(BIPOINT2 *)BiMalloc(BI_RENDER_MAX_VTX*4*sizeof(BIPOINT2));
	dif=(real *)BiMalloc(BI_RENDER_MAX_VTX*2*sizeof(real));
	spe=(real *)BiMalloc(BI_RENDER_MAX_VTX*2*sizeof(real));

	if(locPnt!=NULL && locNom!=NULL && cutPnt!=NULL && scrPnt!=NULL && dif!=NULL && spe!=NULL)
	{
		return BI_OK;
	}
	return BI_ERR;
}

int BiRenderDrawPolygonOneSide(int np,BIPOINT *p,BIPOINT *nom,BICOLOR *c)
{
	BIPOINT ev1,ev2;
	/* Back Face Removal : Refer Two Vertex */
	BiSubPoint(&ev1,&p[0   ],&info.eyePos.p);
	BiSubPoint(&ev2,&p[np/2],&info.eyePos.p);
	if(RnPolygonBackFaceRemove(nom,&ev1)==BI_TRUE || RnPolygonBackFaceRemove(nom,&ev2)==BI_TRUE)
	{
		return BI_TRUE;
	}
	else
	{
		return BiRenderDrawPolygonTwoSide(np,p,c);
	}
}

int BiRenderDrawPolygonTwoSide(int np,BIPOINT *p,BICOLOR *c)
{
	int i;

	if(np>BI_RENDER_MAX_VTX)
	{
		/* printf("BiRenderDrawPolygon@render/render.c\n"); */
		/* printf("Warning : Too Many Points.\n"); */
		return BI_TRUE;
	}

	/* Phase 1  Transform & Clip Points into cutPnt[] */
	RnTransformPoints(locPnt,np,p);
	BiNearClipPolyg(&nCutPnt,cutPnt,np,locPnt,info.cpz);
	if(nCutPnt<=0)
	{
		return BI_TRUE;
	}


	/* Phase 2  Calculate Common Variables */
	BiSubPoint(&locLight,&info.lit.pnt,&info.eyePos.p);
	BiRotFastGtoL(&locLight,&locLight,&info.eyeTrg);

	BiAverageNormalVector(&nomVec,np,locPnt);

	locPln.o=locPnt[0];
	locPln.n=nomVec;

	BiSetPoint(&locCen,0,0,0);
	for(i=0; i<nCutPnt; i++)
	{
		BiAddPoint(&locCen,&locCen,&cutPnt[i]);
	}
	BiDivPoint(&locCen,&locCen,(real)nCutPnt);


	/* Phase 3  Calculate Color at each points */
	/* RnMakeTable(nLocPnt,locPnt,dif,spe); */


	/* Phase 4  Project to Screen Coordinate */
	RnProjectToScreen(scrPnt,nCutPnt,cutPnt);


	/* Phase 5  Polygon Paint */
	return RnPolygonPaint(nCutPnt,scrPnt,c->r,c->g,c->b);
}

/* ------------------------------------------------------------------------- */
int BiRenderDrawRoundPolygonOneSide(int np,BIPOINT *p,BIPOINT *n,BIPOINT *avgNom,BICOLOR *c)
{
	BIPOINT ev1,ev2;
	/* Back Face Removal : Refer Two Vertex */
	BiSubPoint(&ev1,&p[0   ],&info.eyePos.p);
	BiSubPoint(&ev2,&p[np/2],&info.eyePos.p);
	if(RnPolygonBackFaceRemove(avgNom,&ev1)==BI_TRUE || RnPolygonBackFaceRemove(avgNom,&ev2)==BI_TRUE)
	{
		return BI_TRUE;
	}
	else
	{
		return BiRenderDrawRoundPolygonTwoSide(np,p,n,c);
	}
}


int BiRenderDrawRoundPolygonTwoSide(int np,BIPOINT *p,BIPOINT *n,BICOLOR *c)
{
	int i,nTri,tri[(BI_RENDER_MAX_VTX-2)*3];
	BIPOINT pnt[3],nom[3];

	if(np==3)
	{
		return BiRenderDrawRoundPolygon3(p,n,c);
	}
	else if(BiRenderTrianglize(&nTri,tri,np,p)==BI_TRUE)
	{
		for(i=0; i<nTri; i++)
		{
			pnt[0]=p[tri[i*3  ]];
			pnt[1]=p[tri[i*3+1]];
			pnt[2]=p[tri[i*3+2]];
			nom[0]=n[tri[i*3  ]];
			nom[1]=n[tri[i*3+1]];
			nom[2]=n[tri[i*3+2]];
			if(BiRenderDrawRoundPolygon3(pnt,nom,c)!=BI_TRUE)
			{
				return BI_FALSE;
			}
		}
	}
	else
	{
		/* printf("Separation Error!\n"); */
	}
	return BI_TRUE;
}

int BiRenderDrawRoundPolygon3(BIPOINT *p,BIPOINT *n,BICOLOR *c)
{
	int i;

	/* Phase 1  Transform & Clip Points into cutPnt[] */
	RnTransformPoints(locPnt,3,p);
	BiNearClipPolyg(&nCutPnt,cutPnt,3,locPnt,info.cpz);
	if(nCutPnt<=0)
	{
		return BI_TRUE;
	}


	/* Phase 2  Calculate Common Variables */
	BiSubPoint(&locLight,&info.lit.pnt,&info.eyePos.p);
	BiRotFastGtoL(&locLight,&locLight,&info.eyeTrg);

	BiAverageNormalVector(&nomVec,3,locPnt);

	locPln.o=locPnt[0];
	locPln.n=nomVec;

	BiSetPoint(&locCen,0,0,0);
	for(i=0; i<nCutPnt; i++)
	{
		BiAddPoint(&locCen,&locCen,&cutPnt[i]);
	}
	BiDivPoint(&locCen,&locCen,(real)nCutPnt);

	for(i=0; i<3; i++)
	{
		locNom[i]=n[i];
		BiRotFastGtoL(&locNom[i],&locNom[i],&info.eyeTrg);
	}


	/* Phase 3  Calculate Color at each points */
	RnMakeTable(3,locPnt,locNom,dif,spe);


	/* Phase 4  Project to Screen Coordinate */
	RnProjectToScreen(scrPnt,nCutPnt,cutPnt);


	/* Phase 5  Polygon Paint */
	return RnRoundPolygon3Paint(nCutPnt,scrPnt,c->r,c->g,c->b);
}

/* ------------------------------------------------------------------------- */
static int RnPolygonBackFaceRemove(BIPOINT *v1,BIPOINT *v2)
{
	if(BiInnerPoint(v1,v2)>0)
	{
		return BI_TRUE;
	}
	return BI_FALSE;
}


static void RnTransformPoints(BIPOINT *dst,int np,BIPOINT *src)
{
	int i;
	for(i=0; i<np; i++)
	{
		BiSubPoint(&dst[i], &src[i],&info.eyePos.p);
		BiRotFastGtoL(&dst[i],&dst[i],&info.eyeTrg);
	}
}

static void RnMakeTable(int np,BIPOINT *p,BIPOINT *n,real *dif,real *spe)
{
	int i;
	BIPOINT ev,lv;

	for(i=0; i<np; i++)
	{
		/* Phase 1 Calc Light */
		BiSubPoint(&lv, &locLight,&p[i]);
		ev=p[i];
		BiNormalize(&lv,&lv);
		BiNormalize(&ev,&ev);

		/* Phase 2 Calc Coefficiency */
		if(RnFrontFace(&n[i],&ev,&lv)==BI_TRUE)
		{
			dif[i]=RnDiffuseReflectionLight(&n[i],&lv);
			spe[i]=RnSpecularReflectionLight(&n[i],&ev,&lv);
		}
		else
		{
			dif[i]=0.0F;
			spe[i]=0.0F;
		}
	}
}

static int RnFrontFace(BIPOINT *nv,BIPOINT *ev,BIPOINT *lv)
{
	real col,coe;

	col= nv->x*lv->x +nv->y*lv->y +nv->z*lv->z;
	coe= nv->x*ev->x +nv->y*ev->y +nv->z*ev->z;

	if(col*coe>=0)
	{
		return BI_FALSE;
	}
	else
	{
		return BI_TRUE;
	}
}

static real RnDiffuseReflectionLight(BIPOINT *nv,BIPOINT *lv)
{
	BIPOINT unv,ulv;

	unv=(*nv);
	ulv=(*lv);
	BiNormalize(&unv,&unv);
	BiNormalize(&ulv,&ulv);

	return BiAbs(BiInnerPoint(&unv,&ulv));
}

static real RnSpecularReflectionLight(BIPOINT *nv,BIPOINT *ev,BIPOINT *lv)
{
	BIPOINT unv,uhv,uev,ulv;
	real co;

	unv=(*nv);
	BiNormalize(&unv,&unv);
	uev=(*ev);
	BiNormalize(&uev,&uev);
	ulv=(*lv);
	BiNormalize(&ulv,&ulv);

	BiSubPoint(&uhv,&ulv,&uev);
	BiNormalize(&uhv,&uhv);

	co=BiAbs(BiInnerPoint(&unv,&uhv));
	return (real)pow(co,info.lit.spePow);
}

static void RnProjectToScreen(BIPOINT2 *dst,int np,BIPOINT *p)
{
	int i;
	for(i=0; i<np; i++)
	{
		dst[i].x=info.scr.cx+(real)(p[i].x*info.mag/p[i].z);
		dst[i].y=info.scr.cy-(real)(p[i].y*info.mag/p[i].z);
	}
}

static int RnPolygonPaint(int np,BIPOINT2 *p,int r,int g,int b)
{
	int i;
	long x,y,ip[(BI_RENDER_MAX_VTX*2)*2];
	long xmin,ymin,xmax,ymax;

	if((*AppAbortFunc)()!=0)
	{
		return BI_FALSE;
	}

	for(i=0; i<np; i++)
	{
		ip[i*2  ]=(long)p[i].x;
		ip[i*2+1]=(long)p[i].y;
	}

	RnPolygPaintMinMax(np,ip,&xmin,&xmax,&ymin,&ymax);
	if(xmax<info.scr.x1 || info.scr.x2<xmin || ymax<info.scr.y1 || info.scr.y2<ymin)
	{
		return BI_TRUE;
	}

	xmin=BiLarger(xmin,info.scr.x1);
	ymin=BiLarger(ymin,info.scr.y1);
	xmax=BiSmaller(xmax,info.scr.x2);
	ymax=BiSmaller(ymax,info.scr.y2);

	BiRenderStartPolygon(np,ip);
	for(y=ymin; y<=ymax; y++)
	{
		for(x=xmin; x<=xmax; x++)
		{
			if((*AppAbortFunc)()!=0)
			{
				return BI_FALSE;
			}

			if(BiRenderCheckInside(x,y)==BI_TRUE)
			{
				RnPolygPaintPset((int)x,(int)y,r,g,b);
			}
		}
	}
	BiRenderEndPolygon();
	return BI_TRUE;
}

static void RnPolygPaintMinMax(int np,long *p,long *xmin,long *xmax,long *ymin,long *ymax)
{
	int i;
	*xmin=p[0];
	*ymin=p[1];
	*xmax=*xmin;
	*ymax=*ymin;
	for(i=1; i<np; i++)
	{
		if(p[i*2]<*xmin)
		{
			*xmin=p[i*2];
		}
		if(*xmax<p[i*2])
		{
			*xmax=p[i*2];
		}

		if(p[i*2+1]<*ymin)
		{
			*ymin=p[i*2+1];
		}
		if(*ymax<p[i*2+1])
		{
			*ymax=p[i*2+1];
		}
	}
}

static void RnPolygPaintPset(int x,int y,int r,int g,int b)
{
	BILINE eyeLin;
	BIPOINT eyeVec,eyeCrs,litVec;
	real pntDif,pntSpe,pntZ,dr,dg,db;
	int pntR,pntG,pntB;

	extern int BiZBuffer;

	eyeVec.x=(real)(x-info.scr.cx);
	eyeVec.y=(real)(info.scr.cy-y);
	eyeVec.z=info.mag;

	BiSetPoint(&eyeLin.o,0,0,0);
	eyeLin.v=eyeVec;
	if(BiCrossPoint(&eyeCrs,&eyeLin,&locPln)!=BI_TRUE)
	{
		/* ^‰¡Œü‚©‚ê‚é‚Æ‚±‚¤‚¢‚¤‘[’u‚µ‚©‚È‚¢ */
		eyeCrs=locCen;
	}
	pntZ=eyeCrs.z;

	if(shadMode==BI_ON && BiZBuffer==BI_ON)
	{
		BiSubPoint(&litVec,&info.lit.pnt,&info.eyePos.p);
		BiRotFastGtoL(&litVec,&litVec,&info.eyeTrg);
		BiSubPoint(&litVec,&litVec,&eyeCrs);

		if(RnFrontFace(&nomVec,&eyeVec,&litVec)==BI_TRUE)
		{
			pntDif=RnDiffuseReflectionLight(&nomVec,&litVec);
			pntSpe=RnSpecularReflectionLight(&nomVec,&eyeVec,&litVec);
		}
		else
		{
			pntDif=0.0F;
			pntSpe=0.0F;
		}

		pntDif*=info.lit.dif;
		pntSpe*=info.lit.spe;

		dr=((real)r)/255;
		dg=((real)g)/255;
		db=((real)b)/255;

		pntG=(int)((dg*(info.lit.amb+pntDif)+pntSpe)*255);
		pntR=(int)((dr*(info.lit.amb+pntDif)+pntSpe)*255);
		pntB=(int)((db*(info.lit.amb+pntDif)+pntSpe)*255);

		pntG=cutoff(pntG,0,255);
		pntR=cutoff(pntR,0,255);
		pntB=cutoff(pntB,0,255);
		(*AppDrawFunc)(objNo,x,y,pntR,pntG,pntB,pntZ);
	}
	else
	{
		(*AppDrawFunc)(objNo,x,y,r,g,b,pntZ);
	}
}



static int RnRoundPolygon3Paint(int np,BIPOINT2 *p,int r,int g,int b)
{
	int i;
	long x,y,ip[(BI_RENDER_MAX_VTX*2)*2];
	long xmin,ymin,xmax,ymax;

	if((*AppAbortFunc)()!=0)
	{
		return BI_FALSE;
	}

	for(i=0; i<np; i++)
	{
		ip[i*2  ]=(long)p[i].x;
		ip[i*2+1]=(long)p[i].y;
	}

	RnPolygPaintMinMax(np,ip,&xmin,&xmax,&ymin,&ymax);
	if(xmax<info.scr.x1 || info.scr.x2<xmin ||
	   ymax<info.scr.y1 || info.scr.y2<ymin)
	{
		return BI_TRUE;
	}

	xmin=BiLarger(xmin,info.scr.x1);
	ymin=BiLarger(ymin,info.scr.y1);
	xmax=BiSmaller(xmax,info.scr.x2);
	ymax=BiSmaller(ymax,info.scr.y2);

	BiRenderStartPolygon(np,ip);
	for(y=ymin; y<=ymax; y++)
	{
		for(x=xmin; x<=xmax; x++)
		{
			if((*AppAbortFunc)()!=0)
			{
				return BI_FALSE;
			}

			if(BiRenderCheckInside(x,y)==BI_TRUE)
			{
				RnRoundPolyg3PaintPset((int)x,(int)y,r,g,b);
			}
		}
	}
	BiRenderEndPolygon();
	return BI_TRUE;
}

static void RnRoundPolyg3PaintPset(int x,int y,int r,int g,int b)
{
	BILINE eyeLin;
	BIPOINT eyeVec,eyeCrs;
	real pntDif,pntSpe,pntZ,dr,dg,db,t[3],tSum;
	int pntR,pntG,pntB;

	eyeVec.x=(real)(x-info.scr.cx);
	eyeVec.y=(real)(info.scr.cy-y);
	eyeVec.z=info.mag;

	BiSetPoint(&eyeLin.o,0,0,0);
	eyeLin.v=eyeVec;
	if(BiCrossPoint(&eyeCrs,&eyeLin,&locPln)!=BI_TRUE)
	{
		/* ^‰¡Œü‚©‚ê‚é‚Æ‚±‚¤‚¢‚¤‘[’u‚µ‚©‚È‚¢ */
		eyeCrs=locCen;
	}
	pntZ=eyeCrs.z;

	switch(shadMode)
	{
	case BI_ON:
		t[0]=OuterLength(&eyeCrs,&locPnt[1],&locPnt[2]);
		t[1]=OuterLength(&eyeCrs,&locPnt[0],&locPnt[2]);
		t[2]=OuterLength(&eyeCrs,&locPnt[0],&locPnt[1]);
		tSum=t[0]+t[1]+t[2];
		t[0]/=tSum;
		t[1]/=tSum;
		t[2]/=tSum;

		pntDif=info.lit.dif*(t[0]*dif[0]+t[1]*dif[1]+t[2]*dif[2]);
		pntSpe=info.lit.spe*(t[0]*spe[0]+t[1]*spe[1]+t[2]*spe[2]);

		dr=((real)r)/255;
		dg=((real)g)/255;
		db=((real)b)/255;

		pntG=(int)((dg*(info.lit.amb+pntDif)+pntSpe)*255);
		pntR=(int)((dr*(info.lit.amb+pntDif)+pntSpe)*255);
		pntB=(int)((db*(info.lit.amb+pntDif)+pntSpe)*255);

		pntG=cutoff(pntG,0,255);
		pntR=cutoff(pntR,0,255);
		pntB=cutoff(pntB,0,255);
		(*AppDrawFunc)(objNo,x,y,pntR,pntG,pntB,pntZ);
		break;
	case BI_OFF:
		(*AppDrawFunc)(objNo,x,y,r,g,b,pntZ);
		break;
	}
}

static real OuterLength(BIPOINT *p1,BIPOINT *p2,BIPOINT *p3)
{
	BIPOINT o,v1,v2;
	BiSubPoint(&v1,p2,p1);
	BiSubPoint(&v2,p3,p1);
	BiOuterProduct(&o,&v1,&v2);
	return BiLengthPoint3(&o);
}


/***********************************************************************/
static BIPOINTS org1,org2;

static void RnPushClipScreen(void)
{
	BIPOINTS t1,t2;
	BiGetClipScreenFrame(&org1,&org2);
	BiSetPointS(&t1,info.scr.x1  ,info.scr.y1  );
	BiSetPointS(&t2,info.scr.x2-1,info.scr.y2-1);
	BiSetClipScreenFrame(&t1,&t2);
}

static void RnPopClipScreen(void)
{
	BiSetClipScreenFrame(&org1,&org2);
}

static int RnClipLineScreen(BIPOINTS a[2],BIPOINT2 b[2])
{
	int r;
	BIPOINTS c,d;

	BiSetPointS(&c,b[0].x,b[0].y);
	BiSetPointS(&d,b[1].x,b[1].y);

	RnPushClipScreen();
	r=BiClipLineScrn(&a[0],&a[1],&c,&d);
	RnPopClipScreen();

	return r;
}

static real RnParametalizeLineX(BIPOINT a[2],int x)
{
	BIPOINT2 p;
	BILINE2 a1,a2;

	BiSetPoint2(&a1.o,0,0);
	BiSetPoint2(&a1.v,(x-info.scr.cx),info.mag);

	BiSetPoint2(&a2.o,a[0].x,a[0].z);
	BiSetPoint2(&a2.v,a[1].x-a[0].x,a[1].z-a[0].z);

	return (BiLine2CrossPoint(&p,&a1,&a2)==BI_OK ? p.y : 0);
}

static real RnParametalizeLineY(BIPOINT a[2],int y)
{
	BIPOINT2 p;
	BILINE2 a1,a2;

	BiSetPoint2(&a1.o,0,0);
	BiSetPoint2(&a1.v,(info.scr.cy-y),info.mag);

	BiSetPoint2(&a2.o,a[0].y,a[0].z);
	BiSetPoint2(&a2.v,a[1].y-a[0].y,a[1].z-a[0].z);

	return (BiLine2CrossPoint(&p,&a1,&a2)==BI_OK ? p.y : 0);
}

static real RnParametalizeLine(BIPOINT a[2],int x,int y)
{
	real dx,dy;
	dx=a[1].x-a[0].x;
	dy=a[1].y-a[0].y;

	if(BiAbs(dx)<YSEPS && BiAbs(dy)<YSEPS)
	{
		return 0.0F;
	}
	else if(BiAbs(dx)>BiAbs(dy))
	{
		return RnParametalizeLineX(a,x);
	}
	else
	{
		return RnParametalizeLineY(a,y);;
	}
}



/***********************************************************************/
int BiRenderDrawLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	BIPOINT a[2];
	BIPOINT2 b[2];
	BIPOINTS c[2];

	a[0]=*p1;
	a[1]=*p2;
	RnTransformPoints(a,2,a);
	BiNearClipLine(a,a,info.cpz);
	RnProjectToScreen(b,2,a);

	if(RnClipLineScreen(c,b)==BI_IN)
	{
		int x1,y1,x2,y2;
		int x,y,vx,vy;
		int dx,dy,sum;
		real z;

		/* (b[0].x,b[0].x,a[0].z)->(b[1].x,b[1].y,a[1].z) */

		x1=(int)c[0].x;
		y1=(int)c[0].y;
		x2=(int)c[1].x;
		y2=(int)c[1].y;

		x=x1;
		y=y1;
		dx=BiAbs(x2-x1);
		dy=BiAbs(y2-y1);
		vx=BiSgn(x2-x1);
		vy=BiSgn(y2-y1);
		sum=0;
		if(dx==0)
		{
			while(dy>=0)
			{
				z=RnParametalizeLine(a,x,y);
				(*AppDrawFunc)(objNo,x,y,col->r,col->g,col->b,z);
				y+=vy;
				dy--;
			}
		}
		else if(dy==0)
		{
			while(dx>=0)
			{
				z=RnParametalizeLine(a,x,y);
				(*AppDrawFunc)(objNo,x,y,col->r,col->g,col->b,z);
				x+=vx;
				dx--;
			}
		}
		else if(dx>dy)
		{
			while(x!=x2 || y!=y2)
			{
				z=RnParametalizeLine(a,x,y);
				(*AppDrawFunc)(objNo,x,y,col->r,col->g,col->b,z);
				if(sum>=0)
				{
					x+=vx;
					sum=sum-dy;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum-dy+dx;
				}
			}
		}
		else if(dx<dy)
		{
			while(x!=x2 || y!=y2)
			{
				z=RnParametalizeLine(a,x,y);
				(*AppDrawFunc)(objNo,x,y,col->r,col->g,col->b,z);
				if(sum<=0)
				{
					y+=vy;
					sum=sum+dx;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum+dx-dy;
				}
			}
		}
		else /* dx==dy */
		{
			while(x!=x2)
			{
				z=RnParametalizeLine(a,x,y);
				(*AppDrawFunc)(objNo,x,y,col->r,col->g,col->b,z);
				x+=vx;
				y+=vy;
			}
		}
	}
	return BI_OK;
}

int BiRenderDrawPset(BIPOINT *p,BICOLOR *col)
{
	BIPOINT a;
	BIPOINT2 b;

	RnTransformPoints(&a,1,p);
	if(a.z>info.cpz)
	{
		RnProjectToScreen(&b,1,&a);
		if(info.scr.x1<=(int)b.x && (int)b.x<info.scr.x2-1 && info.scr.y1<=(int)b.y && (int)b.y<info.scr.y2-1)
		{
			(*AppDrawFunc)(objNo,(int)b.x  ,(int)b.y  ,col->r,col->g,col->b,a.z);
			(*AppDrawFunc)(objNo,(int)b.x+1,(int)b.y  ,col->r,col->g,col->b,a.z);
			(*AppDrawFunc)(objNo,(int)b.x  ,(int)b.y+1,col->r,col->g,col->b,a.z);
			(*AppDrawFunc)(objNo,(int)b.x+1,(int)b.y+1,col->r,col->g,col->b,a.z);
		}
	}
	return BI_OK;
}

static void BiRenderDrawHorizontalLine(int x1,int x2,int y,BICOLOR *c,real z)
{
	int x;
	for(x=x1; x<x2; x++)
	{
		(*AppDrawFunc)(objNo,x,y,c->r,c->g,c->b,z);
	}
}

int BiRenderDrawCircle(BIPOINT *p,real r,BICOLOR *col)
{
	BIPOINT a;
	BIPOINT2 b;
	BIPOINTS c;

	RnTransformPoints(&a,1,p);
	if(a.z>info.cpz)
	{
		int sum,x,y,x1,x2,y1,y2;

		RnProjectToScreen(&b,1,&a);
		r=r*info.mag/a.z;
		BiSetPointS(&c,b.x,b.y);

		if(info.scr.x1<=c.x+r && c.x-r<info.scr.x2 && info.scr.y1<=c.y+r && c.y-r<info.scr.y2)
		{
			if(info.scr.y1<=c.y && c.y<info.scr.y2)
			{
				x1=(int)((c.x-r< info.scr.x1) ? info.scr.x1 : c.x-r);
				x2=(int)((c.x+r>=info.scr.x2) ? info.scr.x2 : c.x+r);
				BiRenderDrawHorizontalLine(x1,x2,(int)c.y,col,a.z);
			}

			x=(int)r;
			y=0;
			sum=0;
			while(y<r)
			{
				if(sum<=0)
				{
					sum+=(2*y+1);
					y++;
					x1=(int)((c.x-x< info.scr.x1) ? info.scr.x1 : c.x-x);
					x2=(int)((c.x+x>=info.scr.x2) ? info.scr.x2 : c.x+x);
					y1=(int)c.y+y;
					y2=(int)c.y-y;
					if(info.scr.y1<=y1 && y1<info.scr.y2)
					{
						BiRenderDrawHorizontalLine(x1,x2,y1,col,a.z);
					}
					if(info.scr.y1<=y2 && y2<info.scr.y2)
					{
						BiRenderDrawHorizontalLine(x1,x2,y2,col,a.z);
					}
				}
				else
				{
					sum+=(-2*x+1);
					x--;
				}
			}
		}
	}

	return BI_OK;
}

/***********************************************************************/
static void BiRenderDrawHorizontalLine2(int x1,int x2,int y,BICOLOR *c)
{
	int x;
	for(x=x1; x<x2; x++)
	{
		(*App2DDrawFunc)(objNo,x,y,c->r,c->g,c->b);
	}
}

/***********************************************************************/

void BiRenderDrawLine2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINT2 b[2];
	BIPOINTS c[2];

	BiSetPoint2(&b[0],s1->x,s1->y);
	BiSetPoint2(&b[1],s2->x,s2->y);
	if(RnClipLineScreen(c,b)==BI_IN)
	{
		int x1,y1,x2,y2;
		int x,y,vx,vy;
		int dx,dy,sum;

		/* (b[0].x,b[0].x,a[0].z)->(b[1].x,b[1].y,a[1].z) */

		x1=(int)c[0].x;
		y1=(int)c[0].y;
		x2=(int)c[1].x;
		y2=(int)c[1].y;

		x=x1;
		y=y1;
		dx=BiAbs(x2-x1);
		dy=BiAbs(y2-y1);
		vx=BiSgn(x2-x1);
		vy=BiSgn(y2-y1);
		sum=0;
		if(dx==0)
		{
			while(dy>=0)
			{
				(*App2DDrawFunc)(objNo,x,y,col->r,col->g,col->b);
				y+=vy;
				dy--;
			}
		}
		else if(dy==0)
		{
			while(dx>=0)
			{
				(*App2DDrawFunc)(objNo,x,y,col->r,col->g,col->b);
				x+=vx;
				dx--;
			}
		}
		else if(dx>dy)
		{
			while(x!=x2 || y!=y2)
			{
				(*App2DDrawFunc)(objNo,x,y,col->r,col->g,col->b);
				if(sum>=0)
				{
					x+=vx;
					sum=sum-dy;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum-dy+dx;
				}
			}
		}
		else if(dx<dy)
		{
			while(x!=x2 || y!=y2)
			{
				(*App2DDrawFunc)(objNo,x,y,col->r,col->g,col->b);
				if(sum<=0)
				{
					y+=vy;
					sum=sum+dx;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum+dx-dy;
				}
			}
		}
		else /* dx==dy */
		{
			while(x!=x2)
			{
				(*App2DDrawFunc)(objNo,x,y,col->r,col->g,col->b);
				x+=vx;
				y+=vy;
			}
		}
	}
}

void BiRenderDrawPset2(BIPOINTS *s,BICOLOR *c)
{
	if(info.scr.x1<=s->x && s->x<info.scr.x2-1 && info.scr.y1<=s->y && s->y<info.scr.y2-1)
	{
		(*App2DDrawFunc)(objNo,(int)s->x  ,(int)s->y  ,c->r,c->g,c->b);
		(*App2DDrawFunc)(objNo,(int)s->x+1,(int)s->y  ,c->r,c->g,c->b);
		(*App2DDrawFunc)(objNo,(int)s->x  ,(int)s->y+1,c->r,c->g,c->b);
		(*App2DDrawFunc)(objNo,(int)s->x+1,(int)s->y+1,c->r,c->g,c->b);
	}
}

void BiRenderDrawEllipse2(BIPOINTS *c,long radx,long rady,BICOLOR *col)
{
	long xe;
	int sum,x,y,x1,x2;

	if(info.scr.x1<=c->x+radx && c->x-radx<info.scr.x2-1 && info.scr.y1<=c->y+rady && c->y-rady<info.scr.y2-1)
	{
		if(0<=c->y && c->y<info.scr.y2-1)
		{
			x1=(int)((c->x-radx< info.scr.x1)   ? info.scr.x1   : c->x-radx);
			x2=(int)((c->x+radx>=info.scr.x2-1) ? info.scr.x2-1 : c->x+radx);
			BiRenderDrawHorizontalLine2(x1,x2,(int)c->y,col);
		}

		x=(int)rady;
		y=0;
		sum=0;
		while(y<rady)
		{
			if(sum<=0)
			{
				sum+=(2*y+1);
				y++;
				xe=(int)((long)x*radx/rady);
				x1=(int)((c->x-xe< info.scr.x1)   ? info.scr.x1   : (int)c->x-xe);
				x2=(int)((c->x+xe>=info.scr.x2-1) ? info.scr.x2-1 : (int)c->x+xe);
				if(info.scr.y1<=c->y-y && c->y-y<info.scr.y2-1)
				{
					BiRenderDrawHorizontalLine2(x1,x2,(int)c->y-y,col);
				}
				if(info.scr.y1<=c->y+y && c->y+y<info.scr.y2-1)
				{
					BiRenderDrawHorizontalLine2(x1,x2,(int)c->y+y,col);
				}
			}
			else
			{
				sum+=(-2*x+1);
				x--;
			}
		}
	}
}

void BiRenderDrawPolygon2(int ns,BIPOINTS *s,BICOLOR *c)
{
	int i;
	long x,y,ip[(BI_RENDER_MAX_VTX*2)*2];
	long xmin,ymin,xmax,ymax;

	if((*AppAbortFunc)()!=0)
	{
		return;
	}

	for(i=0; i<ns; i++)
	{
		ip[i*2  ]=s[i].x;
		ip[i*2+1]=s[i].y;
	}

	RnPolygPaintMinMax(ns,ip,&xmin,&xmax,&ymin,&ymax);
	if(xmax<info.scr.x1 || info.scr.x2<xmin || ymax<info.scr.y1 || info.scr.y2<ymin)
	{
		return;
	}

	xmin=BiLarger(xmin,info.scr.x1);
	ymin=BiLarger(ymin,info.scr.y1);
	xmax=BiSmaller(xmax,info.scr.x2);
	ymax=BiSmaller(ymax,info.scr.y2);

	BiRenderStartPolygon(ns,ip);
	for(y=ymin; y<=ymax; y++)
	{
		for(x=xmin; x<=xmax; x++)
		{
			if((*AppAbortFunc)()!=0)
			{
				return;
			}

			if(BiRenderCheckInside(x,y)==BI_TRUE)
			{
				(*App2DDrawFunc)(objNo,(int)x,(int)y,c->r,c->g,c->b);
			}
		}
	}
	BiRenderEndPolygon();
}

void BiRenderDrawRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	long x,y;
	BIPOINTS a,b;

	BiSetPointS(&a,BiSmaller(s1->x,s2->x),BiSmaller(s1->y,s2->y));
	BiSetPointS(&b,BiLarger(s1->x,s2->x),BiLarger(s1->y,s2->y));

	a.x=BiLarger(a.x,(long)info.scr.x1);
	a.y=BiLarger(a.y,(long)info.scr.y1);
	b.x=BiSmaller(b.x,(long)info.scr.x2);
	b.y=BiSmaller(b.y,(long)info.scr.y2);
	if(a.x<=b.x && a.y<=b.y)
	{
		for(y=a.y; y<=b.y; y++)
		{
			for(x=a.x; x<=b.x; x++)
			{
				(*App2DDrawFunc)(objNo,(int)x,(int)y,c->r,c->g,c->b);
			}
		}
	}
}

void BiRenderDrawEmptyEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
}

