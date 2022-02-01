/* High Transportable Real-Time 3D Graphics Library  Blue Impulse */
/*                                    Begin Nov. 9th 1994         */



#ifndef __IMPULSE_H  /* { */
#define __IMPULSE_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef __REAL_DEFINED
#define __REAL_DEFINED
typedef double real;
#endif


#ifndef YSPI
#define YSPI 3.14159265
#endif

#ifndef YSEPS
#define YSEPS 0.0001
#endif

enum {
	BI_FALSE,
	BI_TRUE
};
enum {
	BI_OFF,
	BI_ON
};
enum {
	BI_OUT,
	BI_IN
};
enum {
	BI_ERR,
	BI_OK
};

#define BiLarger(a,b) ((a)>(b) ? (a) : (b))
#define BiSmaller(a,b) ((a)<(b) ? (a) : (b))

/******************************************************************************

                Blue Impulse - Calculating Library

******************************************************************************/

/* Primitive */
#define BiAbs(a) ((a)>=0 ? (a) : -(a))
#define BiSgn(a) ((a)>=0 ?  1  :  -1 )
#define BiSqr(a) ((a)*(a))

/* 2002/08/14 following 3 lines float->real */
#define BiSin(a) (real)sin(((real)(a))*YSPI/32768.0F)
#define BiCos(a) (real)cos(((real)(a))*YSPI/32768.0F)
#define BiTan(a) (real)tan(((real)(a))*YSPI/32768.0F)

typedef struct {
	real x,y,z;
} BIPOINT;

typedef BIPOINT BIVECTOR;

typedef struct {
	BIPOINT p1,p2;
} BILIMLINE;

typedef struct {
	BIPOINT o,v;
} BILINE;

typedef struct {
	int np;
	BIPOINT *p;
} BIPOLYGON;

typedef struct {
	long h,p,b;
} BIANGLE;

typedef struct {
	float h,p,b;
} BIANGLEF;

typedef BIANGLE BIATTITUDE;

typedef struct {
	BIPOINT p;
	BIANGLE a;
} BIPNTANG;

typedef BIPNTANG BIPOSATT;

typedef struct {
	real x,y;
} BIPOINT2;

typedef struct {
	BIPOINT2 p1,p2;
} BILIMLINE2;

typedef struct {
	BIPOINT2 o,v;
} BILINE2;

typedef struct {
	int np;
	BIPOINT2 *p;
} BIPOLYGON2;

typedef struct {
	real sinh,cosh;
	real sinp,cosp;
	real sinb,cosb;
} BITRIGON;

#define BiMakeTrigonomy(trg,src)   \
{	(trg)->sinh = BiSin((src)->h); \
	(trg)->cosh = BiCos((src)->h); \
	(trg)->sinp = BiSin((src)->p); \
	(trg)->cosp = BiCos((src)->p); \
	(trg)->sinb = BiSin((src)->b); \
	(trg)->cosb = BiCos((src)->b); }

typedef struct {
	BIPOINT o,n;
} BIPLANE;

typedef struct {
	BIPOINT p;
	BIANGLE a;
	BITRIGON t;
} BIAXIS;

typedef struct {
	BIPOINT p;
	BIANGLEF a;
	BITRIGON t;
} BIAXISF;

#define BiMakeAxis(dst,src) BiPntAngToAxis((dst),(src))

/******* Screen Projection *******/
typedef struct {
	long lx,ly;
	long cx,cy;
	real magx,magy;
	real nearz,farz;
} BIPROJ;

typedef struct {
	long x,y;   /* cf. Screen Coord */
} BIPOINTS;

typedef struct {
	unsigned g,r,b;
} BICOLOR;

/******* 2D Picture *******/
enum {
	BIP2_PSET,
	BIP2_POLYLINE,
	BIP2_LINESEQ,
	BIP2_POLYG,
	BIP2_ENDPICT,
	BIP2_REM
};

typedef struct {
	int n;
	BIPOINT2 *p;
} BIPC2NPOINT;

typedef union {
	BIPC2NPOINT npnt;
} BIPC2OBJUNI;

typedef struct {
	int objType;
	real visiDist;
	BIPOINT2 cen;
	BICOLOR col;
	BIPC2OBJUNI objUni;
} BIPC2OBJ;

typedef struct {
	int nObj;
	BIPOINT2 min,max,siz;
	BIPC2OBJ *obj;
} BIPC2;



/******* SRF Model *******/
typedef struct {
	BIPOINT p,n;
	int r;
} BISRFVERTEX;

typedef struct {
	int backFaceRemove;
	BICOLOR col;
	BIPOINT normal;
	BIPOINT center;
	int nVt,*vt,bright;
} BISRFPOLYGON;

typedef struct {
	BIPOINT bbox[8];
	int *vtTab;
	int nv;
	BISRFVERTEX *v,*tmp; /* tmp is used for temporary Calculation */
	int np;
	BISRFPOLYGON *p;
} BISRF;



/* Terrain Mesh */
typedef struct {
/* public */
	real y;
	int lup,vis[2];
	BICOLOR col[2];

/* protected */
	BIPOINT n[2],c[2],tr0[3],tr1[3];
} BITERRBLOCK;

typedef struct {
/* public: */
	int xSiz,zSiz;
	real xWid,zWid;
	BITERRBLOCK *blk;
	int side[4];
	BICOLOR sdCol[4];

/* protected: */
	BIPOINT bbox[8];
	BIPOINT *sdPlg[4];
} BITER;



/* Fld */
enum {
	BIFLD_PC2MAP,
	BIFLD_SRF,
	BIFLD_PC2,
	BIFLD_TER,
	BIFLD_RGN,
	BIFLD_FLD
};

#define BIFLD_TAG_LEN 80

typedef struct {
	BIPOSATT pos;
	BISRF srf;
	char fn[80];
	int id;
	char tag[BIFLD_TAG_LEN];
	real lodDist;
} BIFLDSRF;

typedef struct {
	BIPOSATT pos;
	BITER ter;
	char fn[80];
	int id;
	char tag[BIFLD_TAG_LEN];
	real lodDist;
} BIFLDTER;

typedef struct {
	BIPOSATT pos;
	BIPC2 pc2;
	char fn[80];
	real lodDist;
} BIFLDPC2;

typedef struct {
	BIPOSATT pos;
	BIPOINT2 min,max;
	int id;
	char tag[BIFLD_TAG_LEN];
	real lodDist;
} BIFLDRGN;

typedef struct BiFldTag {
	BICOLOR sky,gnd;

	int nSrf;
	BIFLDSRF *srf;
	int nTer;
	BIFLDTER *ter;
	int nPc2;
	BIFLDPC2 *pc2;
	int nRgn;
	BIFLDRGN *rgn;
	int nPlt;
	BIFLDPC2 *plt;
	int nFld;
	struct BiFldFldTag *fld;
} BIFLD;

typedef struct BiFldFldTag {
	BIPOSATT pos;
	BIFLD fld;
	char fn[80];
	real lodDist;
} BIFLDFLD;

/* 1996/02/06 update */
int BiGetFldNumObj(int *nObj,BIFLD *fld,int typ);
int BiGetFldObjPosition(BIPOSATT *pos,BIFLD *fld,BIPOSATT *lay,int typ,int id);
int BiGetFldObjId(int *oId,char tab[],BIFLD *fld,int typ,int id);



