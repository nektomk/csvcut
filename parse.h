#ifndef PARSE_H
#define PARSE_H  1

#include "ranges.h"
/* формат(вариации) CSV */
typedef struct CsvFormat {
	char *fs;	// разделитель полей	(по умолчанию ", и ;")
	char *rs;	// разделитель записей  (по умолчанию \n или \n\r)
	int quoting;// обрамление кавычками (по умолчанию - как есть)
	int omit_trail_cells;	// =1 если можно опускать пустые ячейки в конце строк
	int omit_trail_rows;	// =1 если можно опускать пустые строки в конце файла
} CsvFormat;

extern CsvFormat csvformat;

#define SPACES " \t\v\f\r\n"
#define BLANK " \t\v\f"

int parse_index(char **ps,Index *pnum);
Range *parse_range(Range *r,char *s,char **saveptr);
RangeSet *parse_rangeset(RangeSet *rs,char *s,char **saveptr);

/** разные полезняшки **/
int parse_name(char **pname,char *s,char **saveptr);
int parse_value(char **pname,char *s,char **saveptr);
int parse_name_value(char **pname,char **pvalue,char *s,char **saveptr);
#endif
