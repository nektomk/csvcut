TARGETS=csvcut samplecsv
TESTS=ranges_test parse_test

CFLAGS=-std=gnu99 -D_GNU_SOURCE -Wall -Wextra 

all: $(TARGETS)

csvcut: csvcut.o ranges.o parse.o csv.o

samplecsv: samplecsv.c

.PHONY: clean distclean touch

clean:
		$(RM) *.o

distclean:
		$(RM) *.o
		$(RM) $(TARGETS)
		$(RM) *.obj
		$(RM) *.lnk
		$(RM) *.exe

touch:
		touch -c *.c
		touch -c *.h
