#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "impulse.h"
#include "i3dg.h"

#include "irender.h"
#include "ivrmfile.h"
#include "iutil.h"



extern void BiBeginTempHeap(void);
extern void BiEndTempHeap(void);
extern void *BiTempMalloc(unsigned long siz,size_t uni);
extern void *BiTempPushMalloc(long *prm,long *stk,unsigned long siz,size_t uni);
extern void BiTempPopMalloc(long prm,long stk);




typedef struct biPointList {
	struct biPointList *next;
	BISRFVERTEX p;
} BIVERTEXLIST;

typedef struct biFaceList {
	struct biFaceList *next;
	BISRFPOLYGON f;
} BIFACELIST;

typedef struct biIdList {
	struct biIdList *next;
	int id;
} BIIDLIST;


static void BiLoadSrfSetNormal(BISRF *mdl);
static void BiLoadSrfSetVertexNorm(BIPOINT *nom,BISRF *mdl,int vtId);
static void BiLoadSrfSetFaceNorm(BIPOINT *nom,int nVt,int *vt,BISRFVERTEX *v);
static void BiLoadSrfConstrainTwist(BISRF *mdl);
static void BiLoadSrfSortByColor(BISRF *mdl);
static void BiLoadSrfMakeRightTwist(BISRFPOLYGON *plg,BISRFVERTEX *v);
static int BiLoadSrfIdInclude(int id,int nId,int *idList);
static int BiLoadSrfMainLoop(BISRF *mdl);
static int BiSrfSetVtx(BISRF *mdl,int nVt,BIVERTEXLIST *vtx);
static int BiSrfSetFace(BISRF *mdl,int nFac,int nVtTab,BIFACELIST *fac);
static void BiMakeBoundingBox(BIPOINT *bbox,int nVt,BISRFVERTEX *vt);
static int BiSrfInsVtx(BIVERTEXLIST *(*vtx),int ac,char *av[]);
static int BiSrfInsPlg(BIFACELIST *(*fac));
static int BiSrfSetVtxId(BISRFPOLYGON *neo,int nVt,BIIDLIST *vt);
static int BiSrfInsId(BIIDLIST *(*lst),int id);
static int BiSrfInsAxs(int ac,char *av[]);




/* public */int BiLoadSrf(BISRF *mdl,char fnOrg[])
{
	FILE *fp;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp==NULL)
	{
		return BI_ERR;
	}


	BiPushStringFunc();
	BiSetFp(fp);
	BiInitGetStringFuncEx();

	BiBeginTempHeap();

	if(BiLoadSrfMainLoop(mdl)!=BI_OK)
	{
		BiEndTempHeap();
		BiPopStringFunc();
		fclose(fp);
		return BI_ERR;
	}
	BiLoadSrfSetNormal(mdl);
	BiLoadSrfConstrainTwist(mdl);
	BiLoadSrfSortByColor(mdl);
	fclose(fp);

	BiEndTempHeap();
	BiPopStringFunc();

	return BI_OK;
}

/* public */int BiLoadSrfFromString(BISRF *mdl,char *str[])
{

	BiPushStringFunc();
	BiSetStringList(str);
	BiInitGetStringFuncEx();

	BiBeginTempHeap();
	/* printf("BiLoadSrfFromString in\n"); */
	if(BiLoadSrfMainLoop(mdl)!=BI_OK)
	{
		printf("Err (Root)\n");

		BiEndTempHeap();
		BiPopStringFunc();
		return BI_ERR;
	}
	BiLoadSrfSetNormal(mdl);
	BiLoadSrfConstrainTwist(mdl);
	BiLoadSrfSortByColor(mdl);

	BiEndTempHeap();
	BiPopStringFunc();
	/* printf("BiLoadSrfFromString out\n"); */
	return BI_OK;
}



/* Experimental for GL and OpenGL Acceleration */
static void BiLoadSrfSortByColor(BISRF *mdl)
{
	int i,j;
	unsigned char r,g,b;
	BISRFPOLYGON *lst,buf;

	lst=mdl->p;
	for(i=0; i<mdl->np-1; i++)
	{
		r=lst[i].col.r;
		g=lst[i].col.g;
		b=lst[i].col.b;
		if(r!=lst[i+1].col.r || g!=lst[i+1].col.g || b!=lst[i+1].col.b)
		{
			for(j=mdl->np-1; j>i+1; j--)
			{
				if(r==lst[j].col.r && g==lst[j].col.g && b==lst[j].col.b)
				{
					buf     =lst[i+1];
					lst[i+1]=lst[j];
					lst[j]  =buf;
					break;
				}
			}
		}
	}
}


static void BiLoadSrfSetNormal(BISRF *mdl)
{
	int i;
	BISRFVERTEX *vtx;
	BISRFPOLYGON *plg;

	for(i=0; i<mdl->np; i++)
	{
		plg=&mdl->p[i];
		if(plg->backFaceRemove==BI_OFF)
		{
			BiLoadSrfSetFaceNorm(&plg->normal,plg->nVt,plg->vt,mdl->v);
		}
	}
	for(i=0; i<mdl->nv; i++)
	{
		vtx=&mdl->v[i];
		BiLoadSrfSetVertexNorm(&vtx->n,mdl,i);
	}
}

static void BiLoadSrfSetFaceNorm(BIPOINT *nom,int nVt,int *vt,BISRFVERTEX *v)
{
	int i;
	BIPOINT *tmp;
	long prm,stk;

	tmp=(BIPOINT *)malloc(sizeof(BIPOINT)*nVt); //tmp=(BIPOINT *)BiTempPushMalloc(&prm,&stk,nVt,sizeof(BIPOINT));
	for(i=0; i<nVt; i++)
	{
		tmp[i]=v[vt[i]].p;
	}
	BiAverageNormalVector(nom,nVt,tmp);
	BiNormalize(nom,nom);
	free(tmp); //BiTempPopMalloc(prm,stk);
}

