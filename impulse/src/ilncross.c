#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "impulse.h"

static int BiLine2CrossStateParallel(BILIMLINE2 *ln1,BILIMLINE2 *ln2);
static int BiLine2CrossStateNotParallel(real s,real t);
static int BiLineCrossS(real *s,BILINE2 *l1,BILINE2 *l2);
static int BiLineCrossT(real *t,BILINE2 *l1,BILINE2 *l2);
static int BiCloopDeletability(int nPl,BIPOLYGON2 *pl,int clp,int ppn,int cpn);
static real BiCloopDelLng(BIPOLYGON2 *pl,int clp,int ppn,int cpn);
static int BiDelCLoop(int *nwn,BIPOLYGON2 *(*nw),int nPl,BIPOLYGON2 *pl);
static int BiOnePolyg2ToTris(int *nTr,BIPOINT2 *tr,BIPOLYGON2 *pl);
static void BiOnePolyg2TrisAppend(int *nTr,BIPOINT2 *tr,BIPOLYGON2 *pl);
static int BiOnePolyg2TrisSep(BIPOLYGON2 *pl1,BIPOLYGON2 *pl2,BIPOLYGON2 *org,int strategy);
static int BiOnePolyg2TrisSeparatability(BIPOLYGON2 *pl,BILIMLINE2 *ln1);
static int BiOnePolyg2TrisInside(BIPOLYGON2 *pl,BIPOINT2 *pnt);
static int BiOnePolyg2TrisInsideCountTPlus(BIPOLYGON2 *pl,BILINE2 *ln);

enum {
	BISEP_SHORTEST,
	BISEP_CONVEX
};

typedef struct {
	BIPOINT org;
	BIPOINT2 cnv;
} BI3VS2;

static BI3VS2 *BiPlgTrisMapBuf(int nPl,BIPOLYGON *pl);
static BIPOLYGON2 *BiPlgTrisPlg2Buf(int nPl,BIPOLYGON *pl);
static void BiPlgTrisFreePlg2(int nPl,BIPOLYGON2 *cnv);
static int BiPlgTrisNumMap(int nPl,BIPOLYGON *pl);
static void BiPlgTrisMap(BI3VS2 *mp,BIPOLYGON2 *cv,int nPl,BIPOLYGON *pl);
static BIPOINT2 *BiPlgTrisPnt2Buf(int nPl,BIPOLYGON *pl);
static void BiPlgTrisUnmap(BIPOINT *t,int nt,BIPOINT2 *t2,int nm,BI3VS2 *m);
static void BiKclUnmap(BIPOLYGON *plp,BIPOLYGON2 *plp2,int nm,BI3VS2 *m);

int BiPolygToTris(int *nTr,BIPOINT *tr,int nPl,BIPOLYGON *pl)
{
	int ret,nMp;
	BI3VS2 *map;
	BIPOLYGON2 *cnv;
	BIPOINT2 *tr2;
	ret=BI_OK;
	nMp=BiPlgTrisNumMap(nPl,pl);
	map=BiPlgTrisMapBuf(nPl,pl);
	cnv=BiPlgTrisPlg2Buf(nPl,pl);
	tr2=BiPlgTrisPnt2Buf(nPl,pl);
	if(map==NULL || cnv==NULL || tr2==NULL)
	{
		ret=BI_ERR;
		goto END;
	}

	BiPlgTrisMap(map,cnv,nPl,pl);
	ret=BiPolyg2ToTris(nTr,tr2,nPl,cnv);
	BiPlgTrisUnmap(tr,*nTr,tr2,nMp,map);

END:
	BiPlgTrisFreePlg2(nPl,cnv);
	BiFree(map);
	BiFree(cnv);
	BiFree(tr2);
	return ret;
}

int BiKillCLoop(BIPOLYGON *neo,int nPl,BIPOLYGON *pl)
{
	int ret,nMp;
	BI3VS2 *map;
	BIPOLYGON2 *cnv,plp;

	ret=BI_OK;
	nMp=BiPlgTrisNumMap(nPl,pl);
	map=BiPlgTrisMapBuf(nPl,pl);
	cnv=BiPlgTrisPlg2Buf(nPl,pl);
	plp.p=NULL;
	if(map==NULL || cnv==NULL)
	{
		ret=BI_ERR;
		goto END;
	}

	BiPlgTrisMap(map,cnv,nPl,pl);
	ret=BiKillCLoop2(&plp,nPl,cnv);
	BiKclUnmap(neo,&plp,nMp,map);
	BiFree(plp.p);

END:
	BiPlgTrisFreePlg2(nPl,cnv);
	BiFree(map);
	BiFree(cnv);
	return ret;
}

