#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impulse.h"
#include "i3dg.h"


#ifdef SEEKTIMETEST
extern long nAttach,nSeek;
#endif


static void BiAttachNew3dObj(BIOBJECT *neo);


extern int BiEpsMode;


static BIOBJECT *BiObjTop;   // Top of linear connection
static BIOBJECT *BiObjTree;  // Tree root



void BiClear3dObject(void)
{
	BiObjTop=NULL;
	BiObjTree=NULL;
}

BIOBJECT *BiTopObject(void)
{
	return BiObjTop;
}


static int ObjSiz[]=
{
	0,                     /* BIOBJ_NULL */
	sizeof(BIONEPOINT),    /* BIOBJ_PSET */
	sizeof(BITWOPOINT),    /* BIOBJ_LINE */
	sizeof(BICIRCLE),      /* BIOBJ_CIRCLE */
	sizeof(BINPOINT),      /* BIOBJ_POLYLINE */
	sizeof(BINPOINT),      /* BIOBJ_LINESEQ */
	sizeof(BINPOINT),      /* BIOBJ_POLYGON */
	sizeof(BIMARKER),      /* BIOBJ_MARKER */
	sizeof(BITEXT),        /* BIOBJ_TEXT */
	sizeof(BISRFMODELPOS), /* BIOBJ_SRFMODEL */
	sizeof(BIPICT2POS),    /* BIOBJ_PICT2 */
	sizeof(BINPNTNOM),     /* BIOBJ_RPOLYGON */
	sizeof(BISUBTREE),    /* BIOBJ_SUBTREE */
	0
};

#define BIOBJ_CORESIZE (sizeof(BIOBJECT)-sizeof(BIOBJUNION))

BIOBJECT *BiNew3dObject(int type,real z)
{
	BIOBJECT *neo;
	neo=(BIOBJECT *)BiGetTmpStack(BIOBJ_CORESIZE+ObjSiz[type]);
	if(neo!=NULL)
	{
		neo->z=z;
		neo->type=type;
		BiAttachNew3dObj(neo);
	}
	return neo;
}

static void BiAttachNew3dObj(BIOBJECT *neo)
{
	BIOBJECT *seek;



#ifdef BIACCEL3D
	if(BiEpsMode==BI_OFF)
	{
		neo->treePrev=NULL;
		neo->treeNext=BiObjTree;
		BiObjTree=neo;
		neo->prev=NULL;
		neo->next=BiObjTop;
		BiObjTop=neo;
		return;
	}
#endif



#ifdef SEEKTIMETEST
	nAttach++;
#endif

	neo->treeNext=NULL;
	neo->treePrev=NULL;

	if(BiObjTree==NULL)
	{
		BiObjTree=neo;
		BiObjTop=neo;
		neo->next=NULL;
		neo->prev=NULL;
		return;
	}

	seek=BiObjTree;
	while(1)
	{
	#ifdef SEEKTIMETEST
		nSeek++;
	#endif
		if(seek->z >= neo->z)
		{
			if(seek->treeNext==NULL)
			{
				seek->treeNext=neo;
				/* Also Generate Linear Chain */
				neo->prev=seek;
				neo->next=seek->next;
				seek->next=neo;
				if(neo->next!=NULL)
				{
					neo->next->prev=neo;
				}

				// Refine Tree
				if(neo->prev!=NULL &&
				   neo->prev->treePrev==NULL &&
				   neo->prev->prev!=NULL &&
				   neo->prev->prev->treePrev==NULL &&
				   neo->prev->prev->prev!=NULL &&
				   neo->prev->prev->prev->treePrev==NULL &&
				   neo->prev->prev->prev->prev!=NULL)
				{
					BIOBJECT *r,*l1,*l2,*l3,*l4;
					l4=neo;
					l3=neo->prev;
					l2=neo->prev->prev;
					l1=neo->prev->prev->prev;
					r=neo->prev->prev->prev->prev;

					if(r->treeNext==l1 &&
					   l1->treeNext==l2 &&
					   l2->treeNext==l3 &&
					   l3->treeNext==l4)
					{
						r->treeNext=l4;

						l4->treePrev=l2;
						l4->treeNext=NULL;

						l2->treePrev=l1;
						l2->treeNext=l3;

						l1->treeNext=NULL;
						l3->treeNext=NULL;
					}
				}

				break;
			}
			else
			{
				seek=seek->treeNext;
				continue;
			}
		}
		else
		{
			if(seek->treePrev==NULL)
			{
				seek->treePrev=neo;
				/* Also Generate Linear Tree */
				neo->next=seek;
				neo->prev=seek->prev;
				seek->prev=neo;
				if(neo->prev!=NULL)
				{
					neo->prev->next=neo;
				}
				else
				{
					BiObjTop=neo;
				}

				// Refine Tree
				if(neo->next!=NULL &&
				   neo->next->treeNext==NULL &&
				   neo->next->next!=NULL &&
				   neo->next->next->treeNext==NULL &&
				   neo->next->next->next!=NULL &&
				   neo->next->next->next->treeNext==NULL &&
				   neo->next->next->next->next!=NULL)
				{
					BIOBJECT *r,*l1,*l2,*l3,*l4;
					l4=neo;
					l3=neo->next;
					l2=neo->next->next;
					l1=neo->next->next->next;
					r=neo->next->next->next->next;

					if(r->treePrev==l1 &&
					   l1->treePrev==l2 &&
					   l2->treePrev==l3 &&
					   l3->treePrev==l4)
					{
						r->treePrev=l4;

						l4->treePrev=NULL;
						l4->treeNext=l2;

						l2->treePrev=l3;
						l2->treeNext=l1;

						l1->treePrev=NULL;
						l3->treePrev=NULL;
					}
				}

				break;
			}
			else
			{
				seek=seek->treePrev;
				continue;
			}
		}
	}
}

