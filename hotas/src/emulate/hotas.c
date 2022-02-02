#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "hotas.h"


#ifdef WIN32
#include <windows.h>
int HtWindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	return BI_ERR;
}
#endif


typedef struct HtShortCutKeyTag {
    struct HtShortCutKeyTag *next;
    int bikey;
    void (*f)(void);
} HTSHORTCUTKEY;

typedef struct HtPullDownMenuTag {
    struct HtPullDownMenuTag *next;
    int key;
    void (*f)(void);
    char str[128];
} HTPULLDOWNMENU;

typedef struct HtMenuBarItemTag {
    char str[128];
    HTPULLDOWNMENU *menu;
} HTMENUBARITEM;


static void (*HtRepaintFunc)(void);
static void (*HtLButton)(int trg,HTMOUSE *mos);
static void (*HtMButton)(int trg,HTMOUSE *mos);
static void (*HtRButton)(int trg,HTMOUSE *mos);
static void (*HtDrag)(HTMOUSE *mos);
static void (*HtIntervalFunc)(void);

static void HtButtonDummyFunc(int trg,HTMOUSE *mos){}
static void HtDragDummyFunc(HTMOUSE *mos){}
static void HtDummyVoidFunc(void){}



static HTSHORTCUTKEY *sKeyTab=NULL;
static int nMBarTab;
static HTMENUBARITEM *mBarTab=NULL;



void HtInitialize(void)
{
        HtRepaintFunc=HtDummyVoidFunc;
        HtIntervalFunc=HtDummyVoidFunc;
    HtLButton=HtButtonDummyFunc;
    HtMButton=HtButtonDummyFunc;
    HtRButton=HtButtonDummyFunc;
    HtDrag=HtDragDummyFunc;

    sKeyTab=NULL;
    mBarTab=NULL;
}

void HtOpenWindow(long sizx,long sizy)
{
    BiOpenWindow(sizx,sizy);
}


/***************************************************************************/

void HtStartSetMenu(char *tit[])
{
    int i,nTit;

    for(nTit=0; tit[nTit]!=NULL; nTit++);

    nMBarTab=nTit;

    mBarTab=(HTMENUBARITEM *)BiMalloc(nTit*sizeof(HTMENUBARITEM));
    if(mBarTab!=NULL)
    {
        for(i=0; i<nTit; i++)
        {
            mBarTab[i].menu=NULL;
            strcpy(mBarTab[i].str,tit[i]);
        }
    }
    else
    {
        printf("Can't allocate memory for Pull Down Menu.\n");
        exit(1);
    }
}

void HtSetMenuItem(int n,char *str,char key,void (*f)(void))
{
    HTPULLDOWNMENU *neo,*btm;
    if(0<n && n<=nMBarTab)
    {
        neo=(HTPULLDOWNMENU *)BiMalloc(sizeof(HTPULLDOWNMENU));
        if(neo!=NULL)
        {
            strcpy(neo->str,str);
            neo->f=f;
            neo->key=key;
            neo->next=NULL;
            if(mBarTab[n-1].menu==NULL)
            {
                mBarTab[n-1].menu=neo;
            }
            else
            {
                for(btm=mBarTab[n-1].menu; btm->next!=NULL; btm=btm->next);
                btm->next=neo;
            }
        }
    }
}

void HtEndSetMenu(void)
{
    /* Nothing To Do */
}


/***************************************************************************/
void HtStartSetShortCut(void)
{
    /* Nothing To Do */
}

void HtSetShortCut(int key,void (*func)(void))
{
    HTSHORTCUTKEY *neo;
    neo=(HTSHORTCUTKEY *)BiMalloc(sizeof(HTSHORTCUTKEY));
    neo->bikey=key;
    neo->f=func;
    neo->next=sKeyTab;
    sKeyTab=neo;
}

void HtEndSetShortCut(void)
{
    /* Nothing To Do */
}


/***************************************************************************/

static int HaltFlag=BI_OFF;

void HtQuitProgram(void)
{
    HaltFlag=BI_ON;
}

static void HtCheckExecShortCut(int key);
static void HtPrintMenu(void);
static void HtInvokePullDown(void);
static void HtInvokePullDownMenu(HTMENUBARITEM *bar);

