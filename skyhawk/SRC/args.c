#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "skyhawk.h"

/***********************************************************************/
static char *SkStrtok(char **src);

int SkArguments(int *ac,char *av[],int mxac,char *src)
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
	while((arg=SkStrtok(&src))!=NULL && *ac<mxac)
	{
		av[*ac]=arg;
		(*ac)++;
	}
	av[*ac]=NULL;
	return SK_OK;
}

static char *SkStrtok(char **src)
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


/***********************************************************************/
int SkCommandNumber(int *cmd,char *in,char *lst[])
{
	int i,l;
	for(i=0; lst[i]!=NULL; i++)
	{
		l=strlen(lst[i]);
		if(strncmp(in,lst[i],l)==0)
		{
			*cmd=i;
			return SK_OK;
		}
	}
	*cmd=0;
	return SK_ERR;
}

