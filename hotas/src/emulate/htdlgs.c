#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "hotas.h"


extern void BiArguments(int *ac,char *av[],int mxac,char str[]);


static int HtReturnValueYesNo(char yes[],char no[])
{
	char yn[16],*msg1,*msg2;

	msg1=(yes!=NULL ? yes : "OK");
	msg2=(no!=NULL ? no : "CANCEL");

	yn[0]=0;
	while(yn[0]!='1' && yn[0]!='2')
	{
		printf("1:%s    2:%s\n",msg1,msg2);
		printf(">");
		gets(yn);
		printf("**************\n");
	}
	return (yn[0]=='1' ? HT_YES : HT_NO);
}



void HtMessageBox(char *msg,char *btn)
{
	char str[80];
	printf("**************\n");
	printf("%s\n",msg);
	printf("Hit Return To Continue\n");
	printf(">");
	gets(str);
	printf("**************\n");
}

void HtMessageBox3(char *msg[3],char *btn)
{
	int i;
	char str[80];
	printf("**************\n");
	for(i=0; i<3; i++)
	{
		if(msg[i]!=NULL)
		{
			printf("%s\n",msg[i]);
		}
	}
	printf("Hit Return To Continue\n");
	printf(">");
	gets(str);
	printf("**************\n");
}

int HtLineInput(char *str,char *msg,char *def)
{
	printf("**************\n");
	printf("%s\n",msg);
	printf(">");
	gets(str);
	printf("**************\n");
	return HT_YES;
}

int HtYesNo(char *msg,char *yes,char *no)
{
	printf("**************\n");
	printf("%s\n",msg);
	return HtReturnValueYesNo(yes,no);
}

int HtYesNo3(char *msg[3],char *yes,char *no)
{
	int i;
	printf("**************\n");
	for(i=0; i<3; i++)
	{
		if(msg[i]!=NULL)
		{
			printf("%s\n",msg[i]);
		}
	}
	return HtReturnValueYesNo(yes,no);
}

int HtInputNumber(real *n,real dflt,char *msg,char *yes,char *no)
{
	char str[128];
	printf("**************\n");
	printf("%s\n",msg);
	printf("(Default Value:%f)\n",dflt);

	printf(">");
	gets(str);
	if(str[0]==0 || str[0]=='\n')
	{
		*n=dflt;
		printf("%f\n",dflt);
	}
	else
	{
		*n=(real)atof(str);
	}
	printf("**************\n");
	return HT_YES;
}

int HtInputInteger(int *n,int dflt,char *msg,char *yes,char *no)
{
	char str[128];
	printf("**************\n");
	printf("%s\n",msg);
	printf("(Default Value:%d)\n",dflt);

	printf(">");
	gets(str);
	if(str[0]==0 || str[0]=='\n')
	{
		*n=dflt;
		printf("%d\n",dflt);
	}
	else
	{
		*n=atoi(str);
	}
	printf("**************\n");
	return HT_YES;
}

int HtInputPoint2(BIPOINT2 *p,BIPOINT2 *df,char *msg,char *y,char *n)
{
	int ac;
	char str[128],*av[16];
	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("(Default Value:%f %f)\n",df->x,df->y);
	}
	ac=0;
	while(ac!=2)
	{
		printf("Answer X Y\n");
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			p->x=(real)atof(av[0]);
			p->y=(real)atof(av[1]);
			printf("%.2f %.2f\n",atof(av[0]),atof(av[1]));
			goto EXIT;
		}
	}
	BiSetPoint2(p,atof(av[0]),atof(av[1]));
EXIT:
	printf("**************\n");
	return HT_YES;
}

int HtInputPoint2i(int p[2],int df[2],char *msg,char *y,char *n)
{
	int ac;
	char str[128],*av[16];
	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("(Default Value:%d %d)\n",df[0],df[1]);
	}
	ac=0;
	while(ac!=2)
	{
		printf("Answer X Y\n");
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			p[0]=df[0];
			p[1]=df[1];
			printf("%d %d\n",p[0],p[1]);
			goto EXIT;
		}
	}
	p[0]=atoi(av[0]);
	p[1]=atoi(av[1]);
EXIT:
	printf("**************\n");
	return HT_YES;
}

int HtInputPoint3(BIPOINT *p,BIPOINT *df,char *msg,char *y,char *n)
{
	int ac;
	char str[128],*av[16];
	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("(Default Value:%f %f %f)\n",df->x,df->y,df->z);
	}
	ac=0;
	while(ac!=3)
	{
		printf("Answer X Y Z\n");
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			*p=*df;
			printf("%.2f %.2f %.2f\n",df->x,df->y,df->z);
			goto EXIT;
		}
	}
	BiSetPoint(p,atof(av[0]),atof(av[1]),atof(av[2]));
EXIT:
	printf("**************\n");
	return HT_YES;
}

