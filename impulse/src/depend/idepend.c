#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../impulse.h"




/*****************************************************************************

   Setting

*****************************************************************************/
int BiFatal(void)
{
	/* Default */
	return BI_OFF;
}



extern char BiWindowName[];

void BiOpenWindow(long reqX,long reqY)
{
}

void BiCloseWindow(void)
{
}

void BiGetWindowSize(long *wid,long *hei)
{
}

void BiGetStdProjection(BIPROJ *prj)
{
	/* Default */
    long wid,hei;
    BiGetWindowSize(&wid,&hei);
    prj->lx=wid;
    prj->ly=hei;
    prj->cx=wid/2;
    prj->cy=hei/2;
    prj->magx=(real)wid/1.41421356;
    prj->magy=prj->magx;
    prj->nearz=0.5;
    prj->farz=10000.0;
}

void BiSwapBuffers(void)
{
}




/*****************************************************************************

   Device

*****************************************************************************/

void BiDeviceInitialize(void)
{
}

void BiUpdateDevice(void)
{
}

void BiMouse(int *lbt,int *mbt,int *rbt,long *mx,long *my)
{
}

int BiKey(int kcode)
{
}

int BiInkey(void)
{
}




/*****************************************************************************

   Memory Management

*****************************************************************************/
void BiMemoryInitialize(void)
{
}

void *BiMalloc(size_t uni)
{
	/* Default */
    return (void *)malloc(uni);
}

void BiFree(void *ptr)
{
	/* Default */
    free(ptr);
}





/*****************************************************************************

   File Name

*****************************************************************************/
void BiConstrainFileName(char neo[],char org[])
{
	/* Default */
	if(neo!=org)
	{
		strcpy(neo,org);
	}
}





/*****************************************************************************

   Draw

*****************************************************************************/
void BiClearScreenD(void)
{
}

void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
}

void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
}

void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
}

void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
}

void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
}

void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
}

void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
}

void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	/* Default */
    #define FONTX 8
    #define FONTY 16
    int i;
    long sWid,sHei,xSta,ySta;
    BIPOINTS tmp;

    sWid=0;
    sHei=0;
    for(i=0; str[i]!=NULL; i++)
    {
        sWid=BiLarger(strlen(str[i]),sWid);
        sHei++;
    }

    switch(h)
    {
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
        BiDrawString2D(&tmp,str[i],c);
        tmp.y+=FONTY;
    }
}

void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
}

void BiDrawEmptyPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
}

void BiDrawPolyline2D(int ns,BIPOINTS *s,BICOLOR *c)
{
}

void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
}

void BiGetColor(BICOLOR *col,BIPOINTS *s)
{
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
}




/*****************************************************************************

   3D Accelerator Support

*****************************************************************************/
#ifdef BIACCEL3D
void BiAfterSetProjection(int pMode,BIPROJ *prj)
{
}

void BiAfterStartBuffer(BIPOSATT *eye)
{
}

void BiAfterChangeMatrix(BIAXISF *cnvaxs,BIAXISF *mdlaxs,BIAXISF *eyeaxs)
{
}

void BiAfterSetLightPosition(BIPOINT *lPos)
{
}

void BiAfterSetProjectionMode(int pMode,BIPROJ *prj)
{
}

void BiAfterSetOrthoDist(int pMode,BIPROJ *prj,real dist)
{
}

void BiAfterSetShadeMode(int sMode)
{
}

void BiOverWriteLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
}

void BiOverWritePset3(BIPOINT *p,BICOLOR *col)
{
}

void BiOverWritePolyg3(int np,BIPOINT *p,BICOLOR *col)
{
}

void BiDrawLine3(BIPOINT *a,BIPOINT *b,BICOLOR *col)
{
}

void BiDrawPset3(BIPOINT *p,BICOLOR *col)
{
}

void BiDrawCircle3(BIPOINT *p,real radx,real rady,BICOLOR *col)
{
}

void BiDrawPolyg3(int np,BIPOINT *p,BIPOINT *n,BICOLOR *col)
{
}

void BiDrawPolygGrad3(int np,BIPOINT *p,BIPOINT *n,BICOLOR *col)
{
}

void BiDrawText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col)
{
}

void BiOverWriteText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col)
{
}

void BiDrawMarker3(BIPOINT *p,BICOLOR *col,int mkType)
{
}

void BiOverWriteMarker3(BIPOINT *p,BICOLOR *col,int mkType)
{
}
#endif /* corr. #ifdef BIACCEL3D */
