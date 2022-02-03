/* Machine Depending Module : FM-TOWNS none GUI-Library version */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "concorde.h"

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
#include "../impulse.h"



extern char BiWindowName[];

static int FullScrnFlag=BI_OFF;



PAGE *BiWritePage;
char BiEgb[EgbWorkSize];
char BiMos[MosWorkSize];
static PAGE BiVirtual;
static short *BiVirtualVram=NULL;
static long winX,winY;


int BiWndId;


#define COL15(c) ((((c)->g<<7)&0x7c00)+(((c)->r<<2)&0x3e0)+(((c)->b>>3)&0x1f))

/*****************************************************************************

   Setting

*****************************************************************************/
/*link*/void BiFmtInitialize(void);
/*link*/void BiFmtOpenWindow(long reqX,long reqY);
/*link*/void BiFmtTerminate(void);
/*link*/void BiFmtSetExtWindowFunc(int (*f)(int,int,int,EVENT *));
/*link*/int BiFmtKeyToBikey(int key);

static void BiFmtGetMmiCtrl(MMICTRL *ct);
static void BiFmtInitFdlg(void);
static void BiFmtTermFdlg(void);
static int BiFmtInitAllGuiParts(void);
static void BiFmtOpenVirtualWindow(void);
static void BiAllocVram(long reqX,long reqY);
static void BiFmtInitGuiItem(void);
static void BiFmtSetShellFunc(void);
static int BiShellCallFunc(int apId,int messId,int info,int data);
static void BiRepaintWindow(int WinId);
static void BiRepaintWindowUserArea(int WinId);
static int ConstrainClipFrame(FRAME *fr);
static void BiFmtSetCurrentDirectory(void);


static int BiFatalFlag=BI_OFF;
/* depend */int BiFatal(void)
{
	return BiFatalFlag;
}

/* depend */void BiOpenWindow(long reqX,long reqY)
{
	if((KYB_shift()&4)!=0 || FullScrnFlag==BI_ON)
	{
		FullScrnFlag=BI_ON;
		reqX=300;
		reqY=220;
	}


	BiFmtInitialize();
	/* BiFmtInitilize内でエラーが起きたらどうすることもできない ^_^; **
	** さすがにこういうときはexitくらい使えてほしい ^_^;             */

	BiAllocVram(reqX,reqY);
	YGH_initPage(&BiVirtual,SEG_VIRTU,ADR(BiVirtualVram),reqX,reqY);
	YGH_clearPage(&BiVirtual,0);

	BiWritePage=&BiVirtual;

	BiFmtInitGuiItem();
	MMI_SendMessage(BiWndId,MM_SETMSG,1,BiWindowName);
	BiFmtOpenVirtualWindow();
	if(FullScrnFlag==BI_ON)
	{
		HYPER hyp;
		MMI_SendMessage(BiWndId,MM_GETHYPER,1,&hyp);
		MMI_SendMessage(BiWndId,MM_MOVE,2,-hyp.fr.lupx,-hyp.fr.lupy-8);
		BiRepaintWindow(BiWndId);
	}

	BiFmtSetCurrentDirectory();
}

static void BiFmtGetMmiCtrl(MMICTRL *ct)
{
	ct->page0=SCREEN32K;
	ct->page1=SCREENUNUSED;
	ct->writePage=0;
	ct->displayPage=1;
	ct->priority=0;
	ct->mode=SCREENAVAILABLE;
	ct->width=SCREENEXPAND;

	ct->size=0;
	ct->ptr=NULL;

	ct->asize=0;
	ct->aptr=NULL;

	ct->move.lupx=-16384;
	ct->move.lupy=    20;
	ct->move.rdwx= 16383;
	ct->move.rdwy= 16383;

	ct->white=15;
	ct->black=0;
	ct->gray=7;
	ct->xor=15;
}

static void BiFmtInitFdlg(void)
{
	FDG_SaveCurDir();
	FDG_InitFileDlg();
}

static void BiFmtTermFdlg(void)
{
	FDG_FreeFileDlg();
	FDG_RecovCurDir();
}

static int BiFmtInitAllGuiParts(void)
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

static void BiFmtOpenVirtualWindow(void)
{
	long winWid,winHei,wid,hei;
	HYPER hyp;
	FRAME usr,rusr;
	PAGE oldPage;
	short *oldVram;

	oldPage=BiVirtual;
	oldVram=BiVirtualVram;

	MMI_SendMessage(BiWndId,MM_GETHYPER,1,&hyp);
	MMI_SendMessage(BiWndId,MM_GETUSER,2,&usr,&rusr);
	winWid=hyp.fr.rdwx-hyp.fr.lupx+1;
	winHei=hyp.fr.rdwy-hyp.fr.lupy+1;
	wid=winWid-4;
	hei=winHei-20;
	usr.rdwx=wid;
	usr.rdwy=hei;
	MMI_SendMessage(BiWndId,MM_SETUSER,2,&usr,&rusr);

	BiAllocVram(wid,hei);
	YGH_initPage(&BiVirtual,SEG_VIRTU,ADR(BiVirtualVram),wid,hei);
	YGH_clearPage(&BiVirtual,0);
	YGH_pageCopy(&BiVirtual,&oldPage);
	BiRepaintWindow(BiWndId);

	BiFree(oldVram);
}

static void BiAllocVram(long reqX,long reqY)
{
	winX=reqX;
	winY=reqY;
	BiVirtualVram=(short *)TL_malloc(sizeof(short)*reqX*reqY);
}

