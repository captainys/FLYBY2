#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
#else /* GLX */
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glx.h>
	#define CALLBACK
#endif


#include "../impulse.h"
#include "../iutil.h"


/* Following two macros must be syncronized with iwin32og.c */
#define BIOGL_MAX_VTX_PER_POLYG 128
#define BI_GL_FONT_BITMAP_BASE 1000

static long BiPrevColor=-1;
static GLdouble BiTessVtx[BIOGL_MAX_VTX_PER_POLYG*6];  /* 6=vtx&norm */
static GLUtriangulatorObj *BiTessObj;



void BiInitOpenGL(long wid,long hei)
{
	float asp;
	GLfloat dif[]={0.8F,0.8F,0.8F,1.0F};
	GLfloat amb[]={0.4F,0.4F,0.4F,1.0F};
	GLfloat spc[]={0.9F,0.9F,0.9F,1.0F};
	GLfloat shininess[]={50.0,50.0,50.0,0.0};

    glClearColor(0.0F,0.0F,0.0F,1.0F);
    glClearDepth(1.0F);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    asp=(float)wid/(float)hei;
    gluPerspective(45.0F,asp,0.5,16000.0F);  /* Temporary Setting */

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0,0,wid,hei);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
	glLightfv(GL_LIGHT0,GL_SPECULAR,spc);
	glMaterialfv(GL_FRONT|GL_BACK,GL_SHININESS,shininess);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	/* glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
	     Doesn't work, because normal is inverted when backface is drawn!!! What's dumb!!! */
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_NORMALIZE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	BiTessObj=gluNewTess();
}

/*****************************************************************************

   Draw

*****************************************************************************/
static BIAXISF BiCurAxs;
static BIVECTOR BiCurNom;
static int BiCurPrjMode;
static BIPROJ BiCurPrj;
static int BiCurShadMode;



#define COLOR(ptr) RGB((ptr)->r,(ptr)->g,(ptr)->b)
#define BIINBOUND(a) (0<=(a)->x && (a)->x<BiWinX && 0<=(a)->y && (a)->y<BiWinY)

static void BiSetGlColor(BICOLOR *c)
{
	long cc;

	cc=(c->g<<16)+(c->r<<8)+(c->b);
	if(cc!=BiPrevColor)
	{
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
		glColor3ub((GLubyte)c->r,(GLubyte)c->g,(GLubyte)c->b);
		BiPrevColor=cc;
	}
}


enum {
	BIGL_ZBUFFER,
	BIGL_ZBUFFERNOLIGHT,
	BIGL_OVERWRITE,
	BIGL_2D
};



static int BiGlMode=BIGL_ZBUFFER;

static void BiGlEndOverWrite(void);
static void BiGlEndDraw2(void);

static void BiGlEndDrawMode(void)
{
	switch(BiGlMode)
	{
	case BIGL_OVERWRITE:
		BiGlEndOverWrite();
		break;
	case BIGL_2D:
		BiGlEndDraw2();
		break;
	}
	BiPrevColor=-1;  // 2002/05/09
}

void BiGlStartZBufferWithNoLight(void)
{
	if(BiGlMode!=BIGL_ZBUFFERNOLIGHT)
	{
		BiGlEndDrawMode();

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		BiGlMode=BIGL_ZBUFFERNOLIGHT;
	}
}

void BiGlStartZBufferWithLight(void)
{
	if(BiCurShadMode==BI_OFF)
	{
		BiGlStartZBufferWithNoLight();
	}
	else if(BiGlMode!=BIGL_ZBUFFER)
	{
		BiGlEndDrawMode();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		BiGlMode=BIGL_ZBUFFER;
	}
}

void BiGlStartOverWrite(void)
{
	if(BiGlMode!=BIGL_OVERWRITE)
	{
		BiGlEndDrawMode();

		glDepthFunc(GL_ALWAYS);
		glDepthMask(0);

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		BiGlMode=BIGL_OVERWRITE;
	}
}