static void BiPlgTrisUnmap(BIPOINT *t,int nt,BIPOINT2 *t2,int nm,BI3VS2 *m)
{
	int i,j;
	for(i=0; i<nt*3; i++)
	{
		BiSetPoint(&t[i],0,0,0);
		for(j=0; j<nm; j++)
		{
			if(BiSamePoint2(&t2[i],&m[j].cnv)==BI_TRUE)
			{
				t[i]=m[j].org;
				break;
			}
		}
	}
}

static void BiKclUnmap(BIPOLYGON *plp,BIPOLYGON2 *plp2,int nm,BI3VS2 *m)
{
	int i,j;
	plp->np=plp2->np;
	plp->p=(BIPOINT *)BiMalloc(plp->np*sizeof(BIPOINT));
	for(i=0; i<plp2->np; i++)
	{
		BiSetPoint(&plp->p[i],0,0,0);
		for(j=0; j<nm; j++)
		{
			if(BiSamePoint2(&plp2->p[i],&m[j].cnv)==BI_TRUE)
			{
				plp->p[i]=m[j].org;
				break;
			}
		}
	}
}

static int BiPlgTrisNumMap(int nPl,BIPOLYGON *pl)
{
	int sum,i;
	sum=0;
	for(i=0; i<nPl; i++)
	{
		sum+=pl[i].np;
	}
	return sum;
}

static BI3VS2 *BiPlgTrisMapBuf(int nPl,BIPOLYGON *pl)
{
	long sum;
	sum=BiPlgTrisNumMap(nPl,pl);
	return (BI3VS2 *)BiMalloc(sum*sizeof(BI3VS2));
}

static BIPOLYGON2 *BiPlgTrisPlg2Buf(int nPl,BIPOLYGON *pl)
{
	int i;
	BIPOLYGON2 *ret;
	ret=(BIPOLYGON2 *)BiMalloc(nPl*sizeof(BIPOLYGON2));
	for(i=0; i<nPl; i++)
	{
		ret[i].np=pl[i].np;
		ret[i].p=(BIPOINT2 *)BiMalloc(pl[i].np*sizeof(BIPOINT2));
		if(ret[i].p==NULL)
		{
			/* Some Bugs Here */
			return NULL;
		}
	}
	return ret;
}

static void BiPlgTrisFreePlg2(int nPl,BIPOLYGON2 *cnv)
{
	int i;
	for(i=0; i<nPl; i++)
	{
		BiFree(cnv[i].p);
	}
}

static BIPOINT2 *BiPlgTrisPnt2Buf(int nPl,BIPOLYGON *pl)
{
	int nTri,nVtx,i;
	nVtx=pl[0].np;
	for(i=1; i<nPl; i++)
	{
		nVtx=nVtx+2+pl[i].np;
	}
	nTri=nVtx-2;
	return (BIPOINT2 *)BiMalloc(nTri*3*sizeof(BIPOINT2));
}


static void BiPlgTrisMap(BI3VS2 *mp,BIPOLYGON2 *cv,int nPl,BIPOLYGON *pl)
{
	int i,j,nMp;
	BIPOINT nom,vec,tmp;
	BIANGLE ang;
	BITRIGON trg;

	BiAverageNormalVector(&nom,pl[0].np,pl[0].p);
	BiRightAngleVector(&vec,&nom);
	BiVectorToAngle(&ang,&vec,&nom);
	BiMakeTrigonomy(&trg,&ang);

	nMp=0;
	for(i=0; i<nPl; i++)
	{
		cv[i].np=pl[i].np;
		for(j=0; j<pl[i].np; j++)
		{
			BiRotFastGtoL(&tmp,&pl[i].p[j],&trg);
			cv[i].p[j].x=tmp.x;
			cv[i].p[j].y=tmp.z;
			mp[nMp].org=pl[i].p[j];
			mp[nMp].cnv=cv[i].p[j];
			nMp++;
		}
	}
}

/*****************************************************************************/

int BiPolyg2ToTris(int *nTr,BIPOINT2 *tr,int nPl,BIPOLYGON2 *pl)
{
	BIPOLYGON2 kcl;
	if(nPl>1)
	{
		if(BiKillCLoop2(&kcl,nPl,pl)!=BI_OK)
		{
			return BI_ERR;
		}
		if(BiPolyg2ToTris(nTr,tr,1,&kcl)!=BI_OK)
		{
			BiFree(kcl.p);
			return BI_ERR;
		}
		return BI_TRUE;
	}
	else if(nPl<1)
	{
		return BI_ERR;
	}
	else
	{
		*nTr=0;
		return BiOnePolyg2ToTris(nTr,tr,pl);
	}
}

