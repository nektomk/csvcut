/** csv zip - поменять местами столбцы и строки таблицы
	Читает исходную таблицу из stdin и выводит результат в stdout
**/
#include <stdio.h>

#include "csv.h"

int main(void) {
	Table *tab;
	int row,col;
	int colspan,rowspan;
	char *text;

	csvformat.omit_trail_cells=1;
	csvformat.omit_trail_rows=0;

	tab=read_table(NULL,stdin);
	rowspan=0;
	// по всем выводимым столбцам
	for(row=0;row<tab->width;row++) {
		colspan=0;
		// по всем выводимым ячейкам
		for(col=0;col<tab->height;col++) {
			// получить данные
			text=table_cell(tab,col,row);	// zip - обращение в обратном порядке
			if (text && text[0]) {
				// есть данные для вывода
				if (col==colspan) {
					// первая значащая ячейка в строке
					if (rowspan!=row)	// не первая значащая в тексте
						putchar('\n');	// закрыть предыдущую строку
					// вывести пустые пропущенные строки
					for(;rowspan;rowspan--)
						putchar('\n');
				} else {
					putchar(',');	// закрыть пред.запись
				}
				// вывести пустые ячейки
				for(;colspan;colspan--)
					putchar(',');
				printf("%s",text);
			} else {
				colspan++;
			}
		}
		if (colspan==col) {
			rowspan++;
		}
		// если необходимо - вывести пустые ячейки в конце строк
		if (!csvformat.omit_trail_cells) {
			for(;colspan;colspan--)
				putchar(',');
		}
	}
	// если необходимо - вывести пустые строки в конце файла
	if (!csvformat.omit_trail_rows) {
		for(;rowspan;rowspan--)
			putchar('\n');
	}
	return 0;
}


