#ifndef CSV_H
#define CSV_H 1

#include "parse.h"
#include "ranges.h"
#include "parse.h"

/** строка CSV как динамический массив строк
*/
typedef struct Row {
	char **text;	/// массив ячеек
	int sz;			/// выделенно места (макс.число строк)
	int len;		/// кол-во строк
	int bound[2];	/// индекс первой значащей ячейки и ширина значащего поля
} Row;

Row *new_row(void);				/// создать новую пустую строку
Row *row_add(Row *row,char *s);	/// добавить запись (текст) в конец строки
Row *row_parse(Row *row,char *s,char **saveptr);/// разобрать из текста в соотв. с csvformat
Row *row_clear(Row *row,void (*cb)(char *));	/// очистить строку

char *row_cell(Row *,int);

typedef struct Table {
	Row **row;
	int sz;			/// allocated in row[]
	int len;		/// used in row[]
	int width;		/// ширина таблицы (= макс.ширина строк в row[])
	int height;		/// длина таблицы
	int bound[4];	/// индексы первой значащей ячейки и ширина/длина значащего поля
} Table;

/* xxx_table() - constructors */
Table *new_table(void);
Table *read_table(Table *,FILE *);
Table *parse_table(Table *,char *s,char **saveptr);

/* table_xxx() - methods */
Table *table_add(Table *,Row *);
Row *table_row(Table *,int);
char *table_cell(Table *,int,int);

int print_table_cut(Table *,RangeSet *,RangeSet *);
#endif
