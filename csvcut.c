#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string.h>

#include "csv.h"
#include "ranges.h"
#include "parse.h"

/*
#define SEE(format,...) do { \
	fprintf(stderr,"%s:%d %s : ",__FILE__,__LINE__,__PRETTY_FUNCTION__); \
	fprintf(stderr,format,##__VA_ARGS__); \
	fputc('\n',stderr); \
} while(0)
*/

/** объеденить аргументы в строку через ' '
*/
char *
join_args(int quantity,char *args[]) {
	int t;
	int sz;
	char *ret,*s,*p;
	int len;
	/* выделить память под буфер
		для начала ориентируемся на 32 байта на аргумент
	*/
	sz=quantity*32;
	s=ret=malloc(sz);
	/* добавлять аргументы */
	for(t=0;t<quantity;t++) {
		p=args[t];
		p+=strspn(p," \t\v\f\r\n");	// пропустить лидирующие пробелы
		if (*p==0) continue;
		len=strlen(p);
		if (len==0) continue;
		if (s+len+1>ret+sz) {
			// требуется увеличить буфер
			int newsz;
			char *tmp;
			newsz=sz+len+32;		// c некоторым запасом
			newsz+=32-(newsz%32);	// и выравнено по размеру на 32
			tmp=malloc(newsz);
			if (tmp==NULL) goto FAULT;
			memcpy(tmp,ret,s-ret);
			sz=newsz;
			s=tmp+(s-ret);
			free(ret);
			ret=tmp;
		}
		memcpy(s,p,len);
		s+=len;
		*s=' ';
		s++;
	}
	*s=0;
	return ret;
FAULT:
	if (ret) free(ret);
	return NULL;
}

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
		s+=strspn(s,BLANK);	// начальные пробелы игнорируются (todo: настраивается)
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
		s+=strspn(s,BLANK);
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

static void
usage(void) {
	printf("cut intersection of columns and rows from cvs-formatted input\n");
	printf("Usage: cvscut <columns> : <rows>\n");
	exit(EXIT_FAILURE);
}
int
main(int argc,char *argv[]) {
	char *args,*s;
	Table *tab;
	RangeSet *colset=NULL;
	RangeSet *rowset=NULL;

	if (argc==1) usage();

	s=args=join_args(argc-1,argv+1);
	colset=parse_rangeset(NULL,s,&s);
	if (colset==NULL) { colset=new_rangeset();rangeset_add_range(colset,&FULLRANGE); }
	if (*s==':') {
		s++;
		rowset=parse_rangeset(NULL,s,&s);
	}
	if (rowset==NULL) { rowset=new_rangeset();rangeset_add_range(rowset,&FULLRANGE); }
	// наборы получены, дальше доп.инструкции
/*	if (*s) {
		char *name,*value;
		s++;
		while(*s) {
			name=NULL;value=NULL;
			if (parse_name_value(&name,&value,s,&s)) {
				if (strcasecmp(name,"z") || strcasecmp(name,"zip")) {
					csvformat.zip=1;
				}
			}
		}
	}
*/
	tab=read_table(NULL,stdin);
	if (tab==NULL) {
		fprintf(stderr,"error reading table\n");
		exit(EXIT_FAILURE);
	}
/*	printf("table %d x %d loaded\n",tab->width,tab->height);
	printf("select columns:");
	print_rangeset(colset);
	putchar('\n');
	printf("select rows:");
	print_rangeset(rowset);
	putchar('\n');
*/
	print_table_cut(tab,colset,rowset);
	return 0;
}
