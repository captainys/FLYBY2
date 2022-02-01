// cl simple.cpp user32.lib kernel32.lib gdi32.lib wimpulse32d.lib

#include <windows.h>

#include <impulse.h>


// BLUE IMPULSE PLUG INTERFACE
extern "C"
{
	HPALETTE BiWinCreatePalette(HDC dc);
	void BiWinPlug(HDC dc,long wid,long hei,int bitPerPix);
	void BiWinUnplug(void);
}



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

/********************************************************************/
static int OpenWindow(int lupX,int lupY,int sizX,int sizY);
static int CloseWindow(void);
LRESULT CALLBACK SimpleCallBack(HWND wnd,UINT msg,WPARAM w,LPARAM l);
static void SimplePalette(HDC dc,HPALETTE pal);
static void SimpleMainLoop(void);;
/********************************************************************/

static char cla[256];   // Class Name
static char wnn[256];   // Window Name
static char icn[256];   // Icon Name
static DWORD sty;       // Window Style
static HMENU menu;      // Application Menu
static WNDPROC cbf;     // Window Call Back Fuction
static HWND wnd;        // Window

static HPALETTE pal;    // Palette (if necessary)

static int QuitFlag;    // Program will end if QuitFlag!=0;

static int lbt;         // Mouse Left Button
static int rbt;         // Mouse Right Button
static int msx;         // Mouse X
static int msy;         // Mouse Y
static int szx;         // Window Size X
static int szy;         // Window Size Y


// Keep DC for BLUE IMPULSE PLUG INTERFACE
static HDC dc;
#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];


// For Test
BISRF srf;



int main(int ac,char *av[])
{
	strcpy(cla,"SimpleCLASS");
	strcpy(wnn,"Simple WINDOW APPLICATION");
	strcpy(icn,"");
	sty=WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	menu=NULL;
	cbf=SimpleCallBack;
	pal=NULL;
	QuitFlag==0;

	if(OpenWindow(100,100,640,480)==0)
	{
		// Initialize Blue Impulse As Usual (Except Device and Window)
		BIPROJ prj;

		BiMemoryInitialize();
		BiCalcInitialize();
		BiGraphInitialize(BiWork,BIWORKSIZE);
		BiRenderInitialize();

		BiGetStdProjection(&prj);
		BiSetProjection(&prj);
		BiSetShadMode(BI_ON);

		// BLUE IMPULSE PLUG INTERFACE
		dc=GetDC(wnd);
		HPALETTE pal;
		pal=BiWinCreatePalette(dc);
		BiWinPlug(dc,640,480,0);
		             //      ^
		             //      0->Same As Display
		             //      8->8 bit per pix  16->16 bit perpix

		BiLoadSrf(&srf,"t4blue.srf");

		SimpleMainLoop();

		// BLUE IMPULSE PLUG INTERFACE
		BiWinUnplug();
		ReleaseDC(wnd,dc);

		CloseWindow();
	}
	return 0L;
}

static int OpenWindow(int lupX,int lupY,int sizX,int sizY)
{
	WNDCLASS cls;

	cls.style=CS_OWNDC|CS_BYTEALIGNWINDOW; //|CS_NOCLOSE
	cls.lpfnWndProc=SimpleCallBack;
	cls.cbClsExtra=0;
	cls.cbWndExtra=0;
	cls.hInstance=NULL;
	cls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	cls.hCursor=LoadCursor(NULL,IDC_ARROW);
	cls.hbrBackground=GetStockObject(WHITE_BRUSH);
	cls.lpszMenuName=NULL;
	cls.lpszClassName=cla;
	if(RegisterClass(&cls))
	{
		RECT rc;
		rc.left=lupX;
		rc.top=lupY;
		rc.right=(unsigned long)(lupX+sizX-1);
		rc.bottom=(unsigned long)(lupY+sizY-1);
		AdjustWindowRect(&rc,sty,FALSE);
		lupX=rc.left;
		lupY=rc.top;
		sizX=rc.right-rc.left+1;
		sizY=rc.bottom-rc.top+1;

		wnd=CreateWindow(cla,wnn,sty,lupX,lupY,sizX,sizY,NULL,menu,NULL,NULL);
		if(wnd!=NULL)
		{
			szx=sizX;
			szy=sizY;
			ShowWindow(wnd,SW_SHOWNORMAL);
			return 0;
		}
	}
	return 1;
}

static int CloseWindow(void)
{
	DestroyWindow(wnd);
	return 0;
}

LRESULT CALLBACK SimpleCallBack(HWND wnd,UINT msg,WPARAM w,LPARAM l)
{
	HDC hdc;

	switch(msg)
	{
	case WM_PAINT:
	case WM_QUERYNEWPALETTE:
	case WM_PALETTECHANGED:
		hdc=GetDC(wnd);
		SimplePalette(hdc,pal);
		ReleaseDC(wnd,hdc);
		return DefWindowProc(wnd,msg,w,l);
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		lbt=((w & MK_LBUTTON)!=0 ? 1 : 0);
		rbt=((w & MK_RBUTTON)!=0 ? 1 : 0);
		msx=LOWORD(l);
		msy=HIWORD(l);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		QuitFlag=1;
		break;
	case WM_SIZE:
		szx=LOWORD(l);
		szy=HIWORD(l);

		// BLUE IMPULSE PLUG INTERFACE
		BiWinPlug(dc,szx,szy,0);


		break;
	case WM_KEYDOWN:
		// Add Code Here
		MessageBox(NULL,"",NULL,MB_OK);
		return DefWindowProc(wnd,msg,w,l);
	case WM_COMMAND:
		// Add Code Here
		return DefWindowProc(wnd,msg,w,l);
	case WM_SYSCOMMAND:
		// Add Code Here
		return DefWindowProc(wnd,msg,w,l);
	default:
		return DefWindowProc(wnd,msg,w,l);
	}
	return 0L;
}

static void SimplePalette(HDC hdc,HPALETTE pal)
{
	if(hdc!=NULL && pal!=NULL)
	{
		SelectPalette(hdc,pal,FALSE);
		RealizePalette(hdc);
	}
}

static void SimpleMainLoop(void)
{
	MSG msg;

	BIPOSATT pos;

	BiSetPoint(&pos.p,0,0,20);
	BiSetAngle(&pos.a,0,3000,0);

	while(QuitFlag==0)
	{
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		BIPROJ prj;
		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();
		BiStartBuffer(&BiOrgPA);
		BiInsSrf(&srf,&pos);
		BiFlushBuffer();
		BiSwapBuffers();

		pos.a.h+=1000;
		pos.a.b+=500;
		pos.a.p+=256;
	}
}