static int BiOnePolyg2ToTris(int *nTr,BIPOINT2 *tr,BIPOLYGON2 *pl)
{
	BIPOLYGON2 pl1,pl2;
	if(pl->np==3)
	{
		BiOnePolyg2TrisAppend(nTr,tr,pl);
		return BI_OK;
	}
	else if(pl->np<3)
	{
		/* "BiOnePolyg2ToTris@ilncross.c\n" */
		/* "Warning:less than 3 edges of polygon appeared.\n" */
		return BI_OK;
	}
	else
	{
		if(BiOnePolyg2TrisSep(&pl1,&pl2,pl,BISEP_SHORTEST)!=BI_OK)
		{
			return BI_ERR;
		}
		if(BiOnePolyg2ToTris(nTr,tr,&pl1)!=BI_OK || BiOnePolyg2ToTris(nTr,tr,&pl2)!=BI_OK)
		{
			BiFree(pl1.p);
			BiFree(pl2.p);
			return BI_ERR;
		}
		BiFree(pl1.p);
		BiFree(pl2.p);
		return BI_OK;
	}
}

static void BiOnePolyg2TrisAppend(int *nTr,BIPOINT2 *tr,BIPOLYGON2 *pl)
{
	if(tr!=NULL)
	{
		tr[(*nTr)*3  ]=pl->p[0];
		tr[(*nTr)*3+1]=pl->p[1];
		tr[(*nTr)*3+2]=pl->p[2];
	}
	(*nTr)++;
}





static void BiExecSeparate2(BIPOLYGON2 *pl1,BIPOLYGON2 *pl2,BIPOLYGON2 *org,int i,int j);

static int BiOnePolyg2TrisSep(BIPOLYGON2 *pl1,BIPOLYGON2 *pl2,BIPOLYGON2 *org,int strategy)
{
	int i,j,n1,n2,ex;
	BILIMLINE2 ln;
	BIPOINT2 vec;
	real lnLen,tmpLen;

	n1=-1;
	n2=-1;
	lnLen=(real)0.0;
	for(i=0; i<org->np; i++)
	{
		for(j=org->np-1; j>i+1; j--)
		{
			ln.p1=org->p[i];
			ln.p2=org->p[j];
			ex=BiOnePolyg2TrisSeparatability(org,&ln);
			if(ex==BI_TRUE)
			{
				BiSubPoint2(&vec,&ln.p1,&ln.p2);
				tmpLen=vec.x*vec.x+vec.y*vec.y;
				if(n1<0 || tmpLen<lnLen)
				{
					lnLen=tmpLen;
					n1=i;
					n2=j;
				}
			}
		}
	}

	if(n1<0)
	{
		return BI_ERR;
	}

	i=n1;
	j=n2;
	pl1->np= j-i+1;
	pl1->p=(BIPOINT2 *)BiMalloc(pl1->np*sizeof(BIPOINT2));
	pl2->np= org->np -(j-i) +1;
	pl2->p=(BIPOINT2 *)BiMalloc(pl2->np*sizeof(BIPOINT2));
	if(pl1->p==NULL || pl2->p==NULL)
	{
		BiFree(pl1->p);
		BiFree(pl2->p);
		return BI_ERR;
	}

	BiExecSeparate2(pl1,pl2,org,n1,n2);
	return BI_OK;
}

static void BiExecSeparate2(BIPOLYGON2 *pl1,BIPOLYGON2 *pl2,BIPOLYGON2 *org,int i,int j)
{
	int k;
	pl1->np=0;
	pl2->np=0;
	for(k=0; k<org->np; k++)
	{
		if(i<=k && k<=j)
		{
			pl1->p[pl1->np]=org->p[k];
			pl1->np++;
		}
		if(k<=i || j<=k)
		{
			pl2->p[pl2->np]=org->p[k];
			pl2->np++;
		}
	}
}

static int BiOnePolyg2TrisSeparatability(BIPOLYGON2 *pl,BILIMLINE2 *ln)
{
	int i,np1,np2,sta;
	BILIMLINE2 ln1,ln2;
	BIPOINT2 cen;

	/* 2直線の両端点は、ポリゴン中で2度以上使われていない */
	np1=0;
	np2=0;
	for(i=0; i<pl->np; i++)
	{
		if(BiSamePoint2(&pl->p[i],&ln->p1)==BI_TRUE)
		{
			np1++;
			if(np1>=2)
			{
				return BI_FALSE;
			}
		}
		if(BiSamePoint2(&pl->p[i],&ln->p2)==BI_TRUE)
		{
			np2++;
			if(np2>=2)
			{
				return BI_FALSE;
			}
		}
	}

	/* どの辺とも交わらない */
	ln1=*ln;
	for(i=0; i<pl->np; i++)
	{
		ln2.p1=pl->p[i];
		ln2.p2=pl->p[(i+1)%pl->np];
		sta=BiLine2CrossStatus(&ln1,&ln2);
		if(sta!=BI_CRS_NOCROSS && sta!=BI_CRS_PARALLEL && sta!=BI_CRS_ONESIDE_SAME)
		{
			return BI_FALSE;
		}
	}

	/* その中点はポリゴン内部である */
	BiAddPoint2(&cen,&ln->p1,&ln->p2);
	BiDivPoint2(&cen,&cen,2.0);
	if(BiOnePolyg2TrisInside(pl,&cen)!=BI_TRUE)
	{
		return BI_FALSE;
	}

	return BI_TRUE;
}

