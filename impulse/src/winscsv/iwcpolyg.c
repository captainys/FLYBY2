/* DIB Direct Access Manager   :  Code Name = WildCat */
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <memory.h>
/* #include "../impulse.h" */

#include <windowsx.h>
#include "iwildcat.h"



/***********************************************************************
int WcSetRegion(WCBMP *bm,int np,WCPNT *p);
***********************************************************************/
static void WcCleanRegion(WCBMP *bm);
static void WcGenerateRegion(WCBMP *bm,int np,WCPNT *p);
static void WcInsertX(int *hl,int x);
static void WcInsertPoint(WCRGN *rgn,int x,int y);
static void WcDrawRegionLine(WCBMP *bm,WCPNT *p1,WCPNT *p2,int *prevVy);
static void WcDDA(WCBMP *bm,int x1,int y1,int x2,int y2);

static int FirstVy,LastVy;

int WcSetRegion(WCBMP *bm,int np,WCPNT *p)
{
	if(bm->alive!=0)
	{
		WcCleanRegion(bm);
		if(np>=3)
		{
			WcGenerateRegion(bm,np,p);
			return WC_OK;
		}
	}
	return WC_ERR;
}

static void WcCleanRegion(WCBMP *bm)
{
	int y;
	bm->rgn.y1=bm->hei-1;
	bm->rgn.y2=0;
	for(y=0; y<bm->hei; y++)
	{
		bm->rgn.x[y*WC_RGN_X_PER_LINE  ]=0;
	}
}

#define WcInsertX(hl,x)                   \
{                                         \
	if((hl)[0]<WC_RGN_X_PER_LINE-2)       \
	{                                     \
		int i;                            \
		switch((hl)[0])                   \
		{                                 \
		case 0:                           \
			(hl)[0]=1;                    \
			(hl)[1]=(x);                  \
			break;                        \
		case 1:                           \
			if((hl)[1]<(x))               \
			{                             \
				(hl)[2]=(x);              \
				(hl)[0]=2;                \
				break;                    \
			}                             \
			else                          \
			{                             \
				(hl)[2]=(hl)[1];          \
				(hl)[1]=(x);              \
				(hl)[0]=2;                \
				break;                    \
			}                             \
			break;                        \
		default:                          \
			for(i=(hl)[0]+1; i>=1; i--)   \
			{                             \
				if(i==1 || (hl)[i-1]<(x)) \
				{                         \
					(hl)[i]=(x);          \
					break;                \
				}                         \
				else                      \
				{                         \
					(hl)[i]=(hl)[i-1];    \
				}                         \
			}                             \
			(hl)[0]++;                    \
			break;                        \
		}                                 \
	}                                     \
}

#define WcInsertPoint(rgn,xx,yy)                       \
{                                                      \
	WcInsertX(&(rgn)->x[(yy)*WC_RGN_X_PER_LINE],(xx)); \
}

static void WcGenerateRegion(WCBMP *bm,int np,WCPNT *p)
{
	int i,prevVy;

	FirstVy=0;
	LastVy=0;
	prevVy=0;

	bm->rgn.y1=(int)p[0].y;
	bm->rgn.y2=(int)p[0].y;
	for(i=0; i<np-1; i++)
	{
		WcDrawRegionLine(bm,&p[i],&p[i+1],&prevVy);
	}
	WcDrawRegionLine(bm,&p[np-1],&p[0],&prevVy);

	if((FirstVy<0 && LastVy>0) || (FirstVy>0 && LastVy<0))
	{
		WcInsertPoint(&bm->rgn,(unsigned)p[0].x,(unsigned)p[0].y);
	}
}

static void WcDrawRegionLine(WCBMP *bm,WCPNT *p1,WCPNT *p2,int *prevVy)
{
	int vy;

	vy=WcSgn(p2->y -p1->y);


	if(vy==0)
	{
		return;
	}


	if(p2->y < bm->rgn.y1)
	{
		bm->rgn.y1=(int)p2->y;
	}
	else if(p2->y > bm->rgn.y2)
	{
		bm->rgn.y2=(int)p2->y;
	}


	if((*prevVy>0 && vy<0) || (*prevVy<0 && vy>0))
	{
		WcInsertPoint(&bm->rgn,(unsigned)p1->x,(unsigned)p1->y);
	}

	if(vy!=0)
	{
		*prevVy=vy;
		LastVy=vy;
		if(FirstVy==0)
		{
			FirstVy=vy;
		}
	}

	WcDDA(bm,(int)p1->x,(int)p1->y,(int)p2->x,(int)p2->y);
}


