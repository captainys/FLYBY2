#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>
#include "aurora.h"


/***********************************************************************

    Wait! Wait!
    There's no way to share DNM & MPA. Because they keep
    instances inside.

***********************************************************************/

#undef MBOX
/* #define MBOX(x) HtMessageBox(x,NULL) */
#define MBOX(x)

extern int ArExecLoadFld(BIFLD *fld,char fn[]);

extern void BiSetFp(FILE *fp);
extern void BiSetStringList(char *(*lst));
extern char *BiGetStringFunc(char *str,int x);


static int ShareIsWorking=BI_ON;

int ArSetAuroraShare(int sw)
{
	ShareIsWorking=sw;
	return BI_OK;
}

int ArGetAuroraShareIsWorking(int *sw)
{
	*sw=ShareIsWorking;
	return BI_OK;
}

/***********************************************************************/
enum {
	ARSHARE_NONE,
	ARSHARE_SRF,
	ARSHARE_DNM,
	ARSHARE_PC2,
	ARSHARE_TER,
	ARSHARE_FLD
};

typedef union {
	BISRF srf;
	BIPC2 pc2;
	BITER ter;
	BIFLD fld;
	ARDNM dnm;
} ARSHAREDOBJUNION;

typedef struct ArSharedObjTag {
	struct ArSharedObjTag *next;
	unsigned int refer;
	char fn[256];
	int type;
	ARSHAREDOBJUNION uni;
} ARSHAREDOBJ;

static ARSHAREDOBJ *lst=NULL;

static ARSHAREDOBJ *ArNewSharedObj(int type,char fn[])
{
	ARSHAREDOBJ *neo;
	neo=(ARSHAREDOBJ *)BiMalloc(sizeof(ARSHAREDOBJ));
	if(neo!=NULL)
	{
		neo->refer=1;
		strcpy(neo->fn,fn);
		neo->type=type;

		neo->next=lst;
		lst=neo;
	}
	return neo;
}

static ARSHAREDOBJ *ArLookUpSharedObj(int type,char fn[])
{
	ARSHAREDOBJ *ptr;
	for(ptr=lst; ptr!=NULL; ptr=ptr->next)
	{
		if(ptr->type==type && strcmp(ptr->fn,fn)==0)
		{
			return ptr;
		}
	}
	return NULL;
}

static int ArFreeSharedObj(ARSHAREDOBJ *obj)
{
	if(obj==lst)
	{
		lst=obj->next;
		BiFree(obj);
	}
	else
	{
		ARSHAREDOBJ *ptr;
		for(ptr=lst; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->next==obj)
			{
				ptr->next=obj->next;
				BiFree(obj);
				return BI_OK;
			}
		}
		MBOX("ArFreeSharedObj@ashare.c(1)");
		return BI_ERR;
	}
	return BI_OK;
}

