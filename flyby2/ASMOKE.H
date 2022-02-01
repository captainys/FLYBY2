#include <impulse.h>

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
#define ARS_ALLSMOKE   ~0

typedef struct {
	int stp[3];     /* Step inside [0]:Min BBox [1]:Mid Bbox [3]:Max BBox */
	BIPOINT bbx[3]; /* Bounding Box  [0]:min  [1]:mid  [2]:max */
	int sw;
	ARSMOKEATTR rbn;
	ARSMOKEATTR wir;
	ARSMOKEATTR trl;
	ARSMOKEATTR sld;
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
