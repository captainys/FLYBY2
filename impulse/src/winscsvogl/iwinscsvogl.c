/* Machine Depending Module : Windows95 DIB Screen Saver Version */

/* Use ScSvMain(void) instead of main(int ac,char *av[]) */


/* 01/12/1999

Now, application has to implement

void GetSizeLimit(int *sizeLimitX,int *sizeLimitY);
BOOL WINAPI ScreenSaverConfigureDialog(HWND w,UINT msg,WPARAM wp,LPARAM lp);
BOOL WINAPI RegisterDialogClasses(HANDLE hinst);


*/


extern void GetSizeLimit(int *sizeLimitX,int *sizeLimitY);


#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "../impulse.h"
#include "../iutil.h"

#include <windowsx.h>
#include <scrnsave.h>


char writeOrAdd[]="w";


/* for compatibility */
#define BI_MAX_NUM_SIMUL_DLG 16
int nBiDlgRegistered=0;
HWND BiDlgRegistered[BI_MAX_NUM_SIMUL_DLG];
FILE *fp=NULL;

/* External Function in iopengl.c */
#define BIOGL_MAX_VTX_PER_POLYG 128
#define BI_GL_FONT_BITMAP_BASE 1000
extern void BiInitOpenGL(long wid,long hei);

HWND BiWnd;
HDC BiWndDC=NULL;
HGLRC BiGlRC=NULL;   // =NULL is added on 2001/06/10
static HPALETTE glPal;

static long BiWinX,BiWinY;


int BiWinKeyCodeToVirtualKey(int bikey)
{
	return 0;
}

int BiWinVirtualKeyToKeyCode(int vk)
{
	return 0;
}


/*****************************************************************************/
static void BiSetPixelFormat(HDC dc);
static unsigned char YsPalVal(unsigned long n,unsigned bit,unsigned sft);
static HPALETTE BiCreatePalette(HDC dc);
/*****************************************************************************/


HMENU BiMenu; /* Just to link with HOTAS Library */

void BiWinSetExtraWindowFunc(int (*func)(HWND,UINT,WPARAM,LPARAM))
{
	/* Nothing to do. Because this function is just
	   to link with HOTAS Library */
}



void BiWinCheckMessage(char *msg)
{
	MessageBox(NULL,msg,"Check",MB_APPLMODAL);
}

/***************************************************************************

    OpenGL for Windows Support

***************************************************************************/

static void BiSetPixelFormat(HDC dc)
{
	static PIXELFORMATDESCRIPTOR pfd,altPfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};
	int pfm;

/* I can choose a pixel format with 32bit z-buffer, but
   it typically chooses non-hardware accelerated pixel format.

	int i,nPfd;
	nPfd=DescribePixelFormat(dc,1,sizeof(pfd),&pfd);
	for(i=1; i<nPfd; i++)
	{
		if(DescribePixelFormat(dc,i,sizeof(pfd),&pfd)>0)
		{
			if(pfd.cDepthBits>=32)
			{
				printf("!\n");
			}
			if((pfd.dwFlags&PFD_SUPPORT_OPENGL)!=0 &&
			   (pfd.dwFlags&PFD_DOUBLEBUFFER)!=0 &&
			   (pfd.dwFlags&PFD_DRAW_TO_WINDOW)!=0 &&
			   pfd.iPixelType==PFD_TYPE_RGBA &&
			    pfd.cDepthBits>=32)
			{
				pfm=i;
				printf("Pixel format found!\n");
				printf("  Color depth   %d\n",pfd.cColorBits);
				goto PFMFOUND;
			}
		}
		else
		{
			break;
		}
	}
*/

	pfd=altPfd;
	pfm=ChoosePixelFormat(dc,&pfd);
	DescribePixelFormat(dc,pfm,sizeof(pfd),&pfd);
	if((pfd.dwFlags&PFD_GENERIC_ACCELERATED)!=0)
	{
		printf("Accelerated by the hardward.\n");
	}
	if((pfd.dwFlags&PFD_SUPPORT_OPENGL)==0)
	{
		MessageBox(NULL,"OpenGL not supported","Error",MB_OK);
		exit(1);
	}

