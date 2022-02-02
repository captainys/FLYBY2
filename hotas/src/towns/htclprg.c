#include <stdio.h>
#include <stdlib.h>
#include <winb.h>
#include <te.h>
#include <fntb.h>
#include <gui.h>

#include "hotas.h"
#include "htfmt.h"


int HtColorPanelId = -1 ;
int HtInputColorIconId = -1 ;
int HtColNumId[6] = -1 ;
int HtColBarId[6] = -1 ;
int HtColGrbYesBtnId = -1 ;
int HtColGrbNoBtnId = -1 ;


static int HtColSetIcon(void);
static void HtColSaveIconTable(void);
static void HtColLoadIconTable(void);
static void SetValueByGrb();
static void SetValueByHsv();
static void SetGuiItems();
static void ResetNumBox(int kobj,int var);
static void ResetScrBar(int kobj,int var);
static void ResetColorIcon(int g,int r,int b);
static void ShowGuiItems(void);
static void HtColorHSVtoGRB(int *g,int *r,int *b,int h,int s,int v);
static void HtColorGRBtoHSV(int *h,int *s,int *v,int g,int r,int b);


static int result;
static int g,r,b,h,s,v;
static unsigned short InpColIconPat[2+64*32];


extern MMIINIT initDataHTCLGUS;

int HtColNumberFunc() ;
int HtColBarFunc() ;
int HtColExitFunc() ;




int HtFmtColorDialog(BICOLOR *col,BICOLOR *def,char *msg)
{
	if(HtColorPanelId<0)
	{
		if(MMI_Init(&initDataHTCLGUS)<0)
		{
			BiSetColorRGB(col,0,0,0);
			return HT_NO;
		}
	}

	HtColSaveIconTable();
	HtColSetIcon();

	g=(def!=NULL ? def->g : 0);
	r=(def!=NULL ? def->r : 0);
	b=(def!=NULL ? def->b : 0);
	SetValueByGrb();

	HtFmtExecAlertObj(HtColorPanelId,MMI_GetBaseObj());
	BiSetColorRGB(col,r,g,b);

	HtColLoadIconTable();

	return result;
}


static unsigned char *iconTabOrg[512];
static int iconIdOrg;

static void HtColSaveIconTable(void)
{
	int i;
	unsigned char *(*tabOrg);

	MMI_GetIconTable(&tabOrg,&iconIdOrg);
	if(iconIdOrg>511)
	{
		for(i=0; i<=iconIdOrg-512; i++)
		{
			iconTabOrg[i]=tabOrg[i+512];
		}
	}
}

static void HtColLoadIconTable(void)
{
	MMI_SetIconTable(iconTabOrg,iconIdOrg);
}


static int HtColSetIcon(void)
{
	int i;
	static unsigned char *tabNew[1];

	tabNew[0]=(unsigned char *)InpColIconPat;

	InpColIconPat[0]=0x2040;
	InpColIconPat[1]=0x0400;
	for(i=0; i<64*32; i++)
	{
		InpColIconPat[2+i]=0;
	}
	MMI_SetIconTable(tabNew,512);
	MMI_SendMessage(HtInputColorIconId,MM_SETSHAPE,1,512);
	MMI_SendMessage(HtInputColorIconId,MM_SHOW,0);
	return HT_TRUE;
}


int	HtColNumberFunc(int kobj)
{
	int trig;
	int min,max,del,col;

	for(trig=0; trig<6; trig++)
	{
		if(kobj==HtColNumId[trig])
		{
			switch(trig)
			{
			case 0:
				MMI_SendMessage(kobj,MM_GETNUMBOX,5,&h,&min,&max,&del,&col);
				SetValueByHsv();
				break;
			case 1:
				MMI_SendMessage(kobj,MM_GETNUMBOX,5,&s,&min,&max,&del,&col);
				SetValueByHsv();
				break;
			case 2:
				MMI_SendMessage(kobj,MM_GETNUMBOX,5,&v,&min,&max,&del,&col);
				SetValueByHsv();
				break;
			case 3:
				MMI_SendMessage(kobj,MM_GETNUMBOX,5,&g,&min,&max,&del,&col);
				SetValueByGrb();
				break;
			case 4:
				MMI_SendMessage(kobj,MM_GETNUMBOX,5,&r,&min,&max,&del,&col);
				SetValueByGrb();
				break;
			case 5:
				MMI_SendMessage(kobj,MM_GETNUMBOX,5,&b,&min,&max,&del,&col);
				SetValueByGrb();
				break;
			}
			ShowGuiItems();
			break;
		}
	}
	return NOERR ;
}

