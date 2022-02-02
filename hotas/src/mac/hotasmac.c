// hotas_mac.c
// hotasをサポートするファイル
// Macintosh , for CodeWarrior CW4
//	exporting from imac.c (Blue Impulse Library)
//	1995. 5.3 by Shuhei Tomita (shuhei@sfc.wide.ad.jp)
//	1995. 6.1 描画に必要な関数や変数はimac.c(BiLib)の方を呼び出すようにする

//  1995.10.27 引き継ぎのため有澤・山川方式のインデントに変更作業



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDOffScreen.h>
#include <Picker.h>


// List Box Extension 1995/11/02 ->
#include <AppleEvents.h>
#include <Dialogs.h>
#include <Lists.h>
// <- List Box Extension 1995/11/02


#include <impulse.h>
#include "hotas.h"

#define MAXMENUNUM 10

typedef void (*FUNC)();

typedef struct
{
	int exist;
	unsigned char kc;
	void (*func)();
} HTMACKEYTABLE;


HTMACKEYTABLE HtKeyTab[256];
long          HtNKeyTab;

long       HtMenuBar;
MenuHandle HtMenuHnd[MAXMENUNUM];
FUNC       HtMenuFunc[MAXMENUNUM][255];
short      HtMenuListLn[MAXMENUNUM];
short      HtQuitFlg;
char       HtAboutStr[255];


extern EventRecord    BiEvt;
extern WindowPtr      BiWinPt;
extern Rect           BiWinRc,BiOffRc;
extern CGrafPtr       BiCurPort;
extern GDHandle       BiCurDev;
extern GWorldPtr      BiOffPort;
extern unsigned short BiFrmBufSw;
extern PixMapHandle	  BiFrmBuf;

extern short BiFgnd;
extern short BiMosBtn;
extern long BiWinW,BiWinH;


extern void BiMacDoDrag(WindowPtr);
extern void BiMacDoDeactivate(WindowPtr);
extern void BiMacDoActivate(WindowPtr);
extern void BiMacDoResume();
extern void BiMacDoSuspend();
extern void BiSwapBuffers();
extern void BiMacDoGrow();


static void HtDummyButtonFunc(int trig,HTMOUSE *mos){}
static void HtDummyDragFunc(HTMOUSE *mos){}
static void HtDummyVoidFunc(void){}
static int HtMacVkToKc(unsigned char);


void (*HtOnLeftButton)(int trig,HTMOUSE *mos)=HtDummyButtonFunc;
void (*HtOnMiddleButton)(int trig,HTMOUSE *mos)=HtDummyButtonFunc;
void (*HtOnRightButton)(int trig,HTMOUSE *mos)=HtDummyButtonFunc;
void (*HtOnDrag)(HTMOUSE *mos)=HtDummyDragFunc;
void (*HtRepaintFunc)(void)=HtDummyVoidFunc;
void (*HtIntervalFunc)(void)=HtDummyVoidFunc;

void HtDialog();
void HtMacSetupAboutAlert(char *);
Boolean HtMacGetPathName(char *,short,long);

void HtMacDoMenu(long);
void HtMacDoMenuDown();
void HtMacDoKey();
void HtMacDoMouse();
void HtMacDoUpdate(WindowPtr);

void HtMessageBox(char *,char *);
void HtMessageBox3(char *msg[3],char *);
int HtLineInput(char *,char *,char *);
int HtYesNo(char *,char *,char *);
int HtYesNo3(char *msg[3],char *,char *);
void HtMacCopyRect(Rect *,Rect *);

//Macの初期化
void HtInitialize(void)
{
	// List Box Extension 1995/11/02  ->
	Ptr p;
	p = GetApplLimit();
	p -= 64000;
	SetApplLimit(p);
	MaxApplZone();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	// <-  List Box Extension 1995/11/02

	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	FlushEvents(everyEvent,0);
	TEInit();
	InitDialogs(0L);
	InitCursor();
	strcpy(HtAboutStr,"About ...");
}

void HtMacSetupAboutAlert(char *str)
{
	long len;
	len=strlen(str);
	if(len > 255)
	{
		len=255;
	}
	strncpy(HtAboutStr,str,len);
}

//メインでのキーを押された時の設定用の関数
void HtStartSetShortCut(void)
{
	int i;
	HtNKeyTab=0;
	for(i=0;i<256;i++)
	{
		HtKeyTab[i].exist=HT_OFF;
	}
}

//メインでのキーを押された時の設定用の関数(設定)
void HtSetShortCut(int vk,void (*func)(void))
{
	int kc;

	kc=HtMacVkToKc(vk);
	if(HtNKeyTab>255 || kc==0)
	{
		return;
	}

	HtKeyTab[HtNKeyTab].exist=HT_ON;
	HtKeyTab[HtNKeyTab].kc=kc;
	HtKeyTab[HtNKeyTab].func=func;
	HtNKeyTab++;
}

//メインでのキーを押された時の設定用の関数(設定終了)
void HtEndSetShortCut(void)
{
}

//メニュー設定用の関数
void HtStartSetMenu(char *MenuString[])
{
	Str255 AppMenu;
	unsigned char gomi[255];
	MenuHandle AppMenuHandle;
	long i,j;

	for(i=0;i<MAXMENUNUM;i++)
	{
		for(j=0;j<255;j++)
		{
			HtMenuFunc[i][j]=0;
		}
	}

	for(i=0;i<MAXMENUNUM;i++)
	{
		HtMenuListLn[i]=0;
	}

	AppMenu[0]=1;
	AppMenu[1]=appleMark;
	AppMenuHandle=NewMenu(128,AppMenu);
	AppendMenu(AppMenuHandle,"\pAbout...;-");
	AddResMenu(AppMenuHandle,'DRVR');
	InsertMenu(AppMenuHandle,0);
	for(i=0; i<MAXMENUNUM && MenuString[i]!=NULL ;i++)
	{
		strcpy((char *)gomi,MenuString[i]);
		CtoPstr((char *)gomi);
		HtMenuHnd[i+1]=NewMenu(128+i+1,gomi);
	}
	HtMenuBar=i;
}

