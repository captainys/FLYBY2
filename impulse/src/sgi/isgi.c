#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gl/gl.h>
#include <gl/device.h>

#include "../impulse.h"


/*****************************************************************************

  Known Bugs
    たまにDiffuseの計算がオーバーフローするのか、極端に光が強く見えるこ
    とがある。ただし、色合いは変わっていない。ちょうどライトがオフにな
    って、RGBの元の色が出ているように見えるが、陰がついている部分もある。
  テスト予定
    nnormalize
     -> BiQuickNormalizeの問題?
    ライトを常にオフにしてみる
     -> オーバーフローのせいだったらこれで速くなるかも

*****************************************************************************/


static void BiInitGlMaterial(void);
static void BiGlBindMaterial(BICOLOR *col);


/*****************************************************************************

   Setting

*****************************************************************************/
static int BiFatalFlag=BI_OFF;
int BiFatal(void)
{
    return BiFatalFlag;
}



static int BiWinOpened=BI_OFF;
static int BiPreviousBindMaterial=-1;
static int BiPreviousGlMode=-1;

extern int BiShadMode;
extern int BiPrjMode;
extern BIPROJ BiPrj;
extern real BiPrjFovX,BiPrjFovY,BiOrthoDist;




extern char BiWindowName[];

void BiOpenWindow(long reqX,long reqY)
{
    long xsize,ysize;

    static Matrix Identity = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

    static float lm[] =
    {
        AMBIENT, 0.4, 0.4, 0.4,
        LOCALVIEWER, 0,
        LMNULL
    };
    static float lt[] =
    {
        LCOLOR, 1.0, 1.0, 1.0,
        POSITION, 0.7, 0.7, 0.7, 0.0,
        LMNULL
    };

    prefsize(reqX,reqY);
    foreground();
    winopen(BiWindowName);
    winconstraints();

    keepaspect(1,1);
    winconstraints();

    BiWinOpened=BI_ON;

    getsize(&xsize,&ysize);

    RGBmode();
    doublebuffer();
    /* ## concave(TRUE); */
    gconfig();

    /* A Little Bit Faster? */
    lsetdepth(getgdesc(GD_ZMIN),getgdesc(GD_ZMAX));

    zbuffer(TRUE);
    mmode(MVIEWING);
    loadmatrix(Identity);

    lmdef(DEFLIGHT,1,0,lt);
    lmdef(DEFLMODEL,1,0,lm);
    lmbind(LMODEL,1);
    lmbind(LIGHT0,1);

    BiInitGlMaterial();
    BiPreviousBindMaterial=-1;

    viewport(0,xsize-1,0,ysize-1);
    zfunction(ZF_LEQUAL);

    czclear(0,getgdesc(GD_ZMAX));
}

void BiCloseWindow(void)
{
    /* Nothing To Do */
}

void BiGetWindowSize(long *wid,long *hei)
{
    long xsize,ysize;
    getsize(&xsize,&ysize);
    *wid=(long)xsize;
    *hei=(long)ysize;
}

void BiGetStdProjection(BIPROJ *prj)
{
    long wid,hei;
    BiGetWindowSize(&wid,&hei);
    prj->lx=wid;
    prj->ly=hei;
    prj->cx=wid/2;
    prj->cy=hei/2;
    prj->magx=(real)wid/1.41421356;
    prj->magy=prj->magx;
    prj->nearz=0.5;
    prj->farz=10000.0;
}

void BiSwapBuffers(void)
{
    swapbuffers();
    BiPreviousBindMaterial=-1;
}





/*****************************************************************************

   File Name

*****************************************************************************/
void BiConstrainFileName(char neo[],char org[])
{
    int i;

    if(neo!=org)
    {
        strcpy(neo,org);
    }

    for(i=0; neo[i]!=0; i++)
    {
        if(neo[i]==':' || neo[i]=='\\')
        {
            neo[i]='/';
        }
    }
}




/*****************************************************************************

   Device

*****************************************************************************/

static Device BiKeyMap[512];
static int BiDevToKcode[512];
static int BiDevToAsc[512];

#define NULLKEY 0

static void BiDeviceMatchKey(int kcode,Device dev,char asc)
{
    BiKeyMap[kcode]=dev;
    BiDevToKcode[dev]=kcode;
    BiDevToAsc[dev]=asc;
    qdevice(dev);
}