static int BiOnePolyg2TrisInside(BIPOLYGON2 *pl,BIPOINT2 *pnt)
{
	int i,k;
	BIPOINT2 tmp;
	BILINE2 ln;
	for(i=0; i<pl->np-1; i++)
	{
		BiAddPoint2(&tmp,&pl->p[i],&pl->p[i+1]);
		BiDivPoint2(&tmp,&tmp,2.0);
		/* ln.o=*pnt;
		BiSubPoint2(&ln.v,&tmp,&ln.o); */
		BiMakeLine2(&ln,pnt,&tmp);

		k=BiOnePolyg2TrisInsideCountTPlus(pl,&ln);
		if(k>0)
		{
			return ((k&1)!=0 ? BI_TRUE : BI_FALSE);
		}
	}
	return BI_FALSE;
}

static int BiOnePolyg2TrisInsideCountTPlus(BIPOLYGON2 *pl,BILINE2 *ln)
{
	real s,t;
	int i,sta,tplus;
	BILIMLINE2 l1,l2;
	BILINE2 ln2;

	tplus=0;

	/* l1.p1=ln->o;
	BiAddPoint2(&l1.p2, &l1.p1,&ln->v); */
	BiLineToLimLine2(&l1,ln);
	for(i=0; i<pl->np; i++)
	{
		BiMakeLimLine2(&l2,&pl->p[i],&pl->p[(i+1)%pl->np]);
		BiLimLineToLine2(&ln2,&l2);
		/* l2.p1=pl->p[i];
		l2.p2=pl->p[(i+1)%pl->np];
		ln2.o=l2.p1;
		BiSubPoint2(&ln2.v,&l2.p2,&l2.p1); */

		sta=BiLine2CrossStatus(&l1,&l2);
		switch(sta)
		{
		case BI_CRS_ON_LINE:
		case BI_CRS_SAME:
			return 0;
		case BI_CRS_PARALLEL:
			continue;
		default:
			BiLineCrossS(&s,ln,&ln2);
			BiLineCrossT(&t,ln,&ln2);
			if(BiSame(s,0.0)==BI_TRUE||BiSame(s,1.0)==BI_TRUE)
			{
				return 0;
			}
			if(t>0 && 0.0<s && s<1.0)
			{
				tplus++;
			}
			break;
		}
	}
	return tplus;
}

/*****************************************************************************/
int BiCheckConvex(BIPOLYGON *plg)
{
	int i,r;
	BIPOINT nom,vec,tmp;
	BIANGLE ang;
	BITRIGON trg;
	BIPOLYGON2 plg2;

	if(plg->np<=3)
	{
		return BI_TRUE;
	}

	plg2.np=plg->np;
	plg2.p=(BIPOINT2 *)BiMalloc(plg->np*sizeof(BIPOINT2));
	r=BI_FALSE;
	if(plg2.p!=NULL)
	{
		BiAverageNormalVector(&nom,plg->np,plg->p);
		BiRightAngleVector(&vec,&nom);
		BiVectorToAngle(&ang,&vec,&nom);
		BiMakeTrigonomy(&trg,&ang);

		for(i=0; i<plg->np; i++)
		{
			BiRotFastGtoL(&tmp,&plg->p[i],&trg);
			plg2.p[i].x=tmp.x;
			plg2.p[i].y=tmp.z;
		}

		r=BiCheckConvex2(&plg2);
		BiFree(plg2.p);
	}
	return r;
}

static int BiPolygToConvexUnmap(BIPOLYGON cvx[],int nCvx,BIPOLYGON2 cvx2[],int nMp,BI3VS2 *map);

int BiPolygToConvex(int *nCvx,BIPOLYGON *cvx,BIPOLYGON *org)
{
	int ret,nMp;
	BI3VS2 *map;
	BIPOLYGON2 *cnv,*tmp;
	BIPOINT2 *tr2;
	ret=BI_OK;
	nMp=BiPlgTrisNumMap(1,org);
	map=BiPlgTrisMapBuf(1,org);
	cnv=BiPlgTrisPlg2Buf(1,org);
	tr2=BiPlgTrisPnt2Buf(1,org);
	tmp=(BIPOLYGON2 *)BiMalloc((org->np-2)*sizeof(BIPOLYGON2));
	if(map==NULL || cnv==NULL || tr2==NULL || tmp==NULL)
	{
		ret=BI_ERR;
		goto END;
	}

	BiPlgTrisMap(map,cnv,1,org);
	ret=BiPolygToConvex2(nCvx,tmp,cnv);
	if(ret==BI_OK)
	{
		ret=BiPolygToConvexUnmap(cvx,*nCvx,tmp,nMp,map);
	}

END:
	BiPlgTrisFreePlg2(1,cnv);
	BiFree(tmp);
	BiFree(map);
	BiFree(cnv);
	BiFree(tr2);
	return ret;
}

