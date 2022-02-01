#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

#include "impulse.h"
#include "ivrmfile.h"


int BiVrmlOpen(char fn[]);
int BiVrmlSetOffset(BIVECTOR *v);
int BiVrmlPolygon(int np,BIPOINT p[],BICOLOR *c);
int BiVrmlOneSidedPolygon(int np,BIPOINT p[],BIVECTOR *n,BICOLOR *c);
int BiVrmlRoundPolygon(int np,BIPOINT p[],BIVECTOR n[],BICOLOR *c);
int BiVrmlLine(BIPOINT *v1,BIPOINT *v2,BICOLOR *c);
int BiVrmlPset(BIPOINT *v1,BICOLOR *c);
int BiVrmlClose(void);



enum {
	VRML_FACE,
	VRML_LINE
};

typedef struct VrmlVtxTag {
	struct VrmlVtxTag *next;
	BIPOINT p;
	int round;
} VRMLVTX;

typedef struct VrmlFaceTag {
	struct VrmlFaceTag *next;
	int type,twoSide;
	BICOLOR c;
	int nv,*v;
	int nn,*n;
	int done;
} VRMLFACE;


enum {
	BIVRM_VRML,
	BIVRM_SRF,
	BIVRM_VRML2
};


int BiVrmlMode=BI_OFF;
static int BiVrmlType=0;

static char vrmlFileName[256];
static int nVlist;
static VRMLVTX *vList,*vListBtm;
static int nNlist;
static VRMLVTX *nList,*nListBtm;
static int nFlist;
static VRMLFACE *fList,*fListBtm;

static BIVECTOR offset;

extern BIAXISF BiEyeAxs;




/**********************************************************************/
static int BiLookForIndex(BIPOINT *p,int r,VRMLVTX *lst);
static int BiLookForVtxIndex(BIPOINT *v,int r);
static int BiLookForNomIndex(BIVECTOR *n);
static int BiGetNewIndex(BIPOINT *v,int r,int *nlst,VRMLVTX **lst,VRMLVTX **btm);
static int BiGetVtxIndex(BIPOINT *v,int r);
static int BiGetNomIndex(BIPOINT *n);
static VRMLFACE *BiGetNewFace(void);
static void BiVrmlWriteHeader(void);
static void BiVrmlWriteTail(void);
static void BiVrmlWriteCoordNormal(void);
static void BiVrmlWriteFace(VRMLFACE *ptr);
static void BiVrmlWriteLine(VRMLFACE *ptr);
static void BiVrmlWriteFaceLine(void);
static void BiVrmlWrite(void);
/**********************************************************************/
static int BiGetValue(BIPOINT *p,VRMLVTX *lst,int id)
{
	int i;
	VRMLVTX *ini;
	if(lst!=NULL)
	{
		ini=lst;
		for(i=0; i<id; i++)
		{
			ini=ini->next;
		}
		*p=ini->p;
		return BI_OK;
	}
	return BI_ERR;
}

static int BiLookForIndex(BIPOINT *p,int r,VRMLVTX *lst)
{
	int i;
	VRMLVTX *ptr;
	ptr=lst;
	for(i=0; ptr!=NULL; i++)
	{
		if(BiSamePoint(p,&ptr->p)==BI_TRUE && ptr->round==r)
		{
			return i;
		}
		ptr=ptr->next;
	}
	return -1;
}

static int BiLookForVtxIndex(BIPOINT *v,int r)
{
	return BiLookForIndex(v,r,vList);
}

static int BiLookForNomIndex(BIVECTOR *n)
{
	return BiLookForIndex(n,BI_OFF,nList);
}

static int BiGetNewIndex(BIPOINT *v,int r,int *nlst,VRMLVTX **lst,VRMLVTX **btm)
{
	int idx;
	VRMLVTX *neo;
	neo=(VRMLVTX *)BiMalloc(sizeof(VRMLVTX));
	if(neo!=NULL)
	{
		neo->next=NULL;
		neo->p=*v;
		neo->round=r;

		idx=*nlst;
		(*nlst)++;

		if((*lst)!=NULL)
		{
			(*btm)->next=neo;
			(*btm)=neo;
		}
		else
		{
			(*lst)=neo;
			(*btm)=neo;
		}
		return idx;
	}
	else
	{
		return -1;
	}
}

static int BiGetVtxIndex(BIPOINT *vOrg,int round)
{
	int idx;
	BIPOINT v;
	BiConvLtoG(&v,vOrg,&BiEyeAxs);
	BiAddPoint(&v,&v,&offset);
	if((idx=BiLookForVtxIndex(&v,round))>=0)
	{
		return idx;
	}
	else
	{
		return BiGetNewIndex(&v,round,&nVlist,&vList,&vListBtm);
	}
}

static int BiGetNomIndex(BIPOINT *nOrg)
{
	int idx;
	BIPOINT n;
	BiRotFastLtoG(&n,nOrg,&BiEyeAxs.t);
	if((idx=BiLookForNomIndex(&n))>=0)
	{
		return idx;
	}
	else
	{
		return BiGetNewIndex(&n,BI_OFF,&nNlist,&nList,&nListBtm);
	}
}

