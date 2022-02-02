#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <msdos.cf>

#include <egb.h>
#include <mos.h>
#include <fmcfrb.h>

#include <gui.h>
#include <guidbg.h>
#include <wgb.h>
#include <file_dlg.h>

#include <math.h>


#include "hotas.h"
#include "htfmt.h"
#include "impulse.h"

#ifdef __cplusplus
	extern "C" {
#endif

int HtFmtWindowFunc(int kobj,int messId,int argc,EVENT *pev);

extern void BiFmtInitialize(void);
extern void BiFmtOpenWindow(long reqX,long reqY);
extern void BiFmtTerminate(void);
extern void BiFmtSetExtWindowFunc(int (*f)(int,int,int,EVENT *));
extern int BiFmtKeyToBikey(int key);

int	HtDlg = -1;
int	HtPd = -1;
int	HtPdTw = -1;
int	HtPdTwLst = -1;
int	HtFmtMenuSrc = -1;
int	HtFmtMenuLstSrc = -1;
int	HtFmtMenuItemSrc = -1;

void HtInitialize(void)
{
	BiFmtInitialize();
}

void HtOpenWindow(long sizx,long sizy)
{
	BiFmtOpenWindow(sizx,sizy);
	BiFmtSetExtWindowFunc(HtFmtWindowFunc);
}

/**************************************************************************/
typedef struct {
	int pdId,mnId;
	char str[80];
} HTPULLDOWN;

static HTPULLDOWN *HtOptPd;

void HtStartSetMenu(char *tit[])
{
	int i,n;
	extern MMIINIT initDataHTFMTDLG;

	MMI_Init(&initDataHTFMTDLG);
	MMI_SetUpSDKMenu(HtPdTw,1);
	MMI_SetUpPrgMenu(HtPdTw,0);

	for(i=0; tit[i]!=NULL; i++);
	n=i;
	HtOptPd=(HTPULLDOWN *)BiMalloc(n*sizeof(HTPULLDOWN));
	if(HtOptPd!=NULL)
	{
		for(i=0; tit[i]!=NULL; i++)
		{
			HtOptPd[i].pdId=MMI_SendMessage(HtFmtMenuSrc,MM_NEW,0);
			HtOptPd[i].mnId=MMI_SendMessage(HtFmtMenuLstSrc,MM_NEW,0);
			if(HtOptPd[i].pdId>=0 && HtOptPd[i].mnId>=0)
			{
				strcpy(HtOptPd[i].str,tit[i]);
				MMI_SendMessage(HtOptPd[i].pdId,MM_ATTACH,1,HtPd);
				MMI_SendMessage(HtOptPd[i].pdId,MM_SETMSG,1,HtOptPd[i].str);
				MMI_SendMessage(HtOptPd[i].mnId,MM_ATTACH,1,HtOptPd[i].pdId);
			}
		}
	}
}

typedef struct HtMenuItemTag {
	struct HtMenuItemTag *next;
	int id;
	char str[80];
} HTMENUITEM;

static HTMENUITEM *HtOptMenuItem;

void HtSetMenuItem(int n,char *str,char key,void (*f)(void))
{
	int itm;
	HTMENUITEM *neo;

	itm=MMI_SendMessage(HtFmtMenuItemSrc,MM_NEW,0);
	neo=(HTMENUITEM *)BiMalloc(sizeof(HTMENUITEM));
	if(itm>=0 && neo!=NULL)
	{
		neo->next=HtOptMenuItem;
		HtOptMenuItem=neo;
		neo->id=itm;
		strcpy(neo->str,str);
		MMI_SendMessage(itm,MM_ATTACH,1,HtOptPd[n-1].mnId);
		MMI_SendMessage(itm,MM_SETMSG,1,neo->str);
		MMI_SendMessage(itm,MM_SETEXEC,1,f);
		MMI_SendMessage(itm,MM_SETKEY,1,key);
	}
}

void HtEndSetMenu(void)
{
	MMI_SendMessage(HtDlg,MM_SHOW,0);
}




/*************************************************************************/
typedef struct HtShortCutTag {
	struct HtShortCutTag *next;
	int bikey;
	void (*func)(void);
} HTSHORTCUT;

static HTSHORTCUT *HtSCut=NULL;

void HtStartSetShortCut(void)
{
	HtSCut=NULL;
}

void HtSetShortCut(int key,void (*func)(void))
{
	HTSHORTCUT *neo;
	neo=(HTSHORTCUT *)BiMalloc(sizeof(HTSHORTCUT));
	if(neo!=NULL)
	{
		neo->next=HtSCut;
		HtSCut=neo;
		neo->bikey=key;
		neo->func=func;
	}
}

void HtEndSetShortCut(void)
{
}

static void HtExecShortCut(int key)
{
	int bikey;
	HTSHORTCUT *ptr;
	if(0<=key && key<256)
	{
		bikey=BiFmtKeyToBikey(key);
		for(ptr=HtSCut; ptr!=NULL; ptr=ptr->next)
		{
			if(ptr->bikey==bikey)
			{
				(*ptr->func)();
				return;
			}
		}
	}
}

/*************************************************************************/

static int HtQuitFlag=HT_OFF;
static void HtDummyVoidFunc(void){}

static void (*HtIntervalFunc)(void)=HtDummyVoidFunc;

void HtQuitProgram(void)
{
	HtQuitFlag=HT_ON;
}

void HtEventLoop(void)
{
	EVENT *pev;

	while(HtQuitFlag==HT_OFF && BiFatal()==BI_OFF)
	{
		MMI_iosense();

		(*HtIntervalFunc)();

		/* Changed 1996/3/13  if -> while */
	    while(MMI_GetEvnt(EVALL,&pev)==0 && HtQuitFlag==HT_OFF)
		{
			switch(pev->what)
			{
			case EVKEY:
				HtExecShortCut(pev->info);
				break;
			}
			MMI_ExecEvnt(pev);
		}
	}
	BiFmtTerminate();
}

/****************************************************************************

                            Window Event Manager

****************************************************************************/
static void HtDummyButtonFunc(int trig,HTMOUSE *mos){}
static void HtDummyDragFunc(HTMOUSE *mos){}

/* trig:HT_ON/HT_OFF */
void (*HtOnLeftButton)(int trig,HTMOUSE *mos)=HtDummyButtonFunc;
void (*HtOnMiddleButton)(int trig,HTMOUSE *mos)=HtDummyButtonFunc;
void (*HtOnRightButton)(int trig,HTMOUSE *mos)=HtDummyButtonFunc;
void (*HtOnDrag)(HTMOUSE *mos)=HtDummyDragFunc;
void (*HtRepaintFunc)(void)=HtDummyVoidFunc;

/*pub*/void HtSetOnLeftButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtOnLeftButton=f;
}
/*pub*/void HtResetOnLeftButtonFunc(void)
{
	HtOnLeftButton=HtDummyButtonFunc;
}

