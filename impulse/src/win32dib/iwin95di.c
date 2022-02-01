/* Machine Depending Module : Windows95 DIB Version */
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../impulse.h"
#include "../iutil.h"

#include <windowsx.h>
#include "iwildcat.h"


/***

Plug Interface
  最初の一回だけ呼ぶ(DCは保持した方が多分いい)
    HPALETTE BiWinCreatePalette(HDC dc);
    int BiWinCreatePalette(CPalette *pal,CDC *dc);

  経過に応じて何度でも呼んでいい
    void BiWinPlug(HDC dc,long wid,long hei,int bitPerPix);
    void BiWinUnplug(void);

***/


extern HFONT BiWinFont;


/**********************************************************************/

HDC BiWndDC;
long BiWinX,BiWinY;
int BiBpp;

static int BiDBufExists=BI_OFF;
static WCBMP BiDBuf;

static int BiFontWid=8,BiFontHei=16;

/**********************************************************************/
LOGPALETTE *BiWinGetLogicalPalette(LOGPALETTE *lp,HDC dc)
{
	int i,n,r,g,b;
	int base,gray;
	HPALETTE org;
	PALETTEENTRY *ent;

	lp->palVersion=0x300;
	lp->palNumEntries=256;
	ent=lp->palPalEntry;

	org=CreateHalftonePalette(dc);
	n=GetPaletteEntries(org,0,256,ent);
	base=(n<40 ? n : 40);
	for(r=0; r<6; r++)
	{
		for(g=0; g<6; g++)
		{
			for(b=0; b<6; b++)
			{
				i=g*36+r*6+b+base;
				ent[i].peRed  =r*51;
				ent[i].peGreen=g*51;
				ent[i].peBlue =b*51;
				ent[i].peFlags=0;
			}
		}
	}

	base+=216;
	gray=256-base;
	for(i=0; i<gray; i++)
	{
		ent[base+i].peRed  =i*255/gray;
		ent[base+i].peGreen=i*255/gray;
		ent[base+i].peBlue =i*255/gray;
		ent[base+i].peFlags=0;
	}
	DeleteObject(org);
	return lp;
}

HPALETTE BiWinCreatePalette(HDC dc)
{
	HPALETTE neo;
	char lpBuf[sizeof(WORD)*2+sizeof(PALETTEENTRY)*256];
	LPLOGPALETTE lp;

	lp=(LPLOGPALETTE)lpBuf;
	BiWinGetLogicalPalette(lp,dc);
	neo=CreatePalette(lp);
	if(neo==NULL)
	{
		MessageBox(NULL,"Can't Create Palette.",NULL,MB_APPLMODAL);
		exit(1);
	}
	WcSetWinGPalette(neo);
	return neo;
}

void BiWinPlug(HDC dc,long wid,long hei,int bitPerPix)
{
	if(dc!=BiWndDC || wid!=BiWinX || hei!=BiWinY || bitPerPix!=BiBpp)
	{
		if(BiDBufExists==BI_ON)
		{
			WcDeleteBmp(&BiDBuf);
			BiDBufExists=BI_OFF;
		}

		wid=BiLarger(16,wid);
		hei=BiLarger(16,hei);

		BiWinX=wid;
		BiWinY=hei;
		WcCreateBmp(&BiDBuf,dc,wid,hei,bitPerPix);

		BiBpp=bitPerPix;
		BiWndDC=dc;

		BiDBufExists=BI_ON;
	}
}

void BiWinUnplug(void)
{
	if(BiDBufExists==BI_ON)
	{
		WcDeleteBmp(&BiDBuf);
		BiDBufExists=BI_OFF;
		BiWndDC=NULL;   // 2003/11/28
	}
}

void BiWinSetFontHandle(HFONT fnt)
{
	LOGFONT logFont;

	BiWinFont=fnt;

	if(GetObject(fnt,sizeof(logFont),&logFont)>0)
	{
		if(logFont.lfHeight<0)
		{
			BiFontHei=-logFont.lfHeight;
		}
		else if(logFont.lfHeight>0)
		{
			BiFontHei=MulDiv(logFont.lfHeight,GetDeviceCaps(BiWndDC,LOGPIXELSY),72);
		}

		BiFontWid=BiFontHei/2;
	}
	else
	{
	}
}

/**********************************************************************/

void BiGetWindowSize(long *wid,long *hei)
{
	*wid = BiWinX;
	*hei = BiWinY;
}