/**********************************************************************/

static VRMLFACE *BiGetNewFace(void)
{
	VRMLFACE *neo;
	neo=(VRMLFACE *)BiMalloc(sizeof(VRMLFACE));
	if(neo!=NULL)
	{
		neo->next=NULL;
		neo->v=NULL;
		neo->n=NULL;
		neo->done=BI_FALSE;
		nFlist++;
		if(fList!=NULL)
		{
			fListBtm->next=neo;
			fListBtm=neo;
		}
		else
		{
			fList=neo;
			fListBtm=neo;
		}
	}
	return neo;
}

/**********************************************************************/

int BiVrmlOpen(char fn[])
{
	strcpy(vrmlFileName,fn);
	nVlist=0;
	vList=NULL;
	vListBtm=NULL;

	nNlist=0;
	nList=NULL;
	nListBtm=NULL;

	nFlist=0;
	fList=NULL;
	fListBtm=NULL;

	BiVrmlMode=BI_ON;
	BiVrmlType=BIVRM_VRML;

	offset=BiOrgP;

	return BI_OK;
}

int BiVrmlSetOffset(BIVECTOR *v)
{
	offset=*v;
	return BI_OK;
}

int BiSrfOpen(char fn[])
{
	if(BiVrmlOpen(fn)==BI_OK)
	{
		BiVrmlType=BIVRM_SRF;
		return BI_OK;
	}
	return BI_ERR;
}

int BiSrfSetOffset(BIVECTOR *v)
{
	return BiVrmlSetOffset(v);
}

/**********************************************************************/

int BiVrmlPolygon(int np,BIPOINT p[],BICOLOR *c)
{
	int i,*idx1;
	VRMLFACE *fac1;

	fac1=BiGetNewFace();
	idx1=(int *)BiMalloc(sizeof(int)*np);
	if(fac1!=NULL && idx1!=NULL)
	{
		for(i=0; i<np; i++)
		{
			idx1[i]=BiGetVtxIndex(&p[i],BI_OFF);
			if(idx1[i]<0)
			{
				BiFree(idx1);
				return BI_ERR;
			}
		}
		fac1->type=VRML_FACE;
		fac1->twoSide=BI_ON;   /* OFF->ON 11/27/1999 */
		fac1->c=*c;
		fac1->nv=np;
		fac1->v=idx1;
		fac1->nn=0;
		fac1->n=NULL;

		return BI_OK;
	}
	return BI_ERR;

// ####Modified 11/10/1999

//	int i,*idx1,*idx2;
//	VRMLFACE *fac1,*fac2;
//
//	fac1=BiGetNewFace();
//	idx1=(int *)BiMalloc(sizeof(int)*np);
//	fac2=BiGetNewFace();
//	idx2=(int *)BiMalloc(sizeof(int)*np);
//	if(fac1!=NULL && fac2!=NULL && idx1!=NULL && idx2!=NULL)
//	{
//		for(i=0; i<np; i++)
//		{
//			idx1[i     ]=BiGetVtxIndex(&p[i],BI_OFF);
//			idx2[np-1-i]=idx1[i];
//			if(idx1[i]<0)
//			{
//				BiFree(idx1);
//				BiFree(idx2);
//				return BI_ERR;
//			}
//		}
//		fac1->type=VRML_FACE;
//		fac1->twoSide=BI_OFF;
//		fac1->c=*c;
//		fac1->nv=np;
//		fac1->v=idx1;
//		fac1->nn=0;
//		fac1->n=NULL;
//
//		fac2->type=VRML_FACE;
//		fac2->twoSide=BI_OFF;
//		fac2->c=*c;
//		fac2->nv=np;
//		fac2->v=idx2;
//		fac2->nn=0;
//		fac2->n=NULL;
//		return BI_OK;
//	}
//	return BI_ERR;
}

int BiVrmlOneSidedPolygon(int np,BIPOINT p[],BIVECTOR *n,BICOLOR *c)
{
	int i,*vidx,*nidx;
	VRMLFACE *fac;

	fac=BiGetNewFace();
	vidx=(int *)BiMalloc(sizeof(int)*np);
	nidx=(int *)BiMalloc(sizeof(int));
	if(fac!=NULL && vidx!=NULL && nidx!=NULL)
	{
		if(BiTwist3(np,p,n)==BITWIST_LEFT)
		{
			for(i=0; i<np; i++)
			{
				vidx[i]=BiGetVtxIndex(&p[i],BI_OFF);
				if(vidx[i]<0)
				{
					goto ERREND;
				}
			}
		}
		else
		{
			for(i=0; i<np; i++)
			{
				vidx[i]=BiGetVtxIndex(&p[np-1-i],BI_OFF);
				if(vidx[i]<0)
				{
					goto ERREND;
				}
			}
		}
		nidx[0]=BiGetNomIndex(n);
		if(nidx[0]<0)
		{
			goto ERREND;
		}

		fac->type=VRML_FACE;
		fac->twoSide=BI_OFF;
		fac->c=*c;
		fac->nv=np;
		fac->v=vidx;
		fac->nn=1;
		fac->n=nidx;
		return BI_OK;
	}
	return BI_ERR;
ERREND:
	BiFree(vidx);
	BiFree(nidx);
	return BI_ERR;
}