static void BiLoadSrfSetVertexNorm(BIPOINT *nom,BISRF *mdl,int vtId)
{
	int i,j,n;
	BISRFPOLYGON *plg;
	BIVECTOR pNom,*lst;
	long prm,stk;

	*nom=BiOrgP;
	n=0;
	lst=(BIVECTOR *)malloc(sizeof(BIVECTOR)*mdl->np); //lst=(BIVECTOR *)BiTempPushMalloc(&prm,&stk,mdl->np,sizeof(BIVECTOR));

	if(lst!=NULL)
	{
		for(i=0; i<mdl->np; i++)
		{
			plg=&mdl->p[i];

			if(plg->bright!=BI_ON &&  // 2002/10/26 Anti-shadow
			   plg->backFaceRemove==BI_ON &&
			   BiLoadSrfIdInclude(vtId,plg->nVt,plg->vt)==BI_TRUE)
			{
				pNom=plg->normal;
				for(j=0; j<n; j++)
				{
					if(BiSamePoint(&lst[j],&pNom)==BI_TRUE ||
					   BiInnerPoint(&lst[j],&pNom)<0.0F)
					{
						break;
					}
				}
				if(j==n)
				{
					lst[n]=pNom;
					n++;
				}
			}
		}
		if(n>0)
		{
			for(i=0; i<n; i++)
			{
				BiAddPoint(nom,nom,&lst[i]);
			}
			BiNormalize(nom,nom);
		}
		free(lst); //BiTempPopMalloc(prm,stk);
	}
}

/* All Polygons should twist right. */
static void BiLoadSrfConstrainTwist(BISRF *mdl)
{
	int plId;
	for(plId=0; plId<mdl->np; plId++)
	{
		BiLoadSrfMakeRightTwist(&mdl->p[plId],mdl->v);
	}
}

static void BiLoadSrfMakeRightTwist(BISRFPOLYGON *plg,BISRFVERTEX *v)
{
	int i,sw1,sw2,vtId;
	long prm,stk;
	BIPOINT *tmp;

	if(plg->backFaceRemove==BI_OFF)
	{
		return;
	}

	tmp=(BIPOINT *)malloc(plg->nVt*sizeof(BIPOINT)); //tmp=(BIPOINT *)BiTempPushMalloc(&prm,&stk,plg->nVt,sizeof(BIPOINT));
	if(tmp==NULL)
	{
		return;
	}

	for(i=0; i<plg->nVt; i++)
	{
		vtId=plg->vt[i];
		tmp[i]=v[vtId].p;
	}

	if(BiTwist3(plg->nVt,tmp,&plg->normal)==BITWIST_RIGHT)
	{
		for(i=0; i<plg->nVt/2; i++)
		{
			sw1=plg->vt[i           ];
			sw2=plg->vt[plg->nVt-1-i];
			plg->vt[i           ]=sw2;
			plg->vt[plg->nVt-1-i]=sw1;
		}
	}

	free(tmp); //BiTempPopMalloc(prm,stk);
}

static int BiLoadSrfIdInclude(int id,int nId,int *idList)
{
	int i;
	for(i=0; i<nId; i++)
	{
		if(id==idList[i])
		{
			return BI_TRUE;
		}
	}
	return BI_FALSE;
}


static char *srfCmd[]=
{
	"BLK",
	"EBK",
	"R",
	"V",   /* VERTEX */
	"F",   /* FACE */
	"B",   /* BASEAXIS */
	"E",
	NULL
};

static int BiLoadSrfMainLoop(BISRF *mdl)
{
	int ac,cmd,er;
	char str[256],tmp[256],*av[128];
	int nVtx,nVtTabAll,nFac;
	BIVERTEXLIST *vtx;
	BIFACELIST *fac;

	BiGetStringFuncEx(str,255);
	BiCapitalizeString(str);
	if(strncmp(str,"SURF",4)!=0)
	{
		return BI_ERR;
	}

	nVtx=0;
	vtx=NULL;
	nFac=0;
	nVtTabAll=0;
	fac=NULL;
	while(BiGetStringFuncEx(str,255)!=NULL)
	{
		strcpy(tmp,str);

		BiArguments(&ac,av,127,tmp);
		if(ac==0 || str[0]=='#')
		{
			continue;
		}

		if(BiCommandNumber(&cmd,av[0],srfCmd)!=BI_OK)
		{
			printf("Err 1\n");
			return BI_ERR;
		}

		switch(cmd)
		{
		case 0:  /* "BLK" Ignored */
		case 1:  /* "EBK" Ignored */
		case 2:  /* "REM" */
			break;
		case 3:  /* "VERTEX" */
			er=BiSrfInsVtx(&vtx,ac,av);
			nVtx++;
			break;
		case 4:  /* "FACE" */
			er=BiSrfInsPlg(&fac);
			nFac++;
			nVtTabAll+=fac->f.nVt;
			break;
		case 5:  /* "BASEAXIS" */
			er=BiSrfInsAxs(ac,av);
			break;
		case 6:  /* "END" */
			if(BiSrfSetVtx(mdl,nVtx,vtx)!=BI_OK || BiSrfSetFace(mdl,nFac,nVtTabAll,fac)!=BI_OK)
			{
				printf("Err 2\n");
				return BI_ERR;
			}
			BiMakeBoundingBox(mdl->bbox,mdl->nv,mdl->v);
			return BI_OK;
		}

		if(er!=BI_OK)
		{
			printf("Err 3\n");
			return BI_ERR;
		}
	}

	/* "Warning : Missing END Directive\n" */
	return BI_OK;
}