void BiGetStdProjection(BIPROJ *prj)
{
	long wid,hei;
	BiGetWindowSize(&wid,&hei);
	prj->lx=wid;
	prj->ly=hei;
	prj->cx=wid/2;
	prj->cy=hei/2;
	prj->magx=(real)wid/(real)1.41421356;
	prj->magy=prj->magx;
	prj->nearz=(real)2.5;
	prj->farz=(real)10000.0;
}

static void BiSetLineColor(BICOLOR *ptr);
static void BiEndLineColor(void);

void BiSwapBuffers(void)
{
	BitBlt(BiWndDC,0,0,(int)BiWinX,(int)BiWinY,BiDBuf.dc,0,0,SRCCOPY);
}

#define COLOR(ptr) RGB((ptr)->r,(ptr)->g,(ptr)->b)

static HPEN orgPen,neoPen;
static HBRUSH orgBrs,neoBrs;

static void BiSetLineColor(BICOLOR *ptr)
{
	neoPen=CreatePen(PS_SOLID,1,COLOR(ptr));
	orgPen=SelectObject(BiDBuf.dc,neoPen);
	orgBrs=SelectObject(BiDBuf.dc,GetStockObject(NULL_BRUSH));
}

static void BiEndLineColor(void)
{
	SelectObject(BiDBuf.dc,orgPen);
	SelectObject(BiDBuf.dc,orgBrs);
	DeleteObject(neoPen);
}

/*****************************************************************************

   Memory Management

*****************************************************************************/

void BiMemoryInitialize(void)
{
}

void *BiMalloc(size_t sz)
{
	return malloc(sz);
}

void BiFree(void *ptr)
{
	free(ptr);
}

/*****************************************************************************

   Draw

*****************************************************************************/
void BiClearScreenD(void)
{
	WCCOLOR col={0,0,0};
	WcRect(&BiDBuf,0,0,(int)(BiWinX-1),(int)(BiWinY-1),&col);
}

static BIPOINTS orgLup,orgRdw;
static void BiWinSetClipScrnRect(void)
{
	BIPOINTS lup,rdw;
	BiSetPointS(&lup,0,0);
	BiSetPointS(&rdw,BiWinX-1,BiWinY-1);
	BiGetClipScreenFrame(&orgLup,&orgRdw);
	BiSetClipScreenFrame(&lup,&rdw);
}

static void BiWinResetClipScrnRect(void)
{
	BiSetClipScreenFrame(&orgLup,&orgRdw);
}

#define BIINBOUND(a) (0<=(a)->x && (a)->x<BiWinX && 0<=(a)->y && (a)->y<BiWinY)

void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BIPOINTS a1,a2;
	BiWinSetClipScrnRect();

	if(BIINBOUND(s1) && BIINBOUND(s2))
	{
		WcPset(&BiDBuf,(int)s1->x,(int)s1->y,(WCCOLOR *)c);
		WcLine(&BiDBuf,(int)s1->x,(int)s1->y,(int)s2->x,(int)s2->y,(WCCOLOR *)c);
	}
	else if(BiClipLineScrn(&a1,&a2,s1,s2)==BI_IN)
	{
		WcPset(&BiDBuf,(int)a1.x,(int)a1.y,(WCCOLOR *)c);
		WcLine(&BiDBuf,(int)a1.x,(int)a1.y,(int)a2.x,(int)a2.y,(WCCOLOR *)c);
 	}
	BiWinResetClipScrnRect();
}

void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
	if(BIINBOUND(s))
	{
		WcPset(&BiDBuf,s->x,s->y,(WCCOLOR *)c);
	}

/*	BIPOINTS t1,t2;

	t1=*s;
	BiSetPointS(&t2,t1.x+1,t1.y+1);
	if(BIINBOUND(&t1) && BIINBOUND(&t2))
	{
		WcPset(&BiDBuf,(int)t1.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&BiDBuf,(int)t2.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&BiDBuf,(int)t1.x,(int)t2.y,(WCCOLOR *)c);
		WcPset(&BiDBuf,(int)t2.x,(int)t2.y,(WCCOLOR *)c);
	} */
}

void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	if(BIINBOUND(s))
	{
		if(radx==rady)
		{
			WcSetCircleRegion(&BiDBuf,(WCPNT *)s,radx);
		}
		else
		{
			WcSetEllipseRegion(&BiDBuf,(WCPNT *)s,radx,rady);
		}
		WcPaintRegion(&BiDBuf,(WCCOLOR *)c);
	}
}

void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
	unsigned long stk;
	int i,nsNew;
	BIPOINTS *sNew,*sP;

	sP=s;
	for(i=0; i<ns; i++)
	{
		if(!BIINBOUND(&s[i]))
		{
			goto CLIPPING;
		}
	}
	if(ns>0)
	{
		WcPset(&BiDBuf,s[0].x,s[0].y,(WCCOLOR *)c);
		WcSetRegion(&BiDBuf,ns,(WCPNT *)s);
		WcPaintRegion(&BiDBuf,(WCCOLOR *)c);
	}
	return;

