/* WinG Bitmap Direct Access Manager   :  Code Name = WildCat */
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* #include "../impulse.h" */

#include <windowsx.h>
#include "iwildcat.h"



static HPALETTE WinGPal=NULL;
static int cPal[512];
/***********************************************************************
void WcSetWinGPalette(HPALETTE pal);

  Note:
    ColorIndex |XXXX|XXXX|XXXX|
                Grn  Red  Blue
***********************************************************************/
static void WcMakeColorTable(int tab[512],HPALETTE pal);
static void WcSetNearestIndex(int *tab,PALETTEENTRY *ent,int r,int g,int b);
static void WcNumToRGB(int *r,int *g,int *b,int i);

int WcSetWinGPalette(HPALETTE pal)
{
	WinGPal=pal;
	WcMakeColorTable(cPal,pal);
	return WC_OK;
}

static void WcMakeColorTable(int tab[],HPALETTE pal)
{
	int i,r,g,b;
	PALETTEENTRY ent[256];

	GetPaletteEntries(pal,0,256,ent);
	for(i=0; i<512; i++)
	{
		tab[i]=255;
		WcNumToRGB(&r,&g,&b,i);
		WcSetNearestIndex(&tab[i],ent,r,g,b);
	}
}

static void WcNumToRGB(int *r,int *g,int *b,int i)
{
	*g = ((i>>6)&7)*255/7;
	*r = ((i>>3)&7)*255/7;
	*b = ((i   )&7)*255/7;
}

static void WcSetNearestIndex(int *tab,PALETTEENTRY *ent,int r,int g,int b)
{
	int i;
	long r1,g1,b1,r2,g2,b2,dif,tmp;

	r1=(long)r;
	g1=(long)g;
	b1=(long)b;
	dif=0x30001; /* Maximum difference is 0x30000 */
	for(i=0; i<256; i++)
	{
		r2=(long)ent[i].peRed;
		g2=(long)ent[i].peGreen;
		b2=(long)ent[i].peBlue;
		tmp=(r1-r2)*(r1-r2)+(g1-g2)*(g1-g2)+(b1-b2)*(b1-b2);
		if(tmp<dif)
		{
			*tab=i;
			dif=tmp;
		}
	}
}



/***********************************************************************
void WcGetColorIndex8(int *idx,WCCOLOR *col);
void WcGetColorIndex16(int *idx,WCCOLOR *col);
***********************************************************************/
int WcGetColorIndex8(int *idx,WCCOLOR *col)
{
	*idx=cPal[(((col->g>>5)&7)<<6)+(((col->r>>5)&7)<<3)+((col->b>>5)&7)];
	return WC_OK;
}

int WcGetColorIndex16(int *idx,WCCOLOR *col)
{
	*idx=((col->r>>3)<<10)+((col->g>>3)<<5)+(col->b>>3);
	return WC_OK;
}



/***********************************************************************
int WcCreateBmp(WCBMP *bm,int wid,int hei,int bpp);
    Return
      0 No Error
      1 Error Occured
***********************************************************************/
static void WcAdjustWidth(int *wid);
static int WcCreateDC(WCBMP *bm,HDC WndDc,int wid,int hei,int bpp);
static int WcCreateBitmap(WCBMP *bm,int wid,int hei);
static void WcSwapRedBlue(RGBQUAD *col);
static void WcGetDIBFormat(WCBMP *bm,WCBMPINFO *inf,int wid,int hei);
static int WcCreateLineTop(WCBMP *bm,WCBMPINFO *inf,int wid,int hei);
static int WcCreatePolygBuffer(WCBMP *bm,WCBMPINFO *inf,int wid,int hei);


int WcCreateBmp(WCBMP *bm,HDC WndDc,int wid,int hei,int bpp)
{
	bm->alive=0;
	WcAdjustWidth(&wid);
	if(WcCreateDC(bm,WndDc,wid,hei,bpp)==WC_OK)
	{
		bm->alive=1;
		return 0;
	}
	return 1;
}

static void WcAdjustWidth(int *wid)
{
	int tmp,bin;
	tmp=*wid;
	for(bin=8; bin<tmp; bin=bin*2);
	*wid=bin;
}