/* Functions : icalc.c */
void BiCalcInitialize(void);
long BiAsin(real s); /* Ret : -0x4000 -> +0x4000 */
long BiAcos(real s); /* Ret : +0x0000 -> +0x8000 */
long BiAtan(real s); /* Ret : -0x4000 -> +0x4000 */
real BiAsinF(real s);
real BiAcosF(real s);
real BiAtanF(real s);

void BiNearClipLine(BIPOINT *c,BIPOINT *p,real z);
void BiNearClipPolyg(int *nc,BIPOINT *c,int np,BIPOINT *p,real z);
void BiNormalize(BIPOINT *dst,BIPOINT *src);
int BiAverageNormalVector(BIPOINT *nom,int np,BIPOINT p[]);
void BiOuterProduct(BIPOINT *ou,BIPOINT *v1,BIPOINT *v2);

void BiAngleToVector(BIPOINT *eye,BIPOINT *up,BIANGLE *an);
void BiVectorToHeadPitch(BIANGLE *an,BIPOINT *vec);
void BiVectorToAngle(BIANGLE *an,BIPOINT *eye,BIPOINT *up);
void BiAngleToVectorF(BIPOINT *eye,BIPOINT *up,BIANGLEF *an);
void BiVectorToHeadPitchF(BIANGLEF *an,BIPOINT *vec);
void BiVectorToAngleF(BIANGLEF *an,BIPOINT *eye,BIPOINT *up);
void BiPitchUp(BIANGLE *dst,BIANGLE *src,long pit,long yaw);

void BiRot2(BIPOINT2 *dst,BIPOINT2 *src,long ang);
void BiRot2F(BIPOINT2 *dst,BIPOINT2 *src,real ang);
long BiAngle2(BIPOINT2 *vec); /* Ret : -0x8000 -> +0x8000 */
real BiAngle2F(BIPOINT2 *vec); /* Ret : -0x8000 -> +0x8000 */

real BiLength2(real x,real y);
void BiNormalize2(BIPOINT2 *dst,BIPOINT2 *src);

int BiAverageNormalVector(BIPOINT *nom,int np,BIPOINT p[]);
void BiOuterProduct(BIPOINT *ou,BIPOINT *a,BIPOINT *b);
void BiRightAngleVector(BIPOINT *ret,BIPOINT *src);
void BiVectorToHeadPitch(BIANGLE *an,BIPOINT *vec);
int BiMakePlane(BIPLANE *pln,int nPnt,BIPOINT *pnt);
void BiProjectToPlane(BIPOINT *pnt,BIPLANE *pln,BIPOINT *org);
void BiLineProjectionVector(BIPOINT *v,BIPOINT *org,BILINE *ln);
real BiLineDistance2(BILINE2 *ln,BIPOINT2 *p);
real BiLineDistance(BILINE *ln,BIPOINT *p);
int BiCrossPoint(BIPOINT *out,BILINE *ln,BIPLANE *pl);
void BiAxisToPlane(BIPLANE *dst,BIAXIS *src);

int BiKeyToAscii(int bik);

int BiMakePolyg(BIPOLYGON *plg,int np,BIPOINT *p);
int BiMakePolyg2(BIPOLYGON2 *plg,int np,BIPOINT2 *p);
int BiCopyPolyg(BIPOLYGON *neo,BIPOLYGON *org);
int BiCopyPolyg2(BIPOLYGON2 *neo,BIPOLYGON2 *org);
void BiFreePolyg(BIPOLYGON *plg);
void BiFreePolyg2(BIPOLYGON2 *plg);

void BiConvAxisGtoL(BIAXIS *neo,BIAXIS *src,BIAXIS *bas);
void BiConvAxisLtoG(BIAXIS *neo,BIAXIS *src,BIAXIS *bas);
void BiConvAxisGtoLF(BIAXISF *neo,BIAXISF *src,BIAXISF *bas);
void BiConvAxisLtoGF(BIAXISF *neo,BIAXISF *src,BIAXISF *bas);

int BiPushMatrix(BIPOSATT *pos);
int BiPopMatrix(void);

void BiInBetweenAngle(BIANGLE *dst,BIANGLE *a1,BIANGLE *a2,real t);
void BiInBetweenPoint(BIPOINT *dst,BIPOINT *p1,BIPOINT *p2,real t);

real BiPow60(real t); /* Library Local Function */
int BiQuickNormalize(BIPOINT *dst,BIPOINT *src);

int BiSetShadowPlane(BIPOSATT *pos);
int BiOvwSrfShadow(BISRF *mdl,BIPOSATT *pos);

int BiOvwPolygShadow(int np,BIPOINT p[],BICOLOR *col);
int BiOvwLineShadow(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);

int BiQuickCheckConvex(BIPOLYGON *plg);
int BiQuickCheckConvex2(BIPOLYGON2 *plg);


/******************************************************************************

                Blue Impulse - 3D Graphics Library

******************************************************************************/
enum {
	BI3DG_NOERR,
	BI3DG_STACKOVERFLOW,
	BI3DG_UNSUPPORTED  /* BiMdlAxsSw==YSON && BiInsSrf */
};

enum {
	BIPRJ_PERS,
	BIPRJ_ORTHO
};

enum {
	BIMK_RECT,
	BIMK_CIRCLE,
	BIMK_CROSS
};

enum {
	BIAL_CENTER,
	BIAL_LEFT,
	BIAL_RIGHT,
	BIAL_TOP,
	BIAL_BOTTOM
};

#define BIMG_CENTER BIAL_CENTER
#define BIMG_LEFT   BIAL_LEFT
#define BIMG_RIGHT  BIAL_RIGHT
#define BIMG_TOP    BIAL_TOP
#define BIMG_BOTTOM BIAL_BOTTOM


void BiGraphInitialize(char *work,unsigned long wSize);
void BiGraphSetBuffer2(char *work,unsigned long wSize);
void BiGraphSetBuffer3(char *work,unsigned long wSize);
void BiSetMemoryFunc(void *(*mal)(size_t),void (*fre)(void *));
void BiSetProjectionMode(int mode);
void BiSetShadMode(int mode);
void BiSetOrthoDist(real dist);
void BiSetProjection(BIPROJ *prj);
void BiStartBuffer(BIPOSATT *eye);
void BiSetLocalMatrix(BIPOSATT *mdl);
void BiClearLocalMatrix(void);
void *BiGetTmpStack(size_t siz);
void *BiPushTmpStack(unsigned long *ptr,size_t siz);
void BiPopTmpStack(unsigned long ptr);
int BiInsLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);
int BiInsPset(BIPOINT *p,BICOLOR *col);
int BiInsCircle(BIPOINT *p,real r,BICOLOR *col);
int BiInsRoundPolyg(int np,BIPOINT p[],BIPOINT n[],BICOLOR *col);
int BiInsPolygFast(int n,BIPOINT *p,BIPOINT *c,BIPOINT *m,BICOLOR *col);
int BiInsPolyg(int n,BIPOINT *p,BICOLOR *col);
int BiInsMarker(BIPOINT *p,BICOLOR *col,int mkType);
int BiInsString(BIPOINT *p,char *str,BICOLOR *col,int hori,int vert);
int BiInsText(BIPOINT *p,char *str[],BICOLOR *col,int hori,int vert);
int BiInsSrf(BISRF *mdl,BIPOSATT *pos);
int BiInsSrfDivide(BISRF *mdl,BIPOSATT *pos);
int BiInsSrfDivideLowPriority(BISRF *mdl,BIPOSATT *pos);
int BiInsTer(BITER *ter,BIPOSATT *pos);
int BiInsPc2(BIPC2 *pc2,BIPOSATT *pos);
void BiOvwLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);
void BiOvwPc2(BIPC2 *pic,BIPOSATT *pnt);
void BiOvwPset(BIPOINT *p,BICOLOR *col);
void BiOvwPolyg(int n,BIPOINT *p,BICOLOR *col);
void BiOvwMarker(BIPOINT *p,BICOLOR *col,int mkType);
int BiOvwString(BIPOINT *p,char *str,BICOLOR *col,int hori,int vert);
int BiOvwText(BIPOINT *p,char *str[],BICOLOR *col,int hori,int vert);
void BiFlushBuffer(void);
void BiShadColor(BICOLOR *dst,BICOLOR *src,BIPOINT *cen,BIPOINT *nom);