void HtEndSetMenu(void)
{
	DrawMenuBar();
}

void HtSetMenuItem(int mbaritem,char *menustr,char shortcutkey,void (*func)())
{
	unsigned char gomi[255];

	if(mbaritem>MAXMENUNUM)
	{
		return;
	}

	// ※メニュー文字列に代入されるものは，英字数字日本語文字でなければならない
	sprintf((char *)gomi,"%s;",menustr);
	CtoPstr((char *)gomi);
	AppendMenu(HtMenuHnd[mbaritem],gomi);
	InsertMenu(HtMenuHnd[mbaritem],0);
	HtMenuFunc[mbaritem][HtMenuListLn[mbaritem]]=func;
	SetItemCmd(HtMenuHnd[mbaritem],HtMenuListLn[mbaritem]+1,shortcutkey);
	HtMenuListLn[mbaritem]++;
}

extern char BiWindowName[];
void BiMacInitAccel(void);
void BiMacOpenWindow(long lupX,long lupY,long reqX,long reqY);

//ウインドウのオープン
void HtOpenWindow(long reqX,long reqY)
{
	BiMacOpenWindow(0,0,reqX,reqY);

//	RGBColor col;
//	OSErr er;
//	Rect r;
//
//	/* 1996/03/15 */
//	static unsigned char WindowName[256];
//	strcpy((char *)WindowName,BiWindowName);
//	CtoPstr((char *)WindowName);
//
//
//	reqX=BiSmaller(reqX,512);
//	reqY=BiSmaller(reqY,480);
//
//	BiFgnd=true;
//	BiMosBtn=false;
//	col.red  =0x0001;
//	col.blue =0x0001;
//	col.green=0x0001;
//	BiWinW=reqX;
//	BiWinH=reqY;
//	SetRect(&BiWinRc,0+16,0+40,reqX+16+15,reqY+40+15);
//	SetRect(&BiOffRc,0,0,512,480);
//	BiWinPt=NewCWindow(0,&BiWinRc,WindowName,true,zoomDocProc,(WindowPtr)-1,true,0L);
//	SetPort(BiWinPt);
//
//	RGBBackColor(&col);
//	col.red  =0xffff;
//	col.blue =0xffff;
//	col.green=0xffff;
//	RGBForeColor(&col);
//
//	GetGWorld(&BiCurPort,&BiCurDev);
//	er=NewGWorld(&BiOffPort,8,&BiOffRc,0,0,0);
//	if(er !=noErr)
//	{
//		BiFrmBufSw=0;
//		HtQuitFlg=1;
//
//		ParamText("\pcan't allocate memory for Frame Buffer.","\p","\p","\p");
//		Alert(5000,0L);
//	}
//
//	BiFrmBufSw=1;
//	SetGWorld(BiOffPort,0);
//	col.red  =1;
//	col.blue =1;
//	col.green=1;
//	RGBForeColor(&col);
//	PaintRect(&BiOffPort->portRect);
//	BiFrmBuf=GetGWorldPixMap(BiOffPort);
//	LockPixels(BiFrmBuf);
//	ShowWindow(BiWinPt);
//	SetGWorld(BiCurPort,BiCurDev);
//	SetPort(BiWinPt);
//
//	BiMacInitAccel();
}