static int WcCreateDC(WCBMP *bm,HDC WndDc,int wid,int hei,int bpp)
{
	if(bpp==0)
	{
		bpp=GetDeviceCaps(WndDc,BITSPIXEL);
	}

	bm->dc=CreateCompatibleDC(WndDc);
	bm->wid=wid;
	bm->hei=hei;
	bm->bpp=bpp;
	SetTextAlign(bm->dc,TA_LEFT|TA_BASELINE);
	if(bm->dc!=NULL)
	{
		SetBkMode(bm->dc,TRANSPARENT);
		SetPolyFillMode(bm->dc,ALTERNATE);
		if(WcCreateBitmap(bm,wid,hei)==WC_OK)
		{
			return WC_OK;
		}
		DeleteDC(bm->dc);
	}
	return WC_ERR;
}

static int WcCreateBitmap(WCBMP *bm,int wid,int hei)
{
	void *bit;
	WCBMPINFO inf=
	{
		sizeof(BITMAPINFOHEADER),
		0,0,1,8,BI_RGB,0,0,0,0,0
	};

	if(bm->bpp>=16)
	{
		bm->bpp=16;
	}
	else
	{
		bm->bpp=8;
		GetPaletteEntries(WinGPal,0,256,(PALETTEENTRY *)inf.col);
		WcSwapRedBlue(inf.col);
	}

	WcGetDIBFormat(bm,&inf,wid,hei);

	bm->bmp=CreateDIBSection(bm->dc,(BITMAPINFO *)&inf,DIB_RGB_COLORS,&bit,NULL,0);
	if(bm->bmp!=NULL)
	{
		bm->dat=(unsigned char *)bit;
		if(WcCreateLineTop(bm,&inf,wid,hei)==WC_OK)
		{
			bm->org=SelectBitmap(bm->dc,bm->bmp);
			PatBlt(bm->dc,0,0,wid,hei,BLACKNESS);
			return WC_OK;
		}
		DeleteObject(bm->bmp);
	}
	return WC_ERR;
}

static void WcSwapRedBlue(RGBQUAD *col)
{
	/* Because RGBQUAD & PALETTEENTRY is different. */
	int i,r,b;
	for(i=0; i<256; i++)
	{
		r=col[i].rgbBlue;
		b=col[i].rgbRed;
		col[i].rgbRed=r;
		col[i].rgbBlue=b;
	}
}

static void WcGetDIBFormat(WCBMP *bm,WCBMPINFO *inf,int wid,int hei)
{
	/**********************************************
	bm->ori=WC_TOPDOWN;
	WinGRecommendDIBFormat((BITMAPINFO *)inf);
	bm->ori=(inf->hdr.biHeight<0 ? WC_TOPDOWN : WC_BOTTOMUP);
	inf->hdr.biWidth=wid;
	inf->hdr.biHeight*=hei;
    ***********************************************/

	bm->ori=WC_TOPDOWN;
	inf->hdr.biWidth=wid;
	inf->hdr.biHeight=-hei;

	inf->hdr.biBitCount=bm->bpp;

	/* Force Uncompressed */
	inf->hdr.biCompression=BI_RGB;
}

static int WcCreateLineTop(WCBMP *bm,WCBMPINFO *inf,int wid,int hei)
{
	int y,byt;
	unsigned char *(*top);
	unsigned char *ptr;

	top=(unsigned char *(*))malloc(hei*sizeof(unsigned char *));
	byt=((wid+3)/4)*4*bm->bpp/8;
	if(top!=NULL)
	{
		ptr=bm->dat;
		for(y=0; y<hei; y++)
		{
			switch(bm->ori)
			{
			default:
			case WC_TOPDOWN:
				top[y]=(unsigned char *)ptr;
				break;
			case WC_BOTTOMUP:
				top[hei-1-y]=(unsigned char *)ptr;
				break;
			}
			ptr+=byt;
		}
		if(WcCreatePolygBuffer(bm,inf,wid,hei)==WC_OK)
		{
			bm->hTop=top;
			return WC_OK;
		}
		free(top);
	}
	return WC_ERR;
}

static int WcCreatePolygBuffer(WCBMP *bm,WCBMPINFO *inf,int wid,int hei)
{
	int i;
	bm->rgn.y1=0;
	bm->rgn.y2=0;
	bm->rgn.x=(unsigned *)malloc(WC_RGN_X_PER_LINE*hei*sizeof(unsigned));
	if(bm->rgn.x!=NULL)
	{
		for(i=0; i<hei; i++)
		{
			bm->rgn.x[i*WC_RGN_X_PER_LINE]=0;
		}
		return WC_OK;
	}
	return WC_ERR;
}


