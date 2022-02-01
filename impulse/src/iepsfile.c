/* EPS File Output Module */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "impulse.h"
#include "iepsfile.h"


#define EPSRESOLUTION 32.0F


static char *head[]={
	"",
	"%%BeginProlog",
	"512 dict begin",
	"",
	"/L {",
	"    lineto",
	"} def",
	"",
	"/m {",
	"    moveto",
	"} def",
	"",
	"/@w {",
	"    pop",
	"    setlinewidth",
	"    pop",
	"    pop",
	"} def",
	"",
	"/K {",
	"    /fgColK exch def",
	"    /fgColY exch def",
	"    /fgColM exch def",
	"    /fgColC exch def",
	"} def",
	"",
	"/k {",
	"    /bgColK exch def",
	"    /bgColY exch def",
	"    /bgColM exch def",
	"    /bgColC exch def",
	"} def",
	"",
	"/S {",
	"    fgColC fgColM fgColY fgColK setcmykcolor",
	"    stroke",
	"} def",
	"",
	"/B {",
	"    bgColC bgColM bgColY bgColK setcmykcolor",
	"    eofill",
	"    fgColC fgColM fgColY fgColK setcmykcolor",
	"    stroke",
	"} def",
	"",
	"/z {",
	"    25 div",
	"    /fntSize exch def",
	"    /fntType exch def",
	"} def",
	"",
	"/@t {",
	"    /textString exch def",
	"    /textPosY exch def",
	"    /textPosX exch def",
	"} def",
	"",
	"/T {",
	"    fgColC fgColM fgColY fgColK setcmykcolor",
	"    fntType findfont",
	"    fntSize scalefont",
	"    setfont",
	"    textPosX textPosY m",
	"    textString show",
	"} def",
	"%%EndProlog",
	"",
	"%%BeginSetup",
	"%%EndSetup",
	"",
	NULL
};

static char *tail[]={
	"",
	"%%Trailer",
	" showpage",
	"",
	" end",
	NULL
};


/* #define MALLOCFUNC(a,b) malloc(a*b) */
/* #define FREEFUNC(a) free(a) */

void *BiMalloc(size_t uni);
void BiFree(void *ptr);
#define MALLOCFUNC(a) BiMalloc(a)
#define FREEFUNC(a) BiFree(a);


/***********************************************************************
    px : Paper Left X
    py : Paper Bottom Y
    pWid : Window Width on Paper
    pHei : Window Height on Paper
    sWid : Window Width on Screen
    sHei : Window Height on Screen
***********************************************************************/
BIEPSHANDLE *BiBeginEps(char fnOrg[],long pLft,long pBtm,long pWid,long pHei,long sWid,long sHei)
{
	int i;
	BIEPSHANDLE *ep;
	FILE *fp;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	ep=(BIEPSHANDLE *)MALLOCFUNC(sizeof(BIEPSHANDLE));
	if(ep!=NULL)
	{
		fp=fopen(fn,"w");
		if(fp!=NULL)
		{
			fprintf(fp,"%s\n","%!PS-Adobe-2.0 EPSF-2.0");
			fprintf(fp,"%s %ld %ld %ld %ld\n",
			           "%%BoundingBox:",pLft,pBtm,pLft+pWid,pBtm+pHei);

			for(i=0; head[i]!=NULL; i++)
			{
				fprintf(fp,"%s\n",head[i]);
			}

			ep->pLft=(real)pLft;
			ep->pBtm=(real)pBtm;
			ep->pWid=(real)pWid;
			ep->pHei=(real)pHei;
			ep->sWid=(real)sWid;
			ep->sHei=(real)sHei;
			ep->fp=fp;
			return ep;
		}
	}
	return NULL;
}

void BiEndEps(BIEPSHANDLE *ep)
{
	int i;
	if(ep!=NULL)
	{
		for(i=0; tail[i]!=NULL; i++)
		{
			fprintf(ep->fp,"%s\n",tail[i]);
		}
		fclose(ep->fp);
		FREEFUNC(ep);
	}
}

static int BiSetEpsColor(BIEPSHANDLE *ep,int r,int g,int b)
{
	real c,m,y;
	if(ep!=NULL)
	{
		c=(real)(255-r)/255;
		m=(real)(255-g)/255;
		y=(real)(255-b)/255;
		fprintf(ep->fp,"%1.1f %1.1f %1.1f 0.0 K\n",c,m,y);
		fprintf(ep->fp,"%1.1f %1.1f %1.1f 0.0 k\n",c,m,y);
		return 0;
	}
	return -1;
}

