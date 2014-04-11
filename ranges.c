#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ranges.h"
#include "parse.h"

Range FULLRANGE={0,-1,0};

void
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