int HtInputPoint3i(int p[3],int df[3],char *msg,char *y,char *n)
{
	int ac;
	char str[128],*av[16];
	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("(Default Value:%d %d %d)\n",df[0],df[1],df[2]);
	}
	ac=0;
	while(ac!=3)
	{
		printf("Answer X Y Z\n");
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			p[0]=df[0];
			p[1]=df[1];
			p[2]=df[2];
			printf("%d %d %d\n",df[0],df[1],df[2]);
			goto EXIT;
		}
	}
	p[0]=atoi(av[0]);
	p[1]=atoi(av[1]);
	p[2]=atoi(av[2]);
EXIT:
	printf("**************\n");
	return HT_YES;
}

int HtInputAngle3(BIANGLE *a,BIANGLE *df,char *msg,char *y,char *n)
{
	int ac;
	char str[128],*av[16];
	long h,p,b;

	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		h=df->h*90/16384;
		p=df->p*90/16384;
		b=df->b*90/16384;
		printf("(Default Value:%d %d %d)\n",h,p,b);
	}
	ac=0;
	while(ac!=3)
	{
		printf("HEADING PITCH BANK>");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			*a=*df;
			goto EXIT;
		}
	}
	a->h=(long)atoi(av[0])*16384/90;
	a->p=(long)atoi(av[1])*16384/90;
	a->b=(long)atoi(av[2])*16384/90;
EXIT:
	printf("**************\n");
	return HT_YES;
}

int HtLoadFileDialog(char *fn,char *msg,char *df,char *ext)
{
	int ac,i;
	char str[128],*av[16];

	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("Default Value:%s\n",df);
	}
	ac=0;
	while(ac!=1)
	{
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			strcpy(str,df);
			BiArguments(&ac,av,15,str);
			if(ac>0)
			{
				break;
			}
		}
	}

	for(i=0; av[0][i]!=0; i++)
	{
		if(av[0][i]=='/')
		{
			strcpy(fn,av[0]);
			return HtReturnValueYesNo("OK","CANCEL");
		}
	}

	sprintf(fn,"./%s",av[0]);
	return HtReturnValueYesNo("OK","CANCEL");
}

int HtSaveFileDialog(char *fn,char *msg,char *df,char *ext)
{
	int ac,i;
	char str[128],*av[16];

	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("Default Value:%s\n",df);
	}
	ac=0;
	while(ac!=1)
	{
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			strcpy(str,df);
			BiArguments(&ac,av,15,str);
			if(ac>0)
			{
				break;
			}
		}
	}

	for(i=0; av[0][i]!=0; i++)
	{
		if(av[0][i]=='/')
		{
			strcpy(fn,av[0]);
			return HtReturnValueYesNo("OK","CANCEL");
		}
	}

	sprintf(fn,"./%s",av[0]);
	return HtReturnValueYesNo("OK","CANCEL");
}

int HtColorDialog(BICOLOR *col,BICOLOR *df,char *msg)
{
	int ac;
	char str[128],*av[16];

	printf("**************\n");
	printf("%s\n",msg);
	if(df!=NULL)
	{
		printf("Default Value:%d %d %d\n",df->r,df->g,df->b);
	}
	ac=0;
	while(ac!=3)
	{
		printf("Input R G B  (0->255 each)\n");
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==0 && df!=NULL)
		{
			*col=*df;
			goto EXIT;
		}
	}
	col->r=atoi(av[0]);
	col->g=atoi(av[1]);
	col->b=atoi(av[2]);
	col->r=BiLarger(col->r,0);
	col->g=BiLarger(col->g,0);
	col->b=BiLarger(col->b,0);
	col->r=BiSmaller(col->r,255);
	col->g=BiSmaller(col->g,255);
	col->b=BiSmaller(col->b,255);
EXIT:
	printf("**************\n");
	return HT_YES;
}

int HtListBox(int *n,char *lst[],char *msg,char *yes,char *no)
{
	int ac;
	char str[128],*av[16];
	int i,nStr;

	printf("**************\n");
	printf("%s\n",msg);
	for(i=0; lst[i]!=NULL; i++)
	{
		printf("[%3d]:%s\n",i,lst[i]);
	}

	nStr=i;
	*n=-1;
	ac=0;
	while(ac!=1 || *n<0 || nStr<=*n)
	{
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac==1)
		{
			*n=atoi(av[0]);
		}
	}
	printf("**************\n");
	return HT_YES;
}

int HtListBoxPl(int *nm,int n[],char *lst[],char *msg,char *yes,char *no)
{
	int ac;
	char str[128],*av[16];
	int i,nStr;

	printf("**************\n");
	printf("%s\n",msg);
	for(i=0; lst[i]!=NULL; i++)
	{
		printf("[%3d]:%s\n",i,lst[i]);
	}
	nStr=i;
	ac=0;
	while(ac==0)
	{
		printf(">");
		gets(str);
		BiArguments(&ac,av,15,str);
		if(ac>0)
		{
			for(i=0; i<ac; i++)
			{
				if(atoi(av[i])<0 || nStr<=atoi(av[i]))
				{
					ac=0;
					break;
				}
				n[i]=atoi(av[i]);
			}
		}
	}
	*nm=ac;
	printf("**************\n");
	return HT_YES;
}