int BiVrmlRoundPolygon(int np,BIPOINT p[],BIVECTOR n[],BICOLOR *c)
{
	int i,*vidx1,*nidx1;
	VRMLFACE *fac1;
	BIVECTOR a;

	a=BiOrgP;
	for(i=0; i<np; i++)
	{
		BiAddPoint(&a,&a,&n[i]);
	}
	if(a.x*a.x+a.y*a.y+a.z*a.z>YSEPS)
	{
		BiNormalize(&a,&a);
	}

	fac1=BiGetNewFace();
	vidx1=(int *)BiMalloc(sizeof(int)*np);
	nidx1=(int *)BiMalloc(sizeof(int)*np);

	if(fac1!=NULL && vidx1!=NULL && nidx1!=NULL)
	{
		for(i=0; i<np; i++)
		{
			if(BiTwist3(np,p,&a)==BITWIST_LEFT)
			{
				vidx1[i     ]=BiGetVtxIndex(&p[i],BI_ON);
				nidx1[i     ]=BiGetNomIndex(&n[i]);
				if(vidx1[i]<0 || nidx1[i]<0)
				{
					goto ERREND;
				}
			}
			else
			{
				vidx1[np-1-i]=BiGetVtxIndex(&p[i],BI_ON);
				nidx1[np-1-i]=BiGetNomIndex(&n[i]);
				if(vidx1[np-1-i]<0 || nidx1[np-1-i]<0)
				{
					goto ERREND;
				}
			}
		}

		fac1->type=VRML_FACE;
		fac1->twoSide=BI_OFF;
		fac1->c=*c;
		fac1->nv=np;
		fac1->v=vidx1;
		fac1->nn=np;
		fac1->n=nidx1;

		return BI_OK;
	}
	return BI_ERR;
ERREND:
	BiFree(vidx1);
	BiFree(nidx1);
	return BI_ERR;
}

int BiVrmlOneSidedRoundPolygon(int np,BIPOINT p[],BIVECTOR n[],BIVECTOR *nom,BICOLOR *c)
{
	int i,*vidx,*nidx;
	VRMLFACE *fac;

	fac=BiGetNewFace();
	vidx=(int *)BiMalloc(sizeof(int)*np);
	nidx=(int *)BiMalloc(sizeof(int)*np);
	if(fac!=NULL && vidx!=NULL && nidx!=NULL)
	{
		if(BiTwist3(np,p,nom)==BITWIST_LEFT)
		{
			for(i=0; i<np; i++)
			{
				vidx[i]=BiGetVtxIndex(&p[i],BI_ON);
				nidx[i]=BiGetNomIndex(&n[i]);
				if(vidx[i]<0 || nidx[i]<0)
				{
					goto ERREND;
				}
			}
		}
		else
		{
			for(i=0; i<np; i++)
			{
				vidx[i]=BiGetVtxIndex(&p[np-1-i],BI_ON);
				nidx[i]=BiGetNomIndex(&n[np-1-i]);
				if(vidx[i]<0 || nidx[i]<0)
				{
					goto ERREND;
				}
			}
		}

		fac->type=VRML_FACE;
		fac->twoSide=BI_OFF;
		fac->c=*c;
		fac->nv=np;
		fac->v=vidx;
		fac->nn=np;
		fac->n=nidx;
		return BI_OK;
	}
	return BI_ERR;
ERREND:
	BiFree(vidx);
	BiFree(nidx);
	return BI_ERR;
}

int BiVrmlLine(BIPOINT *v1,BIPOINT *v2,BICOLOR *c)
{
	int *vidx;
	VRMLFACE *fac;

	fac=BiGetNewFace();
	vidx=(int *)BiMalloc(sizeof(int)*2);
	if(fac!=NULL && vidx!=NULL)
	{
		vidx[0]=BiGetVtxIndex(v1,BI_OFF);
		vidx[1]=BiGetVtxIndex(v2,BI_OFF);
		if(vidx[0]>=0 && vidx[1]>=0)
		{
			fac->type=VRML_LINE;
			fac->twoSide=BI_OFF;
			fac->c=*c;
			fac->nv=2;
			fac->v=vidx;
			fac->nn=0;
			fac->n=NULL;
			return BI_OK;
		}
	}
	return BI_ERR;
}

int BiVrmlPset(BIPOINT *v1,BICOLOR *c)
{
	return BI_OK;
}

/**********************************************************************/
static FILE *vf;

