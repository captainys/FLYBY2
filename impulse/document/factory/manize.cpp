#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/***********************************************************************/
class HeaderFile
{
public:
	HeaderFile();
	int open(char fn[]);
	int close(void);
	char *GetOneFunction(char func[]);
protected:
	FILE *fp;
	int tableUsed;
	char table[2048];

	int secUsed;
	char second[2048];

	int funcUsed;
	char *func;

	void AppendFunc(char c);
	void FlushSecond(void);
};

HeaderFile::HeaderFile()
{
	fp=NULL;
	tableUsed=0;
	table[0]=0;
	secUsed=0;
	second[0]=0;
}

int HeaderFile::open(char fn[])
{
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		return 0;
	}
	return 1;
}

int HeaderFile::close(void)
{
	if(fp!=NULL)
	{
		fclose(fp);
		fp=NULL;
	}
	return 0;
}

void HeaderFile::FlushSecond(void)
{
	int i;
	for(i=0; i<secUsed; i++)
	{
		func[funcUsed++]=second[i];
	}
	func[funcUsed]=0;
	secUsed=0;
}

void HeaderFile::AppendFunc(char c)
{
	int i;

	if(funcUsed==0 && (c==' ' || c=='\t'))
	{
		return;
	}

	switch(c)
	{
	case ' ':
	case '\t':
		if(secUsed<=0 ||
		   (second[secUsed-1]!=' ' &&
		    second[secUsed]!='(' &&
		    second[secUsed]!=')'))
		{
			second[secUsed  ]=' ';
			second[secUsed+1]=0;
			secUsed++;
		}
		break;
	case ')':
	case '(':
	case ',':
		while(secUsed>0 && second[secUsed-1]==' ')
		{
			secUsed--;
		}
		FlushSecond();
		func[funcUsed++]=c;
		func[funcUsed  ]=0;
		break;
	default:
		FlushSecond();
		func[funcUsed++]=c;
		func[funcUsed  ]=0;
		secUsed=0;
		break;
	}
}

char *HeaderFile::GetOneFunction(char funcBuf[])
{
	funcUsed=0;
	func=funcBuf;
	func[0]=0;

	while(1)
	{
		if(table[tableUsed]==0)
		{
			if(fp!=NULL)
			{
				if(fgets(table,2048,fp)==NULL)
				{
					return NULL;
				}
				if(table[strlen(table)-1]=='\n')
				{
					table[strlen(table)-1]==0;
				}
			}
			else
			{
				if(gets(table)==NULL)
				{
					return NULL;
				}
			}
			tableUsed=0;
		}
		else
		{
			while(table[tableUsed]!=0 && table[tableUsed]!=';')
			{
				AppendFunc(table[tableUsed]);
				tableUsed++;
			}
			if(table[tableUsed]==';')
			{
				tableUsed++;
				AppendFunc(';');
				return func;
			}
		}
	}
}

/***********************************************************************/
class FunctionTable {
public:
	FunctionTable();
	void Set(char str[]);
	void Print(void);
protected:
	void ExtractName(void);
	void ExtractParameters(void);

	char decl[256];
	char name[256];

	char buf[2048];

	int nParam;
	char *param[256];
};

FunctionTable::FunctionTable()
{
	decl[0]=0;
	name[0]=0;
	buf[0]=0;
	nParam=0;
	param[0]=NULL;
}

void FunctionTable::Set(char str[])
{
	decl[0]=0;
	name[0]=0;
	buf[0]=0;
	nParam=0;
	param[0]=NULL;

	strcpy(decl,str);
	ExtractName();
	ExtractParameters();
}

void FunctionTable::Print(void)
{
	printf("----------\n");
	printf("NAME:%s\n",name);
	printf("CATEGORY:ŠÖ”\n");
	printf("DEFINITION:\n");
	printf("  %s\n",decl);
	if(nParam>0)
	{
		printf("PARAMETER:\n");
		for(int i=0; i<nParam; i++)
		{
			char tab[256];
			sprintf(tab,"%s;",param[i]);
			printf("  %-12s\n",tab);
		}
	}
	if(strncmp(decl,"void",4)!=0)
	{
		printf("RETURN:\n");
	}
	printf("DESCRIPTION:\n");
	printf("REFERENCE:\n");
}

void FunctionTable::ExtractName(void)
{
	int i,spc;
	char table[256];

	strcpy(table,decl);

	spc=0;
	for(i=0; table[i]!=0; i++)
	{
		if(table[i]==' ' || table[i]=='\t')
		{
			spc=i+1;
		}
		else if(table[i]=='(')
		{
			table[i]=0;
			break;
		}
	}
	strcpy(name,table+spc);
}

void FunctionTable::ExtractParameters(void)
{
	int top,btm,lBuf;
	char table[256];

	strcpy(table,decl);
	for(top=0; table[top]!=0; top++)
	{
		if(table[top]=='(')
		{
			top++;
			break;
		}
	}
	for(btm=strlen(table)-1; btm>0; btm--)
	{
		if(table[btm]==')')
		{
			table[btm]=0;
			break;
		}
	}

	while(table[top]==' ' || table[top]=='\t')
	{
		top++;
	}
	while(btm>0 && (table[btm-1]==' ' || table[btm-1]=='\t'))
	{
		table[btm]=0;
		btm--;
	}

	strcpy(buf,table+top);

	if(strcmp(buf,"void")!=0)
	{
		param[0]=buf;
		nParam=1;
		lBuf=strlen(buf);
		for(int i=0; i<lBuf; i++)
		{
			if(buf[i]==',')
			{
				buf[i]=0;
				param[nParam]=buf+i+1;
				nParam++;
			}
		}
		param[nParam]=NULL;
	}
}
/***********************************************************************/

int main(void)
{
	HeaderFile file;
	FunctionTable func;
	char str[2048];

	while(file.GetOneFunction(str)!=NULL)
	{
		func.Set(str);
		func.Print();
	}

	return 0;
}



