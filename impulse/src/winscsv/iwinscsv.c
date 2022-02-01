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
#include "../impulse.h"
#include "../iutil.h"

#include <windowsx.h>
#include <scrnsave.h>
#include "iwildcat.h"


/* for compatibility */
#define BI_MAX_NUM_SIMUL_DLG 16
int nBiDlgRegistered=0;
HWND BiDlgRegistered[BI_MAX_NUM_SIMUL_DLG];


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

extern int ScSvMain(void);
extern int ScSvInitialize(void);
extern int ScSvTerminate(void);

static void LparamToCoord(int *x,int *y,LPARAM lPara);

HINSTANCE BiCInst;
HANDLE thread=NULL;
int threadId;

LRESULT CALLBACK ScreenSaverProc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	HBRUSH BlackBrush;
	RECT WinRect;

	switch(msg)
	{
	case WM_CREATE:
		if(thread!=NULL)
		{
			TerminateThread(thread,0);
			ScSvTerminate();
			thread=NULL;
		}
		if(nWaveDevs==0)
		{
			SaveWaveVolume();
		}
		strcpy(szAppName,"BI SCREEN SAVER");
		BiOpenWindowForScreenSaver(w);
		strcpy(szAppName,BiWindowName);
		BiCInst=hMainInstance;
		break;
	case WM_ERASEBKGND:
		if(thread==NULL)
		{
			BlackBrush=GetStockObject(BLACK_BRUSH);
			GetClientRect(w,&WinRect);
			FillRect((HDC)wPara,&WinRect,BlackBrush);
			DeleteObject(BlackBrush);

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
		}
		return 0L;
	case WM_DESTROY:
		if(thread!=NULL)
		{
			TerminateThread(thread,0);
			ScSvTerminate();
			thread=NULL;
		}
		if(nWaveDevs>0)
		{
			RestoreWaveVolume();
		}
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

HWND BiWnd;

static long winX,winY;

static WCBMP dBuf;
static HPALETTE WinGPal;

static int BiFatalFlag=BI_OFF;
/* depend */int BiFatal(void)
{
	return BiFatalFlag;
}

static void BiGenerateDrawBuffer(void);

void BiOpenWindowForScreenSaver(HWND w)
{
	int lupX,lupY,sizX,sizY;
	int sizeLimitX,sizeLimitY;
	RECT WinRect;
	HDC BiWndDc;

	GetWindowRect(w,&WinRect);
	sizX=WinRect.right-WinRect.left+1;
	sizY=WinRect.bottom-WinRect.top+1;

	GetSizeLimit(&sizeLimitX,&sizeLimitY);

	sizX=BiSmaller(sizX,sizeLimitX);
	sizY=BiSmaller(sizY,sizeLimitY);
	lupX=0;
	lupY=0;
	winX=sizX;
	winY=sizY;

	BiWnd=w;

	BiWndDc=GetDC(BiWnd);
	BiGenerateDrawBuffer();

	SetBkMode(BiWndDc,TRANSPARENT);
	SetPolyFillMode(BiWndDc,ALTERNATE);

	SelectPalette(BiWndDc,WinGPal,FALSE);
	RealizePalette(BiWndDc);

	ReleaseDC(BiWnd,BiWndDc);
}

void BiOpenWindow(long wx,long wy)
{
	/* Nothing to do here */
}

static HPALETTE BiCreatePalette(HDC dc);

static void BiGenerateDrawBuffer(void)
{
	HDC BiWndDc;

	BiWndDc=GetDC(BiWnd);

	WinGPal=BiCreatePalette(BiWndDc);
	WcSetWinGPalette(WinGPal);
	WcCreateBmp(&dBuf,BiWndDc,(int)winX,(int)winY,0);

	ReleaseDC(BiWnd,BiWndDc);
}

static HPALETTE BiCreatePalette(HDC WndDc)
{
	int i,n,r,g,b;
	int base,gray;
	HPALETTE org,neo;
	LOGPALETTE *tmp;
	PALETTEENTRY *ent;

	tmp=(LOGPALETTE *)malloc(sizeof(WORD)*2+sizeof(PALETTEENTRY)*256);
	if(tmp==NULL)
	{
		MessageBox(NULL,"Malloc Error (BiCreatePalette)",NULL,MB_APPLMODAL);
		exit(1);
	}

	tmp->palVersion=0x300;
	tmp->palNumEntries=256;
	ent=tmp->palPalEntry;

	org=CreateHalftonePalette(WndDc);

	n=GetPaletteEntries(org,0,256,ent);

	base=(n<40 ? n : 40);

	for(r=0; r<6; r++)
	{
		for(g=0; g<6; g++)
		{
			for(b=0; b<6; b++)
			{
				i=g*36+r*6+b+base;
				ent[i].peRed  =r*51;
				ent[i].peGreen=g*51;
				ent[i].peBlue =b*51;
				ent[i].peFlags=0;
			}
		}
	}

	base+=216;
	gray=256-base;
	for(i=0; i<gray; i++)
	{
		ent[base+i].peRed  =i*255/gray;
		ent[base+i].peGreen=i*255/gray;
		ent[base+i].peBlue =i*255/gray;
		ent[base+i].peFlags=0;
	}

	DeleteObject(org);
	neo=CreatePalette(tmp);
	free(tmp);

	if(neo==NULL)
	{
		MessageBox(NULL,"Can't Create Palette (BiCreatePalette)",NULL,MB_APPLMODAL);
		exit(1);
	}
	return neo;
}

static void BiResizeDrawBuffer(int w,int h)
{
	HDC BiWndDc;

	w=BiLarger(16,w);
	h=BiLarger(16,h);

	winX=w;
	winY=h;
	WcDeleteBmp(&dBuf);

	BiWndDc=GetDC(BiWnd);
	WcCreateBmp(&dBuf,BiWndDc,w,h,0);
	ReleaseDC(BiWnd,BiWndDc);
}

void BiCloseWindow(void)
{
	WcDeleteBmp(&dBuf);
}

void BiGetWindowSize(long *wid,long *hei)
{
	*wid = winX;
	*hei = winY;
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
	/* WinGBitBlt(BiWndDc,0,0,(int)winX,(int)winY,dBuf.dc,0,0); */
	long lupX,lupY,sizX,sizY;
	RECT WinRect;
	HDC BiWndDc;

	GetWindowRect(BiWnd,&WinRect);
	sizX=WinRect.right-WinRect.left+1;
	sizY=WinRect.bottom-WinRect.top+1;

	lupX=(sizX-winX)/2;
	lupY=(sizY-winY)/2;
	BiWndDc=GetDC(BiWnd);
	BitBlt(BiWndDc,lupX,lupY,(int)winX,(int)winY,dBuf.dc,0,0,SRCCOPY);
	ReleaseDC(BiWnd,BiWndDc);
}

#define COLOR(ptr) RGB((ptr)->r,(ptr)->g,(ptr)->b)

static HPEN orgPen,neoPen;
static HBRUSH orgBrs,neoBrs;

static void BiSetLineColor(BICOLOR *ptr)
{
	neoPen=CreatePen(PS_SOLID,1,COLOR(ptr));
	orgPen=SelectObject(dBuf.dc,neoPen);
	orgBrs=SelectObject(dBuf.dc,GetStockObject(NULL_BRUSH));
}

static void BiEndLineColor(void)
{
	SelectObject(dBuf.dc,orgPen);
	SelectObject(dBuf.dc,orgBrs);
	DeleteObject(neoPen);
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

   Draw

*****************************************************************************/
void BiClearScreenD(void)
{
	WCCOLOR col={0,0,0};
	WcRect(&dBuf,0,0,(int)(winX-1),(int)(winY-1),&col);
}

static BIPOINTS orgLup,orgRdw;
static void BiWinSetClipScrnRect(void)
{
	BIPOINTS lup,rdw;
	BiSetPointS(&lup,0,0);
	BiSetPointS(&rdw,winX-1,winY-1);
	BiGetClipScreenFrame(&orgLup,&orgRdw);
	BiSetClipScreenFrame(&lup,&rdw);
}

static void BiWinResetClipScrnRect(void)
{
	BiSetClipScreenFrame(&orgLup,&orgRdw);
}

#define BIINBOUND(a) (0<=(a)->x && (a)->x<winX && 0<=(a)->y && (a)->y<winY)

void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BIPOINTS a1,a2;
	BiWinSetClipScrnRect();

	if(BIINBOUND(s1) && BIINBOUND(s2))
	{
		WcLine(&dBuf,(int)s1->x,(int)s1->y,(int)s2->x,(int)s2->y,(WCCOLOR *)c);
	}
	else if(BiClipLineScrn(&a1,&a2,s1,s2)==BI_IN)
	{
		WcLine(&dBuf,(int)a1.x,(int)a1.y,(int)a2.x,(int)a2.y,(WCCOLOR *)c);
 	}
	BiWinResetClipScrnRect();
}

void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
	if(BIINBOUND(s))
	{
		WcPset(&dBuf,s->x,s->y,(WCCOLOR *)c);
	}

/*	BIPOINTS t1,t2;

	t1=*s;
	BiSetPointS(&t2,t1.x+1,t1.y+1);
	if(BIINBOUND(&t1) && BIINBOUND(&t2))
	{
		WcPset(&dBuf,(int)t1.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&dBuf,(int)t2.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&dBuf,(int)t1.x,(int)t2.y,(WCCOLOR *)c);
		WcPset(&dBuf,(int)t2.x,(int)t2.y,(WCCOLOR *)c);
	} */
}

