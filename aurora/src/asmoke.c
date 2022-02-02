#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "aurora.h"


/***************************************************************************

  When add a smoke type.....
    1)Create
        ArIns?????Smoke
    2)Add code in
		ArInitSmokeClass
		ArSetSmokeLife
        ArSetSmokeWidth
        ArSetSmokeColor
        ArInsSmoke

***************************************************************************/
static void ArInitSmokeAttribute(ARSMOKEATTR *at,real t0,real t1,real wid,BICOLOR *c)
{
	at->t0=t0;
	at->t1=t1;
	at->iniw=wid;
	at->maxw=wid;
	at->dw=0.0F;
	at->inic=*c;
	at->endc=*c;
	at->tc=0.0F;
}

int ArInitSmokeClass(ARSMOKECLASS *s)
{
	int i;
	for(i=0; i<3; i++)
	{
		s->stp[i]=1;
		s->bbx[i]=BiOrgP;
	}
	s->sw=0;
	ArInitSmokeAttribute(&s->sld,0.0F,1.0F,1.0F,&BiWhite);
	ArInitSmokeAttribute(&s->wir,0.0F,1.0F,1.0F,&BiWhite);
	ArInitSmokeAttribute(&s->trl,0.0F,1.0F,1.0F,&BiWhite);
	ArInitSmokeAttribute(&s->rbn,0.0F,1.0F,1.0F,&BiWhite);
	ArInitSmokeAttribute(&s->net,0.0F,1.0F,1.0F,&BiWhite);
	return BI_OK;
}

int ArSetSmokeBoundingBox(ARSMOKECLASS *s,int stp[3],BIPOINT bbx[3])
{
	int i;
	for(i=0; i<3; i++)
	{
		s->stp[i]=BiLarger(1,stp[i]);
		s->bbx[i]=bbx[i];
	}
	return BI_OK;
}

/***************************************************************************/
static void ArInitSmokeWidth(ARSMOKEATTR *at,real iniw,real maxw,real dw)
{
	at->iniw=iniw;
	at->maxw=maxw;
	at->dw=dw;
}

static void ArInitSmokeColor(ARSMOKEATTR *at,BICOLOR *inic,BICOLOR *endc,real tc)
{
	at->inic=*inic;
	at->endc=*endc;
	at->tc=tc;
}

int ArSetSmokeType(ARSMOKECLASS *s,int attr)
{
	s->sw=attr;
	return BI_OK;
}

int ArGetSmokeType(int *attr,ARSMOKECLASS *s)
{
	*attr=s->sw;
	return BI_OK;
}

int ArSetSmokeLife(ARSMOKECLASS *s,int type,real t0,real t1)
{
	if(type&ARS_RIBBONSMOKE)
	{
		s->rbn.t0=t0;
		s->rbn.t1=t1;
	}
	if(type&ARS_WIRESMOKE)
	{
		s->wir.t0=t0;
		s->wir.t1=t1;
	}
	if(type&ARS_TRAILSMOKE)
	{
		s->trl.t0=t0;
		s->trl.t1=t1;
	}
	if(type&ARS_SOLIDSMOKE)
	{
		s->sld.t0=t0;
		s->sld.t1=t1;
	}
	if(type&ARS_NETSMOKE)
	{
		s->net.t0=t0;
		s->net.t1=t1;
	}
	return BI_OK;
}

int ArSetSmokeWidth(ARSMOKECLASS *s,int type,real w0,real w1,real dw)
{
	if(type&ARS_RIBBONSMOKE)
	{
		ArInitSmokeWidth(&s->rbn,w0,w1,dw);
	}
	if(type&ARS_WIRESMOKE)
	{
		ArInitSmokeWidth(&s->wir,w0,w1,dw);
	}
	if(type&ARS_TRAILSMOKE)
	{
		ArInitSmokeWidth(&s->trl,w0,w1,dw);
	}
	if(type&ARS_SOLIDSMOKE)
	{
		ArInitSmokeWidth(&s->sld,w0,w1,dw);
	}
	if(type&ARS_NETSMOKE)
	{
		ArInitSmokeWidth(&s->net,w0,w1,dw);
	}
	return BI_OK;
}

int ArSetSmokeColor(ARSMOKECLASS *s,int type,BICOLOR *c1,BICOLOR *c2,real t)
{
	if(type&ARS_RIBBONSMOKE)
	{
		ArInitSmokeColor(&s->rbn,c1,c2,t);
	}
	if(type&ARS_WIRESMOKE)
	{
		ArInitSmokeColor(&s->wir,c1,c2,t);
	}
	if(type&ARS_TRAILSMOKE)
	{
		ArInitSmokeColor(&s->trl,c1,c2,t);
	}
	if(type&ARS_SOLIDSMOKE)
	{
		ArInitSmokeColor(&s->sld,c1,c2,t);
	}
	if(type&ARS_NETSMOKE)
	{
		ArInitSmokeColor(&s->net,c1,c2,t);
	}
	return BI_OK;
}