static void WcDDA(WCBMP *bm,int x1,int y1,int x2,int y2)
{
	register int *hl;
	register int vhl;
	int x,y,vx,vy;
	int dx,dy,sum;

	dx=WcAbs(x2-x1);
	dy=WcAbs(y2-y1);
	vx=WcSgn(x2-x1);
	vy=WcSgn(y2-y1);
	sum=0;
	if(dy==0)
	{
		return;
	}
	else if(dx==0)
	{
		hl=&bm->rgn.x[y1*WC_RGN_X_PER_LINE];
		vhl=vy*WC_RGN_X_PER_LINE;
		while(dy>0)
		{
			hl+=vhl;
			WcInsertX(hl,x1);
			dy--;
		}
	}
	else if(dx>dy*4)
	{
		if(vx>0)
		{
			x=x1;
			y=y1;
			hl=&bm->rgn.x[y1*WC_RGN_X_PER_LINE];
			vhl=vy*WC_RGN_X_PER_LINE;
			while(y!=y2)
			{
				y+=vy;
				hl+=vhl;
				x=(int)((long)(x2-x1)*(long)(y-y1)/(long)(y2-y1))+x1;
				WcInsertX(hl,x);
			}
		}
		else
		{
			x=x1;
			y=y1;
			hl=&bm->rgn.x[y1*WC_RGN_X_PER_LINE];
			vhl=vy*WC_RGN_X_PER_LINE;
			while(y!=y2)
			{
				y+=vy;
				hl+=vhl;
				x=(int)((long)(x1-x2)*(long)(y-y2)/(long)(y1-y2))+x2;
				WcInsertX(hl,x);
			}
		}
	}
	else if(dx>dy)
	{
		if(vx>0)
		{
			x=x1;
			y=y1;
			hl=&bm->rgn.x[y1*WC_RGN_X_PER_LINE];
			vhl=vy*WC_RGN_X_PER_LINE;
			while(y!=y2)
			{
				if(sum>=0)
				{
					x++;
					sum=sum-dy;
				}
				else
				{
					x++;
					y+=vy;
					hl+=vhl;
					sum=sum-dy+dx;
					WcInsertX(hl,x);
				}
			}
		}
		else
		{
			x=x2;
			y=y2;
			hl=&bm->rgn.x[y2*WC_RGN_X_PER_LINE];
			vhl=vy*WC_RGN_X_PER_LINE;
			WcInsertX(hl,x);
			while(y!=y1+vy)
			{
				if(sum>=0)
				{
					x++;
					sum=sum-dy;
				}
				else
				{
					x++;
					y-=vy;
					sum=sum-dy+dx;
					hl-=vhl;
					WcInsertX(hl,x);
				}
			}
		}
	}
	else if(dx<dy)
	{
		if(vx>0)
		{
			x=x1;
			y=y1;
			hl=&bm->rgn.x[y1*WC_RGN_X_PER_LINE];
			vhl=vy*WC_RGN_X_PER_LINE;
			while(y!=y2)
			{
				if(sum<=0)
				{
					y+=vy;
					hl+=vhl;
					sum=sum+dx;
					WcInsertX(hl,x);
				}
				else
				{
					x+=vx;
					y+=vy;
					hl+=vhl;
					sum=sum+dx-dy;
					WcInsertX(hl,x);
				}
			}
		}
		else
		{
			x=x2;
			y=y2;
			hl=&bm->rgn.x[y2*WC_RGN_X_PER_LINE];
			vhl=vy*WC_RGN_X_PER_LINE;
			WcInsertX(hl,x);
			while(y!=y1+vy)
			{
				if(sum<=0)
				{
					y-=vy;
					hl-=vhl;
					sum=sum+dx;
					WcInsertX(hl,x);
				}
				else
				{
					x-=vx;
					y-=vy;
					hl-=vhl;
					sum=sum+dx-dy;
					WcInsertX(hl,x);
				}
			}
		}
	}
	else /* dx==dy */
	{
		x=x1;
		y=y1;
		hl=&bm->rgn.x[y1*WC_RGN_X_PER_LINE];
		vhl=vy*WC_RGN_X_PER_LINE;
		while(y!=y2)
		{
			x+=vx;
			y+=vy;
			hl+=vhl;
			WcInsertX(hl,x);
		}
	}
}



