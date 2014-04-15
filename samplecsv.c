#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_WIDTH
#define MAX_WIDTH 200
#endif
#ifndef MAX_HEIGHT
#define MAX_HEIGHT 20000
#endif

int main(int argc,char *argv[]) {
	int i,j;
	int width,height;
	int triangle=0;
	int chess=0;
	int oddrow=0;
	int oddcol=0;
	char *s;
	if (argc<3) {
		printf("generate sample CSV tables\n");
		printf("Usage: samplecsv [width] [height] [option]\n");
		printf("Options are\n");
		printf("\t triangle,-triangle - triangular table\n");
		printf("\t chess,-chess - chess table\n");
		printf("\t row,-row - omit odd/even rows\n");
		printf("\t col,-col - omit add/even columns\n");
		exit(EXIT_FAILURE);
	}
	width=strtol(argv[1],&s,0);
	if (errno!=0 || s[0]!=0 || width<1 || width>=MAX_WIDTH) {
		printf("'width' should be int between 1..%d\n",MAX_WIDTH);
		exit(EXIT_FAILURE);
	}
	height=strtol(argv[2],&s,0);
	if (errno!=0 || s[0]!=0 || height<1 || height>=MAX_HEIGHT) {
		printf("'height' should be int between 1..%d but %s\n",MAX_HEIGHT,argv[3]);
		exit(EXIT_FAILURE);
	}
	for(argc-=3,argv+=3;argc;argc--,argv++) {
		if (strcmp(*argv,"triangle")==0) triangle=1;
		if (strcmp(*argv,"-triangle")==0) triangle=-1;
		if (strcmp(*argv,"chess")==0) chess=1;
		if (strcmp(*argv,"-chess")==0) chess=-1;
		if (strcmp(*argv,"row")==0) oddrow=1;
		if (strcmp(*argv,"-row")==0) oddrow=-1;
		if (strcmp(*argv,"col")==0) oddcol=1;
		if (strcmp(*argv,"-col")==0) oddcol=-1;
	}
	for(i=1;i<=height;i++) {
		for(j=1;j<=width;j++) {
			if (j>1) putchar(',');
			if (triangle==1 && i>j) continue;
			if (triangle==-1 && i<j) continue;
			if (chess==1 && (i%2)==(j%2)) continue;
			if (chess==-1 && (i%2)!=(j%2)) continue;
			if (oddcol==1 && (j%2)) continue;
			if (oddcol==-1 && !(j%2)) continue;
			if (oddrow==1 && (i%2)) continue;
			if (oddrow==-1 && !(i%2)) continue;
			
			printf("%d.%d",i,j);
		}
		putchar('\n');
	}
	return 0;
}