//メインループの設定....BiDeviceUpdateを使う
void HtEventLoop(void)
{
	Boolean isEvent;
	long key;
	HtQuitFlg=0;
	while(HtQuitFlg==0)
	{
		(*HtIntervalFunc)();

		isEvent=WaitNextEvent(everyEvent,&BiEvt,7,0);
		if(isEvent==true)
		{
			switch(BiEvt.what)
			{
			case keyDown:
				HtMacDoKey();
				break;
			case mouseDown:
				HtMacDoMouse();
				break;
			case updateEvt:
				HtMacDoUpdate((WindowPtr)BiEvt.message);
				{
					static int AntiRecursive=0;
					if(AntiRecursive==0)
					{
						AntiRecursive=1;
						(*HtRepaintFunc)();
						AntiRecursive=0;
					}
				}
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
				switch((unsigned long)(BiEvt.message & osEvtMessageMask)/0x1000000)
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
	}
	if(BiFrmBufSw==1)
	{
		UnlockPixels(BiFrmBuf);
		SetGWorld(BiCurPort,BiCurDev);
		DisposeGWorld(BiOffPort);
		SetPort(BiWinPt);
	}
	DisposeWindow(BiWinPt);
}

void HtQuitProgram(void)
{
	HtQuitFlg=1;
}

void HtMacDoMouse()
{
	WindowPtr whichWindow;
	short Click;
	HTMOUSE ms;
	Point pt;

	Click=FindWindow(BiEvt.where,&whichWindow);
	switch(Click)
	{
	case inDesk:
		break;
	case inMenuBar:
		HtMacDoMenuDown();
		break;
	case inSysWindow:
		if(whichWindow==BiWinPt)
		{
			SystemClick(&BiEvt,whichWindow);
		}
		break;
	case inDrag:
		if(whichWindow==BiWinPt)
		{
			BiMacDoDrag(whichWindow);
		}
		break;
	case inGrow:
		if(whichWindow==BiWinPt)
		{
			static int AntiRecursive=0;
			BiMacDoGrow(whichWindow);
			if(AntiRecursive==0)
			{
				AntiRecursive=1;
				(*HtRepaintFunc)();
				AntiRecursive=0;
			}
		}
		break;
	case inContent:
		// User Pressed the Mouse Button
		if(whichWindow==BiWinPt)
		{
			GetMouse(&pt);
			ms.mx=pt.h;
			ms.my=pt.v;
			ms.lbt=HT_ON;
			ms.mbt=HT_OFF;
			ms.rbt=HT_OFF;
			(*HtOnLeftButton)(HT_ON,&ms);
			while(StillDown()==true)
			{
				GetMouse(&pt);
				ms.mx=pt.h;
				ms.my=pt.v;
				ms.lbt=HT_ON;
				(*HtOnDrag)(&ms);
			}
			//User Released the Mouse Button
			GetMouse(&pt);
			ms.mx=pt.h;
			ms.my=pt.v;
			ms.lbt=HT_OFF;
			(*HtOnLeftButton)(HT_OFF,&ms);
		}
		break;
	case inGoAway:
		if(whichWindow==BiWinPt && TrackGoAway(whichWindow,BiEvt.where))
		{
			HtQuitProgram();
		}
		break;
	}
}


//Bi....ではできないことを行うため
void HtMacDoMenuDown()
{
	long itemID;
	itemID=MenuSelect(BiEvt.where);
	HtMacDoMenu(itemID);
}


//Bi....ではできないことを行うため
void HtMacDoMenu(long item)
{
	short MenuID,SelectID;
	Str255 DAname;
	short DAstat;
	unsigned char gomi[255];

	MenuID=HiWord(item);
	SelectID=LoWord(item);
	if(MenuID==128)
	{
		switch(SelectID)
		{
		case 1:
			HtMessageBox(HtAboutStr,NULL);
			break;
		default:
			GetItem(GetMHandle(128),SelectID,DAname);
			DAstat=OpenDeskAcc(DAname);
			break;
		}
	}
	else
	{
		if(MenuID>128 && MenuID<(128+MAXMENUNUM) )
		{
			if(HtMenuFunc[MenuID-128][SelectID-1]!=0)
			{
				(*HtMenuFunc[MenuID-128][SelectID-1])();
			}
		}
	}
	HiliteMenu(0);
}


void HtMacDoKey()
{
	long ItemID;
	long i;
	int kc;

	if((BiEvt.modifiers & cmdKey)!=0)
	{
		ItemID=MenuKey(BiEvt.message & charCodeMask);
		if(ItemID)
		{
			HtMacDoMenu(ItemID);
		}
	}

	kc=(BiEvt.message & keyCodeMask)>>8;

	for(i=0;i<HtNKeyTab;i++)
	{
		if(HtKeyTab[i].kc==kc)
		{
			(*HtKeyTab[i].func)();
			break;
		}
	}
}


void HtMacDoUpdate(WindowPtr win)
{
	if(win==BiWinPt)
	{
		SetPort(BiWinPt);
		BeginUpdate(win);
		BiSwapBuffers();
		DrawGrowIcon(win);
		EndUpdate(win);
	}
}


//void HtMacDoGrow(WindowPtr win)
//{
//	OSErr er;
//	long growth;
//	Rect gr,mr;
//	RGBColor col;
//
//	SetPort(win);
//	HtMacCopyRect(&gr,&win->portRect);
//	gr.left +=40;
//	gr.top  +=40;
//	gr.right =512;
//	gr.bottom=480;
//
//	growth=GrowWindow(win,BiEvt.where,&gr);
//	if(growth!=0)
//	{
//		SizeWindow(win,LoWord(growth),HiWord(growth),true);
//		EraseRect(&win->portRect);
//		//仮想画面の変更
//		BiWinW=LoWord(growth)-16;
//		BiWinH=HiWord(growth)-16;
//		BiSwapBuffers();
//		DrawGrowIcon(win);
//	}
//}
//
//
//void HtMacCopyRect(Rect *distRect,Rect *srcRect)
//{
//	SetRect(distRect,srcRect->left,srcRect->top,srcRect->right,srcRect->bottom);
//}




/***********************************************************************/
void HtMacSetIText(Handle hnd,char *txt)
{
	unsigned char buf[256];
	txt=(txt!=NULL ? txt : "");
	strcpy((char *)buf,txt);
	CtoPstr((char *)buf);
	SetIText(hnd,buf);
}

void HtMacGetIText(char *txt,Handle hnd)
{
	unsigned char buf[256];
	GetIText(hnd,buf);
	PtoCstr(buf);
	strcpy(txt,(char *)buf);
}


/***********************************************************************/
void HtMessageBox(char *msg,char *btn)
{
	DialogPtr mDialog;
	short i,item;
	Rect r;
	Handle H1,H2;

	mDialog=GetNewDialog(1024,0,(WindowPtr)-1);
	GetDItem(mDialog,2,&i,&H2,&r);
	GetDItem(mDialog,1,&i,&H1,&r);
	SetPort(mDialog);
	InsetRect(&r,2,2);

	btn=(btn!=NULL ? btn : "OK");
	TextBox(btn,strlen(btn),&r,1);

	HtMacSetIText(H2,msg);
//	strcpy((char *)gom,msg);
//	CtoPstr((char *)gom);
//	SetIText(H2,gom);
	item=0;
	while(item!=1)
	{
		ModalDialog(0,&item);
	}
	DisposeDialog(mDialog);
}


void HtMessageBox3(char *msg[3],char *btn)
{
	DialogPtr mDialog;
	unsigned char gom[255];
	short i,item;
	Rect r;
	Handle H1,H2,H3,H4;

	mDialog=GetNewDialog(1025,0,(WindowPtr)-1);
	GetDItem(mDialog,2,&i,&H2,&r);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,1,&i,&H1,&r);
	SetPort(mDialog);

	InsetRect(&r,2,2);

	btn=(btn!=NULL ? btn : "OK");
	TextBox(btn,strlen(btn),&r,1);

	HtMacSetIText(H2,msg[0]);
	HtMacSetIText(H3,msg[1]);
	HtMacSetIText(H4,msg[2]);

	item=0;
	while(item!=1)
	{
		ModalDialog(0,&item);
	}
	DisposeDialog(mDialog);
}


int HtLineInput(char *str,char *msg,char *def)
{
	DialogPtr mDialog;
	char gom[255];
	short i,item;
	Rect r;
	Handle H1,H2,H3,H4;

	mDialog=GetNewDialog(1026,0,(WindowPtr)-1);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,1,&i,&H1,&r);
	HtMacSetIText(H4,def);

	SetPort(mDialog);
	HtMacSetIText(H3,msg);
	InsetRect(&r,-4,-4);
	PenSize(3,3);
	FrameRoundRect(&r,16,16);
	PenNormal();

	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}

	HtMacGetIText(gom,H4);
	DisposeDialog(mDialog);
	if(item!=2)
	{
		strcpy(str,(char *)gom);
		return HT_YES;
	}
	else
	{
		strcpy(str,"");
		return HT_NO;
	}
}


