/* Machine Depending Module : Skelton */
// Macintosh , for CodeWarrior CW4
//  1995.3.6 by Shuhei Tomita (shuhei@sfc.wide.ad.jp)
//  1995.4.16 Edited for task
//  1995.6.  1 HotasLibとの分担をはっきりさせるため

//  1995.10.27 作業引き継ぎのためインデントを有澤・山川式に変更


//  1995.6.17
// 1.
//  BIPOINTSは32ビット数なので、それぞれの描画ルーチン内で16ビットの
//  範囲にクリップするように書き加えました。(書換えは0)
// 2.
//   マウスアップイベントを検出してなかったんで、検出するように追加。


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "impulse.h"
#include "imacwcat.h"

#define COLMUL 256

//#define BI_COPYMODE ditherCopy
#define BI_COPYMODE srcCopy


#include <QDOffScreen.h>


/* Following Should not be a "static" variables (?) */
Rect            BiWinRc,BiOffRc;
WindowPtr       BiWinPt;
CGrafPtr        BiCurPort;
GDHandle        BiCurDev;
GWorldPtr       BiOffPort;
short           BiFgnd;
short           BiMosBtn;
EventRecord     BiEvt;
PixMapHandle    BiFrmBuf;
unsigned short  BiFrmBufSw;
long BiWinW,BiWinH;
long BiMaxW,BiMaxH;

//ここから
#define BiFRMPIXELDEPTH	16
char	*BiFrmBufAddrBase;
long	BiFrmBufRowBytes;
long	BiFrmBufRowBytesdiv4;
static WCBMP BiWildcat;

//ここまで

#define MAX_KEYBUF 256
static int nKeyBuf;
static char keyBuf[MAX_KEYBUF];

static unsigned char kbs[256],pKbs[256];
static unsigned char VkToKc[256];
static unsigned char KcToVk[256];

void InitToolbox(void);
void BiMacDoMouse(void);
void BiMacDoDrag(WindowPtr);

void BiMacDoSuspend();
void BiMacDoResume();
void BiMacDoDeactivate(WindowPtr);
void BiMacDoActivate(WindowPtr);
void BiMacDoUpdate(WindowPtr);
void BiMacDoMenuDown();
void BiMacDoMouseUp(void);
void BiMacDoMenu(long);
void BiMacDoGrow(WindowPtr win);

//この関数のプロトタイプ宣言がなかった
void BiMacCopyRect(Rect *,Rect *);
void BiMacTestPsetFunction(BIPOINTS *,BICOLOR *);


/*****************************************************************************

   Setting

*****************************************************************************/

static int BiFatalFlag=BI_OFF;
int BiFatal(void)
{
	return BiFatalFlag;
}


void InitToolbox()
{
	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	FlushEvents(everyEvent,0);
	TEInit();
	InitDialogs(0L);
	InitCursor();
}




/*****************************************************************************

   File Name

*****************************************************************************/
void BiConstrainFileName(char neo[],char org[])
{
	int i;
	char tmp[256],cwd[256];

	for(i=0; org[i]!=0; i++)
	{
		if(org[i]==':')
		{
			strcpy(neo,org);
			goto CONST;
		}
		else if(org[i]=='\\' || org[i]=='/')
		{
			getcwd(cwd,255);
			sprintf(tmp,"%s%s",cwd,org);
			strcpy(neo,tmp);
			goto CONST;
		}
	}
	strcpy(neo,org);

CONST:
	for(i=0; neo[i]!=0; i++)
	{
		if(neo[i]=='\\' || neo[i]=='/')
		{
			neo[i]=':';
		}
	}
}





/**********************************************************************
ウィンドウのサイズを保持する。ただし、この形式に従わなくてもいい。その
他、ウィンドウへの描画に必要な変数は、この部分に記述する。
**********************************************************************/
extern char BiWindowName[];
void BiMacInitAccel(void);
void BiMacOpenWindow(long lupX,long lupY,long reqX,long reqY);

void BiOpenWindow(long reqX,long reqY)
{
	Str255   AppMenu;
	MenuHandle AppMenuHandle,FileMenuHandle;

	InitToolbox();

	// initialize menu
	AppMenu[0]=1;
	AppMenu[1]=appleMark;
	AppMenuHandle=NewMenu(128,AppMenu);
	AppendMenu(AppMenuHandle,"\pabout this program...;-");
	AddResMenu(AppMenuHandle,'DRVR');
	InsertMenu(AppMenuHandle,0);
	FileMenuHandle=NewMenu(129,"\pFile");
	AppendMenu(FileMenuHandle,"\pQuit;-");
	InsertMenu(FileMenuHandle,0);
	DrawMenuBar();

	BiMacOpenWindow(0,0,reqX,reqY);
}

