#ifndef CSV_H
#define CSV_H 1

typedef struct Row {
	char **text;
	int sz;
	int len;
} Row;

Row *new_row();
Row *row_add(Row *row,char *s);
Row *row_parse(Row *row,char *s,char **saveptr);
Row *row_clear(Row *row,void (*cb)(char *));

char *row_cell(Row *,int);

typedef struct Table {
	Row **row;
	int sz;
	int len;
	int width;
	int height;
} Table;

/* xxx_table() - constructors */
Table *new_table();
Table *read_table(Table *,FILE *);
Table *parse_table(Table *,char *s,char **saveptr);

/* table_xxx() - methods */
Table *table_add(Table *,Row *);
Row *table_row(Table *,int);
char *table_cell(Table *,int,int);

typedef long Index;
typedef struct Range {
	Index d1;
	Index d2;
	int every;
} Range;

Range *new_range(Index d1,Index d2);
Range *set_range(Range *r,Index d1,Index d2,int every);

typedef struct RangeSet {
	int sz,len;
	Range **range;
} RangeSet;

#endif
