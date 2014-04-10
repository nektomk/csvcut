csvcut
======

Утилита для "вырезания" диапазонов ячеек из CSV файлов

Синтаксис
---------
`csvcut ВЫБОР_КОЛОНОК : ВЫБОР_СТРОК`

Описание
--------
cvscut читает из стандартного ввода CSV файл, делает выборку по перечесению заданных колонок и строк,
и выводит результат на стандартный вывод в формате аналогичном вводу;

Колонки и строки нумеруются начиная с 0, отрицательное значение интерпретируется как относительные от последнего.
Можно также задавать диапазоны в виде *НОМЕР..НОМЕР*, к примеру диапазон `1..3` соответсвует набору `1 2 3`, а диапазон `3..1` соответствует `3 2 1`

Примеры
-------
* `csvcut 0 -1` вывести первую и последнюю колонку
* `csvcut 2..-1` вывод всего кроме первых двух колонок
* `csvcut 2..-1:1..-1` вывести всё кроме первых двух колонок и первой строки
* `csvcut -1..0` вывести весь файл поменяв порядок колонок на обратный
* `csvcut :-1..-3` вывести последние 3 строки в обратном порядке

Ссылки
------
RFC-1480 Рекомендуемый формат CSV файлов

Примечания
----------
* в отличии от RFC допускается обрамление строк " (двойными кавычками) и ' (одинарными кавычками)
* в отличии от RFC допускается разделители ,(запятая) и ;(точка с запятой)
* в отличии от RFC конец строки в выходном файле зависит от системы (\n или \r\n)