PFMFOUND:
	if(pfm!=0)
	{
		if(SetPixelFormat(dc,pfm,&pfd)!=FALSE)
		{
			glPal=BiCreatePalette(dc);
			if(glPal!=NULL)
			{
				SelectPalette(dc,glPal,FALSE);
				RealizePalette(dc);
			}
			return;
		}
	}
	MessageBox(NULL,"Error In YsSetPixelFormat.",NULL,MB_OK);
	exit(1);
}

static unsigned char BiPalVal(unsigned long n,unsigned bit,unsigned sft)
{
	unsigned long msk;
	n>>=sft;
	msk=(1<<bit)-1;
	n&=msk;
	return (unsigned char)(n*255/msk);
}

static HPALETTE BiCreatePalette(HDC dc)
{
	HPALETTE neo;
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *lp;
    int n,i;

    n=GetPixelFormat(dc);
	if(n!=0)
	{
		neo=NULL;
	    DescribePixelFormat(dc,n,sizeof(PIXELFORMATDESCRIPTOR),&pfd);
	    if(pfd.dwFlags & PFD_NEED_PALETTE)
	    {
			printf("BI:Making Palette.\n");
	        n=1<<pfd.cColorBits;
	        lp=(LOGPALETTE *)malloc(sizeof(LOGPALETTE)+n*sizeof(PALETTEENTRY));
	        lp->palVersion=0x300;
	        lp->palNumEntries=n;
	        for (i=0; i<n; i++)
	        {
	            lp->palPalEntry[i].peRed  =BiPalVal(i,pfd.cRedBits,pfd.cRedShift);
	            lp->palPalEntry[i].peGreen=BiPalVal(i,pfd.cGreenBits,pfd.cGreenShift);
	            lp->palPalEntry[i].peBlue =BiPalVal(i,pfd.cBlueBits,pfd.cBlueShift);
	            lp->palPalEntry[i].peFlags=0;
	        }

	        neo=CreatePalette(lp);
			free(lp);
	    }
	    return neo;
	}
	MessageBox(NULL,"Error in GetPixelFormat",NULL,MB_APPLMODAL);
	exit(1);
}


/*****************************************************************************

   Save and Restore Wave Volume

*****************************************************************************/


int nWaveDevs=0;
unsigned long *waveVolume=NULL;

void SaveWaveVolume(void)
{
	nWaveDevs=waveOutGetNumDevs();
	if(nWaveDevs>0)
	{
		waveVolume=(unsigned long *)malloc(sizeof(unsigned long)*nWaveDevs);
		if(waveVolume!=NULL)
		{
			int i;
			for(i=0; i<nWaveDevs; i++)
			{
				waveOutGetVolume((HWAVEOUT)i,&waveVolume[i]);
			}
		}
	}
}

void RestoreWaveVolume(void)
{
	if(nWaveDevs>0 && waveVolume!=NULL)
	{
		int i;
		for(i=0; i<nWaveDevs; i++)
		{
			waveOutSetVolume((HWAVEOUT)i,waveVolume[i]);
		}
		free(waveVolume);
	}
	nWaveDevs=0;
	waveVolume=NULL;
}




/*****************************************************************************

   Functions called by scrnsave.lib

*****************************************************************************/
void BiOpenWindowForScreenSaver(HWND w);

extern char BiWindowName[];

extern int ScSvInterval(void);
extern int ScSvMain(void);
extern int ScSvInitialize(void);
extern int ScSvTerminate(void);

static void LparamToCoord(int *x,int *y,LPARAM lPara);

HINSTANCE BiCInst;
int started=BI_FALSE;
int irqMask=0;  // <- To prevent infinite loop.
unsigned prevTickCount=0;  // Currently not used

HANDLE thread=NULL;
int threadId;

