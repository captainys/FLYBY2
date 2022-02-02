#include <stdio.h>
#include <string.h>

#include "skyhawk.h"

/***********************************************************************/
static void SkAppendSlash(char *dir);
static void SkAnalyze(char *dst,char *str,char *dir);
static void SkRemoveSamePart(char **str,char **dir);
static void SkMoveUpward(char *dst,char *dir);
static void SkAntiMacintosh(char rel[]);

#define SkIsSprtr(a) ((a)=='/' || (a)=='\\' || (a)==':')

int SkRelativePath(char *dst,char *src,char *org)
{
	char str[256],dir[256];

	strcpy(str,src);
	strcpy(dir,org);
	SkAppendSlash(dir);
	SkAnalyze(dst,str,dir);
	SkAntiMacintosh(dst);
	return SK_OK;
}

static void SkAntiMacintosh(char rel[])
{
	int i;
	/* Change ':' into '/' because Macintosh doesn't know what the relative path is. */
	for(i=0; rel[i]!=0; i++)
	{
		if(rel[i]==':')
		{
			rel[i]='/';
		}
	}
}

static void SkAppendSlash(char *dir)
{
	int ln;
	ln=strlen(dir);
	if(!SkIsSprtr(dir[ln-1]))
	{
		dir[ln  ]='/';
		dir[ln+1]=0;
	}
}

static void SkAnalyze(char *dst,char *str,char *dir)
{
	char relPath[256];
	SkRemoveSamePart(&str,&dir);
	SkMoveUpward(relPath,dir);
	sprintf(dst,"%s%s",relPath,str);
}

static void SkRemoveSamePart(char **str,char **dir)
{
	int n;
	char *a,*b;

	a=*str;
	b=*dir;
	while(*a!=0 && *b!=0)
	{
		while(SkIsSprtr(*a) && SkIsSprtr(*b))
		{
			a++;
			b++;
		}

		n=0;
		while(a[n]!=0 && b[n]!=0 && !SkIsSprtr(a[n]) && !SkIsSprtr(b[n]))
		{
			n++;
		}
		if(n==0 || strncmp(a,b,n)!=0)
		{
			break;
		}
		a+=n;
		b+=n;
	}
	*str=a;
	*dir=b;
}

static void SkMoveUpward(char *dst,char *dir)
{
	dst[0]=0;
	while(*dir!=0)
	{
		if(SkIsSprtr(*dir))
		{
			strcpy(dst,"../");
			dst+=3;
		}
		dir++;
	}
}