CLIPPING:
	sNew=BiPushTmpStack(&stk,sizeof(BIPOINTS)*ns*4);
	BiWinSetClipScrnRect();
	if(sNew!=NULL && BiClipPolygScrn(&nsNew,sNew,ns,s)==BI_IN && nsNew>0)
	{
		if(nsNew>0)
		{
			WcPset(&BiDBuf,sNew[0].x,sNew[0].y,(WCCOLOR *)c);
		}
		WcSetRegion(&BiDBuf,nsNew,(WCPNT *)sNew);
		WcPaintRegion(&BiDBuf,(WCCOLOR *)c);
	}
	BiWinResetClipScrnRect();
	BiPopTmpStack(stk);
}

void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BIPOINTS a1,a2;

	if(BIINBOUND(s1) && BIINBOUND(s2))
	{
		int x1,y1,x2,y2;  // 2005/01/03
		x1=BiSmaller(s1->x,s2->x);  // 2005/01/03
		y1=BiSmaller(s1->y,s2->y);  // 2005/01/03
		x2=BiLarger(s1->x,s2->x);  // 2005/01/03
		y2=BiLarger(s1->y,s2->y);  // 2005/01/03
		WcRect(&BiDBuf,x1,y1,x2,y2,(WCCOLOR *)c);
	}
	else
	{
		a1.x=BiSmaller(s1->x,s2->x);
		a1.y=BiSmaller(s1->y,s2->y);
		a2.x=BiLarger(s1->x,s2->x);
		a2.y=BiLarger(s1->y,s2->y);
		if(a1.x>=BiWinX || a1.y>=BiWinY || a2.x<0 || a2.y<0)
		{
			return;
		}
		a1.x=BiLarger(a1.x,0);
		a1.y=BiLarger(a1.y,0);
		a2.x=BiSmaller(a2.x,BiWinX-1);
		a2.y=BiSmaller(a2.y,BiWinY-1);
		WcRect(&BiDBuf,(int)a1.x,(int)a1.y,(int)a2.x,(int)a2.y,(WCCOLOR *)c);
	}
}

void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
	BIPOINTS p1,p2;
	switch(mkType)
	{
	case BIMK_RECT:
		BiSetPointS(&p1, s->x-2,s->y-2);
		BiSetPointS(&p2, s->x+2,s->y+2);
		BiDrawRect2D(&p1,&p2,c);
		break;
	case BIMK_CIRCLE:
		BiDrawCircle2D(s,3,3,c);
		break;
	case BIMK_CROSS:
		BiSetPointS(&p1, s->x-4,s->y-4);
		BiSetPointS(&p2, s->x+4,s->y+4);
		BiDrawLine2D(&p1,&p2,c);
		BiSetPointS(&p1, s->x+4,s->y-4);
		BiSetPointS(&p2, s->x-4,s->y+4);
		BiDrawLine2D(&p1,&p2,c);
		break;
	}
}

static unsigned char **biFontPtr=NULL;
static unsigned int biFontWidth=0,biFontHeight=0;

void BiSetBitmapFontPtr(unsigned char **ptr,unsigned int wid,unsigned int hei)
{
	biFontPtr=ptr;
	biFontWidth=wid;
	biFontHeight=hei;
}

void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
	if(NULL!=biFontPtr && 0<biFontWidth && 0<biFontHeight)
	{
		WcDrawBitmapFont(&BiDBuf,(int)s->x,(int)s->y-biFontHeight+1,str,(WCCOLOR *)c,biFontPtr,biFontWidth,biFontHeight);
	}
	else
	{
		if(BIINBOUND(s))
		{
			HFONT stk;
			if(BiWinFont==NULL)
			{
				stk=SelectObject(BiDBuf.dc,GetStockObject(SYSTEM_FIXED_FONT));
			}
			else
			{
				stk=SelectObject(BiDBuf.dc,BiWinFont);
			}

			SetTextColor(BiDBuf.dc,COLOR(c));
			TextOut(BiDBuf.dc,(int)s->x,(int)s->y,(LPCSTR)str,strlen(str));
			SelectObject(BiDBuf.dc,stk);
		}
	}
}

