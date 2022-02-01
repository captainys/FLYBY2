#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include "impulse.h"

static int BiFindKeenEdge(BIPOINT o[3],int np,BIPOINT *p);

extern real BiPow60Tab[1025];
extern BIPOINT BiUnitTab[4096];

void BiCalcInitialize(void)
{
	/* Nothing Special To Do */
}

int BiQuickNormalize(BIPOINT *dst,BIPOINT *src)
{
	long tabx,taby;
	real abx,aby,abz;
	BIPOINT tmp;
	abx=BiAbs(src->x);
	aby=BiAbs(src->y);
	abz=BiAbs(src->z);
	if(abx>aby && abx>abz && abx>=YSEPS)  /* X is largest */
	{
		BiDivPoint(&tmp,src,src->x);
		tabx=(long)(tmp.z*31)+31;
		taby=(long)(tmp.y*31)+31;
		tmp=BiUnitTab[taby*64+tabx];
		if(src->x>0)
		{
			dst->x=tmp.z;
			dst->y=tmp.y;
			dst->z=tmp.x;
		}
		else
		{
			dst->x=-tmp.z;
			dst->y=-tmp.y;
			dst->z=-tmp.x;
		}
		return BI_OK;
	}
	else if(aby>abz && aby>=YSEPS)         /* Y is largest */
	{
		BiDivPoint(&tmp,src,src->y);
		tabx=(long)(tmp.x*31)+31;
		taby=(long)(tmp.z*31)+31;
		tmp=BiUnitTab[taby*64+tabx];
		if(src->y>0)
		{
			dst->x=tmp.x;
			dst->y=tmp.z;
			dst->z=tmp.y;
		}
		else
		{
			dst->x=-tmp.x;
			dst->y=-tmp.z;
			dst->z=-tmp.y;
		}
		return BI_OK;
	}
	else if(abz>0.0F && abz>=YSEPS)        /* Z is largest */
	{
		BiDivPoint(&tmp,src,src->z);
		tabx=(long)(tmp.x*31)+31;
		taby=(long)(tmp.y*31)+31;
		tmp=BiUnitTab[taby*64+tabx];
		if(src->z>0)
		{
			dst->x=tmp.x;
			dst->y=tmp.y;
			dst->z=tmp.z;
		}
		else
		{
			dst->x=-tmp.x;
			dst->y=-tmp.y;
			dst->z=-tmp.z;
		}
		return BI_OK;
	}
	BiSetPoint(dst,0,0,0);
	return BI_ERR;
}

long BiAsin(real s)
{
	real t;
	t=(real)asin(s);
	return (long)(t*32768.0/YSPI);
}

long BiAcos(real s)
{
	real t;
	t=(real)acos(s);
	return (long)(t*32768.0/YSPI);
}

long BiAtan(real s)
{
	real t;
	t=(real)atan(s);
	return (long)(t*32768.0/YSPI);
}

real BiAsinF(real s)
{
	real t;
	t=(real)asin(s);
	return (real)(t*32768.0F/YSPI);
}

real BiAcosF(real s)
{
	real t;
	t=(real)acos(s);
	return (real)(t*32768.0F/YSPI);
}

real BiAtanF(real s)
{
	real t;
	t=(real)atan(s);
	return (real)(t*32768.0F/YSPI);
}

real BiLength2(real x,real y)
{
/* Testing Accuracy and Delay */
	return (real)sqrt(x*x+y*y);
/*
	x=BiAbs(x);
	y=BiAbs(y);
	if(x==0.0 && y==0.0)
	{
		return 0.0F;
	}
	else if(y<=x)
	{
		return x/BiCos(BiAtan(y/x));
	}
	else
	{
		return y/BiCos(BiAtan(x/y));
	}
*/
}

real BiPow60(real t)
{
	if(1.0F<=t)
	{
		return 1.0F;
	}
	else if(t<0.0F)
	{
		return 0.0F;
	}
	else
	{
		return BiPow60Tab[(int)(t*1024)];
	}
}

void BiNormalize2(BIPOINT2 *dst,BIPOINT2 *src)
{
	real l;
	l=BiLengthPoint2(src);
	if(l>=YSEPS)
	{
		BiDivPoint2(dst,src,l);
	}
}

