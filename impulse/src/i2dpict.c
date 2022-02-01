/* Blue Impulse Library PICTURE Format **************************************
Pict2                           At the top of file

PST                             psets
DST d                           maximum visible distance
COL r g b
VER x1 y1
VER x2 y2
    :
ENDO

PLL                             Polyline of n vertex
DST d                           maximum visible distance
COL r g b
VER x1 y1
VER x2 y2
    :
ENDO

LSQ                             Line Sequence of n vertex
DST d                           maximum visible distance
COL r g b
VER x1 y1
VER x2 y2
    :
ENDO

PLG                             Polygon of n vertex
DST d                           maximum visible distance
COL r g b
VER x1 y1
VER x2 y2
    :
ENDO

ENDPICT                         Termination Code
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "impulse.h"
#include "i3dg.h"
#include "irender.h"
#include "iutil.h"
#include "ivrmfile.h"

extern void BiBeginTempHeap(void);
extern void BiEndTempHeap(void);
extern void *BiTempMalloc(unsigned long siz,size_t uni);

extern int BiRendLinkFlag;



/***************************************************************************/
static char *p2cmd[]=
{
	"PST","PLL","LSQ","PLG","ENDPICT","REM",NULL
};

static char *objcmd[]=
{
	"COL","VER","DST","ENDO",NULL
};

enum {
	BIP2STD_COLOR,
	BIP2STD_VERTEX,
	BIP2STD_DISTANCE,
	BIP2STD_ENDOBJ
};


#define IDENTIFY "PICT2"
#define VISIDISTDEFAULT 8000000.0F

typedef struct BiPoint2List {
	struct BiPoint2List *prev,*next;
	BIPOINT2 pnt;
} BIPOINT2LIST;

typedef struct BiPc2ObjList {
	struct BiPc2ObjList *prev,*next;
	BIPC2OBJ obj;

	int np;
	BIPOINT2LIST *top,*btm;
} BIPC2OBJLIST;

static int BiLoadPc2Main(BIPC2 *pic);
static int BiTestPc2(void);
static int BiReadPc2(BIPC2 *tmp);
static BIPC2OBJLIST *BiP2NewObjList(int type);
static BIPOINT2LIST *BiP2NewPointList(real x,real y);
static int BiP2ReadStandardObj(int oType);
static int BiP2ReadStdObjColor(BIPC2OBJLIST *neo,int ac,char *av[]);
static int BiP2ReadStdObjVertex(BIPC2OBJLIST *neo,int ac,char *av[]);
static int BiP2ReadStdObjDistance(BIPC2OBJLIST *neo,int ac,char *av[]);
static int BiP2Generate(BIPC2 *pic);
static void BiP2GenerateMinMax(BIPOINT2 *min,BIPOINT2 *max,BIPOINT2LIST *top);
static void BiP2GenerateCenter(BIPOINT2 *cen,BIPOINT2LIST *top);
static int BiP2GeneratePntArray(BIPOINT2 *p,BIPOINT2LIST *top);
static unsigned long BiP2ReadSize(void);


int BiLoadPc2(BIPC2 *pic,char fnOrg[])
{
	int ret;
	FILE *fp;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp==NULL)
	{
		return BI_ERR;
	}

	BiPushStringFunc();
	BiSetFp(fp);
	BiInitGetStringFuncEx();

	BiBeginTempHeap();
	ret=BiLoadPc2Main(pic);
	BiEndTempHeap();
	fclose(fp);

	BiPopStringFunc();

	return ret;
}

int BiLoadPc2FromString(BIPC2 *pic,char *str[])
{
	int ret;

	BiPushStringFunc();
	BiSetStringList(str);
	BiInitGetStringFuncEx();

	BiBeginTempHeap();
	ret=BiLoadPc2Main(pic);
	BiEndTempHeap();

	BiPopStringFunc();
	return ret;
}


static int BiLoadPc2Main(BIPC2 *pic)
{
	if(BiTestPc2()==BI_OK && BiReadPc2(pic)==BI_OK)
	{
		return BI_OK;
	}
	else
	{
		return BI_ERR;
	}
}

static int BiTestPc2(void)
{
	char str[256];
	BiGetStringFuncEx(str,255);

	BiCapitalizeString(str);
	if(strncmp(str,IDENTIFY,strlen(IDENTIFY))==0)
	{
		return BI_OK;
	}
	return BI_ERR;
}


static int nobj;
static BIPC2OBJLIST *top,*btm;

static int npnt;
static BIPOINT2LIST *ptop,*pbtm;

static int BiReadPc2(BIPC2 *tmp)
{
	int ac,cmd,ret;
	char str[256],*av[32];

	nobj=0;
	while(BiGetStringFuncEx(str,255)!=NULL)
	{
		BiArguments(&ac,av,31,str);
		if(ac==0)
		{
			continue;
		}
		if(BiCommandNumber(&cmd,av[0],p2cmd)!=BI_OK)
		{
			return BI_ERR;
		}

		switch(cmd)
		{
		case BIP2_REM:
			ret=BI_OK;
			break;
		case BIP2_PSET:
		case BIP2_POLYLINE:
		case BIP2_LINESEQ:
		case BIP2_POLYG:
			ret=BiP2ReadStandardObj(cmd);
			break;
		case BIP2_ENDPICT:
			return BiP2Generate(tmp);
		}

		if(ret!=BI_OK)
		{
			return BI_ERR;
		}
	}
	/* Illegal EOF */
	return BI_ERR;
}