int HtYesNo(char *msg,char *yes,char *no)
{
	DialogPtr mDialog;
	unsigned char gom[255];
	short i,item;
	Rect r_yes,r_no,r;
	Handle H1,H2,H3;

	mDialog=GetNewDialog(1027,0,(WindowPtr)-1);
	GetDItem(mDialog,1,&i,&H1,&r_yes);
	GetDItem(mDialog,2,&i,&H2,&r_no);
	GetDItem(mDialog,3,&i,&H3,&r);
	SetPort(mDialog);

	InsetRect(&r_yes,2,2);
	InsetRect(&r_no,2,2);

	yes=(yes!=NULL ? yes : "OK");
	no =(no !=NULL ? no  : "Cancel");

	TextBox(yes,strlen(yes),&r_yes,1);
	InsetRect(&r_yes,-6,-6);
	PenSize(3,3);
	FrameRoundRect(&r_yes,16,16);
	PenNormal();

	TextBox(no,strlen(no),&r_no,1);

	HtMacSetIText(H3,msg);
	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}
	DisposeDialog(mDialog);

	return (item==1 ? HT_YES : HT_NO);
}


int HtYesNo3(char *msg[3],char *yes,char *no)
{
	DialogPtr mDialog;
	unsigned char gom[255];
	short i,item;
	Rect r_yes,r_no,r;
	Handle H1,H2,H3,H4,H5;

	mDialog=GetNewDialog(1028,0,(WindowPtr)-1);
	GetDItem(mDialog,1,&i,&H1,&r_yes);
	GetDItem(mDialog,2,&i,&H2,&r_no);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,5,&i,&H5,&r);
	SetPort(mDialog);

	yes=(yes!=NULL ? yes : "OK");
	no =(no !=NULL ? no  : "Cancel");

	InsetRect(&r_yes,2,2);InsetRect(&r_no,2,2);
	TextBox(yes,strlen(yes),&r_yes,1);
	InsetRect(&r_yes,-6,-6);
	PenSize(3,3);
	FrameRoundRect(&r_yes,16,16);
	PenNormal();

	TextBox(no,strlen(no),&r_no,1);

	HtMacSetIText(H3,msg[0]);
	HtMacSetIText(H4,msg[1]);
	HtMacSetIText(H5,msg[2]);

	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}
	DisposeDialog(mDialog);
	return (item==1 ? HT_YES : HT_NO);
}

int HtInputNumber(real *n,real dflt,char *msg,char *yes,char *no)
{
	DialogPtr mDialog;
	char gom[255];
	short i,item;
	Rect r,r_yes,r_no;
	Handle H1,H2,H3,H4;

	mDialog=GetNewDialog(1029,0,(WindowPtr)-1);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,1,&i,&H1,&r_yes);
	GetDItem(mDialog,2,&i,&H2,&r_no);

	sprintf(gom,"%f",dflt);
	HtMacSetIText(H4,gom);
	SetPort(mDialog);
	SelIText(mDialog,4,0,128);
	HtMacSetIText(H3,msg);

	yes=(yes!=NULL ? yes : "OK");
	no =(no !=NULL ? no  : "Cancel");

	InsetRect(&r_yes,2,2);
	InsetRect(&r_no,2,2);
	TextBox(yes,strlen(yes),&r_yes,1);

	InsetRect(&r_yes,-6,-6);
	PenSize(3,3);
	FrameRoundRect(&r_yes,16,16);
	PenNormal();

	TextBox(no,strlen(no),&r_no,1);

	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}

	HtMacGetIText(gom,H4);
	DisposeDialog(mDialog);
	if(item!=2)
	{
		*n=atof(gom);
		return HT_YES;
	}
	else{
		*n=0.0;
		return HT_NO;
	}
}


int HtInputPoint2(BIPOINT2 *p,BIPOINT2 *df,char *msg,char *yes,char *no)
{
	DialogPtr mDialog;
	char gom[255];
	short i,item;
	Rect r,r_yes,r_no;
	Handle H1,H2,H3,H4,H5;


	mDialog=GetNewDialog(1030,0,(WindowPtr)-1);

	GetDItem(mDialog,1,&i,&H1,&r_yes);
	GetDItem(mDialog,2,&i,&H2,&r_no);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,5,&i,&H5,&r);

	if(df != NULL)
	{
		sprintf(gom,"%f",(*df).x);
		HtMacSetIText(H4,gom);
		sprintf(gom,"%f",(*df).y);
		HtMacSetIText(H5,gom);
	}
	else
	{
		HtMacSetIText(H4,"0.0000");
		HtMacSetIText(H5,"0.0000");
	}

	SetPort(mDialog);
	SelIText(mDialog,4,0,128);
	HtMacSetIText(H3,msg);

	yes=(yes!=NULL ? yes : "OK");
	no =(no !=NULL ? no  : "Cancel");

	InsetRect(&r_yes,2,2);
	InsetRect(&r_no,2,2);
	TextBox(yes,strlen(yes),&r_yes,1);

	InsetRect(&r_yes,-6,-6);
	PenSize(3,3);
	FrameRoundRect(&r_yes,16,16);
	PenNormal();

	TextBox(no,strlen(no),&r_no,1);

	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}

	HtMacGetIText(gom,H4);
	p->x=atof(gom);
	HtMacGetIText(gom,H5);
	p->y=atof((char *)gom);
	DisposeDialog(mDialog);

	return (item!=2 ? HT_YES : HT_NO);
}