/* BIPOINT *******************************
#define BiSetPoint(dst,xx,yy,zz)
#define BiAddPoint(dst,a,b)
#define BiSubPoint(dst,a,b)
#define BiMulPoint(dst,src,m)
#define BiDivPoint(dst,src,d)
#define BiInnerPoint(a,b)
#define BiRotLtoG(dst,src,ang)
#define BiRotGtoL(dst,src,ang)
#define BiRotFastLtoG(dst,src,trg)
#define BiRotFastGtoL(dst,src,trg)
#define BiProject(dst,src,prj)
#define BiLength3(x,y,z)
#define BiLengthPoint3(p)
*****************************************/
void BiNearClipLine(BIPOINT *c,BIPOINT *p,real z)
{
	BIPOINT vec;

	if(p[0].z>z && p[1].z>z)
	{
		c[0]=p[0];
		c[1]=p[1];
	}
	else if(p[0].z<=z && p[1].z<=z)
	{
		c[0]=p[0];
		c[0].z=z;
		c[1]=p[1];
		c[1].z=z;
	}
	else if(p[0].z<=z)
	{
		BiSubPoint(&vec,&p[1],&p[0]);
		BiMulPoint(&vec,&vec,(z-p[0].z)/vec.z);
		BiAddPoint(&c[0],&p[0],&vec);
		c[1]=p[1];
	}
	else
	{
		BiSubPoint(&vec,&p[0],&p[1]);
		BiMulPoint(&vec,&vec,(z-p[1].z)/vec.z);
		c[0]=p[0];
		BiAddPoint(&c[1],&p[1],&vec);
	}
}

void BiNearClipPolyg(int *nc,BIPOINT *c,int np,BIPOINT *p,real z)
{
	int i;
	BIPOINT vec;

	*nc=0;
	for(i=0; i<np-1; i++)
	{
		if(p[i].z>z)
		{
			c[*nc]=p[i];
			(*nc)++;
			if(p[i+1].z<=z)
			{
				BiSubPoint(&vec,&p[i+1],&p[i]);
				BiMulPoint(&vec,&vec,(z-p[i].z)/vec.z);
				BiAddPoint(&c[*nc],&p[i],&vec);
				(*nc)++;
			}
		}
		else if(p[i+1].z>z)
		{
			BiSubPoint(&vec,&p[i+1],&p[i]);
			BiMulPoint(&vec,&vec,(z-p[i].z)/vec.z);
			BiAddPoint(&c[*nc],&p[i],&vec);
			(*nc)++;
		}
	}

	if(p[np-1].z>z)
	{
		c[*nc]=p[np-1];
		(*nc)++;
		if(p[0].z<=z)
		{
			BiSubPoint(&vec,&p[0],&p[np-1]);
			BiMulPoint(&vec,&vec,(z-p[np-1].z)/vec.z);
			BiAddPoint(&c[*nc],&p[np-1],&vec);
			(*nc)++;
		}
	}
	else if(p[0].z>z)
	{
		BiSubPoint(&vec,&p[0],&p[np-1]);
		BiMulPoint(&vec,&vec,(z-p[np-1].z)/vec.z);
		BiAddPoint(&c[*nc],&p[np-1],&vec);
		(*nc)++;
	}
}

void BiNormalize(BIPOINT *dst,BIPOINT *src)
{
	real l;
	l=BiLengthPoint3(src);
	if(l>=YSEPS)
	{
		BiDivPoint(dst,src,l);
	}
}

int BiAverageNormalVector(BIPOINT *nom,int np,BIPOINT p[])
{
	BIPOINT k[3],v[2];

	if(BiFindKeenEdge(k,np,p)==BI_TRUE)
	{
		BiSubPoint(&v[0], &k[1],&k[0]);
		BiSubPoint(&v[1], &k[2],&k[1]);
		BiOuterProduct(nom,&v[0],&v[1]);
		return BI_TRUE;
	}
	else
	{
		return BI_FALSE;
	}
}