/***********************************************************************
int WcPaintRegion(WCBMP *bm,WCCOLOR *col);
***********************************************************************/
static void WcFillLine
	(unsigned char *bmp,int x1,int x2,register unsigned long pat);
static void WcFillLine16
	(unsigned short *bmp,int x1,int x2,register unsigned long pat);

int WcPaintRegion(WCBMP *bm,WCCOLOR *col)
{
	int c,y,x;
	int *xtb;
	unsigned long pat;
	WCRGN *rgn;

	rgn=&bm->rgn;
	if(rgn->y1<=rgn->y2)
	{
		xtb=&bm->rgn.x[rgn->y1*WC_RGN_X_PER_LINE];

		if(bm->bpp<16)
		{
			WcGetColorIndex8(&c,col);
			pat=(unsigned long)(((unsigned)c) + (((unsigned)c)<<8));
			pat=pat+(pat<<16);
			for(y=rgn->y1; y<=rgn->y2; y++)
			{
				for(x=1; x<xtb[0]; x+=2)
				{
					WcFillLine(bm->hTop[y],xtb[x],xtb[x+1],pat);
				}
				xtb+=WC_RGN_X_PER_LINE;
			}
		}
		else
		{
			WcGetColorIndex16(&c,col);
			pat=(unsigned long)(((unsigned)c) + (((unsigned)c)<<16));
			for(y=rgn->y1; y<=rgn->y2; y++)
			{
				for(x=1; x<xtb[0]; x+=2)
				{
					WcFillLine16((unsigned short *)bm->hTop[y],xtb[x],xtb[x+1],pat);
				}
				xtb+=WC_RGN_X_PER_LINE;
			}
		}
	}
	return WC_OK;
}

static void WcFillLine
	(unsigned char *bmp,int x1,int x2,register unsigned long pat)
{
	if(x1<=x2)
	{
		memset(bmp+x1,(int)pat,x2-x1+1);
	}
}

static void WcFillLine16
	(unsigned short *bmp,int x1,int x2,register unsigned long pat)
{
	register int x;

	if(x1&1)
	{
		bmp[x1]=(unsigned short)(pat&65535);
		x1++;
	}
	if((x2&1)==0)
	{
		bmp[x2]=(unsigned short)(pat&65535);
		x2--;
	}

	for(x=x1; x<x2; x+=2)
	{
		*((unsigned long *)(bmp+x))=pat;
	}
}



/***********************************************************************
int WcRect(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col);
***********************************************************************/
int WcRect(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col)
{
	int c,y,xs,xl;
	unsigned long pat;

	if(bm->bpp<16)
	{
		WcGetColorIndex8(&c,col);
		pat=(unsigned long)(((unsigned)c) + (((unsigned)c)<<8));
		pat=pat+(pat<<16);

		xs=(x1<x2 ? x1 : x2);
		xl=(x1<x2 ? x2 : x1);
		for(y=y1; y<=y2; y++)
		{
			WcFillLine(bm->hTop[y],xs,xl,pat);
		}
	}
	else
	{
		WcGetColorIndex16(&c,col);
		pat=(unsigned long)(((unsigned)c) + (((unsigned)c)<<16));

		xs=(x1<x2 ? x1 : x2);
		xl=(x1<x2 ? x2 : x1);
		for(y=y1; y<=y2; y++)
		{
			WcFillLine16((unsigned short *)(bm->hTop[y]),xs,xl,pat);
		}
	}
	return WC_OK;
}




/***********************************************************************
int WcSetCircleRegion(WCBMP *bm,WCPNT *c,long rad);
***********************************************************************/
static void WcGenerateCircleRegion(WCBMP *bm,WCPNT *c,long rad);

int WcSetCircleRegion(WCBMP *bm,WCPNT *c,long rad)
{
	if(bm->alive!=0)
	{
		WcCleanRegion(bm);
		WcGenerateCircleRegion(bm,c,rad);
		return WC_OK;
	}
	return WC_ERR;
}

