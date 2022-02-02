#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>
#include "aurora.h"


#ifdef WIN32
#include <windows.h>
#endif


#define MDB BiFree(BiMalloc(256));

/***************************************************************************/
static int ArGetPathObjPosition(BIPOSATT *p,ARMPA *mpa,int id,long t);
static int ArGetPathObjPositionSmooth(BIPOSATT *p,ARMPA *mpa,int id,real t);
static real ArCalcPathNodeSmoother(real t);
static int ArCalcPathNodeFWD(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeACC(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodePRC(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeMTL(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeMTR(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeMTS(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeMSR(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeKRP(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeHPB(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodePYR(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeKRA(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNodeFCS(ARMPA *mpa,int id,int n,long t,BIPOSATT *p);
static int ArCalcPathNode(ARMPA *mpa,int id,int n,int antiRecursive);
static int ArCalcPathObj(ARMPA *mpa,int id,long eTim,int antiRecursive);
static void ArCalcPathInit(ARMPA *mpa);
static int ArCalcPathMain(ARMPA *mpa);
static int ArMallocPath(ARMPA *mpa);
static int ArSetPathObjSmoke(ARMPOBJECT *obj,ARMPA *mpa,real ct);
static int ArSetPathObjStatus(ARMPOBJECT *obj,ARMPA *mpa,real ct);
/***************************************************************************/



/***************************************************************************/

int ArBeginSetPath(ARMPA *mpa)
{
	mpa->nam[0]=0;

	mpa->fld[0]=0;

	mpa->cam[0].typ=ARMP_NOTSPECIFIED;
	mpa->lig.typ=ARMP_NOLIGHT;
	mpa->clk=0.03F;
	mpa->nCam=0;
	mpa->nObj=0;
	mpa->obj=0;

	mpa->cNod=0;
	mpa->nBuf=(ARMPNODE *)BiMalloc(sizeof(ARMPNODE)*ARP_MAX_NUM_NODE);
	mpa->cObj=0;
	mpa->oBuf=(ARMPOBJECT *)BiMalloc(sizeof(ARMPOBJECT)*ARP_MAX_NUM_OBJ);

	if(mpa->nBuf!=NULL && mpa->oBuf!=NULL)
	{
		return BI_OK;
	}
	else
	{
		MBOX("ArBeginSetPath");
		return BI_ERR;
	}
}

int ArPathSetMpaName(ARMPA *mpa,char name[])
{
	strncpy(mpa->nam,name,sizeof(mpa->nam)-1);
	return BI_OK;
}

int ArPathSetFixedCamera(ARMPA *mpa,BIPOINT *pos,int id)
{
	ARMPMAGNIFY *mag;

	if(mpa->nCam<ARP_MAX_NUM_CAMERA)
	{
		mag=(ARMPMAGNIFY *)BiMalloc(sizeof(ARMPMAGNIFY)*ARP_MAX_NUM_MAG);
		if(mag!=NULL)
		{
			mpa->cam[mpa->nCam].typ=ARMP_FIXEDCAM;
			mpa->cam[mpa->nCam].atr.fxc.id=id;
			mpa->cam[mpa->nCam].atr.fxc.pos=*pos;
			mpa->cam[mpa->nCam].nMag=0;
			mpa->cam[mpa->nCam].mag=mag;
			mpa->nCam++;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int ArPathSetOnBoardCamera(ARMPA *mpa,int id)
{
	ARMPMAGNIFY *mag;

	if(mpa->nCam<ARP_MAX_NUM_CAMERA)
	{
		mag=(ARMPMAGNIFY *)BiMalloc(sizeof(ARMPMAGNIFY)*ARP_MAX_NUM_MAG);
		if(mag!=NULL)
		{
			mpa->cam[mpa->nCam].typ=ARMP_ONBOARDCAM;
			mpa->cam[mpa->nCam].atr.obc.id=id;
			mpa->cam[mpa->nCam].nMag=0;
			mpa->cam[mpa->nCam].mag=mag;
			mpa->nCam++;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int ArPathSetStaticCamera(ARMPA *mpa,BIPOSATT *pos)
{
	ARMPMAGNIFY *mag;

	if(mpa->nCam<ARP_MAX_NUM_CAMERA)
	{
		mag=(ARMPMAGNIFY *)BiMalloc(sizeof(ARMPMAGNIFY)*ARP_MAX_NUM_MAG);
		if(mag!=NULL)
		{
			mpa->cam[mpa->nCam].typ=ARMP_STATICCAM;
			mpa->cam[mpa->nCam].atr.stc.pos=*pos;
			mpa->cam[mpa->nCam].nMag=0;
			mpa->cam[mpa->nCam].mag=mag;
			mpa->nCam++;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int ArPathSetFixedLight(ARMPA *mpa,BIPOINT *pos)
{
	mpa->lig.typ=ARMP_FIXEDLIGHT;
	mpa->lig.atr.fxl.pos=*pos;
	return BI_OK;
}

int ArPathSetOnBoardLight(ARMPA *mpa,int id)
{
	mpa->lig.typ=ARMP_ONBOARDLIGHT;
	mpa->lig.atr.obl.id=id;
	return BI_OK;
}

int ArPathSetMagnification(ARMPA *mpa,real t,real magx,real magy)
{
	int i,j;
	ARMPCAMERA *cam;

	if(mpa->nCam>0)
	{
		cam=&mpa->cam[mpa->nCam-1];
		if(cam->nMag<ARP_MAX_NUM_MAG)
		{
			if(cam->nMag==0)
			{
				cam->mag[0].t=t;
				cam->mag[0].magx=magx;
				cam->mag[0].magy=magy;
				cam->nMag=1;
				return BI_OK;
			}

			for(i=0; i<=cam->nMag; i++)
			{
				if(i==cam->nMag || t<=cam->mag[i].t)
				{
					for(j=cam->nMag; j>i; j--)
					{
						cam->mag[j]=cam->mag[j-1];
					}
					cam->mag[i].t=t;
					cam->mag[i].magx=magx;
					cam->mag[i].magy=magy;
					cam->nMag++;
					return BI_OK;
				}
			}
		}
	}
	return BI_ERR;
}

int ArPathSetClockCount(ARMPA *mpa,real clk)
{
	mpa->clk=clk;
	return BI_OK;
}

int ArEndSetPath(ARMPA *mpa)
{
	int i;
	mpa->nObj=mpa->cObj;
	mpa->obj=(ARMPOBJECT *)BiMalloc(sizeof(ARMPOBJECT)*mpa->cObj);
	if(mpa->obj!=NULL)
	{
		for(i=0; i<mpa->cObj; i++)
		{
			mpa->obj[i]=mpa->oBuf[i];
		}
		BiFree(mpa->nBuf);
		BiFree(mpa->oBuf);
		return BI_OK;
	}
	MBOX("ArEndSetPath");
	return BI_ERR;
}


/***************************************************************************/

int ArPathBeginSetObject(ARMPA *mpa)
{
	int i;
	mpa->oBuf[mpa->cObj].alive=BI_ON;
	mpa->oBuf[mpa->cObj].t0=ARP_DEFAULT_T0;
	mpa->oBuf[mpa->cObj].t1=ARP_DEFAULT_T1;
	mpa->oBuf[mpa->cObj].shp.typ=ARMP_VIRTUAL;
	mpa->oBuf[mpa->cObj].iniP=BiOrgPA;
	mpa->oBuf[mpa->cObj].iniS=0;
	mpa->oBuf[mpa->cObj].nNod=0;
	mpa->oBuf[mpa->cObj].nod=NULL;
	mpa->oBuf[mpa->cObj].nPath=0;
	mpa->oBuf[mpa->cObj].path=NULL;
	mpa->oBuf[mpa->cObj].nSta=0;
	for(i=0; i<4; i++)
	{
		mpa->oBuf[mpa->cObj].smk[i].nSmk=0;
		mpa->oBuf[mpa->cObj].smk[i].alive=BI_OFF;
		mpa->oBuf[mpa->cObj].pSmokeExist[i]=BI_OFF;
	}
	mpa->cNod=0;
	return BI_OK;
}

int ArPathSetObjectIsDead(ARMPA *mpa)
{
	mpa->oBuf[mpa->cObj].alive=BI_OFF;
	return BI_OK;

}

int ArPathSetObjectShape(ARMPA *mpa,char fnOrg[])
{
	char crc[128],fn[256];
	FILE *fp;

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp!=NULL && fgets(crc,128,fp)!=NULL)
	{
		fclose(fp);
		SkCapitalize(crc);
		strcpy(mpa->oBuf[mpa->cObj].shp.fn,fn);
		if(strncmp(crc,"SURF",4)==0)
		{
			mpa->oBuf[mpa->cObj].shp.typ=ARMP_SRF;
			return ArLoadSrf(&mpa->oBuf[mpa->cObj].shp.shp.srf,fn);
		}
		else if(strncmp(crc,"DYNAMODEL",9)==0)
		{
			mpa->oBuf[mpa->cObj].shp.typ=ARMP_DNM;
			return ArLoadDnm(&mpa->oBuf[mpa->cObj].shp.shp.dnm,fn);
		}
	}
	MBOX("ArPathSetObjectShape");
	return BI_ERR;
}

int ArPathSetObjectVirtual(ARMPA *mpa)
{
	mpa->oBuf[mpa->cObj].shp.typ=ARMP_VIRTUAL;
	return BI_OK;
}

int ArPathSetObjectTime(ARMPA *mpa,real t0,real t1)
{
	mpa->oBuf[mpa->cObj].t0=(long)(t0/mpa->clk);
	mpa->oBuf[mpa->cObj].t1=(long)(t1/mpa->clk);
	return BI_OK;
}

int ArPathSetObjectInitialPosition(ARMPA *mpa,BIPOINT *pos)
{
	mpa->oBuf[mpa->cObj].iniP.p=*pos;
	return BI_OK;
}

int ArPathSetObjectInitialAttitude(ARMPA *mpa,BIATTITUDE *att)
{
	mpa->oBuf[mpa->cObj].iniP.a=*att;
	return BI_OK;
}

int ArPathSetObjectInitialStatus(ARMPA *mpa,int sta)
{
	mpa->oBuf[mpa->cObj].iniS=sta;
	return BI_OK;
}

int ArPathSetObjectSmokeClass(ARMPA *mpa,int id,int sty,real t0,real t1,real iw,real ew,real dw,BICOLOR *c)
{
	ARMPSMOKE *smk;
	if(0<=id && id<4)
	{
		smk=&mpa->oBuf[mpa->cObj].smk[id];
		smk->alive=BI_ON;
		smk->sty=sty;
		smk->t0=t0;
		smk->t1=t1;
		smk->iniw=iw;
		smk->maxw=ew;
		smk->dw=dw;
		smk->inic=*c;
		smk->endc=*c;
		smk->tc=1.0F;

		smk->stp[0]=1;
		BiSetPoint(&smk->bbx[0],100,100,100);
		smk->stp[1]=2;
		BiSetPoint(&smk->bbx[1],500,500,500);
		smk->stp[2]=3;

		smk->gen=BiOrgP;

		return BI_OK;
	}
	MBOX("ArPathSetObjectSmokeClass");
	return BI_ERR;
}

int ArPathSetObjectSmokeBbox(ARMPA *mpa,int id,int stp[3],BIPOINT bbx[2])
{
	ARMPSMOKE *smk;
	if(0<=id && id<4)
	{
		smk=&mpa->oBuf[mpa->cObj].smk[id];
		smk->stp[0]=stp[0];
		smk->stp[1]=stp[1];
		smk->stp[2]=stp[2];
		smk->bbx[0]=bbx[0];
		smk->bbx[1]=bbx[1];
		return BI_OK;
	}
	return BI_ERR;
}

int ArPathSetObjSmokeOffset(ARMPA *mpa,int id,BIPOINT *ofs)
{
	ARMPSMOKE *smk;
	if(0<=id && id<4)
	{
		smk=&mpa->oBuf[mpa->cObj].smk[id];
		smk->gen=*ofs;
		return BI_OK;
	}
	return BI_ERR;
}

int ArPathSetObjectSmokeTime(ARMPA *mpa,int id,real t0,real t1)
{
	int i,j;
	int nSmk;
	ARMPSMOKETIME *smk;
	real t;

	if(t0>t1)
	{
		t=t0;
		t0=t1;
		t1=t;
	}

	if(0<=id && id<4)
	{
		nSmk=mpa->oBuf[mpa->cObj].smk[id].nSmk;
		smk=mpa->oBuf[mpa->cObj].smk[id].smk;
		if(nSmk<ARP_MAX_NUM_SMOKE && t0<t1)
		{
			if(nSmk==0 || t1<=smk[0].t0)
			{
				for(j=nSmk; j>0; j--)
				{
					smk[j]=smk[j-1];
				}
				mpa->oBuf[mpa->cObj].smk[id].nSmk++;
				smk[0].t0=t0;
				smk[0].t1=t1;
				return BI_OK;
			}

			for(i=0; i<nSmk; i++)
			{
				if(smk[i].t1<=t0)
				{
					for(j=nSmk; j>i; j--)
					{
						smk[j]=smk[j-1];
					}
					mpa->oBuf[mpa->cObj].smk[id].nSmk++;
					smk[i].t0=t0;
					smk[i].t1=t1;
					return BI_OK;
				}
			}

			smk[nSmk].t0=t0;
			smk[nSmk].t1=t1;
			mpa->oBuf[mpa->cObj].smk[id].nSmk++;
			return BI_OK;
		}
	}
	MBOX("ArPathSetObjectSmokeTime");
	return BI_ERR;
}

int ArPathSetObjectStatus(ARMPA *mpa,real t,int s)
{
	int i,j;
	int nSta;
	ARMPDYNASTA *sta;
	nSta=mpa->oBuf[mpa->cObj].nSta;
	sta=mpa->oBuf[mpa->cObj].sta;
	if(nSta<ARP_MAX_NUM_STATUS)
	{
		if(nSta==0)
		{
			sta[0].t=t;
			sta[0].s=s;
			mpa->oBuf[mpa->cObj].nSta=1;
			return BI_OK;
		}

		for(i=0; i<=nSta; i++)
		{
			if(i==nSta || t<=sta[i].t)
			{
				for(j=nSta; j>i; j--)
				{
					sta[j]=sta[j-1];
				}
				sta[i].t=t;
				sta[i].s=s;
				mpa->oBuf[mpa->cObj].nSta++;
				return BI_OK;
			}
		}
	}
	return BI_OK;
}

int ArPathEndSetObject(ARMPA *mpa)
{
	int i;
	/* long t; */
	mpa->oBuf[mpa->cObj].nod=(ARMPNODE *)BiMalloc(sizeof(ARMPNODE)*mpa->cNod);
	if(mpa->oBuf[mpa->cObj].nod!=NULL || mpa->cNod<=0)
	{
		mpa->oBuf[mpa->cObj].nNod=mpa->cNod;
		/* t=mpa->oBuf[mpa->cObj].t0; */
		for(i=0; i<mpa->cNod; i++)
		{
			mpa->oBuf[mpa->cObj].nod[i]=mpa->nBuf[i];
		/* 	mpa->oBuf[mpa->cObj].nod[i].t0=t; */
		/* 	t+=mpa->oBuf[mpa->cObj].nod[i].t; */
		/* 	mpa->oBuf[mpa->cObj].nod[i].t1=t; */
		}
		mpa->cObj++;
		return BI_OK;
	}
	MBOX("ArPathEndSetObject");
	return BI_ERR;
}

/***************************************************************************/

int ArPathBeginSetNode(ARMPA *mpa)
{
	mpa->nBuf[mpa->cNod].t=(long)(1.0F/mpa->clk);
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_FORWARD;
	mpa->nBuf[mpa->cNod].mot.mot.fwd.spd=100.0F;
	mpa->nBuf[mpa->cNod].rot.sty=ARMP_HPB;
	mpa->nBuf[mpa->cNod].rot.smooth=ARMP_NOSMOOTH;
	mpa->nBuf[mpa->cNod].rot.rot.hpb.att=BiOrgA;
	return BI_OK;
}

int ArPathSetNodeTime(ARMPA *mpa,real t)
{
	mpa->nBuf[mpa->cNod].t=(long)((t+(mpa->clk/10.0F))/mpa->clk);
	return BI_OK;
}

int ArPathSetNodeFWD(ARMPA *mpa,real spd)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_FORWARD;
	mpa->nBuf[mpa->cNod].mot.mot.fwd.spd=spd;
	return BI_OK;
}

int ArPathSetNodeACC(ARMPA *mpa,real spd1,real spd2)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_ACCEL;
	mpa->nBuf[mpa->cNod].mot.mot.acc.spd1=spd1;
	mpa->nBuf[mpa->cNod].mot.mot.acc.spd2=spd2;
	return BI_OK;
}

int ArPathSetNodePRC(ARMPA *mpa,BIPOINT *v)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_PROCEED;
	mpa->nBuf[mpa->cNod].mot.mot.prc.v=*v;
	return BI_OK;
}

int ArPathSetNodeMTL(ARMPA *mpa,BIPOINT *p)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_MOVETO;
	mpa->nBuf[mpa->cNod].mot.mot.mtl.p=*p;
	return BI_OK;
}

int ArPathSetNodeMTR(ARMPA *mpa,int par,BIPOINT *p)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_MOVETOREL;
	mpa->nBuf[mpa->cNod].mot.mot.mtr.id=par;
	mpa->nBuf[mpa->cNod].mot.mot.mtr.p=*p;
	return BI_OK;
}

int ArPathSetNodeMTS(ARMPA *mpa,BIPOINT *p)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_MOVETOSMO;
	mpa->nBuf[mpa->cNod].mot.mot.mtl.p=*p;
	return BI_OK;
}

int ArPathSetNodeMSR(ARMPA *mpa,int par,BIPOINT *p)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_MOVETOSMOREL;
	mpa->nBuf[mpa->cNod].mot.mot.mtr.id=par;
	mpa->nBuf[mpa->cNod].mot.mot.mtr.p=*p;
	return BI_OK;
}

int ArPathSetNodeKRP(ARMPA *mpa,int par)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_KEEPRELPOS;
	mpa->nBuf[mpa->cNod].mot.mot.krp.id=par;
	return BI_OK;
}

int ArPathSetNodePRL(ARMPA *mpa,int par)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_PARALLEL;
	mpa->nBuf[mpa->cNod].mot.mot.prl.id=par;
	return BI_OK;
}

int ArPathSetNodeENG(ARMPA *mpa,real spd1)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_CONSTENERGY;
	mpa->nBuf[mpa->cNod].mot.mot.eng.spd1=spd1;
	return BI_OK;
}

int ArPathSetNodeCHS(ARMPA *mpa,int par)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_CHASE;
	mpa->nBuf[mpa->cNod].mot.mot.chs.id=par;
	return BI_OK;
}

int ArPathSetNodeRAW(ARMPA *mpa,int nNod,ARMPRAWDATANODE *nod)
{
	ARMPRAWDATANODE *neo;
	neo=(ARMPRAWDATANODE *)BiMalloc(sizeof(ARMPRAWDATANODE)*nNod);
	if(neo!=NULL)
	{
		int i;
		for(i=0; i<nNod; i++)
		{
			neo[i]=nod[i];
		}
		mpa->nBuf[mpa->cNod].mot.sty=ARMP_RAWDATA;
		mpa->nBuf[mpa->cNod].mot.mot.raw.nNode=nNod;
		mpa->nBuf[mpa->cNod].mot.mot.raw.node=neo;
		return BI_OK;
	}
}

int ArPathSetNodeABS(ARMPA *mpa,BIPOINT *p)
{
	mpa->nBuf[mpa->cNod].mot.sty=ARMP_MOVETOABS;
	mpa->nBuf[mpa->cNod].mot.mot.mtl.p=*p;
	return BI_OK;
}

int ArPathSetNodeHPB(ARMPA *mpa,long vh,long vp,long vb)
{
	mpa->nBuf[mpa->cNod].rot.sty=ARMP_HPB;
	mpa->nBuf[mpa->cNod].rot.rot.hpb.att.h=vh;
	mpa->nBuf[mpa->cNod].rot.rot.hpb.att.p=vp;
	mpa->nBuf[mpa->cNod].rot.rot.hpb.att.b=vb;
	return BI_OK;
}

int ArPathSetNodePYR(ARMPA *mpa,long vp,long vy,long vr)
{
	mpa->nBuf[mpa->cNod].rot.sty=ARMP_PYR;
	mpa->nBuf[mpa->cNod].rot.rot.pyr.pit=vp;
	mpa->nBuf[mpa->cNod].rot.rot.pyr.yaw=vy;
	mpa->nBuf[mpa->cNod].rot.rot.pyr.rot=vr;
	return BI_OK;
}

int ArPathSetNodeKRA(ARMPA *mpa,int id)
{
	mpa->nBuf[mpa->cNod].rot.sty=ARMP_KRA;
	mpa->nBuf[mpa->cNod].rot.rot.kra.id=id;
	return BI_OK;
}

int ArPathSetNodeFCS(ARMPA *mpa,int id)
{
	mpa->nBuf[mpa->cNod].rot.sty=ARMP_FOCUS;
	mpa->nBuf[mpa->cNod].rot.rot.fcs.id=id;
	return BI_OK;
}

int ArPathSetNodeSmoothRotation(ARMPA *mpa)
{
	mpa->nBuf[mpa->cNod].rot.smooth=ARMP_SMOOTH;
	return BI_OK;
}

int ArPathEndSetNode(ARMPA *mpa)
{
	mpa->cNod++;
	return BI_OK;
}


/***************************************************************************/

int ArFreeMpa(ARMPA *mpa)
{
	int i,j,k;

	for(i=0; i<mpa->nCam; i++)
	{
		BiFree(mpa->cam[i].mag);
	}

	for(i=0; i<mpa->nObj; i++)
	{
		if(mpa->obj[i].alive==BI_ON)
		{
			for(k=0; k<mpa->obj[i].nNod; k++)
			{
				if(mpa->obj[i].nod[k].mot.sty==ARMP_RAWDATA)
				{
					BiFree(mpa->obj[i].nod[k].mot.mot.raw.node);
				}
			}

			BiFree(mpa->obj[i].nod);

			for(k=0; k<i; k++)
			{
				if(strcmp(mpa->obj[k].shp.fn,mpa->obj[i].shp.fn)==0)
				{
					break;
				}
			}
			if(k==i)
			{
				switch(mpa->obj[i].shp.typ)
				{
				case ARMP_SRF:
					ArFreeSrf(&mpa->obj[i].shp.shp.srf);
					break;
				case ARMP_DNM:
					ArFreeDnm(&mpa->obj[i].shp.shp.dnm);
					break;
				}
			}

			for(j=0; j<4; j++)
			{
				if(mpa->obj[i].pSmokeExist[j]==BI_ON)
				{
					ArFreeSmokeInstance(&mpa->obj[i].pSmoke[j]);
				}
			}
		}
	}
	BiFree(mpa->obj);
	return BI_OK;
}



/***************************************************************************/

static int ArGetPathObjPosition(BIPOSATT *p,ARMPA *mpa,int id,long t)
{
	ARMPOBJECT *obj;

	obj=&mpa->obj[id];
	if(obj->tCalced<=t)
	{
		*p=obj->last;
	}
	else if(t<obj->t0)
	{
		*p=obj->iniP;
	}
	else
	{
		*p=obj->path[t];
	}
	return BI_OK;
}

static int ArGetPathObjPositionSmooth(BIPOSATT *p,ARMPA *mpa,int id,real ct)
{
	long idx;
	real t;
	BIPOSATT p1,p2;

	t=ct/mpa->clk;
	idx=(long)t;

	if(ArGetPathObjPosition(&p1,mpa,id,idx  )==BI_OK &&
	   ArGetPathObjPosition(&p2,mpa,id,idx+1)==BI_OK)
	{
		t=(t-(real)idx);
		BiInBetweenPoint(&p->p,&p1.p,&p2.p,t);
		BiInBetweenAngle(&p->a,&p1.a,&p2.a,t);
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetMpaObjPosition(BIPOSATT *p,ARMPA *mpa,int id,real t)
{
	return ArGetPathObjPositionSmooth(p,mpa,id,t);
}

/***************************************************************************/

static real ArCalcPathNodeSmoother(real t)
{
	return (1.0F-BiCos((long)(t*32768.0F)))/2.0F;
}

static void ArCalcPathSmoothRotation
    (long *dp,long *vp,long ldpx,long ldt,long lt)
{
	float vpx;
	float dpx,dt,tu,t,a;
	long dumb;

	if(dp==NULL)
	{
		dp=&dumb;
	}
	if(vp==NULL)
	{
		vp=&dumb;
	}

	dpx=(float)ldpx;
	t=(float)lt;
	dt=(float)ldt;
	tu=dt*0.2F;
	if(tu>40.0F)
	{
		tu=40.0F;
	}

	vpx=dpx/(dt-tu);
	a=vpx/tu;

	if(t<=tu)
	{
		*vp=(long)(vpx*t/tu);
		*dp=(long)(0.5*a*t*t);
	}
	else if(tu<t && t<dt-tu)
	{
		*vp=(long)(vpx);
		*dp=(long)(0.5*a*tu*tu+vpx*(t-tu));
	}
	else
	{
		*vp=(long)((vpx*(dt-t))/tu);
		*dp=(long)(dpx-(0.5*a*(dt-t)*(dt-t)));
	}
}

/***************************************************************************/

#define ARP_ANTIRECURSIVE 32

static int ArCalcPathNodeFWD(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	real v;
	BIVECTOR d;
	if(mpa->obj[id].nod[n].t0<t)
	{
		v=mpa->obj[id].nod[n].mot.mot.fwd.spd;
		v*=mpa->clk;
		BiSetPoint(&d,0,0,v);
		BiRotLtoG(&d,&d,&p->a);
		BiAddPoint(&p->p,&p->p,&d);
	}
	return BI_OK;
}

static int ArCalcPathNodeACC(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	long dt;
	real v,v1,v2;
	BIVECTOR d;
	if(mpa->obj[id].nod[n].t0<t)
	{
		t=t-mpa->obj[id].nod[n].t0;
		dt=mpa->obj[id].nod[n].t;

		v1=mpa->obj[id].nod[n].mot.mot.acc.spd1;
		v2=mpa->obj[id].nod[n].mot.mot.acc.spd2;
		v=v1+(v2-v1)*(real)t/(real)dt;
		v*=mpa->clk;
		BiSetPoint(&d,0,0,v);
		BiRotLtoG(&d,&d,&p->a);
		BiAddPoint(&p->p,&p->p,&d);
	}
	return BI_OK;
}

static int ArCalcPathNodePRC(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	BIVECTOR d;
	if(mpa->obj[id].nod[n].t0<t)
	{
		BiMulPoint(&d,&mpa->obj[id].nod[n].mot.mot.prc.v,mpa->clk);
		BiRotLtoG(&d,&d,&p->a);
		BiAddPoint(&p->p,&p->p,&d);
	}
	return BI_OK;
}

static int ArCalcPathNodeMTL(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	long dt;
	BIVECTOR d;
	t=t-mpa->obj[id].nod[n].t0;
	dt=mpa->obj[id].nod[n].t;
	BiMulPoint(&d,&mpa->obj[id].nod[n].mot.mot.mtl.p,(real)t/(real)dt);
	BiRotLtoG(&d,&d,&mpa->obj[id].nod[n].iniP.a);
	BiAddPoint(&p->p,&mpa->obj[id].nod[n].iniP.p,&d);
	return BI_OK;
}

static int ArCalcPathNodeABS(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	long dt;
	BIPOINT endp;
	ARMPOBJECT *obj;
	ARMPNODE *nod;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	t=t-nod->t0;
	dt=nod->t;
	endp=nod->mot.mot.abs.p;
	BiInBetweenPoint(&p->p,&nod->iniP.p,&endp,(real)t/(real)dt);
	return BI_OK;
}

static int ArCalcPathNodeMTR(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	BIPOSATT parSt,parTm,parEd;
	BIPOINT pSt,pTm,pEd;
	BIAXIS parStAxs,parTmAxs,parEdAxs;
	BIVECTOR v;
	real param;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	param=(real)(t-nod->t0)/(real)nod->t;

	ArGetPathObjPosition(&parSt,mpa,nod->mot.mot.mtr.id,nod->t0);
	BiMakeAxis(&parStAxs,&parSt);

	ArGetPathObjPosition(&parTm,mpa,nod->mot.mot.mtr.id,t);
	BiMakeAxis(&parTmAxs,&parTm);

	ArGetPathObjPosition(&parEd,mpa,nod->mot.mot.mtr.id,nod->t1);
	BiMakeAxis(&parEdAxs,&parEd);

	BiConvGtoL(&pSt,&nod->iniP.p,&parStAxs);
	pEd=nod->mot.mot.mtr.p;
	BiSubPoint(&v,&pEd,&pSt);
	BiMulPoint(&v,&v,param);
	BiAddPoint(&pTm,&pSt,&v);

	BiConvLtoG(&p->p,&pTm,&parTmAxs);

	return BI_OK;
}
static int ArCalcPathNodeMTS(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	real dt;
	BIVECTOR d;
	dt=(real)(t-mpa->obj[id].nod[n].t0)/(real)mpa->obj[id].nod[n].t;
	dt=ArCalcPathNodeSmoother(dt);
	BiMulPoint(&d,&mpa->obj[id].nod[n].mot.mot.mtl.p,dt);
	BiRotLtoG(&d,&d,&mpa->obj[id].nod[n].iniP.a);
	BiAddPoint(&p->p,&mpa->obj[id].nod[n].iniP.p,&d);
	return BI_OK;
}
static int ArCalcPathNodeMSR(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	BIPOSATT parSt,parTm,parEd;
	BIPOINT pSt,pTm,pEd;
	BIAXIS parStAxs,parTmAxs,parEdAxs;
	BIVECTOR v;
	real param;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	param=ArCalcPathNodeSmoother((real)(t-nod->t0)/(real)nod->t);

	ArGetPathObjPosition(&parSt,mpa,nod->mot.mot.mtr.id,nod->t0);
	BiMakeAxis(&parStAxs,&parSt);

	ArGetPathObjPosition(&parTm,mpa,nod->mot.mot.mtr.id,t);
	BiMakeAxis(&parTmAxs,&parTm);

	ArGetPathObjPosition(&parEd,mpa,nod->mot.mot.mtr.id,nod->t1);
	BiMakeAxis(&parEdAxs,&parEd);

	BiConvGtoL(&pSt,&nod->iniP.p,&parStAxs);
	pEd=nod->mot.mot.mtr.p;
	BiSubPoint(&v,&pEd,&pSt);
	BiMulPoint(&v,&v,param);
	BiAddPoint(&pTm,&pSt,&v);

	BiConvLtoG(&p->p,&pTm,&parTmAxs);

	return BI_OK;
}
static int ArCalcPathNodeKRP(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	BIPOSATT parSt,parTm;
	BIPOINT pSt;
	BIAXIS parStAxs,parTmAxs;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	ArGetPathObjPosition(&parSt,mpa,nod->mot.mot.krp.id,nod->t0);
	ArGetPathObjPosition(&parTm,mpa,nod->mot.mot.krp.id,t);

	BiMakeAxis(&parStAxs,&parSt);
	BiMakeAxis(&parTmAxs,&parTm);

	BiConvGtoL(&pSt,&nod->iniP.p,&parStAxs);
	BiConvLtoG(&p->p,&pSt,&parTmAxs);

	return BI_OK;
}

static int ArCalcPathNodePRL(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	BIPOSATT parSt,parTm;
	BIPOINT dif;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	ArGetPathObjPosition(&parSt,mpa,nod->mot.mot.prl.id,nod->t0);
	ArGetPathObjPosition(&parTm,mpa,nod->mot.mot.prl.id,t);

	BiSubPoint(&dif,&nod->iniP.p,&parSt.p);
	BiAddPoint(&p->p,&parTm.p,&dif);

	return BI_OK;
}

static int ArCalcPathNodeENG(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	real v1,y1,y2,v;
	BIVECTOR d;
	if(mpa->obj[id].nod[n].t0<t)
	{
		v1=mpa->obj[id].nod[n].mot.mot.eng.spd1;
		y1=mpa->obj[id].nod[n].iniP.p.y;

		y2=p->p.y;

		v=(real)sqrt(v1*v1+2.0F*9.8F*(y1-y2));

		v*=mpa->clk;
		BiSetPoint(&d,0,0,v);
		BiRotLtoG(&d,&d,&p->a);
		BiAddPoint(&p->p,&p->p,&d);
	}
	return BI_OK;
}

static int ArCalcPathNodeCHS(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	int parId;
	long dt;
	ARMPOBJECT *obj,*par;
	ARMPNODE *nod;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];
	parId=nod->mot.mot.chs.id;
	par=&mpa->obj[parId];

	dt=20;
	for(dt=0; dt<60; dt++)
	{
		BIPOSATT par;
		BIAXIS pax;
		BIPOINT rel;
		ArGetPathObjPosition(&par,mpa,parId,t-dt-1);
		BiMakeAxis(&pax,&par);
		BiConvGtoL(&rel,&p->p,&pax);
		if(rel.z>=0)
		{
			break;
		}
	}

	if(obj->t0<=t-dt-1 && par->t0<=t-dt-1)
	{
		BIPOSATT paro,part;
		BIPOSATT relo;
		BIAXIS paxo,paxt;
		BIPOINT rel;

		ArGetPathObjPosition(&paro,mpa,parId,t-dt-1);
		ArGetPathObjPosition(&part,mpa,parId,t-dt);
		BiMakeAxis(&paxo,&paro);
		BiMakeAxis(&paxt,&part);

		ArGetPathObjPosition(&relo,mpa,id,t-1);

		BiConvGtoL(&rel,&relo.p,&paxo);
		BiConvLtoG(&rel,&rel,&paxt);

		p->p=rel;
	}
	else
	{
		BIPOSATT parSt,parTm;
		BIPOINT rel;
		BIAXIS parStAxs,parTmAxs;

		ArGetPathObjPosition(&parSt,mpa,parId,nod->t0);
		BiMakeAxis(&parStAxs,&parSt);
		BiConvGtoL(&rel,&nod->iniP.p,&parStAxs);

		ArGetPathObjPosition(&parTm,mpa,parId,t);
		BiMakeAxis(&parTmAxs,&parTm);
		BiConvLtoG(&p->p,&rel,&parTmAxs);
	}

	return BI_OK;
}

static int ArCalcPathNodeRAW
(ARMPA *mpa,int id,int n,long t,BIPOSATT *pos,long *lastRefer,real *lastTime)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	ARMPRAW *raw;
	real rt;
	int i;

	BIAXIS path0,node0;
	BIPOINT rel,relev,reluv;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];
	raw=&nod->mot.mot.raw;

	rt=(real)(t-nod->t0)*mpa->clk;
	if(t<nod->t0)
	{
		*pos=raw->node[0].p;
	}
	else
	{
		real t0,t1;
		t0=*lastTime;
		for(i=*lastRefer; i<raw->nNode-1; i++)
		{
			*lastRefer=i;
			*lastTime=t0;

			t1=t0+raw->node[i].t;

			if(t0<=rt && rt<t1)
			{
				real param;
				BIPOSATT pos0,pos1;
				pos0=raw->node[i].p;
				pos1=raw->node[i+1].p;
				param=(rt-t0)/(t1-t0);
				BiInBetweenPoint(&pos->p,&pos0.p,&pos1.p,param);
				BiInBetweenAngle(&pos->a,&pos0.a,&pos1.a,param);
				goto ADJUST;
			}
			t0=t0+raw->node[i].t;
		}
		*pos=raw->node[raw->nNode-1].p;
	}
ADJUST:
	BiMakeAxis(&path0,&raw->node[0].p);
	BiMakeAxis(&node0,&nod->iniP);

	BiAngleToVector(&relev,&reluv,&pos->a);
	BiConvGtoL(&rel,&pos->p,&path0);
	BiRotFastGtoL(&relev,&relev,&path0.t);
	BiRotFastGtoL(&reluv,&reluv,&path0.t);
	BiConvLtoG(&rel,&rel,&node0);
	BiRotFastLtoG(&relev,&relev,&node0.t);
	BiRotFastLtoG(&reluv,&reluv,&node0.t);

	pos->p=rel;
	BiVectorToAngle(&pos->a,&relev,&reluv);

	return BI_OK;
}

static int ArCalcPathNodeHPB(ARMPA *mpa,int id,int n,long t,BIPOSATT *pos)
{
	long h,p,b,dh,dp,db;
	long dt;
	BIATTITUDE sta;
	t=t-mpa->obj[id].nod[n].t0;
	dt=mpa->obj[id].nod[n].t;

	h=mpa->obj[id].nod[n].rot.rot.hpb.att.h;
	p=mpa->obj[id].nod[n].rot.rot.hpb.att.p;
	b=mpa->obj[id].nod[n].rot.rot.hpb.att.b;

	if(mpa->obj[id].nod[n].rot.smooth==ARMP_NOSMOOTH)
	{
		dh=t*h/dt;
		dp=t*p/dt;
		db=t*b/dt;
	}
	else
	{
		ArCalcPathSmoothRotation(&dh,NULL,h,dt,t);
		ArCalcPathSmoothRotation(&dp,NULL,p,dt,t);
		ArCalcPathSmoothRotation(&db,NULL,b,dt,t);
	}

	sta=mpa->obj[id].nod[n].iniP.a;
	pos->a.h=sta.h+dh;
	pos->a.p=sta.p+dp;
	pos->a.b=sta.b+db;
	return BI_OK;
}
static int ArCalcPathNodePYR(ARMPA *mpa,int id,int n,long t,BIPOSATT *pos)
{
	long h,p,b,dh,dp,db;
	long dt;

	t=t-mpa->obj[id].nod[n].t0;
	dt=mpa->obj[id].nod[n].t;

	p=mpa->obj[id].nod[n].rot.rot.pyr.pit;
	h=mpa->obj[id].nod[n].rot.rot.pyr.yaw;
	b=mpa->obj[id].nod[n].rot.rot.pyr.rot;

	if(mpa->obj[id].nod[n].rot.smooth==ARMP_NOSMOOTH)
	{
		dp=p/dt;
		dh=h/dt;
		db=b/dt;
	}
	else
	{
		ArCalcPathSmoothRotation(NULL,&dp,p,dt,t);
		ArCalcPathSmoothRotation(NULL,&dh,h,dt,t);
		ArCalcPathSmoothRotation(NULL,&db,b,dt,t);
	}
	pos->a.b+=db;
	BiPitchUp(&pos->a,&pos->a,dp,dh);
	return BI_OK;
}
static int ArCalcPathNodeKRA(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	BIPOSATT parSt,parTm;
	BIVECTOR ev,uv;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	ArGetPathObjPosition(&parSt,mpa,nod->rot.rot.kra.id,nod->t0);
	ArGetPathObjPosition(&parTm,mpa,nod->rot.rot.kra.id,t);

	BiAngleToVector(&ev,&uv,&nod->iniP.a);
	BiRotGtoL(&ev,&ev,&parSt.a);
	BiRotGtoL(&uv,&uv,&parSt.a);
	BiRotLtoG(&ev,&ev,&parTm.a);
	BiRotLtoG(&uv,&uv,&parTm.a);
	BiVectorToAngle(&p->a,&ev,&uv);

	return BI_OK;
}
static int ArCalcPathNodeFCS(ARMPA *mpa,int id,int n,long t,BIPOSATT *p)
{
	ARMPOBJECT *obj;
	ARMPNODE *nod;
	BIPOSATT parTm;
	BIVECTOR ev;

	obj=&mpa->obj[id];
	nod=&obj->nod[n];

	ArGetPathObjPosition(&parTm,mpa,nod->rot.rot.fcs.id,t);

	BiSubPoint(&ev,&parTm.p,&p->p);
	BiVectorToHeadPitch(&p->a,&ev);
	return BI_OK;
}

static int ArCalcPathNode(ARMPA *mpa,int id,int n,int antiRecursive)
{
	// Currently lastRefer,lastTime is for RAW data only
	// It's so special.
	long lastRefer;
	real lastTime;

	long t,t0,t1;
	int er;
	ARMPOBJECT *obj;
	int nNod;
	ARMPNODE *nod;
	BIPOSATT pth;

	obj=&mpa->obj[id];

	nNod=obj->nNod;
	nod=&obj->nod[n];
	pth=obj->last;
	t0=nod->t0;
	t1=nod->t1;

	nod->iniP=pth;

	lastRefer=0;
	lastTime=0.0F;

	er=BI_OK;
	switch(nod->mot.sty)
	{
	case ARMP_MOVETOREL:
		er=ArCalcPathObj(mpa,nod->mot.mot.mtr.id,t1,antiRecursive);
		break;
	case ARMP_MOVETOSMOREL:
		er=ArCalcPathObj(mpa,nod->mot.mot.mtr.id,t1,antiRecursive);
		break;
	case ARMP_KEEPRELPOS:
		er=ArCalcPathObj(mpa,nod->mot.mot.krp.id,t1,antiRecursive);
		break;
	case ARMP_PARALLEL:
		er=ArCalcPathObj(mpa,nod->mot.mot.prl.id,t1,antiRecursive);
		break;
	}
	if(er!=BI_OK)
	{
		MBOX("ArCalcPathNode(1)");
		return BI_ERR;
	}
	switch(nod->rot.sty)
	{
	case ARMP_KRA:
		er=ArCalcPathObj(mpa,nod->rot.rot.kra.id,t1,antiRecursive);
		break;
	case ARMP_FOCUS:
		er=ArCalcPathObj(mpa,nod->rot.rot.fcs.id,t1,antiRecursive);
		break;
	}
	if(er!=BI_OK)
	{
		MBOX("ArCalcPathNode(2)");
		return BI_ERR;
	}

	for(t=t0; t<=t1; t++)
	{
		if(obj->tCalced<t || (obj->tCalced<=t && nod->rot.sty==ARMP_FOCUS))
		{
			switch(nod->mot.sty)
			{
			case ARMP_FORWARD:
				ArCalcPathNodeFWD(mpa,id,n,t,&pth);
				break;
			case ARMP_ACCEL:
				ArCalcPathNodeACC(mpa,id,n,t,&pth);
				break;
			case ARMP_PROCEED:
				ArCalcPathNodePRC(mpa,id,n,t,&pth);
				break;
			case ARMP_MOVETO:
				ArCalcPathNodeMTL(mpa,id,n,t,&pth);
				break;
			case ARMP_MOVETOABS:
				ArCalcPathNodeABS(mpa,id,n,t,&pth);
				break;
			case ARMP_MOVETOREL:
				ArCalcPathNodeMTR(mpa,id,n,t,&pth);
				break;
			case ARMP_MOVETOSMO:
				ArCalcPathNodeMTS(mpa,id,n,t,&pth);
				break;
			case ARMP_MOVETOSMOREL:
				ArCalcPathNodeMSR(mpa,id,n,t,&pth);
				break;
			case ARMP_KEEPRELPOS:
				ArCalcPathNodeKRP(mpa,id,n,t,&pth);
				break;
			case ARMP_PARALLEL:
				ArCalcPathNodePRL(mpa,id,n,t,&pth);
				break;
			case ARMP_CONSTENERGY:
				ArCalcPathNodeENG(mpa,id,n,t,&pth);
				break;
			case ARMP_CHASE:
				ArCalcPathNodeCHS(mpa,id,n,t,&pth);
				break;
			case ARMP_RAWDATA:
				ArCalcPathNodeRAW(mpa,id,n,t,&pth,&lastRefer,&lastTime);
				goto SKIPROTATION;
				break;
			}
			switch(nod->rot.sty)
			{
			case ARMP_HPB:
				ArCalcPathNodeHPB(mpa,id,n,t,&pth);
				break;
			case ARMP_PYR:
				ArCalcPathNodePYR(mpa,id,n,t,&pth);
				break;
			case ARMP_KRA:
				ArCalcPathNodeKRA(mpa,id,n,t,&pth);
				break;
			case ARMP_FOCUS:
				ArCalcPathNodeFCS(mpa,id,n,t,&pth);
				if(t==nod->t0)
				{
					nod->iniP.a=pth.a;
				}
				break;
			}
SKIPROTATION:   // for RAW data
			obj->path[t-obj->t0]=pth;
			obj->last=pth;
			obj->tCalced=t;
		}
	}
	return BI_OK;
}

static int ArCalcPathObj(ARMPA *mpa,int id,long eTim,int antiRecursive)
{
	int i,nSta;
	ARMPOBJECT *obj;
	int nNod;
	ARMPNODE *nod;
	long tCalced;

	if(mpa->obj[id].alive!=BI_ON)
	{
		return BI_ERR;
	}

	antiRecursive--;
	if(antiRecursive<0)
	{
		MBOX("ArCalcPathObj(1)");
		return BI_ERR;
	}

	obj=&mpa->obj[id];

	nNod=obj->nNod;
	nod=obj->nod;
	tCalced=obj->tCalced;

	if(eTim<=tCalced)                   /* Already Calculated */
	{
		return BI_OK;
	}
	if(nNod<=0)     /* || nod[nNod-1].t1<eTim)  Time Exceeded */
	{
		MBOX("ArCalcPathObj(2)");
		return BI_ERR;
	}

	for(nSta=0; nSta<nNod && nod[nSta].t1<=tCalced; nSta++)
	{
		/* if(nSta>=nNod-1 && nod[nSta].t1<=tCalced)
		{
			MBOX("ArCalcPathObj(3)");
			return BI_ERR;
		} */
	}

	for(i=nSta; i<nNod && nod[i].t0<=eTim ;i++)
	{
		if(ArCalcPathNode(mpa,id,i,antiRecursive)!=BI_OK)
		{
			MBOX("ArCalcPathObj(4)");
			return BI_ERR;
		}
	}
	return BI_OK;
}

static void ArCalcPathInit(ARMPA *mpa)
{
	int i;
	for(i=0; i<mpa->nObj; i++)
	{
		mpa->obj[i].tCalced=-1;
		mpa->obj[i].last=mpa->obj[i].iniP;
	}
}

static int ArCalcPathMain(ARMPA *mpa)
{
	int i;
	long eTim;
	for(i=0; i<mpa->nObj; i++)
	{
		if(mpa->obj[i].alive==BI_ON && mpa->obj[i].nNod>0)
		{
			eTim=mpa->obj[i].nod[mpa->obj[i].nNod-1].t1;
			if(ArCalcPathObj(mpa,i,eTim,ARP_ANTIRECURSIVE)!=BI_OK)
			{
				return BI_ERR;
			}
		}
	}
	return BI_OK;
}

/***************************************************************************/
static int ArMallocPath(ARMPA *mpa)
{
	int i,j;
	long np,t;
	ARMPOBJECT *obj;
	for(i=0; i<mpa->nObj; i++)
	{
		obj=&mpa->obj[i];

		if(obj->alive==BI_ON && obj->nNod>0)
		{
			t=obj->t0;
			for(j=0; j<obj->nNod; j++)
			{
				obj->nod[j].t0=t;
				t+=obj->nod[j].t;
				obj->nod[j].t1=t;
			}
			np=obj->nod[obj->nNod-1].t1-obj->t0+1;
			obj->nPath=np;
			obj->path=(BIPOSATT *)BiMalloc(sizeof(BIPOSATT)*np);
			if(obj->path==NULL && obj->nPath>0)
			{
				return BI_ERR;
			}
		}
		else
		{
			obj->nPath=0;
			obj->path=NULL;
		}
	}
	return BI_OK;
}

int ArCalcCompletePath(ARMPA *mpa)
{
	MBOX("ArCalcCompletePath(1)");
	if(ArMallocPath(mpa)==BI_OK)
	{
		MBOX("ArCalcCompletePath(2)");
		ArCalcPathInit(mpa);
		MBOX("ArCalcCompletePath(3)");
		if(ArCalcPathMain(mpa)==BI_OK)
		{
			return BI_OK;
		}
		MBOX("ArCalcCompletePath(4)");
	}
	MBOX("ArCalcCompletePath(5)");
	return BI_ERR;
}

int ArEraseCompletePath(ARMPA *mpa)
{
	int i;
	for(i=0; i<mpa->nObj; i++)
	{
		if(mpa->obj[i].alive==BI_ON)
		{
			BiFree(mpa->obj[i].path);
		}
	}
	return BI_OK;
}

/***************************************************************************/

static int ArSetMpaLightPosition(ARMPA *mpa,real ct)
{
	int er;
	long t;
	BIPOSATT d;
	t=(long)(ct/mpa->clk);
	switch(mpa->lig.typ)
	{
	default:
	case ARMP_NOLIGHT:
		return BI_ERR;
	case ARMP_ONBOARDLIGHT:
		er=ArGetPathObjPosition(&d,mpa,mpa->lig.atr.obl.id,t);
		if(er==BI_OK)
		{
			mpa->pLig=d.p;
		}
		return er;
	case ARMP_FIXEDLIGHT:
		mpa->pLig=mpa->lig.atr.fxl.pos;
		return BI_OK;
	}
}

static int ArSetPathObjSmoke(ARMPOBJECT *obj,ARMPA *mpa,real ct)
{
	int i,j,sw;
	BIPOINT ofs;
	BIPOSATT skp;
	for(i=0; i<4; i++)
	{
		if(obj->pSmokeExist[i]==BI_ON)
		{
			sw=BI_OFF;
			for(j=0; j<obj->smk[i].nSmk; j++)
			{
				if(obj->smk[i].smk[j].t0<=ct && ct<=obj->smk[i].smk[j].t1)
				{
					sw=BI_ON;
					break;
				}
			}
			if(obj->ppSmk[i]==BI_OFF && sw==BI_ON)
			{
				ArBeginAppendSmokeNode(&obj->pSmoke[i]);
			}
			else if(obj->ppSmk[i]==BI_ON && sw==BI_OFF)
			{
				ArEndAppendSmokeNode(&obj->pSmoke[i]);
			}
			if(sw==BI_ON)
			{
				BiRotLtoG(&ofs,&obj->smk[i].gen,&obj->pPos.a);
				BiAddPoint(&skp.p,&obj->pPos.p,&ofs);
				skp.a=obj->pPos.a;
				ArAppendSmokeNode(&obj->pSmoke[i],&skp,ct);
			}
			obj->ppSmk[i]=sw;
		}
	}
	return BI_OK;
}

static int ArSetPathObjStatus(ARMPOBJECT *obj,ARMPA *mpa,real ct)
{
	int i,s0,s1;
	real t;

	s0=obj->iniS;
	s1=obj->iniS;
	t=0.0F;
	if(obj->nSta>1)
	{
		for(i=0; i<obj->nSta-1; i++)
		{
			if(obj->sta[i].t<=ct && ct<=obj->sta[i+1].t)
			{
				s0=obj->sta[i  ].s;
				s1=obj->sta[i+1].s;
				t=(ct-obj->sta[i].t)/(obj->sta[i+1].t-obj->sta[i].t);
			}
			else if(obj->sta[i+1].t<=ct)
			{
				s0=obj->sta[i+1].s;
				s1=obj->sta[i+1].s;
				t=0.0F;
			}
		}
	}
	else if(obj->nSta==1)
	{
		if(obj->sta[0].t<=ct)
		{
			s0=obj->sta[0].s;
			s1=obj->sta[0].s;
		}
		else
		{
			s1=obj->sta[0].s;
			t=ct/obj->sta[0].t;
		}
	}
	obj->pS1=s0;
	obj->pS2=s1;
	obj->pParam=t;
	return BI_OK;
}

int ArEndPlayMpa(ARMPA *mpa)
{
	int i,j;
	for(i=0; i<mpa->nObj; i++)
	{
		if(mpa->obj[i].alive==BI_ON)
		{
			for(j=0; j<4; j++)
			{
				if(mpa->obj[i].smk[j].alive==BI_ON &&
				   mpa->obj[i].pSmokeExist[j]==BI_ON)
				{
					ArFreeSmokeInstance(&mpa->obj[i].pSmoke[j]);
					mpa->obj[i].pSmokeExist[j]=BI_OFF;
				}
			}
		}
	}
	return BI_OK;
}

int ArInitPlayMpa(ARMPA *mpa,int nSmk,int nSmkDel)
{
	int i,j,sty;
	ARMPOBJECT *obj;
	for(i=0; i<mpa->nObj; i++)
	{
		obj=&mpa->obj[i];
		if(obj->alive==BI_ON)
		{
			obj->pPos=obj->iniP;
			obj->pS1=obj->iniS;
			obj->pS2=obj->iniS;
			obj->pParam=0.0F;
			for(j=0; j<4; j++)
			{
				if(obj->smk[j].alive==BI_ON)
				{
					obj->ppSmk[j]=BI_OFF;

					sty=obj->smk[j].sty;
					ArInitSmokeClass(&obj->pSmokeClass[j]);
					ArSetSmokeType(&obj->pSmokeClass[j],sty);
					ArSetSmokeLife(&obj->pSmokeClass[j],sty,obj->smk[j].t0,obj->smk[j].t1);
					ArSetSmokeWidth(&obj->pSmokeClass[j],sty,obj->smk[j].iniw,obj->smk[j].maxw,obj->smk[j].dw);
					ArSetSmokeColor(&obj->pSmokeClass[j],sty,&obj->smk[j].inic,&obj->smk[j].endc,obj->smk[j].tc);
					ArSetSmokeBoundingBox(&obj->pSmokeClass[j],obj->smk[j].stp,obj->smk[j].bbx);

					if(ArInitSmokeInstance(&obj->pSmoke[j],nSmk,nSmkDel)==BI_OK)
					{
						obj->pSmokeExist[j]=BI_ON;
					}
					else
					{
						obj->pSmokeExist[j]=BI_OFF;
					}
				}
			}
		}
	}
	for(i=0; i<mpa->nCam; i++)
	{
		ARMPMAGNIFY *mag;
		mag=mpa->cam[i].mag;
		for(j=0; j<mpa->cam[i].nMag-1; j++)
		{
			real dt,dmx,dmy;
			dt=mag[j+1].t-mag[j].t;
			if(dt<YSEPS || mag[j].magx<YSEPS || mag[j].magy<YSEPS)
			{
				mag[j].rootx=1.0F;
				mag[j].rooty=1.0F;
			}
			else
			{
				dmx=mag[j+1].magx/mag[j].magx;
				dmy=mag[j+1].magy/mag[j].magy;
				mag[j].rootx=(real)pow(dmx,1.0F/dt);
				mag[j].rooty=(real)pow(dmy,1.0F/dt);
			}
		}
	}
	mpa->pTim=0.0F;
	return BI_OK;
}

int ArRewindMpa(ARMPA *mpa)
{
	int i,j;
	ARMPOBJECT *obj;
	for(i=0; i<mpa->nObj; i++)
	{
		obj=&mpa->obj[i];
		if(obj->alive==BI_ON)
		{
			obj->pPos=obj->iniP;
			obj->pS1=obj->iniS;
			obj->pS2=obj->iniS;
			obj->pParam=0.0F;
			for(j=0; j<4; j++)
			{
				if(obj->smk[j].alive==BI_ON)
				{
					obj->ppSmk[j]=BI_OFF;
					ArClearSmokeInstance(&obj->pSmoke[j]);
				}
			}
		}
	}
	mpa->pTim=0.0F;
	return BI_OK;
}

int ArPlayMpa(ARMPA *mpa,real ct)
{
	int i;
	ARMPOBJECT *obj;
	mpa->pTim=ct;
	ArSetMpaLightPosition(mpa,ct);
	for(i=0; i<mpa->nObj; i++)
	{
		obj=&mpa->obj[i];
		if(obj->alive==BI_ON && obj->t0<=ct && ct<=obj->t1)
		{
			ArGetPathObjPositionSmooth(&mpa->obj[i].pPos,mpa,i,ct);
			ArSetPathObjSmoke(&mpa->obj[i],mpa,ct);
			ArSetPathObjStatus(&mpa->obj[i],mpa,ct);

			if(mpa->obj[i].shp.typ==ARMP_DNM)
			{
				int s0,s1;
				real t;
				s0=mpa->obj[i].pS1;
				s1=mpa->obj[i].pS2;
				t=mpa->obj[i].pParam;
				ArInBetweenDnmStatus(&mpa->obj[i].shp.shp.dnm,-1,s0,s1,t);
			}
		}
	}
	return BI_OK;
}

int ArGetMpaCamera(BIPOSATT *cam,ARMPA *mpa,int cId)
{
	int er;
	real t;
	BIPOSATT d;
	BIVECTOR v;

	if(0<=cId && cId<mpa->nCam)
	{
		t=mpa->pTim;
		switch(mpa->cam[cId].typ)
		{
		default:
		case ARMP_NOTSPECIFIED:
			return BI_ERR;
		case ARMP_ONBOARDCAM:
			return ArGetPathObjPositionSmooth(cam,mpa,mpa->cam[cId].atr.obc.id,t);
		case ARMP_FIXEDCAM:
			er=ArGetPathObjPositionSmooth(&d,mpa,mpa->cam[cId].atr.fxc.id,t);
			if(er==BI_OK)
			{
				cam->p=mpa->cam[cId].atr.fxc.pos;
				BiSubPoint(&v,&d.p,&mpa->cam[cId].atr.fxc.pos);
				BiVectorToHeadPitch(&cam->a,&v);
			}
			return er;
		case ARMP_STATICCAM:
			*cam=mpa->cam[cId].atr.stc.pos;
			return BI_OK;
		}
		return BI_OK;
	}
	return BI_ERR;
}

int ArGetMpaLight(BIPOINT *lig,ARMPA *mpa)
{
	*lig=mpa->pLig;
	return BI_OK;
}

int ArInsMpa(ARMPA *mpa,int detail)
{
	int i;
	real ct;
	ct=mpa->pTim;
	for(i=0; i<mpa->nObj; i++)
	{
		ARMPOBJECT *obj;
		obj=&mpa->obj[i];
		if(obj->alive==BI_ON  && obj->t0<=ct && ct<=obj->t1)
		{
			switch(obj->shp.typ)
			{
			case ARMP_DNM:
				switch(detail)
				{
				case BI_ON:
					ArInsDnmDivide(&obj->shp.shp.dnm,&obj->pPos);
					break;
				case BI_OFF:
					ArInsDnm(&obj->shp.shp.dnm,&obj->pPos);
					break;
				}
				break;
			case ARMP_SRF:
				BiInsSrf(&obj->shp.shp.srf,&obj->pPos);
				break;
			}
		}
	}
	return BI_OK;
}

int ArOvwMpaShadow(ARMPA *mpa)
{
	int i;
	real ct;
	ct=mpa->pTim;
	for(i=0; i<mpa->nObj; i++)
	{
		ARMPOBJECT *obj;
		obj=&mpa->obj[i];
		if(obj->alive==BI_ON && obj->t0<=ct && ct<=obj->t1)
		{
			switch(obj->shp.typ)
			{
			case ARMP_DNM:
				ArOvwDnmShadow(&obj->shp.shp.dnm,&obj->pPos);
				break;
			case ARMP_SRF:
				BiOvwSrfShadow(&obj->shp.shp.srf,&obj->pPos);
				break;
			}
		}
	}
	return BI_OK;
}

int ArInsMpaSmoke(ARMPA *mpa,BIPOSATT *eye)
{
	int i,j;
	for(i=0; i<mpa->nObj; i++)
	{
		if(mpa->obj[i].alive==BI_ON)
		{
			for(j=0; j<4; j++)
			{
				if(mpa->obj[i].pSmokeExist[j]==BI_ON)
				{
					ArInsSmoke(&mpa->obj[i].pSmokeClass[j],&mpa->obj[i].pSmoke[j],mpa->pTim,eye);
				}
			}
		}
	}
	return BI_OK;
}


int ArGetMpaMagnification(real *mx,real *my,ARMPA *mpa,int cId)
{
	int i;
	real t,ct;
	ARMPCAMERA *cam;

	if(0<=cId && cId<mpa->nCam)
	{
		ct=mpa->pTim;
		cam=&mpa->cam[cId];

		if(cam->nMag>0)
		{
			if(ct<cam->mag[0].t)
			{
				*mx=cam->mag[0].magx;
				*my=cam->mag[0].magy;
				return BI_OK;
			}
			else if(cam->mag[cam->nMag-1].t<=ct)
			{
				*mx=cam->mag[cam->nMag-1].magx;
				*my=cam->mag[cam->nMag-1].magy;
				return BI_OK;
			}
			else
			{
				for(i=0; i<cam->nMag-1; i++)
				{
					if(cam->mag[i].t<=ct && ct<=cam->mag[i+1].t)
					{
						t=ct-cam->mag[i].t;
						*mx=cam->mag[i].magx*(real)pow(cam->mag[i].rootx,t);
						*my=cam->mag[i].magy*(real)pow(cam->mag[i].rooty,t);
						/* t=(ct-cam->mag[i].t)/(cam->mag[i+1].t-cam->mag[i].t); */
						/* *mx=cam->mag[i].magx+(cam->mag[i+1].magx-cam->mag[i].magx)*t; */
						/* *my=cam->mag[i].magy+(cam->mag[i+1].magy-cam->mag[i].magy)*t; */
						return BI_OK;
					}
				}
			}
		}
		else
		{
			*mx=1.0F;
			*my=1.0F;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int ArGetMpaActualTime(real *t0,real *t1,ARMPA *mpa)
{
	int i,first;
	long u0,u1;
	ARMPOBJECT *obj;

	obj=&mpa->obj[0];
	u0=0;
	u1=0;
	first=BI_ON;
	for(i=0; i<mpa->nObj; i++)
	{
		obj=&mpa->obj[i];
		if(obj->alive==BI_ON && obj->nNod>0)
		{
			if(first!=BI_ON)
			{
				u0=BiSmaller(u0,obj->nod[0          ].t0);
				u1=BiLarger (u1,obj->nod[obj->nNod-1].t1);
			}
			else
			{
				u0=obj->nod[0          ].t0;
				u1=obj->nod[obj->nNod-1].t1;
				first=BI_OFF;
			}
		}
	}
	*t0=(real)u0*mpa->clk;
	*t1=(real)u1*mpa->clk;
	return BI_OK;
}

int ArGetMpaHasField(ARMPA *mpa)
{
	return (mpa->fld[0]!=0 ? BI_OK : BI_ERR);
}

int ArGetMpaName(char nam[],ARMPA *mpa)
{
	strcpy(nam,mpa->nam);
	return BI_OK;
}

int ArGetMpaObjShapeType(int *type,ARMPA *mpa,int id)
{
	if(0<=id && id<mpa->nObj)
	{
		*type=mpa->obj[id].shp.typ;
		return BI_OK;
	}
	*type=0;
	return BI_ERR;
}

int ArGetMpaNumObj(int *nObj,ARMPA *mpa)
{
	*nObj=mpa->nObj;
	return BI_OK;
}
