#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include "impulse.h"


/* static void BiClipPointX(BIPOINTS *d,BIPOINTS *p1,BIPOINTS *p2,long x); */
/* static void BiClipPointY(BIPOINTS *d,BIPOINTS *p1,BIPOINTS *p2,long y); */

/* static void BiClipPointX(BIPOINTS *d,BIPOINTS *p1,BIPOINTS *p2,long x) */
#define BiClipPointX(d,p1,p2,xx)                   \
{                                                  \
	real dx,dy,ax;                                 \
	BIPOINT2 f1;                                   \
                                                   \
	BiSetPoint2(&f1,(real)(p1)->x,(real)(p1)->y);  \
	dx=(real)((p2)->x-(p1)->x);                    \
	dy=(real)((p2)->y-(p1)->y);                    \
	ax=(real)((xx)   -(p1)->x);                    \
	if(BiAbs(dx)>YSEPS)                            \
	{                                              \
		(d)->x = (xx);                             \
		(d)->y = (long)(dy/dx*ax+f1.y);            \
	}                                              \
	else                                           \
	{                                              \
		/* Eject Now! */                           \
	}                                              \
}

/* static void BiClipPointY(BIPOINTS *d,BIPOINTS *p1,BIPOINTS *p2,long y) */
#define BiClipPointY(d,p1,p2,yy)                   \
{                                                  \
	real dx,dy,ay;                                 \
	BIPOINT2 f1;                                   \
                                                   \
	BiSetPoint2(&f1,(real)(p1)->x,(real)(p1)->y);  \
	dx=(real)((p2)->x-(p1)->x);                    \
	dy=(real)((p2)->y-(p1)->y);                    \
	ay=(real)((yy)   -(p1)->y);                    \
	if(BiAbs(dy)>YSEPS)                            \
	{                                              \
		(d)->x = (long)(dx/dy*ay+f1.x);            \
		(d)->y = (yy);                             \
	}                                              \
	else                                           \
	{                                              \
		/* Eject Now! */                           \
	}                                              \
}

static void BiClipLine2Scrn(BIPOINTS *p1,BIPOINTS *p2);

int BiClipPolygTop(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p);
int BiClipPolygBottom(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p);
int BiClipPolygLeft(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p);
int BiClipPolygRight(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p);

static BIPOINTS lup={0,0};
static BIPOINTS rdw={32767,32767};

/* public */void BiGetClipScreenFrame(BIPOINTS *l,BIPOINTS *r)
{
	*l=lup;
	*r=rdw;
}

/* public */void BiSetClipScreenFrame(BIPOINTS *l,BIPOINTS *r)
{
	lup.x = BiSmaller(l->x,r->x);
	lup.y = BiSmaller(l->y,r->y);
	rdw.x = BiLarger(l->x,r->x);
	rdw.y = BiLarger(l->y,r->y);
}

/* public */int BiClipPolygScrn(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p)
{
	unsigned long stk;
	int n1,n2,n3;
	BIPOINTS *tmp1,*tmp2,*tmp3;

	tmp1=(BIPOINTS *)BiPushTmpStack(&stk,sizeof(BIPOINTS)*np*2);
	if(tmp1!=NULL)
	{
		BiClipPolygTop(&n1,tmp1,np,p);
		tmp2=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*n1*2);
		if(tmp2!=NULL)
		{
			BiClipPolygBottom(&n2,tmp2,n1,tmp1);
			tmp3=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*n2*2);
			if(tmp3!=NULL)
			{
				BiClipPolygLeft(&n3,tmp3,n2,tmp2);
				BiClipPolygRight(npNew,pNew,n3,tmp3);

				BiPopTmpStack(stk);
				return (*npNew>0 ? BI_IN : BI_OUT);
			}
		}
		BiPopTmpStack(stk);
	}
	return BI_OUT;
}

int BiClipPolygTop(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p)
{
	int i;
	BIPOINTS *p1,*p2;

	*npNew=0;
	if(np==0 || p==NULL)
	{
		return BI_OUT;
	}

	for(i=0; i<np; i++)
	{
		p1=&p[i];
		p2=&p[(i+1)%np];
		if(p1->y>=lup.y && p2->y>=lup.y)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
		}
		else if(p1->y<lup.y && p2->y>=lup.y)
		{
			BiClipPointY(&pNew[*npNew],p1,p2,lup.y);
			(*npNew)++;
		}
		else if(p1->y>=lup.y && p2->y<lup.y)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
			BiClipPointY(&pNew[*npNew],p1,p2,lup.y);
			(*npNew)++;
		}
	}
	return (*npNew>0 ? BI_IN : BI_OUT);
}