LRESULT CALLBACK ScreenSaverProc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	/* FILE *fp;
	fp=fopen("C:\\loglog",writeOrAdd);
	if(msg==WM_CREATE)
	{
		fprintf(fp,"%08d  WM_CREATE\n",GetTickCount());
	}
	else if(msg==WM_ERASEBKGND)
	{
		fprintf(fp,"%08d  WM_ERASEBKGND\n",GetTickCount());
	}
	else if(msg==WM_DESTROY)
	{
		fprintf(fp,"%08d  WM_DESTROY\n",GetTickCount());
	}
	else if(msg==WM_SYSCOMMAND)
	{
		fprintf(fp,"%08d  WM_SYSCOMMAND\n",GetTickCount());
	}
	else if(msg==WM_DESTROY)
	{
		fprintf(fp,"%08d  WM_DESTROY\n",GetTickCount());
		}
	else if(msg==WM_SETCURSOR)
	{
		fprintf(fp,"%08d  WM_SETCURSOR\n",GetTickCount());
	}
	else if(msg==WM_NCACTIVATE)
	{
		fprintf(fp,"%08d  WM_NCACTIVATE\n",GetTickCount());
	}
	else if(msg==WM_ACTIVATE)
	{
		fprintf(fp,"%08d  WM_ACTIVATE\n",GetTickCount());
	}
	else if(msg==WM_ACTIVATEAPP)
	{
		fprintf(fp,"%08d  WM_ACTIVATEAPP\n",GetTickCount());
	}
	else if(msg==WM_MOUSEMOVE)
	{
		fprintf(fp,"%08d  WM_MOUSEMOVE\n",GetTickCount());
	}
	else if(msg==WM_LBUTTONDOWN)
	{
		fprintf(fp,"%08d  WM_LBUTTONDOWN\n",GetTickCount());
	}
	else if(msg==WM_MBUTTONDOWN)
	{
		fprintf(fp,"%08d  WM_MBUTTONDOWN\n",GetTickCount());
	}
	else if(msg==WM_RBUTTONDOWN)
	{
		fprintf(fp,"%08d  WM_RBUTTONDOWN\n",GetTickCount());
	}
	else if(msg==WM_KEYDOWN)
	{
		fprintf(fp,"%08d  WM_KEYDOWN\n",GetTickCount());
	}
	else if(msg==WM_SYSKEYDOWN)
	{
		fprintf(fp,"%08d  WM_SYSKEYDOWN\n",GetTickCount());
	}
	else
	{
		fprintf(fp,"%08d  %d\n",GetTickCount(),msg);
	}
	fclose(fp);

	if(writeOrAdd[0]=='w')
	{
		writeOrAdd[0]='a';
	} */


	int cxScrn,cyScrn;
	int isFullScrn;
	cxScrn=GetSystemMetrics(SM_CXSCREEN);
	cyScrn=GetSystemMetrics(SM_CYSCREEN);

	// If the window size is more than 1/3 of the full screen size,
	// it's highly likely that this is running in full screen.
	if(cxScrn<BiWinX*3 && cyScrn<BiWinY*3)
	{
		isFullScrn=BI_TRUE;
	}
	else
	{
		isFullScrn=BI_FALSE;
	}


	switch(msg)
	{
	case WM_CREATE:
		{
			long exstyle;
			exstyle=GetWindowLong(w,GWL_EXSTYLE);
			SetWindowLong(w,GWL_EXSTYLE,exstyle|WS_EX_TRANSPARENT);

			if(nWaveDevs==0)
			{
				SaveWaveVolume();
			}
			strcpy(szAppName,"BI SCREEN SAVER");

			BiOpenWindowForScreenSaver(w);
			strcpy(szAppName,BiWindowName);
			BiCInst=hMainInstance;

			irqMask=0;
			prevTickCount=GetTickCount();
		}
		break;
	case WM_ERASEBKGND:
		if(started!=BI_TRUE)
		{
			if(isFullScrn==BI_TRUE)
			{
				ShowCursor(FALSE);
			}

			if(irqMask==0)
			{
				irqMask=1;

				ScSvInitialize();

				thread=CreateThread(NULL,0,
				                    (LPTHREAD_START_ROUTINE)ScSvMain,
				                    NULL,
				                    STANDARD_RIGHTS_REQUIRED,
				                    &threadId);
				if(thread!=NULL)
				{
					SetThreadPriority(thread, THREAD_PRIORITY_BELOW_NORMAL);
				}

				irqMask=0;
			}

			// if(timerId==0)
			// {
			// 	timerId=SetTimer(w,0,1,NULL);
			// }
			PostMessage(w,WM_APP,0,0);

			started=BI_TRUE;
		}
		return 1L;
	case WM_DESTROY:
		// if(timerId!=0)
		// {
		// 	KillTimer(w,timerId);
		// }
		irqMask=1;
		ScSvTerminate();
		if(nWaveDevs>0)
		{
			RestoreWaveVolume();
		}
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(BiGlRC);
		SendMessage(BiWnd,WM_CLOSE,0,0);
		BiWnd=NULL;
		if(isFullScrn==BI_TRUE)
		{
			ShowCursor(TRUE);
		}
		exit(0);
		break;
	}
	return DefScreenSaverProc(w,msg,wPara,lPara);
}