void BiDeviceInitialize(void)
{
    BiDeviceMatchKey(BIKEY_NULL    ,NULLKEY      ,0x00);
    BiDeviceMatchKey(BIKEY_STOP    ,BREAKKEY     ,0x00);
    BiDeviceMatchKey(BIKEY_COPY    ,NULLKEY      ,0x00);
    BiDeviceMatchKey(BIKEY_F1      ,F1KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F2      ,F2KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F3      ,F3KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F4      ,F4KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F5      ,F5KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F6      ,F6KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F7      ,F7KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F8      ,F8KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F9      ,F9KEY        ,0x00);
    BiDeviceMatchKey(BIKEY_F10     ,F10KEY       ,0x00);
    BiDeviceMatchKey(BIKEY_ONE     ,ONEKEY       ,'1');
    BiDeviceMatchKey(BIKEY_TWO     ,TWOKEY       ,'2');
    BiDeviceMatchKey(BIKEY_THREE   ,THREEKEY     ,'3');
    BiDeviceMatchKey(BIKEY_FOUR    ,FOURKEY      ,'4');
    BiDeviceMatchKey(BIKEY_FIVE    ,FIVEKEY      ,'5');
    BiDeviceMatchKey(BIKEY_SIX     ,SIXKEY       ,'6');
    BiDeviceMatchKey(BIKEY_SEVEN   ,SEVENKEY     ,'7');
    BiDeviceMatchKey(BIKEY_EIGHT   ,EIGHTKEY     ,'8');
    BiDeviceMatchKey(BIKEY_NINE    ,NINEKEY      ,'9');
    BiDeviceMatchKey(BIKEY_ZERO    ,ZEROKEY      ,'0');
    BiDeviceMatchKey(BIKEY_A       ,AKEY         ,'A');
    BiDeviceMatchKey(BIKEY_B       ,BKEY         ,'B');
    BiDeviceMatchKey(BIKEY_C       ,CKEY         ,'C');
    BiDeviceMatchKey(BIKEY_D       ,DKEY         ,'D');
    BiDeviceMatchKey(BIKEY_E       ,EKEY         ,'E');
    BiDeviceMatchKey(BIKEY_F       ,FKEY         ,'F');
    BiDeviceMatchKey(BIKEY_G       ,GKEY         ,'G');
    BiDeviceMatchKey(BIKEY_H       ,HKEY         ,'H');
    BiDeviceMatchKey(BIKEY_I       ,IKEY         ,'I');
    BiDeviceMatchKey(BIKEY_J       ,JKEY         ,'J');
    BiDeviceMatchKey(BIKEY_K       ,KKEY         ,'K');
    BiDeviceMatchKey(BIKEY_L       ,LKEY         ,'L');
    BiDeviceMatchKey(BIKEY_M       ,MKEY         ,'M');
    BiDeviceMatchKey(BIKEY_N       ,NKEY         ,'N');
    BiDeviceMatchKey(BIKEY_O       ,OKEY         ,'O');
    BiDeviceMatchKey(BIKEY_P       ,PKEY         ,'P');
    BiDeviceMatchKey(BIKEY_Q       ,QKEY         ,'Q');
    BiDeviceMatchKey(BIKEY_R       ,RKEY         ,'R');
    BiDeviceMatchKey(BIKEY_S       ,SKEY         ,'S');
    BiDeviceMatchKey(BIKEY_T       ,TKEY         ,'T');
    BiDeviceMatchKey(BIKEY_U       ,UKEY         ,'U');
    BiDeviceMatchKey(BIKEY_V       ,VKEY         ,'V');
    BiDeviceMatchKey(BIKEY_W       ,WKEY         ,'W');
    BiDeviceMatchKey(BIKEY_X       ,XKEY         ,'X');
    BiDeviceMatchKey(BIKEY_Y       ,YKEY         ,'Y');
    BiDeviceMatchKey(BIKEY_Z       ,ZKEY         ,'Z');
    BiDeviceMatchKey(BIKEY_SPACE   ,SPACEKEY     ,' ');
    BiDeviceMatchKey(BIKEY_ESC     ,ESCKEY       ,0x1b);
    BiDeviceMatchKey(BIKEY_TAB     ,TABKEY       ,'\t');
    BiDeviceMatchKey(BIKEY_CTRL    ,CTRLKEY      ,0x00);
    BiDeviceMatchKey(BIKEY_SHIFT   ,LEFTSHIFTKEY ,0x00);
    BiDeviceMatchKey(BIKEY_ALT     ,LEFTALTKEY   ,0x00);
    BiDeviceMatchKey(BIKEY_BS      ,BACKSPACEKEY ,0x00);
    BiDeviceMatchKey(BIKEY_RET     ,RETKEY       ,0x0d);
    BiDeviceMatchKey(BIKEY_HOME    ,HOMEKEY      ,0x00);
    BiDeviceMatchKey(BIKEY_DEL     ,DELKEY       ,0x00);
    BiDeviceMatchKey(BIKEY_UP      ,UPARROWKEY   ,0x1e);
    BiDeviceMatchKey(BIKEY_DOWN    ,DOWNARROWKEY ,0x1f);
    BiDeviceMatchKey(BIKEY_LEFT    ,LEFTARROWKEY ,0x1d);
    BiDeviceMatchKey(BIKEY_RIGHT   ,RIGHTARROWKEY,0x1c);
    BiDeviceMatchKey(BIKEY_TEN0    ,NULLKEY      ,'0');
    BiDeviceMatchKey(BIKEY_TEN1    ,NULLKEY      ,'1');
    BiDeviceMatchKey(BIKEY_TEN2    ,NULLKEY      ,'2');
    BiDeviceMatchKey(BIKEY_TEN3    ,NULLKEY      ,'3');
    BiDeviceMatchKey(BIKEY_TEN4    ,NULLKEY      ,'4');
    BiDeviceMatchKey(BIKEY_TEN5    ,NULLKEY      ,'5');
    BiDeviceMatchKey(BIKEY_TEN6    ,NULLKEY      ,'6');
    BiDeviceMatchKey(BIKEY_TEN7    ,NULLKEY      ,'7');
    BiDeviceMatchKey(BIKEY_TEN8    ,NULLKEY      ,'8');
    BiDeviceMatchKey(BIKEY_TEN9    ,NULLKEY      ,'9');
    BiDeviceMatchKey(BIKEY_TENMUL  ,NULLKEY      ,'*');
    BiDeviceMatchKey(BIKEY_TENDIV  ,NULLKEY      ,'/');
    BiDeviceMatchKey(BIKEY_TENPLUS ,NULLKEY      ,'+');
    BiDeviceMatchKey(BIKEY_TENMINUS,NULLKEY      ,'-');
    BiDeviceMatchKey(BIKEY_TENEQUAL,EQUALKEY     ,'=');
    BiDeviceMatchKey(BIKEY_TENDOT  ,PERIODKEY    ,'.');
}

