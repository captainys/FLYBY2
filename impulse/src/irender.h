#define BI_RENDER_MAX_VTX 64

#define cutoff(a,b,c) (((b)<=(a) && (a)<=(c)) ? (a) : ((a)<(b) ? (b) : (c)))


int BiRenderStartPolygon(int n,long *p);
int BiRenderEndPolygon(void);
int BiRenderCheckInside(long x,long y);
int BiRenderGetCrossPoint(int *nx,long *x,long y);


int BiRenderInitialize(void);
int BiRenderGetReflection(real *amb,real *dif,real *spe,real *spePow);
int BiRenderGetScreen(int *x1,int *y1,int *x2,int *y2,int *cx,int *cy);
int BiRenderGetProjection(real *mag,real *cpz);
int BiRenderSetLightPosition(BIPOINT *pnt);
int BiRenderSetObjectNumber(int num);
int BiRenderSetReflection(real amb,real dif,real spe,real spePow);
int BiRenderSetScreen(int x1,int y1,int x2,int y2,int cx,int cy);
int BiRenderSetProjection(real mag,real cpz);
int BiRenderSetEyePosition(BIPOSATT *eyePos);
int BiRenderSetDrawFunc(void (*drawFunc)(int,int,int,int,int,int,real));
int BiRenderSet2DDrawFunc(void (*drawFunc)(int,int,int,int,int,int));
int BiRenderSetAbortFunc(int (*abortFunc)(void));
int BiRenderSetShadMode(int sw);
int BiRenderGetShadMode(int *sw);
int BiRenderDrawPolygonTwoSide(int np,BIPOINT p[],BICOLOR *c);
int BiRenderDrawPolygonOneSide(int np,BIPOINT p[],BIPOINT *nom,BICOLOR *c);
int BiRenderDrawPolygon(int np,BIPOINT p[],BIPOINT n[],BICOLOR *c);
int BiRenderDrawRoundPolygonOneSide(int np,BIPOINT *p,BIPOINT *n,BIPOINT *avgNom,BICOLOR *c);
int BiRenderDrawRoundPolygonTwoSide(int np,BIPOINT *p,BIPOINT *n,BICOLOR *c);
int BiRenderDrawRoundPolygon3(BIPOINT *p,BIPOINT *n,BICOLOR *c);

int BiRenderDrawLine(BIPOINT *p1,BIPOINT *p2,BICOLOR *col);
int BiRenderDrawPset(BIPOINT *p,BICOLOR *col);
int BiRenderDrawCircle(BIPOINT *p,real r,BICOLOR *col);

void BiRenderDrawLine2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *);
void BiRenderDrawPset2(BIPOINTS *s,BICOLOR *);
void BiRenderDrawEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *);
void BiRenderDrawPolygon2(int ns,BIPOINTS *s,BICOLOR *);
void BiRenderDrawRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *);
void BiRenderDrawMarker2(BIPOINTS *s,int mkType,BICOLOR *);
void BiRenderDrawString2(BIPOINTS *s,char *str,BICOLOR *);
void BiRenderDrawText2(BIPOINTS *s,char *str[],int h,int v,BICOLOR *);
void BiRenderDrawEmptyRect2(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *ol);
void BiRenderDrawEmptyPolygon2(int n,BIPOINTS *s,BICOLOR *ol);
void BiRenderDrawPolyline2(int n,BIPOINTS *s,BICOLOR *ol);
void BiRenderDrawEmptyEllipse2(BIPOINTS *s,long radx,long rady,BICOLOR *);

int BiRenderTrianglize(int *nt,int *t,int np,BIPOINT *p);
int BiRenderTrianglize2(int *nt,int *t,int np,BIPOINT2 *p);
