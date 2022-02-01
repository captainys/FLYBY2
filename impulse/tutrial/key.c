#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0xf000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void KeyBoardMain(void);
void DrawCursor(long x,long y);
void ClearKeyBuffer(void);

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("key");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);

	KeyBoardMain();

	BiCloseWindow();
	return 0;
}

void KeyBoardMain(void)
{
	int key;
	long x,y;

	x=0;
	y=0;
	while((key=BiInkey())!=BIKEY_X && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();

		if(BiKey(BIKEY_LEFT)==BI_ON)
		{
			x--;
			ClearKeyBuffer();
		}
		if(BiKey(BIKEY_RIGHT)==BI_ON)
		{
			x++;
			ClearKeyBuffer();
		}
		if(BiKey(BIKEY_UP)==BI_ON)
		{
			y--;
			ClearKeyBuffer();
		}
		if(BiKey(BIKEY_DOWN)==BI_ON)
		{
			y++;
			ClearKeyBuffer();
		}

		BiClearScreen();
		DrawCursor(x,y);
		BiSwapBuffers();
	}
}

void DrawCursor(long x,long y)
{
	long wid,hei;
	BIPOINTS s1,s2;
	BICOLOR col;

	BiGetWindowSize(&wid,&hei);

	BiSetColorRGB(&col,255,255,255);
	BiSetPointS(&s1,wid* x   /40  ,hei* y   /25);
	BiSetPointS(&s2,wid*(x+1)/40-1,hei*(y+1)/25-1);
	BiDrawRect2(&s1,&s2,&col);
}

void ClearKeyBuffer(void)
{
	while(BiInkey()!=BIKEY_NULL)
	{
		BiUpdateDevice();
	}
}