static int BiFindKeenEdge(BIPOINT o[3],int np,BIPOINT *p)
{
	int i,idTop;
	real vCos,inr;
	BIPOINT v[2];

	vCos = ((real)1.0);
	idTop= 0;
	for(i=0; i<np-2; i++)
	{
		BiSubPoint(&v[0],&p[i+1],&p[i]);
		BiSubPoint(&v[1],&p[i+2],&p[i+1]);
		BiNormalize(&v[0],&v[0]);
		BiNormalize(&v[1],&v[1]);
		inr = BiAbs(BiInnerPoint(&v[0],&v[1]));
		if(inr<vCos)
		{
			vCos = inr;
			idTop=i;
		}
	}

	o[0]=p[idTop];
	o[1]=p[idTop+1];
	o[2]=p[idTop+2];

	if(vCos==1.0)
	{
		return BI_FALSE;
	}
	else
	{
		return BI_TRUE;
	}
}

void BiOuterProduct(BIPOINT *ou,BIPOINT *v1,BIPOINT *v2)
{
	BIPOINT tmp;
	tmp.x = v1->y*v2->z -v1->z*v2->y;
	tmp.y = v1->z*v2->x -v1->x*v2->z;
	tmp.z = v1->x*v2->y -v1->y*v2->x;
	*ou = tmp;
}

void BiRightAngleVector(BIPOINT *ret,BIPOINT *src)
{
	real a,b,c;
	if(BiAbs(src->x)<=YSEPS)
	{
		BiSetPoint(ret,1.0,0,0);
	}
	else if(BiAbs(src->y)<=YSEPS)
	{
		BiSetPoint(ret,0,1.0,0);
	}
	else if(BiAbs(src->z)<=YSEPS)
	{
		BiSetPoint(ret,0,0,1.0);
	}
	else
	{
		/* (a,b,c)(d,e,f) -> ad+be+cf=0                */
		/* P= ad ,Q=  be,R=  cf -> P+Q+R=0             */
		/* P=2abc,Q=-abc,R=-abc -> P+Q+R=0             */
	    /*   -> d=2bc,e=-ac,f=-ab                      */
		a=src->x;
		b=src->y;
		c=src->z;
		BiSetPoint(ret,2*b*c,-a*c,-a*b);
		BiNormalize(ret,ret);
	}
}

/* BIANGLE *******************************
#define BiSetAngleDeg(dst,hh,pp,bb)
#define BiSetAngleRad(dst,hh,pp,bb)
#define BiSetAngle(dst,hh,pp,bb)
#define BiMakeTrigonomy(trg,src)
*****************************************/
void BiAngleToVector(BIPOINT *eye,BIPOINT *up,BIANGLE *an)
{
	BITRIGON trg;
	BiMakeTrigonomy(&trg,an);
	BiSetPoint(eye,0.0,0.0,1.0);
	BiRotFastLtoG(eye,eye,&trg);
	BiSetPoint(up ,0.0,1.0,0.0);
	BiRotFastLtoG(up,up,&trg);
}

void BiVectorToAngle(BIANGLE *an,BIPOINT *eye,BIPOINT *up)
{
	BIPOINT tmp;
	BIPOINT2 v2;

	/* Eye vector derives an->h,an->p. */
	BiVectorToHeadPitch(an,eye);

	/* Up vector derives an->b. */
	BiRotGtoL(&tmp,up,an);
	v2.x= tmp.y;
	v2.y=-tmp.x;
	an->b=BiAngle2(&v2);
}

void BiAngleToVectorF(BIPOINT *eye,BIPOINT *up,BIANGLEF *an)
{
	BITRIGON trg;
	BiMakeTrigonomy(&trg,an);
	BiSetPoint(eye,0.0,0.0,1.0);
	BiRotFastLtoG(eye,eye,&trg);
	BiSetPoint(up ,0.0,1.0,0.0);
	BiRotFastLtoG(up,up,&trg);
}

void BiVectorToAngleF(BIANGLEF *an,BIPOINT *eye,BIPOINT *up)
{
	BIPOINT tmp;
	BIPOINT2 v2;

	/* Eye vector derives an->h,an->p. */
	BiVectorToHeadPitchF(an,eye);

	/* Up vector derives an->b. */
	BiRotGtoL(&tmp,up,an);
	v2.x= tmp.y;
	v2.y=-tmp.x;
	an->b=BiAngle2F(&v2);
}