/***************************************************************************/

int ArInitSmokeInstance(ARSMOKEINST *i,int nNode,int nDelWhenOverflow)
{
	ARSMOKENODE *coffee;
	coffee=(ARSMOKENODE *)BiMalloc(sizeof(ARSMOKENODE)*nNode);
	if(coffee!=NULL || nNode<=0)
	{
		i->nMax=nNode;
		i->nDel=nDelWhenOverflow;
		i->nPth=0;
		i->pth=coffee;
		i->nTip=0;
		return BI_OK;
	}
	i->pth=NULL;
	return BI_ERR;
}

int ArBeginAppendSmokeNode(ARSMOKEINST *inst)
{
	if(inst->nTip>=ARS_MAX_TIP_PER_INST)
	{
		int i;
		for(i=0; i<ARS_MAX_TIP_PER_INST-1; i++)
		{
			inst->tip[i*2  ]=inst->tip[i*2+2];
			inst->tip[i*2+1]=inst->tip[i*2+3];
		}
		inst->nTip=ARS_MAX_TIP_PER_INST-1;
	}
	inst->tip[inst->nTip*2  ]=inst->nPth;
	inst->tip[inst->nTip*2+1]=inst->nPth;
	inst->nTip++;
	return BI_OK;
}

int ArAppendSmokeNode(ARSMOKEINST *inst,BIPOSATT *pos,real t)
{
	int i;
	ARSMOKENODE *pth;
	if(inst->nTip>0)
	{
		if(inst->nPth>=inst->nMax)
		{
			if(inst->nDel>0)
			{
				for(i=0; i<(inst->nMax)-(inst->nDel); i++)
				{
					inst->pth[i]=inst->pth[i+inst->nDel];
				}
				for(i=0; i<ARS_MAX_TIP_PER_INST*2; i++)
				{
					inst->tip[i]=BiLarger(0,inst->tip[i]-inst->nDel);
				}
				inst->nPth=(inst->nMax)-(inst->nDel);
			}
			else
			{
				/* Overflow */
				return BI_ERR;
			}
		}

		pth=&inst->pth[inst->nPth];
		BiMakeAxis(&pth->axs,pos);
		BiRotFastLtoG(&pth->left,&BiVecX,&pth->axs.t);
		BiRotFastLtoG(&pth->up,&BiVecY,&pth->axs.t);
		pth->t=t;

		inst->tip[inst->nTip*2-1]=inst->nPth;

		inst->nPth++;

		return BI_OK;
	}
	return BI_ERR;
}

int ArEndAppendSmokeNode(ARSMOKEINST *inst)
{
	if(inst->nTip>0)
	{
		inst->tip[inst->nTip*2-1]=inst->nPth-1;
		return BI_OK;
	}
	return BI_ERR;
}

int ArClearSmokeInstance(ARSMOKEINST *inst)
{
	inst->nPth=0;
	inst->nTip=0;
	return BI_OK;
}

int ArFreeSmokeInstance(ARSMOKEINST *inst)
{
	BiFree(inst->pth);
	return BI_OK;
}

/***************************************************************************/
static int ArInsSmokeStep(BIPOINT *ndp,BIPOINT *eye,ARSMOKECLASS *cla)
{
	int i;
	BIPOINT dif;

	BiSubPoint(&dif,eye,ndp);
	for(i=0; i<2; i++)
	{
		dif.x=BiAbs(dif.x);
		dif.y=BiAbs(dif.y);
		dif.z=BiAbs(dif.z);
		if(dif.x<cla->bbx[i].x && dif.y<cla->bbx[i].y && dif.z<cla->bbx[i].z)
		{
			break;
		}
	}
	return cla->stp[i];
}

static void ArGetCurrentSmokeColor(BICOLOR *c,ARSMOKEATTR *att,real rt)
{
	if(rt<att->tc)
	{
		c->r=att->inic.r + (int)((real)(att->endc.r-att->inic.r)*rt/att->tc);
		c->g=att->inic.g + (int)((real)(att->endc.g-att->inic.g)*rt/att->tc);
		c->b=att->inic.b + (int)((real)(att->endc.b-att->inic.b)*rt/att->tc);
	}
	else
	{
		*c=att->endc;
	}
}

