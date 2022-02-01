#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "impulse.h"
#include "iutil.h"



/************************************************************************
FIELD

GND r g b
SKY r g b

PC2
FIL filename.pc2
POS x y z h p b
END

PLT
FIL filename.pc2
POS x y z h p b
END

SRF
FIL filename.srf
POS x y z h p b
ID id
TAG string
LOD distance
END

TER
FIL filename.ter
POS x y z h p b
ID id
TAG string
LOD distance
END

RGN
ARE xmin zmin xmax zmax
POS x y z h p b
ID id
TAG string
END

FLD
FIL filename.fld
POS x y z h p b
LOD distance
END

ENDF
************************************************************************/


static int BiCountNumObj(int *nSrf,int *nTer,int *nPc2,int *nRgn,int *nFld,int *nPlt,FILE *fp);
static int BiMallocObj(BIFLD *fld,int nSrf,int nTer,int nPc2,int nRgn,int nFld,int nPlt);
static int BiReadObj(BIFLD *fld,char pth[],FILE *fp);
static int BiReadPc2(BIFLDPC2 *pc2,char pth[],FILE *fp,int n,BIFLDPC2 lst[]);
static int BiReadSrf(BIFLDSRF *srf,char pth[],FILE *fp,int n,BIFLDSRF lst[]);
static int BiReadTer(BIFLDTER *ter,char pth[],FILE *fp,int n,BIFLDTER lst[]);
static int BiReadFld(BIFLDFLD *fld,char pth[],FILE *fp,int n,BIFLDFLD lst[]);
static int BiReadRgn(BIFLDRGN *rgn,FILE *fp);

int BiLoadFld(BIFLD *fld,char fnOrg[])
{
	int nSrf,nTer,nRgn,nPc2,nFld,nPlt;
	char pth[256],fil[256],fn[256];
	FILE *fp;

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp!=NULL &&
	   BiCountNumObj(&nSrf,&nTer,&nPc2,&nRgn,&nFld,&nPlt,fp)==BI_OK)
	{
		BiSeparatePathFile(pth,fil,fn);
		fseek(fp,0,0);
		if(BiMallocObj(fld,nSrf,nTer,nPc2,nRgn,nFld,nPlt)==BI_OK &&
		   BiReadObj(fld,pth,fp)==BI_OK)
		{
			fclose(fp);
			return BI_OK;
		}
		fclose(fp);
	}
	return BI_ERR;
}

static char *topCmd[]=
{
	"FIELD","PC2","SRF","TER","RGN","FLD","GND","SKY","ENDF","PLT",NULL
};

static int BiCountNumObj(int *nSrf,int *nTer,int *nPc2,int *nRgn,int *nFld,int *nPlt,FILE *fp)
{
	int ac,cmd;
	char str[256],*av[16];
	*nSrf=0;
	*nTer=0;
	*nPc2=0;
	*nRgn=0;
	*nFld=0;
	*nPlt=0;
	while(fgets(str,255,fp)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0 && BiCommandNumber(&cmd,av[0],topCmd)==BI_OK)
		{
			switch(cmd)
			{
			case 1:
				(*nPc2)++;
				break;
			case 2:
				(*nSrf)++;
				break;
			case 3:
				(*nTer)++;
				break;
			case 4:
				(*nRgn)++;
				break;
			case 5:
				(*nFld)++;
				break;
			case 9:
				(*nPlt)++;
				break;
			}
		}
	}
	return BI_OK;
}

static int BiMallocObj
    (BIFLD *fld,int nSrf,int nTer,int nPc2,int nRgn,int nFld,int nPlt)
{
	fld->srf=(BIFLDSRF *)BiMalloc(nSrf*sizeof(BIFLDSRF));
	fld->ter=(BIFLDTER *)BiMalloc(nTer*sizeof(BIFLDTER));
	fld->pc2=(BIFLDPC2 *)BiMalloc(nPc2*sizeof(BIFLDPC2));
	fld->rgn=(BIFLDRGN *)BiMalloc(nRgn*sizeof(BIFLDRGN));
	fld->plt=(BIFLDPC2 *)BiMalloc(nPlt*sizeof(BIFLDPC2));
	fld->fld=(void *)BiMalloc(nFld*sizeof(BIFLDFLD));
	if((nSrf>0 && fld->srf==NULL) ||
	   (nTer>0 && fld->ter==NULL) ||
	   (nPc2>0 && fld->pc2==NULL) ||
	   (nRgn>0 && fld->rgn==NULL) ||
	   (nFld>0 && fld->fld==NULL) ||
	   (nPlt>0 && fld->plt==NULL))
	{
		return BI_ERR;
	}
	return BI_OK;
}