static void BiFmtInitGuiItem(void)
{
	extern MMIINIT	initDataITWGUS;

	MMI_Init(&initDataITWGUS);

	MMI_SendMessage(BiWndId,MM_WAKE,0);
	MMI_SendMessage(MMI_GetBaseObj(), MM_SHOW, 0) ;
}

extern char BiWindowName[];

static void BiFmtSetShellFunc(void)
{
	int bas,app;

	bas=MMI_GetBaseObj();
	app=MMI_GetApliId();
	MMI_SendMessage(bas,MM_SETEXEC,1,BiShellCallFunc);
	MMI_CallMessage(app,GM_TITLE,(int)BiWindowName,0);
}

static int BiShellCallFunc(int apId,int messId,int info,int data)
{
	BiFmtSetCurrentDirectory();

	switch(messId)
	{
	case GM_QUIT:
		MMI_SetHaltFlag(TRUE);
		return NOERR;
	default:
		return ILLEGAL_FUNCTION;
	}
}

/* depend */void BiCloseWindow(void)
{
	BiFmtTermFdlg();
	MMI_Close() ;
	TL_free(BiVirtualVram);
}

/* depend */void BiGetWindowSize(long *wid,long *hei)
{
	FRAME usr,rusr;
	MMI_SendMessage(BiWndId,MM_GETUSER,2,&usr,&rusr);
	*wid=usr.rdwx;
	*hei=usr.rdwy;
}

/* depend */void BiGetStdProjection(BIPROJ *prj)
{
	long wid,hei;
	BiGetWindowSize(&wid,&hei);
	prj->lx=wid;
	prj->ly=hei;
	prj->cx=wid/2;
	prj->cy=hei/2;
	prj->magx=(real)wid/1.41421356;
	prj->magy=prj->magx;
	prj->nearz=0.5;
	prj->farz=10000.0;
}

/* depend */void BiSwapBuffers(void)
{
	BiRepaintWindowUserArea(BiWndId);
}



/*****************************************************************************

   GUI-LIB Call Back Function

*****************************************************************************/
static void BiFmtWindowUpdate(int kobj);
static void BiFmtWindowShow(int kobj);

static int BiFmtExtWindowFuncSw=BI_OFF;
static int (*BiFmtExtWindowFunc)(int,int,int,EVENT *);

int BiWindowFunc(int kobj,int messId,int argc,EVENT *pev)
{
	if(messId==MM_UPDATE)
	{
		BiFmtWindowUpdate(kobj);
	}
	else if(messId==MM_SHOW)
	{
		BiFmtWindowShow(kobj);
	}

	if(BiFmtExtWindowFuncSw==BI_ON)
	{
		(*BiFmtExtWindowFunc)(kobj,messId,argc,pev);
	}

	return NOERR;
}

static void BiFmtWindowUpdate(int kobj)
{
	BiFmtOpenVirtualWindow();
	BiRepaintWindowUserArea(kobj);
}

static void BiFmtWindowShow(int kobj)
{
	char *egb;
//	long winX,winY;
	struct {
		unsigned char *dat;
		unsigned short ds;
		short x1,y1,x2,y2;
		unsigned short bmLx,bmLy;
	} para;

	egb=MMI_GetEgbPtr();
//	BiGetWindowSize(&winX,&winY);

	para.dat=(unsigned char *)BiVirtualVram;
	para.ds=getds();
	para.x1=0;
	para.y1=0;
	para.x2=para.x1+winX-1;
	para.y2=para.y1+winY-1;
	MG_mosDisp(2);
	EGB_writeMode(egb,9); /* OPAQUE */
	WGB_putBlock(egb,1,(char *)&para);
	MG_mosDisp(3);
}

// for Reference
//static void BiFmtWindowMouse(int kobj,EVENT *pev)
//{
//	int lf,md,ri;
//	long mx,my,pmx,pmy;
//	POINT pnt;
//
//	BiMouse(&lf,&md,&ri,&mx,&my);
//	pnt = *((POINT *)&pev->info);
//	(*BiOnPress)(lf,md,ri,pnt.x,pnt.y);
//
//	pmx=mx;
//	pmy=my;
//	while(lf==BI_ON || md==BI_ON || ri==BI_ON)
//	{
//		BiMouse(&lf,&md,&ri,&mx,&my);
//		if(pmx!=mx || pmy!=my)
//		{
//			(*BiOnDrag)(lf,md,ri,mx,my);
//		}
//	}
//
//	BiMouse(&lf,&md,&ri,&mx,&my);
//	(*BiOnRelease)(lf,md,ri,mx,my);
//}

static int ConstrainClipFrame(FRAME *fr);

static void BiRepaintWindow(int WinId)
{
	POINT org,orgStk;
	WINCLIP *visi,*visiStk;
	WINCLIP *clip,*clipStk;
	HYPER hyp;
	FRAME frm;

	org.x=0;
	org.y=0;
	MG_PushOrigin(&org,&orgStk);

	MMI_SendMessage(WinId,MM_GETHYPER,1,&hyp);

	frm=hyp.fr;
	if(ConstrainClipFrame(&frm)!=BI_TRUE)
	{
		return;
	}

	clip=WIN_getClipMemory(&frm,NULL);
	visi=WIN_copyClip(clip);
	if(clip!=NULL && visi!=NULL)
	{
		WIN_pushVisible(visi,&visiStk);
		WIN_pushClip(clip,&clipStk);
		MMI_SendMessage(WinId,MM_SHOW,0);
		WIN_popClip(clipStk);
		WIN_popVisible(visiStk);
	}

	MG_PopOrigin(&orgStk);
}