void BiGlStartDraw2(void)
{
	long wid,hei;
	BiGetWindowSize(&wid,&hei);
	if(BiGlMode!=BIGL_2D)
	{
		BiGlEndDrawMode();

		glDepthFunc(GL_ALWAYS);
		glDepthMask(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,(GLdouble)wid,(GLdouble)hei,0,-1,1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		BiGlMode=BIGL_2D;
	}
}

static void BiGlEndOverWrite(void)
{
	glDepthFunc(GL_LEQUAL);
	glDepthMask(1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

static void BiGlEndDraw2(void)
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDepthFunc(GL_LEQUAL);
	glDepthMask(1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}



void BiClearScreenD(void)
{
	BiGlStartZBufferWithLight();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_LINES);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	glVertex2i(s1->x,s1->y);
	glVertex2i(s2->x,s2->y);
	glEnd();
}

void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_POINTS);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	glVertex2i(s->x  ,s->y  );
	glVertex2i(s->x+1,s->y  );
	glVertex2i(s->x+1,s->y+1);
	glVertex2i(s->x  ,s->y+1);
	glEnd();
}

void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	long ang;
	BIPOINTS t;
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_POLYGON);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	for(ang=0; ang<65536; ang+=2048)
	{
		t.x=s->x+(long)(BiCos(ang)*radx);
		t.y=s->y+(long)(BiSin(ang)*rady);
		glVertex2i(t.x,t.y);
	}
	glEnd();
}

static int BiSamePointS(BIPOINTS *s1,BIPOINTS *s2)
{
	return ((s1->x==s2->x && s1->y==s2->y) ? BI_TRUE : BI_FALSE);
}

static void BiSubPointS2(BIPOINT2 *v2,BIPOINTS *s1,BIPOINTS *s2)
{
	BiSetPoint2(v2,s1->x-s2->x,s1->y-s2->y);
}

static int BiQuickCheckConvexS(int np,BIPOINTS *p)
{
	int i;
	BIPOINTS *p1,*p2;
	BIPOINT2 v1,v2;
	real o1,o2;

	while(np>3 && BiSamePointS(&p[0],&p[1])==BI_TRUE)
	{
		p++;
		np--;
	}
	while(np>3 && BiSamePointS(&p[np-1],&p[np-2])==BI_TRUE)
	{
		np--;
	}
	if(np>3 && BiSamePointS(&p[np-1],&p[0])==BI_TRUE)
	{
		np--;
	}

	if(np<=3)
	{
		return BI_TRUE;
	}

 	p1=&p[np-2];
	p2=&p[np-1];
	BiSubPointS2(&v2,p2,p1);
	for(i=0; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(BiSamePointS(p1,p2)!=BI_TRUE)
		{
			v1=v2;
			BiSubPointS2(&v2,p2,p1);

			o1=v1.x*v2.y-v1.y*v2.x;
			if(BiAbs(o1)>YSEPS)
			{
				break;
			}
		}
	}

	for(i=i+1; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(BiSamePointS(p1,p2)!=BI_TRUE)
		{
			v1=v2;
			BiSubPointS2(&v2,p2,p1);

			o2=v1.x*v2.y-v1.y*v2.x;
			if(BiAbs(o2)>YSEPS && o1*o2<0)
			{
				return BI_FALSE;
			}
		}
	}

	return BI_TRUE;
}

void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
	int i;
	BiGlStartDraw2();
	BiSetGlColor(c);
	/* glNormal3f(0.0F,0.0F,1.0F); */

	if(BiQuickCheckConvexS(ns,s)==BI_TRUE || BiTessObj==NULL)
	{
		glBegin(GL_POLYGON);
		for(i=0; i<ns; i++)
		{
			glVertex2i(s[i].x,s[i].y);
		}
		glEnd();
	}
	else
	{
		ns=BiSmaller(BIOGL_MAX_VTX_PER_POLYG,ns);

		gluTessCallback(BiTessObj,GLU_BEGIN, glBegin);
		gluTessCallback(BiTessObj,GLU_VERTEX,glVertex2dv);
		gluTessCallback(BiTessObj,GLU_END,   glEnd);
		gluBeginPolygon(BiTessObj);
		for(i=0; i<ns; i++)
		{
			BiTessVtx[i*3  ]= (GLdouble)s[i].x;
			BiTessVtx[i*3+1]= (GLdouble)s[i].y;
			BiTessVtx[i*3+2]= 0;
			gluTessVertex(BiTessObj,&BiTessVtx[i*3],&BiTessVtx[i*3]);
		}
		gluEndPolygon(BiTessObj);
	}
}