static int BiReadObj(BIFLD *fld,char pth[],FILE *fp)
{
	int ac,cmd,er;
	char str[256],*av[16];
	BIFLDSRF *srf;
	BIFLDPC2 *pc2;
	BIFLDTER *ter;
	BIFLDRGN *rgn;
	BIFLDPC2 *plt;
	BIFLDFLD *ffd;

	srf=fld->srf;
	pc2=fld->pc2;
	ter=fld->ter;
	rgn=fld->rgn;
	plt=fld->plt;
	ffd=fld->fld;

	fld->nSrf=0;
	fld->nTer=0;
	fld->nPc2=0;
	fld->nRgn=0;
	fld->nFld=0;
	fld->nPlt=0;
	BiSetColorRGB(&fld->sky,32,180,180);
	BiSetColorRGB(&fld->gnd,32,128,32);

	if(fgets(str,255,fp)!=NULL && strncmp(str,"FIELD",5)==0)
	{
		while(fgets(str,255,fp)!=NULL)
		{
			er=BI_OK;
			BiArguments(&ac,av,15,str);
			if(ac>0 && BiCommandNumber(&cmd,av[0],topCmd)==BI_OK)
			{
				switch(cmd)
				{
				case 1:
					er=BiReadPc2(&pc2[fld->nPc2],pth,fp,fld->nPc2,pc2);
					fld->nPc2++;
					break;
				case 2:
					er=BiReadSrf(&srf[fld->nSrf],pth,fp,fld->nSrf,srf);
					fld->nSrf++;
					break;
				case 3:
					er=BiReadTer(&ter[fld->nTer],pth,fp,fld->nTer,ter);
					fld->nTer++;
					break;
				case 4:
					er=BiReadRgn(&rgn[fld->nRgn],fp);
					fld->nRgn++;
					break;
				case 5:
					er=BiReadFld(&ffd[fld->nFld],pth,fp,fld->nFld,ffd);
					fld->nFld++;
					break;
				case 6:
					fld->gnd.r=atoi(av[1]);
					fld->gnd.g=atoi(av[2]);
					fld->gnd.b=atoi(av[3]);
					break;
				case 7:
					fld->sky.r=atoi(av[1]);
					fld->sky.g=atoi(av[2]);
					fld->sky.b=atoi(av[3]);
					break;
				case 8:
					return BI_OK;
				case 9:
					er=BiReadPc2(&plt[fld->nPlt],pth,fp,fld->nPlt,plt);
					fld->nPlt++;
					break;
				}
			}
			if(er!=BI_OK)
			{
				return BI_ERR;
			}
		}
		return BI_OK; /* ? It must be `Missing ENDF' error. */
	}
	return BI_ERR;
}

static char *filCmd[]=
{
	"FIL","POS","ID","TAG","END","LOD",NULL
};

static int BiReadPc2(BIFLDPC2 *pc2,char pth[],FILE *fp,int n,BIFLDPC2 lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	pc2->lodDist=10000000.0F;  /* 10000 km */
	while(fgets(str,255,fp)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0 && BiCommandNumber(&cmd,av[0],filCmd)==BI_OK)
		{
			switch(cmd)
			{
			case 0:
				strcpy(pc2->fn,av[1]);
				for(i=0; i<n; i++)
				{
					if(strcmp(lst[i].fn,av[1])==0)
					{
						pc2->pc2=lst[i].pc2;
						break;
					}
				}
				if(i==n)
				{
					sprintf(ful,"%s%s",pth,av[1]);
					if(BiLoadPc2(&pc2->pc2,ful)!=BI_OK)
					{
						return BI_ERR;
					}
				}
				break;
			case 1:
				pc2->pos.p.x=(real)atof(av[1]);
				pc2->pos.p.y=(real)atof(av[2]);
				pc2->pos.p.z=(real)atof(av[3]);
				pc2->pos.a.h=(long)atoi(av[4]);
				pc2->pos.a.p=(long)atoi(av[5]);
				pc2->pos.a.b=(long)atoi(av[6]);
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				return BI_OK;
			case 5:
				pc2->lodDist=(real)atof(av[1]);
				break;
			}
		}
	}
	return BI_ERR;  /* Missing END */
}

