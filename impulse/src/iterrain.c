#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impulse.h"
#include "i3dg.h"
#include "irender.h"
#include "iutil.h"

static void BiTmGetBlkTrgs(BIPOINT *tri,BITER *ter,long x,long z);
static void BiTmConstTrgNorm(BIPOINT *nom,BIPOINT *tri);
static void BiTmConstTrgCntr(BIPOINT *cen,BIPOINT *tri);
static void BiTmGetCenter(BIPOINT *cen,BIPOINT tri[]);
static void BiTmGetBlkPoint(BIPOINT *pnt,BITER *ter,long x,long z);
static BITERRBLOCK *BiTmSeekBlk(BITER *ter,long x,long z);
static void BiMakeSideWall0(BIPOINT *trg,BITER *ter);
static void BiMakeSideWall1(BIPOINT *trg,BITER *ter);
static void BiMakeSideWall2(BIPOINT *trg,BITER *ter);
static void BiMakeSideWall3(BIPOINT *trg,BITER *ter);

int BiConstTer(BITER *ter)
{
	long x,z;
	BIPOINT min,max,tri[6];
	BITERRBLOCK *blk;

	BiSetPoint(&min,0,0,0);
	BiSetPoint(&max,ter->xWid*(real)ter->xSiz,ter->blk[0].y,ter->zWid*(real)ter->zWid);
	for(z=0; z<=ter->zSiz; z++)
	{
		for(x=0; x<=ter->xSiz; x++)
		{
			/* Get min,max to make Bounding Box */
			blk=BiTmSeekBlk(ter,x,z);
			min.y=BiSmaller(blk->y,min.y);
			max.y=BiLarger(blk->y,max.y);

			if(z!=ter->zSiz && x!=ter->xSiz)
			{
				BiTmGetBlkTrgs(tri,ter,x,z);

				/* Keep Triangles */
				blk->tr0[0]=tri[0];
				blk->tr0[1]=tri[1];
				blk->tr0[2]=tri[2];
				blk->tr1[0]=tri[3];
				blk->tr1[1]=tri[4];
				blk->tr1[2]=tri[5];

				/* Make Normals */
				BiTmConstTrgNorm(&blk->n[0],&tri[0]);
				BiTmConstTrgNorm(&blk->n[1],&tri[3]);

				/* Make Center */
				BiTmConstTrgCntr(&blk->c[0],&tri[0]);
				BiTmConstTrgCntr(&blk->c[1],&tri[3]);
			}
		}
	}

	ter->sdPlg[0]=(ter->side[0]==BI_ON ? (BIPOINT *)BiMalloc(sizeof(BIPOINT)*ter->xSiz*4) : NULL);
	ter->sdPlg[1]=(ter->side[1]==BI_ON ? (BIPOINT *)BiMalloc(sizeof(BIPOINT)*ter->zSiz*4) : NULL);
	ter->sdPlg[2]=(ter->side[2]==BI_ON ? (BIPOINT *)BiMalloc(sizeof(BIPOINT)*ter->xSiz*4) : NULL);
	ter->sdPlg[3]=(ter->side[3]==BI_ON ? (BIPOINT *)BiMalloc(sizeof(BIPOINT)*ter->zSiz*4) : NULL);

	if((ter->side[0]==BI_ON && ter->sdPlg[0]==NULL) ||
	   (ter->side[1]==BI_ON && ter->sdPlg[1]==NULL) ||
	   (ter->side[2]==BI_ON && ter->sdPlg[2]==NULL) ||
	   (ter->side[3]==BI_ON && ter->sdPlg[3]==NULL))
	{
		return BI_ERR;
	}

	BiMakeSideWall0(ter->sdPlg[0],ter);
	BiMakeSideWall1(ter->sdPlg[1],ter);
	BiMakeSideWall2(ter->sdPlg[2],ter);
	BiMakeSideWall3(ter->sdPlg[3],ter);

	BiSetPoint(&ter->bbox[0],min.x,min.y,min.z);
	BiSetPoint(&ter->bbox[1],max.x,min.y,min.z);
	BiSetPoint(&ter->bbox[2],min.x,max.y,min.z);
	BiSetPoint(&ter->bbox[3],max.x,max.y,min.z);

	BiSetPoint(&ter->bbox[4],min.x,min.y,max.z);
	BiSetPoint(&ter->bbox[5],max.x,min.y,max.z);
	BiSetPoint(&ter->bbox[6],min.x,max.y,max.z);
	BiSetPoint(&ter->bbox[7],max.x,max.y,max.z);

	return BI_OK;
}