static BIPC2OBJLIST *BiP2NewObjList(int type)
{
	BIPC2OBJLIST *neo;
	neo=(BIPC2OBJLIST *)BiTempMalloc(1,sizeof(BIPC2OBJLIST));
	if(neo!=NULL)
	{
		neo->prev=NULL;
		neo->next=NULL;

		neo->obj.objType=type;
		neo->obj.visiDist=VISIDISTDEFAULT;
		BiSetColorRGB(&neo->obj.col,255,255,255);

		if(nobj==0)
		{
			top=neo;
			btm=neo;
		}
		else
		{
			btm->next=neo;
			neo->prev=btm;
			btm=neo;
		}
		nobj++;
	}
	return neo;
}

static BIPOINT2LIST *BiP2NewPointList(real x,real y)
{
	BIPOINT2LIST *neo;
	neo=(BIPOINT2LIST *)BiTempMalloc(1,sizeof(BIPOINT2LIST));
	if(neo!=NULL)
	{
		neo->prev=NULL;
		neo->next=NULL;
		BiSetPoint2(&neo->pnt,x,y);
		if(npnt==0)
		{
			ptop=neo;
			pbtm=neo;
		}
		else
		{
			pbtm->next=neo;
			neo->prev=pbtm;
			pbtm=neo;
		}
		npnt++;
	}
	return neo;
}

static int BiP2ReadStandardObj(int oType)
{
	int ac,cmd,ret;
	char str[256],*av[32];
	BIPC2OBJLIST *neo;

	if((neo=BiP2NewObjList(oType))==NULL)
	{
		return BI_ERR;
	}

	npnt=0;
	ret=BI_OK;
	while(BiGetStringFuncEx(str,255)!=NULL && ret==BI_OK)
	{
		BiArguments(&ac,av,31,str);
		if(ac==0)
		{
			continue;
		}
		if(BiCommandNumber(&cmd,av[0],objcmd)!=BI_OK)
		{
			return BI_ERR;
		}

		switch(cmd)
		{
		case BIP2STD_COLOR:
			ret=BiP2ReadStdObjColor(neo,ac,av);
			break;
		case BIP2STD_VERTEX:
			ret=BiP2ReadStdObjVertex(neo,ac,av);
			break;
		case BIP2STD_DISTANCE:
			ret=BiP2ReadStdObjDistance(neo,ac,av);
			break;
		case BIP2STD_ENDOBJ:
			neo->np=npnt;
			neo->top=ptop;
			return BI_OK;
		}
	}
	return BI_ERR;
}

static int BiP2ReadStdObjColor(BIPC2OBJLIST *neo,int ac,char *av[])
{
	int r,g,b;
	if(ac>=4)
	{
		r=atoi(av[1]);
		g=atoi(av[2]);
		b=atoi(av[3]);
		BiSetColorRGB(&neo->obj.col,r,g,b);
		return BI_OK;
	}
	return BI_ERR;
}

static int BiP2ReadStdObjVertex(BIPC2OBJLIST *neo,int ac,char *av[])
{
	real x,y;
	if(ac>=3)
	{
		x=(real)atof(av[1]);
		y=(real)atof(av[2]);
		BiP2NewPointList(x,y);
		return BI_OK;
	}
	return BI_ERR;
}

static int BiP2ReadStdObjDistance(BIPC2OBJLIST *neo,int ac,char *av[])
{
	if(ac>=2)
	{
		neo->obj.visiDist=(real)atof(av[1]);
		return BI_OK;
	}
	return BI_ERR;
}

static int BiP2Generate(BIPC2 *pic)
{
	int i;
	unsigned char *buf;
	unsigned long siz;
	BIPOINT2 min,max;
	BIPC2OBJLIST *ptr;

	siz=BiP2ReadSize();
	buf=(unsigned char *)BiMalloc(siz*sizeof(unsigned char));

	pic->nObj=nobj;
	pic->obj=(BIPC2OBJ *)buf;
	buf+=(sizeof(BIPC2OBJ)*nobj/sizeof(unsigned char));

	if(top!=NULL)
	{
		ptr=top;
		BiSetPoint2(&min, 0x7fffffff, 0x7fffffff);
		BiSetPoint2(&max,-0x7fffffff,-0x7fffffff);
		for(i=0; i<nobj; i++)
		{
			pic->obj[i]=ptr->obj;
			switch(ptr->obj.objType)
			{
			case BIP2_PSET:
			case BIP2_POLYLINE:
			case BIP2_LINESEQ:
			case BIP2_POLYG:
			case BIP2_ENDPICT:
				pic->obj[i].objUni.npnt.n=ptr->np;
				pic->obj[i].objUni.npnt.p=(BIPOINT2 *)buf;
				BiP2GenerateMinMax(&min,&max,ptr->top);
				BiP2GenerateCenter(&pic->obj[i].cen,ptr->top);
				buf+=BiP2GeneratePntArray((BIPOINT2 *)buf,ptr->top);
				break;
			}
			ptr=ptr->next;
		}
		pic->min=min;
		pic->max=max;
		BiSubPoint2(&pic->siz,&pic->max,&pic->min);
	}
	else
	{
		BiSetPoint2(&pic->min,0,0);
		BiSetPoint2(&pic->max,0,0);
		BiSetPoint2(&pic->siz,0,0);
	}
	return BI_OK;
}