static void LparamToCoord(int *x,int *y,LPARAM lPara)
{
	*x=LOWORD(lPara);
	*y=HIWORD(lPara);
}







/*****************************************************************************

   Setting

*****************************************************************************/

LRESULT CALLBACK ScreenSaverProc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara);

#define WINCLASS "BIAPPCLASS"
#define WINNAME BiWindowName

static int BiFatalFlag=BI_OFF;
/* depend */int BiFatal(void)
{
	return BiFatalFlag;
}

void BiWinSetFontHandle(HFONT fnt)
{
	HDC glDC;

	glDC=wglGetCurrentDC();
	SelectObject(glDC,fnt);
	wglUseFontBitmaps(glDC,0,255,BI_GL_FONT_BITMAP_BASE);
	glListBase(BI_GL_FONT_BITMAP_BASE);
}

HPALETTE BiWinCreatePalette(HDC dc)
{
	BiGlRC=wglCreateContext(dc);
	if(BiGlRC!=NULL)
	{
		if(wglMakeCurrent(dc,BiGlRC)==TRUE)
		{
			HDC glDC;

			BiInitOpenGL(BiWinX,BiWinY);

			glDC=wglGetCurrentDC();
			SelectObject(glDC,GetStockObject(SYSTEM_FIXED_FONT));
			wglUseFontBitmaps(glDC,0,255,BI_GL_FONT_BITMAP_BASE);

			if(glPal!=NULL)
			{
				SelectPalette(dc,glPal,FALSE);
				RealizePalette(dc);
			}
			return glPal;
		}
		MessageBox(NULL,"Error in wglMakeCurrent.",NULL,MB_APPLMODAL);
		exit(1);
	}

	// Copied from VC++ example
	{
		LPVOID lpMsgBuf;
		FormatMessage(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER |
		    FORMAT_MESSAGE_FROM_SYSTEM |
		    FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL,
		    GetLastError(),
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		    (LPTSTR) &lpMsgBuf,
		    0,
		    NULL);

		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		LocalFree( lpMsgBuf );
	}

	MessageBox(NULL,"Error in wglCreateContext.",NULL,MB_APPLMODAL);
	exit(1);
}

