#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>


#include "aurora.h"

/***********************************************************************/
static ARDYNANODE *ArNewDnmNode(BIPOSATT *cnt,BIPOINT *cld);
static int ArAppendDnmNode(int *id,ARDNM *mdl,int pid,ARDYNANODE *neo);
static void ArRemakeObjectList(ARDYNANODE *(*obj),ARDYNANODE *ptr);
static ARDYNANODE *ArAppendDnmNodeFirst(int *id,ARDNM *mdl,int pid,ARDYNANODE *neo);
static ARDYNANODE *ArAppendDnmNodeSecond(int *id,ARDNM *mdl,int pid,ARDYNANODE *neo);

int ArInitDnm(ARDNM *neo)
{
	neo->RootId=-1;   /* Default : Not a Aurora Share Target */
	neo->nObj=0;
	neo->obj=NULL;
	neo->top=NULL;
	return BI_OK;
}

static ARDYNANODE *ArNewDnmNode(BIPOSATT *par,BIPOINT *cld)
{
	ARDYNANODE *neo;
	neo=(ARDYNANODE *)BiMalloc(sizeof(ARDYNANODE));
	if(neo!=NULL)
	{
		neo->id=0;       /* Temporary */
		neo->classId=0;
		neo->next=NULL;
		neo->par=NULL;   /* Temporary */

		neo->cntPar=(par!=NULL ? *par : BiOrgPA);
		neo->cntCld=(cld!=NULL ? *cld : BiOrgP);

		BiAngleToVector(&neo->cev,&neo->cuv,&neo->cntPar.a);

		neo->rel=ARM_DEPENDENT; /* Temporary */
		neo->mtn=BiOrgPA;

		neo->cld=NULL;

		neo->disp=BI_ON;
		neo->type=ARM_NULL;

		neo->nSta=0;
		neo->sta[0].disp=BI_ON;
		neo->sta[0].pos=BiOrgPA;
	}
	return neo;
}


static int ArAppendDnmNode(int *id,ARDNM *mdl,int pid,ARDYNANODE *neo)
{
	int i;
	ARDYNANODE *(*lst);

	lst=(ARDYNANODE *(*))BiMalloc((mdl->nObj+1)*sizeof(ARDYNANODE *));
	if((mdl->nObj==0 && pid!=0) || (pid<0) || (mdl->nObj!=0 && mdl->nObj<=pid) || lst==NULL)
	{
		return BI_ERR;
	}

	if(mdl->nObj==0)
	{
		mdl->top=ArAppendDnmNodeFirst(id,mdl,pid,neo);
	}
	else
	{
		mdl->top=ArAppendDnmNodeSecond(id,mdl,pid,neo);
	}

	for(i=0; i<mdl->nObj; i++)
	{
		lst[i]=NULL;
	}

	BiFree(mdl->obj);
	mdl->obj=lst;
	ArRemakeObjectList(mdl->obj,mdl->top);

	return BI_OK;
}

static void ArRemakeObjectList(ARDYNANODE *(*obj),ARDYNANODE *ptr)
{
	if(ptr!=NULL)
	{
		obj[ptr->id]=ptr;
		ArRemakeObjectList(obj,ptr->next);
		ArRemakeObjectList(obj,ptr->cld);
	}
}

static ARDYNANODE *ArAppendDnmNodeFirst(int *id,ARDNM *mdl,int pid,ARDYNANODE *neo)
{
	mdl->nObj=1;

	neo->id=0;
	neo->next=NULL;
	neo->par=NULL;
	/* neo->cntCld=BiOrgP; */
	BiAngleToVector(&neo->cev,&neo->cuv,&neo->cntPar.a);
	neo->mtn=BiOrgPA;
	neo->cld=NULL;

	neo->abs=BiOrgPA;

	if(id!=NULL)
	{
		*id=neo->id;
	}

	return neo;
}