static void BiTmGetBlkTrgs(BIPOINT *tri,BITER *ter,long x,long z)
{
	BITERRBLOCK *blk;
	BIPOINT ed[4];

	blk=BiTmSeekBlk(ter,x,z);
	BiTmGetBlkPoint(&ed[0],ter,x  ,z  );
	BiTmGetBlkPoint(&ed[1],ter,x+1,z  );
	BiTmGetBlkPoint(&ed[2],ter,x  ,z+1);
	BiTmGetBlkPoint(&ed[3],ter,x+1,z+1);

	switch(blk->lup)
	{
	default:
	case BI_ON:
		tri[0]=ed[1]; 	/* Left Twisted Order */
		tri[1]=ed[3];   /*  tri [2]--[1]     */
		tri[2]=ed[2];   /*      [5]\  |      */
		tri[3]=ed[0];   /*       | \\ |      */
		tri[4]=ed[1];   /*       |  \[0]     */
		tri[5]=ed[2];   /*      [3]--[4]     */
		break;
	case BI_OFF:
		tri[0]=ed[0];
		tri[1]=ed[3];   /*  tri [2]--[1]     */
		tri[2]=ed[2];   /*       |  /[5]     */
		tri[3]=ed[0];   /*       | // |      */
		tri[4]=ed[1];   /*      [0]/  |      */
		tri[5]=ed[3];   /*      [3]--[4]     */
		break;
	}
}

static void BiTmConstTrgNorm(BIPOINT *nom,BIPOINT *tri)
{
	BIPOINT v1,v2;
	BiSubPoint(&v1,&tri[2],&tri[1]);
	BiSubPoint(&v2,&tri[1],&tri[0]);
	BiOuterProduct(nom,&v1,&v2);
	BiNormalize(nom,nom);
}

static void BiTmConstTrgCntr(BIPOINT *cen,BIPOINT *tri)
{
	BIPOINT sum;
	BiAddPoint(&sum,&tri[0],&tri[1]);
	BiAddPoint(&sum,&sum,&tri[2]);
	BiDivPoint(cen,&sum,3.0);
}

static void BiTmGetCenter(BIPOINT *cen,BIPOINT tri[])
{
	BIPOINT trs[3];

	BiConvLtoG(&trs[0],&tri[0],&BiCnvAxs);
	BiConvLtoG(&trs[1],&tri[1],&BiCnvAxs);
	BiConvLtoG(&trs[2],&tri[2],&BiCnvAxs);

	if(trs[0].z>trs[1].z && trs[0].z>trs[2].z)
	{
		*cen=tri[0];
	}
	else if(trs[1].z>trs[2].z)
	{
		*cen=tri[1];
	}
	else
	{
		*cen=tri[2];
	}
}

static void BiTmGetBlkPoint(BIPOINT *pnt,BITER *ter,long x,long z)
{
	BITERRBLOCK *blk;

	blk=BiTmSeekBlk(ter,x,z);
	pnt->x=ter->xWid*(real)x;
	pnt->y=blk->y;
	pnt->z=ter->zWid*(real)z;
}

static void BiMakeSideWall0(BIPOINT *trg,BITER *ter)
{
	int i;
	BITERRBLOCK *blk0,*blk1;

	if(trg!=NULL)
	{
		for(i=0; i<ter->xSiz; i++)
		{
			blk0=BiTmSeekBlk(ter,i  ,0);
			blk1=BiTmSeekBlk(ter,i+1,0);
			BiSetPoint(&trg[i*4  ], i   *ter->xWid,0      ,0);
			BiSetPoint(&trg[i*4+1],(i+1)*ter->xWid,0      ,0);
			BiSetPoint(&trg[i*4+2],(i+1)*ter->xWid,blk1->y,0);
			BiSetPoint(&trg[i*4+3], i   *ter->xWid,blk0->y,0);
		}
	}
}

