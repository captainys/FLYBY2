#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "hotas.h"
#include "resource.h"



extern HWND BiWnd;

/***********************************************************************/
static int burger;
static char *(*coffee),*yBtn,*nBtn;

static void HtOnInitYesNo(HWND dlg)
{
	SetDlgItemText(dlg,HT_YESNO_MSG1,coffee[0]);
	SetDlgItemText(dlg,HT_YESNO_MSG2,coffee[1]);
	SetDlgItemText(dlg,HT_YESNO_MSG3,coffee[2]);

	SetDlgItemText(dlg,HT_YESNO_BTN1,yBtn);
	SetDlgItemText(dlg,HT_YESNO_BTN2,nBtn);
}

BOOL CALLBACK HtYesNoProc(HWND dlg,UINT msg,WPARAM wPara,LPARAM lPara)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wPara))
		{
		case HT_YESNO_BTN1:
			burger=HT_YES;
			EndDialog(dlg,0);
			break;
		case HT_YESNO_BTN2:
			burger=HT_NO;
			EndDialog(dlg,0);
			break;
		}
		return FALSE;
	case WM_INITDIALOG:
		HtOnInitYesNo(dlg);
		return TRUE;
	default:
		return FALSE;
	}
}

static void HtExecYesNo(char *str[3],char *yb,char *nb)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	coffee=str;
	yBtn=yb;
	nBtn=nb;
	DialogBox(module,"HT_YESNO",BiWnd,(DLGPROC)HtYesNoProc);
}



/***********************************************************************/




/*pub*/int HtYesNo(char *msg,char *yes,char *no)
{
	char *str[3];
	str[0]="";
	str[1]=msg;
	str[2]="";
	yes=(yes!=NULL ? yes : "OK");
	no=(no!=NULL ? no : "Cancel");
	HtExecYesNo(str,yes,no);
	return burger;
}


/*pub*/int HtYesNo3(char *msg[3],char *yes,char *no)
{
	char *str[3];
	int i;
	for(i=0; i<3; i++)
	{
		str[i]=(msg[i]!=NULL ? msg[i] : "");
	}
	yes=(yes!=NULL ? yes : "OK");
	no=(no!=NULL ? no : "Cancel");
	HtExecYesNo(str,yes,no);
	return burger;
}
