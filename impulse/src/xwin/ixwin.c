#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../impulse.h"

/* ************************ Include file for X Window ********************** */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>
/* *******************************  BiFatal ******************************** */
static int BiFatalFlag=BI_OFF;
/*depend*/int BiFatal( void )
{
  return BiFatalFlag;
}

int rex_IO_error( Display *l_d )
{
  BiFatalFlag = BI_ON;
  BiFatal();
}

int rex_error( Display *l_d, XErrorEvent *l_e )
{
  BiFatalFlag = BI_ON;
  BiFatal();
}


/* **************** Global values for drawing to windows ******************* */
extern char BiWindowName[];

static long winX, winY;   /* values to hold the window size */
static Display *rex_d;    /* pointer to display structure */
static XEvent rex_e;      /* X event structure */
static Bool you_meiyou_event_flag_INKEY = 0;
                          /* wether keypress event exist or not... */
static Window rex_w,      /* main window */
              def_root;   /* default root window of the X server */
static GC rex_gc;         /* gc holding current graphic attributes */
static int pltSw;         /* Palette Switch */
static Colormap cmap;     /* for default colormap */
static XColor pal[256];
static unsigned long plt[256];/* for color palet... */
static Pixmap rex_p;
static XFontSet BiRexFontset;
/* ************************************************************************* */

/* *********** prototype definitions for frequet use function ************** */
void BiGetWindowSize( long *, long * );

/* ****************************** BiOpenWindow ***************************** */
static void BiRexCreatePrivatePalette( void );
static void BiCreateHiColorMap( void );
/*depend*/ void BiOpenWindow( long reqX, long reqY )
{
  int sc_x, sc_y, n;
  long int win_center_x, win_center_y;
  XSetWindowAttributes atrb;
  unsigned long plane[1];
  char **m, *def;

  /* create simple window */
  sc_x = DisplayWidth( rex_d, 0 );
  sc_y = DisplayHeight( rex_d, 0 );

  win_center_x = (sc_x/2)-(reqX/2);
  win_center_y = (sc_y/2)-(reqY/2);

  rex_w = XCreateSimpleWindow( rex_d, def_root,
                               win_center_x, win_center_y, reqX, reqY, 2,
                               BlackPixel( rex_d, 0 ),
                               WhitePixel( rex_d, 0 ) );

  /* set the backing store attribute */
  atrb.backing_store = WhenMapped;
  XChangeWindowAttributes( rex_d, rex_w, CWBackingStore, &atrb );

  /* set the window name and icon name */
  XStoreName( rex_d, rex_w, BiWindowName);
  XSetIconName( rex_d, rex_w, BiWindowName);

  /* Map the created window */
  XMapWindow( rex_d, rex_w );

  /* get the default colormap and create private palette */
  if(DefaultDepth(rex_d,0)<16)
  {
	pltSw=BI_ON;
    cmap = XCreateColormap( rex_d, def_root, DefaultVisual(rex_d,0), AllocAll );
    /*  XAllocColorCells( rex_d, cmap, True, plane, 0, plt, 6*6*6 ); */
    BiRexCreatePrivatePalette();

    /* set the colormap to the window */
    XInstallColormap( rex_d, cmap );
    XSetWindowColormap( rex_d, rex_w, cmap );
  }
  else
  {
	pltSw=BI_OFF;
    cmap=DefaultColormap(rex_d,0);
	BiCreateHiColorMap();
	XInstallColormap(rex_d,cmap);
    XSetWindowColormap(rex_d,rex_w,cmap);
  }

  /* set the font set */
  BiRexFontset = XCreateFontSet( rex_d,
                 "-*-*-*-*-*-*-20-20-*-*-*-*-iso8859-1,\
                  -*-*-*-*-*-*-20-20-*-*-*-*-jisx0208.1983-0,\
                  -*-*-*-*-*-*-20-20-*-*-*-*-jisx0201.1976-0",
                  &m, &n, &def );

  /* set window mask */
  XSelectInput(rex_d,rex_w,KeyReleaseMask|KeyPressMask|StructureNotifyMask);

  /* create pixmap */
  BiGetWindowSize( &winX, &winY );
  rex_p = XCreatePixmap( rex_d, rex_w, winX, winY, DefaultDepth( rex_d, 0 ) );
  rex_gc = XCreateGC( rex_d, rex_p, 0, 0 );
  XSetForeground( rex_d, rex_gc, BlackPixel( rex_d, 0 ) );
  XSetBackground( rex_d, rex_gc, BlackPixel( rex_d, 0 ) );
  XFillRectangle( rex_d, rex_p, rex_gc, 0, 0, winX, winY );
  XFlush( rex_d );
}