static ARDYNANODE *ArAppendDnmNodeSecond(int *id,ARDNM *mdl,int pid,ARDYNANODE *neo)
{
	int neoId;
	ARDYNANODE *btm;

	neoId=mdl->nObj;
	mdl->nObj++;

	neo->id=neoId;
	neo->next=NULL;
	neo->par=mdl->obj[pid];
	neo->mtn=BiOrgPA;
	neo->cld=NULL;
	neo->disp=BI_ON;

	if(neo->par->cld==NULL)
	{
		neo->par->cld=neo;
	}
	else
	{
		btm=neo->par->cld;
		while(btm->next!=NULL)
		{
			btm=btm->next;
		}
		btm->next=neo;
	}

	if(id!=NULL)
	{
		(*id)=neoId;
	}
	return mdl->top;
}

int ArAddDnmSrf(int *id,ARDNM *mdl,BISRFMODEL *srf,int pid,BIPOSATT *par,BIPOINT *cld)
{
	ARDYNANODE *neo;
	neo=ArNewDnmNode(par,cld);
	neo->type=ARM_SRFMODEL;
	neo->shape.srf=*srf;
	return ArAppendDnmNode(id,mdl,pid,neo);
}

int ArAddDnmPolyg(int *id,ARDNM *mdl,BIPOLYGON *plg,BICOLOR *col,int pid,BIPOSATT *par,BIPOINT *cld)
{
	ARDYNANODE *neo;
	neo=ArNewDnmNode(par,cld);
	neo->type=ARM_POLYGON;
	neo->shape.plg=*plg;
	neo->col=*col;
	return ArAppendDnmNode(id,mdl,pid,neo);
}

int ArAddDnmLine(int *id,ARDNM *mdl,BILIMLINE *lin,BICOLOR *col,int pid,BIPOSATT *par,BIPOINT *cld)
{
	ARDYNANODE *neo;
	neo=ArNewDnmNode(par,cld);
	neo->type=ARM_LINE;
	neo->shape.lin=*lin;
	neo->col=*col;
	return ArAppendDnmNode(id,mdl,pid,neo);
}

int ArSetDnmRelation(ARDNM *ptr,int id,int rel)
{
	ARDYNANODE *nd;

	if(id<ptr->nObj)
	{
		nd=ptr->obj[id];
		nd->rel=rel;
		return BI_OK;
	}
	return BI_ERR;
}


/***********************************************************************/
int ArCopyDnm(ARDNM *neo,ARDNM *org)
{
	int i,j,id,parId,err;
	ARDYNANODE *nd,*par;

	ArInitDnm(neo);
	for(i=0; i<org->nObj; i++)
	{
		nd=org->obj[i];
		par=nd->par;
		parId=(par!=NULL ? par->id : 0);
		switch(nd->type)
		{
		case ARM_SRFMODEL:
			err=ArAddDnmSrf(&id,neo,&nd->shape.srf,parId,&nd->cntPar,&nd->cntCld);
			break;
		case ARM_POLYGON:
			err=ArAddDnmPolyg(&id,neo,&nd->shape.plg,&nd->col,parId,&nd->cntPar,&nd->cntCld);
			break;
		case ARM_LINE:
			err=ArAddDnmLine(&id,neo,&nd->shape.lin,&nd->col,parId,&nd->cntPar,&nd->cntCld);
			break;
		case ARM_NULL:
			/* (^_^;) */
			break;
		}

		if(err!=BI_OK)
		{
			return err;
		}

		neo->obj[id]->mtn=nd->mtn;
		neo->obj[id]->rel=nd->rel;
		neo->obj[id]->disp=nd->disp;
		neo->obj[id]->classId=nd->classId;

		for(j=0; j<nd->nSta; j++)
		{
			ArAddDnmStatus(NULL,neo,id,nd->sta[j].disp,&nd->sta[j].pos);
		}
	}
	neo->RootId=org->RootId;
	ArIncrementDnmReferenceCount(org);
	return BI_OK;
}