static void ArInsRibbonSmoke(ARSMOKEATTR *att,ARSMOKENODE *node,int n0,int n1,real t)
{
	real rt0,rt1,w0,w1;
	BICOLOR c;
	BIPOINT v1,v2,sq[4],tri[3];

	rt0=t-node[n0].t;
	rt1=t-node[n1].t;

	if(att->t0<=rt0 && rt1<=att->t1)
	{
		ArGetCurrentSmokeColor(&c,att,rt0);
		w0=BiSmaller(att->iniw+att->dw*rt0,att->maxw);
		w1=BiSmaller(att->iniw+att->dw*rt1,att->maxw);

		BiMulPoint(&v1,&node[n0].left,w0/2);
		BiMulPoint(&v2,&node[n1].left,w1/2);

		BiAddPoint(&sq[0],&node[n0].axs.p,&v1);
		BiSubPoint(&sq[1],&node[n0].axs.p,&v1);
		BiAddPoint(&sq[2],&node[n1].axs.p,&v2);
		BiSubPoint(&sq[3],&node[n1].axs.p,&v2);

		tri[0]=sq[0];
		tri[1]=sq[1];
		tri[2]=sq[2];
		BiInsPolyg(3,tri,&c);
		tri[0]=sq[2];
		tri[1]=sq[1];
		tri[2]=sq[3];
		BiInsPolyg(3,tri,&c);
	}
}

static void ArInsWireSmoke(ARSMOKEATTR *att,ARSMOKENODE *node,int n0,int n1,real t)
{
	real rt0,rt1;
	BICOLOR c;

	rt0=t-node[n0].t;
	rt1=t-node[n1].t;

	if(att->t0<=rt0 && rt1<=att->t1)
	{
		ArGetCurrentSmokeColor(&c,att,rt0);
		BiInsLine(&node[n0].axs.p,&node[n1].axs.p,&c);
	}
}

static void ArInsTrailSmoke(ARSMOKEATTR *att,ARSMOKENODE *node,int n0,int n1,real t)
{
	real rt0,rt1,w0,w1;
	BICOLOR c;
	BIPOINT v1,v2,sq[4];

	rt0=t-node[n0].t;
	rt1=t-node[n1].t;

	if(att->t0<=rt0 && rt1<=att->t1)
	{
		ArGetCurrentSmokeColor(&c,att,rt0);
		w0=BiSmaller(att->iniw+att->dw*rt0,att->maxw);
		w1=BiSmaller(att->iniw+att->dw*rt1,att->maxw);

		BiMulPoint(&v1,&node[n0].left,w0/2);
		BiMulPoint(&v2,&node[n1].left,w1/2);

		BiAddPoint(&sq[0],&node[n0].axs.p,&v1);
		BiSubPoint(&sq[1],&node[n0].axs.p,&v1);
		BiAddPoint(&sq[2],&node[n1].axs.p,&v2);
		BiSubPoint(&sq[3],&node[n1].axs.p,&v2);

		BiInsLine(&sq[0],&sq[2],&c);
		BiInsLine(&sq[1],&sq[3],&c);
	}
}

static void ArInsSolidSmokePolyg(BIPOINT *vtx,BIPOINT *nom,int i1,int i2,int i3,int i4,BICOLOR *c)
{
	BIPOINT tri[3],tnm[3];
	tri[0]=vtx[i1];
	tri[1]=vtx[i2];
	tri[2]=vtx[i3];
	tnm[0]=nom[i1];
	tnm[1]=nom[i2];
	tnm[2]=nom[i3];
	BiInsRoundPolyg(3,tri,tnm,c);
	tri[0]=vtx[i1];
	tri[1]=vtx[i3];
	tri[2]=vtx[i4];
	tnm[0]=nom[i1];
	tnm[1]=nom[i3];
	tnm[2]=nom[i4];
	BiInsRoundPolyg(3,tri,tnm,c);
}