static void BiVrmlWriteHeader(void)
{
	BIVECTOR ev,uv;

	fprintf(vf,"#VRML V1.0 ascii\n");
	fprintf(vf,"Separator {\n");
	fprintf(vf,"ShapeHints {\n");
	fprintf(vf,"vertexOrdering UNKNOWN_ORDERING faceType UNKNOWN_FACE_TYPE\n");
	fprintf(vf,"}\n");


	BiAngleToVectorF(&ev,&uv,&BiEyeAxs.a);
	BiNormalize(&ev,&ev);

/*	fprintf(vf,"PerspectiveCamera {\n");
	fprintf(vf,"position %.3f %.3f %.3f\n",BiEyeAxs.p.x,BiEyeAxs.p.y,-BiEyeAxs.p.z);
	fprintf(vf,"orientation %.3f %.3f %.3f 0\n",ev.x,ev.y,-ev.z);
	fprintf(vf,"}\n"); */
}

static void BiVrmlWriteTail(void)
{
	fprintf(vf,"}\n");
}

static void BiVrmlWriteCoordNormal(void)
{
	VRMLVTX *ptr;

	if(vList!=NULL)
	{
		fprintf(vf,"Coordinate3 {\n");
		fprintf(vf,"point [\n");
		for(ptr=vList; ptr!=NULL; ptr=ptr->next)
		{
			fprintf(vf,"%.3f %.3f %.3f,\n",ptr->p.x,ptr->p.y,-ptr->p.z);
		}
		fprintf(vf,"] }\n");
	}

	if(nList!=NULL)
	{
		fprintf(vf,"Normal {\n");
		fprintf(vf,"vector [\n");
		for(ptr=nList; ptr!=NULL; ptr=ptr->next)
		{
			fprintf(vf,"%.3f %.3f %.3f,\n",ptr->p.x,ptr->p.y,-ptr->p.z);
		}
		fprintf(vf,"] }\n");
	}
}

static void BiVrmlWriteFace(VRMLFACE *ptr)
{
	int i,first;
	real r,g,b;
	VRMLFACE *sameColor;

 	fprintf(vf,"Separator {\n");

	if(ptr->twoSide==BI_OFF)
	{
		fprintf(vf,
		"ShapeHints { vertexOrdering COUNTERCLOCKWISE shapeType SOLID }\n");
	}
	else
	{
		fprintf(vf,
		"ShapeHints { vertexOrdering UNKNOWN_ORDERING shapeType UNKNOWN_SHAPE_TYPE }\n");
	}

	if(ptr->nn>1)
	{
		fprintf(vf,"NormalBinding { value PER_VERTEX }\n");
	}
	else
	{
		fprintf(vf,"NormalBinding { value PER_FACE }\n");
	}

	r=(real)ptr->c.r/255.0F;
	g=(real)ptr->c.g/255.0F;
	b=(real)ptr->c.b/255.0F;
	fprintf(vf,"Material { diffuseColor %4.2f %4.2f %4.2f }\n",r,g,b);

	fprintf(vf,"IndexedFaceSet {\n");


	first=BI_TRUE;
	fprintf(vf,"coordIndex [\n");
	for(sameColor=ptr; sameColor!=NULL; sameColor=sameColor->next)
	{
		if(ptr->c.r==sameColor->c.r &&
		   ptr->c.g==sameColor->c.g &&
		   ptr->c.b==sameColor->c.b &&
		   ptr->twoSide==sameColor->twoSide &&
		   sameColor->done!=BI_TRUE)
		{
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}
			if(sameColor->nv>0)
			{
				for(i=0; i<sameColor->nv; i++)
				{
					fprintf(vf,"%4d,",sameColor->v[i]);
					if(i%12==11)
					{
						fprintf(vf,"\n");
					}
				}
			}
			fprintf(vf,"-1\n");
			first=BI_FALSE;
		}
	}
	fprintf(vf,"]\n");

	first=BI_TRUE;
	fprintf(vf,"normalIndex [\n");
	for(sameColor=ptr; sameColor!=NULL; sameColor=sameColor->next)
	{
		if(ptr->c.r==sameColor->c.r &&
		   ptr->c.g==sameColor->c.g &&
		   ptr->c.b==sameColor->c.b &&
		   ptr->twoSide==sameColor->twoSide &&
		   sameColor->done!=BI_TRUE)
		{
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}
			if(sameColor->nn>0)
			{
				for(i=0; i<sameColor->nn; i++)
				{
					fprintf(vf,"%4d,",sameColor->n[i]);
					if(i%12==11)
					{
						fprintf(vf,"\n");
					}
				}
			}
			fprintf(vf,"-1\n");
			first=BI_FALSE;
			sameColor->done=BI_TRUE;
		}
	}
	fprintf(vf,"]\n");

	fprintf(vf,"}\n");

	fprintf(vf,"}\n");
}

