#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impulse.h"
#include "i3dg.h"


#define BICLICK_SQUARE 1
#define BICLICK_POLYG  2
#define BICLICK_LINE   4

#define BICLICK_ALL 0x7fffffff

typedef struct {
	int pix;
	BIPOINTS sc;
} BICLICKABLERECT;

typedef struct {
	BIPOINTS min,max;
	BIPOLYGON2 plg;
} BICLICKABLEPOLYG;

typedef struct {
	BIPOINTS min,max;
	BILINE2 ln;
} BICLICKABLELINE;

typedef union {
	BICLICKABLERECT rect;
	BICLICKABLEPOLYG plg;
	BICLICKABLELINE ln;
} BICLICKABLEDAT;

typedef struct BiClickableTag {
	struct BiClickableTag *root;
	struct BiClickableTag *next,*prev;

	int type;
	BICLICKABLEDAT dat;
	real z;

	int id1,id2,id3;
	void *extra;
} BICLICKABLE;


/*************************************************************************/
static BICLICKABLE *BiNewClickable(real z,int id1,int id2,int id3,void *ext);
static int BiExecCheckClicked(int *i1,int *i2,int *i3,void **ext,BICLICKABLE *ptr,BIPOINTS *sc,int *ctr,int target);
static int BiCheckHit(BICLICKABLE *ptr,BIPOINTS *s);
void *BiPushClickableBuffer(int *used,int siz);
void BiPopClickableBuffer(int used);
/*************************************************************************/

static BICLICKABLE *clTab=NULL;

static int nBuf=0;
static int nBufUsed=0;
static char *clBuf=NULL;
/*************************************************************************/

int BiSetClickableBuffer(char buf[],int nbuf)
{
	nBuf=nbuf;
	nBufUsed=0;
	clBuf=buf;
	return BI_OK;
}

int BiClearClickable(void)
{
	clTab=NULL;
	nBufUsed=0;
	return BI_OK;
}

int BiInsClickable(BIPOINT *p,real rad,int pix,int i1,int i2,int i3,void *extra)
{
	int clip,siz;
	real z;
	BICLICKABLE *neo;
	BIPOINT tmp,sq[4];

	BiConvLtoG(&tmp,p,&BiCnvAxs);
	z=tmp.z;

	if(z>0)
	{
		BiSetOrthoPers(&tmp);

		BiSetPoint(&sq[0],tmp.x-rad,tmp.y-rad,tmp.z);
		BiSetPoint(&sq[1],tmp.x-rad,tmp.y+rad,tmp.z);
		BiSetPoint(&sq[2],tmp.x+rad,tmp.y+rad,tmp.z);
		BiSetPoint(&sq[3],tmp.x+rad,tmp.y-rad,tmp.z);

		if(BiCheckInsideViewPort(&clip,4,sq)==BI_IN)
		{
			if((neo=BiNewClickable(z,i1,i2,i3,extra))!=NULL)
			{
				siz=(long)(rad*(BiPrj.magx+BiPrj.magy)/2.0F/z);

				neo->type=BICLICK_SQUARE;
				neo->dat.rect.pix=BiLarger(siz,pix);
				BiProject(&neo->dat.rect.sc,&tmp,&BiPrj);

				return BI_OK;
			}
			return BI_ERR;
		}
	}
	return BI_OK;
}

