#include <stdio.h>
#include <stddef.h>
#include "impulse.h"



/* Local Heap   to accelerate slug speeded windows halloc function. */

typedef struct {
	long used;
	char *buf;
} BITMPHEAPTAB;

#define TEMPHEAPSIZE 0x20000
#define TEMPHEAPTABSIZE 128
static long BiPrimeHeap;
static BITMPHEAPTAB BiHeap[TEMPHEAPTABSIZE];

void BiBeginTempHeap(void)
{
	int i;                             // 2002/12/01
	for(i=0; i<TEMPHEAPTABSIZE; i++)   // 2002/12/01
	{                                  // 2002/12/01
		BiHeap[i].buf=NULL;            // 2002/12/01
	}                                  // 2002/12/01

	BiPrimeHeap=0;
	if((BiHeap[0].buf=(char *)BiMalloc(TEMPHEAPSIZE*sizeof(char)))!=NULL)
	{
		BiHeap[0].used=0;
	}
	else
	{
		BiHeap[0].used=TEMPHEAPSIZE;
	}
}

void BiEndTempHeap(void)
{
	int i;
	for(i=0; i<TEMPHEAPTABSIZE; i++)
	{
		if(BiHeap[i].buf!=NULL)
		{
			BiFree(BiHeap[i].buf);
		}
	}
}

void *BiTempMalloc(unsigned long siz,size_t uni)
{
	long req,rest;
	void *ret;

	if(BiPrimeHeap<TEMPHEAPTABSIZE)
	{
		req=((siz*uni)+3)&~3;
		rest=TEMPHEAPSIZE-BiHeap[BiPrimeHeap].used;
		if(rest >= req)
		{
			ret=(void *)(BiHeap[BiPrimeHeap].buf+BiHeap[BiPrimeHeap].used);
			BiHeap[BiPrimeHeap].used+=req;
			return ret;
		}
		else if(BiPrimeHeap+1<TEMPHEAPTABSIZE)
		{
			if(BiHeap[BiPrimeHeap+1].buf!=NULL)    // 2002/12/01
			{                                      // 2002/12/01
				BiFree(BiHeap[BiPrimeHeap+1].buf); // 2002/12/01
				BiHeap[BiPrimeHeap+1].buf=NULL;    // 2002/12/01
			}                                      // 2002/12/01

			if(req>TEMPHEAPSIZE)
			{
				ret=(void *)BiMalloc(sizeof(unsigned char)*req);
			}
			else
			{
				ret=(void *)BiMalloc(sizeof(char)*TEMPHEAPSIZE);
			}

			if(ret!=NULL)
			{
				BiPrimeHeap++;
				BiHeap[BiPrimeHeap].used=req;
				BiHeap[BiPrimeHeap].buf=(char *)ret;
				return ret;
			}
		}
	}
	return NULL;
}

void *BiTempPushMalloc(long *prm,long *stk,unsigned long siz,size_t uni)
{
	*prm=BiPrimeHeap;
	*stk=BiHeap[BiPrimeHeap].used;
	return BiTempMalloc(siz,uni);
}

void BiTempPopMalloc(long prm,long stk)
{
	BiPrimeHeap=prm;
	BiHeap[prm].used=stk;
}
