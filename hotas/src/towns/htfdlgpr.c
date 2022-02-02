#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>
#include <file_dlg.H>    /*    gui.hよりあとでincludeすること    */


#include "hotas.h"
#include "htfmt.h"

/***** public *****/
void HtFmtInitFdlg(void)
{
	FDG_SaveCurDir();
	FDG_InitFileDlg();
}

void HtFmtTermFdlg(void)
{
	FDG_FreeFileDlg();
	FDG_RecovCurDir();
}


/***** Local *****/
static void HtFmtSetTitle(char *msg);
static int HtFmtExecSingleFdlg(int kobj,char *def,char *ext);
static void HtFmtSaveClipFrame(WINCLIP *(*visi),WINCLIP *(*clip));
static void HtFmtConstrainExtension(char *dst,char *ext);
static void HtFmtSaveOrigine(POINT *org);

int HtFmtFileDialog(char *fn,char *msg,char *df,char *ext)
{
	int r,dmy;

	HtFmtSetTitle(msg);
	r=HtFmtExecSingleFdlg(MMI_GetBaseObj(),df,ext);
	if(r==HT_YES)
	{
		FDG_GetPathName(fn,&dmy,0);
	}
	return r;
}

static void HtFmtSetTitle(char *msg)
{
	msg=(msg!=NULL ? msg : "ファイル選択");
	FDG_SetTitle(msg,"確定","取消");
}

static int HtFmtExecSingleFdlg(int kobj,char *def,char *ext)
{
	int r;
	unsigned nf;
	char extFul[16],*extLst[2],*defPath;
	POINT org;
	WINCLIP *visi,*clip;

	HtFmtConstrainExtension(extFul,ext);
	extLst[0]=extFul;
	extLst[1]=NULL;

	HtFmtSaveOrigine(&org);
	HtFmtSaveClipFrame(&visi,&clip);

	defPath=NULL; /* 将来的に、なんか設定方法を付けよう */
	FDG_SetFileText(def!=NULL ? def : "");

	r=FDG_DspFileDlg(kobj,FDG_TEXT|FDG_ALERT,defPath,extLst,&nf);

	WIN_popClip(clip);
	WIN_popVisible(visi);
	MG_PopOrigin(&org);
	return (r==1 ? HT_YES : HT_NO);
}

static void HtFmtSaveClipFrame(WINCLIP *(*visi),WINCLIP *(*clip))
{
	WINCTRL *pctrl;
	FRAME area;
	WINCLIP *fulscrn,*duplicate;
	MMI_GetControl(&pctrl);
	area = pctrl->bound;

	fulscrn=WIN_getClipMemory(&area,NULL);
	duplicate=WIN_copyClip(fulscrn);
	WIN_pushVisible(duplicate,visi);
	WIN_pushClip(fulscrn,clip);
}

static void HtFmtConstrainExtension(char *dst,char *ext)
{
	if(ext[0]=='*')
	{
		sprintf(dst,"%s",ext);
	}
	else if(ext[0]=='.')
	{
		sprintf(dst,"*%s",ext);
	}
	else
	{
		sprintf(dst,"*.%s",ext);
	}
}

static void HtFmtSaveOrigine(POINT *org)
{
	POINT neo;
	neo.x=0;
	neo.y=0;
	MG_PushOrigin(&neo,org);
}
