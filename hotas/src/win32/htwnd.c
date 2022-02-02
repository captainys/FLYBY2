#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "hotas.h"
#include "resource.h"


/* Blue Impulse Library Interface **************************************/
extern int BiWinKeyCodeToVirtualKey(int bikey);
extern int BiWinVirtualKeyToKeyCode(int vk);
extern void BiWinSetExtraWindowFunc(int (*func)(HWND,UINT,WPARAM,LPARAM));



/***********************************************************************/

static void HtDumb1(int trg,HTMOUSE *mos)
{
}

static void HtDumb2(HTMOUSE *mos)
{
}

static void HtDumb3(void)
{
}

/***********************************************************************/
static int HtUniqId=16384;

int HtGetUniqId(void)
{
	return (HtUniqId++);
}

/* Open Window *********************************************************/
static void HtRegisterWindowClass(char *cl);

extern HWND BiWnd;
extern HMENU BiMenu;

void HtOpenWindow(long sizx,long sizy)
{
	BiOpenWindow(sizx,sizy);
}

/***********************************************************************/
typedef struct {
	HMENU mn;
	char str[64];
} HTMENUTITLE;

static int HtNTitle;
static HTMENUTITLE *HtTitle;

static int HtNumOfMenuTitle(char *tit[]);

void HtStartSetMenu(char *tit[])
{
	int i;

	BiMenu=CreateMenu();

	HtNTitle=HtNumOfMenuTitle(tit);
	HtTitle=(HTMENUTITLE *)malloc(sizeof(HTMENUTITLE)*HtNTitle);
	for(i=0; i<HtNTitle; i++)
	{
		HtTitle[i].mn=CreatePopupMenu();
		strcpy(HtTitle[i].str,tit[i]);
	}
}

static int HtNumOfMenuTitle(char *tit[])
{
	int i;
	for(i=0; tit[i]!=NULL; i++);
	return i;
}

void HtEndSetMenu(void)
{
	int i;
	for(i=0; i<HtNTitle; i++)
	{
		AppendMenu(BiMenu,MF_ENABLED|MF_POPUP,(UINT)HtTitle[i].mn,HtTitle[i].str);
	}
	if(BiWnd!=NULL)
	{
		SetMenu(BiWnd,BiMenu);
		DrawMenuBar(BiWnd);
	}
}

/***********************************************************************/
typedef struct HtMenuItem {
	struct HtMenuItem *next;
	int id,sCutKey;
	void (*func)(void);
} HTMENUITEM;

static HTMENUITEM *HtMnItem=NULL;

void HtSetMenuItem(int n,char *str,char key,void (*f)(void))
{
	HTMENUITEM *neo;
	char tmp[128];
	int id;

	if(1<=n && n<=HtNTitle)
	{
		neo=(HTMENUITEM *)malloc(sizeof(HTMENUITEM));
		if(neo!=NULL)
		{
			id=HtGetUniqId();
			if(key!=0)
			{
				sprintf(tmp,"%s(&%c)",str,key);
			}
			else
			{
				strcpy(tmp,str);
			}
			AppendMenu(HtTitle[n-1].mn,MF_ENABLED|MF_STRING,id,tmp);
			neo->id=id;
			neo->sCutKey=key;
			neo->func=f;
			neo->next=HtMnItem;
			HtMnItem=neo;
		}
	}
}

static void HtExecMenuFunction(int id)
{
	HTMENUITEM *itm;
	for(itm=HtMnItem; itm!=NULL; itm=itm->next)
	{
		if(itm->id==id)
		{
			(*itm->func)();
			return;
		}
	}
	MessageBox(NULL,"Unknown MENU-ID Detected.",NULL,MB_APPLMODAL);
}

/***********************************************************************/
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
	neo=(HTSHORTCUT *)malloc(sizeof(HTSHORTCUT));
	if(neo!=NULL)
	{
		neo->bikey=key;
		neo->func=func;
		neo->next=HtSCut;
		HtSCut=neo;
	}
}

void HtEndSetShortCut(void)
{
}

static void HtExecShortCut(int bikey)
{
	HTSHORTCUT *ptr;
	for(ptr=HtSCut; ptr!=NULL; ptr=ptr->next)
	{
		if(ptr->bikey==bikey)
		{
			(*ptr->func)();
			return;
		}
	}
}

/***********************************************************************/
void HtQuitProgram(void)
{
	PostQuitMessage(0);
	DestroyWindow(BiWnd);
}

/***********************************************************************/
static int HtFatalFlag=HT_OFF;

static void (*HtIntervalFunc)(void)=HtDumb3;

extern int nBiDlgRegistered;
extern HWND BiDlgRegistered[];

