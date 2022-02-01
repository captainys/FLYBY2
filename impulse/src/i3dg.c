#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impulse.h"
#include "i3dg.h"

#include "irender.h"
#include "ivrmfile.h"


static void BiMakeLocal(BIPOINT *d,BIPOINT *cen,BIPOINT *box,int n,BIPOINT *s);
static void BiMakeLocalFast(BIPOINT *d,BIPOINT *box,int n,BIPOINT *s);
static void BiFlushDrawLine(BITWOPOINT *p,BICOLOR *c);
static void BiFlushDrawPset(BIONEPOINT *p,BICOLOR *c);
static void BiFlushDrawCircle(BICIRCLE *circ,BICOLOR *c);
static void BiFlushDrawPolyg(BINPOINT *plg,BICOLOR *c);
static void BiFlushDrawMarker(BIMARKER *mk,BICOLOR *c);
static void BiFlushDrawText(BITEXT *tx,BICOLOR *c);
static void BiFlushDrawRpolyg(BINPNTNOM *plg,BICOLOR *c);



extern int BiEpsMode;

extern int BiRendLinkFlag;
extern void BiRenderResetProjection(BIPROJ *prj);
extern void BiRenderResetEyePosition(void);
extern void BiRenderResetLightPosition(void);
extern void BiRenderClearZBuffer(void);


static unsigned long BiStackSize=0;
static unsigned long BiStackUsing=0;
static char *BiStack=NULL;

static unsigned long BiStackSize2=0;
static unsigned long BiStackUsing2=0;
static char *BiStack2=NULL;

static unsigned long BiStackSize3=0;
static unsigned long BiStackUsing3=0;
static char *BiStack3=NULL;

int BiPrjMode,BiShadMode;
real BiOrthoDist;
BIPROJ BiPrj;
real BiPrjFovLeft,BiPrjFovRight,BiPrjFovUp,BiPrjFovDown;

BIAXISF BiEyeAxs;     /* Global -> Viewing */
BIAXISF BiMdlAxs;     /* Model  -> Global  */
BIAXISF BiCnvAxs;     /* Model  -> Viewing */
BIAXISF BiShadowAxs;  /* Global -> Shadow Plane */

BIPOINT BiLightPos,BiRelLightPos;

#define MX_CNVAXS 16
static int BiNAxsStack=0;
static BIAXISF BiAxsStack[MX_CNVAXS];


void BiGraphInitialize(char *work,unsigned long wSize)
{
	BiStackSize=wSize;
	BiStackUsing=0;
	BiStack=work;

	BiPrjMode=BIPRJ_PERS;
	BiShadMode=BI_OFF;
	BiOrthoDist=128.0F;

	BiClear3dObject();

	BiMakeAxisF(&BiEyeAxs,&BiOrgPA);
	BiMakeAxisF(&BiMdlAxs,&BiOrgPA);
	BiMakeAxisF(&BiCnvAxs,&BiOrgPA);
	BiMakeAxisF(&BiShadowAxs,&BiOrgPA);

	BiNAxsStack=0;

	BiSetPoint(&BiLightPos,0,10000,-5000);  /* Assume Sun Light from South */
}

void BiGraphSetBuffer2(char *work,unsigned long wSize)
{
	BiStackSize2=wSize;
	BiStackUsing2=0;
	BiStack2=work;
}

void BiGraphSetBuffer3(char *work,unsigned long wSize)
{
	BiStackSize3=wSize;
	BiStackUsing3=0;
	BiStack3=work;
}

void BiSetProjectionMode(int mode)
{
	switch(mode)
	{
	case BIPRJ_PERS:
	case BIPRJ_ORTHO:
		BiPrjMode=mode;
	#ifdef BIACCEL3D
		BiAfterSetProjectionMode(BiPrjMode,&BiPrj);
	#endif
		break;
	default:
		break;
	}
}

void BiSetShadMode(int mode)
{
	switch(mode)
	{
	case BI_ON:
	case BI_OFF:
		BiShadMode=mode;
	#ifdef BIACCEL3D
		BiAfterSetShadeMode(mode);
	#endif
		break;
	default:
		/* "BiSetShadMode\n" */
		/* "  Warning : Illegal Parameter %d\n",mode */
		break;
	}
}

void BiSetOrthoDist(real dist)
{
	if(dist>0)
	{
		BiOrthoDist=dist;
	#ifdef BIACCEL3D
		BiAfterSetOrthoDist(BiPrjMode,&BiPrj,dist);
	#endif
	}
	else
	{
		/* "BiSetOrthoDist\n" */
		/* "  Warning : Illegal Parameter %lf\n" */
	}
}

void BiSetProjection(BIPROJ *prj)
{
	BiPrj=*prj;
	BiPrjFovLeft =((real)        prj->cx)/prj->magx;
	BiPrjFovRight=((real)prj->lx-prj->cx)/prj->magx;
	BiPrjFovUp   =((real)        prj->cy)/prj->magy;
	BiPrjFovDown =((real)prj->ly-prj->cy)/prj->magy;

	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderResetProjection(prj);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsResetProjection(prj);
	}

#ifdef BIACCEL3D
	BiAfterSetProjection(BiPrjMode,prj);
#endif
}


void BiStartBuffer(BIPOSATT *eye)
{
	BiMakeAxisF(&BiEyeAxs,eye);
	BiStackUsing=0;
	BiStackUsing2=0;
	BiStackUsing3=0;

	BiConvGtoL(&BiRelLightPos,&BiLightPos,&BiEyeAxs);

	BiClear3dObject();

	BiClearLocalMatrix();

	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderResetEyePosition();
		BiRenderResetLightPosition();
		BiRenderClearZBuffer();
	}

#ifdef BIACCEL3D
	BiAfterStartBuffer(eye);
#endif
}


/****** Matrix Handling ***********************************************/