/* Modify 1995/12/07 */
#define BI_RSVD_PLT 40

static void BiRexCreatePrivatePalette( void )
{
  unsigned long i,r,g,b;
  Colormap dcmap;
  dcmap=DefaultColormap(rex_d,0);
  for(i=0; i<BI_RSVD_PLT; i++)
  {
    pal[i].pixel=i;
    XQueryColor(rex_d,dcmap,&pal[i]);
    XStoreColor(rex_d,cmap,&pal[i]);
  }
  for(i=0; i<6*6*6; i++)
  {
    r=(((i/36)%6)*65535/5);
    g=(((i/ 6)%6)*65535/5);
    b=(( i    %6)*65535/5);
    pal[i+BI_RSVD_PLT].red  =(unsigned short)r;
    pal[i+BI_RSVD_PLT].green=(unsigned short)g;
    pal[i+BI_RSVD_PLT].blue =(unsigned short)b;
    pal[i+BI_RSVD_PLT].flags=DoRed|DoGreen|DoBlue;
    pal[i+BI_RSVD_PLT].pixel=i+BI_RSVD_PLT;
    XStoreColor(rex_d,cmap,&pal[i+BI_RSVD_PLT]);
  }
}

static void BiCreateHiColorMap( void )
{
  unsigned long i,r,g,b;
  for(i=0; i<6*6*6; i++)
  {
    r=(((i/36)%6)*65535/5);
    g=(((i/ 6)%6)*65535/5);
    b=(( i    %6)*65535/5);
    pal[i].red  =(unsigned short)r;
    pal[i].green=(unsigned short)g;
    pal[i].blue =(unsigned short)b;
    pal[i].flags=DoRed|DoGreen|DoBlue;
    if(XAllocColor(rex_d,cmap,&pal[i])==0)
    {
		printf("Error Alloc Color %d\n",i);
		exit(1);
	}
  }
}


/* ****************************** BiCloseWindow **************************** */
/*depend*/ void BiCloseWindow( void )
{
  XCloseDisplay( rex_d );
}

/* **************************** BiGetWindowSize **************************** */
/*depend*/ void BiGetWindowSize( long *wid, long *hei )
{
  Window root;
  int maped_x, maped_y;
  unsigned int border, depth;

  XGetGeometry( rex_d, rex_w, &root, &maped_x, &maped_y,
                (unsigned int *)wid, (unsigned int *)hei, &border, &depth );
  winX = *wid;
  winY = *hei;
}

/* *************************** BiGetStdProjection ************************** */
/*depend*/ void BiGetStdProjection( BIPROJ *prj )
{
  long wid, hei;
  BiGetWindowSize( &wid, &hei );
  prj->lx = wid;
  prj->ly = hei;
  prj->cx = wid/2;
  prj->cy = hei/2;
  prj->magx = (real)wid/1.41421356;
  prj->magy = prj->magx;
  prj->nearz = 0.5;
  prj->farz = 10000.0;
}

/* ***************************** BiSwapBuffers ***************************** */
/*depend*/ void BiSwapBuffers( void )
{
  long int x, y;

  BiGetWindowSize( &x, &y );
  XFlush( rex_d );
  XCopyArea( rex_d, rex_p, rex_w, rex_gc, 0, 0, x, y, 0, 0 );
  XFlush( rex_d );
}


/*****************************************************************************/
/*                                Device                                     */
/*****************************************************************************/
static KeySym BiKeyMap[512];
static int ntab = 0;
typedef struct{
  int bikcode;
  KeySym ks;
}RexKeyMatchTable;

static RexKeyMatchTable table[512];

#define N_KEYBUF 512
#define N_PRSBUF 512
static int nKBuf,KBuf[N_KEYBUF];
static int prsBuf[N_PRSBUF];

void BiDeviceMatchKey( int kcode, KeySym BiRexXKeySym )
{
  BiKeyMap[kcode] = BiRexXKeySym;
  table[ntab].bikcode = kcode;
  table[ntab].ks = BiRexXKeySym;
  ntab++;
}

