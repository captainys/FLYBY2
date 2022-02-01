#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <time.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>
#include <aurora.h>



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
}

void ScSvMain(void)
{
	/* Main Procedure Here */
}

void ScSvTerminate(void)
{
	/* Free All Allocated Memory Here */
	BiCloseWindow();
}

int PrintFunc(char *str)
{
	HtMessageBox(str,NULL);
	return 0;
}