void BiSetLocalMatrix(BIPOSATT *mdl)
{
	BiMakeAxisF(&BiMdlAxs,mdl);
	BiConvAxisGtoLF(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#ifdef BIACCEL3D
	BiAfterChangeMatrix(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#endif
}

void BiClearLocalMatrix(void)
{
	BiMakeAxisF(&BiMdlAxs,&BiOrgPA);
	BiConvAxisGtoLF(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#ifdef BIACCEL3D
	BiAfterChangeMatrix(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#endif
}

int BiPushMatrix(BIPOSATT *pos)
{
	BIAXISF axs;

	BiMakeAxisF(&axs,pos);
	if(BiNAxsStack<MX_CNVAXS)
	{
		BiAxsStack[BiNAxsStack]=BiMdlAxs;
		BiNAxsStack++;
		BiConvAxisLtoGF(&BiMdlAxs,&axs,&BiMdlAxs);
		BiConvAxisGtoLF(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#ifdef BIACCEL3D
		BiAfterChangeMatrix(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#endif
		return BI3DG_NOERR;
	}
	else
	{
		return BI3DG_STACKOVERFLOW;
	}
}

int BiPopMatrix(void)
{
	if(BiNAxsStack>0)
	{
		BiNAxsStack--;
		BiMdlAxs=BiAxsStack[BiNAxsStack];
		BiConvAxisGtoLF(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#ifdef BIACCEL3D
		BiAfterChangeMatrix(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs);
#endif
		return BI3DG_NOERR;
	}
	else
	{
		return BI3DG_STACKOVERFLOW;
	}
}

int BiSetShadowPlane(BIPOSATT *pos)
{
	BiMakeAxisF(&BiShadowAxs,pos);
	return BI3DG_NOERR;
}

/**********************************************************************/



void *BiGetTmpStack(size_t siz)
{
	register void * ret;
	siz=(siz+3)&(~3);
	if(BiStackUsing3+siz <= BiStackSize3)
	{
		ret=BiStack3+BiStackUsing3;
		BiStackUsing3+=siz;
		return ret;
	}
	else if(BiStackUsing2+siz <= BiStackSize2)
	{
		ret=BiStack2+BiStackUsing2;
		BiStackUsing2+=siz;
		return ret;
	}
	else if(BiStackUsing+siz <= BiStackSize)
	{
		ret=BiStack+BiStackUsing;
		BiStackUsing+=siz;
		return ret;
	}
	else
	{
		return NULL;
	}
}

void *BiPushTmpStack(unsigned long *ptr,size_t siz)
{
	register void * ret;
	siz=(siz+3)&(~3);
	*ptr=BiStackUsing;
	if(BiStackUsing+siz > BiStackSize)
	{
		return NULL;
	}
	ret=BiStack+BiStackUsing;
	BiStackUsing+=siz;
	return ret;
}

void BiPopTmpStack(unsigned long ptr)
{
	BiStackUsing=ptr;
}


/***********************************************************************/
int BiInsLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	int clip;
	BIOBJECT *neo;
	BIPOINT tmp[2];

	BiConvLtoG(&tmp[0],p1,&BiCnvAxs);
	BiConvLtoG(&tmp[1],p2,&BiCnvAxs);

	if(BiVrmlMode==BI_ON)
	{
		BiVrmlLine(&tmp[0],&tmp[1],col);
	}

	if(BiCheckInsideViewPort(&clip,2,tmp)==BI_IN)
	{
#ifndef BIACCEL3D
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkLine(&tmp[0],&tmp[1],col);
		}

		neo=BiNew3dObject(BIOBJ_LINE,(tmp[0].z+tmp[1].z)/2);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		neo->col=*col;
		neo->attr.twoPnt.clip=clip;
		neo->attr.twoPnt.p[0]=tmp[0];
		neo->attr.twoPnt.p[1]=tmp[1];
#else
		BiDrawLine3(p1,p2,col);

		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkLine(&tmp[0],&tmp[1],col);
		}

		if(BiEpsMode==BI_ON)
		{
			neo=BiNew3dObject(BIOBJ_LINE,(tmp[0].z+tmp[1].z)/2);
			if(neo==NULL)
			{
				return BI3DG_STACKOVERFLOW;
			}
			neo->col=*col;
			neo->attr.twoPnt.clip=clip;
			neo->attr.twoPnt.p[0]=tmp[0];
			neo->attr.twoPnt.p[1]=tmp[1];
		}
#endif
	}
	return BI3DG_NOERR;
}

static void BiFlushDrawLine(BITWOPOINT *p,BICOLOR *c)
{
#ifndef BIACCEL3D
	BIPOINT q[2];
	BIPOINTS s[2];

	BiSetOrthoPers(&p->p[0]);
	BiSetOrthoPers(&p->p[1]);

	if(p->clip!=BI_ON)
	{
		BiProject(&s[0],&p->p[0],&BiPrj);
		BiProject(&s[1],&p->p[1],&BiPrj);
		BiDrawLine2D(&s[0],&s[1],c);
	}
	else
	{
		BiNearClipLine(q,p->p,BiPrj.nearz);
		BiProject(&s[0],&q[0],&BiPrj);
		BiProject(&s[1],&q[1],&BiPrj);
		BiDrawLine2D(&s[0],&s[1],c);
	}
#endif

	if(BiEpsMode==BI_ON)
	{
		BiSetOrthoPers(&p->p[0]);
		BiSetOrthoPers(&p->p[1]);
		BiEpsDrawLine3(&p->p[0],&p->p[1],c);
	}
}

void BiOvwLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	int clip;
	BIPOINT q[2];

	BiConvLtoG(&q[0],p1,&BiCnvAxs);
	BiConvLtoG(&q[1],p2,&BiCnvAxs);

	BiSetOrthoPers(&q[0]);
	BiSetOrthoPers(&q[1]);

	if(BiVrmlMode==BI_ON)
	{
		BiVrmlLine(&q[0],&q[1],col);
	}

	if(BiCheckInsideViewPort(&clip,2,q)==BI_IN)
	{
#ifndef BIACCEL3D
		BIPOINT r[2],*s;
		BIPOINTS sc[2];

		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkOvwLine(&q[0],&q[1],col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawLine3(&q[0],&q[1],col);
		}

		if(clip==BI_ON)
		{
			BiNearClipLine(r,q,BiPrj.nearz);
			s=r;
		}
		else
		{
			s=q;
		}
		BiProject(&sc[0],&s[0],&BiPrj);
		BiProject(&sc[1],&s[1],&BiPrj);
		BiDrawLine2D(&sc[0],&sc[1],col);
#else
		BiOverWriteLine3(p1,p2,col);

		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkOvwLine(&q[0],&q[1],col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawLine3(&q[0],&q[1],col);
		}
#endif
	}
}

int BiOvwLineShadow(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	BIAXISF pln;
	BIPOINT v1,v2;


	/* Generate Shadow Projection Plane */
	BiConvAxisGtoLF(&pln,&BiMdlAxs,&BiShadowAxs);


	/* Generate Vertex */
	BiConvLtoG(&v1,p1,&pln);
	v1.y=0.0F;
	BiConvGtoL(&v1,&v1,&pln);

	BiConvLtoG(&v2,p2,&pln);
	v2.y=0.0F;
	BiConvGtoL(&v2,&v2,&pln);


	/* Draw Shadow */
	BiOvwLine(&v1,&v2,col);


	return BI3DG_STACKOVERFLOW;
}

/***********************************************************************/

int BiInsPset(BIPOINT *p,BICOLOR *col)
{
	int clip;
	BIOBJECT *neo;
	BIPOINT tmp;

	BiConvLtoG(&tmp,p,&BiCnvAxs);

	if(BiVrmlMode==BI_ON)
	{
		BiVrmlPset(&tmp,col);
	}

	if(BiCheckInsideViewPort(&clip,1,&tmp)==BI_IN)
	{
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkPset(&tmp,col);
		}

#ifndef BIACCEL3D
		neo=BiNew3dObject(BIOBJ_PSET,tmp.z);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		neo->col=*col;
		neo->attr.onePnt.p=tmp;
#else
		BiDrawPset3(p,col);

		if(BiEpsMode==BI_ON)
		{
			neo=BiNew3dObject(BIOBJ_PSET,tmp.z);
			if(neo==NULL)
			{
				return BI3DG_STACKOVERFLOW;
			}
			neo->col=*col;
			neo->attr.onePnt.p=tmp;
		}
#endif
	}
	return BI3DG_NOERR;
}

static void BiFlushDrawPset(BIONEPOINT *p,BICOLOR *c)
{
#ifndef BIACCEL3D
	BIPOINTS s;

	BiSetOrthoPers(&p->p);

	BiProject(&s,&p->p,&BiPrj);
	BiDrawPset2D(&s,c);
#endif

	if(BiEpsMode==BI_ON)
	{
		BiSetOrthoPers(&p->p);
		BiEpsDrawPset3(&p->p,c);
	}
}

void BiOvwPset(BIPOINT *p,BICOLOR *col)
{
	int clip;
	BIPOINT q;

	BiConvLtoG(&q,p,&BiCnvAxs);

	BiSetOrthoPers(&q);

	if(BiVrmlMode==BI_ON)
	{
		BiVrmlPset(&q,col);
	}

	if(BiCheckInsideViewPort(&clip,1,&q)==BI_IN)
	{
#ifndef BIACCEL3D
		BIPOINTS sc;

		BiProject(&sc,&q,&BiPrj);
		BiDrawPset2D(&sc,col);
#else
		BiOverWritePset3(p,col);
#endif
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkOvwPset(&q,col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawPset3(&q,col);
		}
	}
}

/***********************************************************************/

int BiInsCircle(BIPOINT *p,real r,BICOLOR *col)
{
	int clip;
	BIOBJECT *neo;
	BIPOINT tmp;

	BiConvLtoG(&tmp,p,&BiCnvAxs);
	if(BiCheckInsideViewPort(&clip,1,&tmp)==BI_IN)
	{
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkCircle(&tmp,r,col);
		}

#ifndef BIACCEL3D
		neo=BiNew3dObject(BIOBJ_CIRCLE,tmp.z);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		neo->col=*col;
		neo->attr.circ.p=tmp;
		neo->attr.circ.r=r;
#else
		BiDrawCircle3(p,r,r,col);

		if(BiEpsMode==BI_ON)
		{
			neo=BiNew3dObject(BIOBJ_CIRCLE,tmp.z);
			if(neo==NULL)
			{
				return BI3DG_STACKOVERFLOW;
			}
			neo->col=*col;
			neo->attr.circ.p=tmp;
			neo->attr.circ.r=r;
		}
#endif
	}
	return BI3DG_NOERR;
}

static void BiFlushDrawCircle(BICIRCLE *circ,BICOLOR *c)
{
#ifndef BIACCEL3D
	long sradx,srady;
	BIPOINTS s;

	BiSetOrthoPers(&circ->p);

	BiProject(&s,&circ->p,&BiPrj);
	sradx=(long)(circ->r*BiPrj.magx/circ->p.z);
	srady=(long)(circ->r*BiPrj.magy/circ->p.z);
	BiDrawCircle2D(&s,sradx,srady,c);
#endif

	if(BiEpsMode==BI_ON)
	{
		BiSetOrthoPers(&circ->p);
		BiEpsDrawCircle3(&circ->p,circ->r,c);
	}
}

/***********************************************************************/

int BiInsRoundPolyg(int np,BIPOINT p[],BIPOINT n[],BICOLOR *col)
{
	int i,clip;
	unsigned long ptr;
	BIPOINT *lp,*ln,cen,bbox[2];
	BIOBJECT *neo;

	lp=(BIPOINT *)BiPushTmpStack(&ptr,sizeof(BIPOINT)*np*2);
	if(lp!=NULL)
	{
		ln=lp+np;
		BiMakeLocal(lp,&cen,bbox,np,p);

		if(BiVrmlMode==BI_ON)
		{
			for(i=0; i<np; i++)
			{
				BiRotFastLtoG(&ln[i],&n[i],&BiCnvAxs.t);
			}
			BiVrmlRoundPolygon(np,lp,ln,col);
		}

		if(BiBboxInsideViewPort(&clip,bbox)==BI_IN)
		{
#ifndef BIACCEL3D
			for(i=0; i<np; i++)
			{
				BiRotFastLtoG(&ln[i],&n[i],&BiCnvAxs.t);
			}

			if(BiRendLinkFlag==BI_ON)
			{
				BiRenderDrawRoundPolygonTwoSide(np,lp,ln,col);
			}

			neo=BiNew3dObject(BIOBJ_RPOLYGON,cen.z);
			if(neo!=NULL)
			{
				neo->col=*col;
				neo->attr.nPntNom.clip=clip;
				neo->attr.nPntNom.np=np;
				neo->attr.nPntNom.cen=cen;
				neo->attr.nPntNom.p=lp;
				neo->attr.nPntNom.n=ln;
				return BI3DG_NOERR;
			}
			return BI3DG_STACKOVERFLOW;
#else
			for(i=0; i<np; i++)
			{
				BiQuickNormalize(&ln[i],&n[i]);
			}
			BiDrawPolygGrad3(np,p,ln,col);

			if(BiRendLinkFlag==BI_ON || BiEpsMode==BI_ON)
			{
				for(i=0; i<np; i++)
				{
					BiRotFastLtoG(&ln[i],&n[i],&BiCnvAxs.t);
				}

				if(BiRendLinkFlag==BI_ON)
				{
					BiRenderDrawRoundPolygonTwoSide(np,lp,ln,col);
				}
				if(BiEpsMode==BI_ON)
				{
					neo=BiNew3dObject(BIOBJ_RPOLYGON,cen.z);
					if(neo!=NULL)
					{
						neo->col=*col;
						neo->attr.nPntNom.clip=clip;
						neo->attr.nPntNom.np=np;
						neo->attr.nPntNom.cen=cen;
						neo->attr.nPntNom.p=lp;
						neo->attr.nPntNom.n=ln;
						return BI3DG_NOERR;
					}
					return BI3DG_STACKOVERFLOW;
				}
			}
#endif
		}
		BiPopTmpStack(ptr);
		return BI3DG_NOERR;
	}
	return BI3DG_STACKOVERFLOW;
}

static void BiFlushDrawRpolyg(BINPNTNOM *plg,BICOLOR *c)
{
	int i;
	BICOLOR shad;

#ifndef BIACCEL3D
	unsigned long ptr;
	BIPOINT *q;
	BIPOINTS *s;
	int nq;

	if(BiShadMode==BI_ON)
	{
		BiShadColor(&shad,c,&plg->cen,&plg->n[0]);
	}
	else
	{
		shad=*c;
	}

	if(plg->clip!=BI_ON || BiPrjMode==BIPRJ_ORTHO)
	{
		s=(BIPOINTS *)BiPushTmpStack(&ptr,sizeof(BIPOINTS)*plg->np);
		if(s!=NULL)
		{
			for(i=0; i<plg->np; i++)
			{
				BiSetOrthoPers(&plg->p[i]);
			}

			BiPlProject(plg->np,s,plg->p,&BiPrj);
			BiDrawPolyg2D(plg->np,s,&shad);
			BiPopTmpStack(ptr);
		}
	}
	else
	{
		q=(BIPOINT *)BiPushTmpStack(&ptr,sizeof(BIPOINT)*plg->np*2);
		s=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*plg->np*2);
		if(q!=NULL && s!=NULL)
		{
			BiNearClipPolyg(&nq,q,plg->np,plg->p,BiPrj.nearz);
			BiPlProject(nq,s,q,&BiPrj);
			BiDrawPolyg2D(nq,s,&shad);
			BiPopTmpStack(ptr);
		}
	}
#endif

	if(BiEpsMode==BI_ON)
	{
		for(i=0; i<plg->np; i++)
		{
			BiSetOrthoPers(&plg->p[i]);
		}

		if(BiShadMode==BI_ON)
		{
			BiShadColor(&shad,c,&plg->cen,&plg->n[0]);
			BiEpsDrawPolyg3(plg->np,plg->p,&shad);
		}
		else
		{
			BiEpsDrawPolyg3(plg->np,plg->p,c);
		}
	}
}

