/* Machine Depending Module : Windows3.1 SDK+WinG Version */
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../impulse.h"
#include "../iutil.h"

#include <windowsx.h>
#include <wing.h>

#include "iwildcat.h"



void BiWinCheckMessage(char *msg)
{
	MessageBox(NULL,msg,"Check",MB_APPLMODAL);
}

/*****************************************************************************

   Setting

*****************************************************************************/

extern char BiWindowName[];

LRESULT CALLBACK WindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara);

HWND BiWnd;
HDC BiWndDc;
HINSTANCE BiCInst,BiPInst;

static long winX,winY;

static WCBMP dBuf;
static HPALETTE winGPal;

void BiSetWindowsInstance(HINSTANCE ci,HINSTANCE pi)
{
	BiCInst=ci;
	BiPInst=pi;
}

static int BiFatalFlag=BI_OFF;
/* depend */int BiFatal(void)
{
	return BiFatalFlag;
}


//#define WINSTYLE WS_OVERLAPPED|WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME
#define WINSTYLE WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN
#define WINCLASS "BIAPPCLASS"
#define WINNAME BiWindowName

static void BiConstWindowSize(int *sizX,int *sizY);
static void BiSetWindowLeftUp(int *lupX,int *lupY,int sizX,int sizY);
static void BiRegisterWindowClass(char *cl);
static void BiAdjustWindowSize(int *lupX,int *lupY,int *sizX,int *sizY);
static void BiGenerateDrawBuffer(void);

/* depend */void BiOpenWindow(long wx,long wy)
{
	int lupX,lupY,sizX,sizY;

	sizX=(int)wx;
	sizY=(int)wy;
	BiConstWindowSize(&sizX,&sizY);
	BiSetWindowLeftUp(&lupX,&lupY,sizX,sizY);
	winX=sizX;
	winY=sizY;

	BiGenerateDrawBuffer();

	BiRegisterWindowClass(WINCLASS);
	BiAdjustWindowSize(&lupX,&lupY,&sizX,&sizY);
	BiWnd=CreateWindow(WINCLASS,WINNAME,WINSTYLE,
	                 lupX,lupY,sizX,sizY,NULL,NULL,BiCInst,NULL);

	if(BiWnd==NULL)
	{
		BiFatalFlag=BI_ON;
		return;
	}

	ShowWindow(BiWnd,SW_SHOWNORMAL);

	BiWndDc=GetDC(BiWnd);
	SetBkMode(BiWndDc,TRANSPARENT);
	SetPolyFillMode(BiWndDc,ALTERNATE);

	SelectPalette(BiWndDc,winGPal,FALSE);
	RealizePalette(BiWndDc);
}

static void BiConstWindowSize(int *sizX,int *sizY)
{
	int i;
	long px,py;

	px=*sizX;
	py=*sizY;
	for(i=0; i<32; i++)
	{
		if((1<<i)<=px && px<(1<<(i+1)))
		{
			*sizX=(1<<i);
			*sizY=(int)((py)*(*sizX)/(px));
			return;
		}
	}
}

static void BiSetWindowLeftUp(int *lupX,int *lupY,int sizX,int sizY)
{
	int scnX,scnY;
	scnX=GetSystemMetrics(SM_CXSCREEN);
	scnY=GetSystemMetrics(SM_CYSCREEN);
	*lupX=((scnX-sizX)/2)&~3;
	*lupY=((scnY-sizY)/2)&~3;
}

static void BiRegisterWindowClass(char *cl)
{
	WNDCLASS wCls;
	if(BiPInst==NULL)
	{
		wCls.style=CS_OWNDC|CS_NOCLOSE|CS_BYTEALIGNWINDOW;
		wCls.lpfnWndProc=WindowFunc;
		wCls.cbClsExtra=0;
		wCls.cbWndExtra=0;
		wCls.hInstance=(HINSTANCE)BiCInst;
		if((wCls.hIcon=LoadIcon(BiCInst,"HOTASICON"))==NULL)
		{
			wCls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
		}
		wCls.hCursor=LoadCursor(NULL,IDC_ARROW);
		wCls.hbrBackground=GetStockObject(WHITE_BRUSH);
		wCls.lpszMenuName=NULL;
		wCls.lpszClassName=cl;
		if(!RegisterClass(&wCls))
		{
			BiFatalFlag=BI_ON;
		}
	}
}

static void BiAdjustWindowSize(int *lupX,int *lupY,int *sizX,int *sizY)
{
	RECT rc;
	rc.left  =*lupX;
	rc.top   =*lupY;
	rc.right =*lupX+*sizX-1;
	rc.bottom=*lupY+*sizY-1;
	AdjustWindowRect(&rc,WINSTYLE,FALSE);
	*lupX  =rc.left;
	*lupY  =rc.top;
	*sizX  =rc.right-rc.left+1;
	*sizY  =rc.bottom-rc.top+1;

}

static void BiGenerateDrawBufferPalette(void);
static void BiGenerateDrawBufferBitmap(void);

static void BiGenerateDrawBuffer(void)
{
	winGPal=WinGCreateHalftonePalette();
	WcSetWinGPalette(winGPal);
	WcCreateBmp(&dBuf,(int)winX,(int)winY);
}