static void BiRepaintWindowUserArea(int WinId)
{
	POINT org,orgStk;
	WINCLIP *visi,*visiStk;
	WINCLIP *clip,*clipStk;
	HYPER hyp;
	FRAME frm,usr,rusr;

	org.x=0;
	org.y=0;
	MG_PushOrigin(&org,&orgStk);

	MMI_SendMessage(WinId,MM_GETHYPER,1,&hyp);
	MMI_SendMessage(WinId,MM_GETUSER,2,&usr,&rusr);
	frm.lupx = hyp.fr.lupx+usr.lupx;
	frm.lupy = hyp.fr.lupy+usr.lupy;
	frm.rdwx = frm.lupx  +usr.rdwx-1;
	frm.rdwy = frm.lupy  +usr.rdwy-1;
	if(ConstrainClipFrame(&frm)!=BI_TRUE)
	{
		return;
	}

	clip=WIN_getClipMemory(&frm,NULL);
	visi=WIN_copyClip(clip);
	if(clip!=NULL && visi!=NULL)
	{
		WIN_pushVisible(visi,&visiStk);
		WIN_pushClip(clip,&clipStk);
		MMI_SendMessage(WinId,MM_SHOW,0);
		WIN_popClip(clipStk);
		WIN_popVisible(visiStk);
	}

	MG_PopOrigin(&orgStk);
}

static int ConstrainClipFrame(FRAME *fr)
{
	WINCTRL *pctrl;
	FRAME bnd;

	MMI_GetControl(&pctrl);
	bnd = pctrl->bound;

	if(fr->rdwx < bnd.lupx || fr->lupx > bnd.rdwx ||
	   fr->rdwy < bnd.lupy || fr->lupy > bnd.rdwy)
	{
		return BI_FALSE;
	}

	fr->lupx = BiLarger(fr->lupx, bnd.lupx);
	fr->lupy = BiLarger(fr->lupy, bnd.lupy);
	fr->rdwx = BiSmaller(fr->rdwx, bnd.rdwx);
	fr->rdwy = BiSmaller(fr->rdwy, bnd.rdwy);

	return BI_TRUE;
}





/*****************************************************************************

   Device

*****************************************************************************/
#define MAX_KEYBUF 256
static int nKeyBuf;
static char keyBuf[MAX_KEYBUF];

static unsigned short BiKeyMap[256];
static unsigned char BiKeyMatrix[16];

static unsigned short BiAscToKc[256];
static unsigned short BiKcToAsc[256];

static void BiDeviceMatchKey(unsigned kc,unsigned asc)
{
	BiAscToKc[asc]=kc;
	BiKcToAsc[kc ]=asc;
}