static void BiMakeSideWall1(BIPOINT *trg,BITER *ter)
{
	int i;
	BITERRBLOCK *blk0,*blk1;

	if(trg!=NULL)
	{
		for(i=0; i<ter->zSiz; i++)
		{
			blk0=BiTmSeekBlk(ter,ter->xSiz,i  );
			blk1=BiTmSeekBlk(ter,ter->xSiz,i+1);
			BiSetPoint(&trg[i*4  ],ter->xWid*ter->xSiz,0      , i   *ter->zWid);
			BiSetPoint(&trg[i*4+1],ter->xWid*ter->xSiz,0      ,(i+1)*ter->zWid);
			BiSetPoint(&trg[i*4+2],ter->xWid*ter->xSiz,blk1->y,(i+1)*ter->zWid);
			BiSetPoint(&trg[i*4+3],ter->xWid*ter->xSiz,blk0->y, i   *ter->zWid);
		}
	}
}

static void BiMakeSideWall2(BIPOINT *trg,BITER *ter)
{
	int i;
	BITERRBLOCK *blk0,*blk1;

	if(trg!=NULL)
	{
		for(i=0; i<ter->xSiz; i++)
		{
			blk0=BiTmSeekBlk(ter,i  ,ter->zSiz);
			blk1=BiTmSeekBlk(ter,i+1,ter->zSiz);
			BiSetPoint(&trg[i*4  ], i   *ter->xWid,0      ,ter->zWid*ter->zSiz);
			BiSetPoint(&trg[i*4+1],(i+1)*ter->xWid,0      ,ter->zWid*ter->zSiz);
			BiSetPoint(&trg[i*4+2],(i+1)*ter->xWid,blk1->y,ter->zWid*ter->zSiz);
			BiSetPoint(&trg[i*4+3], i   *ter->xWid,blk0->y,ter->zWid*ter->zSiz);
		}
	}
}

static void BiMakeSideWall3(BIPOINT *trg,BITER *ter)
{
	int i;
	BITERRBLOCK *blk0,*blk1;

	if(trg!=NULL)
	{
		for(i=0; i<ter->zSiz; i++)
		{
			blk0=BiTmSeekBlk(ter,0,i  );
			blk1=BiTmSeekBlk(ter,0,i+1);
			BiSetPoint(&trg[i*4  ],0,0      , i   *ter->zWid);
			BiSetPoint(&trg[i*4+1],0,0      ,(i+1)*ter->zWid);
			BiSetPoint(&trg[i*4+2],0,blk1->y,(i+1)*ter->zWid);
			BiSetPoint(&trg[i*4+3],0,blk0->y, i   *ter->zWid);
		}
	}
}

static BITERRBLOCK *BiTmSeekBlk(BITER *ter,long x,long z)
{
	BITERRBLOCK *blk;
	blk=&ter->blk[z*(ter->xSiz+1)+x];
	return blk;
}



/*****************************************************************************/
static void BiConvToTerrAxis(BIPOINT *pnt,real x,real z,BIPOSATT *pos);
static real BiTmGetHeight(real x,real z,BITER *ter,BIPOSATT *pos);
static void BiTmGetTrg(BIPOINT *t,real locx,real locz,BITER *ter);
static real BiTmGetHeightOnTrg(BIPOINT *tri,real x,real z);
static void BiTmGetEyeVec(BIPOINT *e,real x,real z,long h,BITER *ter,BIPOSATT *pos);
static void BiTmGetUpVec(BIPOINT *u,real x,real z,BITER *ter,BIPOSATT *pos);
static void BiTrgToOrgVxVy(BIPOINT *o,BIPOINT *vx,BIPOINT *vz,BIPOINT *t);

int BiAboveTer(real x,real z,BITER *ter,BIPOSATT *pos)
{
	real xw,zw;
	BIPOINT pnt;

	xw=ter->xWid*(real)ter->xSiz;
	zw=ter->zWid*(real)ter->zSiz;

	BiConvToTerrAxis(&pnt,x,z,pos);

	return ((0<pnt.x && pnt.x<xw && 0<pnt.z && pnt.z<zw) ? BI_IN : BI_OUT);
}

int BiTerHeight(real *y,real x,real z,BITER *ter,BIPOSATT *pos)
{
	if(BiAboveTer(x,z,ter,pos)==BI_IN)
	{
		*y=BiTmGetHeight(x,z,ter,pos);
		return BI_IN;
	}
	else
	{
		*y=0.0F;
		return BI_OUT;
	}
}

