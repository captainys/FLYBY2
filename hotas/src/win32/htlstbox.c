#include <windows.h>
#include <commdlg.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <impulse.h>
#include "hotas.h"
#include "resource.h"


extern HWND BiWnd;




/***********************************************************************/
enum {
	HTLB_SINGLE,
	HTLB_PLURAL
};

static char *caption;
static int lbMode;
static int nItem;
static char *(*lst),*yBtn,*nBtn;
static int nSel,*sel;
static int burger;

static void HtOnInitListBox(HWND dlg)
{
	int i;
	HWND lbx;

	SetDlgItemText(dlg,HT_LISTBOX_BTN1,yBtn);
	SetDlgItemText(dlg,HT_LISTBOX_BTN2,nBtn);

	lbx=GetDlgItem(dlg,HT_LISTBOX_LB);
	nItem=0;
	for(i=0; lst[i]!=NULL; i++)
	{
		SendMessage(lbx,LB_ADDSTRING,0,(LPARAM)(LPSTR)lst[i]);
		nItem++;
	}
	if(lst[0]!=NULL)
	{
		switch(lbMode)
		{
		case HTLB_SINGLE:
			SendMessage(lbx,LB_SETCURSEL,0,0);
			break;
		case HTLB_PLURAL:
			SendMessage(lbx,LB_SETSEL,TRUE,-1);
			break;
		}
	}
	SetFocus(lbx);
	if(caption!=NULL)
	{
		SetWindowText(dlg,caption);
	}
}

static int HtGetSelSingle(HWND dlg)
{
	int itm;
	itm=(int)SendDlgItemMessage(dlg,HT_LISTBOX_LB,LB_GETCURSEL,0,0L);
	if(itm!=LB_ERR)
	{
		nSel=itm;
		return HT_YES;
	}
	else
	{
		return HT_NO;
	}
}

static int HtGetSelPlural(HWND dlg)
{
	int i;
	nSel=0;
	for(i=0; i<nItem; i++)
	{
		if(SendDlgItemMessage(dlg,HT_LISTBOX_LB,LB_GETSEL,i,0L)>0)
		{
			sel[nSel]=i;
			nSel++;
		}
	}
	return HT_YES;
}

static int HtGetSelection(HWND dlg)
{
	switch(lbMode)
	{
	case HTLB_SINGLE:
		return HtGetSelSingle(dlg);
	case HTLB_PLURAL:
		return HtGetSelPlural(dlg);
	}
}


BOOL CALLBACK HtListBoxProc(HWND dlg,UINT msg,WPARAM wPara,LPARAM lPara)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wPara))
		{
		case HT_LISTBOX_LB:
			switch(HIWORD(lPara))
			{
			case LBN_DBLCLK:
				burger=HtGetSelection(dlg);
				EndDialog(dlg,0);
				break;
			}
			break;
		case HT_LISTBOX_BTN1:
			burger=HtGetSelection(dlg);
			EndDialog(dlg,0);
			break;
		case HT_LISTBOX_BTN2:
			burger=HT_NO;
			EndDialog(dlg,0);
			break;
		}
		return FALSE;
	case WM_INITDIALOG:
		HtOnInitListBox(dlg);
		return 0;
	default:
		return FALSE;
	}
}

static void HtExecListBox(void)
{
	HINSTANCE module;
	char *dlgName;

	module=GetModuleHandle(NULL);

	switch(lbMode)
	{
	case HTLB_SINGLE:
		dlgName="HT_LISTBOX";
		break;
	case HTLB_PLURAL:
		dlgName="HT_LISTBOXP";
		break;
	}
	DialogBox(module,dlgName,BiWnd,HtListBoxProc);
}

/***********************************************************************/

/*pub*/int HtListBox(int *n,char *str[],char *msg,char *yes,char *no)
{
	lst=str;
	yBtn=(yes!=NULL ? yes : "OK");
	nBtn=(no!=NULL ? no : "Cancel");
	lbMode=HTLB_SINGLE;
	HtExecListBox();
	if(burger==HT_YES)
	{
		*n=nSel;
	}
	return burger;
}

/*pub*/int HtListBoxPl
	(int *nm,int n[],char *str[],char *msg,char *yes,char *no)
{
	lst=str;
	yBtn=(yes!=NULL ? yes : "OK");
	nBtn=(no!=NULL ? no : "Cancel");
	sel=n;
	lbMode=HTLB_PLURAL;
	caption=msg;
	HtExecListBox();
	if(burger==HT_YES)
	{
		*nm=nSel;
	}
	return burger;
}
