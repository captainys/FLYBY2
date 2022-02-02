#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "hotas.h"
#include "resource.h"



extern HWND BiWnd;

/***********************************************************************/
static char *coffee,*tea,*coke;
static int burger;


static void HtOnInitLInput(HWND dlg)
{
	HWND edt;

	SetDlgItemText(dlg,HT_LINPUT_MSG,tea);
	SetDlgItemText(dlg,HT_LINPUT_EDIT,coke);

	edt=GetDlgItem(dlg,HT_LINPUT_EDIT);
	SetFocus(edt);
	SendMessage(edt,EM_SETSEL,0,-1);

	burger=HT_NO;
}

static void HtOnCreateLInput(HWND dlg)
{
}

BOOL CALLBACK HtLInputProc(HWND dlg,UINT msg,WPARAM wPara,LPARAM lPara)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wPara))
		{
		case IDOK:
			GetDlgItemText(dlg,HT_LINPUT_EDIT,coffee,255);
			burger=HT_YES;
			EndDialog(dlg,0);
			break;
		case IDCANCEL:
			burger=HT_NO;
			coffee[0]=0;
			EndDialog(dlg,0);
			break;
		}
		return FALSE;
	case WM_CREATE:
		HtOnCreateLInput(dlg);
		break;
	case WM_INITDIALOG:
		HtOnInitLInput(dlg);
		return 0;
	default:
		return FALSE;
	}
}

static int HtExecLineInput(char *str,char *msg,char *def)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	coffee=str;
	tea=(msg!=NULL ? msg : "");
	coke=(def!=NULL ? def : "");
	DialogBox(module,"HT_LINEINPUT",BiWnd,(DLGPROC)HtLInputProc);
	return burger;
}



/***********************************************************************/
/*pub*/int HtLineInput(char *str,char *msg,char *def)
{
	return HtExecLineInput(str,msg,def);
}
