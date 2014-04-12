#include <stdio.h>

#include "csv.h"

int main() {
	Table *tab;
	int row,col;
	char *text;
	tab=read_table(NULL,stdin);
	for(col=0;col<tab->width;col++) {
		if (col!=0) putchar('\n');
		for(row=0;row<tab->height;row++) {
			if (row!=0) putchar(',');
			text=table_cell(tab,row,col);
			if (text!=NULL && text[0]!=0) printf("%s",text);
		}
	}
	return 0;
}