static int BiReadSrf(BIFLDSRF *srf,char pth[],FILE *fp,int n,BIFLDSRF lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	srf->id=0;
	srf->tag[0]=0;
	srf->lodDist=10000000.0F;  /* 10000 km */
	while(fgets(str,255,fp)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0 && BiCommandNumber(&cmd,av[0],filCmd)==BI_OK)
		{
			switch(cmd)
			{
			case 0:
				strcpy(srf->fn,av[1]);
				for(i=0; i<n; i++)
				{
					if(strcmp(lst[i].fn,av[1])==0)
					{
						srf->srf=lst[i].srf;
						break;
					}
				}
				if(i==n)
				{
					sprintf(ful,"%s%s",pth,av[1]);
					if(BiLoadSrf(&srf->srf,ful)!=BI_OK)
					{
						return BI_ERR;
					}
				}
				break;
			case 1:
				srf->pos.p.x=(real)atof(av[1]);
				srf->pos.p.y=(real)atof(av[2]);
				srf->pos.p.z=(real)atof(av[3]);
				srf->pos.a.h=(long)atoi(av[4]);
				srf->pos.a.p=(long)atoi(av[5]);
				srf->pos.a.b=(long)atoi(av[6]);
				break;
			case 2:
				srf->id=(int)atoi(av[1]);
				break;
			case 3:
				if(ac>=2 && av[1]!=NULL)
				{
					strncpy(srf->tag,av[1],BIFLD_TAG_LEN-1);
				}
				else
				{
					srf->tag[0]=0;
				}
				break;
			case 4:
				return BI_OK;
			case 5:
				srf->lodDist=(real)atof(av[1]);
				break;
			}
		}
	}
	return BI_ERR;  /* Missing END */
}

static int BiReadTer(BIFLDTER *ter,char pth[],FILE *fp,int n,BIFLDTER lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	ter->id=0;
	ter->tag[0]=0;
	ter->lodDist=10000000.0F;  /* 10000 km */
	while(fgets(str,255,fp)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0 && BiCommandNumber(&cmd,av[0],filCmd)==BI_OK)
		{
			switch(cmd)
			{
			case 0:
				strcpy(ter->fn,av[1]);
				for(i=0; i<n; i++)
				{
					if(strcmp(lst[i].fn,av[1])==0)
					{
						ter->ter=lst[i].ter;
						break;
					}
				}
				if(i==n)
				{
					sprintf(ful,"%s%s",pth,av[1]);
					if(BiLoadTer(&ter->ter,ful)!=BI_OK)
					{
						return BI_ERR;
					}
				}
				break;
			case 1:
				ter->pos.p.x=(real)atof(av[1]);
				ter->pos.p.y=(real)atof(av[2]);
				ter->pos.p.z=(real)atof(av[3]);
				ter->pos.a.h=(long)atoi(av[4]);
				ter->pos.a.p=(long)atoi(av[5]);
				ter->pos.a.b=(long)atoi(av[6]);
				break;
			case 2:
				ter->id=(int)atoi(av[1]);
				break;
			case 3:
				if(ac>=2 && av[1]!=NULL)
				{
					strncpy(ter->tag,av[1],BIFLD_TAG_LEN-1);
				}
				else
				{
					ter->tag[0]=0;
				}
				break;
			case 4:
				return BI_OK;
			case 5:
				ter->lodDist=(real)atof(av[1]);
				break;
			}
		}
	}
	return BI_ERR;  /* Missing END */
}

static int BiReadFld(BIFLDFLD *fld,char pth[],FILE *fp,int n,BIFLDFLD lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	fld->lodDist=10000000.0F;  /* 10000 km */
	while(fgets(str,255,fp)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0 && BiCommandNumber(&cmd,av[0],filCmd)==BI_OK)
		{
			switch(cmd)
			{
			case 0:
				strcpy(fld->fn,av[1]);
				sprintf(ful,"%s%s",pth,av[1]);
				for(i=0; i<n; i++)
				{
					if(strcmp(lst[i].fn,av[1])==0)
					{
						fld->fld=lst[i].fld;
						break;
					}
				}
				if(i==n)
				{
					if(BiLoadFld(&fld->fld,ful)!=BI_OK)
					{
						return BI_ERR;
					}
				}
				break;
			case 1:
				fld->pos.p.x=(real)atof(av[1]);
				fld->pos.p.y=(real)atof(av[2]);
				fld->pos.p.z=(real)atof(av[3]);
				fld->pos.a.h=(long)atoi(av[4]);
				fld->pos.a.p=(long)atoi(av[5]);
				fld->pos.a.b=(long)atoi(av[6]);
				break;
			case 2:
			case 3:
				break;
			case 4:
				return BI_OK;
			case 5:
				fld->lodDist=(real)atof(av[1]);
				break;
			}
		}
	}
	return BI_ERR;  /* Missing END */
}

