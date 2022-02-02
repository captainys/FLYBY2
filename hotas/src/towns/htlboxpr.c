#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

#include "hotas.h"
#include "htfmt.h"




static void ExecListBox(int basobj,char *val[],char *mes,char *yes,char *no);
static void SetPanelMessage(char *mes,char *yes,char *no);

int HtLBoxId=-1;
int HtLBoxMesId=-1;
int HtLBoxMenuId=-1;
int HtLBoxScrId=-1;
int HtLBoxYesBtnId=-1;
int HtLBoxYesMesId=-1;
int HtLBoxNoBtnId=-1;
int HtLBoxNoMesId=-1;


extern MMIINIT initDataHTLBOXGS;


static int result;

int HtFmtListBoxSingle
        (int *ret,char *val[],char *mes,char *yes,char *no)
{
	int i;
	int row,clm;
	LISTITEML40 *itm;

	if(HtLBoxId<0)
	{
		if(MMI_Init(&initDataHTLBOXGS)<0)
		{
			return HT_NO;
		}
	}

	MTL_resetAtrObj(HtLBoxMenuId,~MS_MSELECTL40);
	ExecListBox(MMI_GetBaseObj(),val,mes,yes,no);

	MMI_SendMessage(HtLBoxMenuId,MM_GETLMENUPTR,3,&itm,&row,&clm);
	*ret=0;
	for(i=0; i<row; i++)
	{
		if(itm[i].selectFlag==TRUE)
		{
			*ret=i;
			break;
		}
	}

	if(i==row)
	{
		result=HT_NO;  /* 何も選ばなかった */
	}

	MMI_SendMessage(HtLBoxMenuId,MM_SETMAXROWCOLUM,2,0,0);

	return result;
}

int HtFmtListBoxPl
        (int *nRet,int ret[],char *val[],char *mes,char *yes,char *no)
{
	int i;
	int row,clm;
	LISTITEML40 *itm;

	if(HtLBoxId<0)
	{
		if(MMI_Init(&initDataHTLBOXGS)<0)
		{
			return HT_NO;
		}
	}

	MTL_setAtrObj(HtLBoxMenuId,MS_MSELECTL40);
	ExecListBox(MMI_GetBaseObj(),val,mes,yes,no);

	MMI_SendMessage(HtLBoxMenuId,MM_GETLMENUPTR,3,&itm,&row,&clm);
	*nRet=0;
	for(i=0; i<row; i++)
	{
		if(itm[i].selectFlag==TRUE)
		{
			ret[*nRet]=i;
			(*nRet)++;
		}
	}

	MMI_SendMessage(HtLBoxMenuId,MM_SETMAXROWCOLUM,2,0,0);

	return result;
}


static void ExecListBox(int basobj,char *val[],char *mes,char *yes,char *no)
{
	int i,n,m,wid,hei;
	HYPER hyp;
	int prt,min,max,len,pag;

	SetPanelMessage(mes,yes,no);
	n=ListBoxValCount(val);

	MMI_SendMessage(HtLBoxMenuId,MM_SETMAXROWCOLUM,2,n,1);

	MMI_SendMessage(HtLBoxMenuId,MM_GETHYPER,1,&hyp);
	wid=  hyp.fr.rdwx-hyp.fr.lupx;
	hei=  hyp.fr.rdwy-hyp.fr.lupy;
	MMI_SendMessage(HtLBoxMenuId,MM_SETCOLUMLEN,2,0,wid/6);

	m=hei/12-1;      /* n:選択項目数  m:表示数 */
	prt = m-1;
	min = m-1;
	max = ((n<m) ? (m-1) : (n-1));
	len = m;
	pag = 1;
	MMI_SendMessage(HtLBoxScrId,MM_SETSCROLL,5,prt,min,max,len,pag);
	MMI_SendMessage(HtLBoxMenuId,MM_SETDSPROWCOLUM,2,0,0);

	for(i=0; i<n; i++)
	{
		MMI_SendMessage(HtLBoxMenuId,MM_SETROWCOLUM,3,i,0,val[i]);
	}

	HtFmtExecAlertObj(HtLBoxId,basobj);
}


static int ListBoxValCount(char *val[])
{
	int i;
	for(i=0; val[i]!=NULL; i++);
	return i;
}

static void SetPanelMessage(char *mes,char *yes,char *no)
{
	mes=(mes!=NULL ? mes : "項目を選んでください");
	MMI_SendMessage(HtLBoxMesId,MM_SETMSG,1,mes);

	yes=(yes!=NULL ? yes : "確定");
	MMI_SendMessage(HtLBoxYesMesId,MM_SETMSG,1,yes);

	no=(no!=NULL ? no : "取消");
	MMI_SendMessage(HtLBoxNoMesId,MM_SETMSG,1,no);
}