int BiClipPolygBottom(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p)
{
	int i;
	BIPOINTS *p1,*p2;

	*npNew=0;
	if(np==0 || p==NULL)
	{
		return BI_OUT;
	}

	for(i=0; i<np; i++)
	{
		p1=&p[i];
		p2=&p[(i+1)%np];
		if(p1->y<=rdw.y && p2->y<=rdw.y)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
		}
		else if(p1->y>rdw.y && p2->y<=rdw.y)
		{
			BiClipPointY(&pNew[*npNew],p1,p2,rdw.y);
			(*npNew)++;
		}
		else if(p1->y<=rdw.y && p2->y>rdw.y)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
			BiClipPointY(&pNew[*npNew],p1,p2,rdw.y);
			(*npNew)++;
		}
	}
	return (*npNew>0 ? BI_IN : BI_OUT);
}

int BiClipPolygLeft(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p)
{
	int i;
	BIPOINTS *p1,*p2;

	*npNew=0;
	if(np==0 || p==NULL)
	{
		return BI_OUT;
	}

	for(i=0; i<np; i++)
	{
		p1=&p[i];
		p2=&p[(i+1)%np];
		if(p1->x>=lup.x && p2->x>=lup.x)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
		}
		else if(p1->x<lup.x && p2->x>=lup.x)
		{
			BiClipPointX(&pNew[*npNew],p1,p2,lup.x);
			(*npNew)++;
		}
		else if(p1->x>=lup.x && p2->x<lup.x)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
			BiClipPointX(&pNew[*npNew],p1,p2,lup.x);
			(*npNew)++;
		}
	}
	return (*npNew>0 ? BI_IN : BI_OUT);
}

int BiClipPolygRight(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p)
{
	int i;
	BIPOINTS *p1,*p2;

	*npNew=0;
	if(np==0 || p==NULL)
	{
		return BI_OUT;
	}

	for(i=0; i<np; i++)
	{
		p1=&p[i];
		p2=&p[(i+1)%np];
		if(p1->x<=rdw.x && p2->x<=rdw.x)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
		}
		else if(p1->x>rdw.x && p2->x<=rdw.x)
		{
			BiClipPointX(&pNew[*npNew],p1,p2,rdw.x);
			(*npNew)++;
		}
		else if(p1->x<=rdw.x && p2->x>rdw.x)
		{
			pNew[*npNew]=*p1;
			(*npNew)++;
			BiClipPointX(&pNew[*npNew],p1,p2,rdw.x);
			(*npNew)++;
		}
	}
	return (*npNew>0 ? BI_IN : BI_OUT);
}



/* public */int BiClipLineScrn
        (BIPOINTS *p1New,BIPOINTS *p2New,BIPOINTS *p1,BIPOINTS *p2)
{
	if((p1->y<lup.y && p2->y<lup.y) || (p1->y>rdw.y && p2->y>rdw.y) ||
	   (p1->x<lup.x && p2->x<lup.x) || (p1->x>rdw.x && p2->x>rdw.x)   )
	{
		return BI_OUT;
	}

	*p1New=*p1;
	*p2New=*p2;

	BiClipLine2Scrn(p1New,p2New);
	if((p1New->y<lup.y && p2New->y<lup.y) ||
	   (p1New->y>rdw.y && p2New->y>rdw.y) ||
	   (p1New->x<lup.x && p2New->x<lup.x) ||
	   (p1New->x>rdw.x && p2New->x>rdw.x) ||
	   (p1New->x==p2New->x && p1New->y==p2New->y))
	{
		return BI_OUT;
	}

	BiClipLine2Scrn(p2New,p1New);
	if(p1New->x<lup.x || p2New->x<lup.x || p1New->y<lup.y || p2New->y<lup.y ||
	   p1New->x>rdw.x || p2New->x>rdw.x || p1New->y>rdw.y || p2New->y>rdw.y ||
       (p1New->x==p2New->x && p1New->y==p2New->y))
	{
		return BI_OUT;
	}

	return BI_IN;
}

static void BiClipLine2Scrn(BIPOINTS *p1,BIPOINTS *p2)
{
	BIPOINTS tmp;

	if(p2->x < lup.x)
	{
		BiClipPointX(&tmp,p1,p2,lup.x);
		*p2=tmp;
	}
	else if(p2->x > rdw.x)
	{
		BiClipPointX(&tmp,p1,p2,rdw.x);
		*p2=tmp;
	}

	if(p2->y < lup.y)
	{
		BiClipPointY(&tmp,p1,p2,lup.y);
		*p2=tmp;
	}
	else if(p2->y > rdw.y)
	{
		BiClipPointY(&tmp,p1,p2,rdw.y);
		*p2=tmp;
	}
}