/* depend */void BiDeviceInitialize(void)
{
	nKeyBuf=0;

	BiKeyMap[BIKEY_STOP]    =0x7c;
	BiKeyMap[BIKEY_COPY]    =0x7d;
	BiKeyMap[BIKEY_F1]      =0x5d;
	BiKeyMap[BIKEY_F2]      =0x5e;
	BiKeyMap[BIKEY_F3]      =0x5f;
	BiKeyMap[BIKEY_F4]      =0x60;
	BiKeyMap[BIKEY_F5]      =0x61;
	BiKeyMap[BIKEY_F6]      =0x62;
	BiKeyMap[BIKEY_F7]      =0x63;
	BiKeyMap[BIKEY_F8]      =0x64;
	BiKeyMap[BIKEY_F9]      =0x65;
	BiKeyMap[BIKEY_F10]     =0x66;
	BiKeyMap[BIKEY_ONE]     =0x02;
	BiKeyMap[BIKEY_TWO]     =0x03;
	BiKeyMap[BIKEY_THREE]   =0x04;
	BiKeyMap[BIKEY_FOUR]    =0x05;
	BiKeyMap[BIKEY_FIVE]    =0x06;
	BiKeyMap[BIKEY_SIX]     =0x07;
	BiKeyMap[BIKEY_SEVEN]   =0x08;
	BiKeyMap[BIKEY_EIGHT]   =0x09;
	BiKeyMap[BIKEY_NINE]    =0x0a;
	BiKeyMap[BIKEY_ZERO]    =0x0b;
	BiKeyMap[BIKEY_A]       =0x1e;
	BiKeyMap[BIKEY_B]       =0x2e;
	BiKeyMap[BIKEY_C]       =0x2c;
	BiKeyMap[BIKEY_D]       =0x20;
	BiKeyMap[BIKEY_E]       =0x1e;
	BiKeyMap[BIKEY_F]       =0x21;
	BiKeyMap[BIKEY_G]       =0x22;
	BiKeyMap[BIKEY_H]       =0x23;
	BiKeyMap[BIKEY_I]       =0x18;
	BiKeyMap[BIKEY_J]       =0x24;
	BiKeyMap[BIKEY_K]       =0x25;
	BiKeyMap[BIKEY_L]       =0x26;
	BiKeyMap[BIKEY_M]       =0x30;
	BiKeyMap[BIKEY_N]       =0x2f;
	BiKeyMap[BIKEY_O]       =0x19;
	BiKeyMap[BIKEY_P]       =0x1a;
	BiKeyMap[BIKEY_Q]       =0x11;
	BiKeyMap[BIKEY_R]       =0x14;
	BiKeyMap[BIKEY_S]       =0x1f;
	BiKeyMap[BIKEY_T]       =0x15;
	BiKeyMap[BIKEY_U]       =0x17;
	BiKeyMap[BIKEY_V]       =0x2d;
	BiKeyMap[BIKEY_W]       =0x12;
	BiKeyMap[BIKEY_X]       =0x2b;
	BiKeyMap[BIKEY_Y]       =0x16;
	BiKeyMap[BIKEY_Z]       =0x2a;
	BiKeyMap[BIKEY_SPACE]   =0x35;
	BiKeyMap[BIKEY_ESC]     =0x01;
	BiKeyMap[BIKEY_TAB]     =0x10;
	BiKeyMap[BIKEY_CTRL]    =0x52;
	BiKeyMap[BIKEY_SHIFT]   =0x53;
	BiKeyMap[BIKEY_ALT]     =0x55;
	BiKeyMap[BIKEY_BS]      =0x0f;
	BiKeyMap[BIKEY_RET]     =0x1d;
	BiKeyMap[BIKEY_HOME]    =0x4e;
	BiKeyMap[BIKEY_DEL]     =0x4b;
	BiKeyMap[BIKEY_UP]      =0x4d;
	BiKeyMap[BIKEY_DOWN]    =0x50;
	BiKeyMap[BIKEY_LEFT]    =0x4f;
	BiKeyMap[BIKEY_RIGHT]   =0x51;
	BiKeyMap[BIKEY_TEN0]    =0x46;
	BiKeyMap[BIKEY_TEN1]    =0x42;
	BiKeyMap[BIKEY_TEN2]    =0x43;
	BiKeyMap[BIKEY_TEN3]    =0x44;
	BiKeyMap[BIKEY_TEN4]    =0x3e;
	BiKeyMap[BIKEY_TEN5]    =0x3f;
	BiKeyMap[BIKEY_TEN6]    =0x40;
	BiKeyMap[BIKEY_TEN7]    =0x3a;
	BiKeyMap[BIKEY_TEN8]    =0x3b;
	BiKeyMap[BIKEY_TEN9]    =0x3c;
	BiKeyMap[BIKEY_TENMUL]  =0x36;
	BiKeyMap[BIKEY_TENDIV]  =0x37;
	BiKeyMap[BIKEY_TENPLUS] =0x38;
	BiKeyMap[BIKEY_TENMINUS]=0x39;
	BiKeyMap[BIKEY_TENEQUAL]=0x3d;
	BiKeyMap[BIKEY_TENDOT]  =0x47;

    BiDeviceMatchKey(BIKEY_STOP    ,0x00);
    BiDeviceMatchKey(BIKEY_COPY    ,0x00);
    BiDeviceMatchKey(BIKEY_F1      ,0x00);
    BiDeviceMatchKey(BIKEY_F2      ,0x00);
    BiDeviceMatchKey(BIKEY_F3      ,0x00);
    BiDeviceMatchKey(BIKEY_F4      ,0x00);
    BiDeviceMatchKey(BIKEY_F5      ,0x00);
    BiDeviceMatchKey(BIKEY_F6      ,0x00);
    BiDeviceMatchKey(BIKEY_F7      ,0x00);
    BiDeviceMatchKey(BIKEY_F8      ,0x00);
    BiDeviceMatchKey(BIKEY_F9      ,0x00);
    BiDeviceMatchKey(BIKEY_F10     ,0x00);
    BiDeviceMatchKey(BIKEY_ONE     ,'1');
    BiDeviceMatchKey(BIKEY_TWO     ,'2');
    BiDeviceMatchKey(BIKEY_THREE   ,'3');
    BiDeviceMatchKey(BIKEY_FOUR    ,'4');
    BiDeviceMatchKey(BIKEY_FIVE    ,'5');
    BiDeviceMatchKey(BIKEY_SIX     ,'6');
    BiDeviceMatchKey(BIKEY_SEVEN   ,'7');
    BiDeviceMatchKey(BIKEY_EIGHT   ,'8');
    BiDeviceMatchKey(BIKEY_NINE    ,'9');
    BiDeviceMatchKey(BIKEY_ZERO    ,'0');
    BiDeviceMatchKey(BIKEY_A       ,'a');
    BiDeviceMatchKey(BIKEY_B       ,'b');
    BiDeviceMatchKey(BIKEY_C       ,'c');
    BiDeviceMatchKey(BIKEY_D       ,'d');
    BiDeviceMatchKey(BIKEY_E       ,'e');
    BiDeviceMatchKey(BIKEY_F       ,'f');
    BiDeviceMatchKey(BIKEY_G       ,'g');
    BiDeviceMatchKey(BIKEY_H       ,'h');
    BiDeviceMatchKey(BIKEY_I       ,'i');
    BiDeviceMatchKey(BIKEY_J       ,'j');
    BiDeviceMatchKey(BIKEY_K       ,'k');
    BiDeviceMatchKey(BIKEY_L       ,'l');
    BiDeviceMatchKey(BIKEY_M       ,'m');
    BiDeviceMatchKey(BIKEY_N       ,'n');
    BiDeviceMatchKey(BIKEY_O       ,'o');
    BiDeviceMatchKey(BIKEY_P       ,'p');
    BiDeviceMatchKey(BIKEY_Q       ,'q');
    BiDeviceMatchKey(BIKEY_R       ,'r');
    BiDeviceMatchKey(BIKEY_S       ,'s');
    BiDeviceMatchKey(BIKEY_T       ,'t');
    BiDeviceMatchKey(BIKEY_U       ,'u');
    BiDeviceMatchKey(BIKEY_V       ,'v');
    BiDeviceMatchKey(BIKEY_W       ,'w');
    BiDeviceMatchKey(BIKEY_X       ,'x');
    BiDeviceMatchKey(BIKEY_Y       ,'y');
    BiDeviceMatchKey(BIKEY_Z       ,'z');
    BiDeviceMatchKey(BIKEY_A       ,'A');
    BiDeviceMatchKey(BIKEY_B       ,'B');
    BiDeviceMatchKey(BIKEY_C       ,'C');
    BiDeviceMatchKey(BIKEY_D       ,'D');
    BiDeviceMatchKey(BIKEY_E       ,'E');
    BiDeviceMatchKey(BIKEY_F       ,'F');
    BiDeviceMatchKey(BIKEY_G       ,'G');
    BiDeviceMatchKey(BIKEY_H       ,'H');
    BiDeviceMatchKey(BIKEY_I       ,'I');
    BiDeviceMatchKey(BIKEY_J       ,'J');
    BiDeviceMatchKey(BIKEY_K       ,'K');
    BiDeviceMatchKey(BIKEY_L       ,'L');
    BiDeviceMatchKey(BIKEY_M       ,'M');
    BiDeviceMatchKey(BIKEY_N       ,'N');
    BiDeviceMatchKey(BIKEY_O       ,'O');
    BiDeviceMatchKey(BIKEY_P       ,'P');
    BiDeviceMatchKey(BIKEY_Q       ,'Q');
    BiDeviceMatchKey(BIKEY_R       ,'R');
    BiDeviceMatchKey(BIKEY_S       ,'S');
    BiDeviceMatchKey(BIKEY_T       ,'T');
    BiDeviceMatchKey(BIKEY_U       ,'U');
    BiDeviceMatchKey(BIKEY_V       ,'V');
    BiDeviceMatchKey(BIKEY_W       ,'W');
    BiDeviceMatchKey(BIKEY_X       ,'X');
    BiDeviceMatchKey(BIKEY_Y       ,'Y');
    BiDeviceMatchKey(BIKEY_Z       ,'Z');
    BiDeviceMatchKey(BIKEY_SPACE   ,' ');
    BiDeviceMatchKey(BIKEY_ESC     ,0x1b);
    BiDeviceMatchKey(BIKEY_TAB     ,'\t');
    BiDeviceMatchKey(BIKEY_CTRL    ,0x00);
    BiDeviceMatchKey(BIKEY_SHIFT   ,0x00);
    BiDeviceMatchKey(BIKEY_ALT     ,0x00);
    BiDeviceMatchKey(BIKEY_BS      ,0x08);
    BiDeviceMatchKey(BIKEY_RET     ,0x0d);
    BiDeviceMatchKey(BIKEY_HOME    ,0x00);
    BiDeviceMatchKey(BIKEY_DEL     ,0x00);
    BiDeviceMatchKey(BIKEY_UP      ,0x1e);
    BiDeviceMatchKey(BIKEY_DOWN    ,0x1f);
    BiDeviceMatchKey(BIKEY_LEFT    ,0x1d);
    BiDeviceMatchKey(BIKEY_RIGHT   ,0x1c);
    BiDeviceMatchKey(BIKEY_TEN0    ,0);
    BiDeviceMatchKey(BIKEY_TEN1    ,0);
    BiDeviceMatchKey(BIKEY_TEN2    ,0);
    BiDeviceMatchKey(BIKEY_TEN3    ,0);
    BiDeviceMatchKey(BIKEY_TEN4    ,0);
    BiDeviceMatchKey(BIKEY_TEN5    ,0);
    BiDeviceMatchKey(BIKEY_TEN6    ,0);
    BiDeviceMatchKey(BIKEY_TEN7    ,0);
    BiDeviceMatchKey(BIKEY_TEN8    ,0);
    BiDeviceMatchKey(BIKEY_TEN9    ,0);
    BiDeviceMatchKey(BIKEY_TENMUL  ,'*');
    BiDeviceMatchKey(BIKEY_TENDIV  ,'/');
    BiDeviceMatchKey(BIKEY_TENPLUS ,'+');
    BiDeviceMatchKey(BIKEY_TENMINUS,'-');
    BiDeviceMatchKey(BIKEY_TENEQUAL,'=');
    BiDeviceMatchKey(BIKEY_TENDOT  ,'.');
}

