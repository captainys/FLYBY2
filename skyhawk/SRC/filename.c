#include <stdio.h>
#include <string.h>

#include "skyhawk.h"

void SkPutExt(char *fname,char *ext)
{
	int i;

	for(i=0; fname[i]!=0; i++)
	{
		if(fname[i]=='.')
		{
			return;
		}
	}

	if(*ext == '*')
	{
		ext++;
	}
	if(*ext == '.')
	{
		ext++;
	}

	sprintf(fname,"%s.%s",fname,ext);
}

unsigned long SkFileSize(char *fname)
{
	FILE *fp;
    unsigned long edptr;

	fp=fopen(fname,"rb");
	if(fp==NULL)
	{
		return 0;
	}

    fseek(fp,0,2 /* SEEK_END */);
    edptr=ftell(fp);

	fclose(fp);

    return edptr;
}

int SkFileExist(char *fn)
{
	FILE *fp;
	fp=fopen(fn,"rb");
	fclose(fp);

	if(fp!=NULL)
	{
		return SK_YES;
	}
	else
	{
		return SK_NO;
	}
}

int SkSeparatePathFile(char *pth,char *fil,char *org)
{
	char tmp[256],*seekPtr,*cutPtr;

	strcpy(tmp,org);
	cutPtr=tmp;

	/* Skip Drive Name */
	if(tmp[1]==':')
	{
		cutPtr+=2;
	}

	/* scan until nul Stopper */
	for(seekPtr=cutPtr; *seekPtr!=0; seekPtr++)
	{
		if(*seekPtr=='\\' || *seekPtr=='/' || *seekPtr==':')
		{
			cutPtr=seekPtr+1;
		}
	}

	/* cutPtr points *tmp  or after ':' or after last '\\' */
	strcpy(fil,cutPtr);

	*cutPtr = 0;
	strcpy(pth,tmp);
	return SK_OK;
}