static void BiConvToTerrAxis(BIPOINT *pnt,real x,real z,BIPOSATT *pos)
{
	BiSetPoint(pnt,x,0,z);
	BiSubPoint(pnt,pnt,&pos->p);
	BiRotGtoL(pnt,pnt,&pos->a);
}

static real BiTmGetHeight(real x,real z,BITER *ter,BIPOSATT *pos)
{
	BIPOINT loc,tri[3];
	BiConvToTerrAxis(&loc,x,z,pos);
	BiTmGetTrg(tri,loc.x,loc.z,ter);
	return BiTmGetHeightOnTrg(tri,loc.x,loc.z);
}

static void BiTmGetTrg(BIPOINT *t,real locx,real locz,BITER *ter)
{
	long bx,bz;
	real ibx,ibz; /* x,z in Block */
	BITERRBLOCK *blk;
	BIPOINT tris[6],*tri;

	bx=(long)(locx/ter->xWid);
	bz=(long)(locz/ter->zWid);

	ibx=locx -(real)bx *ter->xWid;
	ibz=locz -(real)bz *ter->zWid;

	blk=BiTmSeekBlk(ter,bx,bz);
	BiTmGetBlkTrgs(tris,ter,bx,bz);
	switch(blk->lup)
	{
	default:
	case BI_ON:  // 1999/04/02  change < into >
		tri=(ibz/ter->zWid > 1.0-ibx/ter->xWid ? tris : tris+3);
		break;
	case BI_OFF:
		tri=(ibz/ter->zWid >     ibx/ter->xWid ? tris : tris+3);
		break;
	}

	t[0]=tri[0];
	t[1]=tri[1];
	t[2]=tri[2];
}

static real BiTmGetHeightOnTrg(BIPOINT *tri,real x,real z)
{
	BIPOINT vx,vz,o;
	BiTrgToOrgVxVy(&o,&vx,&vz,tri);
	return o.y +vx.y*(x-o.x)/vx.x +vz.y*(z-o.z)/vz.z;
}

int BiTerEyeVecUpVec(BIPOINT *e,BIPOINT *u,real x,real z,long h,BITER *ter,BIPOSATT *pos)
{
	if(BiAboveTer(x,z,ter,pos)==BI_IN)
	{
		BiTmGetEyeVec(e,x,z,h,ter,pos);
		BiTmGetUpVec(u,x,z,ter,pos);
		return BI_IN;
	}
	else
	{
		BiSetPoint(e,-BiSin(h),0,BiCos(h));
		BiSetPoint(u,0,1,0);
		return BI_OUT;
	}
}

static void BiTmGetEyeVec(BIPOINT *e,real x,real z,long h,BITER *ter,BIPOSATT *pos)
{
	real fx,fz;
	BIPOINT loc,tri[3],o,vx,vz;
	BIPOINT2 uni;

	BiSetPoint2(&uni,0,1);
	BiRot2(&uni,&uni,h-pos->a.h);

	BiConvToTerrAxis(&loc,x,z,pos);
	BiTmGetTrg(tri,loc.x,loc.z,ter);
	BiTrgToOrgVxVy(&o,&vx,&vz,tri);

	fx=vx.x;
	fz=vz.z;
	BiDivPoint(&vx,&vx,fx);
	BiDivPoint(&vz,&vz,fz);

	BiMulPoint(&vx,&vx,uni.x);
	BiMulPoint(&vz,&vz,uni.y);
	BiAddPoint(e,&vx,&vz);
	BiRotLtoG(e,e,&pos->a);
}

static void BiTmGetUpVec(BIPOINT *u,real x,real z,BITER *ter,BIPOSATT *pos)
{
	BIPOINT loc,tri[3],v1,v2;

	BiConvToTerrAxis(&loc,x,z,pos);
	BiTmGetTrg(tri,loc.x,loc.z,ter);
	BiSubPoint(&v1,&tri[1],&tri[0]);
	BiSubPoint(&v2,&tri[2],&tri[1]);

	BiOuterProduct(u,&v2,&v1);
	BiRotLtoG(u,u,&pos->a);
}