static void BiP2GenerateMinMax(BIPOINT2 *min,BIPOINT2 *max,BIPOINT2LIST *top)
{
	while(top!=NULL)
	{
		min->x = BiSmaller(min->x,top->pnt.x);
		min->y = BiSmaller(min->y,top->pnt.y);
		max->x = BiLarger(max->x,top->pnt.x);
		max->y = BiLarger(max->y,top->pnt.y);
		top=top->next;
	}
}

static void BiP2GenerateCenter(BIPOINT2 *cen,BIPOINT2LIST *top)
{
	int np;
	BIPOINT2 sum;

	np=0;
	BiSetPoint2(&sum,0,0);
	while(top!=NULL)
	{
		BiAddPoint2(&sum,&sum,&top->pnt);
		top=top->next;
		np++;
	}
	BiDivPoint2(cen,&sum,(real)np);
}

static int BiP2GeneratePntArray(BIPOINT2 *p,BIPOINT2LIST *top)
{
	int np;
	np=0;
	while(top!=NULL)
	{
		*p = top->pnt;
		top=top->next;
		np++;
		p++;
	}
	return ((sizeof(BIPOINT2)*np+3)&~3);
}

static unsigned long BiP2ReadSize(void)
{
	unsigned long siz;
	BIPC2OBJLIST *ptr;

	siz=((sizeof(BIPC2OBJ)*nobj)+3)&~3;
	for(ptr=top; ptr!=NULL; ptr=ptr->next)
	{
		switch(ptr->obj.objType)
		{
		case BIP2_PSET:
		case BIP2_POLYLINE:
		case BIP2_LINESEQ:
		case BIP2_POLYG:
			siz+=((sizeof(BIPOINT2)*ptr->np+3)&~3);
			break;
		}
	}
	return siz;
}



/***************************************************************************/
static void BiDrawPc2Pset(BIPC2OBJ *ob);
static void BiDrawPc2Polyline(BIPC2OBJ *np);
static void BiDrawPc2Lineseq(BIPC2OBJ *np);
static void BiDrawPc2Polygon(BIPC2OBJ *np);
static void BiRenderPc2Pset(BIPC2OBJ *ob,int zBuf);
static void BiRenderPc2Polyline(BIPC2OBJ *ob,int zBuf);
static void BiRenderPc2Lineseq(BIPC2OBJ *ob,int zBuf);
static void BiRenderPc2Polygon(BIPC2OBJ *ob,int zBuf);

static BIPOINTS DwSc;

void BiDrawPc2(BIPC2 *pic,BIPOINTS *sc)
{
	int i;
	DwSc=*sc;
	for(i=0; i<pic->nObj; i++)
	{
		switch(pic->obj[i].objType)
		{
		case BIP2_PSET:
			BiDrawPc2Pset(&pic->obj[i]);
			break;
		case BIP2_POLYLINE:
			BiDrawPc2Polyline(&pic->obj[i]);
			break;
		case BIP2_LINESEQ:
			BiDrawPc2Lineseq(&pic->obj[i]);
			break;
		case BIP2_POLYG:
			BiDrawPc2Polygon(&pic->obj[i]);
			break;
		}
	}
}

static void BiDwPicToScn(BIPOINTS *sc,BIPOINT2 *p)
{
	sc->x = (long)p->x +DwSc.x;
	sc->y = DwSc.y-(long)p->y;
}

static void BiDrawPc2Pset(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS tmp;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n; i++)
	{
		BiDwPicToScn(&tmp,&np->p[i]);
		BiDrawPset2D(&tmp,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkPset2(&tmp,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawPset(&tmp,&ob->col);
		}
	}
}