static int BiSrfSetVtx(BISRF *mdl,int nVt,BIVERTEXLIST *vtx)
{
	int i;
	BISRFVERTEX *tab,*tmp;

	tab=(BISRFVERTEX *)BiMalloc(nVt*sizeof(BISRFVERTEX));
	tmp=(BISRFVERTEX *)BiMalloc(nVt*sizeof(BISRFVERTEX));
	if(tab!=NULL && tmp!=NULL)
	{
		for(i=nVt-1; i>=0; i--)
		{
			tab[i]=vtx->p;
			vtx=vtx->next;
		}
		mdl->nv=nVt;
		mdl->v=tab;
		mdl->tmp=tmp;
		return BI_OK;
	}
	printf("Err 4\n");
	return BI_ERR;
}

static int BiSrfSetFace(BISRF *mdl,int nFac,int nVtTab,BIFACELIST *fac)
{
	int i,j;
	BISRFPOLYGON *tab;
	int *vtTab;

	tab=(BISRFPOLYGON *)BiMalloc(nFac*sizeof(BISRFPOLYGON));
	vtTab=(int *)BiMalloc(nVtTab*sizeof(int));
	if(tab!=NULL && vtTab!=NULL)
	{
		mdl->vtTab=vtTab; /* For BiFreeSrf() */
		for(i=nFac-1; i>=0; i--)
		{
			tab[i]=fac->f;
			for(j=0; j<fac->f.nVt; j++)
			{
				vtTab[j]=fac->f.vt[j];
			}
			tab[i].vt=vtTab;
			vtTab+=fac->f.nVt;
			fac=fac->next;
		}
		mdl->np=nFac;
		mdl->p=tab;
		return BI_OK;
	}
	printf("Err 5\n");
	return BI_ERR;
}

static void BiMakeBoundingBox(BIPOINT *bbox,int nVt,BISRFVERTEX *vt)
{
	int i;
	BIPOINT min,max;

	min=vt[0].p;
	max=vt[0].p;
	for(i=1; i<nVt; i++)
	{
		min.x=BiSmaller(min.x,vt[i].p.x);
		min.y=BiSmaller(min.y,vt[i].p.y);
		min.z=BiSmaller(min.z,vt[i].p.z);
		max.x=BiLarger(max.x,vt[i].p.x);
		max.y=BiLarger(max.y,vt[i].p.y);
		max.z=BiLarger(max.z,vt[i].p.z);
	}

	BiSetPoint(&bbox[0],min.x,min.y,min.z);
	BiSetPoint(&bbox[1],max.x,min.y,min.z);
	BiSetPoint(&bbox[2],min.x,max.y,min.z);
	BiSetPoint(&bbox[3],max.x,max.y,min.z);

	BiSetPoint(&bbox[4],min.x,min.y,max.z);
	BiSetPoint(&bbox[5],max.x,min.y,max.z);
	BiSetPoint(&bbox[6],min.x,max.y,max.z);
	BiSetPoint(&bbox[7],max.x,max.y,max.z);
}

static int BiSrfInsVtx(BIVERTEXLIST *(*vtx),int ac,char *av[])
{
	BIVERTEXLIST *neo;
	neo=(BIVERTEXLIST *)BiTempMalloc(1,sizeof(BIVERTEXLIST));
	if(neo!=NULL)
	{
		neo->next=NULL;
		BiSetPoint(&neo->p.p,atof(av[1]),atof(av[2]),atof(av[3]));
		BiSetPoint(&neo->p.n,0,0,0);

		if(ac>=5 && av[4][0]=='R')
		{
			neo->p.r=BI_ON;
		}
		else
		{
			neo->p.r=BI_OFF;
		}

		neo->next=*vtx;
		*vtx = neo;
		return BI_OK;
	}
	else
	{
		return BI_ERR;
	}
}


static char *plgCmd[]=
{
	"BLK",    /* BLK Ignored */
	"EBK",    /* EBK Ignored */
	"C",     /* COLOR */
	"N",     /* NORMVECT */
	"V",     /* VERTEX */
	"B",     /* BRIGHT */
	"E",     /* ENDFACE */
	NULL
};

static int BiSrfInsPlg(BIFACELIST *(*fac))
{
	int i,ac,cmd;
	char str[256],tmp[256],*av[128];
	int nVtx,col15,bfr,bri;
	BICOLOR col;
	BIPOINT nom,cen;
	BIIDLIST *vtx;
	BIFACELIST *neo;

	neo=(BIFACELIST *)BiTempMalloc(1,sizeof(BIFACELIST));
	if(neo==NULL)
	{
		return BI_ERR;
	}

	bri=BI_OFF;
	bfr=BI_OFF;
	col.r=0;
	col.g=0;
	col.b=0;
	BiSetPoint(&nom,0,0,0);
	BiSetPoint(&cen,0,0,0);
	nVtx=0;
	vtx=NULL;
	while(BiGetStringFuncEx(str,255)!=NULL)
	{
		strcpy(tmp,str);

		BiArguments(&ac,av,127,tmp);
		if(ac==0 || str[0]=='#')
		{
			continue;
		}

		if(BiCommandNumber(&cmd,av[0],plgCmd)!=BI_OK)
		{
			return BI_ERR;
		}

		switch(cmd)
		{
		case 0:    /* "BLK" Ignored */
		case 1:    /* "EBK" Ignored */
			break;
		case 2:    /* "COLOR" */
			col15 = atoi(av[1]);
			col.g = ((col15>>10)&31)*255/31;
			col.r = ((col15>> 5)&31)*255/31;
			col.b = ((col15    )&31)*255/31;
			break;
		case 3:    /* "NORMVECT" */
			BiSetPoint(&cen,atof(av[1]),atof(av[2]),atof(av[3]));
			BiSetPoint(&nom,atof(av[4]),atof(av[5]),atof(av[6]));
			if(BiAbs(nom.x)>YSEPS || BiAbs(nom.y)>YSEPS || BiAbs(nom.z)>YSEPS)
			{
				BiNormalize(&nom,&nom);
				bfr=BI_ON;
			}
			break;
		case 4:    /* "VERTEX" */
			for(i=1; i<ac; i++)
			{
				if(BiSrfInsId(&vtx,atoi(av[i]))!=BI_OK)
				{
					return BI_ERR;
				}
				nVtx++;
			}
			break;
		case 5:    /* "BRIGHT" */
			bri=BI_ON;
			break;
		case 6:    /* "ENDFACE" */
			if(BiSrfSetVtxId(&neo->f,nVtx,vtx)==BI_OK)
			{
				neo->f.col=col;
				neo->f.backFaceRemove=bfr;
				neo->f.normal=nom;
				neo->f.center=cen;
				neo->f.bright=bri;

				neo->next=*fac;
				*fac=neo;

				return BI_OK;
			}
			else
			{
				return BI_ERR;
			}
		}
	}
	return BI_ERR;
}

