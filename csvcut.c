#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string.h>

#include "csv.h"
#include "ranges.h"
#include "parse.h"

/*
#define SEE(format,...) do { \
	fprintf(stderr,"%s:%d %s : ",__FILE__,__LINE__,__PRETTY_FUNCTION__); \
	fprintf(stderr,format,##__VA_ARGS__); \
	fputc('\n',stderr); \
} while(0)
*/

/** объеденить аргументы в строку через ' '
*/
char *
join_args(int quantity,char *args[]) {
	int t;
	int sz;
	char *ret,*s,*p;
	int len;
	/* выделить память под буфер
		для начала ориентируемся на 32 байта на аргумент
	*/
	sz=quantity*32;
	s=ret=malloc(sz);
	/* добавлять аргументы */
	for(t=0;t<quantity;t++) {
		p=args[t];
		p+=strspn(p," \t\v\f\r\n");	// пропустить лидирующие пробелы
		if (*p==0) continue;
		len=strlen(p);
		if (len==0) continue;
		if (s+len+1>=ret+sz) {
			// требуется увеличить буфер
			int newsz;
			char *tmp;
			newsz=sz+len+32;		// c некоторым запасом
			newsz+=32-(newsz%32);	// и выравнено по размеру на 32
			tmp=malloc(newsz);
			if (tmp==NULL) goto FAULT;
			memcpy(tmp,ret,s-ret);
			sz=newsz;
			s=tmp+(s-ret);
			free(ret);
			ret=tmp;
		}
		memcpy(s,p,len);
		s+=len;
		*s=' ';
		s++;
	}
	*s=0;
	return ret;
FAULT:
	if (ret) free(ret);
	return NULL;
}


static void
usage(void) {
	printf("cut intersection of columns and rows from cvs-formatted input\n");
	printf("Usage: cvscut <columns> : <rows>\n");
	exit(EXIT_FAILURE);
}
int
main(int argc,char *argv[]) {
	char *args,*s;
	Table *tab;
	RangeSet *colset=NULL;
	RangeSet *rowset=NULL;

	if (argc==1) usage();

	s=args=join_args(argc-1,argv+1);
	colset=parse_rangeset(NULL,s,&s);
	if (colset==NULL) { colset=new_rangeset();rangeset_add_range(colset,&FULLRANGE); }
	if (*s==':') {
		s++;
		rowset=parse_rangeset(NULL,s,&s);
	}
	if (rowset==NULL) { rowset=new_rangeset();rangeset_add_range(rowset,&FULLRANGE); }
	// наборы получены, дальше доп.инструкции
/*	if (*s) {
		char *name,*value;
		s++;
		while(*s) {
			name=NULL;value=NULL;
			if (parse_name_value(&name,&value,s,&s)) {
				if (strcasecmp(name,"z") || strcasecmp(name,"zip")) {
					csvformat.zip=1;
				}
			}
		}
	}
*/
	tab=read_table(NULL,stdin);
	if (tab==NULL) {
		fprintf(stderr,"error reading table\n");
		exit(EXIT_FAILURE);
	}
/*	printf("table %d x %d loaded\n",tab->width,tab->height);
	printf("select columns:");
	print_rangeset(colset);
	putchar('\n');
	printf("select rows:");
	print_rangeset(rowset);
	putchar('\n');
*/
	print_table_cut(tab,colset,rowset);
	return 0;
}
