#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impulse.h"

extern int BiRendLinkFlag;

void BiClearScreen(void)
{
	BiClearScreenD();
	if(BiEpsMode==BI_ON)
	{
		BiEpsClearScreen();
	}
}

void BiDrawLine2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BiDrawLine2D(s1,s2,c);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkLine2(s1,s2,c);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawLine(s1,s2,c);
	}

}

void BiDrawPset2(BIPOINTS *s,BICOLOR *c)
{
	BiDrawPset2D(s,c);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkPset2(s,c);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPset(s,c);
	}
}

void BiDrawCircle2(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	BiDrawCircle2D(s,radx,rady,c);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkEllipse2(s,radx,rady,c);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawEllipse(s,radx,rady,c);
	}
}

void BiDrawPolyg2(int ns,BIPOINTS *s,BICOLOR *c)
{
	BiDrawPolyg2D(ns,s,c);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkPolygon2(ns,s,c);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPolygon(ns,s,c);
	}
}

void BiDrawRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BiDrawRect2D(s1,s2,c);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkRect2(s1,s2,c);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawRect(s1,s2,c);
	}
}

void BiDrawMarker2(BIPOINTS *s,int mkType,BICOLOR *c)
{
	BiDrawMarker2D(s,mkType,c);
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawMarker(s,mkType,c);
	}
}

void BiDrawString2(BIPOINTS *s,char *str,BICOLOR *c)
{
	BiDrawString2D(s,str,c);
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawString(str,s,c);
	}
}

void BiDrawText2(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	BiDrawText2D(s,str,h,v,c);
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawText(s,str,h,v,c);
	}
}

void BiDrawEmptyRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BiDrawEmptyRect2D(s1,s2,col);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkEmptyRect2(s1,s2,col);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawEmptyRect(s1,s2,col);
	}
}

void BiDrawEmptyPolyg2(int n,BIPOINTS *s,BICOLOR *col)
{
	BiDrawEmptyPolyg2D(n,s,col);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkEmptyPolygon2(n,s,col);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawEmptyPolyg(n,s,col);
	}
}

void BiDrawPolyline2(int n,BIPOINTS *s,BICOLOR *col)
{
	BiDrawPolyline2D(n,s,col);
	if(BiRendLinkFlag==BI_ON)
	{
		BiRenderLinkPolyline2(n,s,col);
	}
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPolyline(n,s,col);
	}
}

void BiDrawEmptyCircle2(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	BiDrawEmptyCircle2D(s,radx,rady,col);
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawEmptyEllipse(s,radx,rady,col);
	}
}