/* Modified 1995/01/10 */
static int BiSrfSetVtxId(BISRFPOLYGON *neo,int nVt,BIIDLIST *vt)
{
	int i;
	int *vtLst;

	vtLst=(int *)BiTempMalloc(nVt,sizeof(int));
	if(vtLst!=NULL)
	{
		for(i=nVt-1; i>=0; i--)
		{
			vtLst[i]=vt->id;
			vt=vt->next;
		}
		if(vtLst[0]==vtLst[nVt-1])
		{
			nVt--;
		}
		neo->nVt=nVt;
		neo->vt=vtLst;
		return BI_OK;
	}
	return BI_ERR;
}

static int BiSrfInsId(BIIDLIST *(*lst),int id)
{
	BIIDLIST *neo;
	neo=(BIIDLIST *)BiTempMalloc(1,sizeof(BIIDLIST));
	if(neo!=NULL)
	{
		neo->id=id;
		neo->next=*lst;
		*lst=neo;
		return BI_OK;
	}
	else
	{
		return BI_ERR;
	}
}

static int BiSrfInsAxs(int ac,char *av[])
{
	/* Ignore */
	return BI_OK;
}



/*****************************************************************************/
/* public */void BiScaleSrf(BISRF *srf,real scl)
{
	int i;

	for(i=0; i<srf->nv; i++)
	{
		BiMulPoint(&srf->v[i].p,&srf->v[i].p,scl);
	}
	for(i=0; i<srf->np; i++)
	{
		BiMulPoint(&srf->p[i].center,&srf->p[i].center,scl);
	}
	for(i=0; i<8; i++)
	{
		BiMulPoint(&srf->bbox[i],&srf->bbox[i],scl);
	}
}

/*****************************************************************************/
int BiSrfCollision(BISRF *srf,BIPOSATT *pos,BIPOINT *p,real bump)
{
	BIPOINT min,max,q;
	BIAXIS axs;
	min=srf->bbox[0];
	max=srf->bbox[7];

	min.x-=bump;
	min.y-=bump;
	min.z-=bump;
	max.x+=bump;
	max.y+=bump;
	max.z+=bump;

	BiPntAngToAxis(&axs,pos);
	BiConvGtoL(&q,p,&axs);
	if(min.x<=q.x && q.x<=max.x && min.y<=q.y && q.y<=max.y && min.z<=q.z && q.z<=max.z)
	{
		return BI_IN;
	}
	else
	{
		return BI_OUT;
	}
}

/*****************************************************************************/
#ifdef BIACCEL3D
static void BiDrawSrf3(BISRF *mdl,BIPOSATT *pos);
#endif

extern int BiVrmlMode;
extern int BiRendLinkFlag;

static int BiRenderSrf(BISRF *mdl,BIPOSATT *pos);

int BiInsSrf(BISRF *mdl,BIPOSATT *pos)
{
	int i,clip;
	BIAXISF axs,byp;
	BIPOINT bbox[8];
	BIOBJECT *neo;

	if(BiRendLinkFlag==BI_ON || BiVrmlMode==BI_ON)
	{
		BiRenderSrf(mdl,pos);
	}

	BiMakeAxisF(&axs,pos);
	BiConvAxisLtoGF(&byp,&axs,&BiCnvAxs);
	for(i=0; i<8; i++)
	{
		BiConvLtoG(&bbox[i],&mdl->bbox[i],&byp);
	}

	if(BiCheckInsideViewPort(&clip,8,bbox)!=BI_IN)
	{
		return BI3DG_NOERR;
	}

#ifndef BIACCEL3D
	neo=BiNew3dObject(BIOBJ_SRFMODEL,byp.p.z);
	if(neo==NULL)
	{
		return BI3DG_STACKOVERFLOW;
	}
	neo->attr.smdl.clip=clip;
	neo->attr.smdl.axs=byp;
	neo->attr.smdl.mdl=mdl;
#else
	BiDrawSrf3(mdl,pos);
	if(BiEpsMode==BI_ON)
	{
		neo=BiNew3dObject(BIOBJ_SRFMODEL,byp.p.z);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		neo->attr.smdl.clip=clip;
		neo->attr.smdl.axs=byp;
		neo->attr.smdl.mdl=mdl;
	}
#endif

	return BI3DG_NOERR;
}


/***********************************************************************/

extern void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c);
extern int BiPrjMode,BiShadMode;
extern real BiOrthoDist;
extern BIPROJ BiPrj;