/***********************************************************************
void WcDeleteBmp(WCBMP *bm);
***********************************************************************/
int WcDeleteBmp(WCBMP *bm)
{
	if(bm!=NULL && bm->alive==1)
	{
		free(bm->hTop);
		free(bm->rgn.x);
		SelectObject(bm->dc,bm->org);
		DeleteObject(bm->bmp);
		DeleteDC(bm->dc);
		bm->alive=0;
	}
	return WC_OK;
}


/***********************************************************************
void WcPset(WCBMP *bm,int x,int y,WCCOLOR *col);
***********************************************************************/
void WcPset(WCBMP *bm,int x,int y,WCCOLOR *col)
{
	int c;
	if(bm->bpp<16)
	{
		WcGetColorIndex8(&c,col);
		bm->hTop[y][x]=c;
	}
	else
	{
		WcGetColorIndex16(&c,col);
		((short *)(bm->hTop[y]))[x]=c;
	}
}


/***********************************************************************
***********************************************************************/
int WcDrawBitmapFont(WCBMP *bm,int bottomLeftX,int bottomLeftY,const char str[],WCCOLOR *col,unsigned char *fontPtr[],int fontWid,int fontHei)
{
	int bmpY0,bmpX,fontY0;
	int strPtr;

	const unsigned int bytePerFontLine=((fontWid+31)/32)*4;

	const unsigned int bmpWid=bm->wid;
	const unsigned int bmpHei=bm->hei;

	int colIdx;

	if(bottomLeftY<0)
	{
		fontY0=-bottomLeftY;
		bmpY0=0;
	}
	else
	{
		fontY0=0;
		bmpY0=bottomLeftY;
	}

	if(bm->bpp<16)
	{
		WcGetColorIndex8(&colIdx,col);
	}
	else
	{
		WcGetColorIndex16(&colIdx,col);
	}
	


	bmpX=bottomLeftX;
	for(strPtr=0; 0!=str[strPtr]; strPtr++,bmpX+=fontWid)
	{
		const unsigned char c=str[strPtr];
		unsigned char *fontLineTop;

		int fontY=fontY0;
		int bmpY=bmpY0;

		int x;
		unsigned int fontX;
		unsigned int bit;

		if(bmpX<=-fontWid)
		{
			continue;
		}

		while(fontY<fontHei && bmpY<bmpHei)
		{
			fontLineTop=fontPtr[c]+bytePerFontLine*(fontHei-fontY-1);

			fontX=0;
			bit=fontLineTop[fontX];

			x=0;

			while(x+bmpX<0)
			{
				bit<<=1;
				if(7==(x&7))
				{
					bit=fontLineTop[++fontX];
				}
				x++;
			}

			for(x=x; x<fontWid && x+bmpX<bmpWid; x++)
			{
				if(bit&0x80)
				{
					if(bm->bpp<16)
					{
						bm->hTop[bmpY][bmpX+x]=colIdx;
					}
					else
					{
						((short *)(bm->hTop[bmpY]))[bmpX+x]=colIdx;
					}
				}

				bit<<=1;
				if(7==(x&7))
				{
					bit=fontLineTop[++fontX];
				}
			}

			fontY++;
			bmpY++;
		}
	}

	return 0;
}