int HtInputPoint3(BIPOINT *p,BIPOINT *df,char *msg,char *yes,char *no)
{
	DialogPtr mDialog;
	char gom[255];
	short i,item;
	Rect r,r_yes,r_no;
	Handle H1,H2,H3,H4,H5,H6;

	mDialog=GetNewDialog(1031,0,(WindowPtr)-1);

	GetDItem(mDialog,1,&i,&H1,&r_yes);
	GetDItem(mDialog,2,&i,&H2,&r_no);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,5,&i,&H5,&r);
	GetDItem(mDialog,6,&i,&H6,&r);

	if(df != NULL)
	{
		sprintf(gom,"%f",(*df).x);
		HtMacSetIText(H4,gom);
		sprintf(gom,"%f",(*df).y);
		HtMacSetIText(H5,gom);
		sprintf(gom,"%f",(*df).z);
		HtMacSetIText(H6,gom);
	}
	else
	{
		HtMacSetIText(H4,"0.0000");
		HtMacSetIText(H5,"0.0000");
		HtMacSetIText(H6,"0.0000");
	}

	SetPort(mDialog);
	SelIText(mDialog,4,0,128);
	HtMacSetIText(H3,msg);

	yes=(yes!=NULL ? yes : "OK");
	no =(no !=NULL ? no  : "Cancel");

	InsetRect(&r_yes,2,2);
	InsetRect(&r_no,2,2);
	TextBox(yes,strlen(yes),&r_yes,1);

	InsetRect(&r_yes,-6,-6);
	PenSize(3,3);
	FrameRoundRect(&r_yes,16,16);
	PenNormal();

	TextBox(no,strlen(no),&r_no,1);

	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}
	HtMacGetIText(gom,H4);
	p->x=atof(gom);
	HtMacGetIText(gom,H5);
	p->y=atof(gom);
	HtMacGetIText(gom,H6);
	p->z=atof(gom);

	DisposeDialog(mDialog);

	return (item!=2 ? HT_YES : HT_NO);
}




/***********************************************************************/
int HtInputAngle3(BIANGLE *a,BIANGLE *df,char *msg,char *yes,char *no)
{
	DialogPtr mDialog;
	char gom[255];
	short i,item;
	Rect r,r_yes,r_no;
	Handle H1,H2,H3,H4,H5,H6;

	mDialog=GetNewDialog(1032,0,(WindowPtr)-1);

	GetDItem(mDialog,1,&i,&H1,&r_yes);
	GetDItem(mDialog,2,&i,&H2,&r_no);
	GetDItem(mDialog,3,&i,&H3,&r);
	GetDItem(mDialog,4,&i,&H4,&r);
	GetDItem(mDialog,5,&i,&H5,&r);
	GetDItem(mDialog,6,&i,&H6,&r);

	if(df != NULL)
	{
		sprintf(gom,"%f",(*df).h);
		HtMacSetIText(H4,gom);
		sprintf(gom,"%f",(*df).p);
		HtMacSetIText(H5,gom);
		sprintf(gom,"%f",(*df).b);
		HtMacSetIText(H6,gom);
	}
	else
	{
		HtMacSetIText(H4,"0.0000");
		HtMacSetIText(H5,"0.0000");
		HtMacSetIText(H6,"0.0000");
	}

	SetPort(mDialog);
	SelIText(mDialog,4,0,128);
	HtMacSetIText(H3,msg);

	yes=(yes!=NULL ? yes : "OK");
	no =(no !=NULL ? no  : "Cancel");

	InsetRect(&r_yes,2,2);
	InsetRect(&r_no,2,2);
	TextBox(yes,strlen(yes),&r_yes,1);

	InsetRect(&r_yes,-6,-6);
	PenSize(3,3);
	FrameRoundRect(&r_yes,16,16);
	PenNormal();
	TextBox(no,strlen(no),&r_no,1);

	item=0;
	while(item!=1 && item!=2)
	{
		ModalDialog(0,&item);
	}

	HtMacGetIText(gom,H4);
	a->h=(long)(atof((char *)gom)*65536/360);
	HtMacGetIText(gom,H5);
	a->p=(long)(atof((char *)gom)*65536/360);
	HtMacGetIText(gom,H6);
	a->b=(long)(atof((char *)gom)*65536/360);
	DisposeDialog(mDialog);

	return (item!=2 ? HT_YES : HT_NO);
}

int HtInputInteger(int *n,int dflt,char *msg,char *yes,char *no)
{
	real ddf;
	real dn;
	int ret;
	ddf=(real)dflt;
	ret=HtInputNumber(&dn,ddf,msg,yes,no);
	*n=(int)dn;
	return ret;
}

int HtInputPoint2i(int p[2],int df[2],char *msg,char *yes,char *no)
{
	BIPOINT2 ddf,dp;
	int ret;
	if(df==NULL)
	{
		ddf.x=0;
		ddf.y=0;
	}
	else
	{
		ddf.x=df[0];
		ddf.y=df[1];
	}

	ret=HtInputPoint2(&dp,&ddf,msg,yes,no);
	p[0]=(int)dp.x;
	p[1]=(int)dp.y;
	return ret;
}

int HtInputPoint3i(int p[3],int df[3],char *msg,char *yes,char *no)
{
	BIPOINT ddf,dp;
	int ret;

	if(df==NULL)
	{
		ddf.x=0;
		ddf.y=0;
		ddf.z=0;
	}
	else
	{
		ddf.x=df[0];
		ddf.y=df[1];
		ddf.z=df[2];
	}

	ret=HtInputPoint3(&dp,&ddf,msg,yes,no);
	p[0]=(int)dp.x;
	p[1]=(int)dp.y;
	p[2]=(int)dp.z;
	return ret;
}