/***********************************************************************/
int ArLoadSrf(BISRFMODEL *srf,char fnOrg[])
{
	ARSHAREDOBJ *ptr;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	if(ShareIsWorking==BI_ON)
	{
		ptr=ArLookUpSharedObj(ARSHARE_SRF,fn);
		if(ptr!=NULL)
		{
			ptr->refer++;
			*srf=ptr->uni.srf;
			return BI_OK;
		}
		else if(BiLoadSrf(srf,fn)==BI_OK)
		{
			ptr=ArNewSharedObj(ARSHARE_SRF,fn);
			if(ptr!=NULL)
			{
				ptr->uni.srf=*srf;
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else
	{
		return BiLoadSrf(srf,fn);
	}
}

int ArLoadSrfFromString(BISRF *srf,char *lst[])
{
	return BiLoadSrfFromString(srf,lst);
}

int ArFreeSrf(BISRFMODEL *srf)
{
	ARSHAREDOBJ *ptr;
	if(ShareIsWorking==BI_ON)
	{
		for(ptr=lst; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->type==ARSHARE_SRF &&
			   ptr->uni.srf.vtTab==srf->vtTab &&
			   ptr->uni.srf.nv   ==srf->nv &&
			   ptr->uni.srf.v    ==srf->v &&
			   ptr->uni.srf.tmp  ==srf->tmp &&
			   ptr->uni.srf.np   ==srf->np &&
			   ptr->uni.srf.p    ==srf->p)
			{
				ptr->refer--;
				if(ptr->refer<=0)
				{
					BiFreeSrf(&ptr->uni.srf);
					ArFreeSharedObj(ptr);
				}
				return BI_OK;
			}
		}
	}
	BiFreeSrf(srf);
	return BI_OK;
}

/***********************************************************************/
int ArLoadFld(BIFLD *fld,char fnOrg[])
{
	ARSHAREDOBJ *ptr;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);
	if(ShareIsWorking==BI_ON)
	{
		ptr=ArLookUpSharedObj(ARSHARE_FLD,fn);
		if(ptr!=NULL)
		{
			ptr->refer++;
			*fld=ptr->uni.fld;
			return BI_OK;
		}
		else if(ArExecLoadFld(fld,fn)==BI_OK)
		{
			ptr=ArNewSharedObj(ARSHARE_FLD,fn);
			if(ptr!=NULL)
			{
				ptr->uni.fld=*fld;
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else
	{
		return ArExecLoadFld(fld,fn);
	}
}

int ArFreeFld(BIFLD *fld)
{
	ARSHAREDOBJ *ptr;
	if(ShareIsWorking==BI_ON)
	{
		for(ptr=lst; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->type==ARSHARE_FLD &&
			   ptr->uni.fld.nSrf==fld->nSrf &&
			   ptr->uni.fld.srf==fld->srf &&
			   ptr->uni.fld.nTer==fld->nTer &&
			   ptr->uni.fld.ter==fld->ter &&
			   ptr->uni.fld.nPc2==fld->nPc2 &&
			   ptr->uni.fld.pc2==fld->pc2 &&
			   ptr->uni.fld.nRgn==fld->nRgn &&
			   ptr->uni.fld.rgn==fld->rgn &&
			   ptr->uni.fld.nPlt==fld->nPlt &&
			   ptr->uni.fld.plt==fld->plt)
			{
				ptr->refer--;
				if(ptr->refer<=0)
				{
					BiFree(fld->srf);
					BiFree(fld->ter);
					BiFree(fld->pc2);
					BiFree(fld->rgn);
					BiFree(fld->plt);
					ArFreeSharedObj(ptr);
				}
				return BI_OK;
			}
		}
	}

	/* Cause Memory Leak! I'm still looking for solution */
	BiFree(fld->srf);
	BiFree(fld->ter);
	BiFree(fld->pc2);
	BiFree(fld->rgn);
	BiFree(fld->plt);
	return BI_OK;
}

/***********************************************************************/
int ArLoadPc2(BIPC2 *pc2,char fnOrg[])
{
	ARSHAREDOBJ *ptr;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);
	if(ShareIsWorking==BI_ON)
	{
		ptr=ArLookUpSharedObj(ARSHARE_PC2,fn);
		if(ptr!=NULL)
		{
			ptr->refer++;
			*pc2=ptr->uni.pc2;
			return BI_OK;
		}
		else if(BiLoadPc2(pc2,fn)==BI_OK)
		{
			ptr=ArNewSharedObj(ARSHARE_PC2,fn);
			if(ptr!=NULL)
			{
				ptr->uni.pc2=*pc2;
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else
	{
		return BiLoadPc2(pc2,fn);
	}
}

int ArLoadPc2FromString(BIPC2 *pc2,char *lst[])
{
	return BiLoadPc2FromString(pc2,lst);
}

int ArFreePc2(BIPC2 *pc2)
{
	ARSHAREDOBJ *ptr;
	if(ShareIsWorking==BI_ON)
	{
		for(ptr=lst; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->type==ARSHARE_PC2 &&
			   ptr->uni.pc2.nObj==pc2->nObj &&
			   ptr->uni.pc2.obj==pc2->obj)
			{
				ptr->refer--;
				if(ptr->refer<=0)
				{
					BiFree(pc2->obj);
					ArFreeSharedObj(ptr);
				}
				return BI_OK;
			}
		}
	}
	BiFree(pc2->obj);
	return BI_OK;
}

/***********************************************************************/
int ArLoadTer(BITER *ter,char fnOrg[])
{
	ARSHAREDOBJ *ptr;
	char fn[256];
	BiConstrainFileName(fn,fnOrg);
	if(ShareIsWorking==BI_ON)
	{
		ptr=ArLookUpSharedObj(ARSHARE_TER,fn);
		if(ptr!=NULL)
		{
			ptr->refer++;
			*ter=ptr->uni.ter;
			return BI_OK;
		}
		else if(BiLoadTer(ter,fn)==BI_OK)
		{
			ptr=ArNewSharedObj(ARSHARE_TER,fn);
			if(ptr!=NULL)
			{
				ptr->uni.ter=*ter;
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else
	{
		return BiLoadTer(ter,fn);
	}
}

int ArLoadTerFromString(BITER *ter,char *lst[])
{
	return BiLoadTerFromString(ter,lst);
}

int ArFreeTer(BITER *ter)
{
	ARSHAREDOBJ *ptr;
	if(ShareIsWorking==BI_ON)
	{
		for(ptr=lst; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->type==ARSHARE_TER &&
			   ptr->uni.ter.blk==ter->blk &&
			   ptr->uni.ter.sdPlg[0]==ter->sdPlg[0] &&
			   ptr->uni.ter.sdPlg[1]==ter->sdPlg[1] &&
			   ptr->uni.ter.sdPlg[2]==ter->sdPlg[2] &&
			   ptr->uni.ter.sdPlg[3]==ter->sdPlg[3])
			{
				ptr->refer--;
				if(ptr->refer<=0)
				{
					BiFree(ptr->uni.ter.blk);
					BiFree(ptr->uni.ter.sdPlg[0]);
					BiFree(ptr->uni.ter.sdPlg[1]);
					BiFree(ptr->uni.ter.sdPlg[2]);
					BiFree(ptr->uni.ter.sdPlg[3]);
					ArFreeSharedObj(ptr);
				}
				return BI_OK;
			}
		}
	}
	BiFree(ter->blk);
	BiFree(ter->sdPlg[0]);
	BiFree(ter->sdPlg[1]);
	BiFree(ter->sdPlg[2]);
	BiFree(ter->sdPlg[3]);
	return BI_OK;
}

/***********************************************************************/
static int DnmSerial=1;

int ArIncrementDnmReferenceCount(ARDNM *dnm)
{
	ARSHAREDOBJ *ptr;
	for(ptr=lst; ptr!=NULL; ptr=ptr->next)
	{
		if(ptr->type==ARSHARE_DNM && ptr->uni.dnm.RootId==dnm->RootId)
		{
			ptr->refer++;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int ArLoadDnm(ARDNM *dnm,char fnOrg[])
{
	ARSHAREDOBJ *ptr;
	char fn[256];
	BiConstrainFileName(fn,fnOrg);
	if(ShareIsWorking==BI_ON)
	{
		ptr=ArLookUpSharedObj(ARSHARE_DNM,fn);
		if(ptr!=NULL)
		{
			/* ArCopyDnm will increment reference count. */
			ArCopyDnm(dnm,&ptr->uni.dnm);
			return BI_OK;
		}
		else if(ArExecLoadDnm(dnm,fn)==BI_OK)
		{
			ptr=ArNewSharedObj(ARSHARE_DNM,fn);
			if(ptr!=NULL)
			{
				dnm->RootId=DnmSerial++;
				ArCopyDnm(&ptr->uni.dnm,dnm);
				/* Copy Dnm Increments reference count. So Reset it again. */
				ptr->refer=1;
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else
	{
		return ArExecLoadDnm(dnm,fn);
	}
}

int ArLoadDnmFromString(ARDNM *dnm,char *lst[])
{
	ARSHAREDOBJ *ptr;
	if(ShareIsWorking==BI_ON)
	{
		if(ArExecLoadDnmFromString(dnm,lst)==BI_OK)
		{
			ptr=ArNewSharedObj(ARSHARE_DNM,"");
			if(ptr!=NULL)
			{
				dnm->RootId=DnmSerial++;
				ArCopyDnm(&ptr->uni.dnm,dnm);
				/* ArCopyDnm Increments reference count. So Reset it again. */
				ptr->refer=1;
			}
			return BI_OK;
		}
		return BI_ERR;
	}
	else
	{
		return ArExecLoadDnmFromString(dnm,lst);
	}
}

int ArFreeDnm(ARDNM *dnm)
{
	int i;
	ARSHAREDOBJ *ptr;

	if(ShareIsWorking==BI_ON)
	{
		for(ptr=lst; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->type==ARSHARE_DNM && ptr->uni.dnm.RootId==dnm->RootId)
			{
				/* Custom generated DNMs will never hit this case. */
				ptr->refer--;
				if(ptr->refer<=0)
				{
					for(i=0; i<ptr->uni.dnm.nObj; i++)
					{
						switch(ptr->uni.dnm.obj[i]->type)
						{
						case ARM_SRFMODEL:
							ArFreeSrf(&ptr->uni.dnm.obj[i]->shape.srf);
							break;
						case ARM_POLYGON:
							BiFree(ptr->uni.dnm.obj[i]->shape.plg.p);
							break;
						case ARM_LINE:
							break;
						}
						BiFree(ptr->uni.dnm.obj[i]);
					}
				}
				ArFreeSharedObj(ptr);
				break;
			}
		}
	}

	/* There are some risks freeing BISRF due to PackedDNM */
	for(i=0; i<dnm->nObj; i++)
	{
		BiFree(dnm->obj[i]);
	}
	BiFree(dnm->obj);
	return BI_OK;
}

/***********************************************************************

    Wait! Wait!
    There's no way to share DNM & MPA. Because they keep
    instances inside.

***********************************************************************/

/***********************************************************************/
enum {
	ARPCKD_NOTDECIDED,
	ARPCKD_SRF,
	ARPCKD_PC2,
	ARPCKD_TER,
	ARPCKD_FLD,   /* Future Extension */
	ARPCKD_DNM    /* Future Extension */
};

typedef union {
	BISRF srf;
	BIPC2 pc2;
	BITER ter;
	BIFLD fld;
	ARDNM dnm;
} ARPACKEDUNI;

typedef struct ArPackedObject {
	struct ArPackedObject *next;
	char fn[256];
	int nl;
	char **l,*buf;

	int oType;
	ARPACKEDUNI uni;

	ARSHAREDOBJ *shared;
} ARPACKED;

typedef struct ArPackGroup {
	struct ArPackGroup *next;
	ARPACKED *lst;
} ARPACKGRP;




static ARPACKGRP *grp=NULL;



static ARPACKED *ArNewPackedObject(char fn[],int nl)
{
	ARPACKED *neo;
	neo=(ARPACKED *)BiMalloc(sizeof(ARPACKED));
	if(neo!=NULL)
	{
		neo->l=(char **)BiMalloc(sizeof(char *)*(nl+1));
		neo->buf=(char *)BiMalloc(sizeof(char)*128*nl);
		if(neo!=NULL && neo->buf!=NULL)
		{
			int i;
			for(i=0; i<nl; i++)
			{
				neo->l[i]=&neo->buf[i*128];
				neo->buf[i*128]=0;
			}
			strcpy(neo->fn,fn);
			neo->nl=nl;
			neo->l[nl]=NULL;
			neo->oType=ARPCKD_NOTDECIDED;

			neo->next=grp->lst;
			grp->lst=neo;

			neo->shared=NULL;

			return neo;
		}
		BiFree(neo);
	}
	return NULL;
}



int ArBeginPackedObject(void)
{
	ARPACKGRP *neo;
	neo=(ARPACKGRP *)BiMalloc(sizeof(ARPACKGRP));
	if(neo!=NULL)
	{
		neo->lst=NULL;
		neo->next=grp;
		grp=neo;
		return BI_OK;
	}
	return BI_ERR;
}

static void ArEndPackedObjectLoop(ARPACKED *ptr)
{
	if(ptr!=NULL)
	{
		ArEndPackedObjectLoop(ptr->next);
		BiFree(ptr->l);
		BiFree(ptr->buf);
		BiFree(ptr);
	}
}

int ArEndPackedObject(void)
{
	ARPACKGRP *next;
	if(grp!=NULL)
	{
		next=grp->next;

		ArEndPackedObjectLoop(grp->lst);
		BiFree(grp);

		grp=next;
		return BI_OK;
	}
}


int ArInsertPackedObject(char fn[],int nl)
{
	int i;
	ARPACKED *neo;
	neo=ArNewPackedObject(fn,nl);
	if(neo!=NULL)
	{
		for(i=0; i<nl; i++)
		{
			BiGetStringFunc(neo->l[i],128);
		}
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetFirstLineOfPackedObj(char str[],char fn[])
{
	ARPACKED *ptr;
	for(ptr=grp->lst; ptr!=NULL; ptr=ptr->next)
	{
		if(strcmp(ptr->fn,fn)==0 && ptr->nl>0)
		{
			strcpy(str,ptr->l[0]);
			return BI_OK;
		}
	}
	return BI_ERR;
}

int ArLoadSrfFromPack(BISRF *srf,char fn[])
{
	ARPACKED *ptr;
	for(ptr=grp->lst; ptr!=NULL; ptr=ptr->next)
	{
		if(strcmp(ptr->fn,fn)==0)
		{
			if(ptr->oType==ARPCKD_SRF)
			{
				*srf=ptr->uni.srf;
				ptr->shared->refer++;
				return BI_OK;
			}
			else
			{
				if(BiLoadSrfFromString(srf,ptr->l)==BI_OK)
				{
					ptr->oType=ARPCKD_SRF;
					ptr->uni.srf=*srf;

					ptr->shared=ArNewSharedObj(ARSHARE_SRF,"*##SHARED##*");
					if(ptr->shared!=NULL)
					{
						ptr->shared->uni.srf=*srf;
					}

					return BI_OK;
				}
				else
				{
					return BI_ERR;
				}
			}
		}
	}
	return BI_ERR;
}

int ArLoadPc2FromPack(BIPC2 *pc2,char fn[])
{
	ARPACKED *ptr;
	for(ptr=grp->lst; ptr!=NULL; ptr=ptr->next)
	{
		if(strcmp(ptr->fn,fn)==0)
		{
			if(ptr->oType==ARPCKD_PC2)
			{
				*pc2=ptr->uni.pc2;
				ptr->shared->refer++;
				return BI_OK;
			}
			else
			{
				if(BiLoadPc2FromString(pc2,ptr->l)==BI_OK)
				{
					ptr->oType=ARPCKD_PC2;
					ptr->uni.pc2=*pc2;

					ptr->shared=ArNewSharedObj(ARSHARE_PC2,"*##SHARED##*");
					if(ptr->shared!=NULL)
					{
						ptr->shared->uni.pc2=*pc2;
					}
					return BI_OK;
				}
				else
				{
					return BI_ERR;
				}
			}
		}
	}
	return BI_ERR;
}

int ArLoadTerFromPack(BITER *ter,char fn[])
{
	ARPACKED *ptr;
	for(ptr=grp->lst; ptr!=NULL; ptr=ptr->next)
	{
		if(strcmp(ptr->fn,fn)==0)
		{
			if(ptr->oType==ARPCKD_TER)
			{
				*ter=ptr->uni.ter;
				ptr->shared->refer++;
				return BI_OK;
			}
			else
			{
				if(BiLoadTerFromString(ter,ptr->l)==BI_OK)
				{
					ptr->oType=ARPCKD_TER;
					ptr->uni.ter=*ter;

					ptr->shared=ArNewSharedObj(ARSHARE_TER,"*##SHARED##*");
					if(ptr->shared!=NULL)
					{
						ptr->shared->uni.ter=*ter;
					}
					return BI_OK;
				}
				else
				{
					return BI_ERR;
				}
			}
		}
	}
	return BI_ERR;
}

int ArLoadFldFromPack(BIFLD *fld,char fn[])
{
	ARPACKED *ptr;
	for(ptr=grp->lst; ptr!=NULL; ptr=ptr->next)
	{
		if(strcmp(ptr->fn,fn)==0)
		{
			if(ptr->oType==ARPCKD_FLD)
			{
				*fld=ptr->uni.fld;
				ptr->shared->refer++;
				return BI_OK;
			}
			else
			{
				if(ArLoadFldFromString(fld,ptr->l)==BI_OK)
				{
					ptr->oType=ARPCKD_FLD;
					ptr->uni.fld=*fld;

					ptr->shared=ArNewSharedObj(ARSHARE_FLD,"*##SHARED##*");
					if(ptr->shared!=NULL)
					{
						ptr->shared->uni.fld=*fld;
					}
					return BI_OK;
				}
				else
				{
					return BI_ERR;
				}
			}
		}
	}
	return BI_ERR;
}

int ArLoadDnmFromPack(ARDNM *dnm,char fn[])
{
	ARPACKED *ptr;
	for(ptr=grp->lst; ptr!=NULL; ptr=ptr->next)
	{
		if(strcmp(ptr->fn,fn)==0)
		{
			if(ptr->oType==ARPCKD_DNM)
			{
				return ArCopyDnm(dnm,&ptr->uni.dnm);
			}
			else
			{
				if(ArLoadDnmFromString(dnm,ptr->l)==BI_OK)
				{
					ptr->oType=ARPCKD_DNM;
					ptr->uni.dnm=*dnm;

					ptr->shared=ArNewSharedObj(ARSHARE_DNM,"*##SHARED##*");
					if(ptr->shared!=NULL)
					{
						ptr->shared->uni.dnm=*dnm;
						ArCopyDnm(&ptr->shared->uni.dnm,dnm);
						/* Copy Dnm Increments reference count. So Reset it again. */
						ptr->shared->refer=1;
					}
					return BI_OK;
				}
				else
				{
					return BI_ERR;
				}
			}
		}
	}
	return BI_ERR;
}
