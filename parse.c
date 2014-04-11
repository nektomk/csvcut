#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "ranges.h"

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