/***********************************************************************/

int BiInsPolygFast(int np,BIPOINT p[],BIPOINT *c,BIPOINT *n,BICOLOR *col)
{
	int clip;
	unsigned long ptr;
	BIPOINT *loc,cen,bbox[2],nom;
	BIOBJECT *neo;

	/** BackFaceRemove **/
	if(c!=NULL)
	{
		cen=*c;
	}
	else
	{
		int i;
		cen=p[0];
		for(i=1; i<np; i++)
		{
			BiAddPoint(&cen,&cen,&p[i]);
		}
		BiDivPoint(&cen,&cen,np);
	}

	BiConvLtoG(&cen,&cen,&BiCnvAxs);
	BiRotFastLtoG(&nom,n,&BiCnvAxs.t);

	if(BiVrmlMode==BI_OFF &&
	   ((BiPrjMode==BIPRJ_PERS && BiInnerPoint(&cen,&nom)>0) ||
	    (BiPrjMode==BIPRJ_ORTHO && nom.z>0)))
	{
		return BI3DG_NOERR;
	}
	/********************/

	if((loc=(BIPOINT *)BiPushTmpStack(&ptr,sizeof(BIPOINT)*np))!=NULL)
	{
		BiMakeLocalFast(loc,bbox,np,p);

		if(BiVrmlMode==BI_ON)
		{
			BiVrmlOneSidedPolygon(np,loc,&nom,col);
		}

		if(BiBboxInsideViewPort(&clip,bbox)==BI_IN)
		{
			if(BiRendLinkFlag==BI_ON)
			{
				BiRenderLinkPolygonOneSide(np,loc,&nom,col);
			}

#ifndef BIACCEL3D
			neo=BiNew3dObject(BIOBJ_POLYGON,cen.z);
			if(neo==NULL)
			{
				BiPopTmpStack(ptr);
				return BI3DG_STACKOVERFLOW;
			}
			neo->col=*col;
			neo->attr.nPnt.clip=clip;
			neo->attr.nPnt.cen=cen;
			neo->attr.nPnt.n=np;
			neo->attr.nPnt.p=loc;
			neo->attr.nPnt.nomSw=BI_ON;
			neo->attr.nPnt.nom=nom;
#else
			BiDrawPolyg3(np,p,n,col);

			if(BiEpsMode==BI_ON)
			{
				neo=BiNew3dObject(BIOBJ_POLYGON,cen.z);
				if(neo==NULL)
				{
					BiPopTmpStack(ptr);
					return BI3DG_STACKOVERFLOW;
				}
				neo->col=*col;
				neo->attr.nPnt.clip=clip;
				neo->attr.nPnt.cen=cen;
				neo->attr.nPnt.n=np;
				neo->attr.nPnt.p=loc;
				neo->attr.nPnt.nomSw=BI_ON;
				neo->attr.nPnt.nom=nom;
			}
			else
			{
				BiPopTmpStack(ptr); // In this case, no need to preserve transformed points.
			}
#endif
			return BI3DG_NOERR;
		}
		BiPopTmpStack(ptr);
		return BI3DG_NOERR;
	}
	return BI3DG_STACKOVERFLOW;
}

