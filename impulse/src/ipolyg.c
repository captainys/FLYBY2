#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "impulse.h"



/************************************************************************

   1)y-y1=dy/dx*(x-x1)
   2)dx(y-y1)=dy(x-x1)
   3)dx*y-dx*y1=dy*x-dy*x1
   4)dy*x-dx*y-dy*x1+dx*y1=0

   ax+by+c=0
     a= dy
     b=-dx
     c=-dy*x1+dx*y1

   a1 x  +b1 y  +c1 = 0
   a2 x  +b2 y  +c2 = 0

   a2a1 x  +a2b1 y  +a2c1 = 0
   a1a2 x  +a1b2 y  +a1c2 = 0
  ----------------------------
         (a2b1-a1b2)y +a2c1-a1c2 = 0


   a1b2 x  +b1b2 y  +c1b2 = 0
   a2b1 x  +b2b1 y  +c2b1 = 0
  ----------------------------
   (a1b2-a2b1)x +c1b2-c2b1 = 0

************************************************************************/

static void BiLineGetCoefficiency(real *a,real *b,real *c,BILINE2 *l)
{
	*a= l->v.y;
	*b=-l->v.x;
	*c=-l->v.y*l->o.x +l->v.x*l->o.y;
}

static int BiLineCrossPoint2(BIPOINT2 *pnt,BILINE2 *l1,BILINE2 *l2)
{
	real a1,b1,c1;
	real a2,b2,c2;
	real p,q;
	real x,y;

	BiLineGetCoefficiency(&a1,&b1,&c1,l1);
	BiLineGetCoefficiency(&a2,&b2,&c2,l2);

	p=a2*b1-a1*b2;
	q=a2*c1-a1*c2;
	if(-0.0001<=p && p<=0.0001)
	{
		return BI_ERR;
	}
	y=-q/p;

	p=a1*b2-a2*b1;
	q=c1*b2-c2*b1;
	if(-0.0001<=p && p<=0.0001)
	{
		return BI_ERR;
	}
	x=-q/p;

	BiSetPoint2(pnt,x,y);
	return BI_OK;
}

/* If a line is just parallel, then, it can be considered as no crossing
   point. But, if two lines are same, it must be considered as crossing */
static int BiLinesAreJustParallel(BILINE2 *l1,BILINE2 *l2)
{
	real a1,b1,c1;
	real a2,b2,c2;
	real p,q;

	BiLineGetCoefficiency(&a1,&b1,&c1,l1);
	BiLineGetCoefficiency(&a2,&b2,&c2,l2);

	p=a2*b1-a1*b2;
	q=a2*c1-a1*c2;
	if(-0.0001<=p && p<=0.0001 && (q<=-0.0001 || 0.0001<=q))
	{
		return BI_OK;
	}

	p=a1*b2-a2*b1;
	q=c1*b2-c2*b1;
	if(-0.0001<=p && p<=0.0001 && (q<=-0.0001 || 0.0001<=q))
	{
		return BI_OK;
	}

	return BI_ERR;
}

static int BiPointIsOnTheEdge(BIPOINT2 *crs,BIPOINT2 *e1,BIPOINT2 *e2)
{
	BIPOINT2 v1,v2;

	BiSubPoint2(&v1,e1,crs);
	BiSubPoint2(&v2,e2,crs);

	if(v1.x*v2.x+v1.y*v2.y<0)
	{
		return BI_TRUE;
	}
	else
	{
		return BI_FALSE;
	}
}

#define RESULT_ON_BOUNDARY BI_OUT

int BiCheckInsidePolyg2(BIPOLYGON2 *plg,BIPOINT2 *pnt)
{
	int i,j,CrossCounter,vt1,vt2;
	long ang;
	BIPOINT2 vec,crs,cVec;
	BILINE2 l1,l2;

	ang=0;
	for(i=0; i<256; i++)
	{
		BiSetPoint2(&vec,BiCos(ang),BiSin(ang));

		l1.o=*pnt;
		l1.v=vec;

		CrossCounter=0;
		for(j=0; j<plg->np; j++)
		{
			vt1= j;
			vt2=(j+1)%plg->np;

			if(BiSamePoint2(&plg->p[vt1],&plg->p[vt2])==BI_TRUE)
			{
				continue;
			}

			BiMakeLine2(&l2,&plg->p[vt1],&plg->p[vt2]);
			if(BiLineCrossPoint2(&crs,&l1,&l2)==BI_OK)
			{
				if(BiSamePoint2(&crs,pnt)==BI_TRUE)
				{
					return RESULT_ON_BOUNDARY;
				}
				if(BiSamePoint2(&crs,&plg->p[vt1])==BI_TRUE ||
				   BiSamePoint2(&crs,&plg->p[vt2])==BI_TRUE)
				{
					goto NEXTANGLE;
				}

				BiSubPoint2(&cVec,&crs,pnt);
				if(cVec.x*vec.x+cVec.y*vec.y>0 &&
				   BiPointIsOnTheEdge(&crs,&plg->p[vt1],&plg->p[vt2])==BI_TRUE)
				{
					CrossCounter++;
				}
			}
			else if(BiLinesAreJustParallel(&l1,&l2)==BI_FALSE)
			{
				goto NEXTANGLE;
			}
		}
		if(CrossCounter%2==0)
		{
			return BI_OUT;
		}
		else
		{
			return BI_IN;
		}

	NEXTANGLE:
		ang+=16384+256;  /* n.e. 92degree */
	}
	return BI_OUT;
}
