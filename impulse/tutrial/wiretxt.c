#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0x20000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

void WireTextMain();

int main(int ac,char *av[])
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiGraphSetBuffer2(BiWork2,BIWORKSIZE);
	BiGraphSetBuffer3(BiWork3,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("WireText");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	WireTextMain();

	BiCloseWindow();
	return 0;
}

void WireTextMain()
{
	BIPROJ prj;
	BI3DFONT fnt;
	BIPOSATT pos;

	char *txt[]=
	{
		"SOLID",
		"FONT",
		NULL
	};

	BiSetPoint(&pos.p,0,0,40);
	BiSetAngleDeg(&pos.a,180,0,0);

	if(BiMakeStdWireFont(&fnt,10.0F,&BiBlue)==BI_OK)
	{
		while(BiInkey()==BIKEY_NULL)
		{
			BiUpdateDevice();

			BiGetStdProjection(&prj);
			BiSetProjection(&prj);

			BiClearScreen();
			BiStartBuffer(&BiOrgPA);

			pos.a.h=(pos.a.h+256)&0xffff;
			if(0x4000<pos.a.h && pos.a.h<0xc000)
			{
				BiInsSolidString(&pos,&fnt,"(^_^;)",BIMG_CENTER,BIMG_CENTER);
			}
			else
			{
				BiInsSolidText(&pos,&fnt,txt,BIMG_CENTER,BIMG_CENTER);
			}

			BiFlushBuffer();
			BiSwapBuffers();
		}

		BiFreeSolidFont(&fnt);
	}
}