void HtEventLoop(void)
{
    int key;
    long pWid,pHei,wid,hei;
    int pLbt,pMbt,pRbt;
    long pMx,pMy;
    int cLbt,cMbt,cRbt;
    long cMx,cMy;
    HTMOUSE mos;

    HtPrintMenu();

    BiUpdateDevice();
    BiMouse(&pLbt,&pMbt,&pRbt,&pMx,&pMy);
    BiGetWindowSize(&pWid,&pHei);
    while(HaltFlag==BI_OFF)
    {
                (*HtIntervalFunc)();

        BiUpdateDevice();
        BiMouse(&cLbt,&cMbt,&cRbt,&cMx,&cMy);
        while((key=BiInkey())!=BIKEY_NULL)
        {
            if(key==BIKEY_ESC)
            {
                HtInvokePullDown();
            }
            else
            {
                HtCheckExecShortCut(key);
            }
        }

        mos.mx=cMx;
        mos.my=cMy;
        mos.lbt=cLbt;
        mos.rbt=cRbt;
        mos.mbt=cMbt;

        if(pLbt!=cLbt)
        {
            (*HtLButton)(cLbt,&mos);
        }
        if(pMbt!=cMbt)
        {
            (*HtMButton)(cMbt,&mos);
        }
        if(pRbt!=cRbt)
        {
            (*HtRButton)(cRbt,&mos);
        }
        if((cLbt==BI_ON || cMbt==BI_ON || cRbt==BI_ON) && (cMx!=pMx || cMy!=pMy))
        {
            (*HtDrag)(&mos);
        }

        pLbt=cLbt;
        pMbt=cMbt;
        pRbt=cRbt;
        pMx=cMx;
        pMy=cMy;

                BiGetWindowSize(&wid,&hei);
                if(wid!=pWid || hei!=pHei)
                {
                        (*HtRepaintFunc)();
                        pWid=wid;
                        pHei=hei;
                }
    }

    BiCloseWindow();
}

static void HtCheckExecShortCut(int key)
{
    HTSHORTCUTKEY *ptr;
    for(ptr=sKeyTab; ptr!=NULL; ptr=ptr->next)
    {
        if(key==ptr->bikey)
        {
            (*ptr->f)();
        }
    }
}

static void HtPrintMenu(void)
{
    int i;

    printf("TOP MENU\n");
    for(i=0; i<nMBarTab; i++)
    {
        printf("  ESC+'%d':%s\n",i+1,mBarTab[i].str);
    }
}

static void HtInvokePullDown(void)
{
    int mn,key;

    printf("ESC...\n");
    HtPrintMenu();
    while((key=BiInkey())==BIKEY_NULL)
    {
        BiUpdateDevice();
    }
    mn=key-BIKEY_ONE;
    if(0<=mn && mn<nMBarTab)
    {
        HtInvokePullDownMenu(&mBarTab[mn]);
    }
    printf("\n");
    HtPrintMenu();
}

static void HtInvokePullDownMenu(HTMENUBARITEM *bar)
{
    int i,itm,nItm,key;
    HTPULLDOWNMENU *ptr;

    printf("\n");
    printf("MENU OPTION\n");

    nItm=0;
    for(ptr=bar->menu; ptr!=NULL; ptr=ptr->next)
    {
        printf("  [%c]:%s\n",'A'+nItm,ptr->str);
        nItm++;
    }

    while(1)
    {
        BiUpdateDevice();
		key=BiInkey();
	    itm=key-BIKEY_A;
	    if(0<=itm && itm<nItm)
	    {
	        ptr=bar->menu;
	        for(i=0; i<itm; i++)
	        {
	            ptr=ptr->next;
	        }
	        (*ptr->f)();
	        return;
	    }
	    else if(key==BIKEY_ESC)
	    {
			return;
		}
    }
}



/***************************************************************************/

void HtSetRepaintFunc(void (*f)(void))
{
        HtRepaintFunc=f;
}

void HtSetOnLeftButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
    HtLButton=f;
}

void HtResetOnLeftButtonFunc(void)
{
    HtLButton=HtButtonDummyFunc;
}

void HtSetOnMiddleButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
    HtMButton=f;
}

void HtResetOnMiddleButtonFunc(void)
{
    HtMButton=HtButtonDummyFunc;
}

void HtSetOnRightButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
    HtRButton=f;
}

void HtResetOnRightButtonFunc(void)
{
    HtRButton=HtButtonDummyFunc;
}

void HtSetOnDragFunc(void (*f)(HTMOUSE *mos))
{
    HtDrag=f;
}

void HtResetOnDragFunc(void)
{
    HtDrag=HtDragDummyFunc;
}

void HtSetIntervalFunc(void (*f)(void))
{
        HtIntervalFunc=f;
}

void HtResetIntervalFunc(void)
{
        HtIntervalFunc=HtDummyVoidFunc;
}

/***************************************************************************/

void HtGetShift(HTSHIFT *sft)
{
    BiUpdateDevice();
    sft->shift=BiKey(BIKEY_SHIFT);
    sft->ctrl=BiKey(BIKEY_CTRL);
}

/***************************************************************************/