static void BiDrawPc2Polyline(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS s1,s2;
	int i;

	np=&ob->objUni.npnt;
	BiDwPicToScn(&s2,&np->p[0]);
	for(i=1; i<np->n; i++)
	{
		s1=s2;
		BiDwPicToScn(&s2,&np->p[i]);
		BiDrawLine2D(&s1,&s2,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkLine2(&s1,&s2,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawLine(&s1,&s2,&ob->col);
		}
	}
}

static void BiDrawPc2Lineseq(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS s1,s2;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n-1; i+=2)
	{
		BiDwPicToScn(&s1,&np->p[i  ]);
		BiDwPicToScn(&s2,&np->p[i+1]);
		BiDrawLine2D(&s1,&s2,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkLine2(&s1,&s2,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawLine(&s1,&s2,&ob->col);
		}
	}
}

static void BiDrawPc2Polygon(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS *s;
	int i;
	unsigned long stk;

	np=&ob->objUni.npnt;
	s=(BIPOINTS *)BiPushTmpStack(&stk,sizeof(BIPOINTS)*np->n);
	if(s!=NULL)
	{
		for(i=0; i<np->n; i++)
		{
			BiDwPicToScn(&s[i],&np->p[i]);
		}
		BiDrawPolyg2D(np->n,s,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkPolygon2(np->n,s,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawPolygon(np->n,s,&ob->col);
		}
		BiPopTmpStack(stk);
	}
}




/*  min                                                             *
 *   |<----    siz     ------------>|                               *
 *                                                                  *
 *             s1                    s2                             *
 *              |<------ d  --------->|                             */

static void BiDrawPc2ZoomPset(BIPC2OBJ *ob);
static void BiDrawPc2ZoomPolyline(BIPC2OBJ *ob);
static void BiDrawPc2ZoomLineseq(BIPC2OBJ *ob);
static void BiDrawPc2ZoomPolygon(BIPC2OBJ *ob);

static real ZmDx,ZmDy;
static BIPOINTS ZmS1,ZmS2;
static BIPOINT2 ZmMin,ZmSiz;

void BiDrawPc2Zoom(BIPC2 *pic,BIPOINTS *s1,BIPOINTS *s2)
{
	int i;
	ZmDx=(real)(s2->x -s1->x);
	ZmDy=(real)(s2->y -s1->y);
	ZmS1.x=s1->x;
	ZmS1.y=s1->y;
	ZmS2.x=s2->x;
	ZmS2.y=s2->y;
	ZmMin=pic->min;
	ZmSiz=pic->siz;
	for(i=0; i<pic->nObj; i++)
	{
		switch(pic->obj[i].objType)
		{
		case BIP2_PSET:
			BiDrawPc2ZoomPset(&pic->obj[i]);
			break;
		case BIP2_POLYLINE:
			BiDrawPc2ZoomPolyline(&pic->obj[i]);
			break;
		case BIP2_LINESEQ:
			BiDrawPc2ZoomLineseq(&pic->obj[i]);
			break;
		case BIP2_POLYG:
			BiDrawPc2ZoomPolygon(&pic->obj[i]);
			break;
		}
	}
}

static void BiZmPicToScn(BIPOINTS *s,BIPOINT2 *p)
{
	BIPOINT2 tmp;
	BiSubPoint2(&tmp,p,&ZmMin);
	s->x = ZmS1.x +(long)(tmp.x * (real)ZmDx / ZmSiz.x);
	s->y = ZmS2.y -(long)(tmp.y * (real)ZmDy / ZmSiz.y);
}

static void BiDrawPc2ZoomPset(BIPC2OBJ *ob)
{
	BIPOINTS sc;
	BIPC2NPOINT *np;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n; i++)
	{
		BiZmPicToScn(&sc,&np->p[i]);
		BiDrawPset2D(&sc,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkPset2(&sc,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawPset(&sc,&ob->col);
		}
	}
}

static void BiDrawPc2ZoomPolyline(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS s1,s2;
	int i;

	np=&ob->objUni.npnt;
	BiZmPicToScn(&s2,&np->p[0]);
	for(i=1; i<np->n; i++)
	{
		s1=s2;
		BiZmPicToScn(&s2,&np->p[i]);
		BiDrawLine2D(&s1,&s2,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkLine2(&s1,&s2,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawLine(&s1,&s2,&ob->col);
		}
	}
}

static void BiDrawPc2ZoomLineseq(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS s1,s2;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n-1; i+=2)
	{
		BiZmPicToScn(&s1,&np->p[i  ]);
		BiZmPicToScn(&s2,&np->p[i+1]);
		BiDrawLine2D(&s1,&s2,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkLine2(&s1,&s2,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawLine(&s1,&s2,&ob->col);
		}
	}
}

static void BiDrawPc2ZoomPolygon(BIPC2OBJ *ob)
{
	BIPC2NPOINT *np;
	BIPOINTS *s;
	int i;
	unsigned long stk;

	np=&ob->objUni.npnt;
	s=(BIPOINTS *)BiPushTmpStack(&stk,sizeof(BIPOINTS)*np->n);
	if(s!=NULL)
	{
		for(i=0; i<np->n; i++)
		{
			BiZmPicToScn(&s[i],&np->p[i]);
		}
		BiDrawPolyg2D(np->n,s,&ob->col);
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderLinkPolygon2(np->n,s,&ob->col);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsDrawPolygon(np->n,s,&ob->col);
		}
		BiPopTmpStack(stk);
	}
}


/***************************************************************************/
static void BiDrawPc2Pset3(BIPC2OBJ *ob,int zBuf);
static void BiDrawPc2Polyline3(BIPC2OBJ *ob,int zBuf);
static void BiDrawPc2Lineseq3(BIPC2OBJ *ob,int zBuf);
static void BiDrawPc2Polygon3(BIPC2OBJ *ob,int zBuf);
static void BiEpsPc2Pset(BIPC2OBJ *ob,int zBuf);
static void BiEpsPc2Polyline(BIPC2OBJ *ob,int zBuf);
static void BiEpsPc2Lineseq(BIPC2OBJ *ob,int zBuf);
static void BiEpsPc2Polygon(BIPC2OBJ *ob,int zBuf);
static int BiP2VisibilityCheck(BIPC2OBJ *ob);
static int BiP2InsideViewPort(BIPC2 *pic,int *clp);
static void BiRenderPc2(BIPC2 *pic,int zBuf);
static void BiEpsPc2(BIPC2 *pic);

#ifdef BIACCEL3D
static void BiDrawPc23(BIPC2 *pic,BIPOSATT *pos);
#endif


static int OvClp;

static BIAXISF OvAxs;
extern BIAXISF BiCnvAxs,BiMdlAxs,BiEyeAxs;

static void BiPc2ConvLtoG(BIPOINT *dst,BIPOINT *org)
{
	BiConvLtoG(dst,org,&OvAxs);
}

void BiOvwPc2(BIPC2 *pic,BIPOSATT *pnt)
{
	int i;

	BiMakeAxisF(&OvAxs,pnt);
	BiConvAxisLtoGF(&OvAxs,&OvAxs,&BiCnvAxs);

	if(BiVrmlMode==BI_ON)
	{
		BiPushMatrix(pnt);
		BiRenderPc2(pic,BI_OFF);
		BiPopMatrix();
	}

	if(BiP2InsideViewPort(pic,&OvClp)==BI_IN)
	{
		BiPushMatrix(pnt);
		for(i=0; i<pic->nObj; i++)
		{
			if(BiP2VisibilityCheck(&pic->obj[i])==BI_TRUE)
			{
				switch(pic->obj[i].objType)
				{
				case BIP2_PSET:
					BiDrawPc2Pset3(&pic->obj[i],BI_OFF);
					break;
				case BIP2_POLYLINE:
					BiDrawPc2Polyline3(&pic->obj[i],BI_OFF);
					break;
				case BIP2_LINESEQ:
					BiDrawPc2Lineseq3(&pic->obj[i],BI_OFF);
					break;
				case BIP2_POLYG:
					BiDrawPc2Polygon3(&pic->obj[i],BI_OFF);
					break;
				}
			}
		}

		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderPc2(pic,BI_OFF);
		}
		if(BiEpsMode==BI_ON)
		{
			BiEpsPc2(pic);
		}

		BiPopMatrix();
	}
}

static int BiP2VisibilityCheck(BIPC2OBJ *ob)
{
	BIPOINT div;
	BiSetPoint(&div,ob->cen.x,ob->cen.y,0);
	BiConvLtoG(&div,&div,&OvAxs);
	if(BiAbs(div.x)<=ob->visiDist && BiAbs(div.y)<=ob->visiDist && BiAbs(div.z)<=ob->visiDist)
	{
		return BI_TRUE;
	}
	else
	{
		return BI_FALSE;
	}
}

static int BiP2InsideViewPort(BIPC2 *pic,int *clp)
{
	int i;
	BIPOINT edg[4];
	BiSetPoint(&edg[0],pic->min.x,pic->min.y,0);
	BiSetPoint(&edg[1],pic->max.x,pic->min.y,0);
	BiSetPoint(&edg[2],pic->max.x,pic->max.y,0);
	BiSetPoint(&edg[3],pic->min.x,pic->max.y,0);
	for(i=0; i<4; i++)
	{
		BiConvLtoG(&edg[i],&edg[i],&OvAxs);
	}
	return BiCheckInsideViewPort(clp,4,edg);
}

static void BiDrawPc2Pset3(BIPC2OBJ *ob,int zBuf)
{
	BIPOINT tmp;
	BIPC2NPOINT *np;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n; i++)
	{
		BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);

#ifndef BIACCEL3D
		BiPc2ConvLtoG(&tmp,&tmp);
		if(BiPrj.nearz<=tmp.z && tmp.z<=BiPrj.farz)
		{
			BIPOINTS scn;
			BiSetOrthoPers(&tmp);
			BiProject(&scn,&tmp,&BiPrj);
			BiDrawPset2D(&scn,&ob->col);
		}
#else
		BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);
		switch(zBuf)
		{
		case BI_ON:
			BiOverWritePset3(&tmp,&ob->col);
			break;
		case BI_OFF:
			BiDrawPset3(&tmp,&ob->col);
			break;
		}
