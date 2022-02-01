#ifndef __REAL_DEFINED
#define __REAL_DEFINED
typedef float real;
#endif

typedef struct {
	real pLft,pBtm; /* Left,Bottom of Paper */
	real pWid,pHei; /* Window Size On Paper */
	real sWid,sHei; /* Window Size On Screen */
	FILE *fp;       /* File Handle */
} BIEPSHANDLE;



BIEPSHANDLE *BiBeginEps
	(char *fn,long px,long py,long pWid,long pHei,long sWid,long sHei);
void BiEndEps(BIEPSHANDLE *ep);
int BiDrawEpsRect
    (BIEPSHANDLE *ep,long x1,long y1,long x2,long y2,int r,int g,int b);
int BiDrawEpsLine
	(BIEPSHANDLE *ep,long x1,long y1,long x2,long y2,int r,int g,int b);
int BiDrawEpsPset(BIEPSHANDLE *ep,long x1,long y1,int r,int g,int b);
int BiDrawEpsPolygon(BIEPSHANDLE *ep,int np,long *p,int r,int g,int b);
int BiDrawEpsString(BIEPSHANDLE *ep,char *str,long x,long y,int r,int g,int b);
int BiDrawEpsEllipse
    (BIEPSHANDLE *ep,long x,long y,long radx,long rady,int r,int g,int b);
