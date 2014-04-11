TARGETS=csvcut samplecsv
TESTS=ranges_test

CFLAGS=-Wall -Wextra

all: $(TARGETS)

csvcut: csvcut.o ranges.o parse.o

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