static void BiTrgToOrgVxVy(BIPOINT *o,BIPOINT *vx,BIPOINT *vz,BIPOINT *t)
{
	BIPOINT v1,v2;

	if(BiSame(t[0].x,t[1].x)!=BI_TRUE && BiSame(t[0].z,t[1].z)!=BI_TRUE)
	{
		BiSubPoint(&v1,&t[0],&t[2]);
		BiSubPoint(&v2,&t[1],&t[2]);
		*o = t[2];
	}
	else if(BiSame(t[1].x,t[2].x)!=BI_TRUE && BiSame(t[1].z,t[2].z)!=BI_TRUE)
	{
		BiSubPoint(&v1,&t[1],&t[0]);
		BiSubPoint(&v2,&t[2],&t[0]);
		*o = t[0];
	}
	else
	{
		BiSubPoint(&v1,&t[2],&t[1]);
		BiSubPoint(&v2,&t[0],&t[1]);
		*o = t[1];
	}

	if(BiAbs(v1.x) > YSEPS)
	{
		*vx=v1;
		*vz=v2;
	}
	else
	{
		*vx=v2;
		*vz=v1;
	}
}


/*****************************************************************************/
int BiInsTer(BITER *ter,BIPOSATT *pos)
{
	int i,x,z,clip;
	BIPOINT cen,bbx[8];
	BITERRBLOCK *blk;

	BiPushMatrix(pos);

	// 2002/05/09  Correct visibility check
	for(i=0; i<8; i++)
	{
		BiConvLtoG(&bbx[i],&ter->bbox[i],&BiCnvAxs);
	}
	if(BiCheckInsideViewPort(&clip,8,bbx)!=BI_IN)
	{
		BiPopMatrix();    // 2001/06/09
		return BI3DG_NOERR;
	}

	// Buggy code.  Commented out 2002/05/09
	//if(BiCheckInsideViewPort(&clip,8,ter->bbox)!=BI_IN)
	//{
	//	BiPopMatrix();    // 2001/06/09
	//	return BI3DG_NOERR;
	//}

	for(z=0; z<ter->zSiz; z++)
	{
		for(x=0; x<ter->xSiz; x++)
		{
			blk=BiTmSeekBlk(ter,x,z);
			if(blk->vis[0]==BI_ON)
			{
				BiTmGetCenter(&cen,blk->tr0);
				BiInsPolygFast(3,blk->tr0,&cen,&blk->n[0],&blk->col[0]);
			/*BiInsPolygFast(3,blk->tr0,&blk->c[0],&blk->n[0],&blk->col[0]);*/
			}
			if(blk->vis[1]==BI_ON)
			{
				BiTmGetCenter(&cen,blk->tr1);
				BiInsPolygFast(3,blk->tr1,&cen,&blk->n[1],&blk->col[1]);
			/*BiInsPolygFast(3,blk->tr1,&blk->c[1],&blk->n[1],&blk->col[1]);*/
			}
		}
	}

	for(i=0; i<4; i++)
	{
		static BIVECTOR nom[4]={{0.0F,0.0F,-1.0F},{1.0F,0.0F,0.0F},{0.0F,0.0F,1.0F},{-1.0F,0.0F,0.0F}};
		int j;
		if(ter->side[i]==BI_ON)
		{
			int siz;
			siz=((i%2)==0 ? ter->xSiz : ter->zSiz);
			for(j=0; j<siz; j++)
			{
				BiInsPolygFast(4,&ter->sdPlg[i][j*4],NULL,&nom[i],&ter->sdCol[i]);
			}
		}
	}

	BiPopMatrix();
	return BI3DG_NOERR;
}


/***********************************************************************
Terrain Mesh File Format

TerrMesh     <- Header
NBLOCK x z   <- Number of Blocks
TMSIZE x z   <- Real Size of Terrain Mesh
BOTTOM R G B <- Enable Botton Wall and Set Color
RIGHT R G B  <- Enable Right Wall and Set Color
LEFT R G B   <- Enable Left Wall and Set Color
TOP R G B    <- Enable Top Wall and Set Color
BLOCK y [L/R] [ON/OFF] R1 G1 B1 [ON/OFF] R2 G2 B2
END

in short
  NBLOCK -> NBL
  TMSIZE -> TMS
  BOTTOM -> BOT
  RIGHT  -> RIG
  LEFT   -> LEF
  TOP    -> TOP
  BLOCK  -> BLO
  END    -> END
***********************************************************************/
static int BiTmCheckFileHead(void);
static int BiTmLoadData(BITER *tm);
static void BiTmInitTer(BITER *tm);
static int BiTmExecFileCommand(BITER *tm,int ac,char *av[],int *blk);