static void BiVrmlWriteLine(VRMLFACE *ptr)
{
	int i,first;
	real r,g,b;
	VRMLFACE *sameColor;

/*	fprintf(vf,"Separator {\n");  */

	r=(real)ptr->c.r/255.0F;
	g=(real)ptr->c.g/255.0F;
	b=(real)ptr->c.b/255.0F;
	fprintf(vf,"Material { diffuseColor %4.2f %4.2f %4.2f }\n",r,g,b);

	fprintf(vf,"IndexedLineSet {\n");

	fprintf(vf,"coordIndex [\n");

	first=BI_TRUE;
	for(sameColor=ptr; sameColor!=NULL; sameColor=sameColor->next)
	{
		if(ptr->c.r==sameColor->c.r &&
		   ptr->c.g==sameColor->c.g &&
		   ptr->c.b==sameColor->c.b &&
		   ptr->twoSide==sameColor->twoSide &&
		   sameColor->done!=BI_TRUE)
		{
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}

			for(i=0; i<sameColor->nv; i++)
			{
				fprintf(vf,"%4d,",sameColor->v[i]);
				if(i%12==11)
				{
					fprintf(vf,"\n");
				}
			}
			fprintf(vf,"-1\n");
			first=BI_FALSE;
		}
	}
	fprintf(vf,"]\n");

	fprintf(vf,"}\n");

/* 	fprintf(vf,"}\n"); */
}

static void BiVrmlWriteFaceLine(void)
{
	VRMLFACE *ptr;
	for(ptr=fList; ptr!=NULL; ptr=ptr->next)
	{
		if(ptr->done!=BI_TRUE)  /* 11/27/1999 */
		{
			switch(ptr->type)
			{
			case VRML_FACE:
				BiVrmlWriteFace(ptr);
				break;
			case VRML_LINE:
				BiVrmlWriteLine(ptr);
				break;
			}
		}
	}
}

static void BiVrmlWrite(void)
{
	BiVrmlWriteHeader();
	BiVrmlWriteCoordNormal();
	BiVrmlWriteFaceLine();
	BiVrmlWriteTail();
}


static void BiVrmlFreeVtxList(VRMLVTX *ptr)
{
	if(ptr!=NULL)
	{
		BiVrmlFreeVtxList(ptr->next);
	}
	BiFree(ptr);
}

static void BiVrmlFreeFaceList(VRMLFACE *ptr)
{
	if(ptr!=NULL)
	{
		BiVrmlFreeFaceList(ptr->next);
	}
	BiFree(ptr->v);
	BiFree(ptr->n);
	BiFree(ptr);
}

static void BiVrmlFree(void)
{
	BiVrmlFreeVtxList(vList);
	BiVrmlFreeVtxList(nList);
}

int BiVrmlClose(void)
{
	if(BiVrmlMode==BI_ON && BiVrmlType==BIVRM_VRML)
	{
		BiVrmlMode=BI_OFF;
		vf=fopen(vrmlFileName,"w");
		if(vf!=NULL)
		{
			BiVrmlWrite();
			fclose(vf);
			BiVrmlFree();
			return BI_OK;
		}
	}
	return BI_ERR;
}

/**********************************************************/

static void BiVrml2WriteCoordNormalColorList(void)
{
	VRMLVTX *ptr;
	VRMLFACE *geo;

	fprintf(vf,"        Shape\n");
	fprintf(vf,"        {\n");
	fprintf(vf,"            geometry IndexedFaceSet\n");
	fprintf(vf,"            {\n");
	fprintf(vf,"                coord \n");

	fprintf(vf,"                DEF BI_COORD Coordinate\n");
	fprintf(vf,"                {\n");
	fprintf(vf,"                    point\n");
	fprintf(vf,"                    [\n");

	for(ptr=vList; ptr!=NULL; ptr=ptr->next)
	{
		fprintf
		   (vf,"                        %.3f %.3f %.3f",
		    ptr->p.x,ptr->p.y,-ptr->p.z);
		fprintf(vf,(ptr->next!=NULL ? ",\n" : "\n"));
	}

	fprintf(vf,"                    ]\n");
	fprintf(vf,"                }\n");

	fprintf(vf,"                normal \n");
	fprintf(vf,"                DEF BI_NORMAL Normal\n");
	fprintf(vf,"                {\n");
	fprintf(vf,"                    vector\n");
	fprintf(vf,"                    [\n");

	for(ptr=nList; ptr!=NULL; ptr=ptr->next)
	{
		BiNormalize(&ptr->p,&ptr->p);
		fprintf
		   (vf,"                        %.3f %.3f %.3f",
		    ptr->p.x,ptr->p.y,-ptr->p.z);
		fprintf(vf,(ptr->next!=NULL ? ",\n" : "\n"));
	}

	fprintf(vf,"                    ]\n");
	fprintf(vf,"                }\n");

	fprintf(vf,"                color \n");
	fprintf(vf,"                DEF BI_COLOR Color\n");
	fprintf(vf,"                {\n");
	fprintf(vf,"                    color\n");
	fprintf(vf,"                    [\n");

	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		real r,g,b;

		r=(real)geo->c.r/255.0F;
		g=(real)geo->c.g/255.0F;
		b=(real)geo->c.b/255.0F;
		fprintf
		   (vf,"                        %.3f %.3f %.3f",r,g,b);
		fprintf(vf,(geo->next!=NULL ? ",\n" : "\n"));
	}

	fprintf(vf,"                    ]\n");
	fprintf(vf,"                }\n");
	fprintf(vf,"            }\n");
	fprintf(vf,"        }\n");
}