void BiOpenWindowEx(long lupX,long lupY,long reqX,long reqY)
{
	Str255   AppMenu;
	MenuHandle AppMenuHandle,FileMenuHandle;

	InitToolbox();

	// initialize menu
	AppMenu[0]=1;
	AppMenu[1]=appleMark;
	AppMenuHandle=NewMenu(128,AppMenu);
	AppendMenu(AppMenuHandle,"\pabout this program...;-");
	AddResMenu(AppMenuHandle,'DRVR');
	InsertMenu(AppMenuHandle,0);
	FileMenuHandle=NewMenu(129,"\pFile");
	AppendMenu(FileMenuHandle,"\pQuit;-");
	InsertMenu(FileMenuHandle,0);
	DrawMenuBar();

	BiMacOpenWindow(lupX,lupY,reqX,reqY);
}

void BiMacOpenWindow(long lupX,long lupY,long reqX,long reqY)
{
	OSErr    err;
	RGBColor col;

    /* Modified By Captain YS11  1996/03/14 */
    static unsigned char WindowName[256];

    strcpy((char *)WindowName,BiWindowName);
    CtoPstr((char *)WindowName);

	reqX=BiLarger(reqX,320);
	reqY=BiLarger(reqY,240);

	BiFgnd=true;
	BiMosBtn=false;
	col.red  =0x0001;
	col.blue =0x0001;
	col.green=0x0001;
	BiWinW=reqX-16;
	BiWinH=reqY-16;

	SetRect(&BiWinRc,lupX+16,lupX+40,lupX+reqX+16,lupY+reqY+40);
//changed 512x480でない場合にも対応
	SetRect(&BiOffRc,0,0,BiWinW,BiWinH);
	BiWinPt=NewCWindow(0,&BiWinRc,WindowName,true,documentProc,(WindowPtr)-1,false,0);
	SetPort(BiWinPt);
	RGBBackColor(&col);
	col.red  =0xffff;
	col.blue =0xffff;
	col.green=0xffff;
	RGBForeColor(&col);

	//Initialize Frame Buffer
	GetGWorld(&BiCurPort,&BiCurDev);

	err=NewGWorld(&BiOffPort,BiFRMPIXELDEPTH,&BiOffRc,0,0,0);
	if(err !=noErr)
	{
		BiFrmBufSw=0;
		BiFatalFlag=BI_ON;
	}
	BiMaxW=BiWinW;
	BiMaxH=BiWinH;
	BiFrmBufSw=1;
	SetGWorld(BiOffPort,0);

	col.red  =1;
	col.blue =1;
	col.green=1;
	RGBForeColor(&col);
//added (以下3行)この設定を最初に行っておく。
	col.red  =0xffff;
	col.blue =0xffff;
	col.green=0xffff;
	RGBBackColor(&col);
	PaintRect(&BiOffPort->portRect);

	BiFrmBuf=GetGWorldPixMap(BiOffPort);
	LockPixels(BiFrmBuf);

	ShowWindow(BiWinPt);
	SetGWorld(BiCurPort,BiCurDev);

	BiMacInitAccel();
}

void BiMacInitAccel(void)
{
	//ここから
	BiFrmBufAddrBase = (char *)GetPixBaseAddr(BiFrmBuf);
	BiFrmBufRowBytes = ((**BiFrmBuf).rowBytes & 0x07fff);
	BiFrmBufRowBytesdiv4 = BiFrmBufRowBytes / 4;
	//ここまで

	// Accelerate by Wild Cat Library for Mac  1996/03/31
	WcCreateBmp(&BiWildcat,BiWinW,BiWinH,(unsigned char *)BiFrmBufAddrBase,BiFrmBufRowBytes);
}

void BiCloseWindow(void)
{
	if(BiFrmBufSw==1)
	{
		UnlockPixels(BiFrmBuf);
		SetGWorld(BiCurPort,BiCurDev);
		DisposeGWorld(BiOffPort);
		SetPort(BiWinPt);
	}
	DisposeWindow(BiWinPt);
}

void BiGetWindowSize(long *wid,long *hei)
{
	*wid=BiWinW;
	*hei=BiWinH;

}

