/* Machine Depending Module : Windows95 DIB Version */
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../impulse.h"
#include "../iutil.h"

#include <windowsx.h>

#include "wimpulse.h"

HFONT BiWinFont=NULL;

int BiFatal(void)
{
	static int first=1;
	char *msg=
		"Function "
		"BiFatal()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
	return BI_OK;
}

void BiOpenWindow(long wx,long wy)
{
	static int first=1;
	char *msg=
		"Function "
		"BiOpenWindow()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
}

void BiOpenWindowEx(long x,long y,long wx,long wy)
{
	static int first=1;
	char *msg=
		"Function "
		"BiOpenWindowEx()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
}

void BiCloseWindow(void)
{
	static int first=1;
	char *msg=
		"Function "
		"BiCloseWindow()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
}

/*****************************************************************************

   Device

*****************************************************************************/

void BiDeviceInitialize(void)
{
	static int first=1;
	char *msg=
		"Function "
		"BiDeviceInitialize()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
}

void BiUpdateDevice(void)
{
	static int first=1;
	char *msg=
		"Function "
		"BiUpdateDevice()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
}

void BiMouse(int *l,int *m,int *r,long *mx,long *my)
{
	static int first=1;
	char *msg=
		"Function "
		"BiMouse()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
}

int BiKey(int kcode)
{
	static int first=1;
	char *msg=
		"Function "
		"BiKey()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
	return 0;
}

int BiInkey(void)
{
	static int first=1;
	char *msg=
		"Function "
		"BiInkey()"
		" is called\n"
		"This function is not allowed in\n"
		"Blue Impulse-Win32API Plug Interface";
	if(first==1)
	{
		MessageBox(NULL,msg,"NOTICE TO DEVELOPER",MB_OK);
		first=0;
	}
	return 0;
}