/* ************************* BiDeviceInitialize **************************** */
/*depend*/ void BiDeviceInitialize( void )
{
	int i;

	nKBuf=0;
	for(i=0; i<N_PRSBUF; i++)
	{
		prsBuf[i]=BI_OFF;
	}

  /* set the error handler */
  XSetIOErrorHandler( rex_IO_error );   /* fatal error about system */
  XSetErrorHandler( rex_error );     /* not fatal error, but can't continue */

  /* open display and set root window */
  rex_d = XOpenDisplay( NULL );
  def_root = DefaultRootWindow( rex_d );

  /* keyboard initialize */
  BiDeviceMatchKey(BIKEY_STOP    ,XK_Cancel );
  BiDeviceMatchKey(BIKEY_F1      ,XK_F1     );
  BiDeviceMatchKey(BIKEY_F2      ,XK_F2     );
  BiDeviceMatchKey(BIKEY_F3      ,XK_F3     );
  BiDeviceMatchKey(BIKEY_F4      ,XK_F4     );
  BiDeviceMatchKey(BIKEY_F5      ,XK_F5     );
  BiDeviceMatchKey(BIKEY_F6      ,XK_F6     );
  BiDeviceMatchKey(BIKEY_F7      ,XK_F7     );
  BiDeviceMatchKey(BIKEY_F8      ,XK_F8     );
  BiDeviceMatchKey(BIKEY_F9      ,XK_F9     );
  BiDeviceMatchKey(BIKEY_F10     ,XK_F10    );
  BiDeviceMatchKey(BIKEY_ONE     ,XK_1      );
  BiDeviceMatchKey(BIKEY_TWO     ,XK_2      );
  BiDeviceMatchKey(BIKEY_THREE   ,XK_3      );
  BiDeviceMatchKey(BIKEY_FOUR    ,XK_4      );
  BiDeviceMatchKey(BIKEY_FIVE    ,XK_5      );
  BiDeviceMatchKey(BIKEY_SIX     ,XK_6      );
  BiDeviceMatchKey(BIKEY_SEVEN   ,XK_7      );
  BiDeviceMatchKey(BIKEY_EIGHT   ,XK_8      );
  BiDeviceMatchKey(BIKEY_NINE    ,XK_9      );
  BiDeviceMatchKey(BIKEY_ZERO    ,XK_0      );
  BiDeviceMatchKey(BIKEY_TEN1    ,XK_KP_1   );
  BiDeviceMatchKey(BIKEY_TEN2    ,XK_KP_2   );
  BiDeviceMatchKey(BIKEY_TEN3    ,XK_KP_3   );
  BiDeviceMatchKey(BIKEY_TEN4    ,XK_KP_4   );
  BiDeviceMatchKey(BIKEY_TEN5    ,XK_KP_5   );
  BiDeviceMatchKey(BIKEY_TEN6    ,XK_KP_6   );
  BiDeviceMatchKey(BIKEY_TEN7    ,XK_KP_7   );
  BiDeviceMatchKey(BIKEY_TEN8    ,XK_KP_8   );
  BiDeviceMatchKey(BIKEY_TEN9    ,XK_KP_9   );
  BiDeviceMatchKey(BIKEY_TEN0    ,XK_KP_0   );
  BiDeviceMatchKey(BIKEY_A       ,XK_A      );
  BiDeviceMatchKey(BIKEY_B       ,XK_B      );
  BiDeviceMatchKey(BIKEY_C       ,XK_C      );
  BiDeviceMatchKey(BIKEY_D       ,XK_D      );
  BiDeviceMatchKey(BIKEY_E       ,XK_E      );
  BiDeviceMatchKey(BIKEY_F       ,XK_F      );
  BiDeviceMatchKey(BIKEY_G       ,XK_G      );
  BiDeviceMatchKey(BIKEY_H       ,XK_H      );
  BiDeviceMatchKey(BIKEY_I       ,XK_I      );
  BiDeviceMatchKey(BIKEY_J       ,XK_J      );
  BiDeviceMatchKey(BIKEY_K       ,XK_K      );
  BiDeviceMatchKey(BIKEY_L       ,XK_L      );
  BiDeviceMatchKey(BIKEY_M       ,XK_M      );
  BiDeviceMatchKey(BIKEY_N       ,XK_N      );
  BiDeviceMatchKey(BIKEY_O       ,XK_O      );
  BiDeviceMatchKey(BIKEY_P       ,XK_P      );
  BiDeviceMatchKey(BIKEY_Q       ,XK_Q      );
  BiDeviceMatchKey(BIKEY_R       ,XK_R      );
  BiDeviceMatchKey(BIKEY_S       ,XK_S      );
  BiDeviceMatchKey(BIKEY_T       ,XK_T      );
  BiDeviceMatchKey(BIKEY_U       ,XK_U      );
  BiDeviceMatchKey(BIKEY_V       ,XK_V      );
  BiDeviceMatchKey(BIKEY_W       ,XK_W      );
  BiDeviceMatchKey(BIKEY_X       ,XK_X      );
  BiDeviceMatchKey(BIKEY_Y       ,XK_Y      );
  BiDeviceMatchKey(BIKEY_Z       ,XK_Z      );
  BiDeviceMatchKey(BIKEY_A       ,XK_a      );
  BiDeviceMatchKey(BIKEY_B       ,XK_b      );
  BiDeviceMatchKey(BIKEY_C       ,XK_c      );
  BiDeviceMatchKey(BIKEY_D       ,XK_d      );
  BiDeviceMatchKey(BIKEY_E       ,XK_e      );
  BiDeviceMatchKey(BIKEY_F       ,XK_f      );
  BiDeviceMatchKey(BIKEY_G       ,XK_g      );
  BiDeviceMatchKey(BIKEY_H       ,XK_h      );
  BiDeviceMatchKey(BIKEY_I       ,XK_i      );
  BiDeviceMatchKey(BIKEY_J       ,XK_j      );
  BiDeviceMatchKey(BIKEY_K       ,XK_k      );
  BiDeviceMatchKey(BIKEY_L       ,XK_l      );
  BiDeviceMatchKey(BIKEY_M       ,XK_m      );
  BiDeviceMatchKey(BIKEY_N       ,XK_n      );
  BiDeviceMatchKey(BIKEY_O       ,XK_o      );
  BiDeviceMatchKey(BIKEY_P       ,XK_p      );
  BiDeviceMatchKey(BIKEY_Q       ,XK_q      );
  BiDeviceMatchKey(BIKEY_R       ,XK_r      );
  BiDeviceMatchKey(BIKEY_S       ,XK_s      );
  BiDeviceMatchKey(BIKEY_T       ,XK_t      );
  BiDeviceMatchKey(BIKEY_U       ,XK_u      );
  BiDeviceMatchKey(BIKEY_V       ,XK_v      );
  BiDeviceMatchKey(BIKEY_W       ,XK_w      );
  BiDeviceMatchKey(BIKEY_X       ,XK_x      );
  BiDeviceMatchKey(BIKEY_Y       ,XK_y      );
  BiDeviceMatchKey(BIKEY_Z       ,XK_z      );
  BiDeviceMatchKey(BIKEY_SPACE   ,XK_space  );
  BiDeviceMatchKey(BIKEY_ESC     ,XK_Escape );
  BiDeviceMatchKey(BIKEY_TAB     ,XK_Tab    );
  BiDeviceMatchKey(BIKEY_CTRL    ,XK_Control_R);
  BiDeviceMatchKey(BIKEY_CTRL    ,XK_Control_L);
  BiDeviceMatchKey(BIKEY_SHIFT   ,XK_Shift_R );
  BiDeviceMatchKey(BIKEY_SHIFT   ,XK_Shift_L );
  BiDeviceMatchKey(BIKEY_ALT     ,XK_Alt_L   );
  BiDeviceMatchKey(BIKEY_ALT     ,XK_Alt_R   );
  BiDeviceMatchKey(BIKEY_BS      ,XK_BackSpace);
  BiDeviceMatchKey(BIKEY_RET     ,XK_Return  );
  BiDeviceMatchKey(BIKEY_HOME    ,XK_Home    );
  BiDeviceMatchKey(BIKEY_DEL     ,XK_Delete  );
  BiDeviceMatchKey(BIKEY_UP      ,XK_Up      );
  BiDeviceMatchKey(BIKEY_DOWN    ,XK_Down    );
  BiDeviceMatchKey(BIKEY_LEFT    ,XK_Left    );
  BiDeviceMatchKey(BIKEY_RIGHT   ,XK_Right   );
  BiDeviceMatchKey(BIKEY_TENMUL  ,XK_asterisk);
  BiDeviceMatchKey(BIKEY_TENDIV  ,XK_slash   );
  BiDeviceMatchKey(BIKEY_TENPLUS ,XK_plus    );
  BiDeviceMatchKey(BIKEY_TENMINUS,XK_minus   );
  BiDeviceMatchKey(BIKEY_TENEQUAL,XK_equal   );
  BiDeviceMatchKey(BIKEY_TENDOT  ,XK_period  );
}