static void BiResizeDrawBuffer(int w,int h)
{
	winX=w;
	winY=h;
	WcDeleteBmp(&dBuf);
	WcCreateBmp(&dBuf,w,h);
}

/* depend */void BiCloseWindow(void)
{
	ReleaseDC(BiWnd,BiWndDc);
	WcDeleteBmp(&dBuf);
}

/* depend */void BiGetWindowSize(long *wid,long *hei)
{
	*wid = winX;
	*hei = winY;
}

/* depend */void BiGetStdProjection(BIPROJ *prj)
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
static void BiSetRegionColor(BICOLOR *ptr);
static void BiEndRegionColor(void);

/* depend */void BiSwapBuffers(void)
{
	WinGBitBlt(BiWndDc,0,0,(int)winX,(int)winY,dBuf.dc,0,0);
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

static void BiSetRegionColor(BICOLOR *ptr)
{
	neoBrs=WinGCreateHalftoneBrush(dBuf.dc,COLOR(ptr),WING_DISPERSED_8x8);
	orgBrs=SelectObject(dBuf.dc,neoBrs);
	orgPen=SelectObject(dBuf.dc,GetStockObject(NULL_PEN));
}

static void BiEndRegionColor(void)
{
	SelectObject(dBuf.dc,orgPen);
	SelectObject(dBuf.dc,orgBrs);
	DeleteObject(neoBrs);
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

/* depend */void BiDeviceInitialize(void)
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

/* depend */void BiUpdateDevice(void)
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

/* depend */void BiMouse(int *l,int *m,int *r,long *mx,long *my)
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

/* depend */int BiKey(int kcode)
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

/* depend */int BiInkey(void)
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
/* depend */void BiClearScreenD(void)
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

/* depend */void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
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

/* depend */void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
	BIPOINTS t1,t2;

	t1=*s;
	BiSetPointS(&t2,t1.x+1,t1.y+1);
	if(BIINBOUND(&t1) && BIINBOUND(&t2))
	{
		WcPset(&dBuf,(int)t1.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&dBuf,(int)t2.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&dBuf,(int)t1.x,(int)t2.y,(WCCOLOR *)c);
		WcPset(&dBuf,(int)t2.x,(int)t2.y,(WCCOLOR *)c);
	}
}

/* depend */void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
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

/* depend */void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
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

/* depend */void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
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

/* depend */void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
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

/* depend */void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
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

/* depend */void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
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

   Window Callback Function

*****************************************************************************/

static void LparamToCoord(int *x,int *y,LPARAM lPara);

LRESULT CALLBACK WindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	switch(msg)
	{
	case WM_PAINT:
	case WM_QUERYNEWPALETTE:
	case WM_PALETTECHANGED:
		if(BiWndDc!=NULL)
		{
			SelectPalette(BiWndDc,winGPal,FALSE);
			RealizePalette(BiWndDc);
		}
		return DefWindowProc(w,msg,wPara,lPara);
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		lbt=((wPara & MK_LBUTTON)!=0 ? BI_ON : BI_OFF);
		rbt=((wPara & MK_RBUTTON)!=0 ? BI_ON : BI_OFF);
		LparamToCoord(&mosx,&mosy,lPara);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		BiFatalFlag=1;
		break;
	case WM_SIZE:
		BiResizeDrawBuffer(LOWORD(lPara),HIWORD(lPara));
		break;
	default:
		return DefWindowProc(w,msg,wPara,lPara);
	}
	return 0L;
}

static void LparamToCoord(int *x,int *y,LPARAM lPara)
{
	*x=LOWORD(lPara);
	*y=HIWORD(lPara);
}




/*****************************************************************************

   HOTAS Library Linkage

*****************************************************************************/
/* depend */void BiWinMakeDrawBuffer(long wx,long wy)
{
	winX=wx;
	winY=wy;
	BiGenerateDrawBuffer();
}

/* depend */void BiWinSetPalette(void)
{
	if(BiWndDc!=NULL)
	{
		SelectPalette(BiWndDc,winGPal,FALSE);
		RealizePalette(BiWndDc);
	}
}

/* depend */int BiWinKeyCodeToVirtualKey(int bikey)
{
	return KcToVk[bikey];
}

/* depend */int BiWinVirtualKeyToKeyCode(int vk)
{
	return VkToKc[vk];
}

/* depend */void BiWinResize(long wx,long wy)
{
	BiResizeDrawBuffer((int)wx,(int)wy);
}



/*****************************************************************************

   WildCat-WinG Acceleration Module Test.

*****************************************************************************/
void BiWinGetWildCatBmp(WCBMP *bmp)
{
	*bmp=dBuf;
}




/***************************************************************************

   Anti F______ "WinMain" Function.

***************************************************************************/
extern main(int ac,char *av[]);

int PASCAL WinMain(HINSTANCE BiCi,HINSTANCE BiPi,LPSTR BiCmdStr,int BiCmdShow)
{
	int ac;
	char *av[128],tmp[256];

	if(BiPi)
	{
		return 0;
	}
	else
	{
		BiSetWindowsInstance(BiCi,BiPi);
		strncpy(tmp,BiCmdStr,256);
		av[0]="dummy.exe";
		BiArguments(&ac,av+1,126,tmp);
		return main(ac+1,av);
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
		if((i!=1 && neo[i]==':') || neo[i]=='\\')
		{
			neo[i]='/';
		}
	}
}
