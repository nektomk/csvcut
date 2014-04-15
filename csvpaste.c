/** csv paste - построчное объединение таблиц CSV
	близкий аналог 'paste', ориентированный на CSV таблицы
*/
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"

void print_row_x(Row *row) {
	int t;
	if (row==NULL || row->len==0) {
		return ; //putchar('\n');
	}
	for(t=0;t<row->len;t++) {
		if (t!=0) putchar(',');
		printf("%s",row->text[t]);
	}
}
void print_colspan(int count) {
	int t;
	for(t=0;t<count;t++)
		putchar(',');
}
void print_rowspan(int count,int width) {
	int t;
	for(t=0;t<count;t++) {
		if (width>0) print_colspan(width);
		putchar('\n');
	}
}
void print_table_x(Table *tab) {
	int t;
	if (tab==NULL || tab->len==0) return;
	for(t=0;t<tab->len;t++) {
		if (t!=0) putchar('\n');
		print_row_x(tab->row[t]);
		if (tab->row[t]->len<tab->width) {
			print_colspan(tab->width-tab->row[t]->len);
		}
	}
}


void print_tabs(Table **tabs,int from,int to) {
	int totwidth,totheight;
	int t;
	int row;
	int colspan,rowspan;
	int firstcol;
	Table *tab;
	totwidth=0;
	totheight=0;
	for(t=from;t<=to;t++) {
		if (!tabs[t]) continue;
		totwidth+=tabs[t]->width;
		if (tabs[t]->height>totheight) {
			totheight=tabs[t]->height;
		}
	}
	rowspan=0;
	for(row=0;row<totheight;row++) {
		colspan=0;
		firstcol=1;
		for(t=from;t<=to;t++) {
			tab=tabs[t];
			if (!tab) break;
			if (row<tab->height && tab->row[row] && tab->row[row]->len) {
				if (rowspan) {
					print_rowspan(rowspan,(csvformat.omit_trail_cells?0:totwidth));
					rowspan=0;
				}
				if (firstcol) firstcol=!firstcol;
				else putchar(',');
				if (colspan) {
					print_colspan(colspan);
					colspan=0;
				}
				print_row_x(tab->row[row]);
				colspan+=tab->width-tab->row[row]->len;
			} else {
				colspan+=tab->width;
			}
		}
		if (colspan && !csvformat.omit_trail_cells) print_colspan(colspan);
		if (colspan==totwidth) {
			rowspan++;
		}
		if (rowspan==0 && row+1<totheight) putchar('\n');
	}
	if (rowspan && !csvformat.omit_trail_rows) print_rowspan(rowspan,(csvformat.omit_trail_cells?0:totwidth));
}

static void
usage() {
	printf("csvpaste [file]\n");
	exit(EXIT_FAILURE);
}

int
main(int argc,char **argv) {
	int t;
	Table **tabs;
	csvformat.omit_trail_cells=1;
	csvformat.omit_trail_rows=1;
	if (argc<2) {
		usage();
		return 0;
	}
	tabs=calloc(sizeof(Table *),argc);
	for(t=1;t<argc;t++) {
		if (argv[t][0]=='-' && argv[t][1]=='\0') {
			tabs[t]=read_table(NULL,stdin);
		} else {
			FILE *f;
			f=fopen(argv[t],"r");
			if (f==NULL) {
				fprintf(stderr,"Unable to open %s\n",argv[t]);
				exit(EXIT_FAILURE);
			}
			tabs[t]=read_table(NULL,f);
			fclose(f);
		}
	}
	print_tabs(tabs,1,argc-1);
	return 0;
}