static char *tmCmd[]=
{
	"NBL",
	"TMS",
	"BOT",
	"RIG",
	"LEF",
	"TOP",
	"BLO",
	"END",
	"REM",
	NULL
};

int BiLoadTer(BITER *tm,char fnOrg[])
{
	FILE *fp;
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"r");
	if(fp==NULL)
	{
		goto NOFILE;
	}

	BiPushStringFunc();
	BiSetFp(fp);

	if(BiTmCheckFileHead()!=BI_OK)
	{
		goto READERR;
	}

	if(BiTmLoadData(tm)!=BI_OK)
	{
		goto READERR;
	}

	fclose(fp);
	BiPopStringFunc();

	return BI_OK;
NOFILE:
	BiPopStringFunc();
	return BI_ERR;
READERR:
	fclose(fp);
	BiPopStringFunc();
	return BI_ERR;
}

int BiLoadTerFromString(BITER *tm,char *str[])
{
	BiPushStringFunc();
	BiSetStringList(str);

	if(BiTmCheckFileHead()!=BI_OK)
	{
		goto READERR;
	}

	if(BiTmLoadData(tm)!=BI_OK)
	{
		goto READERR;
	}

	BiPopStringFunc();
	return BI_OK;
READERR:
	BiPopStringFunc();
	return BI_ERR;
}




static int BiTmCheckFileHead(void)
{
	char str[256];

	BiGetStringFunc(str,255);
	BiCapitalizeString(str);
	if(strncmp(str,"TERRMESH",8)!=0)
	{
		return BI_ERR;
	}
	return BI_OK;
}

#define BI_END 3

static int BiTmLoadData(BITER *tm)
{
	int ac,i;
	char *av[16];
	char str[256];

	i=0;
	BiTmInitTer(tm);
	while(BiGetStringFunc(str,255)!=NULL)
	{
		BiArguments(&ac,av,15,str);
		if(ac>0)
		{
			switch(BiTmExecFileCommand(tm,ac,av,&i))
			{
			case BI_ERR:
				return BI_ERR;
			case BI_OK:
				break;
			case BI_END:
				BiConstTer(tm);
				return BI_OK;
			}
		}
	}

	return BI_ERR;
}

static void BiTmInitTer(BITER *tm)
{
	tm->blk=NULL;
	tm->side[0]=BI_OFF;
	tm->side[1]=BI_OFF;
	tm->side[2]=BI_OFF;
	tm->side[3]=BI_OFF;
	tm->xSiz=0;
	tm->zSiz=0;
	tm->xWid=100.0F;
	tm->zWid=100.0F;
}

static int BiTmExecNBlock(BITER *tm,int ac,char *av[]);
static int BiTmExecTmsize(BITER *tm,int ac,char *av[]);
static int BiTmExecBottom(BITER *tm,int ac,char *av[]);
static int BiTmExecRight(BITER *tm,int ac,char *av[]);
static int BiTmExecLeft(BITER *tm,int ac,char *av[]);
static int BiTmExecTop(BITER *tm,int ac,char *av[]);
static int BiTmExecBlock(BITER *tm,int ac,char *av[],int *blk);
static int BiTmExecEnd(BITER *tm,int ac,char *av[]);

static int BiTmExecFileCommand(BITER *tm,int ac,char *av[],int *blk)
{
	int cmd,sta;

	if(BiCommandNumber(&cmd,av[0],tmCmd)==BI_OK)
	{
		switch(cmd)
		{
		case 0:	/* "NBL" */
			sta=BiTmExecNBlock(tm,ac,av);
			break;
		case 1:	/* "TMS" */
			sta=BiTmExecTmsize(tm,ac,av);
			break;
		case 2:	/* "BOT" */
			sta=BiTmExecBottom(tm,ac,av);
			break;
		case 3:	/* "RIG" */
			sta=BiTmExecRight(tm,ac,av);
			break;
		case 4:	/* "LEF" */
			sta=BiTmExecLeft(tm,ac,av);
			break;
		case 5:	/* "TOP" */
			sta=BiTmExecTop(tm,ac,av);
			break;
		case 6:	/* "BLO" */
			sta=BiTmExecBlock(tm,ac,av,blk);
			break;
		case 7:	/* "END" */
			sta=BiTmExecEnd(tm,ac,av);
			break;
		case 8: /* "REM" */
			sta=BI_OK;
			break;
		}
		return sta;
	}
	return BI_ERR;
}