void BiGetStdProjection(BIPROJ *prj)
{
	long wid,hei;
	BiGetWindowSize(&wid,&hei);
	prj->lx=wid;
	prj->ly=hei;
	prj->cx=wid/2;
	prj->cy=hei/2;
	prj->magx=(real)wid/1.41421356;
	prj->magy=prj->magx;
	prj->nearz=0.5;
	prj->farz=10000.0;
}

void BiSwapBuffers(void)
{
	RGBColor col;
	//追加
/*
	long	w,h;
	Rect	rec;
	w =512;
	if(BiWinW < 512 ){w = BiWinW;}
	h =480;
	if(BiWinH < 480 ){h = BiWinH;}
	SetRect(&rec,0,0,w,h);
*/
	if(BiFrmBufSw==0 /* || BiFgnd!=true */)
	{
		return;
	}

	UnlockPixels(BiFrmBuf);

	SetPort(BiWinPt);
	SetGWorld(BiCurPort,BiCurDev);

	col.red  =1;
	col.blue =1;
	col.green=1;
	RGBForeColor(&col);
	col.red  =0xffff;
	col.blue =0xffff;
	col.green=0xffff;
	RGBBackColor(&col);
//小さな画面を表示するための変更
	CopyBits((BitMap*)*BiFrmBuf,(BitMap*)*BiCurPort->portPixMap,&BiOffRc,&BiOffRc,BI_COPYMODE,0);
//	CopyBits((BitMap*)*BiFrmBuf,(BitMap*)*BiCurPort->portPixMap,&rec,&rec,BI_COPYMODE,0);

	// SetGWorld(BiOffPort,0);
	LockPixels(BiFrmBuf);
	// SetGWorld(BiCurPort,BiCurDev);
	DrawGrowIcon(BiWinPt);
}





/*****************************************************************************

   Device

*****************************************************************************/
static void BiDeviceMatchKey(int kc,unsigned char vk)
{
	VkToKc[vk]=kc;
	KcToVk[kc]=vk;
}