void *BiGetCurrentTreeRoot(void);
void *BiGetCurrentTreeHead(void);
void BiSetTreeRootAndHead(void *treeRoot,void *treeHead);
void *BiInsSubTree(BIPOINT *cen,BIPOINT bbx[8]);
void *BiInsSubTreeZ(real z);
void BiSetSubTreeRootHead(void *subTree,void *root,void *head);


int BiLoadSrf(BISRF *mdl,char fn[]);
int BiLoadSrfFromString(BISRF *mdl,char *str[]);
void BiScaleSrf(BISRF *srf,real scl);
void BiGetSrfRadius(real *rad,BISRF *srf);
void BiFreeSrf(BISRF *srf);

int BiLoadPc2(BIPC2 *pic,char fn[]);
int BiLoadPc2FromString(BIPC2 *pic,char *str[]);
void BiDrawPc2(BIPC2 *pic,BIPOINTS *sc);
void BiDrawPc2Zoom(BIPC2 *pic,BIPOINTS *s1,BIPOINTS *s2);

void BiGetClipScreenFrame(BIPOINTS *l,BIPOINTS *r);
void BiSetClipScreenFrame(BIPOINTS *l,BIPOINTS *r);
int BiClipPolygScrn(int *npNew,BIPOINTS *pNew,int np,BIPOINTS *p);
int BiClipLineScrn(BIPOINTS *p1New,BIPOINTS *p2New,BIPOINTS *p1,BIPOINTS *p2);


int BiSrfCollision(BISRF *srf,BIPOSATT *pos,BIPOINT *p,real bump);


int BiConstTer(BITER *ter);
int BiAboveTer(real x,real z,BITER *ter,BIPOSATT *pos);
int BiTerHeight(real *y,real x,real z,BITER *ter,BIPOSATT *pos);
int BiTerEyeVecUpVec
    (BIPOINT *e,BIPOINT *v,real x,real z,long h,BITER *ter,BIPOSATT *pos);
void BiDrawGroundSky(BICOLOR *grd,BICOLOR *sky);
int BiLoadTer(BITER *tm,char fn[]);
int BiLoadTerFromString(BITER *tm,char *str[]);


void BiSetLightPosition(BIPOINT *p);


/***********************************************************************

              Eps Output  (Secret Functions for a while)

***********************************************************************/
extern int BiEpsMode;
int BiEpsOpen(char *fn,int wid,int hei);
int BiEpsClose(void);
int BiEpsClearScreen(void);
int BiEpsResetProjection(BIPROJ *prj);
int BiEpsDrawRect(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col);
int BiEpsDrawLine(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col);
int BiEpsDrawPset(BIPOINTS *s1,BICOLOR *col);
int BiEpsDrawPolygon(int np,BIPOINTS *p,BICOLOR *col);
int BiEpsDrawString(char *str,BIPOINTS *s,BICOLOR *col);
int BiEpsDrawEllipse(BIPOINTS *c,long radx,long rady,BICOLOR *col);

int BiEpsDrawMarker(BIPOINTS *s,int mkType,BICOLOR *c);
int BiEpsDrawText(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c);
int BiEpsDrawEmptyRect(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col);
int BiEpsDrawEmptyPolyg(int n,BIPOINTS *s,BICOLOR *col);
int BiEpsDrawPolyline(int n,BIPOINTS *s,BICOLOR *col);
int BiEpsDrawEmptyEllipse(BIPOINTS *s,long radx,long rady,BICOLOR *col);
int BiEpsDrawLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);
int BiEpsDrawMarker3(BIPOINT *p,int mkType,BICOLOR *col);
int BiEpsDrawPset3(BIPOINT *p,BICOLOR *col);
int BiEpsDrawPolyg3(int np,BIPOINT p[],BICOLOR *col);
int BiEpsDrawCircle3(BIPOINT *p,real rad,BICOLOR *col);
int BiEpsDrawText3(BIPOINT *p,char *str[],int h,int v,BICOLOR *c);


/******************************************************************************

                              Twist Judgement

******************************************************************************/
enum {
	BITWIST_UNKNOWN,
	BITWIST_RIGHT,
	BITWIST_LEFT
};

int BiTwist3(int np,BIPOINT *p,BIPOINT *nom);
int BiTwist2(int np,BIPOINT2 *p);



/******************************************************************************

                    PC-Loop Separate & Line Cross Check

******************************************************************************/
enum {
	BI_CRS_SAME,          /* 一致 */
	BI_CRS_PARALLEL,      /* 平行 */
	BI_CRS_ON_LINE,       /* 同一直線上 */
	BI_CRS_ONESIDE_SAME,  /* 片側の頂点が一致 */
	BI_CRS_CROSS,         /* 交差 */
	BI_CRS_NOCROSS,       /* 平行でもないし交差もしない */
	BI_CRS_LN1_TOUCH_LN2, /* 線分1の片方の点が線分2に接している */
	BI_CRS_LN2_TOUCH_LN1  /* 線分2の片方の点が線分1に接している */
};

int BiPolygToTris(int *nTr,BIPOINT *tr,int nPl,BIPOLYGON *pl);
int BiLine2CrossStatus(BILIMLINE2 *ln1,BILIMLINE2 *ln2);
int BiLine2CrossPoint(BIPOINT2 *o,BILINE2 *l1,BILINE2 *l2);
int BiKillCLoop2(BIPOLYGON2 *dst,int nPl,BIPOLYGON2 *pl);
int BiKillCLoop(BIPOLYGON *dst,int nPl,BIPOLYGON *pl);
int BiPolyg2ToTris(int *nTr,BIPOINT2 *tr,int nPl,BIPOLYGON2 *pl);

int BiCheckConvex2(BIPOLYGON2 *plg);
int BiPolygToConvex2(int *nCvx,BIPOLYGON2 *cvx,BIPOLYGON2 *org);

int BiCheckConvex(BIPOLYGON *plg);
int BiPolygToConvex(int *nCvx,BIPOLYGON *cvx,BIPOLYGON *org);