void BiVectorToHeadPitch(BIANGLE *an,BIPOINT *eye)
{
	real hor;
	BIPOINT2 v2;

	if(BiAbs(eye->x)<=YSEPS && BiAbs(eye->z)<=YSEPS)
	{
		BiSetAngleDeg(an,0,(eye->y>=0 ? 90 : -90),0);
	}
	else
	{
		v2.x= eye->z;
		v2.y=-eye->x;
		an->h=BiAngle2(&v2);

		if(BiAbs(eye->z)>BiAbs(eye->x))
		{
			hor=eye->z/BiCos(an->h);
		}
		else
		{
			hor=-eye->x/BiSin(an->h);
		}
		v2.x=hor;
		v2.y=eye->y;
		an->p=BiAngle2(&v2);
		an->b=0;
	}
}

void BiVectorToHeadPitchF(BIANGLEF *an,BIPOINT *eye)
{
	real hor;
	BIPOINT2 v2;

	if(BiAbs(eye->x)<=YSEPS && BiAbs(eye->z)<=YSEPS)
	{
		an->h=0.0F;
		an->p=(eye->y>=0 ? 16384.0F : -16384.0F);
		an->b=0.0F;
	}
	else
	{
		v2.x= eye->z;
		v2.y=-eye->x;
		an->h=BiAngle2F(&v2);

		if(BiAbs(eye->z)>BiAbs(eye->x))
		{
			hor=eye->z/BiCos(an->h);
		}
		else
		{
			hor=-eye->x/BiSin(an->h);
		}
		v2.x=hor;
		v2.y=eye->y;
		an->p=BiAngle2F(&v2);
		an->b=0.0F;
	}
}

void BiPitchUp(BIANGLE *dst,BIANGLE *src,long pit,long yaw)
{
	BIPOINT eye,up;
	BIPOINT2 tmp;

	BiSetPoint2(&tmp, 0.0,BiCos(pit));
	BiRot2(&tmp,&tmp,yaw);
	BiSetPoint(&eye,tmp.x,BiSin(pit),tmp.y);

	BiSetPoint2(&tmp,0.0,-BiSin(pit));
	BiRot2(&tmp,&tmp,yaw);
	BiSetPoint(&up,tmp.x,BiCos(pit),tmp.y);

	BiRotLtoG(&eye,&eye,src);
	BiRotLtoG(&up,&up,src);
	BiVectorToAngle(dst,&eye,&up);
}

static void BiInBetweenUnitVector(BIPOINT *dst,BIPOINT *v1,BIPOINT *v2,real t);

void BiInBetweenAngle(BIANGLE *dst,BIANGLE *a1,BIANGLE *a2,real t)
{
	BIPOINT ev1,uv1,ev2,uv2,ev,uv;

	BiAngleToVector(&ev1,&uv1,a1);
	BiAngleToVector(&ev2,&uv2,a2);

	BiInBetweenUnitVector(&ev,&ev1,&ev2,t);
	BiInBetweenUnitVector(&uv,&uv1,&uv2,t);

	BiVectorToAngle(dst,&ev,&uv);
}

static void BiInBetweenUnitVector(BIPOINT *dst,BIPOINT *v1,BIPOINT *v2,real t)
{
	real c;
	long th,al;
	BIPOINT rig;
	real a,b;

	c=BiInnerPoint(v1,v2);
	th=BiAcos(c);
	if(th<1024)
	{
		BiInBetweenPoint(dst,v1,v2,t);
	}
	else if(th>32768-512)
	{
		BiRightAngleVector(&rig,v1);
		if(t<0.5F)
		{
			BiInBetweenUnitVector(dst,v1,&rig,t*2.0F);
		}
		else
		{
			BiInBetweenUnitVector(dst,&rig,v2,(t-0.5F)*2.0F);
		}
		return;
	}
	else
	{
		al=(long)((real)th*t);
		a=BiSin(th-al);
		b=BiSin(al);
		BiInBetweenPoint(dst,v1,v2,b/(a+b));
	}
	BiNormalize(dst,dst);
}

void BiInBetweenPoint(BIPOINT *dst,BIPOINT *p1,BIPOINT *p2,real t)
{
	BIPOINT v;
	BiSubPoint(&v,p2,p1);
	BiMulPoint(&v,&v,t);
	BiAddPoint(dst,p1,&v);
}



/* BILINE *******************************
*****************************************/

void BiLineProjectionVector(BIPOINT *v,BIPOINT *org,BILINE *ln)
{
	real fra,frb;
    BIPOINT va,vp;

	va = ln->v;
	BiSubPoint(&vp,org,&ln->o);

	fra = BiInnerPoint(&va,&vp);
	frb = BiInnerPoint(&va,&va); /* ==|va|^2 */
	va.x = va.x*fra/frb;
	va.y = va.y*fra/frb;
	va.z = va.z*fra/frb;

	BiAddPoint(v,&ln->o,&va);
    BiSubPoint(v,v,org);
}

