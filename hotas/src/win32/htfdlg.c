#include <windows.h>
#include <commdlg.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <direct.h>


#include <impulse.h>
#include "hotas.h"
#include "resource.h"


extern HWND BiWnd;




static void HtBarToZero(char *str)
{
	int i,ln;
	ln=strlen(str);
	for(i=0; i<ln; i++)
	{
		if(str[i]=='|')
		{
			str[i]=0;
		}
	}
}



static void HtUncapitalize(char *str)
{
	while(*str!=0)
	{
		if('A'<=*str && *str<='Z')
		{
			*str=*str+'a'-'A';
		}
		str++;
	}
}

/*pub*/int HtLoadFileDialog(char *fn,char *msg,char *df,char *ext)
{
	int result;
	OPENFILENAME para;
	char coffee[256],burger[256];
	char curpath[256];
	HINSTANCE module;

	module=GetModuleHandle(NULL);

	ext=(ext[0]=='*' ? ext+1 : ext);
	ext=(ext[0]=='.' ? ext+1 : ext);

	sprintf(burger,"(*.%s)|*.%s||",ext,ext);
	HtBarToZero(burger);

	para.lStructSize=sizeof(OPENFILENAME);
	para.hwndOwner=BiWnd /* NULL */;
	para.hInstance=module;
	para.lpstrFilter=burger;
	para.lpstrCustomFilter=NULL;
	para.nFilterIndex=0;
	if(df!=NULL)
	{
		strcpy(coffee,df);
	}
	else
	{
		strcpy(coffee,"*.");
		strcat(coffee,ext);
	}
	para.lpstrFile=coffee;
	para.nMaxFile=sizeof(coffee);
	para.lpstrFileTitle=NULL;
	para.lpstrInitialDir=NULL;
	para.lpstrTitle=(msg!=NULL ? msg : "Load");
	para.Flags=OFN_HIDEREADONLY/* |OFN_OVERWRITEPROMPT */;
	para.lpstrDefExt=ext;

	_getcwd(curpath,256);
	result=GetOpenFileName(&para);
	_chdir(curpath);

	if(result!=0)
	{
		strcpy(fn,coffee);
		HtUncapitalize(fn);
		return HT_YES;
	}
	else
	{
		fn[0]=0;
		return HT_NO;
	}
}

/*pub*/int HtSaveFileDialog(char *fn,char *msg,char *df,char *ext)
{
	OPENFILENAME para;
	char coffee[256],burger[256];
	char curpath[256];
	int result;
	HINSTANCE module;

	module=GetModuleHandle(NULL);

	ext=(ext[0]=='*' ? ext+1 : ext);
	ext=(ext[0]=='.' ? ext+1 : ext);

	sprintf(burger,"(*.%s)|*.%s||",ext,ext);
	HtBarToZero(burger);

	para.lStructSize=sizeof(OPENFILENAME);
	para.hwndOwner=BiWnd /* NULL */;
	para.hInstance=module;
	para.lpstrFilter=burger;
	para.lpstrCustomFilter=NULL;
	para.nFilterIndex=0;
	if(df!=NULL)
	{
		strcpy(coffee,df);
	}
	else
	{
		strcpy(coffee,"*.");
		strcat(coffee,ext);
	}
	para.lpstrFile=coffee;
	para.nMaxFile=sizeof(coffee);
	para.lpstrFileTitle=NULL;
	para.lpstrInitialDir=NULL;
	para.lpstrTitle=(msg!=NULL ? msg : "Save");
	para.Flags=OFN_HIDEREADONLY/* |OFN_OVERWRITEPROMPT */;
	para.lpstrDefExt=ext;

	_getcwd(curpath,256);
	result=GetSaveFileName(&para);
	_chdir(curpath);

	if(result!=0)
	{
		strcpy(fn,coffee);
		HtUncapitalize(fn);
		return HT_YES;
	}
	else
	{
		fn[0]=0;
		return HT_NO;
	}
}