/*pub*/void HtSetOnMiddleButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtOnMiddleButton=f;
}
/*pub*/void HtResetOnMiddleButtonFunc(void)
{
	HtOnMiddleButton=HtDummyButtonFunc;
}

/*pub*/void HtSetOnRightButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtOnRightButton=f;
}
/*pub*/void HtResetOnRightButtonFunc(void)
{
	HtOnRightButton=HtDummyButtonFunc;
}

/*pub*/void HtSetOnDragFunc(void (*f)(HTMOUSE *mos))
{
	HtOnDrag=f;
}
/*pub*/void HtResetOnDragFunc(void)
{
	HtOnDrag=HtDummyDragFunc;
}

/*pub*/void HtSetRepaintFunc(void (*f)(void))
{
	HtRepaintFunc=f;
}


void HtSetIntervalFunc(void (*func)(void))
{
	HtIntervalFunc=func;
}

void HtResetIntervalFunc(void)
{
	HtIntervalFunc=HtDummyVoidFunc;
}


/****************************************************************************

                         Shift Key Status Routine

****************************************************************************/
/*pub*/void HtGetShift(HTSHIFT *sft)
{
//	unsigned int mode;
//	mode=KYB_shift();
//	sft->shift=((mode & 1) ? HT_ON : HT_OFF);
//	sft->ctrl =((mode & 0x10) ? HT_ON : HT_OFF);

	unsigned char BiKeyMatrix[16];
	unsigned val;

	KYB_matrix((char *)BiKeyMatrix);

	val=BiKeyMatrix[0x53/8];
	val=(val&(1<<(0x53&7)));
	sft->shift=(val!=0 ? HT_ON : HT_OFF);

	val=BiKeyMatrix[0x52/8];
	val=(val&(1<<(0x52&7)));
	sft->ctrl=(val!=0 ? HT_ON : HT_OFF);
}




/****************************************************************************

                             Service Routines

  以下の関数で、値を返すものは、
           入力があった  →  HT_YES
           入力がなかった→  HT_NO
  を返すものとする。
****************************************************************************/
/*pub*/void HtMessageBox(char *msg,char *btn)
{
	char *msg3[3];
	msg3[0]=NULL;
	msg3[1]=msg;
	msg3[2]=NULL;
	HtFmtMessageBox(msg3,btn);
}

/*pub*/void HtMessageBox3(char *msg[3],char *btn)
{
	HtFmtMessageBox(msg,btn);
}