/*****************************************************************************/
#ifndef BIACCEL3D
int BiInsSrfDivideSub(BISRF *mdl,BIPOSATT *pos,int lowPriorityObject)
{
	int i,j,clip;
	BIAXISF axs,byp;
	BIPOINT bbox[8],c,n,*p,backMost;
	BIOBJECT *neo;

	if(BiRendLinkFlag==BI_ON || BiVrmlMode==BI_ON)
	{
		BiRenderSrf(mdl,pos);
	}

	BiMakeAxisF(&axs,pos);
	BiConvAxisLtoGF(&byp,&axs,&BiCnvAxs);
	for(i=0; i<8; i++)
	{
		BiConvLtoG(&bbox[i],&mdl->bbox[i],&byp);
	}

	if(BiCheckInsideViewPort(&clip,8,bbox)!=BI_IN)
	{
		return BI3DG_NOERR;
	}


	for(i=0; i<mdl->nv; i++)
	{
		BiConvLtoG(&mdl->tmp[i].p,&mdl->v[i].p,&byp);
	}

	for(i=0; i<mdl->np; i++)
	{
		int nVt,*vt;

		BiRotFastLtoG(&n,&mdl->p[i].normal,&byp.t);
		BiConvLtoG(&c,&mdl->p[i].center,&byp);

		if(mdl->p[i].backFaceRemove==BI_ON &&
		  ((BiPrjMode==BIPRJ_PERS && BiInnerPoint(&c,&n)>0) ||
		   (BiPrjMode==BIPRJ_ORTHO && n.z>0)))
		{
			continue;
		}
		if((BiPrjMode==BIPRJ_PERS && BiInnerPoint(&c,&n)>0) ||
		   (BiPrjMode==BIPRJ_ORTHO && n.z>0))
		{
			BiSubPoint(&n,&BiOrgP,&n);
		}

		nVt=mdl->p[i].nVt;
		vt=mdl->p[i].vt;

		p=(BIPOINT *)BiGetTmpStack(mdl->p[i].nVt*sizeof(BIPOINT));
		if(p==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		for(j=0; j<nVt; j++)
		{
			p[j]=mdl->tmp[vt[j]].p;
			if(j==0 || backMost.z<p[j].z)
			{
				backMost=p[j];
			}
		}

		if(lowPriorityObject!=BI_TRUE)
		{
			neo=BiNew3dObject(BIOBJ_POLYGON,c.z);
		}
		else
		{
			neo=BiNew3dObject(BIOBJ_POLYGON,backMost.z);
		}
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}

		neo->col=mdl->p[i].col;
		neo->attr.nPnt.clip=clip;
		neo->attr.nPnt.cen=c;
		neo->attr.nPnt.n=mdl->p[i].nVt;
		neo->attr.nPnt.p=p;
		neo->attr.nPnt.nom=n;
		neo->attr.nPnt.nomSw=(mdl->p[i].bright==BI_ON ? BI_OFF : BI_ON);
	}
	return BI3DG_NOERR;
}

int BiInsSrfDivide(BISRF *mdl,BIPOSATT *pos)
{
	return BiInsSrfDivideSub(mdl,pos,BI_FALSE);
}

int BiInsSrfDivideLowPriority(BISRF *mdl,BIPOSATT *pos)
{
	return BiInsSrfDivideSub(mdl,pos,BI_TRUE);
}

#else /* <- #ifndef BIACCEL3D */

int BiInsSrfDivide(BISRF *mdl,BIPOSATT *pos)
{
	return BiInsSrf(mdl,pos);
}

int BiInsSrfDivideLowPriority(BISRF *mdl,BIPOSATT *pos)
{
	return BiInsSrf(mdl,pos);
}

#endif /* <-#ifndef BIACCEL3D */



