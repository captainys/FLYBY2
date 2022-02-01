#include <stdio.h>
#include <stdlib.h>


/* #define TESTMODE */
/* ^^^^^^^^^^^^^^^^ define TESTMODE to run without sorting */

typedef struct StringTag {
	struct StringTag *next;
	char str[1];
} STRING;

typedef struct BlockTag {
	struct BlockTag *prev,*next;
	STRING *top,*btm;
	STRING *refTop;
} BLOCK;


void PrintList(BLOCK *blk);
void ExtractRef(BLOCK *top);
void AnalyzeRef(BLOCK *top,BLOCK *ptr);
BLOCK *LookForBlock(BLOCK *top,char str[]);
int SwitchRef(int ref,char str[]);
void CrossLink(BLOCK *target,BLOCK *org);
BLOCK *ReadFile(void);
BLOCK *ReadBlock(char fst[]);
STRING *FirstString(char str[]);
STRING *AppendString(STRING *btm,char str[]);
STRING *NewString(char str[]);
BLOCK *AppendBlock(BLOCK *top,BLOCK *neo);
void AppendRefString(BLOCK *blk,char str[]);

int main(void)
{
	BLOCK *top;
	top=ReadFile();
	ExtractRef(top);
	AnalyzeRef(top,top);
	PrintList(top);
	printf("------------\n");
}

void PrintList(BLOCK *blk)
{
	int i,ref,col;
	STRING *ptr;
	if(blk!=NULL)
	{
		PrintList(blk->prev);
		printf("------------\n");
		ref=0;
		for(ptr=blk->top; ptr!=NULL; ptr=ptr->next)
		{
			ref=SwitchRef(ref,ptr->str);
			if(ref==0)
			{
				printf("%s\n",ptr->str);
			}
		}
		if(blk->refTop!=NULL)
		{
			printf("REFERENCE:\n");
			printf("  ");
			col=2;
			for(ptr=blk->refTop; ptr!=NULL; ptr=ptr->next)
			{
				if(col+strlen(ptr->str)>=72)
				{
					printf("\n");
					printf("  ");
					col=2;
				}

				col+=printf("%s",ptr->str)+1;
				if(ptr->next!=NULL)
				{
					printf(",");
				}
			}
			printf("\n");
		}
		PrintList(blk->next);
	}
}

void ExtractRef(BLOCK *blk)
{
	int ref;
	STRING *str;

	if(blk!=NULL)
	{
		ref=0;
		for(str=blk->top; str!=NULL; str=str->next)
		{
			ref=SwitchRef(ref,str->str);
			if(ref==1 && strncmp(str->str,"REFERENCE:",10)!=0)
			{
				int i,j;
				char ext[256];
				j=0;
				i=0;
				while(1)
				{
					if(str->str[i]==',' || str->str[i]==' ' || str->str[i]==0)
					{
						if(j>0)
						{
							ext[j]=0;
							j=0;
							AppendRefString(blk,ext);
						}
					}
					else
					{
						ext[j++]=str->str[i];
					}

					if(str->str[i]==0)
					{
						break;
					}
					i++;
				}
			}
		}
		ExtractRef(blk->prev);
		ExtractRef(blk->next);
	}
}

void AnalyzeRef(BLOCK *top,BLOCK *blk)
{
	BLOCK *crs;
	STRING *prv,*ref,*nam;
	if(blk!=NULL)
	{
		prv=NULL;
	RETRYREF:
		for(ref=blk->refTop; ref!=NULL; ref=ref->next)
		{
			for(nam=blk->top; nam!=NULL; nam=nam->next)
			{
				if(strncmp(nam->str,"NAME:",5)==0 &&
				   strcmp(nam->str+5,ref->str)==0)
				{
					fprintf(stderr,"Omitted %s@%s\n",ref->str,blk->top->str+5);
					if(prv==NULL)
					{
						blk->refTop=ref->next;
					}
					else
					{
						prv->next=ref->next;
					}
					goto RETRYREF;
				}
			}
			prv=ref;
		}

		for(ref=blk->refTop; ref!=NULL; ref=ref->next)
		{
			crs=LookForBlock(top,ref->str);
			if(crs==NULL)
			{
				fprintf(stderr,"Broken Link %s@%s\n",ref->str,blk->top->str+5);
			}
			else if(crs!=blk)
			{
				CrossLink(crs,blk);
			}
		}

		AnalyzeRef(top,blk->prev);
		AnalyzeRef(top,blk->next);
	}
}

int SwitchRef(int ref,char str[])
{
	if(strncmp(str,"REFERENCE:",10)==0)
	{
		return 1;
	}
	else
	{
		int i;
		for(i=0; str[i]!=0; i++)
		{
			if(str[i]==':')
			{
				return 0;
			}
		}
	}
	return ref;
}


