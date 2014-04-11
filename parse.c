#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "ranges.h"

/** ������� ����� ������/������
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
		// ����� ����� � ���� �-�� Exel
		do {
			if (num) num=num*('z'-'a');
			if (*s>='a' && *s<='z') num+=*s-'a';
			else if (*s>='A' && *s<='Z') num+=*s-'A';
			else if (*s>='0' && *s<='9') num+=*s-'0';
			else break;	// ����� ������
			s++;
		} while(*s);
	} else if (*s>='0' && *s<='9') {
		// ����� ����� � ���������� ����
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


/** ������� ����������� ��������� �� ������ � ��������� � pfrom pto
	������� ������ ����������� ���������
	���������� ��������:
		1 : �������� �� ������������� �������� {1 1}
		1..2 ��� 1.30 : �������� �� ���� �������� {1 30}
		..2  4..      : �������� �� ������ ������ {1..2} �� ��������� ������ {4..-1}
*/
Range *
parse_range(Range *r,char *s,char **saveptr) {
	Index d1,d2;
	int every;
	every=1;
	s+=strspn(s,SPACES);
	if (!parse_index(&s,&d1)) d1=-1;	// ��������� ������ �����
	if (*s=='.') {
		// �������� ����� ����� �������
		s+=strspn(s,".");
		if (!parse_index(&s,&d2)) d2=-1;	// ��������� ������ �����
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