/*****************************************************************************/
static int BiRenderSrf(BISRF *mdl,BIPOSATT *pos)
{
	int i,j,k,clip;
	BIAXISF axs,byp;
	BIPOINT bbox[8],c,n,*vtx,*nom;
	BICOLOR col;

	BiMakeAxisF(&axs,pos);
	BiConvAxisLtoGF(&byp,&axs,&BiCnvAxs);
	for(i=0; i<8; i++)
	{
		BiConvLtoG(&bbox[i],&mdl->bbox[i],&byp);
	}

	if(BiCheckInsideViewPort(&clip,8,bbox)!=BI_IN && BiVrmlMode==BI_OFF)
	{
		return BI3DG_NOERR;
	}


	for(i=0; i<mdl->nv; i++)
	{
		BiConvLtoG(&mdl->tmp[i].p,&mdl->v[i].p,&byp);
		BiRotFastLtoG(&mdl->tmp[i].n,&mdl->v[i].n,&byp.t);
	}

	for(i=0; i<mdl->np; i++)
	{
		unsigned long stk;
		int nVt,*vt;

		vtx=(BIPOINT *)BiPushTmpStack(&stk,mdl->p[i].nVt*sizeof(BIPOINT));
		nom=(BIPOINT *)BiGetTmpStack(mdl->p[i].nVt*sizeof(BIPOINT));
		if(vtx==NULL || nom==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}

		BiRotFastLtoG(&n,&mdl->p[i].normal,&byp.t);
		BiConvLtoG(&c,&mdl->p[i].center,&byp);

		if(BiVrmlMode==BI_ON || mdl->p[i].backFaceRemove!=BI_ON || BiInnerPoint(&c,&n)<=0)
		{
			int round;

			BiRenderSetShadMode(mdl->p[i].bright==BI_ON ? BI_OFF : BI_ON);

			nVt=mdl->p[i].nVt;
			vt=mdl->p[i].vt;
			col=mdl->p[i].col;

			round=BI_OFF;
			for(j=0; j<nVt; j++)
			{
				vtx[j]=mdl->tmp[vt[j]].p;
				nom[j]=mdl->tmp[vt[j]].n;
				if(mdl->v[vt[j]].r==BI_ON)
				{
					round=BI_ON;
				}
			}

			/* Not treat as smooth shaded if the polygon has */
			/* opposing normals. */
			for(j=0; j<nVt; j++)
			{
				for(k=j; k<nVt; k++)
				{
					if(BiInnerPoint(&nom[j],&nom[k])<0.0F)
					{
						round=BI_OFF;
						break;
					}
				}
			}

			switch(mdl->p[i].backFaceRemove)
			{
			case BI_ON:
				if(BiRendLinkFlag==BI_ON)
				{
					switch(round)
					{
					case BI_ON:
						BiRenderLinkRoundPolygonOneSide(nVt,vtx,nom,&n,&col);
						break;
					case BI_OFF:
						BiRenderLinkPolygonOneSide(nVt,vtx,&n,&col);
						break;
					}
				}
				if(BiVrmlMode==BI_ON)
				{
					switch(round)
					{
					case BI_ON:
						BiVrmlOneSidedRoundPolygon(nVt,vtx,nom,&n,&col);
						break;
					case BI_OFF:
						BiVrmlOneSidedPolygon(nVt,vtx,&n,&col);
						break;
					}
				}
				break;
			case BI_OFF:
				if(BiRendLinkFlag==BI_ON)
				{
					BiRenderLinkPolygonTwoSide(nVt,vtx,&col);
				}
				if(BiVrmlMode==BI_ON)
				{
					BiVrmlPolygon(nVt,vtx,&col);
				}
				break;
			}
		}

		BiPopTmpStack(stk);
	}
	return BI3DG_NOERR;
}
/*****************************************************************************/
static int BiVrmlSrf(BISRF *mdl,BIPOSATT *pos)
{
	int i,j,clip;
	BIAXISF axs,byp;
	BIPOINT bbox[8],c,n,*vtx,*nom;
	BICOLOR col;

	BiMakeAxisF(&axs,pos);
	BiConvAxisLtoGF(&byp,&axs,&BiCnvAxs);
	for(i=0; i<8; i++)
	{
		BiConvLtoG(&bbox[i],&mdl->bbox[i],&byp);
	}

	if(BiCheckInsideViewPort(&clip,8,bbox)!=BI_IN)
	{
		return BI3DG_NOERR;
	}


	for(i=0; i<mdl->nv; i++)
	{
		BiConvLtoG(&mdl->tmp[i].p,&mdl->v[i].p,&byp);
		BiRotFastLtoG(&mdl->tmp[i].n,&mdl->v[i].n,&byp.t);
	}

	for(i=0; i<mdl->np; i++)
	{
		unsigned long stk;
		int nVt,*vt;

		vtx=(BIPOINT *)BiPushTmpStack(&stk,mdl->p[i].nVt*sizeof(BIPOINT));
		nom=(BIPOINT *)BiGetTmpStack(mdl->p[i].nVt*sizeof(BIPOINT));
		if(vtx==NULL || nom==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}

		BiRotFastLtoG(&n,&mdl->p[i].normal,&byp.t);
		BiConvLtoG(&c,&mdl->p[i].center,&byp);

		if(mdl->p[i].backFaceRemove!=BI_ON || BiInnerPoint(&c,&n)<=0)
		{
			int round;

			BiRenderSetShadMode(mdl->p[i].bright==BI_ON ? BI_OFF : BI_ON);

			nVt=mdl->p[i].nVt;
			vt=mdl->p[i].vt;
			col=mdl->p[i].col;

			round=BI_OFF;
			for(j=0; j<nVt; j++)
			{
				vtx[j]=mdl->tmp[vt[j]].p;
				nom[j]=mdl->tmp[vt[j]].n;
				if(mdl->v[vt[j]].r==BI_ON)
				{
					round=BI_ON;
				}
			}

			switch(mdl->p[i].backFaceRemove)
			{
			case BI_ON:
				switch(round)
				{
				case BI_ON:
					BiRenderLinkRoundPolygonOneSide(nVt,vtx,nom,&n,&col);
					break;
				case BI_OFF:
					BiRenderLinkPolygonOneSide(nVt,vtx,&n,&col);
					break;
				}
				break;
			case BI_OFF:
				BiRenderLinkPolygonTwoSide(nVt,vtx,&col);
				break;
			}
		}

		BiPopTmpStack(stk);
	}
	return BI3DG_NOERR;
}
/*****************************************************************************/
typedef struct BiSrfPolygLst {
	struct BiSrfPolygLst *next,*prev;
	struct BiSrfPolygLst *drwNext,*drwPrev;
	BISRFPOLYGON *ptr;
	BIPOINT cen,nom;
	real z;
} BISRFPOLYGLIST;

/* Original static-function is frozen at the bottom of the file */
#define BiDrwSrfAttach(lstOrg,buf,top) \
{ \
	BISRFPOLYGLIST *lstTmp; \
	lstTmp=(lstOrg); \
	while(1)  \
	{  \
		if((lstTmp)->z > (buf)->z)  \
		{  \
			if((lstTmp)->next==NULL)  \
			{  \
				(lstTmp)->next=(buf);  \
				(buf)->drwPrev=(lstTmp);  \
				(buf)->drwNext=(lstTmp)->drwNext;  \
				(lstTmp)->drwNext=(buf);  \
				if((buf)->drwNext!=NULL)  \
				{  \
					(buf)->drwNext->drwPrev=(buf);  \
				}  \
				break;  \
			}  \
			else  \
			{  \
				(lstTmp)=(lstTmp)->next;  \
				continue;  \
			}  \
		}  \
		else  \
		{  \
			if((lstTmp)->prev==NULL)  \
			{  \
				(lstTmp)->prev=(buf);  \
				(buf)->drwPrev=(lstTmp)->drwPrev;  \
				(buf)->drwNext=(lstTmp);  \
				(lstTmp)->drwPrev=(buf);  \
				if((buf)->drwPrev!=NULL)  \
				{  \
					(buf)->drwPrev->drwNext=(buf);  \
				}  \
				else  \
				{  \
					*(top)=(buf);  \
				}  \
				break;  \
			}  \
			else  \
			{  \
				(lstTmp)=(lstTmp)->prev;  \
				continue;  \
			}  \
		}  \
	}  \
}


