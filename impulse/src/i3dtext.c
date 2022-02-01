/* FreePC2Ç™ñ≥Ç¢Ç‡ÇÒÇæÇ©ÇÁÅAWireFontÇäJï˙Ç∑ÇÈéËíiÇ™ç°Ç»Ç¢ ^_^;) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "impulse.h"



extern char *BiFoResSurprize[];
extern char *BiFoResDquart[];
extern char *BiFoResSharp[];
extern char *BiFoResDollar[];
extern char *BiFoResPercent[];
extern char *BiFoResAnd[];
extern char *BiFoResSquart[];
extern char *BiFoResLparenth[];
extern char *BiFoResRparenth[];
extern char *BiFoResAstersk[];
extern char *BiFoResPlus[];
extern char *BiFoResComma[];
extern char *BiFoResMinus[];
extern char *BiFoResPeriod[];
extern char *BiFoResSlash[];
extern char *BiFoResZero[];
extern char *BiFoResOne[];
extern char *BiFoResTwo[];
extern char *BiFoResThree[];
extern char *BiFoResFour[];
extern char *BiFoResFive[];
extern char *BiFoResSix[];
extern char *BiFoResSeven[];
extern char *BiFoResEight[];
extern char *BiFoResNine[];
extern char *BiFoResColon[];
extern char *BiFoResSemcolon[];
extern char *BiFoResSmaller[];
extern char *BiFoResLarger[];
extern char *BiFoResEqual[];
extern char *BiFoResQuestion[];
extern char *BiFoResAlpha[];
extern char *BiFoResA[];
extern char *BiFoResB[];
extern char *BiFoResC[];
extern char *BiFoResD[];
extern char *BiFoResE[];
extern char *BiFoResF[];
extern char *BiFoResG[];
extern char *BiFoResH[];
extern char *BiFoResI[];
extern char *BiFoResJ[];
extern char *BiFoResK[];
extern char *BiFoResL[];
extern char *BiFoResM[];
extern char *BiFoResN[];
extern char *BiFoResO[];
extern char *BiFoResP[];
extern char *BiFoResQ[];
extern char *BiFoResR[];
extern char *BiFoResS[];
extern char *BiFoResT[];
extern char *BiFoResU[];
extern char *BiFoResV[];
extern char *BiFoResW[];
extern char *BiFoResX[];
extern char *BiFoResY[];
extern char *BiFoResZ[];
extern char *BiFoResLbracket[];
extern char *BiFoResBslash[];
extern char *BiFoResRbracket[];
extern char *BiFoResUscore[];
extern char *BiFoResUline[];
extern char *BiFoResQuart[];
extern char *BiFoResLbrace[];
extern char *BiFoResSeparate[];
extern char *BiFoResRbrace[];
extern char *BiFoResTline[];



extern char *BiFoRes2Surprize[];
extern char *BiFoRes2Dquote[];
extern char *BiFoRes2Sharp[];
extern char *BiFoRes2Dollar[];
extern char *BiFoRes2Percent[];
extern char *BiFoRes2And[];
extern char *BiFoRes2Quote[];
extern char *BiFoRes2Lparenth[];
extern char *BiFoRes2Rparenth[];
extern char *BiFoRes2Asterisk[];
extern char *BiFoRes2Plus[];
extern char *BiFoRes2Comma[];
extern char *BiFoRes2Minus[];
extern char *BiFoRes2Period[];
extern char *BiFoRes2Slash[];
extern char *BiFoRes2Zero[];
extern char *BiFoRes2One[];
extern char *BiFoRes2Two[];
extern char *BiFoRes2Three[];
extern char *BiFoRes2Four[];
extern char *BiFoRes2Five[];
extern char *BiFoRes2Six[];
extern char *BiFoRes2Seven[];
extern char *BiFoRes2Eight[];
extern char *BiFoRes2Nine[];
extern char *BiFoRes2Colon[];
extern char *BiFoRes2Semicolon[];
extern char *BiFoRes2Smaller[];
extern char *BiFoRes2Equal[];
extern char *BiFoRes2Larger[];
extern char *BiFoRes2Question[];
extern char *BiFoRes2Alpha[];
extern char *BiFoRes2La[];
extern char *BiFoRes2Lb[];
extern char *BiFoRes2Lc[];
extern char *BiFoRes2Ld[];
extern char *BiFoRes2Le[];
extern char *BiFoRes2Lf[];
extern char *BiFoRes2Lg[];
extern char *BiFoRes2Lh[];
extern char *BiFoRes2Li[];
extern char *BiFoRes2Lj[];
extern char *BiFoRes2Lk[];
extern char *BiFoRes2Ll[];
extern char *BiFoRes2Lm[];
extern char *BiFoRes2Ln[];
extern char *BiFoRes2Lo[];
extern char *BiFoRes2Lp[];
extern char *BiFoRes2Lq[];
extern char *BiFoRes2Lr[];
extern char *BiFoRes2Ls[];
extern char *BiFoRes2Lt[];
extern char *BiFoRes2Lu[];
extern char *BiFoRes2Lv[];
extern char *BiFoRes2Lw[];
extern char *BiFoRes2Lx[];
extern char *BiFoRes2Ly[];
extern char *BiFoRes2Lz[];
extern char *BiFoRes2Lbraket[];
extern char *BiFoRes2Bslash[];
extern char *BiFoRes2Rbraket[];
extern char *BiFoRes2Uscore[];
extern char *BiFoRes2Uline[];
extern char *BiFoRes2Bquote[];
extern char *BiFoRes2Sa[];
extern char *BiFoRes2Sb[];
extern char *BiFoRes2Sc[];
extern char *BiFoRes2Sd[];
extern char *BiFoRes2Se[];
extern char *BiFoRes2Sf[];
extern char *BiFoRes2Sg[];
extern char *BiFoRes2Sh[];
extern char *BiFoRes2Si[];
extern char *BiFoRes2Sj[];
extern char *BiFoRes2Sk[];
extern char *BiFoRes2Sl[];
extern char *BiFoRes2Sm[];
extern char *BiFoRes2Sn[];
extern char *BiFoRes2So[];
extern char *BiFoRes2Sp[];
extern char *BiFoRes2Sq[];
extern char *BiFoRes2Sr[];
extern char *BiFoRes2Ss[];
extern char *BiFoRes2St[];
extern char *BiFoRes2Su[];
extern char *BiFoRes2Sv[];
extern char *BiFoRes2Sw[];
extern char *BiFoRes2Sx[];
extern char *BiFoRes2Sy[];
extern char *BiFoRes2Sz[];
extern char *BiFoRes2Lbrace[];
extern char *BiFoRes2Separate[];
extern char *BiFoRes2Rbrace[];
extern char *BiFoRes2Tline[];




static BISRF *BiMakeStdFontSrf(int *sta,real w,BICOLOR *c,char *(*dat));

int BiMakeStdSolidFont(BI3DFONT *fnt,real wid,BICOLOR *col)
{
	int i,sta;

	fnt->type=BI3DFONT_SRF;
	fnt->nSrf=BI_N_3DFONT;
	fnt->srf=(void *(*))BiMalloc(sizeof(BISRF *)*BI_N_3DFONT);
	if(fnt->srf!=NULL)
	{
		fnt->wid=wid;
		fnt->hei=wid*1.5F;
		fnt->col=*col;

		for(i=0; i<BI_N_3DFONT; i++)
		{
			fnt->srf[i]=NULL;
		}

		sta=BI_OK;
		fnt->srf[ 33]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSurprize);
		fnt->srf[ 34]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResDquart);
		fnt->srf[ 35]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSharp);
		fnt->srf[ 36]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResDollar);
		fnt->srf[ 37]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResPercent);
		fnt->srf[ 38]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResAnd);
		fnt->srf[ 39]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSquart);
		fnt->srf[ 40]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResLparenth);
		fnt->srf[ 41]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResRparenth);
		fnt->srf[ 42]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResAstersk);
		fnt->srf[ 43]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResPlus);
		fnt->srf[ 44]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResComma);
		fnt->srf[ 45]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResMinus);
		fnt->srf[ 46]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResPeriod);
		fnt->srf[ 47]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSlash);
		fnt->srf[ 48]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResZero);
		fnt->srf[ 49]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResOne);
		fnt->srf[ 50]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResTwo);
		fnt->srf[ 51]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResThree);
		fnt->srf[ 52]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResFour);
		fnt->srf[ 53]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResFive);
		fnt->srf[ 54]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSix);
		fnt->srf[ 55]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSeven);
		fnt->srf[ 56]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResEight);
		fnt->srf[ 57]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResNine);
		fnt->srf[ 58]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResColon);
		fnt->srf[ 59]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSemcolon);
		fnt->srf[ 60]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSmaller);
		fnt->srf[ 61]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResEqual);
		fnt->srf[ 62]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResLarger);
		fnt->srf[ 63]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResQuestion);
		fnt->srf[ 64]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResAlpha);
		fnt->srf[ 65]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResA);
		fnt->srf[ 66]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResB);
		fnt->srf[ 67]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResC);
		fnt->srf[ 68]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResD);
		fnt->srf[ 69]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResE);
		fnt->srf[ 70]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResF);
		fnt->srf[ 71]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResG);
		fnt->srf[ 72]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResH);
		fnt->srf[ 73]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResI);
		fnt->srf[ 74]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResJ);
		fnt->srf[ 75]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResK);
		fnt->srf[ 76]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResL);
		fnt->srf[ 77]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResM);
		fnt->srf[ 78]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResN);
		fnt->srf[ 79]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResO);
		fnt->srf[ 80]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResP);
		fnt->srf[ 81]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResQ);
		fnt->srf[ 82]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResR);
		fnt->srf[ 83]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResS);
		fnt->srf[ 84]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResT);
		fnt->srf[ 85]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResU);
		fnt->srf[ 86]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResV);
		fnt->srf[ 87]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResW);
		fnt->srf[ 88]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResX);
		fnt->srf[ 89]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResY);
		fnt->srf[ 90]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResZ);
		fnt->srf[ 91]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResLbracket);
		fnt->srf[ 92]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResBslash);
		fnt->srf[ 93]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResRbracket);
		fnt->srf[ 94]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResUscore);
		fnt->srf[ 95]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResUline);
		fnt->srf[ 96]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResQuart);
		fnt->srf[ 97]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResA);
		fnt->srf[ 98]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResB);
		fnt->srf[ 99]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResC);
		fnt->srf[100]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResD);
		fnt->srf[101]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResE);
		fnt->srf[102]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResF);
		fnt->srf[103]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResG);
		fnt->srf[104]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResH);
		fnt->srf[105]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResI);
		fnt->srf[106]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResJ);
		fnt->srf[107]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResK);
		fnt->srf[108]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResL);
		fnt->srf[109]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResM);
		fnt->srf[110]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResN);
		fnt->srf[111]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResO);
		fnt->srf[112]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResP);
		fnt->srf[113]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResQ);
		fnt->srf[114]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResR);
		fnt->srf[115]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResS);
		fnt->srf[116]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResT);
		fnt->srf[117]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResU);
		fnt->srf[118]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResV);
		fnt->srf[119]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResW);
		fnt->srf[120]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResX);
		fnt->srf[121]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResY);
		fnt->srf[122]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResZ);
		fnt->srf[123]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResLbrace);
		fnt->srf[124]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResSeparate);
		fnt->srf[125]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResRbrace);
		fnt->srf[126]=(void *)BiMakeStdFontSrf(&sta,wid,col,BiFoResTline);
		return BI_OK;
	}
	return BI_ERR;
}

static BISRF *BiMakeStdFontSrf(int *sta,real w,BICOLOR *c,char *(*dat))
{
	int i;
	BISRF *neo;
	if(*sta==BI_OK)
	{
		neo=(BISRF *)BiMalloc(sizeof(BISRF));
		if(neo!=NULL)
		{
			if(BiLoadSrfFromString(neo,dat)==BI_OK)
			{
				BiScaleSrf(neo,w/8.5F);
				for(i=0; i<neo->np; i++)
				{
					neo->p[i].col=*c;
				}
				return neo;
			}
		}
	}
	*sta=BI_ERR;
	return NULL;
}





static BIPC2 *BiMakeStdFontPc2(int *sta,real w,BICOLOR *c,char *(*dat));

int BiMakeStdWireFont(BI3DFONT *fnt,real wid,BICOLOR *col)
{
	int i,sta;

	fnt->type=BI3DFONT_PC2;
	fnt->nSrf=BI_N_3DFONT;
	fnt->srf=(void *(*))BiMalloc(sizeof(BIPC2 *)*BI_N_3DFONT);
	if(fnt->srf!=NULL)
	{
		fnt->wid=wid;
		fnt->hei=wid*1.5F;
		fnt->col=*col;

		for(i=0; i<BI_N_3DFONT; i++)
		{
			fnt->srf[i]=NULL;
		}

		sta=BI_OK;
		fnt->srf[ 33]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Surprize);
		fnt->srf[ 34]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Dquote);
		fnt->srf[ 35]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sharp);
		fnt->srf[ 36]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Dollar);
		fnt->srf[ 37]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Percent);
		fnt->srf[ 38]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2And);
		fnt->srf[ 39]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Quote);
		fnt->srf[ 40]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lparenth);
		fnt->srf[ 41]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Rparenth);
		fnt->srf[ 42]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Asterisk);
		fnt->srf[ 43]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Plus);
		fnt->srf[ 44]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Comma);
		fnt->srf[ 45]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Minus);
		fnt->srf[ 46]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Period);
		fnt->srf[ 47]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Slash);
		fnt->srf[ 48]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Zero);
		fnt->srf[ 49]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2One);
		fnt->srf[ 50]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Two);
		fnt->srf[ 51]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Three);
		fnt->srf[ 52]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Four);
		fnt->srf[ 53]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Five);
		fnt->srf[ 54]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Six);
		fnt->srf[ 55]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Seven);
		fnt->srf[ 56]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Eight);
		fnt->srf[ 57]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Nine);
		fnt->srf[ 58]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Colon);
		fnt->srf[ 59]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Semicolon);
		fnt->srf[ 60]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Smaller);
		fnt->srf[ 61]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Equal);
		fnt->srf[ 62]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Larger);
		fnt->srf[ 63]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Question);
		fnt->srf[ 64]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Alpha);
		fnt->srf[ 65]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2La);
		fnt->srf[ 66]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lb);
		fnt->srf[ 67]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lc);
		fnt->srf[ 68]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Ld);
		fnt->srf[ 69]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Le);
		fnt->srf[ 70]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lf);
		fnt->srf[ 71]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lg);
		fnt->srf[ 72]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lh);
		fnt->srf[ 73]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Li);
		fnt->srf[ 74]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lj);
		fnt->srf[ 75]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lk);
		fnt->srf[ 76]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Ll);
		fnt->srf[ 77]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lm);
		fnt->srf[ 78]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Ln);
		fnt->srf[ 79]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lo);
		fnt->srf[ 80]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lp);
		fnt->srf[ 81]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lq);
		fnt->srf[ 82]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lr);
		fnt->srf[ 83]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Ls);
		fnt->srf[ 84]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lt);
		fnt->srf[ 85]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lu);
		fnt->srf[ 86]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lv);
		fnt->srf[ 87]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lw);
		fnt->srf[ 88]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lx);
		fnt->srf[ 89]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Ly);
		fnt->srf[ 90]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lz);
		fnt->srf[ 91]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lbraket);
		fnt->srf[ 92]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Bslash);
		fnt->srf[ 93]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Rbraket);
		fnt->srf[ 94]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Uscore);
		fnt->srf[ 95]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Uline);
		fnt->srf[ 96]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Bquote);
		fnt->srf[ 97]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sa);
		fnt->srf[ 98]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sb);
		fnt->srf[ 99]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sc);
		fnt->srf[100]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sd);
		fnt->srf[101]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Se);
		fnt->srf[102]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sf);
		fnt->srf[103]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sg);
		fnt->srf[104]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sh);
		fnt->srf[105]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Si);
		fnt->srf[106]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sj);
		fnt->srf[107]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sk);
		fnt->srf[108]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sl);
		fnt->srf[109]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sm);
		fnt->srf[110]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sn);
		fnt->srf[111]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2So);
		fnt->srf[112]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sp);
		fnt->srf[113]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sq);
		fnt->srf[114]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sr);
		fnt->srf[115]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Ss);
		fnt->srf[116]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2St);
		fnt->srf[117]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Su);
		fnt->srf[118]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sv);
		fnt->srf[119]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sw);
		fnt->srf[120]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sx);
		fnt->srf[121]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sy);
		fnt->srf[122]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Sz);
		fnt->srf[123]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Lbrace);
		fnt->srf[124]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Separate);
		fnt->srf[125]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Rbrace);
		fnt->srf[126]=(void *)BiMakeStdFontPc2(&sta,wid,col,BiFoRes2Tline);
		return BI_OK;
	}
	return BI_ERR;
}

static BIPC2 *BiMakeStdFontPc2(int *sta,real w,BICOLOR *c,char *(*dat))
{
	int i,j;
	BIPC2 *neo;
	if(*sta==BI_OK)
	{
		neo=(BIPC2 *)BiMalloc(sizeof(BIPC2));
		if(neo!=NULL)
		{
			if(BiLoadPc2FromString(neo,dat)==BI_OK)
			{
				real scale=w/90.0F;
				BiMulPoint2(&neo->min,&neo->min,scale);
				BiMulPoint2(&neo->max,&neo->max,scale);
				BiMulPoint2(&neo->siz,&neo->siz,scale);
				for(i=0; i<neo->nObj; i++)
				{
					BIPC2NPOINT *npnt;

					neo->obj[i].col=*c;
					BiMulPoint2(&neo->obj[i].cen,&neo->obj[i].cen,scale);
					switch(neo->obj[i].objType)
					{
					case BIP2_PSET:
					case BIP2_POLYLINE:
					case BIP2_LINESEQ:
					case BIP2_POLYG:
						npnt=&neo->obj[i].objUni.npnt;
						for(j=0; j<npnt->n; j++)
						{
							BiMulPoint2(&npnt->p[j],&npnt->p[j],scale);
						}
						break;
					}
				}
				return neo;
			}
		}
	}
	*sta=BI_ERR;
	return NULL;
}





void BiFreeSolidFont(BI3DFONT *fnt)
{
	int i;
	for(i=0; i<fnt->nSrf; i++)
	{
		if(fnt->srf[i]!=NULL)
		{
			switch(fnt->type)
			{
			case BI3DFONT_SRF:
				BiFreeSrf(fnt->srf[i]);
				break;
			case BI3DFONT_PC2:
				break;
			}
			BiFree(fnt->srf[i]);
		}
	}
	BiFree(fnt->srf);
}




static void BiGetStartPoint(BIPOINT *cur,BI3DFONT *fnt,char *str,int h,int v);

void BiInsSolidString(BIPOSATT *pos,BI3DFONT *fnt,char str[],int h,int v)
{
	int i;
	BIAXIS axs;
	BIPOINT cur,vec;
	BIPOSATT let;

	BiGetStartPoint(&cur,fnt,str,h,v);
	BiSetPoint(&vec,fnt->wid,0,0);

	BiMakeAxis(&axs,pos);
	BiConvLtoG(&cur,&cur,&axs);
	BiRotFastLtoG(&vec,&vec,&axs.t);

	let.p=cur;
	switch(fnt->type)
	{
	case BI3DFONT_SRF:
		BiPitchUp(&let.a,&pos->a,0,32768);
		break;
	case BI3DFONT_PC2:
		let.a=pos->a;
		break;
	}
	for(i=0; str[i]!=0; i++)
	{
		if(0<str[i] && str[i]<fnt->nSrf && fnt->srf[str[i]]!=NULL)
		{
			switch(fnt->type)
			{
			case BI3DFONT_SRF:
				BiInsSrf(fnt->srf[str[i]],&let);
				break;
			case BI3DFONT_PC2:
				BiInsPc2(fnt->srf[str[i]],&let);
				break;
			}
		}
		BiAddPoint(&let.p,&let.p,&vec);
	}
}

static void BiGetStartPoint(BIPOINT *cur,BI3DFONT *fnt,char *str,int h,int v)
{
	int sl;
	real wid;
	switch(h)
	{
	default:
	case BIMG_CENTER:
		sl=strlen(str);
		wid=fnt->wid*(real)sl;
		cur->x=-(wid/2.0F)+fnt->wid/2.0F;
		break;
	case BIMG_LEFT:
		cur->x=fnt->wid/2.0F;
		break;
	case BIMG_RIGHT:
		sl=strlen(str);
		wid=fnt->wid*(real)sl;
		cur->x=-wid+fnt->wid/2.0F;
		break;
	}

	switch(v)
	{
	default:
	case BIMG_CENTER:
		cur->y=0.0F;
		break;
	case BIMG_TOP:
		cur->y=-fnt->hei/2.0F;
		break;
	case BIMG_BOTTOM:
		cur->y= fnt->hei/2.0F;
		break;
	}

	cur->z=0.0F;
}


static void BiGetTextStartPoint(BIPOINT *sta,BIPOINT *cr,BI3DFONT *fnt,BIPOSATT *org,char *str[],int h,int v);

void BiInsSolidText(BIPOSATT *pos,BI3DFONT *fnt,char *str[],int h,int v)
{
	int i;
	BIPOSATT tmp;
	BIVECTOR vec;

	tmp.a=pos->a;
	BiGetTextStartPoint(&tmp.p,&vec,fnt,pos,str,h,v);

	for(i=0; str[i]!=NULL; i++)
	{
		BiInsSolidString(&tmp,fnt,str[i],h,v);
		BiAddPoint(&tmp.p,&tmp.p,&vec);
	}
}

static void BiGetTextStartPoint(BIPOINT *sta,BIPOINT *cr,BI3DFONT *fnt,BIPOSATT *org,char *str[],int h,int v)
{
	int tw,th;
	BIVECTOR hv,vv,wid,hei;

	tw=0;
	for(th=0; str[th]!=NULL; th++)
	{
		tw=BiLarger((unsigned)tw,strlen(str[th]));
	}


	BiSetPoint(&hv, fnt->wid/2.0F,0,0);
	BiSetPoint(&vv,0,-fnt->hei/2.0F,0);

	BiRotLtoG(&hv,&hv,&org->a);
	BiRotLtoG(&vv,&vv,&org->a);

	BiMulPoint(&wid,&hv, tw);
	BiMulPoint(&hei,&vv,-th);


	*sta=org->p;

	switch(v)
	{
	case BIMG_CENTER:
		BiAddPoint(sta,sta,&hei);
		BiAddPoint(sta,sta,&vv);
		break;
	case BIMG_TOP:
		break;
	case BIMG_BOTTOM:
		BiAddPoint(sta,sta,&hei);
		BiAddPoint(sta,sta,&hei);
		BiAddPoint(sta,sta,&vv);
		BiAddPoint(sta,sta,&vv);
		break;
	}


	BiMulPoint(cr,&vv,2.0F);
}