static void WcGenerateCircleRegion(WCBMP *bm,WCPNT *c,long rad)
{
	int sum,x,y,x1,x2;
	register int *hlu,*hld;

	if(0<=c->x+rad && c->x-rad<bm->wid && 0<=c->y+rad && c->y-rad<bm->hei)
	{
		bm->rgn.y1=((c->y-rad<0) ? 0 : c->y-rad);
		bm->rgn.y2=((c->y+rad>=bm->hei) ? bm->hei-1 : c->y+rad);

		hlu=&bm->rgn.x[c->y*WC_RGN_X_PER_LINE];
		hld=&bm->rgn.x[c->y*WC_RGN_X_PER_LINE];

		if(0<=c->y && c->y<bm->hei)
		{
			x1=((c->x-rad<0) ? 0 : c->x-rad);
			x2=((c->x+rad>=bm->wid) ? bm->wid-1 : c->x+rad);
			WcInsertX(hlu,x1);
			WcInsertX(hlu,x2);
		}

		x=rad;
		y=0;
		sum=0;
		while(y<rad)
		{
			if(sum<=0)
			{
				sum+=(2*y+1);
				y++;
				hld+=WC_RGN_X_PER_LINE;
				hlu-=WC_RGN_X_PER_LINE;
				x1=((c->x-x<0) ? 0 : c->x-x);
				x2=((c->x+x>=bm->wid) ? bm->wid-1 : c->x+x);
				if(0<=c->y-y && c->y-y<bm->hei)
				{
					WcInsertX(hlu,x1);
					WcInsertX(hlu,x2);
				}
				if(0<=c->y+y && c->y+y<bm->hei)
				{
					WcInsertX(hld,x1);
					WcInsertX(hld,x2);
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


/***********************************************************************
int WcSetEllipseRegion(WCBMP *bm,WCPNT *c,long radx,long rady);
***********************************************************************/
static void WcGenerateEllipseRegion(WCBMP *bm,WCPNT *c,long radx,long rady);

int WcSetEllipseRegion(WCBMP *bm,WCPNT *c,long radx,long rady)
{
	if(bm->alive!=0)
	{
		WcCleanRegion(bm);
		WcGenerateEllipseRegion(bm,c,radx,rady);
		return WC_OK;
	}
	return WC_ERR;
}

static void WcGenerateEllipseRegion(WCBMP *bm,WCPNT *c,long radx,long rady)
{
	long xe;
	int sum,x,y,x1,x2;
	register int *hlu,*hld;

	if(0<=c->x+radx && c->x-radx<bm->wid && 0<=c->y+rady && c->y-rady<bm->hei)
	{
		bm->rgn.y1=((c->y-rady<0) ? 0 : c->y-rady);
		bm->rgn.y2=((c->y+rady>=bm->hei) ? bm->hei-1 : c->y+rady);

		hlu=&bm->rgn.x[c->y*WC_RGN_X_PER_LINE];
		hld=&bm->rgn.x[c->y*WC_RGN_X_PER_LINE];

		if(0<=c->y && c->y<bm->hei)
		{
			x1=((c->x-radx<0) ? 0 : c->x-radx);
			x2=((c->x+radx>=bm->wid) ? bm->wid-1 : c->x+radx);
			WcInsertX(hlu,x1);
			WcInsertX(hlu,x2);
		}

		x=rady;
		y=0;
		sum=0;
		while(y<rady)
		{
			if(sum<=0)
			{
				sum+=(2*y+1);
				y++;
				xe=(int)((long)x*radx/rady);
				hld+=WC_RGN_X_PER_LINE;
				hlu-=WC_RGN_X_PER_LINE;
				x1=((c->x-xe<0) ? 0 : c->x-xe);
				x2=((c->x+xe>=bm->wid) ? bm->wid-1 : c->x+xe);
				if(0<=c->y-y && c->y-y<bm->hei)
				{
					WcInsertX(hlu,x1);
					WcInsertX(hlu,x2);
				}
				if(0<=c->y+y && c->y+y<bm->hei)
				{
					WcInsertX(hld,x1);
					WcInsertX(hld,x2);
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
