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
} BLOCK;


void PrintList(BLOCK *blk);
BLOCK *ReadFile(void);
BLOCK *ReadBlock(char fst[]);
STRING *FirstString(char str[]);
STRING *AppendString(STRING *btm,char str[]);
STRING *NewString(char str[]);
BLOCK *AppendBlock(BLOCK *top,BLOCK *neo);

int main(void)
{
	BLOCK *top;
	top=ReadFile();
	PrintList(top);
}

void PrintList(BLOCK *blk)
{
	STRING *ptr;
	if(blk!=NULL)
	{
		PrintList(blk->prev);
		printf("----------\n");
		for(ptr=blk->top; ptr!=NULL; ptr=ptr->next)
		{
			printf("%s\n",ptr->str);
		}
		PrintList(blk->next);
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
	printf("Malloc Error in Append String.\n");
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