/***********************************************************************/

int BiInsPolyg(int np,BIPOINT p[],BICOLOR *col)
{
	int clip;
	unsigned long ptr;
	BIPOINT *loc,cen,bbox[2],nom;
	BIOBJECT *neo;

	loc=(BIPOINT *)BiPushTmpStack(&ptr,sizeof(BIPOINT)*np);
	if(loc!=NULL)
	{
		BiMakeLocal(loc,&cen,bbox,np,p);

		if(BiVrmlMode==BI_ON)
		{
			BiVrmlPolygon(np,loc,col);
		}

		if(BiBboxInsideViewPort(&clip,bbox)==BI_IN)
		{
#ifndef BIACCEL3D
			neo=BiNew3dObject(BIOBJ_POLYGON,cen.z);
			if(neo==NULL)
			{
				BiPopTmpStack(ptr);
				return BI3DG_STACKOVERFLOW;
			}

			neo->col=*col;
			neo->attr.nPnt.clip=clip;
			neo->attr.nPnt.cen=cen;
			neo->attr.nPnt.n=np;
			neo->attr.nPnt.p=loc;

			if(BiShadMode==BI_OFF)
			{
				if(BiRendLinkFlag==BI_ON)
				{
					BiRenderLinkPolygonTwoSide(np,loc,col);
				}
				neo->attr.nPnt.nomSw=BI_OFF;
			}
			else
			{
				BiAverageNormalVector(&nom,np,loc);
				if(BiInnerPoint(&nom,&loc[0])>0)
				{
					BiSubPoint(&nom,&BiOrgP,&nom);
				}
				if(BiRendLinkFlag==BI_ON)
				{
					BiRenderLinkPolygonOneSide(np,loc,&nom,col);
				}
				neo->attr.nPnt.nomSw=BI_ON;
				neo->attr.nPnt.nom=nom;
			}
#else
			BIPOINT lNom,lVtx;

			BiAverageNormalVector(&nom,np,p);
			BiConvLtoG(&lVtx,&p[0],&BiCnvAxs);
			BiRotFastLtoG(&lNom,&nom,&BiCnvAxs.t);
			if(BiInnerPoint(&lVtx,&lNom)>0)
			{
				BiSubPoint(&nom,&BiOrgP,&nom);
			}
			BiDrawPolyg3(np,p,&nom,col);

			if(BiRendLinkFlag==BI_ON || BiEpsMode==BI_ON)
			{
				BiAverageNormalVector(&nom,np,loc);
				if(BiInnerPoint(&nom,&loc[0])>0)
				{
					BiSubPoint(&nom,&BiOrgP,&nom);
				}

				if(BiEpsMode==BI_ON)
				{
					neo=BiNew3dObject(BIOBJ_POLYGON,cen.z);
					if(neo==NULL)
					{
						BiPopTmpStack(ptr);
						return BI3DG_STACKOVERFLOW;
					}

					neo->col=*col;
					neo->attr.nPnt.clip=clip;
					neo->attr.nPnt.cen=cen;
					neo->attr.nPnt.n=np;
					neo->attr.nPnt.p=loc;
					neo->attr.nPnt.nomSw=BiShadMode;
					neo->attr.nPnt.nom=nom;
				}

				if(BiRendLinkFlag==BI_ON)
				{
					if(BiShadMode==BI_OFF)
					{
						BiRenderLinkPolygonTwoSide(np,loc,col);
					}
					else
					{
						BiRenderLinkPolygonOneSide(np,loc,&nom,col);
					}
				}
			}

			if(BiEpsMode!=BI_ON)
			{
				BiPopTmpStack(ptr); // no need to preserve
			}
#endif
			return BI3DG_NOERR;
		}
		BiPopTmpStack(ptr);
		return BI3DG_NOERR;
	}
	return BI3DG_STACKOVERFLOW;
}

