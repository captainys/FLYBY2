#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "impulse.h"

typedef struct {
	int bik,asc;
} BIMATCHINGTABLE;

#define N_TAB (sizeof(BiTab)/sizeof(BIMATCHINGTABLE))

static BIMATCHINGTABLE BiTab[]=
{
	{BIKEY_ZERO      ,'0'},
	{BIKEY_ONE       ,'1'},
	{BIKEY_TWO       ,'2'},
	{BIKEY_THREE     ,'3'},
	{BIKEY_FOUR      ,'4'},
	{BIKEY_FIVE      ,'5'},
	{BIKEY_SIX       ,'6'},
	{BIKEY_SEVEN     ,'7'},
	{BIKEY_EIGHT     ,'8'},
	{BIKEY_NINE      ,'9'},
	{BIKEY_A         ,'A'},
	{BIKEY_B         ,'B'},
	{BIKEY_C         ,'C'},
	{BIKEY_D         ,'D'},
	{BIKEY_E         ,'E'},
	{BIKEY_F         ,'F'},
	{BIKEY_G         ,'G'},
	{BIKEY_H         ,'H'},
	{BIKEY_I         ,'I'},
	{BIKEY_J         ,'J'},
	{BIKEY_K         ,'K'},
	{BIKEY_L         ,'L'},
	{BIKEY_M         ,'M'},
	{BIKEY_N         ,'N'},
	{BIKEY_O         ,'O'},
	{BIKEY_P         ,'P'},
	{BIKEY_Q         ,'Q'},
	{BIKEY_R         ,'R'},
	{BIKEY_S         ,'S'},
	{BIKEY_T         ,'T'},
	{BIKEY_U         ,'U'},
	{BIKEY_V         ,'V'},
	{BIKEY_W         ,'W'},
	{BIKEY_X         ,'X'},
	{BIKEY_Y         ,'Y'},
	{BIKEY_Z         ,'Z'},
	{BIKEY_TEN0      ,'0'},
	{BIKEY_TEN1      ,'1'},
	{BIKEY_TEN2      ,'2'},
	{BIKEY_TEN3      ,'3'},
	{BIKEY_TEN4      ,'4'},
	{BIKEY_TEN5      ,'5'},
	{BIKEY_TEN6      ,'6'},
	{BIKEY_TEN7      ,'7'},
	{BIKEY_TEN8      ,'8'},
	{BIKEY_TEN9      ,'9'},
	{BIKEY_TENMUL    ,'*'},
	{BIKEY_TENDIV    ,'/'},
	{BIKEY_TENPLUS   ,'+'},
	{BIKEY_TENMINUS  ,'-'},
	{BIKEY_TENDOT    ,'.'}
};

/* {BIKEY_NULL      ,0} */
/* {BIKEY_STOP      ,0} */
/* {BIKEY_COPY      ,0} */
/* {BIKEY_F1        ,0} */
/* {BIKEY_F2        ,0} */
/* {BIKEY_F3        ,0} */
/* {BIKEY_F4        ,0} */
/* {BIKEY_F5        ,0} */
/* {BIKEY_F6        ,0} */
/* {BIKEY_F7        ,0} */
/* {BIKEY_F8        ,0} */
/* {BIKEY_F9        ,0} */
/* {BIKEY_F10       ,0} */
/* {BIKEY_SPACE     ,0} */
/* {BIKEY_ESC       ,0} */
/* {BIKEY_TAB       ,0} */
/* {BIKEY_CTRL      ,0} */
/* {BIKEY_SHIFT     ,0} */
/* {BIKEY_ALT       ,0} */
/* {BIKEY_BS        ,0} */
/* {BIKEY_RET       ,0} */
/* {BIKEY_HOME      ,0} */
/* {BIKEY_DEL       ,0} */
/* {BIKEY_UP        ,0} */
/* {BIKEY_DOWN      ,0} */
/* {BIKEY_LEFT      ,0} */
/* {BIKEY_RIGHT     ,0} */

int BiKeyToAscii(int bik)
{
	int i;
	for(i=0; i<N_TAB; i++)
	{
		if(bik==BiTab[i].bik)
		{
			return BiTab[i].asc;
		}
	}
	return 0;
}



char BiWindowName[256]="Application Window";

void BiSetWindowName(char str[])
{
	if(str!=NULL)
	{
		strncpy(BiWindowName,str,255);
	}
}