/* *************************** BiUpdateDevice ****************************** */
static int BiRexChangePixmapSize( void );
static int BiXUpdateKeyboard(void);

/*depend*/ void BiUpdateDevice( void )
{
  BiRexChangePixmapSize();
  BiXUpdateKeyboard();
}

#define KeyPressRelease (KeyPressMask|KeyReleaseMask)

static int BiXUpdateKeyboard(void)
{
	int i;
	int bikey;
	char tmp[256];
	KeySym ks;
	XComposeStatus stat;

	while(XCheckWindowEvent(rex_d,rex_w,KeyPressRelease,&rex_e)==True)
	{
		XLookupString((XKeyEvent *)&rex_e,tmp,256,&ks,&stat);
		for(i=0; i<ntab; i++)
		{
			if(ks==table[i].ks)
			{
				bikey=table[i].bikcode;
				switch(rex_e.type)
				{
				case KeyPress:
					KBuf[nKBuf]=bikey;
					nKBuf++;
					prsBuf[bikey]=BI_ON;
					break;
				case KeyRelease:
					prsBuf[bikey]=BI_OFF;
					break;
				}
				break;
			}
		}
	}
/*  int i, n;
  char return_c[256];
  KeySym ks;
  XComposeStatus stat;

  if( XCheckTypedWindowEvent( rex_d, rex_w, KeyPress, &rex_e ) == True ){
    XLookupString( (XKeyEvent *)&rex_e, return_c, 256, &ks, &stat );
    for( i = 0; i<ntab; i++ ){
      if( ks == table[i].ks ){ return( table[i].bikcode ); }
     }
    return( 0 );
   }
  else{
    return( 0 );
   } */
}