void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BiGlStartDraw2();
	BiSetGlColor(c);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	glBegin(GL_POLYGON);
	glVertex2i(s1->x,s1->y);
	glVertex2i(s2->x,s1->y);
	glVertex2i(s2->x,s2->y);
	glVertex2i(s1->x,s2->y);
	glEnd();
}

void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
	BIPOINTS p1,p2;
	switch(mkType)
	{
	case BIMK_RECT:
		BiSetPointS(&p1, s->x-2,s->y-2);
		BiSetPointS(&p2, s->x+2,s->y+2);
		BiDrawRect2D(&p1,&p2,c);
		break;
	case BIMK_CIRCLE:
		BiDrawCircle2D(s,3,3,c);
		break;
	case BIMK_CROSS:
		BiSetPointS(&p1, s->x-4,s->y-4);
		BiSetPointS(&p2, s->x+4,s->y+4);
		BiDrawLine2D(&p1,&p2,c);
		BiSetPointS(&p1, s->x+4,s->y-4);
		BiSetPointS(&p2, s->x-4,s->y+4);
		BiDrawLine2D(&p1,&p2,c);
		break;
	}
}

void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
	int l;
	GLint listbase;

	l=strlen(str);
	BiGlStartDraw2();
	BiSetGlColor(c);

	glGetIntegerv(GL_LIST_BASE,&listbase);
	if(listbase!=BI_GL_FONT_BITMAP_BASE)
	{
		glListBase(BI_GL_FONT_BITMAP_BASE);
	}
	glRasterPos2i(s->x,s->y);
	glCallLists(l,GL_UNSIGNED_BYTE,str);
}

void BiDrawText2D(BIPOINTS *s,char *str[],int h,int v,BICOLOR *c)
{
	#define FONTX 8
	#define FONTY 16
	int i;
	long sWid,sHei,xSta,ySta;
	BIPOINTS tmp;

	sWid=0;
	sHei=0;
	for(i=0; str[i]!=NULL; i++)
	{
		sWid=BiLarger(strlen(str[i]),(unsigned long)sWid);
		sHei++;
	}

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

	tmp.x=xSta;
	tmp.y=ySta;
	for(i=0; str[i]!=NULL; i++)
	{
		BiDrawString2D(&tmp,str[i],c);
		tmp.y+=FONTY;
	}
}




void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_LINE_LOOP);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	glVertex2i(s1->x,s1->y);
	glVertex2i(s2->x,s1->y);
	glVertex2i(s2->x,s2->y);
	glVertex2i(s1->x,s2->y);
	glEnd();
}

void BiDrawEmptyPolyg2D(int n,BIPOINTS *s,BICOLOR *c)
{
	int i;
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_LINE_LOOP);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	for(i=0; i<n; i++)
	{
		glVertex2i(s[i].x,s[i].y);
	}
	glEnd();
}

void BiDrawPolyline2D(int n,BIPOINTS *s,BICOLOR *c)
{
	int i;
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_LINE_STRIP);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	for(i=0; i<n; i++)
	{
		glVertex2i(s[i].x,s[i].y);
	}
	glEnd();
}

void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
	long ang;
	BIPOINTS t;
	BiGlStartDraw2();
	BiSetGlColor(c);
	glBegin(GL_LINE_LOOP);
	/* glNormal3f(0.0F,0.0F,1.0F); */
	for(ang=0; ang<65536; ang+=2048)
	{
		t.x=s->x+(long)(BiCos(ang)*radx);
		t.y=s->y+(long)(BiSin(ang)*rady);
		glVertex2i(t.x,t.y);
	}
	glEnd();
}



/*****************************************************************************

    OpenGL Support

*****************************************************************************/
extern BIPROJ BiPrj;
extern real BiPrjFovX,BiPrjFovY;
extern real BiOrthoDist;
extern int BiPrjMode;

void BiAfterSetProjection(int pMode,BIPROJ *prj)
{
	BiCurPrjMode=pMode;
	BiCurPrj=*prj;
}