real BiLineDistance2(BILINE2 *ln,BIPOINT2 *p)
{
	BIPOINT2 pd,vd,qd;
	real t;

	BiSubPoint2(&pd,p,&ln->o);
	BiNormalize2(&vd,&ln->v);

	t=BiInnerPoint2(&pd,&vd);
	BiMulPoint2(&qd,&vd,t);
	BiSubPoint2(&qd,&qd,&pd);

	return BiLengthPoint2(&qd);
}

real BiLineDistance(BILINE *ln,BIPOINT *p)
{
	BIPOINT pd,vd,qd;
	real t;

	BiSubPoint(&pd,p,&ln->o);
	BiNormalize(&vd,&ln->v);

	t=BiInnerPoint(&pd,&vd);
	BiMulPoint(&qd,&vd,t);
	BiSubPoint(&qd,&qd,&pd);

	return BiLengthPoint3(&qd);
}

/* BIPLANE ******************************
*****************************************/

int BiMakePlane(BIPLANE *pln,int nPnt,BIPOINT *pnt)
{
    BIPOINT nom;
	if(BiAverageNormalVector(&nom,nPnt,pnt)==BI_TRUE)
	{
		pln->o=pnt[0];
		pln->n=nom;
		return BI_TRUE;
	}
	else
	{
		return BI_FALSE;
	}
}

void BiProjectToPlane(BIPOINT *pnt,BIPLANE *pln,BIPOINT *org)
{
    BIPOINT nv,pr,bs;
    real fra,frb;

    pr=*org;
    bs= pln->o;
    nv= pln->n;
    fra=BiSqr(nv.x)+BiSqr(nv.y)+BiSqr(nv.z);
    frb=nv.x*(bs.x-pr.x)+nv.y*(bs.y-pr.y)+nv.z*(bs.z-pr.z);
    if(fra==0)
    {
    	return;
    }

    pnt->x = pr.x+nv.x*frb/fra;
    pnt->y = pr.y+nv.y*frb/fra;
    pnt->z = pr.z+nv.z*frb/fra;
}

int BiCrossPoint(BIPOINT *out,BILINE *ln,BIPLANE *pl)
{
	real fa,fb;

	fa=-pl->n.x * (ln->o.x-pl->o.x)
	   -pl->n.y * (ln->o.y-pl->o.y)
	   -pl->n.z * (ln->o.z-pl->o.z);
	fb= pl->n.x *  ln->v.x
	   +pl->n.y *  ln->v.y
	   +pl->n.z *  ln->v.z;

	if(BiAbs(fb)<YSEPS)
	{
		return BI_FALSE;
	}

	out->x = ln->o.x + ln->v.x*fa/fb;
	out->y = ln->o.y + ln->v.y*fa/fb;
	out->z = ln->o.z + ln->v.z*fa/fb;

	return BI_TRUE;
}



/* BIAXIS *******************************
#define BiPntAngToAxis(dst,src)
#define BiConvLtoG(dst,src,axs)
#define BiConvGtoT(dst,src,axs)
*****************************************/
void BiAxisToPlane(BIPLANE *dst,BIAXIS *src)
{
	BIPOINT vec;
	BiSetPoint(&vec,0,1.0,0);
	BiRotFastLtoG(&vec,&vec,&src->t);
	dst->o=src->p;
	dst->n=vec;
}

void BiConvAxisGtoL(BIAXIS *neo,BIAXIS *src,BIAXIS *bas)
{
	BIPOSATT pos;
	BIPOINT ev,uv;

	BiConvGtoL(&pos.p,&src->p,bas);

	BiAngleToVector(&ev,&uv,&src->a);
	BiRotFastGtoL(&ev,&ev,&bas->t);
	BiRotFastGtoL(&uv,&uv,&bas->t);
	BiVectorToAngle(&pos.a,&ev,&uv);

	BiMakeAxis(neo,&pos);
}