void BiUpdateDevice(void)
{
    /* Nothing To Do */
}

void BiMouse(int *lbt,int *mbt,int *rbt,long *mx,long *my)
{
        long xorg,yorg,xsize,ysize;
    getorigin(&xorg,&yorg);
    getsize(&xsize,&ysize);
    *mx=getvaluator(MOUSEX)-xorg;
    *my=ysize-(getvaluator(MOUSEY)-yorg);

    *lbt=(getvaluator(LEFTMOUSE)==TRUE ? BI_ON : BI_OFF);
    *mbt=(getvaluator(MIDDLEMOUSE)==TRUE ? BI_ON : BI_OFF);
    *rbt=(getvaluator(RIGHTMOUSE)==TRUE ? BI_ON : BI_OFF);
}

int BiKey(int kcode)
{
    return (getvaluator(BiKeyMap[kcode])==TRUE ? BI_ON : BI_OFF);
}

int BiInkey(void)
{
    short val;
    Device dev;
    while(qtest()!=FALSE)
    {
        dev=qread(&val);

        if(val==TRUE && 0<=dev && dev<256 && BiDevToKcode[dev]!=0)
        {
            return BiDevToKcode[dev];
        }
    }
    return 0;
}




/*****************************************************************************

   Memory Management

*****************************************************************************/

void BiMemoryInitialize(void)
{
    /* Nothing To Do */
}

void *BiMalloc(size_t uni)
{
    return (void *)malloc(uni);
}

void BiFree(void *ptr)
{
    free(ptr);
}





/*****************************************************************************

   Draw

*****************************************************************************/


#define BiGlV3f(a) {real v[3]; v[0]=(a)->x; v[1]=(a)->y; v[2]=-(a)->z; v3f(v);}
#define BiGlN3f(a) {real n[3]; n[0]=(a)->x; n[1]=(a)->y; n[2]=-(a)->z; n3f(n);}


static BIAXIS BiCurAxs;
static int BiCurPrjMode;
static BIPROJ BiCurPrj;
static int BiCurShadMode=BI_ON;

enum {
    BIGL_ZBUFFER,
    BIGL_ZBUFFERNOLIGHT,
    BIGL_OVERWRITE,
    BIGL_2D
};

static Matrix Identity = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
static Matrix ProjectionStack;

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
}