static char *rgnCmd[]=
{
	"ARE","POS","ID","TAG","END",NULL
};

static int BiReadRgn(BIFLDRGN *rgn,FILE *fp)
{
	int ac,cmd;
	char str[256],*av[16];
	while(fgets(str,255,fp)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0 && BiCommandNumber(&cmd,av[0],rgnCmd)==BI_OK)
		{
			switch(cmd)
			{
			case 0:
				rgn->min.x=(real)atof(av[1]);
				rgn->min.y=(real)atof(av[2]);
				rgn->max.x=(real)atof(av[3]);
				rgn->max.y=(real)atof(av[4]);
				break;
			case 1:
				rgn->pos.p.x=(real)atof(av[1]);
				rgn->pos.p.y=(real)atof(av[2]);
				rgn->pos.p.z=(real)atof(av[3]);
				rgn->pos.a.h=(long)atoi(av[4]);
				rgn->pos.a.p=(long)atoi(av[5]);
				rgn->pos.a.b=(long)atoi(av[6]);
				break;
			case 2:
				rgn->id=(int)atoi(av[1]);
				break;
			case 3:
				if(ac>=2 && av[1]!=NULL)
				{
					strncpy(rgn->tag,av[1],BIFLD_TAG_LEN-1);
				}
				else
				{
					rgn->tag[0]=0;
				}
				break;
			case 4:
				return BI_OK;
			}
		}
	}
	return BI_ERR;  /* Missing END */
}


/************************************************************************/
static void BiConvPosLtoG(BIPOSATT *neo,BIPOSATT *org,BIAXIS *axs)
{
	BIPOINT ev,uv;

	BiAngleToVector(&ev,&uv,&org->a);
	BiRotFastLtoG(&ev,&ev,&axs->t);
	BiRotFastLtoG(&uv,&uv,&axs->t);
	BiVectorToAngle(&neo->a,&ev,&uv);

	BiConvLtoG(&neo->p,&org->p,axs);
}

/***************************************************************************/

int BiGetFldNumObj(int *nObj,BIFLD *fld,int typ)
{
	switch(typ)
	{
	default:
		*nObj=0;
		return BI_ERR;
	case BIFLD_PC2MAP:
		*nObj=fld->nPc2;
		return BI_OK;
	case BIFLD_SRF:
		*nObj=fld->nSrf;
		return BI_OK;
	case BIFLD_PC2:
		*nObj=fld->nPlt;
		return BI_OK;
	case BIFLD_TER:
		*nObj=fld->nTer;
		return BI_OK;
	case BIFLD_RGN:
		*nObj=fld->nRgn;
		return BI_OK;
	case BIFLD_FLD:
		*nObj=fld->nFld;
		return BI_OK;
	}
}

int BiGetFldObjPosition(BIPOSATT *pos,BIFLD *fld,BIPOSATT *lay,int typ,int id)
{
	int nObj;
	BIPOSATT tmp;
	BIAXIS axs;

	if(BiGetFldNumObj(&nObj,fld,typ)==BI_OK && 0<=id && id<nObj)
	{
		BiMakeAxis(&axs,lay);

		switch(typ)
		{
		default:
			*pos=BiOrgPA;
			return BI_ERR;
		case BIFLD_PC2MAP:
			tmp=fld->pc2[id].pos;
			break;
		case BIFLD_SRF:
			tmp=fld->srf[id].pos;
			break;
		case BIFLD_PC2:
			tmp=fld->plt[id].pos;
			break;
		case BIFLD_TER:
			tmp=fld->ter[id].pos;
			break;
		case BIFLD_RGN:
			tmp=fld->rgn[id].pos;
			break;
		case BIFLD_FLD:
			tmp=((BIFLDFLD *)(fld->fld))[id].pos;
			break;
		}

		BiConvPosLtoG(pos,&tmp,&axs);
		return BI_OK;
	}
	return BI_ERR;
}