void BiAfterSetLightPosition(BIPOINT *lPos)
{
	GLfloat p[4];

	BiGlStartZBufferWithLight();

	p[0]= lPos->x;
	p[1]= lPos->y;
	p[2]=-lPos->z;
	p[3]= 1.0F;
	glLightfv(GL_LIGHT0,GL_POSITION,p);
}

void BiAfterSetProjectionMode(int pMode,BIPROJ *prj)
{
	BiGlStartZBufferWithLight();
	BiAfterSetProjection(pMode,prj);
}

void BiAfterSetOrthoDist(int pMode,BIPROJ *prj,real dist)
{
	BiGlStartZBufferWithLight();
	BiAfterSetProjection(pMode,prj);
}

void BiAfterSetShadeMode(int sMode)
{
	BiCurShadMode=sMode;
}

void BiAfterStartBuffer(BIPOSATT *eye)
{
	BiGlStartZBufferWithLight();
	glClear(GL_DEPTH_BUFFER_BIT);
}

void BiAfterChangeMatrix(BIAXISF *cnvaxs,BIAXISF *mdlaxs,BIAXISF *eyeaxs)
{
	real lft,rit,top,btm;
	long wid,hei;

	BiGlStartZBufferWithLight();

	BiGetWindowSize(&wid,&hei);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch(BiCurPrjMode)
	{
	case BIPRJ_PERS:
		lft=(real)(   -BiCurPrj.cx)*BiCurPrj.nearz/BiCurPrj.magx;
		rit=(real)(wid-BiCurPrj.cx)*BiCurPrj.nearz/BiCurPrj.magx;
		top=(real)(   BiCurPrj.cy)*BiCurPrj.nearz/BiCurPrj.magy;
		btm=(real)(BiCurPrj.cy-hei)*BiCurPrj.nearz/BiCurPrj.magy;
		glFrustum(lft,rit,btm,top,BiCurPrj.nearz,BiCurPrj.farz);
		break;
	case BIPRJ_ORTHO:
		lft=(real)(   -BiCurPrj.cx)*BiOrthoDist/BiCurPrj.magx;
		rit=(real)(wid-BiCurPrj.cx)*BiOrthoDist/BiCurPrj.magx;
		top=(real)(    BiCurPrj.cy)*BiOrthoDist/BiCurPrj.magy;
		btm=(real)(BiCurPrj.cy-   hei)*BiOrthoDist/BiCurPrj.magy;
		glOrtho(lft,rit,btm,top,BiCurPrj.nearz,BiCurPrj.farz);
		break;
	}

	// (*20000523)

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 05/23/2000 Following four lines are moved from (*20000523)
	glRotatef(-eyeaxs->a.b*45.0F/8192.0F,0.0F,0.0F,1.0F);
	glRotatef(-eyeaxs->a.p*45.0F/8192.0F,1.0F,0.0F,0.0F);
	glRotatef(-eyeaxs->a.h*45.0F/8192.0F,0.0F,1.0F,0.0F);
	glTranslated(-eyeaxs->p.x,-eyeaxs->p.y,eyeaxs->p.z);

	glTranslated(mdlaxs->p.x,mdlaxs->p.y,-mdlaxs->p.z);
	glRotatef(mdlaxs->a.h*45.0F/8192.0F,0.0F,1.0F,0.0F);
	glRotatef(mdlaxs->a.p*45.0F/8192.0F,1.0F,0.0F,0.0F);
	glRotatef(mdlaxs->a.b*45.0F/8192.0F,0.0F,0.0F,1.0F);

	BiCurAxs=*cnvaxs;

	BiSetPoint(&BiCurNom,0,0,-1);
	BiRotFastGtoL(&BiCurNom,&BiCurNom,&BiCurAxs.t);

/*
	BiGlStartZBufferWithLight();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(cnvaxs->p.x,cnvaxs->p.y,-cnvaxs->p.z);
	glRotatef((real)cnvaxs->a.h*45.0F/8192.0F,0.0F,1.0F,0.0F);
	glRotatef((real)cnvaxs->a.p*45.0F/8192.0F,1.0F,0.0F,0.0F);
	glRotatef((real)cnvaxs->a.b*45.0F/8192.0F,0.0F,0.0F,1.0F);

	BiCurAxs=*cnvaxs;

	BiSetPoint(&BiCurNom,0,0,-1);
	BiRotFastGtoL(&BiCurNom,&BiCurNom,&BiCurAxs.t);
*/
}

void BiOverWriteLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	/* glShadeModel(GL_FLAT); */
	BiGlStartOverWrite();
	BiSetGlColor(col);
	glBegin(GL_LINES);
	/* glNormal3f(BiCurNom.x,BiCurNom.y,-BiCurNom.z); */
	glVertex3d(p1->x,p1->y,-p1->z);
	glVertex3d(p2->x,p2->y,-p2->z);
	glEnd();
}

void BiOverWritePset3(BIPOINT *p,BICOLOR *col)
{
	/* glShadeModel(GL_FLAT); */
	BiGlStartOverWrite();
	BiSetGlColor(col);
	glBegin(GL_POINTS);
	/* glNormal3f(BiCurNom.x,BiCurNom.y,-BiCurNom.z); */
	glVertex3d(p->x,p->y,-p->z);
	glEnd();
}

void BiOverWritePolyg3(int np,BIPOINT *p,BICOLOR *col)
{
	int i;
	BIPOLYGON plg;

	/* glShadeModel(GL_FLAT); */
	BiGlStartOverWrite();
	BiSetGlColor(col);
	/* glNormal3f(BiCurNom.x,BiCurNom.y,-BiCurNom.z); */

	plg.np=np;
	plg.p=p;
	if(BiQuickCheckConvex(&plg)==BI_TRUE || BiTessObj==NULL)
	{
		glBegin(GL_POLYGON);
		for(i=0; i<np; i++)
		{
			glVertex3d(p[i].x,p[i].y,-p[i].z);
		}
		glEnd();
	}
	else
	{
		np=BiSmaller(BIOGL_MAX_VTX_PER_POLYG,np);

		gluTessCallback(BiTessObj,GLU_BEGIN, glBegin);
		gluTessCallback(BiTessObj,GLU_VERTEX,glVertex3dv);
		gluTessCallback(BiTessObj,GLU_END,   glEnd);
		gluBeginPolygon(BiTessObj);
		for(i=0; i<np; i++)
		{
			BiTessVtx[i*3  ]= (GLdouble)p[i].x;
			BiTessVtx[i*3+1]= (GLdouble)p[i].y;
			BiTessVtx[i*3+2]=-(GLdouble)p[i].z;
			gluTessVertex(BiTessObj,&BiTessVtx[i*3],&BiTessVtx[i*3]);
		}
		gluEndPolygon(BiTessObj);
	}
}



void BiDrawLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
	BiGlStartZBufferWithNoLight();

	BiSetGlColor(col);

	glBegin(GL_LINES);
	glVertex3d(p1->x,p1->y,-p1->z);
	glVertex3d(p2->x,p2->y,-p2->z);
	glEnd();
}

void BiDrawPset3(BIPOINT *p,BICOLOR *col)
{
	BiGlStartZBufferWithNoLight();

	BiSetGlColor(col);
	glBegin(GL_POINTS);
	glVertex3d(p->x,p->y,-p->z);
	glEnd();
}

void BiDrawCircle3(BIPOINT *p,real radx,real rady,BICOLOR *c)
{
	long ang;
	BIPOINT q;

	BiGlStartZBufferWithNoLight();

	BiSetGlColor(c);
	glBegin(GL_POLYGON);
	for(ang=0; ang<65536; ang+=2048)
	{
		q.x=p->x+BiCos(ang)*radx;
		q.y=p->y+BiSin(ang)*rady;
		glVertex3d(q.x,q.y,-p->z);
	}
	glEnd();
}

