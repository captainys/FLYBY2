#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>



extern void BiPushStringFunc(void);
extern void BiPopStringFunc(void);
extern void BiSetFp(FILE *fp);
extern void BiSetStringList(char *(*lst));
extern char *BiGetStringFunc(char *str,int x);

#include "aurora.h"




/***********************************************************************
DYNAMODEL

PCK filename nline

SRF tag
CLA classId
POS x y z h p b d
REL relation [DEPendent|INDependent|depend on TOP]
NST n
STA x y z h p b d
STA x y z h p b d
STA x y z h p b d
FIL fname
CNT x y z
NCH n
CLD tag
CLD tag
CLD tag
END

LIN tag
POS x y z h p b d
REL relation
NST n
STA x y z h p b d
STA x y z h p b d
STA x y z h p b d
VTX x y z x y z
CNT x y z
NCH n
CLD tag
CLD tag
END

PLG tag
POS x y z h p b d
REL relation
NST n
STA x y z h p b d
STA x y z h p b d
STA x y z h p b d
NVT n
VTX x y z
VTX x y z
VTX x y z
VTX x y z
CNT x y z
NCH n
CLD tag
CLD tag
CLD tag
END

END
***********************************************************************/

typedef struct {
	char tag[80];
} ARF_CHILDOBJECT;

typedef struct ArfDynaObjTag {
	int id;
	int classId;
	struct ArfDynaObjTag *par;

	struct ArfDynaObjTag *next;

	char tag[80];

	BIPNTANG pos;
	int dispOrg;

	BIPOINT cnt;
	BICOLOR col;
	int rel;

	int type;
	union {
		BISRFMODEL srf;
		BIPOLYGON plg;
		BILIMLINE lin;
	} uni;

	int nSta;
	int disp[ARM_MAX_N_STATUS];
	BIPNTANG sta[ARM_MAX_N_STATUS];

	int nCh;
	ARF_CHILDOBJECT *ch;
} ARF_DYNAOBJECT;


static int ArLoadDnmMainLoop(ARDNM *neo,char pth[]);
static ARF_DYNAOBJECT *ArNewCmObject(int type,char *tag);
static int ArDmCommonOperation(ARF_DYNAOBJECT *neo,int cmd,int ac,char *av[]);
static ARF_DYNAOBJECT *ArLoadDnmSrf(char *tag,char pth[]);
static ARF_DYNAOBJECT *ArLoadDnmPlg(char *tag);
static ARF_DYNAOBJECT *ArLoadDnmLine(char *tag);
static int ArMakeDnm(ARDNM *neo,ARF_DYNAOBJECT *lst);
static int ArMakeLink(ARF_DYNAOBJECT *ptr,ARF_DYNAOBJECT *lst);
static int ArAppendChainModel(ARDNM *neo,ARF_DYNAOBJECT *lst);
static void ArFreeDynaModel(ARF_DYNAOBJECT *ptr);


int ArExecLoadDnm(ARDNM *neo,char fnOrg[])
{
	FILE *fp;
	int ret;
	char pth[256],fil[256];
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	SkSeparatePathFile(pth,fil,fn);

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		ArBeginPackedObject();
		BiPushStringFunc();
		BiSetFp(fp);

		ret=ArLoadDnmMainLoop(neo,pth);

		BiPopStringFunc();
		ArEndPackedObject();

		fclose(fp);
		return ret;
	}
	MBOX("ArLoadDnm(1)");
	return BI_ERR;
}

int ArExecLoadDnmFromString(ARDNM *dnm,char *lst[])
{
	int ret;

	ArBeginPackedObject();
	BiPushStringFunc();
	BiSetStringList(lst);

	ret=ArLoadDnmMainLoop(dnm,".");

	BiPopStringFunc();
	ArEndPackedObject();

	if(ret!=BI_OK)
	{
		MBOX("ArLoadDnmFromString(1)");
	}
	return ret;
}

static char *topCmd[]=
{
	"END",
	"SRF",
	"PLG",
	"LIN",
	"PCK",
	NULL
};