#endif
	}
}

static void BiEpsPc2Pset(BIPC2OBJ *ob,int zBuf)
{
	BIPOINT tmp;
	BIPC2NPOINT *np;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n; i++)
	{
		BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);

		BiPc2ConvLtoG(&tmp,&tmp);

		if(BiPrj.nearz<=tmp.z && tmp.z<=BiPrj.farz)
		{
			BiSetOrthoPers(&tmp);
			BiEpsDrawPset3(&tmp,&ob->col);
		}
	}
}

static void BiDrawPc2Polyline3(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	int i;

#ifndef BIACCEL3D
	BIPOINT l[2];

	np=&ob->objUni.npnt;

	if(OvClp==BI_OFF)
	{
		BIPOINTS s1,s2;
		BIPOINT tmp;

		BiSetPoint(&tmp,np->p[0].x,np->p[0].y,0);
		BiPc2ConvLtoG(&tmp,&tmp);
		BiSetOrthoPers(&tmp);

		BiProject(&s2,&tmp,&BiPrj);
		for(i=1; i<np->n; i++)
		{
			s1=s2;
			BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);
			BiPc2ConvLtoG(&tmp,&tmp);
			BiSetOrthoPers(&tmp);
			BiProject(&s2,&tmp,&BiPrj);
			BiDrawLine2D(&s1,&s2,&ob->col);
		}
	}
	else
	{
		BIPOINTS s1,s2;

		for(i=0; i<np->n-1; i++)
		{
			BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
			BiPc2ConvLtoG(&l[0],&l[0]);
			BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);
			BiPc2ConvLtoG(&l[1],&l[1]);

			if(l[0].z>BiPrj.nearz || l[1].z>BiPrj.nearz)
			{
				BiNearClipLine(l,l,BiPrj.nearz);

				BiSetOrthoPers(&l[0]);
				BiSetOrthoPers(&l[1]);
				BiProject(&s1,&l[0],&BiPrj);
				BiProject(&s2,&l[1],&BiPrj);

				BiDrawLine2D(&s1,&s2,&ob->col);
			}
		}
	}