/***********************************************************************

                        2D Draw

***********************************************************************/
void BiClearScreen(void);
void BiDrawLine2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c);
void BiDrawPset2(BIPOINTS *s,BICOLOR *c);
void BiDrawCircle2(BIPOINTS *s,long radx,long rady,BICOLOR *c);
void BiDrawPolyg2(int ns,BIPOINTS *s,BICOLOR *c);
void BiDrawRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c);
void BiDrawMarker2(BIPOINTS *s,int mkType,BICOLOR *c);
void BiDrawString2(BIPOINTS *s,char *str,BICOLOR *c);
void BiDrawText2(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c);
void BiDrawEmptyRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col);
void BiDrawEmptyPolyg2(int n,BIPOINTS *s,BICOLOR *col);
void BiDrawPolyline2(int n,BIPOINTS *s,BICOLOR *col);
void BiDrawEmptyCircle2(BIPOINTS *s,long radx,long rady,BICOLOR *col);




/******************************************************************************

                        Timer Module

******************************************************************************/
void BiResetClock(void);
real BiPassedTime(void);
real BiLoopTime(real minimum);
void BiWaitClock(real sec);




/******************************************************************************

                        Rendering Module

******************************************************************************/
enum {
	BIRENDER_TIFF,
	BIRENDER_BMP,
	BIRENDER_PPM
};

int BiRenderInitialize(void);
int BiRenderOpen(char *fn,int type,int wid,int hei);
void BiRenderClose(void);

void BiRenderLinkPolygonTwoSide(int np,BIPOINT p[],BICOLOR *c);
void BiRenderLinkPolygonOneSide(int np,BIPOINT p[],BIPOINT *nom,BICOLOR *c);
void BiRenderLinkRoundPolygonOneSide(int np,BIPOINT *p,BIPOINT *n,BIPOINT *avgNom,BICOLOR *c);
void BiRenderLinkRoundPolygonTwoSide(int np,BIPOINT *p,BIPOINT *n,BICOLOR *c);
void BiRenderLinkLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *c);
void BiRenderLinkPset(BIPOINT *p,BICOLOR *c);
void BiRenderLinkCircle(BIPOINT *p,real r,BICOLOR *c);

void BiRenderLinkOvwPolygon(int np,BIPOINT p[],BICOLOR *c);
void BiRenderLinkOvwLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *c);
void BiRenderLinkOvwPset(BIPOINT *p,BICOLOR *c);
void BiRenderLinkOvwCircle(BIPOINT *p,real r,BICOLOR *c);

void BiRenderLinkLine2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c);
void BiRenderLinkPolygon2(int ns,BIPOINTS *s,BICOLOR *c);
void BiRenderLinkRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c);
void BiRenderLinkPset2(BIPOINTS *s,BICOLOR *col);
void BiRenderLinkEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *col);
void BiRenderLinkEmptyRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col);
void BiRenderLinkPolyline2(int n,BIPOINTS *s,BICOLOR *col);
void BiRenderLinkEmptyPolygon2(int n,BIPOINTS *s,BICOLOR *col);
void BiRenderLinkEmptyEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *col);

int BiOpenTiff24(char *fn,long wid,long hei);
int BiWriteLineTiff24(BICOLOR *dat);
int BiCloseTiff24(void);

int BiOpenBmp24(char *fn,long wid,long hei);
int BiWriteLineBmp24(BICOLOR *dat);
int BiCloseBmp24(void);

int BiOpenPpm24(char *fn,long wid,long hei);
int BiWriteLinePpm24(BICOLOR *dat);
int BiClosePpm24(void);



/******************************************************************************

                        Vrml Module

******************************************************************************/
int BiVrmlOpen(char fn[]);
int BiVrmlSetOffset(BIVECTOR *v);
int BiVrmlClose(void);

int BiSrfClose(void);
int BiSrfOpen(char fn[]);
int BiSrfSetOffset(BIVECTOR *v);

int BiVrml2Open(char fn[]);
int BiVrml2Close(void);



/******************************************************************************

                                Fld

******************************************************************************/
int BiLoadFld(BIFLD *fld,char fn[]);
void BiOvwFld(BIFLD *fld,BIPOSATT *pos);
void BiInsFld(BIFLD *fld,BIPOSATT *pos);
int BiGetFldRegion(int *rgId,char tag[],BIFLD *fld,BIPOSATT *pos,BIPOINT *p);
int BiGetFldElevation(int *trId,char tag[],real *elv,BIPOINT *ev,BIPOINT *uv,BIFLD *fld,BIPOSATT *pos,BIPOINT *p,long hdg);
int BiGetFldSrfCollision(int *sfId,char tag[],BIFLD *fld,BIPOSATT *pos,BIPOINT *p,real bump);
int BiGetFldGroundSky(BICOLOR *gnd,BICOLOR *sky,BIFLD *fld);

/******************************************************************************

                         Machine Depending Module

******************************************************************************/
void BiOpenWindow(long reqX,long reqY);
void BiOpenWindowEx(long x,long y,long wx,long wy);
void BiCloseWindow(void);
void BiGetWindowSize(long *wid,long *hei);
void BiGetStdProjection(BIPROJ *prj);
void BiClearScreenD(void);
void BiSwapBuffers(void);
void BiSetWireframeMode(int wfMode);
void BiGetWireframeMode(int *wfMode);
void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c);
void BiDrawPset2D(BIPOINTS *s,BICOLOR *c);
void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c);
void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c);
void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c);
void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c);
void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c);
void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c);
/* 1995/06/28 Update */
void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col);
void BiDrawEmptyPolyg2D(int n,BIPOINTS *s,BICOLOR *col);
void BiDrawPolyline2D(int n,BIPOINTS *s,BICOLOR *col);
void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *col);

#ifdef BIACCEL3D
void BiAfterSetProjection(int pMode,BIPROJ *prj);
void BiAfterSetLightPosition(BIPOINT *lPos);
void BiAfterSetProjectionMode(int pMode,BIPROJ *prj);
void BiAfterSetOrthoDist(int pMode,BIPROJ *prj,real dist);
void BiAfterSetShadeMode(int sMode);
void BiAfterStartBuffer(BIPOSATT *eye);
void BiAfterChangeMatrix(BIAXISF *cnvaxs,BIAXISF *mdlaxs,BIAXISF *eyeaxs);
void BiOverWriteLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);
void BiOverWritePset3(BIPOINT *p,BICOLOR *col);
void BiOverWritePolyg3(int np,BIPOINT *p,BICOLOR *col);
void BiOverWriteMarker3(BIPOINT *p,BICOLOR *col,int mkType);
void BiOverWriteText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col);
void BiDrawLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);
void BiDrawPset3(BIPOINT *p,BICOLOR *col);
void BiDrawCircle3(BIPOINT *cen,real radx,real rady,BICOLOR *col);
void BiDrawPolyg3(int np,BIPOINT *p,BIPOINT *n,BICOLOR *col);
void BiDrawPolygGrad3(int np,BIPOINT *p,BIPOINT *n,BICOLOR *col);
void BiDrawText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col);
void BiDrawMarker3(BIPOINT *p,BICOLOR *col,int type);
#endif

void BiGetColor(BICOLOR *col,BIPOINTS *s);
void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2);

void BiConstrainFileName(char neo[],char org[]);