static int BiPolygToConvexUnmap(BIPOLYGON cvx[],int nCvx,BIPOLYGON2 cvx2[],int nMp,BI3VS2 *map)
{
	int i,j,k;
	BIPOLYGON *plg;
	BIPOLYGON2 *plg2;
	for(i=0; i<nCvx; i++)
	{
		plg=&cvx[i];
		plg2=&cvx2[i];

		plg->np=plg2->np;
		plg->p=(BIPOINT *)BiMalloc(plg2->np*sizeof(BIPOINT));
		if(plg->p==NULL)
		{
			return BI_ERR;
		}
		for(j=0; j<plg2->np; j++)
		{
			BiSetPoint(&plg->p[j],0,0,0);
			for(k=0; k<nMp; k++)
			{
				if(BiSamePoint2(&plg2->p[j],&map[k].cnv)==BI_TRUE)
				{
					plg->p[j]=map[k].org;
					break;
				}
			}
		}
		BiFree(plg2->p);
	}
	return BI_OK;
}

static int BiCheckStraight(BIPOINT2 *a,BIPOINT2 *b,BIPOINT2 *c)
{
	BIPOINT2 v1,v2,zero={0.0F,0.0F};
	BiSubPoint2(&v1,b,a);
	BiSubPoint2(&v2,c,b);
	if(BiSamePoint2(&v1,&zero)==BI_TRUE || BiSamePoint2(&v2,&zero)==BI_TRUE)
	{
		return BI_ERR;
	}
	else
	{
		real x2y1,x1y2;
		x2y1=v2.x*v1.y;
		x1y2=v1.x*v2.y;
		if(BiSame(x2y1,x1y2)==BI_TRUE)
		{
			return BI_ERR;
		}
	}
	return BI_OK;
}

static int BiCheckStraightEdge(BIPOLYGON2 *plg)
{
	int i1,i2,i3;
	for(i1=0; i1<plg->np; i1++)
	{
		i2=(i1+1)%plg->np;
		i3=(i1+2)%plg->np;
		if(BiCheckStraight(&plg->p[i1],&plg->p[i2],&plg->p[i3])!=BI_OK)
		{
			return BI_ERR;
		}
	}
	return BI_OK;
}

static int BiRemoveStraightEdgeSeq(BIPOLYGON2 *neo,BIPOLYGON2 *org)
{
	int i1,i2,i3;
	if(BiCopyPolyg2(neo,org)==BI_OK)
	{
		neo->np=1;
		for(i1=0; i1<org->np-1; i1++)
		{
			i2=(i1+1)%org->np;
			i3=(i1+2)%org->np;
			if(BiCheckStraight(&org->p[i1],&org->p[i2],&org->p[i3])==BI_OK)
			{
				neo->p[neo->np]=org->p[i2];
				neo->np++;
			}
		}
		if(BiCheckStraight(&neo->p[neo->np-1],&neo->p[0],&neo->p[1])!=BI_OK)
		{
			int i;
			for(i=0; i<neo->np-1; i++)
			{
				neo->p[i]=neo->p[i+1];
			}
		}
		return BI_OK;
	}
	return BI_ERR;
}

static int BiPolygToConvex2Loop(int *nCvx,BIPOLYGON2 cvx[],BIPOLYGON2 *org);

int BiPolygToConvex2(int *nCvx,BIPOLYGON2 cvx[],BIPOLYGON2 *org)
{
	*nCvx=0;
	return BiPolygToConvex2Loop(nCvx,cvx,org);
}

static int BiPolygToConvex2Loop(int *nCvx,BIPOLYGON2 cvx[],BIPOLYGON2 *org)
{
	if(BiCheckConvex2(org)==BI_TRUE)
	{
		int i;
		if((cvx[*nCvx].p=(BIPOINT2 *)BiMalloc(org->np*sizeof(BIPOINT2)))!=NULL)
		{
			cvx[*nCvx].np=org->np;
			for(i=0; i<org->np; i++)
			{
				cvx[*nCvx].p[i]=org->p[i];
			}
			(*nCvx)++;
			return BI_OK;
		}
		(*nCvx)=0;
		return BI_ERR;
	}
	else
	{
		BIPOLYGON2 pl1,pl2;
		if(BiOnePolyg2TrisSep(&pl1,&pl2,org,BISEP_CONVEX)==BI_OK)
		{
			if(BiPolygToConvex2Loop(nCvx,cvx,&pl1)==BI_OK && BiPolygToConvex2Loop(nCvx,cvx,&pl2)==BI_OK)
			{
				BiFree(pl1.p);
				BiFree(pl2.p);
				return BI_OK;
			}
		}
		(*nCvx)=0;
		return BI_ERR;
	}
}