static void BiFlushDrawPolyg(BINPOINT *plg,BICOLOR *c)
{
	int i;
	BICOLOR shad;

#ifndef BIACCEL3D
	unsigned long ptr;
	int nq;
	BIPOINT *q;
	BIPOINTS *s;

	if(BiShadMode==BI_ON && plg->nomSw==BI_ON)
	{
		BiShadColor(&shad,c,&plg->cen,&plg->nom);
	}
	else
	{
		shad=*c;
	}

	if(plg->clip!=BI_ON || BiPrjMode==BIPRJ_ORTHO)
	{
		s=(BIPOINTS *)BiPushTmpStack(&ptr,sizeof(BIPOINTS)*plg->n);
		if(s!=NULL)
		{
			for(i=0; i<plg->n; i++)
			{
				BiSetOrthoPers(&plg->p[i]);
			}

			BiPlProject(plg->n,s,plg->p,&BiPrj);
			BiDrawPolyg2D(plg->n,s,&shad);
			BiPopTmpStack(ptr);
		}
	}
	else
	{
		q=(BIPOINT *)BiPushTmpStack(&ptr,sizeof(BIPOINT)*plg->n*2);
		s=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*plg->n*2);
		if(q!=NULL && s!=NULL)
		{
			BiNearClipPolyg(&nq,q,plg->n,plg->p,BiPrj.nearz);
			BiPlProject(nq,s,q,&BiPrj);
			BiDrawPolyg2D(nq,s,&shad);
			BiPopTmpStack(ptr);
		}
	}
#endif

	if(BiEpsMode==BI_ON)
	{
		for(i=0; i<plg->n; i++)
		{
			BiSetOrthoPers(&plg->p[i]);
		}

		if(BiShadMode==BI_ON && plg->nomSw==BI_ON)
		{
			BiShadColor(&shad,c,&plg->cen,&plg->nom);
			BiEpsDrawPolyg3(plg->n,plg->p,&shad);
		}
		else
		{
			BiEpsDrawPolyg3(plg->n,plg->p,c);
		}
	}
}


////////////////////////////////////////////////////////////




/*///////////////////////////////////////////////////////////
>> The following extension is exclusively for YSFLIGHT */

/* BiGetBufferForInsPolygNoTfm,BiInsPolygNoTfm, BiInsPolygFastNoTfm
   Coordinates must be already transformed to the viewing coordinates.
   Bounding box check is skipped.
   Loc must be obtained in ysdnmbi.cpp with BiGetBufferForInsPolygNoTfm.
   Does nothing if BIACCEL3D is defined.
   No VRML output.
   No EPS output.
   No BMP output.  */

BIPOINT *BiGetBufferForInsPolygNoTfm(unsigned long *stk,int np)
{
	return (BIPOINT *)BiPushTmpStack(stk,sizeof(BIPOINT)*np);
}

int BiInsPolygNoTfm(int np,BIPOINT loc[],BICOLOR *col,unsigned long discardTmpStkPtr)
{
	int clip;
	BIPOINT cen,nom;
	BIOBJECT *neo;

	int i;
	BiSetPoint(&cen,0,0,0);
	clip=BI_OFF;
	for(i=0; i<np; i++)
	{
		BiAddPoint(&cen,&cen,&loc[i]);
		if(loc[i].z<BiPrj.nearz)
		{
			clip=BI_ON;
		}
	}
	BiDivPoint(&cen,&cen,(double)np);

#ifndef BIACCEL3D
	neo=BiNew3dObject(BIOBJ_POLYGON,cen.z);
	if(neo==NULL)
	{
		BiPopTmpStack(discardTmpStkPtr);
		return BI3DG_STACKOVERFLOW;
	}

	neo->col=*col;
	neo->attr.nPnt.clip=clip;
	neo->attr.nPnt.cen=cen;
	neo->attr.nPnt.n=np;
	neo->attr.nPnt.p=loc;

	if(BiShadMode==BI_OFF)
	{
		neo->attr.nPnt.nomSw=BI_OFF;
	}
	else
	{
		BiAverageNormalVector(&nom,np,loc);
		if(BiInnerPoint(&nom,&loc[0])>0)
		{
			BiSubPoint(&nom,&BiOrgP,&nom);
		}
		neo->attr.nPnt.nomSw=BI_ON;
		neo->attr.nPnt.nom=nom;
	}
#else
	/* not supposed to come here */
#endif
	return BI3DG_NOERR;
}

int BiInsPolygFastNoTfm(int np,BIPOINT loc[],BIPOINT *c,BIPOINT *n,BICOLOR *col,unsigned long discardTmpStkPtr)
{
	int clip;
	BIPOINT cen,nom;
	BIOBJECT *neo;
	int i;

	/** BackFaceRemove **/
	if(c!=NULL)
	{
		cen=*c;
	}
	else
	{
		int i;
		cen=loc[0];
		for(i=1; i<np; i++)
		{
			BiAddPoint(&cen,&cen,&loc[i]);
		}
		BiDivPoint(&cen,&cen,np);
	}

	nom=*n;

	if((BiPrjMode==BIPRJ_PERS && BiInnerPoint(&cen,&nom)>0) ||
	   (BiPrjMode==BIPRJ_ORTHO && nom.z>0))
	{
		BiPopTmpStack(discardTmpStkPtr);
		return BI3DG_NOERR;
	}
	/********************/

	clip=BI_OFF;
	for(i=0; i<np; i++)
	{
		if(loc[i].z<BiPrj.nearz)
		{
			clip=BI_ON;
		}
	}

#ifndef BIACCEL3D
	neo=BiNew3dObject(BIOBJ_POLYGON,cen.z);
	if(neo==NULL)
	{
		BiPopTmpStack(discardTmpStkPtr);
		return BI3DG_STACKOVERFLOW;
	}
	neo->col=*col;
	neo->attr.nPnt.clip=clip;
	neo->attr.nPnt.cen=cen;
	neo->attr.nPnt.n=np;
	neo->attr.nPnt.p=loc;
	neo->attr.nPnt.nomSw=BI_ON;
	neo->attr.nPnt.nom=nom;
#else
	/* not supposed to come here */
#endif
	return BI3DG_NOERR;
}
/* << The above extension is for exclusively YSFLIGHT not to be used by other applications.
////////////////////////////////////////////////////////////*/