void BiOpenWindowForScreenSaver(HWND w)
{
	int lupX,lupY,sizX,sizY,sizeLimitX,sizeLimitY;
	RECT WinRect;
{
FILE *fp;
fp=fopen("c:\\tmp\\txt.txt","w");
fprintf(fp,"A\n");
fclose(fp);
}
	GetWindowRect(w,&WinRect);
	sizX=WinRect.right-WinRect.left+1;
	sizY=WinRect.bottom-WinRect.top+1;

	GetSizeLimit(&sizeLimitX,&sizeLimitY);

	sizX=BiSmaller(sizX,sizeLimitX);
	sizY=BiSmaller(sizY,sizeLimitY);
	lupX=0;
	lupY=0;
	BiWinX=sizX;
	BiWinY=sizY;

	BiWnd=w;

	GetWindowRect(w,&WinRect);

	sizX=WinRect.right-WinRect.left+1;
	sizY=WinRect.bottom-WinRect.top+1;

	lupX=WinRect.left;
	lupY=WinRect.top;
	BiWinX=sizX;
	BiWinY=sizY;

	BiWndDC=GetDC(BiWnd);

	BiSetPixelFormat(BiWndDC);
	BiWinCreatePalette(BiWndDC);

	wglMakeCurrent(BiWndDC,BiGlRC);
	glViewport(0,0,sizX,sizY);

	SendMessage(w,WM_ERASEBKGND,(unsigned)BiWndDC,0);






/////*	Nothing surprising.  However, Microsoft is betraying software developers by
////	providing incomplete SCRNSAVE.LIB .  Look at OpenGL Screensaver Examples.
////	OpenGL Screensaver Example is taking over one function of SCRNSAVE.LIB .
////	That information is hidden to software developers unless he reads through
////	that intentionally-made-unreadable examples. */
////
////	WNDCLASS wCls;
////	HINSTANCE module;
////	int lupX,lupY,sizX,sizY;
////	RECT WinRect;
////
/////* -> Not taking over fucking SCRNSAVE.LIB
////	GetWindowRect(w,&WinRect);
////	sizX=WinRect.right-WinRect.left+1;
////	sizY=WinRect.bottom-WinRect.top+1;
////
////	GetSizeLimit(&sizeLimitX,&sizeLimitY);
////
////	sizX=BiSmaller(sizX,sizeLimitX);
////	sizY=BiSmaller(sizY,sizeLimitY);
////	lupX=0;
////	lupY=0;
////	BiWinX=sizX;
////	BiWinY=sizY;
////
////	BiWnd=w;
////<- Not taking over fucking SCRNSAVE.LIB */
////
////
////
////	GetWindowRect(w,&WinRect);
////
////	sizX=WinRect.right-WinRect.left+1;
////	sizY=WinRect.bottom-WinRect.top+1;
////
////	lupX=WinRect.left;
////	lupY=WinRect.top;
////	BiWinX=sizX;
////	BiWinY=sizY;
////
/////* -> Take Over Fucking SCRNSAVE.LIB */
////	// To counter Microsoft's fucking SCRNSAVE.LIB, I take over the window.
////	// Death to SCRNSAVE.LIB
////	module=GetModuleHandle(NULL);
////
////	wCls.style=CS_VREDRAW|CS_HREDRAW|CS_OWNDC|CS_BYTEALIGNWINDOW;
////	wCls.lpfnWndProc=BiWindowFunc;
////	wCls.cbClsExtra=0;
////	wCls.cbWndExtra=0;
////	wCls.hInstance=module;
////	wCls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
////	wCls.hCursor=NULL; //LoadCursor(NULL,IDC_ARROW);
////	wCls.hbrBackground=NULL;
////	wCls.lpszMenuName=NULL;
////	wCls.lpszClassName="SCRNSAVE_CLASS";
////	if(!RegisterClass(&wCls))
////	{
////		exit(1);
////	}
////
////	module=GetModuleHandle(NULL);
////	BiWnd=CreateWindow
////	    ("SCRNSAVE_CLASS","SCRNSAVE",WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
////	     lupX,lupY,sizX,sizY,w,NULL,module,NULL);
////	ShowWindow(BiWnd,SW_SHOWNORMAL);
////
////	if(BiWnd==NULL)
////	{
////		BiFatalFlag=BI_ON;
////		exit(1);
////	}
////
/////*	 <- Take Over Fucking SCRNSAVE.LIB */
////
////	BiWndDc=GetDC(BiWnd);
////
////	BiSetPixelFormat(BiWndDc);
////	BiWinCreatePalette(BiWndDc);
////
////	wglMakeCurrent(BiWndDc,BiGlRC);
////	glViewport(0,0,sizX,sizY);
////
////	SendMessage(w,WM_ERASEBKGND,(unsigned)BiWndDc,0);
}

void BiOpenWindow(long wx,long wy)
{
	/* Nothing to do here */
}

void BiCloseWindow(void)
{
}

