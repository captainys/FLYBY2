#include <stdio.h>
#include <stddef.h>
#include "impulse.h"



static int BiTwist2One(int np,BIPOINT2 *p,int n);
static void BiTwistVector2(BIPOINT2 *vec,int np,BIPOINT2 *p,int n);
static int BiTwoLineCross(BIPOINT2 *a1,BIPOINT2 *avec,BIPOINT2 *b1,BIPOINT2 *b2);



#define MALLOCFUNC BiMalloc
#define FREEFUNC BiFree



/* public */int BiTwist3(int np,BIPOINT *p,BIPOINT *nom)
{
	int i,tw;
	BIPOINT plnPrj,zVec;
	BIPOINT2 *tmp;
	BIPOSATT loc;
	BIAXIS axs;

	if(p[0].x==p[np-1].x && p[0].y==p[np-1].y && p[0].z==p[np-1].z)
	{
		np--;
	}

	tmp = (BIPOINT2 *)MALLOCFUNC(np*sizeof(BIPOINT2));
	if(tmp==NULL)
	{
		goto ERROREND;
	}

	BiRightAngleVector(&zVec,nom);

	loc.p=p[0];
	BiVectorToAngle(&loc.a,&zVec,nom);
	BiPntAngToAxis(&axs,&loc);

	for(i=0; i<np; i++)
	{
		BiConvGtoL(&plnPrj,&p[i],&axs);
		tmp[i].x = plnPrj.x;
		tmp[i].y = plnPrj.z;
	}

	tw = BiTwist2(np,tmp);

	FREEFUNC(tmp);
	return tw;

ERROREND:
	if(tmp!=NULL)
	{
		FREEFUNC(tmp);
	}
	return BITWIST_UNKNOWN;
}



/* public */int BiTwist2(int np,BIPOINT2 *p)
{
	int i,tw;

	if(p[0].x==p[np-1].x && p[0].y==p[np-1].y)
	{
		np--;
	}

	for(i=0; i<np; i++)
	{
		tw = BiTwist2One(np,p,i);
		if(tw!=BITWIST_UNKNOWN)
		{
			return tw;
		}
	}
	return BITWIST_UNKNOWN;
}

enum {
	TWCRS_ONVTX,
	TWCRS_INSIDE,
	TWCRS_OUTSIDE,
	TWCRS_NOCROSS
};

/*
  p[n] -> p[n+1] の辺を基準辺として、右回りか左回りを判定する.
*/
static int BiTwist2One(int np,BIPOINT2 *p,int n)
{
	int i,nCrs;
	BIPOINT2 vec;
	BIPOINT2 a1,avec,b1,b2;

	BiTwistVector2(&vec,np,p,n);
	if(BiAbs(vec.x)+BiAbs(vec.y)<=YSEPS)
	{
		return BITWIST_UNKNOWN;
	}

	a1.x = p[n].x+vec.x/2;
	a1.y = p[n].y+vec.y/2;
	avec.x =-vec.y;
	avec.y = vec.x;

	/* 左回りと仮定して計算してみる */
	/* a1   : 基準辺の中点 */
	/* avec : 左回りと仮定したとき,a1から内側を向いているべきベクトル */
	/* b1   : 交点計算する辺の始点 */
	/* b2   : 交点計算する辺の終点 */
	nCrs=0;
	for(i=0; i<np; i++)
	{
		if(i==n)
		{
			continue;
		}

		BiTwistVector2(&vec,np,p,i);
		b1.x = p[i].x;
		b1.y = p[i].y;
		b2.x = b1.x+vec.x;
		b2.y = b1.y+vec.y;

		/* 交点計算して,交点があれば交点数を加算 */
		/* 頂点に接する場合は,結果を保留する */
		switch(BiTwoLineCross(&a1,&avec,&b1,&b2))
		{
		case TWCRS_INSIDE:
			nCrs++;
			break;
		case TWCRS_ONVTX:
			return BITWIST_UNKNOWN;
		}
	}

	if((nCrs%2)==0)
	{
		return BITWIST_RIGHT;
	}
	else
	{
		return BITWIST_LEFT;
	}
}

static void BiTwistVector2(BIPOINT2 *vec,int np,BIPOINT2 *p,int n)
{
	BIPOINT2 *org;
	if(n==np-1)
	{
		org = &p[0];
	}
	else
	{
		org = &p[n+1];
	}
	BiSubPoint2(vec,org,&p[n]);
}


/* ２次元半直線 a と、２次元線分 b の交点チェック */
static int BiTwoLineCross(BIPOINT2 *a1,BIPOINT2 *avec,BIPOINT2 *b1,BIPOINT2 *b2)
{
	real t,s,fra,frb;
	BIPOINT2 c,p,q;

	BiSubPoint2(&c, b1,a1);

	p = *avec;
	BiSubPoint2(&q, b2,b1);
	fra = c.x*p.y -c.y*p.x;
	frb = q.y*p.x -q.x*p.y;

	if(BiAbs(frb)<=YSEPS)  /* 交点が計算できない */
	{
		/* printf("BiTwoLineCross@itwist.c : frb=0\n"); */
		return TWCRS_NOCROSS;
	}

	t = fra/frb;
	if(BiAbs(p.x)>YSEPS)
	{
		s = (c.x+q.x*t)/p.x;
	}
	else if(BiAbs(p.y)>YSEPS)
	{
		s = (c.y+q.y*t)/p.y;
	}
	else
	{
		/* printf("BiTwoLineCross@itwist.c : Can't calc S value.\n"); */
		return TWCRS_NOCROSS;
	}

	/* 交点 = b1 + q * fra / frb */

	if(s<=0)                 /* 半直線の範囲外 */
	{
		/* printf("BiTwoLineCross@itwist.c : BEHIND\n"); */
		return TWCRS_NOCROSS;
	}
	else if(BiAbs(t)<=YSEPS || BiAbs(1.0-t)<=YSEPS)  /* 線分Bの端点上 */
	{
		/* printf("BiTwoLineCross@itwist.c : ONVTX %d\n",t); */
		return TWCRS_ONVTX;
	}
	else if(t<YSEPS || 1.0-YSEPS<t)    /* 線分Bの範囲外 */
	{
		/* printf("BiTwoLineCross@itwist.c : OUTSIDE %d\n",t); */
		return TWCRS_OUTSIDE;
	}
	else                     /* 線分Bの内部の点と交わる */
	{
		/* printf("BiTwoLineCross@itwist.c : INSIDE %d\n",t); */
		return TWCRS_INSIDE;
	}
}