void BiOvwPolyg(int n,BIPOINT *p,BICOLOR *col)
{
	unsigned long ptr;
	int i,clip;
	BIPOINT *loc,*clp;
	BIPOINTS *sc;

	loc=BiPushTmpStack(&ptr,sizeof(BIPOINT)*n*3);
	clp=loc+n;
	sc=BiGetTmpStack(sizeof(BIPOINTS)*n*2);
	if(loc==NULL || sc==NULL)
	{
		goto END;
	}

	for(i=0; i<n; i++)
	{
		BiConvLtoG(&loc[i],&p[i],&BiCnvAxs);
		BiSetOrthoPers(&loc[i]);
	}

	if(BiVrmlMode==BI_ON)
	{
		BiVrmlPolygon(n,loc,col);
	}

	if(BiCheckInsideViewPort(&clip,n,loc)==BI_IN)
	{
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkOvwPolygon(n,loc,col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawPolyg3(n,loc,col);
		}

#ifndef BIACCEL3D
		if(clip==BI_ON)
		{
			int nclp;
			BiNearClipPolyg(&nclp,clp,n,loc,BiPrj.nearz);
			BiPlProject(nclp,sc,clp,&BiPrj);
			BiDrawPolyg2D(nclp,sc,col);
		}
		else
		{
			BiPlProject(n,sc,loc,&BiPrj);
			BiDrawPolyg2D(n,sc,col);
		}
#else
		BiOverWritePolyg3(n,p,col);
#endif

	}
END:
	BiPopTmpStack(ptr);
}

static void BiMakeLocal(BIPOINT *d,BIPOINT *cen,BIPOINT *box,int n,BIPOINT *s)
{
	int i;

	BiConvLtoG(&d[0],&s[0],&BiCnvAxs);
	*cen=d[0];
	box[0]=d[0];
	box[1]=d[0];
	for(i=1; i<n; i++)
	{
		BiConvLtoG(&d[i],&s[i],&BiCnvAxs);
		BiAddPoint(cen,cen,&d[i]);
		box[0].x=BiSmaller(box[0].x,d[i].x);
		box[0].y=BiSmaller(box[0].y,d[i].y);
		box[0].z=BiSmaller(box[0].z,d[i].z);
		box[1].x=BiLarger(box[1].x,d[i].x);
		box[1].y=BiLarger(box[1].y,d[i].y);
		box[1].z=BiLarger(box[1].z,d[i].z);
	}
	BiDivPoint(cen,cen,(real)n);
}

static void BiMakeLocalFast(BIPOINT *d,BIPOINT *box,int n,BIPOINT *s)
{
	int i;

	BiConvLtoG(&d[0],&s[0],&BiCnvAxs);
	box[0]=d[0];
	box[1]=d[0];
	for(i=1; i<n; i++)
	{
		BiConvLtoG(&d[i],&s[i],&BiCnvAxs);
		box[0].x=BiSmaller(box[0].x,d[i].x);
		box[0].y=BiSmaller(box[0].y,d[i].y);
		box[0].z=BiSmaller(box[0].z,d[i].z);
		box[1].x=BiLarger(box[1].x,d[i].x);
		box[1].y=BiLarger(box[1].y,d[i].y);
		box[1].z=BiLarger(box[1].z,d[i].z);
	}
}

int BiOvwPolygShadow(int np,BIPOINT p[],BICOLOR *col)
{
	unsigned long stk;
	int i;
	BIAXISF pln;
	BIPOINT *vtx;


	/* Generate Shadow Projection Plane */
	BiConvAxisGtoLF(&pln,&BiMdlAxs,&BiShadowAxs);


	/* Generate Vertex */
	vtx=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*np);
	if(vtx!=NULL)
	{
		for(i=0; i<np; i++)
		{
			BiConvLtoG(&vtx[i],&p[i],&pln);
			vtx[i].y=0.0F;
			BiConvGtoL(&vtx[i],&vtx[i],&pln);
		}


		/* Draw Shadow */
		BiOvwPolyg(np,vtx,col);


		BiPopTmpStack(stk);
		return BI3DG_NOERR;
	}
	return BI3DG_STACKOVERFLOW;
}


/***********************************************************************/
/*pub  int BiInsBitmap  :  type BIBITMAP is not fixed. */
/***********************************************************************/

int BiInsMarker(BIPOINT *p,BICOLOR *col,int mkType)
{
	int clip;
	BIOBJECT *neo;
	BIPOINT tmp;

	BiConvLtoG(&tmp,p,&BiCnvAxs);
	if(BiCheckInsideViewPort(&clip,1,&tmp)==BI_IN)
	{
#ifdef BIACCEL3D
		BiDrawMarker3(p,col,mkType);
		if(BiEpsMode==BI_OFF)
		{
			return BI3DG_NOERR;
		}
#endif
		neo=BiNew3dObject(BIOBJ_MARKER,tmp.z);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		neo->col=*col;
		neo->attr.mark.p=tmp;
		neo->attr.mark.mkType=mkType;
	}
	return BI3DG_NOERR;
}

static void BiFlushDrawMarker(BIMARKER *mk,BICOLOR *c)
{
#ifndef BIACCEL3D
	BIPOINTS s;
	BiSetOrthoPers(&mk->p);
	BiProject(&s,&mk->p,&BiPrj);
	BiDrawMarker2D(&s,mk->mkType,c);
#endif

	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawMarker3(&mk->p,mk->mkType,c);
	}
}

void BiOvwMarker(BIPOINT *p,BICOLOR *col,int mkType)
{
	int clip;
	BIPOINT q;

	BiConvLtoG(&q,p,&BiCnvAxs);

	BiSetOrthoPers(&q);

	if(BiCheckInsideViewPort(&clip,1,&q)==BI_IN)
	{
#ifndef BIACCEL3D
		BIPOINTS sc;
		BiProject(&sc,&q,&BiPrj);
		BiDrawMarker2D(&sc,mkType,col);
#else
		BiOverWriteMarker3(p,col,mkType);
#endif
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawMarker3(&q,mkType,col);
		}
	}
}