void HtEventLoop(void)
{
	MSG msg;
	int i,bikey;

	while(HtFatalFlag==HT_OFF)
	{
		(*HtIntervalFunc)();

		while(HtFatalFlag==HT_OFF)
		{
			if(HtIntervalFunc==HtDumb3)
			{
				if(GetMessage(&msg,0,0,0)!=TRUE)
				{
					break;
				}
			}
			else
			{
				if(PeekMessage(&msg,0,0,0,PM_REMOVE)!=TRUE)
				{
					break;
				}
			}

			if(msg.message==WM_KEYDOWN)
			{
				if(0<=msg.wParam && msg.wParam<0x80)
				{
					bikey=BiWinVirtualKeyToKeyCode(msg.wParam);
					HtExecShortCut(bikey);
				}
			}

			for(i=0; i<nBiDlgRegistered; i++)
			{
				if(IsDialogMessage(BiDlgRegistered[i],&msg)==TRUE)
				{
					goto NODISPATCH;
				}
			}

			if(HtFatalFlag==HT_OFF)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		NODISPATCH:
			;
		}
	}
}

/***********************************************************************/
static void (*HtRepaintFunc)(void)=HtDumb3;

static void (*HtLBtnFunc)(int trg,HTMOUSE *mos)=HtDumb1;
static void (*HtMBtnFunc)(int trg,HTMOUSE *mos)=HtDumb1;
static void (*HtRBtnFunc)(int trg,HTMOUSE *mos)=HtDumb1;
static void (*HtDragFunc)(HTMOUSE *mos)=HtDumb2;

static void HtInitCallBack(void)
{
}

void HtSetRepaintFunc(void (*f)(void))
{
	HtRepaintFunc=f;
}

void HtSetOnLeftButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtLBtnFunc=f;
}

void HtResetOnLeftButtonFunc(void)
{
	HtLBtnFunc=HtDumb1;
}

void HtSetOnMiddleButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtMBtnFunc=f;
}

void HtResetOnMiddleButtonFunc(void)
{
	HtMBtnFunc=HtDumb1;
}

void HtSetOnRightButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtRBtnFunc=f;
}

void HtResetOnRightButtonFunc(void)
{
	HtRBtnFunc=HtDumb1;
}

void HtSetOnDragFunc(void (*f)(HTMOUSE *mos))
{
	HtDragFunc=f;
}

void HtResetOnDragFunc(void)
{
	HtDragFunc=HtDumb2;
}

void HtGetShift(HTSHIFT *sft)
{
	BYTE kst[256];
	GetKeyboardState((LPBYTE)kst);
	sft->shift=((kst[VK_SHIFT] & 0x80)!=0 ? HT_ON : HT_OFF);
	sft->ctrl=((kst[VK_CONTROL] & 0x80)!=0 ? HT_ON : HT_OFF);
}



void HtSetIntervalFunc(void (*func)(void))
{
	HtIntervalFunc=func;
}

void HtResetIntervalFunc(void)
{
	HtIntervalFunc=HtDumb3;
}

/***********************************************************************/
static void HtWparamToButton(int *lbt,int *rbt,WPARAM wPara)
{
	*lbt=((wPara & MK_LBUTTON)!=0 ? BI_ON : BI_OFF);
	*rbt=((wPara & MK_RBUTTON)!=0 ? BI_ON : BI_OFF);
}

static void HtLparamToCoord(int *x,int *y,LPARAM lPara)
{
	*x=LOWORD(lPara);
	*y=HIWORD(lPara);
}

static void HtParamToData(HTMOUSE *mos,WPARAM wp,LPARAM lp)
{
	HtWparamToButton(&mos->lbt,&mos->rbt,wp);
	mos->mbt=HT_OFF;
	HtLparamToCoord(&mos->mx,&mos->my,lp);
}

int HtWindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	HTMOUSE mos;
	switch(msg)
	{
	case WM_COMMAND:
		HtExecMenuFunction(wPara);
		break;
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		SetCapture(w);
		HtParamToData(&mos,wPara,lPara);
		(*HtLBtnFunc)(HT_ON,&mos);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		HtParamToData(&mos,wPara,lPara);
		(*HtLBtnFunc)(HT_OFF,&mos);
		break;
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		SetCapture(w);
		HtParamToData(&mos,wPara,lPara);
		(*HtRBtnFunc)(HT_ON,&mos);
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		HtParamToData(&mos,wPara,lPara);
		(*HtRBtnFunc)(HT_OFF,&mos);
		break;
	case WM_MOUSEMOVE:
		HtParamToData(&mos,wPara,lPara);
		if(mos.lbt==HT_ON || mos.rbt==HT_ON)
		{
			(*HtDragFunc)(&mos);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		HtFatalFlag=1;
		break;
	case WM_QUIT:
		HtFatalFlag=1;
		break;
	case WM_PAINT:
		if(HtRepaintFunc!=HtDumb3)
		{
			(*HtRepaintFunc)();
			DefWindowProc(w,msg,wPara,lPara);
			return BI_OK;
		}
		break;
	}
	return BI_ERR;
}



/***********************************************************************/
void HtInitialize(void)
{
	HtInitCallBack();
	BiWinSetExtraWindowFunc(HtWindowFunc);
}


