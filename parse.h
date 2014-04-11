#ifndef PARSE_H
#define PARSE_H  1

#include "ranges.h"

#define SPACES " \t\v\f\r\n"
#define BLANK " \t\v\f"

int parse_index(char **ps,Index *pnum);
Range *parse_range(Range *r,char *s,char **saveptr);
RangeSet *parse_rangeset(RangeSet *rs,char *s,char **saveptr);
#endif