static int BiRexChangePixmapSize( void )
{
  XEvent cne;

  if( XCheckTypedWindowEvent( rex_d, rex_w, ConfigureNotify, &cne ) == True ){
    XFreePixmap( rex_d, rex_p );
    BiGetWindowSize( &winX, &winY );
    rex_p = XCreatePixmap( rex_d, rex_w, winX, winY, DefaultDepth(rex_d,0) );
    rex_gc = XCreateGC( rex_d, rex_p, 0, 0 );
    XSetForeground( rex_d, rex_gc, BlackPixel( rex_d, 0 ) );
    XSetBackground( rex_d, rex_gc, BlackPixel( rex_d, 0 ) );
    XFillRectangle( rex_d, rex_p, rex_gc, 0, 0, winX, winY );
    XFlush( rex_d );
   }
}
/* ******************************* BiMouse ********************************* */
/*depend*/ void BiMouse( int *lbt, int *mbt, int *rbt, long *mx, long *my )
{
  Window r, c;
  int x_in_root, y_in_root;
  unsigned int mask;

  *lbt = BI_OFF;
  *mbt = BI_OFF;
  *rbt = BI_OFF;

  XQueryPointer( rex_d, rex_w, &r, &c, &x_in_root, &y_in_root,
                                           (int *)mx, (int *)my, &mask );
  if( mask & Button1Mask ){ *lbt = BI_ON; }
  if( mask & Button2Mask ){ *mbt = BI_ON; }
  if( mask & Button3Mask ){ *rbt = BI_ON; }
}

/* ******************************** BiKey ********************************** */
/*depend*/ int BiKey( int kcode )
{
	return prsBuf[kcode];
/*  int BiRexKs,byt,bit;
  char prs[32];

  XQueryKeymap( rex_d , prs );
  BiRexKs = BiKeyMap[kcode];

  byt=BiRexKs/8;
  bit=(1<<(BiRexKs%8));

  return ((prs[byt]&bit)!=0 ? BI_ON : BI_OFF); */
}

/* ******************************* BiInkey ********************************* */
/*depend*/ int BiInkey( void )
{
	int i,r;
	if(nKBuf>0)
	{
		r=KBuf[0];
		for(i=0; i<nKBuf-1; i++)
		{
			KBuf[i]=KBuf[i+1];
		}
		nKBuf--;
		return r;
	}
	else
	{
		return BIKEY_NULL;
	}

/*  int i, n;
  char return_c[256];
  KeySym ks;
  XComposeStatus stat;

  if( XCheckTypedWindowEvent( rex_d, rex_w, KeyPress, &rex_e ) == True ){
    XLookupString( (XKeyEvent *)&rex_e, return_c, 256, &ks, &stat );
    for( i = 0; i<ntab; i++ ){
      if( ks == table[i].ks ){ return( table[i].bikcode ); }
     }
    return( 0 );
   }
  else{
    return( 0 );
   }  */
}

/* ************************************************************************* */
/*                            Memory Management                              */
/* ************************************************************************* */

void BiMemoryInitialize( void )
{
  /* Nothing to do */
}

void *BiMalloc(size_t uni)
{
  return (void *)malloc(uni);
}

