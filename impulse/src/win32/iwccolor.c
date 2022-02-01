#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../impulse.h"

#include <windowsx.h>
#include <wing.h>
#include "iwildcat.h"


/*
  WcColorHSVtoGRB(&g,&r,&b,h,s,v);
  WcColorGRBtoHSV(&h,&s,&v,g,r,b);

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

void WcColorHSVtoGRB(int *g,int *r,int *b,int h,int s,int v)
{
	long rateArea,rateVal,gRate,rRate,bRate;
	long white;
	long eff,bas,fre;

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

	white = (long)SATURATION_MAX-s;
	eff = (long)GRB_MAX *v/VAUE_MAX;
	bas = (long)eff *white/SATURATION_MAX;
	fre = (long)eff *s/SATURATION_MAX;

	*g = (int)(bas  +fre*gRate/HUE_UNIT);
	*r = (int)(bas  +fre*rRate/HUE_UNIT);
	*b = (int)(bas  +fre*bRate/HUE_UNIT);
}

void WcColorGRBtoHSV(int *h,int *s,int *v,int g,int r,int b)
{
	long hue,sat,vau,white;
	long gRate,rRate,bRate;

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
	*h = (int)(hue *HUE_MAX/1536);
	*s = (int)(sat *SATURATION_MAX/GRB_MAX);
	*v = (int)(vau *VAUE_MAX/GRB_MAX);
}

static void ColorErrorEnd(int *a,int *b,int *c)
{
	*a=0;
	*b=0;
	*c=0;
}
