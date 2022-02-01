#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <time.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>
#include <aurora.h>

#include <windows.h>


BOOL WINAPI ScreenSaverConfigureDialog(HWND w,UINT msg,WPARAM wp,LPARAM lp)
{
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hinst)
{
	return TRUE;
}

void GetSizeLimit(int *sizeLimitX,int *sizeLimitY)
{
	*sizeLimitX=800;
	*sizeLimitY=600;
}

/***********************************************************************

  Windows用スクリーンセーバースケルトン

  注意点
  (1)データファイルが必要なときは c:\\windows\\???? のように、フルパス
     で指定する。
  (2)BiLoad??? の代わりに ArLoad??? を使う。
  (3)ArLoad??? で読み込んだオブジェクトは、ScSvTerminate関数の中で必ず
     ArFree??? でメモリを解放する。
  (4)プロジェクトワークスペースは、vc40_s.mdp,vc40_s.mak を使う。
     (VC++ 4.0専用)

***********************************************************************/


int PrintFunc(char *str);

#define BIWORKSIZE 0x20000L
char BiWork[BIWORKSIZE];

BI3DFONT fnt;

void ScSvInitialize(void)
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();

	BiSetWindowName("Screen Saver Skeleton");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	HtInitialize();

	ArInitialize();

	SkSetPrintFunc(PrintFunc);

	BiMakeStdSolidFont(&fnt,1.0F,&BiBlue);
}

void ScSvMain(void)
{
	BIPOSATT pos;

	BiSetPoint(&pos.p,0,0,30);
	BiSetAngleDeg(&pos.a,0,-15,0);

	while(1)
	{
		BiClearScreen();
		BiStartBuffer(&BiOrgPA);
		BiInsSolidString(&pos,&fnt,"BI-SCREEN SAVER",BIMG_CENTER,BIMG_CENTER);
		BiFlushBuffer();
		BiSwapBuffers();

		pos.a.h+=910;
	}
}

void ScSvTerminate(void)
{
	BiFreeSolidFont(&fnt);
	BiCloseWindow();
}

int PrintFunc(char *str)
{
	HtMessageBox(str,NULL);
	return 0;
}