void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;

	/* テキストの横文字数と縦文字数を数える */
	sWid=0;
	sHei=0;
	for(i=0; str[i]!=NULL; i++)
	{
		sWid=BiLarger(strlen(str[i]),(unsigned long)sWid);
		sHei++;
	}

	/* 情報を元に、書き出し位置を決める */
	switch(h)
	{
	default:
	case BIMG_CENTER:
		xSta=s->x -(BiFontWid*sWid)/2;
		break;
	case BIMG_LEFT:
		xSta=s->x;
		break;
	case BIMG_RIGHT:
		xSta=s->x -(BiFontWid*sWid);
		break;
	}
	switch(v)
	{
	default:
	case BIMG_CENTER:
		ySta=(s->y+BiFontHei/2)-(BiFontHei*sHei)/2;
		break;
	case BIMG_TOP:
		ySta=s->y+BiFontHei-1;
		break;
	case BIMG_BOTTOM:
		ySta=s->y-(BiFontHei*sHei);
		break;
	}

	/* 後は書くだけ */
	tmp.x=xSta;
	tmp.y=ySta;
	for(i=0; str[i]!=NULL; i++)
	{
		BiDrawString2D(&tmp,str[i],c);
		tmp.y+=BiFontHei;
	}
}

void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINTS p1,p2;
	BiSetPointS(&p1,s1->x,s1->y);
	BiSetPointS(&p2,s2->x,s1->y);
	BiDrawLine2D(&p1,&p2,col);
	BiSetPointS(&p1,s2->x,s1->y);
	BiSetPointS(&p2,s2->x,s2->y);
	BiDrawLine2D(&p1,&p2,col);
	BiSetPointS(&p1,s2->x,s2->y);
	BiSetPointS(&p2,s1->x,s2->y);
	BiDrawLine2D(&p1,&p2,col);
	BiSetPointS(&p1,s1->x,s2->y);
	BiSetPointS(&p2,s1->x,s1->y);
	BiDrawLine2D(&p1,&p2,col);
}

void BiDrawEmptyPolyg2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiDrawLine2D(&s[i],&s[i+1],col);
	}
	BiDrawLine2D(&s[0],&s[n-1],col);
}

void BiDrawPolyline2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiDrawLine2D(&s[i],&s[i+1],col);
	}
}

void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	int i;
	BIPOINTS v1,v2,p1,p2;

	BiSetPointS(&v2,radx,0);
	for(i=1; i<=8; i++)
	{
		v1=v2;

		v2.x=(long)((real)radx*BiCos(i*2048));
		v2.y=(long)((real)rady*BiSin(i*2048));

		BiSetPointS(&p1,s->x+v1.x,s->y+v1.y);
		BiSetPointS(&p2,s->x+v2.x,s->y+v2.y);
		BiDrawLine2D(&p1,&p2,col);

		BiSetPointS(&p1,s->x-v1.x,s->y+v1.y);
		BiSetPointS(&p2,s->x-v2.x,s->y+v2.y);
		BiDrawLine2D(&p1,&p2,col);

		BiSetPointS(&p1,s->x+v1.x,s->y-v1.y);
		BiSetPointS(&p2,s->x+v2.x,s->y-v2.y);
		BiDrawLine2D(&p1,&p2,col);

		BiSetPointS(&p1,s->x-v1.x,s->y-v1.y);
		BiSetPointS(&p2,s->x-v2.x,s->y-v2.y);
		BiDrawLine2D(&p1,&p2,col);
	}
}

void BiGetColor(BICOLOR *col,BIPOINTS *s)
{
	static int first=BI_ON;
	if(first==BI_ON)
	{
		first=BI_OFF;
		MessageBox(NULL,"BiGetColor is Under Construction.",NULL,MB_APPLMODAL);
	}
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
	static int first=BI_ON;
	if(first==BI_ON)
	{
		first=BI_OFF;
		MessageBox
			(NULL,"BiGetColorArray is Under Construction.",NULL,MB_APPLMODAL);
	}
}





/*****************************************************************************

   WildCat-WinG Acceleration Module Test.

*****************************************************************************/
void BiWinGetWildCatBmp(WCBMP *bmp)
{
	*bmp=BiDBuf;
}


HDC BiWinGetDrawBufferDC(void)
{
	return BiDBuf.dc;
}



/*****************************************************************************

   File Name

*****************************************************************************/
void BiConstrainFileName(char neo[],char org[])
{
	int i;

	if(neo!=org)
	{
		strcpy(neo,org);
	}

	if(neo[0]=='\\' && neo[1]=='\\')
	{
		/* Support network shared file */
		return;
	}

	for(i=0; neo[i]!=0; i++)
	{
		if((i!=1 && neo[i]==':') || neo[i]=='\\')
		{
			neo[i]='/';
		}
	}
}