/***********************************************************************/
int HtLoadFileDialog(char *fn,char *msg,char *df,char *ext)
{
	StandardFileReply rep;
	SFTypeList myType;

	StandardGetFile(0,-1,myType,&rep);
	if(rep.sfGood)
	{
		HtMacGetPathName((char *)rep.sfFile.name,rep.sfFile.vRefNum,rep.sfFile.parID);
		PtoCstr(rep.sfFile.name);
		strcpy(fn,(char *)rep.sfFile.name);
		return HT_YES;
	}
	return HT_NO;
}

int HtSaveFileDialog(char *fn,char *msg,char *df,char *ext)
{
	StandardFileReply rep;
	SFTypeList myType;
	char GotExtension[16],DefaultFilename[255];
	char *pt;
	int flag;// きちんとした拡張子が検出されたか //

	flag=0;
	pt=ext;
	while(pt!='\0')
	{
		if(*pt=='.')
		{
		    flag=1;
			strcpy(GotExtension,pt);
			break;
		}
		pt++;
	}

	strcpy(DefaultFilename,"untitled");
	if(flag==1)
	{
		strcat(DefaultFilename,GotExtension);
	}

	StandardPutFile(CtoPstr(msg), CtoPstr(DefaultFilename),&rep);
	if(rep.sfGood)
	{
		HtMacGetPathName((char *)rep.sfFile.name,rep.sfFile.vRefNum,rep.sfFile.parID);
		PtoCstr(rep.sfFile.name);
		strcpy(fn,(char *)rep.sfFile.name);
		return HT_YES;
	}
	return HT_NO;
}

Boolean HtMacGetPathName(char *pathStr,short oVRef,long oParID)
{
	Str255 fName;
	FSSpec resolved;
	OSErr er;
	Boolean result;
	char gom[255];

	fName[0]=0;
	er=FSMakeFSSpec(oVRef,oParID,fName,&resolved);
	if(er!=noErr)
	{
		return false;
	}

	sprintf(gom,"%s:%s",PtoCstr(resolved.name),PtoCstr((unsigned char *)pathStr));
	strcpy(pathStr,gom);
	CtoPstr(pathStr);
	result=HtMacGetPathName(pathStr,resolved.vRefNum,resolved.parID);
}




/***********************************************************************/
int HtColorDialog(BICOLOR *col,BICOLOR *df,char *msg)
{
	unsigned char gom[255];
	RGBColor icol,ocol;
	Point pt;
	Boolean ret;

	strcpy((char *)gom,msg);
//	GetMouse(&pt);
//	LocalToGlobal(&pt);
	pt.h=40;
	pt.v=40;
	CtoPstr((char *)gom);

	icol.red  =df->r*256+df->r;
	icol.blue =df->b*256+df->b;
	icol.green=df->g*256+df->g;
	ret=GetColor(pt,gom,&icol,&ocol);

	col->r=ocol.red/256&255;
	col->b=ocol.blue/256&255;
	col->g=ocol.green/256&255;

	return (ret==true ? HT_YES : HT_NO);
}




/***********************************************************************/
void HtSetOnLeftButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtOnLeftButton=f;
}
void HtResetOnLeftButtonFunc(void)
{
	HtOnLeftButton=HtDummyButtonFunc;
}

void HtSetOnMiddleButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtOnMiddleButton=f;
}
void HtResetOnMiddleButtonFunc(void)
{
	HtOnMiddleButton=HtDummyButtonFunc;
}

void HtSetOnRightButtonFunc(void (*f)(int trig,HTMOUSE *mos))
{
	HtOnRightButton=f;
}
void HtResetOnRightButtonFunc(void)
{
	HtOnRightButton=HtDummyButtonFunc;
}

void HtSetOnDragFunc(void (*f)(HTMOUSE *mos))
{
	HtOnDrag=f;
}
void HtResetOnDragFunc(void)
{
	HtOnDrag=HtDummyDragFunc;
}

void HtSetRepaintFunc(void (*f)(void))
{
	HtRepaintFunc=f;
}

void HtSetIntervalFunc(void (*f)(void))
{
	HtIntervalFunc=f;
}

void HtResetIntervalFunc(void)
{
	HtIntervalFunc=HtDummyVoidFunc;
}

/****************************************************************************

                         Shift Key Status Routine

****************************************************************************/
void HtGetShift(HTSHIFT *sft)
{
	KeyMap mkey;
	GetKeys(mkey);

	if((mkey[1] & 0x0001) > 0)
	{
		sft->shift=HT_ON;
	}
	else
	{
		sft->shift=HT_OFF;
	}

	if((mkey[1] & 0x0008) > 0)
	{
		sft->ctrl=HT_ON;
	}
	else
	{
		sft->ctrl=HT_OFF;
	}
}

