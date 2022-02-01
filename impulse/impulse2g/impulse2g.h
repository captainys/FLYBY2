#ifndef IMPULSE_G_IS_INCLUDED
#define IMPULSE_G_IS_INCLUDED
/* { */

#include <impulse.h>

#ifdef __cplusplus
extern "C" {
#endif

int BiOpenWindow2G(int wid,int hei);
int BiUpdateDevice2G(void);
void BiMouse2G(int *lb,int *mb,int *rb,int *mx,int *my,int wait);
int BiKey2G(void);

void BiStartBuffer2G(BIPOSATT *vp);
void BiSwapBuffers2G(void);
void BiZoom(void);
void BiMooz(void);
void BiOrbitViewer2G(void);
void BiGetOrbitViewer2G(BIPOSATT *pos);
void BiCube2G(float x,float y,float z,float lx,float ly,float lz,BICOLOR *c);
void BiLine2G(float x1,float y1,float z1,float x2,float y2,float z2,BICOLOR *c);void BiPset2G(float x,float y,float z,BICOLOR *c);
void BiPlane2G(BIPOSATT *pos);
void BiFalcon2G(BIPOSATT *pos);

#ifdef __cplusplus
}
#endif

/* } */
#endif
