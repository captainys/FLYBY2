#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>


#include "impulse.h"


#ifdef CLOCKS_PER_SEC
static unsigned long StartClock=0,PrevClock=0;
#else
static real PassedTime=0;
#endif



void BiResetClock(void)
{
#ifdef CLOCKS_PER_SEC
	PrevClock=clock();
	StartClock=clock();
#else
	PassedTime=0;
#endif
}

real BiPassedTime(void)
{
#ifdef CLOCKS_PER_SEC
	real passed;
	passed=(real)(clock()-StartClock)/CLOCKS_PER_SEC;
	return passed;
#else
	PassedTime+=0.05F;
	return PassedTime;
#endif
}

real BiLoopTime(real minimum)
{
#ifdef CLOCKS_PER_SEC
	real passed;
	while((passed=BiAbs((real)(clock()-PrevClock)/CLOCKS_PER_SEC))<minimum);
	PrevClock=clock();
	return passed;
#else
	minimum=BiLarger(minimum,0.050F);
	return minimum;		/* Assume 20 Frame Per Sec */
#endif
}

void BiWaitClock(real sec)
{
#ifdef CLOCKS_PER_SEC
	unsigned long OrgClock;
	real passed;
	OrgClock=clock();
	passed=0.0F;
	while(passed<sec)
	{
		passed=BiAbs((real)(clock()-OrgClock)/CLOCKS_PER_SEC);
	}
#else
	/* I can do nothing here! */
#endif
}