/***********************************************************************/

int BiInsString(BIPOINT *p,char *str,BICOLOR *col,int hori,int vert)
{
	char *strSeq[2];
	strSeq[0]=str;
	strSeq[1]=NULL;
	BiInsText(p,strSeq,col,hori,vert);
	return BI3DG_NOERR;
}

int BiOvwString(BIPOINT *p,char *str,BICOLOR *col,int hori,int vert)
{
	char *strSeq[2];
	strSeq[0]=str;
	strSeq[1]=NULL;
	BiOvwText(p,strSeq,col,hori,vert);
	return BI3DG_NOERR;
}

/***********************************************************************/

int BiInsText(BIPOINT *p,char *str[],BICOLOR *col,int hori,int vert)
{
	int clip,siz,i;
	BIOBJECT *neo;
	BIPOINT tmp;

	BiConvLtoG(&tmp,p,&BiCnvAxs);
	if(BiCheckInsideViewPort(&clip,1,&tmp)==BI_IN)
	{
#ifdef BIACCEL3D
		BiDrawText3(p,str,hori,vert,col);
		if(BiEpsMode==BI_OFF)
		{
			return BI3DG_NOERR;
		}
#endif

		neo=BiNew3dObject(BIOBJ_TEXT,tmp.z);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}

		neo->col=*col;
		neo->attr.text.p=tmp;
		neo->attr.text.hori=hori;
		neo->attr.text.vert=vert;

		for(i=0; str[i]!=NULL; i++);
		neo->attr.text.nLine=i;
		neo->attr.text.str=(char *(*))BiGetTmpStack(sizeof(char *)*(i+1));
		if(neo->attr.text.str==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}

		for(i=0; str[i]!=NULL; i++)
		{
			siz=strlen(str[i])+1;
			neo->attr.text.str[i]=(char *)BiGetTmpStack(sizeof(char)*siz);
			if(neo->attr.text.str[i]==NULL)
			{
				return BI3DG_STACKOVERFLOW;
			}
			strcpy(neo->attr.text.str[i],str[i]);
		}
		neo->attr.text.str[i]=NULL;
	}
	return BI3DG_NOERR;
}

static void BiFlushDrawText(BITEXT *tx,BICOLOR *c)
{
#ifndef BIACCEL3D
	BIPOINTS s;

	BiSetOrthoPers(&tx->p);
	BiProject(&s,&tx->p,&BiPrj);
	BiDrawText2D(&s,tx->str,tx->hori,tx->vert,c);
#endif

	if(BiEpsMode==BI_ON)
	{
		BiSetOrthoPers(&tx->p);
		BiEpsDrawText3(&tx->p,tx->str,tx->hori,tx->vert,c);
	}
}

int BiOvwText(BIPOINT *p,char *str[],BICOLOR *col,int hori,int vert)
{
	int clip;
	BIPOINT tmp;

	BiConvLtoG(&tmp,p,&BiCnvAxs);
	if(BiCheckInsideViewPort(&clip,1,&tmp)==BI_IN)
	{
#ifndef BIACCEL3D
		BIPOINTS s;
		BiSetOrthoPers(&tmp);
		BiProject(&s,&tmp,&BiPrj);
		BiDrawText2D(&s,str,hori,vert,col);
#else
		BiOverWriteText3(p,str,hori,vert,col);
#endif

		if(BiEpsMode==BI_ON)
		{
			BiSetOrthoPers(&tmp);
			BiEpsDrawText3(&tmp,str,hori,vert,col);
		}
	}
	return BI3DG_NOERR;
}

/***********************************************************************/

static void BiGSFillScreen(BICOLOR *col);
static void BiGSPartScreen(BICOLOR *grd,BICOLOR *sky);
static void BiGSGetTwoPoint(BIPOINTS l[2]);
static void BiGSGetArea(int *np,BIPOINTS *pnt,BIPOINTS *right,BIPOINTS *left);

extern BIPROJ BiPrj;

void BiDrawGroundSky(BICOLOR *grd,BICOLOR *sky)
{
	if(BiEyeAxs.a.p>14563)
	{
		BiGSFillScreen(sky);
	}
	else if(BiEyeAxs.a.p<-14563)
	{
		BiGSFillScreen(grd);
	}
	else
	{
		BiGSPartScreen(grd,sky);
	}
}

static void BiGSFillScreen(BICOLOR *col)
{
	BIPOINTS lup,rdw;
	BiSetPointS(&lup,0,0);
	BiSetPointS(&rdw,BiPrj.lx,BiPrj.ly);
	BiDrawRect2D(&lup,&rdw,col);

	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkRect2(&lup,&rdw,col);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawRect(&lup,&rdw,col);
	}
}

static void BiGSPartScreen(BICOLOR *grd,BICOLOR *sky)
{
	int np;
	BIPOINTS sLup,sRdw,lup,rdw,l[2],p[8];

	BiSetPointS(&lup,0,0);
	BiSetPointS(&rdw,BiPrj.lx,BiPrj.ly);
	BiGetClipScreenFrame(&sLup,&sRdw);
	BiSetClipScreenFrame(&lup,&rdw);

	BiGSGetTwoPoint(l);
	if(BiClipLineScrn(&l[0],&l[1],&l[0],&l[1])!=BI_IN)
	{
		if(BiEyeAxs.a.p>0)
		{
			BiGSFillScreen(sky);
		}
		else
		{
			BiGSFillScreen(grd);
		}
		goto EXIT;
	}
	BiGSGetArea(&np,p,&l[1],&l[0]);
	BiDrawPolyg2D(np,p,grd);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkPolygon2(np,p,grd);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPolygon(np,p,grd);
	}
	BiGSGetArea(&np,p,&l[0],&l[1]);
	BiDrawPolyg2D(np,p,sky);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkPolygon2(np,p,sky);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPolygon(np,p,sky);
	}

EXIT:
	BiSetClipScreenFrame(&sLup,&sRdw);
}

static void BiGSGetTwoPoint(BIPOINTS l[2])
{
	BIPOINT2 cenVec,leftVec,left,right;

	cenVec.x= 0.0F;
	cenVec.y=-BiPrj.magy*BiTan(BiEyeAxs.a.p);
	BiRot2F(&cenVec,&cenVec,-BiEyeAxs.a.b);
	cenVec.x = (real)BiPrj.cx+cenVec.x;
	cenVec.y = (real)BiPrj.cy-cenVec.y;

	BiSetPoint2(&leftVec,16384,0);
	BiRot2F(&leftVec,&leftVec,-BiEyeAxs.a.b);
	leftVec.y = -leftVec.y;

	BiSubPoint2(&left, &cenVec,&leftVec);
	BiAddPoint2(&right,&cenVec,&leftVec);

	BiSetPointS(&l[0],(long)left.x, (long)left.y);
	BiSetPointS(&l[1],(long)right.x,(long)right.y);
}

