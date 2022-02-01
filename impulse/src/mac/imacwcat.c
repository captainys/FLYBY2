/* WinG Bitmap Direct Access Manager   :  Code Name = WildCat */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "impulse.h"
#include "imacwcat.h"



/***********************************************************************
void WcGetColorIndex(unsigned short *idx,WCCOLOR *col);
***********************************************************************/
int WcGetColorIndex(unsigned short *idx,WCCOLOR *col)
{
	*idx=(((col->r>>3)&31)<<10)+(((col->g>>3)&31)<<5)+((col->b>>3)&31);
	return WC_OK;
}



/***********************************************************************
int WcCreateBmp(WCBMP *bm,int wid,int hei);
    Return
      0 No Error
      1 Error Occured
***********************************************************************/
static int WcCreateLineTop(WCBMP *bm,int wid,int hei,unsigned long rowbyte);
static int WcCreatePolygBuffer(WCBMP *bm,int wid,int hei);


int WcCreateBmp(WCBMP *bm,int wid,int hei,unsigned char *bmp,unsigned long rowbyte)
{
	bm->dat=(unsigned short *)bmp;
	if(WcCreateLineTop(bm,wid,hei,rowbyte)==WC_OK)
	{
		bm->wid=wid;
		bm->hei=hei;
		bm->alive=1;
		return 0;
	}
	bm->alive=0;
	return 1;
}

static int WcCreateLineTop(WCBMP *bm,int wid,int hei,unsigned long rowbyte)
{
	int y;
	unsigned short *(*top);
	unsigned short *ptr;

	top=(unsigned short *(*))BiMalloc(hei*sizeof(unsigned short *));
	if(top!=NULL)
	{
		ptr=bm->dat;
		for(y=0; y<hei; y++)
		{
			top[y]=(unsigned short *)ptr;
			ptr+=(rowbyte/sizeof(unsigned short));
		}
		if(WcCreatePolygBuffer(bm,wid,hei)==WC_OK)
		{
			bm->hTop=top;
			return WC_OK;
		}
		BiFree(top);
	}
	return WC_ERR;
}

static int WcCreatePolygBuffer(WCBMP *bm,int wid,int hei)
{
	int i;
	bm->rgn.y1=0;
	bm->rgn.y2=0;
	bm->rgn.x=(int *)BiMalloc(WC_RGN_X_PER_LINE*hei*sizeof(int));
	if(bm->rgn.x!=NULL)
	{
		for(i=0; i<hei; i++)
		{
			bm->rgn.x[i*WC_RGN_X_PER_LINE]=0;
		}
		return WC_OK;
	}
	return WC_ERR;
}


/***********************************************************************
void WcDeleteBmp(WCBMP *bm);
***********************************************************************/
int WcDeleteBmp(WCBMP *bm)
{
	if(bm!=NULL && bm->alive==1)
	{
		BiFree(bm->hTop);
		BiFree(bm->rgn.x);
		bm->alive=0;
	}
	return WC_OK;
}


/***********************************************************************
void WcPset(WCBMP *bm,int x,int y,WCCOLOR *col);
***********************************************************************/
void WcPset(WCBMP *bm,int x,int y,WCCOLOR *col)
{
	unsigned short c;
	WcGetColorIndex(&c,col);
	bm->hTop[y][x]=c;
}


/***********************************************************************
void WcLine(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col);
***********************************************************************/
void WcLine(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col)
{
	unsigned short c;
	int x,y,vx,vy;
	int dx,dy,sum;

	WcGetColorIndex(&c,col);

	x=x1;
	y=y1;
	dx=WcAbs(x2-x1);
	dy=WcAbs(y2-y1);
	vx=WcSgn(x2-x1);
	vy=WcSgn(y2-y1);
	sum=0;
	if(dx==0)
	{
		while(dy>=0)
		{
			bm->hTop[y][x]=c;
			y+=vy;
			dy--;
		}
	}
	else if(dy==0)
	{
		while(dx>=0)
		{
			bm->hTop[y][x]=c;
			x+=vx;
			dx--;
		}
	}
	else if(dx>dy)
	{
		while(x!=x2 || y!=y2)
		{
			bm->hTop[y][x]=c;
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
		bm->hTop[y][x]=c;
	}
	else if(dx<dy)
	{
		while(x!=x2 || y!=y2)
		{
			bm->hTop[y][x]=c;
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
		bm->hTop[y][x]=c;
	}
	else /* dx==dy */
	{
		while(x!=x2)
		{
			bm->hTop[y][x]=c;
			x+=vx;
			y+=vy;
		}
		bm->hTop[y][x]=c;
	}
}


/* For More Functions (^_^;) */
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
