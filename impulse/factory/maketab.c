#include <stdio.h>
#include <math.h>


#define YSPI 3.14159265
#define YSEPS 0.0001


typedef float real;

typedef struct {
	real x,y,z;
} BIPOINT;


static real BiSinTab[1025];
static real BiCosTab[1025];
static real BiTanTab[1025];
static long BiAsinTab[1025];
static long BiAcosTab[1025];
static long BiAtanTab1[1025]; /*  0Å`45deg */
static long BiAtanTab2[1025]; /* 45Å`90deg */
static real BiPow60Tab[1025];

static BIPOINT BiUnitTab[64*64];

static void BiMakeSinCosTanTable(void)
{
	int i;
	real rad;
	for(i=0; i<=1024; i++)
	{
		rad=(real)(2.0*YSPI)*((real)i)/(real)1024;
		BiSinTab[i]=(real)sin(rad);
		BiCosTab[i]=(real)cos(rad);
		if(i==256)
		{
			BiTanTab[i]= (real)40000000;
		}
		else if(i==768)
		{
			BiTanTab[i]=-(real)40000000;
		}
		else
		{
			BiTanTab[i]=(real)tan(rad);
		}

	}
}

static void BiMakeAsinAcosTable(void)
{
	int i;
	double a;
	for(i=0; i<=1024; i++)
	{
		a=((double)(i-512))/512.0;
		BiAsinTab[i]=(long)(asin(a)*0x8000/YSPI);
		BiAcosTab[i]=(long)(acos(a)*0x8000/YSPI);
	}
}

static void BiMakeAtanTable(void)
{
	int i;
	double a;
	for(i=0; i<=1024; i++)
	{
		a=((double)i)/1024;
		BiAtanTab1[i]=(long)(atan(a)*0x8000/YSPI);
		a=((real)i);
		BiAtanTab2[i]=(long)(atan(a)*0x8000/YSPI);
	}
}

static void BiMakeNormalizerTable(void)
{
	int i,j;
	real x,y,z,l;
	for(i=0; i<64; i++)
	{
		for(j=0; j<64; j++)
		{
			x=(real)(j-31)/31.0F;
			y=(real)(i-31)/31.0F;
			z=1.0F;
			l=sqrt(x*x+y*y+z*z);
			x/=l;
			y/=l;
			z/=l;
			BiUnitTab[i*64+j].x=x;
			BiUnitTab[i*64+j].y=y;
			BiUnitTab[i*64+j].z=z;
		}
	}
}



static void BiWriteTableR(char var[],real lst[],int n)
{
	int i,j;

	printf("real %s[%d]=\n{\n",var,n);
	for(i=0; i<n; i+=4)
	{
		printf("\t");
		for(j=0; j<4 && i+j<n; j++)
		{
			printf("% 2.6fF",lst[i+j]);
			if(i+j+1<n)
			{
				printf(",");
			}
		}
		printf("\n");
	}
	printf("};\n\n");
}

static void BiWriteTableL(char var[],long lst[],int n)
{
	int i,j;

	printf("long %s[%d]=\n{\n",var,n);
	for(i=0; i<n; i+=8)
	{
		printf("\t");
		for(j=0; j<8 && i+j<n; j++)
		{
			printf("%8ld",lst[i+j]);
			if(i+j+1<n)
			{
				printf(",");
			}
		}
		printf("\n");
	}
	printf("};\n\n");
}

static void BiWriteTableV(char var[],BIPOINT lst[],int n)
{
	int i;
	printf("BIPOINT %s[%d]=\n{\n",var,n);
	for(i=0; i<n; i++)
	{
		printf("\t");
		printf("{% 2.8fF,% 2.8fF,% 2.8fF}",lst[i].x,lst[i].y,lst[i].z);
		if(i+1<n)
		{
			printf(",");
		}
		printf("\n");
	}
	printf("};\n\n");
}


static void BiMakePow60Table(void)
{
	int i;
	real t;
	for(i=0; i<=1024; i++)
	{
		t=(real)i/1024;
		BiPow60Tab[i]=pow(t,60.0F);
	}
}


int main(void)
{
	BiMakeSinCosTanTable();
	BiMakeAsinAcosTable();
	BiMakeAtanTable();
	BiMakePow60Table();
	BiMakeNormalizerTable();

	printf("#include <stdio.h>\n");
	printf("#include <stddef.h>\n");
	printf("#include <math.h>\n");
	printf("#include \"impulse.h\"\n");

	BiWriteTableR("BiSinTab",BiSinTab,1025);
	BiWriteTableR("BiCosTab",BiCosTab,1025);
	BiWriteTableR("BiTanTab",BiTanTab,1025);
	BiWriteTableL("BiAsinTab",BiAsinTab,1025);
	BiWriteTableL("BiAcosTab",BiAcosTab,1025);
	BiWriteTableL("BiAtanTab1",BiAtanTab1,1025);
	BiWriteTableL("BiAtanTab2",BiAtanTab2,1025);
	BiWriteTableR("BiPow60Tab",BiPow60Tab,1025);
	BiWriteTableV("BiUnitTab",BiUnitTab,64*64);
	return 0;
}

