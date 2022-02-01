pragma On(Align_all_labels);

#define SEG_VRAMP 0x120      /* Segment:  VRAM Pararel */
#define SEG_VRAMS 0x128      /* Segment:  VRAM Single  */
#define SEG_VIRTU (getds())  /* Segment:  Vertual VRAM */

#define ADR(x) (int)(x)

#ifndef CONCORDE
#define CONCORDE
typedef struct
{
	int pen,col;          /* pen:色識別番号  col:実際に書き込まれる色 */
	int wdx,wdy;          /* ページの大きさ                           */
	int vx1,vy1,vx2,vy2;  /* 書き込み枠                               */
	int seg,adr;          /* ピクセル格納セグメント,アドレス          */
	unsigned short *plt;  /* 仮想パレットへのポインタ                 */

	int odw,oup;          /* アドレスオフセット 上移動,下移動         */
} PAGE;
#endif


/* setting.c */
PAGE *YGH_initPage(PAGE *p,int seg,int adr,int wdx,int wdy);
int   YGH_deletePaletteBlock(PAGE *p);
int   YGH_color(PAGE *p,int pen);
int   YGH_makePaletteBlock(PAGE *p);
int   YGH_palette(PAGE *p,int pen,int col);
int   YGH_viewPort(PAGE *p,int vx1,int vy1,int vx2,int vy2);



int   YGH_areaPaint(PAGE *p,int *plg,int *edg);

int   YGH_box(PAGE *p,int x1,int y1,int x2,int y2);

int   YGH_circle(PAGE *p,int x,int y,int r);
int   YGH_circleArea(PAGE *p,int *plg,int *edg,int x,int y,int r);

int   YGH_clearPage(PAGE *p,int col);
int   YGH_clearView(PAGE *p,int col);

int   YGH_filledBox(PAGE *p,int x1,int y1,int x2,int y2);

int   YGH_get(PAGE *p,char *dat,int x1,int y1,int x2,int y2);
int   YGH_getMono(PAGE *p,char *dat,int x1,int y1,int x2,int y2,int clr);

int   YGH_getColor(PAGE *p,int x,int y);

int   YGH_line(PAGE *p,int x1,int y1,int x2,int y2);
int   YGH_lineClip(PAGE *p,int *x1,int *y1,int *x2,int *y2);
int   YGH_connect(PAGE *p,int *pnt);
int   YGH_unConnect(PAGE *p,int *pnt);
int   YGH_emptyPolygon(PAGE *p,int *pnt);


int   YGH_pageCopy(PAGE *destination,PAGE *source);
int   YGH_pagePartCopy(PAGE *,int,int,PAGE *,int,int,int,int);

int   YGH_polygonArea(PAGE *p,int *plg,int *edg,int *pnt);

int   YGH_pset(PAGE *p,int x,int y);
int   YGH_psets(PAGE *p,int *pnt);

int   YGH_putMono(PAGE *p,char *dat,int x1,int y1,int x2,int y2);
int   YGH_put(PAGE *p,char *dat,int x1,int y1,int x2,int y2);
int   YGH_putZoom(PAGE *p,char *dat,int lx,int ly,int x1,int y1,int x2,int y2);

int   YGH_print8font(PAGE *p,int x,int y,char *dat);
int   YGH_print5font(PAGE *p,int x,int y,char *dat);

int   YGH_triangleArea(PAGE *p,int *plg,int *edg,int *pnt);