/*pub*/int HtLineInput(char *str,char *msg,char *def)
{
	return HtFmtLineInput(str,msg,def);
}

/*pub*/int HtYesNo(char *msg,char *yes,char *no)
{
	char *msg3[3];
	msg3[0]=NULL;
	msg3[1]=msg;
	msg3[2]=NULL;
	return HtFmtYesNo(msg3,yes,no);
}

/*pub*/int HtYesNo3(char *msg[3],char *yes,char *no)
{
	return HtFmtYesNo(msg,yes,no);
}

/*pub*/int HtInputNumber(real *n,real dflt,char *msg,char *yes,char *no)
{
	char *tit[3]={NULL,NULL,NULL};
	real num[3];
	int result;

	tit[1]="数値";
	num[1]=dflt;
	result=HtFmtInputNumbers(num,tit,msg,yes,no);
	*n=num[1];
	return result;
}

/*pub*/int HtInputInteger(int *n,int dflt,char *msg,char *yes,char *no)
{
	char *tit[3]={NULL,NULL,NULL};
	int num[3];
	int result;

	tit[1]="数値";
	num[1]=dflt;
	result=HtFmtInputInteger(num,tit,msg,yes,no);
	*n=num[1];
	return result;
}

/*pub*/int HtInputPoint2(BIPOINT2 *p,BIPOINT2 *df,char *msg,char *y,char *n)
{
	char *tit[3]={"X :","Y :",NULL};
	real num[3];
	int result;

	num[0]=(df!=NULL ? df->x : 0);
	num[1]=(df!=NULL ? df->y : 0);
	result=HtFmtInputNumbers(num,tit,msg,y,n);
	p->x=num[0];
	p->y=num[1];
	return result;
}

/*pub*/int HtInputPoint2i(int p[2],int df[2],char *msg,char *y,char *n)
{
	char *tit[3]={"X :","Y :",NULL};
	int num[3];
	int result;

	num[0]=(df!=NULL ? df[0] : 0);
	num[1]=(df!=NULL ? df[1] : 0);
	result=HtFmtInputInteger(num,tit,msg,y,n);
	p[0]=num[0];
	p[1]=num[1];
	return result;
}

/*pub*/int HtInputPoint3(BIPOINT *p,BIPOINT *df,char *msg,char *y,char *n)
{
	char *tit[3]={"X :","Y :","Z :"};
	real num[3];
	int result;

	num[0]=(df!=NULL ? df->x : 0);
	num[1]=(df!=NULL ? df->y : 0);
	num[2]=(df!=NULL ? df->z : 0);
	result=HtFmtInputNumbers(num,tit,msg,y,n);
	p->x=num[0];
	p->y=num[1];
	p->z=num[2];
	return result;
}

/*pub*/int HtInputPoint3i(int p[3],int df[3],char *msg,char *y,char *n)
{
	char *tit[3]={"X :","Y :","Z:"};
	int num[3];
	int result;

	num[0]=(df!=NULL ? df[0] : 0);
	num[1]=(df!=NULL ? df[1] : 0);
	num[2]=(df!=NULL ? df[2] : 0);
	result=HtFmtInputInteger(num,tit,msg,y,n);
	p[0]=num[0];
	p[1]=num[1];
	p[2]=num[2];
	return result;
}

/*pub*/int HtInputAngle3(BIANGLE *a,BIANGLE *df,char *msg,char *yes,char *no)
{
	char *tit[3]={"Heading  :","Pitching :","Bank     :"};
	real n[3];
	int result;

	n[0]=0;
	n[1]=0;
	n[2]=0;
	result=HtFmtInputNumbers(n,tit,msg,yes,no);
	a->h=(long)(n[0] * 32768 / 180);
	a->p=(long)(n[1] * 32768 / 180);
	a->b=(long)(n[2] * 32768 / 180);
	return result;
}

/*pub*/int HtLoadFileDialog(char *fn,char *msg,char *df,char *ext)
{
	return HtFmtFileDialog(fn,msg,df,ext);
}

/*pub*/int HtSaveFileDialog(char *fn,char *msg,char *df,char *ext)
{
	return HtFmtFileDialog(fn,msg,df,ext);
}

/*pub*/int HtColorDialog(BICOLOR *col,BICOLOR *def,char *mes)
{
	return HtFmtColorDialog(col,def,mes);
}

int HtListBox(int *n,char *str[],char *msg,char *yes,char *no)
{
	return HtFmtListBoxSingle(n,str,msg,yes,no);
}

int HtListBoxPl(int *nm,int n[],char *str[],char *msg,char *yes,char *no)
{
	return HtFmtListBoxPl(nm,n,str,msg,yes,no);
}