int BiInsClickablePolyg(int np,BIPOINT p[],BIVECTOR *n,int i1,int i2,int i3,void *extra)
{
	int i;
	int clip;
	int ptr;
	BIPOINT2 *scr;
	BIPOINT *loc,cen,nom;
	int nq;
	BIPOINT *q;
	BIPOINTS s,min,max;
	BICLICKABLE *neo;
	long wid,hei;

	/** BackFaceRemove **/
	cen=p[0];
	for(i=1; i<np; i++)
	{
		BiAddPoint(&cen,&cen,&p[i]);
	}
	BiDivPoint(&cen,&cen,np);
	BiConvLtoG(&cen,&cen,&BiCnvAxs);
	if(n!=NULL)
	{
		BiRotFastLtoG(&nom,n,&BiCnvAxs.t);
		if((BiPrjMode==BIPRJ_PERS && BiInnerPoint(&cen,&nom)>0) ||
		   (BiPrjMode==BIPRJ_ORTHO && nom.z>0))
		{
			return BI_OK;
		}
	}
	/********************/
	if((loc=(BIPOINT *)BiPushClickableBuffer(&ptr,sizeof(BIPOINT)*np))!=NULL)
	{
		for(i=0; i<np; i++)
		{
			BiConvLtoG(&loc[i],&p[i],&BiCnvAxs);
		}

		if(BiCheckInsideViewPort(&clip,np,loc)==BI_IN)
		{
			BiGetWindowSize(&wid,&hei);
			BiSetPointS(&max,0,0);
			BiSetPointS(&min,wid,hei);

			if(clip!=BI_ON || BiPrjMode==BIPRJ_ORTHO)
			{
				scr=(BIPOINT2 *)loc;  /* Danger? ^_^; */
				if(scr!=NULL)
				{
					for(i=0; i<np; i++)
					{
						BiSetOrthoPers(&loc[i]);
						BiProject(&s,&loc[i],&BiPrj);
						BiSetPoint2(&scr[i],s.x,s.y);

						max.x=BiLarger(max.x,s.x);
						max.y=BiLarger(max.y,s.y);
						min.x=BiSmaller(min.x,s.x);
						min.y=BiSmaller(min.y,s.y);
					}
				}
			}
			else
			{
				BiPushClickableBuffer(NULL,sizeof(BIPOINT2)*np*2-sizeof(BIPOINT)*np);
				scr=(BIPOINT2 *)loc;
				q=(BIPOINT *)BiPushClickableBuffer(&ptr,sizeof(BIPOINT)*np*2);
				if(q!=NULL)
				{
					BiNearClipPolyg(&nq,q,np,loc,BiPrj.nearz);
					for(i=0; i<nq; i++)
					{
						BiProject(&s,&q[i],&BiPrj);
						BiSetPoint2(&scr[i],s.x,s.y);

						max.x=BiLarger(max.x,s.x);
						max.y=BiLarger(max.y,s.y);
						min.x=BiSmaller(min.x,s.x);
						min.y=BiSmaller(min.y,s.y);
					}
					BiPopClickableBuffer(ptr);
				}
				np=nq;
			}
			if((neo=BiNewClickable(cen.z,i1,i2,i3,extra))!=NULL)
			{
				neo->type=BICLICK_POLYG;
				neo->dat.plg.plg.np=np;
				neo->dat.plg.plg.p=scr;
				neo->dat.plg.min=min;
				neo->dat.plg.max=max;
				return BI_OK;
			}
			return BI_OK;
		}
		BiPopClickableBuffer(ptr);
		return BI_OK;
	}
	return BI_ERR;
}

int BiInsClickableLine(BIPOINT *p1,BIPOINT *p2,int i1,int i2,int i3,void *extra)
{
	int clip;
	BIPOINT cen;
	BIPOINT q[2];
	BIPOINT2 r[2];
	BIPOINTS s[2],min,max;
	BICLICKABLE *neo;

	BiAddPoint(&cen,p1,p2);
	BiDivPoint(&cen,&cen,2);
	BiConvLtoG(&cen,&cen,&BiCnvAxs);

	BiConvLtoG(&q[0],p1,&BiCnvAxs);
	BiConvLtoG(&q[1],p2,&BiCnvAxs);

	if(BiCheckInsideViewPort(&clip,2,q)==BI_IN)
	{
		if(clip!=BI_ON || BiPrjMode==BIPRJ_ORTHO)
		{
			BiSetOrthoPers(&q[0]);
			BiSetOrthoPers(&q[1]);
		}
		else
		{
			q[0]=*p1;
			q[1]=*p2;
			BiNearClipLine(q,q,BiPrj.nearz);
		}


		BiProject(&s[0],&q[0],&BiPrj);
		BiProject(&s[1],&q[1],&BiPrj);

		BiSetPoint2(&r[0],s[0].x,s[0].y);
		BiSetPoint2(&r[1],s[1].x,s[1].y);

		max.x=BiLarger(s[0].x,s[1].x);
		max.y=BiLarger(s[0].y,s[1].y);
		min.x=BiSmaller(s[0].x,s[1].x);
		min.y=BiSmaller(s[0].y,s[1].y);

		if((neo=BiNewClickable(cen.z,i1,i2,i3,extra))!=NULL)
		{
			neo->type=BICLICK_LINE;
			neo->dat.ln.min=min;
			neo->dat.ln.max=max;
			BiMakeLine2(&neo->dat.ln.ln,&r[0],&r[1]);
			return BI_OK;
		}
		return BI_ERR;
	}
	return BI_OK;
}

int BiCheckClicked(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr)
{
	if(ctr<1)
	{
		ctr=1;
	}
	if(clTab!=NULL)
	{
		return BiExecCheckClicked(i1,i2,i3,ext,&clTab[0],sc,&ctr,BICLICK_ALL);
	}
	else
	{
		return BI_ERR;
	}
}

int BiCheckClickedPoint(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr)
{
	if(ctr<1)
	{
		ctr=1;
	}
	if(clTab!=NULL)
	{
		return BiExecCheckClicked(i1,i2,i3,ext,&clTab[0],sc,&ctr,BICLICK_SQUARE);
	}
	else
	{
		return BI_ERR;
	}
}