/******************************************************************************

                    Primitive Resource  Added on 1996/01/15

******************************************************************************/
extern BIPOINT BiOrgP;
extern BIATTITUDE BiOrgA;
extern BIPOSATT BiOrgPA;
extern BIAXIS BiOrgAxs;
extern BIPLANE BiOrgPln;
extern BIPOINT BiVecX;
extern BIPOINT BiVecY;
extern BIPOINT BiVecZ;
extern BICOLOR BiBlack;
extern BICOLOR BiBlue;
extern BICOLOR BiRed;
extern BICOLOR BiMagenta;
extern BICOLOR BiGreen;
extern BICOLOR BiCyan;
extern BICOLOR BiYellow;
extern BICOLOR BiWhite;


/***************************************************************************

                     3D Solid Text Extension  1996/03/10

***************************************************************************/
#define BI_N_3DFONT 128

enum {
	BI3DFONT_SRF,
	BI3DFONT_PC2
};

typedef struct {
	real wid,hei;
	BICOLOR col;
	int type;
	int nSrf;
	void *(*srf);
}BI3DFONT;

int BiMakeStdSolidFont(BI3DFONT *fnt,real wid,BICOLOR *col);
int BiMakeStdWireFont(BI3DFONT *fnt,real wid,BICOLOR *col);
void BiFreeSolidFont(BI3DFONT *fnt);
void BiInsSolidString(BIPOSATT *pos,BI3DFONT *fnt,char str[],int h,int v);
void BiInsSolidText(BIPOSATT *pos,BI3DFONT *fnt,char *str[],int h,int v);



/***************************************************************************

                             Clickable

***************************************************************************/
int BiSetClickableBuffer(char buf[],int nbuf);
int BiClearClickable(void);
int BiInsClickable(BIPOINT *p,real rad,int pix,int i1,int i2,int i3,void *extra);
int BiCheckClicked(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr);
int BiCheckClickedPoint(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr);
int BiCheckClickedPolyg(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr);

int BiSetClickablePolygBuffer(unsigned char *buf,int siz);
int BiInsClickablePolyg(int np,BIPOINT p[],BIVECTOR *n,int i1,int i2,int i3,void *extra);

int BiCheckClickedLine(int *i1,int *i2,int *i3,void **ext,BIPOINTS *sc,int ctr);
int BiInsClickableLine(BIPOINT *p1,BIPOINT *p2,int i1,int i2,int i3,void *extra);

/***************************************************************************

                      Polygon Inside/Outside(General Exportable)

***************************************************************************/
int BiCheckInsidePolyg2(BIPOLYGON2 *plg,BIPOINT2 *pnt);



/******************************************************************************

              Machine Depending Module    (Device & GUI Support)

******************************************************************************/
enum {
	BIKEY_NULL,    BIKEY_STOP,    BIKEY_COPY,
	BIKEY_F1,      BIKEY_F2,      BIKEY_F3,      BIKEY_F4,      BIKEY_F5,
	BIKEY_F6,      BIKEY_F7,      BIKEY_F8,      BIKEY_F9,      BIKEY_F10,
	BIKEY_ZERO,    BIKEY_ONE,     BIKEY_TWO,     BIKEY_THREE,   BIKEY_FOUR,
	BIKEY_FIVE,    BIKEY_SIX,     BIKEY_SEVEN,   BIKEY_EIGHT,   BIKEY_NINE,
	BIKEY_A,       BIKEY_B,       BIKEY_C,       BIKEY_D,       BIKEY_E,
	BIKEY_F,       BIKEY_G,       BIKEY_H,       BIKEY_I,       BIKEY_J,
	BIKEY_K,       BIKEY_L,       BIKEY_M,       BIKEY_N,       BIKEY_O,
	BIKEY_P,       BIKEY_Q,       BIKEY_R,       BIKEY_S,       BIKEY_T,
	BIKEY_U,       BIKEY_V,       BIKEY_W,       BIKEY_X,       BIKEY_Y,
	BIKEY_Z,       BIKEY_SPACE,
	BIKEY_ESC,     BIKEY_TAB,     BIKEY_CTRL,    BIKEY_SHIFT,   BIKEY_ALT,
	BIKEY_BS,      BIKEY_RET,     BIKEY_HOME,    BIKEY_DEL,
	BIKEY_UP,      BIKEY_DOWN,    BIKEY_LEFT,    BIKEY_RIGHT,
	BIKEY_TEN0,    BIKEY_TEN1,    BIKEY_TEN2,    BIKEY_TEN3,    BIKEY_TEN4,
	BIKEY_TEN5,    BIKEY_TEN6,    BIKEY_TEN7,    BIKEY_TEN8,    BIKEY_TEN9,
	BIKEY_TENMUL,  BIKEY_TENDIV,  BIKEY_TENPLUS,
	BIKEY_TENMINUS,BIKEY_TENEQUAL,BIKEY_TENDOT,
	BIKEY_INS,     BIKEY_END,     BIKEY_PGUP    ,BIKEY_PGDOWN,
	BIKEY_COMMA,   BIKEY_DOT,     BIKEY_SLASH,   BIKEY_BSLASH,
	BIKEY_LBRACKET,BIKEY_RBRACKET
};
int BiFatal(void);
void BiDeviceInitialize(void);
void BiUpdateDevice(void);
void BiMouse(int *lbt,int *mbt,int *rbt,long *mx,long *my);
int BiKey(int kcode);
int BiInkey(void);
void BiMemoryInitialize(void);
void *BiMalloc(size_t uni);
void BiFree(void *ptr);

void BiSetWindowName(char wn[]);



#define BIAPPENTRY int main(int ac,char *av[])
#define BIAPPINIT {}


#define BITERRMESH               BITER
#define BiLoadTerrMesh           BiLoadTer
#define BiLoadTerrMeshFromString BiLoadTerFromString
#define BiInsTerrMesh            BiInsTer
#define BiConstTerrMesh          BiConstTer
#define BiAboveTerrMesh          BiAboveTer
#define BiTerrMeshHeight         BiTerHeight
#define BiTerrMeshEyeVecUpVec    BiTerEyeVecUpVec

#define BIPICT2                  BIPC2
#define BiLoadPict2              BiLoadPc2
#define BiLoadPict2FromString    BiLoadPc2FromString
#define BiInsPict2               BiInsPc2
#define BiOvwPict2               BiOvwPc2
#define BiDrawPict2              BiDrawPc2
#define BiDrawPict2Zoom          BiDrawPc2Zoom

#define BISRFMODEL               BISRF
#define BiInsSrfModel            BiInsSrf
#define BiInsSrfModelDivide      BiInsSrfDivide
#define BiFreeSrfModel           BiFreeSrf

#define BIFIELD                  BIFLD
#define BiFieldNumObj            BiFldNumObj
#define BiFieldObjPosition       BiFldObjPosition
#define BiFieldObjId             BiFldObjId
#define BiOvwField               BiOvwFld
#define BiInsField               BiInsFld
#define BiGetFieldRegion         BiGetFldRegion
#define BiGetFieldElevation      BiGetFldElevation
#define BiGetFieldSrfCollision   BiGetFldSrfCollision
#define BiGetFieldGroundSky      BiGetFldGroundSky
#define BiGetFieldNumObj         BiGetFldNumObj
#define BiGetFieldObjPosition    BiGetFldObjPosition
#define BiGetFieldObjId          BiGetFieldObjId


#ifdef __cplusplus
}
#endif