/***********************************************************************/
int ArGetDnmNumObj(int *nObj,ARDNM *mdl)
{
	*nObj=mdl->nObj;
	return BI_OK;
}

int ArSetDnmRelativePosition(ARDNM *mdl,int id,BIPOSATT *pos)
{
	if(id<mdl->nObj)
	{
		mdl->obj[id]->mtn=*pos;
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetDnmRelativePosition(BIPOSATT *pos,ARDNM *mdl,int id)
{
	if(id<mdl->nObj)
	{
		*pos=mdl->obj[id]->mtn;
		return BI_OK;
	}
	return BI_ERR;
}

int ArSetDnmRelativeAttitude(ARDNM *mdl,int id,BIANGLE *att)
{
	if(id<mdl->nObj)
	{
		mdl->obj[id]->mtn.a=*att;
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetDnmRelativeAttitude(BIANGLE *att,ARDNM *mdl,int id)
{
	if(id<mdl->nObj)
	{
		*att=mdl->obj[id]->mtn.a;
		return BI_OK;
	}
	return BI_ERR;
}

int ArSetDnmDrawSwitch(ARDNM *mdl,int id,int disp)
{
	if(id<mdl->nObj)
	{
		mdl->obj[id]->disp=disp;
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetDnmDrawSwitch(int *disp,ARDNM *mdl,int id)
{
	if(id<mdl->nObj)
	{
		*disp=mdl->obj[id]->disp;
		return BI_OK;
	}
	return BI_ERR;
}

int ArSetDnmClassId(ARDNM *mdl,int id,int classId)
{
	if(id<mdl->nObj)
	{
		mdl->obj[id]->classId=classId;
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetDnmClassId(int *classId,ARDNM *mdl,int id)
{
	if(id<mdl->nObj)
	{
		*classId=mdl->obj[id]->classId;
		return BI_OK;
	}
	return BI_ERR;
}



/***********************************************************************/

int ArSetDnmConnectingPosition(ARDNM *mdl,int id,BIPOSATT *pos)
{
	if(id<mdl->nObj)
	{
		mdl->obj[id]->cntPar=*pos;
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetDnmConnectingePosition(BIPOSATT *pos,ARDNM *mdl,int id)
{
	if(id<mdl->nObj)
	{
		*pos=mdl->obj[id]->cntPar;
		return BI_OK;
	}
	return BI_ERR;
}


/***********************************************************************/
static int ArCalcNodeAbsPos(ARDYNANODE *ptr,BIAXIS *par,BIAXIS *top);

int ArCalcDnmAbsPosition(ARDNM *neo,BIPOSATT *pos)
{
	BIAXIS axs;
	BiMakeAxis(&axs,pos);
	return ArCalcNodeAbsPos(neo->top,&axs,&axs);
}

static int ArCalcNodeAbsPos(ARDYNANODE *ptr,BIAXIS *par,BIAXIS *top)
{
	BIAXIS aAxs,cAxs;
	BIPOINT pnt,ev,uv,cntCld;
	BIPOSATT aPos;

	if(ptr!=NULL)
	{
		BiMakeAxis(&cAxs,&ptr->cntPar);
		pnt=ptr->mtn.p;
		BiAngleToVector(&ev,&uv,&ptr->mtn.a);

		switch(ptr->rel)
		{
		case ARM_DEPENDENT:
			BiConvLtoG(&pnt,&pnt,&cAxs);
			BiRotFastLtoG(&ev,&ev,&cAxs.t);
			BiRotFastLtoG(&uv,&uv,&cAxs.t);

			BiConvLtoG(&aPos.p,&pnt,par);
			BiRotFastLtoG(&ev,&ev,&par->t);
			BiRotFastLtoG(&uv,&uv,&par->t);
			BiVectorToAngle(&aPos.a,&ev,&uv);

			BiRotLtoG(&cntCld,&ptr->cntCld,&aPos.a);
			BiSubPoint(&aPos.p,&aPos.p,&cntCld);

			BiMakeAxis(&aAxs,&aPos);
			break;
		case ARM_TOPPARENT:
			aPos.p=top->p;
			aPos.a=top->a;
			aAxs=*top;
			break;
		case ARM_INDEPENDENT:
			BiConvLtoG(&pnt,&pnt,&cAxs);
			BiConvLtoG(&aPos.p,&pnt,par);
			aPos.a=ptr->mtn.a;

			BiRotLtoG(&cntCld,&ptr->cntCld,&aPos.a);
			BiSubPoint(&aPos.p,&aPos.p,&cntCld);

			BiMakeAxis(&aAxs,&aPos);
			break;
		case ARM_DEPENDONTOP:
			BiConvLtoG(&pnt,&pnt,&cAxs);
			BiRotFastLtoG(&ev,&ev,&cAxs.t);
			BiRotFastLtoG(&uv,&uv,&cAxs.t);

			BiConvLtoG(&aPos.p,&pnt,par);
			BiRotFastLtoG(&ev,&ev,&top->t);
			BiRotFastLtoG(&uv,&uv,&top->t);
			BiVectorToAngle(&aPos.a,&ev,&uv);

			BiRotLtoG(&cntCld,&ptr->cntCld,&aPos.a);
			BiSubPoint(&aPos.p,&aPos.p,&cntCld);

			BiMakeAxis(&aAxs,&aPos);
			break;
		}

		ptr->abs=aPos;

		ArCalcNodeAbsPos(ptr->next,par,top);
		ArCalcNodeAbsPos(ptr->cld,&aAxs,top);
	}
	return BI_OK;
}

int ArGetDnmAbsPosition(BIPOSATT *pos,ARDNM *mdl,int id)
{
	if(id<mdl->nObj)
	{
		*pos=mdl->obj[id]->abs;
		return BI_OK;
	}
	return BI_ERR;
}



/***************************************************************************/
int ArAddDnmStatus(int *stId,ARDNM *mdl,int id,int disp,BIPOSATT *pos)
{
	int nSta;
	nSta=mdl->obj[id]->nSta;
	if(nSta<ARM_MAX_N_STATUS)
	{
		mdl->obj[id]->sta[nSta].disp=disp;
		mdl->obj[id]->sta[nSta].pos=*pos;
		if(stId!=NULL)
		{
			*stId=mdl->obj[id]->nSta;
		}
		mdl->obj[id]->nSta++;
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetDnmStatus(int *disp,BIPOSATT *pos,ARDNM *mdl,int id,int stId)
{
	if(0<=stId && stId<mdl->obj[id]->nSta)
	{
		*disp=mdl->obj[id]->sta[stId].disp;
		*pos=mdl->obj[id]->sta[stId].pos;
		return BI_OK;
	}
	return BI_ERR;
}

int ArSetDnmStatus(ARDNM *mdl,int id,int stId)
{
	if(id==-1)
	{
		int i;
		for(i=0; i<mdl->nObj; i++)
		{
			ArSetDnmStatus(mdl,i,stId);
		}
		return BI_OK;
	}

	if(0<=stId && stId<mdl->obj[id]->nSta)
	{
		mdl->obj[id]->disp=mdl->obj[id]->sta[stId].disp;
		mdl->obj[id]->mtn=mdl->obj[id]->sta[stId].pos;
		return BI_OK;
	}
	return BI_ERR;
}

int ArInBetweenDnmStatus(ARDNM *mdl,int id,int st1,int st2,real t)
{
	BIPOSATT pos1,pos2,pos;

	if(st1==st2)
	{
		return ArSetDnmStatus(mdl,id,st1);
	}

	if(id==-1)
	{
		int i;
		for(i=0; i<mdl->nObj; i++)
		{
			ArInBetweenDnmStatus(mdl,i,st1,st2,t);
		}
		return BI_OK;
	}

	if(0<=st1 && st1<mdl->obj[id]->nSta && 0<=st2 && st2<mdl->obj[id]->nSta)
	{
		pos1=mdl->obj[id]->sta[st1].pos;
		pos2=mdl->obj[id]->sta[st2].pos;

		BiInBetweenPoint(&pos.p,&pos1.p,&pos2.p,t);
		BiInBetweenAngle(&pos.a,&pos1.a,&pos2.a,t);

		mdl->obj[id]->mtn=pos;

		if(t<=0.01)
		{
			mdl->obj[id]->disp=mdl->obj[id]->sta[st1].disp;
		}
		else if(t>=0.99)
		{
			mdl->obj[id]->disp=mdl->obj[id]->sta[st2].disp;
		}
		else
		{
			mdl->obj[id]->disp=
				((mdl->obj[id]->sta[st1].disp==BI_ON ||
				  mdl->obj[id]->sta[st2].disp==BI_ON) ?
				  BI_ON :
				  BI_OFF);
		}
		return BI_OK;
	}
	return BI_ERR;
}


/***************************************************************************/

int ArInsDnm(ARDNM *neo,BIPOSATT *pos)
{
	int i;
	ARDYNANODE *nd;

	ArCalcDnmAbsPosition(neo,pos);

	for(i=0; i<neo->nObj; i++)
	{
		nd=neo->obj[i];
		if(nd->disp==BI_ON)
		{
			BiPushMatrix(&nd->abs);

			switch(nd->type)
			{
			case ARM_SRFMODEL:
				BiInsSrf(&nd->shape.srf,&BiOrgPA);
				break;
			case ARM_LINE:
				BiInsLine(&nd->shape.lin.p1,&nd->shape.lin.p2,&nd->col);
				break;
			case ARM_POLYGON:
				BiInsPolyg(nd->shape.plg.np,nd->shape.plg.p,&nd->col);
				break;
			}

			BiPopMatrix();
		}
	}
	return BI_OK;
}

int ArInsDnmDivide(ARDNM *neo,BIPOSATT *pos)
{
	int i;
	ARDYNANODE *nd;

	ArCalcDnmAbsPosition(neo,pos);

	for(i=0; i<neo->nObj; i++)
	{
		nd=neo->obj[i];
		if(nd->disp==BI_ON)
		{
			BiPushMatrix(&nd->abs);

			switch(nd->type)
			{
			case ARM_SRFMODEL:
				BiInsSrfDivide(&nd->shape.srf,&BiOrgPA);
				break;
			case ARM_LINE:
				BiInsLine(&nd->shape.lin.p1,&nd->shape.lin.p2,&nd->col);
				break;
			case ARM_POLYGON:
				BiInsPolyg(nd->shape.plg.np,nd->shape.plg.p,&nd->col);
				break;
			}

			BiPopMatrix();
		}
	}
	return BI_OK;
}

int ArOvwDnmShadow(ARDNM *neo,BIPOSATT *pos)
{
	int i;
	ARDYNANODE *nd;

	ArCalcDnmAbsPosition(neo,pos);

	for(i=0; i<neo->nObj; i++)
	{
		nd=neo->obj[i];
		if(nd->disp==BI_ON)
		{
			BiPushMatrix(&nd->abs);

			switch(nd->type)
			{
			case ARM_SRFMODEL:
				BiOvwSrfShadow(&nd->shape.srf,&BiOrgPA);
				break;
			case ARM_LINE:
				BiOvwLineShadow(&nd->shape.lin.p1,&nd->shape.lin.p2,&BiBlack);
				break;
			case ARM_POLYGON:
				BiOvwPolygShadow(nd->shape.plg.np,nd->shape.plg.p,&BiBlack);
				break;
			}

			BiPopMatrix();
		}
	}
	return BI_OK;
}