static void ArInsSolidSmoke(ARSMOKEATTR *att,ARSMOKENODE *node,int n0,int n1,real t)
{
	real rt0,rt1,w0,w1;
	BICOLOR c;
	BIPOINT lv1,lv2,uv1,uv2,vtx[8],nom[8];

	rt0=t-node[n0].t;
	rt1=t-node[n1].t;

	if(att->t0<=rt0 && rt1<=att->t1)
	{
		ArGetCurrentSmokeColor(&c,att,rt0);
		w0=BiSmaller(att->iniw+att->dw*rt0,att->maxw);
		w1=BiSmaller(att->iniw+att->dw*rt1,att->maxw);

		BiMulPoint(&lv1,&node[n0].left,w0/2);
		BiMulPoint(&lv2,&node[n1].left,w1/2);
		BiMulPoint(&uv1,&node[n0].up  ,w0/2);
		BiMulPoint(&uv2,&node[n1].up  ,w1/2);

		BiAddPoint(&vtx[0],&node[n0].axs.p,&lv1);
		BiSubPoint(&vtx[1],&node[n0].axs.p,&lv1);
		BiAddPoint(&vtx[2],&node[n1].axs.p,&lv2);
		BiSubPoint(&vtx[3],&node[n1].axs.p,&lv2);
		BiAddPoint(&vtx[4],&node[n0].axs.p,&uv1);
		BiSubPoint(&vtx[5],&node[n0].axs.p,&uv1);
		BiAddPoint(&vtx[6],&node[n1].axs.p,&uv2);
		BiSubPoint(&vtx[7],&node[n1].axs.p,&uv2);

		nom[0]=node[n0].left;
		BiSubPoint(&nom[1],&BiOrgP,&node[n0].left);
		nom[2]=node[n1].left;
		BiSubPoint(&nom[3],&BiOrgP,&node[n1].left);
		nom[4]=node[n0].up;
		BiSubPoint(&nom[5],&BiOrgP,&node[n0].up);
		nom[6]=node[n1].up;
		BiSubPoint(&nom[7],&BiOrgP,&node[n1].up);

		ArInsSolidSmokePolyg(vtx,nom,0,2,6,4,&c);
		ArInsSolidSmokePolyg(vtx,nom,4,6,3,1,&c);
		ArInsSolidSmokePolyg(vtx,nom,1,3,7,5,&c);
		ArInsSolidSmokePolyg(vtx,nom,5,7,2,0,&c);
	}
}

static void ArInsNetSmoke(ARSMOKEATTR *att,ARSMOKENODE *node,int n0,int n1,real t)
{
	real rt0,rt1,w0,w1;
	BICOLOR c;
	BIPOINT lv1,lv2,uv1,uv2,lu1,lu2,ld1,ld2,vtx[16];

	rt0=t-node[n0].t;
	rt1=t-node[n1].t;

	if(att->t0<=rt0 && rt1<=att->t1)
	{
		ArGetCurrentSmokeColor(&c,att,rt0);
		w0=BiSmaller(att->iniw+att->dw*rt0,att->maxw);
		w1=BiSmaller(att->iniw+att->dw*rt1,att->maxw);

		BiMulPoint(&lv1,&node[n0].left,w0/2);
		BiMulPoint(&lv2,&node[n1].left,w1/2);
		BiMulPoint(&uv1,&node[n0].up  ,w0/2);
		BiMulPoint(&uv2,&node[n1].up  ,w1/2);

		BiAddPoint(&lu1,&lv1,&uv1);
		BiMulPoint(&lu1,&lu1,0.8);
		BiAddPoint(&lu2,&lv2,&uv2);
		BiMulPoint(&lu2,&lu2,0.8);
		BiSubPoint(&ld1,&lv1,&uv1);
		BiMulPoint(&ld1,&ld1,0.8);
		BiSubPoint(&ld2,&lv2,&uv2);
		BiMulPoint(&ld2,&ld2,0.8);

		BiAddPoint(&vtx[ 0],&node[n0].axs.p,&uv1);
		BiAddPoint(&vtx[ 1],&node[n0].axs.p,&lu1);
		BiAddPoint(&vtx[ 2],&node[n0].axs.p,&lv1);
		BiAddPoint(&vtx[ 3],&node[n0].axs.p,&ld1);
		BiSubPoint(&vtx[ 4],&node[n0].axs.p,&uv1);
		BiSubPoint(&vtx[ 5],&node[n0].axs.p,&lu1);
		BiSubPoint(&vtx[ 6],&node[n0].axs.p,&lv1);
		BiSubPoint(&vtx[ 7],&node[n0].axs.p,&ld1);

		BiAddPoint(&vtx[ 8],&node[n1].axs.p,&uv2);
		BiAddPoint(&vtx[ 9],&node[n1].axs.p,&lu2);
		BiAddPoint(&vtx[10],&node[n1].axs.p,&lv2);
		BiAddPoint(&vtx[11],&node[n1].axs.p,&ld2);
		BiSubPoint(&vtx[12],&node[n1].axs.p,&uv2);
		BiSubPoint(&vtx[13],&node[n1].axs.p,&lu2);
		BiSubPoint(&vtx[14],&node[n1].axs.p,&lv2);
		BiSubPoint(&vtx[15],&node[n1].axs.p,&ld2);

		BiInsLine(&vtx[0],&vtx[ 9],&c);
		BiInsLine(&vtx[1],&vtx[10],&c);
		BiInsLine(&vtx[2],&vtx[11],&c);
		BiInsLine(&vtx[3],&vtx[12],&c);
		BiInsLine(&vtx[4],&vtx[13],&c);
		BiInsLine(&vtx[5],&vtx[14],&c);
		BiInsLine(&vtx[6],&vtx[15],&c);
		BiInsLine(&vtx[7],&vtx[ 8],&c);

		BiInsLine(&vtx[0],&vtx[ 1],&c);
		BiInsLine(&vtx[1],&vtx[ 2],&c);
		BiInsLine(&vtx[2],&vtx[ 3],&c);
		BiInsLine(&vtx[3],&vtx[ 4],&c);
		BiInsLine(&vtx[4],&vtx[ 5],&c);
		BiInsLine(&vtx[5],&vtx[ 6],&c);
		BiInsLine(&vtx[6],&vtx[ 7],&c);
		BiInsLine(&vtx[7],&vtx[ 0],&c);
	}
}