static int ArLoadDnmMainLoop(ARDNM *mdl,char pth[])
{
	int ac,cmd;
	char str[1024],*av[256];
	ARF_DYNAOBJECT *lst,*neo;


	if(BiGetStringFunc(str,1024)!=NULL)
	{
		SkCapitalize(str);
		if(strncmp(str,"DYNAMODEL",9)!=0)
		{
			return BI_ERR;
		}

		lst=NULL;
		while(BiGetStringFunc(str,1024)!=NULL)
		{
			SkArguments(&ac,av,255,str);
			if(ac>0 && SkCommandNumber(&cmd,av[0],topCmd)==BI_OK)
			{
				switch(cmd)
				{
				case 0:
					MBOX("ArLoadDnmMainLoop(1)");
					return ArMakeDnm(mdl,lst);
				case 1:
					neo=ArLoadDnmSrf(av[1],pth);
					if(neo!=NULL)
					{
						neo->next=lst;
						lst=neo;
					}
					else
					{
						MBOX("ArLoadDnmMainLoop(2)");
						return BI_ERR;
					}
					break;
				case 2:
					neo=ArLoadDnmPlg(av[1]);
					if(neo!=NULL)
					{
						neo->next=lst;
						lst=neo;
					}
					else
					{
						MBOX("ArLoadDnmMainLoop(3)");
						return BI_ERR;
					}
					break;
				case 3:
					neo=ArLoadDnmLine(av[1]);
					if(neo!=NULL)
					{
						neo->next=lst;
						lst=neo;
					}
					else
					{
						MBOX("ArLoadDnmMainLoop(4)");
						return BI_ERR;
					}
					break;
				case 4:
					if(ArInsertPackedObject(av[1],atoi(av[2]))!=BI_OK)
					{
						MBOX("ArLoadDnmMainLoop(5)");
						return BI_ERR;
					}
					break;
				default:
					MBOX("ArLoadDnmMainLoop(6)");
					return BI_ERR;
				}
			}
		}
	}
	MBOX("ArLoadDnmMainLoop(7)");
	return BI_ERR;
}

#define COMCMDS "END","CNT","NCH","CLD","COL","REL","NST","STA","POS","CLA"

static ARF_DYNAOBJECT *ArNewCmObject(int type,char *tag)
{
	ARF_DYNAOBJECT *neo;
	neo=BiMalloc(sizeof(ARF_DYNAOBJECT));
	if(neo!=NULL)
	{
		neo->id=0;
		neo->classId=0;
		neo->nSta=0;
		neo->pos=BiOrgPA;
		neo->dispOrg=BI_ON;
		neo->rel=ARM_DEPENDENT;
		neo->par=NULL;
		neo->next=NULL;
		strcpy(neo->tag,tag);
		neo->type=type;
		neo->nCh=0;
		neo->ch=NULL;
		neo->col=BiWhite;
	}
	return neo;
}

static char *relCmd[]=
{
	"DEP","IND","TOP",NULL
};

static int relTab[]=
{
	ARM_DEPENDENT,ARM_INDEPENDENT,ARM_DEPENDONTOP
};

static int ArDmCommonOperation(ARF_DYNAOBJECT *neo,int cmd,int ac,char *av[])
{
	int nCh,nSt,rel;
	switch(cmd)
	{
	case 1: /* CNT */
		BiSetPoint(&neo->cnt,atof(av[1]),atof(av[2]),atof(av[3]));
		return BI_OK;
	case 2: /* NCH */
		nCh=atoi(av[1]);
		neo->nCh=0;
		neo->ch=(ARF_CHILDOBJECT *)BiMalloc(nCh*sizeof(ARF_CHILDOBJECT));
		if(nCh>0 && neo->ch==NULL)  /* Modified 1996/04/02 */
		{
			/* MessageBox(NULL,"4",NULL,MB_APPLMODAL); */
			return BI_ERR;
		}
		return BI_OK;
	case 3: /* CLD */
		nCh=neo->nCh;
		strcpy(neo->ch[nCh].tag,av[1]);
		neo->nCh++;
		return BI_OK;
	case 4: /* COL */
		BiSetColorRGB(&neo->col,atoi(av[1]),atoi(av[2]),atoi(av[3]));
		return BI_OK;
	case 5: /* REL */
		if(SkCommandNumber(&rel,av[1],relCmd)==BI_OK)
		{
			neo->rel=relTab[rel];
			return BI_OK;
		}
		return BI_ERR;
	case 6: /* NST */
		return BI_OK;
	case 7: /* STA */
		nSt=neo->nSta;
		BiSetPoint(&neo->sta[nSt].p,atof(av[1]),atof(av[2]),atof(av[3]));
		BiSetAngle(&neo->sta[nSt].a,atoi(av[4]),atoi(av[5]),atoi(av[6]));
		neo->disp[nSt]=(ac>=8 ? atoi(av[7]) : BI_ON);
		neo->nSta++;
		return BI_OK;
	case 8: /* POS */
		BiSetPoint(&neo->pos.p,atof(av[1]),atof(av[2]),atof(av[3]));
		BiSetAngle(&neo->pos.a,atoi(av[4]),atoi(av[5]),atoi(av[6]));
		neo->dispOrg=(ac>=8 ? atoi(av[7]) : BI_ON);
		return BI_OK;
	case 9: /* CLA */
		neo->classId=atoi(av[1]);
		return BI_OK;
	}
	/* MessageBox(NULL,"5",NULL,MB_APPLMODAL); */
	return BI_ERR;
}