static void BiFmtAddKeyToBuffer(int kc)
{
	if(0<=kc && kc<256)
	{
		keyBuf[nKeyBuf]=BiAscToKc[kc];
		nKeyBuf++;
	}
}

/* depend */void BiUpdateDevice(void)
{
	EVENT *pev;

	KYB_matrix((char *)BiKeyMatrix);
	MMI_iosense();

    while(MMI_GetEvnt(EVALL,&pev)==0)
	{
		switch(pev->what)
		{
		case EVKEY:
			BiFmtAddKeyToBuffer(pev->info & 0xff);
			break;
		}
		MMI_ExecEvnt(pev);
		MMI_iosense();
	}
}

//.MMI_SetEvnt
//イベントの登録
//
//定  　義
//int    MMI_SetEvnt( pevent );
//EVENT    *pevent;
//
//戻り値
//0 　：正常終了
//負数：エラーコード
//
//参　　照
//MMI_ExecSystem, MMI_GetEvnt, MMI_SnsEvnt, MMI_ExecEvnt
//
//例
//EVENT    event;
//
//MMI_SetEvnt( &event );
//
//内　　容
//新しいイベント構造体を、イベントキューにセットします。イベントの内容はイベント//キューに複写されますので、呼び出し元のイベントは保存しておく必要はありません。//
//取得時間は、MOS_getTime関数で得られた値を設定します。
//
//EVENT 構造体
//short    what;　　　 /*   イベントの種類　*/
//short 　 shift; 　　 /*   シフト状態　　　*/
//int      info;       /*   付属情報　　　　*/
//int      data;       /*   付属データ　　　*/
//int      time;       /*   取得時間　　　　*/
//
//MOUSEON 　イベント
//what　 : EVMOSDN
//shift  : shift情報
//info　 : 座標(POINT型)
//data　 : なし
//time　 : 取得時間
//
//MOUSEOFF　イベント
//what   : EVMOSUP
//shift  : shift情報
//info　 : 座標(POINT型)
//data　 : なし
//time　 : 取得時間
//
//KEYON 　　イベント
//what　 : EVKEY
//shift  : shift情報
//info　 : encode情報(漢字は、1文字として代入)
//data　 : なし
//time　 : 取得時間
//
//MOUSEMOVE イベント
//what　 : EVMOSMOVE
//shift  : shift情報
//info　 : 座標(POINT型)
//data　 : なし
//time　 : 取得時間
//
//MOUSEDRAG イベント
//what　 : EVMOSDRAG
//shift  : shift情報
//info　 : 座標(POINT型)
//data　 : なし
//time　 : 取得時間
//
//MESSAGE   イベント
//what　 : EVMESSAGE
//shift  : オブジェクトID
//info　 : メッセージID
//data　 : MESSAGEイベントデータ列へのポインタ
//time　 : 取得時間
//
//TIMER     イベント
//what　 : EVTIMER
//shift  : shift情報
//info　 : なし
//data　 : なし
//time　 : 取得時間
//
//EXEC      イベント
//what　 : EVEXEC
//shift  : 引数の数
//info　 : 関数のアドレス
//data　 : 引数へのアドレス
//time　 : 取得時間



