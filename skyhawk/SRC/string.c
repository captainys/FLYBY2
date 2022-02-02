#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "skyhawk.h"


void SkStringHead(char **line)
{
	char *ptr=(*line);

	while(*ptr != 0)
	{
		if(isprint(*ptr) && *ptr!=' ')
		{
			break;
		}
		ptr++;
	}

	(*line)=ptr;
}


void SkStringTail(char *l)
{
	unsigned int chr;
	int i,cut,len;

	len=strlen(l);
	cut=0;
	for(i=0; i<len; i++)
	{
		chr=((unsigned char *)l)[i];

		if((0x81<=chr && chr<=0x9f) || 0xe0<=chr)
		{
			cut=i+2;
			i++;
			continue;
		}
		else if(isprint(chr) || chr==' ')
		{
			cut=i+1;
			continue;
		}
	}

	for(i=cut; i<len; i++)
	{
		l[i]=0;
	}
}

void SkCapitalize(char l[])
{
	int i;
	for(i=0; l[i]!=0; i++)
	{
		if('a'<=l[i] && l[i]<='z')
		{
			l[i]=l[i]+'A'-'a';
		}
	}
}

void SkUncapitalize(char l[])
{
	int i;
	for(i=0; l[i]!=0; i++)
	{
		if('A'<=l[i] && l[i]<='Z')
		{
			l[i]=l[i]+'a'-'A';
		}
	}
}