#ifndef __cplusplus
/* (inline functions for C */

#define BiSame(x,y) ((BiAbs((x)-(y))<=YSEPS) ? BI_TRUE : BI_FALSE)

#define BiSamePoint(a,b) \
    ((BiSame((a)->x,(b)->x)&&\
      BiSame((a)->y,(b)->y)&&\
      BiSame((a)->z,(b)->z)) ? BI_TRUE : BI_FALSE)

#define BiSamePoint2(a,b) \
    ((BiSame((a)->x,(b)->x)&&\
      BiSame((a)->y,(b)->y)) ? BI_TRUE : BI_FALSE)

#define BiSetPoint(dst,xx,yy,zz) \
{ \
	(dst)->x = (real)(xx); \
	(dst)->y = (real)(yy); \
	(dst)->z = (real)(zz); \
}

#define BiAddPoint(dst,a,b) \
{ \
	((dst)->x) = (real)((a)->x+(b)->x); \
	((dst)->y) = (real)((a)->y+(b)->y); \
	((dst)->z) = (real)((a)->z+(b)->z); \
}

#define BiSubPoint(dst,a,b) \
{ \
	(dst)->x = (real)((a)->x-(b)->x); \
	(dst)->y = (real)((a)->y-(b)->y); \
	(dst)->z = (real)((a)->z-(b)->z); \
}

#define BiMulPoint(dst,src,m) \
{ \
	(dst)->x = (real)((src)->x*(m)); \
	(dst)->y = (real)((src)->y*(m)); \
	(dst)->z = (real)((src)->z*(m)); \
}

#define BiDivPoint(dst,src,d) \
{ \
	(dst)->x = (real)((src)->x/(d)); \
	(dst)->y = (real)((src)->y/(d)); \
	(dst)->z = (real)((src)->z/(d)); \
}

#define BiInnerPoint(a,b) \
	(real)(((a)->x*(b)->x)+((a)->y*(b)->y)+((a)->z*(b)->z))

#define BiLength3(x,y,z) \
	(real)(BiLength2((x),BiLength2((y),(z))))

#define BiLengthPoint3(p) \
	(real)(BiLength3((p)->x,(p)->y,(p)->z))


#define BiMakeLine(ln,a,b) {(ln)->o=*(a);BiSubPoint(&(ln)->v,(b),(a));}

#define BiMakeLine2(ln,a,b) {(ln)->o=*(a);BiSubPoint2(&(ln)->v,(b),(a));}

#define BiMakeLimLine(ln,a,b) {(ln)->p1=*(a);(ln)->p2=*(b);}

#define BiMakeLimLine2(ln,a,b) BiMakeLimLine((ln),(a),(b))

#define BiLineToLimLine(lm,ln) \
{ \
	(lm)->p1=(ln)->o; \
	BiAddPoint(&(lm)->p2,&(ln)->o,&(ln)->v); \
}

#define BiLimLineToLine(ln,lm) \
{ \
	(ln)->o=(lm)->p1; \
	BiSubPoint(&(ln)->v,&(lm)->p2,&(lm)->p1); \
}

#define BiLineToLimLine2(lm,ln) \
{ \
	(lm)->p1=(ln)->o; \
	BiAddPoint2(&(lm)->p2,&(ln)->o,&(ln)->v); \
}

#define BiLimLineToLine2(ln,lm) \
{ \
	(ln)->o=(lm)->p1; \
	BiSubPoint2(&(ln)->v,&(lm)->p2,&(lm)->p1); \
}

#define BiSetAngleDeg(dst,hh,pp,bb) \
{	(dst)->h = (hh)*0x10000/360; \
	(dst)->p = (pp)*0x10000/360; \
	(dst)->b = (bb)*0x10000/360; }

#define BiSetAngleRad(dst,hh,pp,bb) \
{	(dst)->h = (hh)*0x8000/YSPI; \
	(dst)->p = (pp)*0x8000/YSPI; \
	(dst)->b = (bb)*0x8000/YSPI; }

#define BiSetAngle(dst,hh,pp,bb) \
{	(dst)->h = (hh); \
	(dst)->p = (pp); \
	(dst)->b = (bb); }

#define BiSetPoint2(dst,xx,yy)        \
{	(dst)->x = (real)(xx);            \
	(dst)->y = (real)(yy); }

#define BiAddPoint2(dst,a,b)          \
{	(dst)->x = (real)((a)->x+(b)->x); \
	(dst)->y = (real)((a)->y+(b)->y); }

#define BiSubPoint2(dst,a,b)          \
{	(dst)->x = (real)((a)->x-(b)->x); \
	(dst)->y = (real)((a)->y-(b)->y); }

#define BiMulPoint2(dst,src,m)        \
{	(dst)->x = (real)((src)->x*(m));  \
	(dst)->y = (real)((src)->y*(m)); }

#define BiDivPoint2(dst,src,d)        \
{	(dst)->x = (real)((src)->x/(d));  \
	(dst)->y = (real)((src)->y/(d)); }

#define BiInnerPoint2(a,b)            \
	(real)(((a)->x*(b)->x)+((a)->y*(b)->y))

#define BiLengthPoint2(p)             \
	(real)(BiLength2((p)->x,(p)->y))

#define BiRotLtoG(dst,src,ang)        \
{ \
	BITRIGON tmpt;                                     \
	BIPOINT tmpp;                                      \
	BiMakeTrigonomy(&tmpt,(ang));                      \
                                                       \
	tmpp.x   = tmpt.cosb*(src)->x -tmpt.sinb*(src)->y; \
	tmpp.y   = tmpt.sinb*(src)->x +tmpt.cosb*(src)->y; \
                                                       \
	tmpp.z   = tmpt.cosp*(src)->z - tmpt.sinp*tmpp.y;  \
	(dst)->y = tmpt.sinp*(src)->z + tmpt.cosp*tmpp.y;  \
                                                       \
	(dst)->x = tmpt.cosh*tmpp.x - tmpt.sinh*tmpp.z;    \
	(dst)->z = tmpt.sinh*tmpp.x + tmpt.cosh*tmpp.z;    \
}

#define BiRotGtoL(dst,src,ang)                         \
{                                                      \
	BITRIGON tmpt;                                     \
	BIPOINT tmpp;                                      \
	BiMakeTrigonomy(&tmpt,(ang));                      \
                                                         \
	tmpp.x   =  tmpt.cosh*(src)->x + tmpt.sinh*(src)->z; \
	tmpp.z   = -tmpt.sinh*(src)->x + tmpt.cosh*(src)->z; \
                                                         \
	(dst)->z =  tmpt.cosp*tmpp.z + tmpt.sinp*(src)->y; \
	tmpp.y   = -tmpt.sinp*tmpp.z + tmpt.cosp*(src)->y; \
                                                       \
	(dst)->x =  tmpt.cosb*tmpp.x + tmpt.sinb*tmpp.y;   \
	(dst)->y = -tmpt.sinb*tmpp.x + tmpt.cosb*tmpp.y;   \
}

