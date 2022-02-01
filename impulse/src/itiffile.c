#include <stdio.h>
#include <stddef.h>

#include "impulse.h"



static void ShortToChar(unsigned char *chr,unsigned short dat);
static void LongToChar(unsigned char *chr,unsigned long dat);


static FILE *fp;
static long tWid,tHei;
static unsigned char *buf;

static unsigned char hed[]={'M','M',0,0x2a,0,0,0,8,0,0x0f};
static unsigned char fil[]={0,8,0,8,0,8, 0,255,0,255,0,255};

static void WriteTag(FILE *fp,int tag,int type,long lng,long value);

int BiOpenTiff24(char fnOrg[],long wid,long hei)
{
	long a;
	unsigned char pad[290];
	char fn[256];

	BiConstrainFileName(fn,fnOrg);

	/* !! buf=(unsigned char *)BiMalloc(wid*3,sizeof(char)); */
	buf=(unsigned char *)BiMalloc(wid*3*sizeof(unsigned char));
	if(buf!=NULL)
	{
		fp=fopen(fn,"wb");
		if(fp!=NULL)
		{
			tWid=wid;
			tHei=hei;

			fwrite(hed,1,10,fp);
			WriteTag(fp,0x0fe,4,1,0);
			WriteTag(fp,0x100,3,1,wid<<16);
			WriteTag(fp,0x101,3,1,hei<<16);
			WriteTag(fp,0x102,3,3,0x1e4);
			WriteTag(fp,0x103,3,1,(long)1<<16);
			WriteTag(fp,0x106,3,1,(long)2<<16);
			WriteTag(fp,0x10A,3,1,(long)1<<16);
			WriteTag(fp,0x111,4,1,0x200);
			WriteTag(fp,0x115,3,1,(long)3<<16);
			WriteTag(fp,0x116,4,1,hei);
			WriteTag(fp,0x117,4,1,wid*hei*3);
			WriteTag(fp,0x119,3,3,0x1ea);
			WriteTag(fp,0x11a,5,1,0x1f0);
			WriteTag(fp,0x11b,5,1,0x1f8);
			WriteTag(fp,0x11c,3,1,(long)1<<16);
			a=0;
			fwrite(&a,1,4,fp);
			/* 290 Bytes Padding */
			for(a=0; a<290; a++)
			{
				pad[a]=0;
			}
			fwrite(pad,1,290,fp);
			fwrite(fil,1,12,fp);

			LongToChar(pad   ,0x4b);
			LongToChar(pad+ 4,1);
			LongToChar(pad+ 8,0x4b);
			LongToChar(pad+12,1);
			fwrite(pad,1,16,fp);
			return BI_OK;
		}
	}
	return BI_ERR;
}

static void WriteTag(FILE *fp,int tag,int type,long lng,long value)
{
	unsigned char dat[12];

	ShortToChar(dat  ,(short)tag);
	ShortToChar(dat+2,(short)type);
	LongToChar (dat+4,lng);
	LongToChar (dat+8,value);
	fwrite(dat,1,12,fp);
}


int BiWriteLineTiff24(BICOLOR *dat)
{
	int i,j;
	j=0;
	for(i=0; i<tWid; i++)
	{
		buf[j  ]=(unsigned char)dat[i].r;
		buf[j+1]=(unsigned char)dat[i].g;
		buf[j+2]=(unsigned char)dat[i].b;
		j+=3;
	}
	fwrite(buf,1,j,fp);
	return BI_OK;
}

int BiCloseTiff24(void)
{
	BiFree(buf);
	fclose(fp);
	return BI_OK;
}




/***********************************************************************/
static void ShortToChar(unsigned char *chr,unsigned short dat)
{
	chr[0]=(unsigned char)((dat>>8)&255);
	chr[1]=(unsigned char)(dat&255);
}

static void LongToChar(unsigned char *chr,unsigned long dat)
{
	chr[0]=(unsigned char)((dat>>24)&255);
	chr[1]=(unsigned char)((dat>>16)&255);
	chr[2]=(unsigned char)((dat>> 8)&255);
	chr[3]=(unsigned char)(dat&255);
}