/* depend */void BiMouse(int *lbt,int *mbt,int *rbt,long *mx,long *my)
{
	HYPER hyp;
	FRAME usr,rusr;
	int bt,tmx,tmy;

	MMI_SendMessage(BiWndId,MM_GETHYPER,1,&hyp);
	MMI_SendMessage(BiWndId,MM_GETUSER,2,&usr,&rusr);
	MOS_rdpos(&bt,&tmx,&tmy);

	tmx-=(hyp.fr.lupx+usr.lupx);
	tmy-=(hyp.fr.lupy+usr.lupy);

	*lbt=((bt&1)!=0 ? BI_ON : BI_OFF);
	*mbt=BI_OFF;
	*rbt=((bt&2)!=0 ? BI_ON : BI_OFF);
	*mx=tmx;
	*my=tmy;
}

/* depend */int BiKey(int kcode)
{
	unsigned adr,val;
	adr=BiKeyMap[kcode];
	val=BiKeyMatrix[adr/8];
	val=(val&(1<<(adr&7)));
	return (val!=0 ? BI_ON : BI_OFF);
}

/* depend */int BiInkey(void)
{
	int i;
	char key;
	if(nKeyBuf>0)
	{
		key=keyBuf[0];
		for(i=0; i<nKeyBuf-1; i++)
		{
			keyBuf[i]=keyBuf[i+1];
		}
		nKeyBuf--;
		return (int)key;
	}
	else
	{
		return 0;
	}
}






/*****************************************************************************

   GUI Support

*****************************************************************************/

/* depend */void BiSetOnPressFunc(void (*f)(int ,int ,int ,long ,long ))
{
    /**********************************************************************
    ウィンドウ上でマウスのボタンを押したとき呼び出す関数を登録する。この関
    数は、非GUI環境用ライブラリでは何もしない。
    **********************************************************************/
}

/* depend */void BiSetOnReleaseFunc(void (*f)(int ,int ,int ,long ,long ))
{
    /**********************************************************************
    ウィンドウ上でマウスのボタンを放したとき呼び出す関数を登録する。この関
    数は、非GUI環境用ライブラリでは何もしない。
    **********************************************************************/
}

/* depend */void BiSetOnDragFunc(void (*f)(int ,int ,int ,long ,long ))
{
    /**********************************************************************
    ウィンドウ上でマウスをドラッグ移動したとき呼び出す関数を登録する。この
    関数は、非GUI環境用ライブラリでは何もしない。
    **********************************************************************/
}

/* depend */void BiClearMouseOnFunc(void)
{
    /**********************************************************************
    BiSetOnPressFuncで設定した関数をクリアする。
    **********************************************************************/
}

/* depend */void BiClearMouseOffFunc(void)
{
    /**********************************************************************
    BiSetOnReleaseFuncで設定した関数をクリアする。
    **********************************************************************/
}

/* depend */void BiClearMouseDragFunc(void)
{
    /**********************************************************************
    BiSetOnDragFuncで設定した関数をクリアする。
    **********************************************************************/
}





/*****************************************************************************

   Memory Management

*****************************************************************************/

void BiMemoryInitialize(void)
{
	/* Nothing To Do */
}

void *BiMalloc(size_t uni)
{
	return (void *)TL_malloc(uni);
}

void BiFree(void *ptr)
{
	TL_free(ptr);
}



/*****************************************************************************

   Draw

*****************************************************************************/
/********************************************************************
このBiPrjは、BiGetStdProjectionが返すものではなく、アプリケーションが
ライブラリに登録したものなので注意
********************************************************************/
extern BIPROJ BiPrj;