/****************************************************************************

                          TOWNS Local Function

****************************************************************************/
//MOUSEON 　イベント
//what　 : EVMOSDN
//shift  : shift情報
//info　 : 座標(POINT型)
//data　 : なし
//time　 : 取得時間
static void HtFmtLeftButtonOn(void);
static void HtFmtLeftButtonOff(void);
static void HtFmtRightButtonOn(void);
static void HtFmtRightButtonOff(void);
static void HtFmtSetHtmouse(HTMOUSE *mos);
static void HtFmtDragLoop(void);

int HtFmtWindowFunc(int kobj,int messId,int argc,EVENT *pev)
{
	if(messId==MM_MOUSEON)
	{
		int bt,mx,my;

		MOS_rdpos(&bt,&mx,&my);
		if((bt&1)!=0)
		{
			HtFmtLeftButtonOn();
			HtFmtDragLoop();
			HtFmtLeftButtonOff();
		}
		else if((bt&2)!=0)
		{
			HtFmtRightButtonOn();
			HtFmtDragLoop();
			HtFmtRightButtonOff();
		}
	}
	else if(messId==MM_SHOW || messId==MM_UPDATE)
	{
		static AntiRecursive=0;
		if(AntiRecursive==0)
		{
			AntiRecursive=1;
			(*HtRepaintFunc)();
			AntiRecursive=0;
		}
	}
	return NOERR;
}

static void HtFmtLeftButtonOn(void)
{
	HTMOUSE mos;
	HtFmtSetHtmouse(&mos);
	(*HtOnLeftButton)(HT_ON,&mos);
}

static void HtFmtLeftButtonOff(void)
{
	HTMOUSE mos;
	HtFmtSetHtmouse(&mos);
	(*HtOnLeftButton)(HT_OFF,&mos);
}

static void HtFmtRightButtonOn(void)
{
	HTMOUSE mos;
	HtFmtSetHtmouse(&mos);
	(*HtOnRightButton)(HT_ON,&mos);
}

static void HtFmtRightButtonOff(void)
{
	HTMOUSE mos;
	HtFmtSetHtmouse(&mos);
	(*HtOnRightButton)(HT_OFF,&mos);
}

static void HtFmtSetHtmouse(HTMOUSE *mos)
{
	int bt,mx,my;
	POINT pnt;
	MOS_rdpos(&bt,&mx,&my);
	pnt.x=mx;
	pnt.y=my;
	MG_GlobalToLocal(&pnt);
	mos->mx=pnt.x;
	mos->my=pnt.y;
	mos->lbt=((bt&1)!=0 ? HT_ON : HT_OFF);
	mos->mbt=HT_OFF;
	mos->rbt=((bt&2)!=0 ? HT_ON : HT_OFF);
}

static void HtFmtDragLoop(void)
{
	POINT pnt;
	EVENT *pev;
	HTMOUSE mos;

	while(1)
	{
		MMI_iosense();
		if(MMI_GetEvnt(EVMOSUP | EVMOSMOVE | EVMOSDRAG,&pev)==0)
		{
			pnt = *((POINT *)&pev->info);
			MG_GlobalToLocal(&pnt);
			if((pev->what & EVMOSUP)!=0)
			{
				return;
			}
			else if((pev->what & (EVMOSMOVE|EVMOSDRAG))!=0)
			{
				HtFmtSetHtmouse(&mos);
				(*HtOnDrag)(&mos);
			}
		}
	}
}


int HtFmtInitAllGuiParts(void)
{
	int ret;
	if ((ret = MMI_initHyper())         < 0)return ret ;
	if ((ret = MMI_initDialogL40())     < 0)return ret ;
	if ((ret = MMI_initAlertL40())      < 0)return ret ;
	if ((ret = MMI_initWindowL40())     < 0)return ret ;
	if ((ret = MMI_initMessageL40())    < 0)return ret ;
	if ((ret = MMI_initMenuL40())       < 0)return ret ;
	if ((ret = MMI_initPMenuL40())      < 0)return ret ;
	if ((ret = MMI_initButtonL40())     < 0)return ret ;
	if ((ret = MMI_initDrawButtonL40()) < 0)return ret ;
	if ((ret = MMI_initIconL40())       < 0)return ret ;
	if ((ret = MMI_initToggleIconL40()) < 0)return ret ;
	if ((ret = MMI_initMenuItemL40())   < 0)return ret ;
	if ((ret = MMI_initScrollBarL40())  < 0)return ret ;
	if ((ret = MMI_initTextL40())       < 0)return ret ;
	if ((ret = MMI_initListMenuL40())   < 0)return ret ;
	if ((ret = MMI_initNumBoxL40())     < 0)return ret ;
	return NOERR ;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
