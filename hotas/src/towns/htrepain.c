#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>


#include "hotas.h"
#include "htfmt.h"
#include <impulse.h>


static int ConstrainClipFrame(FRAME *fr);

void HtFmtRepaintWindow(int WinId)
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
	if(ConstrainClipFrame(&frm)!=HT_TRUE)
	{
		return;
	}

	clip=WIN_getClipMemory(&frm,NULL);
	visi=WIN_copyClip(clip);
	if(clip!=NULL && visi!=NULL)
	{
		WIN_pushVisible(visi,&visiStk);
		WIN_pushClip(clip,&clipStk);

		MMI_SendMessage(WinId,MM_UPDATE,1,clip);

		WIN_popClip(clipStk);
		WIN_popVisible(visiStk);
	}

	MG_PopOrigin(&orgStk);
}



void HtFmtRepaintWindowUserArea(int WinId)
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
	if(ConstrainClipFrame(&frm)!=HT_TRUE)
	{
		return;
	}

	clip=WIN_getClipMemory(&frm,NULL);
	visi=WIN_copyClip(clip);
	if(clip!=NULL && visi!=NULL)
	{
		WIN_pushVisible(visi,&visiStk);
		WIN_pushClip(clip,&clipStk);

		MMI_SendMessage(WinId,MM_UPDATE,1,clip);

		WIN_popClip(clipStk);
		WIN_popVisible(visiStk);
	}

	MG_PopOrigin(&orgStk);
}




void HtFmtExecAlertObj(int alertObj,int basobj)
{
	POINT org,orgStk;
	WINCLIP *visi,*visiStk;
	WINCLIP *clip,*clipStk;
	WINCTRL *pctrl;
	FRAME frm;

	org.x=0;
	org.y=0;
	MG_PushOrigin(&org,&orgStk);

	MMI_GetControl(&pctrl);
	frm = pctrl->bound;
	if(ConstrainClipFrame(&frm)!=HT_TRUE)
	{
		return;
	}

	clip=WIN_getClipMemory(&frm,NULL);
	visi=WIN_copyClip(clip);
	if(clip!=NULL && visi!=NULL)
	{
		WIN_pushVisible(visi,&visiStk);
		WIN_pushClip(clip,&clipStk);

		MMI_SendMessage(alertObj, MM_ATTACH, 1 ,basobj);
		MMI_SendMessage(alertObj, MM_SHOW, 0);
		MMI_ExecSystem();
		MMI_SendMessage(alertObj, MM_ERASE, 0);
		MMI_SendMessage(alertObj, MM_DETACH, 0);

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
		return HT_FALSE;
	}

	fr->lupx = BiLarger(fr->lupx, bnd.lupx);
	fr->lupy = BiLarger(fr->lupy, bnd.lupy);
	fr->rdwx = BiSmaller(fr->rdwx, bnd.rdwx);
	fr->rdwy = BiSmaller(fr->rdwy, bnd.rdwy);

	return HT_TRUE;
}
