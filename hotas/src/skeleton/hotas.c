#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "hotas.h"


static void HtButtonDummyFunc(int trg,HTMOUSE *mos){}
static void HtDragDummyFunc(HTMOUSE *mos){}
static void HtDummyVoidFunc(void){}

static void (*HtRepaintFunc)(void)=HtDummyVoidFunc;
static void (*HtLButton)(int trg,HTMOUSE *mos)=HtButtonDummyFunc;
static void (*HtMButton)(int trg,HTMOUSE *mos)=HtButtonDummyFunc;
static void (*HtRButton)(int trg,HTMOUSE *mos)=HtButtonDummyFunc;
static void (*HtDrag)(HTMOUSE *mos)=HtDragDummyFunc;
static void (*HtIntervalFunc)(void)=HtDummyVoidFunc;



void HtInitialize(void)
{
}

void HtOpenWindow(long sizx,long sizy)
{
}


/***************************************************************************/
void HtStartSetMenu(char *tit[])
{
}

void HtSetMenuItem(int n,char *str,char key,void (*f)(void))
{
}

void HtEndSetMenu(void)
{
}

/***************************************************************************/
void HtStartSetShortCut(void)
{
}

void HtSetShortCut(int key,void (*func)(void))
{
}

void HtEndSetShortCut(void)
{
}

/***************************************************************************/
void HtQuitProgram(void)
{
}

void HtEventLoop(void)
{
}

/***************************************************************************/
void HtSetRepaintFunc(void (*f)(void))
{
	/* Default */
	HtRepaintFunc=f;
}

void HtSetOnLeftButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	/* Default */
    HtLButton=f;
}

void HtResetOnLeftButtonFunc(void)
{
	/* Default */
    HtLButton=HtButtonDummyFunc;
}

void HtSetOnMiddleButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	/* Default */
    HtRButton=f;
}

void HtResetOnMiddleButtonFunc(void)
{
	/* Default */
    HtRButton=HtButtonDummyFunc;
}

void HtSetOnRightButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	/* Default */
    HtMButton=f;
}

void HtResetOnRightButtonFunc(void)
{
	/* Default */
    HtMButton=HtButtonDummyFunc;
}

void HtSetOnDragFunc(void (*f)(HTMOUSE *mos))
{
	/* Default */
    HtDrag=f;
}

void HtResetOnDragFunc(void)
{
	/* Default */
    HtDrag=HtDragDummyFunc;
}

void HtSetIntervalFunc(void (*f)(void))
{
	/* Default */
	HtIntervalFunc=f;
}

void HtResetIntervalFunc(void)
{
	/* Default */
	HtIntervalFunc=HtDummyVoidFunc;
}

/***************************************************************************/
void HtGetShift(HTSHIFT *sft)
{
}

/***************************************************************************/
void HtMessageBox(char *msg,char *btn)
{
}

void HtMessageBox3(char *msg[3],char *btn)
{
}

int HtLineInput(char *str,char *msg,char *def)
{
}

int HtYesNo(char *msg,char *yes,char *no)
{
}

int HtYesNo3(char *msg[3],char *yes,char *no)
{
}

int HtInputNumber(real *n,real dflt,char *msg,char *yes,char *no)
{
}

int HtInputInteger(int *n,int dflt,char *msg,char *yes,char *no)
{
}

int HtInputPoint2(BIPOINT2 *p,BIPOINT2 *df,char *msg,char *y,char *n)
{
}

int HtInputPoint2i(int p[2],int df[2],char *msg,char *y,char *n)
{
}

int HtInputPoint3(BIPOINT *p,BIPOINT *df,char *msg,char *y,char *n)
{
}

int HtInputPoint3i(int p[3],int df[3],char *msg,char *y,char *n)
{
}

int HtInputAngle3(BIANGLE *a,BIANGLE *df,char *msg,char *y,char *n)
{
}

int HtLoadFileDialog(char *fn,char *msg,char *df,char *ext)
{
}

int HtSaveFileDialog(char *fn,char *msg,char *df,char *ext)
{
}

int HtColorDialog(BICOLOR *col,BICOLOR *df,char *msg)
{
}

int HtListBox(int *n,char *lst[],char *msg,char *yes,char *no)
{
}

int HtListBoxPl(int *nm,int n[],char *lst[],char *msg,char *yes,char *no)
{
}

