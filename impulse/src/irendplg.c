#include <stdio.h>
#include <stddef.h>

#include "impulse.h"
#include "irender.h"




static void GetPointN(long *x,long *y,int n);
static void InsertCrossPoint(long x);
static void MakeCrossPoints(long y);


static int np=0;
static long pnt[BI_RENDER_MAX_VTX*2];
static int ncrs=0;
static long crsY,crs[BI_RENDER_MAX_VTX];
static long lupx,lupy,rdwx,rdwy;
static long LastDy;

int BiRenderStartPolygon(int n,long *p)
{
	int i;
	long x1,y1,x2,y2;

	np=0;
	crsY=0;
	ncrs=0;

	if(n<3)
	{
		goto ERREND;
	}

	np=n;
	lupx=p[0];
	lupy=p[1];
	rdwx=p[0];
	rdwy=p[1];
	for(i=0; i<n; i++)
	{
		pnt[i*2  ]=p[i*2  ];
		pnt[i*2+1]=p[i*2+1];
		lupx=BiSmaller(lupx,p[i*2  ]);
		lupy=BiSmaller(lupy,p[i*2+1]);
		rdwx=BiLarger(rdwx,p[i*2  ]);
		rdwy=BiLarger(rdwy,p[i*2+1]);
	}

	LastDy=0;
	for(i=0; i<np; i++)
	{
		GetPointN(&x1,&y1,i  );
		GetPointN(&x2,&y2,i+1);
		if(y1!=y2)
		{
			LastDy=y2-y1;
		}
	}

	return BI_TRUE;
ERREND:
	return BI_FALSE;
}

int BiRenderEndPolygon(void)
{
	if(pnt!=NULL)
	{
		np=0;
		ncrs=0;
		return BI_TRUE;
	}
	else
	{
		return BI_FALSE;
	}
}

int BiRenderCheckInside(long x,long y)
{
	int i;

	if(x<lupx || y<lupy || x>rdwx || y>rdwy)
	{
		return BI_FALSE;
	}

	if(LastDy==0)
	{
		if(y==lupy && lupx<=x && x<=rdwx)
		{
			return BI_TRUE;
		}
	}

	if(ncrs==0 || crsY!=y)
	{
		MakeCrossPoints(y);
	}

	for(i=0; i<ncrs; i+=2)
	{
		if((crs[i]<x && x<crs[i+1]) || crs[i]==x)
		{
			return BI_TRUE;
		}
	}

	return BI_FALSE;
}

int BiRenderGetCrossPoint(int *nx,long *x,long y)
{
	int i;

	if(y<lupy || y>rdwy)
	{
		*nx=0;
		return BI_TRUE;
	}

	if(ncrs==0 || crsY!=y)
	{
		MakeCrossPoints(y);
	}

	*nx=ncrs;
	for(i=0; i<ncrs; i++)
	{
		x[i]=crs[i];
	}
	return BI_TRUE;
}

static void GetPointN(long *x,long *y,int n)
{
	n=n%np;
	*x=pnt[n*2  ];
	*y=pnt[n*2+1];
}

static void MakeCrossPoints(long y)
{
	int i;
	long x,x1,y1,x2,y2,dy,lDy;
	long swa,swb;

	crsY=y;
	ncrs=0;
	lDy=LastDy;
	for(i=0; i<np; i++)
	{
		GetPointN(&x1,&y1,i  );
		GetPointN(&x2,&y2,i+1);
		dy=y2-y1;
		if(dy==0)
		{
			continue;
		}
		if((y<y1 && y<y2) || (y>y1 && y>y2) || (lDy*dy>=0 && y1==y))
		{
			continue;
		}

		if(y1>y2)
		{
			swa=x1; swb=x2;
			x1=swb; x2=swa;
			swa=y1; swb=y2;
			y1=swb; y2=swa;
		}
		x=(long)((real)(x2-x1)*(real)(y-y1)/(real)(y2-y1)+(real)x1);
		InsertCrossPoint(x);
		lDy=dy;
	}
}

static void InsertCrossPoint(long x)
{
	int i,j;

	for(i=0; i<ncrs; i++)
	{
		if(x<crs[i])
		{
			break;
		}
	}
	for(j=ncrs; j>i; j--)
	{
		crs[j]=crs[j-1];
	}
	crs[i]=x;
	ncrs++;
}
