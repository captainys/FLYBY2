/* Machine Depending Module : Windows32API Version */
#include <windows.h>
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

#include "wimpulse.h"


/* External Function in iopengl.c */
#define BIOGL_MAX_VTX_PER_POLYG 128
#define BI_GL_FONT_BITMAP_BASE 1000
extern void BiInitOpenGL(long wid,long hei);

/*****************************************************************************/
static void BiSetPixelFormat(HDC dc);
static unsigned char YsPalVal(unsigned long n,unsigned bit,unsigned sft);
static HPALETTE BiCreatePalette(HDC dc);
/*****************************************************************************/


extern HWND BiWnd;
HDC BiWndDC=NULL;


HGLRC BiGlRC=NULL;   // =NULL is added on 2001/06/10

static HPALETTE glPal;

long BiWinX,BiWinY;


void BiWinPlug(HDC dc,long wid,long hei,int bitPerPix)
{
	HDC glDC;

	if(BiGlRC==NULL)
	{
		BiWinCreatePalette(dc);
	}

	BiWinX=wid;
	BiWinY=hei;
	wglMakeCurrent(dc,BiGlRC);
	glViewport(0,0,wid,hei);

	glDC=wglGetCurrentDC();
	wglUseFontBitmaps(glDC,0,255,BI_GL_FONT_BITMAP_BASE);  /* 2003/05/09 */
	glListBase(BI_GL_FONT_BITMAP_BASE);                    /* 2003/05/09 */

	BiWndDC=dc;
}

void BiWinUnplug(void)
{
	wglMakeCurrent(NULL,NULL);

	if(BiGlRC!=NULL)
	{
		wglDeleteContext(BiGlRC);
		BiGlRC=NULL;
	}

	if(glPal!=NULL)
	{
		SelectPalette(BiWndDC,(HPALETTE)GetStockObject(DEFAULT_PALETTE),TRUE);
		DeleteObject(glPal);
		glPal=NULL;
	}

	ReleaseDC(BiWnd,BiWndDC);
	BiWndDC=NULL;
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
	BiSetPixelFormat(dc);
	BiGlRC=wglCreateContext(dc);
	if(BiGlRC!=NULL)
	{
		if(wglMakeCurrent(dc,BiGlRC)==TRUE)
		{
			HDC glDC;

			BiInitOpenGL(32,32);  /* Temporarily, assume 32x32 screen */

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

void BiSwapBuffers(void)
{
	HDC dc;
	glFlush();
	dc=wglGetCurrentDC();
	SwapBuffers(dc);
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
		MessageBox(NULL,"BiGetColor is Under Construction.",NULL,MB_APPLMODAL);
	}
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
	static int first=BI_ON;
	if(first==BI_ON)
	{
		first=BI_OFF;
		MessageBox(NULL,"BiGetColorArray is Under Construction.",NULL,MB_APPLMODAL);
	}
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
		16,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		32,
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
	printf("Pixel format not found.\n");
	DescribePixelFormat(dc,pfm,sizeof(pfd),&pfd);
	if((pfd.dwFlags&PFD_GENERIC_ACCELERATED)!=0)
	{
		printf("Accelerated by the hardward.\n");
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