int	HtColBarFunc(int kobj)
{
	int trig;
	int min,max,len,pag;

	for(trig=0; trig<6; trig++)
	{
		if(kobj==HtColBarId[trig])
		{
			switch(trig)
			{
			case 0:
				MMI_SendMessage(kobj,MM_GETSCROLL,5,&h,&min,&max,&len,&pag);
				h-=min;
				SetValueByHsv();
				break;
			case 1:
				MMI_SendMessage(kobj,MM_GETSCROLL,5,&s,&min,&max,&len,&pag);
				s-=min;
				SetValueByHsv();
				break;
			case 2:
				MMI_SendMessage(kobj,MM_GETSCROLL,5,&v,&min,&max,&len,&pag);
				v-=min;
				SetValueByHsv();
				break;
			case 3:
				MMI_SendMessage(kobj,MM_GETSCROLL,5,&g,&min,&max,&len,&pag);
				g-=min;
				SetValueByGrb();
				break;
			case 4:
				MMI_SendMessage(kobj,MM_GETSCROLL,5,&r,&min,&max,&len,&pag);
				r-=min;
				SetValueByGrb();
				break;
			case 5:
				MMI_SendMessage(kobj,MM_GETSCROLL,5,&b,&min,&max,&len,&pag);
				b-=min;
				SetValueByGrb();
				break;
			}
			ShowGuiItems();
			break;
		}
	}
	return NOERR ;
}

int	HtColExitFunc(int kobj)
{
	if(kobj==HtColGrbYesBtnId)
	{
		result=HT_YES;
	}
	else
	{
		result=HT_NO;
	}
	MMI_SetHaltFlag(TRUE);
	return NOERR ;
}






static void SetValueByGrb()
{
	HtColorGRBtoHSV(&h,&s,&v,g,r,b);
	SetGuiItems();
}

static void SetValueByHsv()
{
	HtColorHSVtoGRB(&g,&r,&b,h,s,v);
	SetGuiItems();
}

static void SetGuiItems()
{
	ResetNumBox(HtColNumId[0],h);
	ResetNumBox(HtColNumId[1],s);
	ResetNumBox(HtColNumId[2],v);
	ResetNumBox(HtColNumId[3],g);
	ResetNumBox(HtColNumId[4],r);
	ResetNumBox(HtColNumId[5],b);

	ResetScrBar(HtColBarId[0],h);
	ResetScrBar(HtColBarId[1],s);
	ResetScrBar(HtColBarId[2],v);
	ResetScrBar(HtColBarId[3],g);
	ResetScrBar(HtColBarId[4],r);
	ResetScrBar(HtColBarId[5],b);

	ResetColorIcon(g,r,b);
}

static void ResetNumBox(int kobj,int var)
{
	int dmy,min,max,del,col;
	MMI_SendMessage(kobj,MM_GETNUMBOX,5,&dmy,&min,&max,&del,&col);
	MMI_SendMessage(kobj,MM_SETNUMBOX,5,var,min,max,del,col);
}

static void ResetScrBar(int kobj,int var)
{
	int dmy,min,max,len,pag;
	MMI_SendMessage(kobj,MM_GETSCROLL,5,&dmy,&min,&max,&len,&pag);
	var+=min;
	MMI_SendMessage(kobj,MM_SETSCROLL,5,var,min,max,len,pag);
}

static void ResetColorIcon(int g,int r,int b)
{
	int i,col;

	col= (g*31/255)*1024+(r*31/255)*32+b*31/255;
	for(i=0; i<64*32; i++)
	{
		InpColIconPat[i+2]=col;
	}
}


static void ShowGuiItems(void)
{
	MMI_SendMessage(HtColNumId[0],MM_SHOW,0);
	MMI_SendMessage(HtColNumId[1],MM_SHOW,0);
	MMI_SendMessage(HtColNumId[2],MM_SHOW,0);
	MMI_SendMessage(HtColNumId[3],MM_SHOW,0);
	MMI_SendMessage(HtColNumId[4],MM_SHOW,0);
	MMI_SendMessage(HtColNumId[5],MM_SHOW,0);

	MMI_SendMessage(HtColBarId[0],MM_SHOW,0);
	MMI_SendMessage(HtColBarId[1],MM_SHOW,0);
	MMI_SendMessage(HtColBarId[2],MM_SHOW,0);
	MMI_SendMessage(HtColBarId[3],MM_SHOW,0);
	MMI_SendMessage(HtColBarId[4],MM_SHOW,0);
	MMI_SendMessage(HtColBarId[5],MM_SHOW,0);

	MMI_SendMessage(HtInputColorIconId,MM_SHOW,0);
}



/* Following Functions can be recycled in any ANSI-C program. */
/*
  HtColorHSVtoGRB(&g,&r,&b,h,s,v);
  HtColorGRBtoHSV(&h,&s,&v,g,r,b);

    h:0`1535
    s:0`255
    v:0`255
	g:0`255
	r:0`255
	b:0`255
*/

static void ColorErrorEnd(int *a,int *b,int *c);

#define HUE_MAX        1535
#define SATURATION_MAX  255
#define VAUE_MAX        255
#define GRB_MAX         255

#define HUE_UNIT        256  /* HUE 1‹æŠÔ */

/* Hue        F‘Š */
/* Saturation Ê“x */
/* Vaue       –¾“x */