static void BiVrml2WriteOneSidedFaceSet_Color(BICOLOR *c)
{
	int idx,first;
	real r,g,b;
	VRMLFACE *geo;

	r=(real)c->r/255.0F;
	g=(real)c->g/255.0F;
	b=(real)c->b/255.0F;

	fprintf(vf,"        ,\n");

	fprintf(vf,"        Shape\n");
	fprintf(vf,"        {\n");
	fprintf(vf,"            appearance Appearance\n");
	fprintf(vf,"            {\n");
	fprintf(vf,"                material Material\n");
	fprintf(vf,"                {\n");
	fprintf(vf,"                    diffuseColor %.3f %.3f %.3f\n",r,g,b);
	fprintf(vf,"                }\n");
	fprintf(vf,"            }\n");

	fprintf(vf,"            geometry IndexedFaceSet\n");
	fprintf(vf,"            {\n");
	fprintf(vf,"                normalPerVertex TRUE\n");
	fprintf(vf,"                coord USE BI_COORD\n");
	fprintf(vf,"                color NULL\n");
	fprintf(vf,"                normal USE BI_NORMAL\n");
	fprintf(vf,"                creaseAngle 0.5\n");
	fprintf(vf,"                solid TRUE\n");
	fprintf(vf,"\n");

	idx=0;
	first=BI_TRUE;
	fprintf(vf,"                coordIndex\n");
	fprintf(vf,"                [\n");
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(geo->type==VRML_FACE &&
		   geo->twoSide!=BI_TRUE &&  /* 11/27/1999 */
		   geo->v!=NULL &&
		   geo->n!=NULL &&
		   geo->c.r==c->r &&
		   geo->c.g==c->g &&
		   geo->c.b==c->b)
		{
			int i;
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}
			fprintf(vf,"                    ");

			fprintf(vf,"%4d",geo->v[0]);
			for(i=1; i<geo->nv; i++)
			{
				fprintf(vf,",%4d",geo->v[i]);
			}
			fprintf(vf,",-1");
			first=BI_FALSE;
		}
		idx++;
	}
	fprintf(vf,"\n");
	fprintf(vf,"                ]\n");


	idx=0;
	first=BI_TRUE;
	fprintf(vf,"                normalIndex\n");
	fprintf(vf,"                [\n");
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(geo->type==VRML_FACE &&
		   geo->twoSide!=BI_TRUE &&  /* 11/27/1999 */
		   geo->v!=NULL &&
		   geo->n!=NULL &&
		   geo->c.r==c->r &&
		   geo->c.g==c->g &&
		   geo->c.b==c->b)
		{
			int i;
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}
			fprintf(vf,"                    ");

			if(geo->nn==geo->nv)
			{
				fprintf(vf,"%4d",geo->n[0]);
				for(i=1; i<geo->nn; i++)
				{
					fprintf(vf,",");
					fprintf(vf,"%4d",geo->n[i]);
				}
			}
			else
			{
				fprintf(vf,"%4d",geo->n[0]);
				for(i=1; i<geo->nv; i++)
				{
					fprintf(vf,",");
					fprintf(vf,"%4d",geo->n[0]);
				}
			}

			fprintf(vf,",-1");
			first=BI_FALSE;
			geo->done=BI_TRUE;
		}
		idx++;
	}
	fprintf(vf,"\n");
	fprintf(vf,"                ]\n");

	fprintf(vf,"            }\n");
	fprintf(vf,"        }\n");
}

static void BiVrml2WriteOneSidedFaceSet(void)
{
	VRMLFACE *geo;
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(geo->type==VRML_FACE &&
		   geo->twoSide!=BI_TRUE && /* 11/27/1999 */
		   geo->v!=NULL &&
		   geo->n!=NULL &&
		   geo->done!=BI_TRUE)
		{
			BiVrml2WriteOneSidedFaceSet_Color(&geo->c);
		}
	}
}