/* depend */void BiClearScreenD(void)
{
	YGH_clearPage(BiWritePage,0);
}

/* depend */void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	/* c->r,c->g,c->b の色で s[0],s[1]間に直線をひく */
	YGH_color(BiWritePage,COL15(c));
	YGH_line(BiWritePage,s1->x,s1->y,s2->x,s2->y);
}

/* depend */void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
	YGH_color(BiWritePage,COL15(c));
	YGH_pset(BiWritePage,s->x,s->y);
}

/* depend */void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	int *ar,ed[4];
	unsigned long ptr;

	ar=BiPushTmpStack(&ptr,sizeof(int)*16*BiPrj.ly);
	YGH_color(BiWritePage,COL15(c));
	YGH_circleArea(BiWritePage,ar,ed,s->x,s->y,(radx+rady)>>1);
	YGH_areaPaint(BiWritePage,ar,ed);
	BiPopTmpStack(ptr);
}

/* depend */void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
	int *ar,ed[4],i,*para;
	unsigned long ptr;

	ar=BiPushTmpStack(&ptr,sizeof(int)*16*winY);
	para=BiGetTmpStack(sizeof(int)*(1+ns*2));
	if(ar!=NULL && para!=NULL)
	{
		para[0]=ns;
		for(i=0; i<ns; i++)
		{
			para[i*2+1]=(long)s[i].x;
			para[i*2+2]=(long)s[i].y;
		}
		YGH_color(BiWritePage,COL15(c));
		if(YGH_polygonArea(BiWritePage,ar,ed,para)==0)
		{
			YGH_areaPaint(BiWritePage,ar,ed);
		}
	}
	BiPopTmpStack(ptr);
}

/* depend */void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	YGH_color(BiWritePage,COL15(c));
	YGH_filledBox(BiWritePage,s1->x,s1->y,s2->x,s2->y);
}

/* depend */void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
	#define BIMKRAD 2
	BIPOINTS p1,p2;
	switch(mkType)
	{
	case BIMK_RECT:
		BiSetPoint2(&p1, s->x-BIMKRAD,s->y-BIMKRAD);
		BiSetPoint2(&p2, s->x+BIMKRAD,s->y+BIMKRAD);
		BiDrawRect2D(&p1,&p2,c);
		break;
	case BIMK_CIRCLE:
		BiDrawCircle2D(s,BIMKRAD,BIMKRAD,c);
		break;
	case BIMK_CROSS:
		BiSetPoint2(&p1, s->x-BIMKRAD,s->y-BIMKRAD);
		BiSetPoint2(&p2, s->x+BIMKRAD,s->y+BIMKRAD);
		BiDrawLine2D(&p1,&p2,c);
		BiSetPoint2(&p1, s->x+BIMKRAD,s->y-BIMKRAD);
		BiSetPoint2(&p2, s->x-BIMKRAD,s->y+BIMKRAD);
		BiDrawLine2D(&p1,&p2,c);
		break;
	}
}

/* depend */void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
	YGH_color(BiWritePage,COL15(c));
	YGH_print5font(BiWritePage,s->x,s->y,str);
}

/* depend */void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	#define FONTX 5 /* フォントサイズ X */
	#define FONTY 6 /* フォントサイズ Y */
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;

	/* テキストの横文字数と縦文字数を数える */
	sWid=0;
	sHei=0;
	for(i=0; str[i]!=NULL; i++)
	{
		sWid=BiLarger(strlen(str[i]),sWid);
		sHei++;
	}

	/* 情報を元に、書き出し位置を決める */
	switch(h)
	{
	default:
	case BIMG_CENTER:
		xSta=s->x -(FONTX*sWid)/2;
		break;
	case BIMG_LEFT:
		xSta=s->x;
		break;
	case BIMG_RIGHT:
		xSta=s->x -(FONTX*sWid);
		break;
	}
	switch(v)
	{
	default:
	case BIMG_CENTER:
		ySta=(s->y+FONTY/2)-(FONTY*sHei)/2;
		break;
	case BIMG_TOP:
		ySta=s->y+FONTY-1;
		break;
	case BIMG_BOTTOM:
		ySta=s->y-(FONTY*sHei);
		break;
	}

	/* 後は書くだけ */
	tmp.x=xSta;
	tmp.y=ySta;
	for(i=0; str[i]!=NULL; i++)
	{
		BiDrawString2D(&tmp,str[i],c);
		tmp.y+=FONTY;
	}
}



/* 950628 Update */

void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	YGH_color(BiWritePage,COL15(col));
	YGH_box(BiWritePage,s1->x,s1->y,s2->x,s2->y);
}

void BiDrawEmptyPolyg2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	YGH_color(BiWritePage,COL15(col));
	for(i=0; i<n-1; i++)
	{
		YGH_line(BiWritePage,s[i].x,s[i].y,s[i+1].x,s[i+1].y);
	}
	YGH_line(BiWritePage,s[0].x,s[0].y,s[n-1].x,s[n-1].y);
}

void BiDrawPolyline2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	YGH_color(BiWritePage,COL15(col));
	for(i=0; i<n-1; i++)
	{
		YGH_line(BiWritePage,s[i].x,s[i].y,s[i+1].x,s[i+1].y);
	}
}