static void HtColorHSVtoGRB(int *g,int *r,int *b,int h,int s,int v)
{
	int rateArea,rateVal,gRate,rRate,bRate;
	int white;
	int eff,bas,fre;

	h %= (HUE_MAX+1);
	if(s>SATURATION_MAX)s=SATURATION_MAX;
	if(s<0)s=0;
	if(v>VAUE_MAX)v=VAUE_MAX;
	if(v<0)v=0;


	/*
	  HUE (1‹æŠÔ‚Ì’·‚³‚ª HUE_UNIT)
	  0                                                   HUE_MAX
	  |       |        |        |        |        |        |

      |    ^~|~~~~~~~~|~~~~~~~~|~_     |        |        |
	G 0  ^  256      256      256  _   0        0        |
	  |^@@ |        |        |     __|________|________|

      |       |        |        |        |        |        |
      |~~~~~~~|~_     |        |        |     ^~|~~~~~~~~|
    R 256    256  _   0        0        0   ^  256       |
      |       |     __|________|________|_^     |        |

      |       |        |    ^~~|~~~~~~~~|~~~~~~~~|~_     |
    B 0       0        0  ^   256      256      256  _   |
      |_______|________|^      |        |        |     __|
	*/

	rateArea = h /HUE_UNIT;
	rateVal  = h %HUE_UNIT;
	switch(rateArea)
	{
	default:
		ColorErrorEnd(g,r,b);
		return;
	case 0:
		gRate = rateVal;
		rRate = HUE_UNIT;
		bRate = 0;
		break;
	case 1:
		gRate = HUE_UNIT;
		rRate = HUE_UNIT-rateVal;
		bRate = 0;
		break;
	case 2:
		gRate = HUE_UNIT;
		rRate = 0;
		bRate = rateVal;
		break;
	case 3:
		gRate = HUE_UNIT-rateVal;
		rRate = 0;
		bRate = HUE_UNIT;
		break;
	case 4:
		gRate = 0;
		rRate = rateVal;
		bRate = HUE_UNIT;
		break;
	case 5:
		gRate = 0;
		rRate = HUE_UNIT;
		bRate = HUE_UNIT-rateVal;
		break;
	}

	/*
	   0<-     —LŒø”ÍˆÍ     ->VAUE     255(GRB_MAX)
	   |-----------------------|          |
	   |         eff           |
	   0<-  ”’‚³ ->|<- Ê“x ->VAUE     255(GRB_MAX)
	   |-----------|-----------|          |
	   |    bas    |    fre    |
	*/

	white = SATURATION_MAX-s;
	eff = GRB_MAX *v/VAUE_MAX;
	bas = eff *white/SATURATION_MAX;
	fre = eff *s/SATURATION_MAX;

	*g = bas  +fre*gRate/HUE_UNIT;
	*r = bas  +fre*rRate/HUE_UNIT;
	*b = bas  +fre*bRate/HUE_UNIT;
}

static void HtColorGRBtoHSV(int *h,int *s,int *v,int g,int r,int b)
{
	int hue,sat,vau,white;
	int gRate,rRate,bRate;

	/* vaue : Largest in G & R & B. */
	vau = g;
	if(r>vau)vau=r;
	if(b>vau)vau=b;

	/* saturation : vaue - (Smallest in G & R & B) */
	white = g;
	if(r<white)white=r;
	if(b<white)white=b;
	sat = vau-white;

	/* hue : Calculated with ratio (g-white):(r-white):(b-white) */
	if(sat==0)
	{
		hue=0;
	}
	else
	{
		gRate=(g-white)*256/sat;
		rRate=(r-white)*256/sat;
		bRate=(b-white)*256/sat;
		if(bRate==0)      /*    0<=hue< 512 */
		{
			if(rRate==256)  /* gRate‚ª’P’²‘‰Á‹æŠÔ */
			{
				hue=gRate;
			}
			else            /* rRate‚ª’P’²Œ¸­‹æŠÔ */
			{
				hue=512-rRate;
			}
		}
		else if(rRate==0) /*  512<=hue<1024 */
		{
			if(gRate==256)  /* bRate‚ª’P’²‘‰Á‹æŠÔ */
			{
				hue=512+bRate;
			}
			else            /* gRate‚ª’P’²Œ¸­‹æŠÔ */
			{
				hue=1024-gRate;
			}
		}
		else /* gRate==0     1024<=hue<1536 */
		{
			if(bRate==256)  /* rRate‚ª’P’²‘‰Á‹æŠÔ */
			{
				hue=1024+rRate;
			}
			else            /* bRate‚ª’P’²Œ¸­‹æŠÔ */
			{
				hue=1536-bRate;
			}
		}
	}

	/* Conversion ‚¢‚ç‚È‚¢‚æ‚¤‚Å,”÷–­‚É•K—v(^_^;) */
	*h = hue *HUE_MAX/1536;
	*s = sat *SATURATION_MAX/GRB_MAX;
	*v = vau *VAUE_MAX/GRB_MAX;
}

static void ColorErrorEnd(int *a,int *b,int *c)
{
	*a=0;
	*b=0;
	*c=0;
}
