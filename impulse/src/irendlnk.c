#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

#include "impulse.h"
#include "irender.h"



extern BIPROJ BiPrj;
extern BIAXISF BiEyeAxs,BiMdlAxs,BiCnvAxs;
extern BIPOINT BiLightPos;



int BiZBuffer=BI_OFF;
int BiRendLinkFlag=BI_OFF;

typedef struct {
	real *z;
	BICOLOR *buf;
} BIRENDLINE;

static int type;
static long rWid=0,rHei=0;
static BIRENDLINE *rBuf;
static char fnm[256];


void BiRenderResetProjection(BIPROJ *prj);
void BiRenderResetEyePosition(void);


int BiRenderOpen(char *fn,int t,int wid,int hei)
{
	int x,y;
	BIPOSATT eye={{0.0F,0.0F,0.0F},{0,0,0}};

	rBuf=(BIRENDLINE *)BiMalloc((hei+1)*sizeof(BIRENDLINE));
	if(rBuf!=NULL)
	{
		/* Allocating more 1 pixel is defensive coding */
		for(y=0; y<=hei; y++)
		{
			rBuf[y].z=(real *)BiMalloc((wid+1)*sizeof(real));
			rBuf[y].buf=(BICOLOR *)BiMalloc((wid+1)*sizeof(BICOLOR));
			if(rBuf[y].z!=NULL && rBuf[y].buf!=NULL)
			{
				for(x=0; x<=wid; x++)
				{
					rBuf[y].z[x]=BiPrj.farz;
					rBuf[y].buf[x].r=0;
					rBuf[y].buf[x].g=0;
					rBuf[y].buf[x].b=0;
				}
			}
			else
			{
				goto ERREND;
			}
		}

		rWid=wid;
		rHei=hei;
		BiRenderResetProjection(&BiPrj);
		BiRenderResetEyePosition();

		strcpy(fnm,fn);
		type=t;

		BiRendLinkFlag=BI_ON;

		return BI_OK;
	}
ERREND:
	return BI_ERR;
}

/* Library Local */
void BiRenderDrawFunc(int objNo,int x,int y,int r,int g,int b,real z)
{
	switch(BiZBuffer)
	{
	case BI_ON:
		if(z<rBuf[y].z[x])
		{
			rBuf[y].z[x]=z;
			rBuf[y].buf[x].r=(unsigned char)r;
			rBuf[y].buf[x].g=(unsigned char)g;
			rBuf[y].buf[x].b=(unsigned char)b;
		}
		break;
	case BI_OFF:
		rBuf[y].buf[x].r=(unsigned char)r;
		rBuf[y].buf[x].g=(unsigned char)g;
		rBuf[y].buf[x].b=(unsigned char)b;
		break;
	}
}

void BiRender2DDrawFunc(int objNo,int x,int y,int r,int g,int b)
{
	rBuf[y].buf[x].r=(unsigned char)r;
	rBuf[y].buf[x].g=(unsigned char)g;
	rBuf[y].buf[x].b=(unsigned char)b;
}

/* Library Local */
void BiRenderClearZBuffer(void)
{
	int x,y;
	for(y=0; y<rHei; y++)
	{
		for(x=0; x<rWid; x++)
		{
			rBuf[y].z[x]=BiPrj.farz;
		}
	}
}

/* Library Local */
void BiRenderResetProjection(BIPROJ *prj)
{
	long wid,hei,mag,cx,cy;

	BiGetWindowSize(&wid,&hei);
	if(wid>hei)
	{
		mag=(long)((real)prj->magx*(real)rWid/(real)wid);
	}
	else
	{
		mag=(long)((real)prj->magy*(real)rHei/(real)hei);
	}
	cx=prj->cx*rWid/wid;
	cy=prj->cy*rHei/hei;

	BiRenderSetScreen(0,0,(int)rWid-1,(int)rHei-1,(int)cx,(int)cy);
	BiRenderSetProjection((real)mag,prj->nearz);

	BiRenderSetDrawFunc(BiRenderDrawFunc);
	BiRenderSet2DDrawFunc(BiRender2DDrawFunc);
	/* BiRenderSetAbortFunc(int (*abortFunc)(void)); */
}

/* Library Local */
void BiRenderResetLightPosition(void)
{
	BIPOINT lit;
	BiConvLtoG(&lit,&BiLightPos,&BiCnvAxs);
	BiRenderSetLightPosition(&lit);
}

/* Library Local */
void BiRenderResetEyePosition(void)
{
	BiRenderResetLightPosition();
}


void BiRendSaveTiff(char *fn)
{
	int y;
	BiOpenTiff24(fn,rWid,rHei);
	for(y=0; y<rHei; y++)
	{
		BiWriteLineTiff24(rBuf[y].buf);
	}
	BiCloseTiff24();
}

void BiRendSaveBmp(char *fn)
{
	int y;
	BiOpenBmp24(fn,rWid,rHei);
	for(y=rHei-1; y>=0; y--)
	{
		BiWriteLineBmp24(rBuf[y].buf);
	}
	BiCloseBmp24();
}

void BiRendSavePpm(char *fn)
{
	int y;
	BiOpenPpm24(fn,rWid,rHei);
	for(y=0; y<rHei; y++)
	{
		BiWriteLinePpm24(rBuf[y].buf);
	}
	BiClosePpm24();
}