static int HtMacVkToKc(unsigned char Vk)
{
    if(Vk==BIKEY_STOP    )return 0x77;
    if(Vk==BIKEY_COPY    )return 0   ;
    if(Vk==BIKEY_F1      )return 0x7A;
    if(Vk==BIKEY_F2      )return 0x78;
    if(Vk==BIKEY_F3      )return 0x63;
    if(Vk==BIKEY_F4      )return 0x76;
    if(Vk==BIKEY_F5      )return 0x60;
    if(Vk==BIKEY_F6      )return 0x61;
    if(Vk==BIKEY_F7      )return 0x62;
    if(Vk==BIKEY_F8      )return 0x64;
    if(Vk==BIKEY_F9      )return 0x65;
    if(Vk==BIKEY_F10     )return 0x6D;
    if(Vk==BIKEY_ONE     )return 0x12;
    if(Vk==BIKEY_TWO     )return 0x13;
    if(Vk==BIKEY_THREE   )return 0x14;
    if(Vk==BIKEY_FOUR    )return 0x15;
    if(Vk==BIKEY_FIVE    )return 0x17;
    if(Vk==BIKEY_SIX     )return 0x16;
    if(Vk==BIKEY_SEVEN   )return 0x1A;
    if(Vk==BIKEY_EIGHT   )return 0x1C;
    if(Vk==BIKEY_NINE    )return 0x19;
    if(Vk==BIKEY_ZERO    )return 0x1D;
    if(Vk==BIKEY_A       )return 0x00;
    if(Vk==BIKEY_B       )return 0x0B;
    if(Vk==BIKEY_C       )return 0x08;
    if(Vk==BIKEY_D       )return 0x02;
    if(Vk==BIKEY_E       )return 0x0E;
    if(Vk==BIKEY_F       )return 0x03;
    if(Vk==BIKEY_G       )return 0x05;
    if(Vk==BIKEY_H       )return 0x04;
    if(Vk==BIKEY_I       )return 0x22;
    if(Vk==BIKEY_J       )return 0x26;
    if(Vk==BIKEY_K       )return 0x28;
    if(Vk==BIKEY_L       )return 0x25;
    if(Vk==BIKEY_M       )return 0x2E;
    if(Vk==BIKEY_N       )return 0x2D;
    if(Vk==BIKEY_O       )return 0x1F;
    if(Vk==BIKEY_P       )return 0x23;
    if(Vk==BIKEY_Q       )return 0x0C;
    if(Vk==BIKEY_R       )return 0x0F;
    if(Vk==BIKEY_S       )return 0x01;
    if(Vk==BIKEY_T       )return 0x11;
    if(Vk==BIKEY_U       )return 0x20;
    if(Vk==BIKEY_V       )return 0x09;
    if(Vk==BIKEY_W       )return 0x0D;
    if(Vk==BIKEY_X       )return 0x07;
    if(Vk==BIKEY_Y       )return 0x10;
    if(Vk==BIKEY_Z       )return 0x06;
    if(Vk==BIKEY_SPACE   )return 0x31;
    if(Vk==BIKEY_ESC     )return 0x35;
    if(Vk==BIKEY_TAB     )return 0x30;
//    if(Vk==BIKEY_CTRL    )return 0x3B;
//    if(Vk==BIKEY_SHIFT   )return 0x38;
    if(Vk==BIKEY_ALT     )return 0   ;
    if(Vk==BIKEY_BS      )return 0x33;
    if(Vk==BIKEY_RET     )return 0x24;
    if(Vk==BIKEY_HOME    )return 0x73;
    if(Vk==BIKEY_DEL     )return 0x71;
    if(Vk==BIKEY_UP      )return 0x7E;
    if(Vk==BIKEY_DOWN    )return 0x7D;
    if(Vk==BIKEY_LEFT    )return 0x7B;
    if(Vk==BIKEY_RIGHT   )return 0x7C;
    if(Vk==BIKEY_TEN0    )return 0x52;
    if(Vk==BIKEY_TEN1    )return 0x53;
    if(Vk==BIKEY_TEN2    )return 0x54;
    if(Vk==BIKEY_TEN3    )return 0x55;
    if(Vk==BIKEY_TEN4    )return 0x56;
    if(Vk==BIKEY_TEN5    )return 0x57;
    if(Vk==BIKEY_TEN6    )return 0x58;
    if(Vk==BIKEY_TEN7    )return 0x59;
    if(Vk==BIKEY_TEN8    )return 0x5B;
    if(Vk==BIKEY_TEN9    )return 0x5C;
    if(Vk==BIKEY_TENMUL  )return 0x43;
    if(Vk==BIKEY_TENDIV  )return 0x4B;
    if(Vk==BIKEY_TENPLUS )return 0x45;
    if(Vk==BIKEY_TENMINUS)return 0x4E;
    if(Vk==BIKEY_TENEQUAL)return 0x51;
    if(Vk==BIKEY_TENDOT  )return 0x41;
    return 0x00;
}



/***********************************************************************

                    List Box Extension 1995/11/02

***********************************************************************/

//# of DLOG defined in ResEdit
#define rHtMacListDialog		4000

// Local Macros
#define HTMAC_MODE_SINGLE 0
#define HTMAC_MODE_MULTI 1


// Unavoidable Global Variable
static ListHandle HtMacTheList;
static short HtMacLBoxMode;
static Rect HtMacLBoxRect;

// Internal Function
static int HtMacListBox(int,int,int [],char *itemstr[],char *,char *,char *);
static pascal Boolean HtMacListBoxCallBack(DialogPtr,EventRecord *,short *);

static int HtMacListBox(int mode,int itemnum,int itemhit[],char *itemstr[],char *msg,char *yes,char *no);


int HtListBox(int *n,char *str[],char *msg,char *yes,char *no)
{
	int *itemhit;
	int i,nItm;
	int ret;

	for(nItm=0; str[nItm]!=NULL; nItm++);

	itemhit=(int *)BiMalloc(nItm*sizeof(int));
	if(itemhit!=NULL)
	{
		ret=HtMacListBox(HTMAC_MODE_SINGLE,nItm,itemhit,str,msg,yes,no);
		for(i=0 ; i<nItm ; i++)
		{
			if(itemhit[i]!=HT_NO)
			{
				*n=i;
				BiFree(itemhit);
				return HT_YES;
			}
		}
		BiFree(itemhit);
	}
	*n=0;
	return HT_NO;
}



int HtListBoxPl(int *nm,int n[],char *str[],char *msg,char *yes,char *no)
{
	int i,nItm,*itemhit;
	int ret;

	for(nItm=0; str[nItm]!=NULL; nItm++);

	itemhit=(int *)BiMalloc(nItm*sizeof(int));
	if(itemhit!=NULL)
	{
		ret=HtMacListBox(HTMAC_MODE_MULTI,nItm,itemhit,str,msg,yes,no);

		*nm=0;
		for(i=0 ; i<nItm ; i++)
		{
			if(itemhit[i]!=HT_NO)
			{
				n[*nm]=i;
				(*nm)++;
			}
		}
		BiFree(itemhit);
	}
	return ret;
}