void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	if(BIINBOUND(s))
	{
		if(radx==rady)
		{
			WcSetCircleRegion(&dBuf,(WCPNT *)s,radx);
		}
		else
		{
			WcSetEllipseRegion(&dBuf,(WCPNT *)s,radx,rady);
		}
		WcPaintRegion(&dBuf,(WCCOLOR *)c);
	}
}

void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
	unsigned long stk;
	int i,nsNew;
	BIPOINTS *sNew,*sP;

	sP=s;
	for(i=0; i<ns; i++)
	{
		if(!BIINBOUND(&s[i]))
		{
			goto CLIPPING;
		}
	}
	WcSetRegion(&dBuf,ns,(WCPNT *)s);
	WcPaintRegion(&dBuf,(WCCOLOR *)c);
	return;

CLIPPING:
	sNew=BiPushTmpStack(&stk,sizeof(BIPOINTS)*ns*4);
	BiWinSetClipScrnRect();
	if(sNew!=NULL && BiClipPolygScrn(&nsNew,sNew,ns,s)==BI_IN)
	{
		WcSetRegion(&dBuf,nsNew,(WCPNT *)sNew);
		WcPaintRegion(&dBuf,(WCCOLOR *)c);
	}
	BiWinResetClipScrnRect();
	BiPopTmpStack(stk);
}