void BiDrawPolyg3(int np,BIPOINT p[],BIPOINT *n,BICOLOR *col)
{
	int i;
	BIPOLYGON plg;

	BiGlStartZBufferWithLight();

	/* glShadeModel(GL_SMOOTH); */
	BiSetGlColor(col);
	if(BiSamePoint(n,&BiOrgP)!=BI_TRUE)
	{
		glNormal3d(n->x,n->y,-n->z);
	}
	else
	{
		/* printf("Zero Normal Detected!\n"); */
	}


	/* OK. Culling is done in the higher layer. So, I need a polygon exactly shaded only
	   by specified normal. Not by flip direction. */

	glEnable(GL_CULL_FACE); /* Darkside of the polygon is automatically culled. */

	plg.p=p;
	plg.np=np;
	if(BiQuickCheckConvex(&plg)==BI_TRUE || BiTessObj==NULL)
	{
		glBegin(GL_POLYGON);
		for(i=0; i<np; i++)
		{
			glVertex3d(p[i].x,p[i].y,-p[i].z);
		}
		glEnd();


		glBegin(GL_POLYGON);
		for(i=np-1; i>=0; i--)
		{
			glVertex3d(p[i].x,p[i].y,-p[i].z);
		}
		glEnd();
	}
	else
	{
		np=BiSmaller(BIOGL_MAX_VTX_PER_POLYG,np);

		gluTessCallback(BiTessObj,GLU_BEGIN, glBegin);
		gluTessCallback(BiTessObj,GLU_VERTEX,glVertex3dv);
		gluTessCallback(BiTessObj,GLU_END,   glEnd);
		gluBeginPolygon(BiTessObj);
		for(i=0; i<np; i++)
		{
			BiTessVtx[i*3  ]= (GLdouble)p[i].x;
			BiTessVtx[i*3+1]= (GLdouble)p[i].y;
			BiTessVtx[i*3+2]=-(GLdouble)p[i].z;
			gluTessVertex(BiTessObj,&BiTessVtx[i*3],&BiTessVtx[i*3]);
		}
		gluEndPolygon(BiTessObj);


		gluTessCallback(BiTessObj,GLU_BEGIN, glBegin);
		gluTessCallback(BiTessObj,GLU_VERTEX,glVertex3dv);
		gluTessCallback(BiTessObj,GLU_END,   glEnd);
		gluBeginPolygon(BiTessObj);
		for(i=np-1; i>=0; i--)
		{
			BiTessVtx[i*3  ]= (GLdouble)p[i].x;
			BiTessVtx[i*3+1]= (GLdouble)p[i].y;
			BiTessVtx[i*3+2]=-(GLdouble)p[i].z;
			gluTessVertex(BiTessObj,&BiTessVtx[i*3],&BiTessVtx[i*3]);
		}
		gluEndPolygon(BiTessObj);
	}

	glDisable(GL_CULL_FACE);
}

static void CALLBACK BiGlVertexNormal3dv(GLdouble *v);

void BiDrawPolygGrad3(int np,BIPOINT p[],BIPOINT n[],BICOLOR *col)
{
	int i;
	BIPOLYGON plg;
	plg.p=p;
	plg.np=np;

	BiGlStartZBufferWithLight();

	/* glShadeModel(GL_SMOOTH); */
	BiSetGlColor(col);

	glEnable(GL_CULL_FACE);  /* Same strategy as BiDrawPolyg3 */

	if(BiQuickCheckConvex(&plg)==BI_TRUE || BiTessObj==NULL)
	{
		glBegin(GL_POLYGON);
		for(i=0; i<np; i++)
		{
			if(BiSamePoint(&n[i],&BiOrgP)!=BI_TRUE)
			{
				glNormal3d(n[i].x,n[i].y,-n[i].z);
			}
			else
			{
				/* printf("Zero Normal is Detected!\n"); */
			}
			glVertex3d(p[i].x,p[i].y,-p[i].z);
		}
		glEnd();

		glBegin(GL_POLYGON);
		for(i=np-1; i>=0; i--)
		{
			if(BiSamePoint(&n[i],&BiOrgP)!=BI_TRUE)
			{
				glNormal3d(n[i].x,n[i].y,-n[i].z);
			}
			else
			{
				/* printf("Zero Normal is Detected!\n"); */
			}
			glVertex3d(p[i].x,p[i].y,-p[i].z);
		}
		glEnd();
	}
	else
	{
		np=BiSmaller(BIOGL_MAX_VTX_PER_POLYG,np);

		gluTessCallback(BiTessObj,GLU_BEGIN, glBegin);
		gluTessCallback(BiTessObj,GLU_VERTEX,BiGlVertexNormal3dv);
		gluTessCallback(BiTessObj,GLU_END,   glEnd);
		gluBeginPolygon(BiTessObj);
		for(i=0; i<np; i++)
		{
			BiTessVtx[i*6  ]= (GLdouble)p[i].x;
			BiTessVtx[i*6+1]= (GLdouble)p[i].y;
			BiTessVtx[i*6+2]=-(GLdouble)p[i].z;
			BiTessVtx[i*6+3]= (GLdouble)n[i].x;
			BiTessVtx[i*6+4]= (GLdouble)n[i].y;
			BiTessVtx[i*6+5]=-(GLdouble)n[i].z;
			gluTessVertex(BiTessObj,&BiTessVtx[i*6],&BiTessVtx[i*6]);
		}
		gluEndPolygon(BiTessObj);


		gluTessCallback(BiTessObj,GLU_BEGIN, glBegin);
		gluTessCallback(BiTessObj,GLU_VERTEX,BiGlVertexNormal3dv);
		gluTessCallback(BiTessObj,GLU_END,   glEnd);
		gluBeginPolygon(BiTessObj);
		for(i=np-1; i>=0; i--)
		{
			BiTessVtx[i*6  ]= (GLdouble)p[i].x;
			BiTessVtx[i*6+1]= (GLdouble)p[i].y;
			BiTessVtx[i*6+2]=-(GLdouble)p[i].z;
			BiTessVtx[i*6+3]= (GLdouble)n[i].x;
			BiTessVtx[i*6+4]= (GLdouble)n[i].y;
			BiTessVtx[i*6+5]=-(GLdouble)n[i].z;
			gluTessVertex(BiTessObj,&BiTessVtx[i*6],&BiTessVtx[i*6]);
		}
		gluEndPolygon(BiTessObj);
	}

	glDisable(GL_CULL_FACE);
}