void BiFree( void *ptr )
{
  free( ptr );
}

/* ************************************************************************* */
/*                                   Draw                                    */
/* ************************************************************************* */
unsigned long BiRexReturnPixel( BICOLOR *c )
{
  switch(pltSw)
  {
	case BI_ON:
      return BI_RSVD_PLT+(((c->r)*5/255)*36+((c->g)*5/255)*6+((c->b)*5/255));
    case BI_OFF:
      return pal[(((c->r)*5/255)*36+((c->g)*5/255)*6+((c->b)*5/255))].pixel;
  }
}

/******************** Clipping Transaction by YS11 ************************* */
/** Begin YS11 **/
static BIPOINTS orgLup,orgRdw;
static void BiMacSetClipRect(void)
{
	static BIPOINTS lup;
	static BIPOINTS rdw;
	BiSetPointS(&lup,0,0);
	BiSetPointS(&rdw,winX-1,winY-1);
	BiGetClipScreenFrame(&orgLup,&orgRdw);
	BiSetClipScreenFrame(&lup,&rdw);
}

static void BiMacEndClipRect(void)
{
	BiSetClipScreenFrame(&orgLup,&orgRdw);
}

#define BIINBOUND(a) (0<=(a)->x && (a)->x<winX-1 && 0<=(a)->y && (a)->y<winY-1)

/** End YS11 **/

/* **************************** BiClearScreen ****************************** */
/*depend*/ void BiClearScreenD( void )
{
  XSetForeground( rex_d, rex_gc, BlackPixel( rex_d, 0 ) );
  XFillRectangle( rex_d, rex_p, rex_gc, 0, 0, winX, winY );

    /* Begin YS11 */
    if(BiEpsMode==BI_ON)
	{
		BiEpsClearScreen();
	}
    /* End YS11 */
}

/* ***************************** BiDrawLine2 ******************************* */
/*depend*/ void BiDrawLine2D( BIPOINTS *s1, BIPOINTS *s2, BICOLOR *c )
{
/********************** Clipping Transaction by YS11 *********************** */
/** Begin YS11 **/
	BiMacSetClipRect();
	if(!BIINBOUND(s1) || !BIINBOUND(s2))
	{
		BIPOINTS a1,a2;
		if(BiClipLineScrn(&a1,&a2,s1,s2)==BI_IN)
		{
			s1=&a1;
			s2=&a2;
		}
		else
		{
			goto END;
		}
	}
/** End YS11 **/
  XSetForeground( rex_d, rex_gc, BiRexReturnPixel( c ));
  XDrawLine( rex_d, rex_p, rex_gc, s1->x, s1->y, s2->x, s2->y );

	/* Begin YS11 */
	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawLine(s1,s2,c);
	}
	/* End YS11 */

/********************** Clipping Transaction by YS11 *********************** */
/** Begin YS11 **/
END:
	BiMacEndClipRect();
/** End YS11 **/
}

/* ***************************** BiDrawPset2 ******************************* */
/*depend*/ void BiDrawPset2D( BIPOINTS *s, BICOLOR *c )
{
/** Begin YS11 **/
	if(!BIINBOUND(s))
	{
		return;
	}

	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPset(s,c);
	}
/** End YS11 **/

  XSetForeground( rex_d, rex_gc, BiRexReturnPixel( c ) );
  XDrawPoint( rex_d, rex_p, rex_gc, s->x, s->y );
}

/* *************************** BiDrawCircle2 ******************************* */
/* depend*/ void BiDrawCircle2D( BIPOINTS *s, long radx, long rady, BICOLOR *c )
{
/*********************** Clipping Transaction by YS11 ********************** */
/** Begin YS11 **/
/** Circleだと簡単にクリップちゅうわけにいかん。どうしたもんかね ^_^; **/
	if(!BIINBOUND(s))
	{
		return;
	}

	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawEllipse(s,radx,rady,c);
	}
/** End YS11 **/
  XSetForeground( rex_d, rex_gc, BiRexReturnPixel( c ) );

  XFillArc( rex_d, rex_p, rex_gc,
                      (s->x)-radx, (s->y)-rady, radx*2, rady*2, 0, 360*64 );
}

