#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

#include "hotas.h"
#include "htfmt.h"

int	HtMbDlg = -1 ;
int HtMbMsg[3] = -1 ;
int HtMbYesBtn = -1 ;
int HtMbNoBtn = -1 ;
int HtMbYesMsg = -1 ;
int HtMbNoMsg = -1 ;
int HtMbBtn = -1 ;
int HtMbBtnMsg = -1 ;

static int result;
static void HtFmtSetMessages(char *msg[3]);
static void HtFmtSetYesNoMessages(char *yes,char *no);
static void HtFmtSetButtonMessage(char *msg);
static int HtFmtGenGuiItems(void);
static void HtFmtDstGuiItems(void);

int HtFmtYesNo(char *msg[3],char *yes,char *no)
{
	if(HtFmtGenGuiItems()>=0)
	{
		HtFmtSetMessages(msg);
		HtFmtSetYesNoMessages(yes,no);
		HtFmtExecAlertObj(HtMbDlg,MMI_GetBaseObj());
		HtFmtDstGuiItems();
		return result;
	}
	return HT_YES;
}

void HtFmtMessageBox(char *msg[3],char *btn)
{
	if(HtFmtGenGuiItems()>=0)
	{
		HtFmtSetMessages(msg);
		HtFmtSetButtonMessage(btn);
		HtFmtExecAlertObj(HtMbDlg,MMI_GetBaseObj());
		HtFmtDstGuiItems();
	}
}



/***** Internal Funcs *****/
static void HtFmtSetMessages(char *msg[3])
{
	int i;
	for(i=0; i<3; i++)
	{
		MMI_SendMessage(HtMbMsg[i],MM_SETMSG,1,(msg[i]!=NULL ? msg[i] : ""));
	}
}

static void HtFmtSetYesNoMessages(char *yes,char *no)
{
	MMI_SendMessage(HtMbYesMsg,MM_SETMSG,1,(yes!=NULL ? yes : "‚n‚j"));
	MMI_SendMessage(HtMbNoMsg ,MM_SETMSG,1,(no !=NULL ? no  : "CANCEL"));

	MMI_SendMessage(HtMbYesBtn,MM_ATTACH,1,HtMbDlg);
	MMI_SendMessage(HtMbYesMsg,MM_ATTACH,1,HtMbDlg);
	MMI_SendMessage(HtMbNoBtn ,MM_ATTACH,1,HtMbDlg);
	MMI_SendMessage(HtMbNoMsg ,MM_ATTACH,1,HtMbDlg);
	MMI_SendMessage(HtMbBtn,MM_DETACH,0);
	MMI_SendMessage(HtMbBtnMsg,MM_DETACH,0);
}

static void HtFmtSetButtonMessage(char *msg)
{
	MMI_SendMessage(HtMbBtnMsg,MM_SETMSG,1,(msg!=NULL ? msg : "ROGER"));

	MMI_SendMessage(HtMbYesBtn,MM_DETACH,0);
	MMI_SendMessage(HtMbYesMsg,MM_DETACH,0);
	MMI_SendMessage(HtMbNoBtn ,MM_DETACH,0);
	MMI_SendMessage(HtMbNoMsg ,MM_DETACH,0);
	MMI_SendMessage(HtMbBtn,MM_ATTACH,1,HtMbDlg);
	MMI_SendMessage(HtMbBtnMsg,MM_ATTACH,1,HtMbDlg);
}

static int HtFmtGenGuiItems(void)
{
	extern MMIINIT initDataHTMBGUS;
	if(MMI_Init(&initDataHTMBGUS)>=0)
	{
		HtFmtCentering(HtMbDlg);
		return NOERR;
	}
	return -1;
}

static void HtFmtDstGuiItems(void)
{
	MMI_SendMessage(HtMbDlg,MM_DETACH,0);
	MMI_SendMessage(HtMbDlg,MM_DESTROY,0);
}



/***** Call Backs *****/

int HtMbYesFunc(void)
{
	result=HT_YES;
	MMI_SetHaltFlag(TRUE);
	return NOERR;
}

int HtMbNoFunc(void)
{
	result=HT_NO;
	MMI_SetHaltFlag(TRUE);
	return NOERR;
}
