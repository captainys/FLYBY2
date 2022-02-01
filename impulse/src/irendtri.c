#include <stdio.h>
#include <stddef.h>
#include <math.h>

#include "impulse.h"
#include "irender.h"


static int TsMainLoop(int np,int *idx);
static void InsertTriangle(int *idx);
static int CheckSeparatability(int np,int *idx,int a,int b);
static int PolygonInside(int np,int *idx,BIPOINT2 *c);
static int MakeSeparation(int *np1,int *idx1,int np,int *idx,int a,int b);
static int TwoLineCross(BIPOINT2 *a1,BIPOINT2 *a2,BIPOINT2 *b1,BIPOINT2 *b2);


static int nTri,*tri;
static int nPnt;
static BIPOINT2 *pnt;


int BiRenderTrianglize(int *nt,int *t,int np,BIPOINT *p)
{
	int i;
	real mulx,muly;
	BIPOINT nom,cen,eye,tmp;
	BIANGLE ang;
	BIAXIS axs;
	BIPOSATT pos;
	BIPOINT2 pnt2[BI_RENDER_MAX_VTX],min,max;

	if(np<3 || BI_RENDER_MAX_VTX<np)
	{
		*nt=0;
		return BI_FALSE;
	}

	BiAverageNormalVector(&nom,np,p);
	BiSubPoint(&eye,&p[1],&p[0]);
	BiVectorToAngle(&ang,&eye,&nom);

	BiSetPoint(&cen,0,0,0);
	for(i=0; i<np; i++)
	{
		BiAddPoint(&cen,&cen,&p[i]);
	}
	BiDivPoint(&cen,&cen,np);

	pos.p=cen;
	pos.a=ang;
	BiPntAngToAxis(&axs,&pos);

	BiConvGtoL(&tmp,&p[0],&axs);
	BiSetPoint2(&min,tmp.x,tmp.z);
	BiSetPoint2(&max,tmp.x,tmp.z);
	for(i=0; i<np; i++)
	{
		BiConvGtoL(&tmp,&p[i],&axs);
		pnt2[i].x=tmp.x;
		pnt2[i].y=tmp.z;
		BiSetPoint2(&min,BiSmaller(min.x,tmp.x),BiSmaller(min.y,tmp.y));
		BiSetPoint2(&max,BiLarger(max.x,tmp.x) ,BiLarger(max.y,tmp.y));
	}

	if(min.x==max.x || min.y==max.y)
	{
		return BI_FALSE;
	}
	else
	{
		if(max.x-min.x<256)
		{
			mulx=256/(max.x-min.x);
			for(i=0; i<np; i++)
			{
				pnt2[i].x*=mulx;
			}
		}
		if(max.y-min.y<256)
		{
			muly=256/(max.y-min.y);
			for(i=0; i<np; i++)
			{
				pnt2[i].y*=muly;
			}
		}
	}

	return BiRenderTrianglize2(nt,t,np,pnt2);
}

int BiRenderTrianglize2(int *nt,int *t,int np,BIPOINT2 *p)
{
	int ret;
	int i,idx[BI_RENDER_MAX_VTX];

	/* printf("Begin Separate.\n"); */

	nTri=0;
	tri=t;
	nPnt=np;
	pnt=p;

	for(i=0; i<np; i++)
	{
		idx[i]=i;
	}

	ret=TsMainLoop(np,idx);

	*nt=nTri;
	return ret;
}

static int TsMainLoop(int np,int *idx)
{
	int sp1,sp2;
	real lng,tmpLng;
	int i,j,np1,idx1[BI_RENDER_MAX_VTX],np2,idx2[BI_RENDER_MAX_VTX],ret;

	/* printf("NP : %d\n",np); */

	if(idx[0]==idx[np-1])
	{
		return TsMainLoop(np-1,idx);
	}

	if(np<3)
	{
		/* printf("TsMainLoop@triangle.c\n"); */
		/* printf("Polygon of 2 points detected.\n"); */
		return BI_FALSE;
	}

	if(np==3)
	{
		InsertTriangle(idx);
		return BI_TRUE;
	}

	sp1=-1;
	sp2=-1;
	for(i=0; i<np; i++)
	{
		for(j=i+2; j<np; j++)
		{
			if(CheckSeparatability(np,idx,i,j)==BI_TRUE)
			{
				BIPOINT2 vec;
				BiSubPoint2(&vec,&pnt[idx[i]],&pnt[idx[j]]);
				tmpLng=BiLengthPoint2(&vec);
				if(sp1<0 || tmpLng<lng)
				{
					sp1=i;
					sp2=j;
					lng=tmpLng;
				}
			}
		}
	}

	if(sp1>=0)
	{
		if(MakeSeparation(&np1,idx1,np,idx,sp1,sp2)!=BI_TRUE ||
		   MakeSeparation(&np2,idx2,np,idx,sp2,sp1)!=BI_TRUE)
		{
			/* printf("TsMainLoop@triangle.c\n"); */
			/* printf("Failed To Make Separated Polygons.\n"); */
			goto ERREND;
		}

		if(TsMainLoop(np1,idx1)==BI_TRUE &&
		   TsMainLoop(np2,idx2)==BI_TRUE)
		{
			ret=BI_TRUE;
		}
		else
		{
			ret=BI_FALSE;
		}
		return ret;
	}

	/* printf("TsMainLoop@triangle.c\n"); */
	/* printf("Separation Failure(Maybe crossing edge exists).\n"); */
ERREND:
	return BI_FALSE;
}

