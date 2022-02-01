// extract fonts

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <impulse.h>

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];



typedef struct
{
	int nPolyg;
	int *nVtx;    /* Number of vertices of Polygon N */
	double *vtx;  /* Just x and y */
} VectorFont;



VectorFont vf[128];



void ExtractPolygons(VectorFont *vf,BISRF *srf);
void MakeTable(VectorFont vf[]);


int main(int ac,char *av[])
{
	unsigned char c;
	BIPROJ prj;

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

	/* Main Procedure Here */

	MakeTable(vf);

	int key;
	c='A';
	while((key=BiInkey())!=BIKEY_ESC)
	{
		BiUpdateDevice();

		BiClearScreen();

		if(key!=BIKEY_NULL)
		{
			c=BiKeyToAscii(key);
			printf("[%c] %d\n",c,vf[c].nPolyg);
		}

		int i;
		BIPOINTS s[256];
		double *seeker;
		seeker=vf[c].vtx;
		for(i=0; i<vf[c].nPolyg; i++)
		{
			int j;
			for(j=0; j<vf[c].nVtx[i]; j++)
			{
				s[j].x=long(320.0F+(*(seeker++))*100.0F);
				s[j].y=long(240.0F-(*(seeker++))*100.0F);
			}
			BiDrawPolyg2(vf[c].nVtx[i],s,&BiWhite);
		}

		BiSwapBuffers();
	}

	BiCloseWindow();
	return 0;
}


void MakeTable(VectorFont vf[])
{
	BI3DFONT bifont;
	BiMakeStdSolidFont(&bifont,1.0,&BiWhite);

	int i;
	for(i=0; i<128; i++)
	{
		printf("%d\n",i);
		if(bifont.srf[i]==NULL)
		{
			vf[i].nPolyg=0;
			vf[i].nVtx=NULL;
			vf[i].vtx=NULL;
		}
		else
		{
			ExtractPolygons(&vf[i],(BISRF *)bifont.srf[i]);
		}
	}
}


void ExtractPolygons(VectorFont *vf,BISRF *srf)
{
	int i,j,k;
	int nPolygTmp;
	int nVtxTmp[1024];
	double vtxTmp[1024];

	nPolygTmp=0;
	k=0;
	for(i=0; i<srf->np; i++)
	{
		for(j=0; j<srf->p[i].nVt; j++)
		{
			if(srf->v[srf->p[i].vt[j]].p.z<0.15)
			{
				goto SKIP;
			}
		}

		nVtxTmp[nPolygTmp]=srf->p[i].nVt;
		for(j=0; j<srf->p[i].nVt; j++)
		{
			BIPOINT v;
			v=srf->v[srf->p[i].vt[j]].p;
			vtxTmp[k*2  ]=v.x;
			vtxTmp[k*2+1]=v.y;
			k++;
		}
		nPolygTmp++;
	SKIP:
		;
	}

	printf("  %d %d\n",nPolygTmp,k);

	vf->nPolyg=nPolygTmp;
	vf->nVtx=(int *)malloc(sizeof(int)*nPolygTmp);
	for(i=0; i<nPolygTmp; i++)
	{
		vf->nVtx[i]=nVtxTmp[i];
	}
	vf->vtx=(double *)malloc(sizeof(double)*k*2);
	for(i=0; i<k*2; i++)
	{
		vf->vtx[i]=vtxTmp[i];
	}
}
