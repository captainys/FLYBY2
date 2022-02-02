#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <time.h>

#include <impulse.h>
#include <hotas.h>
#include <skyhawk.h>

/* Until Smoke-Routine is implemented in AURORA.LIB */
#include "asmoke.h"


typedef struct {
	int aircraft;
} SHOWOBJ;

/***************************************************************************/
int PrintFunc(char *str);
int LoadFile(void);
void FlyByMain(void);
void FlyByShow(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void FlyByStraight(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void FlyByRoll(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void FlyByLoop(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void FlyByClimb(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void FlyByEight(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void FlyBy360(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
real PassedTime(void);
void DrawScreen(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye);
void Proceed(BIPOSATT *obj,real dist);
void FlyByAhead(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,real dist);
void FlyByPitch(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,long ctr,long sgn);
void FlyByBank(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,long ctr,long sgn);
void FlyByTurn(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,long ctr,long sgn);
/***************************************************************************/

real TimeScale=1.0F;

#define BIWORKSIZE 0x20000L
char BiWork[BIWORKSIZE];
char BiWork2[BIWORKSIZE];
char BiWork3[BIWORKSIZE];

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

	BiSetWindowName("FLYBY");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	if(ac>=2)
	{
		TimeScale=BiLarger(1.0F,(real)atof(av[1]));
	}

	SkSetPrintFunc(PrintFunc);

	srand((long)time(NULL));

	if(LoadFile()==BI_OK)
	{
		FlyByMain();
	}

	BiCloseWindow();
	return 0;
}

int PrintFunc(char *str)
{
	HtMessageBox(str,NULL);
	return 0;
}

/***************************************************************************/

#define MAX_N_AIRCRAFT 64
int nFld;
BIFIELD fld[1];
int nAir;
BISRFMODEL air[MAX_N_AIRCRAFT];
real altitude=120.0F;

int DefSmokeType=ARS_SOLIDSMOKE;

/***************************************************************************/

char *script[]=
{
	"AIRCRAFT",
	"FIELD",
	"ALTITUDE",
	"SOLIDSMOKE",
	"WIRESMOKE",
	"TRAILSMOKE",
	"RIBBONSMOKE",
	NULL
};

int LoadFile(void)
{
	int ac,cmd;
	char *av[16],str[128];
	FILE *fp;

	int firstWarning;

	firstWarning=BI_OFF;
	fp=fopen("flyby.inf","r");
	if(fp!=NULL)
	{
		nAir=0;
		while(fgets(str,128,fp)!=NULL)
		{
			SkArguments(&ac,av,15,str);
			if(ac>0)
			{
				SkCommandNumber(&cmd,av[0],script);
				switch(cmd)
				{
				case 0:
					if(nAir>=MAX_N_AIRCRAFT)
					{
						if(firstWarning==BI_OFF)
						{
							SkPrintf("WARNING : Too Many Aircrafts.");
							firstWarning=BI_ON;
						}
					}
					else if(BiLoadSrf(&air[nAir],av[1])==BI_OK)
					{
						nAir++;
					}
					else
					{
						SkPrintf("Can't Open %s",av[1]);
						return BI_ERR;
					}
					break;
				case 1:
					if(BiLoadFld(&fld[0],av[1])==BI_OK)
					{
						nFld=1;
					}
					else
					{
						SkPrintf("Can't Open %s\n",av[1]);
					}
					break;
				case 2:
					altitude=(real)atof(av[1]);
					break;
				case 3:
					DefSmokeType=ARS_SOLIDSMOKE;
					break;
				case 4:
					DefSmokeType=ARS_WIRESMOKE;
					break;
				case 5:
					DefSmokeType=ARS_TRAILSMOKE;
					break;
				case 6:
					DefSmokeType=ARS_RIBBONSMOKE;
					break;
				default:
					SkPrintf("Script Error = %s",av[0]);
					return BI_ERR;
				}
			}
		}
		fclose(fp);
		return BI_OK;
	}
	SkPrintf("Can't Open [flyby.inf].");
	return BI_ERR;
}


/***************************************************************************/

real CurrentTime;
ARSMOKECLASS SmokeClass,VaporClass;
ARSMOKEINST SmokeInst,VaporInst;

int QuitFlag;

void FlyByMain(void)
{
	int dir;
	BIPOSATT obj,eye;
	SHOWOBJ show;
	int stp[3];
	BIPOINT bbx[2];

	ArInitSmokeClass(&SmokeClass);
	ArInitSmokeClass(&VaporClass);

	stp[0]=1;
	stp[1]=2;
	stp[2]=4;
	BiSetPoint(&bbx[0],200,200,200);
	BiSetPoint(&bbx[1],500,500,500);
	ArSetSmokeBoundingBox(&SmokeClass,stp,bbx);
	ArSetSmokeBoundingBox(&VaporClass,stp,bbx);

	ArSetSmokeType(&SmokeClass,DefSmokeType);
	ArSetSmokeType(&VaporClass,ARS_TRAILSMOKE);

	ArSetSmokeLife(&SmokeClass,DefSmokeType,0.2F,30.0F);
	ArSetSmokeLife(&VaporClass,ARS_TRAILSMOKE,0.1F,1.0F);

	ArSetSmokeWidth(&SmokeClass,DefSmokeType,0.0F,10.0F,3.0F);
	ArSetSmokeWidth(&VaporClass,ARS_TRAILSMOKE,10.0F,10.0F,0.0F);

	ArInitSmokeInstance(&SmokeInst,1000,100);
	ArInitSmokeInstance(&VaporInst,100,10);

	if(nAir==0)
	{
		SkPrintf("No Aircraft is specified.");
		return;
	}

	QuitFlag=BI_OFF;
	while(QuitFlag==BI_OFF)
	{
		real distance;

		distance=(real)(rand()%150+50);

		show.aircraft=rand()%nAir;

		dir=rand()%0x10000;
		obj.p.x=-500.0F*BiSin(dir);
		obj.p.y= altitude;
		obj.p.z= 500.0F*BiCos(dir);
		obj.a.h= dir+0x8000;
		obj.a.p= 0;
		obj.a.b= 0;

		dir=rand()%0x10000;
		eye.p.x=-distance*BiSin(dir);
		eye.p.y= altitude+(real)(rand()%50)-25.0F;
		eye.p.z= distance*BiCos(dir);
		eye.a.b= 0;

		FlyByShow(&show,&obj,&eye);
	}
}

void FlyByShow(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	int acro;

	CurrentTime=0.0F;
	ArClearSmokeInstance(&SmokeInst);
	ArClearSmokeInstance(&VaporInst);

	acro=rand()%6;
	switch(acro)
	{
	case 0:
		FlyByStraight(show,obj,eye);
		break;
	case 1:
		FlyByRoll(show,obj,eye);
		break;
	case 2:
		FlyByLoop(show,obj,eye);
		break;
	case 3:
		FlyByClimb(show,obj,eye);
		break;
	case 4:
		FlyByEight(show,obj,eye);
		break;
	case 5:
		FlyBy360(show,obj,eye);
		break;
	}
}

int helpCount=0;

void FlyByStraight(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	PassedTime();
	FlyByAhead(show,obj,eye,1000.0F);
}

void FlyByRoll(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	PassedTime();
	FlyByAhead(show,obj,eye,300.0F);
	FlyByBank(show,obj,eye,65536,1);
	FlyByAhead(show,obj,eye,300.0F);
}

void FlyByLoop(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	PassedTime();
	FlyByAhead(show,obj,eye,500.0F);
	FlyByPitch(show,obj,eye,65536,1);
	FlyByAhead(show,obj,eye,500.0F);
}

void FlyByClimb(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	PassedTime();
	FlyByAhead(show,obj,eye,450.0F);
	FlyByPitch(show,obj,eye,12800,1);
	FlyByAhead(show,obj,eye,500.0F);
}

void FlyByEight(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	PassedTime();
	FlyByAhead(show,obj,eye,400.0F);
	FlyByPitch(show,obj,eye,0x4000,1);
	FlyByAhead(show,obj,eye,50.0F);
	FlyByPitch(show,obj,eye,0x6000,1);
	FlyByBank(show,obj,eye,0x18000,1);
	FlyByPitch(show,obj,eye,0x6000,1);
	FlyByAhead(show,obj,eye,50.0F);
	FlyByPitch(show,obj,eye,0x6000,1);
	FlyByBank(show,obj,eye,0x18000,1);
	FlyByPitch(show,obj,eye,0x2000,1);
	FlyByAhead(show,obj,eye,400.0F);
}

void FlyBy360(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	PassedTime();
	FlyByAhead(show,obj,eye,450.0F);
	FlyByBank(show,obj,eye,12800,1);
	FlyByTurn(show,obj,eye,65536,1);
	FlyByBank(show,obj,eye,12800,-1);
	FlyByAhead(show,obj,eye,500.0F);
}

/***************************************************************************/

void FlyByAhead(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,real dist)
{
	real t,vel;
	while(QuitFlag==BI_OFF && dist>0)
	{
		DrawScreen(show,obj,eye);
		t=PassedTime();
		CurrentTime+=t;
		vel=t*100.0F;
		Proceed(obj,vel);
		dist-=vel;
	}
}

void FlyByPitch(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,long ctr,long sgn)
{
	real t,vel;
	ArBeginAppendSmokeNode(&SmokeInst);
	ArBeginAppendSmokeNode(&VaporInst);
	while(QuitFlag==BI_OFF && ctr>0)
	{
		DrawScreen(show,obj,eye);
		t=PassedTime();
		CurrentTime+=t;
		vel=t*100.0F;
		Proceed(obj,vel);
		BiPitchUp(&obj->a,&obj->a,sgn*(long)(t*8192.0F),0);
		ctr-=(long)(t*8192.0F);
		ArAppendSmokeNode(&SmokeInst,obj,CurrentTime);
		ArAppendSmokeNode(&VaporInst,obj,CurrentTime);
	}
	ArEndAppendSmokeNode(&SmokeInst);
	ArEndAppendSmokeNode(&VaporInst);
}

void FlyByBank(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,long ctr,long sgn)
{
	real t,vel;
	ArBeginAppendSmokeNode(&VaporInst);
	while(QuitFlag==BI_OFF && ctr>0)
	{
		DrawScreen(show,obj,eye);
		t=PassedTime();
		CurrentTime+=t;
		vel=t*100.0F;
		Proceed(obj,vel);
		obj->a.b+=sgn*(long)(t*32768.0F);
		ctr-=(long)(t*32768.0F);
		ArAppendSmokeNode(&VaporInst,obj,CurrentTime);
	}
	ArEndAppendSmokeNode(&VaporInst);
}

void FlyByTurn(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye,long ctr,long sgn)
{
	real t,vel;
	ArBeginAppendSmokeNode(&SmokeInst);
	ArBeginAppendSmokeNode(&VaporInst);
	while(QuitFlag==BI_OFF && ctr>0)
	{
		ArAppendSmokeNode(&SmokeInst,obj,CurrentTime);
		ArAppendSmokeNode(&VaporInst,obj,CurrentTime);
		DrawScreen(show,obj,eye);
		t=PassedTime();
		CurrentTime+=t;
		vel=t*100.0F;
		Proceed(obj,vel);
		obj->a.h+=sgn*(long)(t*8192.0F);
		ctr-=(long)(t*8192.0F);
	}
	ArEndAppendSmokeNode(&SmokeInst);
	ArEndAppendSmokeNode(&VaporInst);
}

real PassedTime(void)
{
#ifdef CLOCKS_PER_SEC
	static long last=0;
	long current;
	real pass;

	current=clock();
	pass=(real)(current-last)/(real)CLOCKS_PER_SEC;
	last=current;

	return pass*TimeScale;
#else
	return 0.1F*TimeScale;
#endif
}

void DrawScreen(SHOWOBJ *show,BIPOSATT *obj,BIPOSATT *eye)
{
	int rdr;
	BIPROJ prj;
	BIVECTOR vec,light;
	long x;
	BIPOINT p1,p2;

	BiUpdateDevice();

	rdr=0;
	switch(BiInkey())
	{
	case BIKEY_NULL:
		break;
	case BIKEY_T:
		rdr=1;
		break;
	case BIKEY_X:
		QuitFlag=BI_ON;
		break;
	default:
		helpCount=30;
		break;
	}

	BiGetStdProjection(&prj);
	prj.magx*=2.0F;
	prj.magy*=2.0F;
	BiSetProjection(&prj);

	BiSubPoint(&vec,&obj->p,&eye->p);
	BiVectorToHeadPitch(&eye->a,&vec);

	light=eye->p;
	light.y+=1000.0F;
	BiSetLightPosition(&light);

	if(rdr==1)
	{
		char fn[256];
		if(HtSaveFileDialog(fn,"TIFF FILE NAME",NULL,"*.tif")==BI_OK)
		{
			BiRenderOpen(fn,BIRENDER_TIFF,640,480);
		}
		else
		{
			rdr=0;
		}
		PassedTime();
	}

	BiClearScreen();
	BiStartBuffer(eye);

	if(nFld>0)
	{
		BICOLOR grd,sky;
		BiGetFieldGroundSky(&grd,&sky,&fld[0]);
		BiDrawGroundSky(&grd,&sky);
		BiOvwField(&fld[0],&BiOrgPA);
	}

	for(x=-20000; x<=20000; x+=1000)
	{
		BiSetPoint(&p1,x,0,-20000);
		BiSetPoint(&p2,x,0, 20000);
		BiOvwLine(&p1,&p2,&BiBlue);

		BiSetPoint(&p1,-20000,0,x);
		BiSetPoint(&p2, 20000,0,x);
		BiOvwLine(&p1,&p2,&BiBlue);
	}

	if(nFld>0)
	{
		BiInsField(&fld[0],&BiOrgPA);
	}

	BiInsSrfModel(&air[show->aircraft],obj);

	ArInsSmoke(&SmokeClass,&SmokeInst,CurrentTime,eye);
	ArInsSmoke(&VaporClass,&VaporInst,CurrentTime,eye);

	BiFlushBuffer();

	if(helpCount>0)
	{
		BIPOINTS s;
		BiSetPointS(&s,48,48);
		BiDrawString2(&s,"PRESS X-KEY TO EXIT",&BiWhite);
		helpCount--;
	}

	BiSwapBuffers();

	if(rdr==1)
	{
		BiRenderClose();
		PassedTime();
	}
}

void Proceed(BIPOSATT *obj,real dist)
{
	BIVECTOR vec;
	BiSetPoint(&vec,0,0,dist);
	BiRotLtoG(&vec,&vec,&obj->a);
	BiAddPoint(&obj->p,&obj->p,&vec);
}

