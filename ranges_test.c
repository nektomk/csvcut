#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ranges.h"


static void
test_rsi() {
	char *test[]={
		"1..5 5..1 -2..2",
		NULL
	};
	int t;
	RangeSet *rs;
	RSI rsi;
	char *s;
	int i;
	for(t=0;test[t];t++) {
		s=test[t];
		printf("parse_rangeset %s\n",s);
		rs=parse_rangeset(NULL,s,&s);
		if (!rs) {
			fprintf(stderr,"error parse_rangeset %s\n",s);
		}
		print_rangeset(rs);
		putchar('\n');
		printf("on width=10\n");
		for(rsi_first(&rsi,&i,rs,10);!rsi_end(&rsi,&i);rsi_next(&rsi,&i)) {
			printf("%d ",i);
		}
		putchar('\n');
	}
}
static void
test_parse_range() {
	// todo: 1/vX..100/xxAxxA
	static char *range[]={
		"1",
		"a",
		"1.2",
		"2..1",
		"-2...-3",
		".1",
		"..2",
		"1.",
		"2..",
		NULL
	};
	int t;
	char *s;
	Range r;
	for(t=0;range[t];t++) {
		s=range[t];
		printf("%d check \"%s\" ",t,s);
		if (!parse_range(&r,s,&s)) {
			printf("fault\n");
			continue;
		}
		printf("ok %ld..%ld/%d\n",r.d1,r.d2,r.every);
	}
}

static void
test_rangeset_parse() {
	static char *test[]={
		"1 2 3",
		"1 2 3 : helo",
		NULL
	};
	int t;
	char *s;
	RangeSet rs={0};
	for(t=0;test[t];t++) {
		s=test[t];
		printf("%d check \"%s\" ",t,s);
		if (!parse_rangeset(&rs,s,&s)) {
			printf("fault\n");
			continue;
		}
		printf("ok ");
		print_rangeset(&rs);
		putchar('\n');
		rangeset_clear(&rs);
	}
}
