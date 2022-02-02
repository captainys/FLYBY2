#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

#include "hotas.h"
#include "htfmt.h"

static int result;
int HtNbDlg = -1 ;
int HtNbMsg[3] = -1 ;
int HtNbNb[3] = -1 ;
int HtNbYesBtn = -1 ;
int HtNbNoBtn = -1 ;
int HtNbYesMsg = -1 ;
int HtNbNoMsg = -1 ;
int HtNbTitle = -1 ;


#define COLUMN     2
#define DIVISION 100  /* DIVISION should be 10^COLUMN */

#define NUMBER_MINIMUM -0x80000000
#define NUMBER_MAXIMUM +0x7fffffff

static int min=NUMBER_MINIMUM;
static int max=NUMBER_MAXIMUM;

static void HtFmtSetHeadTitle(char *tit);
static void HtFmtSetNumberTitle(real n[3],char *tit[3]);
static void HtFmtSetIntegerTitle(int n[3],char *tit[3]);
static void HtFmtSetYesNoTitle(char *yes,char *no);
static void HtFmtGetNumber(real n[3],char *tit[3]);
static void HtFmtGetInteger(int n[3],char *tit[3]);
static int HtFmtGenGuiParts(void);
static void HtFmtDstGuiParts(void);


int HtFmtInputNumbers(real n[3],char *tit[3],char *msg,char *yes,char *no)
{
	if(HtFmtGenGuiParts()>=0)
	{
		HtFmtSetHeadTitle(msg);
		HtFmtSetNumberTitle(n,tit);
		HtFmtSetYesNoTitle(yes,no);
		HtFmtExecAlertObj(HtNbDlg,MMI_GetBaseObj());
		if(result==HT_YES)
		{
			HtFmtGetNumber(n,tit);
		}
		HtFmtDstGuiParts();
		return result;
	}
	return HT_NO;
}

int HtFmtInputInteger(int n[3],char *tit[3],char *msg,char *yes,char *no)
{
	if(HtFmtGenGuiParts()>=0)
	{
		HtFmtSetHeadTitle(msg);
		HtFmtSetIntegerTitle(n,tit);
		HtFmtSetYesNoTitle(yes,no);
		HtFmtExecAlertObj(HtNbDlg,MMI_GetBaseObj());
		if(result==HT_YES)
		{
			HtFmtGetInteger(n,tit);
		}
		HtFmtDstGuiParts();
		return result;
	}
	return HT_NO;
}




/***** Internal Functions *****/
static void HtFmtSetHeadTitle(char *tit)
{
	MMI_SendMessage(HtNbTitle,MM_SETMSG,1,(tit!=NULL ? tit : ""));
}

static void HtFmtSetNumberTitle(real n[3],char *tit[3])
{
	int i,val;
	for(i=0; i<3; i++)
	{
		if(tit[i]!=NULL)
		{
			val=(int)n[i] * DIVISION;
			MMI_SendMessage(HtNbMsg[i],MM_ATTACH,1,HtNbDlg);
			MMI_SendMessage(HtNbMsg[i],MM_SETMSG,1,tit[i]);
			MMI_SendMessage(HtNbNb[i],MM_ATTACH,1,HtNbDlg);
			MMI_SendMessage
				(HtNbNb[i],MM_SETNUMBOX,5,val,min,max,DIVISION,COLUMN);
		}
		else
		{
			MMI_SendMessage(HtNbNb[i],MM_DETACH,0);
			MMI_SendMessage(HtNbMsg[i],MM_DETACH,0);
		}
	}
}

static void HtFmtSetIntegerTitle(int n[3],char *tit[3])
{
	int i;
	for(i=0; i<3; i++)
	{
		if(tit[i]!=NULL)
		{
			MMI_SendMessage(HtNbMsg[i],MM_ATTACH,1,HtNbDlg);
			MMI_SendMessage(HtNbMsg[i],MM_SETMSG,1,tit[i]);
			MMI_SendMessage(HtNbNb[i],MM_ATTACH,1,HtNbDlg);
			MMI_SendMessage(HtNbNb[i],MM_SETNUMBOX,5,n[i],min,max,1,0);
		}
		else
		{
			MMI_SendMessage(HtNbNb[i],MM_DETACH,0);
			MMI_SendMessage(HtNbMsg[i],MM_DETACH,0);
		}
	}
}

static void HtFmtSetYesNoTitle(char *yes,char *no)
{
	MMI_SendMessage(HtNbYesMsg,MM_SETMSG,1,(yes!=NULL ? yes : "Šm’è"));
	MMI_SendMessage(HtNbNoMsg ,MM_SETMSG,1,(no!=NULL  ? no  : "ŽæÁ"));
}

static void HtFmtGetNumber(real n[3],char *tit[3])
{
	int i,j,var,min,max,del,col;
	for(i=0; i<3; i++)
	{
		if(tit[i]!=NULL)
		{
			MMI_SendMessage(HtNbNb[i],MM_GETNUMBOX,5,&var,&min,&max,&del,&col);
			n[i]=(real)var;
			for(j=0; j<col; j++)
			{
				n[i]/=10.0;
			}
		}
	}
}

static void HtFmtGetInteger(int n[3],char *tit[3])
{
	int i,var,min,max,del,col;
	for(i=0; i<3; i++)
	{
		if(tit[i]!=NULL)
		{
			MMI_SendMessage(HtNbNb[i],MM_GETNUMBOX,5,&var,&min,&max,&del,&col);
			n[i]=var;
		}
	}
}

static int HtFmtGenGuiParts(void)
{
	extern MMIINIT initDataHTNBGUS;
	if(MMI_Init(&initDataHTNBGUS)>=0)
	{
		HtFmtCentering(HtNbDlg);
		return NOERR;
	}
	return -1;
}

static void HtFmtDstGuiParts(void)
{
	MMI_SendMessage(HtNbDlg,MM_DETACH,0);
	MMI_SendMessage(HtNbDlg,MM_DESTROY,0);
}


/***** GUI-LIB Call Backs *****/

int HtNbYesFunc(void)
{
	result=HT_YES;
	MMI_SetHaltFlag(TRUE);
	return NOERR;
}

int HtNbNoFunc(void)
{
	result=HT_NO;
	MMI_SetHaltFlag(TRUE);
	return NOERR;
}