////////////////////////////////////////////////////////////

void *BiGetCurrentTreeRoot(void)
{
	return BiObjTree;
}

void *BiGetCurrentTreeHead(void)
{
	return BiObjTop;
}

void BiSetTreeRootAndHead(void *treeRoot,void *treeHead)
{
	BiObjTree=(BIOBJECT *)treeRoot;
	BiObjTop=(BIOBJECT *)treeHead;
}

void *BiInsSubTree(BIPOINT *cen,BIPOINT bbx[8])
{
	int clip;
	BIPOINT tmp[8],tmpCen;
	real z;

	BiConvLtoG(&tmp[0],&bbx[0],&BiCnvAxs);
	BiConvLtoG(&tmp[1],&bbx[1],&BiCnvAxs);
	BiConvLtoG(&tmp[2],&bbx[2],&BiCnvAxs);
	BiConvLtoG(&tmp[3],&bbx[3],&BiCnvAxs);
	BiConvLtoG(&tmp[4],&bbx[4],&BiCnvAxs);
	BiConvLtoG(&tmp[5],&bbx[5],&BiCnvAxs);
	BiConvLtoG(&tmp[6],&bbx[6],&BiCnvAxs);
	BiConvLtoG(&tmp[7],&bbx[7],&BiCnvAxs);

	if(BiCheckInsideViewPort(&clip,8,tmp)==BI_IN)
	{
		BIOBJECT *neo;

		BiConvLtoG(&tmpCen,cen,&BiCnvAxs);
		z=tmpCen.z;

		neo=BiNew3dObject(BIOBJ_SUBTREE,z);
		if(neo!=NULL)
		{
			neo->attr.subTree.subTree=NULL;
			neo->attr.subTree.subHead=NULL;
			return neo;
		}
	}
	return NULL;
}

void *BiInsSubTreeZ(real z)
{
	BIOBJECT *neo;

	neo=BiNew3dObject(BIOBJ_SUBTREE,z);
	if(neo!=NULL)
	{
		neo->attr.subTree.subTree=NULL;
		neo->attr.subTree.subHead=NULL;
		return neo;
	}

	return NULL;
}

void BiSetSubTreeRootHead(void *subTree,void *root,void *head)
{
	BIOBJECT *obj;
	obj=(BIOBJECT *)subTree;
	obj->attr.subTree.subTree=(BIOBJECT *)root;
	obj->attr.subTree.subHead=(BIOBJECT *)head;
}