void BiDeviceInitialize(void)
{
//	lbt=BI_OFF;
//	rbt=BI_OFF;

    BiDeviceMatchKey(BIKEY_STOP    ,0x77         );
    BiDeviceMatchKey(BIKEY_COPY    ,0xff         );
    BiDeviceMatchKey(BIKEY_F1      ,0x7A         );
    BiDeviceMatchKey(BIKEY_F2      ,0x78         );
    BiDeviceMatchKey(BIKEY_F3      ,0x63         );
    BiDeviceMatchKey(BIKEY_F4      ,0x76         );
    BiDeviceMatchKey(BIKEY_F5      ,0x60         );
    BiDeviceMatchKey(BIKEY_F6      ,0x61         );
    BiDeviceMatchKey(BIKEY_F7      ,0x62         );
    BiDeviceMatchKey(BIKEY_F8      ,0x64         );
    BiDeviceMatchKey(BIKEY_F9      ,0x65         );
    BiDeviceMatchKey(BIKEY_F10     ,0x6D         );
    BiDeviceMatchKey(BIKEY_ONE     ,0x12         );
    BiDeviceMatchKey(BIKEY_TWO     ,0x13         );
    BiDeviceMatchKey(BIKEY_THREE   ,0x14         );
    BiDeviceMatchKey(BIKEY_FOUR    ,0x15         );
    BiDeviceMatchKey(BIKEY_FIVE    ,0x17         );
    BiDeviceMatchKey(BIKEY_SIX     ,0x16         );
    BiDeviceMatchKey(BIKEY_SEVEN   ,0x1A         );
    BiDeviceMatchKey(BIKEY_EIGHT   ,0x1C         );
    BiDeviceMatchKey(BIKEY_NINE    ,0x19         );
    BiDeviceMatchKey(BIKEY_ZERO    ,0x1D         );
    BiDeviceMatchKey(BIKEY_A       ,0x00         );
    BiDeviceMatchKey(BIKEY_B       ,0x0B         );
    BiDeviceMatchKey(BIKEY_C       ,0x08         );
    BiDeviceMatchKey(BIKEY_D       ,0x02         );
    BiDeviceMatchKey(BIKEY_E       ,0x0E         );
    BiDeviceMatchKey(BIKEY_F       ,0x03         );
    BiDeviceMatchKey(BIKEY_G       ,0x05         );
    BiDeviceMatchKey(BIKEY_H       ,0x04         );
    BiDeviceMatchKey(BIKEY_I       ,0x22         );
    BiDeviceMatchKey(BIKEY_J       ,0x26         );
    BiDeviceMatchKey(BIKEY_K       ,0x28         );
    BiDeviceMatchKey(BIKEY_L       ,0x25         );
    BiDeviceMatchKey(BIKEY_M       ,0x2E         );
    BiDeviceMatchKey(BIKEY_N       ,0x2D         );
    BiDeviceMatchKey(BIKEY_O       ,0x1F         );
    BiDeviceMatchKey(BIKEY_P       ,0x23         );
    BiDeviceMatchKey(BIKEY_Q       ,0x0C         );
    BiDeviceMatchKey(BIKEY_R       ,0x0F         );
    BiDeviceMatchKey(BIKEY_S       ,0x01         );
    BiDeviceMatchKey(BIKEY_T       ,0x11         );
    BiDeviceMatchKey(BIKEY_U       ,0x20         );
    BiDeviceMatchKey(BIKEY_V       ,0x09         );
    BiDeviceMatchKey(BIKEY_W       ,0x0D         );
    BiDeviceMatchKey(BIKEY_X       ,0x07         );
    BiDeviceMatchKey(BIKEY_Y       ,0x10         );
    BiDeviceMatchKey(BIKEY_Z       ,0x06         );
    BiDeviceMatchKey(BIKEY_SPACE   ,0x31         );
    BiDeviceMatchKey(BIKEY_ESC     ,0x35         );
    BiDeviceMatchKey(BIKEY_TAB     ,0x30         );
    BiDeviceMatchKey(BIKEY_CTRL    ,0x3B         );
    BiDeviceMatchKey(BIKEY_SHIFT   ,0x38         );
    BiDeviceMatchKey(BIKEY_ALT     ,0xff         );
    BiDeviceMatchKey(BIKEY_BS      ,0x33         );
    BiDeviceMatchKey(BIKEY_RET     ,0x24         );
    BiDeviceMatchKey(BIKEY_HOME    ,0x73         );
    BiDeviceMatchKey(BIKEY_DEL     ,0x71         );

    BiDeviceMatchKey(BIKEY_UP      ,0x7E         );
    BiDeviceMatchKey(BIKEY_DOWN    ,0x7D         );
    BiDeviceMatchKey(BIKEY_LEFT    ,0x7B         );
    BiDeviceMatchKey(BIKEY_RIGHT   ,0x7C         );
    BiDeviceMatchKey(BIKEY_TEN0    ,0x52         );
    BiDeviceMatchKey(BIKEY_TEN1    ,0x53         );
    BiDeviceMatchKey(BIKEY_TEN2    ,0x54         );
    BiDeviceMatchKey(BIKEY_TEN3    ,0x55         );
    BiDeviceMatchKey(BIKEY_TEN4    ,0x56         );
    BiDeviceMatchKey(BIKEY_TEN5    ,0x57         );
    BiDeviceMatchKey(BIKEY_TEN6    ,0x58         );
    BiDeviceMatchKey(BIKEY_TEN7    ,0x59         );
    BiDeviceMatchKey(BIKEY_TEN8    ,0x5B         );
    BiDeviceMatchKey(BIKEY_TEN9    ,0x5C         );
    BiDeviceMatchKey(BIKEY_TENMUL  ,0x43         );
    BiDeviceMatchKey(BIKEY_TENDIV  ,0x4B         );
    BiDeviceMatchKey(BIKEY_TENPLUS ,0x45         );
    BiDeviceMatchKey(BIKEY_TENMINUS,0x4E         );
    BiDeviceMatchKey(BIKEY_TENEQUAL,0x51         );
    BiDeviceMatchKey(BIKEY_TENDOT  ,0x41         );

	nKeyBuf=0;
}

