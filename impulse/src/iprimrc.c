#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "impulse.h"

/* Primitive Resource */

BIPOINT BiOrgP={0.0F,0.0F,0.0F};
BIATTITUDE BiOrgA={0,0,0};
BIPOSATT BiOrgPA={
	{0.0F,0.0F,0.0F},
	{0,0,0}
};
BIAXIS BiOrgAxs={
	{0.0F,0.0F,0.0F},
	{0,0,0},
	{0.0F,1.0F,0.0F,1.0F,0.0F,1.0F}
};
BIPLANE BiOrgPln={
	{0.0F,0.0F,0.0F},
	{0.0F,1.0F,0.0F}
};



BIPOINT BiVecX={1.0F,0.0F,0.0F};
BIPOINT BiVecY={0.0F,1.0F,0.0F};
BIPOINT BiVecZ={0.0F,0.0F,1.0F};



BICOLOR BiBlack  ={  0,  0,  0};
BICOLOR BiBlue   ={  0,  0,255};
BICOLOR BiRed    ={  0,255,  0};
BICOLOR BiMagenta={  0,255,255};
BICOLOR BiGreen  ={255,  0,  0};
BICOLOR BiCyan   ={255,  0,255};
BICOLOR BiYellow ={255,255,  0};
BICOLOR BiWhite  ={255,255,255};
