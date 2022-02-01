#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************************************************************

Environment Variable BIMAN should be set as follows.

In Windows :
  set BIMAN=c:/docs/impulse.txt;c:/docs/hotas.txt

In Unix :
  setenv BIMANN "/home/ys11/pub/impulse.txt;/home/ys11/pub/hotas.txt"

***********************************************************************/

/***********************************************************************/
static char *ExtractFile(char fn[],char *ptr);
static void SearchFiles(char key[]);
static int LookForBlock(char fn[],char tag[]);
static void Capitalize(char neo[],char org[]);
static void InitCandidate(void);
static void AppendCandidate(char str[]);
static void ShowCandidate(void);
/***********************************************************************/


int main(int ac,char *av[])
{
	if(ac!=2)
	{
		printf("Usage : biman <name>\n");
		exit(1);
	}

	SearchFiles(av[1]);

	return 0;
}

static char *ExtractFile(char fn[],char *ptr)
{
	if(ptr[0]==';')
	{
		ptr++;
	}

	if(ptr[0]!=0)
	{
		int i;
		for(i=0; ptr[i]!=';' && ptr[i]!=0; i++)
		{
			fn[i]=ptr[i];
		}
		fn[i]=0;
		return ptr+i;
	}
	return NULL;
}

static void SearchFiles(char key[])
{
	char fn[256],*str;

	str=getenv("BIMAN");
	if(str==NULL)
	{
		printf("Environment Variable BIMAN is not set.\n");
		exit(1);
	}

	InitCandidate();

	while((str=ExtractFile(fn,str))!=NULL)
	{
		if(LookForBlock(fn,key)!=0)
		{
			return;
		}
	}

	ShowCandidate();
}

static int LookForBlock(char fn[],char tag[])
{
	FILE *fp;
	char str[256];
	char cNam[256],cTag[256];
	int lTag,lNam,i;

	fp=fopen(fn,"r");
	if(fp==NULL)
	{
		printf("Warning : Can't open manual file %s.\n",fn);
		return 0;
	}

	lTag=strlen(tag);
	Capitalize(cTag,tag);
	while(fgets(str,256,fp)!=NULL)
	{
		if(strncmp(str,"NAME:",5)==0)
		{
			Capitalize(cNam,str+5);
			if(strcmp(cNam,cTag)==0)
			{
				goto FOUND;
			}
			else
			{
				lNam=strlen(cNam);
				for(i=0; lTag+i<=lNam; i++)
				{
					if(strncmp(cTag,cNam+i,lTag)==0)
					{
						AppendCandidate(str);
						break;
					}
				}
			}
		}
	}

NOTFOUND:
	fclose(fp);
	return 0;
FOUND:
	printf("----------\n");
	printf(str);
	while(fgets(str,256,fp)!=NULL && strncmp(str,"--------",8)!=0)
	{
		if(strncmp(str,"NAME:",5)!=0)
		{
			printf(str);
		}
	}
	printf("----------\n");
	fclose(fp);
	return 1;
}

static void Capitalize(char neo[],char org[])
{
	int i;
	for(i=0; org[i]!=0; i++)
	{
		if('a'<=org[i] && org[i]<'z')
		{
			neo[i]=org[i]+'A'-'a';
		}
		else if(org[i]=='\n')
		{
			neo[i]=0;
		}
		else
		{
			neo[i]=org[i];
		}
	}
	neo[i]=0;
}

typedef struct CandidateTag {
	struct CandidateTag *next;
	char str[256];
} CANDIDATE;

CANDIDATE *top=NULL,*btm=NULL;

static void InitCandidate(void)
{
	top=NULL;
	btm=NULL;
}

static void AppendCandidate(char str[])
{
	CANDIDATE *neo;
	neo=(CANDIDATE *)malloc(sizeof(CANDIDATE));
	if(neo!=NULL)
	{
		neo->next=NULL;
		strcpy(neo->str,str);

		if(top==NULL)
		{
			top=neo;
			btm=neo;
		}
		else
		{
			btm->next=neo;
			btm=neo;
		}
	}
}

static void ShowCandidate(void)
{
	CANDIDATE *ptr;

	if(top==NULL)
	{
		printf("No candidates are found.\n");
		return;
	}
	else if(top->next==NULL)
	{
		printf("One Candidate is found.\n");
		SearchFiles(top->str+5);
	}
	else
	{
		int i,n;
		char str[256];

		printf("--------Candidates\n");
		i=1;
		for(ptr=top; ptr!=NULL; ptr=ptr->next)
		{
			printf("[%d]%s",i,ptr->str);
			i++;
		}
		printf("--------\n");

		printf("Select #");
		gets(str);
		n=atoi(str);

		ptr=top;
		for(i=1; i<n && ptr!=NULL; i++)
		{
			ptr=ptr->next;
		}
		if(ptr!=NULL)
		{
			SearchFiles(ptr->str+5);
		}
	}
}