void BiUpdateDevice(void)
{
	Boolean isEvent;
	long key;

	// if(Button()!=true){BiMosBtn=false;}

	isEvent=WaitNextEvent(everyEvent,&BiEvt,7,0);
	if(isEvent == true)
	{
		switch(BiEvt.what)
		{
		case keyDown:
			key=(BiEvt.message & keyCodeMask)>>8;
			if(nKeyBuf < MAX_KEYBUF)
			{
				keyBuf[nKeyBuf]=VkToKc[key];
				nKeyBuf++;
			}
			break;
		case mouseDown:
			BiMacDoMouse();
			break;
		case mouseUp:
			BiMacDoMouseUp();
			break;
		case updateEvt:
			BiMacDoUpdate((WindowPtr)BiEvt.message);
			break;
		case activateEvt:
			if(BitAnd(BiEvt.modifiers,activeFlag)!=1)
			{
				BiMacDoDeactivate((WindowPtr)BiEvt.message);
			}
			else
			{
				BiMacDoActivate((WindowPtr)BiEvt.message);
			}
			break;
		case osEvt:
			switch( (unsigned long)(BiEvt.message & osEvtMessageMask)/0x1000000)
			{
			case suspendResumeMessage:
				if(BiEvt.message & resumeFlag)
				{
					BiMacDoResume();
				}
				else
				{
					BiMacDoSuspend();
				}
				break;
			case mouseMovedMessage:
				break;
			}
			break;
		}
	}
	else
	{
		/* ? */
	}
}


/***********************************************************************/


void BiMouse(int *lbt,int *mbt,int *rbt,long *mx,long *my)
{
	Point Bi_mac_mouse_point;
	long key;
	unsigned long bit,byte;
	KeyMap mkeyread;
	unsigned char *mkey;

	GetKeys(mkeyread);
	GetMouse(&Bi_mac_mouse_point);

	mkey=(unsigned char *)mkeyread;
	key=0x3A;
	bit=(key % 8);
	byte=key / 8;
	*rbt=((mkey[byte]&(1<<bit))>0 ? BI_ON : BI_OFF);

	*lbt= (BiMosBtn==true ? BI_ON : BI_OFF);
	*mbt=BI_OFF;
	*mx=Bi_mac_mouse_point.h;
	*my=Bi_mac_mouse_point.v;
}

int BiKey(int kcode)
{
	long key;
	unsigned long bit,byte;

	KeyMap mkeyread;
	unsigned char *mkey;
	GetKeys(mkeyread);
	mkey=(unsigned char *)mkeyread;
	key=KcToVk[kcode];
	bit=(key%8);
	byte=(key/8);
	return ((mkey[byte]&(1<<bit))>0 ? BI_ON : BI_OFF);
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
}

void *BiMalloc(size_t uni)
{
	return (void *)malloc(uni);
}

void BiFree(void *ptr)
{
	free(ptr);
}






/*****************************************************************************

   Draw

   全体の注意
     世の中が32bit時代に突入したにもかかわらず、BIOSレベルで画面座標をいまだ
     に16bitでしか受け付けないシステム(TOWNS-BIOSですらそうだからCONCORDEとか
     GL程度しかちゃんと32bitで受け付けるのはないけどな)のために、関数

       BiClipPolygScrn
       BiClipLineScrn

     を利用して、32bit整数で受け取った画面座標をクリッピングすることができま
     す。

*****************************************************************************/

/** Begin YS11 **/
static BIPOINTS orgLup,orgRdw;
static void BiMacSetClipRect(void)
{
	static BIPOINTS lup,rdw;
	BiGetClipScreenFrame(&orgLup,&orgRdw);
	BiSetPointS(&lup,0,0);
	BiSetPointS(&rdw,BiWinW-1,BiWinH-1);
	BiSetClipScreenFrame(&lup,&rdw);
}

static void BiMacEndClipRect(void)
{
	BiSetClipScreenFrame(&orgLup,&orgRdw);
}

#define BIINBOUND(a) (0<=(a)->x && (a)->x<BiWinW && 0<=(a)->y && (a)->y<BiWinH)
/** End YS11 **/


void BiClearScreenD(void)
{
	/* Accelerated *******************
	RGBColor col;

	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	col.red  =0x0001;
	col.blue =0x0001;
	col.green=0x0001;
	RGBForeColor(&col);
	PaintRect(&BiOffPort->portRect);
	SetGWorld(BiCurPort,BiCurDev);
    ************************************/
	WCCOLOR col={0,0,0};
	WcRect(&BiWildcat,0,0,(int)(BiWinW-1),(int)(BiWinH-1),&col);
}

void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	RGBColor col;
	col.green=c->g*COLMUL+c->g;
	col.blue =c->b*COLMUL+c->b;
	col.red  =c->r*COLMUL+c->r;


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

	/* Changed To Accelerated Function **
	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	RGBForeColor(&col);
	MoveTo((short)s1->x,(short)s1->y);
	LineTo((short)s2->x,(short)s2->y);
	SetGWorld(BiCurPort,BiCurDev);
	*************************************/
	WcLine(&BiWildcat,s1->x,s1->y,s2->x,s2->y,(WCCOLOR *)c);