void BiConvAxisLtoG(BIAXIS *neo,BIAXIS *src,BIAXIS *bas)
{
	BIPOSATT pos;
	BIPOINT ev,uv;

	BiConvLtoG(&pos.p,&src->p,bas);

	BiAngleToVector(&ev,&uv,&src->a);
	BiRotFastLtoG(&ev,&ev,&bas->t);
	BiRotFastLtoG(&uv,&uv,&bas->t);
	BiVectorToAngle(&pos.a,&ev,&uv);

	BiMakeAxis(neo,&pos);
}

void BiConvAxisGtoLF(BIAXISF *neo,BIAXISF *src,BIAXISF *bas)
{
	BIPOINT p;
	BIANGLEF a;
	BIPOINT ev,uv;

	BiConvGtoL(&p,&src->p,bas);

	BiAngleToVectorF(&ev,&uv,&src->a);
	BiRotFastGtoL(&ev,&ev,&bas->t);
	BiRotFastGtoL(&uv,&uv,&bas->t);
	BiVectorToAngleF(&a,&ev,&uv);

	neo->p=p;
	neo->a=a;
	BiMakeTrigonomy(&neo->t,&a);
}

void BiConvAxisLtoGF(BIAXISF *neo,BIAXISF *src,BIAXISF *bas)
{
	BIPOINT p;
	BIANGLEF a;
	BIPOINT ev,uv;

	BiConvLtoG(&p,&src->p,bas);

	BiAngleToVectorF(&ev,&uv,&src->a);
	BiRotFastLtoG(&ev,&ev,&bas->t);
	BiRotFastLtoG(&uv,&uv,&bas->t);
	BiVectorToAngleF(&a,&ev,&uv);

	neo->p=p;
	neo->a=a;
	BiMakeTrigonomy(&neo->t,&a);
}


/* BIPOINT2 ******************************
#define BiSetPoint2(dst,xx,yy)
#define BiAddPoint2(dst,a,b)
#define BiSubPoint2(dst,a,b)
#define BiMulPoint2(dst,src,m)
#define BiDivPoint2(dst,src,d)
#define BiInnerPoint2(a,b)
#define BiLengthPoint2(p)
*****************************************/
void BiRot2(BIPOINT2 *dst,BIPOINT2 *src,long ang)
{
	real s,c;
	BIPOINT2 tmp;
	s=BiSin(ang);
	c=BiCos(ang);
	tmp.x = c*src->x -s*src->y;
	tmp.y = s*src->x +c*src->y;
	*dst=tmp;
}

void BiRot2F(BIPOINT2 *dst,BIPOINT2 *src,real ang)
{
	real s,c;
	BIPOINT2 tmp;
	s=BiSin(ang);
	c=BiCos(ang);
	tmp.x = c*src->x -s*src->y;
	tmp.y = s*src->x +c*src->y;
	*dst=tmp;
}

long BiAngle2(BIPOINT2 *vec)
{
	long a;
	real ax,ay;

	ax=BiAbs(vec->x);
	ay=BiAbs(vec->y);
	if(ax<YSEPS && ay<YSEPS)
	{
		return 0;
	}
	else if(ax>=ay)
	{
		a=BiAtan(vec->y/ax);
		if(vec->x > 0)
		{
			return a;
		}
		else
		{
			if(vec->y > 0)
			{
				return  0x8000L-a;
			}
			else
			{
				return -0x8000L-a;
			}
		}
	}
	else if(ax<ay)
	{
		a=BiAtan(vec->x/ay);
		if(vec->y > 0)
		{
			return  0x4000L-a;
		}
		else
		{
			return -0x4000L+a;
		}
	}
}

real BiAngle2F(BIPOINT2 *vec)
{
	real a;
	real ax,ay;

	ax=BiAbs(vec->x);
	ay=BiAbs(vec->y);
	if(ax<YSEPS && ay<YSEPS)
	{
		return 0.0F;
	}
	else if(ax>=ay)
	{
		a=BiAtanF(vec->y/ax);
		if(vec->x > 0)
		{
			return a;
		}
		else
		{
			if(vec->y > 0)
			{
				return  32768.0F-a;
			}
			else
			{
				return -32768.0F-a;
			}
		}
	}
	else if(ax<ay)
	{
		a=BiAtanF(vec->x/ay);
		if(vec->y > 0)
		{
			return  16384.0F-a;
		}
		else
		{
			return -16384.0F+a;
		}
	}
}