#define BiRotFastLtoG(dst,src,trg)                          \
{                                                           \
	BIPOINT tmpp;                                           \
	tmpp.x   = (trg)->cosb*(src)->x - (trg)->sinb*(src)->y; \
	tmpp.y   = (trg)->sinb*(src)->x + (trg)->cosb*(src)->y; \
                                                            \
	tmpp.z   = (trg)->cosp*(src)->z - (trg)->sinp*tmpp.y;   \
	(dst)->y = (trg)->sinp*(src)->z + (trg)->cosp*tmpp.y;   \
                                                            \
	(dst)->x = (trg)->cosh*tmpp.x  - (trg)->sinh*tmpp.z;    \
	(dst)->z = (trg)->sinh*tmpp.x  + (trg)->cosh*tmpp.z;    \
}

#define BiRotFastGtoL(dst,src,trg)                           \
{                                                            \
	BIPOINT tmpp;                                            \
	tmpp.x   =  (trg)->cosh*(src)->x + (trg)->sinh*(src)->z; \
	tmpp.z   = -(trg)->sinh*(src)->x + (trg)->cosh*(src)->z; \
                                                             \
	(dst)->z =  (trg)->cosp*tmpp.z  + (trg)->sinp*(src)->y;  \
	tmpp.y   = -(trg)->sinp*tmpp.z  + (trg)->cosp*(src)->y;  \
                                                             \
	(dst)->x =  (trg)->cosb*tmpp.x  + (trg)->sinb*tmpp.y;    \
	(dst)->y = -(trg)->sinb*tmpp.x  + (trg)->cosb*tmpp.y;    \
}

#define BiProject(dst,src,prj)                                      \
{                                                                   \
	(dst)->x = (prj)->cx+(long)((src)->x * (prj)->magx / (src)->z); \
	(dst)->y = (prj)->cy-(long)((src)->y * (prj)->magy / (src)->z); \
}

#define BiPlProject(n,dst,src,prj)      \
{                                       \
	int i;                              \
	for(i=0; i<n; i++)                  \
	{                                   \
		BiProject(&dst[i],&src[i],prj); \
	}                                   \
}

/* 1995/01/10 update */
#define BiPntAngToAxis(dst,src) \
{ \
	(dst)->p=(src)->p; \
	(dst)->a=(src)->a; \
	BiMakeTrigonomy(&(dst)->t,&(src)->a); \
}

#define BiMakeAxisF(dst,src) \
{ \
	(dst)->p=(src)->p; \
	(dst)->a.h=(float)(src)->a.h; \
	(dst)->a.p=(float)(src)->a.p; \
	(dst)->a.b=(float)(src)->a.b; \
	BiMakeTrigonomy(&(dst)->t,&(src)->a); \
}

#define BiConvLtoG(dst,src,axs) \
{ \
	BiRotFastLtoG((dst),(src),&(axs)->t); \
	BiAddPoint((dst),(dst),&(axs)->p); \
}

#define BiConvGtoL(dst,src,axs) \
{ \
	BiSubPoint((dst),(src),&(axs)->p); \
	BiRotFastGtoL((dst),(dst),&(axs)->t); \
}

#define BiSetPointS(p,xx,yy) {(p)->x=(long)(xx);(p)->y=(long)(yy);}

#define BiSetColorRGB(col,rr,gg,bb)  {(col)->r=(rr);(col)->g=(gg);(col)->b=(bb);}
/* )inline functions for C */
#else
/* (inline functions for C++ */
inline int BiSame(real x,real y)
{
	return (BiAbs(x-y)<=YSEPS ? BI_TRUE : BI_FALSE);
}

inline int BiSamePoint(BIPOINT *a,BIPOINT *b)
{
	if(BiSame(a->x,b->x)==BI_TRUE &&
	   BiSame(a->y,b->y)==BI_TRUE &&
	   BiSame(a->z,b->z)==BI_TRUE)
	{
		return BI_TRUE;
	}
	return BI_FALSE;
}

inline int BiSamePoint2(BIPOINT2 *a,BIPOINT2 *b)
{
	if(BiSame(a->x,b->x)==BI_TRUE &&
	   BiSame(a->y,b->y)==BI_TRUE)
	{
		return BI_TRUE;
	}
	return BI_FALSE;
}

#define BiSetPoint(a,b,c,d) BiSetPointBody((a),(real)(b),(real)(c),(real)(d))
inline void BiSetPointBody(BIPOINT *dst,real x,real y,real z)
{
	dst->x=x;
	dst->y=y;
	dst->z=z;
}

inline void BiAddPoint(BIPOINT *dst,BIPOINT *a,BIPOINT *b)
{
	dst->x = a->x+b->x;
	dst->y = a->y+b->y;
	dst->z = a->z+b->z;
}

inline void BiSubPoint(BIPOINT *dst,BIPOINT *a,BIPOINT *b)
{
	dst->x = a->x-b->x;
	dst->y = a->y-b->y;
	dst->z = a->z-b->z;
}

#define BiMulPoint(a,b,c) BiMulPointBody((a),(b),(real)(c))
inline void BiMulPointBody(BIPOINT *dst,BIPOINT *src,real m)
{
	dst->x=src->x*m;
	dst->y=src->y*m;
	dst->z=src->z*m;
}

#define BiDivPoint(a,b,c) BiDivPointBody((a),(b),(real)(c))
inline void BiDivPointBody(BIPOINT *dst,BIPOINT *src,real d)
{
	dst->x=src->x/d;
	dst->y=src->y/d;
	dst->z=src->z/d;
}

#define BiSetPoint2(a,b,c) BiSetPoint2Body((a),(real)(b),(real)(c))
inline void BiSetPoint2Body(BIPOINT2 *dst,real x,real y)
{
	dst->x=x;
	dst->y=y;
}

inline void BiAddPoint2(BIPOINT2 *dst,BIPOINT2 *a,BIPOINT2 *b)
{
	dst->x=a->x+b->x;
	dst->y=a->y+b->y;
}

inline void BiSubPoint2(BIPOINT2 *dst,BIPOINT2 *a,BIPOINT2 *b)
{
	dst->x=a->x-b->x;
	dst->y=a->y-b->y;
}

#define BiMulPoint2(a,b,c) BiMulPoint2Body((a),(b),(real)(c))
inline void BiMulPoint2Body(BIPOINT2 *dst,BIPOINT2 *src,real m)
{
	dst->x=src->x*m;
	dst->y=src->y*m;
}

#define BiDivPoint2(a,b,c) BiDivPoint2Body((a),(b),(real)(c))
inline void BiDivPoint2Body(BIPOINT2 *dst,BIPOINT2 *src,real d)
{
	dst->x=src->x/d;
	dst->y=src->y/d;
}

inline real BiInnerPoint2(BIPOINT2 *a,BIPOINT2 *b)
{
	return a->x*b->x +a->y*b->y;
}

inline real BiLengthPoint2(BIPOINT2 *a)
{
	return BiLength2(a->x,a->y);
}

inline real BiInnerPoint(BIPOINT *a,BIPOINT *b)
{
	return a->x*b->x +a->y*b->y +a->z*b->z;
}

inline real BiLength3(real x,real y,real z)
{
	return BiLength2(x,BiLength2(y,z));
}

inline real BiLengthPoint3(BIPOINT *p)
{
	return BiLength3(p->x,p->y,p->z);
}

inline void BiMakeLine(BILINE *ln,BIPOINT *a,BIPOINT *b)
{
	ln->o=*a;
	BiSubPoint(&ln->v,b,a);
}

