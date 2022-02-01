#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impulse.h"
#include "i3dg.h"


extern int BiPrjMode;
extern real BiOrthoDist;


int BiBboxInsideViewPort(int *clip,BIPOINT *bbox)
{
	BIPOINT edg[8];
	edg[0]=bbox[0];
	BiSetPoint(&edg[1],bbox[0].x,bbox[1].y,bbox[0].z);
	BiSetPoint(&edg[2],bbox[1].x,bbox[0].y,bbox[0].z);
	BiSetPoint(&edg[3],bbox[1].x,bbox[1].y,bbox[0].z);
	edg[4]=bbox[1];
	BiSetPoint(&edg[5],bbox[0].x,bbox[1].y,bbox[1].z);
	BiSetPoint(&edg[6],bbox[1].x,bbox[0].y,bbox[1].z);
	BiSetPoint(&edg[7],bbox[0].x,bbox[0].y,bbox[1].z);
	return BiCheckInsideViewPort(clip,8,edg);
}



int BiCheckInsideViewPortOrtho(int *clip,int np,BIPOINT *p)
{
	int i,nea,fa,left,right,up,down;
	real x,y;

	nea=1;
	left=1;
	right=1;
	up=1;
	down=1;
	fa=1;

	*clip=BI_OFF;
	for(i=0; i<np; i++)
	{
		x=p[i].x/BiOrthoDist;
		y=p[i].y/BiOrthoDist;

		if(p[i].z >= BiPrj.nearz)
		{
			nea=0;
		}
		else
		{
			*clip=BI_ON;
		}

		if(p[i].z <= BiPrj.farz)
		{
			fa=0;
		}

		if(x<=BiPrjFovRight)
		{
			right=0;
		}
		if(x>=-BiPrjFovLeft)
		{
			left=0;
		}
		if(y<=BiPrjFovUp)
		{
			up=0;
		}
		if(y>=-BiPrjFovDown)
		{
			down=0;
		}
	}

	return ((nea+fa+up+down+right+left)==0 ? BI_IN : BI_OUT);
}

int BiCheckInsideViewPortPers(int *clip,int np,BIPOINT *p)
{
	int i,nea,fa,left,right,up,down;
	real x,y;

	nea=1;
	left=1;
	right=1;
	up=1;
	down=1;
	fa=1;

	*clip=BI_OFF;
	for(i=0; i<np; i++)
	{
		x=p[i].x/BiLarger(p[i].z,BiPrj.nearz);
		y=p[i].y/BiLarger(p[i].z,BiPrj.nearz);

		if(p[i].z >= BiPrj.nearz)
		{
			nea=0;
		}
		else
		{
			*clip=BI_ON;
		}

		if(p[i].z <= BiPrj.farz)
		{
			fa=0;
		}

		if(x<=BiPrjFovRight)
		{
			right=0;
		}
		if(x>=-BiPrjFovLeft)
		{
			left=0;
		}
		if(y<=BiPrjFovUp)
		{
			up=0;
		}
		if(y>=-BiPrjFovDown)
		{
			down=0;
		}
	}

	return ((nea+fa+up+down+right+left)==0 ? BI_IN : BI_OUT);
}

int BiCheckInsideViewPort(int *clip,int np,BIPOINT *p)
{
	switch(BiPrjMode)
	{
	case BIPRJ_ORTHO:
		return BiCheckInsideViewPortOrtho(clip,np,p);
	default:
	case BIPRJ_PERS:
		return BiCheckInsideViewPortPers(clip,np,p);
	}
}