static void CALLBACK BiGlVertexNormal3dv(GLdouble *v)
{
	BiGlStartZBufferWithLight();

	if(BiAbs(v[3])+BiAbs(v[4])+BiAbs(v[5])>=0.05F)
	{
		glNormal3dv(v+3);
	}
	else
	{
		/* printf("Zero Normal is Detected!\n"); */
	}
	glVertex3dv(v);
}

static void BiXText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col)
{
	int i;
	GLint listbase;
	real siz;
	BIVECTOR tmp,dwn,loc;

	BiConvLtoG(&loc,p,&BiCurAxs);

	siz=20.0F*(BiPrjMode==BIPRJ_PERS ? loc.z : BiOrthoDist)/BiPrj.magy;
	BiSetPoint(&dwn,0,-siz,0);
	BiRotFastGtoL(&dwn,&dwn,&BiCurAxs.t);

	BiSetGlColor(col);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glGetIntegerv(GL_LIST_BASE,&listbase);
	if(listbase!=BI_GL_FONT_BITMAP_BASE)
	{
		glListBase(BI_GL_FONT_BITMAP_BASE);
	}
	tmp=*p;
	for(i=0; tx[i]!=NULL; i++)
	{
		glRasterPos3d(tmp.x,tmp.y,-tmp.z);
		glCallLists(strlen(tx[i]),GL_UNSIGNED_BYTE,tx[i]);
		BiAddPoint(&tmp,&tmp,&dwn);
	}

	glPopMatrix();
	glPopAttrib();
}

void BiDrawText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col)
{
	BiGlStartZBufferWithNoLight();
	BiXText3(p,tx,hori,vert,col);
}

void BiOverWriteText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col)
{
	BiGlStartOverWrite();
	BiXText3(p,tx,hori,vert,col);
}

