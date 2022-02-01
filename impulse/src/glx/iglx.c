/* gcc ###.c -lGLU -lGL -lX11 -lXext -lm */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include "../impulse.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>


/* External Function in iopengl.c */
#define BIOGL_MAX_VTX_PER_POLYG 128
#define BI_GL_FONT_BITMAP_BASE 1000
void BiInitOpenGL(long wid,long hei);




extern char BiWindowName[];

static Display *dsp;
static Window wnd;
static Colormap cmap;
static XVisualInfo *xvi;
static int wWid,wHei;
static GLXContext ctx;
static int glxCfg[]={GLX_DOUBLEBUFFER,GLX_RGBA,GLX_DEPTH_SIZE,16,None};

#define BIXEVENTMASK (KeyPress|KeyRelease|ExposureMask|StructureNotifyMask)


void BiOpenWindowEx(long x,long y,long wid,long hei)
{
	int n;
	char **m,*def;
	XSetWindowAttributes swa;
	Font font;

	dsp=XOpenDisplay(NULL);
	if(dsp==NULL)
	{
		fprintf(stderr,"BlueImpulse reports : Cannot Open Display.\n");
		exit(1);
	}

	if(glXQueryExtension(dsp,NULL,NULL)==0)
	{
		fprintf(stderr,"BlueImpulse reports : OpenGL is not supported.\n");
		exit(1);
	}

	xvi=glXChooseVisual(dsp,DefaultScreen(dsp),glxCfg);
	if(xvi==NULL)
	{
		fprintf(stderr,"BlueImpulse reports : DoubleBuffer not supported.\n");
		exit(1);
	}

	if(xvi->class!=TrueColor)
	{
		fprintf(stderr,"BlueImpulse reports : Not true color warning.\n");
	}

	cmap=XCreateColormap
	    (dsp,RootWindow(dsp,xvi->screen),xvi->visual,AllocNone);

	ctx=glXCreateContext(dsp,xvi,None,GL_TRUE);
	if(ctx==NULL)
	{
		fprintf(stderr,"BlueImpulse reports : Cannot create context.\n");
		exit(1);
	}


	swa.colormap=cmap;
	swa.border_pixel=0;
	swa.event_mask=BIXEVENTMASK;
	wnd=XCreateWindow(dsp,RootWindow(dsp,xvi->screen),
	                  x+wid/2,y+hei/2,wid,hei,
	                  0,
	                  xvi->depth,
	                  InputOutput,
	                  xvi->visual,
	                  CWEventMask|CWBorderPixel|CWColormap,&swa);

	XStoreName(dsp,wnd,BiWindowName);
	XSetIconName(dsp,wnd,BiWindowName);
	XMapWindow(dsp,wnd);

	glXMakeCurrent(dsp,wnd,ctx);

	BiInitOpenGL(wid,hei);

	font=XLoadFont(dsp,"-*-*-*-*-*-*-20-20-*-*-*-*-iso8859-1");
	glXUseXFont(font,0,255,BI_GL_FONT_BITMAP_BASE);
}

void BiOpenWindow(long wid,long hei)
{
	BiOpenWindowEx(64,64,wid,hei);
}

void BiCloseWindow(void)
{
  XCloseDisplay(dsp);
}

/**********************************************************************/

static int BiFatalFlag=BI_OFF;
int BiFatal( void )
{
  return BiFatalFlag;
}

void BiGetWindowSize(long *wid,long *hei)
{
	*wid=wWid;
	*hei=wHei;
}

void BiGetStdProjection(BIPROJ *prj)
{
	long wid,hei;
	BiGetWindowSize(&wid,&hei);
	prj->lx=wid;
	prj->ly=hei;
	prj->cx=wid/2;
	prj->cy=hei/2;
	prj->magx=(real)wid/(real)1.41421356;
	prj->magy=prj->magx;
	prj->nearz=(real)2.5;
	prj->farz=(real)10000.0;
}

void BiSwapBuffers(void)
{
	glFlush();
	glXSwapBuffers(dsp,wnd);
}





/*****************************************************************************

   Memory Management

*****************************************************************************/

void BiMemoryInitialize(void)
{
	/* Nothing To Do */
}

void *BiMalloc(size_t uni)
{
	return malloc(uni);
}

void BiFree(void *ptr)
{
	free(ptr);
}

void BiGetColor(BICOLOR *col,BIPOINTS *s)
{
	static int first=BI_ON;
	if(first==BI_ON)
	{
		first=BI_OFF;
		fprintf(stderr,"BiGetColor is future function.\n");
	}
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
	static int first=BI_ON;
	if(first==BI_ON)
	{
		first=BI_OFF;
		fprintf(stderr,"BiGetColorArray is future function.\n");
	}
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





/*****************************************************************************

   Device

*****************************************************************************/
static KeySym BiKeyMap[512];
static int ntab = 0;
typedef struct
{
	int bikcode;
	KeySym ks;
} RexKeyMatchTable;

static RexKeyMatchTable table[512];

#define N_KEYBUF 512
#define N_PRSBUF 512
static int nKBuf,KBuf[N_KEYBUF];
static int prsBuf[N_PRSBUF];

void BiDeviceMatchKey(int kcode,KeySym BiRexXKeySym)
{
	BiKeyMap[kcode] = BiRexXKeySym;
	table[ntab].bikcode = kcode;
	table[ntab].ks = BiRexXKeySym;
	ntab++;
}

void BiDeviceInitialize(void)
{
	int i;

	nKBuf=0;
	for(i=0; i<N_PRSBUF; i++)
	{
		prsBuf[i]=BI_OFF;
	}

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

static void BiResize(void);
static void BiUpdateKeyboard(void);

void BiUpdateDevice(void)
{
	BiResize();
	BiUpdateKeyboard();
}

static void BiUpdateKeyboard(void)
{
	int i;
	int bikey;
	char tmp[256];
	KeySym ks;
	XComposeStatus stat;
	XEvent ev;

	while(XCheckWindowEvent(dsp,wnd,KeyPressMask|KeyReleaseMask,&ev)==True)
	{
		XLookupString((XKeyEvent *)&ev,tmp,256,&ks,&stat);
		for(i=0; i<ntab; i++)
		{
			if(ks==table[i].ks)
			{
				bikey=table[i].bikcode;
				switch(ev.type)
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
}

static void BiResize(void)
{
	XEvent ev;
	if(XCheckTypedWindowEvent(dsp,wnd,ConfigureNotify,&ev)==True)
	{
		wWid=ev.xconfigure.width;
		wHei=ev.xconfigure.height;
		glViewport(0,0,wWid,wHei);
	}
}

void BiMouse(int *lbt,int *mbt,int *rbt,long *mx,long *my)
{
	Window r,c;
	int xInRoot,yInRoot;
	unsigned int mask;

	*lbt = BI_OFF;
	*mbt = BI_OFF;
	*rbt = BI_OFF;

	XQueryPointer(dsp,wnd,&r,&c,&xInRoot,&yInRoot,(int *)mx,(int *)my,&mask);
	if(mask & Button1Mask)
	{
		*lbt = BI_ON;
	}
	if(mask & Button2Mask)
	{
		*mbt = BI_ON;
	}
	if(mask & Button3Mask)
	{
		*rbt = BI_ON;
	}
}

int BiKey( int kcode )
{
	return prsBuf[kcode];
}

int BiInkey( void )
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
}