int BiMakePolyg(BIPOLYGON *plg,int np,BIPOINT p[])
{
	int i;
	plg->np=np;
	plg->p=BiMalloc(np*sizeof(BIPOINT));
	if(plg->p!=NULL)
	{
		for(i=0; i<np; i++)
		{
			plg->p[i]=p[i];
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiMakePolyg2(BIPOLYGON2 *plg,int np,BIPOINT2 *p)
{
	int i;
	plg->np=np;
	plg->p=BiMalloc(np*sizeof(BIPOINT2));
	if(plg->p!=NULL)
	{
		for(i=0; i<np; i++)
		{
			plg->p[i]=p[i];
		}
		return BI_OK;
	}
	return BI_ERR;
}

int BiCopyPolyg(BIPOLYGON *neo,BIPOLYGON *org)
{
	return BiMakePolyg(neo,org->np,org->p);
}

int BiCopyPolyg2(BIPOLYGON2 *neo,BIPOLYGON2 *org)
{
	return BiMakePolyg2(neo,org->np,org->p);
}

void BiFreePolyg(BIPOLYGON *plg)
{
	BiFree(plg->p);
}

void BiFreePolyg2(BIPOLYGON2 *plg)
{
	BiFree(plg->p);
}


/* */
int BiQuickCheckConvex(BIPOLYGON *plg)
{
	int i,np;
	BIVECTOR *p,*p1,*p2,v1,v2,o1,o2;

	np=plg->np;
	p=plg->p;
	while(np>3 && BiSamePoint(&p[0],&p[1])==BI_TRUE)
	{
		p++;
		np--;
	}
	while(np>3 && BiSamePoint(&p[np-1],&p[np-2])==BI_TRUE)
	{
		np--;
	}
	if(np>3 && BiSamePoint(&p[np-1],&p[0])==BI_TRUE)
	{
		np--;
	}

	if(np<=3)
	{
		return BI_TRUE;
	}

 	p1=&p[np-2];
	p2=&p[np-1];
	BiSubPoint(&v2,p2,p1);
	for(i=0; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(BiSamePoint(p1,p2)!=BI_TRUE)
		{
			v1=v2;
			BiSubPoint(&v2,p2,p1);

			BiOuterProduct(&o1,&v1,&v2);
			if(o1.x*o1.x+o1.y*o1.y+o1.z*o1.z>YSEPS)
			{
				break;
			}
		}
	}

	for(i=i+1; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(BiSamePoint(p1,p2)!=BI_TRUE)
		{
			v1=v2;
			BiSubPoint(&v2,p2,p1);

			BiOuterProduct(&o2,&v1,&v2);
			if(o2.x*o2.x+o2.y*o2.y+o2.z*o2.z>YSEPS &&
			   BiInnerPoint(&o1,&o2)<0)
			{
				return BI_FALSE;
			}
		}
	}

	return BI_TRUE;
}

int BiQuickCheckConvex2(BIPOLYGON2 *plg)
{
	int i,np;
	BIPOINT2 *p,*p1,*p2,v1,v2;
	real o1,o2;

	np=plg->np;
	p=plg->p;
	while(np>3 && BiSamePoint2(&p[0],&p[1])==BI_TRUE)
	{
		p++;
		np--;
	}
	while(np>3 && BiSamePoint2(&p[np-1],&p[np-2])==BI_TRUE)
	{
		np--;
	}
	if(np>3 && BiSamePoint2(&p[np-1],&p[0])==BI_TRUE)
	{
		np--;
	}

	if(np<=3)
	{
		return BI_TRUE;
	}

 	p1=&p[np-2];
	p2=&p[np-1];
	BiSubPoint2(&v2,p2,p1);
	for(i=0; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(BiSamePoint2(p1,p2)!=BI_TRUE)
		{
			v1=v2;
			BiSubPoint2(&v2,p2,p1);

			o1=v1.x*v2.y-v1.y*v2.x;
			if(BiAbs(o1)>YSEPS)
			{
				break;
			}
		}
	}

	for(i=i+1; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(BiSamePoint2(p1,p2)!=BI_TRUE)
		{
			v1=v2;
			BiSubPoint2(&v2,p2,p1);

			o2=v1.x*v2.y-v1.y*v2.x;
			if(BiAbs(o2)>YSEPS && o1*o2<0)
			{
				return BI_FALSE;
			}
		}
	}

	return BI_TRUE;
}
