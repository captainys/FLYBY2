#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "impulse.h"




static FILE *fp;
static long tWid,tHei;
static unsigned char *buf;

int BiOpenPpm24(char fnOrg[],long wid,long hei)
{
	char head[80];
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
			fwrite("P6\n",1,3,fp);

			sprintf(head,"%d %d\n",tWid,tHei);
			fwrite(head,1,strlen(head),fp);

			fwrite("255\n",1,4,fp);

 			return BI_OK;
		}
	}
	return BI_ERR;
}

int BiWriteLinePpm24(BICOLOR *dat)
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

int BiClosePpm24(void)
{
	BiFree(buf);
	fclose(fp);
	return BI_OK;
}
