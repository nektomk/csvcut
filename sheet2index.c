#include <stdio.h>
/** конверсия из вида принятого в табличных процессорах (A B X Y Z AA AB..)
	в обычный номер :)
		0 = A
		1 = B
		...
		25 = Z
		26 = AA
		27 = AB
		...
		701 = ZZ
		702 = AAA	
	ToDo: контроль переполнения
*/
unsigned long
sheet2ul(char *str,char **saveptr) {
	char *s;
	unsigned long index;
	int digit;
	s=str;
	index=(unsigned long)-1UL;
	
	while(*s) {
		if (*s>='a' || *s<='z') {
			digit=*s-'a';
		} else if (*s>='A' || *s<='Z') {
			digit=*s-'A';
		} else {
			break;
		}
		if (index==(unsigned long)-1UL) {
			index=digit;
		} else {
			index++;
			index*='z'-'a'+1;
			index+=digit;
		}
		s++;
	}
	if (saveptr) *saveptr=s;
	return index;
}
int main(int argc,char **argv) {
	int t;
	unsigned long i;
	for(t=1;t<argc;t++) {
		i=sheet2ul(argv[t],NULL);
		printf("%s = %lu\n",argv[t],i);
	}
	return 0;
}