static void BiVrml2WriteTwoSidedFaceSet(void)
{
	int idx,first;
	VRMLFACE *geo;

	fprintf(vf,"        ,\n");

	fprintf(vf,"        Shape\n");
	fprintf(vf,"        {\n");
	fprintf(vf,"            geometry IndexedFaceSet\n");
	fprintf(vf,"            {\n");

	fprintf(vf,"                colorPerVertex FALSE\n");
	fprintf(vf,"                coord USE BI_COORD\n");
	fprintf(vf,"                normal NULL\n");
	fprintf(vf,"                color USE BI_COLOR\n");
	fprintf(vf,"                solid FALSE\n");
	fprintf(vf,"\n");

	idx=0;
	first=BI_TRUE;
	fprintf(vf,"                coordIndex\n");
	fprintf(vf,"                [\n");
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(geo->type==VRML_FACE &&
		   geo->twoSide==BI_TRUE && /* 11/27/1999 */
		   geo->v!=NULL && geo->n==NULL &&
		   geo->done!=BI_TRUE) /* 11/27/1999 */
		{
			int i;
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}
			fprintf(vf,"                    ");

			fprintf(vf,"%4d",geo->v[0]);
			for(i=1; i<geo->nv; i++)
			{
				fprintf(vf,",%4d",geo->v[i]);
			}
			fprintf(vf,",-1");
			first=BI_FALSE;
		}
		idx++;
	}
	fprintf(vf,"\n");
	fprintf(vf,"                ]\n");


	idx=0;
	first=BI_TRUE;
	fprintf(vf,"                colorIndex\n");
	fprintf(vf,"                [\n");
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(idx%12==0)
		{
			fprintf(vf,"\n");
			fprintf(vf,"                    ");
		}

		if(geo->type==VRML_FACE &&
		   geo->twoSide==BI_TRUE && /* 11/27/1999 */
		   geo->v!=NULL && geo->n==NULL &&
		   geo->done!=BI_TRUE) /* 11/27/1999 */
		{
			if(first!=BI_TRUE)
			{
				fprintf(vf,",");
			}
			fprintf(vf,"%4d",idx);
			first=BI_FALSE;
		}
		idx++;
	}
	fprintf(vf,"\n");
	fprintf(vf,"                ]\n");

	fprintf(vf,"            }\n");
	fprintf(vf,"        }\n");
}

static void BiVrml2WriteLineSet(void)
{
	int idx,first;
	VRMLFACE *geo;

	fprintf(vf,"        ,\n");

	fprintf(vf,"        Shape\n");
	fprintf(vf,"        {\n");
	fprintf(vf,"            geometry IndexedLineSet\n");
	fprintf(vf,"            {\n");

	fprintf(vf,"                colorPerVertex FALSE\n");
	fprintf(vf,"                coord USE BI_COORD\n");
	fprintf(vf,"                color USE BI_COLOR\n");
	fprintf(vf,"\n");


	idx=0;
	first=BI_TRUE;
	fprintf(vf,"                coordIndex\n");
	fprintf(vf,"                [\n");
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(geo->type==VRML_LINE)
		{
			int i;
			if(first!=BI_TRUE)
			{
				fprintf(vf,",\n");
			}
			fprintf(vf,"                    ");

			fprintf(vf,"%4d",geo->v[0]);
			for(i=1; i<geo->nv; i++)
			{
				fprintf(vf,",%4d",geo->v[i]);
			}
			fprintf(vf,",-1");
			first=BI_FALSE;
		}
		idx++;
	}
	fprintf(vf,"\n");
	fprintf(vf,"                ]\n");


	idx=0;
	first=BI_TRUE;
	fprintf(vf,"                colorIndex\n");
	fprintf(vf,"                [\n");
	for(geo=fList; geo!=NULL; geo=geo->next)
	{
		if(idx%12==0)
		{
			fprintf(vf,"\n");
			fprintf(vf,"                    ");
		}

		if(geo->type==VRML_LINE)
		{
			if(first!=BI_TRUE)
			{
				fprintf(vf,",");
			}
			fprintf(vf,"%4d",idx);
			first=BI_FALSE;
		}
		idx++;
	}
	fprintf(vf,"\n");
	fprintf(vf,"                ]\n");


	fprintf(vf,"            }\n");
	fprintf(vf,"        }\n");
}

static void BiVrml2Write(void)
{
	fprintf(vf,"#VRML V2.0 utf8\n");

	fprintf(vf,"Transform\n");
	fprintf(vf,"{\n");
	fprintf(vf,"    children\n");
	fprintf(vf,"    [\n");

	BiVrml2WriteCoordNormalColorList();
	BiVrml2WriteOneSidedFaceSet();
	BiVrml2WriteTwoSidedFaceSet();
	BiVrml2WriteLineSet();

	fprintf(vf,"    ]\n");
	fprintf(vf,"}\n");
}

static void BiVrml2Free(void)
{
	BiVrmlFree();
}

int BiVrml2Open(char fn[])
{
	if(BiVrmlOpen(fn)==BI_OK)
	{
		BiVrmlType=BIVRM_VRML2;
		return BI_OK;
	}
	return BI_ERR;
}

int BiVrml2Close(void)
{
	if(BiVrmlMode==BI_ON && BiVrmlType==BIVRM_VRML2)
	{
		VRMLFACE *geo;
		for(geo=fList; geo!=NULL; geo=geo->next)
		{
			geo->done=BI_FALSE;
		}

		BiVrmlMode=BI_OFF;
		vf=fopen(vrmlFileName,"w");
		if(vf!=NULL)
		{
			BiVrml2Write();
			fclose(vf);
			BiVrml2Free();
			return BI_OK;
		}
	}
	return BI_ERR;
}


