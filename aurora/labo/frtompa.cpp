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
	float *t;
	BIPOSATT *p;

	FILE *fp;
	fp=fopen(av[1],"r");
	n=0;
	i=0;
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
				i++;
			}
		}

		printf("%d\n",n);
		for(i=0; i<n; i++)
		{
			printf("%2f %2f %2f %d %d %d %2f\n",
			       p[i].p.x,p[i].p.y,p[i].p.z,
			       p[i].a.h,p[i].a.p,p[i].a.b,
			       t[i]);
		}
		fclose(fp);
	}
	return 0;
}

