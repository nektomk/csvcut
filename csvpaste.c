/** csv paste - построчное объединение таблиц CSV
	близкий аналог 'paste', ориентированный на CSV таблицы
*/
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"

static void
usage() {
	printf("csvpaste [file]\n");
	exit(EXIT_FAILURE);
}
int
main(int argc,char **argv) {
	int t;
	Table *src,*dst;
	Index dst_width,insert_pos;
	if (argc<2) {
		usage();
		return 0;
	}
	dst=NULL;
	if (argv[1][0]=='-' && argv[1][1]=='\0') {
		fprintf(stderr,"from stdin\n");
		dst=read_table(NULL,stdin);
	} else {
		FILE *f;
		f=fopen(argv[1],"r");
		if (f==NULL) {
			fprintf(stderr,"Unable to open %s\n",argv[1]);
			exit(EXIT_FAILURE);
		}
		dst=read_table(NULL,f);
		fclose(f);
	}
	if (dst==NULL) {
		fprintf(stderr,"Error reading table from %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}
	print_table_info(stdout,src);
	dst_width=dst->width;
	insert_pos=dst_width;
	for(t=2;t<argc;t++) {
		FILE *f;
		f=fopen(argv[1],"r");
		if (f==NULL) {
			fprintf(stderr,"Unable to open %s\n",argv[1]);
			exit(EXIT_FAILURE);
		}
		src=read_table(NULL,f);
		fclose(f);
	}
	return 0;
}