void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	int i;
	BIPOINTS v1,v2;

	YGH_color(BiWritePage,COL15(col));

	if(radx==rady)
	{
		YGH_circle(BiWritePage,s->x,s->y,radx);
	}
	else
	{
		BiSetPointS(&v2,radx,0);
		for(i=1; i<=8; i++)
		{
			v1=v2;

			v2.x=(long)((real)radx*BiCos(i*4096));
			v2.y=(long)((real)rady*BiSin(i*4096));

			YGH_line(BiWritePage,s->x+v1.x,s->y+v1.y,s->x+v2.x,s->y+v2.y);
			YGH_line(BiWritePage,s->x-v1.x,s->y+v1.y,s->x-v2.x,s->y+v2.y);
			YGH_line(BiWritePage,s->x+v1.x,s->y-v1.y,s->x+v2.x,s->y-v2.y);
			YGH_line(BiWritePage,s->x-v1.x,s->y-v1.y,s->x-v2.x,s->y-v2.y);
		}
	}
}

void BiGetColor(BICOLOR *col,BIPOINTS *s)
{
	int c;
	c=YGH_getColor(BiWritePage,s->x,s->y);
	col->g=((c>>10)&31)*255/31;
	col->r=((c>>5 )&31)*255/31;
	col->b=( c     &31)*255/31;
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
	int x,y,x1,y1,x2,y2;
	x1=BiSmaller(s1->x,s2->x);
	y1=BiSmaller(s1->y,s2->y);
	x2=BiLarger(s1->x,s2->x);
	y2=BiLarger(s1->y,s2->y);

	for(y=y1; y<=y2; y++)
	{
		for(x=x1; x<=x2; x++)
		{
			int c;
			c=YGH_getColor(BiWritePage,x,y);
			col->g=((c>>10)&31)*255/31;
			col->r=((c>>5 )&31)*255/31;
			col->b=( c     &31)*255/31;
			col++;
		}
	}
/* Future : YGH_get(PAGE *p,char *dat,int x1,int y1,int x2,int y2); */
}


/***************************************************************************

    HOTAS Library Linkage

***************************************************************************/

/*link*/void BiFmtInitialize(void)
{
	static int first=BI_ON;
	static MMICTRL ct;
	BiFmtGetMmiCtrl(&ct);

	if((KYB_shift()&4)!=0 || FullScrnFlag==BI_ON)
	{
		FullScrnFlag=BI_ON;

		ct.page0=10;
		ct.page1=10;
		ct.writePage=0;
		ct.displayPage=3;
		ct.priority=0;
		ct.mode=0;
		ct.width=1024;

		ct.fr.lupx=0;
		ct.fr.lupy=0;
		ct.fr.rdwx=319;
		ct.fr.rdwy=239;

		ct.move.lupx=-16384;
		ct.move.lupy=-16384;
		ct.move.rdwx= 16383;
		ct.move.rdwy= 16383;
	}

	if(first==BI_ON && MMI_Open(&ct)==NOERR)
	{
		BiFmtInitAllGuiParts();
		if(FullScrnFlag==BI_ON)
		{
			char *egb;
			egb=MMI_GetEgbPtr();
			EGB_displayStart(egb,2,2,2);
			EGB_displayStart(egb,3,320,240);
		}
		BiFmtInitFdlg();
		BiFmtSetShellFunc();
		first=BI_OFF;
	}
	/* エラーが起きたらどうすることもできない  ^_^; */
}

/*link*/void BiFmtOpenWindow(long reqX,long reqY)
{
	BiAllocVram(reqX,reqY);
	YGH_initPage(&BiVirtual,SEG_VIRTU,ADR(BiVirtualVram),reqX,reqY);
	YGH_clearPage(&BiVirtual,0);

	BiWritePage=&BiVirtual;

	BiFmtInitGuiItem();
	BiFmtOpenVirtualWindow();
}

/*link*/void BiFmtTerminate(void)
{
	BiFmtTermFdlg();
	MMI_Close() ;
	TL_free(BiVirtualVram);
}

/*link*/void BiFmtSetExtWindowFunc(int (*f)(int,int,int,EVENT *))
{
	BiFmtExtWindowFuncSw=BI_ON;
	BiFmtExtWindowFunc=f;
}

/*link*/int BiFmtKeyToBikey(int key)
{
	switch(key)
	{
	default:
		return BiAscToKc[key & 0xff];
	case 0x8001:
		return BIKEY_F1;
	case 0x8002:
		return BIKEY_F2;
	case 0x8003:
		return BIKEY_F3;
	case 0x8004:
		return BIKEY_F4;
	case 0x8005:
		return BIKEY_F5;
	case 0x8006:
		return BIKEY_F6;
	case 0x8007:
		return BIKEY_F7;
	case 0x8008:
		return BIKEY_F8;
	case 0x8009:
		return BIKEY_F9;
	case 0x800A:
		return BIKEY_F10;
	}
}



static void BiFmtSetCurrentDirectory(void)
{
	int app,stl;
	char dir[128];

	app=MMI_GetApliId();
	MMI_CallMessage(app,GM_QUERYID,QM_DIRECTRY,(int)dir);

	stl=strlen(dir);
	if(stl>0 && dir[stl-1]=='\\')
	{
		dir[stl-1]=0;
	}
	_chdir(dir);
}



/*****************************************************************************

   File Name

*****************************************************************************/
void BiConstrainFileName(char neo[],char org[])
{
	int i;

	if(neo!=org)
	{
		strcpy(neo,org);
	}

	for(i=0; neo[i]!=0; i++)
	{
		if((i!=1 && neo[i]==':') || neo[i]=='\\')
		{
			neo[i]='/';
		}
	}
}