#ifndef BIACCEL3D
void BiFlushDrawSrf(BISRF *mdl,BIAXISF *mdlAx,int clip)
#else
void BiEpsDrawSrf(BISRF *mdl,BIAXISF *mdlAx,int clip)
#endif
{
	int i,maxEd;
	unsigned long stk;
	BIPOINT *vtx;
	BIPOINTS *scr,*scrAll;
	BISRFPOLYGLIST *lst,*buf,*top;
	BISRFPOLYGON *ptr;
	BICOLOR *col,shad;
	real inp;
#ifndef BIACCEL3D
	int nClp;
#endif

	/* Prepare */
	vtx=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*mdl->nv);
	buf=(BISRFPOLYGLIST *)BiGetTmpStack(sizeof(BISRFPOLYGLIST)*mdl->np);
	if(vtx==NULL || buf==NULL)
	{
		goto END;
	}

	for(i=0; i<mdl->nv; i++)
	{
		BiConvLtoG(&vtx[i],&mdl->v[i].p,mdlAx);
	}

	/* Make Z-Sort Table */
	lst=NULL;
	top=NULL;
	maxEd=0;
	for(i=0; i<mdl->np; i++)
	{
		BIPOINT cen,nom;

		ptr=&mdl->p[i];
		maxEd=BiLarger(ptr->nVt,maxEd);
		BiConvLtoG(&cen,&ptr->center,mdlAx);
		BiRotFastLtoG(&nom,&ptr->normal,&mdlAx->t);
		inp=BiInnerPoint(&cen,&nom);
		if(ptr->backFaceRemove==BI_ON && inp>0)
		{
			/* Back Face Remove */
			ptr=NULL; /* ? High-C's bug ? */
			continue;
		}
		if(inp>0)
		{
			BiSubPoint(&nom,&BiOrgP,&nom);
		}

		buf->z=cen.z;
		buf->ptr=ptr;
		buf->cen=cen;
		buf->nom=nom;
		buf->next=NULL;
		buf->prev=NULL;
		if(lst==NULL)
		{
			lst=buf;
			buf->drwPrev=NULL;
			buf->drwNext=NULL;
			top=lst;
		}
		else
		{
			BiDrwSrfAttach(lst,buf,&top);
		}

		buf++;
	}

	/* Binary Tree Top -> Linear Tree Top */
	lst=top;

	/* Projection & Draw */
	scrAll=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*mdl->nv);
	scr=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*maxEd);
	if(scrAll==NULL || scr==NULL)
	{
		goto END;
	}

	if(BiPrjMode==BIPRJ_ORTHO)
	{
		for(i=0; i<mdl->nv; i++)
		{
			vtx[i].z=BiOrthoDist;
		}
	}

	if(clip==BI_OFF && BiEpsMode==BI_OFF)
	{
		BiPlProject(mdl->nv,scrAll,vtx,&BiPrj);
		while(lst!=NULL)
		{
			int *vtP;
			BIPOINTS *scrP;

			ptr=lst->ptr;
			if(ptr->bright==BI_OFF && BiShadMode==BI_ON)
			{
				BiShadColor(&shad,&ptr->col,&lst->cen,&lst->nom);
				col=&shad;
			}
			else
			{
				col=&ptr->col;
			}

			vtP=ptr->vt;
			scrP=scr;
			for(i=0; i<ptr->nVt; i++)
			{
				(*scrP)=scrAll[*vtP];
				scrP++;
				vtP++;
				/* original:scr[i]=scrAll[ptr->vt[i]]; */
			}
			BiDrawPolyg2D(ptr->nVt,scr,col);
			lst=lst->drwNext;
		}
	}
	else
	{
		BIPOINT *tab,*clp;
		tab=(BIPOINT *)BiGetTmpStack(sizeof(BIPOINT)*maxEd);
		clp=(BIPOINT *)BiGetTmpStack(sizeof(BIPOINT)*maxEd*2);
		if(clp==NULL || tab==NULL)
		{
			goto END;
		}

		if(BiEpsMode==BI_OFF)
		{
			for(i=0; i<mdl->nv; i++)
			{
				if(vtx[i].z>BiPrj.nearz)
				{
					BiProject(&scrAll[i],&vtx[i],&BiPrj);
				}
			}
		}

		while(lst!=NULL)
		{
			ptr=lst->ptr;

			if(ptr->bright==BI_OFF && BiShadMode==BI_ON && ptr->backFaceRemove==BI_ON)
			{
				BiShadColor(&shad,&ptr->col,&lst->cen,&lst->nom);
				col=&shad;
			}
			else
			{
				col=&ptr->col;
			}

			if(BiEpsMode==BI_OFF)
			{
				for(i=0; i<ptr->nVt; i++)
				{
					if(vtx[ptr->vt[i]].z<=BiPrj.nearz)
					{
						break;
					}
					scr[i]=scrAll[ptr->vt[i]];
				}
			}

			if(BiEpsMode==BI_OFF && i==ptr->nVt)
			{
				BiDrawPolyg2D(ptr->nVt,scr,col);
			}
			else
			{
				for(i=0; i<ptr->nVt; i++)
				{
					tab[i]=vtx[ptr->vt[i]];
				}
				if(BiEpsMode==BI_ON)
				{
					BiEpsDrawPolyg3(ptr->nVt,tab,col);
				}
			#ifndef BIACCEL3D
				BiNearClipPolyg(&nClp,clp,ptr->nVt,tab,BiPrj.nearz);
				BiPlProject(nClp,scr,clp,&BiPrj);
				BiDrawPolyg2D(nClp,scr,col);
			#endif
			}

			lst=lst->drwNext;
		}
	}

END:
	BiPopTmpStack(stk);
}


