#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"
#include "parse.h"

static void
test_row() {
	char *test[2]={
		"one,two,three",
		NULL
	};
	int t;
	Row r={0};
	char *s;
	for(t=0;test[t];t++) {
		s=strdup(test[t]);
		printf("%d check \"%s\" ",t,test[t]);
		if (!row_parse(&r,s,&s)) {
			printf("fault");
		} else {
			printf("ok");
			print_row(&r);
		}
		//free(p);
		putchar('\n');
	}
}
static void
test_table() {
	char *test[2]={
		"one,two,three",
		NULL
	};
	int t;
	Table tab={0};
	char *s;
	for(t=0;test[t];t++) {
		s=strdup(test[t]);
		printf("%d check \"%s\" ",t,test[t]);
		if (!parse_table(&tab,s,&s)) {
			printf("fault");
		} else {
			printf("ok");
			printf("tab.sz=%u tab.len=%u tab->row=%p",tab.sz,tab.len,tab.row);
			print_table(&tab);
		}
		//free(p);
		putchar('\n');
	}
}
static void
test_table_cut() {
	RangeSet *colset;
	RangeSet *rowset;
	Table *tab;
	FILE *f;
	f=fopen("sample.csv","rt");
	if (f==NULL) {
		fprintf(stderr,"Unable to open %s\n","sample.csv");
		return;
	}
	tab=read_table(NULL,f);
	if (tab==NULL) {
		fprintf(stderr,"Error reading table from %s\n","sample.csv");
	}
	printf("table %d x %d loaded\n",tab->width,tab->height);
	colset=parse_rangeset(NULL,"4..0 7",NULL);
	printf("select columns:");
	print_rangeset(colset);
	putchar('\n');
	rowset=parse_rangeset(NULL,"0..4 1 -1",NULL);
	printf("select rows:");
	print_rangeset(rowset);
	putchar('\n');
	print_table_cut(tab,colset,rowset);
}
