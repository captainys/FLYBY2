/*
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

BITMAPFILEHEADER     14bytes
 2 WORD   "BM"
 4 DWORD  fileSize(#ofBytes/4)
 2 WORD   0
 2 WORD   0
 4 DWORD  bytes from BITMAPFILEHEADER to the bitmap (offset of bitmap)
BITMAPINFOHEADER     40bytes
 4 DWORD  sizeof(BITMAPINFOHEADER)
 4 DWORD  width
 4 DWORD  height  (Negative Number means Top-Down DIB)
 2 WORD   1
 2 WORD   bit count(24)
 4 DWORD  BI_RGB(Not compressed)
 4 DWORD  image size  0 is allowed for BI_RGB bitmap
 4 LONG   biXPelsPerMeter nearly equals to 2048
 4 LONG   biYPelsPerMeter nearly equals to 2048
 4 DWORD  biClrUsed  0 is allowed ?
 4 DWORD  biClrImportant  0 is allowed ?
bitmapdata
*/


#include <stdio.h>
#include <stddef.h>

#include "impulse.h"


#define SIZEOFBMFILE 14
#define SIZEOFBMINFO 40


/***********************************************************************/
static void ShortToCharI(unsigned char chr[],unsigned short dat)
{
	chr[0]=(unsigned char)(dat&255);
	chr[1]=(unsigned char)((dat>>8)&255);
}

static void LongToCharI(unsigned char chr[],unsigned long dat)
{
	chr[3]=(unsigned char)((dat>>24)&255);
	chr[2]=(unsigned char)((dat>>16)&255);
	chr[1]=(unsigned char)((dat>> 8)&255);
	chr[0]=(unsigned char)(dat&255);
}


/***********************************************************************/
static FILE *fp=NULL;
static unsigned char *buf=NULL;
static long tWid,tHei;

int BiOpenBmp24(char fnOrg[],long wid,long hei)
{
	unsigned long fsize;
	unsigned char hdr[SIZEOFBMFILE+SIZEOFBMINFO];
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	fp=fopen(fn,"wb");
	buf=(unsigned char *)BiMalloc(sizeof(unsigned char)*wid*3);
	if(fp!=NULL && buf!=NULL)
	{
		tWid=wid;
		tHei=hei;

		fsize=SIZEOFBMFILE+SIZEOFBMINFO+wid*hei*3;

		hdr[0]='B';
		hdr[1]='M';
		LongToCharI (hdr+ 2,fsize/4);
		ShortToCharI(hdr+ 6,0);
		ShortToCharI(hdr+ 8,0);
		LongToCharI (hdr+10,SIZEOFBMFILE+SIZEOFBMINFO);

		LongToCharI (hdr+14,SIZEOFBMINFO);
		LongToCharI (hdr+18,(unsigned long)wid);
		LongToCharI (hdr+22,(unsigned long)hei);
		ShortToCharI(hdr+26,1);
		ShortToCharI(hdr+28,24);
		LongToCharI (hdr+30,0 /* BI_RGB (This case, BI means Bitmap Info */);
		LongToCharI (hdr+34,0);
		LongToCharI (hdr+38,0);
		LongToCharI (hdr+42,0);
		LongToCharI (hdr+46,0);
		LongToCharI (hdr+50,0);

		fwrite(hdr,1,SIZEOFBMFILE+SIZEOFBMINFO,fp);

		return BI_OK;
	}
	return BI_ERR;
}

int BiWriteLineBmp24(BICOLOR *dat)
{
	int i,j;
	j=0;
	for(i=0; i<tWid; i++)
	{
		buf[j  ]=(unsigned char)dat[i].b;
		buf[j+1]=(unsigned char)dat[i].g;
		buf[j+2]=(unsigned char)dat[i].r;
		j+=3;
	}
	fwrite(buf,1,j,fp);
	return BI_OK;
}

int BiCloseBmp24(void)
{
	BiFree(buf);
	fclose(fp);
	buf=NULL;
	fp=NULL;
	return BI_OK;
}