/* **************************** BiDrawPolyg2 ******************************* */
/*depend*/ void BiDrawPolyg2D( int ns, BIPOINTS *s, BICOLOR *c )
{
  int i;
  XPoint *xversion_points;

/*********************** Clipping Transaction by YS11 ********************** */
/** Begin YS11 **/
	unsigned long stk,stk2;
	int nsNew;
	BIPOINTS *sNew,*sP;

        BiMacSetClipRect();

	sNew=NULL;
	for(i=0; i<ns; i++)
	{
		if(!BIINBOUND(&s[i]))
		{
			sNew=BiPushTmpStack(&stk,sizeof(BIPOINTS)*ns*4);
			if(sNew==NULL || BiClipPolygScrn(&nsNew,sNew,ns,s)!=BI_IN)
			{
				return;
			}
			ns=nsNew;
			s=sNew;
			break;
		}
	}

	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawPolygon(ns,s,c);
	}
/** End YS11 **/

  xversion_points = (XPoint *)BiPushTmpStack(&stk2, ns * sizeof( XPoint ) );

  for( i = 0; i < ns; i++ ){
    ( xversion_points + i )->x = s[i].x;
    ( xversion_points + i )->y = s[i].y;
   }

  XSetForeground( rex_d, rex_gc, BiRexReturnPixel( c ) );
  XSetFillRule( rex_d, rex_gc, WindingRule );
  XFillPolygon( rex_d, rex_p, rex_gc, xversion_points,
                                               ns, Complex, CoordModeOrigin );
/*********************** Clipping Transaction by YS11 ********************** */
/** Begin YS11 **/
  BiPopTmpStack(stk2);

  if(sNew!=NULL)
  {
    BiMacEndClipRect();
    BiPopTmpStack(stk);
  }
/** End YS11 **/
}

/* ***************************** BiDrawRect2 ******************************* */
/*depend*/ void BiDrawRect2D( BIPOINTS *s1, BIPOINTS *s2, BICOLOR *c )
{
/* ******************** Clipping Transaction by YS11 *********************** */
/** Begin YS11 **/
	BIPOINTS a1,a2;
	a1.x=BiSmaller(s1->x,s2->x);
	a1.y=BiSmaller(s1->y,s2->y);
	a2.x=BiLarger(s1->x,s2->x);
	a2.y=BiLarger(s1->y,s2->y);

	a1.x=BiLarger(a1.x,0);
	a1.y=BiLarger(a1.y,0);
	a2.x=BiSmaller(a2.x,winX);
	a2.y=BiSmaller(a2.y,winY);

	s1=&a1;
	s2=&a2;

	if(BiEpsMode==BI_ON)
	{
		BiEpsDrawRect(s1,s2,c);
	}
/** End YS11 **/

  XSetForeground( rex_d, rex_gc, BiRexReturnPixel( c ) );
  XFillRectangle( rex_d, rex_p, rex_gc,
                    s1->x, s1->y,
                    BiAbs((s1->x)-(s2->x)), BiAbs((s1->y)-(s2->y)) );
}

/* **************************** BiDrawMarker2 ****************************** */
/*depend*/ void BiDrawMarker2D( BIPOINTS *s, int mkType, BICOLOR *c )
{
	#define BIMKRAD 3
	BIPOINTS p1,p2;

/* ******************** Clipping Transaction by YS11 *********************** */
/** Begin YS11 **/
	if(!BIINBOUND(s))
	{
		return;
	}
/** End YS11 **/
	switch(mkType)
	{
	case BIMK_RECT:
		BiSetPoint2(&p1, s->x-BIMKRAD,s->y-BIMKRAD);
		BiSetPoint2(&p2, s->x+BIMKRAD,s->y+BIMKRAD);
		BiDrawRect2D(&p1,&p2,c);
		break;
	case BIMK_CIRCLE:
		BiDrawCircle2D(s,BIMKRAD,BIMKRAD,c);
		break;
	case BIMK_CROSS:
		BiSetPoint2(&p1, s->x-BIMKRAD,s->y-BIMKRAD);
		BiSetPoint2(&p2, s->x+BIMKRAD,s->y+BIMKRAD);
		BiDrawLine2D(&p1,&p2,c);
		BiSetPoint2(&p1, s->x+BIMKRAD,s->y-BIMKRAD);
		BiSetPoint2(&p2, s->x-BIMKRAD,s->y+BIMKRAD);
		BiDrawLine2D(&p1,&p2,c);
		break;
	}
}

/* **************************** BiDrawString2 ****************************** */
/*depend*/ void BiDrawString2D( BIPOINTS *s, char *str, BICOLOR *c )
{

/** Begin YS11 **/
	if(!BIINBOUND(s))
	{
		return;
	}
/** End YS11 **/

  XSetForeground( rex_d, rex_gc, BiRexReturnPixel( c ) );
  XmbDrawString( rex_d, rex_p, BiRexFontset, rex_gc,
                                             s->x, s->y, str, strlen( str ) );
}

