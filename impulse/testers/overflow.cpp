#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int ac,char *av[])
{
	BIPROJ prj;
	BIPC2 pc2;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("ApplicationWindow");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	BIPOSATT view={{1120.0F,2.0F,420.0F},{-45400,0,0}};
	BIPOSATT mapPos={{-81636.34,0.00,-68993.46},{0,0,0}};

	BiLoadPc2(&pc2,"miyagi.pc2");

	BiPitchUp(&mapPos.a,&mapPos.a,-16384,0);

	int key;
	while((key=BiInkey())!=BIKEY_Q && BiFatal()==BI_FALSE)
	{
		BiUpdateDevice();
		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		BiClearScreen();
		BiStartBuffer(&view);

		BiPushMatrix(&mapPos);
		BiOvwPc2(&pc2,&BiOrgPA);
		BiPopMatrix();

		BiFlushBuffer();


		BIAXIS mapAxis,viewAxis;
		BIPOINT2 *p2;
		int i,nOrg,nTrans,nClip,nScrn;
		BIPOINT org[256],trans[256],clip[256];
		BIPOINTS scrn[256];

		nOrg=pc2.obj[0].objUni.npnt.n;
		p2=pc2.obj[0].objUni.npnt.p;

		for(i=0; i<nOrg; i++)
		{
			BiSetPoint(&org[i],p2[i].x,p2[i].y,0.0F);
		}


		BiMakeAxis(&mapAxis,&mapPos);
		BiMakeAxis(&viewAxis,&view);
// {
		nTrans=nOrg;
		for(i=0; i<nOrg; i++)
		{
			BiConvLtoG(&trans[i],&org[i],&mapAxis);
			BiConvGtoL(&trans[i],&trans[i],&viewAxis);
		}

//		BIAXIS cnvAxis;
///*	in i3dg.c BiConvAxisGtoL(&BiCnvAxs,&BiMdlAxs,&BiEyeAxs); */
//		BiConvAxisGtoL(&cnvAxis,&mapAxis,&viewAxis);
//		nTrans=nOrg;
//		for(i=0; i<nOrg; i++)
//		{
//			BiConvLtoG(&trans[i],&org[i],&cnvAxis);
//		}
//}

		BiNearClipPolyg(&nClip,clip,nTrans,trans,prj.nearz);

		nScrn=nClip;
		BiPlProject(nScrn,scrn,clip,&prj);

//		BiDrawPolyg2(nScrn,scrn,&BiBlue);

		BiSwapBuffers();

		if(key==BIKEY_DOWN)
		{
			view.a.p-=450;
			printf("%d\n",view.a.p);
		}
		else if(key==BIKEY_UP)
		{
			view.a.p+=450;
			printf("%d\n",view.a.p);
		}
	}

	BiCloseWindow();
	return 0;
}
