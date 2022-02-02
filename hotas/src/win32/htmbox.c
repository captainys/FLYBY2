#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "hotas.h"
#include "resource.h"



extern HWND BiWnd;

/***********************************************************************/
static char *(*coffee),*tea;

static void HtOnInitMbox(HWND dlg)
{
	/* SetDlgItem???? Sets Parameters to the Dialog Item */
	SetDlgItemText(dlg,HT_MBOX_MSG1,coffee[0]);
	SetDlgItemText(dlg,HT_MBOX_MSG2,coffee[1]);
	SetDlgItemText(dlg,HT_MBOX_MSG3,coffee[2]);

	SetDlgItemText(dlg,HT_MBOX_BTN,tea);
}

BOOL CALLBACK HtMboxProc(HWND dlg,UINT msg,WPARAM wPara,LPARAM lPara)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wPara))
		{
		case HT_MBOX_BTN:
			EndDialog(dlg,0);
			break;
		}
		return FALSE;
	case WM_INITDIALOG:
		HtOnInitMbox(dlg);
		return TRUE;
	default:
		return FALSE;
	}
}

static void HtExecMbox(char *str[3],char *btn)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	coffee=str;
	tea=btn;
	DialogBox(module,"HT_MBOX",BiWnd,(DLGPROC)HtMboxProc);
}



/***********************************************************************/




/*pub*/void HtMessageBox(char *msg,char *btn)
{
	char *str[3];
	str[0]="";
	str[1]=msg;
	str[2]="";
	HtExecMbox(str,(btn!=NULL ? btn : "OK"));
}


/*pub*/void HtMessageBox3(char *msg[3],char *btn)
{
	char *str[3];
	int i;
	for(i=0; i<3; i++)
	{
		str[i]=(msg[i]!=NULL ? msg[i] : "");
	}
	HtExecMbox(str,(btn!=NULL ? btn : "OK"));
}
