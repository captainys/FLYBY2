/***********************************************************************
使用上の注意
  このカスタム描画ルーチンは、座標のチェックなど一切してません。だから、
  ここに渡す画面座標は、絶対にWcCreateBmpで作ったビットマップの範囲を越
  えないようにしてください。(要するにエラー処理は呼出し元でやってね、と
  いうことで。)
***********************************************************************/

/* Orientation */
enum {
	WC_TOPDOWN,
	WC_BOTTOMUP
};

/* Error or OK */
enum {
	WC_OK,
	WC_ERR
};

typedef struct {
	BITMAPINFOHEADER hdr;
	RGBQUAD col[256];
} WCBMPINFO;

#define WC_RGN_X_PER_LINE 16

typedef struct {
	int y1,y2;
	unsigned *x;
} WCRGN;

typedef struct {
	int alive;
	HDC dc;
	HBITMAP org,bmp;
	HPALETTE pal;
	unsigned char *dat;
	unsigned char *(*hTop);
	int ori;
	int wid,hei;
	WCRGN rgn;
} WCBMP;


typedef struct {
	long x,y;
} WCPNT;

typedef struct {
	unsigned g,r,b;
} WCCOLOR;


/* iwildcat.c */
int WcSetWinGPalette(HPALETTE pal);
int WcCreateBmp(WCBMP *bm,int wid,int hei);
int WcDeleteBmp(WCBMP *bm);
int WcGetColorIndex(int *idx,WCCOLOR *col);

void WcPset(WCBMP *bm,int x,int y,WCCOLOR *col);
void WcLine(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col);

int WcTestPattern(WCBMP *bm);
int WcTestPattern2(WCBMP *bm);
int WcTestPattern3(WCBMP *bm);



/* iwcpolyg.c */
int WcSetRegion(WCBMP *bm,int np,WCPNT *p);
int WcPaintRegion(WCBMP *bm,WCCOLOR *col);
int WcRect(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col);
int WcSetCircleRegion(WCBMP *bm,WCPNT *c,long rad);
int WcSetEllipseRegion(WCBMP *bm,WCPNT *c,long radx,long rady);


/* iwccolor.c */
void WcColorHSVtoGRB(int *g,int *r,int *b,int h,int s,int v);
void WcColorGRBtoHSV(int *h,int *s,int *v,int g,int r,int b);


#define WcAbs(x) ((x)>=0 ? (x) : -(x))
#define WcSgn(x) ((x)==0 ? 0 : ((x)>0 ? 1 : -1))