#ifdef BIACCEL3D
static void BiDrawSrf3(BISRF *mdl,BIPOSATT *pos)
{
	int i,j,vtId;
	unsigned long stk2;
	BIPOINT nom,cen;
	BIPOINT *plVt,*plNm;
	BISRFPOLYGON *spl;
	BIAXISF mdlAx;
	real inp;

	BiMakeAxisF(&mdlAx,pos);
	BiConvAxisLtoGF(&mdlAx,&mdlAx,&BiCnvAxs);
	BiPushMatrix(pos);

	for(i=0; i<mdl->np; i++)
	{
		spl=&mdl->p[i];

		BiConvLtoG(&cen,&spl->center,&mdlAx);
		BiRotFastLtoG(&nom,&spl->normal,&mdlAx.t);

		inp=BiInnerPoint(&cen,&nom);
		if(spl->backFaceRemove==BI_ON && inp>0)
		{
			continue;
		}

		// Note: nom is refreshed here.
		if(inp>0)
		{
			BiSubPoint(&nom,&BiOrgP,&spl->normal);
		}
		else
		{
			nom=spl->normal;
		}

		plVt=(BIPOINT *)BiPushTmpStack(&stk2,sizeof(BIPOINT)*spl->nVt*2);
		plNm=plVt+spl->nVt;
		if(plVt==NULL)
		{
			continue;
		}

		for(j=0; j<spl->nVt; j++)
		{
			vtId=spl->vt[j];
			plVt[j]=mdl->v[vtId].p;
/* #### */
			if(mdl->v[vtId].r==BI_ON && spl->backFaceRemove==BI_ON)
			{
				plNm[j]=mdl->v[vtId].n;
			}
			else
			{
				plNm[j]=nom;
			}
		}

		BiDrawPolygGrad3(spl->nVt,plVt,plNm,&spl->col);
		BiPopTmpStack(stk2);
	}

	BiPopMatrix();
}

void BiFlushDrawSrf(BISRF *mdl,BIAXISF *mdlAx,int clip)
{
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawSrf(mdl,mdlAx,clip);
	}
}
#endif

void BiGetSrfRadius(real *rad,BISRF *srf)
{
	BIPOINT min,max;

	min=srf->bbox[0];
	max=srf->bbox[7];
	*rad=BiLarger(BiLengthPoint3(&min),BiLengthPoint3(&max));
}

void BiFreeSrf(BISRF *srf)
{
	BiFree(srf->vtTab);
	BiFree(srf->tmp);
	BiFree(srf->v);
	BiFree(srf->p);
}





/*****************************************************************************/
int BiOvwSrfShadow(BISRF *mdl,BIPOSATT *pos)
{
	unsigned long stk;
	int i,clip;
	BIAXISF pln;
	BIPOINT bbx[8],*vtx;

	BiPushMatrix(pos);



	/* Generate Shadow Projection Plane */
	BiConvAxisGtoLF(&pln,&BiMdlAxs,&BiShadowAxs);



	/* Bounding Box Check */
	for(i=0; i<8; i++)
	{
		BiConvLtoG(&bbx[i],&mdl->bbox[i],&pln);
		bbx[i].y=0.0F;
		BiConvGtoL(&bbx[i],&bbx[i],&pln);
		BiConvLtoG(&bbx[i],&bbx[i],&BiCnvAxs);
	}
	if(BiCheckInsideViewPort(&clip,8,bbx)!=BI_IN)
	{
		BiPopMatrix();
		return BI3DG_NOERR;
	}



	/* Generate Vertex */
	vtx=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*mdl->nv);
	if(vtx!=NULL)
	{
		for(i=0; i<mdl->nv; i++)
		{
			BiConvLtoG(&vtx[i],&mdl->v[i].p,&pln);
			vtx[i].y=0.0F;
			BiConvGtoL(&vtx[i],&vtx[i],&pln);
		}



		/* Draw Shadow */
		for(i=0; i<mdl->np; i++)
		{
			BIVECTOR nom;
			BiRotFastLtoG(&nom,&mdl->p[i].normal,&pln.t);
			if(nom.y>0.0F || mdl->p[i].backFaceRemove==BI_OFF)
			{
				int j;
				unsigned long stk;
				BIPOINT *edg;
				BISRFPOLYGON *pl;

				pl=&mdl->p[i];

				edg=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*pl->nVt);
				if(edg!=NULL)
				{
					for(j=0; j<pl->nVt; j++)
					{
						edg[j]=vtx[pl->vt[j]];
					}
					BiOvwPolyg(pl->nVt,edg,&BiBlack);
				}

				BiPopTmpStack(stk);
			}
		}
		BiPopTmpStack(stk);
	}



	BiPopMatrix();
	return BI3DG_NOERR;
}
/*****************************************************************************/

/* static void BiDrwSrfAttach */
/* 	(BISRFPOLYGLIST *lst,BISRFPOLYGLIST *buf,BISRFPOLYGLIST *(*top)) */
/* { */
/* #ifdef SEEKTIMETEST */
/* 	nAttach++; */
/* #endif */
/* 	while(1) */
/* 	{ */
/* 	#ifdef SEEKTIMETEST */
/* 		nSeek++; */
/* 	#endif */
/* 		if(lst->z > buf->z) */
/* 		{ */
/* 			if(lst->next==NULL) */
/* 			{ */
/* 				lst->next=buf; */
				/* also Generate Linear Chain */
/* 				buf->drwPrev=lst; */
/* 				buf->drwNext=lst->drwNext; */
/* 				lst->drwNext=buf; */
/* 				if(buf->drwNext!=NULL) */
/* 				{ */
/* 					buf->drwNext->drwPrev=buf; */
/* 				} */
/* 				break; */
/* 			} */
/* 			else */
/* 			{ */
/* 				lst=lst->next; */
/* 				continue; */
/* 			} */
/* 		} */
/* 		else */
/* 		{ */
/* 			if(lst->prev==NULL) */
/* 			{ */
/* 				lst->prev=buf; */
 				/* also Generate Linear Chain */
/* 				buf->drwPrev=lst->drwPrev; */
/* 				buf->drwNext=lst; */
/* 				lst->drwPrev=buf; */
/* 				if(buf->drwPrev!=NULL) */
/* 				{ */
/* 					buf->drwPrev->drwNext=buf; */
/* 				} */
/* 				else */
/* 				{ */
/* 					*top=buf; */
/* 				} */
/* 				break; */
/* 			} */
/* 			else */
/* 			{ */
/* 				lst=lst->prev; */
/* 				continue; */
/* 			} */
/* 		} */
/* 	} */
/* } */
