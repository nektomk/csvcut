#include <stdio.h>
int main() {
	int i,j;
	for(i='a';i<'z';i++) {
		for(j=0;j<25;j++) {
			if (j) putchar(',');
			printf("%c%d",i,j+1);
		}
		putchar('\n');
	}
	return 0;
}