static void InsertTriangle(int *idx)
{
	tri[nTri*3  ]=idx[0];
	tri[nTri*3+1]=idx[1];
	tri[nTri*3+2]=idx[2];
	nTri++;
}

static int CheckSeparatability(int np,int *idx,int a,int b)
{
	int i,i1,i2;
	BIPOINT2 p1,p2; /* 切れるかチェックする2点 */
	BIPOINT2 q1,q2; /* 干渉計算する辺の2点 */
	BIPOINT2 c;

	p1=pnt[idx[a]];
	p2=pnt[idx[b]];

	/* 隣接する点ではカットできない */
	if(BiAbs(a-b)<=1 || (a==0 && b==np-1) || (a==np-1 && b==0))
	{
		/* printf("%d %d 隣接点\n",a,b); */
		return BI_FALSE;
	}

	/* 二点で出来る線分と他の辺とが交わっていてはならない */
	for(i=0; i<np; i++)
	{
		i1= i;
		i2=(i+1)%np;
		if(a==i1 || b==i1 || a==i2 || b==i2)
		{
			continue;
		}

		q1=pnt[idx[i1]];
		q2=pnt[idx[i2]];
		if(TwoLineCross(&p1,&p2,&q1,&q2)==BI_TRUE)
		{
			/* printf("%d %d 交点\n",a,b); */
			return BI_FALSE;
		}
	}

	/* 2点の中点(任意の点)はポリゴン内部でなくてはならない */
	c.x=(p1.x+p2.x)/2;
	c.y=(p1.y+p2.y)/2;
	if(PolygonInside(np,idx,&c)!=BI_TRUE)
	{
		/* printf("%d %d 中点外部\n",a,b); */
		return BI_FALSE;
	}

	/* printf("%d %d 確定\n",a,b); */
	return BI_TRUE;
}

static int PolygonInside(int np,int *idx,BIPOINT2 *c)
{
	int i,sw;
	long p[BI_RENDER_MAX_VTX*2];

	for(i=0; i<np; i++)
	{
		p[i*2  ]=(int)pnt[idx[i]].x;
		p[i*2+1]=(int)pnt[idx[i]].y;
	}

	BiRenderStartPolygon(np,p);
	sw=BiRenderCheckInside((int)c->x,(int)c->y);
	BiRenderEndPolygon();

	return sw;
}

static int MakeSeparation(int *np1,int *idx1,int np,int *idx,int a,int b)
{
	int i,n,sta;

	sta=-1;
	for(i=0; i<np; i++)
	{
		if(idx[i]==idx[a])
		{
			sta=i;
			break;
		}
	}

	if(sta<0)
	{
		return BI_FALSE;
	}

	(*np1)=0;
	for(i=sta; i<sta+np; i++)
	{
		n=i%np;
		idx1[*np1]=idx[n];
		(*np1)++;

		if(idx[n]==idx[b])
		{
			return BI_TRUE;
		}
	}

	return BI_FALSE;
}

static int TwoLineCross(BIPOINT2 *a1,BIPOINT2 *a2,BIPOINT2 *b1,BIPOINT2 *b2)
{
	real fra,frb;
	BIPOINT2 c,p,q,r;

	c.x = b1->x -a1->x;
	c.y = b1->y -a1->y;

	p.x = a2->x-a1->x;
	p.y = a2->y-a1->y;

	q.x = b2->x-b1->x;
	q.y = b2->y-b1->y;

	fra = c.x*p.y -c.y*p.x;
	frb = q.y*p.x -q.x*p.y;

	if(frb==0)  /* 交点が計算できない */
	{
		/* printf("TwoLineCross@triangle.c : frb=0\n"); */
		return BI_FALSE;
	}

	/* 交点 = r + q * fra / frb */
	r.x = b1->x +q.x*fra/frb;
	r.y = b1->y +q.y*fra/frb;

	if((r.x < a1->x && r.x < a2->x) ||
	   (r.x > a1->x && r.x > a2->x) ||
	   (r.y < a1->y && r.y < a2->y) ||
	   (r.y > a1->y && r.y > a2->y) ||
	   (r.x < b1->x && r.x < b2->x) ||
	   (r.x > b1->x && r.x > b2->x) ||
	   (r.y < b1->y && r.y < b2->y) ||
	   (r.y > b1->y && r.y > b2->y))
	{
		return BI_FALSE;
	}

	return BI_TRUE;
}