int BiCheckConvex2(BIPOLYGON2 *plg)
{
	if(plg->np<=3)
	{
		return BI_TRUE;
	}

	if(BiCheckStraightEdge(plg)==BI_ERR)
	{
		int r;
		BIPOLYGON2 neo;
		if(BiRemoveStraightEdgeSeq(&neo,plg)==BI_OK)
		{
			r=BiCheckConvex2(&neo);
			BiFreePolyg2(&neo);
			return r;
		}
		return BI_FALSE;
	}
	else
	{
		int i,j;
		BILIMLINE2 ln;
		for(i=0; i<plg->np-2; i++)
		{
			for(j=i+2; j<plg->np; j++)
			{
				if(i==0 && j==plg->np-1)
				{
					break;
				}
				ln.p1=plg->p[i];
				ln.p2=plg->p[j];
				if(BiOnePolyg2TrisSeparatability(plg,&ln)!=BI_TRUE)
				{
					return BI_FALSE;
				}
			}
		}
		return BI_TRUE;
	}
}

/*****************************************************************************/
int BiKillCLoop2(BIPOLYGON2 *dst,int nPl,BIPOLYGON2 *pl)
{
	int i,ret,nwNPlg;
	BIPOLYGON2 *nwPlg;

	if(nPl==1)
	{
		dst->np=pl->np;
		dst->p=(BIPOINT2 *)BiMalloc(pl->np*sizeof(BIPOINT2));
		if(dst->p!=NULL)
		{
			for(i=0; i<pl->np; i++)
			{
				dst->p[i]=pl->p[i];
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else if(nPl<1)
	{
		return BI_ERR;
	}
	else
	{
		if(BiDelCLoop(&nwNPlg,&nwPlg,nPl,pl)!=BI_OK)
		{
			return BI_ERR;
		}
		ret=BiKillCLoop2(dst,nwNPlg,nwPlg);
		BiFree(nwPlg->p);
		BiFree(nwPlg);
		return ret;
	}
}

static int BiDelCLoop(int *nwn,BIPOLYGON2 *(*nw),int nPl,BIPOLYGON2 *pl)
{
	int i,j,k,ex;
	int l,m;
	int clp,ppn,cpn;
	real lng,tmpLng;
	BIPOLYGON2 *ptr;

	ex=BI_ERR;
	ppn=-1;
	clp=-1;
	cpn=-1;
	lng=(real)0.0;
	for(i=1; i<nPl; i++)
	{
		for(j=0; j<pl[0].np; j++)
		{
			for(k=0; k<pl[i].np; k++)
			{
				ex=BiCloopDeletability(nPl,pl,i,j,k);
				tmpLng=BiCloopDelLng(pl,i,j,k);
				if(ex==BI_OK && (clp<0 || tmpLng<lng))
				{
					clp=i;
					ppn=j;
					cpn=k;
					lng=tmpLng;
				}
			}
		}
	}

	if(clp<0)
	{
		return BI_ERR;
	}

	i=clp;
	j=ppn;
	k=cpn;
	/* pl[i]を削除して、pl[0]にその頂点を加えた新たなPC Loopを作る */
	ptr=(BIPOLYGON2 *)BiMalloc((nPl-1)*sizeof(BIPOLYGON2));
	ptr[0].np=pl[0].np+pl[i].np+2;
	ptr[0].p=(BIPOINT2 *)BiMalloc(ptr[0].np*sizeof(BIPOINT2));
	m=0;
	for(l=0; l<=j; l++)
	{
		ptr[0].p[m]=pl[0].p[l];
		m++;
	}
	for(l=0; l<=pl[i].np; l++)
	{
		ptr[0].p[m]=pl[i].p[(l+k)%pl[i].np];
		m++;
	}
	for(l=j; l<pl[0].np; l++)
	{
		ptr[0].p[m]=pl[0].p[l];
		m++;
	}

	for(l=1; l<i; l++)
	{
		ptr[l]=pl[l];
	}
	for(l=i+1; l<nPl; l++)
	{
		ptr[l-1]=pl[l];
	}

	*nw=ptr;
	*nwn=nPl-1;
	return BI_OK;
}

/****************
  clp : 比較対象のC-Loop番号
  ppn : P-Loop上の頂点番号
  cpn : C-Loop上の頂点番号
****************/
static int BiCloopDeletability(int nPl,BIPOLYGON2 *pl,int clp,int ppn,int cpn)
{
	int i,j,sta;
	BILIMLINE2 l1,l2;
	BIPOINT2 chk;

	j=0;
	chk=pl[0].p[ppn];
	for(i=0; i<pl[0].np; i++)
	{
		if(BiSamePoint2(&pl[0].p[i],&chk)==BI_TRUE)
		{
			j++;
			if(j>=2)
			{
				return BI_ERR;
			}
		}
	}

	l1.p1=pl[0].p[ppn];
	l1.p2=pl[clp].p[cpn];
	for(i=0; i<nPl; i++)
	{
		for(j=0; j<pl[i].np; j++)
		{
			l2.p1=pl[i].p[j];
			l2.p2=pl[i].p[(j+1)%(pl[i].np)];
			sta=BiLine2CrossStatus(&l1,&l2);
			/* ? BI_CRS_ON_LINE is it correct? */
			if(sta!=BI_CRS_NOCROSS && sta!=BI_CRS_PARALLEL && sta!=BI_CRS_ON_LINE && sta!=BI_CRS_ONESIDE_SAME)
			{
				return BI_ERR;
			}
		}
	}
	return BI_OK;
}

static real BiCloopDelLng(BIPOLYGON2 *pl,int clp,int ppn,int cpn)
{
	BIPOINT2 p1,p2,v;
	p1=pl[0].p[ppn];
	p2=pl[clp].p[cpn];
	BiSubPoint2(&v,&p2,&p1);
	return v.x*v.x+v.y*v.y;
}

/*****************************************************************************/
int BiLine2CrossStatus(BILIMLINE2 *ln1,BILIMLINE2 *ln2)
{
	int parS,parT;
	real s,t;
	BILINE2 l1,l2;

	BiLimLineToLine2(&l1,ln1);
	BiLimLineToLine2(&l2,ln2);
	/* l1.o=ln1->p1;
	BiSubPoint2(&l1.v, &ln1->p2,&ln1->p1);
	l2.o=ln2->p1;
	BiSubPoint2(&l2.v, &ln2->p2,&ln2->p1); */

	parS=BiLineCrossS(&s,&l1,&l2);
	parT=BiLineCrossT(&t,&l1,&l2);
	if(parS!=BI_OK || parT!=BI_OK)
	{
		/* 平行なとき */
		return BiLine2CrossStateParallel(ln1,ln2);
	}
	else
	{
		/* 平行じゃないとき */
		return BiLine2CrossStateNotParallel(s,t);
	}
}

static int BiLine2CrossStateParallel(BILIMLINE2 *ln1,BILIMLINE2 *ln2)
{
	real a,b,c,d;
	BIPOINT2 v1,v2;

	if((BiSamePoint2(&ln1->p1,&ln2->p1)==BI_TRUE&& BiSamePoint2(&ln1->p2,&ln2->p2)==BI_TRUE) ||
	   (BiSamePoint2(&ln1->p1,&ln2->p2)==BI_TRUE&& BiSamePoint2(&ln1->p2,&ln2->p1)==BI_TRUE))
	{
		return BI_CRS_SAME;
	}

	BiSubPoint2(&v1,&ln1->p2,&ln1->p1);
	BiSubPoint2(&v2,&ln2->p1,&ln1->p1);

	a=v1.x;
	b=v1.y;
	c=v2.x;
	d=v2.y;
	if(BiSame(a*d-b*c,0.0)==BI_TRUE)
	{
		return BI_CRS_ON_LINE;
	}
	else
	{
		return BI_CRS_PARALLEL;
	}
}


static int BiLine2CrossStateNotParallel(real s,real t)
{
	if((BiSame(s,0.0)==BI_TRUE||BiSame(s,1.0)==BI_TRUE)&&
	   (BiSame(t,0.0)==BI_TRUE||BiSame(t,1.0)==BI_TRUE))
	{
		return BI_CRS_ONESIDE_SAME;
	}
	else if((BiSame(s,0.0)==BI_TRUE||BiSame(s,1.0)==BI_TRUE) && 0.0F<=t && t<=1.0F)
	{
		return BI_CRS_LN2_TOUCH_LN1;
	}
	else if((BiSame(t,0.0)==BI_TRUE||BiSame(t,1.0)==BI_TRUE) && 0.0F<=s && s<=1.0F)
	{
		return BI_CRS_LN1_TOUCH_LN2;
	}
	else if(0.0<=s && s<=1.0 && 0.0<=t && t<=1.0)
	{
		return BI_CRS_CROSS;
	}
	else
	{
		return BI_CRS_NOCROSS;
	}
}

int BiLine2CrossPoint(BIPOINT2 *o,BILINE2 *l1,BILINE2 *l2)
{
	real s;
	BIPOINT v;
	if(BiLineCrossS(&s,l1,l2)==BI_OK)
	{
		BiMulPoint2(&v,&l2->v, s);
		BiAddPoint2( o,&l2->o,&v);
		return BI_OK;
	}
	else
	{
		return BI_ERR;
	}
}

/**************************************************************************
  Line1: Org(x1,y1)  Vec(ax,ay)
  Line2: Org(x2,y2)  Vec(bx,by)

    x1+t*ax = x2+s*bx
    y1+t*ay = y2+s*by
            |
            V
    x1*ay+t*ax*ay = x2*ay+s*bx*ay
  -)y1*ax+t*ay*ax = y2*ax+s*by*ax
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    x1*ay-y1*ax   = x2*ay-y2*ax +s*bx*ay-s*by*ax

   s*(bx*ay-by*ax)=x1*ay-y1*ax -x2*ay+y2*ax

   s = (ay*(x1-x2)+ax*(y2-y1))/(bx*ay-by*ax)

**************************************************************************/

/* Return Value : Crossing Point is   l2->o + s*l2->v */
static int BiLineCrossS(real *s,BILINE2 *l1,BILINE2 *l2)
{
	real fr,x1,x2,y1,y2,ax,ay,bx,by;
	x1=l1->o.x;
	y1=l1->o.y;
	x2=l2->o.x;
	y2=l2->o.y;
	ax=l1->v.x;
	ay=l1->v.y;
	bx=l2->v.x;
	by=l2->v.y;
	fr=bx*ay-by*ax;
	if(BiAbs(fr)<=YSEPS)
	{
		return BI_ERR;
	}
	else
	{
		*s=(ay*(x1-x2)+ax*(y2-y1))/fr;
		return BI_OK;
	}
}

static int BiLineCrossT(real *t,BILINE2 *l1,BILINE2 *l2)
{
	return BiLineCrossS(t,l2,l1);
}






/* 封印
static int BiOnePolyg2TrisSep(BIPOLYGON2 *pl1,BIPOLYGON2 *pl2,BIPOLYGON2 *org,int strategy)
{
	int i,j,n1,n2,ex;
	BILIMLINE2 ln;
	BIPOINT2 vec;
	BIPOLYGON2 tmp1,tmp2;
	int mxEdg,tmpEdg,cvx1,cvx2;
	real lnLen,tmpLen;

	if(strategy==BISEP_CONVEX)
	{
		if(BiCopyPolyg2(&tmp1,org)!=BI_OK || BiCopyPolyg2(&tmp2,org)!=BI_OK)
		{
			return BI_ERR;
		}
	}

	n1=-1;
	n2=-1;
	lnLen=(real)0.0;
	mxEdg=0;
	for(i=0; i<org->np; i++)
	{
		for(j=org->np-1; j>i+1; j--)
		{
			ln.p1=org->p[i];
			ln.p2=org->p[j];
			ex=BiOnePolyg2TrisSeparatability(org,&ln);
			if(ex==BI_TRUE)
			{
				switch(strategy)
				{
				case BISEP_SHORTEST:
					BiSubPoint2(&vec,&ln.p1,&ln.p2);
					tmpLen=vec.x*vec.x+vec.y*vec.y;
					if(n1<0 || tmpLen<lnLen)
					{
						lnLen=tmpLen;
						n1=i;
						n2=j;
					}
					break;
				case BISEP_CONVEX:
					BiExecSeparate2(&tmp1,&tmp2,org,i,j);
					cvx1=BiCheckConvex2(&tmp1);
					cvx2=BiCheckConvex2(&tmp2);
					if(cvx1==BI_TRUE || cvx2==BI_TRUE)
					{
						if(cvx1==BI_TRUE && cvx2!=BI_TRUE)
						{
							tmpEdg=tmp1.np;
						}
						else if(cvx1!=BI_TRUE && cvx2==BI_TRUE)
						{
							tmpEdg=tmp2.np;
						}
						else
						{
							tmpEdg=BiLarger(tmp1.np,tmp2.np);
						}
						if(tmpEdg>mxEdg)
						{
							mxEdg=tmpEdg;
							n1=i;
							n2=j;
						}
					}
					else if(n1<0)
					{
						n1=i;
						n2=j;
					}
					break;
				}
			}
		}
	}

	if(n1<0)
	{
		return BI_ERR;
	}

	i=n1;
	j=n2;
	pl1->np= j-i+1;
	pl1->p=(BIPOINT2 *)BiMalloc(pl1->np*sizeof(BIPOINT2));
	pl2->np= org->np -(j-i) +1;
	pl2->p=(BIPOINT2 *)BiMalloc(pl2->np*sizeof(BIPOINT2));
	if(pl1->p==NULL || pl2->p==NULL)
	{
		BiFree(pl1->p);
		BiFree(pl2->p);
		return BI_ERR;
	}

	BiExecSeparate2(pl1,pl2,org,i,j);
	return BI_OK;
}
*/