int BiGetFldObjId(int *oId,char tag[],BIFLD *fld,int typ,int id)
{
	int nObj;

	if(BiGetFldNumObj(&nObj,fld,typ)==BI_OK && 0<=id && id<nObj)
	{
		switch(typ)
		{
		default:
		case BIFLD_PC2:
		case BIFLD_FLD:
		case BIFLD_PC2MAP:
			*oId=0;
			tag[0]=0;
			return BI_ERR;
		case BIFLD_SRF:
			*oId=fld->srf[id].id;
			strcpy(tag,fld->srf[id].tag);
			break;
		case BIFLD_TER:
			*oId=fld->ter[id].id;
			strcpy(tag,fld->ter[id].tag);
			break;
		case BIFLD_RGN:
			*oId=fld->rgn[id].id;
			strcpy(tag,fld->rgn[id].tag);
			break;
		}
		return BI_OK;
	}
	return BI_ERR;
}


/************************************************************************/

extern BIAXISF BiEyeAxs;

static int BiCheckLod(BIPOSATT *pos,real lodDist)
{
	BIVECTOR d;
	BiSubPoint(&d,&pos->p,&BiEyeAxs.p);
	if(lodDist*lodDist<d.x*d.x+d.y*d.y+d.z*d.z)
	{
		return BI_FALSE;
	}
	return BI_TRUE;
}

void BiOvwFld(BIFLD *fld,BIPOSATT *pos)
{
	int i;
	BIAXIS axs;
	BIPOSATT obj;
	BiPntAngToAxis(&axs,pos);
	for(i=0; i<fld->nPc2; i++)
	{
		obj=fld->pc2[i].pos;
		BiPitchUp(&obj.a,&obj.a,-16384,0);
		BiConvPosLtoG(&obj,&obj,&axs);
		if(BiCheckLod(&obj,fld->pc2[i].lodDist)==BI_TRUE)
		{
			BiOvwPc2(&fld->pc2[i].pc2,&obj);
		}
	}
	for(i=0; i<fld->nFld; i++)
	{
		BiConvPosLtoG(&obj,&fld->fld[i].pos,&axs);
		if(BiCheckLod(&obj,fld->fld[i].lodDist)==BI_TRUE)
		{
			BiOvwFld(&fld->fld[i].fld,&obj);
		}
	}
}


/************************************************************************/
void BiInsFld(BIFLD *fld,BIPOSATT *pos)
{
	int i;
	BIAXIS axs;
	BIPOSATT obj;
	BiPntAngToAxis(&axs,pos);
	for(i=0; i<fld->nSrf; i++)
	{
		BiConvPosLtoG(&obj,&fld->srf[i].pos,&axs);
		if(BiCheckLod(&obj,fld->srf[i].lodDist)==BI_TRUE)
		{
			BiInsSrf(&fld->srf[i].srf,&obj);
		}
	}
	for(i=0; i<fld->nTer; i++)
	{
		BiConvPosLtoG(&obj,&fld->ter[i].pos,&axs);
		if(BiCheckLod(&obj,fld->ter[i].lodDist)==BI_TRUE)
		{
			BiInsTer(&fld->ter[i].ter,&obj);
		}
	}
	for(i=0; i<fld->nPlt; i++)
	{
		BiConvPosLtoG(&obj,&fld->plt[i].pos,&axs);
		if(BiCheckLod(&obj,fld->plt[i].lodDist)==BI_TRUE)
		{
			BiInsPc2(&fld->plt[i].pc2,&obj);
		}
	}
	for(i=0; i<fld->nFld; i++)
	{
		BiConvPosLtoG(&obj,&fld->fld[i].pos,&axs);
		if(BiCheckLod(&obj,fld->fld[i].lodDist)==BI_TRUE)
		{
			BiInsFld(&fld->fld[i].fld,&obj);
		}
	}
}

/************************************************************************/
int BiGetFldRegion(int *rgId,char tag[],BIFLD *fld,BIPOSATT *pos,BIPOINT *p)
{
	int i;
	BIAXIS fAxs,cAxs;
	BIPOINT q;
	BIPOINT2 min,max;
	BIFLD *cFld;
	BIPOSATT cPos;

	BiPntAngToAxis(&fAxs,pos);
	for(i=0; i<fld->nRgn; i++)
	{
		min=fld->rgn[i].min;
		max=fld->rgn[i].max;

		BiConvPosLtoG(&cPos,&fld->rgn[i].pos,&fAxs);
		BiPntAngToAxis(&cAxs,&cPos);
		BiConvGtoL(&q,p,&cAxs);

		if(min.x<=q.x && q.x<=max.x && min.y<=q.z && q.z<=max.y)
		{
			if(rgId!=NULL)
			{
				*rgId=fld->rgn[i].id;
			}
			if(tag!=NULL)
			{
				strcpy(tag,fld->rgn[i].tag);
			}
			return BI_IN;
		}
	}
	for(i=0; i<fld->nFld; i++)
	{
		cFld=&fld->fld[i].fld;
		BiConvPosLtoG(&cPos,&((BIFLDFLD *)(fld->fld))[i].pos,&fAxs);
		if(BiGetFldRegion(rgId,tag,cFld,&cPos,p)==BI_IN)
		{
			return BI_IN;
		}
	}
	return BI_OUT;
}