static void BiGlStartZBufferWithLight(void)
{
    if(BiGlMode!=BIGL_ZBUFFER)
    {
        BiGlEndDrawMode();

        switch(BiCurShadMode)
        {
        case BI_ON:
            lmbind(LMODEL,1);
            lmbind(LIGHT0,1);
            break;
        case BI_OFF:
            lmbind(LMODEL,0);
            lmbind(LIGHT0,0);
            break;
        }

        BiGlMode=BIGL_ZBUFFER;
    }
}

static void BiGlStartZBufferWithNoLight(void)
{
    if(BiGlMode!=BIGL_ZBUFFERNOLIGHT)
    {
        BiGlEndDrawMode();

        lmbind(LMODEL,0);
        lmbind(LIGHT0,0);

        BiGlMode=BIGL_ZBUFFERNOLIGHT;
    }
}

static void BiGlStartOverWrite(void)
{
    if(BiGlMode!=BIGL_OVERWRITE)
    {
        BiGlEndDrawMode();

        zwritemask(0);
        zfunction(ZF_ALWAYS);

        lmbind(LMODEL,0);
        lmbind(LIGHT0,0);

        BiGlMode=BIGL_OVERWRITE;
    }
}

static void BiGlStartDraw2(void)
{
    if(BiGlMode!=BIGL_2D)
    {
        static Matrix Identity = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
        long xsize,ysize;

        BiGlEndDrawMode();

        zwritemask(0);
        zfunction(ZF_ALWAYS);

        mmode(MPROJECTION);
        getmatrix(ProjectionStack);
        getsize(&xsize,&ysize);
        loadmatrix(Identity);
        ortho2(0.0,(float)xsize,(float)ysize,0.0);

        mmode(MVIEWING);
        pushmatrix();
        loadmatrix(Identity);

        BiGlMode=BIGL_2D;

        lmbind(LMODEL,0);
        lmbind(LIGHT0,0);
    }
}

static void BiGlEndOverWrite(void)
{
    zfunction(ZF_LEQUAL);
    zwritemask((unsigned long)0xFFFFFFFF);

    lmbind(LMODEL,1);
    lmbind(LIGHT0,1);
}

static void BiGlEndDraw2(void)
{
    mmode(MVIEWING);
    popmatrix();

    mmode(MPROJECTION);
    loadmatrix(ProjectionStack);

    zfunction(ZF_LEQUAL);
    zwritemask((unsigned long)0xFFFFFFFF);

    lmbind(LMODEL,1);
    lmbind(LIGHT0,1);
}





void BiClearScreenD(void)
{
    czclear(0,getgdesc(GD_ZMAX));
    BiPreviousBindMaterial=-1;
}

void BiDrawLine2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
    float v[2];

    BiGlBindMaterial(c);
    BiGlStartDraw2();
    bgnline();
    v[0]= (float)s1->x;
    v[1]= (float)s1->y;
    v2f(v);
    v[0]= (float)s2->x;
    v[1]= (float)s2->y;
    v2f(v);
    endline();
}

void BiDrawPset2D(BIPOINTS *s,BICOLOR *c)
{
    float v[2];
    BiGlBindMaterial(c);
    BiGlStartDraw2();
    bgnline();  /* not bgnpoint() in order to enable linewidth */
    v[0]= (float)s->x;
    v[1]= (float)s->y;
    v2f(v);
    v2f(v);
    endline();
}

void BiDrawCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *c)
{
    float v[2];
    long ang;

    BiGlBindMaterial(c);
    BiGlStartDraw2();

    bgnpolygon();
    for(ang=0; ang<65536; ang+=2048)
    {
        v[0]=s->x+(long)(BiCos(ang)*radx);
        v[1]=s->y+(long)(BiSin(ang)*rady);
        v2f(v);
    }
    endpolygon();
}

void BiDrawPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
    int i;
    float v[2];

    BiGlStartDraw2();
    BiGlBindMaterial(c);
    bgnpolygon();

    for(i=0; i<ns; i++)
    {
        v[0]=(float)s[i].x;
        v[1]=(float)s[i].y;
        v2f(v);
    }
    endpolygon();
}

void BiDrawRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
    int i;
    float v[2];

    BiGlStartDraw2();
    BiGlBindMaterial(c);
    bgnpolygon();

    v[0]= (float)s1->x;
    v[1]= (float)s1->y;
    v2f(v);
    v[0]= (float)s2->x;
    v[1]= (float)s1->y;
    v2f(v);
    v[0]= (float)s2->x;
    v[1]= (float)s2->y;
    v2f(v);
    v[0]= (float)s1->x;
    v[1]= (float)s2->y;
    v2f(v);

    endpolygon();
}

