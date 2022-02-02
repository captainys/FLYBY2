#include <stdio.h>
#include <stdlib.h>
#include <skyhawk.h>
#include <string.h>
#include <impulse.h>


int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage : frtompa ?.fr > ?.mpa\n");
		return 1;
	}

	int i,n;
	float *t,totalTime;
	BIPOSATT *p;

	FILE *fp;
	fp=fopen(av[1],"r");
	n=0;
	i=0;
	totalTime=0.0F;
	if(fp!=NULL)
	{
		char buf[256];
		while(fgets(buf,256,fp)!=NULL)
		{
			int ac;
			char *av[32];
			SkArguments(&ac,av,31,buf);

			if(strcmp(av[0],"MDF")==0)
			{
				// Skip
			}
			else if(strcmp(av[0],"NFR")==0)
			{
				n=atoi(av[1]);
				t=new float[n];
				p=new BIPOSATT[n];
			}
			else if(strcmp(av[0],"F")==0)
			{
				if(n==0)
				{
					fprintf(stderr,"F appeared before NFR\n");
					fclose(fp);
					return 1;
				}
				if(i>=n)
				{
					fprintf(stderr,"F exceeded NFR\n");
					fclose(fp);
					return 1;
				}
				p[i].p.x=atof(av[1]);
				p[i].p.y=atof(av[2]);
				p[i].p.z=atof(av[3]);
				p[i].a.h=atoi(av[4]);
				p[i].a.p=atoi(av[5]);
				p[i].a.b=atoi(av[6]);
				t[i]=atof(av[7]);
				totalTime=t[i];
				i++;
			}
		}

		printf("MOTIONPATH\n");
		printf("FXC 0 300.00 200.00 0.00\n");
		printf("MAG 0.00 1.00 1.00\n");
		printf("FXL -500.00 1000.00 0.00\n");
		printf("CLK 0.05\n");
		printf("\n");
		printf("OBJ\n");
		printf("VIR\n");
		printf("TIM 0.00 %.2f\n",totalTime);
		printf("INP %.2f %.2f %.2f\n",p[0].p.x,p[0].p.y,p[0].p.z);
		printf("INA %d %d %d\n",p[0].a.h,p[0].a.p,p[0].a.b);
		printf("INS 0\n");
		printf("SC1 1 0.00 120.00 0.00 8.50 2.00 255 255 255\n");
		printf("SB1 1 500.00 2 2000.00 3\n");
		printf("SO1 0.30 -0.30 -9.00\n");
		printf("SC2 4 0.00 0.10 9.50 9.50 0.00 255 255 255\n");
		printf("SB2 1 500.00 2 2000.00 3\n");
		printf("SO2 0.00 0.00 -1.00\n");
		printf("PTH\n");
		printf("TIM %.2f\n",totalTime);


		printf("\n");
		printf("RAW\n");
		printf("%d\n",n);
		for(i=0; i<n-1; i++)
		{
			printf("%2f %2f %2f %d %d %d %2f\n",
			       p[i].p.x,p[i].p.y,p[i].p.z,
			       p[i].a.h,p[i].a.p,p[i].a.b,
			       t[i+1]-t[i]);
		}
		printf("%2f %2f %2f %d %d %d %2f\n",
		       p[i].p.x,p[i].p.y,p[i].p.z,
		       p[i].a.h,p[i].a.p,p[i].a.b,
		       0.1F);

		printf("HPB 0 0 0\n");
		printf("EON\n");
		printf("EOO\n");
		printf("\n");
		printf("END\n");


		fclose(fp);
	}
	return 0;
}

