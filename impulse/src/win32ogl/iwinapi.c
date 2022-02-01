/* Machine Depending Module : Windows95 DIB Version */
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../impulse.h"
#include "../iutil.h"

#include <windowsx.h>

#include "wimpulse.h"



/*****************************************************************************

   Setting

*****************************************************************************/

extern char BiWindowName[];

LRESULT CALLBACK BiWindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara);

HWND BiWnd=NULL;
HFONT BiWinFont=NULL;
HPALETTE BiPal=NULL;

static HDC WndDC=NULL;
static long WinX,WinY;
static int BitPerPix;
static int BiConfirmQuit=BI_ON;


/* New Method to link with HOTAS.LIB */
HMENU BiMenu=NULL;
static int BiDefaultExtraWindowFunc(HWND w,UINT msg,WPARAM wp,LPARAM lp)
{
	return BI_ERR;
}
static int (*BiExtraWindowFunc)(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
                              =BiDefaultExtraWindowFunc;

void BiWinSetExtraWindowFunc(int (*func)(HWND,UINT,WPARAM,LPARAM))
{
	BiExtraWindowFunc=func;
}



/*****************************************************************************/
static void BiSetWindowLeftUp(int *lupX,int *lupY,int sizX,int sizY);
static void BiRegisterWindowClass(char *cl);
static void BiAdjustWindowSize(int *lupX,int *lupY,int *sizX,int *sizY);
static void BiGenerateDrawBuffer(void);
static void BiCreateWindow(long x,long y,long wx,long wy,char cn[],HMENU menu);
static void BiCheckSystemMenu(int bpp);
/*****************************************************************************/
static int BiFatalFlag=BI_OFF;
int BiFatal(void)
{
	return BiFatalFlag;
}

#define WINSTYLE WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN
//#define WINSTYLE WS_OVERLAPPED|WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME
#define WINCLASS "BIAPPCLASS"
#define WINNAME BiWindowName

#define WINSMENU_8BPP  20000
#define WINSMENU_16BPP 20001
#define WINSMENU_DEFLT 20002

void BiOpenWindow(long wx,long wy)
{
	int x,y;
	BiRegisterWindowClass(WINCLASS);
	BiSetWindowLeftUp(&x,&y,wx,wy);
	BiAdjustWindowSize(&x,&y,&wx,&wy);
	BiCreateWindow(x,y,wx,wy,WINCLASS,BiMenu);
}

void BiOpenWindowEx(long x,long y,long wx,long wy)
{
	BiRegisterWindowClass(WINCLASS);
	BiAdjustWindowSize(&x,&y,&wx,&wy);
	BiCreateWindow(x,y,wx,wy,WINCLASS,BiMenu);
}

void BiCloseWindow(void)
{
	ShowWindow(BiWnd,SW_HIDE);
	ReleaseDC(BiWnd,WndDC);
	BiWinUnplug();
}

static void BiCreateWindow(long x,long y,long wx,long wy,char cn[],HMENU menu)
{
	int lupX,lupY,sizX,sizY;
	HMENU sysmenu;
	HINSTANCE module;

	sizX=(int)wx;
	sizY=(int)wy;
	lupX=(int)x;
	lupY=(int)y;
	WinX=sizX;
	WinY=sizY;

	module=GetModuleHandle(NULL);
	BiWnd=CreateWindow
	    (cn,WINNAME,WINSTYLE,lupX,lupY,sizX,sizY,NULL,menu,module,NULL);
	if(BiWnd==NULL)
	{
		BiFatalFlag=BI_ON;
		return;
	}

	WndDC=GetDC(BiWnd);

	BitPerPix=0;
	BiPal=BiWinCreatePalette(WndDC);
	BiWinPlug(WndDC,WinX,WinY,BitPerPix);

	sysmenu=GetSystemMenu(BiWnd,FALSE);
	AppendMenu(sysmenu,MF_STRING,WINSMENU_8BPP ,"256 Colors");
	AppendMenu(sysmenu,MF_STRING,WINSMENU_16BPP,"32K Colors");
	AppendMenu(sysmenu,MF_STRING,WINSMENU_DEFLT,"Same as display");

	BiCheckSystemMenu(BitPerPix);

	ShowWindow(BiWnd,SW_SHOWNORMAL);
	SetBkMode(WndDC,TRANSPARENT);
	SetPolyFillMode(WndDC,ALTERNATE);
	SelectPalette(WndDC,BiPal,FALSE);
	RealizePalette(WndDC);
}

static void BiCheckSystemMenu(int bpp)
{
	HMENU sysmenu;
	sysmenu=GetSystemMenu(BiWnd,FALSE);
	switch(bpp)
	{
	case 8:
		CheckMenuItem(sysmenu,WINSMENU_8BPP ,MF_CHECKED);
		CheckMenuItem(sysmenu,WINSMENU_16BPP,MF_UNCHECKED);
		CheckMenuItem(sysmenu,WINSMENU_DEFLT,MF_UNCHECKED);
		break;
	case 16:
		CheckMenuItem(sysmenu,WINSMENU_8BPP ,MF_UNCHECKED);
		CheckMenuItem(sysmenu,WINSMENU_16BPP,MF_CHECKED);
		CheckMenuItem(sysmenu,WINSMENU_DEFLT,MF_UNCHECKED);
		break;
	default:
		CheckMenuItem(sysmenu,WINSMENU_8BPP ,MF_UNCHECKED);
		CheckMenuItem(sysmenu,WINSMENU_16BPP,MF_UNCHECKED);
		CheckMenuItem(sysmenu,WINSMENU_DEFLT,MF_CHECKED);
		break;
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
	HINSTANCE module;

	module=GetModuleHandle(NULL);

	wCls.style=CS_OWNDC|CS_BYTEALIGNWINDOW;
	wCls.lpfnWndProc=BiWindowFunc;
	wCls.cbClsExtra=0;
	wCls.cbWndExtra=0;
	wCls.hInstance=module;
	if((wCls.hIcon=LoadIcon(module,"HOTASICON"))==NULL)
	{
		wCls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	}
	wCls.hCursor=LoadCursor(NULL,IDC_ARROW);
	wCls.hbrBackground=GetStockObject(WHITE_BRUSH);
	wCls.lpszMenuName=NULL;
	wCls.lpszClassName=cl;
	if(!RegisterClass(&wCls))
	{
		exit(1);
	}
}

static void BiAdjustWindowSize(int *lupX,int *lupY,int *sizX,int *sizY)
{
	RECT rc;
	rc.left  =*lupX;
	rc.top   =*lupY;
	rc.right =(unsigned long)(*lupX+*sizX-1);
	rc.bottom=(unsigned long)(*lupY+*sizY-1);
	AdjustWindowRect(&rc,WINSTYLE,FALSE);
	*lupX  =rc.left;
	*lupY  =rc.top;
	*sizX  =rc.right-rc.left+1;
	*sizY  =rc.bottom-rc.top+1;
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
	int i;

	lbt=BI_OFF;
	rbt=BI_OFF;

	for(i=0; i<256; i++)
	{
		kbs[i]=0;
		pKbs[i]=0;
	}

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
    BiDeviceMatchKey(BIKEY_TEN0    ,VK_NUMPAD0   );
    BiDeviceMatchKey(BIKEY_TEN1    ,VK_NUMPAD1   );
    BiDeviceMatchKey(BIKEY_TEN2    ,VK_NUMPAD2   );
    BiDeviceMatchKey(BIKEY_TEN3    ,VK_NUMPAD3   );
    BiDeviceMatchKey(BIKEY_TEN4    ,VK_NUMPAD4   );
    BiDeviceMatchKey(BIKEY_TEN5    ,VK_NUMPAD5   );
    BiDeviceMatchKey(BIKEY_TEN6    ,VK_NUMPAD6   );
    BiDeviceMatchKey(BIKEY_TEN7    ,VK_NUMPAD7   );
    BiDeviceMatchKey(BIKEY_TEN8    ,VK_NUMPAD8   );
    BiDeviceMatchKey(BIKEY_TEN9    ,VK_NUMPAD9   );
    BiDeviceMatchKey(BIKEY_TENMUL  ,VK_MULTIPLY  );
    BiDeviceMatchKey(BIKEY_TENDIV  ,VK_DIVIDE    );
    BiDeviceMatchKey(BIKEY_TENPLUS ,VK_ADD       );
    BiDeviceMatchKey(BIKEY_TENMINUS,VK_SUBTRACT  );
    BiDeviceMatchKey(BIKEY_TENEQUAL,'='          );
    BiDeviceMatchKey(BIKEY_TENDOT  ,VK_DECIMAL   );

	BiDeviceMatchKey(BIKEY_INS,VK_INSERT);
	BiDeviceMatchKey(BIKEY_END,VK_END);
	BiDeviceMatchKey(BIKEY_PGUP,VK_PRIOR);
	BiDeviceMatchKey(BIKEY_PGDOWN,VK_NEXT);
	BiDeviceMatchKey(BIKEY_COMMA,0);
	BiDeviceMatchKey(BIKEY_DOT,0);
	BiDeviceMatchKey(BIKEY_SLASH,0);
	BiDeviceMatchKey(BIKEY_BSLASH,0);
	BiDeviceMatchKey(BIKEY_LBRACKET,0);
	BiDeviceMatchKey(BIKEY_RBRACKET,0);

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

#define BI_MAX_NUM_SIMUL_DLG 16
int nBiDlgRegistered=0;
HWND BiDlgRegistered[BI_MAX_NUM_SIMUL_DLG];

void BiUpdateDevice(void)
{
	int i;
	MSG msg;

	if(nBiDlgRegistered>0)
	{
		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			for(i=0; i<nBiDlgRegistered; i++)
			{
				if(IsDialogMessage(BiDlgRegistered[i],&msg)==TRUE)
				{
					goto NODISPATCH;
				}
			}

			if(msg.message==WM_KEYDOWN)
			{
				AddKeyToBuffer(msg.wParam);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		NODISPATCH:
			;
		}
	}
	else
	{
		while(PeekMessage(&msg,BiWnd,0,0,PM_REMOVE))
		{
			if(msg.message==WM_KEYDOWN)
			{
				AddKeyToBuffer(msg.wParam);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	for(i=0; i<256; i++)
	{
		pKbs[i]=kbs[i];
	}
	GetKeyboardState((LPBYTE)kbs);
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

   Window Callback Function

*****************************************************************************/

static void LparamToCoord(int *x,int *y,LPARAM lPara);

LRESULT CALLBACK BiWindowFunc(HWND w,UINT msg,WPARAM wPara,LPARAM lPara)
{
	if((*BiExtraWindowFunc)(w,msg,wPara,lPara)!=BI_OK)
	{
		switch(msg)
		{
		case WM_CREATE:
			break;
		case WM_PAINT:
			BiSwapBuffers();
			return DefWindowProc(w,msg,wPara,lPara);
		case WM_QUERYNEWPALETTE:
		case WM_PALETTECHANGED:
			if(WndDC!=NULL && BiPal!=NULL)
			{
				SelectPalette(WndDC,BiPal,FALSE);
				RealizePalette(WndDC);
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
		case WM_CLOSE:
			if(BiConfirmQuit==BI_OFF ||
			   MessageBox(w,
			              "Really Quit? You may lose some data!",
			              "QUIT?",
			              MB_YESNO)==IDYES)
			{
				return DefWindowProc(w,msg,wPara,lPara);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			BiFatalFlag=1;
			exit(1);
			break;
		case WM_SIZE:
			BiWinPlug(WndDC,LOWORD(lPara),HIWORD(lPara),BitPerPix);
			WinX=LOWORD(lPara);
			WinY=HIWORD(lPara);
			RedrawWindow(w,NULL,NULL,RDW_INTERNALPAINT);
			break;
		case WM_SYSCOMMAND:
			switch(wPara)
			{
			default:
				return DefWindowProc(w,msg,wPara,lPara);
			case WINSMENU_8BPP:
				BitPerPix=8;
				BiCheckSystemMenu(BitPerPix);
				BiWinPlug(WndDC,WinX,WinY,BitPerPix);
				break;
			case WINSMENU_16BPP:
				BitPerPix=16;
				BiCheckSystemMenu(BitPerPix);
				BiWinPlug(WndDC,WinX,WinY,BitPerPix);
				break;
			case WINSMENU_DEFLT:
				BitPerPix=0;
				BiCheckSystemMenu(BitPerPix);
				BiWinPlug(WndDC,WinX,WinY,BitPerPix);
				break;
			}
			break;
		default:
			return DefWindowProc(w,msg,wPara,lPara);
		}
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
int BiWinKeyCodeToVirtualKey(int bikey)
{
	return KcToVk[bikey];
}

int BiWinVirtualKeyToKeyCode(int vk)
{
	return VkToKc[vk];
}




/***************************************************************************

   To cooperate with Win32 dialogs

***************************************************************************/

void BiWinNoQuitConfirm(void)
{
	BiConfirmQuit=BI_OFF;
}

int BiWinRegisterDialog(HWND dlg)
{
	if(nBiDlgRegistered<BI_MAX_NUM_SIMUL_DLG)
	{
		BiDlgRegistered[nBiDlgRegistered]=dlg;
		nBiDlgRegistered++;
		return BI_OK;
	}
	return BI_ERR;
}

int BiWinUnregisterDialog(HWND dlg)
{
	int i,j;
	for(i=0; i<nBiDlgRegistered; i++)
	{
		if(dlg==BiDlgRegistered[i])
		{
			for(j=i; j<nBiDlgRegistered; j++)
			{
				BiDlgRegistered[j]=BiDlgRegistered[j+1];
			}
			nBiDlgRegistered--;
			return BI_OK;
		}
	}
	return BI_ERR;
}




/***************************************************************************

   Anti F______ "WinMain" Function.

***************************************************************************/
extern main(int ac,char *av[]);

int PASCAL WinMain(HINSTANCE inst,HINSTANCE dumb,LPSTR param,int show)
{
	int ac;
	char *av[128],tmp[256],prog[260];

	strcpy(prog,"Unknown");
	GetModuleFileName(inst,prog,260);

	strncpy(tmp,param,256);
	av[0]=prog;
	BiArguments(&ac,av+1,126,tmp);
	return main(ac+1,av);
}