static void ResetListMenu(int sel);
static int GetSelection(int *sel,LISTITEML40 *itm,int row,int clm);

int HtLBoxUpFunc()
{
	int sel;
	int row,clm;
	LISTITEML40 *itm;

	MMI_SendMessage(HtLBoxMenuId,MM_GETLMENUPTR,3,&itm,&row,&clm);
	if(GetSelection(&sel,itm,row,clm)==HT_YES)
	{
		if(sel>0)
		{
			itm[sel-1].selectFlag=FALSE;
		}

		sel--;
		if(sel<0)
		{
			sel=row;
		}

		if(sel>0)
		{
			itm[sel-1].selectFlag=TRUE;
		}
		ResetListMenu(sel);
	}

	return NOERR;
}

int HtLBoxDownFunc()
{
	int sel;
	int row,clm;
	LISTITEML40 *itm;

	MMI_SendMessage(HtLBoxMenuId,MM_GETLMENUPTR,3,&itm,&row,&clm);
	if(GetSelection(&sel,itm,row,clm)==HT_YES)
	{
		if(sel>0)
		{
			itm[sel-1].selectFlag=FALSE;
		}

		sel++;
		if(sel>row)
		{
			sel=0;
		}

		if(sel>0)
		{
			itm[sel-1].selectFlag=TRUE;
		}
		ResetListMenu(sel);
	}

	return NOERR;
}

static int GetSelection(int *sel,LISTITEML40 *itm,int row,int clm)
{
	int nSel,i;

	*sel=0;
	nSel=0;
	for(i=0; i<row; i++)
	{
		if(itm[i].selectFlag==TRUE)
		{
			*sel=i+1;
			nSel++;
			if(nSel>1)
			{
				return HT_NO;
			}
		}
	}
	return HT_YES;
}

static void ResetListMenu(int sel)
{
	int row,clm;
	LISTITEML40 *itm;
	int prt,min,max,len,pag;
	int top,btm;

	MMI_SendMessage(HtLBoxMenuId,MM_GETLMENUPTR,3,&itm,&row,&clm);
	MMI_SendMessage(HtLBoxScrId,MM_GETSCROLL,5,&prt,&min,&max,&len,&pag);

	top=prt-min;
	btm=top+min; /* min = 表示数-1 の利用 */
	sel--;
	if(sel<0)
	{
		sel=0;
	}

	if(sel<top)
	{
		/* top=sel -> prt-min=sel -> prt=sel+min */
		prt=sel+min;
	}
	else if(sel>btm)
	{
		/* btm=sel -> prt-min+min=sel -> prt=sel */
		prt=sel;
	}

	MMI_SendMessage(HtLBoxScrId,MM_SETSCROLL,5,prt,min,max,len,pag);
	MMI_SendMessage(HtLBoxMenuId,MM_SETDSPROWCOLUM,2,prt-min,0);
	HtFmtRepaintWindow(HtLBoxScrId);
	HtFmtRepaintWindow(HtLBoxMenuId);
}



int HtLBoxScrolLFunc(void)
{
	int prt,min,max,len,pag;

	MMI_SendMessage(HtLBoxScrId,MM_GETSCROLL,5,&prt,&min,&max,&len,&pag);
	MMI_SendMessage(HtLBoxMenuId,MM_SETDSPROWCOLUM,2,prt-min,0);
	MMI_SendMessage(HtLBoxMenuId,MM_SHOW,0);

	return NOERR ;
}

int HtLBoxExitFunc(int kobj)
{
	if(kobj==HtLBoxYesBtnId)
	{
		result=HT_YES;
	}
	else
	{
		result=HT_NO;
	}

	MMI_SetHaltFlag(TRUE);

	return NOERR ;
}

int HtLBoxMenuFunc(int kobj,int messId,int argc,EVENT *pev,int trigger)
{
	HYPER hyp;

	if((pev->what & EVMOSUP)==0)
	{
		while(1)
		{
			MMI_iosense();
			if(MMI_SnsEvnt(EVMOSUP,&pev)==0)
			{
				if((pev->what & EVMOSUP)!=0)
				{
					break;
				}
			}
		}
	}

	MMI_SendMessage(kobj,MM_GETHYPER,1,&hyp);
	if(MMI_DoubleClickCheck(&hyp.fr,pev)==TRUE)
	{
		result=HT_YES;
		MMI_SetHaltFlag(TRUE);
	}

	return NOERR ;
}