#else
	BIPOINT p1,p2;

	np=&ob->objUni.npnt;

	BiSetPoint(&p2,np->p[0].x,np->p[0].y,0);

	for(i=0; i<np->n; i++)
	{
		p1=p2;
		BiSetPoint(&p2,np->p[i].x,np->p[i].y,0);

		switch(zBuf)
		{
		case BI_OFF:
			BiOverWriteLine3(&p1,&p2,&ob->col);
			break;
		case BI_ON:
			BiDrawLine3(&p1,&p2,&ob->col);
			break;
		}
	}
#endif
}

static void BiEpsPc2Polyline(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	BIPOINT l[2];
	int i;

	np=&ob->objUni.npnt;

	for(i=0; i<np->n-1; i++)
	{
		BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
		BiPc2ConvLtoG(&l[0],&l[0]);
		BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);
		BiPc2ConvLtoG(&l[1],&l[1]);

		if(l[0].z>BiPrj.nearz || l[1].z>BiPrj.nearz)
		{
			BiNearClipLine(l,l,BiPrj.nearz);
			BiSetOrthoPers(&l[0]);
			BiSetOrthoPers(&l[1]);
			BiEpsDrawLine3(&l[0],&l[1],&ob->col);
		}
	}
}

static void BiDrawPc2Lineseq3(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	BIPOINT l[2];
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n-1; i+=2)
	{
#ifndef BIACCEL3D
		BIPOINTS s1,s2;

		BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
		BiPc2ConvLtoG(&l[0],&l[0]);
		BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);
		BiPc2ConvLtoG(&l[1],&l[1]);

		if(l[0].z>BiPrj.nearz || l[1].z>BiPrj.nearz)
		{
			if(OvClp==BI_ON)
			{
				BiNearClipLine(l,l,BiPrj.nearz);
			}

			BiSetOrthoPers(&l[0]);
			BiSetOrthoPers(&l[1]);
			BiProject(&s1,&l[0],&BiPrj);
			BiProject(&s2,&l[1],&BiPrj);
			BiDrawLine2D(&s1,&s2,&ob->col);
		}
#else
		BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
		BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);

		switch(zBuf)
		{
		case BI_ON:
			BiDrawLine3(&l[0],&l[1],&ob->col);
			break;
		case BI_OFF:
			BiOverWriteLine3(&l[0],&l[1],&ob->col);
			break;
		}
#endif
	}
}


static void BiEpsPc2Lineseq(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	BIPOINT l[2];
	int i;

	np=&ob->objUni.npnt;

	for(i=0; i<np->n-1; i+=2)
	{
		BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
		BiPc2ConvLtoG(&l[0],&l[0]);
		BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);
		BiPc2ConvLtoG(&l[1],&l[1]);

		if(l[0].z>BiPrj.nearz || l[1].z>BiPrj.nearz)
		{
			if(OvClp==BI_ON)
			{
				BiNearClipLine(l,l,BiPrj.nearz);
			}
			BiSetOrthoPers(&l[0]);
			BiSetOrthoPers(&l[1]);
			BiEpsDrawLine3(&l[0],&l[1],&ob->col);
		}
	}
}

static void BiDrawPc2Polygon3(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	int i;
	unsigned long stk;

#ifndef BIACCEL3D
	BIPOINT tmp;
	BIPOINTS *s;
	int nc,clp;

	np=&ob->objUni.npnt;

	if(OvClp==BI_OFF)
	{
		s=(BIPOINTS *)BiPushTmpStack(&stk,sizeof(BIPOINTS)*np->n);
		if(s!=NULL)
		{
			for(i=0; i<np->n; i++)
			{
				BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);
				BiPc2ConvLtoG(&tmp,&tmp);
				BiSetOrthoPers(&tmp);
				BiProject(&s[i],&tmp,&BiPrj);
			}
			BiDrawPolyg2D(np->n,s,&ob->col);
			BiPopTmpStack(stk);
		}
	}
	else
	{
		BIPOINT *tab1,*tab2;

		s=(BIPOINTS *)BiPushTmpStack(&stk,sizeof(BIPOINTS)*np->n*4);
		tab1=(BIPOINT *)BiGetTmpStack(sizeof(BIPOINT)*np->n);
		tab2=(BIPOINT *)BiGetTmpStack(sizeof(BIPOINT)*np->n*4);
		clp=BI_OFF;
		if(s!=NULL && tab1!=NULL && tab2!=NULL)
		{
			for(i=0; i<np->n; i++)
			{
				BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);
				BiPc2ConvLtoG(&tab1[i],&tmp);
				clp=(tab1[i].z<BiPrj.nearz ? BI_ON : clp);
			}

			switch(clp)
			{
			case BI_ON:
				BiNearClipPolyg(&nc,tab2,np->n,tab1,BiPrj.nearz);
				BiPlProject(nc,s,tab2,&BiPrj);
				BiDrawPolyg2D(nc,s,&ob->col);
				break;
			case BI_OFF:
				BiPlProject(np->n,s,tab1,&BiPrj);
				BiDrawPolyg2D(np->n,s,&ob->col);
				break;
			}
			BiPopTmpStack(stk);
		}
	}