static int ArInsSmokeTips(ARSMOKECLASS *cla,ARSMOKEINST *inst,int nEnd,int nSta,real ctim,BIPOSATT *eye)
{
	int n,m,stp;

	/* Experimental : Anti Smoke Trembling. Ribbon,Solid Smoke Only */
	if(cla->sw & ARS_RIBBONSMOKE)
	{
		m=nEnd;
		while(m<nSta)
		{
			stp=ArInsSmokeStep(&inst->pth[m].axs.p,&eye->p,cla);
			n=BiSmaller(nSta,m+stp);
			while(cla->rbn.t0>ctim-inst->pth[n].t)
			{
				n--;
			}
			if(m<n)
			{
				ArInsRibbonSmoke(&cla->rbn,inst->pth,n,m,ctim);
			}
			m+=stp;
		}
	}

	if(cla->sw & ARS_SOLIDSMOKE)
	{
		m=nEnd;
		while(m<nSta)
		{
			stp=ArInsSmokeStep(&inst->pth[m].axs.p,&eye->p,cla);
			n=BiSmaller(nSta,m+stp);
			while(cla->sld.t0>ctim-inst->pth[n].t)
			{
				n--;
			}
			if(m<n)
			{
				ArInsSolidSmoke(&cla->sld,inst->pth,n,m,ctim);
			}
			m+=stp;
		}

/*		n=nSta;
		while(cla->sld.t0>ctim-inst->pth[n].t)
		{
			n--;
		}
		while(n>nEnd)
		{
			stp=ArInsSmokeStep(&inst->pth[n].axs.p,&eye->p,cla);
			m=BiLarger(nEnd,n-stp);
			ArInsSolidSmoke(&cla->sld,inst->pth,n,m,ctim);
			n-=stp;
		} */
	}



	n=nSta;
	if(cla->sw & ARS_WIRESMOKE)
	{
		while(cla->wir.t0>ctim-inst->pth[n].t)
		{
			n--;
		}
		while(n>nEnd)
		{
			stp=ArInsSmokeStep(&inst->pth[n].axs.p,&eye->p,cla);
			m=BiLarger(nEnd,n-stp);
			ArInsWireSmoke(&cla->wir,inst->pth,n,m,ctim);
			n-=stp;
		}
	}

	n=nSta;
	if(cla->sw & ARS_TRAILSMOKE)
	{
		while(cla->trl.t0>ctim-inst->pth[n].t)
		{
			n--;
		}
		while(n>nEnd)
		{
			stp=ArInsSmokeStep(&inst->pth[n].axs.p,&eye->p,cla);
			m=BiLarger(nEnd,n-stp);
			ArInsTrailSmoke(&cla->trl,inst->pth,n,m,ctim);
			n-=stp;
		}
	}

	n=nSta;
	if(cla->sw & ARS_NETSMOKE)
	{
		while(cla->net.t0>ctim-inst->pth[n].t)
		{
			n--;
		}
		while(n>nEnd)
		{
			stp=ArInsSmokeStep(&inst->pth[n].axs.p,&eye->p,cla);
			m=BiLarger(nEnd,n-stp);
			ArInsNetSmoke(&cla->net,inst->pth,n,m,ctim);
			n-=stp;
		}
	}

	return BI_OK;
}


int ArInsSmoke(ARSMOKECLASS *cla,ARSMOKEINST *inst,real ctim,BIPOSATT *eye)
{
	int i;
	for(i=0; i<inst->nTip; i++)
	{
		ArInsSmokeTips(cla,inst,inst->tip[i*2],inst->tip[i*2+1],ctim,eye);
	}
	return BI_OK;
}