BLOCK *LookForBlock(BLOCK *top,char tag[])
{
	BLOCK *ret;
	STRING *str;

	if(top!=NULL)
	{
		for(str=top->top; str!=NULL; str=str->next)
		{
			if(strncmp(str->str,"NAME:",5)==0 && strcmp(str->str+5,tag)==0)
			{
				return top;
			}
		}

		if((ret=LookForBlock(top->prev,tag))!=NULL)
		{
			return ret;
		}
		if((ret=LookForBlock(top->next,tag))!=NULL)
		{
			return ret;
		}
	}

	return NULL;
}

void CrossLink(BLOCK *target,BLOCK *org)
{
	STRING *ref,*nam;
	char *str;

	for(nam=org->top; nam!=NULL; nam=nam->next)
	{
		if(strncmp(nam->str,"NAME:",5)==0)
		{
			str=nam->str+5;
			for(ref=target->refTop; ref!=NULL; ref=ref->next)
			{
				if(strcmp(ref->str,str)==0)
				{
					break;
				}
			}

			if(ref==NULL)
			{
				AppendRefString(target,str);
				fprintf(stderr,"Cross Linked %s to %s\n",str,target->top->str+5);
			}
		}
	}
}

BLOCK *ReadFile(void)
{
	BLOCK *top;
	char str[2048];

	top=NULL;
	while(gets(str)!=NULL)
	{
		if(strncmp(str,"NAME:",5)==0)
		{
			top=AppendBlock(top,ReadBlock(str));
		}
	}
	return top;
}

BLOCK *ReadBlock(char fst[])
{
	char str[2048];
	BLOCK *neo;

	neo=(BLOCK *)malloc(sizeof(BLOCK));
	if(neo!=NULL)
	{
		neo->prev=NULL;
		neo->next=NULL;
		neo->top=FirstString(fst);
		neo->btm=neo->top;
		neo->refTop=NULL;
		while(gets(str)!=NULL && strncmp(str,"----------",10)!=0)
		{
			neo->btm=AppendString(neo->btm,str);
		}
		return neo;
	}
}

STRING *FirstString(char str[])
{
	STRING *neo;
	neo=NewString(str);
	if(neo!=NULL)
	{
		return neo;
	}
	printf("Malloc Error in First String.\n");
	exit(1);
}

STRING *AppendString(STRING *btm,char str[])
{
	STRING *neo;
	neo=NewString(str);
	if(neo!=NULL)
	{
		btm->next=neo;
		return neo;
	}
	fprintf(stderr,"Malloc Error in Append String.\n");
	exit(1);
}

void AppendRefString(BLOCK *blk,char str[])
{
	STRING *neo;
	neo=NewString(str);
	if(neo!=NULL)
	{
		if(blk->refTop==NULL)
		{
			blk->refTop=neo;
		}
		else if(strcmp(str,blk->refTop->str)<0)
		{
			neo->next=blk->refTop;
			blk->refTop=neo;
		}
		else
		{
			STRING *ref;
			for(ref=blk->refTop; ref->next!=NULL; ref=ref->next)
			{
				if(strcmp(ref->str,str)==0)
				{
					fprintf(stderr,"Double reference %s@%s is omitted\n",str,blk->top->str);
					return;
				}

				if(strcmp(ref->str,str)<0 && strcmp(str,ref->next->str)<0)
				{
					break;
				}
			}
			neo->next=ref->next;
			ref->next=neo;
		}
		return;
	}
	fprintf(stderr,"Malloc Error in Append String.\n");
	exit(1);
}

STRING *NewString(char str[])
{
	STRING *neo,*add;
	int len;

	len=strlen(str);
	neo=(STRING *)malloc(sizeof(STRING *)+(len+1)*sizeof(char));
	if(neo!=NULL)
	{
		neo->next=NULL;
		strcpy(neo->str,str);
	}
	return neo;
}

BLOCK *AppendBlock(BLOCK *top,BLOCK *neo)
{
	int cmp;
	if(top==NULL)
	{
		neo->prev=NULL;
		neo->next=NULL;
		return neo;
	}
	else
	{
#ifdef TESTMODE
		BLOCK *ptr;
		for(ptr=top; ptr->next!=NULL; ptr=ptr->next);
		ptr->next=neo;
		return top;
#else
		cmp=strcmp(top->top->str,neo->top->str);
		if(cmp>0)
		{
			if(top->prev==NULL)
			{
				top->prev=neo;
			}
			else
			{
				AppendBlock(top->prev,neo);
			}
		}
		else
		{
			if(top->next==NULL)
			{
				top->next=neo;
			}
			else
			{
				AppendBlock(top->next,neo);
			}
		}
		return top;
#endif
	}
}