/* ***************************** BiDrawText2 ******************************* */
/* depend */void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
    /**********************************************************************
	(s->x,s->y)を基準点とするテキストstr[0〜]を描く。下のプログラムに沿って
	書くこと。なお、漢字の表示までは保証しなくていい。
	(他の関数をきちんと書いてあれば FONTX,FONTY の変更だけでよい)
    **********************************************************************/

	#define FONTX 20  /* フォントサイズ X */
	#define FONTY 20 /* フォントサイズ Y */
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;

	/* テキストの横文字数と縦文字数を数える */
	sWid=0;
	sHei=0;
	for(i=0; str[i]!=NULL; i++)
	{
		sWid=BiLarger(strlen(str[i]),sWid);
		sHei++;
	}

	/* 情報を元に、書き出し位置を決める */
	switch(h)
	{
	case BIMG_CENTER:
		xSta=s->x -(FONTX*sWid)/2;
		break;
	case BIMG_LEFT:
		xSta=s->x;
		break;
	case BIMG_RIGHT:
		xSta=s->x -(FONTX*sWid);
		break;
	}
	switch(v)
	{
	case BIMG_CENTER:
		ySta=(s->y+FONTY/2)-(FONTY*sHei)/2;
		break;
	case BIMG_TOP:
		ySta=s->y+FONTY-1;
		break;
	case BIMG_BOTTOM:
		ySta=s->y-(FONTY*sHei);
		break;
	}

	/* 後は書くだけ */
	tmp.x=xSta;
	tmp.y=ySta;
	for(i=0; str[i]!=NULL; i++)
	{
		BiDrawString2D(&tmp,str[i],c);
		tmp.y+=FONTY;
	}
}





void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINTS p1,p2;
	BiSetPointS(&p1,s1->x,s1->y);
	BiSetPointS(&p2,s2->x,s1->y);
	BiDrawLine2D(&p1,&p2,col);
	BiSetPointS(&p1,s2->x,s1->y);
	BiSetPointS(&p2,s2->x,s2->y);
	BiDrawLine2D(&p1,&p2,col);
	BiSetPointS(&p1,s2->x,s2->y);
	BiSetPointS(&p2,s1->x,s2->y);
	BiDrawLine2D(&p1,&p2,col);
	BiSetPointS(&p1,s1->x,s2->y);
	BiSetPointS(&p2,s1->x,s1->y);
	BiDrawLine2D(&p1,&p2,col);
}

void BiDrawEmptyPolyg2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiDrawLine2D(&s[i],&s[i+1],col);
	}
	BiDrawLine2D(&s[0],&s[n-1],col);
}

void BiDrawPolyline2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiDrawLine2D(&s[i],&s[i+1],col);
	}
}

void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
	int i;
	BIPOINTS v1,v2,p1,p2;

	BiSetPointS(&v2,radx,0);
	for(i=1; i<=8; i++)
	{
		v1=v2;

		v2.x=(long)((real)radx*BiCos(i*4096));
		v2.y=(long)((real)rady*BiSin(i*4096));

		BiSetPointS(&p1,s->x+v1.x,s->y+v1.y);
		BiSetPointS(&p2,s->x+v2.x,s->y+v2.y);
		BiDrawLine2D(&p1,&p2,col);

		BiSetPointS(&p1,s->x-v1.x,s->y+v1.y);
		BiSetPointS(&p2,s->x-v2.x,s->y+v2.y);
		BiDrawLine2D(&p1,&p2,col);

		BiSetPointS(&p1,s->x+v1.x,s->y-v1.y);
		BiSetPointS(&p2,s->x+v2.x,s->y-v2.y);
		BiDrawLine2D(&p1,&p2,col);

		BiSetPointS(&p1,s->x-v1.x,s->y-v1.y);
		BiSetPointS(&p2,s->x-v2.x,s->y-v2.y);
		BiDrawLine2D(&p1,&p2,col);
	}
}

void BiGetColor(BICOLOR *col,BIPOINTS *s)
{
	/* Write This Function in Each Platrome! */
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
	/* Write This Function in Each Platrome! */
}





/*****************************************************************************

   File Name

*****************************************************************************/
void BiConstrainFileName(char neo[],char org[])
{
	int i;

	if(neo!=org)
	{
		strcpy(neo,org);
	}

	for(i=0; neo[i]!=0; i++)
	{
		if(neo[i]==':' || neo[i]=='\\')
		{
			neo[i]='/';
		}
	}
}