static void BiGSGetArea(int *np,BIPOINTS *pnt,BIPOINTS *right,BIPOINTS *left)
{
	int i;
	BIPOINTS tmp;
	BIPOINTS rc[2];

	BiGetClipScreenFrame(&rc[0],&rc[1]);
	tmp=*right;
	for(i=0; i<7; i++)
	{
		pnt[i]=tmp;

		if(tmp.y==rc[0].y && tmp.x<rc[1].x)            /* Go Right */
		{
			if(left->y==tmp.y && tmp.x<left->x)break;
			tmp.x=rc[1].x;
			tmp.y=rc[0].y;
		}
		else if(tmp.x==rc[1].x && tmp.y<rc[1].y)       /* Go Down */
		{
			if(left->x==tmp.x && tmp.y<left->y)break;
			tmp  =rc[1];
		}
		else if(tmp.y==rc[1].y && rc[0].x<tmp.x)       /* Go Left */
		{
			if(left->y==tmp.y && left->x<tmp.x)break;
			tmp.x=rc[0].x;
			tmp.y=rc[1].y;
		}
		else if(tmp.x==rc[0].x && rc[0].y<tmp.y)       /* Go Up */
		{
			if(left->x==tmp.x && left->y<tmp.y)break;
			tmp  =rc[0];
		}
	}
	i++;
	pnt[i]  =*left;
	*np=i+1;
}



/*****************************************************************************/

int BiTestTreeStructureLoop(BIOBJECT *tree,BIOBJECT *head)
{
	int n;

	if(tree==NULL)
	{
		return 0;
	}

	n=1+BiTestTreeStructureLoop(tree->treePrev,NULL)+BiTestTreeStructureLoop(tree->treeNext,NULL);

	if(head!=NULL)
	{
		int nTest;
		BIOBJECT *seek;
		nTest=0;
		for(seek=head; seek!=NULL; seek=seek->next)
		{
			nTest++;
		}
		if(nTest!=n)
		{
			printf("Linear list and tree structure conflict!  %d %d\n",n,nTest);
		}

		for(seek=head; seek!=NULL; seek=seek->next)
		{
			if(seek->type==BIOBJ_SUBTREE)
			{
				int n;
				n=BiTestTreeStructureLoop(seek->attr.subTree.subTree,seek->attr.subTree.subHead);
				printf("# of Nodes of Sub Tree=%d\n",n);
			}
		}
	}

	return n;
}

void BiTestTreeStructure(void)
{
	int n;
	printf("Testing Tree Structure.\n");
	n=BiTestTreeStructureLoop(BiGetCurrentTreeRoot(),BiGetCurrentTreeHead());
	printf("# of Root Tree Nodes=%d\n",n);
}

int BiFlushBufferLoop(BIOBJECT *seek)
{
	int nObj;
	static int depth=0;
	depth++;
	// printf("Depth=%d %08x\n",depth,seek);

	nObj=0;
	for(seek=seek; seek!=NULL; seek=seek->next)
	{
		switch(seek->type)
		{
		case BIOBJ_LINE:
			BiFlushDrawLine(&seek->attr.twoPnt,&seek->col);
			break;
		case BIOBJ_PSET:
			BiFlushDrawPset(&seek->attr.onePnt,&seek->col);
			break;
		case BIOBJ_CIRCLE:
			BiFlushDrawCircle(&seek->attr.circ,&seek->col);
			break;
		case BIOBJ_POLYGON:
			BiFlushDrawPolyg(&seek->attr.nPnt,&seek->col);
			break;
		case BIOBJ_MARKER:
			BiFlushDrawMarker(&seek->attr.mark,&seek->col);
			break;
		case BIOBJ_TEXT:
			BiFlushDrawText(&seek->attr.text,&seek->col);
			break;
		case BIOBJ_SRFMODEL:
			BiFlushDrawSrf(seek->attr.smdl.mdl,&seek->attr.smdl.axs,seek->attr.smdl.clip);
			break;
		case BIOBJ_PICT2:
			BiFlushDrawPc2(seek->attr.pc2.pc2,&seek->attr.pc2.axs,seek->attr.pc2.clip);
			break;
		case BIOBJ_RPOLYGON:
			BiFlushDrawRpolyg(&seek->attr.nPntNom,&seek->col);
			break;
		case BIOBJ_SUBTREE:
			nObj+=BiFlushBufferLoop(seek->attr.subTree.subHead);
			break;
		/* Check BiNew3DObject@i3dgobj.c whenever add new type! */
		}
		nObj++;
	}
	/* printf("%d 3d objs\n",nObj); */

	depth--;

	return nObj;
}

void BiFlushBuffer(void)
{
	int nObj;

#ifdef BIACCEL3D
	if(BiEpsMode!=BI_ON)
	{
		return;
	}
#endif

	nObj=BiFlushBufferLoop(BiTopObject());

	BiClear3dObject();  // 2003/05/30
}

void BiSetLightPosition(BIPOINT *p)
{
	BiLightPos=*p;

	BiConvGtoL(&BiRelLightPos,&BiLightPos,&BiEyeAxs);

	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderResetLightPosition();
	}
#ifdef BIACCEL3D
	BiAfterSetLightPosition(p);
#endif
}



static real amb=0.3F;
static real dif=0.8F;
static real spe=0.8F;

void BiShadColor(BICOLOR *dst,BICOLOR *src,BIPOINT *cen,BIPOINT *nom)
{
	long iDif,iSpe;
	real rDif,rSpe;
	BIVECTOR lit,eye,rnm,rhv;

	BiSubPoint(&lit,&BiRelLightPos,cen);
	BiQuickNormalize(&lit,&lit);
	BiQuickNormalize(&eye, cen);
	BiQuickNormalize(&rnm, nom);
	BiSubPoint(&rhv,&lit,&eye);
	BiQuickNormalize(&rhv,&rhv);

	rDif=BiLarger(0.0F,BiInnerPoint(&lit,&rnm));
	iDif=(long)((rDif+amb)*256);

	rSpe=(real)BiPow60(BiAbs(BiInnerPoint(&rnm,&rhv)));
	iSpe=(long)(rSpe*spe*256);

	dst->g=(long)src->g*iDif/256+iSpe;
	dst->r=(long)src->r*iDif/256+iSpe;
	dst->b=(long)src->b*iDif/256+iSpe;

	dst->g=BiSmaller(dst->g,255);
	dst->r=BiSmaller(dst->r,255);
	dst->b=BiSmaller(dst->b,255);

	/* Tiny Calc Version *************************************************
	dst->g=(src->g+(unsigned)((real)src->g*(nom->x+nom->y+nom->z+3.0)))/7;
	dst->r=(src->r+(unsigned)((real)src->r*(nom->x+nom->y+nom->z+3.0)))/7;
	dst->b=(src->b+(unsigned)((real)src->b*(nom->x+nom->y+nom->z+3.0)))/7;
	*********************************************************************/
}