void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BIPOINTS a1,a2;

	if(BIINBOUND(s1) && BIINBOUND(s2))
	{
		WcRect(&dBuf,(int)s1->x,(int)s1->y,(int)s2->x,(int)s2->y,(WCCOLOR *)c);
	}
	else
	{
		a1.x=BiSmaller(s1->x,s2->x);
		a1.y=BiSmaller(s1->y,s2->y);
		a2.x=BiLarger(s1->x,s2->x);
		a2.y=BiLarger(s1->y,s2->y);
		if(a1.x>=winX || a1.y>=winY || a2.x<0 || a2.y<0)
		{
			return;
		}
		a1.x=BiLarger(a1.x,0);
		a1.y=BiLarger(a1.y,0);
		a2.x=BiSmaller(a2.x,winX-1);
		a2.y=BiSmaller(a2.y,winY-1);
		WcRect(&dBuf,(int)a1.x,(int)a1.y,(int)a2.x,(int)a2.y,(WCCOLOR *)c);
	}
}

void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
	BIPOINTS p1,p2;
	switch(mkType)
	{
	case BIMK_RECT:
		BiSetPointS(&p1, s->x-2,s->y-2);
		BiSetPointS(&p2, s->x+2,s->y+2);
		BiDrawRect2D(&p1,&p2,c);
		break;
	case BIMK_CIRCLE:
		BiDrawCircle2D(s,3,3,c);
		break;
	case BIMK_CROSS:
		BiSetPointS(&p1, s->x-4,s->y-4);
		BiSetPointS(&p2, s->x+4,s->y+4);
		BiDrawLine2D(&p1,&p2,c);
		BiSetPointS(&p1, s->x+4,s->y-4);
		BiSetPointS(&p2, s->x-4,s->y+4);
		BiDrawLine2D(&p1,&p2,c);
		break;
	}
}

void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
	if(BIINBOUND(s))
	{
		HFONT stk;
		stk=SelectObject(dBuf.dc,GetStockObject(SYSTEM_FIXED_FONT));
		SetTextColor(dBuf.dc,COLOR(c));
		TextOut(dBuf.dc,(int)s->x,(int)s->y-16,(LPCSTR)str,strlen(str));
		SelectObject(dBuf.dc,stk);
	}
}

void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	#define FONTX 8  /* フォントサイズ X */
	#define FONTY 16 /* フォントサイズ Y */
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;

	/* テキストの横文字数と縦文字数を数える */
	sWid=0;
	sHei=0;
	for(i=0; str[i]!=NULL; i++)
	{
		sWid=BiLarger(strlen(str[i]),(unsigned long)sWid);
		sHei++;
	}

	/* 情報を元に、書き出し位置を決める */
	switch(h)
	{
	default:
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
	default:
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

		v2.x=(long)((real)radx*BiCos(i*2048));
		v2.y=(long)((real)rady*BiSin(i*2048));

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
		MessageBox
			(NULL,"BiGetColorArray is Under Construction.",NULL,MB_APPLMODAL);
	}
}





/*****************************************************************************

   HOTAS Library Linkage

   BiWinCreateWindow and BiWindowFunc exists just for linking.

*****************************************************************************/
void BiWinCreateWindow(long wx,long wy,char cn[],HMENU menu)
{
	/* Nothing should be done. because this function will never be
	called in Screen Saver application */
}

LRESULT CALLBACK BiWindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	/* Nothing should be done. because this function will never be
	called in Screen Saver application */
	return 0L;
}

int BiWinKeyCodeToVirtualKey(int bikey)
{
	return KcToVk[bikey];
}

int BiWinVirtualKeyToKeyCode(int vk)
{
	return VkToKc[vk];
}


/*****************************************************************************

   WildCat-WinG Acceleration Module Test.

*****************************************************************************/
void BiWinGetWildCatBmp(WCBMP *bmp)
{
	*bmp=dBuf;
}

HDC BiWinGetDrawBufferDC(void)
{
	return dBuf.dc;
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






/* This BiWndDc must not be used! It is the reason why I wrote it at the
   bottom. This BiWndDc is just to cheat HtOpenWindow, that must not be
   used in Screen Saver Program. */
HDC BiWndDc;
