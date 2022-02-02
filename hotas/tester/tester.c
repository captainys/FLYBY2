#include <stdio.h>
#include <stdlib.h>
#include "hotas.h"



#include <impulse.h>





void TestMbox(void)
{
	char *msg[3]={"Message","Box","Test"};
	HtMessageBox("Message Box Test","Roger");
	HtMessageBox3(msg,"Affirm");
}

void TestLineInput(void)
{
	int y;
	char str[80];
	y=HtLineInput(str,"Line Input Test","Cessna015 Cleared for Takeoff");
	if(y==HT_YES)
	{
		HtMessageBox(str,NULL);
	}
}

void TestNumber(void)
{
	int y;
	real nmb;
	char str[80];
	y=HtInputNumber(&nmb,19015,"Test Number","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"%f",nmb);
		HtMessageBox(str,NULL);
	}
}

void TestInteger(void)
{
	int y,nmb;
	char str[80];
	y=HtInputInteger(&nmb,19015,"Test Integer","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"%d",nmb);
		HtMessageBox(str,NULL);
	}
}

void TestPoint2(void)
{
	int y;
	BIPOINT2 p2;
	char str[80];
	y=HtInputPoint2(&p2,NULL,"Test Point2","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"(%f,%f)",p2.x,p2.y);
		HtMessageBox(str,NULL);
	}
}

void TestPoint2i(void)
{
	int y,i[2];
	char str[80];
	y=HtInputPoint2i(i,NULL,"Test Point2i","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"%d %d",i[0],i[1]);
		HtMessageBox(str,NULL);
	}
}

void TestPoint3(void)
{
	int y;
	BIPOINT p;
	char str[80];
	y=HtInputPoint3(&p,NULL,"Test Point3","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"(%f,%f,%f)",p.x,p.y,p.z);
		HtMessageBox(str,NULL);
	}
}

void TestPoint3i(void)
{
	int y,i[3];
	char str[80];
	y=HtInputPoint3i(i,NULL,"Test Point3i","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"%d %d %d",i[0],i[1],i[2]);
		HtMessageBox(str,NULL);
	}
}

void TestAttitude(void)
{
	int y;
	BIANGLE ang;
	char str[80];
	y=HtInputAngle3(&ang,NULL,"Test Angle","O K","CANCEL");
	if(y==HT_YES)
	{
		sprintf(str,"%ld %ld %ld",ang.h,ang.p,ang.b);
		HtMessageBox(str,NULL);
	}
}

void TestLoadFileDialog(void)
{
	char str[128];
	if(HtLoadFileDialog(str,"Test Load Fdlg",NULL,"txt")==HT_YES)
	{
		HtMessageBox(str,NULL);
	}
}

void TestSaveFileDialog(void)
{
	char str[128];
	if(HtSaveFileDialog(str,"Test Save Fdlg",NULL,"txt")==HT_YES)
	{
		HtMessageBox(str,NULL);
	}
}

void TestColorDialog(void)
{
	char str[128];
	BICOLOR neo,src;
	BiSetColorRGB(&src,64,224,64);
	if(HtColorDialog(&neo,&src,"Test Choose Color")==HT_YES)
	{
		sprintf(str,"%d %d %d",neo.r,neo.g,neo.b);
		HtMessageBox(str,NULL);
	}
}

void TestListBoxSingle(void)
{
	int n;
	char str[80];
	char *itm[]={
		"F-4 Phantom",
		"F-5 Tiger2",
		"F-8 Crusader",
		"F-14 Tomcat",
		"F-15 Eagle",
		"F-16 Fighting Falcon",
		"F-18 Hornet",
		"F-22 Lightning2",
		NULL
	};

	if(HtListBox(&n,itm,"U.S. Fighters","O K","CANCEL")==HT_YES)
	{
		sprintf(str,"Selected %s",itm[n]);
		HtMessageBox(str,NULL);
	}
}

void TestListBoxPlural(void)
{
	char str[1024];
	char *itm[]={
		"A-4 Skyhawk",
		"A-6 Intruder",
		"A-7 Corsair",
		"AV8B Harrier",
		"A-10 Thunderbolt2",
		NULL
	};
	int i,n,sel[sizeof(itm)/sizeof(char *)];

	if(HtListBoxPl(&n,sel,itm,"U.S. Attackers","O K","CANCEL")==HT_YES)
	{
		strcpy(str,"Selected ");
		for(i=0; i<n; i++)
		{
			strcat(str,itm[sel[i]]);
			strcat(str," ");
		}
		HtMessageBox(str,NULL);
	}
}

void TestBiDrawRect2(void)
{
	long wid,hei;
	BICOLOR neo,src;
	BIPOINTS p1,p2;
	BiSetColorRGB(&src,64,224,64);
	if(HtColorDialog(&neo,&src,"Test Choose Color")==HT_YES)
	{
		BiGetWindowSize(&wid,&hei);
		BiSetPointS(&p1,0,0);
		BiSetPointS(&p2,wid-1,hei-1);
		BiClearScreen();
		BiDrawRect2(&p1,&p2,&neo);
		BiSwapBuffers();
	}
}

/***********************************************************************/
char *mn[]={"File","Test","BlueImpulseLib",NULL};

/* Blue Impulse Library用ワークエリア */
#define BIWORKSIZE 0xff00
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

BIAPPENTRY
{
	BIAPPINIT;
	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork2,BIWORKSIZE);
	BiDeviceInitialize();
	BiSetShadMode(BI_ON);

	HtInitialize();

	HtStartSetMenu(mn);

	HtSetMenuItem(1,"Exit",'Q',HtQuitProgram);

	HtSetMenuItem(2,"MBox", 0 ,TestMbox);
	HtSetMenuItem(2,"LineInput",0,TestLineInput);
	HtSetMenuItem(2,"Number Box",0,TestNumber);
	HtSetMenuItem(2,"Integer Box",0,TestInteger);
	HtSetMenuItem(2,"Point2",0,TestPoint2);
	HtSetMenuItem(2,"2 Integers",0,TestPoint2i);
	HtSetMenuItem(2,"Point3",0,TestPoint3);
	HtSetMenuItem(2,"3 Integers",0,TestPoint3i);
	HtSetMenuItem(2,"Attitude",0,TestAttitude);
	HtSetMenuItem(2,"Load Fdlg",0,TestLoadFileDialog);
	HtSetMenuItem(2,"Save Fdlg",0,TestSaveFileDialog);
	HtSetMenuItem(2,"Color Dialog",0,TestColorDialog);
	HtSetMenuItem(2,"Single List Box",0,TestListBoxSingle);
	HtSetMenuItem(2,"Plural List Box",0,TestListBoxPlural);

	HtSetMenuItem(3,"Filled Rectangle",0,TestBiDrawRect2);

	HtEndSetMenu();

	HtOpenWindow(640,480);
	HtEventLoop();
	return 0;
}
