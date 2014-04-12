#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"
#include "ranges.h"

// режим работы задаваемый разными опци€ми
CsvFormat csvformat={
	.fs=",;",
	.rs="\n",
};

/** считать номер строки/€чейки
*/
int
parse_index(char **ps,Index *pnum) {
	char *s;
	Index num;
	int negate;
	if (ps==NULL) return 0;
	s=*ps;
	num=0L;
	negate=0;
	if (*s=='-') {
		negate=1;
		s++;
	}
	if ((*s>='a' && *s<='z') || (*s>='A' && *s<='Z')) {
		// номер задан в виде а-л€ Exel
		do {
			if (num) num=num*('z'-'a');
			if (*s>='a' && *s<='z') num+=*s-'a';
			else if (*s>='A' && *s<='Z') num+=*s-'A';
			else if (*s>='0' && *s<='9') num+=*s-'0';
			else break;	// конец номера
			s++;
		} while(*s);
	} else if (*s>='0' && *s<='9') {
		// номер задан в дес€тичном виде
		while (*s && (*s>='0' && *s<='9')) {
			if (num) num=num*10;
			num+=*s-'0';
			s++;
		}
	} else {
		return 0;
	}
	if (ps) *ps=s;
	if (pnum) *pnum=negate?-num:num;
	return 1;
}


/** считать определение диапазона
	вернуть размер полученного диапазона
	допустимые варианты:
		1 : диапазон из единственного значени€ {1 1}
		1..2 или 1..30 : диапазон из двух значений {1 30}
		..2  4..      : диапазон от первой €чейки {1..2} до последней €чейки {4..-1}
*/
Range *
parse_range(Range *r,char *s,char **saveptr) {
	Index d1,d2;
	int every;
	every=1;
	s+=strspn(s,SPACES);
	if (!parse_index(&s,&d1)) d1=-1;	// считываем первый номер
	if (*s=='.') {
		// диапазон задан двум€ числами
		s+=strspn(s,".");
		if (!parse_index(&s,&d2)) d2=-1;	// считываем второй номер
	} else {
		d2=d1;
	}
	if (saveptr) *saveptr=s;
	return set_range(r,d1,d2,every);
}

RangeSet *
parse_rangeset(RangeSet *rs,char *s,char **saveptr) {
	Range *r;
	if (rs==NULL)rs=new_rangeset();
	r=NULL;
	while(*s && (r=parse_range(NULL,s,&s))!=NULL) {
		rangeset_add_range(rs,r);
		r=NULL;
		s+=strspn(s,SPACES);
		if (*s==':') break;
	}
	if (saveptr) *saveptr=s;
	return rs;
}
int
parse_name(char **pname,char *s,char **saveptr) {
	char *p;
	s+=strspn(s,SPACES);
	p=s;
	if (!s || !isalpha(*s)) return 0;
	s++;
	while(isalnum(*s))
		s++;
	*s=0;
	s++;
	if (pname) *pname=p;
	if (saveptr) *saveptr=s;
	return 1;
}
int parse_value(char **pvalue,char *s,char **saveptr) {
	char *p;
	char *value=NULL;
	p=s;
	s+=strcspn(s,SPACES);
	if (p==s) return 0;

	*s=0;
	s++;
	value=p;

	if (pvalue) *pvalue=value;
	if (saveptr) *saveptr=s;

	return 1;
}
/* прочесть опции в виде »ћя=«Ќј„≈Ќ»≈
*/
int
parse_name_value(char **pname,char **pvalue,char *s,char **saveptr) {
	char *name=NULL,*value=NULL;
	if (pname) name=*pname;
	if (pvalue) value=*pvalue;
	if (!parse_name(&name,s,&s)) return 0;
	s+=strspn(s,SPACES);
	if (*s=='=') parse_value(&value,s+1,&s);
	if (pname) *pname=name;
	if (pvalue) *pvalue=value;
	if (saveptr) *saveptr=s;
	return 1;
}
