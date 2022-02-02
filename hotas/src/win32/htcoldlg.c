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

static char *caption;


static UINT APIENTRY CCHookProc(HWND hdlg,UINT msg,WPARAM wp,LPARAM lp)
{
	if(msg==WM_INITDIALOG && caption!=NULL)
	{
		SetWindowText(hdlg,caption);
	}
	return 0;
}

/*pub*/int HtColorDialog(BICOLOR *col,BICOLOR *df,char *msg)
{
	int i;
	CHOOSECOLOR cc;
	COLORREF clr,aclrCust[16];

	for(i=0; i<16; i++)
	{
		aclrCust[i]=RGB(255,255,255);
	}

	clr=RGB(0,0,0);
	if(df!=NULL)
	{
		clr=RGB(df->r,df->g,df->b);
		aclrCust[0]=clr;
	}
	memset(&cc,0,sizeof(CHOOSECOLOR));

	cc.lStructSize=sizeof(CHOOSECOLOR);
	cc.hwndOwner=BiWnd;
	cc.rgbResult=clr;
	cc.lpCustColors=aclrCust;
	cc.Flags=CC_FULLOPEN|CC_ENABLEHOOK|CC_RGBINIT;
	cc.lpfnHook=CCHookProc;

	caption=msg;

	if(ChooseColor(&cc)!=0)
	{
		col->r=GetRValue(cc.rgbResult);
		col->g=GetGValue(cc.rgbResult);
		col->b=GetBValue(cc.rgbResult);
		return HT_YES;
	}
	else
	{
		return HT_NO;
	}
}

