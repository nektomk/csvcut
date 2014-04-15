#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "csv.h"

int print_table_cut(Table *tab,RangeSet *colset,RangeSet *rowset) {
	RSI irow,icol;
	int row,col;
	int is_first_col;
	char *text;
	int rowspan=0;	// пропущенно пустых строк
	/// по всем выбранным строкам
	for(rsi_first(&irow,&row,rowset,tab->height);!rsi_end(&irow,&row);rsi_next(&irow,&row)) {
		int colspan=0;	// пропущенно пустых ячеек
		is_first_col=1;
		/// по всем выбранным столбцам
		for(rsi_first(&icol,&col,colset,tab->width);!rsi_end(&icol,&col);rsi_next(&icol,&col)) {
			text=tab->row[row]->text[col];
			if (text && text[0]) {
				//if (is_first_row) is_first_row=!is_first_row;
				//else putchar('\n');
				for(;rowspan>0;rowspan--) {
					printf(";\n");
				}
				for(;colspan>0;colspan--) {
					printf(",");
				}
				if (is_first_col) is_first_col=!is_first_col;
				else putchar(',');
				printf("%s",text);
			} else {
				colspan++;
			}
		}
		if (is_first_col) rowspan++;
		else putchar('\n');
	}
//	for(;rowspan>0;rowspan--) {
//		printf(";\n");
//	}
	return 0;
}

Row *new_row(void) {
	Row *r;
	r=malloc(sizeof(Row));
	if (!r) return NULL;
	memset(r,0,sizeof(Row));
	return r;
}
/** добавить ячейку в конец строки
*/
Row *row_add(Row *r,char *text) {
	if (r==NULL) r=new_row();
	if (r->sz==r->len) {
		char **tmp;
		int newsz;
		newsz=r->sz+32;
		tmp=calloc(sizeof(char *),newsz);
		if (r->text) {
			memcpy(tmp,r,sizeof(char *)*r->len);
			free(r->text);
		}
		r->text=tmp;
		r->sz=newsz;
	}
	r->text[r->len++]=text;
	return r;
}
Row *row_clear(Row *r,void (*cb)(char *)) {
	if (!r) return r;
	if (r->len && cb) {
		int t;
		for(t=0;t<r->len;t++) {
			if (r->text[t]) cb(r->text[t]);
		}
	}
	if (r->text)
		free(r->text);
	r->text=0;
	r->len=0;
	r->sz=0;
	return r;
}
/** разобрать CSV строку
	см RFC4180
**/
Row *
row_parse(Row *r,char *s,char **saveptr) {
	if (r==NULL) r=new_row();
	while(*s) {
		char *p;
		int quote;
		quote=0;
		//s+=strspn(s,BLANK);	// начальные пробелы игнорируются (todo: настраивается)
		while(isblank(*s))
			s++;
		p=s;
		while(*p) {
			if ((*p=='\"' || *p=='\'')) {
				if (*p==*(p+1)) {
					// две подряд следующие кавычки - пропускаем одну из них
					p++;
				} else if (*p==quote) {
					// закрылась кавычка
					quote=0;
				} else {
					// открылась кавычка
					quote=*p;
				}
			}
			if (quote==0) {
				if (*p==',' || *p==';') break;
				if (*p=='\r' && *(p+1)=='\n') p++;
				if (*p=='\n') break;
			}
			p++;
		}
		row_add(r,s);
		if (*p) { *p=0; s=p+1; }
		else s=p;
	}
	if (saveptr) *saveptr=s;
	return r;
}

char *row_cell(Row *row,int t) {
	if (row==NULL) return NULL;
	if (t<0) t=1-t;
	if (t>row->len) return NULL;
	return row->text[t];
}

Table *
new_table(void) {
	return calloc(sizeof(Table),1);
}
/** добавить строку в конец таблицы
*/
Table *
table_add(Table *tab,Row *r) {
	if (tab==NULL) tab=new_table();
	if (tab->sz==tab->len) {
		Row **tmp;
		int newsz;
		newsz=tab->sz+32;
		tmp=calloc(sizeof(Row *),newsz);
		if (!tmp) return NULL;
		if (tab->row) {
			memcpy(tmp,tab->row,sizeof(Row *)*tab->len);
			free(tab->row);
		}
		tab->row=tmp;
		tab->sz=newsz;
	}
	tab->row[tab->len++]=r;
	if (r) {
		if (r->len>tab->width)
			tab->width=r->len;
	}
	tab->height++;
	return tab;
}
Table *
parse_table(Table *tab,char *s,char **saveptr) {
	Row *r=NULL;
	if (tab==NULL) tab=new_table();
	while(*s) {
		if ((r=row_parse(r,s,&s))!=NULL) {
			table_add(tab,r);
			r=NULL;
		}
		// s+=strspn(s,BLANK);
		while(isblank(*s))
			s++;
		if (*s) s++;	// maybe '\n'
		//s+=strspn(s,BLANK);
	}
	if (saveptr) *saveptr=s;
	return tab;
}

Table *
read_table(Table *tab,FILE *f) {
	char *buf,*s;
	buf=malloc(2048);
	while(!feof(f)) {
		if ((s=fgets(buf,2047,f))==NULL) break;
		if (tab==NULL) tab=new_table();
		if (*s) s=strdup(s);
		while(*s && *s!=-1) {
			if (parse_table(tab,s,&s)==NULL) s++;
		}
	}
	return tab;
}

Row *
table_row(Table *tab,int t) {
	if (tab==NULL) return NULL;
	if (t<0) t=1-t;
	if (t>tab->len) return NULL;
	return tab->row[t];
}
char *
table_cell(Table *tab,int t,int k) {
	if (tab==NULL) return NULL;
	if (t<0) t=1-t;
	if (t>tab->len) return NULL;
	return row_cell(tab->row[t],k);
}

void
print_row(Row *r) {
	if (!r) printf("null");
	else if (!r->len) printf("empty");
	else {
		int t;
		for(t=0;r->text[t];t++) {
			if (t) putchar(',');
			printf("%s",r->text[t]);
		}
	}
}

void print_table(Table *tab) {
	if (!tab) printf("null");
	else if (!tab->len) printf("empty");
	else {
		int t;
		for(t=0;t<tab->len;t++) {
			print_row(tab->row[t]);
		}
	}
}
void print_table_info(FILE *f,Table *tab) {
	if (tab==NULL) printf("(null)");
	printf("CSV table %d cols * %d rows\n",tab->width,tab->height);
}