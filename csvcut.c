#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "csv.h"

#define SPACES " \t\v\f\r\n"
#define BLANK " \t\v\f"
/*
#define SEE(format,...) do { \
	fprintf(stderr,"%s:%d %s : ",__FILE__,__LINE__,__PRETTY_FUNCTION__); \
	fprintf(stderr,format,##__VA_ARGS__); \
	fputc('\n',stderr); \
} while(0)
*/
Range FULLRANGE={0,-1,0};
Range *
new_range(Index d1,Index d2) {
	Range *r;
	r=calloc(sizeof(Range),1);
	if (!r) return NULL;
	r->d1=d1;
	r->d2=d2;
	return r;
}
Range *
set_range(Range *r,Index d1,Index d2,int every) {
	if (r==NULL)
		r=malloc(sizeof(Range));
	if (!r) return NULL;
	r->d1=d1;
	r->d2=d2;
	r->every=every;
	return r;
}

RangeSet *
new_rangeset() {
	RangeSet *rs;
	rs=malloc(sizeof(RangeSet));
	if (rs==NULL) return NULL;
	memset(rs,0,sizeof(RangeSet));
	return rs;
}
RangeSet *
rangeset_add_range(RangeSet *rs,Range *r) {
	if (!rs) rs=new_rangeset();
	if (rs->len==rs->sz) {
		// нужно увеличить длину буфера
		Range **tmp;
		int newsz;
		newsz=rs->sz+32;
		tmp=calloc(sizeof(Range *),newsz);
		if (!tmp) return NULL;
		if (rs->range) {
			memcpy(tmp,rs->range,sizeof(Range *)*rs->len);
			free(rs->range);
		}
		rs->range=tmp;
		rs->sz=newsz;
	}
	rs->range[rs->len++]=r;
	return rs;
}
RangeSet *
rangeset_add(RangeSet *rs,Index d1,Index d2) {
	return rangeset_add_range(rs,new_range(d1,d2));
}
void
rangeset_clear(RangeSet *rs) {
	if (!rs) return;
	if (rs->range) {
		int t;
		for(t=0;t<rs->len;t++)
			if (rs->range[t]) free(rs->range[t]);
		free(rs->range);
	}
	memset(rs,0,sizeof(RangeSet));
}
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
/** считать номер строки/ячейки
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
		// номер задан в виде а-ля Exel
		do {
			if (num) num=num*('z'-'a');
			if (*s>='a' && *s<='z') num+=*s-'a';
			else if (*s>='A' && *s<='Z') num+=*s-'A';
			else if (*s>='0' && *s<='9') num+=*s-'0';
			else break;	// конец номера
			s++;
		} while(*s);
	} else if (*s>='0' && *s<='9') {
		// номер задан в десятичном виде
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
/** считать определение диапазона из строки и сохранить в pfrom pto
	вернуть размер полученного диапазона
	допустимые варианты:
		1 : диапазон из единственного значения {1 1}
		1..2 или 1.30 : диапазон из двух значений {1 30}
		..2  4..      : диапазон от первой ячейки {1..2} до последней ячейки {4..-1}
*/
Range *
parse_range(Range *r,char *s,char **saveptr) {
	Index d1,d2;
	int every;
	every=1;
	s+=strspn(s,SPACES);
	if (!parse_index(&s,&d1)) d1=-1;	// считываем первый номер
	if (*s=='.') {
		// диапазон задан двумя числами
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
static int
next_in_range(int *p,int d1,int d2) {
	int i;
	if (!p) return 0;
	i=*p;
	if (i==d2) return 0;
	if (d1<d2) {
		i++;
	} else if (d1>d2) {
		i--;
	}
	//if (i<0) exit(1);
	*p=i;
	return 1;
}
static int
first_in_range(int *p,int d1,int d2) {
	if (!p) return 0;
	*p=d1;
	return 1;
}

/** а-ля итератор индексов по RangeSet */
typedef struct RSI {
	RangeSet *rs;	// набор
	int	range_num;	// номер диапазона в наборе
	int num;		// номер внутри диапазона
	int width;
} RSI;

int rsi_first(RSI *rsi,int *p,RangeSet *rs,int width); // перейти к первому в наборе
int rsi_next(RSI *rsi,int *p);		// перейти к след. *p в наборе
int rsi_end(RSI *rsi,int *p);		// =1 если *p вне набора

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

int rsi_end(RSI *rsi,int *p) {
	int d1,d2;
	if (rsi==NULL || p==NULL) return 1;
	if (rsi->rs==NULL) return 1;
	if (rsi->range_num<0 || rsi->range_num>=rsi->rs->len) return 1;
	d1=rsi->rs->range[rsi->range_num]->d1;
	d2=rsi->rs->range[rsi->range_num]->d2;
	if (d1<0) d1+=rsi->width;
	if (d2<0) d2+=rsi->width;
	if (*p<min(d1,d2) || *p>max(d1,d2)) return 1;
	return 0;
}
int rsi_first(RSI *rsi,int *p,RangeSet *rs,int width) {
	int d1;
	if (rsi==NULL) return 0;
	if (rsi->rs==NULL && rs==NULL) return 0;
	if (rs!=NULL) {
		rsi->rs=rs;
		rsi->range_num=0;
		rsi->num=0;
		rsi->width=width;
	} else {
		rsi->range_num=0;
		rsi->num=0;
	}
	d1=rsi->rs->range[rsi->range_num]->d1;
	if (d1<0)d1+=rsi->width;
	if (p) *p=d1;
	return 1;
}
int rsi_next(RSI *rsi,int *p) {
	Range *range;
	int i,d1,d2;
	i=*p;
	if (rsi==NULL || p==NULL) return 0;	// неправильные параметры
	if (rsi->rs==NULL || rsi->width<=0) {	// неинициализованный или неправильный rsi
		return 0;
	}
	if (rsi->range_num<0 || rsi->range_num>=rsi->rs->len) {
		return 0;
	}
	range=rsi->rs->range[rsi->range_num];
	d1=range->d1;
	d2=range->d2;
	if (d1<0) d1+=rsi->width;	// отрицательные индексы отн.конца
	if (d2<0) d2+=rsi->width;

	// сдвинуть i в нужную сторону внутри диапазона
	if (d1<d2) i++;
	else i--;
	// i вышел за диапазон - перейти к следующему
	if (i<min(d1,d2) || i>max(d1,d2)) {
		rsi->range_num++;
		if (rsi->range_num>=rsi->rs->len) {
			// достигнут конец набора
			return 0;
		}
		range=rsi->rs->range[rsi->range_num];
		i=range->d1;
		if (i<0) i+=rsi->width;
	}
	if (p) *p=i;
	return 1;
}
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
int print_table_cut2(Table *tab,RangeSet *colset,RangeSet *rowset) {
	RSI irow,icol;
	int row,col;
	int is_first_row,is_first_col;
	char *text;
	is_first_row=1;
	/// по всем выбранным строкам
	for(rsi_first(&irow,&row,rowset,tab->height);!rsi_end(&irow,&row);rsi_next(&irow,&row)) {
		if (is_first_row) is_first_row=!is_first_row;
		else putchar('\n');
		is_first_col=1;
		/// по всем выбранным столбцам
		for(rsi_first(&icol,&col,colset,tab->width);!rsi_end(&icol,&col);rsi_next(&icol,&col)) {
			if (is_first_col) is_first_col=!is_first_col;
			else putchar(',');
			text=tab->row[row]->text[col];
			if (text!=NULL) printf("%s",text);
		}
	}
	return 0;
}
int print_table_cut(Table *tab,RangeSet *colset,RangeSet *rowset) {
	Range *rowrange;
	int rowrange_num;
	int colrange_num;
	int firstrow;
	if (tab==NULL || tab->height==0 || tab->width==0) return 0;
	firstrow=1;
	/* перебор строк */
	for(rowrange_num=0;rowrange_num<rowset->len;rowrange_num++) {
		int rnum;
		rowrange=rowset->range[rowrange_num];
		if (rowrange==NULL) rowrange=&FULLRANGE;// bug: potential segfault
		if (rowrange->d1<0) rowrange->d1=tab->height+rowrange->d1;
		if (rowrange->d2<0) rowrange->d2=tab->height+rowrange->d2;
		first_in_range(&rnum,rowrange->d1,rowrange->d2);
		do {
			/* перебор колонок */
			Range *colrange;
			int firstcolumn;
			firstcolumn=1;

			if (firstrow) firstrow=!firstrow;
			else putchar('\n');

			for(colrange_num=0;colrange_num<colset->len;colrange_num++) {
				int cnum;
				colrange=colset->range[colrange_num];
				if (colrange==NULL) colrange=&FULLRANGE;// bug: potential segfault
				if (colrange->d1<0) colrange->d1=tab->width+colrange->d1;
				if (colrange->d2<0) colrange->d2=tab->width+colrange->d2;
				first_in_range(&cnum,colrange->d1,colrange->d2);
				do {
					char *text;

					if (firstcolumn) firstcolumn=!firstcolumn;
					else putchar(',');

					text=table_cell(tab,rnum,cnum);
					if (text && text[0]) printf("%s",text);
					//else printf("\"\"\"\"");
				} while(next_in_range(&cnum,colrange->d1,colrange->d2));
			}
		} while(next_in_range(&rnum,rowrange->d1,rowrange->d2));
	}
	return 0;
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
print_range(Range *r) {
	if (r==NULL) printf("null");
	else printf("%ld..%ld/%d",r->d1,r->d2,r->every);
}
void
print_rangeset(RangeSet *rs) {
	if (rs==NULL) printf("null");
	else if (rs->len==0) printf("empty");
	else {
		int t;
		for(t=0;t<rs->len;t++) {
			print_range(rs->range[t]);
			putchar(' ');
		}
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
Row *new_row() {
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
		int mode;
		mode=0;
		s+=strspn(s,BLANK);	// начальные пробелы игнорируются (todo: настраивается)
		p=s;
		while(*p) {
			if ((*p=='\"' || *p=='\'')) {
				if (*p==*(p+1)) {
					// две подряд следующие кавычки - пропускаем одну из них
					p++;
				} else if (*p==mode) {
					// закрылась кавычка
					mode=0;
				} else {
					// открылась кавычка
					mode=*p;
				}
			}
			if (mode==0) {
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
new_table() {
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
test_row() {
	char *test[2]={
		"one,two,three",
		NULL
	};
	int t;
	Row r={0};
	char *s;
	for(t=0;test[t];t++) {
		s=strdup(test[t]);
		printf("%d check \"%s\" ",t,test[t]);
		if (!row_parse(&r,s,&s)) {
			printf("fault");
		} else {
			printf("ok");
			print_row(&r);
		}
		//free(p);
		putchar('\n');
	}
}
static void
test_table() {
	char *test[2]={
		"one,two,three",
		NULL
	};
	int t;
	Table tab={0};
	char *s;
	for(t=0;test[t];t++) {
		s=strdup(test[t]);
		printf("%d check \"%s\" ",t,test[t]);
		if (!parse_table(&tab,s,&s)) {
			printf("fault");
		} else {
			printf("ok");
			printf("tab.sz=%u tab.len=%u tab->row=%p",tab.sz,tab.len,tab.row);
			print_table(&tab);
		}
		//free(p);
		putchar('\n');
	}
}
static void
test_table_cut() {
	RangeSet *colset;
	RangeSet *rowset;
	Table *tab;
	FILE *f;
	f=fopen("sample.csv","rt");
	if (f==NULL) {
		fprintf(stderr,"Unable to open %s\n","sample.csv");
		return;
	}
	tab=read_table(NULL,f);
	if (tab==NULL) {
		fprintf(stderr,"Error reading table from %s\n","sample.csv");
	}
	printf("table %d x %d loaded\n",tab->width,tab->height);
	colset=parse_rangeset(NULL,"4..0 7",NULL);
	printf("select columns:");
	print_rangeset(colset);
	putchar('\n');
	rowset=parse_rangeset(NULL,"0..4 1 -1",NULL);
	printf("select rows:");
	print_rangeset(rowset);
	putchar('\n');
	print_table_cut(tab,colset,rowset);
}
static void
usage() {
	printf("cut cells from cvs-formatted input\n");
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
	tab=read_table(NULL,stdin);

/*	printf("table %d x %d loaded\n",tab->width,tab->height);
	printf("select columns:");
	print_rangeset(colset);
	putchar('\n');
	printf("select rows:");
	print_rangeset(rowset);
	putchar('\n');
*/
	print_table_cut2(tab,colset,rowset);
	return 0;
}
