#ifndef RANGES_H
#define RANGES_H 1

typedef long Index;

typedef struct Range {
	Index d1;
	Index d2;
	int every;
} Range;

Range *new_range(Index d1,Index d2);
Range *set_range(Range *r,Index d1,Index d2,int every);
// Range *parse_range(Range *r,char *s,char **saveptr); // moved to parse.h

extern Range FULLRANGE;

typedef struct RangeSet {
	int sz,len;
	Range **range;
} RangeSet;

RangeSet *new_rangeset(void);
// RangeSet *parse_rangeset(RangeSet *rs,char *s,char **saveptr); // moved to parse.h
RangeSet *rangeset_add_range(RangeSet *rs,Range *r);
RangeSet *rangeset_add(RangeSet *rs,Index d1,Index d2);

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

void print_range(Range *);
void print_rangeset(RangeSet *);

#endif