static void BiXMarker3(BIPOINT *p,BICOLOR *col,int mkType)
{
	real siz;
	BIPOINT loc,r[8];

	BiConvLtoG(&loc,p,&BiCurAxs);

	siz=3.0F*(BiPrjMode==BIPRJ_PERS ? loc.z : BiOrthoDist)/BiPrj.magx;

	BiSetGlColor(col);

	if(mkType==BIMK_RECT || mkType==BIMK_CROSS)
	{
		BiSetPoint(&r[0],-siz,-siz,0.0F);
		BiSetPoint(&r[1], siz,-siz,0.0F);
		BiSetPoint(&r[2], siz, siz,0.0F);
		BiSetPoint(&r[3],-siz, siz,0.0F);

		BiRotFastGtoL(&r[0],&r[0],&BiCurAxs.t);
		BiRotFastGtoL(&r[1],&r[1],&BiCurAxs.t);
		BiRotFastGtoL(&r[2],&r[2],&BiCurAxs.t);
		BiRotFastGtoL(&r[3],&r[3],&BiCurAxs.t);

		BiAddPoint(&r[0],&r[0],p);
		BiAddPoint(&r[1],&r[1],p);
		BiAddPoint(&r[2],&r[2],p);
		BiAddPoint(&r[3],&r[3],p);

		switch(mkType)
		{
		case BIMK_RECT:
			glBegin(GL_POLYGON);
			glVertex3d(r[0].x,r[0].y,-r[0].z);
			glVertex3d(r[1].x,r[1].y,-r[1].z);
			glVertex3d(r[2].x,r[2].y,-r[2].z);
			glVertex3d(r[3].x,r[3].y,-r[3].z);
			glEnd();
			break;
		case BIMK_CROSS:
			glBegin(GL_LINES);
			glVertex3d(r[0].x,r[0].y,-r[0].z);
			glVertex3d(r[2].x,r[2].y,-r[2].z);
			glEnd();
			glBegin(GL_LINES);
			glVertex3d(r[1].x,r[1].y,-r[1].z);
			glVertex3d(r[3].x,r[3].y,-r[3].z);
			glEnd();
			break;
		}
	}
	else if(mkType==BIMK_CIRCLE)
	{
		BiSetPoint(&r[0],     0.0F,-siz     ,0.0F);
		BiSetPoint(&r[1], siz*0.6F,-siz*0.6F,0.0F);
		BiSetPoint(&r[2], siz     ,     0.0F,0.0F);
		BiSetPoint(&r[3], siz*0.6F, siz*0.6F,0.0F);
		BiSetPoint(&r[4],     0.0F, siz     ,0.0F);
		BiSetPoint(&r[5],-siz*0.6F, siz*0.6F,0.0F);
		BiSetPoint(&r[6],-siz     ,     0.0F,0.0F);
		BiSetPoint(&r[7],-siz*0.6F,-siz*0.6F,0.0F);

		BiRotFastGtoL(&r[0],&r[0],&BiCurAxs.t);
		BiRotFastGtoL(&r[1],&r[1],&BiCurAxs.t);
		BiRotFastGtoL(&r[2],&r[2],&BiCurAxs.t);
		BiRotFastGtoL(&r[3],&r[3],&BiCurAxs.t);
		BiRotFastGtoL(&r[4],&r[4],&BiCurAxs.t);
		BiRotFastGtoL(&r[5],&r[5],&BiCurAxs.t);
		BiRotFastGtoL(&r[6],&r[6],&BiCurAxs.t);
		BiRotFastGtoL(&r[7],&r[7],&BiCurAxs.t);

		BiAddPoint(&r[0],&r[0],p);
		BiAddPoint(&r[1],&r[1],p);
		BiAddPoint(&r[2],&r[2],p);
		BiAddPoint(&r[3],&r[3],p);
		BiAddPoint(&r[4],&r[4],p);
		BiAddPoint(&r[5],&r[5],p);
		BiAddPoint(&r[6],&r[6],p);
		BiAddPoint(&r[7],&r[7],p);

		glBegin(GL_POLYGON);
		glVertex3d(r[0].x,r[0].y,-r[0].z);
		glVertex3d(r[1].x,r[1].y,-r[1].z);
		glVertex3d(r[2].x,r[2].y,-r[2].z);
		glVertex3d(r[3].x,r[3].y,-r[3].z);
		glVertex3d(r[4].x,r[4].y,-r[4].z);
		glVertex3d(r[5].x,r[5].y,-r[5].z);
		glVertex3d(r[6].x,r[6].y,-r[6].z);
		glVertex3d(r[7].x,r[7].y,-r[7].z);
		glEnd();
	}
}

void BiDrawMarker3(BIPOINT *p,BICOLOR *col,int mkType)
{
	BiGlStartZBufferWithNoLight();
	BiXMarker3(p,col,mkType);
}

void BiOverWriteMarker3(BIPOINT *p,BICOLOR *col,int mkType)
{
	BiGlStartOverWrite();
	BiXMarker3(p,col,mkType);
}