END:
	BiMacEndClipRect();
}

void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
	/* Accelerated ************************
	RGBColor	col;

	if(!BIINBOUND(s))
	{
		return;
	}

	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	col.green=c->g*COLMUL+c->g;
	col.blue =c->b*COLMUL+c->b;
	col.red  =c->r*COLMUL+c->r;
	SetCPixel((short)s->x,(short)s->y,&col);
	SetGWorld(BiCurPort,BiCurDev);
	*****************************************/

	BIPOINTS t1,t2;

	t1=*s;
	BiSetPointS(&t2,t1.x+1,t1.y+1);
	if(BIINBOUND(&t1) && BIINBOUND(&t2))
	{
		WcPset(&BiWildcat,(int)t1.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&BiWildcat,(int)t2.x,(int)t1.y,(WCCOLOR *)c);
		WcPset(&BiWildcat,(int)t1.x,(int)t2.y,(WCCOLOR *)c);
		WcPset(&BiWildcat,(int)t2.x,(int)t2.y,(WCCOLOR *)c);
	}
}

void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	/* Accelerated ***************************************
	RGBColor	col;
	Rect		rec;

	if(!BIINBOUND(s))
	{
		return;
	}

	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	SetRect(&rec,(short)(s->x-radx),(short)(s->y-rady),
	             (short)(s->x+radx),(short)(s->y+rady));
	col.green=c->g*COLMUL+c->g;
	col.blue =c->b*COLMUL+c->b;
	col.red  =c->r*COLMUL+c->r;
	RGBForeColor(&col);

	PaintOval(&rec);
	SetGWorld(BiCurPort,BiCurDev);
	***************************************************/
	if(BIINBOUND(s))
	{
		if(radx==rady)
		{
			WcSetCircleRegion(&BiWildcat,(WCPNT *)s,radx);
		}
		else
		{
			WcSetEllipseRegion(&BiWildcat,(WCPNT *)s,radx,rady);
		}
		WcPaintRegion(&BiWildcat,(WCCOLOR *)c);
	}
}

void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
	long i;
	RGBColor col;
	RgnHandle handRgn;
	PolyHandle mpoly;

	unsigned long stk;
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
				goto END;
			}
			ns=nsNew;
			s=sNew;
			break;
		}
	}

	/* Accelerated *****************
	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	col.green=c->g*COLMUL+c->g;
	col.blue =c->b*COLMUL+c->b;
	col.red  =c->r*COLMUL+c->r;
	RGBForeColor(&col);

	mpoly=OpenPoly();
	MoveTo(s[0].x,s[0].y);
	for(i=1;i<ns;i++)
	{
		LineTo(s[i].x,s[i].y);
	}
	LineTo(s[0].x,s[0].y);
	ClosePoly();
	PaintPoly(mpoly);
	KillPoly(mpoly);
	SetGWorld(BiCurPort,BiCurDev);
	**********************************/
	WcSetRegion(&BiWildcat,ns,(WCPNT *)s);
	WcPaintRegion(&BiWildcat,(WCCOLOR *)c);

	if(sNew!=NULL)
	{
		BiPopTmpStack(stk);
	}

END:
	BiMacEndClipRect();
}

void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	/*  Accelerated ***************************
	RGBColor	col;
	Rect		rec;

	BIPOINTS a1,a2;
	a1.x=BiSmaller(s1->x,s2->x);
	a1.y=BiSmaller(s1->y,s2->y);
	a2.x=BiLarger(s1->x,s2->x);
	a2.y=BiLarger(s1->y,s2->y);

	a1.x=BiLarger(a1.x,-0x2000);
	a1.y=BiLarger(a1.y,-0x2000);
	a2.x=BiSmaller(a2.x,0x2000);
	a2.y=BiSmaller(a2.y,0x2000);

	s1=&a1;
	s2=&a2;

	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	SetRect(&rec,(short)s1->x,(short)s1->y,(short)s2->x,(short)s2->y);
	col.green=c->g*COLMUL+c->g;
	col.blue =c->b*COLMUL+c->b;
	col.red  =c->r*COLMUL+c->r;
	RGBForeColor(&col);
	PaintRect(&rec);
	SetGWorld(BiCurPort,BiCurDev);
    *****************************************/
	BIPOINTS a1,a2;

	if(BIINBOUND(s1) && BIINBOUND(s2))
	{
		WcRect(&BiWildcat,(int)s1->x,(int)s1->y,(int)s2->x,(int)s2->y,(WCCOLOR *)c);
	}
	else
	{
		a1.x=BiSmaller(s1->x,s2->x);
		a1.y=BiSmaller(s1->y,s2->y);
		a2.x=BiLarger(s1->x,s2->x);
		a2.y=BiLarger(s1->y,s2->y);
		if(a1.x>=BiWinW || a1.y>=BiWinH || a2.x<0 || a2.y<0)
		{
			return;
		}
		a1.x=BiLarger(a1.x,0);
		a1.y=BiLarger(a1.y,0);
		a2.x=BiSmaller(a2.x,BiWinW-1);
		a2.y=BiSmaller(a2.y,BiWinH-1);
		WcRect(&BiWildcat,(int)a1.x,(int)a1.y,(int)a2.x,(int)a2.y,(WCCOLOR *)c);
	}
}