inline void BiMakeLine2(BILINE2 *ln,BIPOINT2 *a,BIPOINT2 *b)
{
	ln->o=*a;
	BiSubPoint2(&ln->v,b,a);
}

inline void BiMakeLimLine(BILIMLINE *ln,BIPOINT *a,BIPOINT *b)
{
	ln->p1=*a;
	ln->p2=*b;
}

inline void BiMakeLimLine2(BILIMLINE2 *ln,BIPOINT2 *a,BIPOINT2 *b)
{
	ln->p1=*a;
	ln->p2=*b;
}

inline void BiLineToLimLine(BILIMLINE *lm,BILINE *ln)
{
	lm->p1=ln->o;
	BiAddPoint(&lm->p2,&ln->o,&ln->v);
}

inline void BiLimLineToLine(BILINE *ln,BILIMLINE *lm)
{
	BiMakeLine(ln,&lm->p1,&lm->p2);
}

inline void BiLineToLimLine2(BILIMLINE2 *lm,BILINE2 *ln)
{
	lm->p1=ln->o;
	BiAddPoint2(&lm->p2,&ln->o,&ln->v);
}

inline void BiLimLineToLine2(BILINE2 *ln,BILIMLINE2 *lm)
{
	BiMakeLine2(ln,&lm->p1,&lm->p2);
}

#define BiSetAngleDeg(a,b,c,d) BiSetAngleDegBody((a),(real)(b),(real)(c),(real)(d))
inline void BiSetAngleDegBody(BIATTITUDE *dst,real h,real p,real b)
{
	dst->h=(long)(h*0x10000/360);
	dst->p=(long)(p*0x10000/360);
	dst->b=(long)(b*0x10000/360);
}

inline void BiSetAngleRad(BIATTITUDE *dst,real h,real p,real b)
{
	dst->h=(long)(h*0x8000/YSPI);
	dst->p=(long)(p*0x8000/YSPI);
	dst->b=(long)(b*0x8000/YSPI);
}

#define BiSetAngle(a,b,c,d) BiSetAngleBody((a),(long)(b),(long)(c),(long)(d))
inline void BiSetAngleBody(BIATTITUDE *dst,long h,long p,long b)
{
	dst->h=h;
	dst->p=p;
	dst->b=b;
}

inline void BiRotLtoG(BIPOINT *dst,BIPOINT *src,BIATTITUDE *ang)
{
	BITRIGON tmpt;
	BIPOINT tmpp;
	BiMakeTrigonomy(&tmpt,ang);

	tmpp.x= tmpt.cosb*src->x -tmpt.sinb*src->y;
	tmpp.y= tmpt.sinb*src->x +tmpt.cosb*src->y;

	tmpp.z= tmpt.cosp*src->z - tmpt.sinp*tmpp.y;
	dst->y= tmpt.sinp*src->z + tmpt.cosp*tmpp.y;

	dst->x= tmpt.cosh*tmpp.x - tmpt.sinh*tmpp.z;
	dst->z= tmpt.sinh*tmpp.x + tmpt.cosh*tmpp.z;
}

inline void BiRotGtoL(BIPOINT *dst,BIPOINT *src,BIATTITUDE *ang)
{
	BITRIGON tmpt;
	BIPOINT tmpp;
	BiMakeTrigonomy(&tmpt,ang);

	tmpp.x=  tmpt.cosh*src->x + tmpt.sinh*src->z;
	tmpp.z= -tmpt.sinh*src->x + tmpt.cosh*src->z;

	dst->z=  tmpt.cosp*tmpp.z + tmpt.sinp*src->y;
	tmpp.y= -tmpt.sinp*tmpp.z + tmpt.cosp*src->y;

	dst->x=  tmpt.cosb*tmpp.x + tmpt.sinb*tmpp.y;
	dst->y= -tmpt.sinb*tmpp.x + tmpt.cosb*tmpp.y;
}

inline void BiRotFastLtoG(BIPOINT *dst,BIPOINT *src,BITRIGON *trg)
{
	BIPOINT tmpp;
	tmpp.x= trg->cosb*src->x - trg->sinb*src->y;
	tmpp.y= trg->sinb*src->x + trg->cosb*src->y;

	tmpp.z= trg->cosp*src->z - trg->sinp*tmpp.y;
	dst->y= trg->sinp*src->z + trg->cosp*tmpp.y;

	dst->x= trg->cosh*tmpp.x  - trg->sinh*tmpp.z;
	dst->z= trg->sinh*tmpp.x  + trg->cosh*tmpp.z;
}

inline void BiRotFastGtoL(BIPOINT *dst,BIPOINT *src,BITRIGON *trg)
{
	BIPOINT tmpp;
	tmpp.x=  trg->cosh*src->x + trg->sinh*src->z;
	tmpp.z= -trg->sinh*src->x + trg->cosh*src->z;

	dst->z=  trg->cosp*tmpp.z  + trg->sinp*src->y;
	tmpp.y= -trg->sinp*tmpp.z  + trg->cosp*src->y;

	dst->x=  trg->cosb*tmpp.x  + trg->sinb*tmpp.y;
	dst->y= -trg->sinb*tmpp.x  + trg->cosb*tmpp.y;
}

inline void BiProject(BIPOINTS *dst,BIPOINT *src,BIPROJ *prj)
{
	dst->x = prj->cx+(long)(src->x * prj->magx / src->z);
	dst->y = prj->cy-(long)(src->y * prj->magy / src->z);
}

inline void BiProject(BIPOINT2 *dst,BIPOINT *src,BIPROJ *prj)
{
	dst->x = prj->cx+(src->x * prj->magx / src->z);
	dst->y = prj->cy-(src->y * prj->magy / src->z);
}

inline void BiPlProject(int n,BIPOINTS *dst,BIPOINT *src,BIPROJ *prj)
{
	int i;
	for(i=0; i<n; i++)
	{
		BiProject(&dst[i],&src[i],prj);
	}
}

inline void BiPntAngToAxis(BIAXIS *dst,BIPOSATT *src)
{
	dst->p=src->p;
	dst->a=src->a;
	BiMakeTrigonomy(&dst->t,&src->a);
}

inline void BiMakeAxisF(BIAXISF *dst,BIPOSATT *src)
{
	dst->p=src->p;
	dst->a.h=(float)src->a.h;
	dst->a.p=(float)src->a.p;
	dst->a.b=(float)src->a.b;
	BiMakeTrigonomy(&dst->t,&src->a);
}

inline void BiConvLtoG(BIPOINT *dst,BIPOINT *src,BIAXIS *axs)
{
	BiRotFastLtoG(dst,src,&axs->t);
	BiAddPoint(dst,dst,&axs->p);
}

inline void BiConvGtoL(BIPOINT *dst,BIPOINT *src,BIAXIS *axs)
{
	BiSubPoint(dst,src,&axs->p);
	BiRotFastGtoL(dst,dst,&axs->t);
}

#define BiSetPointS(a,b,c) BiSetPointSBody((a),(long)(b),(long)(c))
inline void BiSetPointSBody(BIPOINTS *s,long x,long y)
{
	s->x=x;
	s->y=y;
}

inline void BiSetColorRGB(BICOLOR *col,int r,int g,int b)
{
	col->r=r;
	col->g=g;
	col->b=b;
}
/* )inline functions for C++ */
#endif




#endif /* } #ifndef __IMPULSE_H */