void BiGetWindowSize(long *wid,long *hei)
{
	*wid = BiWinX;
	*hei = BiWinY;
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


//  How To Set Clip Frame in WIN31 API
//	HRGN regio;
//	regio=CreateRectRgn(0,0,(int)winX-1,(int)winY-1);
//	SelectClipRgn(dBuf.dc,regio);
//	DeleteObject(regio);

static void BiSetLineColor(BICOLOR *ptr);
static void BiEndLineColor(void);

void BiSwapBuffers(void)
{
	HDC dc;
	glFlush();
	dc=wglGetCurrentDC();
	SwapBuffers(dc);
}

/*****************************************************************************

   Device

*****************************************************************************/

static BYTE kbs[256],pKbs[256];
static unsigned char VkToKc[256];
static BYTE KcToVk[256];

#define MAX_KEYBUF 256
static int nKeyBuf;
static char keyBuf[MAX_KEYBUF];
static int lbt,rbt,mosx,mosy;

static void BiDeviceMatchKey(int kc,BYTE vk)
{
	VkToKc[vk]=kc;
	KcToVk[kc]=vk;
}

void BiDeviceInitialize(void)
{
	lbt=BI_OFF;
	rbt=BI_OFF;

    BiDeviceMatchKey(BIKEY_STOP    ,VK_END       );
    BiDeviceMatchKey(BIKEY_COPY    ,0            );
    BiDeviceMatchKey(BIKEY_F1      ,VK_F1        );
    BiDeviceMatchKey(BIKEY_F2      ,VK_F2        );
    BiDeviceMatchKey(BIKEY_F3      ,VK_F3        );
    BiDeviceMatchKey(BIKEY_F4      ,VK_F4        );
    BiDeviceMatchKey(BIKEY_F5      ,VK_F5        );
    BiDeviceMatchKey(BIKEY_F6      ,VK_F6        );
    BiDeviceMatchKey(BIKEY_F7      ,VK_F7        );
    BiDeviceMatchKey(BIKEY_F8      ,VK_F8        );
    BiDeviceMatchKey(BIKEY_F9      ,VK_F9        );
    BiDeviceMatchKey(BIKEY_F10     ,VK_F10       );
    BiDeviceMatchKey(BIKEY_ZERO    ,'0'          );
    BiDeviceMatchKey(BIKEY_ONE     ,'1'          );
    BiDeviceMatchKey(BIKEY_TWO     ,'2'          );
    BiDeviceMatchKey(BIKEY_THREE   ,'3'          );
    BiDeviceMatchKey(BIKEY_FOUR    ,'4'          );
    BiDeviceMatchKey(BIKEY_FIVE    ,'5'          );
    BiDeviceMatchKey(BIKEY_SIX     ,'6'          );
    BiDeviceMatchKey(BIKEY_SEVEN   ,'7'          );
    BiDeviceMatchKey(BIKEY_EIGHT   ,'8'          );
    BiDeviceMatchKey(BIKEY_NINE    ,'9'          );
    BiDeviceMatchKey(BIKEY_A       ,'A'          );
    BiDeviceMatchKey(BIKEY_B       ,'B'          );
    BiDeviceMatchKey(BIKEY_C       ,'C'          );
    BiDeviceMatchKey(BIKEY_D       ,'D'          );
    BiDeviceMatchKey(BIKEY_E       ,'E'          );
    BiDeviceMatchKey(BIKEY_F       ,'F'          );
    BiDeviceMatchKey(BIKEY_G       ,'G'          );
    BiDeviceMatchKey(BIKEY_H       ,'H'          );
    BiDeviceMatchKey(BIKEY_I       ,'I'          );
    BiDeviceMatchKey(BIKEY_J       ,'J'          );
    BiDeviceMatchKey(BIKEY_K       ,'K'          );
    BiDeviceMatchKey(BIKEY_L       ,'L'          );
    BiDeviceMatchKey(BIKEY_M       ,'M'          );
    BiDeviceMatchKey(BIKEY_N       ,'N'          );
    BiDeviceMatchKey(BIKEY_O       ,'O'          );
    BiDeviceMatchKey(BIKEY_P       ,'P'          );
    BiDeviceMatchKey(BIKEY_Q       ,'Q'          );
    BiDeviceMatchKey(BIKEY_R       ,'R'          );
    BiDeviceMatchKey(BIKEY_S       ,'S'          );
    BiDeviceMatchKey(BIKEY_T       ,'T'          );
    BiDeviceMatchKey(BIKEY_U       ,'U'          );
    BiDeviceMatchKey(BIKEY_V       ,'V'          );
    BiDeviceMatchKey(BIKEY_W       ,'W'          );
    BiDeviceMatchKey(BIKEY_X       ,'X'          );
    BiDeviceMatchKey(BIKEY_Y       ,'Y'          );
    BiDeviceMatchKey(BIKEY_Z       ,'Z'          );
    BiDeviceMatchKey(BIKEY_SPACE   ,VK_SPACE     );
    BiDeviceMatchKey(BIKEY_ESC     ,VK_ESCAPE    );
    BiDeviceMatchKey(BIKEY_TAB     ,VK_TAB       );
    BiDeviceMatchKey(BIKEY_CTRL    ,VK_CONTROL   );
    BiDeviceMatchKey(BIKEY_SHIFT   ,VK_SHIFT     );
    BiDeviceMatchKey(BIKEY_ALT     ,0            );
    BiDeviceMatchKey(BIKEY_BS      ,VK_BACK      );
    BiDeviceMatchKey(BIKEY_RET     ,VK_RETURN    );
    BiDeviceMatchKey(BIKEY_HOME    ,VK_HOME      );
    BiDeviceMatchKey(BIKEY_DEL     ,VK_DELETE    );
    BiDeviceMatchKey(BIKEY_UP      ,VK_UP        );
    BiDeviceMatchKey(BIKEY_DOWN    ,VK_DOWN      );
    BiDeviceMatchKey(BIKEY_LEFT    ,VK_LEFT      );
    BiDeviceMatchKey(BIKEY_RIGHT   ,VK_RIGHT     );
    BiDeviceMatchKey(BIKEY_TEN0    ,0            );
    BiDeviceMatchKey(BIKEY_TEN1    ,0            );
    BiDeviceMatchKey(BIKEY_TEN2    ,0            );
    BiDeviceMatchKey(BIKEY_TEN3    ,0            );
    BiDeviceMatchKey(BIKEY_TEN4    ,0            );
    BiDeviceMatchKey(BIKEY_TEN5    ,0            );
    BiDeviceMatchKey(BIKEY_TEN6    ,0            );
    BiDeviceMatchKey(BIKEY_TEN7    ,0            );
    BiDeviceMatchKey(BIKEY_TEN8    ,0            );
    BiDeviceMatchKey(BIKEY_TEN9    ,0            );
    BiDeviceMatchKey(BIKEY_TENMUL  ,'*'          );
    BiDeviceMatchKey(BIKEY_TENDIV  ,'/'          );
    BiDeviceMatchKey(BIKEY_TENPLUS ,'+'          );
    BiDeviceMatchKey(BIKEY_TENMINUS,'-'          );
    BiDeviceMatchKey(BIKEY_TENEQUAL,'='          );
    BiDeviceMatchKey(BIKEY_TENDOT  ,'.'          );

	nKeyBuf=0;
}

static void AddKeyToBuffer(int wParam)
{
	if(nKeyBuf<MAX_KEYBUF && 0<=wParam && wParam<0x80)
	{
		keyBuf[nKeyBuf]=VkToKc[wParam];
		nKeyBuf++;
	}
}

void BiUpdateDevice(void)
{
	int i;
	MSG msg;

	while(PeekMessage(&msg,0,0,0,PM_REMOVE))
	{
		if(msg.message==WM_KEYDOWN)
		{
			AddKeyToBuffer(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	for(i=0; i<256; i++)
	{
		pKbs[i]=kbs[i];
	}
	GetKeyboardState((LPBYTE)&kbs);
}

void BiMouse(int *l,int *m,int *r,long *mx,long *my)
{
	POINT cur;
	GetCursorPos(&cur);
	ScreenToClient(BiWnd,&cur);
	*mx=cur.x;
	*my=cur.y;

	*l =lbt;
	*m =BI_OFF;
	*r =rbt;
}

int BiKey(int kcode)
{
	if((kbs[KcToVk[kcode]]&0x80)!=0)
	{
		return BI_ON;
	}
	else
	{
		return BI_OFF;
	}
}

int BiInkey(void)
{
	int i;
	char key;
	if(nKeyBuf>0)
	{
		key=keyBuf[0];
		for(i=0; i<nKeyBuf-1; i++)
		{
			keyBuf[i]=keyBuf[i+1];
		}
		nKeyBuf--;
		return (int)key;
	}
	else
	{
		return 0;
	}
}






/*****************************************************************************

   Memory Management

*****************************************************************************/

void BiMemoryInitialize(void)
{
	/* Nothing To Do */
}

void *BiMalloc(size_t sz)
{
	return malloc(sz);
}

void BiFree(void *ptr)
{
	free(ptr);
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

	if(neo[0]=='\\' && neo[1]=='\\')
	{
		/* Support network shared file */
		return;
	}

	for(i=0; neo[i]!=0; i++)
	{
		if((i!=1 && neo[i]==':') || neo[i]=='\\')
		{
			neo[i]='/';
		}
	}
}



