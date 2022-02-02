#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>

#include "aurora.h"

#undef MBOX
/* #define MBOX(x) HtMessageBox(x,NULL) */
#define MBOX(x)

extern void BiPushStringFunc(void);
extern void BiPopStringFunc(void);
extern void BiSetFp(FILE *fp);
extern void BiSetStringList(char *(*lst));
extern char *BiGetStringFunc(char *str,int x);

/***************************************************************************/
static int ArLoadMPALoop(ARMPA *mpa,BIFIELD *fld);
static int ArLoadMpaField(ARMPA *mpa,BIFIELD *fld,int ac,char *av[]);
static int ArLoadMpaFixedCam(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaOnBoardCam(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaStaticCam(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaFixedLight(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaOnBoardLight(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaMagnification(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaClock(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaObj(ARMPA *mpa);
static int ArLoadMpaSetShape(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetVirtual(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetObjTime(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetObjInitialPos(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetObjInitialAtt(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetObjInitialSta(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSmokeClass(ARMPA *mpa,int id,int ac,char *av[]);
static int ArLoadMpaSmokeBbx(ARMPA *mpa,int id,int ac,char *av[]);
static int ArLoadMpaSmokeInst(ARMPA *mpa,int id,int ac,char *av[]);
static int ArLoadMpaSmokeOffset(ARMPA *mpa,int id,int ac,char *av[]);
static int ArLoadMpaStatus(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaNode(ARMPA *mpa);
static int ArLoadMpaSetNodeTime(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetFWD(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetACC(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetPRC(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetMTL(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetMTR(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetMTS(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetMSR(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetKRP(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetPRL(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetENG(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetCHS(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetRAW(ARMPA *mpa);
static int ArLoadMpaSetABS(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetHPB(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetPYR(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetKRA(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetFCS(ARMPA *mpa,int ac,char *av[]);
static int ArLoadMpaSetSmoothRotation(ARMPA *mpa,int ac,char *av[]);
/***************************************************************************/



/***************************************************************************/

static char PathMPA[256],FilMPA[256];


int ArLoadMpa(ARMPA *mpa,BIFIELD *fld,char fnOrg[]) /* fld can be NULL */
{
	FILE *fp;
	int er;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		SkSeparatePathFile(PathMPA,FilMPA,fn);

		BiPushStringFunc();
		BiSetFp(fp);
		ArBeginPackedObject();

		er=ArLoadMPALoop(mpa,fld);
		if(er!=BI_OK)
		{
			MBOX("ArLoadMPA");
		}

		ArEndPackedObject();
		BiPopStringFunc();

		fclose(fp);
		return er;
	}
	return BI_ERR;
}

int ArLoadMpaFromString(ARMPA *mpa,BIFIELD *fld,char *lst[])
{
	int er;

	BiPushStringFunc();
	BiSetStringList(lst);
	ArBeginPackedObject();

	er=ArLoadMPALoop(mpa,fld);
	if(er!=BI_OK)
	{
		MBOX("ArLoadMPAFromString");
	}

	ArEndPackedObject();
	BiPopStringFunc();

	return er;
}

static char *topCmd[]=
{
	"REM",
	"FLD",
	"FXC",
	"CAM",
	"MAG",
	"OBJ",
	"CLK",
	"END",
	"FXL",
	"LIG",
	"NAM",
	"PCK",
	"STC",
	NULL
};

static int ArLoadMPALoop(ARMPA *mpa,BIFIELD *fld)
{
	int ac,c,er;
	char str[128],*av[64];
	char msg[128];

	BiGetStringFunc(str,128);
	SkCapitalize(str);
	if(strncmp(str,"MOTIONPATH",10)!=0)
	{
		MBOX("ArLoadMPALoop(1)");
		return BI_ERR;
	}

	if(ArBeginSetPath(mpa)==BI_OK)
	{
		er=BI_OK;
		while(BiGetStringFunc(str,128)!=NULL)
		{
			SkArguments(&ac,av,63,str);
			if(ac>0)
			{
				if(SkCommandNumber(&c,av[0],topCmd)==BI_OK)
				{
					switch(c)
					{
					case 0: /* "REM" */
						break;
					case 1: /* "FLD" */
						er=ArLoadMpaField(mpa,fld,ac,av);
						break;
					case 2: /* "FCS" */
						er=ArLoadMpaFixedCam(mpa,ac,av);
						break;
					case 3: /* "CAM" */
						er=ArLoadMpaOnBoardCam(mpa,ac,av);
						break;
					case 4: /* "MAG" */
						er=ArLoadMpaMagnification(mpa,ac,av);
						break;
					case 5: /* "OBJ" */
						er=ArLoadMpaObj(mpa);
						break;
					case 6: /* "CLK" */
						er=ArLoadMpaClock(mpa,ac,av);
						break;
					case 7: /* "END" */
						return ArEndSetPath(mpa);
					case 8: /* "FXL" */
						er=ArLoadMpaFixedLight(mpa,ac,av);
						break;
					case 9: /* "OBL" */
						er=ArLoadMpaOnBoardLight(mpa,ac,av);
						break;
					case 10: /* "NAM" */
						er=ArPathSetMpaName(mpa,av[1]);
						break;
					case 11: /* "PCK" */
						er=ArInsertPackedObject(av[1],atoi(av[2]));
						break;
					case 12: /* "STC" */
						er=ArLoadMpaStaticCam(mpa,ac,av);
						break;
					}
				}
				else
				{
					sprintf(msg,"ArLoadMPALoop(2)[%s]",av[0]);
					MBOX(msg);
					return BI_ERR;
				}
				if(er!=BI_OK)
				{
					sprintf(str,"ArLoadMPALoop(3)-%d",c);
					MBOX(str);
				}
			}
		}
	}
	MBOX("ArLoadMPALoop(4)");
	return BI_ERR;
}

static int ArLoadMpaField(ARMPA *mpa,BIFIELD *fld,int ac,char *av[])
{
	char ful[512];

	strcpy(mpa->fld,av[1]);

	if(fld!=NULL)
	{
		if(ArLoadFldFromPack(fld,av[1])!=BI_OK)
		{
			sprintf(ful,"%s%s",PathMPA,av[1]);
			if(ArLoadFld(fld,ful)!=BI_OK)
			{
				return BI_ERR;
			}
		}
	}
	return BI_OK;
}

static int ArLoadMpaFixedCam(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT p;
	int id;
	id=atoi(av[1]);
	p.x=(real)atof(av[2]);
	p.y=(real)atof(av[3]);
	p.z=(real)atof(av[4]);
	return ArPathSetFixedCamera(mpa,&p,id);
}

static int ArLoadMpaOnBoardCam(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetOnBoardCamera(mpa,id);
}

static int ArLoadMpaStaticCam(ARMPA *mpa,int ac,char *av[])
{
	BIPOSATT pos;
	pos.p.x=(real)atof(av[1]);
	pos.p.y=(real)atof(av[2]);
	pos.p.z=(real)atof(av[3]);
	pos.a.h=atoi(av[4]);
	pos.a.p=atoi(av[5]);
	pos.a.b=atoi(av[6]);
	return ArPathSetStaticCamera(mpa,&pos);
}

static int ArLoadMpaFixedLight(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT p;
	p.x=(real)atof(av[1]);
	p.y=(real)atof(av[2]);
	p.z=(real)atof(av[3]);
	return ArPathSetFixedLight(mpa,&p);
}

static int ArLoadMpaOnBoardLight(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetOnBoardLight(mpa,id);
}

static int ArLoadMpaMagnification(ARMPA *mpa,int ac,char *av[])
{
	real t,magx,magy;
	t=(real)atof(av[1]);
	magx=(real)atof(av[2]);
	magy=(real)atof(av[3]);
	return ArPathSetMagnification(mpa,t,magx,magy);
}

static int ArLoadMpaClock(ARMPA *mpa,int ac,char *av[])
{
	real clk;
	clk=(real)atof(av[1]);
	return ArPathSetClockCount(mpa,clk);
}


static char *objCmd[]=
{
	"REM",
	"SHP",
	"VIR",
	"TIM",
	"INP",
	"INA",
	"INS",
	"PTH",
	"SC1",
	"SC2",
	"SC3",
	"SC4",
	"SI1",
	"SI2",
	"SI3",
	"SI4",
	"STA",
	"EOO",
	"SB1",
	"SB2",
	"SB3",
	"SB4",
	"DED",
	"SO1",
	"SO2",
	"SO3",
	"SO4",
	NULL
};

static int ArLoadMpaObj(ARMPA *mpa)
{
	int ac,c,er;
	char str[128],*av[32];

	if(ArPathBeginSetObject(mpa)==BI_OK)
	{
		er=BI_OK;
		while(BiGetStringFunc(str,128)!=NULL)
		{
			SkArguments(&ac,av,31,str);
			if(ac>0)
			{
				if(SkCommandNumber(&c,av[0],objCmd)==BI_OK)
				{
					switch(c)
					{
					case 0: /* "REM" */
						break;
					case 1: /* "SHP" */
						er=ArLoadMpaSetShape(mpa,ac,av);
						break;
					case 2: /* "VIR" */
						er=ArLoadMpaSetVirtual(mpa,ac,av);
						break;
					case 3: /* "TIM" */
						er=ArLoadMpaSetObjTime(mpa,ac,av);
						break;
					case 4: /* "INP" */
						er=ArLoadMpaSetObjInitialPos(mpa,ac,av);
						break;
					case 5: /* "INA" */
						er=ArLoadMpaSetObjInitialAtt(mpa,ac,av);
						break;
					case 6: /* "INS" */
						er=ArLoadMpaSetObjInitialSta(mpa,ac,av);
						break;
					case 7: /* "PTH" */
						er=ArLoadMpaNode(mpa);
						break;
					case 8: /* "SC1" */
						er=ArLoadMpaSmokeClass(mpa,0,ac,av);
						break;
					case 9: /* "SC2" */
						er=ArLoadMpaSmokeClass(mpa,1,ac,av);
						break;
					case 10: /* "SC3" */
						er=ArLoadMpaSmokeClass(mpa,2,ac,av);
						break;
					case 11: /* "SC4" */
						er=ArLoadMpaSmokeClass(mpa,3,ac,av);
						break;
					case 12: /* "SI1" */
						er=ArLoadMpaSmokeInst(mpa,0,ac,av);
						break;
					case 13: /* "SI2" */
						er=ArLoadMpaSmokeInst(mpa,1,ac,av);
						break;
					case 14: /* "SI3" */
						er=ArLoadMpaSmokeInst(mpa,2,ac,av);
						break;
					case 15: /* "SI4" */
						er=ArLoadMpaSmokeInst(mpa,3,ac,av);
						break;
					case 16: /* "STA" */
						er=ArLoadMpaStatus(mpa,ac,av);
						break;
					case 17: /* "EOO" */
						return ArPathEndSetObject(mpa);
					case 18: /* "SB1" */
						er=ArLoadMpaSmokeBbx(mpa,0,ac,av);
						break;
					case 19: /* "SB2" */
						er=ArLoadMpaSmokeBbx(mpa,1,ac,av);
						break;
					case 20: /* "SB3" */
						er=ArLoadMpaSmokeBbx(mpa,2,ac,av);
						break;
					case 21: /* "SB4" */
						er=ArLoadMpaSmokeBbx(mpa,3,ac,av);
						break;
					case 22: /* "DED" */
						er=ArPathSetObjectIsDead(mpa);
						break;
					case 23:
						er=ArLoadMpaSmokeOffset(mpa,0,ac,av);
						break;
					case 24:
						er=ArLoadMpaSmokeOffset(mpa,1,ac,av);
						break;
					case 25:
						er=ArLoadMpaSmokeOffset(mpa,2,ac,av);
						break;
					case 26:
						er=ArLoadMpaSmokeOffset(mpa,3,ac,av);
						break;
					}
					if(er!=BI_OK)
					{
						char str[256];
						sprintf(str,"ArLoadMpaObj(1) %d",c);
						MBOX(str);
						return BI_ERR;
					}
				}
				else
				{
					MBOX("ArLoadMpaObj(2)");
					return BI_ERR;
				}
			}
		}
	}
	MBOX("ArLoadMpaObj(3)");
	return BI_ERR;
}

static int ArLoadMpaSetShape(ARMPA *mpa,int ac,char *av[])
{
	char pack[256];

	if(ArGetFirstLineOfPackedObj(pack,av[1])==BI_OK)
	{
		SkCapitalize(pack);
		strcpy(mpa->oBuf[mpa->cObj].shp.fn,av[1]);
		if(strncmp(pack,"SURF",4)==0)
		{
			mpa->oBuf[mpa->cObj].shp.typ=ARMP_SRF;
			return ArLoadSrfFromPack(&mpa->oBuf[mpa->cObj].shp.shp.srf,av[1]);
		}
		else if(strncmp(pack,"DYNAMODEL",9)==0)
		{
			mpa->oBuf[mpa->cObj].shp.typ=ARMP_DNM;
			return ArLoadDnmFromPack(&mpa->oBuf[mpa->cObj].shp.shp.dnm,av[1]);
		}
		else
		{
			return BI_ERR;
		}
	}
	else
	{
		int er;
		char ful[512];
		sprintf(ful,"%s%s",PathMPA,av[1]);

		er=ArPathSetObjectShape(mpa,ful);
		strcpy(mpa->oBuf[mpa->cObj].shp.fn,av[1]);

		return er;
	}
}

static int ArLoadMpaSetVirtual(ARMPA *mpa,int ac,char *av[])
{
	return ArPathSetObjectVirtual(mpa);
}

static int ArLoadMpaSetObjTime(ARMPA *mpa,int ac,char *av[])
{
	real t0,t1;
	t0=(real)atof(av[1]);
	t1=(real)atof(av[2]);
	return ArPathSetObjectTime(mpa,t0,t1);
}

static int ArLoadMpaSetObjInitialPos(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT pos;
	pos.x=(real)atof(av[1]);
	pos.y=(real)atof(av[2]);
	pos.z=(real)atof(av[3]);
	return ArPathSetObjectInitialPosition(mpa,&pos);
}

static int ArLoadMpaSetObjInitialAtt(ARMPA *mpa,int ac,char *av[])
{
	BIATTITUDE att;
	att.h=atoi(av[1]);
	att.p=atoi(av[2]);
	att.b=atoi(av[3]);
	return ArPathSetObjectInitialAttitude(mpa,&att);
}

static int ArLoadMpaSetObjInitialSta(ARMPA *mpa,int ac,char *av[])
{
	int sta;
	sta=atoi(av[1]);
	return ArPathSetObjectInitialStatus(mpa,sta);
}

static int ArLoadMpaSmokeClass(ARMPA *mpa,int id,int ac,char *av[])
{
	int sty;
	real t0,t1,iniw,maxw,dw;
	BICOLOR c;

	sty=atoi(av[1]);
	t0=(real)atof(av[2]);
	t1=(real)atof(av[3]);
	iniw=(real)atof(av[4]);
	maxw=(real)atof(av[5]);
	dw=(real)atof(av[6]);
	c.r=atoi(av[7]);
	c.g=atoi(av[8]);
	c.b=atoi(av[9]);
	return ArPathSetObjectSmokeClass(mpa,id,sty,t0,t1,iniw,maxw,dw,&c);
}

static int ArLoadMpaSmokeOffset(ARMPA *mpa,int id,int ac,char *av[])
{
	BIPOINT ofs;
	BiSetPoint(&ofs,atof(av[1]),atof(av[2]),atof(av[3]));
	return ArPathSetObjSmokeOffset(mpa,id,&ofs);
}

static int ArLoadMpaSmokeBbx(ARMPA *mpa,int id,int ac,char *av[])
{
	int stp[3];
	real d[2];
	BIPOINT bbx[2];

	stp[0]=atoi(av[1]);
	d[0]=(real)atof(av[2]);
	stp[1]=atoi(av[3]);
	d[1]=(real)atof(av[4]);
	stp[2]=atoi(av[5]);

	BiSetPoint(&bbx[0],d[0],d[0],d[0]);
	BiSetPoint(&bbx[1],d[1],d[1],d[1]);

	return ArPathSetObjectSmokeBbox(mpa,id,stp,bbx);
}

static int ArLoadMpaSmokeInst(ARMPA *mpa,int id,int ac,char *av[])
{
	real t0,t1;
	t0=(real)atof(av[1]);
	t1=(real)atof(av[2]);
	return ArPathSetObjectSmokeTime(mpa,id,t0,t1);
}

static int ArLoadMpaStatus(ARMPA *mpa,int ac,char *av[])
{
	real t;
	int s;

	t=(real)atof(av[1]);
	s=atoi(av[2]);
	return ArPathSetObjectStatus(mpa,t,s);
}

static char *nodCmd[]=
{
	"REM",
	"TIM",
	"FWD",
	"ACC",
	"PRC",
	"MTL",
	"MTR",
	"MTS",
	"MSR",
	"KRP",
	"HPB",
	"PYR",
	"KRA",
	"FCS",
	"EON",
	"PRL",
	"ENG",
	"RSB", /* Rotate Smooth Both side */
	"CHS",
	"RAW",
	"ABS",
	NULL
};


static int ArLoadMpaNode(ARMPA *mpa)
{
	int ac,c,er;
	char str[128],*av[32];

	if(ArPathBeginSetNode(mpa)==BI_OK)
	{
		while(BiGetStringFunc(str,128)!=NULL)
		{
			SkArguments(&ac,av,31,str);
			if(ac>0)
			{
				if(SkCommandNumber(&c,av[0],nodCmd)==BI_OK)
				{
					switch(c)
					{
					case 0:/* "REM" */
						break;
					case 1:/* "TIM" */
						er=ArLoadMpaSetNodeTime(mpa,ac,av);
						break;
					case 2:/* "FWD" */
						er=ArLoadMpaSetFWD(mpa,ac,av);
						break;
					case 3:/* "ACC" */
						er=ArLoadMpaSetACC(mpa,ac,av);
						break;
					case 4:/* "PRC" */
						er=ArLoadMpaSetPRC(mpa,ac,av);
						break;
					case 5:/* "MTL" */
						er=ArLoadMpaSetMTL(mpa,ac,av);
						break;
					case 6:/* "MTR" */
						er=ArLoadMpaSetMTR(mpa,ac,av);
						break;
					case 7:/* "MTS" */
						er=ArLoadMpaSetMTS(mpa,ac,av);
						break;
					case 8:/* "MSR" */
						er=ArLoadMpaSetMSR(mpa,ac,av);
						break;
					case 9:/* "KRP" */
						er=ArLoadMpaSetKRP(mpa,ac,av);
						break;
					case 10:/* "HPB" */
						er=ArLoadMpaSetHPB(mpa,ac,av);
						break;
					case 11:/* "PYR" */
						er=ArLoadMpaSetPYR(mpa,ac,av);
						break;
					case 12:/* "KRA" */
						er=ArLoadMpaSetKRA(mpa,ac,av);
						break;
					case 13:/* "FCS" */
						er=ArLoadMpaSetFCS(mpa,ac,av);
						break;

					case 14:/* "EON" */  /* End Of Node */
						return ArPathEndSetNode(mpa);

					case 15:/* "PRL" */
						er=ArLoadMpaSetPRL(mpa,ac,av);
						break;
					case 16:/* "ENG" */
						er=ArLoadMpaSetENG(mpa,ac,av);
						break;

					case 17:/* "RSB" */
						er=ArLoadMpaSetSmoothRotation(mpa,ac,av);
						break;

					case 18:/* "CHS" */
						er=ArLoadMpaSetCHS(mpa,ac,av);
						break;
					case 19:/* "RAW" */
						er=ArLoadMpaSetRAW(mpa);
						break;
					case 20:/* "ABS" */
						er=ArLoadMpaSetABS(mpa,ac,av);
						break;
					}
					if(er!=BI_OK)
					{
						MBOX("ArLoadMpaNode(1)");
					}
				}
				else
				{
					MBOX("ArLoadMpaNode(2)");
					return BI_ERR;
				}
			}
		}
	}
	MBOX("ArLoadMpaNode(3)");
	return BI_ERR;
}

static int ArLoadMpaSetNodeTime(ARMPA *mpa,int ac,char *av[])
{
	real t;
	t=(real)atof(av[1]);
	return ArPathSetNodeTime(mpa,t);
}

static int ArLoadMpaSetFWD(ARMPA *mpa,int ac,char *av[])
{
	real spd;
	spd=(real)atof(av[1]);
	return ArPathSetNodeFWD(mpa,spd);
}

static int ArLoadMpaSetACC(ARMPA *mpa,int ac,char *av[])
{
	real spd1,spd2;
	spd1=(real)atof(av[1]);
	spd2=(real)atof(av[2]);
	return ArPathSetNodeACC(mpa,spd1,spd2);
}

static int ArLoadMpaSetPRC(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT p;
	p.x=(real)atof(av[1]);
	p.y=(real)atof(av[2]);
	p.z=(real)atof(av[3]);
	return ArPathSetNodePRC(mpa,&p);
}

static int ArLoadMpaSetMTL(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT p;
	p.x=(real)atof(av[1]);
	p.y=(real)atof(av[2]);
	p.z=(real)atof(av[3]);
	return ArPathSetNodeMTL(mpa,&p);
}

static int ArLoadMpaSetMTR(ARMPA *mpa,int ac,char *av[])
{
	int id;
	BIPOINT p;
	id=atoi(av[1]);
	p.x=(real)atof(av[2]);
	p.y=(real)atof(av[3]);
	p.z=(real)atof(av[4]);
	return ArPathSetNodeMTR(mpa,id,&p);
}

static int ArLoadMpaSetMTS(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT p;
	p.x=(real)atof(av[1]);
	p.y=(real)atof(av[2]);
	p.z=(real)atof(av[3]);
	return ArPathSetNodeMTS(mpa,&p);
}

static int ArLoadMpaSetMSR(ARMPA *mpa,int ac,char *av[])
{
	int id;
	BIPOINT p;
	id=atoi(av[1]);
	p.x=(real)atof(av[2]);
	p.y=(real)atof(av[3]);
	p.z=(real)atof(av[4]);
	return ArPathSetNodeMSR(mpa,id,&p);
}

static int ArLoadMpaSetKRP(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetNodeKRP(mpa,id);
}

static int ArLoadMpaSetPRL(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetNodePRL(mpa,id);
}

static int ArLoadMpaSetENG(ARMPA *mpa,int ac,char *av[])
{
	real spd1;
	spd1=(real)atof(av[1]);
	return ArPathSetNodeENG(mpa,spd1);
}

static int ArLoadMpaSetCHS(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetNodeCHS(mpa,id);
}

static int ArLoadMpaSetRAW(ARMPA *mpa)
{
	int ac;
	char *av[31];
	char str[256];

	int nNode;
	ARMPRAWDATANODE *node;

	if(BiGetStringFunc(str,128)!=NULL)
	{
		SkArguments(&ac,av,31,str);
		if(ac>0)
		{
			nNode=atoi(av[0]);
			node=(ARMPRAWDATANODE *)BiMalloc(sizeof(ARMPRAWDATANODE)*nNode);
			if(node!=NULL)
			{
				int i,r;
				for(i=0; i<nNode && BiGetStringFunc(str,128)!=NULL; i++)
				{
					SkArguments(&ac,av,31,str);
					if(ac<7)
					{
						BiFree(node);
						return BI_ERR;
					}
					node[i].p.p.x=(real)atof(av[0]);
					node[i].p.p.y=(real)atof(av[1]);
					node[i].p.p.z=(real)atof(av[2]);
					node[i].p.a.h=atoi(av[3]);
					node[i].p.a.p=atoi(av[4]);
					node[i].p.a.b=atoi(av[5]);
					node[i].t=(real)atof(av[6]);
				}
				if(i<nNode)
				{
					BiFree(node);
					return BI_ERR;
				}
				r=ArPathSetNodeRAW(mpa,nNode,node);
				BiFree(node);
				return r;
			}
		}
	}
	return BI_ERR;
}

static int ArLoadMpaSetABS(ARMPA *mpa,int ac,char *av[])
{
	BIPOINT p;
	p.x=(real)atof(av[1]);
	p.y=(real)atof(av[2]);
	p.z=(real)atof(av[3]);
	return ArPathSetNodeABS(mpa,&p);
}

static int ArLoadMpaSetHPB(ARMPA *mpa,int ac,char *av[])
{
	long vh,vp,vb;
	vh=atoi(av[1]);
	vp=atoi(av[2]);
	vb=atoi(av[3]);
	return ArPathSetNodeHPB(mpa,vh,vp,vb);
}

static int ArLoadMpaSetPYR(ARMPA *mpa,int ac,char *av[])
{
	long pit,yaw,rol;
	pit=atoi(av[1]);
	yaw=atoi(av[2]);
	rol=atoi(av[3]);
	return ArPathSetNodePYR(mpa,pit,yaw,rol);
}

static int ArLoadMpaSetKRA(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetNodeKRA(mpa,id);
}

static int ArLoadMpaSetFCS(ARMPA *mpa,int ac,char *av[])
{
	int id;
	id=atoi(av[1]);
	return ArPathSetNodeFCS(mpa,id);
}

static int ArLoadMpaSetSmoothRotation(ARMPA *mpa,int ac,char *av[])
{
	return ArPathSetNodeSmoothRotation(mpa);
}