static short HtListBoxLoop(DialogPtr dlog)
{
	short item;
	ModalFilterUPP HtCallBackFunc;
	item=0;
	HtCallBackFunc=NewModalFilterProc(HtMacListBoxCallBack);
	while(item!=1 && item!=2)
	{
		SetPort(dlog);
		ModalDialog(HtCallBackFunc,&item);
	}
	return item;
}

static int HtMacListBox(int mode,int itemnum,int itemhit[],char *itemstr[],char *msg,char *yes,char *no)
{
	DialogPtr dlog;                //Dialog Box Pointer
	Cell ce;                       //To Select Cell
	short px,py;                   //For Writing Coordinate
	short pw,ph;                   //Vertical Length of List Box
	Rect r,list_rect,r1,r2,r3,r4;
	Point pt;
	Boolean finish = 0;
	Boolean isEvent;
	EventRecord	event;
	int i;
	int ret;
	short item;
	Handle IH1,IH2,IH3,IH4;
	short iT1,iT2,iT3,iT4;
	unsigned char tmp[255];

	px = 8;
	py = 8;

//Vertical Size of Window is as follows...
//		Original Position of List Area
//		Part of List Area
//		Part of Button Area
//Horizontal Size of Window is as follows
//		Oritinal Position of List Area
//		Part of List Area
//		Width of Scroll Bar

	HtMacLBoxMode = mode;

	dlog=GetNewDialog(rHtMacListDialog,0L,(WindowPtr)-1L);
	GetDItem(dlog,1,&iT1,&IH1,&r1);
	GetDItem(dlog,2,&iT2,&IH2,&r2);
	GetDItem(dlog,3,&iT3,&IH3,&r3);
	GetDItem(dlog,4,&iT4,&IH4,&r4);

	SetPort(dlog);   // Begin Drawing on dlog

	yes=(yes!=NULL ? yes : "OK");
	InsetRect(&r1,2,2);
	TextBox(yes,strlen((char *)yes),&r1,1);  //1 means Centering
	InsetRect(&r1,-2,-2);

	no=(no!=NULL ? no : "Cancel");
	InsetRect(&r2,2,2);
	TextBox(no,strlen((char *)no),&r2,1);    //1 means Centering
	InsetRect(&r2,-2,-2);

	msg=(msg!=NULL ? msg : "Select Item");
	strcpy((char *)tmp,msg);
	CtoPstr((char *)tmp);
	SetIText(IH3 , tmp);

	InsetRect(&r4,1,1);  // Generate List
	FrameRect(&r4);
	InsetRect(&r4,1,1);

	HtMacLBoxRect.left = r4.left;
	HtMacLBoxRect.right = r4.right+16;
	HtMacLBoxRect.top = r4.top;
	HtMacLBoxRect.bottom = r4.bottom;

	SetRect(&list_rect, 0, 0, 1, itemnum);
	SetPt(&pt,(r4.right-r4.left),16);
	TextSize(12);
	HtMacTheList=LNew(&r4,&list_rect,pt,0,dlog,true,false,false,true);
	if(HtMacTheList==0)
	{
		return(-1);
	}

	HLock((Handle)HtMacTheList);
	if(mode==HTMAC_MODE_SINGLE)
	{
		(**HtMacTheList).selFlags  =-128;//同時に選択できるのは一つだけ
	}
	for(i=0 ;i<itemnum ;i++)
	{
		SetPt(&ce,0, i);
		LSetCell(itemstr[i],strlen(itemstr[i]),ce,HtMacTheList);
	}
	HUnlock((Handle)HtMacTheList);
	LUpdate((**HtMacTheList).port->visRgn,HtMacTheList);

	InsetRect(&r1,-4,-4);
	PenSize(3,3);
	FrameRoundRect(&r1,16,16);

	item=HtListBoxLoop(dlog);

	ret = HT_NO;
	if(item ==1)
	{
		ret = HT_YES;
		for( i = 0 ; i < itemnum ; i++)
		{
			SetPt(&ce,0, i);
			if(LGetSelect(false,&ce,HtMacTheList)==true)
			{
				itemhit[i] = HT_YES;
			}
			else
			{
				itemhit[i] = HT_NO;
			}
		}
	}
	LDispose(HtMacTheList);
	DisposeDialog(dlog);
	return ret;
}

pascal Boolean HtMacListBoxCallBack(DialogPtr dlog,EventRecord *event,short *itemHit)
{
	Point pt;

	if (event->what==keyDown || event->what==autoKey)
	{
		// Command+"." -> Cancel
		if((event->modifiers & cmdKey)!=0)
		{
			if((event->message & keyCodeMask)==0x2F00)
			{
				*itemHit = 2;
				return true;
			}
		}
		// esc -> Cancel
		if(((event->message & keyCodeMask)>>8)==0x35)
		{
			*itemHit = 2;
			return true;
		}
		// return or enter -> OK
		if((event->message & keyCodeMask)==0x4C00 || (event->message & keyCodeMask)==0x2400)
		{
			*itemHit=1;
			return true;
		}
		return false;
	}
	if(event->what==mouseDown)
	{
		SetPt(&pt,event->where.h,event->where.v);

		GlobalToLocal(&pt);
		if( PtInRect(pt,&HtMacLBoxRect)==false)
		{
			return false;
		}
		if(HtMacLBoxMode==HTMAC_MODE_SINGLE)
		{
			event->modifiers&=0xfeff;
		}
		else
		{
			event->modifiers|=0x0100;
		}
		LClick(pt,event->modifiers,HtMacTheList);
		LUpdate( (**HtMacTheList).port->visRgn,HtMacTheList);
		*itemHit = 3;
		return true;
	}
	return false;
}