static char *srfCmd[]=
{
	COMCMDS,"FIL",NULL
};

static ARF_DYNAOBJECT *ArLoadDnmSrf(char *tag,char pth[])
{
	int ac,cmd;
	char str[1024],ful[256],*av[256];
	ARF_DYNAOBJECT *neo;
	neo=ArNewCmObject(ARM_SRFMODEL,tag);
	if(neo!=NULL)
	{
		neo->uni.srf.nv=0;   // 2002/12/19
		neo->uni.srf.v=NULL; // 2002/12/19
		neo->uni.srf.np=0;   // 2002/12/19
		neo->uni.srf.p=NULL; // 2002/12/19
		while(BiGetStringFunc(str,1024)!=NULL)
		{
			SkArguments(&ac,av,255,str);
			if(ac>0 && SkCommandNumber(&cmd,av[0],srfCmd)==BI_OK)
			{
				switch(cmd)
				{
				case 0: /* END */
					return neo;
				case 1: /* CNT */
				case 2: /* NCH */
				case 3: /* CLD */
				case 4: /* COL */
				case 5: /* REL */
				case 6: /* NST */
				case 7: /* STA */
				case 8: /* POS */
				case 9: /* CLA */
					if(ArDmCommonOperation(neo,cmd,ac,av)!=BI_OK)
					{
						MBOX("ArLoadDnmSrf(1)");
						return NULL;
					}
					break;
				case 10: /* FIL */
					if(ArLoadSrfFromPack(&neo->uni.srf,av[1])!=BI_OK)
					{
						sprintf(ful,"%s%s",pth,av[1]);
						if(ArLoadSrf(&neo->uni.srf,ful)!=BI_OK)
						{
							MBOX("ArLoadDnmSrf(2)");
							return NULL;
						}
					}
					break;
				}
			}
		}
		MBOX("ArLoadDnmSrf(3)");
	}
	MBOX("ArLoadDnmSrf(4)");
	return NULL;
}



static char *plgCmd[]=
{
	COMCMDS,"NVT","VTX",NULL
};

static ARF_DYNAOBJECT *ArLoadDnmPlg(char *tag)
{
	int ac,cmd,vt;
	char str[1024],*av[256];
	ARF_DYNAOBJECT *neo;
	neo=ArNewCmObject(ARM_POLYGON,tag);
	if(neo!=NULL)
	{
		vt=0;
		while(BiGetStringFunc(str,1024)!=NULL)
		{
			SkArguments(&ac,av,255,str);
			if(ac>0 && SkCommandNumber(&cmd,av[0],plgCmd)==BI_OK)
			{
				switch(cmd)
				{
				case 0: /* END */
					return neo;
				case 1: /* CNT */
				case 2: /* NCH */
				case 3: /* CLD */
				case 4: /* COL */
				case 5: /* REL */
				case 6: /* NST */
				case 7: /* STA */
				case 8: /* POS */
				case 9: /* CLA */
					if(ArDmCommonOperation(neo,cmd,ac,av)!=BI_OK)
					{
						return NULL;
					}
					break;
				case 10: /* NVT */
					neo->uni.plg.np=atoi(av[1]);
					neo->uni.plg.p=(BIPOINT *)BiMalloc(neo->uni.plg.np*sizeof(BIPOINT));
					if(neo->uni.plg.p==NULL)
					{
						return NULL;
					}
					break;
				case 11: /* VTX */
					BiSetPoint(&neo->uni.plg.p[vt],atof(av[1]),atof(av[2]),atof(av[3]));
					vt++;
					break;
				}
			}
		}
	}
	return neo;
}