static int BiEpsCoord(BIEPSHANDLE *ep,long x,long y)
{
	real px,py;
	px=(real) ep->pLft           +((real)x *(real)ep->pWid /(real)ep->sWid);
	py=(real)(ep->pBtm+ep->pHei) -((real)y *(real)ep->pWid /(real)ep->sWid);
	fprintf(ep->fp,"%.2f %.2f ",px,py);
	return 0;
}

static int BiEpsLineto(BIEPSHANDLE *ep,long x,long y)
{
	BiEpsCoord(ep,x,y);
	fprintf(ep->fp,"L\n");
	return 0;
}

static int BiEpsMoveto(BIEPSHANDLE *ep,long x,long y)
{
	BiEpsCoord(ep,x,y);
	fprintf(ep->fp,"m\n");
	return 0;
}

int BiDrawEpsRect
    (BIEPSHANDLE *ep,long x1,long y1,long x2,long y2,int r,int g,int b)
{
	if(ep!=NULL)
	{
		BiSetEpsColor(ep,r,g,b);
		fprintf(ep->fp,"0 0 0 0 @w\n");
		BiEpsMoveto(ep,x1,y1);
		BiEpsLineto(ep,x2,y1);
		BiEpsLineto(ep,x2,y2);
		BiEpsLineto(ep,x1,y2);
		BiEpsLineto(ep,x1,y1);
		fprintf(ep->fp,"B\n");
		return 0;
	}
	return -1;
}

int BiDrawEpsLine
	(BIEPSHANDLE *ep,long x1,long y1,long x2,long y2,int r,int g,int b)
{
	if(ep!=NULL)
	{
		BiSetEpsColor(ep,r,g,b);
		fprintf(ep->fp,"0 0 0 0 @w\n");
		BiEpsMoveto(ep,x1,y1);
		BiEpsLineto(ep,x2,y2);
		fprintf(ep->fp,"S\n");
		return 0;
	}
	return -1;
}

int BiDrawEpsPset(BIEPSHANDLE *ep,long x1,long y1,int r,int g,int b)
{
	if(ep!=NULL)
	{
		BiSetEpsColor(ep,r,g,b);
		fprintf(ep->fp,"0 2.0 2.0 0 @w\n");
		BiEpsMoveto(ep,x1,y1);
		BiEpsLineto(ep,x1,y1+2);
		fprintf(ep->fp,"S\n");
		return 0;
	}
	return -1;
}

int BiDrawEpsPolygon(BIEPSHANDLE *ep,int np,long *p,int r,int g,int b)
{
	int i;
	if(ep!=NULL)
	{
		BiSetEpsColor(ep,r,g,b);
		fprintf(ep->fp,"0 0 0 0 @w\n");
		BiEpsMoveto(ep,p[0],p[1]);
		for(i=1; i<np; i++)
		{
			BiEpsLineto(ep,p[i*2],p[i*2+1]);
		}
		BiEpsLineto(ep,p[0],p[1]);
		fprintf(ep->fp,"B\n");
		return 0;
	}
	return -1;
}

static int BiIsKanjiString(unsigned char str[])
{
	int i;
	if(str!=NULL)
	{
		for(i=0; str[i]!=0; i++)
		{
			if(0x80<=str[i])
			{
				return BI_TRUE;
			}
		}
	}
	return BI_FALSE;
}

int BiDrawEpsString(BIEPSHANDLE *ep,char *str,long x,long y,int r,int g,int b)
{
	int i;
	unsigned char *ustr;
	ustr=(unsigned char *)str;
	if(ep!=NULL)
	{
		BiSetEpsColor(ep,r,g,b);
		if(BiIsKanjiString(ustr)==BI_FALSE)
		{
			fprintf(ep->fp,"/Times-Roman 333 z\n");
		}
		else
		{
			fprintf(ep->fp,"/Ryumin-Light-83pv-RKSJ-H 333 z\n");
		}
		BiEpsCoord(ep,x,y);
		fprintf(ep->fp,"(");
		for(i=0; ustr[i]!=0; i++)
		{
			fprintf(ep->fp,"\\%03o",ustr[i]);
		}
		fprintf(ep->fp,") @t\n");
		fprintf(ep->fp,"T\n");
		return 0;
	}
	return -1;
}

int BiDrawEpsEllipse
    (BIEPSHANDLE *ep,long x,long y,long radx,long rady,int r,int g,int b)
{
	if(ep!=NULL)
	{
		return 0;
	}
	return -1;
}