#else
	BIPOINT nom;
	BIPOINT *tab1;

	np=&ob->objUni.npnt;

	tab1=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*np->n);
	if(tab1!=NULL)
	{
		for(i=0; i<np->n; i++)
		{
			BiSetPoint(&tab1[i],np->p[i].x,np->p[i].y,0);
		}
		switch(zBuf)
		{
		case BI_ON:
			BiSetPoint(&nom,0,0,-1);
			BiDrawPolyg3(np->n,tab1,&nom,&ob->col);
			break;
		case BI_OFF:
			BiOverWritePolyg3(np->n,tab1,&ob->col);
			break;
		}
		BiPopTmpStack(stk);
	}
#endif
}



static void BiEpsPc2Polygon(BIPC2OBJ *ob,int zBuf)
{
	int i;
	unsigned long stk;
	BIPC2NPOINT *np;
	BIPOINT *tab1;

	np=&ob->objUni.npnt;

	tab1=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*np->n);
	if(tab1!=NULL)
	{
		for(i=0; i<np->n; i++)
		{
			BiSetPoint(&tab1[i],np->p[i].x,np->p[i].y,0);
			BiPc2ConvLtoG(&tab1[i],&tab1[i]);
			BiSetOrthoPers(&tab1[i]);
		}
		BiEpsDrawPolyg3(np->n,tab1,&ob->col);
		BiPopTmpStack(stk);
	}
}


/**********************************************************************S*/
static void BiRenderPc2Pset(BIPC2OBJ *ob,int zBuf)
{
	BIPOINT tmp;
	BIPC2NPOINT *np;
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n; i++)
	{
		BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);
		BiPc2ConvLtoG(&tmp,&tmp);
		if(BiRendLinkFlag==BI_ON)
		{
			switch(zBuf)
			{
			case BI_OFF:
				BiRenderLinkOvwPset(&tmp,&ob->col);
				break;
			case BI_ON:
				BiRenderLinkPset(&tmp,&ob->col);
				break;
			}
		}
		else if(BiVrmlMode==BI_ON)
		{
			BiVrmlPset(&tmp,&ob->col);
		}
	}
}

static void BiRenderPc2Polyline(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	BIPOINT l[2];
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n-1; i++)
	{
		BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
		BiPc2ConvLtoG(&l[0],&l[0]);
		BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);
		BiPc2ConvLtoG(&l[1],&l[1]);
		if(BiRendLinkFlag==BI_ON)
		{
			switch(zBuf)
			{
			case BI_OFF:
				BiRenderLinkOvwLine(&l[0],&l[1],&ob->col);
				break;
			case BI_ON:
				BiRenderLinkLine(&l[0],&l[1],&ob->col);
				break;
			}
		}
		if(BiVrmlMode==BI_ON)
		{
			BiVrmlLine(&l[0],&l[1],&ob->col);
		}
	}
}

static void BiRenderPc2Lineseq(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	BIPOINT l[2];
	int i;

	np=&ob->objUni.npnt;
	for(i=0; i<np->n-1; i+=2)
	{
		BiSetPoint(&l[0],np->p[i  ].x,np->p[i  ].y,0);
		BiPc2ConvLtoG(&l[0],&l[0]);
		BiSetPoint(&l[1],np->p[i+1].x,np->p[i+1].y,0);
		BiPc2ConvLtoG(&l[1],&l[1]);
		if(BiRendLinkFlag==BI_ON)
		{
			switch(zBuf)
			{
			case BI_OFF:
				BiRenderLinkOvwLine(&l[0],&l[1],&ob->col);
				break;
			case BI_ON:
				BiRenderLinkLine(&l[0],&l[1],&ob->col);
				break;
			}
		}
		if(BiVrmlMode==BI_ON)
		{
			BiVrmlLine(&l[0],&l[1],&ob->col);
		}
	}
}

static void BiRenderPc2Polygon(BIPC2OBJ *ob,int zBuf)
{
	BIPC2NPOINT *np;
	BIPOINT tmp,*tab;
	int i;
	unsigned long stk;

	np=&ob->objUni.npnt;
	tab=(BIPOINT *)BiPushTmpStack(&stk,sizeof(BIPOINT)*np->n);
	if(tab!=NULL)
	{
		for(i=0; i<np->n; i++)
		{
			BiSetPoint(&tmp,np->p[i].x,np->p[i].y,0);
			BiPc2ConvLtoG(&tab[i],&tmp);
		}
		if(BiRendLinkFlag==BI_ON)
		{
			switch(zBuf)
			{
			case BI_OFF:
				BiRenderLinkOvwPolygon(np->n,tab,&ob->col);
				break;
			case BI_ON:
				BiRenderLinkPolygonTwoSide(np->n,tab,&ob->col);
				break;
			}
		}
		if(BiVrmlMode==BI_ON)
		{
			BiVrmlPolygon(np->n,tab,&ob->col);
		}
		BiPopTmpStack(stk);
	}
}