static char *linCmd[]=
{
	COMCMDS,"VTX",NULL
};

static ARF_DYNAOBJECT *ArLoadDnmLine(char *tag)
{
	int ac,cmd;
	char str[1024],*av[256];
	ARF_DYNAOBJECT *neo;
	neo=ArNewCmObject(ARM_LINE,tag);
	if(neo!=NULL)
	{
		while(BiGetStringFunc(str,1024)!=NULL)
		{
			SkArguments(&ac,av,255,str);
			if(ac>0 && SkCommandNumber(&cmd,av[0],linCmd)==BI_OK)
			{
				switch(cmd)
				{
				case 0: /* END */
					return neo;
				case 1: /* CNT */
				case 2: /* NCH */
				case 3: /* CLD */
				case 4: /* COL */
				case 5: /* REL */
				case 6: /* NST */
				case 7: /* STA */
				case 8: /* POS */
				case 9: /* CLA */
					if(ArDmCommonOperation(neo,cmd,ac,av)!=BI_OK)
					{
						return NULL;
					}
					break;
				case 10:
					BiSetPoint(&neo->uni.lin.p1,atof(av[1]),atof(av[2]),atof(av[3]));
					BiSetPoint(&neo->uni.lin.p2,atof(av[4]),atof(av[5]),atof(av[6]));
					break;
				}
			}
		}
	}
	return neo;
}

static int ArMakeDnm(ARDNM *neo,ARF_DYNAOBJECT *lst)
{
	if(ArInitDnm(neo)==BI_OK)
	{
		if(ArMakeLink(lst,lst)==BI_OK && ArAppendChainModel(neo,lst)==BI_OK)
		{
			ArFreeDynaModel(lst);
			return BI_OK;
		}
	}
	/* MessageBox(NULL,"6",NULL,MB_APPLMODAL); */
	return BI_ERR;
}

static int ArMakeLink(ARF_DYNAOBJECT *ptr,ARF_DYNAOBJECT *lst)
{
	int i;
	ARF_DYNAOBJECT *seek;

	if(ptr!=NULL)
	{
		for(i=0; i<ptr->nCh; i++)
		{
			for(seek=lst; seek!=NULL; seek=seek->next)
			{
				if(strcmp(ptr->ch[i].tag,seek->tag)==0)
				{
					seek->par=ptr;
					break;
				}
			}
			if(seek==NULL)
			{
				/* MessageBox(NULL,"7",NULL,MB_APPLMODAL); */
				return BI_ERR;
			}
		}

		return ArMakeLink(ptr->next,lst);
	}
	return BI_OK;
}

static int ArAppendChainModel(ARDNM *neo,ARF_DYNAOBJECT *lst)
{
	int parId,i;
	if(lst!=NULL)
	{
		if(ArAppendChainModel(neo,lst->next)!=BI_OK)
		{
			/* MessageBox(NULL,"8",NULL,MB_APPLMODAL); */
			return BI_ERR;
		}

		parId=(lst->par!=NULL ? lst->par->id : 0);

		switch(lst->type)
		{
		case ARM_SRFMODEL:
			ArAddDnmSrf(&lst->id,neo,&lst->uni.srf,parId,&lst->pos,&lst->cnt);
			break;
		case ARM_POLYGON:
			ArAddDnmPolyg(&lst->id,neo,&lst->uni.plg,&lst->col,parId,&lst->pos,&lst->cnt);
			break;
		case ARM_LINE:
			ArAddDnmLine(&lst->id,neo,&lst->uni.lin,&lst->col,parId,&lst->pos,&lst->cnt);
			break;
		default:
			return BI_ERR;
		}

		ArSetDnmDrawSwitch(neo,lst->id,lst->dispOrg);
		ArSetDnmRelation(neo,lst->id,lst->rel);
		ArSetDnmClassId(neo,lst->id,lst->classId);

		for(i=0; i<lst->nSta; i++)
		{
			ArAddDnmStatus(NULL,neo,lst->id,lst->disp[i],&lst->sta[i]);
		}
	}
	return BI_OK;
}

static void ArFreeDynaModel(ARF_DYNAOBJECT *ptr)
{
	if(ptr!=NULL)
	{
		ArFreeDynaModel(ptr->next);
		BiFree(ptr->ch);
		BiFree(ptr);
	}
}
