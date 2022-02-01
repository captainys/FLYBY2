#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include "impulse.h"
#include "iutil.h"

static char *BiStrtok(char **src);

void BiArguments(int *ac,char *av[],int mxac,char *src)
{
	char *arg;
	int len;

	while(*src==' ' || *src=='\t')
	{
		src++;
	}

	len=strlen(src)-1;
	while(len>=0 && isprint(src[len])==0)
	{
		src[len]=0;
		len--;
	}

	*ac=0;
	while((arg=BiStrtok(&src))!=NULL && *ac<mxac)
	{
		av[*ac]=arg;
		(*ac)++;
	}
	av[*ac]=NULL;
}

static char *BiStrtok(char **src)
{
	char *r;
	switch(**src)
	{
	case 0:
		return NULL;
	case '\"':
		(*src)++;
		r=(*src);
		while((**src)!='\"' && (**src)!=0)
		{
			(*src)++;
		}
		break;
	default:
		r=(*src);
		while((**src)!=' ' && (**src)!='\t' && (**src)!=0)
		{
			(*src)++;
		}
		break;
	}
	if((**src)!=0)
	{
		(**src)=0;
		(*src)++;
	}
	while((**src)!=0 && ((**src)==' ' || (**src)=='\t'))
	{
		(*src)++;
	}
	return r;
}


int BiCommandNumber(int *cmd,char *in,char *lst[])
{
	int i,l;
	for(i=0; lst[i]!=NULL; i++)
	{
		l=strlen(lst[i]);
		if(strncmp(in,lst[i],l)==0)
		{
			*cmd=i;
			return BI_OK;
		}
	}
	*cmd=0;
	return BI_ERR;
}

int BiSeparatePathFile(char *pth,char *fil,char *org)
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
	return BI_OK;
}

void BiCapitalizeString(char str[])
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		if('a'<=str[i] && str[i]<='z')
		{
			str[i]=str[i]-'a'+'A';
		}
	}
}




/* {>>>>Load From String Extension>>>> */
typedef struct BiStrFuncList {
	struct BiStrFuncList *next;
	int GetStrMode;
	int nStr;
	char *(*iStr);
	FILE *ifp;
} BISTRFUNCLIST;

enum {
	FROM_NONE,
	FROM_FILE,
	FROM_STRING
};


static BISTRFUNCLIST BiFixed;
static BISTRFUNCLIST *StringFunc=&BiFixed;

int BiPushStringFunc(void)
{
	BISTRFUNCLIST *neo;
	neo=(BISTRFUNCLIST *)BiMalloc(sizeof(BISTRFUNCLIST));
	if(neo!=NULL)
	{
		neo->GetStrMode=FROM_NONE;

		neo->next=StringFunc;
		StringFunc=neo;
		return BI_OK;
	}
	return BI_ERR;
}

int BiPopStringFunc(void)
{
	BISTRFUNCLIST *prv;
	if(StringFunc!=&BiFixed)
	{
		prv=StringFunc;
		StringFunc=StringFunc->next;
		BiFree(prv);
	}
	return BI_ERR;
}

void BiSetFp(FILE *fp)
{
	StringFunc->ifp=fp;
	StringFunc->GetStrMode=FROM_FILE;
}

void BiSetStringList(char *(*lst))
{
	StringFunc->iStr=lst;
	StringFunc->GetStrMode=FROM_STRING;
	StringFunc->nStr=0;
}

char *BiGetStringFunc(char *str,int x)
{
	switch(StringFunc->GetStrMode)
	{
	case FROM_FILE:
		return fgets(str,x,StringFunc->ifp);
	case FROM_STRING:
		if(StringFunc->iStr[StringFunc->nStr]!=NULL)
		{
			strncpy(str,StringFunc->iStr[StringFunc->nStr],x);
			StringFunc->nStr++;
			return str;
		}
		else
		{
			str[0]=0;
			return NULL;
		}
	}
}
/* <<<<Load From String Extension<<<<} */



static char exBuf[256],*exBufPtr;

void BiInitGetStringFuncEx(void)
{
	exBuf[0]=0;
	exBufPtr=exBuf;
}
char *BiGetStringFuncEx(char str[],int nStr)
{
	int i;
	if(exBufPtr[0]==0)
	{
		if(BiGetStringFunc(exBuf,255)==NULL)
		{
			return NULL;
		}
		exBufPtr=exBuf;
	}
	for(i=0; i<nStr && *exBufPtr!=0 && *exBufPtr!='|'; exBufPtr++)
	{
		str[i]=*exBufPtr;
		i++;
	}
	str[i]=0;
	if(*exBufPtr=='|')
	{
		exBufPtr++;
	}
	return str;
}