int BiGetFldElevation(int *fdId,char tag[],real *elv,BIPOINT *ev,BIPOINT *uv,BIFLD *fld,BIPOSATT *pos,BIPOINT *p,long hdg)
{
	int i;
	BIAXIS fAxs;
	BIFLD *cFld;
	BIPOSATT cPos;
	BIPOINT tev,tuv;
	BIANGLE ang;

	BiPntAngToAxis(&fAxs,pos);
	for(i=0; i<fld->nTer; i++)
	{
		BiConvPosLtoG(&cPos,&fld->ter[i].pos,&fAxs);

		if(BiTerHeight(elv,p->x,p->z,&fld->ter[i].ter,&cPos)==BI_IN)
		{
			if(fdId!=NULL)
			{
				*fdId=fld->ter[i].id;
			}
			if(tag!=NULL)
			{
				strcpy(tag,fld->ter[i].tag);
			}
			if(ev!=NULL || uv!=NULL)
			{
				ev=(ev!=NULL ? ev : &tev);
				uv=(uv!=NULL ? uv : &tuv);
				BiTerEyeVecUpVec(ev,uv,p->x,p->z,hdg,&fld->ter[i].ter,&cPos);
			}
			return BI_IN;
		}
	}
	for(i=0; i<fld->nFld; i++)
	{
		cFld=&fld->fld[i].fld;
		BiConvPosLtoG(&cPos,&((BIFLDFLD *)(fld->fld))[i].pos,&fAxs);
		if(BiGetFldElevation(fdId,tag,elv,ev,uv,cFld,&cPos,p,hdg)==BI_IN)
		{
			return BI_IN;
		}
	}
	BiSetAngle(&ang,hdg,0,0);
	ev=(ev!=NULL ? ev : &tev);
	uv=(uv!=NULL ? uv : &tuv);
	BiAngleToVector(ev,uv,&ang);
	*elv=0.0F;
	if(fdId!=NULL)
	{
		*fdId=0;
	}
	if(tag!=NULL)
	{
		tag[0]=0;
	}
	return BI_OUT;
}

int BiGetFldSrfCollision(int *sfId,char tag[],BIFLD *fld,BIPOSATT *pos,BIPOINT *p,real bump)
{
	int i;
	BIAXIS fAxs;
	BIFLD *cFld;
	BIPOSATT cPos;

	BiPntAngToAxis(&fAxs,pos);
	for(i=0; i<fld->nSrf; i++)
	{
		BiConvPosLtoG(&cPos,&fld->srf[i].pos,&fAxs);
		if(BiSrfCollision(&fld->srf[i].srf,&cPos,p,bump)==BI_IN)
		{
			if(sfId!=NULL)
			{
				*sfId=fld->srf[i].id;
			}
			if(tag!=NULL)
			{
				strcpy(tag,fld->srf[i].tag);
			}
			return BI_IN;
		}
	}
	for(i=0; i<fld->nFld; i++)
	{
		cFld=&fld->fld[i].fld;
		BiConvPosLtoG(&cPos,&((BIFLDFLD *)(fld->fld))[i].pos,&fAxs);
		if(BiGetFldSrfCollision(sfId,tag,cFld,&cPos,p,bump)==BI_IN)
		{
			return BI_IN;
		}
	}
	if(sfId!=NULL)
	{
		sfId=0;
	}
	if(tag!=NULL)
	{
		tag[0]=0;
	}
	return BI_OUT;
}

int BiGetFldGroundSky(BICOLOR *gnd,BICOLOR *sky,BIFLD *fld)
{
	if(gnd!=NULL)
	{
		*gnd=fld->gnd;
	}
	if(sky!=NULL)
	{
		*sky=fld->sky;
	}
	return BI_OK;
}