void BiDrawMarker2D(BIPOINTS *s,int mkType,BICOLOR *c)
{
    #define BIMKRAD 3
    BIPOINTS p1,p2;
    switch(mkType)
    {
    case BIMK_RECT:
        BiSetPoint2(&p1, s->x-BIMKRAD,s->y-BIMKRAD);
        BiSetPoint2(&p2, s->x+BIMKRAD,s->y+BIMKRAD);
        BiDrawRect2D(&p1,&p2,c);
        break;
    case BIMK_CIRCLE:
        BiDrawCircle2D(s,BIMKRAD,BIMKRAD,c);
        break;
    case BIMK_CROSS:
        BiSetPoint2(&p1, s->x-BIMKRAD,s->y-BIMKRAD);
        BiSetPoint2(&p2, s->x+BIMKRAD,s->y+BIMKRAD);
        BiDrawLine2D(&p1,&p2,c);
        BiSetPoint2(&p1, s->x+BIMKRAD,s->y-BIMKRAD);
        BiSetPoint2(&p2, s->x-BIMKRAD,s->y+BIMKRAD);
        BiDrawLine2D(&p1,&p2,c);
        break;
    }
}

void BiDrawString2D(BIPOINTS *s,char *str,BICOLOR *c)
{
    BiGlStartDraw2();
    BiGlBindMaterial(c);
    cmov2i((Icoord)s->x,(Icoord)s->y);
    charstr(str);
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
        sWid=BiLarger(strlen(str[i]),sWid);
        sHei++;
    }

    switch(h)
    {
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

/*****************************************************************************/

void BiDrawEmptyRect2D(BIPOINTS *s1,BIPOINTS *s2,BICOLOR *c)
{
    float v[2];

    BiGlBindMaterial(c);
    BiGlStartDraw2();
    bgnline();

    v[0]= (float)s1->x;
    v[1]= (float)s1->y;
    v2f(v);
    v[0]= (float)s2->x;
    v[1]= (float)s1->y;
    v2f(v);
    v[0]= (float)s2->x;
    v[1]= (float)s2->y;
    v2f(v);
    v[0]= (float)s1->x;
    v[1]= (float)s2->y;
    v2f(v);
    endline();
}

void BiDrawEmptyPolyg2D(int ns,BIPOINTS *s,BICOLOR *c)
{
    int i;
    float v[2];

    BiGlBindMaterial(c);
    BiGlStartDraw2();
    bgnline();
    for(i=0; i<ns-1; i++)
    {
        v[0]= (float)s[i].x;
        v[1]= (float)s[i].y;
        v2f(v);
    }
    v[0]= (float)s[0].x;
    v[1]= (float)s[0].y;
    v2f(v);
    endline();
}

void BiDrawPolyline2D(int ns,BIPOINTS *s,BICOLOR *c)
{
    int i;
    float v[2];

    BiGlBindMaterial(c);
    BiGlStartDraw2();
    bgnline();
    for(i=0; i<ns-1; i++)
    {
        v[0]= (float)s[i].x;
        v[1]= (float)s[i].y;
        v2f(v);
    }
    endline();
}

void BiDrawEmptyCircle2D(BIPOINTS *s,long radx,long rady,BICOLOR *col)
{
    int i;
    BIPOINTS v1,v2,p1,p2;

    BiSetPointS(&v2,radx,0);
    for(i=1; i<=8; i++)
    {
        v1=v2;

        v2.x=(long)((real)radx*BiCos(i*4096));
        v2.y=(long)((real)rady*BiSin(i*4096));

        BiSetPointS(&p1,s->x+v1.x,s->y+v1.y);
        BiSetPointS(&p2,s->x+v2.x,s->y+v2.y);
        BiDrawLine2D(&p1,&p2,col);

        BiSetPointS(&p1,s->x-v1.x,s->y+v1.y);
        BiSetPointS(&p2,s->x-v2.x,s->y+v2.y);
        BiDrawLine2D(&p1,&p2,col);

        BiSetPointS(&p1,s->x+v1.x,s->y-v1.y);
        BiSetPointS(&p2,s->x+v2.x,s->y-v2.y);
        BiDrawLine2D(&p1,&p2,col);

        BiSetPointS(&p1,s->x-v1.x,s->y-v1.y);
        BiSetPointS(&p2,s->x-v2.x,s->y-v2.y);
        BiDrawLine2D(&p1,&p2,col);
    }
}

void BiGetColor(BICOLOR *col,BIPOINTS *s)
{
    RGBvalue r,g,b;
    cmov2i((Icoord)s->x,(Icoord)s->y);
    readRGB(1,&r,&g,&b);
    col->r=(int)r;
    col->g=(int)g;
    col->b=(int)b;
}

void BiGetColorArray(BICOLOR *col,BIPOINTS *s1,BIPOINTS *s2)
{
    int x,y,x1,y1,x2,y2;
    BIPOINTS s;
    x1=BiSmaller(s1->x,s2->x);
    y1=BiSmaller(s1->y,s2->y);
    x2=BiLarger(s1->x,s2->x);
    y2=BiLarger(s1->y,s2->y);
    for(y=y1; y<=y2; y++)
    {
        for(x=x1; x<=x2; x++)
        {
            BiSetPointS(&s,x,y);
            BiGetColor(col,&s);
            col++;
        }
    }
}

/*****************************************************************************/


void BiAfterSetProjection(int pMode,BIPROJ *prj)
{
    BiCurPrjMode=pMode;
    BiCurPrj=*prj;
}

void BiAfterStartBuffer(BIPOSATT *eye)
{
    BiGlStartZBufferWithLight();
    zclear();
}

void BiAfterChangeMatrix(BIAXISF *cnvaxs,BIAXISF *mdlaxs,BIAXISF *eyeaxs)
{
    long xsize,ysize;
    real lft,rit,top,btm;

    BiGlStartZBufferWithLight();

    mmode(MPROJECTION);
    loadmatrix(Identity);
    getsize(&xsize,&ysize);
    viewport(0,xsize-1,0,ysize-1);
    switch(BiCurPrjMode)
    {
    case BIPRJ_PERS:
        lft=(real)(     -BiCurPrj.cx)*BiCurPrj.nearz/BiCurPrj.magx;
        rit=(real)(xsize-BiCurPrj.cx)*BiCurPrj.nearz/BiCurPrj.magx;
        top=(real)(      BiCurPrj.cy)*BiCurPrj.nearz/BiCurPrj.magy;
        btm=(real)(BiCurPrj.cy-ysize)*BiCurPrj.nearz/BiCurPrj.magy;
        window(lft,rit,btm,top,BiCurPrj.nearz,BiCurPrj.farz);
        break;
    case BIPRJ_ORTHO:
        lft=(real)(     -BiCurPrj.cx)*BiOrthoDist/BiCurPrj.magx;
        rit=(real)(xsize-BiCurPrj.cx)*BiOrthoDist/BiCurPrj.magx;
        top=(real)(      BiCurPrj.cy)*BiOrthoDist/BiCurPrj.magy;
        btm=(real)(BiCurPrj.cy-ysize)*BiOrthoDist/BiCurPrj.magy;
        ortho(lft,rit,btm,top,BiCurPrj.nearz,BiCurPrj.farz);
        break;
    }
    rot(-eyeaxs->a.b*45.0F/8192.0F,'z');
    rot(-eyeaxs->a.p*45.0F/8192.0F,'x');
    rot(-eyeaxs->a.h*45.0F/8192.0F,'y');
    translate(-eyeaxs->p.x,-eyeaxs->p.y,eyeaxs->p.z);

    mmode(MVIEWING);
    loadmatrix(Identity);
    translate(mdlaxs->p.x,mdlaxs->p.y,-mdlaxs->p.z);
    rot(mdlaxs->a.h*45.0F/8192.0F,'y');
    rot(mdlaxs->a.p*45.0F/8192.0F,'x');
    rot(mdlaxs->a.b*45.0F/8192.0F,'z');

    BiCurAxs=*cnvaxs;
}

void BiAfterSetLightPosition(BIPOINT *lPos)
{
    static float lt[] =
    {
        LCOLOR, 1, 1, 1,
        POSITION, 0.0, 0.0, 0.0, 1.0,
        LMNULL
    };
    BiGlStartZBufferWithLight();

/* ##
    lt[5]= lPos->x;
    lt[6]= lPos->y;
    lt[7]=-lPos->z;
    lt[8]= 1.0F;

    mmode(MVIEWING);
    pushmatrix();
    loadmatrix(Identity);
    lmdef(DEFLIGHT,1,0,lt);
    lmbind(LIGHT0,1);
    popmatrix();
*/

/* */
    lt[5]= 0.0F;
    lt[6]=-1.0F;
    lt[7]= 0.0F;
    lt[8]= 0.0F;
    lmdef(DEFLIGHT,1,0,lt);
    lmbind(LIGHT0,1);
/* */
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

void BiOverWriteLine3(BIPOINT *p1,BIPOINT *p2,BICOLOR *col)
{
    float v[3];

    BiGlStartOverWrite();
    BiGlBindMaterial(col);

    bgnline();
    v[0]= (float)p1->x;
    v[1]= (float)p1->y;
    v[2]=-(float)p1->z;
    v3f(v);
    v[0]= (float)p2->x;
    v[1]= (float)p2->y;
    v[2]=-(float)p2->z;
    v3f(v);
    endline();
}

void BiOverWritePset3(BIPOINT *p,BICOLOR *col)
{
    float v[3];

    BiGlStartOverWrite();
    BiGlBindMaterial(col);

    bgnline();
    v[0]= (float)p->x;
    v[1]= (float)p->y;
    v[2]=-(float)p->z;
    v3f(v);
    v3f(v);
    endline();
}

void BiOverWritePolyg3(int np,BIPOINT *p,BICOLOR *col)
{
    int i;
    float vf[3];

    BiGlStartOverWrite();
    BiGlBindMaterial(col);

    bgnpolygon();
    for(i=0; i<np; i++)
    {
        vf[0]= (float)p[i].x;
        vf[1]= (float)p[i].y;
        vf[2]=-(float)p[i].z;
        v3f(vf);
    }
    endpolygon();
}


void BiDrawLine3(BIPOINT *a,BIPOINT *b,BICOLOR *col)
{
    float v[3];

    BiGlStartZBufferWithNoLight();
    BiGlBindMaterial(col);

    bgnline();
    v[0]= (float)a->x;
    v[1]= (float)a->y;
    v[2]=-(float)a->z;
    v3f(v);
    v[0]= (float)b->x;
    v[1]= (float)b->y;
    v[2]=-(float)b->z;
    v3f(v);
    endline();
}

void BiDrawPset3(BIPOINT *p,BICOLOR *col)
{
    float v[3];

    BiGlStartZBufferWithNoLight();
    BiGlBindMaterial(col);

    bgnline();
    v[0]= (float)p->x;
    v[1]= (float)p->y;
    v[2]=-(float)p->z;
    v3f(v);
    v3f(v);
    endline();
}

void BiDrawCircle3(BIPOINT *p,real radx,real rady,BICOLOR *col)
{
    long ang;
    float v[3];

    BiGlStartZBufferWithNoLight();
    BiGlBindMaterial(col);

    bgnpolygon();
    v[2]=-p->z;
    for(ang=0; ang<65536; ang+=2048)
    {
        v[0]=p->x+BiCos(ang)*radx;
        v[1]=p->y+BiSin(ang)*rady;
        v3f(v);
    }
    endpolygon();
}

void BiDrawPolyg3(int np,BIPOINT *p,BIPOINT *n,BICOLOR *col)
{
    int i;
    float vf[3],nf[3];

    BiGlStartZBufferWithLight();
    BiGlBindMaterial(col);

    bgnpolygon();
    if(BiSamePoint(n,&BiOrgP)!=BI_TRUE)
    {
        nf[0]= (float)n->x;
        nf[1]= (float)n->y;
        nf[2]=-(float)n->z;
        n3f(nf);
    }
    else
    {
        /* printf("Zero Normal is Detected!\n"); */
    }
    for(i=0; i<np; i++)
    {
        if(i==0 || BiSamePoint(&p[i],&p[i-1])!=BI_TRUE)
	{
            vf[0]= (float)p[i].x;
            vf[1]= (float)p[i].y;
            vf[2]=-(float)p[i].z;
            v3f(vf);
	}
    }
    endpolygon();
}

void BiDrawPolygGrad3(int np,BIPOINT *p,BIPOINT *n,BICOLOR *col)
{
    int i;
    float v[6];

    BiGlStartZBufferWithLight();
    BiGlBindMaterial(col);

    bgnpolygon();
    for(i=0; i<np; i++)
    {
        if(BiSamePoint(&n[i],&BiOrgP)!=BI_TRUE)
        {
            v[3]= (float)n[i].x;
            v[4]= (float)n[i].y;
            v[5]=-(float)n[i].z;
            n3f(v+3);
        }
        else
        {
            /* printf("Zero Normal is Detected!\n"); */
        }

        if(i==0 || BiSamePoint(&p[i],&p[i-1])!=BI_TRUE)
	{
            v[0]= (float)p[i].x;
            v[1]= (float)p[i].y;
            v[2]=-(float)p[i].z;
            v3f(v);
        }
    }
    endpolygon();
}


static void BiXText3(BIPOINT *p,char *tx[],int hori,int vert,BICOLOR *col)
{
    int i;
    real siz;
    BIVECTOR tmp,dwn,loc;

    BiConvLtoG(&loc,p,&BiCurAxs);

    siz=20.0F*(BiPrjMode==BIPRJ_PERS ? loc.z : BiOrthoDist)/BiPrj.magy;
    BiSetPoint(&dwn,0,-siz,0);
    BiRotFastGtoL(&dwn,&dwn,&BiCurAxs.t);

    BiGlStartZBufferWithNoLight();
    BiGlBindMaterial(col);

    pushmatrix();

    tmp=*p;
    for(i=0; tx[i]!=NULL; i++)
    {
        cmov(tmp.x,tmp.y,-tmp.z);
        charstr(tx[i]);
        BiAddPoint(&tmp,&tmp,&dwn);
    }

    popmatrix();
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

    BiGlStartZBufferWithNoLight();
    BiGlBindMaterial(col);

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
            bgnpolygon();
            BiGlV3f(&r[0]);
            BiGlV3f(&r[1]);
            BiGlV3f(&r[2]);
            BiGlV3f(&r[3]);
            endpolygon();
            break;
        case BIMK_CROSS:
            bgnline();
            BiGlV3f(&r[0]);
            BiGlV3f(&r[2]);
            endline();
            bgnline();
            BiGlV3f(&r[1]);
            BiGlV3f(&r[3]);
            endline();
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

        bgnpolygon();
        BiGlV3f(&r[0]);
        BiGlV3f(&r[1]);
        BiGlV3f(&r[2]);
        BiGlV3f(&r[3]);
        BiGlV3f(&r[4]);
        BiGlV3f(&r[5]);
        BiGlV3f(&r[6]);
        BiGlV3f(&r[7]);
        endpolygon();
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


/*****************************************************************************/
typedef struct {
    float dat[17];
} BISGIMAT;


static int nextMaterial=1;
static int alive[8*8*8];
static BISGIMAT col[8*8*8];

extern BIAXIS BiEyeAxs;

static void BiDefineGlMaterial(int i);
static void BiMakeGlMaterial(BISGIMAT *mat,int g,int r,int b);

static void BiInitGlMaterial(void)
{
    int i;
    nextMaterial=1;
    for(i=0; i<8*8*8; i++)
    {
        alive[i]=-1;
    }
}

static void BiGlBindMaterial(BICOLOR *col)
{
    int i;

    i=(((col->g>>5)*64 +(col->r>>5)*8 +(col->b>>5))&511);
    if(BiPreviousBindMaterial!=i || BiPreviousGlMode!=BiGlMode)
    {
        switch(BiGlMode)
	{
        case BIGL_ZBUFFER:
            if(alive[i]<0)
            {
                /* "Material Count %d\n",nextMaterial */
                BiDefineGlMaterial(i);
            }

            lmbind(MATERIAL,alive[i]);
            lmbind(BACKMATERIAL,alive[i]);
            break;
        default:
            cpack((unsigned long)0xff000000+
                  (unsigned long)col->b*0x10000+
                  (unsigned long)col->g*0x100+
                  (unsigned long)col->r);
            break;
	}
        BiPreviousBindMaterial=i;
        BiPreviousGlMode=BiGlMode;
    }
}

static void BiDefineGlMaterial(int i)
{
    int g,r,b;

    g=((i>>6)&7)*256/7;
    r=((i>>3)&7)*256/7;
    b=( i    &7)*256/7;
    BiMakeGlMaterial(&col[i],g,r,b);
    lmdef(DEFMATERIAL,nextMaterial,0,col[i].dat);

    alive[i]=nextMaterial;
    nextMaterial++;
}

static void BiMakeGlMaterial(BISGIMAT *mat,int g,int r,int b)
{
    float *dat,rf,gf,bf;

    dat=mat->dat;
    rf=((float)r)/256;
    gf=((float)g)/256;
    bf=((float)b)/256;

    dat[ 0]=ALPHA;
    dat[ 1]=1.0;
    dat[ 2]=AMBIENT;
    dat[ 3]=rf;
    dat[ 4]=gf;
    dat[ 5]=bf;
    dat[ 6]=DIFFUSE;
    dat[ 7]=rf;
    dat[ 8]=gf;
    dat[ 9]=bf;

    dat[10]=SHININESS;
    dat[11]=0.0;
    dat[12]=LMNULL;

/*    dat[10]=SPECULAR;
    dat[11]=0.0F;
    dat[12]=0.0F;
    dat[13]=0.0F;
    dat[14]=SHININESS;
    dat[15]=0.0;
    dat[16]=LMNULL; */
}