void BiRenderClose(void)
{
	int y;
	if(BiRendLinkFlag==BI_ON)
	{
		switch(type)
		{
		case BIRENDER_TIFF:
			BiRendSaveTiff(fnm);
			break;
		case BIRENDER_BMP:
			BiRendSaveBmp(fnm);
			break;
		case BIRENDER_PPM:
			BiRendSavePpm(fnm);
			break;
		}

		for(y=0; y<=rHei; y++)
		{
			BiFree(rBuf[y].buf);
			BiFree(rBuf[y].z);
		}
		BiFree(rBuf);
		BiRendLinkFlag=BI_OFF;
	}
}


/***********************************************************************/
void BiRenderLinkPolygonTwoSide(int np,BIPOINT p[],BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawPolygonTwoSide(np,p,c);
}

void BiRenderLinkPolygonOneSide(int np,BIPOINT p[],BIPOINT *nom,BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawPolygonOneSide(np,p,nom,c);
}

void BiRenderLinkRoundPolygonOneSide(int np,BIPOINT *p,BIPOINT *n,BIPOINT *avgNom,BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawRoundPolygonOneSide(np,p,n,avgNom,c);
}

void BiRenderLinkRoundPolygonTwoSide(int np,BIPOINT *p,BIPOINT *n,BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawRoundPolygonTwoSide(np,p,n,c);
}


void BiRenderLinkLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawLine(p1,p2,c);
}

void BiRenderLinkPset(BIPOINT *p,BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawPset(p,c);
}

void BiRenderLinkCircle(BIPOINT *p,real r,BICOLOR *c)
{
	BiZBuffer=BI_ON;
	BiRenderDrawCircle(p,r,c);
}



/***********************************************************************/
void BiRenderLinkOvwPolygon(int np,BIPOINT p[],BICOLOR *c)
{
	BiZBuffer=BI_OFF;
	BiRenderDrawPolygonTwoSide(np,p,c);
}

void BiRenderLinkOvwLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *c)
{
	BiZBuffer=BI_OFF;
	BiRenderDrawLine(p1,p2,c);
}

void BiRenderLinkOvwPset(BIPOINT *p,BICOLOR *c)
{
	BiZBuffer=BI_OFF;
	BiRenderDrawPset(p,c);
}

void BiRenderLinkOvwCircle(BIPOINT *p,real r,BICOLOR *c)
{
	BiZBuffer=BI_OFF;
	BiRenderDrawCircle(p,r,c);
}


/***********************************************************************/
void BiRenderLinkLine2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	long wid,hei;
	BIPOINTS t1,t2;
	BiGetWindowSize(&wid,&hei);
	BiSetPointS(&t1,s1->x*rWid/wid,s1->y*rHei/hei);
	BiSetPointS(&t2,s2->x*rWid/wid,s2->y*rHei/hei);
	BiRenderDrawLine2(&t1,&t2,c);
}

void BiRenderLinkPolygon2(int ns,BIPOINTS *s,BICOLOR *c)
{
	int i;
	unsigned long stk;
	long wid,hei;
	BIPOINTS *t;
	BiGetWindowSize(&wid,&hei);
	t=BiPushTmpStack(&stk,sizeof(BIPOINTS)*ns);
	for(i=0; i<ns; i++)
	{
		BiSetPointS(&t[i],s[i].x*rWid/wid,s[i].y*rHei/hei);
	}
	BiRenderDrawPolygon2(ns,t,c);
	BiPopTmpStack(stk);
}

void BiRenderLinkRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	long wid,hei;
	BIPOINTS t1,t2;
	BiGetWindowSize(&wid,&hei);
	BiSetPointS(&t1,s1->x*rWid/wid,s1->y*rHei/hei);
	BiSetPointS(&t2,s2->x*rWid/wid,s2->y*rHei/hei);
	BiRenderDrawRect2(&t1,&t2,c);
}

void BiRenderLinkPset2(BIPOINTS *s,BICOLOR *col)
{
	long wid,hei;
	BIPOINTS t;
	BiGetWindowSize(&wid,&hei);
	BiSetPointS(&t,s->x*rWid/wid,s->y*rHei/hei);
	BiRenderDrawPset2(&t,col);
}

void BiRenderLinkEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	long wid,hei;
	BIPOINTS t;
	BiGetWindowSize(&wid,&hei);
	BiSetPointS(&t,s->x*rWid/wid,s->y*rHei/hei);
	radx=radx*rWid/wid;
	rady=rady*rHei/hei;
	BiRenderDrawEllipse2(&t,radx,rady,col);
}

void BiRenderLinkEmptyRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINTS t1,t2;
	BiSetPointS(&t1,s1->x,s2->y);
	BiSetPointS(&t2,s2->x,s1->y);
	BiRenderLinkLine2(s1,&t1,col);
	BiRenderLinkLine2(s1,&t2,col);
	BiRenderLinkLine2(s2,&t1,col);
	BiRenderLinkLine2(s2,&t2,col);
}

void BiRenderLinkPolyline2(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiRenderLinkLine2(&s[i],&s[i+1],col);
	}
}

void BiRenderLinkEmptyPolygon2(int n,BIPOINTS *s,BICOLOR *col)
{
	BiRenderLinkPolyline2(n,s,col);
	BiRenderLinkLine2(&s[0],&s[n-1],col);
}

void BiRenderLinkEmptyEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	long wid,hei;
	BIPOINTS t;
	BiGetWindowSize(&wid,&hei);
	BiSetPointS(&t,s->x*rWid/wid,s->y*rHei/hei);
	radx=radx*rWid/wid;
	rady=rady*rHei/hei;
	BiRenderDrawEmptyEllipse2(&t,radx,rady,col);
}