static int BiTmExecNBlock(BITER *tm,int ac,char *av[])
{
	/* NBLOCK x z   <- Number of Blocks */
	if(ac>=3)
	{
		int nblk;
		tm->xSiz=atoi(av[1]);
		tm->zSiz=atoi(av[2]);
		nblk=(atoi(av[1])+1)*(atoi(av[2])+1);
		tm->blk=(BITERRBLOCK *)BiMalloc(nblk*sizeof(BITERRBLOCK));
		if(tm->blk==NULL)
		{
			return BI_ERR;
		}
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecTmsize(BITER *tm,int ac,char *av[])
{
	/* TMSIZE x z   <- Real Size of Terrain Mesh */
	if(ac>=3)
	{
		tm->xWid=(real)atof(av[1]);
		tm->zWid=(real)atof(av[2]);
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecBottom(BITER *tm,int ac,char *av[])
{
	/* BOTTOM R G B <- Enable Botton Wall and Set Color */
	if(ac>=4)
	{
		tm->side[0]=BI_ON;
		BiSetColorRGB(&tm->sdCol[0],atoi(av[1]),atoi(av[2]),atoi(av[3]));
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecRight(BITER *tm,int ac,char *av[])
{
	/* RIGHT R G B  <- Enable Right Wall and Set Color */
	if(ac>=4)
	{
		tm->side[1]=BI_ON;
		BiSetColorRGB(&tm->sdCol[1],atoi(av[1]),atoi(av[2]),atoi(av[3]));
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecLeft(BITER *tm,int ac,char *av[])
{
	/* LEFT R G B   <- Enable Left Wall and Set Color */
	if(ac>=4)
	{
		tm->side[2]=BI_ON;
		BiSetColorRGB(&tm->sdCol[2],atoi(av[1]),atoi(av[2]),atoi(av[3]));
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecTop(BITER *tm,int ac,char *av[])
{
	/* TOP R G B    <- Enable Top Wall and Set Color */
	if(ac>=4)
	{
		tm->side[3]=BI_ON;
		BiSetColorRGB(&tm->sdCol[3],atoi(av[1]),atoi(av[2]),atoi(av[3]));
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecBlock(BITER *tm,int ac,char *av[],int *blk)
{
	/* BLOCK y [L/R] [ON/OFF] R1 G1 B1 [ON/OFF] R2 G2 B2 */
	if(ac>=2 && tm->xSiz>=0 && tm->zSiz>=0)
	{
		tm->blk[*blk].y=(real)atof(av[1]);
		tm->blk[*blk].lup=BI_OFF;
		tm->blk[*blk].vis[0]=BI_OFF;
		tm->blk[*blk].vis[1]=BI_OFF;
		BiSetColorRGB(&tm->blk[*blk].col[0],0,0,0);
		BiSetColorRGB(&tm->blk[*blk].col[1],0,0,0);

		if(ac>=3)
		{
			char l;
			int sw,r,g,b;
			l=av[2][0];
			tm->blk[*blk].lup=((l=='L' || l=='l') ? BI_ON : BI_OFF);

			if(ac>=11)
			{
				sw=(strcmp(av[3],"ON")==0 ? BI_ON : BI_OFF);
				r=atoi(av[4]);
				g=atoi(av[5]);
				b=atoi(av[6]);
				tm->blk[*blk].vis[0]=sw;
				BiSetColorRGB(&tm->blk[*blk].col[0],r,g,b);

				sw=(strcmp(av[7],"ON")==0 ? BI_ON : BI_OFF);
				r=atoi(av[8]);
				g=atoi(av[9]);
				b=atoi(av[10]);
				tm->blk[*blk].vis[1]=sw;
				BiSetColorRGB(&tm->blk[*blk].col[1],r,g,b);
			}
		}
		(*blk)++;
		return BI_OK;
	}
	return BI_ERR;
}

static int BiTmExecEnd(BITER *tm,int ac,char *av[])
{
	/* END */
	/* Check if it is correct. */
	return BI_END;
}







