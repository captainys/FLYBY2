#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>


#include "hotas.h"
#include "htfmt.h"
#include <impulse.h>


void HtFmtCentering(int kobj)
{
	WINCTRL *pctrl;
	FRAME frm;
	HYPER hyp;
	int wid,hei,cx,cy,neoLupx,neoLupy,dx,dy;

	MMI_GetControl(&pctrl);
	frm = pctrl->bound;
	MMI_SendMessage(kobj,MM_GETHYPER,1,&hyp);

	cx=(frm.rdwx+frm.lupx)/2;
	cy=(frm.rdwy+frm.lupy)/2;
	wid=hyp.fr.rdwx-hyp.fr.lupx+1;
	hei=hyp.fr.rdwy-hyp.fr.lupy+1;

	neoLupx = cx-wid/2;
	neoLupy = cy-hei/2;

	dx=neoLupx -hyp.fr.lupx;
	dy=neoLupy -hyp.fr.lupy;

	MMI_SendMessage(kobj,MM_MOVE,2,dx,dy);
}
