/* Corel Draw PS File Output Module */
/* Will be a universal EPS output module in the future. */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "impulse.h"
#include "iepsfile.h"



extern BIPROJ BiPrj;


int BiEpsMode=BI_OFF;
static BIEPSHANDLE *ep;
static long eWid,eHei;
static real fra,frb,sca;   /* sca : Real Window to Virtual Window */
static BIPROJ ePrj;


#define A4LEFT   50
#define A4BOTTOM 215
#define A4RIGHT  562
#define A4TOP    579
#define A4WIDTH (A4RIGHT-A4LEFT)
#define A4HEIGHT (A4TOP-A4BOTTOM)


int BiEpsOpen(char *fn,int wid,int hei)
{
	long wWid,wHei,vWid,vHei;

	if(BiEpsMode==BI_OFF)
	{
		BiGetWindowSize(&wWid,&wHei);
		if(wWid>=wHei)
		{
			hei=wid*wHei/wWid;
		}
		else
		{
			wid=hei*wWid/wHei;
		}

		vWid=wid*32;  /* Setting Virtual Screen */
		vHei=hei*32;

		fra=(real)vWid;
		frb=(real)wWid;
		sca=fra/frb;

		ep=BiBeginEps(fn,A4LEFT,A4BOTTOM,wid,hei,vWid,vHei);
		if(ep!=NULL)
		{
			eWid=vWid;
			eHei=vHei;
			BiEpsResetProjection(&BiPrj);

			BiEpsMode=BI_ON;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int BiEpsClose(void)
{
	if(BiEpsMode==BI_ON)
	{
		BiEndEps(ep);
		BiEpsMode=BI_OFF;
		return BI_OK;
	}
	return BI_ERR;
}

/***************************************************************************/
int BiEpsResetProjection(BIPROJ *prj)
{
	ePrj=*prj;

	ePrj.magx=prj->magx*sca;
	ePrj.magy=prj->magy*sca;

	ePrj.cx=(long)((real)prj->cx*sca);
	ePrj.cy=(long)((real)prj->cy*sca);
	return BI_OK;
}

/***************************************************************************/
static BIPOINTS org1,org2;

static void BiEpsPushClipScreen(void)
{
	BIPOINTS t1,t2;
	BiGetClipScreenFrame(&org1,&org2);
	BiSetPointS(&t1,0,0);
	BiSetPointS(&t2,eWid-1,eHei-1);
	BiSetClipScreenFrame(&t1,&t2);
}

static void BiEpsPopClipScreen(void)
{
	BiSetClipScreenFrame(&org1,&org2);
}

static int BiEpsClipLineScreen(BIPOINTS *a,BIPOINTS *b)
{
	int r;
	BIPOINTS c,d;

	c=*a;
	d=*b;

	BiEpsPushClipScreen();
	r=BiClipLineScrn(a,b,&c,&d);
	BiEpsPopClipScreen();

	return r;
}

static int BiEpsClipPolygScreen(int *nsNew,BIPOINTS *sNew,int ns,BIPOINTS *s)
{
	int r;

	BiEpsPushClipScreen();
	r=BiClipPolygScrn(nsNew,sNew,ns,s);
	BiEpsPopClipScreen();

	return r;
}


/***************************************************************************/

int BiEpsClearScreen(void)
{
	BIPOINTS s1,s2;
	BICOLOR col;

	if(BiEpsMode==BI_ON)
	{
		BiSetPointS(&s1,0,0);
		BiSetPointS(&s2,eWid-1,eHei-1);
		BiSetColorRGB(&col,0,0,0);
		return BiDrawEpsRect(ep,s1.x,s1.y,s2.x,s2.y,col.r,col.g,col.b);
	}
	return BI_ERR;
}

int BiEpsDrawRect(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINTS r1,r2;
	if(BiEpsMode==BI_ON)
	{
		BiSetPointS(&r1,(real)s1->x*sca,(real)s1->y*sca);
		BiSetPointS(&r2,(real)s2->x*sca,(real)s2->y*sca);

		BiSetPointS(&r1,BiSmaller(r1.x,r2.x),BiSmaller(r1.y,r2.y));
		BiSetPointS(&r2,BiLarger(r1.x,r2.x),BiLarger(r1.y,r2.y));

		r1.x=BiLarger(r1.x,0);
		r1.y=BiLarger(r1.y,0);
		r2.x=BiSmaller(r2.x,eWid-1);
		r2.y=BiSmaller(r2.y,eHei-1);

		BiDrawEpsRect(ep,r1.x,r1.y,r2.x,r2.y,col->r,col->g,col->b);
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawLine(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINTS r1,r2;
	if(BiEpsMode==BI_ON)
	{
		BiSetPointS(&r1,(real)s1->x*sca,(real)s1->y*sca);
		BiSetPointS(&r2,(real)s2->x*sca,(real)s2->y*sca);

		if(BiEpsClipLineScreen(&r1,&r2)==BI_IN)
		{
			BiDrawEpsLine(ep,r1.x,r1.y,r2.x,r2.y,col->r,col->g,col->b);
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawPset(BIPOINTS *s,BICOLOR *col)
{
	BIPOINTS r;
	if(BiEpsMode==BI_ON)
	{
		BiSetPointS(&r,(real)s->x*sca,(real)s->y*sca);
		if(0<=r.x && 0<=r.y && r.x<eWid-1 && r.y<eHei-1)
		{
			BiDrawEpsPset(ep,r.x,r.y,col->r,col->g,col->b);
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawPolygon(int np,BIPOINTS *p,BICOLOR *col)
{
	int i,nr;
	unsigned long stk;
	BIPOINTS *q,*r;
	if(BiEpsMode==BI_ON)
	{
		q=(BIPOINTS *)BiPushTmpStack(&stk,sizeof(BIPOINTS)*np);
		if(q!=NULL)
		{
			r=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*np*4);
			if(r!=NULL)
			{
				for(i=0; i<np; i++)
				{
					BiSetPointS(&q[i],(real)p[i].x*sca,(real)p[i].y*sca);
				}
				if(BiEpsClipPolygScreen(&nr,r,np,q)==BI_IN)
				{
					BiDrawEpsPolygon(ep,nr,(long *)r,col->r,col->g,col->b);
				}
			}
			BiPopTmpStack(stk);
			return BI_OK;
		}
	}
	return BI_ERR;
}

int BiEpsDrawString(char *str,BIPOINTS *s,BICOLOR *col)
{
	BIPOINTS r;
	if(BiEpsMode==BI_ON)
	{
		BiSetPointS(&r,(real)s->x*sca,(real)s->y*sca);
		if(0<=r.x && 0<=r.y && r.x<eWid-1 && r.y<eHei-1)
		{
			BiDrawEpsString(ep,str,r.x,r.y,col->r,col->g,col->b);
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawEllipse(BIPOINTS *c,long radx,long rady,BICOLOR *col)
{
	long i;
	BIPOINTS p[32];
	if(BiEpsMode==BI_ON)
	{
		for(i=0; i<32; i++)
		{
			p[i].x=c->x+(long)(BiCos(i*2048)*(real)radx);
			p[i].y=c->y+(long)(BiSin(i*2048)*(real)rady);
		}
		BiEpsDrawPolygon(32,p,col);
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawMarker(BIPOINTS *s,int mkType,BICOLOR *c)
{
	BIPOINTS p1,p2;
	if(BiEpsMode==BI_ON)
	{
		switch(mkType)
		{
		case BIMK_RECT:
			BiSetPointS(&p1, s->x-2,s->y-2);
			BiSetPointS(&p2, s->x+2,s->y+2);
			BiEpsDrawRect(&p1,&p2,c);
			break;
		case BIMK_CIRCLE:
			BiEpsDrawEllipse(s,3,3,c);
			break;
		case BIMK_CROSS:
			BiSetPointS(&p1, s->x-4,s->y-4);
			BiSetPointS(&p2, s->x+4,s->y+4);
			BiEpsDrawLine(&p1,&p2,c);
			BiSetPointS(&p1, s->x+4,s->y-4);
			BiSetPointS(&p2, s->x-4,s->y+4);
			BiEpsDrawLine(&p1,&p2,c);
			break;
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawText(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;
	if(BiEpsMode==BI_ON)
	{
		#define FONTX 8
		#define FONTY 16

		sWid=0;
		sHei=0;
		for(i=0; str[i]!=NULL; i++)
		{
			sWid=BiLarger(strlen(str[i]),(unsigned long)sWid);
			sHei++;
		}

		switch(h)
		{
		default:
		case BIMG_CENTER:
			xSta=s->x -(FONTX*sWid)/2;
			break;
		case BIMG_LEFT:
			xSta=s->x;
			break;
		case BIMG_RIGHT:
			xSta=s->x -(FONTX*sWid);
			break;
		}
		switch(v)
		{
		default:
		case BIMG_CENTER:
			ySta=(s->y+FONTY/2)-(FONTY*sHei)/2;
			break;
		case BIMG_TOP:
			ySta=s->y+FONTY-1;
			break;
		case BIMG_BOTTOM:
			ySta=s->y-(FONTY*sHei);
			break;
		}

		tmp.x=xSta;
		tmp.y=ySta;
		for(i=0; str[i]!=NULL; i++)
		{
			BiEpsDrawString(str[i],&tmp,c);
			tmp.y+=FONTY;
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawEmptyRect(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	if(BiEpsMode==BI_ON)
	{
		BIPOINTS t1,t2;
		BiSetPointS(&t1,s1->x,s2->y);
		BiSetPointS(&t2,s2->x,s1->y);
		BiEpsDrawLine(s1,&t1,col);
		BiEpsDrawLine(s1,&t2,col);
		BiEpsDrawLine(s2,&t1,col);
		BiEpsDrawLine(s2,&t2,col);
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawEmptyPolyg(int n,BIPOINTS *s,BICOLOR *col)
{
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPolyline(n,s,col);
		BiEpsDrawLine(&s[0],&s[n-1],col);
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawPolyline(int n,BIPOINTS *s,BICOLOR *col)
{
	if(BiEpsMode==BI_ON)
	{
		int i;
		for(i=0; i<n-1; i++)
		{
			BiEpsDrawLine(&s[i],&s[i+1],col);
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawEmptyEllipse(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	long i;
	BIPOINTS p1,p2;
	if(BiEpsMode==BI_ON)
	{
		for(i=0; i<=32; i++)
		{
			p1.x=s->x+(long)(BiCos( i   *2048)*(real)radx);
			p1.y=s->y+(long)(BiSin( i   *2048)*(real)rady);
			p2.x=s->x+(long)(BiCos((i+1)*2048)*(real)radx);
			p2.y=s->y+(long)(BiSin((i+1)*2048)*(real)rady);
			BiEpsDrawLine(&p1,&p2,col);
		}
		return BI_OK;
	}
	return BI_ERR;
}


/***************************************************************************/


int BiEpsDrawLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	BIPOINT t[2];
	BIPOINTS s1,s2;
	if(BiEpsMode==BI_ON)
	{
		t[0]=*p1;
		t[1]=*p2;
		if(p1->z>ePrj.nearz || p2->z>ePrj.nearz)
		{
			BiNearClipLine(t,t,ePrj.nearz);
			BiProject(&s1,&t[0],&ePrj);
			BiProject(&s2,&t[1],&ePrj);
			if(BiEpsClipLineScreen(&s1,&s2)==BI_IN)
			{
				BiDrawEpsLine(ep,s1.x,s1.y,s2.x,s2.y,col->r,col->g,col->b);
			}
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawMarker3(BIPOINT *p,int mkType,BICOLOR *col)
{
	BIPOINTS sc;
	if(BiEpsMode==BI_ON)
	{
		BiProject(&sc,p,&BiPrj);           /* Screen Size(not ePrj) #### */
		BiEpsDrawMarker(&sc,mkType,col);
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawPset3(BIPOINT *p,BICOLOR *col)
{
	BIPOINTS sc;
	if(BiEpsMode==BI_ON)
	{
		BiProject(&sc,p,&BiPrj);          /* Screen Size  (not ePrj) #### */
		BiEpsDrawPset(&sc,col);
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawPolyg3(int np,BIPOINT p[],BICOLOR *col)
{
	unsigned long stk;
	int nclp,nq;
	BIPOINT *clp;
	BIPOINTS *sc,*q;
	if(BiEpsMode==BI_ON)
	{
		clp=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*np*2);
		if(clp!=NULL)
		{
			BiNearClipPolyg(&nclp,clp,np,p,ePrj.nearz);
			if(nclp!=0 && (sc=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*nclp))!=NULL)
			{
				BiPlProject(nclp,sc,clp,&ePrj);

				q=(BIPOINTS *)BiGetTmpStack(sizeof(BIPOINTS)*nclp*4);
				if(q!=NULL && BiEpsClipPolygScreen(&nq,q,nclp,sc)==BI_IN)
				{
					BiDrawEpsPolygon(ep,nq,(long*)q,col->r,col->g,col->b);
				}
			}
			BiPopTmpStack(stk);
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawCircle3(BIPOINT *p,real rad,BICOLOR *col)
{
	if(BiEpsMode==BI_ON)
	{
		int i,nr;
		BIPOINTS s,q[32],r[32];
		long sradx,srady;
		BiProject(&s,p,&ePrj);
		sradx=(long)(rad*ePrj.magx/p->z);
		srady=(long)(rad*ePrj.magy/p->z);

		for(i=0; i<32; i++)
		{
			q[i].x=s.x+(long)(BiCos(i*2048)*(real)sradx);
			q[i].y=s.y+(long)(BiSin(i*2048)*(real)srady);
		}

		if(BiEpsClipPolygScreen(&nr,r,32,q)==BI_IN)
		{
			BiDrawEpsPolygon(ep,nr,(long*)r,col->r,col->g,col->b);
		}

		return BI_OK;
	}
	return BI_ERR;
}

int BiEpsDrawText3(BIPOINT *p,char *str[],int h,int v,BICOLOR *c)
{
	BIPOINTS s;
	if(BiEpsMode==BI_ON)
	{
		BiProject(&s,p,&BiPrj);           /* Screen Size  (not ePrj) #### */
		BiEpsDrawText(&s,str,h,v,c);
		return BI_OK;
	}
	return BI_ERR;
}




/*
int BiEps
{
	if(BiEpsMode==BI_ON)
	{
		return BI_OK;
	}
	return BI_ERR;
}
int BiEps
{
	if(BiEpsMode==BI_ON)
	{
		return BI_OK;
	}
	return BI_ERR;
}
int BiEps
{
	if(BiEpsMode==BI_ON)
	{
		return BI_OK;
	}
	return BI_ERR;
}
*/
