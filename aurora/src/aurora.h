#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <impulse.h>




#ifdef __cplusplus
	extern "C" {
#endif




/***********************************************************************/

int ArInitialize(void);


/***********************************************************************
    adyna.c
    Dynamic Model  [
***********************************************************************/
enum {
	ARM_TOPPARENT,   /* Ignore cnt and treat mtn as Absolute Position */
	ARM_DEPENDENT,   /* Depend on 1-Level Upper Object */
	ARM_INDEPENDENT, /* Independent */
	ARM_DEPENDONTOP  /* Depend on Top Object */
};

enum {
	ARM_NULL,
	ARM_SRFMODEL,
	ARM_POLYGON,
	ARM_LINE
};

typedef union {
	BISRF srf;
	BILIMLINE lin;
	BIPOLYGON plg;
} ARDNMSHAPE;

typedef struct {
	int disp;
	BIPNTANG pos;
} ARDNMSTATUS;


#define ARM_MAX_N_STATUS 64

typedef struct ArDynaNodeTag {
	int id;                       /* Own ID */

	int classId;                 /* Class ID */

	struct ArDynaNodeTag *next;  /* Brother Object */

	struct ArDynaNodeTag *par;   /* Parent Object */
	BIPNTANG cntPar;              /* Connecting Position & Attitude of Parent Coordinate*/
	BIPOINT cntCld;               /* Connecting Position of Child(itself) Coordinate */
	BIPOINT cev,cuv;              /* Eye Vector & Up Vector of Connecting Attitude */
	int rel;                      /* Relation */
	BIPNTANG mtn;                 /* Motion Position & Attitude */

	struct ArDynaNodeTag *cld;   /* Child Object List */

	int disp;                     /* Display On/Off */
	int type;                     /* Type of Shape */
	ARDNMSHAPE shape;        /* Shape */

	BICOLOR col;                  /* Color (for ARM_LINE & ARM_POLYG) */
	BIPNTANG abs;                 /* Absolute Position (to be set) */

	int nSta;                     /* Max # of Status */
	ARDNMSTATUS sta[ARM_MAX_N_STATUS];
} ARDYNANODE;

typedef struct {
	int RootId;                  /* Serial No. for aurora share */
	int nObj;                    /* # of Objects */
	ARDYNANODE *(*obj);          /* Object List */
	ARDYNANODE *top;             /* Top Parent Object */
} ARDNM;

int ArExecLoadDnm(ARDNM *neo,char fnOrg[]);
int ArExecLoadDnmFromString(ARDNM *dnm,char *lst[]);
int ArInitDnm(ARDNM *neo);
int ArAddDnmSrf(int *id,ARDNM *neo,BISRF *srf,int pid,BIPNTANG *par,BIPOINT *cld);
int ArAddDnmPolyg(int *id,ARDNM *neo,BIPOLYGON *plg,BICOLOR *col,int pid,BIPNTANG *par,BIPOINT *cld);
int ArAddDnmLine(int *id,ARDNM *neo,BILIMLINE *lin,BICOLOR *col,int pid,BIPNTANG *par,BIPOINT *cld);
int ArSetDnmRelation(ARDNM *ptr,int id,int rel);
int ArCopyDnm(ARDNM *neo,ARDNM *org);
int ArFreeDnm(ARDNM *mdl);

int ArSetDnmRelativePosition(ARDNM *mdl,int id,BIPNTANG *pos);
int ArGetDnmRelativePosition(BIPNTANG *pos,ARDNM *mdl,int id);
int ArSetDnmConnectingPosition(ARDNM *mdl,int id,BIPNTANG *pos);
int ArGetDnmConnectingePosition(BIPNTANG *pos,ARDNM *mdl,int id);
int ArSetDnmRelativeAttitude(ARDNM *mdl,int id,BIANGLE *att);
int ArGetDnmRelativeAttitude(BIANGLE *att,ARDNM *mdl,int id);
int ArSetDnmDrawSwitch(ARDNM *mdl,int id,int disp);
int ArGetDnmDrawSwitch(int *disp,ARDNM *mdl,int id);
int ArSetDnmClassId(ARDNM *mdl,int id,int classId);
int ArGetDnmClassId(int *classId,ARDNM *mdl,int id);

int ArCalcDnmAbsPosition(ARDNM *neo,BIPNTANG *pos);
int ArGetDnmAbsPosition(BIPNTANG *pos,ARDNM *mdl,int id);
int ArInsDnm(ARDNM *neo,BIPNTANG *pos);
int ArInsDnmDivide(ARDNM *neo,BIPNTANG *pos);
int ArOvwDnmShadow(ARDNM *neo,BIPNTANG *pos);

int ArGetDnmNumObj(int *nObj,ARDNM *mdl);
int ArAddDnmStatus(int *stId,ARDNM *mdl,int id,int disp,BIPNTANG *pos);
int ArGetDnmStatus(int *disp,BIPNTANG *pos,ARDNM *mdl,int id,int stId);
int ArSetDnmStatus(ARDNM *mdl,int id,int stId);
int ArInBetweenDnmStatus(ARDNM *mdl,int id,int st1,int st2,real t);

/***************************************************************************
   ]
***************************************************************************/


/***********************************************************************
    asmoke.c
    Smoke Routine   [
***********************************************************************/
typedef struct {
	real t0,t1;         /* life t0 -> t1 */
	real iniw,maxw,dw;  /* Initial Width,Maximum Width,Width/Sec */
	BICOLOR inic,endc;  /* Initial Color,End Color */
	real tc;            /* Time between Initial Color and End Color (sec) */
} ARSMOKEATTR;


typedef struct {
	BIAXIS axs;
	BIPOINT left,up;
	real t;
} ARSMOKENODE;

#define ARS_MAX_TIP_PER_INST 8

typedef struct {
	int nMax;          /* Maximum Number of Smoke Nodes */
	int nDel;          /* Number of Objects which are deleted when overflow */
	int nPth;          /* Number of Smoke Nodes which is actually used */
	int nTip;          /* Number of Smoke Tips */
	int tip[ARS_MAX_TIP_PER_INST*2];  /* Tips */
	ARSMOKENODE *pth;  /* Smoke Node */
} ARSMOKEINST;


#define ARS_RIBBONSMOKE 1
#define ARS_WIRESMOKE   2
#define ARS_TRAILSMOKE  4
#define ARS_SOLIDSMOKE  8
#define ARS_NETSMOKE   16
#define ARS_ALLSMOKE   ~0

typedef struct {
	int stp[3];     /* Step inside [0]:Min BBox [1]:Mid Bbox [3]:Max BBox */
	BIPOINT bbx[3]; /* Bounding Box  [0]:min  [1]:mid  [2]:max */
	int sw;
	ARSMOKEATTR rbn;
	ARSMOKEATTR wir;
	ARSMOKEATTR trl;
	ARSMOKEATTR sld;
	ARSMOKEATTR net;
} ARSMOKECLASS;


int ArInitSmokeClass(ARSMOKECLASS *s);

int ArSetSmokeBoundingBox(ARSMOKECLASS *s,int stp[3],BIPOINT bbx[2]);

int ArSetSmokeType(ARSMOKECLASS *s,int attr);
int ArGetSmokeType(int *attr,ARSMOKECLASS *s);
int ArSetSmokeLife(ARSMOKECLASS *s,int type,real t0,real t1);
int ArSetSmokeWidth(ARSMOKECLASS *s,int type,real w0,real w1,real dw);
int ArSetSmokeColor(ARSMOKECLASS *s,int type,BICOLOR *c0,BICOLOR *c1,real t);


int ArInitSmokeInstance(ARSMOKEINST *i,int nNode,int nDelWhenOverflow);

int ArBeginAppendSmokeNode(ARSMOKEINST *i);
int ArAppendSmokeNode(ARSMOKEINST *i,BIPOSATT *pos,real t);
int ArEndAppendSmokeNode(ARSMOKEINST *i);

int ArClearSmokeInstance(ARSMOKEINST *inst);
int ArFreeSmokeInstance(ARSMOKEINST *inst);

int ArInsSmoke(ARSMOKECLASS *cla,ARSMOKEINST *inst,real ctim,BIPOSATT *eye);

/***************************************************************************
   ]
***************************************************************************/


/***************************************************************************
    apath.c
    Motion Path Library [
***************************************************************************/
/* #define MBOX(a) HtMessageBox(a,NULL) */
#define MBOX(a)

#define ARP_MAX_NUM_OBJ   64
#define ARP_MAX_NUM_NODE 128
#define ARP_MAX_NUM_SMOKE 64
#define ARP_MAX_NUM_STATUS 8
#define ARP_MAX_NUM_MAG   16
#define ARP_MAX_NUM_CAMERA 8

#define ARP_DEFAULT_T0      0
#define ARP_DEFAULT_T1  10000


enum {
	ARMP_NOTSPECIFIED,  /* Default */
	ARMP_FIXEDCAM,      /* Fixed Point Camera */
	ARMP_ONBOARDCAM,    /* On Board Camera */
	ARMP_STATICCAM      /* Static Camera */
};

typedef struct {
	int id;             /* Object on which the Camera is */
} ARMPONBOARDCAM;

typedef struct {
	int id;             /* Object to Focus on */
	BIPOINT pos;        /* Fixed Camera Position */
} ARMPFIXEDCAM;

typedef struct {
	BIPOSATT pos;       /* Static Camera Position and Attitude */
} ARMPSTATICCAM;

typedef union {
	ARMPONBOARDCAM obc;
	ARMPFIXEDCAM fxc;
	ARMPSTATICCAM stc;
} ARMPCAMERAUNION;

typedef struct {
	real t;
	real magx,magy;
	real rootx,rooty;
} ARMPMAGNIFY;

typedef struct {
	int typ;              /* Camera Type */
	ARMPCAMERAUNION atr;  /* Camera Attribute */
	int nMag;
	ARMPMAGNIFY *mag;
} ARMPCAMERA;


enum {
	ARMP_NOLIGHT,
	ARMP_FIXEDLIGHT,
	ARMP_ONBOARDLIGHT
};

typedef struct {
	int id;
} ARMPONBOARDLIGHT;

typedef struct {
	BIPOINT pos;
} ARMPFIXEDLIGHT;

typedef union {
	ARMPONBOARDLIGHT obl;
	ARMPFIXEDLIGHT fxl;
} ARMPLIGHTUNION;

typedef struct {
	int typ;
	ARMPLIGHTUNION atr;
} ARMPLIGHT;


enum {
	ARMP_FORWARD,         /* Go Forward */
	ARMP_ACCEL,           /* Go Forward with acceleration */
	ARMP_PROCEED,         /* Proceed */
	ARMP_MOVETO,          /* Move To Relative To Initial Position*/
	ARMP_MOVETOREL,       /* Move To Relative To Parent Position*/
	ARMP_MOVETOSMO,       /* Move To Smooth */
	ARMP_MOVETOSMOREL,    /* Move To Smooth & Relative */
	ARMP_KEEPRELPOS,      /* Keep Relative Position */
	ARMP_PARALLEL,        /* Pararel Position */
	ARMP_CONSTENERGY,     /* Go Forward with constant energy */
	ARMP_CHASE,           /* Chase leader */
	ARMP_RAWDATA,         /* Point Sequence */
	ARMP_MOVETOABS        /* Move To Absolute Position */
};

typedef struct {
	real spd;    /* Move Forward Constant Velocity */
} ARMPFWD;

typedef struct {
	real spd1,spd2;    /* Move Forward Accelerating/Decerating Speed */
} ARMPACC;

typedef struct {
	BIPOINT v;   /* Move Keep Relative Vector in itsself's Coordsys */
} ARMPPRC;

typedef struct {
	BIPOINT p;   /* Move to Relative position in to nod->iniP */
} ARMPMTL;

typedef struct {
	int id;
	BIPOINT p;   /* Move to Relative Position in Parent Axis */
} ARMPMTR;

typedef struct {
	int id;
} ARMPKRP;

typedef struct {
	real spd1;    /* Conservation of energy */
} ARMPENG;

typedef struct {
	int id;
} ARMPCHS;

typedef struct {
	real t,relt;  /* relt is for calculating path (not on file) */
	BIPOSATT p;
} ARMPRAWDATANODE;

typedef struct {
	int nNode;
	ARMPRAWDATANODE *node;
} ARMPRAW;

typedef struct {
	BIPOINT p;   /* Move to Relative position in to nod->iniP */
} ARMPABS;

typedef union {
	ARMPFWD fwd;
	ARMPACC acc;
	ARMPPRC prc;
	ARMPMTL mtl;
	ARMPMTR mtr;
	ARMPKRP krp;
	ARMPKRP prl;
	ARMPENG eng;
	ARMPCHS chs;
	ARMPRAW raw;
	ARMPABS abs;
} ARMPMOTIONUNION;

typedef struct {
	int sty;
	ARMPMOTIONUNION mot;
} ARMPMOTION;


enum {
	ARMP_HPB,             /* Heading,Pitching,Bank */
	ARMP_PYR,             /* PitchUp,Yawing,Rolling */
	ARMP_KRA,             /* Keep Relative Attitude */
    ARMP_FOCUS            /* Focus on object */
};

enum {
	ARMP_NOSMOOTH,
	ARMP_SMOOTH
};

typedef struct {
	BIATTITUDE att;
} ARMPHPB;

typedef struct {
	long pit,yaw,rot;
} ARMPPYR;

typedef struct {
	int id;
} ARMPKRA;

typedef struct {
	int id;
} ARMPFCS;

typedef union {
	ARMPHPB hpb;
	ARMPPYR pyr;
	ARMPKRA kra;
	ARMPFCS fcs;
} ARMPROTATIONUNION;

typedef struct {
	int sty,smooth;
	ARMPROTATIONUNION rot;
} ARMPROTATION;


typedef struct {
	long t;              /* Time Span / mpa->clk */
	ARMPMOTION mot;      /* Motion */
	ARMPROTATION rot;    /* Rotation */

	long t0,t1;          /* t0,t1 (to be calculated) */
	BIPOSATT iniP;       /* Initial Position (to be calculated) */
} ARMPNODE;


typedef struct {
	real t0,t1;
} ARMPSMOKETIME;

typedef struct {
	int alive;

	int sty;
	real t0,t1;
	real iniw,maxw,dw;
	BICOLOR inic,endc;
	real tc;

	BIPOINT gen;

	int stp[3];
	BIPOINT bbx[2];

	int nSmk;
	ARMPSMOKETIME smk[ARP_MAX_NUM_SMOKE];
} ARMPSMOKE;

typedef struct {
	real t;
	int s;
} ARMPDYNASTA;

enum {
	ARMP_VIRTUAL,      /* Virtual (Invisible) Object */
	ARMP_SRF,          /* SRF Polygon Model */
	ARMP_DNM           /* DNM Dynamic Model */
};

typedef union {
	BISRF srf;
	ARDNM dnm;
} ARMPSHAPEUNION;

typedef struct {
	int typ;             /* Type of Shape */
	char fn[256];        /* File Name */
	ARMPSHAPEUNION shp;  /* Object Shape */
} ARMPSHAPE;

typedef struct {
	int alive;        /* Alive ? */

	long t0,t1;       /* Existing Time (including Smoke) */
	ARMPSHAPE shp;    /* Shape */
	BIPOSATT iniP;    /* Initial Position and Attitude */
	int iniS;         /* Initial Status of Dynamic Model */
	int nNod;         /* Number of Motion Path Node */
	ARMPNODE *nod;    /* Motion Path Node */

	ARMPSMOKE smk[4];
	int nSta;
	ARMPDYNASTA sta[ARP_MAX_NUM_STATUS];

	/* For CalcCompletePath */
	int nPath;             /* Complete Path (to be calculated) */
	BIPOSATT *path;        /* Complete Path (to be calculated) */
	long tCalced;          /* Last Calculated Time */
	BIPOSATT last;         /* Last Calculated Position */

	/* For Motion Play */
	BIPOSATT pPos;         /* Position */
	int pS1,pS2;           /* Status1,2 for Dynamic Model */
	real pParam;           /* Parameter for Dynamic Model */
	int pSmokeExist[4];    /* Smoke Exsitance */
	ARSMOKECLASS pSmokeClass[4];/* Smoke Class */
	ARSMOKEINST pSmoke[4];      /* Smoke Instance */
	int ppSmk[4];          /* Previous Smoke Switch */
} ARMPOBJECT;


typedef struct {
	char nam[256];    /* Motion Name */

	char fld[256];    /* Field File Name */

	int nCam;
	ARMPCAMERA cam[ARP_MAX_NUM_CAMERA];   /* Camera */
	ARMPLIGHT lig;    /* Light */
	real clk;         /* Time Increment Count of Virtual Clock */
	int nObj;         /* Number of Objects */
	ARMPOBJECT *obj;  /* Object */

	/* For setting */
	int cNod;
	ARMPNODE *nBuf;
	int cObj;
	ARMPOBJECT *oBuf;

	/* For Motion Play */
	real pTim;
	BIPOINT pLig;
} ARMPA;


int ArLoadMpa(ARMPA *mpa,BIFLD *fld,char fn[]);/* fld can be NULL */
int ArLoadMpaFromString(ARMPA *mpa,BIFIELD *fld,char *lst[]);

int ArBeginSetPath(ARMPA *mpa);
 int ArPathSetMpaName(ARMPA *mpa,char name[]);
 int ArPathSetFixedCamera(ARMPA *mpa,BIPOINT *pos,int id);
 int ArPathSetOnBoardCamera(ARMPA *mpa,int id);
 int ArPathSetStaticCamera(ARMPA *mpa,BIPOSATT *pos);
 int ArPathSetFixedLight(ARMPA *mpa,BIPOINT *pos);
 int ArPathSetOnBoardLight(ARMPA *mpa,int id);
 int ArPathSetMagnification(ARMPA *mpa,real t,real magx,real magy);
 int ArPathSetClockCount(ARMPA *mpa,real clk);
 int ArPathBeginSetObject(ARMPA *mpa);
  int ArPathSetObjectIsDead(ARMPA *mpa);
  int ArPathSetObjectShape(ARMPA *mpa,char fn[]);
  int ArPathSetObjectVirtual(ARMPA *mpa);
  int ArPathSetObjectTime(ARMPA *mpa,real t0,real t1);
  int ArPathSetObjectInitialPosition(ARMPA *mpa,BIPOINT *pos);
  int ArPathSetObjectInitialAttitude(ARMPA *mpa,BIATTITUDE *att);
  int ArPathSetObjectInitialStatus(ARMPA *mpa,int sta);
  int ArPathSetObjectSmokeClass(ARMPA *mpa,int id,int sty,real t0,real t1,real iw,real ew,real dw,BICOLOR *c);
  int ArPathSetObjectSmokeBbox(ARMPA *mpa,int id,int stp[3],BIPOINT bbx[2]);
  int ArPathSetObjSmokeOffset(ARMPA *mpa,int id,BIPOINT *ofs);
  int ArPathSetObjectSmokeTime(ARMPA *mpa,int id,real t0,real t1);
  int ArPathSetObjectStatus(ARMPA *mpa,real t,int s);
  int ArPathBeginSetNode(ARMPA *mpa);
   int ArPathSetNodeTime(ARMPA *mpa,real t);
   int ArPathSetNodeFWD(ARMPA *mpa,real spd);
   int ArPathSetNodeACC(ARMPA *mpa,real spd1,real spd2);
   int ArPathSetNodePRC(ARMPA *mpa,BIPOINT *v);
   int ArPathSetNodeMTL(ARMPA *mpa,BIPOINT *p);
   int ArPathSetNodeMTR(ARMPA *mpa,int par,BIPOINT *p);
   int ArPathSetNodeMTS(ARMPA *mpa,BIPOINT *p);
   int ArPathSetNodeMSR(ARMPA *mpa,int par,BIPOINT *p);
   int ArPathSetNodeKRP(ARMPA *mpa,int par);
   int ArPathSetNodePRL(ARMPA *mpa,int par);
   int ArPathSetNodeENG(ARMPA *mpa,real spd1);
   int ArPathSetNodeCHS(ARMPA *mpa,int par);
   int ArPathSetNodeRAW(ARMPA *mpa,int nNode,ARMPRAWDATANODE *node);
   int ArPathSetNodeABS(ARMPA *mpa,BIPOINT *p);
   int ArPathSetNodeHPB(ARMPA *mpa,long vh,long vp,long vb);
   int ArPathSetNodePYR(ARMPA *mpa,long vp,long by,long vr);
   int ArPathSetNodeKRA(ARMPA *mpa,int id);
   int ArPathSetNodeFCS(ARMPA *mpa,int id);
   int ArPathSetNodeSmoothRotation(ARMPA *mpa);
  int ArPathEndSetNode(ARMPA *mpa);
 int ArPathEndSetObject(ARMPA *mpa);
int ArEndSetPath(ARMPA *mpa);

int ArFreeMpa(ARMPA *mpa);

int ArCalcCompletePath(ARMPA *mpa);
int ArEraseCompletePath(ARMPA *mpa);
int ArInitPlayMpa(ARMPA *mpa,int SmokeLength,int SmokeNumDelete);
int ArEndPlayMpa(ARMPA *mpa);
int ArRewindMpa(ARMPA *mpa);
int ArPlayMpa(ARMPA *mpa,real CurrentTime);
int ArGetMpaCamera(BIPOSATT *cam,ARMPA *mpa,int cId);
int ArGetMpaLight(BIPOINT *lig,ARMPA *mpa);
int ArInsMpa(ARMPA *mpa,int detail);
int ArInsMpaSmoke(ARMPA *mpa,BIPOSATT *eye);
int ArOvwMpaShadow(ARMPA *mpa);

int ArGetMpaName(char nam[],ARMPA *mpa);

int ArGetMpaMagnification(real *mx,real *my,ARMPA *mpa,int cId);
int ArGetMpaActualTime(real *t0,real *t1,ARMPA *mpa);

int ArGetMpaHasField(ARMPA *mpa);

int ArGetMpaObjPosition(BIPOSATT *p,ARMPA *mpa,int id,real t);
int ArGetMpaObjShapeType(int *type,ARMPA *mpa,int id);
int ArGetMpaNumObj(int *nObj,ARMPA *mpa);


/***************************************************************************
   ]
***************************************************************************/

/***************************************************************************
   AURORA SHARE
   Anti Duplicate File Read Routine
   [
***************************************************************************/

int ArSetAuroraShare(int sw);
int ArGetAuroraShareIsWorking(int *sw);

int ArLoadSrf(BISRF *srf,char fn[]);
int ArLoadSrfFromString(BISRF *srf,char *lst[]);
int ArFreeSrf(BISRF *srf);

int ArLoadFld(BIFLD *fld,char fn[]);
int ArLoadFldFromString(BIFLD *fld,char *lst[]);
int ArFreeFld(BIFLD *fld);

int ArLoadPc2(BIPC2 *pc2,char fn[]);
int ArLoadPc2FromString(BIPC2 *pc2,char *lst[]);
int ArFreePc2(BIPC2 *pc2);

int ArLoadTer(BITER *ter,char fn[]);
int ArLoadTerFromString(BITER *ter,char *lst[]);
int ArFreeTer(BITER *ter);

int ArLoadDnm(ARDNM *dnm,char fn[]);
int ArLoadDnmFromString(ARDNM *dnm,char *lst[]);
int ArFreeDnm(ARDNM *dnm);

int ArBeginPackedObject(void);
int ArEndPackedObject(void);
int ArInsertPackedObject(char fn[],int nl);
int ArGetFirstLineOfPackedObj(char str[],char fn[]);
int ArLoadSrfFromPack(BISRF *srf,char fn[]);
int ArLoadPc2FromPack(BIPC2 *pc2,char fn[]);
int ArLoadTerFromPack(BITER *ter,char fn[]);
int ArLoadFldFromPack(BIFLD *fld,char fn[]);
int ArLoadDnmFromPack(ARDNM *dnm,char fn[]);

int ArIncrementDnmReferenceCount(ARDNM *dnm);


/***************************************************************************
   ]
***************************************************************************/



#define ArLoadMPA ArLoadMpa
#define ArFreePath ArFreeMpa

/* Following are definitions for Compatibility of SurfView(BI-UTIL) */
#define ARDYNAMDL ARDNM
#define ArGetDynaMdlNumObj       ArGetDnmNumObj
#define ArInBetweenDynaMdlStatus ArInBetweenDnmStatus
#define ArInsDynaMdlDivide       ArInsDnmDivide


#ifdef __cplusplus
	} /* extern "C" */
#endif