/******** ORIGINAL
int BiVrmlPolygon(int np,BIPOINT p[],BICOLOR *c)
{
	int i,*idx;
	VRMLFACE *fac;

	fac=BiGetNewFace();
	idx=(int *)BiMalloc(sizeof(int)*np);
	if(fac!=NULL && idx!=NULL)
	{
		for(i=0; i<np; i++)
		{
			idx[i]=BiGetVtxIndex(&p[i]);
			if(idx[i]<0)
			{
				BiFree(fac);
				BiFree(idx);
				return BI_ERR;
			}
		}
		fac->type=VRML_FACE;
		fac->twoSide=BI_ON;
		fac->c=*c;
		fac->nv=np;
		fac->v=idx;
		fac->nn=0;
		fac->n=NULL;
		return BI_OK;
	}
	return BI_ERR;
}

int BiVrmlRoundPolygon(int np,BIPOINT p[],BIVECTOR n[],BICOLOR *c)
{
	int i,*vidx,*nidx;
	VRMLFACE *fac;

	fac=BiGetNewFace();
	vidx=(int *)BiMalloc(sizeof(int)*np);
	nidx=(int *)BiMalloc(sizeof(int)*np);
	if(fac!=NULL && vidx!=NULL && nidx!=NULL)
	{
		for(i=0; i<np; i++)
		{
			vidx[i]=BiGetVtxIndex(&p[i]);
			nidx[i]=BiGetNomIndex(&n[i]);
			if(vidx[i]<0 || nidx[i]<0)
			{
				goto ERREND;
			}
		}

		fac->type=VRML_FACE;
		fac->twoSide=BI_ON;
		fac->c=*c;
		fac->nv=np;
		fac->v=vidx;
		fac->nn=np;
		fac->n=nidx;
		return BI_OK;
	}
	return BI_ERR;
ERREND:
	BiFree(vidx);
	BiFree(nidx);
	return BI_ERR;
}
****************/

static void BiSrfWriteCoord(void)
{
	VRMLVTX *ptr;

	if(vList!=NULL)
	{
		for(ptr=vList; ptr!=NULL; ptr=ptr->next)
		{
			fprintf(vf,"VER ");
			fprintf(vf,"%.3f %.3f %.3f",ptr->p.x,ptr->p.y,ptr->p.z);
			if(ptr->round==BI_ON)
			{
				fprintf(vf," R");
			}
			fprintf(vf,"\n");
		}
	}
}

static void BiSrfWriteFace(VRMLFACE *ptr)
{
	int i,hiCol;
	BIPOINT cen,nom,tmp;

 	fprintf(vf,"FAC\n");

	hiCol=((ptr->c.g>>3)<<10)+((ptr->c.r>>3)<<5)+(ptr->c.b>>3);
	fprintf(vf,"COL %d\n",hiCol);


	cen=BiOrgP;
	for(i=0; i<ptr->nv; i++)
	{
		BiGetValue(&tmp,vList,ptr->v[i]);
		BiAddPoint(&cen,&cen,&tmp);
	}
	BiDivPoint(&cen,&cen,ptr->nv);

	fprintf(vf,"NOR ");
	fprintf(vf,"%.3f %.3f %.3f ",cen.x,cen.y,cen.z);

	if(ptr->twoSide==BI_OFF)
	{
		nom=BiOrgP;
		for(i=0; i<ptr->nn; i++)
		{
			BiGetValue(&tmp,nList,ptr->n[i]);
			BiAddPoint(&nom,&nom,&tmp);
		}
		BiNormalize(&nom,&nom);
		fprintf(vf,"%.3f %.3f %.3f\n",nom.x,nom.y,nom.z);
	}
	else
	{
		fprintf(vf,"0 0 0\n");
	}

	for(i=0; i<ptr->nv; i++)
	{
		if(i%8==0)
		{
			fprintf(vf,"VER");
		}
		fprintf(vf," %d",ptr->v[i]);
		if(i%8==7 || i==ptr->nv-1)
		{
			fprintf(vf,"\n");
		}
	}

	fprintf(vf,"END\n");
}

static void BiSrfWritePolygon(void)
{
	VRMLFACE *ptr;
	for(ptr=fList; ptr!=NULL; ptr=ptr->next)
	{
		switch(ptr->type)
		{
		case VRML_FACE:
			BiSrfWriteFace(ptr);
			break;
		}
	}
}

static int BiSrfWrite(void)
{
	fprintf(vf,"SURF\n");
	BiSrfWriteCoord();
	BiSrfWritePolygon();
	fprintf(vf,"END\n");
	return BI_OK;
}

int BiSrfClose(void)
{
	if(BiVrmlMode==BI_ON && BiVrmlType==BIVRM_SRF)
	{
		BiVrmlMode=BI_OFF;
		vf=fopen(vrmlFileName,"w");
		if(vf!=NULL)
		{
			BiSrfWrite();
			fclose(vf);
			BiVrmlFree();
			return BI_OK;
		}
	}
	return BI_ERR;
}