/***********************************************************************
void WcLine(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col);
***********************************************************************/
void WcLine(WCBMP *bm,int x1,int y1,int x2,int y2,WCCOLOR *col)
{
	int c,x,y,vx,vy;
	int dx,dy,sum;

	if(bm->bpp<16)
	{
		WcGetColorIndex8(&c,col);

		x=x1;
		y=y1;
		dx=WcAbs(x2-x1);
		dy=WcAbs(y2-y1);
		vx=WcSgn(x2-x1);
		vy=WcSgn(y2-y1);
		sum=0;
		if(dx==0)
		{
			while(dy>=0)
			{
				bm->hTop[y][x]=c;
				y+=vy;
				dy--;
			}
		}
		else if(dy==0)
		{
			while(dx>=0)
			{
				bm->hTop[y][x]=c;
				x+=vx;
				dx--;
			}
		}
		else if(dx>dy)
		{
			while(x!=x2 || y!=y2)
			{
				bm->hTop[y][x]=c;
				if(sum>=0)
				{
					x+=vx;
					sum=sum-dy;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum-dy+dx;
				}
			}
			bm->hTop[y][x]=c;
		}
		else if(dx<dy)
		{
			while(x!=x2 || y!=y2)
			{
				bm->hTop[y][x]=c;
				if(sum<=0)
				{
					y+=vy;
					sum=sum+dx;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum+dx-dy;
				}
			}
			bm->hTop[y][x]=c;
		}
		else /* dx==dy */
		{
			while(x!=x2)
			{
				bm->hTop[y][x]=c;
				x+=vx;
				y+=vy;
			}
			bm->hTop[y][x]=c;
		}
	}
	else
	{
		WcGetColorIndex16(&c,col);

		x=x1;
		y=y1;
		dx=WcAbs(x2-x1);
		dy=WcAbs(y2-y1);
		vx=WcSgn(x2-x1);
		vy=WcSgn(y2-y1);
		sum=0;
		if(dx==0)
		{
			while(dy>=0)
			{
				((short *)(bm->hTop[y]))[x]=c;
				y+=vy;
				dy--;
			}
		}
		else if(dy==0)
		{
			while(dx>=0)
			{
				((short *)(bm->hTop[y]))[x]=c;
				x+=vx;
				dx--;
			}
		}
		else if(dx>dy)
		{
			while(x!=x2 || y!=y2)
			{
				((short *)(bm->hTop[y]))[x]=c;
				if(sum>=0)
				{
					x+=vx;
					sum=sum-dy;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum-dy+dx;
				}
			}
			((short *)(bm->hTop[y]))[x]=c;
		}
		else if(dx<dy)
		{
			while(x!=x2 || y!=y2)
			{
				((short *)(bm->hTop[y]))[x]=c;
				if(sum<=0)
				{
					y+=vy;
					sum=sum+dx;
				}
				else
				{
					x+=vx;
					y+=vy;
					sum=sum+dx-dy;
				}
			}
			((short *)(bm->hTop[y]))[x]=c;
		}
		else /* dx==dy */
		{
			while(x!=x2)
			{
				((short *)(bm->hTop[y]))[x]=c;
				x+=vx;
				y+=vy;
			}
			((short *)(bm->hTop[y]))[x]=c;
		}
	}
}


/***********************************************************************
void WcTestPattern(WCBMP *bm);
***********************************************************************/
int WcTestPattern(WCBMP *bm)
{
	char *top;
	int c,cx,cy,x,y;
	if(bm->alive==1)
	{
		for(y=0; y<bm->hei; y++)
		{
			cy=y*16/bm->hei;
			top=bm->hTop[y];
			for(x=0; x<bm->wid; x++)
			{
				cx=x*16/bm->wid;
				c=cy*16+cx;
				top[x]=c;
			}
		}
	}
	return WC_OK;
}

int WcTestPattern2(WCBMP *bm)
{
	char *top;
	WCCOLOR col;
	int c,x,y;
	if(bm->alive==1)
	{
		for(y=0; y<bm->hei; y++)
		{
			top=bm->hTop[y];
			col.r=y*255/bm->hei;
			for(x=0; x<bm->wid; x++)
			{
				col.b=     x*255/bm->wid ;
				col.g=255-(x*255/bm->wid);
				WcGetColorIndex8(&c,&col);
				top[x]=c;
			}
		}
	}
	return WC_OK;
}

int WcTestPattern3(WCBMP *bm)
{
	char *top;
	WCCOLOR col;
	int c,x,y;
	if(bm->alive==1)
	{
		for(y=0; y<bm->hei; y++)
		{
			top=bm->hTop[y];
			c=y*8/bm->hei;
			col.b=(c&1)*255;
			col.r=(c&2)*255/2;
			col.g=(c&4)*255/4;
			WcGetColorIndex8(&c,&col);
			for(x=0; x<bm->wid; x++)
			{
				top[x]=c;
			}
		}
	}
	return WC_OK;
}


/* For More Functions (^_^;) */
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
/***********************************************************************
***********************************************************************/
