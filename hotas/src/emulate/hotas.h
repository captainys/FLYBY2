#ifndef __HOTAS_H /* { */
#define __HOTAS_H


#ifdef __cplusplus
	extern "C" {
#endif


#include <impulse.h>


enum {
	HT_FALSE,
	HT_TRUE
};

enum {
	HT_NO,
	HT_YES
};

enum {
	HT_OFF,
	HT_ON
};

typedef struct {
	int mx,my;
	int lbt,mbt,rbt;
} HTMOUSE;

typedef struct {
	int shift,ctrl;
} HTSHIFT;



void HtInitialize(void);
void HtOpenWindow(long sizx,long sizy);
void HtStartSetMenu(char *tit[]);
void HtSetMenuItem(int n,char *str,char key,void (*f)(void));
void HtEndSetMenu(void);
void HtStartSetShortCut(void);
void HtSetShortCut(int key,void (*func)(void));
void HtEndSetShortCut(void);
void HtQuitProgram(void);
void HtEventLoop(void);

void HtSetIntervalFunc(void (*func)(void));
void HtResetIntervalFunc(void);


void HtSetRepaintFunc(void (*f)(void));

void HtSetOnLeftButtonFunc(void (*f)(int trig,HTMOUSE *mos));
void HtResetOnLeftButtonFunc(void);
void HtSetOnMiddleButtonFunc(void (*f)(int trig,HTMOUSE *mos));
void HtResetOnMiddleButtonFunc(void);
void HtSetOnRightButtonFunc(void (*f)(int trig,HTMOUSE *mos));
void HtResetOnRightButtonFunc(void);
void HtSetOnDragFunc(void (*f)(HTMOUSE *mos));
void HtResetOnDragFunc(void);
void HtGetShift(HTSHIFT *sft);


void HtMessageBox(char *msg,char *btn);
void HtMessageBox3(char *msg[3],char *btn);
int HtLineInput(char *str,char *msg,char *def);
int HtYesNo(char *msg,char *yes,char *no);
int HtYesNo3(char *msg[3],char *yes,char *no);
int HtInputNumber(real *n,real dflt,char *msg,char *yes,char *no);
int HtInputInteger(int *n,int dflt,char *msg,char *yes,char *no);
int HtInputPoint2(BIPOINT2 *p,BIPOINT2 *df,char *msg,char *y,char *n);
int HtInputPoint2i(int p[2],int df[2],char *msg,char *y,char *n);
int HtInputPoint3(BIPOINT *p,BIPOINT *df,char *msg,char *y,char *n);
int HtInputPoint3i(int p[3],int df[3],char *msg,char *y,char *n);
int HtInputAngle3(BIANGLE *a,BIANGLE *df,char *msg,char *y,char *n);
int HtLoadFileDialog(char *fn,char *msg,char *df,char *ext);
int HtSaveFileDialog(char *fn,char *msg,char *df,char *ext);
int HtColorDialog(BICOLOR *col,BICOLOR *df,char *msg);
int HtListBox(int *n,char *str[],char *msg,char *yes,char *no);
int HtListBoxPl(int *nm,int n[],char *str[],char *msg,char *yes,char *no);

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* } __HOTAS_H */