int BiCheckClickedPolyg(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr)
{
	if(ctr<1)
	{
		ctr=1;
	}
	if(clTab!=NULL)
	{
		return BiExecCheckClicked(i1,i2,i3,ext,&clTab[0],sc,&ctr,BICLICK_POLYG);
	}
	else
	{
		return BI_ERR;
	}
}

int BiCheckClickedLine(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr)
{
	if(ctr<1)
	{
		ctr=1;
	}
	if(clTab!=NULL)
	{
		return BiExecCheckClicked(i1,i2,i3,ext,&clTab[0],sc,&ctr,BICLICK_LINE);
	}
	else
	{
		return BI_ERR;
	}
}

/*************************************************************************/

static BICLICKABLE *BiNewClickable(real z,int i1,int i2,int i3,void *extra)
{
	BICLICKABLE *neo,*seek;

	neo=(BICLICKABLE *)BiPushClickableBuffer(NULL,sizeof(BICLICKABLE));
	if(neo!=NULL)
	{
		neo->z=z;
		neo->root=NULL;
		neo->next=NULL;
		neo->prev=NULL;
		neo->id1=i1;
		neo->id2=i2;
		neo->id3=i3;
		neo->extra=extra;
		if(clTab!=NULL)
		{
			seek=&clTab[0];
			while(1)
			{
				if(z>=seek->z)
				{
					if(seek->next!=NULL)
					{
						seek=seek->next;
					}
					else
					{
						seek->next=neo;
						neo->root=seek;
						break;
					}
				}
				else
				{
					if(seek->prev!=NULL)
					{
						seek=seek->prev;
					}
					else
					{
						seek->prev=neo;
						neo->root=seek;
						break;
					}
				}
			}
		}
		else
		{
			clTab=neo;
		}
		return neo;
	}
	return NULL;
}

static int BiExecCheckClicked(int *i1,int *i2,int *i3,void **ext,BICLICKABLE *ptr,BIPOINTS *sc,int *ctr,int target)
{
	if(ptr->prev!=NULL)
	{
		if(BiExecCheckClicked(i1,i2,i3,ext,ptr->prev,sc,ctr,target)==BI_OK)
		{
			return BI_OK;
		}
	}

	if((ptr->type&target)!=0 && BiCheckHit(ptr,sc)==BI_TRUE)
	{
		(*ctr)--;
		if(*ctr<=0)
		{
			if(i1!=NULL)
			{
				*i1=ptr->id1;
			}
			if(i2!=NULL)
			{
				*i2=ptr->id2;
			}
			if(i3!=NULL)
			{
				*i3=ptr->id3;
			}
			if(ext!=NULL)
			{
				*ext=ptr->extra;
			}
			return BI_OK;
		}
	}

	if(ptr->next!=NULL)
	{
		if(BiExecCheckClicked(i1,i2,i3,ext,ptr->next,sc,ctr,target)==BI_OK)
		{
			return BI_OK;
		}
	}

	return BI_ERR;
}

static int BiCheckHit(BICLICKABLE *ptr,BIPOINTS *s)
{
	BIPOINT2 s2;
	switch(ptr->type)
	{
	default:
		break;
	case BICLICK_SQUARE:
		if(BiAbs(s->x -ptr->dat.rect.sc.x)<ptr->dat.rect.pix &&
		   BiAbs(s->y -ptr->dat.rect.sc.y)<ptr->dat.rect.pix)
		{
			return BI_TRUE;
		}
		break;
	case BICLICK_POLYG:
		if(ptr->dat.plg.min.x<=s->x && s->x<=ptr->dat.plg.max.x &&
		   ptr->dat.plg.min.y<=s->y && s->y<=ptr->dat.plg.max.y)
		{
			BiSetPoint2(&s2,s->x,s->y);
			if(BiCheckInsidePolyg2(&ptr->dat.plg.plg,&s2)==BI_IN)
			{
				return BI_TRUE;
			}
		}
		break;
	case BICLICK_LINE:
		if(ptr->dat.ln.min.x-2<=s->x && s->x<=ptr->dat.ln.max.x+2 &&
		   ptr->dat.ln.min.y-2<=s->y && s->y<=ptr->dat.ln.max.y+2)
		{
			BiSetPoint2(&s2,s->x,s->y);
			if(BiLineDistance2(&ptr->dat.ln.ln,&s2)<=2.0F)
			{
				return BI_TRUE;
			}
		}
		break;
	}
	return BI_FALSE;
}

/*************************************************************************/

void *BiPushClickableBuffer(int *used,int siz)
{
	void *current;

	siz=(siz+3)&~3;

	if(used!=NULL)
	{
		*used=nBufUsed;
	}

	if(nBuf-nBufUsed>=siz)
	{
		current=(void *)(clBuf+nBufUsed);
		nBufUsed+=siz;
		return current;
	}
	return NULL;
}

void BiPopClickableBuffer(int used)
{
	nBufUsed=used;
}

/*************************************************************************/
