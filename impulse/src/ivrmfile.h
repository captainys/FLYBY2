
extern int BiVrmlMode;

int BiVrmlOpen(char fn[]);
int BiVrmlPolygon(int np,BIPOINT p[],BICOLOR *c);
int BiVrmlOneSidedPolygon(int np,BIPOINT p[],BIVECTOR *n,BICOLOR *c);
int BiVrmlRoundPolygon(int np,BIPOINT p[],BIVECTOR n[],BICOLOR *c);
int BiVrmlOneSidedRoundPolygon(int np,BIPOINT p[],BIVECTOR n[],BIVECTOR *nom,BICOLOR *c);
int BiVrmlLine(BIPOINT *v1,BIPOINT *v2,BICOLOR *c);
int BiVrmlPset(BIPOINT *v1,BICOLOR *c);
int BiVrmlClose(void);

int BiSrfClose(void);
int BiSrfOpen(char fn[]);
int BiSrfSetOffset(BIVECTOR *v);
