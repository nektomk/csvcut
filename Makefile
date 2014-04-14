TARGETS=csvcut csvzip csvpaste samplecsv
TESTS=ranges_test parse_test
CSVOBJS=csv.o ranges.o parse.o

CFLAGS=-std=gnu99 -D_GNU_SOURCE -Wall -Wextra 

all: $(TARGETS)

csvcut: csvcut.o $(CSVOBJS)

csvzip: csvzip.o $(CSVOBJS)

csvpaste: csvpaste.o $(CSVOBJS)

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