void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
	#define BIMKRAD 3
	BIPOINTS p1,p2;

	if(!BIINBOUND(s))
	{
		return;
	}

	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
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
	SetGWorld(BiCurPort,BiCurDev);
}

void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
	RGBColor	col;
	char buf[512];

	if(!BIINBOUND(s))
	{
		return;
	}

	SetPort(BiWinPt);
	SetGWorld(BiOffPort,0);
	col.green=c->g*COLMUL+c->g;
	col.blue=c->b*COLMUL+c->b;
	col.red=c->r*COLMUL+c->r;
	RGBForeColor(&col);
	TextSize(9);
	MoveTo(s->x,s->y);
	strcpy(buf,str);
	CtoPstr(buf);
	DrawString((unsigned char *)buf);
	SetGWorld(BiCurPort,BiCurDev);
}

void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	#define FONTX 8  /* フォントサイズ X */
	#define FONTY 16 /* フォントサイズ Y */
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;

	SetPort(BiWinPt);
	// 仮想画面にセット
	SetGWorld(BiOffPort,0);
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
	SetGWorld(BiCurPort,BiCurDev);
}

void BiMacDoMouse()
{
	WindowPtr whichWindow;
	short Click;

	Click=FindWindow(BiEvt.where,&whichWindow);
	if(whichWindow == BiWinPt)
	{
		switch(Click)
		{
		case inDesk:
			break;
		case inMenuBar:
			BiMacDoMenuDown();
			break;
		case inSysWindow:
			SystemClick(&BiEvt,whichWindow);
			break;
		case inDrag:
			BiMacDoDrag(whichWindow);
			break;
		case inGrow:
			BiMacDoGrow(whichWindow);
			break;
		case inContent:
			BiMosBtn=true;
			break;
		//case inGoAway:
		//	BiFatalFlag=BI_ON;
		//	break;
		}
	}
}

void BiMacDoMouseUp(void)
{
    BiMosBtn=false;
}

void BiMacDoDrag(WindowPtr win)
{
	Rect DragArea;
	//ThinkCとはグローバルが違う↓
	DragArea=qd.screenBits.bounds;
	InsetRect(&DragArea,4,4);
	DragWindow(win,BiEvt.where,&DragArea);
}

void BiMacDoUpdate(WindowPtr win)
{
	if(win==BiWinPt)
	{
		SetPort(BiWinPt);
		BeginUpdate(win);
		BiSwapBuffers();
		EndUpdate(win);
	}
}

void BiMacDoActivate(WindowPtr win)
{
	BiFgnd=true;
	SelectWindow(BiWinPt);
}

void BiMacDoDeactivate(WindowPtr win)
{
	BiFgnd=false;
}

void BiMacDoResume()
{
	WindowPtr win;
	SelectWindow(BiWinPt);
	BiFgnd=true;
}

void BiMacDoSuspend()
{
	BiFgnd=false;
}

void BiMacDoMenuDown()
{
	long itemID;
	itemID=MenuSelect(BiEvt.where);
	BiMacDoMenu(itemID);
}

void BiMacDoMenu(long item)
{
	int MenuID,SelectID;
	Str255 DAname;
	int DAstat;

	MenuID=HiWord(item);
	SelectID=LoWord(item);
	switch(MenuID)
	{
	case 128:
		switch(SelectID)
		{
		case 1:
			break;
		default:
			GetItem(GetMHandle(128),SelectID,DAname);
			DAstat=OpenDeskAcc(DAname);
			break;
		}
		break;
	}
}

