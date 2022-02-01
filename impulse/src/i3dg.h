enum {
	BIOBJ_NULL,
	BIOBJ_PSET,
	BIOBJ_LINE,
	BIOBJ_CIRCLE,
	BIOBJ_POLYLINE,
	BIOBJ_LINESEQ,
	BIOBJ_POLYGON,
	BIOBJ_MARKER,
	BIOBJ_TEXT,
	BIOBJ_SRFMODEL,
	BIOBJ_PICT2,
	BIOBJ_RPOLYGON,
	BIOBJ_SUBTREE
};


typedef struct {
	BIPOINT p;
} BIONEPOINT;

typedef struct {
	int clip;
	BIPOINT p[2];
} BITWOPOINT;

typedef struct {
	int clip,n;
	BIPOINT *p;
	int nomSw;
	BIPOINT cen,nom;
} BINPOINT;

typedef struct {
	int clip,np;
	BIPOINT cen,*p,*n;
} BINPNTNOM;

typedef struct {
	BIPOINT p;
	real r;
} BICIRCLE;

typedef struct {
	BIPOINT p;
	int mkType;
} BIMARKER;

typedef struct {
	BIPOINT p;
	int hori,vert;
	int nLine;
	char *(*str);
} BITEXT;

typedef struct {
	int clip;
	BIAXISF axs;
	BISRFMODEL *mdl;
} BISRFMODELPOS;

typedef struct {
	int clip;
	BIAXISF axs;
	BIPICT2 *pc2;
} BIPICT2POS;

typedef struct
{
	struct BiObject *subTree;
	struct BiObject *subHead;
} BISUBTREE;

typedef union {
	BIONEPOINT onePnt;
	BITWOPOINT twoPnt;
	BINPOINT nPnt;
	BINPNTNOM nPntNom;
	BICIRCLE circ;
	BIMARKER mark;
	BITEXT text;
	BISRFMODELPOS smdl;
	BIPICT2POS pc2;
	BISUBTREE subTree;    // 2004/05/30: For accelerating Non-OpenGL version YSFLIGHT.
} BIOBJUNION;


typedef struct BiObject {
	struct BiObject *prev,*next;
	struct BiObject *treePrev,*treeNext;
	BICOLOR col;
	real z;
	int type;
	BIOBJUNION attr;
} BIOBJECT;



extern int BiPrjMode,BiShadMode;
extern real BiOrthoDist;
extern BIPROJ BiPrj;
extern real BiPrjFovLeft,BiPrjFovRight,BiPrjFovUp,BiPrjFovDown;
extern BIAXISF BiEyeAxs,BiMdlAxs,BiCnvAxs,BiShadowAxs;

#define BiSetOrthoPers(p) if(BiPrjMode==BIPRJ_ORTHO){(p)->z=BiOrthoDist;}


void BiFlushDrawSrf(BISRF *mdl,BIAXISF *mdlAx,int clip);
void BiFlushDrawPc2(BIPC2 *pct,BIAXISF *axs,int clip);



/* i3dgobj.c */
BIOBJECT *BiTopObject(void);
BIOBJECT *BiNew3dObject(int type,real z);
void BiClear3dObject(void);

/* i3dgbbox.c */
int BiBboxInsideViewPort(int *clip,BIPOINT *bbox);
int BiCheckInsideViewPort(int *clip,int np,BIPOINT *p);