/***************************************************************************/
int BiInsPc2(BIPC2 *pic,BIPOSATT *pos)
{
	BIOBJECT *neo;

	BiMakeAxisF(&OvAxs,pos);
	BiConvAxisLtoGF(&OvAxs,&OvAxs,&BiCnvAxs);

	if(BiVrmlMode==BI_ON)
	{
		BiRenderPc2(pic,BI_ON);
	}

	if(BiP2InsideViewPort(pic,&OvClp)==BI_IN)
	{
		if(BiRendLinkFlag==BI_ON)
		{
			BiRenderPc2(pic,BI_ON);
		}

#ifndef BIACCEL3D
		neo=BiNew3dObject(BIOBJ_PICT2,OvAxs.p.z);
		if(neo==NULL)
		{
			return BI3DG_STACKOVERFLOW;
		}
		neo->attr.pc2.clip=OvClp;
		neo->attr.pc2.axs=OvAxs;
		neo->attr.pc2.pc2=pic;
#else
		BiDrawPc23(pic,pos);
		if(BiEpsMode==BI_ON)
		{
			neo=BiNew3dObject(BIOBJ_PICT2,OvAxs.p.z);
			if(neo==NULL)
			{
				return BI3DG_STACKOVERFLOW;
			}
			neo->attr.pc2.clip=OvClp;
			neo->attr.pc2.axs=OvAxs;
			neo->attr.pc2.pc2=pic;
		}
#endif
	}
	return BI3DG_NOERR;
}

/* OvAxs must be set before calling */
static void BiRenderPc2(BIPC2 *pic,int zBuf)
{
	int i;
	for(i=0; i<pic->nObj; i++)
	{
		if(BiP2VisibilityCheck(&pic->obj[i])==BI_TRUE)
		{
			switch(pic->obj[i].objType)
			{
			case BIP2_PSET:
				BiRenderPc2Pset(&pic->obj[i],zBuf);
				break;
			case BIP2_POLYLINE:
				BiRenderPc2Polyline(&pic->obj[i],zBuf);
				break;
			case BIP2_LINESEQ:
				BiRenderPc2Lineseq(&pic->obj[i],zBuf);
				break;
			case BIP2_POLYG:
				BiRenderPc2Polygon(&pic->obj[i],zBuf);
				break;
			}
		}
	}
}

static void BiEpsPc2(BIPC2 *pic)
{
	int i;
	for(i=0; i<pic->nObj; i++)
	{
		if(BiP2VisibilityCheck(&pic->obj[i])==BI_TRUE)
		{
			switch(pic->obj[i].objType)
			{
			case BIP2_PSET:
				BiEpsPc2Pset(&pic->obj[i],BI_OFF);
				break;
			case BIP2_POLYLINE:
				BiEpsPc2Polyline(&pic->obj[i],BI_OFF);
				break;
			case BIP2_LINESEQ:
				BiEpsPc2Lineseq(&pic->obj[i],BI_OFF);
				break;
			case BIP2_POLYG:
				BiEpsPc2Polygon(&pic->obj[i],BI_OFF);
				break;
			}
		}
	}
}


#ifndef BIACCEL3D
void BiFlushDrawPc2(BIPC2 *pic,BIAXISF *axs,int clip)
{
	int i;
	OvClp=clip;
	OvAxs=*axs;

	for(i=0; i<pic->nObj; i++)
	{
		if(BiP2VisibilityCheck(&pic->obj[i])==BI_TRUE)
		{
			switch(pic->obj[i].objType)
			{
			case BIP2_PSET:
				BiDrawPc2Pset3(&pic->obj[i],BI_ON);
				break;
			case BIP2_POLYLINE:
				BiDrawPc2Polyline3(&pic->obj[i],BI_ON);
				break;
			case BIP2_LINESEQ:
				BiDrawPc2Lineseq3(&pic->obj[i],BI_ON);
				break;
			case BIP2_POLYG:
				BiDrawPc2Polygon3(&pic->obj[i],BI_ON);
				break;
			}
		}
	}

	if(BiEpsMode==BI_ON)
	{
		BiEpsPc2(pic);
	}
}
#else
void BiFlushDrawPc2(BIPC2 *pic,BIAXISF *axs,int clip)
{
	OvClp=clip;
	OvAxs=*axs;

	if(BiEpsMode==BI_ON)
	{
		BiEpsPc2(pic);
	}
}

static void BiDrawPc23(BIPC2 *pic,BIPOSATT *pos)
{
	int i;

	BiMakeAxisF(&OvAxs,pos);
	BiConvAxisLtoGF(&OvAxs,&OvAxs,&BiCnvAxs);

	BiPushMatrix(pos);
	for(i=0; i<pic->nObj; i++)
	{
		if(BiP2VisibilityCheck(&pic->obj[i])==BI_TRUE)
		{
			switch(pic->obj[i].objType)
			{
			case BIP2_PSET:
				BiDrawPc2Pset3(&pic->obj[i],BI_ON);
				break;
			case BIP2_POLYLINE:
				BiDrawPc2Polyline3(&pic->obj[i],BI_ON);
				break;
			case BIP2_LINESEQ:
				BiDrawPc2Lineseq3(&pic->obj[i],BI_ON);
				break;
			case BIP2_POLYG:
				BiDrawPc2Polygon3(&pic->obj[i],BI_ON);
				break;
			}
		}
	}
	BiPopMatrix();
}
#endif