void BiMacCopyRect(Rect *dst,Rect *src)
{
	SetRect(dst,src->left,src->top,src->right,src->bottom);
}

void BiMacDoGrow(WindowPtr win)
{
	OSErr er;
	long growth;
	Rect gr,mr;
	RGBColor col;

	SetPort(win);
	BiMacCopyRect(&gr,&win->portRect);
	gr.left +=40;
	gr.top  +=40;
	gr.right =BiMaxW+16;
	gr.bottom=BiMaxH+16;

	growth=GrowWindow(win,BiEvt.where,&gr);
	if(growth!=0)
	{
		SizeWindow(win,LoWord(growth),HiWord(growth),true);
		EraseRect(&win->portRect);
		// Change In Frame Buffer
		BiWinW=LoWord(growth)-16;
		BiWinH=HiWord(growth)-16;
		SetRect(&BiOffRc,0,0,BiWinW,BiWinH);
		BiSwapBuffers();
		DrawGrowIcon(win);
	}

	WcDeleteBmp(&BiWildcat);
	WcCreateBmp(&BiWildcat,BiWinW,BiWinH,(unsigned char *)BiFrmBufAddrBase,BiFrmBufRowBytes);
}






/***********************************************************************

  UPDATE MODULE  1995/06/28

  This Code enables most of New Expanded Functions. But, following
  two functions should be written for each platform.

    void BiGetColor(BICOLOR *col,BIPOINTS *s);
    void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2);

***********************************************************************/

void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *col)
{
	BIPOINTS p1,p2;
	BiSetPointS(&p1,s1->x,s1->y);
	BiSetPointS(&p2,s2->x,s1->y);
	BiDrawLine2(&p1,&p2,col);
	BiSetPointS(&p1,s2->x,s1->y);
	BiSetPointS(&p2,s2->x,s2->y);
	BiDrawLine2(&p1,&p2,col);
	BiSetPointS(&p1,s2->x,s2->y);
	BiSetPointS(&p2,s1->x,s2->y);
	BiDrawLine2(&p1,&p2,col);
	BiSetPointS(&p1,s1->x,s2->y);
	BiSetPointS(&p2,s1->x,s1->y);
	BiDrawLine2(&p1,&p2,col);
}

void BiDrawEmptyPolyg2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiDrawLine2(&s[i],&s[i+1],col);
	}
	BiDrawLine2(&s[0],&s[n-1],col);
}

void BiDrawPolyline2D(int n,BIPOINTS *s,BICOLOR *col)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		BiDrawLine2(&s[i],&s[i+1],col);
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
		BiDrawLine2(&p1,&p2,col);

		BiSetPointS(&p1,s->x-v1.x,s->y+v1.y);
		BiSetPointS(&p2,s->x-v2.x,s->y+v2.y);
		BiDrawLine2(&p1,&p2,col);

		BiSetPointS(&p1,s->x+v1.x,s->y-v1.y);
		BiSetPointS(&p2,s->x+v2.x,s->y-v2.y);
		BiDrawLine2(&p1,&p2,col);

		BiSetPointS(&p1,s->x-v1.x,s->y-v1.y);
		BiSetPointS(&p2,s->x-v2.x,s->y-v2.y);
		BiDrawLine2(&p1,&p2,col);
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

/* 1pixel = 16bit版 */
void BiMacTestPsetFunction(BIPOINTS *s,BICOLOR *col)
{
	char *addr;

	// x,yを用いたアドレス計算
	addr = (char *)(BiFrmBufAddrBase +
			(BiFrmBufRowBytes * s->y) +
			s->x * 2);
	// 16bit が 0RRRRRGG GGGBBBBB で表されているようなので、
	// 色をセットする
	(*addr) =  (unsigned char)(((col->r)>>1) & 0x7C) +
				((col->g)>>6 & 0x03);
	(*(addr+1)) = (unsigned char)(((col->g)<<2) & 0x00E0) +
					((col->b)>>3);
}
/* 1pixel = 32bit版
void BiMacTestPsetFunction(BIPOINTS *s,BICOLOR *col)
{
	long *addr;

	// x,yを用いたアドレス計算
	addr = BiFrmBufAddrBase +
			(BiFrmBufRowBytesdiv4 * (short)s->y) +
			((short)s->x);
	// 32bit が 00RRGGBB で表されているようなので、
	// 色をセットする
	*addr = ((long)col->r << 16) + ((long)col->g << 8) + col->b;
}
*/
