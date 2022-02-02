#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <impulse.h>
#include "hotas.h"
#include "resource.h"


extern HWND BiWnd;



enum {
	HTNUM_INTEGER,
	HTNUM_REAL,
	HTNUM_ANGLE
};


/***********************************************************************/
static int InputMode,NNum;
static real FNum[3];
static int INum[3];
static char *tMsg,*yBtn,*nBtn;
static int burger;

static void HtSetMessage(HWND dlg)
{
	switch(InputMode)
	{
	case HTNUM_INTEGER:
	case HTNUM_REAL:
		SetDlgItemText(dlg,HT_NUMBER_MSG1,"X:");
		SetDlgItemText(dlg,HT_NUMBER_MSG2,"Y:");
		SetDlgItemText(dlg,HT_NUMBER_MSG3,"Z:");
		break;
	case HTNUM_ANGLE:
		SetDlgItemText(dlg,HT_NUMBER_MSG1,"H:");
		SetDlgItemText(dlg,HT_NUMBER_MSG2,"P:");
		SetDlgItemText(dlg,HT_NUMBER_MSG3,"B:");
		break;
	}
	SetDlgItemText(dlg,HT_NUMBER_BTN1,(yBtn!=NULL ? yBtn : "OK"));
	SetDlgItemText(dlg,HT_NUMBER_BTN2,(nBtn!=NULL ? nBtn : "Cancel"));
	if(tMsg!=NULL)
	{
		SetWindowText(dlg,tMsg);
	}
}

static void HtSetNumber(HWND dlg)
{
	int i;
	int id[]={HT_NUMBER_EDIT1,HT_NUMBER_EDIT2,HT_NUMBER_EDIT3};
	char tmp[128];

	for(i=0; i<NNum; i++)
	{
		switch(InputMode)
		{
		case HTNUM_INTEGER:
			sprintf(tmp,"%d",INum[i]);
			break;
		case HTNUM_REAL:
			sprintf(tmp,"%.2f",FNum[i]);
			break;
		case HTNUM_ANGLE:
			sprintf(tmp,"%.2f",FNum[i]);
			break;
		}

		SetDlgItemText(dlg,id[i],tmp);
	}
}

static void HtGetNumber(HWND dlg)
{
	int i;
	int id[]={HT_NUMBER_EDIT1,HT_NUMBER_EDIT2,HT_NUMBER_EDIT3};
	char tmp[128];

	for(i=0; i<NNum; i++)
	{
		GetDlgItemText(dlg,id[i],tmp,128);

		switch(InputMode)
		{
		case HTNUM_INTEGER:
			INum[i]=atoi(tmp);
			break;
		case HTNUM_REAL:
			FNum[i]=(real)atof(tmp);
			break;
		case HTNUM_ANGLE:
			FNum[i]=(real)atof(tmp);
			break;
		}
	}
}

static void HtOnInitNumber(HWND dlg)
{
	HtSetMessage(dlg);
	HtSetNumber(dlg);
}

BOOL CALLBACK HtInputNumberProc(HWND dlg,UINT msg,WPARAM wPara,LPARAM lPara)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wPara))
		{
		case HT_NUMBER_BTN1:
			HtGetNumber(dlg);
			burger=HT_YES;
			EndDialog(dlg,0);
			break;
		case HT_NUMBER_BTN2:
			burger=HT_NO;
			EndDialog(dlg,0);
			break;
		}
		return FALSE;
	case WM_INITDIALOG:
		HtOnInitNumber(dlg);
		return TRUE;
	default:
		return FALSE;
	}
}

/***********************************************************************/
/*pub*/int HtInputNumber(real *n,real dflt,char *msg,char *yes,char *no)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_REAL;
	NNum=1;
	FNum[0]=dflt;
	tMsg=msg;
	yBtn=yes;
	nBtn=no;
	DialogBox(module,"HT_NUMBER",BiWnd,HtInputNumberProc);
	*n=FNum[0];
	return burger;
}

/*pub*/int HtInputInteger(int *n,int dflt,char *msg,char *yes,char *no)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_INTEGER;
	NNum=1;
	INum[0]=dflt;
	tMsg=msg;
	yBtn=yes;
	nBtn=no;
	DialogBox(module,"HT_NUMBER",BiWnd,HtInputNumberProc);
	*n=INum[0];
	return burger;
}

/*pub*/int HtInputPoint2(BIPOINT2 *p,BIPOINT2 *df,char *msg,char *y,char *n)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_REAL;
	NNum=2;
	FNum[0]=(df!=NULL ? df->x : 0);
	FNum[1]=(df!=NULL ? df->y : 0);
	tMsg=msg;
	yBtn=y;
	nBtn=n;
	DialogBox(module,"HT_NUMBER2",BiWnd,HtInputNumberProc);
	p->x=FNum[0];
	p->y=FNum[1];
	return burger;
}

/*pub*/int HtInputPoint2i(int p[2],int df[2],char *msg,char *y,char *n)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_INTEGER;
	NNum=2;
	INum[0]=(df!=NULL ? df[0] : 0);
	INum[1]=(df!=NULL ? df[1] : 0);
	tMsg=msg;
	yBtn=y;
	nBtn=n;
	DialogBox(module,"HT_NUMBER2",BiWnd,HtInputNumberProc);
	p[0]=INum[0];
	p[1]=INum[1];
	return burger;
}

/*pub*/int HtInputPoint3(BIPOINT *p,BIPOINT *df,char *msg,char *y,char *n)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_REAL;
	NNum=3;
	FNum[0]=(df!=NULL ? df->x : 0);
	FNum[1]=(df!=NULL ? df->y : 0);
	FNum[2]=(df!=NULL ? df->z : 0);
	tMsg=msg;
	yBtn=y;
	nBtn=n;
	DialogBox(module,"HT_NUMBER3",BiWnd,HtInputNumberProc);
	p->x=FNum[0];
	p->y=FNum[1];
	p->z=FNum[2];
	return burger;
}

/*pub*/int HtInputPoint3i(int p[3],int df[3],char *msg,char *y,char *n)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_INTEGER;
	NNum=3;
	INum[0]=(df!=NULL ? df[0] : 0);
	INum[1]=(df!=NULL ? df[1] : 0);
	INum[2]=(df!=NULL ? df[2] : 0);
	tMsg=msg;
	yBtn=y;
	nBtn=n;
	DialogBox(module,"HT_NUMBER3",BiWnd,HtInputNumberProc);
	p[0]=INum[0];
	p[1]=INum[1];
	p[2]=INum[2];
	return burger;
}

/*pub*/int HtInputAngle3(BIANGLE *a,BIANGLE *df,char *msg,char *y,char *n)
{
	HINSTANCE module;
	module=GetModuleHandle(NULL);

	InputMode=HTNUM_ANGLE;
	NNum=3;
	FNum[0]=(float)(df!=NULL ? (float)df->h*90.0F/16384.0F : 0);
	FNum[1]=(float)(df!=NULL ? (float)df->p*90.0F/16384.0F : 0);
	FNum[2]=(float)(df!=NULL ? (float)df->b*90.0F/16384.0F : 0);
	tMsg=msg;
	yBtn=y;
	nBtn=n;
	DialogBox(module,"HT_NUMBER3",BiWnd,HtInputNumberProc);
	a->h=(long)(FNum[0]*16384.0F/90.0F);
	a->p=(long)(FNum[1]*16384.0F/90.0F);
	a->b=(long)(FNum[2]*16384.0F/90.0F);
	return burger;
}
