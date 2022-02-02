#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>

#include "aurora.h"



extern void BiPushStringFunc(void);
extern void BiPopStringFunc(void);
extern void BiSetFp(FILE *fp);
extern void BiSetStringList(char *(*lst));
extern char *BiGetStringFunc(char *str,int x);



/************************************************************************
FIELD

PCK filename nline

GND r g b
SKY r g b

PC2
FIL filename.pc2
POS x y z h p b
LOD distance
END

PLT
FIL filename.pc2
POS x y z h p b
LOD distance
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


static int ArCountNumObj(int *nSrf,int *nTer,int *nPc2,int *nRgn,int *nFld,int *nPlt);
static int ArMallocObj(BIFLD *fld,int nSrf,int nTer,int nPc2,int nRgn,int nFld,int nPlt);
static int ArReadObj(BIFLD *fld,char pth[]);
static int ArReadPc2(BIFLDPC2 *pc2,char pth[],int n,BIFLDPC2 lst[]);
static int ArReadSrf(BIFLDSRF *srf,char pth[],int n,BIFLDSRF lst[]);
static int ArReadTer(BIFLDTER *ter,char pth[],int n,BIFLDTER lst[]);
static int ArReadFld(BIFLDFLD *fld,char pth[],int n,BIFLDFLD lst[]);
static int ArReadRgn(BIFLDRGN *rgn);

int ArExecLoadFld(BIFLD *fld,char fnOrg[])
{
	int nSrf,nTer,nRgn,nPc2,nFld,nPlt;
	char pth[256],fil[256],fn[256];
	FILE *fp;

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		BiPushStringFunc();
		BiSetFp(fp);
		ArBeginPackedObject();

		if(ArCountNumObj(&nSrf,&nTer,&nPc2,&nRgn,&nFld,&nPlt)==BI_OK)
		{
			SkSeparatePathFile(pth,fil,fn);
			fseek(fp,0,0);
			if(ArMallocObj(fld,nSrf,nTer,nPc2,nRgn,nFld,nPlt)==BI_OK &&
			   ArReadObj(fld,pth)==BI_OK)
			{
				ArEndPackedObject();
				BiPopStringFunc();

				fclose(fp);
				return BI_OK;
			}
		}
		ArEndPackedObject();
		BiPopStringFunc();

		fclose(fp);
	}
	return BI_ERR;
}

int ArLoadFldFromString(BIFLD *fld,char *lst[])
{
	int nSrf,nTer,nRgn,nPc2,nFld,nPlt;

	BiPushStringFunc();
	BiSetStringList(lst);
	ArBeginPackedObject();

	if(ArCountNumObj(&nSrf,&nTer,&nPc2,&nRgn,&nFld,&nPlt)==BI_OK)
	{
		BiSetStringList(lst);
		if(ArMallocObj(fld,nSrf,nTer,nPc2,nRgn,nFld,nPlt)==BI_OK &&
		   ArReadObj(fld,"")==BI_OK)
		{
			ArEndPackedObject();
			BiPopStringFunc();

			return BI_OK;
		}
	}
	ArEndPackedObject();
	BiPopStringFunc();

	MBOX("ArLoadFldFromString(1)");
	return BI_ERR;
}


static char *topCmd[]=
{
	"FIELD","PC2","SRF","TER","RGN","FLD","GND","SKY","ENDF","PLT","PCK",NULL
};

static int ArCountNumObj(int *nSrf,int *nTer,int *nPc2,int *nRgn,int *nFld,int *nPlt)
{
	int ac,cmd;
	char str[256],*av[16];
	*nSrf=0;
	*nTer=0;
	*nPc2=0;
	*nRgn=0;
	*nFld=0;
	*nPlt=0;
	while(BiGetStringFunc(str,255)!=NULL)
	{
		SkArguments(&ac,av,15,str);
		if(ac>0 && SkCommandNumber(&cmd,av[0],topCmd)==BI_OK)
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

static int ArMallocObj(BIFLD *fld,int nSrf,int nTer,int nPc2,int nRgn,int nFld,int nPlt)
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

static int ArReadObj(BIFLD *fld,char pth[])
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
	ffd=(BIFLDFLD *)(fld->fld);

	fld->nSrf=0;
	fld->nTer=0;
	fld->nPc2=0;
	fld->nRgn=0;
	fld->nFld=0;
	fld->nPlt=0;
	BiSetColorRGB(&fld->sky,32,180,180);
	BiSetColorRGB(&fld->gnd,32,128,32);

	if(BiGetStringFunc(str,255)!=NULL && strncmp(str,"FIELD",5)==0)
	{
		while(BiGetStringFunc(str,255)!=NULL)
		{
			er=BI_OK;
			SkArguments(&ac,av,15,str);
			if(ac>0 && SkCommandNumber(&cmd,av[0],topCmd)==BI_OK)
			{
				switch(cmd)
				{
				case 1:
					er=ArReadPc2(&pc2[fld->nPc2],pth,fld->nPc2,pc2);
					fld->nPc2++;
					break;
				case 2:
					er=ArReadSrf(&srf[fld->nSrf],pth,fld->nSrf,srf);
					fld->nSrf++;
					break;
				case 3:
					er=ArReadTer(&ter[fld->nTer],pth,fld->nTer,ter);
					fld->nTer++;
					break;
				case 4:
					er=ArReadRgn(&rgn[fld->nRgn]);
					fld->nRgn++;
					break;
				case 5:
					er=ArReadFld(&ffd[fld->nFld],pth,fld->nFld,ffd);
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
					er=ArReadPc2(&plt[fld->nPlt],pth,fld->nPlt,plt);
					fld->nPlt++;
					break;
				case 10:
					if(ArInsertPackedObject(av[1],atoi(av[2]))!=BI_OK)
					{
						return BI_ERR;
					}
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

static int ArReadPc2(BIFLDPC2 *pc2,char pth[],int n,BIFLDPC2 lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	pc2->lodDist=10000000.0F;  /* 10000 km */
	while(BiGetStringFunc(str,255)!=NULL)
	{
		SkArguments(&ac,av,15,str);
		if(ac>0 && SkCommandNumber(&cmd,av[0],filCmd)==BI_OK)
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
					if(ArLoadPc2FromPack(&pc2->pc2,av[1])!=BI_OK)
					{
						sprintf(ful,"%s%s",pth,av[1]);
						if(ArLoadPc2(&pc2->pc2,ful)!=BI_OK)
						{
							return BI_ERR;
						}
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

static int ArReadSrf(BIFLDSRF *srf,char pth[],int n,BIFLDSRF lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	srf->id=0;
	srf->tag[0]=0;
	srf->lodDist=10000000.0F;  /* 10000 km */
	while(BiGetStringFunc(str,255)!=NULL)
	{
		SkArguments(&ac,av,15,str);
		if(ac>0 && SkCommandNumber(&cmd,av[0],filCmd)==BI_OK)
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
					if(ArLoadSrfFromPack(&srf->srf,av[1])!=BI_OK)
					{
						sprintf(ful,"%s%s",pth,av[1]);
						if(ArLoadSrf(&srf->srf,ful)!=BI_OK)
						{
							return BI_ERR;
						}
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

static int ArReadTer(BIFLDTER *ter,char pth[],int n,BIFLDTER lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	ter->id=0;
	ter->tag[0]=0;
	ter->lodDist=10000000.0F;  /* 10000 km */
	while(BiGetStringFunc(str,255)!=NULL)
	{
		SkArguments(&ac,av,15,str);
		if(ac>0 && SkCommandNumber(&cmd,av[0],filCmd)==BI_OK)
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
					if(ArLoadTerFromPack(&ter->ter,av[1])!=BI_OK)
					{
						sprintf(ful,"%s%s",pth,av[1]);
						if(ArLoadTer(&ter->ter,ful)!=BI_OK)
						{
							return BI_ERR;
						}
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

static int ArReadFld(BIFLDFLD *fld,char pth[],int n,BIFLDFLD lst[])
{
	int ac,cmd,i;
	char str[256],ful[256],*av[16];
	fld->lodDist=10000000.0F;  /* 10000 km */
	while(BiGetStringFunc(str,255)!=NULL)
	{
		SkArguments(&ac,av,15,str);
		if(ac>0 && SkCommandNumber(&cmd,av[0],filCmd)==BI_OK)
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
					if(ArLoadFld(&fld->fld,ful)!=BI_OK)
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

static int ArReadRgn(BIFLDRGN *rgn)
{
	int ac,cmd;
	char str[256],*av[16];
	while(BiGetStringFunc(str,255)!=NULL)
	{
		SkArguments(&ac,av,15,str);
		if(ac>0 && SkCommandNumber(&cmd,av[0],rgnCmd)==BI_OK)
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


