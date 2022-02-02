#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "skyhawk.h"


static int SkFuncSw=SK_ERR;
static int (*SkPrintFunc)(char *);

static char *SkPrintfConvert(char *tmp,char *org);
static va_list ap;

void SkSetPrintFunc(int (*f)(char *))
{
	SkFuncSw=SK_OK;
	SkPrintFunc=f;
}

void SkResetPrintFunc(void)
{
	SkFuncSw=SK_ERR;
}

int SkPrintf(char *fmt,...)
{
	int nTab;
	char tab[512],tmp[512];

	if(SkFuncSw!=SK_OK)
	{
		return 0;
	}

	nTab=0;
	tab[0]=0;
	va_start(ap,fmt);
	while((*fmt)!=0)
	{
		if((*fmt)!='%')
		{
			tab[nTab]=*fmt;
			nTab++;
			fmt++;
			tab[nTab]=0;
		}
		else
		{
			tab[nTab]=0;
			fmt=SkPrintfConvert(tmp,fmt);
			strcat(tab,tmp);
			nTab=strlen(tab);
		}
	}
	va_end(ap);
	(*SkPrintFunc)(tab);
	return strlen(tab);
}

enum {
	SZ_SHORT,
	SZ_NORM,
	SZ_LONG
};

static char *SkPrintfConvert(char *tmp,char *org)
{
	char fmt[256];
	int i,lng;
	unsigned int iv;
	unsigned short siv;
	unsigned long liv;
	char *sv;
	float fv;
	double dv;
	void *vv;
	int *ipv;

	lng=SZ_NORM;

	fmt[0]='%';
	for(i=1; org[i]!=0; i++)
	{
		fmt[i  ]=org[i];
		fmt[i+1]=0;
		switch(org[i])
		{
		case 'l': case 'L':
			lng=SZ_LONG;
			break;
		case 'h':
			lng=SZ_SHORT;
			break;
		case 'd':
		case 'i':
		case 'o':
		case 'x':
		case 'X':
		case 'u':
		case 'c':
			switch(lng)
			{
			case SZ_NORM:
				iv=va_arg(ap,unsigned int);
				sprintf(tmp,fmt,iv);
				break;
			case SZ_SHORT:
				siv=va_arg(ap,unsigned short);
				sprintf(tmp,fmt,siv);
				break;
			case SZ_LONG:
				liv=va_arg(ap,unsigned long);
				sprintf(tmp,fmt,liv);
				break;
			}
			return &org[i+1];
		case 's':
			sv=va_arg(ap,char *);
			sprintf(tmp,fmt,sv);
			return &org[i+1];
		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			switch(lng)
			{
			case SZ_NORM:
			case SZ_SHORT:
				fv=(float)va_arg(ap,double); /* double ? Why isn't it float ? */
				sprintf(tmp,fmt,fv);
				break;
			case SZ_LONG:
				dv=va_arg(ap,double);
				sprintf(tmp,fmt,dv);
				break;
			}
			return &org[i+1];
		case 'p':
			vv=va_arg(ap,void *);
			return &org[i+1];
		case 'n':
			ipv=va_arg(ap,int *);
			return &org[i+1];
		case '%':
			tmp[0]='%';
			tmp[1]=0;
			return &org[i+1];
		}
	}
	tmp[0]=0;
	return &org[i];
}
