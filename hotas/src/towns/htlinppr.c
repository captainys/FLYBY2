#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

#include "hotas.h"
#include "htfmt.h"

static int result;
static char tmp[256];
int HtLinpDlg = -1 ;
int HtLinpTxt = -1 ;
int HtLinpMsg = -1 ;
int HtLinpHiddenButton = -1 ;
int HtLinpYesBtn = -1 ;
int HtLinpNoBtn = -1 ;

static int HtFmtGenGuiItems(void);
static void HtFmtDstGuiItems(void);
static void HtLinpSetDefault(char *def);

int HtFmtLineInput(char *str,char *msg,char *def)
{
	if(HtFmtGenGuiItems()>=0)
	{
		HtLinpSetDefault(def);
		HtFmtExecAlertObj(HtLinpDlg,MMI_GetBaseObj());
		HtFmtDstGuiItems();
		strcpy(str,(result==HT_YES ? tmp : ""));
		return result;
	}
	return HT_NO;
}


/***** Internal Funcs *****/
static int HtFmtGenGuiItems(void)
{
	extern MMIINIT initDataHTLINPGS;
	if(MMI_Init(&initDataHTLINPGS)>=0)
	{
		HtFmtCentering(HtLinpDlg);
		return NOERR;
	}
	return -1;
}

static void HtFmtDstGuiItems(void)
{
	MMI_SendMessage(HtLinpDlg,MM_DETACH,0);
	MMI_SendMessage(HtLinpDlg,MM_DESTROY,0);
}

static void HtLinpSetDefault(char *def)
{
	strcpy(tmp,(def!=NULL ? def : ""));
	MMI_SendMessage(HtLinpTxt,MM_SETTEXT,3,tmp,strlen(tmp),FALSE);
	MMI_SendMessage(HtLinpTxt,MM_WAKE,0);
}

/***** GUI-LIB Call Backs *****/
int HtLinpTxtFunc(int kobj)
{
	MMI_SendMessage(kobj,MM_GETTEXT,3,tmp,sizeof(tmp),FALSE);
	return NOERR;
}

int HtLinpYesFunc(void)
{
	result=HT_YES;
	MMI_SetHaltFlag(TRUE);
	return NOERR;
}

int HtLinpNoFunc(void)
{
	result=HT_NO;
	MMI_SetHaltFlag(TRUE);
	return NOERR;
}
